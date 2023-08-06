static void cmd_get_event_status_notification(IDEState *s,

                                              uint8_t *buf)

{

    const uint8_t *packet = buf;



    struct {

        uint8_t opcode;

        uint8_t polled;        

        uint8_t reserved2[2];

        uint8_t class;

        uint8_t reserved3[2];

        uint16_t len;

        uint8_t control;

    } __attribute__((packed)) *gesn_cdb;



    struct {

        uint16_t len;

        uint8_t notification_class;

        uint8_t supported_events;

    } __attribute((packed)) *gesn_event_header;



    enum notification_class_request_type {

        NCR_RESERVED1 = 1 << 0,

        NCR_OPERATIONAL_CHANGE = 1 << 1,

        NCR_POWER_MANAGEMENT = 1 << 2,

        NCR_EXTERNAL_REQUEST = 1 << 3,

        NCR_MEDIA = 1 << 4,

        NCR_MULTI_HOST = 1 << 5,

        NCR_DEVICE_BUSY = 1 << 6,

        NCR_RESERVED2 = 1 << 7,

    };

    enum event_notification_class_field {

        ENC_NO_EVENTS = 0,

        ENC_OPERATIONAL_CHANGE,

        ENC_POWER_MANAGEMENT,

        ENC_EXTERNAL_REQUEST,

        ENC_MEDIA,

        ENC_MULTIPLE_HOSTS,

        ENC_DEVICE_BUSY,

        ENC_RESERVED,

    };

    unsigned int max_len, used_len;



    gesn_cdb = (void *)packet;

    gesn_event_header = (void *)buf;



    max_len = be16_to_cpu(gesn_cdb->len);



    

    if (!(gesn_cdb->polled & 0x01)) { 

        

        ide_atapi_cmd_error(s, SENSE_ILLEGAL_REQUEST,

                            ASC_INV_FIELD_IN_CMD_PACKET);

        return;

    }



    



    

    gesn_event_header->supported_events = NCR_MEDIA;



    

    gesn_event_header->notification_class = 0;



    

    if (gesn_cdb->class & NCR_MEDIA) {

        gesn_event_header->notification_class |= ENC_MEDIA;

        used_len = event_status_media(s, buf);

    } else {

        gesn_event_header->notification_class = 0x80; 

        used_len = sizeof(*gesn_event_header);

    }

    gesn_event_header->len = cpu_to_be16(used_len

                                         - sizeof(*gesn_event_header));

    ide_atapi_cmd_reply(s, used_len, max_len);

}