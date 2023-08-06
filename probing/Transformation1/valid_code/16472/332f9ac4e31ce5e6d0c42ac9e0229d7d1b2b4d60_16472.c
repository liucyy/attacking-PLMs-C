void h263_encode_picture_header(MpegEncContext * s, int picture_number)

{

    int format;



    align_put_bits(&s->pb);



    

    s->ptr_lastgob = pbBufPtr(&s->pb);

    s->gob_number = 0;



    put_bits(&s->pb, 22, 0x20); 

    put_bits(&s->pb, 8, (((int64_t)s->picture_number * 30 * s->avctx->frame_rate_base) / 

                         s->avctx->frame_rate) & 0xff);



    put_bits(&s->pb, 1, 1);	

    put_bits(&s->pb, 1, 0);	

    put_bits(&s->pb, 1, 0);	

    put_bits(&s->pb, 1, 0);	

    put_bits(&s->pb, 1, 0);	

    

    format = h263_get_picture_format(s->width, s->height);

    if (!s->h263_plus) {

        

        put_bits(&s->pb, 3, format);

        put_bits(&s->pb, 1, (s->pict_type == P_TYPE));

        

        put_bits(&s->pb, 1, 0);	

        put_bits(&s->pb, 1, 0);	

        put_bits(&s->pb, 1, s->obmc);	

        put_bits(&s->pb, 1, 0);	

        put_bits(&s->pb, 5, s->qscale);

        put_bits(&s->pb, 1, 0);	

    } else {

        

        

        put_bits(&s->pb, 3, 7);

        put_bits(&s->pb,3,1); 

        if (format == 7)

            put_bits(&s->pb,3,6); 

        else

            put_bits(&s->pb, 3, format);

            

        put_bits(&s->pb,1,0); 

        s->umvplus = s->unrestricted_mv;

        put_bits(&s->pb, 1, s->umvplus); 

        put_bits(&s->pb,1,0); 

        put_bits(&s->pb,1,s->obmc); 

        put_bits(&s->pb,1,s->h263_aic); 

        put_bits(&s->pb,1,0); 

        put_bits(&s->pb,1,0); 

        put_bits(&s->pb,1,0); 

        put_bits(&s->pb,1,0); 

        put_bits(&s->pb,1,s->alt_inter_vlc); 

        put_bits(&s->pb,1,0); 

        put_bits(&s->pb,1,1); 

        put_bits(&s->pb,3,0); 

		

        put_bits(&s->pb, 3, s->pict_type == P_TYPE);

		

        put_bits(&s->pb,1,0); 

        put_bits(&s->pb,1,0); 

        put_bits(&s->pb,1,s->no_rounding); 

        put_bits(&s->pb,2,0); 

        put_bits(&s->pb,1,1); 

		

        

        put_bits(&s->pb, 1, 0);	

		

		if (format == 7) {

            

            aspect_to_info(s, s->avctx->sample_aspect_ratio);



            put_bits(&s->pb,4,s->aspect_ratio_info);

            put_bits(&s->pb,9,(s->width >> 2) - 1);

            put_bits(&s->pb,1,1); 

            put_bits(&s->pb,9,(s->height >> 2));

            if (s->aspect_ratio_info == FF_ASPECT_EXTENDED){

                put_bits(&s->pb, 8, s->avctx->sample_aspect_ratio.num);

                put_bits(&s->pb, 8, s->avctx->sample_aspect_ratio.den);

	    }

        }

        

        

        if (s->umvplus)



            put_bits(&s->pb,2,1); 



        put_bits(&s->pb, 5, s->qscale);

    }



    put_bits(&s->pb, 1, 0);	



    if(s->h263_aic){

         s->y_dc_scale_table= 

         s->c_dc_scale_table= ff_aic_dc_scale_table;

    }else{

        s->y_dc_scale_table=

        s->c_dc_scale_table= ff_mpeg1_dc_scale_table;

    }

}