static void *do_data_decompress(void *opaque)

{

    DecompressParam *param = opaque;

    unsigned long pagesize;



    while (!quit_decomp_thread) {

        qemu_mutex_lock(&param->mutex);

        while (!param->start && !quit_decomp_thread) {

            qemu_cond_wait(&param->cond, &param->mutex);

        }

        if (!quit_decomp_thread) {

            pagesize = TARGET_PAGE_SIZE;

            

            uncompress((Bytef *)param->des, &pagesize,

                       (const Bytef *)param->compbuf, param->len);

        }

        param->start = false;

        qemu_mutex_unlock(&param->mutex);



        qemu_mutex_lock(&decomp_done_lock);

        param->done = true;

        qemu_cond_signal(&decomp_done_cond);

        qemu_mutex_unlock(&decomp_done_lock);

    }



    return NULL;

}