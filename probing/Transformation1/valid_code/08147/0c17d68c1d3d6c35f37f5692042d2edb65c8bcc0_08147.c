static int get_phys_addr_v5(CPUARMState *env, uint32_t address, int access_type,

                            int is_user, hwaddr *phys_ptr,

                            int *prot, target_ulong *page_size)

{

    CPUState *cs = CPU(arm_env_get_cpu(env));

    int code;

    uint32_t table;

    uint32_t desc;

    int type;

    int ap;

    int domain = 0;

    int domain_prot;

    hwaddr phys_addr;



    

    

    if (!get_level1_table_address(env, &table, address)) {

        

        code = 5;

        goto do_fault;

    }

    desc = ldl_phys(cs->as, table);

    type = (desc & 3);

    domain = (desc >> 5) & 0x0f;

    domain_prot = (env->cp15.c3 >> (domain * 2)) & 3;

    if (type == 0) {

        

        code = 5;

        goto do_fault;

    }

    if (domain_prot == 0 || domain_prot == 2) {

        if (type == 2)

            code = 9; 

        else

            code = 11; 

        goto do_fault;

    }

    if (type == 2) {

        

        phys_addr = (desc & 0xfff00000) | (address & 0x000fffff);

        ap = (desc >> 10) & 3;

        code = 13;

        *page_size = 1024 * 1024;

    } else {

        

	if (type == 1) {

	    

	    table = (desc & 0xfffffc00) | ((address >> 10) & 0x3fc);

	} else {

	    

	    table = (desc & 0xfffff000) | ((address >> 8) & 0xffc);

	}

        desc = ldl_phys(cs->as, table);

        switch (desc & 3) {

        case 0: 

            code = 7;

            goto do_fault;

        case 1: 

            phys_addr = (desc & 0xffff0000) | (address & 0xffff);

            ap = (desc >> (4 + ((address >> 13) & 6))) & 3;

            *page_size = 0x10000;

            break;

        case 2: 

            phys_addr = (desc & 0xfffff000) | (address & 0xfff);

            ap = (desc >> (4 + ((address >> 9) & 6))) & 3;

            *page_size = 0x1000;

            break;

        case 3: 

	    if (type == 1) {

		if (arm_feature(env, ARM_FEATURE_XSCALE)) {

		    phys_addr = (desc & 0xfffff000) | (address & 0xfff);

		} else {

		    

		    code = 7;

		    goto do_fault;

		}

	    } else {

		phys_addr = (desc & 0xfffffc00) | (address & 0x3ff);

	    }

            ap = (desc >> 4) & 3;

            *page_size = 0x400;

            break;

        default:

            

            abort();

        }

        code = 15;

    }

    *prot = check_ap(env, ap, domain_prot, access_type, is_user);

    if (!*prot) {

        

        goto do_fault;

    }

    *prot |= PAGE_EXEC;

    *phys_ptr = phys_addr;

    return 0;

do_fault:

    return code | (domain << 4);

}