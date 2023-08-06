void arm_handle_psci_call(ARMCPU *cpu)

{

    

    CPUARMState *env = &cpu->env;

    uint64_t param[4];

    uint64_t context_id, mpidr;

    target_ulong entry;

    int32_t ret = 0;

    int i;



    for (i = 0; i < 4; i++) {

        

        param[i] = is_a64(env) ? env->xregs[i] : env->regs[i];

    }



    if ((param[0] & QEMU_PSCI_0_2_64BIT) && !is_a64(env)) {

        ret = QEMU_PSCI_RET_INVALID_PARAMS;

        goto err;

    }



    switch (param[0]) {

        CPUState *target_cpu_state;

        ARMCPU *target_cpu;



    case QEMU_PSCI_0_2_FN_PSCI_VERSION:

        ret = QEMU_PSCI_0_2_RET_VERSION_0_2;

        break;

    case QEMU_PSCI_0_2_FN_MIGRATE_INFO_TYPE:

        ret = QEMU_PSCI_0_2_RET_TOS_MIGRATION_NOT_REQUIRED; 

        break;

    case QEMU_PSCI_0_2_FN_AFFINITY_INFO:

    case QEMU_PSCI_0_2_FN64_AFFINITY_INFO:

        mpidr = param[1];



        switch (param[2]) {

        case 0:

            target_cpu_state = arm_get_cpu_by_id(mpidr);

            if (!target_cpu_state) {

                ret = QEMU_PSCI_RET_INVALID_PARAMS;

                break;

            }

            target_cpu = ARM_CPU(target_cpu_state);

            ret = target_cpu->powered_off ? 1 : 0;

            break;

        default:

            

            ret = 0;

        }

        break;

    case QEMU_PSCI_0_2_FN_SYSTEM_RESET:

        qemu_system_reset_request();

        

        goto cpu_off;

    case QEMU_PSCI_0_2_FN_SYSTEM_OFF:

        qemu_system_shutdown_request();

        goto cpu_off;

    case QEMU_PSCI_0_1_FN_CPU_ON:

    case QEMU_PSCI_0_2_FN_CPU_ON:

    case QEMU_PSCI_0_2_FN64_CPU_ON:

    {

        

        int target_el = arm_feature(env, ARM_FEATURE_EL2) ? 2 : 1;

        bool target_aarch64 = arm_el_is_aa64(env, target_el);



        mpidr = param[1];

        entry = param[2];

        context_id = param[3];

        ret = arm_set_cpu_on(mpidr, entry, context_id,

                             target_el, target_aarch64);

        break;

    }

    case QEMU_PSCI_0_1_FN_CPU_OFF:

    case QEMU_PSCI_0_2_FN_CPU_OFF:

        goto cpu_off;

    case QEMU_PSCI_0_1_FN_CPU_SUSPEND:

    case QEMU_PSCI_0_2_FN_CPU_SUSPEND:

    case QEMU_PSCI_0_2_FN64_CPU_SUSPEND:

        

        if (param[1] & 0xfffe0000) {

            ret = QEMU_PSCI_RET_INVALID_PARAMS;

            break;

        }

        

        if (is_a64(env)) {

            env->xregs[0] = 0;

        } else {

            env->regs[0] = 0;

        }

        helper_wfi(env);

        break;

    case QEMU_PSCI_0_1_FN_MIGRATE:

    case QEMU_PSCI_0_2_FN_MIGRATE:

        ret = QEMU_PSCI_RET_NOT_SUPPORTED;

        break;

    default:

        g_assert_not_reached();

    }



err:

    if (is_a64(env)) {

        env->xregs[0] = ret;

    } else {

        env->regs[0] = ret;

    }

    return;



cpu_off:

    ret = arm_set_cpu_off(cpu->mp_affinity);

    

    

    assert(ret == QEMU_ARM_POWERCTL_RET_SUCCESS);

}