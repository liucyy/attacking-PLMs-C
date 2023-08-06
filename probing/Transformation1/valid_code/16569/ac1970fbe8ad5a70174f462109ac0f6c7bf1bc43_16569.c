static void phys_page_set(target_phys_addr_t index, target_phys_addr_t nb,

                          uint16_t leaf)

{

    

    phys_map_node_reserve(3 * P_L2_LEVELS);



    phys_page_set_level(&phys_map, &index, &nb, leaf, P_L2_LEVELS - 1);

}