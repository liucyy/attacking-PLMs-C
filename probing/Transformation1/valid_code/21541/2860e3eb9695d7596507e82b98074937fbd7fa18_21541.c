void bmdma_cmd_writeb(void *opaque, uint32_t addr, uint32_t val)

{

    BMDMAState *bm = opaque;

#ifdef DEBUG_IDE

    printf("%s: 0x%08x\n", __func__, val);

#endif



    

    if ((val & BM_CMD_START) != (bm->cmd & BM_CMD_START)) {

        if (!(val & BM_CMD_START)) {

            

            if (bm->bus->dma->aiocb) {

                qemu_aio_flush();

#ifdef DEBUG_IDE

                if (bm->bus->dma->aiocb)

                    printf("ide_dma_cancel: aiocb still pending\n");

                if (bm->status & BM_STATUS_DMAING)

                    printf("ide_dma_cancel: BM_STATUS_DMAING still pending\n");

#endif

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