int qcrypto_hash_bytesv(QCryptoHashAlgorithm alg,

                        const struct iovec *iov,

                        size_t niov,

                        uint8_t **result,

                        size_t *resultlen,

                        Error **errp)

{

#ifdef CONFIG_AF_ALG

    int ret;



    ret = qcrypto_hash_afalg_driver.hash_bytesv(alg, iov, niov,

                                                result, resultlen,

                                                errp);

    if (ret == 0) {

        return ret;

    }



    

    error_free(*errp);

#endif



    return qcrypto_hash_lib_driver.hash_bytesv(alg, iov, niov,

                                               result, resultlen,

                                               errp);

}