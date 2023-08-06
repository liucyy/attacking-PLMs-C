static void uhci_frame_timer(void *opaque)

{

    UHCIState *s = opaque;

    int64_t expire_time;

    uint32_t frame_addr, link, old_td_ctrl, val, int_mask;

    int cnt, ret;

    UHCI_TD td;

    UHCI_QH qh;

    uint32_t old_async_qh;



    if (!(s->cmd & UHCI_CMD_RS)) {

        qemu_del_timer(s->frame_timer);

        

        s->status |= UHCI_STS_HCHALTED;

        return;

    }

    

    s->frnum = (s->frnum + 1) & 0x7ff;

    if (s->pending_int_mask) {

        s->status2 |= s->pending_int_mask;

        s->status |= UHCI_STS_USBINT;

        uhci_update_irq(s);

    }

    old_async_qh = s->async_qh;

    frame_addr = s->fl_base_addr + ((s->frnum & 0x3ff) << 2);

    cpu_physical_memory_read(frame_addr, (uint8_t *)&link, 4);

    le32_to_cpus(&link);

    int_mask = 0;

    cnt = FRAME_MAX_LOOPS;

    while ((link & 1) == 0) {

        if (--cnt == 0)

            break;

        

        if (link & 2) {

            

            if (link == s->async_qh) {

                

                old_async_qh = 0;

                break;

            }

            cpu_physical_memory_read(link & ~0xf, (uint8_t *)&qh, sizeof(qh));

            le32_to_cpus(&qh.link);

            le32_to_cpus(&qh.el_link);

        depth_first:

            if (qh.el_link & 1) {

                

                link = qh.link;

            } else if (qh.el_link & 2) {

                

                link = qh.el_link;

            } else if (s->async_qh) {

                

                link = qh.link;

            } else {

                

                if (--cnt == 0)

                    break;

                cpu_physical_memory_read(qh.el_link & ~0xf,

                                         (uint8_t *)&td, sizeof(td));

                le32_to_cpus(&td.link);

                le32_to_cpus(&td.ctrl);

                le32_to_cpus(&td.token);

                le32_to_cpus(&td.buffer);

                old_td_ctrl = td.ctrl;

                ret = uhci_handle_td(s, &td, &int_mask, 0);



                

                if (old_td_ctrl != td.ctrl) {

                    val = cpu_to_le32(td.ctrl);

                    cpu_physical_memory_write((qh.el_link & ~0xf) + 4,

                                              (const uint8_t *)&val,

                                              sizeof(val));

                }

                if (ret < 0)

                    break; 

                if (ret == 2) {

                    s->async_qh = link;

                } else if (ret == 0) {

                    

                    qh.el_link = td.link;

                    val = cpu_to_le32(qh.el_link);

                    cpu_physical_memory_write((link & ~0xf) + 4,

                                              (const uint8_t *)&val,

                                              sizeof(val));

                    if (qh.el_link & 4) {

                        

                        goto depth_first;

                    }

                }

                

                link = qh.link;

            }

        } else {

            

            cpu_physical_memory_read(link & ~0xf, (uint8_t *)&td, sizeof(td));

            le32_to_cpus(&td.link);

            le32_to_cpus(&td.ctrl);

            le32_to_cpus(&td.token);

            le32_to_cpus(&td.buffer);



            

            

            old_td_ctrl = td.ctrl;

            ret = uhci_handle_td(s, &td, &int_mask, 0);



            

            if (old_td_ctrl != td.ctrl) {

                val = cpu_to_le32(td.ctrl);

                cpu_physical_memory_write((link & ~0xf) + 4,

                                          (const uint8_t *)&val,

                                          sizeof(val));

            }

            if (ret < 0)

                break; 

            if (ret == 2) {

                s->async_frame_addr = frame_addr;

            }

            link = td.link;

        }

    }

    s->pending_int_mask = int_mask;

    if (old_async_qh) {

        

#ifdef DEBUG

        printf("Discarding USB packet\n");

#endif

        usb_cancel_packet(&s->usb_packet);

        s->async_qh = 0;

    }



    

    expire_time = qemu_get_clock(vm_clock) +

        (ticks_per_sec / FRAME_TIMER_FREQ);

    qemu_mod_timer(s->frame_timer, expire_time);

}