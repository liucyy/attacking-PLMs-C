static void nvic_systick_trigger(void *opaque, int n, int level)

{

    NVICState *s = opaque;



    if (level) {

        

        armv7m_nvic_set_pending(s, ARMV7M_EXCP_SYSTICK);

    }

}