static void monitor_control_read(void *opaque, const uint8_t *buf, int size)

{

    Monitor *old_mon = cur_mon;



    cur_mon = opaque;



    



    cur_mon = old_mon;

}