static void ahci_idp_write(void *opaque, target_phys_addr_t addr,

                           uint64_t val, unsigned size)

{

    AHCIState *s = opaque;



    if (addr == s->idp_offset) {

        

        s->idp_index = (uint32_t)val & ((AHCI_MEM_BAR_SIZE - 1) & ~3);

    } else if (addr == s->idp_offset + 4) {

        

        ahci_mem_write(opaque, s->idp_index, val, size);

    }

}