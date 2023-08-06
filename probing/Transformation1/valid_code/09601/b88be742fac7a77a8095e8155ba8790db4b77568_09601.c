static void encode_frame(VC2EncContext *s, const AVFrame *frame,

                         const char *aux_data, int field)

{

    int i;



    

    encode_parse_info(s, DIRAC_PCODE_SEQ_HEADER);

    encode_seq_header(s);



    

    if (aux_data) {

        encode_parse_info(s, DIRAC_PCODE_AUX);

        avpriv_put_string(&s->pb, aux_data, 1);

    }



    

    encode_parse_info(s, DIRAC_PCODE_PICTURE_HQ);

    encode_picture_start(s);



    for (i = 0; i < 3; i++) {

        s->transform_args[i].ctx   = s;

        s->transform_args[i].field = field;

        s->transform_args[i].plane = &s->plane[i];

        s->transform_args[i].idata = frame->data[i];

        s->transform_args[i].istride = frame->linesize[i];

    }



    

    s->avctx->execute(s->avctx, dwt_plane, s->transform_args, NULL, 3,

                      sizeof(TransformArgs));



    

    calc_slice_sizes(s);



    

    encode_slices(s);



    

    encode_parse_info(s, DIRAC_PCODE_END_SEQ);

}