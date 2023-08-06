static int mov_write_gmhd_tag(AVIOContext *pb)

{

    avio_wb32(pb, 0x20);   

    ffio_wfourcc(pb, "gmhd");

    avio_wb32(pb, 0x18);   

    ffio_wfourcc(pb, "gmin");

    avio_wb32(pb, 0);      

    avio_wb16(pb, 0x40);   

    avio_wb16(pb, 0x8000); 

    avio_wb16(pb, 0x8000); 

    avio_wb16(pb, 0x8000); 

    avio_wb16(pb, 0);      

    avio_wb16(pb, 0);      

    return 0x20;

}