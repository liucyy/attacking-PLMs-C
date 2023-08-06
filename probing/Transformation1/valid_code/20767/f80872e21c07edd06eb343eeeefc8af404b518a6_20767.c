static void init_proc_POWER7 (CPUPPCState *env)

{

    gen_spr_ne_601(env);

    gen_spr_7xx(env);

    

    gen_tbl(env);

    

    spr_register(env, SPR_PIR, "PIR",

                 SPR_NOACCESS, SPR_NOACCESS,

                 &spr_read_generic, &spr_write_pir,

                 0x00000000);

#if !defined(CONFIG_USER_ONLY)

    

    spr_register_kvm(env, SPR_PURR,   "PURR",

                     &spr_read_purr, SPR_NOACCESS,

                     &spr_read_purr, SPR_NOACCESS,

                     KVM_REG_PPC_PURR, 0x00000000);

    spr_register_kvm(env, SPR_SPURR,   "SPURR",

                     &spr_read_purr, SPR_NOACCESS,

                     &spr_read_purr, SPR_NOACCESS,

                     KVM_REG_PPC_SPURR, 0x00000000);

    spr_register(env, SPR_CFAR, "SPR_CFAR",

                 SPR_NOACCESS, SPR_NOACCESS,

                 &spr_read_cfar, &spr_write_cfar,

                 0x00000000);

    spr_register_kvm(env, SPR_DSCR, "SPR_DSCR",

                     SPR_NOACCESS, SPR_NOACCESS,

                     &spr_read_generic, &spr_write_generic,

                     KVM_REG_PPC_DSCR, 0x00000000);

#endif 

    

    

    spr_register(env, SPR_MMUCFG, "MMUCFG",

                 SPR_NOACCESS, SPR_NOACCESS,

                 &spr_read_generic, SPR_NOACCESS,

                 0x00000000); 


    

    spr_register(env, SPR_CTRL, "SPR_CTRLT",

                 SPR_NOACCESS, SPR_NOACCESS,

                 &spr_read_generic, &spr_write_generic,

                 0x80800000);

    spr_register(env, SPR_UCTRL, "SPR_CTRLF",

                 SPR_NOACCESS, SPR_NOACCESS,

                 &spr_read_generic, &spr_write_generic,

                 0x80800000);

    spr_register(env, SPR_VRSAVE, "SPR_VRSAVE",

                 &spr_read_generic, &spr_write_generic,

                 &spr_read_generic, &spr_write_generic,

                 0x00000000);

#if !defined(CONFIG_USER_ONLY)

    env->slb_nr = 32;

#endif

    init_excp_POWER7(env);

    env->dcache_line_size = 128;

    env->icache_line_size = 128;

    

    ppcPOWER7_irq_init(env);

    

    vscr_init(env, 0x00010000);

}