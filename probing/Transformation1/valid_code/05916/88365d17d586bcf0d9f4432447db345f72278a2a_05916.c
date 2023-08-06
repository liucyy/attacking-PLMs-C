static int kvm_handle_debug(PowerPCCPU *cpu, struct kvm_run *run)

{

    CPUState *cs = CPU(cpu);

    CPUPPCState *env = &cpu->env;

    struct kvm_debug_exit_arch *arch_info = &run->debug.arch;

    int handle = 0;



    if (kvm_find_sw_breakpoint(cs, arch_info->address)) {

        handle = 1;

    } else {

        



        cpu_synchronize_state(cs);

        

        env->nip += 4;

        cs->exception_index = POWERPC_EXCP_PROGRAM;

        env->error_code = POWERPC_EXCP_INVAL;

        ppc_cpu_do_interrupt(cs);

    }



    return handle;

}