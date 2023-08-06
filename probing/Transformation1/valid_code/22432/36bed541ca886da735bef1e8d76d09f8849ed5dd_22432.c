static void puv3_load_kernel(const char *kernel_filename)

{

    int size;



    if (kernel_filename == NULL && qtest_enabled()) {

        return;

    }

    assert(kernel_filename != NULL);



    

    size = load_image_targphys(kernel_filename, KERNEL_LOAD_ADDR,

            KERNEL_MAX_SIZE);

    if (size < 0) {

        error_report("Load kernel error: '%s'", kernel_filename);

        exit(1);

    }



    

    graphic_console_init(NULL, 0, &no_ops, NULL);

}