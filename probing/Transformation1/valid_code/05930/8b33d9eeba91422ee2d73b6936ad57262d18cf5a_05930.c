static int check_for_block_signature(BlockDriverState *bs, const uint8_t *buf)

{

    static const uint8_t signatures[][4] = {

        { 'Q', 'F', 'I', 0xfb }, 

        { 'C', 'O', 'W', 'D' }, 

        { 'V', 'M', 'D', 'K' }, 

        { 'O', 'O', 'O', 'M' }, 

        {}

    };

    int i;



    for (i = 0; signatures[i][0] != 0; i++) {

        if (memcmp(buf, signatures[i], 4) == 0) {

            return 1;

        }

    }



    return 0;

}