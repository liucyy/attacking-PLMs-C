static int dirac_unpack_idwt_params(DiracContext *s)

{

    GetBitContext *gb = &s->gb;

    int i, level;

    unsigned tmp;



#define CHECKEDREAD(dst, cond, errmsg) \

    tmp = svq3_get_ue_golomb(gb); \

    if (cond) { \

        av_log(s->avctx, AV_LOG_ERROR, errmsg); \

        return -1; \

    }\

    dst = tmp;



    align_get_bits(gb);



    s->zero_res = s->num_refs ? get_bits1(gb) : 0;

    if (s->zero_res)

        return 0;



    

    CHECKEDREAD(s->wavelet_idx, tmp > 6, "wavelet_idx is too big\n")



    CHECKEDREAD(s->wavelet_depth, tmp > MAX_DWT_LEVELS || tmp < 1, "invalid number of DWT decompositions\n")



    if (!s->low_delay) {

        

        if (get_bits1(gb)) {

            for (i = 0; i <= s->wavelet_depth; i++) {

                CHECKEDREAD(s->codeblock[i].width , tmp < 1, "codeblock width invalid\n")

                CHECKEDREAD(s->codeblock[i].height, tmp < 1, "codeblock height invalid\n")

            }



            CHECKEDREAD(s->codeblock_mode, tmp > 1, "unknown codeblock mode\n")

        } else

            for (i = 0; i <= s->wavelet_depth; i++)

                s->codeblock[i].width = s->codeblock[i].height = 1;

    } else {

        

        

        s->lowdelay.num_x     = svq3_get_ue_golomb(gb);

        s->lowdelay.num_y     = svq3_get_ue_golomb(gb);

        s->lowdelay.bytes.num = svq3_get_ue_golomb(gb);

        s->lowdelay.bytes.den = svq3_get_ue_golomb(gb);



        if (s->lowdelay.bytes.den <= 0) {

            av_log(s->avctx,AV_LOG_ERROR,"Invalid lowdelay.bytes.den\n");

            return AVERROR_INVALIDDATA;

        }



        

        if (get_bits1(gb)) {

            av_log(s->avctx,AV_LOG_DEBUG,"Low Delay: Has Custom Quantization Matrix!\n");

            

            s->lowdelay.quant[0][0] = svq3_get_ue_golomb(gb);

            for (level = 0; level < s->wavelet_depth; level++) {

                s->lowdelay.quant[level][1] = svq3_get_ue_golomb(gb);

                s->lowdelay.quant[level][2] = svq3_get_ue_golomb(gb);

                s->lowdelay.quant[level][3] = svq3_get_ue_golomb(gb);

            }

        } else {

            if (s->wavelet_depth > 4) {

                av_log(s->avctx,AV_LOG_ERROR,"Mandatory custom low delay matrix missing for depth %d\n", s->wavelet_depth);

                return AVERROR_INVALIDDATA;

            }

            

            for (level = 0; level < s->wavelet_depth; level++)

                for (i = 0; i < 4; i++) {

                    s->lowdelay.quant[level][i] = default_qmat[s->wavelet_idx][level][i];

                    

                    if (s->wavelet_idx == 3)

                        s->lowdelay.quant[level][i] += 4*(s->wavelet_depth-1 - level);

                }

        }

    }

    return 0;

}