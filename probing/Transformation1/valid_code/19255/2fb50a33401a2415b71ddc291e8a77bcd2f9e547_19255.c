void armv7m_nvic_set_pending(void *opaque, int irq)

{

    NVICState *s = (NVICState *)opaque;

    VecInfo *vec;



    assert(irq > ARMV7M_EXCP_RESET && irq < s->num_irq);



    vec = &s->vectors[irq];

    trace_nvic_set_pending(irq, vec->enabled, vec->prio);





    if (irq >= ARMV7M_EXCP_HARD && irq < ARMV7M_EXCP_PENDSV) {

        

        int running = nvic_exec_prio(s);

        bool escalate = false;



        if (vec->prio >= running) {

            trace_nvic_escalate_prio(irq, vec->prio, running);

            escalate = true;

        } else if (!vec->enabled) {

            trace_nvic_escalate_disabled(irq);

            escalate = true;

        }



        if (escalate) {

            if (running < 0) {

                

                cpu_abort(&s->cpu->parent_obj,

                          "Lockup: can't escalate %d to HardFault "

                          "(current priority %d)\n", irq, running);

            }



            

            irq = ARMV7M_EXCP_HARD;

            vec = &s->vectors[irq];

            s->cpu->env.v7m.hfsr |= R_V7M_HFSR_FORCED_MASK;

        }

    }



    if (!vec->pending) {

        vec->pending = 1;

        nvic_irq_update(s);

    }

}