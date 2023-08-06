static void rom_reset(void *unused)

{

    Rom *rom;



    QTAILQ_FOREACH(rom, &roms, next) {

        if (rom->fw_file) {

            continue;

        }

        if (rom->data == NULL)

            continue;

        cpu_physical_memory_write_rom(rom->addr, rom->data, rom->romsize);

        if (rom->isrom) {

            

            qemu_free(rom->data);

            rom->data = NULL;

        }

    }

}