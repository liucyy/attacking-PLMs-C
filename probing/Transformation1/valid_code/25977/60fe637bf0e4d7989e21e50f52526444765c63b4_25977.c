static size_t qemu_rdma_save_page(QEMUFile *f, void *opaque,

                                  ram_addr_t block_offset, ram_addr_t offset,

                                  size_t size, int *bytes_sent)

{

    QEMUFileRDMA *rfile = opaque;

    RDMAContext *rdma = rfile->rdma;

    int ret;



    CHECK_ERROR_STATE();



    qemu_fflush(f);



    if (size > 0) {

        

        ret = qemu_rdma_write(f, rdma, block_offset, offset, size);

        if (ret < 0) {

            fprintf(stderr, "rdma migration: write error! %d\n", ret);

            goto err;

        }



        

        if (bytes_sent) {

            *bytes_sent = 1;

        }

    } else {

        uint64_t index, chunk;



        



        ret = qemu_rdma_search_ram_block(rdma, block_offset,

                                         offset, size, &index, &chunk);



        if (ret) {

            fprintf(stderr, "ram block search failed\n");

            goto err;

        }



        qemu_rdma_signal_unregister(rdma, index, chunk, 0);



        

    }



    

    while (1) {

        uint64_t wr_id, wr_id_in;

        int ret = qemu_rdma_poll(rdma, &wr_id_in, NULL);

        if (ret < 0) {

            fprintf(stderr, "rdma migration: polling error! %d\n", ret);

            goto err;

        }



        wr_id = wr_id_in & RDMA_WRID_TYPE_MASK;



        if (wr_id == RDMA_WRID_NONE) {

            break;

        }

    }



    return RAM_SAVE_CONTROL_DELAYED;

err:

    rdma->error_state = ret;

    return ret;

}