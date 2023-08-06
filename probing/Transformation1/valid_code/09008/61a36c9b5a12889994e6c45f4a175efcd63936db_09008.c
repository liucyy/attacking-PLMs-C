static target_ulong compute_tlbie_rb(target_ulong v, target_ulong r,

                                     target_ulong pte_index)

{

    target_ulong rb, va_low;



    rb = (v & ~0x7fULL) << 16; 

    va_low = pte_index >> 3;

    if (v & HPTE64_V_SECONDARY) {

        va_low = ~va_low;

    }

    

    if (!(v & HPTE64_V_1TB_SEG)) {

        va_low ^= v >> 12;

    } else {

        va_low ^= v >> 24;

    }

    va_low &= 0x7ff;

    if (v & HPTE64_V_LARGE) {

        rb |= 1;                         

#if 0 

        if (r & 0xff000) {

            

            

            rb |= 0x1000;                

            rb |= (va_low & 0x7f) << 16; 

            rb |= (va_low & 0xfe);       

        }

#endif

    } else {

        

        rb |= (va_low & 0x7ff) << 12;   

    }

    rb |= (v >> 54) & 0x300;            

    return rb;

}