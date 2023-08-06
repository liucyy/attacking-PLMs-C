void cpu_x86_cpuid(CPUX86State *env, uint32_t index,

                   uint32_t *eax, uint32_t *ebx,

                   uint32_t *ecx, uint32_t *edx)

{

    

    if (index & 0x80000000) {

        if (index > env->cpuid_xlevel)

            index = env->cpuid_level;

    } else {

        if (index > env->cpuid_level)

            index = env->cpuid_level;

    }



    switch(index) {

    case 0:

        *eax = env->cpuid_level;

        *ebx = env->cpuid_vendor1;

        *edx = env->cpuid_vendor2;

        *ecx = env->cpuid_vendor3;



        

        if (kvm_enabled())

            host_cpuid(0, NULL, ebx, ecx, edx);

        break;

    case 1:

        *eax = env->cpuid_version;

        *ebx = (env->cpuid_apic_id << 24) | 8 << 8; 

        *ecx = env->cpuid_ext_features;

        *edx = env->cpuid_features;



        

        if (kvm_enabled())

            *ecx |= (1 << 31);

        break;

    case 2:

        

        *eax = 1;

        *ebx = 0;

        *ecx = 0;

        *edx = 0x2c307d;

        break;

    case 4:

        

        switch (*ecx) {

            case 0: 

                *eax = 0x0000121;

                *ebx = 0x1c0003f;

                *ecx = 0x000003f;

                *edx = 0x0000001;

                break;

            case 1: 

                *eax = 0x0000122;

                *ebx = 0x1c0003f;

                *ecx = 0x000003f;

                *edx = 0x0000001;

                break;

            case 2: 

                *eax = 0x0000143;

                *ebx = 0x3c0003f;

                *ecx = 0x0000fff;

                *edx = 0x0000001;

                break;

            default: 

                *eax = 0;

                *ebx = 0;

                *ecx = 0;

                *edx = 0;

                break;

        }



        break;

    case 5:

        

        *eax = 0; 

        *ebx = 0; 

        *ecx = CPUID_MWAIT_EMX | CPUID_MWAIT_IBE;

        *edx = 0;

        break;

    case 6:

        

        *eax = 0;

        *ebx = 0;

        *ecx = 0;

        *edx = 0;

        break;

    case 9:

        

        *eax = 0; 

        *ebx = 0;

        *ecx = 0;

        *edx = 0;

        break;

    case 0xA:

        

        *eax = 0;

        *ebx = 0;

        *ecx = 0;

        *edx = 0;

        break;

    case 0x80000000:

        *eax = env->cpuid_xlevel;

        *ebx = env->cpuid_vendor1;

        *edx = env->cpuid_vendor2;

        *ecx = env->cpuid_vendor3;

        break;

    case 0x80000001:

        *eax = env->cpuid_features;

        *ebx = 0;

        *ecx = env->cpuid_ext3_features;

        *edx = env->cpuid_ext2_features;



        if (kvm_enabled()) {

            uint32_t h_eax, h_edx;



            host_cpuid(0x80000001, &h_eax, NULL, NULL, &h_edx);



            



            

            if ((h_edx & 0x20000000) == 0 )

                *edx &= ~0x20000000;

            

            if ((h_edx & 0x00000800) == 0)

                *edx &= ~0x00000800;

            

            if ((h_edx & 0x00100000) == 0)

                *edx &= ~0x00100000;



            



            

            *ecx &= ~4UL;

            

            *edx = ~0xc0000000;

        }

        break;

    case 0x80000002:

    case 0x80000003:

    case 0x80000004:

        *eax = env->cpuid_model[(index - 0x80000002) * 4 + 0];

        *ebx = env->cpuid_model[(index - 0x80000002) * 4 + 1];

        *ecx = env->cpuid_model[(index - 0x80000002) * 4 + 2];

        *edx = env->cpuid_model[(index - 0x80000002) * 4 + 3];

        break;

    case 0x80000005:

        

        *eax = 0x01ff01ff;

        *ebx = 0x01ff01ff;

        *ecx = 0x40020140;

        *edx = 0x40020140;

        break;

    case 0x80000006:

        

        *eax = 0;

        *ebx = 0x42004200;

        *ecx = 0x02008140;

        *edx = 0;

        break;

    case 0x80000008:

        

 

        if (env->cpuid_ext2_features & CPUID_EXT2_LM) {

            

#if defined(USE_KQEMU)

            *eax = 0x00003020;	

#else



            *eax = 0x00003028;	

#endif

        } else {

#if defined(USE_KQEMU)

            *eax = 0x00000020;	

#else

            if (env->cpuid_features & CPUID_PSE36)

                *eax = 0x00000024; 

            else

                *eax = 0x00000020; 

#endif

        }

        *ebx = 0;

        *ecx = 0;

        *edx = 0;

        break;

    case 0x8000000A:

        *eax = 0x00000001; 

        *ebx = 0x00000010; 

        *ecx = 0;

        *edx = 0; 

        break;

    default:

        

        *eax = 0;

        *ebx = 0;

        *ecx = 0;

        *edx = 0;

        break;

    }

}