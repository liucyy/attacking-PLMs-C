int arm_set_cpu_on(uint64_t cpuid, uint64_t entry, uint64_t context_id,

                   uint32_t target_el, bool target_aa64)

{

    CPUState *target_cpu_state;

    ARMCPU *target_cpu;



    DPRINTF("cpu %" PRId64 " (EL %d, %s) @ 0x%" PRIx64 " with R0 = 0x%" PRIx64

            "\n", cpuid, target_el, target_aa64 ? "aarch64" : "aarch32", entry,

            context_id);



    

    assert((target_el > 0) && (target_el < 4));



    if (target_aa64 && (entry & 3)) {

        

        return QEMU_ARM_POWERCTL_INVALID_PARAM;

    }



    

    target_cpu_state = arm_get_cpu_by_id(cpuid);

    if (!target_cpu_state) {

        

        return QEMU_ARM_POWERCTL_INVALID_PARAM;

    }



    target_cpu = ARM_CPU(target_cpu_state);

    if (!target_cpu->powered_off) {

        qemu_log_mask(LOG_GUEST_ERROR,

                      "[ARM]%s: CPU %" PRId64 " is already on\n",

                      __func__, cpuid);

        return QEMU_ARM_POWERCTL_ALREADY_ON;

    }



    



    if (((target_el == 3) && !arm_feature(&target_cpu->env, ARM_FEATURE_EL3)) ||

        ((target_el == 2) && !arm_feature(&target_cpu->env, ARM_FEATURE_EL2))) {

        

        return QEMU_ARM_POWERCTL_INVALID_PARAM;

    }



    if (!target_aa64 && arm_feature(&target_cpu->env, ARM_FEATURE_AARCH64)) {

        

        qemu_log_mask(LOG_UNIMP,

                      "[ARM]%s: Starting AArch64 CPU %" PRId64

                      " in AArch32 mode is not supported yet\n",

                      __func__, cpuid);

        return QEMU_ARM_POWERCTL_INVALID_PARAM;

    }



    

    cpu_reset(target_cpu_state);

    target_cpu->powered_off = false;

    target_cpu_state->halted = 0;



    if (target_aa64) {

        if ((target_el < 3) && arm_feature(&target_cpu->env, ARM_FEATURE_EL3)) {

            

            target_cpu->env.cp15.scr_el3 |= SCR_RW;

        }



        if ((target_el < 2) && arm_feature(&target_cpu->env, ARM_FEATURE_EL2)) {

            

            target_cpu->env.cp15.hcr_el2 |= HCR_RW;

        }



        target_cpu->env.pstate = aarch64_pstate_mode(target_el, true);

    } else {

        

        static uint32_t mode_for_el[] = { 0,

                                          ARM_CPU_MODE_SVC,

                                          ARM_CPU_MODE_HYP,

                                          ARM_CPU_MODE_SVC };



        cpsr_write(&target_cpu->env, mode_for_el[target_el], CPSR_M,

                   CPSRWriteRaw);

    }



    if (target_el == 3) {

        

        target_cpu->env.cp15.scr_el3 &= ~SCR_NS;

    } else {

        

        target_cpu->env.cp15.scr_el3 |= SCR_NS;

    }



    

    assert(target_el == arm_current_el(&target_cpu->env));



    if (target_aa64) {

        target_cpu->env.xregs[0] = context_id;

        target_cpu->env.thumb = false;

    } else {

        target_cpu->env.regs[0] = context_id;

        target_cpu->env.thumb = entry & 1;

        entry &= 0xfffffffe;

    }



    

    cpu_set_pc(target_cpu_state, entry);



    qemu_cpu_kick(target_cpu_state);



    

    return QEMU_ARM_POWERCTL_RET_SUCCESS;

}