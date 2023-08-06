bool qvirtio_wait_config_isr(const QVirtioBus *bus, QVirtioDevice *d,

                                                            uint64_t timeout)

{

    do {

        clock_step(100);

        if (bus->get_config_isr_status(d)) {

            break; 

        }

    } while (--timeout);



    return timeout != 0;

}