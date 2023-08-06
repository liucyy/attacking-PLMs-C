void css_generate_sch_crws(uint8_t cssid, uint8_t ssid, uint16_t schid,

                           int hotplugged, int add)

{

    uint8_t guest_cssid;

    bool chain_crw;



    if (add && !hotplugged) {

        return;

    }

    if (channel_subsys.max_cssid == 0) {

        

        guest_cssid = (cssid == channel_subsys.default_cssid) ? 0 : cssid;

    } else {

        

        guest_cssid = cssid;

    }

    

    if ((ssid > channel_subsys.max_ssid) ||

        (guest_cssid > channel_subsys.max_cssid) ||

        ((channel_subsys.max_cssid == 0) &&

         (cssid != channel_subsys.default_cssid))) {

        return;

    }

    chain_crw = (channel_subsys.max_ssid > 0) ||

            (channel_subsys.max_cssid > 0);

    css_queue_crw(CRW_RSC_SUBCH, CRW_ERC_IPI, chain_crw ? 1 : 0, schid);

    if (chain_crw) {

        css_queue_crw(CRW_RSC_SUBCH, CRW_ERC_IPI, 0,

                      (guest_cssid << 8) | (ssid << 4));

    }

    

    css_clear_io_interrupt(css_do_build_subchannel_id(cssid, ssid), schid);

}