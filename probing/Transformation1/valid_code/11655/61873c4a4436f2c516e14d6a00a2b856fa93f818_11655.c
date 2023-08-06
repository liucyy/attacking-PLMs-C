static void render_fragments(Vp3DecodeContext *s,

                             int first_fragment,

                             int fragment_width,

                             int fragment_height,

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



    debug_vp3("  vp3: rendering final fragments for %s\n",

        (plane == 0) ? "Y plane" : (plane == 1) ? "U plane" : "V plane");



    

    if (plane == 0) {

        dequantizer = s->intra_y_dequant;

        output_plane = s->current_frame.data[0];

        last_plane = s->current_frame.data[0];

        golden_plane = s->current_frame.data[0];

        stride = -s->current_frame.linesize[0];

    } else if (plane == 1) {

        dequantizer = s->intra_c_dequant;

        output_plane = s->current_frame.data[1];

        last_plane = s->current_frame.data[1];

        golden_plane = s->current_frame.data[1];

        stride = -s->current_frame.linesize[1];

    } else {

        dequantizer = s->intra_c_dequant;

        output_plane = s->current_frame.data[2];

        last_plane = s->current_frame.data[2];

        golden_plane = s->current_frame.data[2];

        stride = -s->current_frame.linesize[2];

    }



    

    for (y = 0; y < fragment_height; y++) {



        

        for (x = 0; x < fragment_width; x++, i++) {



            

            if (s->all_fragments[i].coding_method == MODE_INTRA) {

                

                for (j = 0; j < 64; j++)

                    dequant_block[dequant_index[j]] =

                        s->all_fragments[i].coeffs[j] *

                        dequantizer[j];

                dequant_block[0] += 1024;



                debug_idct("fragment %d:\n", i);

                debug_idct("dequantized block:\n");

                for (m = 0; m < 8; m++) {

                    for (n = 0; n < 8; n++) {

                        debug_idct(" %5d", dequant_block[m * 8 + n]);

                    }

                    debug_idct("\n");

                }

                debug_idct("\n");



                

                s->dsp.idct_put(

                    output_plane + s->all_fragments[i].first_pixel,

                    stride, dequant_block);





            } else if (s->all_fragments[i].coding_method == MODE_COPY) {



                

                for (m = 0; m < 8; m++)

                    memcpy(

                        output_plane + s->all_fragments[i].first_pixel + stride * m,

                        last_plane + s->all_fragments[i].first_pixel + stride * m,

                        8);



            } else {



                



            }

        }

    }



    emms_c();



}