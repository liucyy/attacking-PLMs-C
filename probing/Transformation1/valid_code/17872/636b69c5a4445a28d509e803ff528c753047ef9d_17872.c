static int amr_nb_decode_frame(AVCodecContext * avctx,

            void *data, int *data_size,

            uint8_t * buf, int buf_size)

{

    AMRContext *s = avctx->priv_data;

    uint8_t*amrData=buf;

    int offset=0;

    UWord8 toc, q, ft;

    Word16 serial[SERIAL_FRAMESIZE];   

    Word16 *synth;

    UWord8 *packed_bits;

    static Word16 packed_size[16] = {12, 13, 15, 17, 19, 20, 26, 31, 5, 0, 0, 0, 0, 0, 0, 0};

    int i;



    



    synth=data;





    {

        toc=amrData[offset];

        

        q  = (toc >> 2) & 0x01;

        ft = (toc >> 3) & 0x0F;



        



        offset++;



        packed_bits=amrData+offset;



        offset+=packed_size[ft];



        

        s->rx_type = UnpackBits(q, ft, packed_bits, &s->mode, &serial[1]);



        

        s->frameCount++;



        if (s->rx_type == RX_NO_DATA)

        {

            s->mode = s->speech_decoder_state->prev_mode;

        }

        else {

            s->speech_decoder_state->prev_mode = s->mode;

        }



        

        if (s->reset_flag_old == 1)

        {

            

            s->reset_flag = decoder_homing_frame_test_first(&serial[1], s->mode);

        }

        

        if ((s->reset_flag != 0) && (s->reset_flag_old != 0))

        {

            for (i = 0; i < L_FRAME; i++)

            {

                synth[i] = EHF_MASK;

            }

        }

        else

        {

            

            Speech_Decode_Frame(s->speech_decoder_state, s->mode, &serial[1], s->rx_type, synth);

        }



        

        *data_size+=160*2;

        synth+=160;



        

        if (s->reset_flag_old == 0)

        {

            

            s->reset_flag = decoder_homing_frame_test(&serial[1], s->mode);

        }

        

        if (s->reset_flag != 0)

        {

            Speech_Decode_Frame_reset(s->speech_decoder_state);

        }

        s->reset_flag_old = s->reset_flag;



    }

    return offset;

}