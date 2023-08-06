uint8_t sd_read_data(SDState *sd)

{

    

    uint8_t ret;

    int io_len;



    if (!sd->blk || !blk_is_inserted(sd->blk) || !sd->enable)

        return 0x00;



    if (sd->state != sd_sendingdata_state) {

        qemu_log_mask(LOG_GUEST_ERROR,

                      "sd_read_data: not in Sending-Data state\n");

        return 0x00;

    }



    if (sd->card_status & (ADDRESS_ERROR | WP_VIOLATION))

        return 0x00;



    io_len = (sd->ocr & (1 << 30)) ? 512 : sd->blk_len;



    switch (sd->current_cmd) {

    case 6:	

        ret = sd->data[sd->data_offset ++];



        if (sd->data_offset >= 64)

            sd->state = sd_transfer_state;

        break;



    case 9:	

    case 10:	

        ret = sd->data[sd->data_offset ++];



        if (sd->data_offset >= 16)

            sd->state = sd_transfer_state;

        break;



    case 11:	

        if (sd->data_offset == 0)

            BLK_READ_BLOCK(sd->data_start, io_len);

        ret = sd->data[sd->data_offset ++];



        if (sd->data_offset >= io_len) {

            sd->data_start += io_len;

            sd->data_offset = 0;

            if (sd->data_start + io_len > sd->size) {

                sd->card_status |= ADDRESS_ERROR;

                break;

            }

        }

        break;



    case 13:	

        ret = sd->sd_status[sd->data_offset ++];



        if (sd->data_offset >= sizeof(sd->sd_status))

            sd->state = sd_transfer_state;

        break;



    case 17:	

        if (sd->data_offset == 0)

            BLK_READ_BLOCK(sd->data_start, io_len);

        ret = sd->data[sd->data_offset ++];



        if (sd->data_offset >= io_len)

            sd->state = sd_transfer_state;

        break;



    case 18:	

        if (sd->data_offset == 0)

            BLK_READ_BLOCK(sd->data_start, io_len);

        ret = sd->data[sd->data_offset ++];



        if (sd->data_offset >= io_len) {

            sd->data_start += io_len;

            sd->data_offset = 0;



            if (sd->multi_blk_cnt != 0) {

                if (--sd->multi_blk_cnt == 0) {

                    

                    sd->state = sd_transfer_state;

                    break;

                }

            }



            if (sd->data_start + io_len > sd->size) {

                sd->card_status |= ADDRESS_ERROR;

                break;

            }

        }

        break;



    case 22:	

        ret = sd->data[sd->data_offset ++];



        if (sd->data_offset >= 4)

            sd->state = sd_transfer_state;

        break;



    case 30:	

        ret = sd->data[sd->data_offset ++];



        if (sd->data_offset >= 4)

            sd->state = sd_transfer_state;

        break;



    case 51:	

        ret = sd->scr[sd->data_offset ++];



        if (sd->data_offset >= sizeof(sd->scr))

            sd->state = sd_transfer_state;

        break;



    case 56:	

        if (sd->data_offset == 0)

            APP_READ_BLOCK(sd->data_start, sd->blk_len);

        ret = sd->data[sd->data_offset ++];



        if (sd->data_offset >= sd->blk_len)

            sd->state = sd_transfer_state;

        break;



    default:

        qemu_log_mask(LOG_GUEST_ERROR, "sd_read_data: unknown command\n");

        return 0x00;

    }



    return ret;

}