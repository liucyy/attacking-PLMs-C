static int swf_write_header(AVFormatContext *s)

{

    SWFContext *swf = s->priv_data;

    AVIOContext *pb = s->pb;

    PutBitContext p;

    uint8_t buf1[256];

    int i, width, height, rate, rate_base;

    int version;



    swf->sound_samples = 0;

    swf->swf_frame_number = 0;

    swf->video_frame_number = 0;



    for(i=0;i<s->nb_streams;i++) {

        AVCodecContext *enc = s->streams[i]->codec;

        if (enc->codec_type == AVMEDIA_TYPE_AUDIO) {

            if (swf->audio_enc) {

                av_log(s, AV_LOG_ERROR, "SWF muxer only supports 1 audio stream\n");

                return AVERROR_INVALIDDATA;


            if (enc->codec_id == AV_CODEC_ID_MP3) {

                if (!enc->frame_size) {

                    av_log(s, AV_LOG_ERROR, "audio frame size not set\n");

                    return -1;


                swf->audio_enc = enc;

                swf->audio_fifo= av_fifo_alloc(AUDIO_FIFO_SIZE);

                if (!swf->audio_fifo)

                    return AVERROR(ENOMEM);

            } else {

                av_log(s, AV_LOG_ERROR, "SWF muxer only supports MP3\n");

                return -1;


        } else {

            if (swf->video_enc) {

                av_log(s, AV_LOG_ERROR, "SWF muxer only supports 1 video stream\n");

                return AVERROR_INVALIDDATA;


            if (enc->codec_id == AV_CODEC_ID_VP6F ||

                enc->codec_id == AV_CODEC_ID_FLV1 ||

                enc->codec_id == AV_CODEC_ID_MJPEG) {

                swf->video_st  = s->streams[i];

                swf->video_enc = enc;

            } else {

                av_log(s, AV_LOG_ERROR, "SWF muxer only supports VP6, FLV1 and MJPEG\n");

                return -1;






    if (!swf->video_enc) {

        

        width = 320;

        height = 200;

        rate = 10;

        rate_base= 1;

    } else {

        width = swf->video_enc->width;

        height = swf->video_enc->height;

        

        rate = swf->video_st->time_base.den;

        rate_base = swf->video_st->time_base.num;




    if (!swf->audio_enc)

        swf->samples_per_frame = (44100LL * rate_base) / rate;

    else

        swf->samples_per_frame = (swf->audio_enc->sample_rate * rate_base) / rate;



    avio_write(pb, "FWS", 3);



    if (!strcmp("avm2", s->oformat->name))

        version = 9;

    else if (swf->video_enc && swf->video_enc->codec_id == AV_CODEC_ID_VP6F)

        version = 8; 

    else if (swf->video_enc && swf->video_enc->codec_id == AV_CODEC_ID_FLV1)

        version = 6; 

    else

        version = 4; 

    avio_w8(pb, version);



    avio_wl32(pb, DUMMY_FILE_SIZE); 



    put_swf_rect(pb, 0, width * 20, 0, height * 20);





    avio_wl16(pb, (rate * 256) / rate_base); 

    swf->duration_pos = avio_tell(pb);

    avio_wl16(pb, (uint16_t)(DUMMY_DURATION * (int64_t)rate / rate_base)); 



    

    if (version == 9) {

        put_swf_tag(s, TAG_FILEATTRIBUTES);

        avio_wl32(pb, 1<<3); 

        put_swf_end_tag(s);




    

    if (swf->video_enc && swf->video_enc->codec_id == AV_CODEC_ID_MJPEG) {

        put_swf_tag(s, TAG_DEFINESHAPE);



        avio_wl16(pb, SHAPE_ID); 

        

        put_swf_rect(pb, 0, width, 0, height);

        

        avio_w8(pb, 1); 

        avio_w8(pb, 0x41); 

        avio_wl16(pb, BITMAP_ID); 

        

        put_swf_matrix(pb, 1 << FRAC_BITS, 0,

                       0,  1 << FRAC_BITS, 0, 0);

        avio_w8(pb, 0); 



        

        init_put_bits(&p, buf1, sizeof(buf1));

        put_bits(&p, 4, 1); 

        put_bits(&p, 4, 0); 



        put_bits(&p, 1, 0); 

        put_bits(&p, 5, FLAG_MOVETO | FLAG_SETFILL0);

        put_bits(&p, 5, 1); 

        put_bits(&p, 1, 0); 

        put_bits(&p, 1, 0); 

        put_bits(&p, 1, 1); 



        

        put_swf_line_edge(&p, width, 0);

        put_swf_line_edge(&p, 0, height);

        put_swf_line_edge(&p, -width, 0);

        put_swf_line_edge(&p, 0, -height);



        

        put_bits(&p, 1, 0); 

        put_bits(&p, 5, 0);



        flush_put_bits(&p);

        avio_write(pb, buf1, put_bits_ptr(&p) - p.buf);



        put_swf_end_tag(s);




    if (swf->audio_enc && swf->audio_enc->codec_id == AV_CODEC_ID_MP3) {

        int v = 0;



        

        put_swf_tag(s, TAG_STREAMHEAD2);

        switch(swf->audio_enc->sample_rate) {

        case 11025: v |= 1 << 2; break;

        case 22050: v |= 2 << 2; break;

        case 44100: v |= 3 << 2; break;

        default:

            

            av_log(s, AV_LOG_ERROR, "swf does not support that sample rate, choose from (44100, 22050, 11025).\n");

            return -1;


        v |= 0x02; 

        if (swf->audio_enc->channels == 2)

            v |= 0x01; 

        avio_w8(s->pb, v);

        v |= 0x20; 

        avio_w8(s->pb, v);

        avio_wl16(s->pb, swf->samples_per_frame);  

        avio_wl16(s->pb, 0);



        put_swf_end_tag(s);




    avio_flush(s->pb);

    return 0;