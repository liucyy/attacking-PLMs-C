static void add_cpreg_to_hashtable(ARMCPU *cpu, const ARMCPRegInfo *r,

                                   void *opaque, int state, int secstate,

                                   int crm, int opc1, int opc2)

{

    

    uint32_t *key = g_new(uint32_t, 1);

    ARMCPRegInfo *r2 = g_memdup(r, sizeof(ARMCPRegInfo));

    int is64 = (r->type & ARM_CP_64BIT) ? 1 : 0;

    int ns = (secstate & ARM_CP_SECSTATE_NS) ? 1 : 0;



    

    r2->secure = secstate;



    if (r->bank_fieldoffsets[0] && r->bank_fieldoffsets[1]) {

        

        r2->fieldoffset = r->bank_fieldoffsets[ns];

    }



    if (state == ARM_CP_STATE_AA32) {

        if (r->bank_fieldoffsets[0] && r->bank_fieldoffsets[1]) {

            

            if ((r->state == ARM_CP_STATE_BOTH && ns) ||

                (arm_feature(&cpu->env, ARM_FEATURE_V8) && !ns)) {

                r2->type |= ARM_CP_NO_MIGRATE;

                r2->resetfn = arm_cp_reset_ignore;

            }

        } else if ((secstate != r->secure) && !ns) {

            

            r2->type |= ARM_CP_NO_MIGRATE;

            r2->resetfn = arm_cp_reset_ignore;

        }



        if (r->state == ARM_CP_STATE_BOTH) {

            

            if (r2->cp == 0) {

                r2->cp = 15;

            }



#ifdef HOST_WORDS_BIGENDIAN

            if (r2->fieldoffset) {

                r2->fieldoffset += sizeof(uint32_t);

            }

#endif

        }

    }

    if (state == ARM_CP_STATE_AA64) {

        

        if (r->cp == 0 || r->state == ARM_CP_STATE_BOTH) {

            r2->cp = CP_REG_ARM64_SYSREG_CP;

        }

        *key = ENCODE_AA64_CP_REG(r2->cp, r2->crn, crm,

                                  r2->opc0, opc1, opc2);

    } else {

        *key = ENCODE_CP_REG(r2->cp, is64, ns, r2->crn, crm, opc1, opc2);

    }

    if (opaque) {

        r2->opaque = opaque;

    }

    

    r2->state = state;

    

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