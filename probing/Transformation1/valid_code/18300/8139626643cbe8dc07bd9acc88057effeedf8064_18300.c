uint32_t lm4549_write_samples(lm4549_state *s, uint32_t left, uint32_t right)

{

    



    if (s->buffer_level >= LM4549_BUFFER_SIZE) {

        DPRINTF("write_sample Buffer full\n");

        return 0;

    }



    

    s->buffer[s->buffer_level++] = (left >> 4);

    s->buffer[s->buffer_level++] = (right >> 4);



    if (s->buffer_level == LM4549_BUFFER_SIZE) {

        

        lm4549_audio_transfer(s);

    }



    return 1;

}