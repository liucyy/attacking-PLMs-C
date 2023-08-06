void ioinst_handle_stsch(S390CPU *cpu, uint64_t reg1, uint32_t ipb)

{

    int cssid, ssid, schid, m;

    SubchDev *sch;

    uint64_t addr;

    int cc;

    SCHIB schib;

    CPUS390XState *env = &cpu->env;

    uint8_t ar;



    addr = decode_basedisp_s(env, ipb, &ar);

    if (addr & 3) {

        program_interrupt(env, PGM_SPECIFICATION, 2);

        return;

    }



    if (ioinst_disassemble_sch_ident(reg1, &m, &cssid, &ssid, &schid)) {

        

        if (!s390_cpu_virt_mem_check_write(cpu, addr, ar, sizeof(schib))) {

            program_interrupt(env, PGM_OPERAND, 2);

        }

        return;

    }

    trace_ioinst_sch_id("stsch", cssid, ssid, schid);

    sch = css_find_subch(m, cssid, ssid, schid);

    if (sch) {

        if (css_subch_visible(sch)) {

            css_do_stsch(sch, &schib);

            cc = 0;

        } else {

            

            cc = 3;

        }

    } else {

        if (css_schid_final(m, cssid, ssid, schid)) {

            cc = 3; 

        } else {

            

            memset(&schib, 0, sizeof(schib));

            cc = 0;

        }

    }

    if (cc != 3) {

        if (s390_cpu_virt_mem_write(cpu, addr, ar, &schib,

                                    sizeof(schib)) != 0) {

            return;

        }

    } else {

        

        if (s390_cpu_virt_mem_check_write(cpu, addr, ar, sizeof(schib)) != 0) {

            return;

        }

    }

    setcc(cpu, cc);

}