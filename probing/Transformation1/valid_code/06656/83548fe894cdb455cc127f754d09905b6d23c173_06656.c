static int aiff_write_trailer(AVFormatContext *s)

{

    AVIOContext *pb = s->pb;

    AIFFOutputContext *aiff = s->priv_data;

    AVCodecParameters *par = s->streams[0]->codecpar;



    

    int64_t file_size, end_size;

    end_size = file_size = avio_tell(pb);

    if (file_size & 1) {

        avio_w8(pb, 0);

        end_size++;

    }



    if (s->pb->seekable) {

        

        avio_seek(pb, aiff->form, SEEK_SET);

        avio_wb32(pb, file_size - aiff->form - 4);



        

        avio_seek(pb, aiff->frames, SEEK_SET);

        avio_wb32(pb, (file_size - aiff->ssnd - 12) / par->block_align);



        

        avio_seek(pb, aiff->ssnd, SEEK_SET);

        avio_wb32(pb, file_size - aiff->ssnd - 4);



        

        avio_seek(pb, end_size, SEEK_SET);



        avio_flush(pb);

    }



    return 0;

}