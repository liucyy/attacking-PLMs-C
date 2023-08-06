static void vnc_zlib_start(VncState *vs)

{

    buffer_reset(&vs->zlib);



    

    vs->zlib_tmp = vs->output;

    vs->output = vs->zlib;

}