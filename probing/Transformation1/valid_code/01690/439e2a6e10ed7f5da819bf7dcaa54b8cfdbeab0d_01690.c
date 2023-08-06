Aml *aml_index(Aml *arg1, Aml *idx)

{

    Aml *var = aml_opcode(0x88 );

    aml_append(var, arg1);

    aml_append(var, idx);

    build_append_byte(var->buf, 0x00 );

    return var;

}