static void vnc_update(VncState *vs, int x, int y, int w, int h)

{

    int i;



    h += y;



    

    w += (x % 16);

    x -= (x % 16);



    x = MIN(x, vs->serverds.width);

    y = MIN(y, vs->serverds.height);

    w = MIN(x + w, vs->serverds.width) - x;

    h = MIN(h, vs->serverds.height);



    for (; y < h; y++)

        for (i = 0; i < w; i += 16)

            vnc_set_bit(vs->dirty_row[y], (x + i) / 16);

}