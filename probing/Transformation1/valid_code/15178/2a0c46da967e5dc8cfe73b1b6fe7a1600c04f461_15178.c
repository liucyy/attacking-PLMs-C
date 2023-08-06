on_host_init(VSCMsgHeader *mhHeader, VSCMsgInit *incoming)

{

    uint32_t *capabilities = (incoming->capabilities);

    int num_capabilities =

        1 + ((mhHeader->length - sizeof(VSCMsgInit)) / sizeof(uint32_t));

    int i;

    QemuThread thread_id;



    incoming->version = ntohl(incoming->version);

    if (incoming->version != VSCARD_VERSION) {

        if (verbose > 0) {

            printf("warning: host has version %d, we have %d\n",

                verbose, VSCARD_VERSION);

        }

    }

    if (incoming->magic != VSCARD_MAGIC) {

        printf("unexpected magic: got %d, expected %d\n",

            incoming->magic, VSCARD_MAGIC);

        return -1;

    }

    for (i = 0 ; i < num_capabilities; ++i) {

        capabilities[i] = ntohl(capabilities[i]);

    }

    

    

    send_msg(VSC_ReaderRemove, VSCARD_MINIMAL_READER_ID, NULL, 0);

    

    qemu_thread_create(&thread_id, "vsc/event", event_thread, NULL, 0);

    return 0;

}