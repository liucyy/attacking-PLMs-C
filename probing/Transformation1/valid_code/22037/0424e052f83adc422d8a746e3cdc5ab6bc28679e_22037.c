static void decode_postinit(H264Context *h){

    MpegEncContext * const s = &h->s;

    Picture *out = s->current_picture_ptr;

    Picture *cur = s->current_picture_ptr;

    int i, pics, out_of_order, out_idx;



    s->current_picture_ptr->qscale_type= FF_QSCALE_TYPE_H264;

    s->current_picture_ptr->pict_type= s->pict_type;



    if (h->next_output_pic) return;



    if (cur->field_poc[0]==INT_MAX || cur->field_poc[1]==INT_MAX) {

        

        

        

        

        return;

    }



    cur->interlaced_frame = 0;

    cur->repeat_pict = 0;



    

    



    if(h->sps.pic_struct_present_flag){

        switch (h->sei_pic_struct)

        {

        case SEI_PIC_STRUCT_FRAME:

            break;

        case SEI_PIC_STRUCT_TOP_FIELD:

        case SEI_PIC_STRUCT_BOTTOM_FIELD:

            cur->interlaced_frame = 1;

            break;

        case SEI_PIC_STRUCT_TOP_BOTTOM:

        case SEI_PIC_STRUCT_BOTTOM_TOP:

            if (FIELD_OR_MBAFF_PICTURE)

                cur->interlaced_frame = 1;

            else

                

                cur->interlaced_frame = h->prev_interlaced_frame;

            break;

        case SEI_PIC_STRUCT_TOP_BOTTOM_TOP:

        case SEI_PIC_STRUCT_BOTTOM_TOP_BOTTOM:

            

            

            cur->repeat_pict = 1;

            break;

        case SEI_PIC_STRUCT_FRAME_DOUBLING:

            

            cur->repeat_pict = 2;

            break;

        case SEI_PIC_STRUCT_FRAME_TRIPLING:

            cur->repeat_pict = 4;

            break;

        }



        if ((h->sei_ct_type & 3) && h->sei_pic_struct <= SEI_PIC_STRUCT_BOTTOM_TOP)

            cur->interlaced_frame = (h->sei_ct_type & (1<<1)) != 0;

    }else{

        

        cur->interlaced_frame = FIELD_OR_MBAFF_PICTURE;

    }

    h->prev_interlaced_frame = cur->interlaced_frame;



    if (cur->field_poc[0] != cur->field_poc[1]){

        

        cur->top_field_first = cur->field_poc[0] < cur->field_poc[1];

    }else{

        if(cur->interlaced_frame || h->sps.pic_struct_present_flag){

            

            if(h->sei_pic_struct == SEI_PIC_STRUCT_TOP_BOTTOM

              || h->sei_pic_struct == SEI_PIC_STRUCT_TOP_BOTTOM_TOP)

                cur->top_field_first = 1;

            else

                cur->top_field_first = 0;

        }else{

            

            cur->top_field_first = 0;

        }

    }



    



    



    if(h->sps.bitstream_restriction_flag

       && s->avctx->has_b_frames < h->sps.num_reorder_frames){

        s->avctx->has_b_frames = h->sps.num_reorder_frames;

        s->low_delay = 0;

    }



    if(   s->avctx->strict_std_compliance >= FF_COMPLIANCE_STRICT

       && !h->sps.bitstream_restriction_flag){

        s->avctx->has_b_frames= MAX_DELAYED_PIC_COUNT;

        s->low_delay= 0;

    }



    pics = 0;

    while(h->delayed_pic[pics]) pics++;



    assert(pics <= MAX_DELAYED_PIC_COUNT);



    h->delayed_pic[pics++] = cur;

    if(cur->reference == 0)

        cur->reference = DELAYED_PIC_REF;



    out = h->delayed_pic[0];

    out_idx = 0;

    for(i=1; h->delayed_pic[i] && !h->delayed_pic[i]->key_frame && !h->delayed_pic[i]->mmco_reset; i++)

        if(h->delayed_pic[i]->poc < out->poc){

            out = h->delayed_pic[i];

            out_idx = i;

        }

    if(s->avctx->has_b_frames == 0 && (h->delayed_pic[0]->key_frame || h->delayed_pic[0]->mmco_reset))

        h->next_outputed_poc= INT_MIN;

    out_of_order = out->poc < h->next_outputed_poc;



    if(h->sps.bitstream_restriction_flag && s->avctx->has_b_frames >= h->sps.num_reorder_frames)

        { }

    else if((out_of_order && pics-1 == s->avctx->has_b_frames && s->avctx->has_b_frames < MAX_DELAYED_PIC_COUNT)

       || (s->low_delay &&

        ((h->next_outputed_poc != INT_MIN && out->poc > h->next_outputed_poc + 2)

         || cur->pict_type == AV_PICTURE_TYPE_B)))

    {

        s->low_delay = 0;

        s->avctx->has_b_frames++;

    }



    if(out_of_order || pics > s->avctx->has_b_frames){

        out->reference &= ~DELAYED_PIC_REF;

        out->owner2 = s; 

                         

        for(i=out_idx; h->delayed_pic[i]; i++)

            h->delayed_pic[i] = h->delayed_pic[i+1];

    }

    if(!out_of_order && pics > s->avctx->has_b_frames){

        h->next_output_pic = out;

        if(out_idx==0 && h->delayed_pic[0] && (h->delayed_pic[0]->key_frame || h->delayed_pic[0]->mmco_reset)) {

            h->next_outputed_poc = INT_MIN;

        } else

            h->next_outputed_poc = out->poc;

    }else{

        av_log(s->avctx, AV_LOG_DEBUG, "no picture\n");

    }



    ff_thread_finish_setup(s->avctx);

}