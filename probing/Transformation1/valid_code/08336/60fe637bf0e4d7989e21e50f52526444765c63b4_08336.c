static int qemu_rdma_get_buffer(void *opaque, uint8_t *buf,

                                int64_t pos, int size)

{

    QEMUFileRDMA *r = opaque;

    RDMAContext *rdma = r->rdma;

    RDMAControlHeader head;

    int ret = 0;



    CHECK_ERROR_STATE();



    

    r->len = qemu_rdma_fill(r->rdma, buf, size, 0);

    if (r->len) {

        return r->len;

    }



    

    ret = qemu_rdma_exchange_recv(rdma, &head, RDMA_CONTROL_QEMU_FILE);



    if (ret < 0) {

        rdma->error_state = ret;

        return ret;

    }



    

    return qemu_rdma_fill(r->rdma, buf, size, 0);

}