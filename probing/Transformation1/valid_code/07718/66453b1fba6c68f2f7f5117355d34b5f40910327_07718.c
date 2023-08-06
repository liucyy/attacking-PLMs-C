static void mov_parse_stsd_video(MOVContext *c, AVIOContext *pb,

                                 AVStream *st, MOVStreamContext *sc)

{

    uint8_t codec_name[32];

    int64_t stsd_start;

    unsigned int len;



    

    stsd_start = avio_tell(pb) - 16;



    avio_rb16(pb); 

    avio_rb16(pb); 

    avio_rb32(pb); 

    avio_rb32(pb); 

    avio_rb32(pb); 



    st->codecpar->width  = avio_rb16(pb); 

    st->codecpar->height = avio_rb16(pb); 



    avio_rb32(pb); 

    avio_rb32(pb); 

    avio_rb32(pb); 

    avio_rb16(pb); 



    len = avio_r8(pb); 

    if (len > 31)

        len = 31;

    mov_read_mac_string(c, pb, len, codec_name, sizeof(codec_name));

    if (len < 31)

        avio_skip(pb, 31 - len);



    if (codec_name[0])

        av_dict_set(&st->metadata, "encoder", codec_name, 0);



    

    if (!memcmp(codec_name, "Planar Y'CbCr 8-bit 4:2:0", 25)) {

        st->codecpar->codec_tag = MKTAG('I', '4', '2', '0');

        st->codecpar->width &= ~1;

        st->codecpar->height &= ~1;

    }

    

    if (st->codecpar->codec_tag == MKTAG('H','2','6','3') &&

        !memcmp(codec_name, "Sorenson H263", 13))

        st->codecpar->codec_id = AV_CODEC_ID_FLV1;



    st->codecpar->bits_per_coded_sample = avio_rb16(pb); 



    avio_seek(pb, stsd_start, SEEK_SET);



    if (ff_get_qtpalette(st->codecpar->codec_id, pb, sc->palette)) {

        st->codecpar->bits_per_coded_sample &= 0x1F;

        sc->has_palette = 1;

    }

}