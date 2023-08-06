static void *do_data_decompress(void *opaque)

{

    DecompressParam *param = opaque;

    unsigned long pagesize;



    while (!quit_decomp_thread) {

        qemu_mutex_lock(&param->mutex);

        while (!param->start && !quit_decomp_thread) {

            qemu_cond_wait(&param->cond, &param->mutex);

            pagesize = TARGET_PAGE_SIZE;

            if (!quit_decomp_thread) {

                

                uncompress((Bytef *)param->des, &pagesize,

                           (const Bytef *)param->compbuf, param->len);

            }

            param->start = false;

        }

        qemu_mutex_unlock(&param->mutex);

    }



    return NULL;

}