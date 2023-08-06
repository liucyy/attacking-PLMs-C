static int nvic_pending_prio(NVICState *s)

{

    

    return s->vectpending ? s->vectors[s->vectpending].prio : NVIC_NOEXC_PRIO;

}