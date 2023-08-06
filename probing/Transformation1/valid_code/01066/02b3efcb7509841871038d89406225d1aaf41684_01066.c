static void check_time(int wiggle)

{

    struct tm start, date[4], end;

    struct tm *datep;

    time_t ts;



    



    ts = time(NULL);

    gmtime_r(&ts, &start);



    cmos_get_date_time(&date[0]);

    cmos_get_date_time(&date[1]);

    cmos_get_date_time(&date[2]);

    cmos_get_date_time(&date[3]);



    ts = time(NULL);

    gmtime_r(&ts, &end);



    if (tm_cmp(&date[0], &date[1]) == 0) {

        datep = &date[0];

    } else if (tm_cmp(&date[1], &date[2]) == 0) {

        datep = &date[1];

    } else if (tm_cmp(&date[2], &date[3]) == 0) {

        datep = &date[2];

    } else {

        g_assert_not_reached();

    }



    if (!(tm_cmp(&start, datep) <= 0 && tm_cmp(datep, &end) <= 0)) {

        time_t t, s;



        start.tm_isdst = datep->tm_isdst;



        t = mktime(datep);

        s = mktime(&start);

        if (t < s) {

            g_test_message("RTC is %ld second(s) behind wall-clock\n", (s - t));

        } else {

            g_test_message("RTC is %ld second(s) ahead of wall-clock\n", (t - s));

        }



        g_assert_cmpint(ABS(t - s), <=, wiggle);

    }

}