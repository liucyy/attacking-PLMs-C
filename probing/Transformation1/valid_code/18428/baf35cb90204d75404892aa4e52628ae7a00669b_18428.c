static void aio_signal_handler(int signum)

{

#if !defined(QEMU_IMG) && !defined(QEMU_NBD)

    CPUState *env = cpu_single_env;

    if (env) {

        

        cpu_interrupt(env, CPU_INTERRUPT_EXIT);

#ifdef USE_KQEMU

        if (env->kqemu_enabled) {

            kqemu_cpu_interrupt(env);

        }

#endif

    }

#endif

}