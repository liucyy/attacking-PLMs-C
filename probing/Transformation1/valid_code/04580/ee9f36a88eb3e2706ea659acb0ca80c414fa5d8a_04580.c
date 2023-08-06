static int crc_write_header(struct AVFormatContext *s)

{

    CRCState *crc = s->priv_data;



    

    crc->crcval = adler32(0, NULL, 0);



    return 0;

}