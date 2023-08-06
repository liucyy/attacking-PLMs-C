static void set_irq_level(void *opaque, int n, int level)

{

    NVICState *s = opaque;

    VecInfo *vec;



    n += NVIC_FIRST_IRQ;



    assert(n >= NVIC_FIRST_IRQ && n < s->num_irq);



    trace_nvic_set_irq_level(n, level);



    

    vec = &s->vectors[n];

    if (level != vec->level) {

        vec->level = level;

        if (level) {

            armv7m_nvic_set_pending(s, n);

        }

    }

}