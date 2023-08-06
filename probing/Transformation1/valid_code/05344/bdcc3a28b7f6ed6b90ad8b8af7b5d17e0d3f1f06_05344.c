void qemu_input_event_send(QemuConsole *src, InputEvent *evt)

{

    QemuInputHandlerState *s;



    if (!runstate_is_running() && !runstate_check(RUN_STATE_SUSPENDED)) {





    qemu_input_event_trace(src, evt);



    

    if (graphic_rotate && (evt->kind == INPUT_EVENT_KIND_ABS)) {

            qemu_input_transform_abs_rotate(evt);




    

    s = qemu_input_find_handler(1 << evt->kind);




    s->handler->event(s->dev, src, evt);

    s->events++;