static void bastardized_rice_decompress(ALACContext *alac,

                                 int32_t *output_buffer,

                                 int output_size,

                                 int readsamplesize, 

                                 int rice_initialhistory, 

                                 int rice_kmodifier, 

                                 int rice_historymult, 

                                 int rice_kmodifier_mask 

        )

{

    int output_count;

    unsigned int history = rice_initialhistory;

    int sign_modifier = 0;



    for (output_count = 0; output_count < output_size; output_count++) {

        int32_t x;

        int32_t x_modified;

        int32_t final_val;



        

        x = get_unary_0_9(&alac->gb);



        if (x > 8) { 

            

            x = get_bits(&alac->gb, readsamplesize);

        } else {

            

            int extrabits;

            int k; 



            

            k = 31 - count_leading_zeros((history >> 9) + 3);



            if (k >= rice_kmodifier)

                k = rice_kmodifier;



            if (k != 1) {

                extrabits = show_bits(&alac->gb, k);



                

                x = (x << k) - x;



                if (extrabits > 1) {

                    x += extrabits - 1;

                    skip_bits(&alac->gb, k);

                } else

                    skip_bits(&alac->gb, k - 1);

            }

        }



        x_modified = sign_modifier + x;

        final_val = (x_modified + 1) / 2;

        if (x_modified & 1) final_val *= -1;



        output_buffer[output_count] = final_val;



        sign_modifier = 0;



        

        history += x_modified * rice_historymult

                   - ((history * rice_historymult) >> 9);



        if (x_modified > 0xffff)

            history = 0xffff;



        

        if ((history < 128) && (output_count+1 < output_size)) {

            int block_size;



            sign_modifier = 1;



            x = get_unary_0_9(&alac->gb);



            if (x > 8) {

                block_size = get_bits(&alac->gb, 16);

            } else {

                int k;

                int extrabits;



                k = count_leading_zeros(history) + ((history + 16) >> 6 ) - 24;



                extrabits = show_bits(&alac->gb, k);



                block_size = (((1 << k) - 1) & rice_kmodifier_mask) * x

                           + extrabits - 1;



                if (extrabits < 2) {

                    x = 1 - extrabits;

                    block_size += x;

                    skip_bits(&alac->gb, k - 1);

                } else {

                    skip_bits(&alac->gb, k);

                }

            }



            if (block_size > 0) {

                memset(&output_buffer[output_count+1], 0, block_size * 4);

                output_count += block_size;

            }



            if (block_size > 0xffff)

                sign_modifier = 0;



            history = 0;

        }

    }

}