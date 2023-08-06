void numa_set_mem_node_id(ram_addr_t addr, uint64_t size, uint32_t node)

{

    struct numa_addr_range *range = g_malloc0(sizeof(*range));



    

    if (!size) {

        return;

    }



    range->mem_start = addr;

    range->mem_end = addr + size - 1;

    QLIST_INSERT_HEAD(&numa_info[node].addr, range, entry);

}