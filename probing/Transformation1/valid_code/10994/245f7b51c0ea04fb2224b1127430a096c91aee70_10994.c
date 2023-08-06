static int find_large_solid_color_rect(VncState *vs, int x, int y,

                                       int w, int h, int max_rows)

{

    int dx, dy, dw, dh;

    int n = 0;



    



    for (dy = y; dy < y + h; dy += VNC_TIGHT_MAX_SPLIT_TILE_SIZE) {



        



        if (dy - y >= max_rows) {

            n += send_rect_simple(vs, x, y, w, max_rows);

            y += max_rows;

            h -= max_rows;

        }



        dh = MIN(VNC_TIGHT_MAX_SPLIT_TILE_SIZE, (y + h - dy));



        for (dx = x; dx < x + w; dx += VNC_TIGHT_MAX_SPLIT_TILE_SIZE) {

            uint32_t color_value;

            int x_best, y_best, w_best, h_best;



            dw = MIN(VNC_TIGHT_MAX_SPLIT_TILE_SIZE, (x + w - dx));



            if (!check_solid_tile(vs, dx, dy, dw, dh, &color_value, false)) {

                continue ;

            }



            



            find_best_solid_area(vs, dx, dy, w - (dx - x), h - (dy - y),

                                 color_value, &w_best, &h_best);



            



            if (w_best * h_best != w * h &&

                w_best * h_best < VNC_TIGHT_MIN_SOLID_SUBRECT_SIZE) {

                continue;

            }



            



            x_best = dx; y_best = dy;

            extend_solid_area(vs, x, y, w, h, color_value,

                              &x_best, &y_best, &w_best, &h_best);



            



            if (y_best != y) {

                n += send_rect_simple(vs, x, y, w, y_best-y);

            }

            if (x_best != x) {

                n += vnc_tight_send_framebuffer_update(vs, x, y_best,

                                                       x_best-x, h_best);

            }



            

            n += send_sub_rect_solid(vs, x_best, y_best, w_best, h_best);



            



            if (x_best + w_best != x + w) {

                n += vnc_tight_send_framebuffer_update(vs, x_best+w_best,

                                                       y_best,

                                                       w-(x_best-x)-w_best,

                                                       h_best);

            }

            if (y_best + h_best != y + h) {

                n += vnc_tight_send_framebuffer_update(vs, x, y_best+h_best,

                                                       w, h-(y_best-y)-h_best);

            }



            

            return n;

        }

    }

    return n + send_rect_simple(vs, x, y, w, h);

}