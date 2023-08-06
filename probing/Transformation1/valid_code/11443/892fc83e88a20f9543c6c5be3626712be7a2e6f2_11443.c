static void unpack_superblocks(Vp3DecodeContext *s, GetBitContext *gb)

{

    int bit = 0;

    int current_superblock = 0;

    int current_run = 0;

    int decode_fully_flags = 0;

    int decode_partial_blocks = 0;



    int i, j;

    int current_fragment;



    debug_vp3("  vp3: unpacking superblock coding\n");



    if (s->keyframe) {



        debug_vp3("    keyframe-- all superblocks are fully coded\n");

        memset(s->superblock_coding, SB_FULLY_CODED, s->superblock_count);



    } else {



        

        bit = get_bits(gb, 1);

        

        bit ^= 1;

        while (current_superblock < s->superblock_count) {

            if (current_run == 0) {

                bit ^= 1;

                current_run = get_superblock_run_length(gb);

                debug_block_coding("      setting superblocks %d..%d to %s\n",

                    current_superblock,

                    current_superblock + current_run - 1,

                    (bit) ? "partially coded" : "not coded");



                

                if (bit == 0)

                    decode_fully_flags = 1;

            } else {



                

                decode_partial_blocks = 1;



            }

            s->superblock_coding[current_superblock++] = 

                (bit) ? SB_PARTIALLY_CODED : SB_NOT_CODED;

            current_run--;

        }



        

        if (decode_fully_flags) {



            current_superblock = 0;

            current_run = 0;

            bit = get_bits(gb, 1);

            

            bit ^= 1;

            while (current_superblock < s->superblock_count) {



                

                if (s->superblock_coding[current_superblock] == SB_NOT_CODED) {



                    if (current_run == 0) {

                        bit ^= 1;

                        current_run = get_superblock_run_length(gb);

                    }



                    debug_block_coding("      setting superblock %d to %s\n",

                        current_superblock,

                        (bit) ? "fully coded" : "not coded");

                    s->superblock_coding[current_superblock] = 

                        (bit) ? SB_FULLY_CODED : SB_NOT_CODED;

                    current_run--;

                }

                current_superblock++;

            }

        }



        

        if (decode_partial_blocks) {



            current_run = 0;

            bit = get_bits(gb, 1);

            

            bit ^= 1;

        }

    }



    

    s->coded_fragment_list_index = 0;

    s->first_coded_y_fragment = s->first_coded_c_fragment = 0;

    s->last_coded_y_fragment = s->last_coded_c_fragment = -1;

    memset(s->macroblock_coded, 0, s->macroblock_count);

    for (i = 0; i < s->superblock_count; i++) {



        

        for (j = 0; j < 16; j++) {



            

            current_fragment = s->superblock_fragments[i * 16 + j];

            if (current_fragment != -1) {

                if (s->superblock_coding[i] == SB_NOT_CODED) {



                    

                    s->all_fragments[current_fragment].coding_method = 

                        MODE_COPY;



                } else if (s->superblock_coding[i] == SB_PARTIALLY_CODED) {



                    

                    if (current_run == 0) {

                        bit ^= 1;

                        current_run = get_fragment_run_length(gb);

                    }



                    if (bit) {

                        

                        s->all_fragments[current_fragment].coding_method = 

                            MODE_INTER_NO_MV;

                        s->coded_fragment_list[s->coded_fragment_list_index] = 

                            current_fragment;

                        if ((current_fragment >= s->u_fragment_start) &&

                            (s->last_coded_y_fragment == -1)) {

                            s->first_coded_c_fragment = s->coded_fragment_list_index;

                            s->last_coded_y_fragment = s->first_coded_c_fragment - 1;

                        }

                        s->coded_fragment_list_index++;

                        s->macroblock_coded[s->all_fragments[current_fragment].macroblock] = 1;

                        debug_block_coding("      superblock %d is partially coded, fragment %d is coded\n",

                            i, current_fragment);

                    } else {

                        

                        s->all_fragments[current_fragment].coding_method =

                            MODE_COPY;

                        debug_block_coding("      superblock %d is partially coded, fragment %d is not coded\n",

                            i, current_fragment);

                    }



                    current_run--;



                } else {



                    

                    s->all_fragments[current_fragment].coding_method = 

                        MODE_INTER_NO_MV;

                    s->coded_fragment_list[s->coded_fragment_list_index] = 

                        current_fragment;

                    if ((current_fragment >= s->u_fragment_start) &&

                        (s->last_coded_y_fragment == -1)) {

                        s->first_coded_c_fragment = s->coded_fragment_list_index;

                        s->last_coded_y_fragment = s->first_coded_c_fragment - 1;

                    }

                    s->coded_fragment_list_index++;

                    s->macroblock_coded[s->all_fragments[current_fragment].macroblock] = 1;

                    debug_block_coding("      superblock %d is fully coded, fragment %d is coded\n",

                        i, current_fragment);

                }

            }

        }

    }



    if (s->first_coded_c_fragment == 0)

        

        s->last_coded_y_fragment = s->coded_fragment_list_index - 1;

    else

        s->last_coded_c_fragment = s->coded_fragment_list_index - 1;

    debug_block_coding("    %d total coded fragments, y: %d -> %d, c: %d -> %d\n",

        s->coded_fragment_list_index,

        s->first_coded_y_fragment,

        s->last_coded_y_fragment,

        s->first_coded_c_fragment,

        s->last_coded_c_fragment);

}