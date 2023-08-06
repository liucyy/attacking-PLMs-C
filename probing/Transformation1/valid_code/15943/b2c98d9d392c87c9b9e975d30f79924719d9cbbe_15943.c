static int tcg_match_ori(TCGType type, tcg_target_long val)

{

    if (facilities & FACILITY_EXT_IMM) {

        if (type == TCG_TYPE_I32) {

            

            return 1;

        }

    }



    

    if (val < 0) {

        if (val == (int16_t)val) {

            return 0;

        }

        if (facilities & FACILITY_EXT_IMM) {

            if (val == (int32_t)val) {

                return 0;

            }

        }

    }



    return 1;

}