static void mxf_write_random_index_pack(AVFormatContext *s)

{

    MXFContext *mxf = s->priv_data;

    AVIOContext *pb = s->pb;

    uint64_t pos = avio_tell(pb);

    int i;



    avio_write(pb, random_index_pack_key, 16);

    klv_encode_ber_length(pb, 28 + 12*mxf->body_partitions_count);



    if (mxf->edit_unit_byte_count)

        avio_wb32(pb, 1); 

    else

        avio_wb32(pb, 0);

    avio_wb64(pb, 0); 



    for (i = 0; i < mxf->body_partitions_count; i++) {

        avio_wb32(pb, 1); 

        avio_wb64(pb, mxf->body_partition_offset[i]);

    }



    avio_wb32(pb, 0); 

    avio_wb64(pb, mxf->footer_partition_offset);



    avio_wb32(pb, avio_tell(pb) - pos + 4);

}