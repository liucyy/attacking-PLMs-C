static bool logic_imm_decode_wmask(uint64_t *result, unsigned int immn,

                                   unsigned int imms, unsigned int immr)

{

    uint64_t mask;

    unsigned e, levels, s, r;

    int len;



    assert(immn < 2 && imms < 64 && immr < 64);



    



    

    len = 31 - clz32((immn << 6) | (~imms & 0x3f));

    if (len < 1) {

        

        return false;

    }

    e = 1 << len;



    levels = e - 1;

    s = imms & levels;

    r = immr & levels;



    if (s == levels) {

        

        return false;

    }



    

    mask = bitmask64(s + 1);

    mask = (mask >> r) | (mask << (e - r));

    

    mask = bitfield_replicate(mask, e);

    *result = mask;

    return true;

}