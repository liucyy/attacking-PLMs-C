static int mxf_get_stream_index(AVFormatContext *s, KLVPacket *klv)

{

    int i;



    for (i = 0; i < s->nb_streams; i++) {

        MXFTrack *track = s->streams[i]->priv_data;

        

        if (!memcmp(klv->key + sizeof(mxf_essence_element_key), track->track_number, sizeof(track->track_number)))

            return i;

    }

    

    return s->nb_streams == 1 ? 0 : -1;

}