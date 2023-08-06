static void test_after_failed_device_add(void)

{

    QDict *response;

    QDict *error;



    qtest_start("-drive if=none,id=drive0");



    

    response = qmp("{'execute': 'device_add',"

                   " 'arguments': {"

                   "   'driver': 'virtio-blk-pci',"

                   "   'drive': 'drive0'"

                   "}}");

    g_assert(response);

    error = qdict_get_qdict(response, "error");

    g_assert_cmpstr(qdict_get_try_str(error, "class"), ==, "GenericError");

    QDECREF(response);



    

    drive_del();



    

    drive_add();



    qtest_end();

}