static void decode_sublayer_hrd(HEVCContext *s, int nb_cpb,

                                int subpic_params_present)

{

    GetBitContext *gb = &s->HEVClc.gb;

    int i;



    for (i = 0; i < nb_cpb; i++) {

        get_ue_golomb_long(gb); 

        get_ue_golomb_long(gb); 



        if (subpic_params_present) {

            get_ue_golomb_long(gb); 

            get_ue_golomb_long(gb); 

        }

        skip_bits1(gb); 

    }

}