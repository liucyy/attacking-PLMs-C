void sd_write_data(SDState *sd, uint8_t value)

{

    int i;



    if (!sd->bdrv || !bdrv_is_inserted(sd->bdrv) || !sd->enable)

        return;



    if (sd->state != sd_receivingdata_state) {

        fprintf(stderr, "sd_write_data: not in Receiving-Data state\n");

        return;

    }



    if (sd->card_status & (ADDRESS_ERROR | WP_VIOLATION))

        return;



    switch (sd->current_cmd) {

    case 24:	

        sd->data[sd->data_offset ++] = value;

        if (sd->data_offset >= sd->blk_len) {

            

            sd->state = sd_programming_state;

            BLK_WRITE_BLOCK(sd->data_start, sd->data_offset);

            sd->blk_written ++;

            sd->csd[14] |= 0x40;

            

            sd->state = sd_transfer_state;

        }

        break;



    case 25:	

        if (sd->data_offset == 0) {

            

            if (sd->data_start + sd->blk_len > sd->size) {

                sd->card_status |= ADDRESS_ERROR;

                break;

            }

            if (sd_wp_addr(sd, sd->data_start)) {

                sd->card_status |= WP_VIOLATION;

                break;

            }

        }

        sd->data[sd->data_offset++] = value;

        if (sd->data_offset >= sd->blk_len) {

            

            sd->state = sd_programming_state;

            BLK_WRITE_BLOCK(sd->data_start, sd->data_offset);

            sd->blk_written++;

            sd->data_start += sd->blk_len;

            sd->data_offset = 0;

            sd->csd[14] |= 0x40;



            

            sd->state = sd_receivingdata_state;

        }

        break;



    case 26:	

        sd->data[sd->data_offset ++] = value;

        if (sd->data_offset >= sizeof(sd->cid)) {

            

            sd->state = sd_programming_state;

            for (i = 0; i < sizeof(sd->cid); i ++)

                if ((sd->cid[i] | 0x00) != sd->data[i])

                    sd->card_status |= CID_CSD_OVERWRITE;



            if (!(sd->card_status & CID_CSD_OVERWRITE))

                for (i = 0; i < sizeof(sd->cid); i ++) {

                    sd->cid[i] |= 0x00;

                    sd->cid[i] &= sd->data[i];

                }

            

            sd->state = sd_transfer_state;

        }

        break;



    case 27:	

        sd->data[sd->data_offset ++] = value;

        if (sd->data_offset >= sizeof(sd->csd)) {

            

            sd->state = sd_programming_state;

            for (i = 0; i < sizeof(sd->csd); i ++)

                if ((sd->csd[i] | sd_csd_rw_mask[i]) !=

                    (sd->data[i] | sd_csd_rw_mask[i]))

                    sd->card_status |= CID_CSD_OVERWRITE;



            

            if (sd->csd[14] & ~sd->data[14] & 0x60)

                sd->card_status |= CID_CSD_OVERWRITE;



            if (!(sd->card_status & CID_CSD_OVERWRITE))

                for (i = 0; i < sizeof(sd->csd); i ++) {

                    sd->csd[i] |= sd_csd_rw_mask[i];

                    sd->csd[i] &= sd->data[i];

                }

            

            sd->state = sd_transfer_state;

        }

        break;



    case 42:	

        sd->data[sd->data_offset ++] = value;

        if (sd->data_offset >= sd->blk_len) {

            

            sd->state = sd_programming_state;

            sd_lock_command(sd);

            

            sd->state = sd_transfer_state;

        }

        break;



    case 56:	

        sd->data[sd->data_offset ++] = value;

        if (sd->data_offset >= sd->blk_len) {

            APP_WRITE_BLOCK(sd->data_start, sd->data_offset);

            sd->state = sd_transfer_state;

        }

        break;



    default:

        fprintf(stderr, "sd_write_data: unknown command\n");

        break;

    }

}