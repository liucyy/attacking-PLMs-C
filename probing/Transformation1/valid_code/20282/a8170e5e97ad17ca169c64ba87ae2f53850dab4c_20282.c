void stq_be_phys(target_phys_addr_t addr, uint64_t val)

{

    val = cpu_to_be64(val);

    cpu_physical_memory_write(addr, &val, 8);

}