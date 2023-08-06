void virtio_setup_block(struct subchannel_id schid)

{

    struct vq_info_block info;

    struct vq_config_block config = {};



    blk_cfg.blk_size = 0; 

    guessed_disk_nature = false;



    virtio_reset(schid);



    



    config.index = 0;

    if (run_ccw(schid, CCW_CMD_READ_VQ_CONF, &config, sizeof(config))) {

        virtio_panic("Could not get block device VQ configuration\n");

    }

    if (run_ccw(schid, CCW_CMD_READ_CONF, &blk_cfg, sizeof(blk_cfg))) {

        virtio_panic("Could not get block device configuration\n");

    }

    vring_init(&block, config.num, ring_area,

               KVM_S390_VIRTIO_RING_ALIGN);



    info.queue = (unsigned long long) ring_area;

    info.align = KVM_S390_VIRTIO_RING_ALIGN;

    info.index = 0;

    info.num = config.num;

    block.schid = schid;



    if (!run_ccw(schid, CCW_CMD_SET_VQ, &info, sizeof(info))) {

        virtio_set_status(schid, VIRTIO_CONFIG_S_DRIVER_OK);

    }



    if (!virtio_ipl_disk_is_valid()) {

        

        memset(&blk_cfg, 0, sizeof(blk_cfg));

        virtio_assume_scsi();

    }

}