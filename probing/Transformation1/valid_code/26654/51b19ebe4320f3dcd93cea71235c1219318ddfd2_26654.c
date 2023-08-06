ssize_t virtio_pdu_vunmarshal(V9fsPDU *pdu, size_t offset,

                              const char *fmt, va_list ap)

{

    V9fsState *s = pdu->s;

    V9fsVirtioState *v = container_of(s, V9fsVirtioState, state);

    VirtQueueElement *elem = &v->elems[pdu->idx];



    return v9fs_iov_vunmarshal(elem->out_sg, elem->out_num, offset, 1, fmt, ap);

}