void qemu_cpu_kick(CPUState *cpu)

{

    qemu_cond_broadcast(cpu->halt_cond);

    if (tcg_enabled()) {

        cpu_exit(cpu);

        

        qemu_cpu_kick_rr_cpu();

    } else {

        if (hax_enabled()) {

            

            cpu->exit_request = 1;

        }

        qemu_cpu_kick_thread(cpu);

    }

}