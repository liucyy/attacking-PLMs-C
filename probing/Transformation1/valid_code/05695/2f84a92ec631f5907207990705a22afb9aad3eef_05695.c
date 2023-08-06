static void test_drive_del_device_del(void)

{

    

    qtest_start("-drive if=none,id=drive0,file=null-co:

                " -device virtio-scsi-pci"

                " -device scsi-hd,drive=drive0,id=dev0");



    

    drive_del();

    device_del();



    qtest_end();

}