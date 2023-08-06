int pci_drive_hot_add(Monitor *mon, const QDict *qdict, DriveInfo *dinfo)

{

    

    monitor_printf(mon, "Can't hot-add drive to type %d\n", dinfo->type);

    return -1;

}