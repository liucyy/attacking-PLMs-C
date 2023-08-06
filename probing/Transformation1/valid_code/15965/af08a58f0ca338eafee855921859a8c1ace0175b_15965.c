static void hypercall_register_types(void)

{

    

    spapr_register_hypercall(H_ENTER, h_enter);

    spapr_register_hypercall(H_REMOVE, h_remove);

    spapr_register_hypercall(H_PROTECT, h_protect);

    spapr_register_hypercall(H_READ, h_read);



    

    spapr_register_hypercall(H_BULK_REMOVE, h_bulk_remove);



    

    spapr_register_hypercall(H_SET_DABR, h_set_dabr);



    

    spapr_register_hypercall(H_REGISTER_VPA, h_register_vpa);

    spapr_register_hypercall(H_CEDE, h_cede);



    

    spapr_register_hypercall(H_SET_SPRG0, h_set_sprg0);

    spapr_register_hypercall(H_SET_MODE, h_set_mode);



    

    spapr_register_hypercall(H_LOGICAL_CI_LOAD, h_logical_load);

    spapr_register_hypercall(H_LOGICAL_CI_STORE, h_logical_store);

    spapr_register_hypercall(H_LOGICAL_CACHE_LOAD, h_logical_load);

    spapr_register_hypercall(H_LOGICAL_CACHE_STORE, h_logical_store);

    spapr_register_hypercall(H_LOGICAL_ICBI, h_logical_icbi);

    spapr_register_hypercall(H_LOGICAL_DCBF, h_logical_dcbf);

    spapr_register_hypercall(KVMPPC_H_LOGICAL_MEMOP, h_logical_memop);



    

    spapr_register_hypercall(KVMPPC_H_RTAS, h_rtas);



    

    spapr_register_hypercall(KVMPPC_H_CAS, h_client_architecture_support);

}