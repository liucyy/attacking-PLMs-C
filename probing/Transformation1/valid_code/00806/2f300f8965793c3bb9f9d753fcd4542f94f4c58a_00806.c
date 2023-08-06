void ff_fix_long_mvs(MpegEncContext * s, uint8_t *field_select_table, int field_select, 

                     int16_t (*mv_table)[2], int f_code, int type, int truncate)

{

    MotionEstContext * const c= &s->me;

    int y, h_range, v_range;



    

    int range = (((s->out_format == FMT_MPEG1) ? 8 : 16) << f_code);



    if(s->msmpeg4_version) range= 16;

    if(c->avctx->me_range && range > c->avctx->me_range) range= c->avctx->me_range;



    h_range= range;

    v_range= field_select_table ? range>>1 : range;



    

    for(y=0; y<s->mb_height; y++){

        int x;

        int xy= y*s->mb_stride;

        for(x=0; x<s->mb_width; x++){

            if (s->mb_type[xy] & type){    

                if(field_select_table==NULL || field_select_table[xy] == field_select){

                    if(   mv_table[xy][0] >=h_range || mv_table[xy][0] <-h_range

                       || mv_table[xy][1] >=v_range || mv_table[xy][1] <-v_range){



                        if(truncate){

                            if     (mv_table[xy][0] > h_range-1) mv_table[xy][0]=  h_range-1;

                            else if(mv_table[xy][0] < -h_range ) mv_table[xy][0]= -h_range;

                            if     (mv_table[xy][1] > v_range-1) mv_table[xy][1]=  v_range-1;

                            else if(mv_table[xy][1] < -v_range ) mv_table[xy][1]= -v_range;

                        }else{

                            s->mb_type[xy] &= ~type;

                            s->mb_type[xy] |= CANDIDATE_MB_TYPE_INTRA;

                            mv_table[xy][0]=

                            mv_table[xy][1]= 0;

                        }

                    }

                }

            }

            xy++;

        }

    }

}