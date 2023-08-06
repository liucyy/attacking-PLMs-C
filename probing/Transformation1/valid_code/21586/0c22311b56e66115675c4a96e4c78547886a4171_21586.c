static void opt_pad_color(const char *arg) {

    

    int rgb = strtol(arg, NULL, 16);

    int r,g,b;



    r = (rgb >> 16);

    g = ((rgb >> 8) & 255);

    b = (rgb & 255);



    padcolor[0] = RGB_TO_Y(r,g,b);

    padcolor[1] = RGB_TO_U(r,g,b,0);

    padcolor[2] = RGB_TO_V(r,g,b,0);

}