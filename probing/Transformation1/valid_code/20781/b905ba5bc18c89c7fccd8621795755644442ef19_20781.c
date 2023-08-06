static int mov_preroll_write_stbl_atoms(AVIOContext *pb, MOVTrack *track)

{

    struct sgpd_entry {

        int count;

        int16_t roll_distance;

        int group_description_index;

    };



    struct sgpd_entry *sgpd_entries = NULL;

    int entries = -1;

    int group = 0;

    int i, j;



    const int OPUS_SEEK_PREROLL_MS = 80;

    int roll_samples = av_rescale_q(OPUS_SEEK_PREROLL_MS,

                                    (AVRational){1, 1000},

                                    (AVRational){1, 48000});



    if (track->entry) {

        sgpd_entries = av_malloc_array(track->entry, sizeof(*sgpd_entries));

        if (!sgpd_entries)

            return AVERROR(ENOMEM);

    }



    av_assert0(track->par->codec_id == AV_CODEC_ID_OPUS);



    for (i = 0; i < track->entry; i++) {

        int roll_samples_remaining = roll_samples;

        int distance = 0;

        for (j = i - 1; j >= 0; j--) {

            roll_samples_remaining -= get_cluster_duration(track, j);

            distance++;

            if (roll_samples_remaining <= 0)

                break;

        }

        

        if (roll_samples_remaining > 0)

            distance = 0;

        

        av_assert0(distance == 0 || (distance >= 2 && distance <= 32));

        if (i && distance == sgpd_entries[entries].roll_distance) {

            sgpd_entries[entries].count++;

        } else {

            entries++;

            sgpd_entries[entries].count = 1;

            sgpd_entries[entries].roll_distance = distance;

            sgpd_entries[entries].group_description_index = distance ? ++group : 0;

        }

    }

    entries++;



    if (!group)

        return 0;



    

    avio_wb32(pb, 24 + (group * 2)); 

    ffio_wfourcc(pb, "sgpd");

    avio_wb32(pb, 1 << 24); 

    ffio_wfourcc(pb, "roll");

    avio_wb32(pb, 2); 

    avio_wb32(pb, group); 

    for (i = 0; i < entries; i++) {

        if (sgpd_entries[i].group_description_index) {

            avio_wb16(pb, -sgpd_entries[i].roll_distance); 

        }

    }



    

    avio_wb32(pb, 20 + (entries * 8)); 

    ffio_wfourcc(pb, "sbgp");

    avio_wb32(pb, 0); 

    ffio_wfourcc(pb, "roll");

    avio_wb32(pb, entries); 

    for (i = 0; i < entries; i++) {

        avio_wb32(pb, sgpd_entries[i].count); 

        avio_wb32(pb, sgpd_entries[i].group_description_index); 

    }



    av_free(sgpd_entries);

    return 0;

}