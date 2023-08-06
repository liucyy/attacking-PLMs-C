void helper_st_asi(CPUSPARCState *env, target_ulong addr, target_ulong val,

                   int asi, int size)

{

#ifdef DEBUG_ASI

    dump_asi("write", addr, asi, size, val);

#endif

    if (asi < 0x80) {

        helper_raise_exception(env, TT_PRIV_ACT);

    }



    helper_check_align(env, addr, size - 1);

    addr = asi_address_mask(env, asi, addr);



    

    switch (asi) {

    case 0x88: 

    case 0x89: 

        switch (size) {

        case 2:

            val = bswap16(val);

            break;

        case 4:

            val = bswap32(val);

            break;

        case 8:

            val = bswap64(val);

            break;

        default:

            break;

        }

    default:

        break;

    }



    switch (asi) {

    case 0x80: 

    case 0x88: 

        {

            switch (size) {

            case 1:

                stb_raw(addr, val);

                break;

            case 2:

                stw_raw(addr, val);

                break;

            case 4:

                stl_raw(addr, val);

                break;

            case 8:

            default:

                stq_raw(addr, val);

                break;

            }

        }

        break;

    case 0x81: 

    case 0x89: 

        

        return;



    case 0x82: 

    case 0x83: 

    case 0x8a: 

    case 0x8b: 

    default:

        helper_raise_exception(env, TT_DATA_ACCESS);

        return;

    }

}