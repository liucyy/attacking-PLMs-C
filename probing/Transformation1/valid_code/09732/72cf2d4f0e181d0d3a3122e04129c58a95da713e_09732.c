static void qemu_opt_del(QemuOpt *opt)

{

    TAILQ_REMOVE(&opt->opts->head, opt, next);

    qemu_free(( char*)opt->name);

    qemu_free(( char*)opt->str);

    qemu_free(opt);

}