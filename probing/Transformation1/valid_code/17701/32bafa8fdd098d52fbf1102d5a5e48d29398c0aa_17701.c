InputEvent *qemu_input_event_new_move(InputEventKind kind,

                                      InputAxis axis, int value)

{

    InputEvent *evt = g_new0(InputEvent, 1);

    InputMoveEvent *move = g_new0(InputMoveEvent, 1);



    evt->type = kind;

    evt->u.rel = move; 

    move->axis = axis;

    move->value = value;

    return evt;

}