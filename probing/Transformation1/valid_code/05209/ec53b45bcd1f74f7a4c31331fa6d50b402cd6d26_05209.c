void cpu_single_step(CPUState *cpu, int enabled)

{

#if defined(TARGET_HAS_ICE)

    if (cpu->singlestep_enabled != enabled) {

        cpu->singlestep_enabled = enabled;

        if (kvm_enabled()) {

            kvm_update_guest_debug(cpu, 0);

        } else {

            

            

            CPUArchState *env = cpu->env_ptr;

            tb_flush(env);

        }

    }

#endif

}