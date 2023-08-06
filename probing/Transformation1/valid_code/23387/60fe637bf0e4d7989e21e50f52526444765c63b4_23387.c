static int qemu_rdma_exchange_send(RDMAContext *rdma, RDMAControlHeader *head,

                                   uint8_t *data, RDMAControlHeader *resp,

                                   int *resp_idx,

                                   int (*callback)(RDMAContext *rdma))

{

    int ret = 0;



    

    if (rdma->control_ready_expected) {

        RDMAControlHeader resp;

        ret = qemu_rdma_exchange_get_response(rdma,

                                    &resp, RDMA_CONTROL_READY, RDMA_WRID_READY);

        if (ret < 0) {

            return ret;

        }

    }



    

    if (resp) {

        ret = qemu_rdma_post_recv_control(rdma, RDMA_WRID_DATA);

        if (ret) {

            fprintf(stderr, "rdma migration: error posting"

                    " extra control recv for anticipated result!");

            return ret;

        }

    }



    

    ret = qemu_rdma_post_recv_control(rdma, RDMA_WRID_READY);

    if (ret) {

        fprintf(stderr, "rdma migration: error posting first control recv!");

        return ret;

    }



    

    ret = qemu_rdma_post_send_control(rdma, data, head);



    if (ret < 0) {

        fprintf(stderr, "Failed to send control buffer!\n");

        return ret;

    }



    

    if (resp) {

        if (callback) {

            DDPRINTF("Issuing callback before receiving response...\n");

            ret = callback(rdma);

            if (ret < 0) {

                return ret;

            }

        }



        DDPRINTF("Waiting for response %s\n", control_desc[resp->type]);

        ret = qemu_rdma_exchange_get_response(rdma, resp,

                                              resp->type, RDMA_WRID_DATA);



        if (ret < 0) {

            return ret;

        }



        qemu_rdma_move_header(rdma, RDMA_WRID_DATA, resp);

        if (resp_idx) {

            *resp_idx = RDMA_WRID_DATA;

        }

        DDPRINTF("Response %s received.\n", control_desc[resp->type]);

    }



    rdma->control_ready_expected = 1;



    return 0;

}