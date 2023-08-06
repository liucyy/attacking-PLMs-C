vnc_display_setup_auth(VncDisplay *vs,

                       bool password,

                       bool sasl,

                       bool tls,

                       bool x509)

{

    

    if (password) {

        if (tls) {

            vs->auth = VNC_AUTH_VENCRYPT;

            if (x509) {

                VNC_DEBUG("Initializing VNC server with x509 password auth\n");

                vs->subauth = VNC_AUTH_VENCRYPT_X509VNC;

            } else {

                VNC_DEBUG("Initializing VNC server with TLS password auth\n");

                vs->subauth = VNC_AUTH_VENCRYPT_TLSVNC;

            }

        } else {

            VNC_DEBUG("Initializing VNC server with password auth\n");

            vs->auth = VNC_AUTH_VNC;

            vs->subauth = VNC_AUTH_INVALID;

        }

    } else if (sasl) {

        if (tls) {

            vs->auth = VNC_AUTH_VENCRYPT;

            if (x509) {

                VNC_DEBUG("Initializing VNC server with x509 SASL auth\n");

                vs->subauth = VNC_AUTH_VENCRYPT_X509SASL;

            } else {

                VNC_DEBUG("Initializing VNC server with TLS SASL auth\n");

                vs->subauth = VNC_AUTH_VENCRYPT_TLSSASL;

            }

        } else {

            VNC_DEBUG("Initializing VNC server with SASL auth\n");

            vs->auth = VNC_AUTH_SASL;

            vs->subauth = VNC_AUTH_INVALID;

        }

    } else {

        if (tls) {

            vs->auth = VNC_AUTH_VENCRYPT;

            if (x509) {

                VNC_DEBUG("Initializing VNC server with x509 no auth\n");

                vs->subauth = VNC_AUTH_VENCRYPT_X509NONE;

            } else {

                VNC_DEBUG("Initializing VNC server with TLS no auth\n");

                vs->subauth = VNC_AUTH_VENCRYPT_TLSNONE;

            }

        } else {

            VNC_DEBUG("Initializing VNC server with no auth\n");

            vs->auth = VNC_AUTH_NONE;

            vs->subauth = VNC_AUTH_INVALID;

        }

    }

}