void qemu_system_reset(void)

{

    QEMUResetEntry *re, *nre;



    

    QTAILQ_FOREACH_SAFE(re, &reset_handlers, entry, nre) {

        re->func(re->opaque);

    }


}