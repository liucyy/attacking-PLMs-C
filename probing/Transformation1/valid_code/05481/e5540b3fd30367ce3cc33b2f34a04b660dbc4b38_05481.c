static int decode_b_mbs(VC9Context *v)

{

    int x, y, current_mb = 0 , last_mb = v->height_mb*v->width_mb,

        i ;

    int direct_b_bit = 0, skip_mb_bit = 0;

    int ac_pred;

    int b_mv1 = 0, b_mv2 = 0, b_mv_type = 0;

    int mquant, mqdiff; 

    int tt_block; 

    

    for (y=0; y<v->height_mb; y++)

    {

        for (x=0; x<v->width_mb; x++)

        {

            if (v->direct_mb_plane[current_mb])

              direct_b_bit = get_bits(&v->gb, 1);

            if (1 )

            {

                

#if 0

                skip_mb_bit = get_bits(&v->gb, n); 

#endif

            }

            if (!direct_b_bit)

            {

                if (skip_mb_bit)

                {

                    

#if 0

                    b_mv_type = get_bits(&v->gb, n); 

#endif

                }

                else

                { 

                    

#if 0

                    b_mv1 = get_bits(&v->gb, n); 

#endif

                    if (1 )

                    {

                        

                        b_mv_type = 0; 

                    }

                }

            }

            if (!skip_mb_bit)

            {

                if (b_mv1 != last_mb)

                {

                    GET_MQUANT();

                    if (1 )

                        ac_pred = get_bits(&v->gb, 1);

                }

                else

                {

                    if (1 )

                    {

                        

                        b_mv2 = 0; 

                    }

                    if (1 )

                    {

                        if (1 )

                            ac_pred = get_bits(&v->gb, 1);

                        GET_MQUANT();

                    }

                }

            }

            

            

#if 0

            if (v->ttmbf)

                v->ttmb = get_bits(&v->gb, n); 

#endif

        }

        

        for (i=0; i<6; i++)

        {

            

        }



        current_mb++;

    }

    return 0;

}