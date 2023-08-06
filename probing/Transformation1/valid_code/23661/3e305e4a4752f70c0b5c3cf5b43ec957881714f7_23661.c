void start_auth_sasl(VncState *vs)

{

    const char *mechlist = NULL;

    sasl_security_properties_t secprops;

    int err;

    char *localAddr, *remoteAddr;

    int mechlistlen;



    VNC_DEBUG("Initialize SASL auth %d\n", vs->csock);



    

    if (!(localAddr = vnc_socket_local_addr("%s;%s", vs->csock)))

        goto authabort;



    if (!(remoteAddr = vnc_socket_remote_addr("%s;%s", vs->csock))) {

        g_free(localAddr);

        goto authabort;

    }



    err = sasl_server_new("vnc",

                          NULL, 

                          NULL, 

                          localAddr,

                          remoteAddr,

                          NULL, 

                          SASL_SUCCESS_DATA,

                          &vs->sasl.conn);

    g_free(localAddr);

    g_free(remoteAddr);

    localAddr = remoteAddr = NULL;



    if (err != SASL_OK) {

        VNC_DEBUG("sasl context setup failed %d (%s)",

                  err, sasl_errstring(err, NULL, NULL));

        vs->sasl.conn = NULL;

        goto authabort;

    }



#ifdef CONFIG_VNC_TLS

    

    if (vs->auth == VNC_AUTH_VENCRYPT &&

        vs->subauth == VNC_AUTH_VENCRYPT_X509SASL) {

        gnutls_cipher_algorithm_t cipher;

        sasl_ssf_t ssf;



        cipher = gnutls_cipher_get(vs->tls.session);

        if (!(ssf = (sasl_ssf_t)gnutls_cipher_get_key_size(cipher))) {

            VNC_DEBUG("%s", "cannot TLS get cipher size\n");

            sasl_dispose(&vs->sasl.conn);

            vs->sasl.conn = NULL;

            goto authabort;

        }

        ssf *= 8; 



        err = sasl_setprop(vs->sasl.conn, SASL_SSF_EXTERNAL, &ssf);

        if (err != SASL_OK) {

            VNC_DEBUG("cannot set SASL external SSF %d (%s)\n",

                      err, sasl_errstring(err, NULL, NULL));

            sasl_dispose(&vs->sasl.conn);

            vs->sasl.conn = NULL;

            goto authabort;

        }

    } else

#endif 

        vs->sasl.wantSSF = 1;



    memset (&secprops, 0, sizeof secprops);

    

    if (vs->vd->is_unix

#ifdef CONFIG_VNC_TLS

        

        || (vs->auth == VNC_AUTH_VENCRYPT &&

            vs->subauth == VNC_AUTH_VENCRYPT_X509SASL)

#endif 

        ) {

        

        secprops.min_ssf = 0;

        secprops.max_ssf = 0;

        secprops.maxbufsize = 8192;

        secprops.security_flags = 0;

    } else {

        

        secprops.min_ssf = 56; 

        secprops.max_ssf = 100000; 

        secprops.maxbufsize = 8192;

        

        secprops.security_flags =

            SASL_SEC_NOANONYMOUS | SASL_SEC_NOPLAINTEXT;

    }



    err = sasl_setprop(vs->sasl.conn, SASL_SEC_PROPS, &secprops);

    if (err != SASL_OK) {

        VNC_DEBUG("cannot set SASL security props %d (%s)\n",

                  err, sasl_errstring(err, NULL, NULL));

        sasl_dispose(&vs->sasl.conn);

        vs->sasl.conn = NULL;

        goto authabort;

    }



    err = sasl_listmech(vs->sasl.conn,

                        NULL, 

                        "", 

                        ",", 

                        "", 

                        &mechlist,

                        NULL,

                        NULL);

    if (err != SASL_OK) {

        VNC_DEBUG("cannot list SASL mechanisms %d (%s)\n",

                  err, sasl_errdetail(vs->sasl.conn));

        sasl_dispose(&vs->sasl.conn);

        vs->sasl.conn = NULL;

        goto authabort;

    }

    VNC_DEBUG("Available mechanisms for client: '%s'\n", mechlist);



    vs->sasl.mechlist = g_strdup(mechlist);

    mechlistlen = strlen(mechlist);

    vnc_write_u32(vs, mechlistlen);

    vnc_write(vs, mechlist, mechlistlen);

    vnc_flush(vs);



    VNC_DEBUG("Wait for client mechname length\n");

    vnc_read_when(vs, protocol_client_auth_sasl_mechname_len, 4);



    return;



 authabort:

    vnc_client_error(vs);

}