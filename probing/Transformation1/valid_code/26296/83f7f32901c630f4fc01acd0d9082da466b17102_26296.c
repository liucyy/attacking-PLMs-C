static bool is_special_wait_psw(CPUState *cs)

{

    

    return cs->kvm_run->psw_addr == 0xfffUL;

}