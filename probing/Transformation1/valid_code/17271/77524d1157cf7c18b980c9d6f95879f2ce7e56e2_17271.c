static void gem_transmit(CadenceGEMState *s)

{

    unsigned    desc[2];

    hwaddr packet_desc_addr;

    uint8_t     tx_packet[2048];

    uint8_t     *p;

    unsigned    total_bytes;

    int q = 0;



    

    if (!(s->regs[GEM_NWCTRL] & GEM_NWCTRL_TXENA)) {

        return;

    }



    DB_PRINT("\n");



    

    p = tx_packet;

    total_bytes = 0;



    for (q = s->num_priority_queues - 1; q >= 0; q--) {

    

    packet_desc_addr = s->tx_desc_addr[q];



    DB_PRINT("read descriptor 0x%" HWADDR_PRIx "\n", packet_desc_addr);

    cpu_physical_memory_read(packet_desc_addr,

                             (uint8_t *)desc, sizeof(desc));

    

    while (tx_desc_get_used(desc) == 0) {



        

        if (!(s->regs[GEM_NWCTRL] & GEM_NWCTRL_TXENA)) {

            return;

        }

        print_gem_tx_desc(desc, q);



        

        if ((tx_desc_get_buffer(desc) == 0) ||

            (tx_desc_get_length(desc) == 0)) {

            DB_PRINT("Invalid TX descriptor @ 0x%x\n",

                     (unsigned)packet_desc_addr);

            break;

        }



        if (tx_desc_get_length(desc) > sizeof(tx_packet) - (p - tx_packet)) {

            DB_PRINT("TX descriptor @ 0x%x too large: size 0x%x space 0x%x\n",

                     (unsigned)packet_desc_addr,

                     (unsigned)tx_desc_get_length(desc),

                     sizeof(tx_packet) - (p - tx_packet));

            break;

        }



        

        cpu_physical_memory_read(tx_desc_get_buffer(desc), p,

                                 tx_desc_get_length(desc));

        p += tx_desc_get_length(desc);

        total_bytes += tx_desc_get_length(desc);



        

        if (tx_desc_get_last(desc)) {

            unsigned    desc_first[2];



            

            cpu_physical_memory_read(s->tx_desc_addr[q], (uint8_t *)desc_first,

                                     sizeof(desc_first));

            tx_desc_set_used(desc_first);

            cpu_physical_memory_write(s->tx_desc_addr[q], (uint8_t *)desc_first,

                                      sizeof(desc_first));

            

            if (tx_desc_get_wrap(desc)) {

                s->tx_desc_addr[q] = s->regs[GEM_TXQBASE];

            } else {

                s->tx_desc_addr[q] = packet_desc_addr + 8;

            }

            DB_PRINT("TX descriptor next: 0x%08x\n", s->tx_desc_addr[q]);



            s->regs[GEM_TXSTATUS] |= GEM_TXSTATUS_TXCMPL;

            s->regs[GEM_ISR] |= GEM_INT_TXCMPL & ~(s->regs[GEM_IMR]);



            

            if (s->num_priority_queues > 1) {

                s->regs[GEM_INT_Q1_STATUS + q] |=

                        GEM_INT_TXCMPL & ~(s->regs[GEM_INT_Q1_MASK + q]);

            }



            

            gem_update_int_status(s);



            

            if (s->regs[GEM_DMACFG] & GEM_DMACFG_TXCSUM_OFFL) {

                net_checksum_calculate(tx_packet, total_bytes);

            }



            

            gem_transmit_updatestats(s, tx_packet, total_bytes);



            

            if (s->phy_loop || (s->regs[GEM_NWCTRL] & GEM_NWCTRL_LOCALLOOP)) {

                gem_receive(qemu_get_queue(s->nic), tx_packet, total_bytes);

            } else {

                qemu_send_packet(qemu_get_queue(s->nic), tx_packet,

                                 total_bytes);

            }



            

            p = tx_packet;

            total_bytes = 0;

        }



        

        if (tx_desc_get_wrap(desc)) {

            tx_desc_set_last(desc);

            packet_desc_addr = s->regs[GEM_TXQBASE];

        } else {

            packet_desc_addr += 8;

        }

        DB_PRINT("read descriptor 0x%" HWADDR_PRIx "\n", packet_desc_addr);

        cpu_physical_memory_read(packet_desc_addr,

                                 (uint8_t *)desc, sizeof(desc));

    }



    if (tx_desc_get_used(desc)) {

        s->regs[GEM_TXSTATUS] |= GEM_TXSTATUS_USED;

        s->regs[GEM_ISR] |= GEM_INT_TXUSED & ~(s->regs[GEM_IMR]);

        gem_update_int_status(s);

    }

    }

}