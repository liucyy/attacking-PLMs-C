static int decode_p_mbs(VC9Context *v)

{

    MpegEncContext *s = &v->s;

    GetBitContext *gb = &v->s.gb;

    int current_mb = 0, i; 

    uint8_t cbpcy[4], previous_cbpcy[4], predicted_cbpcy,

        *p_cbpcy ;

    int hybrid_pred; 

    int mv_mode_bit = 0; 

    int mqdiff, mquant; 

    int ttmb; 



    static const int size_table[6] = { 0, 2, 3, 4, 5, 8 },

        offset_table[6] = { 0, 1, 3, 7, 15, 31 };

        int mb_has_coeffs = 1; 

    int dmv_x, dmv_y; 

    int k_x, k_y; 

    int hpel_flag; 

    int index, index1; 

    int val, sign; 



    

    if (v->pq < 5) v->ttmb_vlc = &vc9_ttmb_vlc[0];

    else if (v->pq < 13) v->ttmb_vlc = &vc9_ttmb_vlc[1];

    else v->ttmb_vlc = &vc9_ttmb_vlc[2];



    

    switch (v->mvrange)

    {

    case 1: k_x = 10; k_y = 9; break;

    case 2: k_x = 12; k_y = 10; break;

    case 3: k_x = 13; k_y = 11; break;

    default:  k_x = 9; k_y = 8; break;

    }



    hpel_flag = v->mv_mode & 1; 

    k_x -= hpel_flag;

    k_y -= hpel_flag;



    

    memset(v->previous_line_cbpcy, 0, s->mb_stride<<2);



    for (s->mb_y=0; s->mb_y<s->mb_height; s->mb_y++)

    {

        

        *((uint32_t*)previous_cbpcy) = 0x00000000;

        p_cbpcy = v->previous_line_cbpcy+4;



        for (s->mb_x=0; s->mb_x<s->mb_width; s->mb_x++, p_cbpcy += 4)

        {

            if (v->mv_type_mb_plane.is_raw)

                v->mv_type_mb_plane.data[current_mb] = get_bits(gb, 1);

            if (v->skip_mb_plane.is_raw)

                v->skip_mb_plane.data[current_mb] = get_bits(gb, 1);

            if (!mv_mode_bit) 

            {

                if (!v->skip_mb_plane.data[current_mb])

                {

                    GET_MVDATA(dmv_x, dmv_y);



                    

                    if (v->mv_mode == MV_PMODE_1MV ||

                        v->mv_mode == MV_PMODE_MIXED_MV)

                        hybrid_pred = get_bits(gb, 1);

                    if (s->mb_intra && !mb_has_coeffs)

                    {

                        GET_MQUANT();

                        s->ac_pred = get_bits(gb, 1);

                    }

                    else if (mb_has_coeffs)

                    {

                        if (s->mb_intra) s->ac_pred = get_bits(gb, 1);

                        predicted_cbpcy = get_vlc2(gb, v->cbpcy_vlc->table, VC9_CBPCY_P_VLC_BITS, 2);

                        cbpcy[0] = (p_cbpcy[-1] == p_cbpcy[2]) ? previous_cbpcy[1] : p_cbpcy[2];

                        cbpcy[0] ^= ((predicted_cbpcy>>5)&0x01);

                        cbpcy[1] = (p_cbpcy[2] == p_cbpcy[3]) ? cbpcy[0] : p_cbpcy[3];

                        cbpcy[1] ^= ((predicted_cbpcy>>4)&0x01);

                        cbpcy[2] = (previous_cbpcy[1] == cbpcy[0]) ? previous_cbpcy[3] : cbpcy[0];

                        cbpcy[2] ^= ((predicted_cbpcy>>3)&0x01);

                        cbpcy[3] = (cbpcy[1] == cbpcy[0]) ? cbpcy[2] : cbpcy[1];

                        cbpcy[3] ^= ((predicted_cbpcy>>2)&0x01);

                        



                        GET_MQUANT();

                    }

                    if (!v->ttmbf)

                        ttmb = get_vlc2(gb, v->ttmb_vlc->table,

                                            VC9_TTMB_VLC_BITS, 12);

                    

                }

                else 

                {

                    

                    if (v->mv_mode == MV_PMODE_1MV ||

                        v->mv_mode == MV_PMODE_MIXED_MV)

                        hybrid_pred = get_bits(gb, 1);

                }

            } 

            else 

            {

              if (!v->skip_mb_plane.data[current_mb] )

                {

                    

                    GET_CBPCY(v->cbpcy_vlc->table, VC9_CBPCY_P_VLC_BITS);

                    for (i=0; i<4; i++) 

                    {

                        if (cbpcy[i] )

                        {

                            GET_MVDATA(dmv_x, dmv_y);

                        }

                        if (v->mv_mode == MV_PMODE_MIXED_MV )

                            hybrid_pred = get_bits(gb, 1);

                        GET_MQUANT();

                        if (s->mb_intra  &&

                            index )

                            s->ac_pred = get_bits(gb, 1);

                        if (!v->ttmbf)

                            ttmb = get_vlc2(gb, v->ttmb_vlc->table,

                                            VC9_TTMB_VLC_BITS, 12);

            

                        

            

                    }

                }

                else 

                {

                    for (i=0; i<4; i++) 

                    {

                        if (v->mv_mode == MV_PMODE_MIXED_MV )

                            hybrid_pred = get_bits(gb, 1);

                        

                        

                    }

                }

            }



            

#if TRACE > 2

            av_log(s->avctx, AV_LOG_DEBUG, "Block %4i: p_cbpcy=%i%i%i%i, previous_cbpcy=%i%i%i%i,"

                   " cbpcy=%i%i%i%i\n", current_mb,

                   p_cbpcy[0], p_cbpcy[1], p_cbpcy[2], p_cbpcy[3],

                   previous_cbpcy[0], previous_cbpcy[1], previous_cbpcy[2], previous_cbpcy[3],

                   cbpcy[0], cbpcy[1], cbpcy[2], cbpcy[3]);

#endif

            *((uint32_t*)p_cbpcy) = *((uint32_t*)previous_cbpcy);

            *((uint32_t*)previous_cbpcy) = *((uint32_t*)cbpcy);

            current_mb++;

        }

    }

    return 0;

}