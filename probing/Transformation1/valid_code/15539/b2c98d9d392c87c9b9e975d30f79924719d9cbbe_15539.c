static void query_facilities(void)

{

    unsigned long hwcap = qemu_getauxval(AT_HWCAP);



    

    if (hwcap & HWCAP_S390_STFLE) {

        register int r0 __asm__("0");

        register void *r1 __asm__("1");



        

        r1 = &facilities;

        asm volatile(".word 0xb2b0,0x1000"

                     : "=r"(r0) : "0"(0), "r"(r1) : "memory", "cc");

    }

}