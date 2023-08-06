static int mjpeg_decode_com(MJpegDecodeContext *s)

{

    int i;

    UINT8 *cbuf;



    

    unsigned int len = get_bits(&s->gb, 16)-2;

    cbuf = av_malloc(len+1);



    for (i = 0; i < len; i++)

	cbuf[i] = get_bits(&s->gb, 8);

    if (cbuf[i-1] == '\n')

	cbuf[i-1] = 0;

    else

	cbuf[i] = 0;



    printf("mjpeg comment: '%s'\n", cbuf);



    

    if (!strcmp(cbuf, "AVID"))

    {

	s->buggy_avid = 1;





    }

    

    av_free(cbuf);



    return 0;

}