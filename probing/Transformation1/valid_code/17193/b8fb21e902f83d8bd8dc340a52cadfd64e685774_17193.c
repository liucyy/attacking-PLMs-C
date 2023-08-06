static int flashsv_decode_frame(AVCodecContext *avctx,

                                    void *data, int *data_size,

                                    AVPacket *avpkt)

{

    const uint8_t *buf = avpkt->data;

    int buf_size = avpkt->size;

    FlashSVContext *s = avctx->priv_data;

    int h_blocks, v_blocks, h_part, v_part, i, j;

    GetBitContext gb;



    

    if (buf_size == 0)

        return 0;





    init_get_bits(&gb, buf, buf_size * 8);



    

    s->block_width = 16* (get_bits(&gb, 4)+1);

    s->image_width =     get_bits(&gb,12);

    s->block_height= 16* (get_bits(&gb, 4)+1);

    s->image_height=     get_bits(&gb,12);



    

    h_blocks = s->image_width / s->block_width;

    h_part = s->image_width % s->block_width;

    v_blocks = s->image_height / s->block_height;

    v_part = s->image_height % s->block_height;



    

    if(s->block_size < s->block_width*s->block_height) {

        if (s->tmpblock != NULL)

            av_free(s->tmpblock);

        if ((s->tmpblock = av_malloc(3*s->block_width*s->block_height)) == NULL) {

            av_log(avctx, AV_LOG_ERROR, "Can't allocate decompression buffer.\n");


        }

    }

    s->block_size = s->block_width*s->block_height;



    

    if((avctx->width==0) && (avctx->height==0)){

        avctx->width = s->image_width;

        avctx->height = s->image_height;

    }



    

    if ((avctx->width != s->image_width) || (avctx->height != s->image_height)) {

        av_log(avctx, AV_LOG_ERROR, "Frame width or height differs from first frames!\n");

        av_log(avctx, AV_LOG_ERROR, "fh = %d, fv %d  vs  ch = %d, cv = %d\n",avctx->height,

        avctx->width,s->image_height,s->image_width);


    }



    av_log(avctx, AV_LOG_DEBUG, "image: %dx%d block: %dx%d num: %dx%d part: %dx%d\n",

        s->image_width, s->image_height, s->block_width, s->block_height,

        h_blocks, v_blocks, h_part, v_part);



    s->frame.reference = 1;

    s->frame.buffer_hints = FF_BUFFER_HINTS_VALID | FF_BUFFER_HINTS_PRESERVE | FF_BUFFER_HINTS_REUSABLE;

    if(avctx->reget_buffer(avctx, &s->frame) < 0){

      av_log(avctx, AV_LOG_ERROR, "reget_buffer() failed\n");


    }



    

    for (j = 0; j < v_blocks + (v_part?1:0); j++)

    {



        int hp = j*s->block_height; 

        int hs = (j<v_blocks)?s->block_height:v_part; 





        

        for (i = 0; i < h_blocks + (h_part?1:0); i++)

        {

            int wp = i*s->block_width; 

            int ws = (i<h_blocks)?s->block_width:h_part; 



            

            int size = get_bits(&gb, 16);

            if (8 * size > get_bits_left(&gb)) {

                avctx->release_buffer(avctx, &s->frame);

                s->frame.data[0] = NULL;


            }



            if (size == 0) {

                

            } else {

                

                int ret = inflateReset(&(s->zstream));

                if (ret != Z_OK)

                {

                    av_log(avctx, AV_LOG_ERROR, "error in decompression (reset) of block %dx%d\n", i, j);

                    

                }

                s->zstream.next_in = buf+(get_bits_count(&gb)/8);

                s->zstream.avail_in = size;

                s->zstream.next_out = s->tmpblock;

                s->zstream.avail_out = s->block_size*3;

                ret = inflate(&(s->zstream), Z_FINISH);

                if (ret == Z_DATA_ERROR)

                {

                    av_log(avctx, AV_LOG_ERROR, "Zlib resync occurred\n");

                    inflateSync(&(s->zstream));

                    ret = inflate(&(s->zstream), Z_FINISH);

                }



                if ((ret != Z_OK) && (ret != Z_STREAM_END))

                {

                    av_log(avctx, AV_LOG_ERROR, "error in decompression of block %dx%d: %d\n", i, j, ret);

                    

                }

                copy_region(s->tmpblock, s->frame.data[0], s->image_height-(hp+hs+1), wp, hs, ws, s->frame.linesize[0]);

                skip_bits_long(&gb, 8*size);   

            }

        }

    }



    *data_size = sizeof(AVFrame);

    *(AVFrame*)data = s->frame;



    if ((get_bits_count(&gb)/8) != buf_size)

        av_log(avctx, AV_LOG_ERROR, "buffer not fully consumed (%d != %d)\n",

            buf_size, (get_bits_count(&gb)/8));



    

    return buf_size;

}