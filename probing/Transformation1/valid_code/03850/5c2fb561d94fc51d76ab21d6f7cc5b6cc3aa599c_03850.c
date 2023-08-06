static int get_last_needed_nal(H264Context *h)

{

    int nals_needed = 0;

    int i;



    for (i = 0; i < h->pkt.nb_nals; i++) {

        H2645NAL *nal = &h->pkt.nals[i];

        GetBitContext gb;



        

        switch (nal->type) {

        case NAL_SPS:

        case NAL_PPS:

            nals_needed = i;

            break;

        case NAL_DPA:

        case NAL_IDR_SLICE:

        case NAL_SLICE:

            init_get_bits(&gb, nal->data + 1, (nal->size - 1) * 8);

            if (!get_ue_golomb(&gb))

                nals_needed = i;

        }

    }



    return nals_needed;

}