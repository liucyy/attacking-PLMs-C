static int bt_hid_out(struct bt_hid_device_s *s)

{

    USBPacket p;



    if (s->data_type == BT_DATA_OUTPUT) {

        p.pid = USB_TOKEN_OUT;

        p.devep = 1;

        p.data = s->dataout.buffer;

        p.len = s->dataout.len;

        s->dataout.len = s->usbdev->info->handle_data(s->usbdev, &p);



        return s->dataout.len;

    }



    if (s->data_type == BT_DATA_FEATURE) {

        

        p.devep = 0;

    }



    return -1;

}