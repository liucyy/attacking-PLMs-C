static int adb_kbd_poll(ADBDevice *d, uint8_t *obuf)

{

    KBDState *s = ADB_KEYBOARD(d);

    int keycode;

    int olen;



    olen = 0;

    if (s->count == 0) {

        return 0;

    }

    keycode = s->data[s->rptr];

    s->rptr++;

    if (s->rptr == sizeof(s->data)) {

        s->rptr = 0;

    }

    s->count--;

    

    if (keycode == 0x7f) {

        obuf[0] = 0x7f;

        obuf[1] = 0x7f;

        olen = 2;

    } else {

        obuf[0] = keycode;

        

        obuf[1] = 0xff;

        olen = 2;

    }



    return olen;

}