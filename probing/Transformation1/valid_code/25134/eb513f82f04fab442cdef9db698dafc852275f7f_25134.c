uint64_t helper_ld_asi(CPUSPARCState *env, target_ulong addr, int asi, int size,

                       int sign)

{

    uint64_t ret = 0;

#if defined(DEBUG_ASI)

    target_ulong last_addr = addr;

#endif



    if (asi < 0x80) {

        helper_raise_exception(env, TT_PRIV_ACT);

    }



    helper_check_align(env, addr, size - 1);

    addr = asi_address_mask(env, asi, addr);



    switch (asi) {

    case 0x82: 

    case 0x8a: 

        if (page_check_range(addr, size, PAGE_READ) == -1) {

#ifdef DEBUG_ASI

            dump_asi("read ", last_addr, asi, size, ret);

#endif

            return 0;

        }

        

    case 0x80: 

    case 0x88: 

        {

            switch (size) {

            case 1:

                ret = ldub_raw(addr);

                break;

            case 2:

                ret = lduw_raw(addr);

                break;

            case 4:

                ret = ldl_raw(addr);

                break;

            default:

            case 8:

                ret = ldq_raw(addr);

                break;

            }

        }

        break;

    case 0x83: 

    case 0x8b: 

        if (page_check_range(addr, size, PAGE_READ) == -1) {

#ifdef DEBUG_ASI

            dump_asi("read ", last_addr, asi, size, ret);

#endif

            return 0;

        }

        

    case 0x81: 

    case 0x89: 

        

        break;

    default:

        break;

    }



    

    switch (asi) {

    case 0x88: 

    case 0x89: 

    case 0x8a: 

    case 0x8b: 

        switch (size) {

        case 2:

            ret = bswap16(ret);

            break;

        case 4:

            ret = bswap32(ret);

            break;

        case 8:

            ret = bswap64(ret);

            break;

        default:

            break;

        }

    default:

        break;

    }



    

    if (sign) {

        switch (size) {

        case 1:

            ret = (int8_t) ret;

            break;

        case 2:

            ret = (int16_t) ret;

            break;

        case 4:

            ret = (int32_t) ret;

            break;

        default:

            break;

        }

    }

#ifdef DEBUG_ASI

    dump_asi("read ", last_addr, asi, size, ret);

#endif

    return ret;

}