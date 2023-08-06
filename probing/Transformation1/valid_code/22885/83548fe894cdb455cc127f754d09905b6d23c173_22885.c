static int avi_write_ix(AVFormatContext *s)

{

    AVIOContext *pb = s->pb;

    AVIContext *avi = s->priv_data;

    char tag[5];

    char ix_tag[] = "ix00";

    int i, j;



    assert(pb->seekable);



    if (avi->riff_id > AVI_MASTER_INDEX_SIZE)

        return -1;



    for (i = 0; i < s->nb_streams; i++) {

        AVIStream *avist = s->streams[i]->priv_data;

        int64_t ix, pos;



        avi_stream2fourcc(tag, i, s->streams[i]->codecpar->codec_type);

        ix_tag[3] = '0' + i;



        

        ix = avio_tell(pb);

        ffio_wfourcc(pb, ix_tag);      

        avio_wl32(pb, avist->indexes.entry * 8 + 24);

        

        avio_wl16(pb, 2);           

        avio_w8(pb, 0);             

        avio_w8(pb, 1);             

        avio_wl32(pb, avist->indexes.entry);

        

        ffio_wfourcc(pb, tag);         

        avio_wl64(pb, avi->movi_list); 

        avio_wl32(pb, 0);              



        for (j = 0; j < avist->indexes.entry; j++) {

            AVIIentry *ie = avi_get_ientry(&avist->indexes, j);

            avio_wl32(pb, ie->pos + 8);

            avio_wl32(pb, ((uint32_t) ie->len & ~0x80000000) |

                          (ie->flags & 0x10 ? 0 : 0x80000000));

        }

        avio_flush(pb);

        pos = avio_tell(pb);



        

        avio_seek(pb, avist->indexes.indx_start - 8, SEEK_SET);

        ffio_wfourcc(pb, "indx");             

        avio_skip(pb, 8);

        avio_wl32(pb, avi->riff_id);          

        avio_skip(pb, 16 * avi->riff_id);

        avio_wl64(pb, ix);                    

        avio_wl32(pb, pos - ix);              

        avio_wl32(pb, avist->indexes.entry);  



        avio_seek(pb, pos, SEEK_SET);

    }

    return 0;

}