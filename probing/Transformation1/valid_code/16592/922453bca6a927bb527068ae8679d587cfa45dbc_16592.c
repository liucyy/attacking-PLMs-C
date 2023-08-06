void bmdma_cmd_writeb(BMDMAState *bm, uint32_t val)

{

#ifdef DEBUG_IDE

    printf("%s: 0x%08x\n", __func__, val);

#endif



    

    if ((val & BM_CMD_START) != (bm->cmd & BM_CMD_START)) {

        if (!(val & BM_CMD_START)) {

            

            if (bm->bus->dma->aiocb) {

                qemu_aio_flush();

                assert(bm->bus->dma->aiocb == NULL);

                assert((bm->status & BM_STATUS_DMAING) == 0);

            }

        } else {

            bm->cur_addr = bm->addr;

            if (!(bm->status & BM_STATUS_DMAING)) {

                bm->status |= BM_STATUS_DMAING;

                

                if (bm->dma_cb)

                    bm->dma_cb(bmdma_active_if(bm), 0);

            }

        }

    }



    bm->cmd = val & 0x09;

}