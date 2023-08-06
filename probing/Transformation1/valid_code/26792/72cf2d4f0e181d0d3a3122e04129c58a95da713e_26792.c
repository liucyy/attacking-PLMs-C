int i2c_start_transfer(i2c_bus *bus, int address, int recv)

{

    DeviceState *qdev;

    i2c_slave *slave = NULL;



    LIST_FOREACH(qdev, &bus->qbus.children, sibling) {

        slave = I2C_SLAVE_FROM_QDEV(qdev);

        if (slave->address == address)

            break;

    }



    if (!slave)

        return 1;



    

    bus->current_dev = slave;

    slave->info->event(slave, recv ? I2C_START_RECV : I2C_START_SEND);

    return 0;

}