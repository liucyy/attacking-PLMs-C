static av_cold int vqa_decode_init(AVCodecContext *avctx)

{

    VqaContext *s = avctx->priv_data;

    unsigned char *vqa_header;

    int i, j, codebook_index;



    s->avctx = avctx;

    avctx->pix_fmt = PIX_FMT_PAL8;



    

    if (s->avctx->extradata_size != VQA_HEADER_SIZE) {

        av_log(s->avctx, AV_LOG_ERROR, "  VQA video: expected extradata size of %d\n", VQA_HEADER_SIZE);

        return -1;

    }



    

    vqa_header = (unsigned char *)s->avctx->extradata;

    s->vqa_version = vqa_header[0];

    s->width = AV_RL16(&vqa_header[6]);

    s->height = AV_RL16(&vqa_header[8]);

    if(av_image_check_size(s->width, s->height, 0, avctx)){

        s->width= s->height= 0;

        return -1;

    }

    s->vector_width = vqa_header[10];

    s->vector_height = vqa_header[11];

    s->partial_count = s->partial_countdown = vqa_header[13];



    

    if ((s->vector_width != 4) ||

        ((s->vector_height != 2) && (s->vector_height != 4))) {

        

        return -1;

    }



    

    s->codebook_size = MAX_CODEBOOK_SIZE;

    s->codebook = av_malloc(s->codebook_size);

    if (!s->codebook)

        goto fail;

    s->next_codebook_buffer = av_malloc(s->codebook_size);

    if (!s->next_codebook_buffer)

        goto fail;



    

    s->decode_buffer_size = (s->width / s->vector_width) *

        (s->height / s->vector_height) * 2;

    s->decode_buffer = av_malloc(s->decode_buffer_size);

    if (!s->decode_buffer)

        goto fail;



    

    if (s->vector_height == 4) {

        codebook_index = 0xFF00 * 16;

        for (i = 0; i < 256; i++)

            for (j = 0; j < 16; j++)

                s->codebook[codebook_index++] = i;

    } else {

        codebook_index = 0xF00 * 8;

        for (i = 0; i < 256; i++)

            for (j = 0; j < 8; j++)

                s->codebook[codebook_index++] = i;

    }

    s->next_codebook_buffer_index = 0;



    s->frame.data[0] = NULL;



    return 0;

fail:

    av_freep(&s->codebook);

    av_freep(&s->next_codebook_buffer);

    av_freep(&s->decode_buffer);

    return AVERROR(ENOMEM);

}