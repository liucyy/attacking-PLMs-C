altivec_yuv2packedX (SwsContext *c,

		       int16_t *lumFilter, int16_t **lumSrc, int lumFilterSize,

		       int16_t *chrFilter, int16_t **chrSrc, int chrFilterSize,

		       uint8_t *dest, int dstW, int dstY)

{

  int i,j;

  vector signed short X,X0,X1,Y0,U0,V0,Y1,U1,V1,U,V;

  vector signed short R0,G0,B0,R1,G1,B1;



  vector unsigned char R,G,B;

  vector unsigned char *out,*nout;



  vector signed short   RND = vec_splat_s16(1<<3);

  vector unsigned short SCL = vec_splat_u16(4);

  unsigned long scratch[16] __attribute__ ((aligned (16)));



  vector signed short *YCoeffs, *CCoeffs;



  YCoeffs = c->vYCoeffsBank+dstY*lumFilterSize;

  CCoeffs = c->vCCoeffsBank+dstY*chrFilterSize;



  out = (vector unsigned char *)dest;



  for(i=0; i<dstW; i+=16){

    Y0 = RND;

    Y1 = RND;

    

    for(j=0; j<lumFilterSize; j++) {

      X0 = vec_ld (0,  &lumSrc[j][i]);

      X1 = vec_ld (16, &lumSrc[j][i]);

      Y0 = vec_mradds (X0, YCoeffs[j], Y0);

      Y1 = vec_mradds (X1, YCoeffs[j], Y1);

    }



    U = RND;

    V = RND;

    

    for(j=0; j<chrFilterSize; j++) {

      X  = vec_ld (0, &chrSrc[j][i/2]);

      U  = vec_mradds (X, CCoeffs[j], U);

      X  = vec_ld (0, &chrSrc[j][i/2+2048]);

      V  = vec_mradds (X, CCoeffs[j], V);

    }



    

    Y0 = vec_sra (Y0, SCL);

    Y1 = vec_sra (Y1, SCL);

    U  = vec_sra (U,  SCL);

    V  = vec_sra (V,  SCL);



    Y0 = vec_clip_s16 (Y0);

    Y1 = vec_clip_s16 (Y1);

    U  = vec_clip_s16 (U);

    V  = vec_clip_s16 (V);



    



    U0 = vec_mergeh (U,U);

    V0 = vec_mergeh (V,V);



    U1 = vec_mergel (U,U);

    V1 = vec_mergel (V,V);



    cvtyuvtoRGB (c, Y0,U0,V0,&R0,&G0,&B0);

    cvtyuvtoRGB (c, Y1,U1,V1,&R1,&G1,&B1);



    R  = vec_packclp (R0,R1);

    G  = vec_packclp (G0,G1);

    B  = vec_packclp (B0,B1);



    switch(c->dstFormat) {

      case PIX_FMT_ABGR: out_abgr (R,G,B,out); break;

      case PIX_FMT_BGRA: out_bgra (R,G,B,out); break;

      case PIX_FMT_RGBA: out_rgba (R,G,B,out); break;

      case PIX_FMT_ARGB: out_argb (R,G,B,out); break;

      case PIX_FMT_RGB24: out_rgb24 (R,G,B,out); break;

      case PIX_FMT_BGR24: out_bgr24 (R,G,B,out); break;

      default:

        {

          

          static int printed_error_message;

          if(!printed_error_message) {

            av_log(c, AV_LOG_ERROR, "altivec_yuv2packedX doesn't support %s output\n",

                    sws_format_name(c->dstFormat));

            printed_error_message=1;

          }

          return;

        }

    }

  }



  if (i < dstW) {

    i -= 16;



    Y0 = RND;

    Y1 = RND;

    

    for(j=0; j<lumFilterSize; j++) {

      X0 = vec_ld (0,  &lumSrc[j][i]);

      X1 = vec_ld (16, &lumSrc[j][i]);

      Y0 = vec_mradds (X0, YCoeffs[j], Y0);

      Y1 = vec_mradds (X1, YCoeffs[j], Y1);

    }



    U = RND;

    V = RND;

    

    for(j=0; j<chrFilterSize; j++) {

      X  = vec_ld (0, &chrSrc[j][i/2]);

      U  = vec_mradds (X, CCoeffs[j], U);

      X  = vec_ld (0, &chrSrc[j][i/2+2048]);

      V  = vec_mradds (X, CCoeffs[j], V);

    }



    

    Y0 = vec_sra (Y0, SCL);

    Y1 = vec_sra (Y1, SCL);

    U  = vec_sra (U,  SCL);

    V  = vec_sra (V,  SCL);



    Y0 = vec_clip_s16 (Y0);

    Y1 = vec_clip_s16 (Y1);

    U  = vec_clip_s16 (U);

    V  = vec_clip_s16 (V);



    



    U0 = vec_mergeh (U,U);

    V0 = vec_mergeh (V,V);



    U1 = vec_mergel (U,U);

    V1 = vec_mergel (V,V);



    cvtyuvtoRGB (c, Y0,U0,V0,&R0,&G0,&B0);

    cvtyuvtoRGB (c, Y1,U1,V1,&R1,&G1,&B1);



    R  = vec_packclp (R0,R1);

    G  = vec_packclp (G0,G1);

    B  = vec_packclp (B0,B1);



    nout = (vector unsigned char *)scratch;

    switch(c->dstFormat) {

      case PIX_FMT_ABGR: out_abgr (R,G,B,nout); break;

      case PIX_FMT_BGRA: out_bgra (R,G,B,nout); break;

      case PIX_FMT_RGBA: out_rgba (R,G,B,nout); break;

      case PIX_FMT_ARGB: out_argb (R,G,B,nout); break;

      case PIX_FMT_RGB24: out_rgb24 (R,G,B,nout); break;

      case PIX_FMT_BGR24: out_bgr24 (R,G,B,nout); break;

      default:

        

        av_log(c, AV_LOG_ERROR, "altivec_yuv2packedX doesn't support %s output\n",

                sws_format_name(c->dstFormat));

        return;

    }



    memcpy (&((uint32_t*)dest)[i], scratch, (dstW-i)/4);

  }



}