uint16_t eeprom93xx_read(eeprom_t *eeprom)

{

    

    logout("CS=%u DO=%u\n", eeprom->eecs, eeprom->eedo);

    return (eeprom->eedo);

}