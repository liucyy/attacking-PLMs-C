static int rpl_read_header(AVFormatContext *s, AVFormatParameters *ap)

{

    AVIOContext *pb = s->pb;

    RPLContext *rpl = s->priv_data;

    AVStream *vst = NULL, *ast = NULL;

    int total_audio_size;

    int error = 0;



    uint32_t i;



    int32_t audio_format, chunk_catalog_offset, number_of_chunks;

    AVRational fps;



    char line[RPL_LINE_LENGTH];



    

    

    

    

    

    

    error |= read_line(pb, line, sizeof(line));      

    error |= read_line(pb, line, sizeof(line));      

    av_dict_set(&s->metadata, "title"    , line, 0);

    error |= read_line(pb, line, sizeof(line));      

    av_dict_set(&s->metadata, "copyright", line, 0);

    error |= read_line(pb, line, sizeof(line));      

    av_dict_set(&s->metadata, "author"   , line, 0);



    

    vst = avformat_new_stream(s, NULL);

    if (!vst)

        return AVERROR(ENOMEM);

    vst->codec->codec_type      = AVMEDIA_TYPE_VIDEO;

    vst->codec->codec_tag       = read_line_and_int(pb, &error);  

    vst->codec->width           = read_line_and_int(pb, &error);  

    vst->codec->height          = read_line_and_int(pb, &error);  

    vst->codec->bits_per_coded_sample = read_line_and_int(pb, &error);  

    error |= read_line(pb, line, sizeof(line));                   

    fps = read_fps(line, &error);

    avpriv_set_pts_info(vst, 32, fps.den, fps.num);



    

    switch (vst->codec->codec_tag) {

#if 0

        case 122:

            vst->codec->codec_id = CODEC_ID_ESCAPE122;

            break;

#endif

        case 124:

            vst->codec->codec_id = CODEC_ID_ESCAPE124;

            

            vst->codec->bits_per_coded_sample = 16;

            break;

        case 130:

            vst->codec->codec_id = CODEC_ID_ESCAPE130;

            break;

        default:

            av_log(s, AV_LOG_WARNING,

                   "RPL video format %i not supported yet!\n",

                   vst->codec->codec_tag);

            vst->codec->codec_id = CODEC_ID_NONE;

    }



    



    

    

    audio_format = read_line_and_int(pb, &error);  

    if (audio_format) {

        ast = avformat_new_stream(s, NULL);

        if (!ast)

            return AVERROR(ENOMEM);

        ast->codec->codec_type      = AVMEDIA_TYPE_AUDIO;

        ast->codec->codec_tag       = audio_format;

        ast->codec->sample_rate     = read_line_and_int(pb, &error);  

        ast->codec->channels        = read_line_and_int(pb, &error);  

        ast->codec->bits_per_coded_sample = read_line_and_int(pb, &error);  

        

        

        if (ast->codec->bits_per_coded_sample == 0)

            ast->codec->bits_per_coded_sample = 4;



        ast->codec->bit_rate = ast->codec->sample_rate *

                               ast->codec->bits_per_coded_sample *

                               ast->codec->channels;



        ast->codec->codec_id = CODEC_ID_NONE;

        switch (audio_format) {

            case 1:

                if (ast->codec->bits_per_coded_sample == 16) {

                    

                    ast->codec->codec_id = CODEC_ID_PCM_S16LE;

                    break;

                }

                

                

                break;

            case 101:

                if (ast->codec->bits_per_coded_sample == 8) {

                    

                    

                    ast->codec->codec_id = CODEC_ID_PCM_U8;

                    break;

                } else if (ast->codec->bits_per_coded_sample == 4) {

                    ast->codec->codec_id = CODEC_ID_ADPCM_IMA_EA_SEAD;

                    break;

                }

                break;

        }

        if (ast->codec->codec_id == CODEC_ID_NONE) {

            av_log(s, AV_LOG_WARNING,

                   "RPL audio format %i not supported yet!\n",

                   audio_format);

        }

        avpriv_set_pts_info(ast, 32, 1, ast->codec->bit_rate);

    } else {

        for (i = 0; i < 3; i++)

            error |= read_line(pb, line, sizeof(line));

    }



    rpl->frames_per_chunk = read_line_and_int(pb, &error);  

    if (rpl->frames_per_chunk > 1 && vst->codec->codec_tag != 124)

        av_log(s, AV_LOG_WARNING,

               "Don't know how to split frames for video format %i. "

               "Video stream will be broken!\n", vst->codec->codec_tag);



    number_of_chunks = read_line_and_int(pb, &error);  

    

    number_of_chunks++;



    error |= read_line(pb, line, sizeof(line));  

    error |= read_line(pb, line, sizeof(line));  

    chunk_catalog_offset =                       

        read_line_and_int(pb, &error);           

    error |= read_line(pb, line, sizeof(line));  

    error |= read_line(pb, line, sizeof(line));  

    error |= read_line(pb, line, sizeof(line));  



    

    avio_seek(pb, chunk_catalog_offset, SEEK_SET);

    total_audio_size = 0;

    for (i = 0; i < number_of_chunks; i++) {

        int64_t offset, video_size, audio_size;

        error |= read_line(pb, line, sizeof(line));

        if (3 != sscanf(line, "%"PRId64" , %"PRId64" ; %"PRId64,

                        &offset, &video_size, &audio_size))

            error = -1;

        av_add_index_entry(vst, offset, i * rpl->frames_per_chunk,

                           video_size, rpl->frames_per_chunk, 0);

        if (ast)

            av_add_index_entry(ast, offset + video_size, total_audio_size,

                               audio_size, audio_size * 8, 0);

        total_audio_size += audio_size * 8;

    }



    if (error) return AVERROR(EIO);



    return 0;

}