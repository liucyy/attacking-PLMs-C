static int mov_write_mvhd_tag(AVIOContext *pb, MOVMuxContext *mov)

{

    int max_track_id = 1, i;

    int64_t max_track_len_temp, max_track_len = 0;

    int version;



    for (i = 0; i < mov->nb_streams; i++) {

        if (mov->tracks[i].entry > 0) {

            max_track_len_temp = av_rescale_rnd(mov->tracks[i].track_duration,

                                                MOV_TIMESCALE,

                                                mov->tracks[i].timescale,

                                                AV_ROUND_UP);

            if (max_track_len < max_track_len_temp)

                max_track_len = max_track_len_temp;

            if (max_track_id < mov->tracks[i].track_id)

                max_track_id = mov->tracks[i].track_id;

        }

    }



    version = max_track_len < UINT32_MAX ? 0 : 1;

    (version == 1) ? avio_wb32(pb, 120) : avio_wb32(pb, 108); 

    ffio_wfourcc(pb, "mvhd");

    avio_w8(pb, version);

    avio_wb24(pb, 0); 

    if (version == 1) {

        avio_wb64(pb, mov->time);

        avio_wb64(pb, mov->time);

    } else {

        avio_wb32(pb, mov->time); 

        avio_wb32(pb, mov->time); 

    }

    avio_wb32(pb, MOV_TIMESCALE);

    (version == 1) ? avio_wb64(pb, max_track_len) : avio_wb32(pb, max_track_len); 



    avio_wb32(pb, 0x00010000); 

    avio_wb16(pb, 0x0100); 

    avio_wb16(pb, 0); 

    avio_wb32(pb, 0); 

    avio_wb32(pb, 0); 



    

    avio_wb32(pb, 0x00010000); 

    avio_wb32(pb, 0x0); 

    avio_wb32(pb, 0x0); 

    avio_wb32(pb, 0x0); 

    avio_wb32(pb, 0x00010000); 

    avio_wb32(pb, 0x0); 

    avio_wb32(pb, 0x0); 

    avio_wb32(pb, 0x0); 

    avio_wb32(pb, 0x40000000); 



    avio_wb32(pb, 0); 

    avio_wb32(pb, 0); 

    avio_wb32(pb, 0); 

    avio_wb32(pb, 0); 

    avio_wb32(pb, 0); 

    avio_wb32(pb, 0); 

    avio_wb32(pb, max_track_id + 1); 

    return 0x6c;

}