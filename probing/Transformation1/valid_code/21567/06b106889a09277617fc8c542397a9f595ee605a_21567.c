static bool find_dirty_block(RAMState *rs, PageSearchStatus *pss,

                             bool *again, ram_addr_t *ram_addr_abs)

{

    pss->offset = migration_bitmap_find_dirty(rs, pss->block, pss->offset,

                                              ram_addr_abs);

    if (pss->complete_round && pss->block == rs->last_seen_block &&

        pss->offset >= rs->last_offset) {

        

        *again = false;

        return false;

    }

    if (pss->offset >= pss->block->used_length) {

        

        pss->offset = 0;

        pss->block = QLIST_NEXT_RCU(pss->block, next);

        if (!pss->block) {

            

            pss->block = QLIST_FIRST_RCU(&ram_list.blocks);

            

            pss->complete_round = true;

            rs->ram_bulk_stage = false;

            if (migrate_use_xbzrle()) {

                

                flush_compressed_data(rs);

            }

        }

        

        *again = true;

        return false;

    } else {

        

        *again = true;

        

        return true;

    }

}