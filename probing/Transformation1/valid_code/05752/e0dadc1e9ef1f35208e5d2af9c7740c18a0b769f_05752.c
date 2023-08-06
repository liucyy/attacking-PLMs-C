AUXReply aux_request(AUXBus *bus, AUXCommand cmd, uint32_t address,

                      uint8_t len, uint8_t *data)

{

    AUXReply ret = AUX_NACK;

    I2CBus *i2c_bus = aux_get_i2c_bus(bus);

    size_t i;

    bool is_write = false;



    DPRINTF("request at address 0x%" PRIX32 ", command %u, len %u\n", address,

            cmd, len);



    switch (cmd) {

    

    case WRITE_AUX:

    case READ_AUX:

        is_write = cmd == READ_AUX ? false : true;

        for (i = 0; i < len; i++) {

            if (!address_space_rw(&bus->aux_addr_space, address++,

                                  MEMTXATTRS_UNSPECIFIED, data++, 1,

                                  is_write)) {

                ret = AUX_I2C_ACK;

            } else {

                ret = AUX_NACK;

                break;

            }

        }

        break;

    

    case READ_I2C:

    case WRITE_I2C:

        is_write = cmd == READ_I2C ? false : true;

        if (i2c_bus_busy(i2c_bus)) {

            i2c_end_transfer(i2c_bus);

        }



        if (i2c_start_transfer(i2c_bus, address, is_write)) {

            ret = AUX_I2C_NACK;

            break;

        }



        ret = AUX_I2C_ACK;

        while (len > 0) {

            if (i2c_send_recv(i2c_bus, data++, is_write) < 0) {

                ret = AUX_I2C_NACK;

                break;

            }

            len--;

        }

        i2c_end_transfer(i2c_bus);

        break;

    

    case WRITE_I2C_MOT:

    case READ_I2C_MOT:

        is_write = cmd == READ_I2C_MOT ? false : true;

        ret = AUX_I2C_NACK;

        if (!i2c_bus_busy(i2c_bus)) {

            

            if (i2c_start_transfer(i2c_bus, address, is_write)) {

                break;

            }

        } else if ((address != bus->last_i2c_address) ||

                   (bus->last_transaction != cmd)) {

            

            i2c_end_transfer(i2c_bus);

            if (i2c_start_transfer(i2c_bus, address, is_write)) {

                break;

            }

        }



        bus->last_transaction = cmd;

        bus->last_i2c_address = address;

        while (len > 0) {

            if (i2c_send_recv(i2c_bus, data++, is_write) < 0) {

                i2c_end_transfer(i2c_bus);

                break;

            }

            len--;

        }

        if (len == 0) {

            ret = AUX_I2C_ACK;

        }

        break;

    default:

        DPRINTF("Not implemented!\n");

        return AUX_NACK;

    }



    DPRINTF("reply: %u\n", ret);

    return ret;

}