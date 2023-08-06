static int n8x0_atag_setup(void *p, int model)

{

    uint8_t *b;

    uint16_t *w;

    uint32_t *l;

    struct omap_gpiosw_info_s *gpiosw;

    struct omap_partition_info_s *partition;

    const char *tag;



    w = p;



    stw_p(w++, OMAP_TAG_UART);			

    stw_p(w++, 4);				

    stw_p(w++, (1 << 2) | (1 << 1) | (1 << 0)); 

    w++;



#if 0

    stw_p(w++, OMAP_TAG_SERIAL_CONSOLE);	

    stw_p(w++, 4);				

    stw_p(w++, XLDR_LL_UART + 1);		

    stw_p(w++, 115200);				

#endif



    stw_p(w++, OMAP_TAG_LCD);			

    stw_p(w++, 36);				

    strcpy((void *) w, "QEMU LCD panel");	

    w += 8;

    strcpy((void *) w, "blizzard");		

    w += 8;

    stw_p(w++, N810_BLIZZARD_RESET_GPIO);	

    stw_p(w++, 24);				



    stw_p(w++, OMAP_TAG_CBUS);			

    stw_p(w++, 8);				

    stw_p(w++, N8X0_CBUS_CLK_GPIO);		

    stw_p(w++, N8X0_CBUS_DAT_GPIO);		

    stw_p(w++, N8X0_CBUS_SEL_GPIO);		

    w++;



    stw_p(w++, OMAP_TAG_EM_ASIC_BB5);		

    stw_p(w++, 4);				

    stw_p(w++, N8X0_RETU_GPIO);			

    stw_p(w++, N8X0_TAHVO_GPIO);		



    gpiosw = (model == 810) ? n810_gpiosw_info : n800_gpiosw_info;

    for (; gpiosw->name; gpiosw++) {

        stw_p(w++, OMAP_TAG_GPIO_SWITCH);	

        stw_p(w++, 20);				

        strcpy((void *) w, gpiosw->name);	

        w += 6;

        stw_p(w++, gpiosw->line);		

        stw_p(w++, gpiosw->type);

        stw_p(w++, 0);

        stw_p(w++, 0);

    }



    stw_p(w++, OMAP_TAG_NOKIA_BT);		

    stw_p(w++, 12);				

    b = (void *) w;

    stb_p(b++, 0x01);				

    stb_p(b++, N8X0_BT_WKUP_GPIO);		

    stb_p(b++, N8X0_BT_HOST_WKUP_GPIO);		

    stb_p(b++, N8X0_BT_RESET_GPIO);		

    stb_p(b++, BT_UART + 1);			

    memcpy(b, &n8x0_bd_addr, 6);		

    b += 6;

    stb_p(b++, 0x02);				

    w = (void *) b;



    stw_p(w++, OMAP_TAG_WLAN_CX3110X);		

    stw_p(w++, 8);				

    stw_p(w++, 0x25);				

    stw_p(w++, N8X0_WLAN_PWR_GPIO);		

    stw_p(w++, N8X0_WLAN_IRQ_GPIO);		

    stw_p(w++, -1);				



    stw_p(w++, OMAP_TAG_MMC);			

    stw_p(w++, 16);				

    if (model == 810) {

        stw_p(w++, 0x23f);			

        stw_p(w++, -1);				

        stw_p(w++, -1);				

        stw_p(w++, -1);				

        stw_p(w++, 0x240);			

        stw_p(w++, 0xc000);			

        stw_p(w++, 0x0248);			

        stw_p(w++, 0xc000);			

    } else {

        stw_p(w++, 0xf);			

        stw_p(w++, -1);				

        stw_p(w++, -1);				

        stw_p(w++, -1);				

        stw_p(w++, 0);				

        stw_p(w++, 0);				

        stw_p(w++, 0);				

        stw_p(w++, 0);				

    }



    stw_p(w++, OMAP_TAG_TEA5761);		

    stw_p(w++, 4);				

    stw_p(w++, N8X0_TEA5761_CS_GPIO);		

    w++;



    partition = (model == 810) ? n810_part_info : n800_part_info;

    for (; partition->name; partition++) {

        stw_p(w++, OMAP_TAG_PARTITION);		

        stw_p(w++, 28);				

        strcpy((void *) w, partition->name);	

        l = (void *) (w + 8);

        stl_p(l++, partition->size);		

        stl_p(l++, partition->offset);		

        stl_p(l++, partition->mask);		

        w = (void *) l;

    }



    stw_p(w++, OMAP_TAG_BOOT_REASON);		

    stw_p(w++, 12);				

#if 0

    strcpy((void *) w, "por");			

    strcpy((void *) w, "charger");		

    strcpy((void *) w, "32wd_to");		

    strcpy((void *) w, "sw_rst");		

    strcpy((void *) w, "mbus");			

    strcpy((void *) w, "unknown");		

    strcpy((void *) w, "swdg_to");		

    strcpy((void *) w, "sec_vio");		

    strcpy((void *) w, "pwr_key");		

    strcpy((void *) w, "rtc_alarm");		

#else

    strcpy((void *) w, "pwr_key");		

#endif

    w += 6;



    tag = (model == 810) ? "RX-44" : "RX-34";

    stw_p(w++, OMAP_TAG_VERSION_STR);		

    stw_p(w++, 24);				

    strcpy((void *) w, "product");		

    w += 6;

    strcpy((void *) w, tag);			

    w += 6;



    stw_p(w++, OMAP_TAG_VERSION_STR);		

    stw_p(w++, 24);				

    strcpy((void *) w, "hw-build");		

    w += 6;

    strcpy((void *) w, "QEMU ");

    pstrcat((void *) w, 12, qemu_get_version()); 

    w += 6;



    tag = (model == 810) ? "1.1.10-qemu" : "1.1.6-qemu";

    stw_p(w++, OMAP_TAG_VERSION_STR);		

    stw_p(w++, 24);				

    strcpy((void *) w, "nolo");			

    w += 6;

    strcpy((void *) w, tag);			

    w += 6;



    return (void *) w - p;

}