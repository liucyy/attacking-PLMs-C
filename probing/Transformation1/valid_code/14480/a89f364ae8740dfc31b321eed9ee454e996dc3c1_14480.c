static void tc6393xb_gpio_set(void *opaque, int line, int level)

{





    if (line > TC6393XB_GPIOS) {

        printf("%s: No GPIO pin %i\n", __FUNCTION__, line);

        return;

    }



    

}