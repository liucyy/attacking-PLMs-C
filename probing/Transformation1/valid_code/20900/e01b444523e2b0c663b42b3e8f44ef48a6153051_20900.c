static int ppc_hash64_pp_check(int key, int pp, bool nx)

{

    int access;



    

    

    access = 0;

    if (key == 0) {

        switch (pp) {

        case 0x0:

        case 0x1:

        case 0x2:

            access |= PAGE_WRITE;

            

        case 0x3:

        case 0x6:

            access |= PAGE_READ;

            break;

        }

    } else {

        switch (pp) {

        case 0x0:

        case 0x6:

            access = 0;

            break;

        case 0x1:

        case 0x3:

            access = PAGE_READ;

            break;

        case 0x2:

            access = PAGE_READ | PAGE_WRITE;

            break;

        }

    }

    if (!nx) {

        access |= PAGE_EXEC;

    }



    return access;

}