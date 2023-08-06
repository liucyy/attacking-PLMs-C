static void puv3_load_kernel(const char *kernel_filename)

{

    int size;



    assert(kernel_filename != NULL);



    

    size = load_image_targphys(kernel_filename, KERNEL_LOAD_ADDR,

            KERNEL_MAX_SIZE);

    if (size < 0) {

        hw_error("Load kernel error: '%s'\n", kernel_filename);

    }



    

    graphic_console_init(NULL, NULL, NULL, NULL, NULL);

}