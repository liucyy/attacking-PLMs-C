void helper_stf_asi(target_ulong addr, int asi, int size, int rd)
{
    unsigned int i;
    target_ulong val = 0;
    helper_check_align(addr, 3);
    addr = asi_address_mask(env, asi, addr);
    switch (asi) {
    case 0xe0: 
    case 0xe1: 
    case 0xf0: 
    case 0xf1: 
    case 0xf8: 
    case 0xf9: 
            helper_st_asi(addr, val, asi & 0x8f, 4);
    default:
        break;
    switch(size) {
    default:
    case 4:
        val = *((uint32_t *)&env->fpr[rd]);
        break;
    case 8:
        val = *((int64_t *)&DT0);
        break;
    case 16:
        
        break;
    helper_st_asi(addr, val, asi, size);