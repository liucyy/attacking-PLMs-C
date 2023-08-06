void ff_ivi_recompose_haar(const IVIPlaneDesc *plane, uint8_t *dst,

                           const int dst_pitch, const int num_bands)

{

    int             x, y, indx, b0, b1, b2, b3, p0, p1, p2, p3;

    const IDWTELEM *b0_ptr, *b1_ptr, *b2_ptr, *b3_ptr;

    int32_t         pitch;



    

    pitch = plane->bands[0].pitch;



    

    b0_ptr = plane->bands[0].buf;

    b1_ptr = plane->bands[1].buf;

    b2_ptr = plane->bands[2].buf;

    b3_ptr = plane->bands[3].buf;



    for (y = 0; y < plane->height; y += 2) {

        for (x = 0, indx = 0; x < plane->width; x += 2, indx++) {

            

            b0 = b0_ptr[indx]; 

            b1 = b1_ptr[indx]; 

            b2 = b2_ptr[indx]; 

            b3 = b3_ptr[indx]; 



            

            p0 = (b0 + b1 + b2 + b3 + 2) >> 2;

            p1 = (b0 + b1 - b2 - b3 + 2) >> 2;

            p2 = (b0 - b1 + b2 - b3 + 2) >> 2;

            p3 = (b0 - b1 - b2 + b3 + 2) >> 2;



            

            dst[x]                 = av_clip_uint8(p0 + 128);

            dst[x + 1]             = av_clip_uint8(p1 + 128);

            dst[dst_pitch + x]     = av_clip_uint8(p2 + 128);

            dst[dst_pitch + x + 1] = av_clip_uint8(p3 + 128);

        }



        dst += dst_pitch << 1;



        b0_ptr += pitch;

        b1_ptr += pitch;

        b2_ptr += pitch;

        b3_ptr += pitch;

    }

}