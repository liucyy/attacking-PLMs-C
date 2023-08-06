static void pixel_format_message (VncState *vs) {

    char pad[3] = { 0, 0, 0 };



    vnc_write_u8(vs, vs->depth * 8); 

    if (vs->depth == 4) vnc_write_u8(vs, 24); 

    else vnc_write_u8(vs, vs->depth * 8); 



#ifdef WORDS_BIGENDIAN

    vnc_write_u8(vs, 1);             

#else

    vnc_write_u8(vs, 0);             

#endif

    vnc_write_u8(vs, 1);             

    if (vs->depth == 4) {

        vnc_write_u16(vs, 0xFF);     

        vnc_write_u16(vs, 0xFF);     

        vnc_write_u16(vs, 0xFF);     

        vnc_write_u8(vs, 16);        

        vnc_write_u8(vs, 8);         

        vnc_write_u8(vs, 0);         

        vs->send_hextile_tile = send_hextile_tile_32;

    } else if (vs->depth == 2) {

        vnc_write_u16(vs, 31);       

        vnc_write_u16(vs, 63);       

        vnc_write_u16(vs, 31);       

        vnc_write_u8(vs, 11);        

        vnc_write_u8(vs, 5);         

        vnc_write_u8(vs, 0);         

        vs->send_hextile_tile = send_hextile_tile_16;

    } else if (vs->depth == 1) {

        

        vnc_write_u16(vs, 7);        

        vnc_write_u16(vs, 7);        

        vnc_write_u16(vs, 3);        

        vnc_write_u8(vs, 5);         

        vnc_write_u8(vs, 2);         

        vnc_write_u8(vs, 0);         

        vs->send_hextile_tile = send_hextile_tile_8;

    }

    vs->client_red_max = vs->server_red_max;

    vs->client_green_max = vs->server_green_max;

    vs->client_blue_max = vs->server_blue_max;

    vs->client_red_shift = vs->server_red_shift;

    vs->client_green_shift = vs->server_green_shift;

    vs->client_blue_shift = vs->server_blue_shift;

    vs->pix_bpp = vs->depth * 8;

    vs->write_pixels = vnc_write_pixels_copy;



    vnc_write(vs, pad, 3);           

}