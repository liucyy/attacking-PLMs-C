static void do_busid_cmd(ESPState *s, uint8_t *buf, uint8_t busid)

{

    int32_t datalen;

    int lun;



    DPRINTF("do_busid_cmd: busid 0x%x\n", busid);

    lun = busid & 7;

    datalen = s->current_dev->info->send_command(s->current_dev, 0, buf, lun);

    s->ti_size = datalen;

    if (datalen != 0) {

        s->rregs[ESP_RSTAT] = STAT_TC;

        s->dma_left = 0;

        s->dma_counter = 0;

        if (datalen > 0) {

            s->rregs[ESP_RSTAT] |= STAT_DI;

            s->current_dev->info->read_data(s->current_dev, 0);

        } else {

            s->rregs[ESP_RSTAT] |= STAT_DO;

            s->current_dev->info->write_data(s->current_dev, 0);

        }

    }

    s->rregs[ESP_RINTR] = INTR_BS | INTR_FC;

    s->rregs[ESP_RSEQ] = SEQ_CD;

    esp_raise_irq(s);

}