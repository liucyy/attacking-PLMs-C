Aml *aml_local(int num)

{

    Aml *var;

    uint8_t op = 0x60  + num;



    assert(num <= 7);

    var = aml_opcode(op);

    return var;

}