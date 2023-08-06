int qemu_can_send_packet(VLANClientState *sender)

{

    VLANState *vlan = sender->vlan;

    VLANClientState *vc;



    for (vc = vlan->first_client; vc != NULL; vc = vc->next) {

        if (vc == sender) {

            continue;

        }



        

        if (!vc->can_receive || vc->can_receive(vc->opaque)) {

            return 1;

        }

    }

    return 0;

}