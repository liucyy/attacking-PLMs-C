void helper_ldf_asi(target_ulong addr, int asi, int size, int rd)
{
    unsigned int i;
    target_ulong val;
    helper_check_align(addr, 3);
    addr = asi_address_mask(env, asi, addr);
    switch (asi) {
    case 0xf0: 
    case 0xf1: 
    case 0xf8: 
    case 0xf9: 
            *(uint32_t *)&env->fpr[rd++] = helper_ld_asi(addr, asi & 0x8f, 4,
    default:
        break;
    val = helper_ld_asi(addr, asi, size, 0);
    switch(size) {
    default:
    case 4:
        *((uint32_t *)&env->fpr[rd]) = val;
        break;
    case 8:
        *((int64_t *)&DT0) = val;
        break;
    case 16:
        
        break;