static void write_odml_master(AVFormatContext *s, int stream_index)

{

    AVIOContext *pb = s->pb;

    AVStream *st = s->streams[stream_index];

    AVCodecContext *enc = st->codec;

    AVIStream *avist = st->priv_data;

    unsigned char tag[5];

    int j;



    

    avist->indexes.entry      = avist->indexes.ents_allocated = 0;

    avist->indexes.indx_start = ff_start_tag(pb, "JUNK");

    avio_wl16(pb, 4);   

    avio_w8(pb, 0);     

    avio_w8(pb, 0);     

    avio_wl32(pb, 0);   

    ffio_wfourcc(pb, avi_stream2fourcc(tag, stream_index, enc->codec_type));

                        

    avio_wl64(pb, 0);   

    avio_wl32(pb, 0);   

    for (j = 0; j < AVI_MASTER_INDEX_SIZE * 2; j++)

        avio_wl64(pb, 0);

    ff_end_tag(pb, avist->indexes.indx_start);

}