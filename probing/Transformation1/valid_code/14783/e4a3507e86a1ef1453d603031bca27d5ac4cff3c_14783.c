qcrypto_block_luks_open(QCryptoBlock *block,

                        QCryptoBlockOpenOptions *options,

                        QCryptoBlockReadFunc readfunc,

                        void *opaque,

                        unsigned int flags,

                        Error **errp)

{

    QCryptoBlockLUKS *luks;

    Error *local_err = NULL;

    int ret = 0;

    size_t i;

    ssize_t rv;

    uint8_t *masterkey = NULL;

    size_t masterkeylen;

    char *ivgen_name, *ivhash_name;

    QCryptoCipherMode ciphermode;

    QCryptoCipherAlgorithm cipheralg;

    QCryptoIVGenAlgorithm ivalg;

    QCryptoCipherAlgorithm ivcipheralg;

    QCryptoHashAlgorithm hash;

    QCryptoHashAlgorithm ivhash;

    char *password = NULL;



    if (!(flags & QCRYPTO_BLOCK_OPEN_NO_IO)) {

        if (!options->u.luks.key_secret) {

            error_setg(errp, "Parameter 'key-secret' is required for cipher");

            return -1;

        }

        password = qcrypto_secret_lookup_as_utf8(

            options->u.luks.key_secret, errp);

        if (!password) {

            return -1;

        }

    }



    luks = g_new0(QCryptoBlockLUKS, 1);

    block->opaque = luks;



    

    rv = readfunc(block, opaque, 0,

                  (uint8_t *)&luks->header,

                  sizeof(luks->header),

                  errp);

    if (rv < 0) {

        ret = rv;

        goto fail;

    }



    

    be16_to_cpus(&luks->header.version);

    be32_to_cpus(&luks->header.payload_offset);

    be32_to_cpus(&luks->header.key_bytes);

    be32_to_cpus(&luks->header.master_key_iterations);



    for (i = 0; i < QCRYPTO_BLOCK_LUKS_NUM_KEY_SLOTS; i++) {

        be32_to_cpus(&luks->header.key_slots[i].active);

        be32_to_cpus(&luks->header.key_slots[i].iterations);

        be32_to_cpus(&luks->header.key_slots[i].key_offset);

        be32_to_cpus(&luks->header.key_slots[i].stripes);

    }



    if (memcmp(luks->header.magic, qcrypto_block_luks_magic,

               QCRYPTO_BLOCK_LUKS_MAGIC_LEN) != 0) {

        error_setg(errp, "Volume is not in LUKS format");

        ret = -EINVAL;

        goto fail;

    }

    if (luks->header.version != QCRYPTO_BLOCK_LUKS_VERSION) {

        error_setg(errp, "LUKS version %" PRIu32 " is not supported",

                   luks->header.version);

        ret = -ENOTSUP;

        goto fail;

    }



    

    ivgen_name = strchr(luks->header.cipher_mode, '-');

    if (!ivgen_name) {

        ret = -EINVAL;

        error_setg(errp, "Unexpected cipher mode string format %s",

                   luks->header.cipher_mode);

        goto fail;

    }

    *ivgen_name = '\0';

    ivgen_name++;



    ivhash_name = strchr(ivgen_name, ':');

    if (!ivhash_name) {

        ivhash = 0;

    } else {

        *ivhash_name = '\0';

        ivhash_name++;



        ivhash = qcrypto_block_luks_hash_name_lookup(ivhash_name,

                                                     &local_err);

        if (local_err) {

            ret = -ENOTSUP;

            error_propagate(errp, local_err);

            goto fail;

        }

    }



    ciphermode = qcrypto_block_luks_cipher_mode_lookup(luks->header.cipher_mode,

                                                       &local_err);

    if (local_err) {

        ret = -ENOTSUP;

        error_propagate(errp, local_err);

        goto fail;

    }



    cipheralg = qcrypto_block_luks_cipher_name_lookup(luks->header.cipher_name,

                                                      ciphermode,

                                                      luks->header.key_bytes,

                                                      &local_err);

    if (local_err) {

        ret = -ENOTSUP;

        error_propagate(errp, local_err);

        goto fail;

    }



    hash = qcrypto_block_luks_hash_name_lookup(luks->header.hash_spec,

                                               &local_err);

    if (local_err) {

        ret = -ENOTSUP;

        error_propagate(errp, local_err);

        goto fail;

    }



    ivalg = qcrypto_block_luks_ivgen_name_lookup(ivgen_name,

                                                 &local_err);

    if (local_err) {

        ret = -ENOTSUP;

        error_propagate(errp, local_err);

        goto fail;

    }



    if (ivalg == QCRYPTO_IVGEN_ALG_ESSIV) {

        if (!ivhash_name) {

            ret = -EINVAL;

            error_setg(errp, "Missing IV generator hash specification");

            goto fail;

        }

        ivcipheralg = qcrypto_block_luks_essiv_cipher(cipheralg,

                                                      ivhash,

                                                      &local_err);

        if (local_err) {

            ret = -ENOTSUP;

            error_propagate(errp, local_err);

            goto fail;

        }

    } else {

        

        ivcipheralg = cipheralg;

    }



    if (!(flags & QCRYPTO_BLOCK_OPEN_NO_IO)) {

        

        if (qcrypto_block_luks_find_key(block,

                                        password,

                                        cipheralg, ciphermode,

                                        hash,

                                        ivalg,

                                        ivcipheralg,

                                        ivhash,

                                        &masterkey, &masterkeylen,

                                        readfunc, opaque,

                                        errp) < 0) {

            ret = -EACCES;

            goto fail;

        }



        

        block->kdfhash = hash;

        block->niv = qcrypto_cipher_get_iv_len(cipheralg,

                                               ciphermode);

        block->ivgen = qcrypto_ivgen_new(ivalg,

                                         ivcipheralg,

                                         ivhash,

                                         masterkey, masterkeylen,

                                         errp);

        if (!block->ivgen) {

            ret = -ENOTSUP;

            goto fail;

        }



        block->cipher = qcrypto_cipher_new(cipheralg,

                                           ciphermode,

                                           masterkey, masterkeylen,

                                           errp);

        if (!block->cipher) {

            ret = -ENOTSUP;

            goto fail;

        }

    }



    block->payload_offset = luks->header.payload_offset *

        QCRYPTO_BLOCK_LUKS_SECTOR_SIZE;



    luks->cipher_alg = cipheralg;

    luks->cipher_mode = ciphermode;

    luks->ivgen_alg = ivalg;

    luks->ivgen_hash_alg = ivhash;

    luks->hash_alg = hash;



    g_free(masterkey);

    g_free(password);



    return 0;



 fail:

    g_free(masterkey);

    qcrypto_cipher_free(block->cipher);

    qcrypto_ivgen_free(block->ivgen);

    g_free(luks);

    g_free(password);

    return ret;

}