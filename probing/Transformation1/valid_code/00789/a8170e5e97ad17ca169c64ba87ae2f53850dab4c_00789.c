static void gem_transmit(GemState *s)

{

    unsigned    desc[2];

    target_phys_addr_t packet_desc_addr;

    uint8_t     tx_packet[2048];

    uint8_t     *p;

    unsigned    total_bytes;



    

    if (!(s->regs[GEM_NWCTRL] & GEM_NWCTRL_TXENA)) {

        return;

    }



    DB_PRINT("\n");



    

    p = tx_packet;

    total_bytes = 0;



    

    packet_desc_addr = s->tx_desc_addr;

    cpu_physical_memory_read(packet_desc_addr,

                             (uint8_t *)&desc[0], sizeof(desc));

    

    while (tx_desc_get_used(desc) == 0) {



        

        if (!(s->regs[GEM_NWCTRL] & GEM_NWCTRL_TXENA)) {

            return;

        }

        print_gem_tx_desc(desc);



        

        if ((tx_desc_get_buffer(desc) == 0) ||

            (tx_desc_get_length(desc) == 0)) {

            DB_PRINT("Invalid TX descriptor @ 0x%x\n", packet_desc_addr);

            break;

        }



        

        cpu_physical_memory_read(tx_desc_get_buffer(desc), p,

                                 tx_desc_get_length(desc));

        p += tx_desc_get_length(desc);

        total_bytes += tx_desc_get_length(desc);



        

        if (tx_desc_get_last(desc)) {

            

            cpu_physical_memory_read(s->tx_desc_addr,

                                     (uint8_t *)&desc[0], sizeof(desc));

            tx_desc_set_used(desc);

            cpu_physical_memory_write(s->tx_desc_addr,

                                      (uint8_t *)&desc[0], sizeof(desc));

            

            if (tx_desc_get_wrap(desc)) {

                s->tx_desc_addr = s->regs[GEM_TXQBASE];

            } else {

                s->tx_desc_addr = packet_desc_addr + 8;

            }

            DB_PRINT("TX descriptor next: 0x%08x\n", s->tx_desc_addr);



            s->regs[GEM_TXSTATUS] |= GEM_TXSTATUS_TXCMPL;



            

            gem_update_int_status(s);



            

            if (s->regs[GEM_DMACFG] & GEM_DMACFG_TXCSUM_OFFL) {

                net_checksum_calculate(tx_packet, total_bytes);

            }



            

            gem_transmit_updatestats(s, tx_packet, total_bytes);



            

            if (s->phy_loop) {

                gem_receive(&s->nic->nc, tx_packet, total_bytes);

            } else {

                qemu_send_packet(&s->nic->nc, tx_packet, total_bytes);

            }



            

            p = tx_packet;

            total_bytes = 0;

        }



        

        if (tx_desc_get_wrap(desc)) {

            packet_desc_addr = s->regs[GEM_TXQBASE];

        } else {

            packet_desc_addr += 8;

        }

        cpu_physical_memory_read(packet_desc_addr,

                                 (uint8_t *)&desc[0], sizeof(desc));

    }



    if (tx_desc_get_used(desc)) {

        s->regs[GEM_TXSTATUS] |= GEM_TXSTATUS_USED;

        gem_update_int_status(s);

    }

}