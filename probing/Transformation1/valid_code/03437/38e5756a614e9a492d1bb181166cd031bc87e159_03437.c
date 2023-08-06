vnc_display_setup_auth(VncDisplay *vs,

                       bool password,

                       bool sasl,

                       bool websocket,

                       Error **errp)

{

    

    if (password) {

        if (vs->tlscreds) {

            vs->auth = VNC_AUTH_VENCRYPT;

            if (websocket) {

                vs->ws_tls = true;

            }

            if (object_dynamic_cast(OBJECT(vs->tlscreds),

                                    TYPE_QCRYPTO_TLS_CREDS_X509)) {

                VNC_DEBUG("Initializing VNC server with x509 password auth\n");

                vs->subauth = VNC_AUTH_VENCRYPT_X509VNC;

            } else if (object_dynamic_cast(OBJECT(vs->tlscreds),

                                           TYPE_QCRYPTO_TLS_CREDS_ANON)) {

                VNC_DEBUG("Initializing VNC server with TLS password auth\n");

                vs->subauth = VNC_AUTH_VENCRYPT_TLSVNC;

            } else {

                error_setg(errp,

                           "Unsupported TLS cred type %s",

                           object_get_typename(OBJECT(vs->tlscreds)));

                return -1;

            }

        } else {

            VNC_DEBUG("Initializing VNC server with password auth\n");

            vs->auth = VNC_AUTH_VNC;

            vs->subauth = VNC_AUTH_INVALID;

        }

        if (websocket) {

            vs->ws_auth = VNC_AUTH_VNC;

        } else {

            vs->ws_auth = VNC_AUTH_INVALID;

        }

    } else if (sasl) {

        if (vs->tlscreds) {

            vs->auth = VNC_AUTH_VENCRYPT;

            if (websocket) {

                vs->ws_tls = true;

            }

            if (object_dynamic_cast(OBJECT(vs->tlscreds),

                                    TYPE_QCRYPTO_TLS_CREDS_X509)) {

                VNC_DEBUG("Initializing VNC server with x509 SASL auth\n");

                vs->subauth = VNC_AUTH_VENCRYPT_X509SASL;

            } else if (object_dynamic_cast(OBJECT(vs->tlscreds),

                                           TYPE_QCRYPTO_TLS_CREDS_ANON)) {

                VNC_DEBUG("Initializing VNC server with TLS SASL auth\n");

                vs->subauth = VNC_AUTH_VENCRYPT_TLSSASL;

            } else {

                error_setg(errp,

                           "Unsupported TLS cred type %s",

                           object_get_typename(OBJECT(vs->tlscreds)));

                return -1;

            }

        } else {

            VNC_DEBUG("Initializing VNC server with SASL auth\n");

            vs->auth = VNC_AUTH_SASL;

            vs->subauth = VNC_AUTH_INVALID;

        }

        if (websocket) {

            vs->ws_auth = VNC_AUTH_SASL;

        } else {

            vs->ws_auth = VNC_AUTH_INVALID;

        }

    } else {

        if (vs->tlscreds) {

            vs->auth = VNC_AUTH_VENCRYPT;

            if (websocket) {

                vs->ws_tls = true;

            }

            if (object_dynamic_cast(OBJECT(vs->tlscreds),

                                    TYPE_QCRYPTO_TLS_CREDS_X509)) {

                VNC_DEBUG("Initializing VNC server with x509 no auth\n");

                vs->subauth = VNC_AUTH_VENCRYPT_X509NONE;

            } else if (object_dynamic_cast(OBJECT(vs->tlscreds),

                                           TYPE_QCRYPTO_TLS_CREDS_ANON)) {

                VNC_DEBUG("Initializing VNC server with TLS no auth\n");

                vs->subauth = VNC_AUTH_VENCRYPT_TLSNONE;

            } else {

                error_setg(errp,

                           "Unsupported TLS cred type %s",

                           object_get_typename(OBJECT(vs->tlscreds)));

                return -1;

            }

        } else {

            VNC_DEBUG("Initializing VNC server with no auth\n");

            vs->auth = VNC_AUTH_NONE;

            vs->subauth = VNC_AUTH_INVALID;

        }

        if (websocket) {

            vs->ws_auth = VNC_AUTH_NONE;

        } else {

            vs->ws_auth = VNC_AUTH_INVALID;

        }

    }

    return 0;

}