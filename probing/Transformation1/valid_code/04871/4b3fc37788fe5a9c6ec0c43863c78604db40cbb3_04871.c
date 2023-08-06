static void gen_spr_970_lpar(CPUPPCState *env)

{

    

    

    spr_register(env, SPR_970_HID4, "HID4",

                 SPR_NOACCESS, SPR_NOACCESS,

                 &spr_read_generic, &spr_write_generic,

                 0x00000000);

}