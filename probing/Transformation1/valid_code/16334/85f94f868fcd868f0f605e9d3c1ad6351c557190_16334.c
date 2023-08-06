static void handle_keyup(DisplayState *ds, SDL_Event *ev)

{

    int mod_state;



    if (!alt_grab) {

        mod_state = (ev->key.keysym.mod & gui_grab_code);

    } else {

        mod_state = (ev->key.keysym.mod & (gui_grab_code | KMOD_LSHIFT));

    }

    if (!mod_state && gui_key_modifier_pressed) {

        gui_key_modifier_pressed = 0;

        if (gui_keysym == 0) {

            

            if (!gui_grab) {

                

                if (is_graphic_console() &&

                    SDL_GetAppState() & SDL_APPACTIVE) {

                    sdl_grab_start();

                }

            } else if (!gui_fullscreen) {

                sdl_grab_end();

            }

            

            reset_keys();

            return;

        }

        gui_keysym = 0;

    }

    if (is_graphic_console() && !gui_keysym) {

        sdl_process_key(&ev->key);

    }

}