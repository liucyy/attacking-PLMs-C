static void check_watchpoint(int offset, int len, int flags)

{

    CPUState *cpu = current_cpu;

    CPUArchState *env = cpu->env_ptr;

    target_ulong pc, cs_base;

    target_ulong vaddr;

    CPUWatchpoint *wp;

    int cpu_flags;



    if (cpu->watchpoint_hit) {

        

        cpu_interrupt(cpu, CPU_INTERRUPT_DEBUG);

        return;

    }

    vaddr = (cpu->mem_io_vaddr & TARGET_PAGE_MASK) + offset;

    QTAILQ_FOREACH(wp, &cpu->watchpoints, entry) {

        if (cpu_watchpoint_address_matches(wp, vaddr, len)

            && (wp->flags & flags)) {

            wp->flags |= BP_WATCHPOINT_HIT;

            if (!cpu->watchpoint_hit) {

                cpu->watchpoint_hit = wp;

                tb_check_watchpoint(cpu);

                if (wp->flags & BP_STOP_BEFORE_ACCESS) {

                    cpu->exception_index = EXCP_DEBUG;

                    cpu_loop_exit(cpu);

                } else {

                    cpu_get_tb_cpu_state(env, &pc, &cs_base, &cpu_flags);

                    tb_gen_code(cpu, pc, cs_base, cpu_flags, 1);

                    cpu_resume_from_signal(cpu, NULL);

                }

            }

        } else {

            wp->flags &= ~BP_WATCHPOINT_HIT;

        }

    }

}