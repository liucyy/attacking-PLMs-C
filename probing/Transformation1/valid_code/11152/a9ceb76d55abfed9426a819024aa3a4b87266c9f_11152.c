static void start_input(DBDMA_channel *ch, int key, uint32_t addr,

                       uint16_t req_count, int is_last)

{

    DBDMA_DPRINTF("start_input\n");



    



    if (!addr || key > KEY_STREAM3) {

        kill_channel(ch);

        return;

    }



    ch->io.addr = addr;

    ch->io.len = req_count;

    ch->io.is_last = is_last;

    ch->io.dma_end = dbdma_end;

    ch->io.is_dma_out = 0;

    ch->processing = 1;

    ch->rw(&ch->io);

}