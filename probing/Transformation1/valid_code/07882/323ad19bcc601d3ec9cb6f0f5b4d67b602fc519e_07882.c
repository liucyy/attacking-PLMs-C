static void dump_ppc_insns (CPUPPCState *env)

{

    opc_handler_t **table, *handler;

    const char *p, *q;

    uint8_t opc1, opc2, opc3;



    printf("Instructions set:\n");

    

    for (opc1 = 0x00; opc1 < PPC_CPU_OPCODES_LEN; opc1++) {

        table = env->opcodes;

        handler = table[opc1];

        if (is_indirect_opcode(handler)) {

            

            for (opc2 = 0; opc2 < PPC_CPU_INDIRECT_OPCODES_LEN; opc2++) {

                table = env->opcodes;

                handler = env->opcodes[opc1];

                table = ind_table(handler);

                handler = table[opc2];

                if (is_indirect_opcode(handler)) {

                    table = ind_table(handler);

                    

                    for (opc3 = 0; opc3 < PPC_CPU_INDIRECT_OPCODES_LEN;

                            opc3++) {

                        handler = table[opc3];

                        if (handler->handler != &gen_invalid) {

                            

                            p = strchr(handler->oname, '_');

                            if (p == NULL) {

                                printf("INSN: %02x %02x %02x (%02d %04d) : "

                                       "%s\n",

                                       opc1, opc2, opc3, opc1,

                                       (opc3 << 5) | opc2,

                                       handler->oname);

                            } else {

                                q = "speundef";

                                if ((p - handler->oname) != strlen(q) ||

                                    memcmp(handler->oname, q, strlen(q)) != 0) {

                                    

                                    printf("INSN: %02x %02x %02x (%02d %04d) : "

                                           "%.*s\n",

                                           opc1, opc2 << 1, opc3, opc1,

                                           (opc3 << 6) | (opc2 << 1),

                                           (int)(p - handler->oname),

                                           handler->oname);

                                }

                                if (strcmp(p + 1, q) != 0) {

                                    

                                    printf("INSN: %02x %02x %02x (%02d %04d) : "

                                           "%s\n",

                                           opc1, (opc2 << 1) | 1, opc3, opc1,

                                           (opc3 << 6) | (opc2 << 1) | 1,

                                           p + 1);

                                }

                            }

                        }

                    }

                } else {

                    if (handler->handler != &gen_invalid) {

                        printf("INSN: %02x %02x -- (%02d %04d) : %s\n",

                               opc1, opc2, opc1, opc2, handler->oname);

                    }

                }

            }

        } else {

            if (handler->handler != &gen_invalid) {

                printf("INSN: %02x -- -- (%02d ----) : %s\n",

                       opc1, opc1, handler->oname);

            }

        }

    }

}