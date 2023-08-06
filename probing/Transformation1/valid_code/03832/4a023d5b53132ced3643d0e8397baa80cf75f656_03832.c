static av_cold int check_format(AVCodecContext *avctx)

{

    AVCodecParserContext *parser;

    uint8_t *pout;

    int psize;

    int index;

    H264Context *h;

    int ret = -1;



    

    parser = av_parser_init(avctx->codec->id);

    if (!parser) {

        av_log(avctx, AV_LOG_ERROR, "Failed to open H.264 parser.\n");

        goto final;

    }

    parser->flags = PARSER_FLAG_COMPLETE_FRAMES;

    index = av_parser_parse2(parser, avctx, &pout, &psize, NULL, 0, 0, 0, 0);

    if (index < 0) {

        av_log(avctx, AV_LOG_ERROR, "Failed to parse this file.\n");

        goto release_parser;

    }



    

    h = parser->priv_data;

    switch (h->sps.bit_depth_luma) {

    case 8:

        if (!CHROMA444(h) && !CHROMA422(h)) {

            

            ret = 0;

            break;

        }

    default:

        av_log(avctx, AV_LOG_ERROR, "Unsupported file.\n");

    }



release_parser:

    av_parser_close(parser);



final:

    return ret;

}