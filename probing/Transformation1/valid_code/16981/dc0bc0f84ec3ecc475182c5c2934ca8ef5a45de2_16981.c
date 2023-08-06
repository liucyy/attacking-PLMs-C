static int compute_bit_allocation(AC3EncodeContext *s,

                                  uint8_t bap[AC3_MAX_BLOCKS][AC3_MAX_CHANNELS][AC3_MAX_COEFS],

                                  uint8_t encoded_exp[AC3_MAX_BLOCKS][AC3_MAX_CHANNELS][AC3_MAX_COEFS],

                                  uint8_t exp_strategy[AC3_MAX_BLOCKS][AC3_MAX_CHANNELS],

                                  int frame_bits)

{

    int blk, ch;

    int coarse_snr_offset, fine_snr_offset;

    uint8_t bap1[AC3_MAX_BLOCKS][AC3_MAX_CHANNELS][AC3_MAX_COEFS];

    int16_t psd[AC3_MAX_BLOCKS][AC3_MAX_CHANNELS][AC3_MAX_COEFS];

    int16_t mask[AC3_MAX_BLOCKS][AC3_MAX_CHANNELS][AC3_CRITICAL_BANDS];

    static const int frame_bits_inc[8] = { 0, 0, 2, 2, 2, 4, 2, 4 };



    

    s->slow_decay_code = 2;

    s->fast_decay_code = 1;

    s->slow_gain_code  = 1;

    s->db_per_bit_code = 2;

    s->floor_code      = 4;

    for (ch = 0; ch < s->channels; ch++)

        s->fast_gain_code[ch] = 4;



    

    s->bit_alloc.slow_decay = ff_ac3_slow_decay_tab[s->slow_decay_code] >> s->bit_alloc.sr_shift;

    s->bit_alloc.fast_decay = ff_ac3_fast_decay_tab[s->fast_decay_code] >> s->bit_alloc.sr_shift;

    s->bit_alloc.slow_gain  = ff_ac3_slow_gain_tab[s->slow_gain_code];

    s->bit_alloc.db_per_bit = ff_ac3_db_per_bit_tab[s->db_per_bit_code];

    s->bit_alloc.floor      = ff_ac3_floor_tab[s->floor_code];



    

    frame_bits += 65;

    

    

    frame_bits += frame_bits_inc[s->channel_mode];



    

    for (blk = 0; blk < AC3_MAX_BLOCKS; blk++) {

        frame_bits += s->fbw_channels * 2 + 2; 

        if (s->channel_mode == AC3_CHMODE_STEREO) {

            frame_bits++; 

            if (!blk)

                frame_bits += 4;

        }

        frame_bits += 2 * s->fbw_channels; 

        if (s->lfe_on)

            frame_bits++; 

        for (ch = 0; ch < s->fbw_channels; ch++) {

            if (exp_strategy[blk][ch] != EXP_REUSE)

                frame_bits += 6 + 2; 

        }

        frame_bits++; 

        frame_bits++; 

        frame_bits += 2; 

    }

    frame_bits++; 

    

    

    

    

    frame_bits += 2*4 + 3 + 6 + s->channels * (4 + 3);



    

    frame_bits += 2;



    

    frame_bits += 16;



    

    bit_alloc_masking(s, encoded_exp, exp_strategy, psd, mask);



    



    coarse_snr_offset = s->coarse_snr_offset;

    while (coarse_snr_offset >= 0 &&

           bit_alloc(s, mask, psd, bap, frame_bits, coarse_snr_offset, 0) < 0)

        coarse_snr_offset -= SNR_INC1;

    if (coarse_snr_offset < 0) {

        av_log(NULL, AV_LOG_ERROR, "Bit allocation failed. Try increasing the bitrate.\n");

        return -1;

    }

    while (coarse_snr_offset + SNR_INC1 <= 63 &&

           bit_alloc(s, mask, psd, bap1, frame_bits,

                     coarse_snr_offset + SNR_INC1, 0) >= 0) {

        coarse_snr_offset += SNR_INC1;

        memcpy(bap, bap1, sizeof(bap1));

    }

    while (coarse_snr_offset + 1 <= 63 &&

           bit_alloc(s, mask, psd, bap1, frame_bits, coarse_snr_offset + 1, 0) >= 0) {

        coarse_snr_offset++;

        memcpy(bap, bap1, sizeof(bap1));

    }



    fine_snr_offset = 0;

    while (fine_snr_offset + SNR_INC1 <= 15 &&

           bit_alloc(s, mask, psd, bap1, frame_bits,

                     coarse_snr_offset, fine_snr_offset + SNR_INC1) >= 0) {

        fine_snr_offset += SNR_INC1;

        memcpy(bap, bap1, sizeof(bap1));

    }

    while (fine_snr_offset + 1 <= 15 &&

           bit_alloc(s, mask, psd, bap1, frame_bits,

                     coarse_snr_offset, fine_snr_offset + 1) >= 0) {

        fine_snr_offset++;

        memcpy(bap, bap1, sizeof(bap1));

    }



    s->coarse_snr_offset = coarse_snr_offset;

    for (ch = 0; ch < s->channels; ch++)

        s->fine_snr_offset[ch] = fine_snr_offset;



    return 0;

}