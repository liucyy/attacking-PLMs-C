static int mov_write_udta_tag(ByteIOContext *pb, MOVContext* mov,

                              AVFormatContext *s)

{

    offset_t pos = url_ftell(pb);

    int i;



    put_be32(pb, 0); 

    put_tag(pb, "udta");



    

    mov_write_meta_tag(pb, mov, s);



  if(mov->mode == MODE_MOV){ 

    

    for (i=0; i<MAX_STREAMS; i++) {

        if(mov->tracks[i].entry <= 0) continue;

        if (mov->tracks[i].enc->codec_id == CODEC_ID_AAC ||

            mov->tracks[i].enc->codec_id == CODEC_ID_MPEG4) {

            mov_write_string_tag(pb, "\251req", "QuickTime 6.0 or greater", 0);

            break;

        }

    }



    mov_write_string_tag(pb, "\251nam", s->title         , 0);

    mov_write_string_tag(pb, "\251aut", s->author        , 0);

    mov_write_string_tag(pb, "\251alb", s->album         , 0);

    mov_write_day_tag(pb, s->year, 0);

    if(mov->tracks[0].enc && !(mov->tracks[0].enc->flags & CODEC_FLAG_BITEXACT))

        mov_write_string_tag(pb, "\251enc", LIBAVFORMAT_IDENT, 0);

    mov_write_string_tag(pb, "\251des", s->comment       , 0);

    mov_write_string_tag(pb, "\251gen", s->genre         , 0);

  }



    return updateSize(pb, pos);

}