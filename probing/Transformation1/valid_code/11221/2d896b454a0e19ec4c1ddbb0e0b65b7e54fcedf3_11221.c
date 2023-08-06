type_init(boston_register_types)



static void gen_firmware(uint32_t *p, hwaddr kernel_entry, hwaddr fdt_addr,

                         bool is_64b)

{

    const uint32_t cm_base = 0x16100000;

    const uint32_t gic_base = 0x16120000;

    const uint32_t cpc_base = 0x16200000;



    

    if (is_64b) {

        stl_p(p++, 0x40287803);                 

        stl_p(p++, 0x00084138);                 

    } else {

        stl_p(p++, 0x40087803);                 

        stl_p(p++, 0x00084100);                 

    }

    stl_p(p++, 0x3c09a000);                     

    stl_p(p++, 0x01094025);                     

    stl_p(p++, 0x3c0a0000 | (cm_base >> 16));   

    if (is_64b) {

        stl_p(p++, 0xfd0a0008);                 

    } else {

        stl_p(p++, 0xad0a0008);                 

    }

    stl_p(p++, 0x012a4025);                     



    

    stl_p(p++, 0x3c090000 | (gic_base >> 16));  

    stl_p(p++, 0x35290001);                     

    if (is_64b) {

        stl_p(p++, 0xfd090080);                 

    } else {

        stl_p(p++, 0xad090080);                 

    }



    

    stl_p(p++, 0x3c090000 | (cpc_base >> 16));  

    stl_p(p++, 0x35290001);                     

    if (is_64b) {

        stl_p(p++, 0xfd090088);                 

    } else {

        stl_p(p++, 0xad090088);                 

    }



    

    stl_p(p++, 0x2404fffe);                     

                                                

    stl_p(p++, 0x3c050000 | ((fdt_addr >> 16) & 0xffff));

    if (fdt_addr & 0xffff) {                    

        stl_p(p++, 0x34a50000 | (fdt_addr & 0xffff));

    }

    stl_p(p++, 0x34060000);                     

    stl_p(p++, 0x34070000);                     



    

                                                

    stl_p(p++, 0x3c190000 | ((kernel_entry >> 16) & 0xffff));

                                                

    stl_p(p++, 0x37390000 | (kernel_entry & 0xffff));

    stl_p(p++, 0x03200009);                     

}