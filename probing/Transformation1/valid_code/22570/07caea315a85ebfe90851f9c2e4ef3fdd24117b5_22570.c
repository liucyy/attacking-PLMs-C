static void network_init(void)

{

    int i;



    for(i = 0; i < nb_nics; i++) {

        NICInfo *nd = &nd_table[i];

        const char *default_devaddr = NULL;



        if (i == 0 && (!nd->model || strcmp(nd->model, "pcnet") == 0))

            

            default_devaddr = "0b";



        pci_nic_init(nd, "pcnet", default_devaddr);

    }

}