static void tcg_out_opc(TCGContext *s, int opc, int r, int rm, int x)

{

    int rex;



    if (opc & P_GS) {

        tcg_out8(s, 0x65);

    }

    if (opc & P_DATA16) {

        

        assert((opc & P_REXW) == 0);

        tcg_out8(s, 0x66);

    }

    if (opc & P_ADDR32) {

        tcg_out8(s, 0x67);

    }



    rex = 0;

    rex |= (opc & P_REXW) ? 0x8 : 0x0;  

    rex |= (r & 8) >> 1;                

    rex |= (x & 8) >> 2;                

    rex |= (rm & 8) >> 3;               



    

    rex |= opc & (r >= 4 ? P_REXB_R : 0);

    rex |= opc & (rm >= 4 ? P_REXB_RM : 0);



    if (rex) {

        tcg_out8(s, (uint8_t)(rex | 0x40));

    }



    if (opc & (P_EXT | P_EXT38)) {

        tcg_out8(s, 0x0f);

        if (opc & P_EXT38) {

            tcg_out8(s, 0x38);

        }

    }



    tcg_out8(s, opc);

}