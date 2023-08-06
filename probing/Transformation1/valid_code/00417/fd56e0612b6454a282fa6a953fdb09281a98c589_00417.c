static int piix3_post_load(void *opaque, int version_id)

{

    PIIX3State *piix3 = opaque;

    int pirq;



    

    piix3->pic_levels = 0;

    for (pirq = 0; pirq < PIIX_NUM_PIRQS; pirq++) {

        piix3_set_irq_level_internal(piix3, pirq,

                            pci_bus_get_irq_level(piix3->dev.bus, pirq));

    }

    return 0;

}