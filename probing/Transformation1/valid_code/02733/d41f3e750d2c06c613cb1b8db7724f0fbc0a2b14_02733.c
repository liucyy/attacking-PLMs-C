void numa_cpu_pre_plug(const CPUArchId *slot, DeviceState *dev, Error **errp)

{

    int mapped_node_id; 

    int node_id = object_property_get_int(OBJECT(dev), "node-id", &error_abort);



    

    mapped_node_id = slot->props.node_id;

    if (!slot->props.has_node_id) {

        mapped_node_id = 0;

    }



    if (node_id == CPU_UNSET_NUMA_NODE_ID) {

        

        object_property_set_int(OBJECT(dev), mapped_node_id, "node-id", errp);

    } else if (node_id != mapped_node_id) {

        error_setg(errp, "node-id=%d must match numa node specified "

                   "with -numa option", node_id);

    }

}