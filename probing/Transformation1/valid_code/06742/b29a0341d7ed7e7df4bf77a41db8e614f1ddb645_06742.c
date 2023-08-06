void op_dmtc0_ebase (void)

{

    

    

    

    env->CP0_EBase = (int32_t)0x80000000 | (T0 & 0x3FFFF000);

    RETURN();

}