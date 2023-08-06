static int tcg_match_xori(TCGType type, tcg_target_long val)

{

    if ((s390_facilities & FACILITY_EXT_IMM) == 0) {

        return 0;

    }



    if (type == TCG_TYPE_I32) {

        

        return 1;

    }



    

    if (val < 0 && val == (int32_t)val) {

        return 0;

    }



    return 1;

}