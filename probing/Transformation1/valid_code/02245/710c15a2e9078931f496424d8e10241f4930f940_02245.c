void OPPROTO op_lmsw_T0(void)

{

    

    T0 = (env->cr[0] & ~0xf) | (T0 & 0xf);

    helper_movl_crN_T0(0);

}