void mkimg(const char *file, const char *fmt, unsigned size_mb)

{

    gchar *cli;

    bool ret;

    int rc;

    GError *err = NULL;

    char *qemu_img_path;

    gchar *out, *out2;

    char *abs_path;



    qemu_img_path = getenv("QTEST_QEMU_IMG");

    abs_path = realpath(qemu_img_path, NULL);

    assert(qemu_img_path);



    cli = g_strdup_printf("%s create -f %s %s %uM", abs_path,

                          fmt, file, size_mb);

    ret = g_spawn_command_line_sync(cli, &out, &out2, &rc, &err);

    if (err) {

        fprintf(stderr, "%s\n", err->message);

        g_error_free(err);

    }

    g_assert(ret && !err);



    

    if (rc) {

        fprintf(stderr, "qemu-img returned status code %d\n", rc);

    }

    g_assert(!rc);



    g_free(out);

    g_free(out2);

    g_free(cli);

    free(abs_path);

}