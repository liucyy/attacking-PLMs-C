static void ahci_migrate(AHCIQState *from, AHCIQState *to, const char *uri)

{

    QOSState *tmp = to->parent;

    QPCIDevice *dev = to->dev;

    char *uri_local = NULL;



    if (uri == NULL) {

        uri_local = g_strdup_printf("%s%s", "unix:", mig_socket);

        uri = uri_local;

    }



    

    migrate(from->parent, to->parent, uri);



    

    memcpy(to, from, sizeof(AHCIQState));

    to->parent = tmp;

    to->dev = dev;



    tmp = from->parent;

    dev = from->dev;

    memset(from, 0x00, sizeof(AHCIQState));

    from->parent = tmp;

    from->dev = dev;



    verify_state(to);

    g_free(uri_local);

}