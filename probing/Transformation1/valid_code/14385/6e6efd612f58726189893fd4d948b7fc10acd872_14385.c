void define_one_arm_cp_reg_with_opaque(ARMCPU *cpu,

                                       const ARMCPRegInfo *r, void *opaque)

{

    

    int crm, opc1, opc2;

    int crmmin = (r->crm == CP_ANY) ? 0 : r->crm;

    int crmmax = (r->crm == CP_ANY) ? 15 : r->crm;

    int opc1min = (r->opc1 == CP_ANY) ? 0 : r->opc1;

    int opc1max = (r->opc1 == CP_ANY) ? 7 : r->opc1;

    int opc2min = (r->opc2 == CP_ANY) ? 0 : r->opc2;

    int opc2max = (r->opc2 == CP_ANY) ? 7 : r->opc2;

    

    assert(!((r->type & ARM_CP_64BIT) && (r->opc2 || r->crn)));

    

    if (!(r->type & (ARM_CP_SPECIAL|ARM_CP_CONST))) {

        if (r->access & PL3_R) {

            assert(r->fieldoffset || r->readfn);

        }

        if (r->access & PL3_W) {

            assert(r->fieldoffset || r->writefn);

        }

    }

    

    assert(cptype_valid(r->type));

    for (crm = crmmin; crm <= crmmax; crm++) {

        for (opc1 = opc1min; opc1 <= opc1max; opc1++) {

            for (opc2 = opc2min; opc2 <= opc2max; opc2++) {

                uint32_t *key = g_new(uint32_t, 1);

                ARMCPRegInfo *r2 = g_memdup(r, sizeof(ARMCPRegInfo));

                int is64 = (r->type & ARM_CP_64BIT) ? 1 : 0;

                *key = ENCODE_CP_REG(r->cp, is64, r->crn, crm, opc1, opc2);

                if (opaque) {

                    r2->opaque = opaque;

                }

                

                r2->crm = crm;

                r2->opc1 = opc1;

                r2->opc2 = opc2;

                

                if ((r->type & ARM_CP_SPECIAL) ||

                    ((r->crm == CP_ANY) && crm != 0) ||

                    ((r->opc1 == CP_ANY) && opc1 != 0) ||

                    ((r->opc2 == CP_ANY) && opc2 != 0)) {

                    r2->type |= ARM_CP_NO_MIGRATE;

                }



                

                if (!(r->type & ARM_CP_OVERRIDE)) {

                    ARMCPRegInfo *oldreg;

                    oldreg = g_hash_table_lookup(cpu->cp_regs, key);

                    if (oldreg && !(oldreg->type & ARM_CP_OVERRIDE)) {

                        fprintf(stderr, "Register redefined: cp=%d %d bit "

                                "crn=%d crm=%d opc1=%d opc2=%d, "

                                "was %s, now %s\n", r2->cp, 32 + 32 * is64,

                                r2->crn, r2->crm, r2->opc1, r2->opc2,

                                oldreg->name, r2->name);

                        g_assert_not_reached();

                    }

                }

                g_hash_table_insert(cpu->cp_regs, key, r2);

            }

        }

    }

}