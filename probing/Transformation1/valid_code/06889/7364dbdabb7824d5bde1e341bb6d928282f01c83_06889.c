static int protocol_client_auth(VncState *vs, uint8_t *data, size_t len)

{

    

    if (data[0] != vs->auth) { 

       VNC_DEBUG("Reject auth %d because it didn't match advertized\n", (int)data[0]);

       vnc_write_u32(vs, 1);

       if (vs->minor >= 8) {

           static const char err[] = "Authentication failed";

           vnc_write_u32(vs, sizeof(err));

           vnc_write(vs, err, sizeof(err));

       }

       vnc_client_error(vs);

    } else { 

       VNC_DEBUG("Client requested auth %d\n", (int)data[0]);

       switch (vs->auth) {

       case VNC_AUTH_NONE:

           VNC_DEBUG("Accept auth none\n");

           if (vs->minor >= 8) {

               vnc_write_u32(vs, 0); 

               vnc_flush(vs);

           }

           start_client_init(vs);

           break;



       case VNC_AUTH_VNC:

           VNC_DEBUG("Start VNC auth\n");

           start_auth_vnc(vs);

           break;



       case VNC_AUTH_VENCRYPT:

           VNC_DEBUG("Accept VeNCrypt auth\n");

           start_auth_vencrypt(vs);

           break;



#ifdef CONFIG_VNC_SASL

       case VNC_AUTH_SASL:

           VNC_DEBUG("Accept SASL auth\n");

           start_auth_sasl(vs);

           break;

#endif 



       default: 

           VNC_DEBUG("Reject auth %d server code bug\n", vs->auth);

           vnc_write_u8(vs, 1);

           if (vs->minor >= 8) {

               static const char err[] = "Authentication failed";

               vnc_write_u32(vs, sizeof(err));

               vnc_write(vs, err, sizeof(err));

           }

           vnc_client_error(vs);

       }

    }

    return 0;

}