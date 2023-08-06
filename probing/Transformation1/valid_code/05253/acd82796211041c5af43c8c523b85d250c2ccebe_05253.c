MemTxResult gicv3_redist_write(void *opaque, hwaddr offset, uint64_t data,
                               unsigned size, MemTxAttrs attrs)
{
    GICv3State *s = opaque;
    GICv3CPUState *cs;
    MemTxResult r;
    int cpuidx;
    
    cpuidx = offset / 0x20000;
    offset %= 0x20000;
    assert(cpuidx < s->num_cpu);
    cs = &s->cpu[cpuidx];
    switch (size) {
    case 1:
        r = gicr_writeb(cs, offset, data, attrs);
        break;
    case 4:
        r = gicr_writel(cs, offset, data, attrs);
        break;
    case 8:
        r = gicr_writell(cs, offset, data, attrs);
        break;
    default:
        r = MEMTX_ERROR;
        break;
    }
    if (r == MEMTX_ERROR) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: invalid guest write at offset " TARGET_FMT_plx
                      "size %u\n", __func__, offset, size);
        trace_gicv3_redist_badwrite(gicv3_redist_affid(cs), offset, data,
                                    size, attrs.secure);
    } else {
        trace_gicv3_redist_write(gicv3_redist_affid(cs), offset, data,
                                 size, attrs.secure);
    }
    return r;
}