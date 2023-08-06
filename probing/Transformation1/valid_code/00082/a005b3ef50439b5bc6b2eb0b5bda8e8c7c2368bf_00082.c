int xics_alloc_block(XICSState *icp, int src, int num, bool lsi, bool align)

{

    int i, first = -1;

    ICSState *ics = &icp->ics[src];



    assert(src == 0);

    

    if (align) {

        assert((num == 1) || (num == 2) || (num == 4) ||

               (num == 8) || (num == 16) || (num == 32));

        first = ics_find_free_block(ics, num, num);

    } else {

        first = ics_find_free_block(ics, num, 1);

    }



    if (first >= 0) {

        for (i = first; i < first + num; ++i) {

            ics_set_irq_type(ics, i, lsi);

        }

    }

    first += ics->offset;



    trace_xics_alloc_block(src, first, num, lsi, align);



    return first;

}