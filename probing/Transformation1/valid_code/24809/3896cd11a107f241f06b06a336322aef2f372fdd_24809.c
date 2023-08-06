static void mxf_write_system_item(AVFormatContext *s)

{

    MXFContext *mxf = s->priv_data;

    AVIOContext *pb = s->pb;

    unsigned frame;

    uint32_t time_code;



    frame = mxf->last_indexed_edit_unit + mxf->edit_units_count;



    

    avio_write(pb, system_metadata_pack_key, 16);

    klv_encode_ber4_length(pb, 57);

    avio_w8(pb, 0x5c); 

    avio_w8(pb, 0x04); 

    avio_w8(pb, 0x00); 

    avio_wb16(pb, 0x00); 

    avio_wb16(pb, mxf->tc.start + frame); 

    if (mxf->essence_container_count > 1)

        avio_write(pb, multiple_desc_ul, 16);

    else {

        MXFStreamContext *sc = s->streams[0]->priv_data;

        avio_write(pb, mxf_essence_container_uls[sc->index].container_ul, 16);

    }

    avio_w8(pb, 0);

    avio_wb64(pb, 0);

    avio_wb64(pb, 0); 



    avio_w8(pb, 0x81); 

    time_code = av_timecode_get_smpte_from_framenum(&mxf->tc, frame);

    avio_wb32(pb, time_code);

    avio_wb32(pb, 0); 

    avio_wb64(pb, 0);



    

    avio_write(pb, system_metadata_package_set_key, 16);

    klv_encode_ber4_length(pb, 35);

    avio_w8(pb, 0x83); 

    avio_wb16(pb, 0x20);

    mxf_write_umid(s, 1);

}