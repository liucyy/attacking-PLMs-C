void net_check_clients(void)

{

    VLANState *vlan;

    VLANClientState *vc;

    int i;



    

    if (default_net) {

        return;

    }



    QTAILQ_FOREACH(vlan, &vlans, next) {

        int has_nic = 0, has_host_dev = 0;



        QTAILQ_FOREACH(vc, &vlan->clients, next) {

            switch (vc->info->type) {

            case NET_CLIENT_OPTIONS_KIND_NIC:

                has_nic = 1;

                break;

            case NET_CLIENT_OPTIONS_KIND_USER:

            case NET_CLIENT_OPTIONS_KIND_TAP:

            case NET_CLIENT_OPTIONS_KIND_SOCKET:

            case NET_CLIENT_OPTIONS_KIND_VDE:

                has_host_dev = 1;

                break;

            default: ;

            }

        }

        if (has_host_dev && !has_nic)

            fprintf(stderr, "Warning: vlan %d with no nics\n", vlan->id);

        if (has_nic && !has_host_dev)

            fprintf(stderr,

                    "Warning: vlan %d is not connected to host network\n",

                    vlan->id);

    }

    QTAILQ_FOREACH(vc, &non_vlan_clients, next) {

        if (!vc->peer) {

            fprintf(stderr, "Warning: %s %s has no peer\n",

                    vc->info->type == NET_CLIENT_OPTIONS_KIND_NIC ? "nic" : "netdev",

                    vc->name);

        }

    }



    

    for (i = 0; i < MAX_NICS; i++) {

        NICInfo *nd = &nd_table[i];

        if (nd->used && !nd->instantiated) {

            fprintf(stderr, "Warning: requested NIC (%s, model %s) "

                    "was not created (not supported by this machine?)\n",

                    nd->name ? nd->name : "anonymous",

                    nd->model ? nd->model : "unspecified");

        }

    }

}