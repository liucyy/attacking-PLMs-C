Aml *aml_arg(int pos)

{

    Aml *var;

    uint8_t op = 0x68  + pos;



    assert(pos <= 6);

    var = aml_opcode(op);

    return var;

}