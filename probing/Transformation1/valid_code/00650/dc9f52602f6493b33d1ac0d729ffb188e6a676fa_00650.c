static int decode_end(AVCodecContext *avctx)

{

    H264Context *h = avctx->priv_data;

    MpegEncContext *s = &h->s;

    


    free_tables(h); 

    MPV_common_end(s);





        

    return 0;

}