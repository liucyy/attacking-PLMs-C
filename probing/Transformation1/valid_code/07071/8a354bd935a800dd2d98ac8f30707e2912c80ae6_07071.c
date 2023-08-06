static void replay_save_event(Event *event, int checkpoint)

{

    if (replay_mode != REPLAY_MODE_PLAY) {

        

        replay_put_event(EVENT_ASYNC);

        replay_put_byte(checkpoint);

        replay_put_byte(event->event_kind);



        

        switch (event->event_kind) {

        default:

            error_report("Unknown ID %d of replay event", read_event_kind);

            exit(1);

            break;

        }

    }

}