static int dump_init(DumpState *s, int fd, bool paging, bool has_filter,

                     int64_t begin, int64_t length, Error **errp)

{

    CPUState *cpu;

    int nr_cpus;

    Error *err = NULL;

    int ret;



    if (runstate_is_running()) {

        vm_stop(RUN_STATE_SAVE_VM);

        s->resume = true;

    } else {

        s->resume = false;

    }



    

    cpu_synchronize_all_states();

    nr_cpus = 0;

    CPU_FOREACH(cpu) {

        nr_cpus++;

    }



    s->errp = errp;

    s->fd = fd;

    s->has_filter = has_filter;

    s->begin = begin;

    s->length = length;



    guest_phys_blocks_init(&s->guest_phys_blocks);

    guest_phys_blocks_append(&s->guest_phys_blocks);



    s->start = get_start_block(s);

    if (s->start == -1) {

        error_set(errp, QERR_INVALID_PARAMETER, "begin");

        goto cleanup;

    }



    

    ret = cpu_get_dump_info(&s->dump_info, &s->guest_phys_blocks);

    if (ret < 0) {

        error_set(errp, QERR_UNSUPPORTED);

        goto cleanup;

    }



    s->note_size = cpu_get_note_size(s->dump_info.d_class,

                                     s->dump_info.d_machine, nr_cpus);

    if (s->note_size < 0) {

        error_set(errp, QERR_UNSUPPORTED);

        goto cleanup;

    }



    

    memory_mapping_list_init(&s->list);

    if (paging) {

        qemu_get_guest_memory_mapping(&s->list, &s->guest_phys_blocks, &err);

        if (err != NULL) {

            error_propagate(errp, err);

            goto cleanup;

        }

    } else {

        qemu_get_guest_simple_memory_mapping(&s->list, &s->guest_phys_blocks);

    }



    s->nr_cpus = nr_cpus;

    s->page_size = TARGET_PAGE_SIZE;

    s->page_shift = ffs(s->page_size) - 1;



    get_max_mapnr(s);



    uint64_t tmp;

    tmp = DIV_ROUND_UP(DIV_ROUND_UP(s->max_mapnr, CHAR_BIT), s->page_size);

    s->len_dump_bitmap = tmp * s->page_size;



    if (s->has_filter) {

        memory_mapping_filter(&s->list, s->begin, s->length);

    }



    

    s->phdr_num = 1; 

    if (s->list.num < UINT16_MAX - 2) {

        s->phdr_num += s->list.num;

        s->have_section = false;

    } else {

        s->have_section = true;

        s->phdr_num = PN_XNUM;

        s->sh_info = 1; 



        

        if (s->list.num <= UINT32_MAX - 1) {

            s->sh_info += s->list.num;

        } else {

            s->sh_info = UINT32_MAX;

        }

    }



    if (s->dump_info.d_class == ELFCLASS64) {

        if (s->have_section) {

            s->memory_offset = sizeof(Elf64_Ehdr) +

                               sizeof(Elf64_Phdr) * s->sh_info +

                               sizeof(Elf64_Shdr) + s->note_size;

        } else {

            s->memory_offset = sizeof(Elf64_Ehdr) +

                               sizeof(Elf64_Phdr) * s->phdr_num + s->note_size;

        }

    } else {

        if (s->have_section) {

            s->memory_offset = sizeof(Elf32_Ehdr) +

                               sizeof(Elf32_Phdr) * s->sh_info +

                               sizeof(Elf32_Shdr) + s->note_size;

        } else {

            s->memory_offset = sizeof(Elf32_Ehdr) +

                               sizeof(Elf32_Phdr) * s->phdr_num + s->note_size;

        }

    }



    return 0;



cleanup:

    guest_phys_blocks_free(&s->guest_phys_blocks);



    if (s->resume) {

        vm_start();

    }



    return -1;

}