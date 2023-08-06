static void adb_keyboard_event(DeviceState *dev, QemuConsole *src,

                               InputEvent *evt)

{

    KBDState *s = (KBDState *)dev;

    int qcode, keycode;



    qcode = qemu_input_key_value_to_qcode(evt->u.key.data->key);

    if (qcode >= ARRAY_SIZE(qcode_to_adb_keycode)) {

        return;

    }

    keycode = qcode_to_adb_keycode[qcode];



    if (evt->u.key.data->down == false) { 

        keycode = keycode | 0x80;   

    }



    adb_kbd_put_keycode(s, keycode);

}