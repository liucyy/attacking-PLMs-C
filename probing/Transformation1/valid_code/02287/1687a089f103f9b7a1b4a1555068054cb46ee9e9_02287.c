event_thread(void *arg)

{

    unsigned char atr[MAX_ATR_LEN];

    int atr_len = MAX_ATR_LEN;

    VEvent *event = NULL;

    unsigned int reader_id;





    while (1) {

        const char *reader_name;



        event = vevent_wait_next_vevent();

        if (event == NULL) {

            break;

        }

        reader_id = vreader_get_id(event->reader);

        if (reader_id == VSCARD_UNDEFINED_READER_ID &&

            event->type != VEVENT_READER_INSERT) {

            

            

            qemu_mutex_lock(&pending_reader_lock);

            if (!pending_reader || (pending_reader != event->reader)) {

                

                qemu_mutex_unlock(&pending_reader_lock);

                vevent_delete(event);

                continue;

            }

            

            while (pending_reader != NULL) {

                qemu_cond_wait(&pending_reader_condition, &pending_reader_lock);

            }

            qemu_mutex_unlock(&pending_reader_lock);

            

            reader_id = vreader_get_id(event->reader);

            if (reader_id == VSCARD_UNDEFINED_READER_ID) {

                

                vevent_delete(event);

                continue;

            }

            

        }

        switch (event->type) {

        case VEVENT_READER_INSERT:

            

            

            qemu_mutex_lock(&pending_reader_lock);

            while (pending_reader != NULL) {

                qemu_cond_wait(&pending_reader_condition, &pending_reader_lock);

            }

            pending_reader = vreader_reference(event->reader);

            qemu_mutex_unlock(&pending_reader_lock);

            reader_name = vreader_get_name(event->reader);

            if (verbose > 10) {

                printf(" READER INSERT: %s\n", reader_name);

            }

            send_msg(VSC_ReaderAdd,

                reader_id, 

                NULL, 0 );

            break;

        case VEVENT_READER_REMOVE:

            

            if (verbose > 10) {

                printf(" READER REMOVE: %u\n", reader_id);

            }

            send_msg(VSC_ReaderRemove, reader_id, NULL, 0);

            break;

        case VEVENT_CARD_INSERT:

            

            atr_len = MAX_ATR_LEN;

            vreader_power_on(event->reader, atr, &atr_len);

            

            if (verbose > 10) {

                printf(" CARD INSERT %u: ", reader_id);

                print_byte_array(atr, atr_len);

            }

            send_msg(VSC_ATR, reader_id, atr, atr_len);

            break;

        case VEVENT_CARD_REMOVE:

            

            if (verbose > 10) {

                printf(" CARD REMOVE %u:\n", reader_id);

            }

            send_msg(VSC_CardRemove, reader_id, NULL, 0);

            break;

        default:

            break;

        }

        vevent_delete(event);

    }

    return NULL;

}