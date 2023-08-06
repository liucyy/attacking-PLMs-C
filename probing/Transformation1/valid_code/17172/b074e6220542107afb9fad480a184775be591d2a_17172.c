static void receive_from_chr_layer(SCLPConsole *scon, const uint8_t *buf,

                                   int size)

{

    

    assert(size <= SIZE_BUFFER_VT220 - scon->iov_data_len);



    

    memcpy(&scon->iov[scon->iov_bs], buf, size);

    scon->iov_data_len += size;

    scon->iov_sclp_rest += size;

    scon->iov_bs += size;

    scon->event.event_pending = true;

}