size_t mptsas_config_manufacturing_1(MPTSASState *s, uint8_t **data, int address)

{

    

    return MPTSAS_CONFIG_PACK(1, MPI_CONFIG_PAGETYPE_MANUFACTURING, 0x00,

                              "s256");

}