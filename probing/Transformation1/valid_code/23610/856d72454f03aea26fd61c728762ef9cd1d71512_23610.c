void address_space_destroy(AddressSpace *as)

{

    

    memory_region_transaction_begin();

    as->root = NULL;

    memory_region_transaction_commit();

    QTAILQ_REMOVE(&address_spaces, as, address_spaces_link);

    address_space_destroy_dispatch(as);

    flatview_destroy(as->current_map);

    g_free(as->name);

    g_free(as->current_map);

    g_free(as->ioeventfds);

}