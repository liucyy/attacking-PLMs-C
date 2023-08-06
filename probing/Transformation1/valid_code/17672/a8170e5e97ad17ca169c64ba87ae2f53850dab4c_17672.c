static void icp_control_write(void *opaque, target_phys_addr_t offset,

                          uint64_t value, unsigned size)

{

    switch (offset >> 2) {

    case 1: 

    case 2: 

    case 3: 

        

        break;

    default:

        hw_error("icp_control_write: Bad offset %x\n", (int)offset);

    }

}