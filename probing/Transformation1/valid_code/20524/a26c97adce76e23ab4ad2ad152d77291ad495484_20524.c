static int protocol_client_auth(VncState *vs, char *data, size_t len)

{

    

    if (data[0] != vs->auth) { 

       VNC_DEBUG("Reject auth %d\n", (int)data[0]);

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

           vnc_write_u32(vs, 0); 

           vnc_read_when(vs, protocol_client_init, 1);

           break;



       case VNC_AUTH_VNC:

           VNC_DEBUG("Start VNC auth\n");

           return start_auth_vnc(vs);



#if CONFIG_VNC_TLS

       case VNC_AUTH_VENCRYPT:

           VNC_DEBUG("Accept VeNCrypt auth\n");;

           return start_auth_vencrypt(vs);

#endif 



       default: 

           VNC_DEBUG("Reject auth %d\n", vs->auth);

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