static int decode_cabac_mb_type( H264Context *h ) {

    MpegEncContext * const s = &h->s;



    if( h->slice_type == I_TYPE ) {

        return decode_cabac_intra_mb_type(h, 3, 1);

    } else if( h->slice_type == P_TYPE ) {

        if( get_cabac( &h->cabac, &h->cabac_state[14] ) == 0 ) {

            

            if( get_cabac( &h->cabac, &h->cabac_state[15] ) == 0 ) {

                

                return 3 * get_cabac( &h->cabac, &h->cabac_state[16] );

            } else {

                

                return 2 - get_cabac( &h->cabac, &h->cabac_state[17] );

            }

        } else {

            return decode_cabac_intra_mb_type(h, 17, 0) + 5;

        }

    } else if( h->slice_type == B_TYPE ) {

        const int mba_xy = h->left_mb_xy[0];

        const int mbb_xy = h->top_mb_xy;

        int ctx = 0;

        int bits;



        if( h->slice_table[mba_xy] == h->slice_num && !IS_DIRECT( s->current_picture.mb_type[mba_xy] ) )

            ctx++;

        if( h->slice_table[mbb_xy] == h->slice_num && !IS_DIRECT( s->current_picture.mb_type[mbb_xy] ) )

            ctx++;



        if( !get_cabac( &h->cabac, &h->cabac_state[27+ctx] ) )

            return 0; 



        if( !get_cabac( &h->cabac, &h->cabac_state[27+3] ) ) {

            return 1 + get_cabac( &h->cabac, &h->cabac_state[27+5] ); 

        }



        bits = get_cabac( &h->cabac, &h->cabac_state[27+4] ) << 3;

        bits|= get_cabac( &h->cabac, &h->cabac_state[27+5] ) << 2;

        bits|= get_cabac( &h->cabac, &h->cabac_state[27+5] ) << 1;

        bits|= get_cabac( &h->cabac, &h->cabac_state[27+5] );

        if( bits < 8 )

            return bits + 3; 

        else if( bits == 13 ) {

            return decode_cabac_intra_mb_type(h, 32, 0) + 23;

        } else if( bits == 14 )

            return 11; 

        else if( bits == 15 )

            return 22; 



        bits= ( bits<<1 ) | get_cabac( &h->cabac, &h->cabac_state[27+5] );

        return bits - 4; 

    } else {

        

        return -1;

    }

}