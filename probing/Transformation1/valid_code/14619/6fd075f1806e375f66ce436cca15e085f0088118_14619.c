int avpriv_adx_decode_header(AVCodecContext *avctx, const uint8_t *buf,

                             int bufsize, int *header_size, int *coeff)

{

    int offset, cutoff;



    if (bufsize < 24)

        return AVERROR_INVALIDDATA;



    if (AV_RB16(buf) != 0x8000)

        return AVERROR_INVALIDDATA;

    offset = AV_RB16(buf + 2) + 4;



    

    if (bufsize >= offset && memcmp(buf + offset - 6, "(c)CRI", 6))

        return AVERROR_INVALIDDATA;



    

    if (buf[4] != 3 || buf[5] != 18 || buf[6] != 4) {

        av_log_ask_for_sample(avctx, "unsupported ADX format\n");

        return AVERROR_PATCHWELCOME;

    }



    

    avctx->channels = buf[7];

    if (avctx->channels > 2)

        return AVERROR_INVALIDDATA;



    

    avctx->sample_rate = AV_RB32(buf + 8);

    if (avctx->sample_rate < 1 ||

        avctx->sample_rate > INT_MAX / (avctx->channels * BLOCK_SIZE * 8))

        return AVERROR_INVALIDDATA;



    

    avctx->bit_rate = avctx->sample_rate * avctx->channels * BLOCK_SIZE * 8 / BLOCK_SAMPLES;



    

    if (coeff) {

        cutoff = AV_RB16(buf + 16);

        ff_adx_calculate_coeffs(cutoff, avctx->sample_rate, COEFF_BITS, coeff);

    }



    *header_size = offset;

    return 0;

}