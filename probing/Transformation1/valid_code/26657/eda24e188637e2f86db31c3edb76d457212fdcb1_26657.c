vnc_display_setup_auth(VncDisplay *vd,

                       bool password,

                       bool sasl,

                       bool websocket,

                       Error **errp)

{

    

    if (password) {

        if (vd->tlscreds) {

            vd->auth = VNC_AUTH_VENCRYPT;

            if (object_dynamic_cast(OBJECT(vd->tlscreds),

                                    TYPE_QCRYPTO_TLS_CREDS_X509)) {

                VNC_DEBUG("Initializing VNC server with x509 password auth\n");

                vd->subauth = VNC_AUTH_VENCRYPT_X509VNC;

            } else if (object_dynamic_cast(OBJECT(vd->tlscreds),

                                           TYPE_QCRYPTO_TLS_CREDS_ANON)) {

                VNC_DEBUG("Initializing VNC server with TLS password auth\n");

                vd->subauth = VNC_AUTH_VENCRYPT_TLSVNC;

            } else {

                error_setg(errp,

                           "Unsupported TLS cred type %s",

                           object_get_typename(OBJECT(vd->tlscreds)));

                return -1;

            }

        } else {

            VNC_DEBUG("Initializing VNC server with password auth\n");

            vd->auth = VNC_AUTH_VNC;

            vd->subauth = VNC_AUTH_INVALID;

        }

        if (websocket) {

            vd->ws_auth = VNC_AUTH_VNC;

        } else {

            vd->ws_auth = VNC_AUTH_INVALID;

        }

    } else if (sasl) {

        if (vd->tlscreds) {

            vd->auth = VNC_AUTH_VENCRYPT;

            if (object_dynamic_cast(OBJECT(vd->tlscreds),

                                    TYPE_QCRYPTO_TLS_CREDS_X509)) {

                VNC_DEBUG("Initializing VNC server with x509 SASL auth\n");

                vd->subauth = VNC_AUTH_VENCRYPT_X509SASL;

            } else if (object_dynamic_cast(OBJECT(vd->tlscreds),

                                           TYPE_QCRYPTO_TLS_CREDS_ANON)) {

                VNC_DEBUG("Initializing VNC server with TLS SASL auth\n");

                vd->subauth = VNC_AUTH_VENCRYPT_TLSSASL;

            } else {

                error_setg(errp,

                           "Unsupported TLS cred type %s",

                           object_get_typename(OBJECT(vd->tlscreds)));

                return -1;

            }

        } else {

            VNC_DEBUG("Initializing VNC server with SASL auth\n");

            vd->auth = VNC_AUTH_SASL;

            vd->subauth = VNC_AUTH_INVALID;

        }

        if (websocket) {

            vd->ws_auth = VNC_AUTH_SASL;

        } else {

            vd->ws_auth = VNC_AUTH_INVALID;

        }

    } else {

        if (vd->tlscreds) {

            vd->auth = VNC_AUTH_VENCRYPT;

            if (object_dynamic_cast(OBJECT(vd->tlscreds),

                                    TYPE_QCRYPTO_TLS_CREDS_X509)) {

                VNC_DEBUG("Initializing VNC server with x509 no auth\n");

                vd->subauth = VNC_AUTH_VENCRYPT_X509NONE;

            } else if (object_dynamic_cast(OBJECT(vd->tlscreds),

                                           TYPE_QCRYPTO_TLS_CREDS_ANON)) {

                VNC_DEBUG("Initializing VNC server with TLS no auth\n");

                vd->subauth = VNC_AUTH_VENCRYPT_TLSNONE;

            } else {

                error_setg(errp,

                           "Unsupported TLS cred type %s",

                           object_get_typename(OBJECT(vd->tlscreds)));

                return -1;

            }

        } else {

            VNC_DEBUG("Initializing VNC server with no auth\n");

            vd->auth = VNC_AUTH_NONE;

            vd->subauth = VNC_AUTH_INVALID;

        }

        if (websocket) {

            vd->ws_auth = VNC_AUTH_NONE;

        } else {

            vd->ws_auth = VNC_AUTH_INVALID;

        }

    }

    return 0;

}