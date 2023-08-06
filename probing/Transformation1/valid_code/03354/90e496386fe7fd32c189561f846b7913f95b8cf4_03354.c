static inline int vec_reg_offset(int regno, int element, TCGMemOp size)

{

    int offs = offsetof(CPUARMState, vfp.regs[regno * 2]);

#ifdef HOST_WORDS_BIGENDIAN

    

    offs += (16 - ((element + 1) * (1 << size)));

    offs ^= 8;

#else

    offs += element * (1 << size);

#endif

    return offs;

}