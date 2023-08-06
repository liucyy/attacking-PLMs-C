static void hid_keyboard_process_keycode(HIDState *hs)
{
    uint8_t hid_code, index, key;
    int i, keycode, slot;
    if (hs->n == 0) {
        return;
    slot = hs->head & QUEUE_MASK; QUEUE_INCR(hs->head); hs->n--;
    keycode = hs->kbd.keycodes[slot];
    key = keycode & 0x7f;
    index = key | ((hs->kbd.modifiers & (1 << 8)) >> 1);
    hid_code = hid_usage_keys[index];
    hs->kbd.modifiers &= ~(1 << 8);
    switch (hid_code) {
    case 0x00:
        return;
    case 0xe0:
        assert(key == 0x1d);
        if (hs->kbd.modifiers & (1 << 9)) {
            
            hs->kbd.modifiers ^= (1 << 8) | (1 << 9);
            return;
        
    case 0xe1 ... 0xe7:
        
        if (keycode & (1 << 7)) {
            hs->kbd.modifiers &= ~(1 << (hid_code & 0x0f));
            return;
        
    case 0xe8 ... 0xe9:
        
        hs->kbd.modifiers |= 1 << (hid_code & 0x0f);
        return;
    case 0xea ... 0xef:
        abort();
    default:
        break;
    if (keycode & (1 << 7)) {
        for (i = hs->kbd.keys - 1; i >= 0; i--) {
            if (hs->kbd.key[i] == hid_code) {
                hs->kbd.key[i] = hs->kbd.key[-- hs->kbd.keys];
                hs->kbd.key[hs->kbd.keys] = 0x00;
                break;
        if (i < 0) {
            return;
    } else {
        for (i = hs->kbd.keys - 1; i >= 0; i--) {
            if (hs->kbd.key[i] == hid_code) {
                break;
        if (i < 0) {
            if (hs->kbd.keys < sizeof(hs->kbd.key)) {
                hs->kbd.key[hs->kbd.keys++] = hid_code;
        } else {
            return;