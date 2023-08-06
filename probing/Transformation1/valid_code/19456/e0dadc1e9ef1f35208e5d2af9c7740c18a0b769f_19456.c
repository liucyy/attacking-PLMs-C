static void aux_bus_class_init(ObjectClass *klass, void *data)

{

    BusClass *k = BUS_CLASS(klass);



    

    k->print_dev = aux_slave_dev_print;

}