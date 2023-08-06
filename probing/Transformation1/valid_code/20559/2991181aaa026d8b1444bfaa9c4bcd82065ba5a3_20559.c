static char *pcibus_get_dev_path(DeviceState *dev)

{

    PCIDevice *d = container_of(dev, PCIDevice, qdev);

    PCIDevice *t;

    int slot_depth;

    

    int domain_len = strlen("DDDD:00");

    int slot_len = strlen(":SS.F");

    int path_len;

    char *path, *p;



    ;

    slot_depth = 0;

    for (t = d; t; t = t->bus->parent_dev) {

        ++slot_depth;

    }



    path_len = domain_len + slot_len * slot_depth;



    

    path = malloc(path_len + 1 );

    path[path_len] = '\0';



    

    snprintf(path, domain_len, "%04x:00", pci_find_domain(d->bus));



    

    p = path + path_len;

    for (t = d; t; t = t->bus->parent_dev) {

        p -= slot_len;

        snprintf(p, slot_len, ":%02x.%x", PCI_SLOT(t->devfn), PCI_FUNC(d->devfn));

    }



    return path;

}