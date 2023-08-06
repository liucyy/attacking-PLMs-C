static int decode_p_mbs(VC9Context *v)

{

    int x, y, current_mb = 0, i; 

    int skip_mb_bit = 0, cbpcy; 

    int hybrid_pred, ac_pred; 

    int mb_has_coeffs = 1  , mb_is_intra;

    int dmv_x, dmv_y; 

    int mv_mode_bit = 0; 

    int mqdiff, mquant; 

    int tt_block; 

    static const int size_table[6] = { 0, 2, 3, 4, 5, 8 },

        offset_table[6] = { 0, 1, 3, 7, 15, 31 };

    int k_x, k_y; 

    int hpel_flag, intra_flag; 

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



    for (y=0; y<v->height_mb; y++)

    {

        for (x=0; x<v->width_mb; x++)

        {

            if (v->mv_type_mb_plane[current_mb])

                mv_mode_bit = get_bits(&v->gb, 1);

            if (0) 

                skip_mb_bit = get_bits(&v->gb, 1);

            if (!mv_mode_bit) 

            {

                if (!v->skip_mb_plane[current_mb])

                {

                    GET_MVDATA();



                    

                    if (v->mv_mode == MV_PMODE_1MV ||

                        v->mv_mode == MV_PMODE_MIXED_MV)

                        hybrid_pred = get_bits(&v->gb, 1);

                    if (mb_is_intra && !mb_has_coeffs)

                    {

                        GET_MQUANT();

                        ac_pred = get_bits(&v->gb, 1);

                    }

                    else if (mb_has_coeffs)

                    {

                        if (mb_is_intra) ac_pred = get_bits(&v->gb, 1);

                        cbpcy = get_vlc2(&v->gb, v->cbpcy_vlc->table,

                                         VC9_CBPCY_P_VLC_BITS, 2);

                        GET_MQUANT();

                    }

                    if (!v->ttmbf)

                        v->ttfrm = get_vlc2(&v->gb, v->ttmb_vlc->table,

                                            VC9_TTMB_VLC_BITS, 2);

                    

                }

                else 

                {

                    

                    if (v->mv_mode == MV_PMODE_1MV ||

                        v->mv_mode == MV_PMODE_MIXED_MV)

                        hybrid_pred = get_bits(&v->gb, 1);

                }

            } 

            else 

            {

              if (!v->skip_mb_plane[current_mb] )

                {

                    cbpcy = get_vlc2(&v->gb, v->cbpcy_vlc->table,

                                     VC9_CBPCY_P_VLC_BITS, 2);

                    for (i=0; i<4; i++) 

                    {

                        if (cbpcy & (1<<6) )

                        {

                            GET_MVDATA();

                        }

                        if (v->mv_mode == MV_PMODE_MIXED_MV )

                            hybrid_pred = get_bits(&v->gb, 1);

                        GET_MQUANT();

                        if (mb_is_intra  &&

                            index )

                            ac_pred = get_bits(&v->gb, 1);

                        if (!v->ttmbf)

                            tt_block = get_vlc2(&v->gb, v->ttmb_vlc->table,

                                                VC9_TTMB_VLC_BITS, 2);

            

                        

            

                        

                        cbpcy <<= 1;

                    }

                }

                else 

                {

                    for (i=0; i<4; i++) 

                    {

                        if (v->mv_mode == MV_PMODE_MIXED_MV )

                            hybrid_pred = get_bits(&v->gb, 1);

                        

                        

                    }

                }

            }

        }

        current_mb++;

    }

    return 0;

}