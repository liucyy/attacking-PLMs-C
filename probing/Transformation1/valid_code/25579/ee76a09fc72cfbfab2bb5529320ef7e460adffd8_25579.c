static void spapr_populate_pa_features(PowerPCCPU *cpu, void *fdt, int offset,

                                       bool legacy_guest)

{

    CPUPPCState *env = &cpu->env;

    uint8_t pa_features_206[] = { 6, 0,

        0xf6, 0x1f, 0xc7, 0x00, 0x80, 0xc0 };

    uint8_t pa_features_207[] = { 24, 0,

        0xf6, 0x1f, 0xc7, 0xc0, 0x80, 0xf0,

        0x80, 0x00, 0x00, 0x00, 0x00, 0x00,

        0x00, 0x00, 0x00, 0x00, 0x80, 0x00,

        0x80, 0x00, 0x80, 0x00, 0x00, 0x00 };

    uint8_t pa_features_300[] = { 66, 0,

        

        

        0xf6, 0x1f, 0xc7, 0xc0, 0x80, 0xf0, 

        

        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 

        

        0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 

        

        0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 

        

        0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 

        

        0x80, 0x00, 0x80, 0x00, 0xC0, 0x00, 

        

        0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 

        

        0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 

        

        0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 

        

        0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 

        

        0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 

    };

    uint8_t *pa_features = NULL;

    size_t pa_size;



    if (ppc_check_compat(cpu, CPU_POWERPC_LOGICAL_2_06, 0, cpu->compat_pvr)) {

        pa_features = pa_features_206;

        pa_size = sizeof(pa_features_206);

    }

    if (ppc_check_compat(cpu, CPU_POWERPC_LOGICAL_2_07, 0, cpu->compat_pvr)) {

        pa_features = pa_features_207;

        pa_size = sizeof(pa_features_207);

    }

    if (ppc_check_compat(cpu, CPU_POWERPC_LOGICAL_3_00, 0, cpu->compat_pvr)) {

        pa_features = pa_features_300;

        pa_size = sizeof(pa_features_300);

    }

    if (!pa_features) {

        return;

    }



    if (env->ci_large_pages) {

        

        pa_features[3] |= 0x20;

    }

    if (kvmppc_has_cap_htm() && pa_size > 24) {

        pa_features[24] |= 0x80;    

    }

    if (legacy_guest && pa_size > 40) {

        

        pa_features[40 + 2] &= ~0x80; 

    }



    _FDT((fdt_setprop(fdt, offset, "ibm,pa-features", pa_features, pa_size)));

}