static uint32_t sdhci_read_dataport(SDHCIState *s, unsigned size)

{

    uint32_t value = 0;

    int i;



    

    if ((s->prnsts & SDHC_DATA_AVAILABLE) == 0) {

        ERRPRINT("Trying to read from empty buffer\n");

        return 0;

    }



    for (i = 0; i < size; i++) {

        value |= s->fifo_buffer[s->data_count] << i * 8;

        s->data_count++;

        

        if ((s->data_count) >= (s->blksize & 0x0fff)) {

            DPRINT_L2("All %u bytes of data have been read from input buffer\n",

                    s->data_count);

            s->prnsts &= ~SDHC_DATA_AVAILABLE; 

            s->data_count = 0;  



            if (s->trnmod & SDHC_TRNS_BLK_CNT_EN) {

                s->blkcnt--;

            }



            

            if ((s->trnmod & SDHC_TRNS_MULTI) == 0 ||

                ((s->trnmod & SDHC_TRNS_BLK_CNT_EN) && (s->blkcnt == 0)) ||

                 

                (s->stopped_state == sdhc_gap_read &&

                 !(s->prnsts & SDHC_DAT_LINE_ACTIVE))) {

                SDHCI_GET_CLASS(s)->end_data_transfer(s);

            } else { 

                SDHCI_GET_CLASS(s)->read_block_from_card(s);

            }

            break;

        }

    }



    return value;

}