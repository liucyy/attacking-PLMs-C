static void kvm_supported_msrs(CPUState *env)

{

    static int kvm_supported_msrs;

    int ret;



    

    if (kvm_supported_msrs == 0) {

        struct kvm_msr_list msr_list, *kvm_msr_list;



        kvm_supported_msrs = -1;



        

        msr_list.nmsrs = 0;

        ret = kvm_ioctl(env->kvm_state, KVM_GET_MSR_INDEX_LIST, &msr_list);

        if (ret < 0 && ret != -E2BIG) {

            return;

        }

        

        kvm_msr_list = qemu_mallocz(MAX(1024, sizeof(msr_list) +

                                              msr_list.nmsrs *

                                              sizeof(msr_list.indices[0])));



        kvm_msr_list->nmsrs = msr_list.nmsrs;

        ret = kvm_ioctl(env->kvm_state, KVM_GET_MSR_INDEX_LIST, kvm_msr_list);

        if (ret >= 0) {

            int i;



            for (i = 0; i < kvm_msr_list->nmsrs; i++) {

                if (kvm_msr_list->indices[i] == MSR_STAR) {

                    has_msr_star = 1;

                    continue;

                }

                if (kvm_msr_list->indices[i] == MSR_VM_HSAVE_PA) {

                    has_msr_hsave_pa = 1;

                    continue;

                }

            }

        }



        free(kvm_msr_list);

    }



    return;

}