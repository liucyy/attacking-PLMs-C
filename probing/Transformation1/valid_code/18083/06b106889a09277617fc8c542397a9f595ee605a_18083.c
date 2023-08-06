static bool get_queued_page(RAMState *rs, PageSearchStatus *pss,

                            ram_addr_t *ram_addr_abs)

{

    RAMBlock  *block;

    ram_addr_t offset;

    bool dirty;



    do {

        block = unqueue_page(rs, &offset, ram_addr_abs);

        

        if (block) {

            unsigned long *bitmap;

            bitmap = atomic_rcu_read(&rs->ram_bitmap)->bmap;

            dirty = test_bit(*ram_addr_abs >> TARGET_PAGE_BITS, bitmap);

            if (!dirty) {

                trace_get_queued_page_not_dirty(

                    block->idstr, (uint64_t)offset,

                    (uint64_t)*ram_addr_abs,

                    test_bit(*ram_addr_abs >> TARGET_PAGE_BITS,

                         atomic_rcu_read(&rs->ram_bitmap)->unsentmap));

            } else {

                trace_get_queued_page(block->idstr,

                                      (uint64_t)offset,

                                      (uint64_t)*ram_addr_abs);

            }

        }



    } while (block && !dirty);



    if (block) {

        

        rs->ram_bulk_stage = false;



        

        pss->block = block;

        pss->offset = offset;

    }



    return !!block;

}