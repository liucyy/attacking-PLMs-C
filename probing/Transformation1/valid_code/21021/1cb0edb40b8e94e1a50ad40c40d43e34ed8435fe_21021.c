static int mpeg_mux_read_header(AVFormatContext *s,

                                AVFormatParameters *ap)

{

    MpegDemuxContext *m;

    int size, startcode, c, rate_bound, audio_bound, video_bound, mux_rate, val;

    int codec_id, n, i, type;

    AVStream *st;



    m = av_mallocz(sizeof(MpegDemuxContext));

    if (!m)

        return -ENOMEM;

    s->priv_data = m;



    

    m->header_state = 0xff;

    size = MAX_SYNC_SIZE;

    for(;;) {

        while (size > 0) {

            startcode = find_start_code(&s->pb, &size, &m->header_state);

            if (startcode == PACK_START_CODE)

                goto found;

        }

        return -ENODATA;

    found:

        

        

        get_byte(&s->pb); 

        get_be16(&s->pb); 

        get_be16(&s->pb); 



        mux_rate = get_byte(&s->pb) << 16; 

        mux_rate |= get_byte(&s->pb) << 8;

        mux_rate |= get_byte(&s->pb);

        mux_rate &= (1 << 22) - 1;

        m->mux_rate = mux_rate;



        startcode = find_start_code(&s->pb, &size, &m->header_state);

        if (startcode == SYSTEM_HEADER_START_CODE)

            break;

    }

    size = get_be16(&s->pb);

    rate_bound = get_byte(&s->pb) << 16;

    rate_bound |= get_byte(&s->pb) << 8;

    rate_bound |= get_byte(&s->pb);

    rate_bound = (rate_bound >> 1) & ((1 << 22) - 1);

    audio_bound = get_byte(&s->pb) >> 2;

    video_bound = get_byte(&s->pb) & 0x1f;

    get_byte(&s->pb); 

#if 0

    printf("mux_rate=%d kbit/s\n", (m->mux_rate * 50 * 8) / 1000);

    printf("rate_bound=%d\n", rate_bound);

    printf("audio_bound=%d\n", audio_bound);

    printf("video_bound=%d\n", video_bound);

#endif

    size -= 6;

    s->nb_streams = 0;

    while (size > 0) {

        c = get_byte(&s->pb);

        size--;

        if ((c & 0x80) == 0)

            break;

        val = get_be16(&s->pb);

        size -= 2;

        if (c >= 0xc0 && c <= 0xdf) {

            

            type = CODEC_TYPE_AUDIO;

            codec_id = CODEC_ID_MP2;

            n = 1;

            c = c | 0x100;

        } else if (c >= 0xe0 && c <= 0xef) {

            type = CODEC_TYPE_VIDEO;

            codec_id = CODEC_ID_MPEG1VIDEO;

            n = 1;

            c = c | 0x100;

        } else if (c == 0xb8) {

            

            

            type = CODEC_TYPE_AUDIO;

            codec_id = CODEC_ID_AC3;

            n = audio_bound;

            c = 0x80;

            

        } else if (c == 0xb9) {

            

            type = CODEC_TYPE_VIDEO;

            codec_id = CODEC_ID_MPEG1VIDEO;

            n = video_bound;

            c = 0x1e0;

        } else {

            type = 0;

            codec_id = 0;

            n = 0;

        }

        for(i=0;i<n;i++) {

            st = av_mallocz(sizeof(AVStream));

            if (!st)

                return -ENOMEM;

            s->streams[s->nb_streams++] = st;

            st->id = c + i;

            st->codec.codec_type = type;

            st->codec.codec_id = codec_id;

        }

    }



    return 0;

}