static int get_packetheader(NUTContext *nut, ByteIOContext *bc, int calculate_checksum)

{

    int64_t start, size;





    size= get_v(bc);



    init_checksum(bc, calculate_checksum ? av_crc04C11DB7_update : NULL, 1);









    return size;

}