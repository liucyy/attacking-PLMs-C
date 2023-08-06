static void menelaus_pre_save(void *opaque)

{

    MenelausState *s = opaque;

    

    s->rtc_next_vmstate =  s->rtc.next - qemu_get_clock(rt_clock);

}