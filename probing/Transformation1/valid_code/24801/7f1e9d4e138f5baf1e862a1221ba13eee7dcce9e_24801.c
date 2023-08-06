static ssize_t nic_receive(VLANClientState *vc, const uint8_t * buf, size_t size)

{

    

    EEPRO100State *s = vc->opaque;

    uint16_t rfd_status = 0xa000;

    static const uint8_t broadcast_macaddr[6] =

        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };



    

    assert(!(s->configuration[20] & BIT(6)));



    if (s->configuration[8] & 0x80) {

        

        logout("%p received while CSMA is disabled\n", s);

        return -1;

    } else if (size < 64 && (s->configuration[7] & 1)) {

        

        logout("%p received short frame (%zu byte)\n", s, size);

        s->statistics.rx_short_frame_errors++;

        

    } else if ((size > MAX_ETH_FRAME_SIZE + 4) && !(s->configuration[18] & 8)) {

        

        logout("%p received long frame (%zu byte), ignored\n", s, size);

        return -1;

    } else if (memcmp(buf, s->macaddr, 6) == 0) {       

        

        

        TRACE(RXTX, logout("%p received frame for me, len=%zu\n", s, size));

    } else if (memcmp(buf, broadcast_macaddr, 6) == 0) {

        

        TRACE(RXTX, logout("%p received broadcast, len=%zu\n", s, size));

        rfd_status |= 0x0002;

    } else if (buf[0] & 0x01) { 

        

        TRACE(RXTX, logout("%p received multicast, len=%zu\n", s, size));

        

        assert(!(s->configuration[21] & BIT(3)));

        int mcast_idx = compute_mcast_idx(buf);

        if (!(s->mult[mcast_idx >> 3] & (1 << (mcast_idx & 7)))) {

            return size;

        }

        rfd_status |= 0x0002;

    } else if (s->configuration[15] & 1) {

        

        TRACE(RXTX, logout("%p received frame in promiscuous mode, len=%zu\n", s, size));

        rfd_status |= 0x0004;

    } else {

        TRACE(RXTX, logout("%p received frame, ignored, len=%zu,%s\n", s, size,

              nic_dump(buf, size)));

        return size;

    }



    if (get_ru_state(s) != ru_ready) {

        

        logout("no resources, state=%u\n", get_ru_state(s));

        s->statistics.rx_resource_errors++;

        

        return -1;

    }

    



    eepro100_rx_t rx;

    cpu_physical_memory_read(s->ru_base + s->ru_offset, (uint8_t *) & rx,

                             offsetof(eepro100_rx_t, packet));

    uint16_t rfd_command = le16_to_cpu(rx.command);

    uint16_t rfd_size = le16_to_cpu(rx.size);

    assert(size <= rfd_size);

    if (size < 64) {

        rfd_status |= 0x0080;

    }

    TRACE(OTHER, logout("command 0x%04x, link 0x%08x, addr 0x%08x, size %u\n",

          rfd_command, rx.link, rx.rx_buf_addr, rfd_size));

    stw_phys(s->ru_base + s->ru_offset + offsetof(eepro100_rx_t, status),

             rfd_status);

    stw_phys(s->ru_base + s->ru_offset + offsetof(eepro100_rx_t, count), size);

    

    

    

    assert(!(s->configuration[18] & 4));

    

    

    cpu_physical_memory_write(s->ru_base + s->ru_offset +

                              offsetof(eepro100_rx_t, packet), buf, size);

    s->statistics.rx_good_frames++;

    eepro100_fr_interrupt(s);

    s->ru_offset = le32_to_cpu(rx.link);

    if (rfd_command & 0x8000) {

        

        assert(0);

    }

    if (rfd_command & 0x4000) {

        

        set_ru_state(s, ru_suspended);

    }

    return size;

}