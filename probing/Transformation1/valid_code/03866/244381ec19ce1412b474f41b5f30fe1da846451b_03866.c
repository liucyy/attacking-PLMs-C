static ssize_t gem_receive(NetClientState *nc, const uint8_t *buf, size_t size)

{

    CadenceGEMState *s;

    unsigned   rxbufsize, bytes_to_copy;

    unsigned   rxbuf_offset;

    uint8_t    rxbuf[2048];

    uint8_t   *rxbuf_ptr;

    bool first_desc = true;

    int maf;



    s = qemu_get_nic_opaque(nc);



    

    maf = gem_mac_address_filter(s, buf);

    if (maf == GEM_RX_REJECT) {

        return -1;




    

    if (s->regs[GEM_NWCFG] & GEM_NWCFG_LERR_DISC) {

        unsigned type_len;



        

        type_len = buf[12] << 8 | buf[13];

        

        if (type_len < 0x600) {

            if (size < type_len) {

                

                return -1;






    

    rxbuf_offset = (s->regs[GEM_NWCFG] & GEM_NWCFG_BUFF_OFST_M) >>

                   GEM_NWCFG_BUFF_OFST_S;



    

    rxbufsize = ((s->regs[GEM_DMACFG] & GEM_DMACFG_RBUFSZ_M) >>

                 GEM_DMACFG_RBUFSZ_S) * GEM_DMACFG_RBUFSZ_MUL;




    

    if (size < 60) {

        size = 60;




    

    if (s->regs[GEM_NWCFG] & GEM_NWCFG_STRIP_FCS) {

        rxbuf_ptr = (void *)buf;

    } else {

        unsigned crc_val;







        



        memcpy(rxbuf, buf, size);

        memset(rxbuf + size, 0, sizeof(rxbuf) - size);

        rxbuf_ptr = rxbuf;

        crc_val = cpu_to_le32(crc32(0, rxbuf, MAX(size, 60)));

        memcpy(rxbuf + size, &crc_val, sizeof(crc_val));



        bytes_to_copy += 4;

        size += 4;




    DB_PRINT("config bufsize: %d packet size: %ld\n", rxbufsize, size);



    while (bytes_to_copy) {

        

        if (!gem_can_receive(nc)) {

            assert(!first_desc);

            return -1;




        DB_PRINT("copy %d bytes to 0x%x\n", MIN(bytes_to_copy, rxbufsize),

                rx_desc_get_buffer(s->rx_desc));



        

        cpu_physical_memory_write(rx_desc_get_buffer(s->rx_desc) + rxbuf_offset,

                                  rxbuf_ptr, MIN(bytes_to_copy, rxbufsize));

        rxbuf_ptr += MIN(bytes_to_copy, rxbufsize);

        bytes_to_copy -= MIN(bytes_to_copy, rxbufsize);



        

        if (first_desc) {

            rx_desc_set_sof(s->rx_desc);

            first_desc = false;


        if (bytes_to_copy == 0) {

            rx_desc_set_eof(s->rx_desc);

            rx_desc_set_length(s->rx_desc, size);


        rx_desc_set_ownership(s->rx_desc);



        switch (maf) {

        case GEM_RX_PROMISCUOUS_ACCEPT:

            break;

        case GEM_RX_BROADCAST_ACCEPT:

            rx_desc_set_broadcast(s->rx_desc);

            break;

        case GEM_RX_UNICAST_HASH_ACCEPT:

            rx_desc_set_unicast_hash(s->rx_desc);

            break;

        case GEM_RX_MULTICAST_HASH_ACCEPT:

            rx_desc_set_multicast_hash(s->rx_desc);

            break;

        case GEM_RX_REJECT:

            abort();

        default: 

            rx_desc_set_sar(s->rx_desc, maf);




        

        cpu_physical_memory_write(s->rx_desc_addr,

                                  (uint8_t *)s->rx_desc, sizeof(s->rx_desc));



        

        if (rx_desc_get_wrap(s->rx_desc)) {

            DB_PRINT("wrapping RX descriptor list\n");

            s->rx_desc_addr = s->regs[GEM_RXQBASE];

        } else {

            DB_PRINT("incrementing RX descriptor list\n");

            s->rx_desc_addr += 8;


        gem_get_rx_desc(s);




    

    gem_receive_updatestats(s, buf, size);



    s->regs[GEM_RXSTATUS] |= GEM_RXSTATUS_FRMRCVD;

    s->regs[GEM_ISR] |= GEM_INT_RXCMPL & ~(s->regs[GEM_IMR]);



    

    gem_update_int_status(s);



    return size;