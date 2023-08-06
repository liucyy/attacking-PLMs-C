static int mov_write_avid_tag(AVIOContext *pb, MOVTrack *track)

{

    int i;

    avio_wb32(pb, 24); 

    ffio_wfourcc(pb, "ACLR");

    ffio_wfourcc(pb, "ACLR");

    ffio_wfourcc(pb, "0001");

    if (track->enc->color_range == AVCOL_RANGE_MPEG || 

        track->enc->color_range == AVCOL_RANGE_UNSPECIFIED) {

        avio_wb32(pb, 1); 

    } else { 

        avio_wb32(pb, 2); 

    }

    avio_wb32(pb, 0); 



    avio_wb32(pb, 24); 

    ffio_wfourcc(pb, "APRG");

    ffio_wfourcc(pb, "APRG");

    ffio_wfourcc(pb, "0001");

    avio_wb32(pb, 1); 

    avio_wb32(pb, 0); 



    avio_wb32(pb, 120); 

    ffio_wfourcc(pb, "ARES");

    ffio_wfourcc(pb, "ARES");

    ffio_wfourcc(pb, "0001");

    avio_wb32(pb, AV_RB32(track->vos_data + 0x28)); 

    avio_wb32(pb, track->enc->width);

    

    if (track->vos_data[5] & 2) { 

        avio_wb32(pb, track->enc->height / 2);

        avio_wb32(pb, 2); 

        avio_wb32(pb, 0); 

        avio_wb32(pb, 4); 

    } else {

        avio_wb32(pb, track->enc->height);

        avio_wb32(pb, 1); 

        avio_wb32(pb, 0); 

        if (track->enc->height == 1080)

            avio_wb32(pb, 5); 

        else

            avio_wb32(pb, 6); 

    }

    

    for (i = 0; i < 10; i++)

        avio_wb64(pb, 0);



    

    avio_wb32(pb, 0);

    return 0;

}