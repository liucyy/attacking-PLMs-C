static int decode_plane(Indeo3DecodeContext *ctx, AVCodecContext *avctx,

                        Plane *plane, const uint8_t *data, int32_t data_size,

                        int32_t strip_width)

{

    Cell            curr_cell;

    int             num_vectors;



    

    

    num_vectors = bytestream_get_le32(&data);

    ctx->mc_vectors  = num_vectors ? data : 0;



    

    init_get_bits(&ctx->gb, &data[num_vectors * 2], data_size << 3);

    ctx->skip_bits   = 0;

    ctx->need_resync = 0;



    ctx->last_byte = data + data_size - 1;



    

    curr_cell.xpos   = curr_cell.ypos = 0;

    curr_cell.width  = plane->width  >> 2;

    curr_cell.height = plane->height >> 2;

    curr_cell.tree   = 0; 

    curr_cell.mv_ptr = 0; 



    return parse_bintree(ctx, avctx, plane, INTRA_NULL, &curr_cell, CELL_STACK_MAX, strip_width);

}