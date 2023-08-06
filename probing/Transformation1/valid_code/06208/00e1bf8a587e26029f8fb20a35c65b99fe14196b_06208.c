static int flic_read_header(AVFormatContext *s)

{

    FlicDemuxContext *flic = s->priv_data;

    AVIOContext *pb = s->pb;

    unsigned char header[FLIC_HEADER_SIZE];

    AVStream *st, *ast;

    int speed;

    int magic_number;

    unsigned char preamble[FLIC_PREAMBLE_SIZE];



    flic->frame_number = 0;



    

    if (avio_read(pb, header, FLIC_HEADER_SIZE) != FLIC_HEADER_SIZE)

        return AVERROR(EIO);



    magic_number = AV_RL16(&header[4]);

    speed = AV_RL32(&header[0x10]);

    if (speed == 0)

        speed = FLIC_DEFAULT_SPEED;



    

    st = avformat_new_stream(s, NULL);

    if (!st)

        return AVERROR(ENOMEM);

    flic->video_stream_index = st->index;

    st->codec->codec_type = AVMEDIA_TYPE_VIDEO;

    st->codec->codec_id = AV_CODEC_ID_FLIC;

    st->codec->codec_tag = 0;  

    st->codec->width = AV_RL16(&header[0x08]);

    st->codec->height = AV_RL16(&header[0x0A]);



    if (!st->codec->width || !st->codec->height) {

        

        

        av_log(s, AV_LOG_WARNING,

               "File with no specified width/height. Trying 640x480.\n");

        st->codec->width  = 640;

        st->codec->height = 480;

    }



    

    st->codec->extradata_size = FLIC_HEADER_SIZE;

    st->codec->extradata = av_malloc(FLIC_HEADER_SIZE);

    memcpy(st->codec->extradata, header, FLIC_HEADER_SIZE);



    

    if (avio_read(pb, preamble, FLIC_PREAMBLE_SIZE) != FLIC_PREAMBLE_SIZE) {

        av_log(s, AV_LOG_ERROR, "Failed to peek at preamble\n");

        return AVERROR(EIO);

    }



    avio_seek(pb, -FLIC_PREAMBLE_SIZE, SEEK_CUR);



    

    if (AV_RL16(&preamble[4]) == FLIC_TFTD_CHUNK_AUDIO) {

        

        ast = avformat_new_stream(s, NULL);

        if (!ast)

            return AVERROR(ENOMEM);



        flic->audio_stream_index = ast->index;



        

        ast->codec->block_align = AV_RL32(&preamble[0]);

        ast->codec->codec_type = AVMEDIA_TYPE_AUDIO;

        ast->codec->codec_id = AV_CODEC_ID_PCM_U8;

        ast->codec->codec_tag = 0;

        ast->codec->sample_rate = FLIC_TFTD_SAMPLE_RATE;

        ast->codec->channels = 1;

        ast->codec->bit_rate = st->codec->sample_rate * 8;

        ast->codec->bits_per_coded_sample = 8;

        ast->codec->channel_layout = AV_CH_LAYOUT_MONO;

        ast->codec->extradata_size = 0;



        

        avpriv_set_pts_info(st, 64, ast->codec->block_align, FLIC_TFTD_SAMPLE_RATE);

        avpriv_set_pts_info(ast, 64, 1, FLIC_TFTD_SAMPLE_RATE);

    } else if (AV_RL16(&header[0x10]) == FLIC_CHUNK_MAGIC_1) {

        avpriv_set_pts_info(st, 64, FLIC_MC_SPEED, 70);



        

        avio_seek(pb, 12, SEEK_SET);



        

        av_free(st->codec->extradata);

        st->codec->extradata_size = 12;

        st->codec->extradata = av_malloc(12);

        memcpy(st->codec->extradata, header, 12);



    } else if (magic_number == FLIC_FILE_MAGIC_1) {

        avpriv_set_pts_info(st, 64, speed, 70);

    } else if ((magic_number == FLIC_FILE_MAGIC_2) ||

               (magic_number == FLIC_FILE_MAGIC_3)) {

        avpriv_set_pts_info(st, 64, speed, 1000);

    } else {

        av_log(s, AV_LOG_ERROR, "Invalid or unsupported magic chunk in file\n");

        return AVERROR_INVALIDDATA;

    }



    return 0;

}