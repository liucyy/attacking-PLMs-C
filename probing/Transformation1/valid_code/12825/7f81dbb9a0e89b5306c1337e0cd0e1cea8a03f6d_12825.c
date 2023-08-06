static void write_bootloader(uint8_t *base, int64_t run_addr,

                             int64_t kernel_entry)

{

    uint32_t *p;



    

    p = (uint32_t *)base;



    stl_p(p++, 0x08000000 |                                      

                 ((run_addr + 0x580) & 0x0fffffff) >> 2);

    stl_p(p++, 0x00000000);                                      



    

    stl_p(base + 0x500, run_addr + 0x0580);      

    stl_p(base + 0x504, run_addr + 0x083c);      

    stl_p(base + 0x520, run_addr + 0x0580);      

    stl_p(base + 0x52c, run_addr + 0x0800);      

    stl_p(base + 0x534, run_addr + 0x0808);      

    stl_p(base + 0x538, run_addr + 0x0800);      

    stl_p(base + 0x53c, run_addr + 0x0800);      

    stl_p(base + 0x540, run_addr + 0x0800);      

    stl_p(base + 0x544, run_addr + 0x0800);      

    stl_p(base + 0x548, run_addr + 0x0800);      

    stl_p(base + 0x54c, run_addr + 0x0800);      

    stl_p(base + 0x550, run_addr + 0x0800);      

    stl_p(base + 0x554, run_addr + 0x0800);      





    

    p = (uint32_t *) (base + 0x580);



    if (semihosting_get_argc()) {

        

        stl_p(p++, 0x00000000);                         

    } else {

        stl_p(p++, 0x24040002);                         

    }

    stl_p(p++, 0x3c1d0000 | (((ENVP_ADDR - 64) >> 16) & 0xffff)); 

    stl_p(p++, 0x37bd0000 | ((ENVP_ADDR - 64) & 0xffff));        

    stl_p(p++, 0x3c050000 | ((ENVP_ADDR >> 16) & 0xffff));       

    stl_p(p++, 0x34a50000 | (ENVP_ADDR & 0xffff));               

    stl_p(p++, 0x3c060000 | (((ENVP_ADDR + 8) >> 16) & 0xffff)); 

    stl_p(p++, 0x34c60000 | ((ENVP_ADDR + 8) & 0xffff));         

    stl_p(p++, 0x3c070000 | (loaderparams.ram_low_size >> 16));     

    stl_p(p++, 0x34e70000 | (loaderparams.ram_low_size & 0xffff));  



    

    stl_p(p++, 0x3c09b400);                                      



#ifdef TARGET_WORDS_BIGENDIAN

    stl_p(p++, 0x3c08df00);                                      

#else

    stl_p(p++, 0x340800df);                                      

#endif

    stl_p(p++, 0xad280068);                                      



    stl_p(p++, 0x3c09bbe0);                                      



#ifdef TARGET_WORDS_BIGENDIAN

    stl_p(p++, 0x3c08c000);                                      

#else

    stl_p(p++, 0x340800c0);                                      

#endif

    stl_p(p++, 0xad280048);                                      

#ifdef TARGET_WORDS_BIGENDIAN

    stl_p(p++, 0x3c084000);                                      

#else

    stl_p(p++, 0x34080040);                                      

#endif

    stl_p(p++, 0xad280050);                                      



#ifdef TARGET_WORDS_BIGENDIAN

    stl_p(p++, 0x3c088000);                                      

#else

    stl_p(p++, 0x34080080);                                      

#endif

    stl_p(p++, 0xad280058);                                      

#ifdef TARGET_WORDS_BIGENDIAN

    stl_p(p++, 0x3c083f00);                                      

#else

    stl_p(p++, 0x3408003f);                                      

#endif

    stl_p(p++, 0xad280060);                                      



#ifdef TARGET_WORDS_BIGENDIAN

    stl_p(p++, 0x3c08c100);                                      

#else

    stl_p(p++, 0x340800c1);                                      

#endif

    stl_p(p++, 0xad280080);                                      

#ifdef TARGET_WORDS_BIGENDIAN

    stl_p(p++, 0x3c085e00);                                      

#else

    stl_p(p++, 0x3408005e);                                      

#endif

    stl_p(p++, 0xad280088);                                      



    

    stl_p(p++, 0x3c1f0000 | ((kernel_entry >> 16) & 0xffff));    

    stl_p(p++, 0x37ff0000 | (kernel_entry & 0xffff));            

    stl_p(p++, 0x03e00009);                                      

    stl_p(p++, 0x00000000);                                      



    

    p = (uint32_t *) (base + 0x800);

    stl_p(p++, 0x03e00009);                                     

    stl_p(p++, 0x24020000);                                     

    

    stl_p(p++, 0x03e06821);                                     

    stl_p(p++, 0x00805821);                                     

    stl_p(p++, 0x00a05021);                                     

    stl_p(p++, 0x91440000);                                     

    stl_p(p++, 0x254a0001);                                     

    stl_p(p++, 0x10800005);                                     

    stl_p(p++, 0x00000000);                                     

    stl_p(p++, 0x0ff0021c);                                     

    stl_p(p++, 0x00000000);                                     

    stl_p(p++, 0x08000205);                                     

    stl_p(p++, 0x00000000);                                     

    stl_p(p++, 0x01a00009);                                     

    stl_p(p++, 0x01602021);                                     

    

    stl_p(p++, 0x03e06821);                                     

    stl_p(p++, 0x00805821);                                     

    stl_p(p++, 0x00a05021);                                     

    stl_p(p++, 0x00c06021);                                     

    stl_p(p++, 0x91440000);                                     

    stl_p(p++, 0x0ff0021c);                                     

    stl_p(p++, 0x00000000);                                     

    stl_p(p++, 0x254a0001);                                     

    stl_p(p++, 0x258cffff);                                     

    stl_p(p++, 0x1580fffa);                                     

    stl_p(p++, 0x00000000);                                     

    stl_p(p++, 0x01a00009);                                     

    stl_p(p++, 0x01602021);                                     

    

    stl_p(p++, 0x3c08b800);                                     

    stl_p(p++, 0x350803f8);                                     

    stl_p(p++, 0x91090005);                                     

    stl_p(p++, 0x00000000);                                     

    stl_p(p++, 0x31290040);                                     

    stl_p(p++, 0x1120fffc);                                     

    stl_p(p++, 0x00000000);                                     

    stl_p(p++, 0x03e00009);                                     

    stl_p(p++, 0xa1040000);                                     



}