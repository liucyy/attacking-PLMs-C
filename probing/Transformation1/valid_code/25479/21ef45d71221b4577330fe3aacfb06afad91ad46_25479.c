static void vnc_dpy_update(DisplayChangeListener *dcl,

                           DisplayState *ds,

                           int x, int y, int w, int h)

{

    int i;

    VncDisplay *vd = ds->opaque;

    struct VncSurface *s = &vd->guest;

    int width = ds_get_width(ds);

    int height = ds_get_height(ds);



    h += y;



    

    w += (x % 16);

    x -= (x % 16);



    x = MIN(x, width);

    y = MIN(y, height);

    w = MIN(x + w, width) - x;

    h = MIN(h, height);



    for (; y < h; y++)

        for (i = 0; i < w; i += 16)

            set_bit((x + i) / 16, s->dirty[y]);

}