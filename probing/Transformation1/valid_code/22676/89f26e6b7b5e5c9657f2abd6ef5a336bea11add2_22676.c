static void fill_prefetch_fifo(struct omap_gpmc_s *s)
{
    
    int fptr;
    int cs = prefetch_cs(s->prefetch.config1);
    int is16bit = (((s->cs_file[cs].config[0] >> 12) & 3) != 0);
    int bytes;
    
    bytes = 64 - s->prefetch.fifopointer;
    if (bytes > s->prefetch.count) {
        bytes = s->prefetch.count;
    s->prefetch.count -= bytes;
    s->prefetch.fifopointer += bytes;
    fptr = 64 - s->prefetch.fifopointer;
    
    while (fptr < (64 - bytes)) {
        s->prefetch.fifo[fptr] = s->prefetch.fifo[fptr + bytes];
        fptr++;
    while (fptr < 64) {
            uint32_t v = omap_nand_read(&s->cs_file[cs], 0, 2);
            s->prefetch.fifo[fptr++] = v & 0xff;
            s->prefetch.fifo[fptr++] = (v >> 8) & 0xff;
        } else {
            s->prefetch.fifo[fptr++] = omap_nand_read(&s->cs_file[cs], 0, 1);
    if (s->prefetch.startengine && (s->prefetch.count == 0)) {
        
        s->irqst |= 2;
        s->prefetch.startengine = 0;
    
    if (s->prefetch.fifopointer != 0) {
        omap_gpmc_dma_update(s, 1);
    omap_gpmc_int_update(s);