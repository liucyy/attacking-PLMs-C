static int aiff_read_header(AVFormatContext *s,

                            AVFormatParameters *ap)

{

    int size, filesize;

    offset_t offset = 0;

    uint32_t tag;

    unsigned version = AIFF_C_VERSION1;

    ByteIOContext *pb = s->pb;

    AVStream * st = s->streams[0];



    

    filesize = get_tag(pb, &tag);

    if (filesize < 0 || tag != MKTAG('F', 'O', 'R', 'M'))

        return AVERROR_INVALIDDATA;



    

    tag = get_le32(pb);

    if (tag == MKTAG('A', 'I', 'F', 'F'))       

        version = AIFF;

    else if (tag != MKTAG('A', 'I', 'F', 'C'))  

        return AVERROR_INVALIDDATA;



    filesize -= 4;



    st = av_new_stream(s, 0);

    if (!st)

        return AVERROR(ENOMEM);



    while (filesize > 0) {

        

        size = get_tag(pb, &tag);

        if (size < 0)

            return size;



        filesize -= size + 8;



        switch (tag) {

        case MKTAG('C', 'O', 'M', 'M'):     

            

            st->nb_frames = get_aiff_header (pb, st->codec, size, version);

            if (st->nb_frames < 0)

                return st->nb_frames;

            if (offset > 0) 

                goto got_sound;

            break;

        case MKTAG('F', 'V', 'E', 'R'):     

            version = get_be32(pb);

            break;

        case MKTAG('N', 'A', 'M', 'E'):     

            get_meta (pb, s->title, sizeof(s->title), size);

            break;

        case MKTAG('A', 'U', 'T', 'H'):     

            get_meta (pb, s->author, sizeof(s->author), size);

            break;

        case MKTAG('(', 'c', ')', ' '):     

            get_meta (pb, s->copyright, sizeof(s->copyright), size);

            break;

        case MKTAG('A', 'N', 'N', 'O'):     

            get_meta (pb, s->comment, sizeof(s->comment), size);

            break;

        case MKTAG('S', 'S', 'N', 'D'):     

            offset = get_be32(pb);      

            get_be32(pb);               

            offset += url_ftell(pb);    

            if (st->codec->codec_id)    

                goto got_sound;

            if (url_is_streamed(pb)) {

                av_log(s, AV_LOG_ERROR, "file is not seekable\n");


            }

            url_fskip(pb, size - 8);

            break;

        case MKTAG('w', 'a', 'v', 'e'):



            st->codec->extradata = av_mallocz(size + FF_INPUT_BUFFER_PADDING_SIZE);

            if (!st->codec->extradata)

                return AVERROR(ENOMEM);

            st->codec->extradata_size = size;

            get_buffer(pb, st->codec->extradata, size);

            break;

        default: 

            if (size & 1)   

                size++;

            url_fskip (pb, size);

        }

    }



    

    return AVERROR_INVALIDDATA;



got_sound:

    

    if (st->nb_frames)

        s->file_size = st->nb_frames * st->codec->block_align;



    av_set_pts_info(st, 64, 1, st->codec->sample_rate);

    st->start_time = 0;

    st->duration = st->codec->frame_size ?

        st->nb_frames * st->codec->frame_size : st->nb_frames;



    

    url_fseek(pb, offset, SEEK_SET);



    return 0;

}