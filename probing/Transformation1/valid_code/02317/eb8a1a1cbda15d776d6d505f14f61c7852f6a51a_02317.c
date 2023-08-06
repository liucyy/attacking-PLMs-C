static void test_leak_bucket(void)
{
    throttle_config_init(&cfg);
    bkt = cfg.buckets[THROTTLE_BPS_TOTAL];
    
    bkt.avg = 150;
    bkt.max = 15;
    bkt.level = 1.5;
    
    throttle_leak_bucket(&bkt, NANOSECONDS_PER_SECOND / 150);
    g_assert(bkt.avg == 150);
    g_assert(bkt.max == 15);
    g_assert(double_cmp(bkt.level, 0.5));
    
    throttle_leak_bucket(&bkt, NANOSECONDS_PER_SECOND / 150);
    g_assert(bkt.avg == 150);
    g_assert(bkt.max == 15);
    g_assert(double_cmp(bkt.level, 0));
    
    throttle_leak_bucket(&bkt, NANOSECONDS_PER_SECOND / 150);
    g_assert(bkt.avg == 150);
    g_assert(bkt.max == 15);
    g_assert(double_cmp(bkt.level, 0));
}