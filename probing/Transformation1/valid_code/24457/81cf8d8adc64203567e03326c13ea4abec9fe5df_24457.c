static inline void check_io(CPUX86State *env, int addr, int size)

{

    int io_offset, val, mask;



    

    if (!(env->tr.flags & DESC_P_MASK) ||

        ((env->tr.flags >> DESC_TYPE_SHIFT) & 0xf) != 9 ||

        env->tr.limit < 103) {

        goto fail;

    }

    io_offset = cpu_lduw_kernel(env, env->tr.base + 0x66);

    io_offset += (addr >> 3);

    

    if ((io_offset + 1) > env->tr.limit) {

        goto fail;

    }

    val = cpu_lduw_kernel(env, env->tr.base + io_offset);

    val >>= (addr & 7);

    mask = (1 << size) - 1;

    

    if ((val & mask) != 0) {

    fail:

        raise_exception_err(env, EXCP0D_GPF, 0);

    }

}