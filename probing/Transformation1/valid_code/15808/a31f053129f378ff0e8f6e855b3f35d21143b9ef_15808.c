GuestNetworkInterfaceList *qmp_guest_network_get_interfaces(Error **errp)

{

    GuestNetworkInterfaceList *head = NULL, *cur_item = NULL;

    struct ifaddrs *ifap, *ifa;

    char err_msg[512];



    if (getifaddrs(&ifap) < 0) {

        snprintf(err_msg, sizeof(err_msg),

                 "getifaddrs failed: %s", strerror(errno));

        error_set(errp, QERR_QGA_COMMAND_FAILED, err_msg);

        goto error;

    }



    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {

        GuestNetworkInterfaceList *info;

        GuestIpAddressList **address_list = NULL, *address_item = NULL;

        char addr4[INET_ADDRSTRLEN];

        char addr6[INET6_ADDRSTRLEN];

        int sock;

        struct ifreq ifr;

        unsigned char *mac_addr;

        void *p;



        g_debug("Processing %s interface", ifa->ifa_name);



        info = guest_find_interface(head, ifa->ifa_name);



        if (!info) {

            info = g_malloc0(sizeof(*info));

            info->value = g_malloc0(sizeof(*info->value));

            info->value->name = g_strdup(ifa->ifa_name);



            if (!cur_item) {

                head = cur_item = info;

            } else {

                cur_item->next = info;

                cur_item = info;

            }

        }



        if (!info->value->has_hardware_address &&

            ifa->ifa_flags & SIOCGIFHWADDR) {

            

            sock = socket(PF_INET, SOCK_STREAM, 0);

            if (sock == -1) {

                snprintf(err_msg, sizeof(err_msg),

                         "failed to create socket: %s", strerror(errno));

                error_set(errp, QERR_QGA_COMMAND_FAILED, err_msg);

                goto error;

            }



            memset(&ifr, 0, sizeof(ifr));

            strncpy(ifr.ifr_name,  info->value->name, IF_NAMESIZE);

            if (ioctl(sock, SIOCGIFHWADDR, &ifr) == -1) {

                snprintf(err_msg, sizeof(err_msg),

                         "failed to get MAC addres of %s: %s",

                         ifa->ifa_name,

                         strerror(errno));

                error_set(errp, QERR_QGA_COMMAND_FAILED, err_msg);

                goto error;

            }



            mac_addr = (unsigned char *) &ifr.ifr_hwaddr.sa_data;



            if (asprintf(&info->value->hardware_address,

                         "%02x:%02x:%02x:%02x:%02x:%02x",

                         (int) mac_addr[0], (int) mac_addr[1],

                         (int) mac_addr[2], (int) mac_addr[3],

                         (int) mac_addr[4], (int) mac_addr[5]) == -1) {

                snprintf(err_msg, sizeof(err_msg),

                         "failed to format MAC: %s", strerror(errno));

                error_set(errp, QERR_QGA_COMMAND_FAILED, err_msg);

                goto error;

            }



            info->value->has_hardware_address = true;

            close(sock);

        }



        if (ifa->ifa_addr &&

            ifa->ifa_addr->sa_family == AF_INET) {

            

            address_item = g_malloc0(sizeof(*address_item));

            address_item->value = g_malloc0(sizeof(*address_item->value));

            p = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;

            if (!inet_ntop(AF_INET, p, addr4, sizeof(addr4))) {

                snprintf(err_msg, sizeof(err_msg),

                         "inet_ntop failed : %s", strerror(errno));

                error_set(errp, QERR_QGA_COMMAND_FAILED, err_msg);

                goto error;

            }



            address_item->value->ip_address = g_strdup(addr4);

            address_item->value->ip_address_type = GUEST_IP_ADDRESS_TYPE_IPV4;



            if (ifa->ifa_netmask) {

                

                p = &((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr;

                address_item->value->prefix = ctpop32(((uint32_t *) p)[0]);

            }

        } else if (ifa->ifa_addr &&

                   ifa->ifa_addr->sa_family == AF_INET6) {

            

            address_item = g_malloc0(sizeof(*address_item));

            address_item->value = g_malloc0(sizeof(*address_item->value));

            p = &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;

            if (!inet_ntop(AF_INET6, p, addr6, sizeof(addr6))) {

                snprintf(err_msg, sizeof(err_msg),

                         "inet_ntop failed : %s", strerror(errno));

                error_set(errp, QERR_QGA_COMMAND_FAILED, err_msg);

                goto error;

            }



            address_item->value->ip_address = g_strdup(addr6);

            address_item->value->ip_address_type = GUEST_IP_ADDRESS_TYPE_IPV6;



            if (ifa->ifa_netmask) {

                

                p = &((struct sockaddr_in6 *)ifa->ifa_netmask)->sin6_addr;

                address_item->value->prefix =

                    ctpop32(((uint32_t *) p)[0]) +

                    ctpop32(((uint32_t *) p)[1]) +

                    ctpop32(((uint32_t *) p)[2]) +

                    ctpop32(((uint32_t *) p)[3]);

            }

        }



        if (!address_item) {

            continue;

        }



        address_list = &info->value->ip_addresses;



        while (*address_list && (*address_list)->next) {

            address_list = &(*address_list)->next;

        }



        if (!*address_list) {

            *address_list = address_item;

        } else {

            (*address_list)->next = address_item;

        }



        info->value->has_ip_addresses = true;





    }



    freeifaddrs(ifap);

    return head;



error:

    freeifaddrs(ifap);

    qapi_free_GuestNetworkInterfaceList(head);

    return NULL;

}