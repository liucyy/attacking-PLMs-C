static int vnc_auth_sasl_check_ssf(VncState *vs)

{

    const void *val;

    int err, ssf;



    if (!vs->sasl.wantSSF)

        return 1;



    err = sasl_getprop(vs->sasl.conn, SASL_SSF, &val);

    if (err != SASL_OK)

        return 0;



    ssf = *(const int *)val;

    VNC_DEBUG("negotiated an SSF of %d\n", ssf);

    if (ssf < 56)

        return 0; 



    

    vs->sasl.runSSF = 1;



    

    return 1;

}