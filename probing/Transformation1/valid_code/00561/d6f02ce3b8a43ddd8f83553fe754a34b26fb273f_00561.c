static void arm_cpu_realizefn(DeviceState *dev, Error **errp)

{

    CPUState *cs = CPU(dev);

    ARMCPU *cpu = ARM_CPU(dev);

    ARMCPUClass *acc = ARM_CPU_GET_CLASS(dev);

    CPUARMState *env = &cpu->env;

    int pagebits;

    Error *local_err = NULL;



    cpu_exec_realizefn(cs, &local_err);

    if (local_err != NULL) {

        error_propagate(errp, local_err);

        return;

    }



    

    if (arm_feature(env, ARM_FEATURE_V8)) {

        set_feature(env, ARM_FEATURE_V7);

        set_feature(env, ARM_FEATURE_ARM_DIV);

        set_feature(env, ARM_FEATURE_LPAE);

    }

    if (arm_feature(env, ARM_FEATURE_V7)) {

        set_feature(env, ARM_FEATURE_VAPA);

        set_feature(env, ARM_FEATURE_THUMB2);

        set_feature(env, ARM_FEATURE_MPIDR);

        if (!arm_feature(env, ARM_FEATURE_M)) {

            set_feature(env, ARM_FEATURE_V6K);

        } else {

            set_feature(env, ARM_FEATURE_V6);

        }



        

        set_feature(env, ARM_FEATURE_VBAR);

    }

    if (arm_feature(env, ARM_FEATURE_V6K)) {

        set_feature(env, ARM_FEATURE_V6);

        set_feature(env, ARM_FEATURE_MVFR);

    }

    if (arm_feature(env, ARM_FEATURE_V6)) {

        set_feature(env, ARM_FEATURE_V5);

        if (!arm_feature(env, ARM_FEATURE_M)) {

            set_feature(env, ARM_FEATURE_AUXCR);

        }

    }

    if (arm_feature(env, ARM_FEATURE_V5)) {

        set_feature(env, ARM_FEATURE_V4T);

    }

    if (arm_feature(env, ARM_FEATURE_M)) {

        set_feature(env, ARM_FEATURE_THUMB_DIV);

    }

    if (arm_feature(env, ARM_FEATURE_ARM_DIV)) {

        set_feature(env, ARM_FEATURE_THUMB_DIV);

    }

    if (arm_feature(env, ARM_FEATURE_VFP4)) {

        set_feature(env, ARM_FEATURE_VFP3);

        set_feature(env, ARM_FEATURE_VFP_FP16);

    }

    if (arm_feature(env, ARM_FEATURE_VFP3)) {

        set_feature(env, ARM_FEATURE_VFP);

    }

    if (arm_feature(env, ARM_FEATURE_LPAE)) {

        set_feature(env, ARM_FEATURE_V7MP);

        set_feature(env, ARM_FEATURE_PXN);

    }

    if (arm_feature(env, ARM_FEATURE_CBAR_RO)) {

        set_feature(env, ARM_FEATURE_CBAR);

    }

    if (arm_feature(env, ARM_FEATURE_THUMB2) &&

        !arm_feature(env, ARM_FEATURE_M)) {

        set_feature(env, ARM_FEATURE_THUMB_DSP);

    }



    if (arm_feature(env, ARM_FEATURE_V7) &&

        !arm_feature(env, ARM_FEATURE_M) &&

        !arm_feature(env, ARM_FEATURE_MPU)) {

        

        pagebits = 12;

    } else {

        

        pagebits = 10;

    }

    if (!set_preferred_target_page_bits(pagebits)) {

        

        error_setg(errp, "This CPU requires a smaller page size than the "

                   "system is using");

        return;

    }



    

    if (cpu->mp_affinity == ARM64_AFFINITY_INVALID) {

        uint32_t Aff1 = cs->cpu_index / ARM_DEFAULT_CPUS_PER_CLUSTER;

        uint32_t Aff0 = cs->cpu_index % ARM_DEFAULT_CPUS_PER_CLUSTER;

        cpu->mp_affinity = (Aff1 << ARM_AFF1_SHIFT) | Aff0;

    }



    if (cpu->reset_hivecs) {

            cpu->reset_sctlr |= (1 << 13);

    }



    if (cpu->cfgend) {

        if (arm_feature(&cpu->env, ARM_FEATURE_V7)) {

            cpu->reset_sctlr |= SCTLR_EE;

        } else {

            cpu->reset_sctlr |= SCTLR_B;

        }

    }



    if (!cpu->has_el3) {

        

        unset_feature(env, ARM_FEATURE_EL3);



        

        cpu->id_pfr1 &= ~0xf0;

        cpu->id_aa64pfr0 &= ~0xf000;

    }



    if (!cpu->has_el2) {

        unset_feature(env, ARM_FEATURE_EL2);

    }



    if (!cpu->has_pmu || !kvm_enabled()) {

        cpu->has_pmu = false;

        unset_feature(env, ARM_FEATURE_PMU);

    }



    if (!arm_feature(env, ARM_FEATURE_EL2)) {

        

        cpu->id_aa64pfr0 &= ~0xf00;

        cpu->id_pfr1 &= ~0xf000;

    }



    if (!cpu->has_mpu) {

        unset_feature(env, ARM_FEATURE_MPU);

    }



    if (arm_feature(env, ARM_FEATURE_MPU) &&

        arm_feature(env, ARM_FEATURE_V7)) {

        uint32_t nr = cpu->pmsav7_dregion;



        if (nr > 0xff) {

            error_setg(errp, "PMSAv7 MPU #regions invalid %" PRIu32, nr);

            return;

        }



        if (nr) {

            env->pmsav7.drbar = g_new0(uint32_t, nr);

            env->pmsav7.drsr = g_new0(uint32_t, nr);

            env->pmsav7.dracr = g_new0(uint32_t, nr);

        }

    }



    if (arm_feature(env, ARM_FEATURE_EL3)) {

        set_feature(env, ARM_FEATURE_VBAR);

    }



    register_cp_regs_for_features(cpu);

    arm_cpu_register_gdb_regs_for_features(cpu);



    init_cpreg_list(cpu);



#ifndef CONFIG_USER_ONLY

    if (cpu->has_el3) {

        cs->num_ases = 2;

    } else {

        cs->num_ases = 1;

    }



    if (cpu->has_el3) {

        AddressSpace *as;



        if (!cpu->secure_memory) {

            cpu->secure_memory = cs->memory;

        }

        as = address_space_init_shareable(cpu->secure_memory,

                                          "cpu-secure-memory");

        cpu_address_space_init(cs, as, ARMASIdx_S);

    }

    cpu_address_space_init(cs,

                           address_space_init_shareable(cs->memory,

                                                        "cpu-memory"),

                           ARMASIdx_NS);

#endif



    qemu_init_vcpu(cs);

    cpu_reset(cs);



    acc->parent_realize(dev, errp);

}