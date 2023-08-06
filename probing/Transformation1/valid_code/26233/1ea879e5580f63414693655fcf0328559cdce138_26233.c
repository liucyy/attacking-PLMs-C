static void sdl_callback (void *opaque, Uint8 *buf, int len)

{

    SDLVoiceOut *sdl = opaque;

    SDLAudioState *s = &glob_sdl;

    HWVoiceOut *hw = &sdl->hw;

    int samples = len >> hw->info.shift;



    if (s->exit) {

        return;

    }



    while (samples) {

        int to_mix, decr;



        

        sdl_wait (s, "sdl_callback");

        if (s->exit) {

            return;

        }



        if (sdl_lock (s, "sdl_callback")) {

            return;

        }



        if (audio_bug (AUDIO_FUNC, sdl->live < 0 || sdl->live > hw->samples)) {

            dolog ("sdl->live=%d hw->samples=%d\n",

                   sdl->live, hw->samples);

            return;

        }



        if (!sdl->live) {

            goto again;

        }



        

        to_mix = audio_MIN (samples, sdl->live);

        decr = to_mix;

        while (to_mix) {

            int chunk = audio_MIN (to_mix, hw->samples - hw->rpos);

            st_sample_t *src = hw->mix_buf + hw->rpos;



            

            hw->clip (buf, src, chunk);

            sdl->rpos = (sdl->rpos + chunk) % hw->samples;

            to_mix -= chunk;

            buf += chunk << hw->info.shift;

        }

        samples -= decr;

        sdl->live -= decr;

        sdl->decr += decr;



    again:

        if (sdl_unlock (s, "sdl_callback")) {

            return;

        }

    }

    

}