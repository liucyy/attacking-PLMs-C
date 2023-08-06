static IOMMUTLBEntry typhoon_translate_iommu(MemoryRegion *iommu, hwaddr addr,

                                             bool is_write)

{

    TyphoonPchip *pchip = container_of(iommu, TyphoonPchip, iommu);

    IOMMUTLBEntry ret;

    int i;



    if (addr <= 0xffffffffu) {

        



        

        if ((pchip->ctl & 0x20)

            && addr >= 0x80000

            && addr <= 0xfffff) {

            goto failure;

        }



        

        for (i = 0; i < 3; ++i) {

            if (window_translate(&pchip->win[i], addr, &ret)) {

                goto success;

            }

        }



        

        if ((pchip->win[3].wba & 0x80000000000ull) == 0

	    && window_translate(&pchip->win[3], addr, &ret)) {

            goto success;

        }

    } else {

        



        if (addr >= 0x10000000000ull && addr < 0x20000000000ull) {

            

            if (pchip->ctl & 0x40) {

                

                make_iommu_tlbe(0, 0x007ffffffffull, &ret);

		goto success;

            }

        }



        if (addr >= 0x80000000000ull && addr <= 0xfffffffffffull) {

            

            if ((pchip->win[3].wba & 0x80000000001ull) == 0x80000000001ull) {

                uint64_t pte_addr;



                pte_addr  = pchip->win[3].tba & 0x7ffc00000ull;

                pte_addr |= (addr & 0xffffe000u) >> 10;

                if (pte_translate(pte_addr, &ret)) {

			goto success;

		}

            }

        }

    }



 failure:

    ret = (IOMMUTLBEntry) { .perm = IOMMU_NONE };

 success:

    return ret;

}