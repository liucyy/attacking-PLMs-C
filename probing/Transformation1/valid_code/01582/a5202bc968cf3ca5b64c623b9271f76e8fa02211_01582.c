static double bessel(double x){

    double lastv=0;

    double t, v;

    int i;

    static const double inv[100]={

 1.0/( 1* 1), 1.0/( 2* 2), 1.0/( 3* 3), 1.0/( 4* 4), 1.0/( 5* 5), 1.0/( 6* 6), 1.0/( 7* 7), 1.0/( 8* 8), 1.0/( 9* 9), 1.0/(10*10),

 1.0/(11*11), 1.0/(12*12), 1.0/(13*13), 1.0/(14*14), 1.0/(15*15), 1.0/(16*16), 1.0/(17*17), 1.0/(18*18), 1.0/(19*19), 1.0/(20*20),

 1.0/(21*21), 1.0/(22*22), 1.0/(23*23), 1.0/(24*24), 1.0/(25*25), 1.0/(26*26), 1.0/(27*27), 1.0/(28*28), 1.0/(29*29), 1.0/(30*30),

 1.0/(31*31), 1.0/(32*32), 1.0/(33*33), 1.0/(34*34), 1.0/(35*35), 1.0/(36*36), 1.0/(37*37), 1.0/(38*38), 1.0/(39*39), 1.0/(40*40),

 1.0/(41*41), 1.0/(42*42), 1.0/(43*43), 1.0/(44*44), 1.0/(45*45), 1.0/(46*46), 1.0/(47*47), 1.0/(48*48), 1.0/(49*49), 1.0/(50*50),

 1.0/(51*51), 1.0/(52*52), 1.0/(53*53), 1.0/(54*54), 1.0/(55*55), 1.0/(56*56), 1.0/(57*57), 1.0/(58*58), 1.0/(59*59), 1.0/(60*60),

 1.0/(61*61), 1.0/(62*62), 1.0/(63*63), 1.0/(64*64), 1.0/(65*65), 1.0/(66*66), 1.0/(67*67), 1.0/(68*68), 1.0/(69*69), 1.0/(70*70),

 1.0/(71*71), 1.0/(72*72), 1.0/(73*73), 1.0/(74*74), 1.0/(75*75), 1.0/(76*76), 1.0/(77*77), 1.0/(78*78), 1.0/(79*79), 1.0/(80*80),

 1.0/(81*81), 1.0/(82*82), 1.0/(83*83), 1.0/(84*84), 1.0/(85*85), 1.0/(86*86), 1.0/(87*87), 1.0/(88*88), 1.0/(89*89), 1.0/(90*90),

 1.0/(91*91), 1.0/(92*92), 1.0/(93*93), 1.0/(94*94), 1.0/(95*95), 1.0/(96*96), 1.0/(97*97), 1.0/(98*98), 1.0/(99*99), 1.0/(10000)

    };



    x= x*x/4;

    t = x;

    v = 1 + x;

    for(i=1; v != lastv; i+=2){

        t *= x*inv[i];

        v += t;

        lastv=v;

        t *= x*inv[i + 1];

        v += t;

        av_assert2(i<98);

    }

    return v;

}