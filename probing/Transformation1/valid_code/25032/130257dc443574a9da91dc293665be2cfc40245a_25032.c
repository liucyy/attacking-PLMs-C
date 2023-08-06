static CharDriverState *qemu_chr_open_mux(const char *id,

                                          ChardevBackend *backend,

                                          ChardevReturn *ret, Error **errp)

{

    ChardevMux *mux = backend->mux;

    CharDriverState *chr, *drv;

    MuxDriver *d;



    drv = qemu_chr_find(mux->chardev);

    if (drv == NULL) {

        error_setg(errp, "mux: base chardev %s not found", mux->chardev);

        return NULL;

    }



    chr = qemu_chr_alloc();

    d = g_new0(MuxDriver, 1);



    chr->opaque = d;

    d->drv = drv;

    d->focus = -1;

    chr->chr_write = mux_chr_write;

    chr->chr_update_read_handler = mux_chr_update_read_handler;

    chr->chr_accept_input = mux_chr_accept_input;

    

    chr->chr_set_fe_open = NULL;

    if (drv->chr_add_watch) {

        chr->chr_add_watch = mux_chr_add_watch;

    }

    

    chr->explicit_be_open = muxes_realized ? 0 : 1;

    chr->is_mux = 1;



    return chr;

}