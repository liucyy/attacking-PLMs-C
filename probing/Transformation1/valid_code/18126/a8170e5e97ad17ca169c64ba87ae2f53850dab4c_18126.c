static uint64_t fw_cfg_data_mem_read(void *opaque, target_phys_addr_t addr,

                                     unsigned size)

{

    return fw_cfg_read(opaque);

}