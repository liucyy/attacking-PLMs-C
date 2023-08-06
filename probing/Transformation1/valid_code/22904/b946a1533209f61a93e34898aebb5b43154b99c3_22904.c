static void usb_net_handle_destroy(USBDevice *dev)

{

    USBNetState *s = (USBNetState *) dev;



    

    qemu_del_vlan_client(s->vc);

    rndis_clear_responsequeue(s);

    qemu_free(s);

}