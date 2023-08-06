void ide_atapi_cmd_reply_end(IDEState *s)

{

    int byte_count_limit, size, ret;

#ifdef DEBUG_IDE_ATAPI

    printf("reply: tx_size=%d elem_tx_size=%d index=%d\n",

           s->packet_transfer_size,

           s->elementary_transfer_size,

           s->io_buffer_index);

#endif

    if (s->packet_transfer_size <= 0) {

        

        ide_atapi_cmd_ok(s);

        ide_set_irq(s->bus);

#ifdef DEBUG_IDE_ATAPI

        printf("status=0x%x\n", s->status);

#endif

    } else {

        

        if (s->lba != -1 && s->io_buffer_index >= s->cd_sector_size) {

            ret = cd_read_sector(s, s->lba, s->io_buffer, s->cd_sector_size);

            if (ret < 0) {

                ide_atapi_io_error(s, ret);

                return;

            }

            s->lba++;

            s->io_buffer_index = 0;

        }

        if (s->elementary_transfer_size > 0) {

            

            size = s->cd_sector_size - s->io_buffer_index;

            if (size > s->elementary_transfer_size)

                size = s->elementary_transfer_size;

            s->packet_transfer_size -= size;

            s->elementary_transfer_size -= size;

            s->io_buffer_index += size;

            ide_transfer_start(s, s->io_buffer + s->io_buffer_index - size,

                               size, ide_atapi_cmd_reply_end);

        } else {

            

            s->nsector = (s->nsector & ~7) | ATAPI_INT_REASON_IO;

            byte_count_limit = atapi_byte_count_limit(s);

#ifdef DEBUG_IDE_ATAPI

            printf("byte_count_limit=%d\n", byte_count_limit);

#endif

            size = s->packet_transfer_size;

            if (size > byte_count_limit) {

                

                if (byte_count_limit & 1)

                    byte_count_limit--;

                size = byte_count_limit;

            }

            s->lcyl = size;

            s->hcyl = size >> 8;

            s->elementary_transfer_size = size;

            

            if (s->lba != -1) {

                if (size > (s->cd_sector_size - s->io_buffer_index))

                    size = (s->cd_sector_size - s->io_buffer_index);

            }

            s->packet_transfer_size -= size;

            s->elementary_transfer_size -= size;

            s->io_buffer_index += size;

            ide_transfer_start(s, s->io_buffer + s->io_buffer_index - size,

                               size, ide_atapi_cmd_reply_end);

            ide_set_irq(s->bus);

#ifdef DEBUG_IDE_ATAPI

            printf("status=0x%x\n", s->status);

#endif

        }

    }

}