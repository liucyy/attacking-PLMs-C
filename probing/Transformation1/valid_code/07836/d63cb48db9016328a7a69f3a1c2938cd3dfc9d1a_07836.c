CPUState *ppc4xx_init (const char *cpu_model,

                       clk_setup_t *cpu_clk, clk_setup_t *tb_clk,

                       uint32_t sysclk)

{

    CPUState *env;



    

    env = cpu_init(cpu_model);

    if (!env) {

        fprintf(stderr, "Unable to find PowerPC %s CPU definition\n",

                cpu_model);

        exit(1);

    }

    cpu_clk->cb = NULL; 

    cpu_clk->opaque = env;

    

    tb_clk->cb = ppc_emb_timers_init(env, sysclk);

    tb_clk->opaque = env;

    ppc_dcr_init(env, NULL, NULL);

    

    qemu_register_reset((QEMUResetHandler*)&cpu_reset, env);



    return env;

}