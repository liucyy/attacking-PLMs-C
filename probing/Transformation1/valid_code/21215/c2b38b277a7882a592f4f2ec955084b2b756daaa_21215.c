int qemu_timeout_ns_to_ms(int64_t ns)

{

    int64_t ms;

    if (ns < 0) {

        return -1;

    }



    if (!ns) {

        return 0;

    }



    

    ms = DIV_ROUND_UP(ns, SCALE_MS);



    

    if (ms > (int64_t) INT32_MAX) {

        ms = INT32_MAX;

    }



    return (int) ms;

}