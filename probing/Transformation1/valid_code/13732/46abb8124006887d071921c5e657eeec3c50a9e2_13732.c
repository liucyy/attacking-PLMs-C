int qemu_add_balloon_handler(QEMUBalloonEvent *event_func,

                             QEMUBalloonStatus *stat_func, void *opaque)

{

    if (balloon_event_fn || balloon_stat_fn || balloon_opaque) {

        

        error_report("Another balloon device already registered");

        return -1;

    }

    balloon_event_fn = event_func;

    balloon_stat_fn = stat_func;

    balloon_opaque = opaque;

    return 0;

}