static void vnc_dpy_update(DisplayState *ds, int x, int y, int w, int h)

{

    VncState *vs = ds->opaque;

    int i;



    h += y;



    

    w += (x % 16);

    x -= (x % 16);



    x = MIN(x, vs->width);

    y = MIN(y, vs->height);

    w = MIN(x + w, vs->width) - x;

    h = MIN(h, vs->height);



    for (; y < h; y++)

	for (i = 0; i < w; i += 16)

	    vnc_set_bit(vs->dirty_row[y], (x + i) / 16);

}