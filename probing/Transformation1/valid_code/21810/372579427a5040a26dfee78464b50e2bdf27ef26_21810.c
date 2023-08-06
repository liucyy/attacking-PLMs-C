static void *qemu_tcg_cpu_thread_fn(void *arg)

{

    CPUState *cpu = arg;



    rcu_register_thread();



    qemu_mutex_lock_iothread();

    qemu_thread_get_self(cpu->thread);



    CPU_FOREACH(cpu) {

        cpu->thread_id = qemu_get_thread_id();

        cpu->created = true;

        cpu->can_do_io = 1;

    }

    qemu_cond_signal(&qemu_cpu_cond);



    

    while (first_cpu->stopped) {

        qemu_cond_wait(first_cpu->halt_cond, &qemu_global_mutex);



        

        CPU_FOREACH(cpu) {

            qemu_wait_io_event_common(cpu);

        }

    }



    start_tcg_kick_timer();



    cpu = first_cpu;



    

    cpu->exit_request = 1;



    while (1) {

        

        qemu_account_warp_timer();



        if (!cpu) {

            cpu = first_cpu;

        }



        while (cpu && !cpu->queued_work_first && !cpu->exit_request) {



            atomic_mb_set(&tcg_current_rr_cpu, cpu);



            qemu_clock_enable(QEMU_CLOCK_VIRTUAL,

                              (cpu->singlestep_enabled & SSTEP_NOTIMER) == 0);



            if (cpu_can_run(cpu)) {

                int r;

                r = tcg_cpu_exec(cpu);

                if (r == EXCP_DEBUG) {

                    cpu_handle_guest_debug(cpu);

                    break;

                }

            } else if (cpu->stop || cpu->stopped) {

                if (cpu->unplug) {

                    cpu = CPU_NEXT(cpu);

                }

                break;

            }



            cpu = CPU_NEXT(cpu);

        } 



        

        atomic_set(&tcg_current_rr_cpu, NULL);



        if (cpu && cpu->exit_request) {

            atomic_mb_set(&cpu->exit_request, 0);

        }



        handle_icount_deadline();



        qemu_tcg_wait_io_event(QTAILQ_FIRST(&cpus));

        deal_with_unplugged_cpus();

    }



    return NULL;

}