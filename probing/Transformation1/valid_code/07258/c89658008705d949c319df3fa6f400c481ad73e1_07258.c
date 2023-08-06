static void sdp_parse_fmtp(AVStream *st, const char *p)

{

    char attr[256];

    

    char value[16384];

    int i;



    RTSPStream *rtsp_st = st->priv_data;

    AVCodecContext *codec = st->codec;

    RTPPayloadData *rtp_payload_data = &rtsp_st->rtp_payload_data;



    

    while(rtsp_next_attr_and_value(&p, attr, sizeof(attr), value, sizeof(value)))

    {

        

        sdp_parse_fmtp_config(codec, rtsp_st->dynamic_protocol_context,

                              attr, value);

        

        for (i = 0; attr_names[i].str; ++i) {

            if (!strcasecmp(attr, attr_names[i].str)) {

                if (attr_names[i].type == ATTR_NAME_TYPE_INT)

                    *(int *)((char *)rtp_payload_data + attr_names[i].offset) = atoi(value);

                else if (attr_names[i].type == ATTR_NAME_TYPE_STR)

                    *(char **)((char *)rtp_payload_data + attr_names[i].offset) = av_strdup(value);

            }

        }

    }

}