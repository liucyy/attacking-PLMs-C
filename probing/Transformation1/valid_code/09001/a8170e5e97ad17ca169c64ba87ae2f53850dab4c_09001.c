int iommu_dma_memory_rw(DMAContext *dma, dma_addr_t addr,

                        void *buf, dma_addr_t len, DMADirection dir)

{

    target_phys_addr_t paddr, plen;

    int err;



#ifdef DEBUG_IOMMU

    fprintf(stderr, "dma_memory_rw context=%p addr=0x" DMA_ADDR_FMT " len=0x"

            DMA_ADDR_FMT " dir=%d\n", dma, addr, len, dir);

#endif



    while (len) {

        err = dma->translate(dma, addr, &paddr, &plen, dir);

        if (err) {

	    

	    memset(buf, 0, len);

            return -1;

        }



        

        if (plen > len) {

            plen = len;

        }



        address_space_rw(dma->as, paddr, buf, plen, dir == DMA_DIRECTION_FROM_DEVICE);



        len -= plen;

        addr += plen;

        buf += plen;

    }



    return 0;

}