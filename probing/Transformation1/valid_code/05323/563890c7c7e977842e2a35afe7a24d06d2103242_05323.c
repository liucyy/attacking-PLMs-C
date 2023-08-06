static void test_media_insert(void)

{

    uint8_t dir;



    

    qmp_discard_response("{'execute':'change', 'arguments':{"

                         " 'device':'floppy0', 'target': '%s' }}",

                         test_image);

    qmp_discard_response(""); 

    qmp_discard_response(""); 



    dir = inb(FLOPPY_BASE + reg_dir);

    assert_bit_set(dir, DSKCHG);

    dir = inb(FLOPPY_BASE + reg_dir);

    assert_bit_set(dir, DSKCHG);



    send_seek(0);

    dir = inb(FLOPPY_BASE + reg_dir);

    assert_bit_set(dir, DSKCHG);

    dir = inb(FLOPPY_BASE + reg_dir);

    assert_bit_set(dir, DSKCHG);



    

    send_seek(1);

    dir = inb(FLOPPY_BASE + reg_dir);

    assert_bit_clear(dir, DSKCHG);

    dir = inb(FLOPPY_BASE + reg_dir);

    assert_bit_clear(dir, DSKCHG);

}