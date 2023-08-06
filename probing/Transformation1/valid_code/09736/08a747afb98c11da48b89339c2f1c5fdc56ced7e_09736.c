void ff_eac3_output_frame_header(AC3EncodeContext *s)

{

    int blk, ch;

    AC3EncOptions *opt = &s->options;



    put_bits(&s->pb, 16, 0x0b77);                   



    

    put_bits(&s->pb,  2, 0);                        

    put_bits(&s->pb,  3, 0);                        

    put_bits(&s->pb, 11, (s->frame_size / 2) - 1);  

    if (s->bit_alloc.sr_shift) {

        put_bits(&s->pb, 2, 0x3);                   

        put_bits(&s->pb, 2, s->bit_alloc.sr_code);  

    } else {

        put_bits(&s->pb, 2, s->bit_alloc.sr_code);  

        put_bits(&s->pb, 2, 0x3);                   

    }

    put_bits(&s->pb, 3, s->channel_mode);           

    put_bits(&s->pb, 1, s->lfe_on);                 

    put_bits(&s->pb, 5, s->bitstream_id);           

    put_bits(&s->pb, 5, -opt->dialogue_level);      

    put_bits(&s->pb, 1, 0);                         

    put_bits(&s->pb, 1, 0);                         

    

    put_bits(&s->pb, 1, 0);                         

    

    put_bits(&s->pb, 1, 0);                         



    

    put_bits(&s->pb, 1, 1);                         

    put_bits(&s->pb, 1, 0);                         

    put_bits(&s->pb, 2, 0);                         

    put_bits(&s->pb, 1, 0);                         

    put_bits(&s->pb, 1, 0);                         

    put_bits(&s->pb, 1, 0);                         

    put_bits(&s->pb, 1, 0);                         

    put_bits(&s->pb, 1, 0);                         

    put_bits(&s->pb, 1, 0);                         

    put_bits(&s->pb, 1, 0);                         

    put_bits(&s->pb, 1, 0);                         

    

    if (s->channel_mode > AC3_CHMODE_MONO) {

        put_bits(&s->pb, 1, s->blocks[0].cpl_in_use);

        for (blk = 1; blk < AC3_MAX_BLOCKS; blk++) {

            AC3Block *block = &s->blocks[blk];

            put_bits(&s->pb, 1, block->new_cpl_strategy);

            if (block->new_cpl_strategy)

                put_bits(&s->pb, 1, block->cpl_in_use);

        }

    }

    

    for (blk = 0; blk < AC3_MAX_BLOCKS; blk++)

        for (ch = !s->blocks[blk].cpl_in_use; ch <= s->fbw_channels; ch++)

            put_bits(&s->pb, 2, s->exp_strategy[ch][blk]);

    if (s->lfe_on) {

        for (blk = 0; blk < AC3_MAX_BLOCKS; blk++)

            put_bits(&s->pb, 1, s->exp_strategy[s->lfe_channel][blk]);

    }

    

    for (ch = 1; ch <= s->fbw_channels; ch++)

        put_bits(&s->pb, 5, 0);

    

    put_bits(&s->pb, 6, s->coarse_snr_offset);

    put_bits(&s->pb, 4, s->fine_snr_offset[1]);

    

    put_bits(&s->pb, 1, 0);

}