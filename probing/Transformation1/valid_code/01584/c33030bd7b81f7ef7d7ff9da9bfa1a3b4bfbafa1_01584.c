static int gif_image_write_header(uint8_t **bytestream,

                                  int width, int height, int loop_count,

                                  uint32_t *palette)

{

    int i;

    unsigned int v;



    bytestream_put_buffer(bytestream, "GIF", 3);

    bytestream_put_buffer(bytestream, "89a", 3);

    bytestream_put_le16(bytestream, width);

    bytestream_put_le16(bytestream, height);



    bytestream_put_byte(bytestream, 0xf7); 

    bytestream_put_byte(bytestream, 0x1f); 

    bytestream_put_byte(bytestream, 0); 



    

    if (!palette) {

        bytestream_put_buffer(bytestream, (const unsigned char *)gif_clut, 216*3);

        for(i=0;i<((256-216)*3);i++)

            bytestream_put_byte(bytestream, 0);

    } else {

        for(i=0;i<256;i++) {

            v = palette[i];

            bytestream_put_be24(bytestream, v);

        }

    }



        



    

#ifdef GIF_ADD_APP_HEADER

    if (loop_count >= 0 && loop_count <= 65535) {

        bytestream_put_byte(bytestream, 0x21);

        bytestream_put_byte(bytestream, 0xff);

        bytestream_put_byte(bytestream, 0x0b);

        bytestream_put_buffer(bytestream, "NETSCAPE2.0", 11);  

        bytestream_put_byte(bytestream, 0x03); 

        bytestream_put_byte(bytestream, 0x01); 

        bytestream_put_le16(bytestream, (uint16_t)loop_count);

        bytestream_put_byte(bytestream, 0x00); 

    }

#endif

    return 0;

}