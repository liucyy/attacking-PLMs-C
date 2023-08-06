static void render_fragments(Vp3DecodeContext *s,

                             int first_fragment,

                             int width,

                             int height,

                             int plane ) 

{

    int x, y;

    int m, n;

    int i = first_fragment;

    int j;

    int16_t *dequantizer;

    DCTELEM dequant_block[64];

    unsigned char *output_plane;

    unsigned char *last_plane;

    unsigned char *golden_plane;

    int stride;

    int motion_x, motion_y;

    int motion_x_limit, motion_y_limit;

    int motion_halfpel_index;

    unsigned int motion_source;



    debug_vp3("  vp3: rendering final fragments for %s\n",

        (plane == 0) ? "Y plane" : (plane == 1) ? "U plane" : "V plane");



    

    if (plane == 0) {

        dequantizer = s->intra_y_dequant;

        output_plane = s->current_frame.data[0];

        last_plane = s->last_frame.data[0];

        golden_plane = s->golden_frame.data[0];

        stride = -s->current_frame.linesize[0];

    } else if (plane == 1) {

        dequantizer = s->intra_c_dequant;

        output_plane = s->current_frame.data[1];

        last_plane = s->last_frame.data[1];

        golden_plane = s->golden_frame.data[1];

        stride = -s->current_frame.linesize[1];

    } else {

        dequantizer = s->intra_c_dequant;

        output_plane = s->current_frame.data[2];

        last_plane = s->last_frame.data[2];

        golden_plane = s->golden_frame.data[2];

        stride = -s->current_frame.linesize[2];

    }



    motion_x_limit = width - 8;

    motion_y_limit = height - 8;



    

    for (y = 0; y < height; y += 8) {



        

        for (x = 0; x < width; x += 8, i++) {



            

            if (s->all_fragments[i].coding_method != MODE_COPY) {



                

                motion_x = s->all_fragments[i].motion_x;

                motion_y = s->all_fragments[i].motion_y;

                motion_halfpel_index = s->all_fragments[i].motion_halfpel_index;



                motion_source = s->all_fragments[i].first_pixel;

                motion_source += motion_x;

                motion_source += (motion_y * stride);



                

                if ((s->all_fragments[i].coding_method == MODE_USING_GOLDEN) ||

                    (s->all_fragments[i].coding_method == MODE_GOLDEN_MV)) {



                    s->dsp.put_pixels_tab[1][motion_halfpel_index](

                        output_plane + s->all_fragments[i].first_pixel,

                        golden_plane + motion_source,

                        stride, 8);



                } else 

                if (s->all_fragments[i].coding_method != MODE_INTRA) {



                    s->dsp.put_pixels_tab[1][motion_halfpel_index](

                        output_plane + s->all_fragments[i].first_pixel,

                        last_plane + motion_source,

                        stride, 8);

                }



                

                debug_idct("fragment %d, coding mode %d, DC = %d, dequant = %d:\n", 

                    i, s->all_fragments[i].coding_method, 

                    s->all_fragments[i].coeffs[0], dequantizer[0]);

                for (j = 0; j < 64; j++)

                    dequant_block[dequant_index[j]] =

                        s->all_fragments[i].coeffs[j] *

                        dequantizer[j];



                debug_idct("dequantized block:\n");

                for (m = 0; m < 8; m++) {

                    for (n = 0; n < 8; n++) {

                        debug_idct(" %5d", dequant_block[m * 8 + n]);

                    }

                    debug_idct("\n");

                }

                debug_idct("\n");



                



                if (s->all_fragments[i].coding_method == MODE_INTRA) {

                    dequant_block[0] += 1024;

                    s->dsp.idct_put(

                        output_plane + s->all_fragments[i].first_pixel,

                        stride, dequant_block);

                } else {

                    s->dsp.idct_add(

                        output_plane + s->all_fragments[i].first_pixel,

                        stride, dequant_block);

                }



                debug_idct("block after idct_%s():\n",

                    (s->all_fragments[i].coding_method == MODE_INTRA)?

                    "put" : "add");

                for (m = 0; m < 8; m++) {

                    for (n = 0; n < 8; n++) {

                        debug_idct(" %3d", *(output_plane + 

                            s->all_fragments[i].first_pixel + (m * stride + n)));

                    }

                    debug_idct("\n");

                }

                debug_idct("\n");



            } else {



                

                s->dsp.put_pixels_tab[1][0](

                    output_plane + s->all_fragments[i].first_pixel,

                    last_plane + s->all_fragments[i].first_pixel,

                    stride, 8);



            }

        }

    }



    emms_c();



}