static int mov_write_mvhd_tag(ByteIOContext *pb, MOVContext *mov)

{

    int maxTrackID = 1, i;

    int64_t maxTrackLenTemp, maxTrackLen = 0;

    int version;



    for (i=0; i<MAX_STREAMS; i++) {

        if(mov->tracks[i].entry > 0) {

            maxTrackLenTemp = av_rescale_rnd(mov->tracks[i].trackDuration, globalTimescale, mov->tracks[i].timescale, AV_ROUND_UP);

            if(maxTrackLen < maxTrackLenTemp)

                maxTrackLen = maxTrackLenTemp;

            if(maxTrackID < mov->tracks[i].trackID)

                maxTrackID = mov->tracks[i].trackID;

        }

    }



    version = maxTrackLen < UINT32_MAX ? 0 : 1;

    (version == 1) ? put_be32(pb, 120) : put_be32(pb, 108); 

    put_tag(pb, "mvhd");

    put_byte(pb, version);

    put_be24(pb, 0); 

    if (version == 1) {

        put_be64(pb, mov->time);

        put_be64(pb, mov->time);

    } else {

        put_be32(pb, mov->time); 

        put_be32(pb, mov->time); 

    }

    put_be32(pb, mov->timescale); 

    (version == 1) ? put_be64(pb, maxTrackLen) : put_be32(pb, maxTrackLen); 



    put_be32(pb, 0x00010000); 

    put_be16(pb, 0x0100); 

    put_be16(pb, 0); 

    put_be32(pb, 0); 

    put_be32(pb, 0); 



    

    put_be32(pb, 0x00010000); 

    put_be32(pb, 0x0); 

    put_be32(pb, 0x0); 

    put_be32(pb, 0x0); 

    put_be32(pb, 0x00010000); 

    put_be32(pb, 0x0); 

    put_be32(pb, 0x0); 

    put_be32(pb, 0x0); 

    put_be32(pb, 0x40000000); 



    put_be32(pb, 0); 

    put_be32(pb, 0); 

    put_be32(pb, 0); 

    put_be32(pb, 0); 

    put_be32(pb, 0); 

    put_be32(pb, 0); 

    put_be32(pb, maxTrackID+1); 

    return 0x6c;

}