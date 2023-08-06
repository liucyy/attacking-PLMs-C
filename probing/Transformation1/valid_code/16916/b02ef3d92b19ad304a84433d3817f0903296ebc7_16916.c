static void assign_storage(SCLPDevice *sclp, SCCB *sccb)

{

    MemoryRegion *mr = NULL;

    uint64_t this_subregion_size;

    AssignStorage *assign_info = (AssignStorage *) sccb;

    sclpMemoryHotplugDev *mhd = get_sclp_memory_hotplug_dev();

    assert(mhd);

    ram_addr_t assign_addr = (assign_info->rn - 1) * mhd->rzm;

    MemoryRegion *sysmem = get_system_memory();



    if ((assign_addr % MEM_SECTION_SIZE == 0) &&

        (assign_addr >= mhd->padded_ram_size)) {

        

        mr = memory_region_find(sysmem, assign_addr, 1).mr;

        memory_region_unref(mr);

        if (!mr) {



            MemoryRegion *standby_ram = g_new(MemoryRegion, 1);



            

            ram_addr_t offset = assign_addr -

                                (assign_addr - mhd->padded_ram_size)

                                % mhd->standby_subregion_size;



            

            char id[16];

            snprintf(id, 16, "standby.ram%d",

                     (int)((offset - mhd->padded_ram_size) /

                     mhd->standby_subregion_size) + 1);



            

            if (offset + mhd->standby_subregion_size >

                mhd->padded_ram_size + mhd->standby_mem_size) {

                this_subregion_size = mhd->padded_ram_size +

                  mhd->standby_mem_size - offset;

            } else {

                this_subregion_size = mhd->standby_subregion_size;

            }



            memory_region_init_ram(standby_ram, NULL, id, this_subregion_size, &error_abort);

            

            object_ref(OBJECT(standby_ram));

            object_unparent(OBJECT(standby_ram));

            vmstate_register_ram_global(standby_ram);

            memory_region_add_subregion(sysmem, offset, standby_ram);

        }

        

        mhd->standby_state_map[(assign_addr - mhd->padded_ram_size)

                               / MEM_SECTION_SIZE] = 1;

    }

    sccb->h.response_code = cpu_to_be16(SCLP_RC_NORMAL_COMPLETION);

}