static void virtio_crypto_instance_init(Object *obj)

{

    VirtIOCrypto *vcrypto = VIRTIO_CRYPTO(obj);



    

    vcrypto->config_size = sizeof(struct virtio_crypto_config);



    object_property_add_link(obj, "cryptodev",

                             TYPE_CRYPTODEV_BACKEND,

                             (Object **)&vcrypto->conf.cryptodev,

                             virtio_crypto_check_cryptodev_is_used,

                             OBJ_PROP_LINK_UNREF_ON_RELEASE, NULL);

}