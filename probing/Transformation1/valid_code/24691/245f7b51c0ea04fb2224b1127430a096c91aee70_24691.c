static void vnc_tight_start(VncState *vs)

{

    buffer_reset(&vs->tight);



    

    vs->tight_tmp = vs->output;

    vs->output = vs->tight;

}