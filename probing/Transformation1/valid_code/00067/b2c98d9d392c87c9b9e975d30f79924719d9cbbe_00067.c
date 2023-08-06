static int tcg_match_cmpi(TCGType type, tcg_target_long val)

{

    if (facilities & FACILITY_EXT_IMM) {

        

        if (type == TCG_TYPE_I32) {

            

            return 1;

        } else {

            

            

            return val >= 0 && val <= 0x7fffffff;

        }

    } else {

        

        return val == 0;

    }

}