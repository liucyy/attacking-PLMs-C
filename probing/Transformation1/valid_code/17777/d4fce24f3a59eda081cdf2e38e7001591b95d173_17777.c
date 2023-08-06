int qtest_init(void)

{

    CharDriverState *chr;



    g_assert(qtest_chrdev != NULL);



    configure_icount("0");

    chr = qemu_chr_new("qtest", qtest_chrdev, NULL);



    qemu_chr_add_handlers(chr, qtest_can_read, qtest_read, qtest_event, chr);

    qemu_chr_fe_set_echo(chr, true);



    inbuf = g_string_new("");



    if (qtest_log) {

        if (strcmp(qtest_log, "none") != 0) {

            qtest_log_fp = fopen(qtest_log, "w+");

        }

    } else {

        qtest_log_fp = stderr;

    }



    qtest_chr = chr;



    return 0;

}