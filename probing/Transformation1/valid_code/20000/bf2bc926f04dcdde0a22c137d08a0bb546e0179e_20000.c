static int decode_i_picture_header(VC9Context *v)

{

    GetBitContext *gb = &v->s.gb;

    int pqindex, status = 0;



    

    

    if (v->profile <= PROFILE_MAIN && get_bits(gb, 7))

    {

        av_log(v->s.avctx, AV_LOG_DEBUG, "I BufferFullness not 0\n");

    }



    

    pqindex = get_bits(gb, 5);

    if (v->quantizer_mode == QUANT_FRAME_IMPLICIT)

        v->pq = pquant_table[0][pqindex];

    else

    {

        v->pq = pquant_table[v->quantizer_mode-1][pqindex];

    }

    if (pqindex < 9) v->halfpq = get_bits(gb, 1);

    if (v->quantizer_mode == QUANT_FRAME_EXPLICIT)

        v->pquantizer = get_bits(gb, 1);

    av_log(v->s.avctx, AV_LOG_DEBUG, "I frame: QP=%i (+%i/2)\n",

           v->pq, v->halfpq);

#if HAS_ADVANCED_PROFILE

    if (v->profile <= PROFILE_MAIN)

#endif

    {

        if (v->extended_mv) v->mvrange = get_prefix(gb, 0, 3);

        if (v->multires) v->respic = get_bits(gb, 2);

    }

#if HAS_ADVANCED_PROFILE

    else

    {

        v->s.ac_pred = get_bits(gb, 1);

        if (v->postprocflag) v->postproc = get_bits(gb, 1);

        

        if (v->overlap && v->pq<9)

        {

            v->condover = get_bits(gb, 1);

            if (v->condover)

            {

                v->condover = 2+get_bits(gb, 1);

                if (v->condover == 3)

                {

                    status = bitplane_decoding(&v->over_flags_plane, v);

                    if (status < 0) return -1;

#if TRACE

                    av_log(v->s.avctx, AV_LOG_DEBUG, "Overflags plane encoding: "

                           "Imode: %i, Invert: %i\n", status>>1, status&1);

#endif

                }

            }

        }

    }

#endif



    

    return status;

}