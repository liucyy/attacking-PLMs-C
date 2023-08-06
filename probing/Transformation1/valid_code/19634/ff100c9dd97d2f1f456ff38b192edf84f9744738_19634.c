static int webm_dash_manifest_read_header(AVFormatContext *s)

{

    char *buf;

    int ret = matroska_read_header(s);

    MatroskaTrack *tracks;

    MatroskaDemuxContext *matroska = s->priv_data;

    if (ret) {

        av_log(s, AV_LOG_ERROR, "Failed to read file headers\n");

        return -1;









    if (!matroska->is_live) {

        buf = av_asprintf("%g", matroska->duration);

        if (!buf) return AVERROR(ENOMEM);

        av_dict_set(&s->streams[0]->metadata, DURATION, buf, 0);

        av_free(buf);



        

        

        av_dict_set_int(&s->streams[0]->metadata, INITIALIZATION_RANGE, avio_tell(s->pb) - 5, 0);




    

    buf = strrchr(s->filename, '/');

    av_dict_set(&s->streams[0]->metadata, FILENAME, buf ? ++buf : s->filename, 0);



    

    tracks = matroska->tracks.elem;

    av_dict_set_int(&s->streams[0]->metadata, TRACK_NUMBER, tracks[0].num, 0);



    

    return matroska->is_live ? 0 : webm_dash_manifest_cues(s);