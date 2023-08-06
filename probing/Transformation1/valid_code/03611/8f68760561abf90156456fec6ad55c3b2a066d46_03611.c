static char *spapr_phb_vfio_get_loc_code(sPAPRPHBState *sphb,  PCIDevice *pdev)

{

    char *path = NULL, *buf = NULL, *host = NULL;



    

    host = object_property_get_str(OBJECT(pdev), "host", NULL);

    if (!host) {

        goto err_out;

    }



    

    path = g_strdup_printf("/sys/bus/pci/devices/%s/devspec", host);

    g_free(host);

    if (!path || !g_file_get_contents(path, &buf, NULL, NULL)) {

        goto err_out;

    }

    g_free(path);



    

    path = g_strdup_printf("/proc/device-tree%s/ibm,loc-code", buf);

    g_free(buf);

    if (!path || !g_file_get_contents(path, &buf, NULL, NULL)) {

        goto err_out;

    }

    return buf;



err_out:

    g_free(path);

    return NULL;

}