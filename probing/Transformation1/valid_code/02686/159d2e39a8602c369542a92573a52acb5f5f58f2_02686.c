static long getrampagesize(void)

{

    long hpsize = LONG_MAX;

    Object *memdev_root;



    if (mem_path) {

        return gethugepagesize(mem_path);

    }



    

    memdev_root = object_resolve_path("/objects", NULL);

    if (!memdev_root) {

        return getpagesize();

    }



    object_child_foreach(memdev_root, find_max_supported_pagesize, &hpsize);



    if (hpsize == LONG_MAX) {

        return getpagesize();

    }



    if (nb_numa_nodes == 0 && hpsize > getpagesize()) {

        

        static bool warned;

        if (!warned) {

            error_report("Huge page support disabled (n/a for main memory).");

            warned = true;

        }

        return getpagesize();

    }



    return hpsize;

}