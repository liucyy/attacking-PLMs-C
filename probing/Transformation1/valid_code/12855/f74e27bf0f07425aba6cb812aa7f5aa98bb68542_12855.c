static void coroutine_fn pdu_complete(V9fsPDU *pdu, ssize_t len)

{

    int8_t id = pdu->id + 1; 

    V9fsState *s = pdu->s;



    if (len < 0) {

        int err = -len;

        len = 7;



        if (s->proto_version != V9FS_PROTO_2000L) {

            V9fsString str;



            str.data = strerror(err);

            str.size = strlen(str.data);



            len += pdu_marshal(pdu, len, "s", &str);

            id = P9_RERROR;

        }



        len += pdu_marshal(pdu, len, "d", err);



        if (s->proto_version == V9FS_PROTO_2000L) {

            id = P9_RLERROR;

        }

        trace_v9fs_rerror(pdu->tag, pdu->id, err); 

    }



    

    pdu_marshal(pdu, 0, "dbw", (int32_t)len, id, pdu->tag);



    

    pdu->size = len;

    pdu->id = id;



    pdu_push_and_notify(pdu);



    

    qemu_co_queue_next(&pdu->complete);



    pdu_free(pdu);

}