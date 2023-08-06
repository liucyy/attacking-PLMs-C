static int start_auth_vencrypt(VncState *vs)

{

    

    vnc_write_u8(vs, 0);

    vnc_write_u8(vs, 2);



    vnc_read_when(vs, protocol_client_vencrypt_init, 2);

    return 0;

}