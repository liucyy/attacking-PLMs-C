static ssize_t nic_receive(NetClientState *nc, const uint8_t * buf, size_t size)

{

    

    EEPRO100State *s = DO_UPCAST(NICState, nc, nc)->opaque;

    uint16_t rfd_status = 0xa000;

#if defined(CONFIG_PAD_RECEIVED_FRAMES)

    uint8_t min_buf[60];

#endif

    static const uint8_t broadcast_macaddr[6] =

        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };



#if defined(CONFIG_PAD_RECEIVED_FRAMES)

    

    if (size < sizeof(min_buf)) {

        memcpy(min_buf, buf, size);

        memset(&min_buf[size], 0, sizeof(min_buf) - size);

        buf = min_buf;

        size = sizeof(min_buf);

    }

#endif



    if (s->configuration[8] & 0x80) {

        

        logout("%p received while CSMA is disabled\n", s);

        return -1;

#if !defined(CONFIG_PAD_RECEIVED_FRAMES)

    } else if (size < 64 && (s->configuration[7] & BIT(0))) {

        

        logout("%p received short frame (%zu byte)\n", s, size);

        s->statistics.rx_short_frame_errors++;

        return -1;

#endif

    } else if ((size > MAX_ETH_FRAME_SIZE + 4) && !(s->configuration[18] & BIT(3))) {

        

        logout("%p received long frame (%zu byte), ignored\n", s, size);

        return -1;

    } else if (memcmp(buf, s->conf.macaddr.a, 6) == 0) {       

        

        

        TRACE(RXTX, logout("%p received frame for me, len=%zu\n", s, size));

    } else if (memcmp(buf, broadcast_macaddr, 6) == 0) {

        

        TRACE(RXTX, logout("%p received broadcast, len=%zu\n", s, size));

        rfd_status |= 0x0002;

    } else if (buf[0] & 0x01) {

        

        TRACE(RXTX, logout("%p received multicast, len=%zu,%s\n", s, size, nic_dump(buf, size)));

        if (s->configuration[21] & BIT(3)) {

          

        } else {

          unsigned mcast_idx = e100_compute_mcast_idx(buf);

          assert(mcast_idx < 64);

          if (s->mult[mcast_idx >> 3] & (1 << (mcast_idx & 7))) {

            

          } else if (s->configuration[15] & BIT(0)) {

              

              rfd_status |= 0x0004;

          } else {

              TRACE(RXTX, logout("%p multicast ignored\n", s));

              return -1;

          }

        }

        

        rfd_status |= 0x0002;

    } else if (s->configuration[15] & BIT(0)) {

        

        TRACE(RXTX, logout("%p received frame in promiscuous mode, len=%zu\n", s, size));

        rfd_status |= 0x0004;

    } else if (s->configuration[20] & BIT(6)) {

        

        unsigned mcast_idx = compute_mcast_idx(buf);

        assert(mcast_idx < 64);

        if (s->mult[mcast_idx >> 3] & (1 << (mcast_idx & 7))) {

            TRACE(RXTX, logout("%p accepted, multiple IA bit set\n", s));

        } else {

            TRACE(RXTX, logout("%p frame ignored, multiple IA bit set\n", s));

            return -1;

        }

    } else {

        TRACE(RXTX, logout("%p received frame, ignored, len=%zu,%s\n", s, size,

              nic_dump(buf, size)));

        return size;

    }



    if (get_ru_state(s) != ru_ready) {

        

        logout("no resources, state=%u\n", get_ru_state(s));

        

        eepro100_rnr_interrupt(s);

        s->statistics.rx_resource_errors++;

#if 0

        assert(!"no resources");

#endif

        return -1;

    }

    

    eepro100_rx_t rx;

    pci_dma_read(&s->dev, s->ru_base + s->ru_offset,

                 &rx, sizeof(eepro100_rx_t));

    uint16_t rfd_command = le16_to_cpu(rx.command);

    uint16_t rfd_size = le16_to_cpu(rx.size);



    if (size > rfd_size) {

        logout("Receive buffer (%" PRId16 " bytes) too small for data "

            "(%zu bytes); data truncated\n", rfd_size, size);

        size = rfd_size;

    }

#if !defined(CONFIG_PAD_RECEIVED_FRAMES)

    if (size < 64) {

        rfd_status |= 0x0080;

    }

#endif

    TRACE(OTHER, logout("command 0x%04x, link 0x%08x, addr 0x%08x, size %u\n",

          rfd_command, rx.link, rx.rx_buf_addr, rfd_size));

    stw_le_pci_dma(&s->dev, s->ru_base + s->ru_offset +

                offsetof(eepro100_rx_t, status), rfd_status);

    stw_le_pci_dma(&s->dev, s->ru_base + s->ru_offset +

                offsetof(eepro100_rx_t, count), size);

    

#if 0

    eepro100_er_interrupt(s);

#endif

    

    if (s->configuration[18] & BIT(2)) {

        missing("Receive CRC Transfer");

        return -1;

    }

    

#if 0

    assert(!(s->configuration[17] & BIT(0)));

#endif

    pci_dma_write(&s->dev, s->ru_base + s->ru_offset +

                  sizeof(eepro100_rx_t), buf, size);

    s->statistics.rx_good_frames++;

    eepro100_fr_interrupt(s);

    s->ru_offset = le32_to_cpu(rx.link);

    if (rfd_command & COMMAND_EL) {

        

        logout("receive: Running out of frames\n");

        set_ru_state(s, ru_suspended);

    }

    if (rfd_command & COMMAND_S) {

        

        set_ru_state(s, ru_suspended);

    }

    return size;

}