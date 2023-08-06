static int scan_mmco_reset(AVCodecParserContext *s, GetBitContext *gb)

{

    H264PredWeightTable pwt;

    int slice_type_nos = s->pict_type & 3;

    H264ParseContext *p = s->priv_data;

    H264Context      *h = &p->h;

    int list_count, ref_count[2];





    if (h->pps.redundant_pic_cnt_present)

        get_ue_golomb(gb); 



    if (slice_type_nos == AV_PICTURE_TYPE_B)

        get_bits1(gb); 



    if (ff_h264_parse_ref_count(&list_count, ref_count, gb, &h->pps,

                                slice_type_nos, h->picture_structure) < 0)

        return AVERROR_INVALIDDATA;



    if (slice_type_nos != AV_PICTURE_TYPE_I) {

        int list;

        for (list = 0; list < list_count; list++) {

            if (get_bits1(gb)) {

                int index;

                for (index = 0; ; index++) {

                    unsigned int reordering_of_pic_nums_idc = get_ue_golomb_31(gb);



                    if (reordering_of_pic_nums_idc < 3)

                        get_ue_golomb(gb);

                    else if (reordering_of_pic_nums_idc > 3) {

                        av_log(h->avctx, AV_LOG_ERROR,

                               "illegal reordering_of_pic_nums_idc %d\n",

                               reordering_of_pic_nums_idc);

                        return AVERROR_INVALIDDATA;

                    } else

                        break;



                    if (index >= ref_count[list]) {

                        av_log(h->avctx, AV_LOG_ERROR,

                               "reference count %d overflow\n", index);

                        return AVERROR_INVALIDDATA;

                    }

                }

            }

        }

    }



    if ((h->pps.weighted_pred && slice_type_nos == AV_PICTURE_TYPE_P) ||

        (h->pps.weighted_bipred_idc == 1 && slice_type_nos == AV_PICTURE_TYPE_B))

        ff_h264_pred_weight_table(gb, &h->sps, ref_count, slice_type_nos,

                                  &pwt);



    if (get_bits1(gb)) { 

        int i;

        for (i = 0; i < MAX_MMCO_COUNT; i++) {

            MMCOOpcode opcode = get_ue_golomb_31(gb);

            if (opcode > (unsigned) MMCO_LONG) {

                av_log(h->avctx, AV_LOG_ERROR,

                       "illegal memory management control operation %d\n",

                       opcode);

                return AVERROR_INVALIDDATA;

            }

            if (opcode == MMCO_END)

               return 0;

            else if (opcode == MMCO_RESET)

                return 1;



            if (opcode == MMCO_SHORT2UNUSED || opcode == MMCO_SHORT2LONG)

                get_ue_golomb(gb);

            if (opcode == MMCO_SHORT2LONG || opcode == MMCO_LONG2UNUSED ||

                opcode == MMCO_LONG || opcode == MMCO_SET_MAX_LONG)

                get_ue_golomb_31(gb);

        }

    }



    return 0;

}