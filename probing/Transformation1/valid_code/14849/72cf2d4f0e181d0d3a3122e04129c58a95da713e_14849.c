void qemu_system_reset(void)

{

    QEMUResetEntry *re, *nre;



    

    TAILQ_FOREACH_SAFE(re, &reset_handlers, entry, nre) {

        re->func(re->opaque);

    }

}