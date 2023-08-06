static int rtl8139_can_receive(void *opaque)

{

    RTL8139State *s = opaque;

    int avail;



    

    if (!s->clock_enabled)

      return 1;

    if (!rtl8139_receiver_enabled(s))

      return 1;



    if (rtl8139_cp_receiver_enabled(s)) {

        

        return 1;

    } else {

        avail = MOD2(s->RxBufferSize + s->RxBufPtr - s->RxBufAddr,

                     s->RxBufferSize);

        return (avail == 0 || avail >= 1514);

    }

}