static void bt_hci_done(struct HCIInfo *info)

{

    struct bt_hci_s *hci = hci_from_info(info);

    int handle;



    bt_device_done(&hci->device);



    if (hci->device.lmp_name)

        g_free((void *) hci->device.lmp_name);



    

    if (hci->conn_req_host) {

        bt_hci_connection_reject(hci,

                                 hci->conn_req_host, HCI_OE_POWER_OFF);

        return;

    }



    for (handle = HCI_HANDLE_OFFSET;

                    handle < (HCI_HANDLE_OFFSET | HCI_HANDLES_MAX); handle ++)

        if (!bt_hci_handle_bad(hci, handle))

            bt_hci_disconnect(hci, handle, HCI_OE_POWER_OFF);



    



    timer_free(hci->lm.inquiry_done);

    timer_free(hci->lm.inquiry_next);

    timer_free(hci->conn_accept_timer);



    g_free(hci);

}