void eeprom93xx_write(eeprom_t *eeprom, int eecs, int eesk, int eedi)

{

    uint8_t tick = eeprom->tick;

    uint8_t eedo = eeprom->eedo;

    uint16_t address = eeprom->address;

    uint8_t command = eeprom->command;



    logout("CS=%u SK=%u DI=%u DO=%u, tick = %u\n",

           eecs, eesk, eedi, eedo, tick);



    if (! eeprom->eecs && eecs) {

        

        logout("Cycle start, waiting for 1st start bit (0)\n");

        tick = 0;

        command = 0x0;

        address = 0x0;

    } else if (eeprom->eecs && ! eecs) {

        

        if (eeprom->writable) {

            uint8_t subcommand = address >> (eeprom->addrbits - 2);

            if (command == 0 && subcommand == 2) {

                

                for (address = 0; address < eeprom->size; address++) {

                    eeprom->contents[address] = 0xffff;

                }

            } else if (command == 3) {

                

                eeprom->contents[address] = 0xffff;

            } else if (tick >= 2 + 2 + eeprom->addrbits + 16) {

                if (command == 1) {

                    

                    eeprom->contents[address] &= eeprom->data;

                } else if (command == 0 && subcommand == 1) {

                    

                    for (address = 0; address < eeprom->size; address++) {

                        eeprom->contents[address] &= eeprom->data;

                    }

                }

            }

        }

        

        eedo = 1;

    } else if (eecs && ! eeprom->eesk && eesk) {

        

        if (tick == 0) {

            

            if (eedi == 0) {

                logout("Got correct 1st start bit, waiting for 2nd start bit (1)\n");

                tick++;

            } else {

                logout("wrong 1st start bit (is 1, should be 0)\n");

                tick = 2;

                

            }

        } else if (tick == 1) {

            

            if (eedi != 0) {

                logout("Got correct 2nd start bit, getting command + address\n");

                tick++;

            } else {

                logout("1st start bit is longer than needed\n");

            }

        } else if (tick < 2 + 2) {

            

            tick++;

            command <<= 1;

            if (eedi) {

                command += 1;

            }

        } else if (tick < 2 + 2 + eeprom->addrbits) {

            

            tick++;

            address = ((address << 1) | eedi);

            if (tick == 2 + 2 + eeprom->addrbits) {

                logout("%s command, address = 0x%02x (value 0x%04x)\n",

                       opstring[command], address, eeprom->contents[address]);

                if (command == 2) {

                    eedo = 0;

                }

                address = address % eeprom->size;

                if (command == 0) {

                    

                    switch (address >> (eeprom->addrbits - 2)) {

                        case 0:

                            logout("write disable command\n");

                            eeprom->writable = 0;

                            break;

                        case 1:

                            logout("write all command\n");

                            break;

                        case 2:

                            logout("erase all command\n");

                            break;

                        case 3:

                            logout("write enable command\n");

                            eeprom->writable = 1;

                            break;

                    }

                } else {

                    

                    eeprom->data = eeprom->contents[address];

                }

            }

        } else if (tick < 2 + 2 + eeprom->addrbits + 16) {

            

            tick++;

            if (command == 2) {

                

                eedo = ((eeprom->data & 0x8000) != 0);

            }

            eeprom->data <<= 1;

            eeprom->data += eedi;

        } else {

            logout("additional unneeded tick, not processed\n");

        }

    }

    

    eeprom->tick = tick;

    eeprom->eecs = eecs;

    eeprom->eesk = eesk;

    eeprom->eedo = eedo;

    eeprom->address = address;

    eeprom->command = command;

}