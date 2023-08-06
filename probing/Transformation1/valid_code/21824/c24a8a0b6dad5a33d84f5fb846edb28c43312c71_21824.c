static inline uint64_t inline_cvttq(CPUAlphaState *env, uint64_t a,

                                    int roundmode, int VI)

{

    uint64_t frac, ret = 0;

    uint32_t exp, sign, exc = 0;

    int shift;



    sign = (a >> 63);

    exp = (uint32_t)(a >> 52) & 0x7ff;

    frac = a & 0xfffffffffffffull;



    if (exp == 0) {

        if (unlikely(frac != 0)) {

            goto do_underflow;

        }

    } else if (exp == 0x7ff) {

        exc = (frac ? FPCR_INV : VI ? FPCR_OVF : 0);

    } else {

        

        frac |= 0x10000000000000ull;



        shift = exp - 1023 - 52;

        if (shift >= 0) {

            

            if (shift < 63) {

                ret = frac << shift;

                if (VI && (ret >> shift) != frac) {

                    exc = FPCR_OVF;

                }

            }

        } else {

            uint64_t round;



            

            shift = -shift;

            if (shift < 63) {

                ret = frac >> shift;

                round = frac << (64 - shift);

            } else {

                

            do_underflow:

                round = 1;

            }



            if (round) {

                exc = (VI ? FPCR_INE : 0);

                switch (roundmode) {

                case float_round_nearest_even:

                    if (round == (1ull << 63)) {

                        

                        ret += (ret & 1);

                    } else if (round > (1ull << 63)) {

                        ret += 1;

                    }

                    break;

                case float_round_to_zero:

                    break;

                case float_round_up:

                    ret += 1 - sign;

                    break;

                case float_round_down:

                    ret += sign;

                    break;

                }

            }

        }

        if (sign) {

            ret = -ret;

        }

    }

    env->error_code = exc;



    return ret;

}