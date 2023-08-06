static int qcow2_set_key(BlockDriverState *bs, const char *key)

{

    BDRVQcow2State *s = bs->opaque;

    uint8_t keybuf[16];

    int len, i;

    Error *err = NULL;



    memset(keybuf, 0, 16);

    len = strlen(key);

    if (len > 16)

        len = 16;

    

    for(i = 0;i < len;i++) {

        keybuf[i] = key[i];

    }

    assert(bs->encrypted);



    qcrypto_cipher_free(s->cipher);

    s->cipher = qcrypto_cipher_new(

        QCRYPTO_CIPHER_ALG_AES_128,

        QCRYPTO_CIPHER_MODE_CBC,

        keybuf, G_N_ELEMENTS(keybuf),

        &err);



    if (!s->cipher) {

        

        error_free(err);

        return -1;

    }

    return 0;

}