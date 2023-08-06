static void init_proc_970MP (CPUPPCState *env)

{

    gen_spr_ne_601(env);

    gen_spr_7xx(env);

    

    gen_tbl(env);

    

    

    spr_register(env, SPR_HID0, "HID0",

                 SPR_NOACCESS, SPR_NOACCESS,

                 &spr_read_generic, &spr_write_clear,

                 0x60000000);

    

    spr_register(env, SPR_HID1, "HID1",

                 SPR_NOACCESS, SPR_NOACCESS,

                 &spr_read_generic, &spr_write_generic,

                 0x00000000);

    

    spr_register(env, SPR_970_HID5, "HID5",

                 SPR_NOACCESS, SPR_NOACCESS,

                 &spr_read_generic, &spr_write_generic,

                 POWERPC970_HID5_INIT);

    

    

    

    gen_low_BATs(env);

    spr_register(env, SPR_HIOR, "SPR_HIOR",

                 SPR_NOACCESS, SPR_NOACCESS,

                 &spr_read_hior, &spr_write_hior,

                 0x00000000);

    

    spr_register_kvm(env, SPR_LPCR, "LPCR",

                     SPR_NOACCESS, SPR_NOACCESS,

                     &spr_read_generic, &spr_write_generic,

                     KVM_REG_PPC_LPCR, 0x00000000);

#if !defined(CONFIG_USER_ONLY)

    env->slb_nr = 32;

#endif

    init_excp_970(env);

    env->dcache_line_size = 128;

    env->icache_line_size = 128;

    

    ppc970_irq_init(env);

    

    vscr_init(env, 0x00010000);

}