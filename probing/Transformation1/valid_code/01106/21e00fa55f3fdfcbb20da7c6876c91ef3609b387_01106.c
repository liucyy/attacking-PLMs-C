static void guest_phys_blocks_region_add(MemoryListener *listener,

                                         MemoryRegionSection *section)

{

    GuestPhysListener *g;

    uint64_t section_size;

    hwaddr target_start, target_end;

    uint8_t *host_addr;

    GuestPhysBlock *predecessor;



    

    if (!memory_region_is_ram(section->mr) ||

        memory_region_is_skip_dump(section->mr)) {

        return;

    }



    g            = container_of(listener, GuestPhysListener, listener);

    section_size = int128_get64(section->size);

    target_start = section->offset_within_address_space;

    target_end   = target_start + section_size;

    host_addr    = memory_region_get_ram_ptr(section->mr) +

                   section->offset_within_region;

    predecessor  = NULL;



    

    if (!QTAILQ_EMPTY(&g->list->head)) {

        hwaddr predecessor_size;



        predecessor = QTAILQ_LAST(&g->list->head, GuestPhysBlockHead);

        predecessor_size = predecessor->target_end - predecessor->target_start;



        

        g_assert(predecessor->target_end <= target_start);



        

        if (predecessor->target_end < target_start ||

            predecessor->host_addr + predecessor_size != host_addr) {

            predecessor = NULL;

        }

    }



    if (predecessor == NULL) {

        

        GuestPhysBlock *block = g_malloc0(sizeof *block);



        block->target_start = target_start;

        block->target_end   = target_end;

        block->host_addr    = host_addr;

        block->mr           = section->mr;

        memory_region_ref(section->mr);



        QTAILQ_INSERT_TAIL(&g->list->head, block, next);

        ++g->list->num;

    } else {

        

        predecessor->target_end = target_end;

    }



#ifdef DEBUG_GUEST_PHYS_REGION_ADD

    fprintf(stderr, "%s: target_start=" TARGET_FMT_plx " target_end="

            TARGET_FMT_plx ": %s (count: %u)\n", __FUNCTION__, target_start,

            target_end, predecessor ? "joined" : "added", g->list->num);

#endif

}