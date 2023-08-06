static int flv_read_metabody(AVFormatContext *s, int64_t next_pos)

{

    AMFDataType type;

    AVStream *stream, *astream, *vstream;

    AVIOContext *ioc;

    int i;

    

    

    char buffer[11];



    astream = NULL;

    vstream = NULL;

    ioc     = s->pb;



    

    type = avio_r8(ioc);

    if (type != AMF_DATA_TYPE_STRING ||

        amf_get_string(ioc, buffer, sizeof(buffer)) < 0)

        return -1;



    if (!strcmp(buffer, "onTextData"))

        return 1;



    if (strcmp(buffer, "onMetaData"))

        return -1;



    

    

    for (i = 0; i < s->nb_streams; i++) {

        stream = s->streams[i];

        if (stream->codec->codec_type == AVMEDIA_TYPE_AUDIO)

            astream = stream;

        else if (stream->codec->codec_type == AVMEDIA_TYPE_VIDEO)

            vstream = stream;

    }



    

    if (amf_parse_object(s, astream, vstream, buffer, next_pos, 0) < 0)

        return -1;



    return 0;

}