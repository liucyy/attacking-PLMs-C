static av_always_inline void mpeg_motion_lowres(MpegEncContext *s,

                               uint8_t *dest_y, uint8_t *dest_cb, uint8_t *dest_cr,

                               int field_based, int bottom_field, int field_select,

                               uint8_t **ref_picture, h264_chroma_mc_func *pix_op,

                               int motion_x, int motion_y, int h)

{

    uint8_t *ptr_y, *ptr_cb, *ptr_cr;

    int mx, my, src_x, src_y, uvsrc_x, uvsrc_y, uvlinesize, linesize, sx, sy, uvsx, uvsy;

    const int lowres= s->avctx->lowres;

    const int block_s= 8>>lowres;

    const int s_mask= (2<<lowres)-1;

    const int h_edge_pos = s->h_edge_pos >> lowres;

    const int v_edge_pos = s->v_edge_pos >> lowres;

    linesize   = s->current_picture.linesize[0] << field_based;

    uvlinesize = s->current_picture.linesize[1] << field_based;



    if(s->quarter_sample){ 

        motion_x/=2;

        motion_y/=2;

    }



    if(field_based){

        motion_y += (bottom_field - field_select)*((1<<lowres)-1);

    }



    sx= motion_x & s_mask;

    sy= motion_y & s_mask;

    src_x = s->mb_x*2*block_s               + (motion_x >> (lowres+1));

    src_y =(s->mb_y*2*block_s>>field_based) + (motion_y >> (lowres+1));



    if (s->out_format == FMT_H263) {

        uvsx = ((motion_x>>1) & s_mask) | (sx&1);

        uvsy = ((motion_y>>1) & s_mask) | (sy&1);

        uvsrc_x = src_x>>1;

        uvsrc_y = src_y>>1;

    }else if(s->out_format == FMT_H261){

        mx = motion_x / 4;

        my = motion_y / 4;

        uvsx = (2*mx) & s_mask;

        uvsy = (2*my) & s_mask;

        uvsrc_x = s->mb_x*block_s               + (mx >> lowres);

        uvsrc_y = s->mb_y*block_s               + (my >> lowres);

    } else {

        mx = motion_x / 2;

        my = motion_y / 2;

        uvsx = mx & s_mask;

        uvsy = my & s_mask;

        uvsrc_x = s->mb_x*block_s               + (mx >> (lowres+1));

        uvsrc_y =(s->mb_y*block_s>>field_based) + (my >> (lowres+1));

    }



    ptr_y  = ref_picture[0] + src_y * linesize + src_x;

    ptr_cb = ref_picture[1] + uvsrc_y * uvlinesize + uvsrc_x;

    ptr_cr = ref_picture[2] + uvsrc_y * uvlinesize + uvsrc_x;



    if(   (unsigned)src_x > h_edge_pos                 - (!!sx) - 2*block_s

       || (unsigned)src_y >(v_edge_pos >> field_based) - (!!sy) - h){

            ff_emulated_edge_mc(s->edge_emu_buffer, ptr_y, s->linesize, 17, 17+field_based,

                             src_x, src_y<<field_based, h_edge_pos, v_edge_pos);

            ptr_y = s->edge_emu_buffer;

            if(!(s->flags&CODEC_FLAG_GRAY)){

                uint8_t *uvbuf= s->edge_emu_buffer+18*s->linesize;

                ff_emulated_edge_mc(uvbuf  , ptr_cb, s->uvlinesize, 9, 9+field_based,

                                 uvsrc_x, uvsrc_y<<field_based, h_edge_pos>>1, v_edge_pos>>1);

                ff_emulated_edge_mc(uvbuf+16, ptr_cr, s->uvlinesize, 9, 9+field_based,

                                 uvsrc_x, uvsrc_y<<field_based, h_edge_pos>>1, v_edge_pos>>1);

                ptr_cb= uvbuf;

                ptr_cr= uvbuf+16;

            }

    }



    if(bottom_field){ 

        dest_y += s->linesize;

        dest_cb+= s->uvlinesize;

        dest_cr+= s->uvlinesize;

    }



    if(field_select){

        ptr_y += s->linesize;

        ptr_cb+= s->uvlinesize;

        ptr_cr+= s->uvlinesize;

    }



    sx <<= 2 - lowres;

    sy <<= 2 - lowres;

    pix_op[lowres-1](dest_y, ptr_y, linesize, h, sx, sy);



    if(!(s->flags&CODEC_FLAG_GRAY)){

        uvsx <<= 2 - lowres;

        uvsy <<= 2 - lowres;

        pix_op[lowres](dest_cb, ptr_cb, uvlinesize, h >> s->chroma_y_shift, uvsx, uvsy);

        pix_op[lowres](dest_cr, ptr_cr, uvlinesize, h >> s->chroma_y_shift, uvsx, uvsy);

    }

    

}