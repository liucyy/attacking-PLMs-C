static char *vio_format_dev_name(VIOsPAPRDevice *dev)

{

    VIOsPAPRDeviceInfo *info = (VIOsPAPRDeviceInfo *)qdev_get_info(&dev->qdev);

    char *name;



    

    if (asprintf(&name, "%s@%x", info->dt_name, dev->reg) < 0) {

        return NULL;

    }



    return name;

}