static void init_band_stepsize(AVCodecContext *avctx,

                               Jpeg2000Band *band,

                               Jpeg2000CodingStyle *codsty,

                               Jpeg2000QuantStyle *qntsty,

                               int bandno, int gbandno, int reslevelno,

                               int cbps)

{

    

    switch (qntsty->quantsty) {

        uint8_t gain;

    case JPEG2000_QSTY_NONE:

        

        band->f_stepsize = 1;

        break;

    case JPEG2000_QSTY_SI:

        











    case JPEG2000_QSTY_SE:

        

        gain            = cbps;

        band->f_stepsize  = pow(2.0, gain - qntsty->expn[gbandno]);

        band->f_stepsize *= qntsty->mant[gbandno] / 2048.0 + 1.0;

        break;

    default:

        band->f_stepsize = 0;

        av_log(avctx, AV_LOG_ERROR, "Unknown quantization format\n");

        break;

    }

    if (codsty->transform != FF_DWT53) {

        int lband = 0;

        switch (bandno + (reslevelno > 0)) {

            case 1:

            case 2:

                band->f_stepsize *= F_LFTG_X * 2;

                lband = 1;

                break;

            case 3:

                band->f_stepsize *= F_LFTG_X * F_LFTG_X * 4;

                break;

        }

        if (codsty->transform == FF_DWT97) {

            band->f_stepsize *= pow(F_LFTG_K, 2*(codsty->nreslevels2decode - reslevelno) + lband - 2);

        }

    }



    band->i_stepsize = band->f_stepsize * (1 << 15);



    

    if (!av_codec_is_encoder(avctx->codec))

        band->f_stepsize *= 0.5;

}