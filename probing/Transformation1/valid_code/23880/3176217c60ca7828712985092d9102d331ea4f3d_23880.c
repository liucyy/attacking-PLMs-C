static inline int decode_hrd_parameters(H264Context *h, SPS *sps)

{

    int cpb_count, i;

    cpb_count = get_ue_golomb_31(&h->gb) + 1;



    if (cpb_count > 32U) {

        av_log(h->avctx, AV_LOG_ERROR, "cpb_count %d invalid\n", cpb_count);

        return AVERROR_INVALIDDATA;

    }



    get_bits(&h->gb, 4); 

    get_bits(&h->gb, 4); 

    for (i = 0; i < cpb_count; i++) {

        get_ue_golomb_long(&h->gb); 

        get_ue_golomb_long(&h->gb); 

        get_bits1(&h->gb);          

    }

    sps->initial_cpb_removal_delay_length = get_bits(&h->gb, 5) + 1;

    sps->cpb_removal_delay_length         = get_bits(&h->gb, 5) + 1;

    sps->dpb_output_delay_length          = get_bits(&h->gb, 5) + 1;

    sps->time_offset_length               = get_bits(&h->gb, 5);

    sps->cpb_cnt                          = cpb_count;

    return 0;

}