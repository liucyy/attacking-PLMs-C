static void test_acpi_one(const char *params)

{

    char *args;

    uint8_t signature_low;

    uint8_t signature_high;

    uint16_t signature;

    int i;

    uint32_t off;





    args = g_strdup_printf("-net none -display none %s %s",

                           params ? params : "", disk);

    qtest_start(args);



   

#define TEST_DELAY (1 * G_USEC_PER_SEC / 10)

#define TEST_CYCLES MAX((60 * G_USEC_PER_SEC / TEST_DELAY), 1)



    

    for (i = 0; i < TEST_CYCLES; ++i) {

        signature_low = readb(BOOT_SECTOR_ADDRESS + SIGNATURE_OFFSET);

        signature_high = readb(BOOT_SECTOR_ADDRESS + SIGNATURE_OFFSET + 1);

        signature = (signature_high << 8) | signature_low;

        if (signature == SIGNATURE) {

            break;

        }

        g_usleep(TEST_DELAY);

    }

    g_assert_cmphex(signature, ==, SIGNATURE);



    

    for (off = 0xf0000; off < 0x100000; off += 0x10)

    {

        uint8_t sig[] = "RSD PTR ";

        int i;



        for (i = 0; i < sizeof sig - 1; ++i) {

            sig[i] = readb(off + i);

        }



        if (!memcmp(sig, "RSD PTR ", sizeof sig)) {

            break;

        }

    }



    g_assert_cmphex(off, <, 0x100000);



    qtest_quit(global_qtest);

    g_free(args);

}