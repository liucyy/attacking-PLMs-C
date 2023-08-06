pvscsi_ring_init_data(PVSCSIRingInfo *m, PVSCSICmdDescSetupRings *ri)

{

    int i;

    uint32_t txr_len_log2, rxr_len_log2;

    uint32_t req_ring_size, cmp_ring_size;

    m->rs_pa = ri->ringsStatePPN << VMW_PAGE_SHIFT;



    if ((ri->reqRingNumPages > PVSCSI_SETUP_RINGS_MAX_NUM_PAGES)

        || (ri->cmpRingNumPages > PVSCSI_SETUP_RINGS_MAX_NUM_PAGES)) {

        return -1;

    }

    req_ring_size = ri->reqRingNumPages * PVSCSI_MAX_NUM_REQ_ENTRIES_PER_PAGE;

    cmp_ring_size = ri->cmpRingNumPages * PVSCSI_MAX_NUM_CMP_ENTRIES_PER_PAGE;

    txr_len_log2 = pvscsi_log2(req_ring_size - 1);

    rxr_len_log2 = pvscsi_log2(cmp_ring_size - 1);



    m->txr_len_mask = MASK(txr_len_log2);

    m->rxr_len_mask = MASK(rxr_len_log2);



    m->consumed_ptr = 0;

    m->filled_cmp_ptr = 0;



    for (i = 0; i < ri->reqRingNumPages; i++) {

        m->req_ring_pages_pa[i] = ri->reqRingPPNs[i] << VMW_PAGE_SHIFT;

    }



    for (i = 0; i < ri->cmpRingNumPages; i++) {

        m->cmp_ring_pages_pa[i] = ri->cmpRingPPNs[i] << VMW_PAGE_SHIFT;

    }



    RS_SET_FIELD(m, reqProdIdx, 0);

    RS_SET_FIELD(m, reqConsIdx, 0);

    RS_SET_FIELD(m, reqNumEntriesLog2, txr_len_log2);



    RS_SET_FIELD(m, cmpProdIdx, 0);

    RS_SET_FIELD(m, cmpConsIdx, 0);

    RS_SET_FIELD(m, cmpNumEntriesLog2, rxr_len_log2);



    trace_pvscsi_ring_init_data(txr_len_log2, rxr_len_log2);



    

    smp_wmb();



    return 0;

}