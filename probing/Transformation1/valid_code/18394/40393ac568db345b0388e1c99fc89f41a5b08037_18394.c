static int mov_write_hdlr_tag(AVIOContext *pb, MOVTrack *track)

{

    const char *hdlr, *descr = NULL, *hdlr_type = NULL;

    int64_t pos = avio_tell(pb);



    if (!track) { 





        hdlr = (track->mode == MODE_MOV) ? "mhlr" : "\0\0\0\0";

        if (track->enc->codec_type == AVMEDIA_TYPE_VIDEO) {

            hdlr_type = "vide";

            descr = "VideoHandler";

        } else if (track->enc->codec_type == AVMEDIA_TYPE_AUDIO) {

            hdlr_type = "soun";

            descr = "SoundHandler";

        } else if (track->enc->codec_type == AVMEDIA_TYPE_SUBTITLE) {

            if (track->tag == MKTAG('t','x','3','g')) hdlr_type = "sbtl";

            else                                      hdlr_type = "text";

            descr = "SubtitleHandler";

        } else if (track->enc->codec_tag == MKTAG('r','t','p',' ')) {

            hdlr_type = "hint";

            descr = "HintHandler";





        }

    }



    avio_wb32(pb, 0); 

    ffio_wfourcc(pb, "hdlr");

    avio_wb32(pb, 0); 

    avio_write(pb, hdlr, 4); 

    ffio_wfourcc(pb, hdlr_type); 

    avio_wb32(pb ,0); 

    avio_wb32(pb ,0); 

    avio_wb32(pb ,0); 

    if (!track || track->mode == MODE_MOV)

        avio_w8(pb, strlen(descr)); 

    avio_write(pb, descr, strlen(descr)); 

    if (track && track->mode != MODE_MOV)

        avio_w8(pb, 0); 

    return update_size(pb, pos);

}