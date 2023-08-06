static int decode_b_mbs(VC9Context *v)

{

    MpegEncContext *s = &v->s;

    GetBitContext *gb = &v->s.gb;

    int current_mb = 0, i ;

    int b_mv_type = BMV_TYPE_BACKWARD;

    int mquant, mqdiff; 

    int ttmb; 

    

    static const int size_table[6] = { 0, 2, 3, 4, 5, 8 },

        offset_table[6] = { 0, 1, 3, 7, 15, 31 };

    int mb_has_coeffs = 1; 

    int dmv1_x, dmv1_y, dmv2_x, dmv2_y; 

    int k_x, k_y; 

    int hpel_flag; 

    int index, index1; 

    int val, sign; 

    

    

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



    

    if (v->pq < 5) v->ttmb_vlc = &vc9_ttmb_vlc[0];

    else if (v->pq < 13) v->ttmb_vlc = &vc9_ttmb_vlc[1];

    else v->ttmb_vlc = &vc9_ttmb_vlc[2];



    for (s->mb_y=0; s->mb_y<s->mb_height; s->mb_y++)

    {

        for (s->mb_x=0; s->mb_x<s->mb_width; s->mb_x++)

        {

            if (v->direct_mb_plane.is_raw)

                v->direct_mb_plane.data[current_mb] = get_bits(gb, 1);

            if (v->skip_mb_plane.is_raw)

                v->skip_mb_plane.data[current_mb] = get_bits(gb, 1);

            

            if (!v->direct_mb_plane.data[current_mb])

            {

                if (v->skip_mb_plane.data[current_mb])

                {

                    b_mv_type = decode012(gb);

                    if (v->bfraction > 420  &&

                        b_mv_type < 3) b_mv_type = 1-b_mv_type;

                }

                else

                { 

                    GET_MVDATA(dmv1_x, dmv1_y);

                    if (!s->mb_intra )

                    {

                        b_mv_type = decode012(gb);

                        if (v->bfraction > 420  &&

                            b_mv_type < 3) b_mv_type = 1-b_mv_type;

                    }

                }

            }

            if (!v->skip_mb_plane.data[current_mb])

            {

                if (mb_has_coeffs )

                {

                    GET_MQUANT();

                    if (s->mb_intra )

                        s->ac_pred = get_bits(gb, 1);

                }

                else

                {

                    

                    if (b_mv_type == BMV_TYPE_INTERPOLATED)

                    {

                        GET_MVDATA(dmv2_x, dmv2_y);

                    }

                    

                    if (mb_has_coeffs )

                    {

                        if (s->mb_intra )

                            s->ac_pred = get_bits(gb, 1);

                        GET_MQUANT();

                    }

                }

            }

            

            if (v->ttmbf)

                ttmb = get_vlc2(gb, v->ttmb_vlc->table,

                                   VC9_TTMB_VLC_BITS, 12);



            

            for (i=0; i<6; i++)

            {

                

            }



            current_mb++;

        }

    }

    return 0;

}