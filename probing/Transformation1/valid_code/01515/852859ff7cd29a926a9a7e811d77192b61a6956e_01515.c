static int mov_read_esds(MOVContext *c, ByteIOContext *pb, MOV_atom_t atom)

{

    AVStream *st = c->fc->streams[c->fc->nb_streams-1];

    int tag, len;



    get_be32(pb); 

    len = mp4_read_descr(c, pb, &tag);

    if (tag == MP4ESDescrTag) {

        get_be16(pb); 

        get_byte(pb); 

    } else

        get_be16(pb); 



    len = mp4_read_descr(c, pb, &tag);

    if (tag == MP4DecConfigDescrTag) {

        int object_type_id = get_byte(pb);

        get_byte(pb); 

        get_be24(pb); 

        get_be32(pb); 

        get_be32(pb); 



        st->codec->codec_id= codec_get_id(ff_mp4_obj_type, object_type_id);

        dprintf(c->fc, "esds object type id %d\n", object_type_id);

        len = mp4_read_descr(c, pb, &tag);

        if (tag == MP4DecSpecificDescrTag) {

            dprintf(c->fc, "Specific MPEG4 header len=%d\n", len);



            st->codec->extradata = av_mallocz(len + FF_INPUT_BUFFER_PADDING_SIZE);

            if (!st->codec->extradata)

                return AVERROR(ENOMEM);

            get_buffer(pb, st->codec->extradata, len);

            st->codec->extradata_size = len;

            

            if ((*st->codec->extradata >> 3) == 29) {

                st->codec->codec_id = CODEC_ID_MP3ON4;

            }

        }

    }

    return 0;

}