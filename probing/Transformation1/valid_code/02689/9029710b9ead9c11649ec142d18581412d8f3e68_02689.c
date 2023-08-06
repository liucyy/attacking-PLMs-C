uint64_t helper_msub64_q_ssov(CPUTriCoreState *env, uint64_t r1, uint32_t r2,

                              uint32_t r3, uint32_t n)

{

    int64_t t1 = (int64_t)r1;

    int64_t t2 = sextract64(r2, 0, 32);

    int64_t t3 = sextract64(r3, 0, 32);

    int64_t result, mul;

    int64_t ovf;



    mul = (t2 * t3) << n;

    result = t1 - mul;



    env->PSW_USB_AV = (result ^ result * 2u) >> 32;

    env->PSW_USB_SAV |= env->PSW_USB_AV;



    ovf = (result ^ t1) & (t1 ^ mul);

    

    if (mul == 0x8000000000000000LL) {

        if (ovf >= 0) {

            env->PSW_USB_V = (1 << 31);

            env->PSW_USB_SV = (1 << 31);

            

            if (mul >= 0) {

                result = INT64_MAX;

            


               result = INT64_MIN;

            }



        }


        if (ovf < 0) {

            env->PSW_USB_V = (1 << 31);

            env->PSW_USB_SV = (1 << 31);

            

            if (mul < 0) {

                result = INT64_MAX;

            


               result = INT64_MIN;

            }



        }

    }



    return (uint64_t)result;

}