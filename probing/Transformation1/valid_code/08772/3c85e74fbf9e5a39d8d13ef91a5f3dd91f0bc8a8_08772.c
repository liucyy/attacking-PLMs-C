int kvm_arch_init(KVMState *s)

{

    uint64_t identity_base = 0xfffbc000;

    int ret;

    struct utsname utsname;



    ret = kvm_get_supported_msrs(s);

    if (ret < 0) {

        return ret;

    }



    uname(&utsname);

    lm_capable_kernel = strcmp(utsname.machine, "x86_64") == 0;



    

#ifdef KVM_CAP_SET_IDENTITY_MAP_ADDR

    if (kvm_check_extension(s, KVM_CAP_SET_IDENTITY_MAP_ADDR)) {

        

        identity_base = 0xfeffc000;



        ret = kvm_vm_ioctl(s, KVM_SET_IDENTITY_MAP_ADDR, &identity_base);

        if (ret < 0) {

            return ret;

        }

    }

#endif

    

    ret = kvm_vm_ioctl(s, KVM_SET_TSS_ADDR, identity_base + 0x1000);

    if (ret < 0) {

        return ret;

    }



    

    ret = e820_add_entry(identity_base, 0x4000, E820_RESERVED);

    if (ret < 0) {

        fprintf(stderr, "e820_add_entry() table is full\n");

        return ret;

    }




    return 0;

}