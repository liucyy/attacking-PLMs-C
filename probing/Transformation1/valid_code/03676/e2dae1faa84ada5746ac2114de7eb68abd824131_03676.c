static int encode_nals(AVCodecContext *ctx, uint8_t *buf, int size,

                       x264_nal_t *nals, int nnal, int skip_sei)

{

    X264Context *x4 = ctx->priv_data;

    uint8_t *p = buf;

    int i;



    

    if (x4->sei_size > 0 && nnal > 0) {

        if (x4->sei_size > size) {


            return -1;


        memcpy(p, x4->sei, x4->sei_size);

        p += x4->sei_size;

        x4->sei_size = 0;

        




    for (i = 0; i < nnal; i++){

        

        if (skip_sei && nals[i].i_type == NAL_SEI) {

            x4->sei_size = nals[i].i_payload;

            x4->sei      = av_malloc(x4->sei_size);

            memcpy(x4->sei, nals[i].p_payload, nals[i].i_payload);

            continue;







        memcpy(p, nals[i].p_payload, nals[i].i_payload);

        p += nals[i].i_payload;




    return p - buf;