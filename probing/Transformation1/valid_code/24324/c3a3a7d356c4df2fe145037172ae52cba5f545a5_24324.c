int kvm_arch_init(KVMState *s, int smp_cpus)

{

    int ret;



    struct utsname utsname;



    uname(&utsname);

    lm_capable_kernel = strcmp(utsname.machine, "x86_64") == 0;



    

    ret = kvm_check_extension(s, KVM_CAP_SET_TSS_ADDR);

    if (ret <= 0) {

        fprintf(stderr, "kvm does not support KVM_CAP_SET_TSS_ADDR\n");

        return ret;

    }



    

    

    if (e820_add_entry(0xfffbc000, 0x4000, E820_RESERVED) < 0) {

        perror("e820_add_entry() table is full");

        exit(1);

    }

    ret = kvm_vm_ioctl(s, KVM_SET_TSS_ADDR, 0xfffbd000);

    if (ret < 0) {

        return ret;

    }



    return kvm_init_identity_map_page(s);

}