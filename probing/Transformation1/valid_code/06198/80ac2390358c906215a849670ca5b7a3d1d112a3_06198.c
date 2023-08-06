void armv7m_nvic_set_pending(void *opaque, int irq, bool secure)

{

    NVICState *s = (NVICState *)opaque;

    bool banked = exc_is_banked(irq);

    VecInfo *vec;



    assert(irq > ARMV7M_EXCP_RESET && irq < s->num_irq);

    assert(!secure || banked);



    vec = (banked && secure) ? &s->sec_vectors[irq] : &s->vectors[irq];



    trace_nvic_set_pending(irq, secure, vec->enabled, vec->prio);



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

            if (arm_feature(&s->cpu->env, ARM_FEATURE_M_SECURITY) &&

                (secure ||

                 !(s->cpu->env.v7m.aircr & R_V7M_AIRCR_BFHFNMINS_MASK))) {

                vec = &s->sec_vectors[irq];

            } else {

                vec = &s->vectors[irq];

            }

            

            s->cpu->env.v7m.hfsr |= R_V7M_HFSR_FORCED_MASK;

        }

    }



    if (!vec->pending) {

        vec->pending = 1;

        nvic_irq_update(s);

    }

}