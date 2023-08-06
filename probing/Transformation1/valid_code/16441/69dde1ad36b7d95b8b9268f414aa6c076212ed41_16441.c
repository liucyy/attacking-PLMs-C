static int mov_write_tkhd_tag(ByteIOContext *pb, MOVTrack* track)

{

    int64_t maxTrackLenTemp;

    put_be32(pb, 0x5c); 

    put_tag(pb, "tkhd");

    put_be32(pb, 0xf); 

    put_be32(pb, track->time); 

    put_be32(pb, track->time); 

    put_be32(pb, track->trackID); 

    put_be32(pb, 0); 

    maxTrackLenTemp = ((int64_t)globalTimescale*(int64_t)track->trackDuration)/(int64_t)track->timescale;

    put_be32(pb, (long)maxTrackLenTemp); 



    put_be32(pb, 0); 

    put_be32(pb, 0); 

    put_be32(pb, 0x0); 

    

    if(track->enc->codec_type == CODEC_TYPE_AUDIO)

        put_be16(pb, 0x0100);

    else

        put_be16(pb, 0);

    put_be16(pb, 0); 



    

    put_be32(pb, 0x00010000); 

    put_be32(pb, 0x0); 

    put_be32(pb, 0x0); 

    put_be32(pb, 0x0); 

    put_be32(pb, 0x00010000); 

    put_be32(pb, 0x0); 

    put_be32(pb, 0x0); 

    put_be32(pb, 0x0); 

    put_be32(pb, 0x40000000); 



    

    if(track->enc->codec_type == CODEC_TYPE_VIDEO) {

        put_be32(pb, track->enc->width*0x10000);

        put_be32(pb, track->enc->height*0x10000);

    }

    else {

        put_be32(pb, 0);

        put_be32(pb, 0);

    }

    return 0x5c;

}