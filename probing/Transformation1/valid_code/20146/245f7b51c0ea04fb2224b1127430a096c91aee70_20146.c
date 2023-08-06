static void vnc_tight_stop(VncState *vs)

{

    

    vs->tight = vs->output;

    vs->output = vs->tight_tmp;

}