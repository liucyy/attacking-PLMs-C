struct tm *brktimegm(time_t secs, struct tm *tm)

{

    int days, y, ny, m;

    int md[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };



    days = secs / 86400;

    secs %= 86400;

    tm->tm_hour = secs / 3600;

    tm->tm_min = (secs % 3600) / 60;

    tm->tm_sec =  secs % 60;



    

    y = 1970; 

    while (days >= (ISLEAP(y)?366:365)) {

        ny = (y + days/366);

        days -= (ny - y) * 365 + LEAPS_COUNT(ny - 1) - LEAPS_COUNT(y - 1);

        y = ny;

    }

    md[1] = ISLEAP(y)?29:28;

    for (m=0; days >= md[m]; m++)

         days -= md[m];



    tm->tm_year = y;  

    tm->tm_mon = m+1; 

    tm->tm_mday = days+1;



    return tm;

}