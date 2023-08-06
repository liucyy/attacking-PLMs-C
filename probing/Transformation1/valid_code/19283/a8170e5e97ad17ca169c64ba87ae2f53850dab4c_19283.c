static void bonito_ldma_writel(void *opaque, target_phys_addr_t addr,

                               uint64_t val, unsigned size)

{

    PCIBonitoState *s = opaque;



    ((uint32_t *)(&s->bonldma))[addr/sizeof(uint32_t)] = val & 0xffffffff;

}