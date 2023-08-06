paint_mouse_pointer(XImage *image, struct x11grab *s)

{

    int x_off = s->x_off;

    int y_off = s->y_off;

    int width = s->width;

    int height = s->height;

    Display *dpy = s->dpy;

    XFixesCursorImage *xcim;

    int x, y;

    int line, column;

    int to_line, to_column;

    int pixstride = image->bits_per_pixel >> 3;

    

    uint8_t *pix = image->data;

    Cursor c;

    Window w;

    XSetWindowAttributes attr;



    

    if (image->bits_per_pixel != 24 && image->bits_per_pixel != 32)

        return;



    c = XCreateFontCursor(dpy, XC_left_ptr);

    w = DefaultRootWindow(dpy);

    attr.cursor = c;

    XChangeWindowAttributes(dpy, w, CWCursor, &attr);



    xcim = XFixesGetCursorImage(dpy);



    x = xcim->x - xcim->xhot;

    y = xcim->y - xcim->yhot;



    to_line = FFMIN((y + xcim->height), (height + y_off));

    to_column = FFMIN((x + xcim->width), (width + x_off));



    for (line = FFMAX(y, y_off); line < to_line; line++) {

        for (column = FFMAX(x, x_off); column < to_column; column++) {

            int  xcim_addr = (line - y) * xcim->width + column - x;

            int image_addr = ((line - y_off) * width + column - x_off) * pixstride;

            int r = (uint8_t)(xcim->pixels[xcim_addr] >>  0);

            int g = (uint8_t)(xcim->pixels[xcim_addr] >>  8);

            int b = (uint8_t)(xcim->pixels[xcim_addr] >> 16);

            int a = (uint8_t)(xcim->pixels[xcim_addr] >> 24);



            if (a == 255) {

                pix[image_addr+0] = r;

                pix[image_addr+1] = g;

                pix[image_addr+2] = b;

            } else if (a) {

                

                pix[image_addr+0] = r + (pix[image_addr+0]*(255-a) + 255/2) / 255;

                pix[image_addr+1] = g + (pix[image_addr+1]*(255-a) + 255/2) / 255;

                pix[image_addr+2] = b + (pix[image_addr+2]*(255-a) + 255/2) / 255;

            }

        }

    }



    XFree(xcim);

    xcim = NULL;

}