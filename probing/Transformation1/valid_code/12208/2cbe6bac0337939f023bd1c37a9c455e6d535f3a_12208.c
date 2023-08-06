static void blend_frames_c(BLEND_FUNC_PARAMS)

{

    int line, pixel;

    for (line = 0; line < height; line++) {

        for (pixel = 0; pixel < width; pixel++) {

            

            

            

            dst[pixel] = ((src1[pixel] * factor1) + (src2[pixel] * factor2) + 128) >> 8;

        }

        src1 += src1_linesize;

        src2 += src2_linesize;

        dst  += dst_linesize;

    }

}