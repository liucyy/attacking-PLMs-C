void qdev_free(DeviceState *dev)

{

#if 0 

    if (dev->info->vmsd)

        vmstate_unregister(dev->info->vmsd, dev);

#endif

    if (dev->info->reset)

        qemu_unregister_reset(dev->info->reset, dev);

    LIST_REMOVE(dev, sibling);

    qemu_free(dev);

}