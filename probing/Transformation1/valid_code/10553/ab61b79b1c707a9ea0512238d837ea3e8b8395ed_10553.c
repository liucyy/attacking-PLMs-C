static int mov_read_mvhd(MOVContext *c, AVIOContext *pb, MOVAtom atom)

{

    int i;

    int64_t creation_time;

    int version = avio_r8(pb); 

    avio_rb24(pb); 



    if (version == 1) {

        creation_time = avio_rb64(pb);

        avio_rb64(pb);

    } else {

        creation_time = avio_rb32(pb);

        avio_rb32(pb); 

    }

    mov_metadata_creation_time(&c->fc->metadata, creation_time);

    c->time_scale = avio_rb32(pb); 

    if (c->time_scale <= 0) {

        av_log(c->fc, AV_LOG_ERROR, "Invalid mvhd time scale %d\n", c->time_scale);

        return AVERROR_INVALIDDATA;

    }

    av_log(c->fc, AV_LOG_TRACE, "time scale = %i\n", c->time_scale);



    c->duration = (version == 1) ? avio_rb64(pb) : avio_rb32(pb); 

    

    

    if (c->time_scale > 0 && !c->trex_data)

        c->fc->duration = av_rescale(c->duration, AV_TIME_BASE, c->time_scale);

    avio_rb32(pb); 



    avio_rb16(pb); 



    avio_skip(pb, 10); 



    

    for (i = 0; i < 3; i++) {

        c->movie_display_matrix[i][0] = avio_rb32(pb); 

        c->movie_display_matrix[i][1] = avio_rb32(pb); 

        c->movie_display_matrix[i][2] = avio_rb32(pb); 

    }



    avio_rb32(pb); 

    avio_rb32(pb); 

    avio_rb32(pb); 

    avio_rb32(pb); 

    avio_rb32(pb); 

    avio_rb32(pb); 

    avio_rb32(pb); 



    return 0;

}