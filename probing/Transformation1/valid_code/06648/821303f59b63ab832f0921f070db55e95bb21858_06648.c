static void hypercall_init(void)
{
    
    spapr_register_hypercall(H_ENTER, h_enter);
    spapr_register_hypercall(H_REMOVE, h_remove);
    spapr_register_hypercall(H_PROTECT, h_protect);
    
    spapr_register_hypercall(KVMPPC_H_RTAS, h_rtas);
}