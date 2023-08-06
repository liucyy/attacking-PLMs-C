static void test_machine(const void *data)

{

    const testdef_t *test = data;

    char tmpname[] = "/tmp/qtest-boot-serial-XXXXXX";

    int fd;



    fd = mkstemp(tmpname);

    g_assert(fd != -1);



    

    global_qtest = qtest_startf("-M %s,accel=tcg:kvm "

                                "-chardev file,id=serial0,path=%s "

                                "-no-shutdown -serial chardev:serial0 %s",

                                test->machine, tmpname, test->extra);

    unlink(tmpname);



    check_guest_output(test, fd);

    qtest_quit(global_qtest);



    close(fd);

}