int dv_produce_packet(DVDemuxContext *c, AVPacket *pkt,

                      uint8_t* buf, int buf_size)

{

    int size, i;

    uint8_t *ppcm[4] = {0};



    if (buf_size < DV_PROFILE_BYTES ||

        !(c->sys = dv_frame_profile(buf)) ||

        buf_size < c->sys->frame_size) {

          return -1;   

    }



    

    

    size = dv_extract_audio_info(c, buf);

    for (i = 0; i < c->ach; i++) {

       c->audio_pkt[i].size = size;

       c->audio_pkt[i].pts  = c->abytes * 30000*8 / c->ast[i]->codec->bit_rate;

       ppcm[i] = c->audio_buf[i];

    }

    dv_extract_audio(buf, ppcm, c->sys);

    c->abytes += size;



    

    if (c->sys->height == 720) {

        if (buf[1] & 0x0C)

            c->audio_pkt[2].size = c->audio_pkt[3].size = 0;

        else

            c->audio_pkt[0].size = c->audio_pkt[1].size = 0;

    }



    

    size = dv_extract_video_info(c, buf);

    av_init_packet(pkt);

    pkt->data         = buf;

    pkt->size         = size;

    pkt->flags       |= PKT_FLAG_KEY;

    pkt->stream_index = c->vst->id;

    pkt->pts          = c->frames;



    c->frames++;



    return size;

}