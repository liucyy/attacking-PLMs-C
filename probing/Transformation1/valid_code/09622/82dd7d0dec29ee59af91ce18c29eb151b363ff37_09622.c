static int decode_frame_header (bit_buffer_t *bitbuf,MpegEncContext *s) {

  int frame_size_code;



  

  get_bits (bitbuf, 8);



  

  s->pict_type = get_bits (bitbuf, 2);



  if (s->pict_type == 3)

    return -1;



  if (s->pict_type == SVQ1_FRAME_INTRA) {



    

    if (s->f_code == 0x50 || s->f_code == 0x60) {

      get_bits (bitbuf, 16);

    }



    if ((s->f_code ^ 0x10) >= 0x50) {

      skip_bits(bitbuf,8*get_bits (bitbuf, 8));

    }



    get_bits (bitbuf, 2);

    get_bits (bitbuf, 2);

    get_bits (bitbuf, 1);



    

    frame_size_code = get_bits (bitbuf, 3);



    if (frame_size_code == 7) {

      

      s->width = get_bits (bitbuf, 12);

      s->height = get_bits (bitbuf, 12);



      if (!s->width || !s->height)

        return -1;

    } else {

      

      s->width = frame_size_table[frame_size_code].width;

      s->height = frame_size_table[frame_size_code].height;

    }

  }



  

  if (get_bits (bitbuf, 1) == 1) {

    get_bits (bitbuf, 1);

    get_bits (bitbuf, 1);



    if (get_bits (bitbuf, 2) != 0)

      return -1;

  }



  if (get_bits (bitbuf, 1) == 1) {

    get_bits (bitbuf, 1);

    get_bits (bitbuf, 4);

    get_bits (bitbuf, 1);

    get_bits (bitbuf, 2);



    while (get_bits (bitbuf, 1) == 1) {

      get_bits (bitbuf, 8);

    }

  }

  

  return 0;

}