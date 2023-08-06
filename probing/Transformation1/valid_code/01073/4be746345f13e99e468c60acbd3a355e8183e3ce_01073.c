DeviceState *nand_init(BlockDriverState *bdrv, int manf_id, int chip_id)

{

    DeviceState *dev;



    if (nand_flash_ids[chip_id].size == 0) {

        hw_error("%s: Unsupported NAND chip ID.\n", __FUNCTION__);

    }

    dev = DEVICE(object_new(TYPE_NAND));

    qdev_prop_set_uint8(dev, "manufacturer_id", manf_id);

    qdev_prop_set_uint8(dev, "chip_id", chip_id);

    if (bdrv) {

        qdev_prop_set_drive_nofail(dev, "drive", bdrv);

    }



    qdev_init_nofail(dev);

    return dev;

}