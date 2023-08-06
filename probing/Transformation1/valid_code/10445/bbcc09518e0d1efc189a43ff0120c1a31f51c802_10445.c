static void update_odml_entry(AVFormatContext *s, int stream_index, int64_t ix)

{

    AVIOContext *pb = s->pb;

    AVIContext *avi = s->priv_data;

    AVIStream *avist = s->streams[stream_index]->priv_data;

    int64_t pos;

    int au_byterate, au_ssize, au_scale;



    avio_flush(pb);

    pos = avio_tell(pb);



    

    avio_seek(pb, avist->indexes.indx_start - 8, SEEK_SET);

    ffio_wfourcc(pb, "indx");             

    avio_skip(pb, 8);

    avio_wl32(pb, avi->riff_id);          

    avio_skip(pb, 16 * avi->riff_id);

    avio_wl64(pb, ix);                    

    avio_wl32(pb, pos - ix);              

    ff_parse_specific_params(s->streams[stream_index], &au_byterate, &au_ssize, &au_scale);

    if (s->streams[stream_index]->codec->codec_type == AVMEDIA_TYPE_AUDIO && au_ssize > 0) {

        uint32_t audio_segm_size = (avist->audio_strm_length - avist->indexes.audio_strm_offset);

        if ((audio_segm_size % au_ssize > 0) && !avist->sample_requested) {

            avpriv_request_sample(s, "OpenDML index duration for audio packets with partial frames");

            avist->sample_requested = 1;

        }

        avio_wl32(pb, audio_segm_size / au_ssize);  

    } else

        avio_wl32(pb, avist->indexes.entry);  



    avio_seek(pb, pos, SEEK_SET);

}