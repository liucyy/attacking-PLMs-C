static void cpu_ppc_set_tb_clk (void *opaque, uint32_t freq)

{

    CPUState *env = opaque;

    ppc_tb_t *tb_env = env->tb_env;



    tb_env->tb_freq = freq;

    tb_env->decr_freq = freq;

    

    _cpu_ppc_store_decr(env, 0xFFFFFFFF, 0xFFFFFFFF, 0);

#if defined(TARGET_PPC64H)

    _cpu_ppc_store_hdecr(env, 0xFFFFFFFF, 0xFFFFFFFF, 0);

    cpu_ppc_store_purr(env, 0x0000000000000000ULL);

#endif 

}