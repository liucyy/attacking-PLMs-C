static uint64_t gt64120_readl (void *opaque,

                               target_phys_addr_t addr, unsigned size)

{

    GT64120State *s = opaque;

    PCIHostState *phb = PCI_HOST_BRIDGE(s);

    uint32_t val;

    uint32_t saddr;



    saddr = (addr & 0xfff) >> 2;

    switch (saddr) {



    

    case GT_MULTI:

        

        val = s->regs[saddr];

        break;



    

    case GT_CPUERR_ADDRLO:

    case GT_CPUERR_ADDRHI:

    case GT_CPUERR_DATALO:

    case GT_CPUERR_DATAHI:

    case GT_CPUERR_PARITY:

        

        val = s->regs[saddr];

        break;



    

    case GT_PCI0SYNC:

    case GT_PCI1SYNC:

        

        val = 0xc000ffee;

        break;



    

    case GT_ECC_ERRDATALO:

    case GT_ECC_ERRDATAHI:

    case GT_ECC_MEM:

    case GT_ECC_CALC:

    case GT_ECC_ERRADDR:

        

        val = s->regs[saddr];

        break;



    case GT_CPU:

    case GT_SCS10LD:

    case GT_SCS10HD:

    case GT_SCS32LD:

    case GT_SCS32HD:

    case GT_CS20LD:

    case GT_CS20HD:

    case GT_CS3BOOTLD:

    case GT_CS3BOOTHD:

    case GT_SCS10AR:

    case GT_SCS32AR:

    case GT_CS20R:

    case GT_CS3BOOTR:

    case GT_PCI0IOLD:

    case GT_PCI0M0LD:

    case GT_PCI0M1LD:

    case GT_PCI1IOLD:

    case GT_PCI1M0LD:

    case GT_PCI1M1LD:

    case GT_PCI0IOHD:

    case GT_PCI0M0HD:

    case GT_PCI0M1HD:

    case GT_PCI1IOHD:

    case GT_PCI1M0HD:

    case GT_PCI1M1HD:

    case GT_PCI0IOREMAP:

    case GT_PCI0M0REMAP:

    case GT_PCI0M1REMAP:

    case GT_PCI1IOREMAP:

    case GT_PCI1M0REMAP:

    case GT_PCI1M1REMAP:

    case GT_ISD:

        val = s->regs[saddr];

        break;

    case GT_PCI0_IACK:

        

        val = pic_read_irq(isa_pic);

        break;



    

    case GT_SCS0LD:

    case GT_SCS0HD:

    case GT_SCS1LD:

    case GT_SCS1HD:

    case GT_SCS2LD:

    case GT_SCS2HD:

    case GT_SCS3LD:

    case GT_SCS3HD:

    case GT_CS0LD:

    case GT_CS0HD:

    case GT_CS1LD:

    case GT_CS1HD:

    case GT_CS2LD:

    case GT_CS2HD:

    case GT_CS3LD:

    case GT_CS3HD:

    case GT_BOOTLD:

    case GT_BOOTHD:

    case GT_ADERR:

        val = s->regs[saddr];

        break;



    

    case GT_SDRAM_CFG:

    case GT_SDRAM_OPMODE:

    case GT_SDRAM_BM:

    case GT_SDRAM_ADDRDECODE:

        val = s->regs[saddr];

        break;



    

    case GT_SDRAM_B0:

    case GT_SDRAM_B1:

    case GT_SDRAM_B2:

    case GT_SDRAM_B3:

        

        val = s->regs[saddr];

        break;



    

    case GT_DEV_B0:

    case GT_DEV_B1:

    case GT_DEV_B2:

    case GT_DEV_B3:

    case GT_DEV_BOOT:

        val = s->regs[saddr];

        break;



    

    case GT_DMA0_CNT:

    case GT_DMA1_CNT:

    case GT_DMA2_CNT:

    case GT_DMA3_CNT:

    case GT_DMA0_SA:

    case GT_DMA1_SA:

    case GT_DMA2_SA:

    case GT_DMA3_SA:

    case GT_DMA0_DA:

    case GT_DMA1_DA:

    case GT_DMA2_DA:

    case GT_DMA3_DA:

    case GT_DMA0_NEXT:

    case GT_DMA1_NEXT:

    case GT_DMA2_NEXT:

    case GT_DMA3_NEXT:

    case GT_DMA0_CUR:

    case GT_DMA1_CUR:

    case GT_DMA2_CUR:

    case GT_DMA3_CUR:

        val = s->regs[saddr];

        break;



    

    case GT_DMA0_CTRL:

    case GT_DMA1_CTRL:

    case GT_DMA2_CTRL:

    case GT_DMA3_CTRL:

        val = s->regs[saddr];

        break;



    

    case GT_DMA_ARB:

        val = s->regs[saddr];

        break;



    

    case GT_TC0:

    case GT_TC1:

    case GT_TC2:

    case GT_TC3:

    case GT_TC_CONTROL:

        val = s->regs[saddr];

        break;



    

    case GT_PCI0_CFGADDR:

        val = phb->config_reg;

        break;

    case GT_PCI0_CFGDATA:

        if (!(phb->config_reg & (1 << 31))) {

            val = 0xffffffff;

        } else {

            val = pci_data_read(phb->bus, phb->config_reg, 4);

        }

        if (!(s->regs[GT_PCI0_CMD] & 1) && (phb->config_reg & 0x00fff800)) {

            val = bswap32(val);

        }

        break;



    case GT_PCI0_CMD:

    case GT_PCI0_TOR:

    case GT_PCI0_BS_SCS10:

    case GT_PCI0_BS_SCS32:

    case GT_PCI0_BS_CS20:

    case GT_PCI0_BS_CS3BT:

    case GT_PCI1_IACK:

    case GT_PCI0_BARE:

    case GT_PCI0_PREFMBR:

    case GT_PCI0_SCS10_BAR:

    case GT_PCI0_SCS32_BAR:

    case GT_PCI0_CS20_BAR:

    case GT_PCI0_CS3BT_BAR:

    case GT_PCI0_SSCS10_BAR:

    case GT_PCI0_SSCS32_BAR:

    case GT_PCI0_SCS3BT_BAR:

    case GT_PCI1_CMD:

    case GT_PCI1_TOR:

    case GT_PCI1_BS_SCS10:

    case GT_PCI1_BS_SCS32:

    case GT_PCI1_BS_CS20:

    case GT_PCI1_BS_CS3BT:

    case GT_PCI1_BARE:

    case GT_PCI1_PREFMBR:

    case GT_PCI1_SCS10_BAR:

    case GT_PCI1_SCS32_BAR:

    case GT_PCI1_CS20_BAR:

    case GT_PCI1_CS3BT_BAR:

    case GT_PCI1_SSCS10_BAR:

    case GT_PCI1_SSCS32_BAR:

    case GT_PCI1_SCS3BT_BAR:

    case GT_PCI1_CFGADDR:

    case GT_PCI1_CFGDATA:

        val = s->regs[saddr];

        break;



    

    case GT_INTRCAUSE:

        val = s->regs[saddr];

        DPRINTF("INTRCAUSE %x\n", val);

        break;

    case GT_INTRMASK:

        val = s->regs[saddr];

        DPRINTF("INTRMASK %x\n", val);

        break;

    case GT_PCI0_ICMASK:

        val = s->regs[saddr];

        DPRINTF("ICMASK %x\n", val);

        break;

    case GT_PCI0_SERR0MASK:

        val = s->regs[saddr];

        DPRINTF("SERR0MASK %x\n", val);

        break;



    

    case GT_HINTRCAUSE:

    case GT_CPU_INTSEL:

    case GT_PCI0_INTSEL:

    case GT_HINTRMASK:

    case GT_PCI0_HICMASK:

    case GT_PCI1_SERR1MASK:

        val = s->regs[saddr];

        break;



    default:

        val = s->regs[saddr];

        DPRINTF ("Bad register offset 0x%x\n", (int)addr);

        break;

    }



    if (!(s->regs[GT_CPU] & 0x00001000))

        val = bswap32(val);



    return val;

}