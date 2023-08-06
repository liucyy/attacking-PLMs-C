void qio_channel_socket_connect_async(QIOChannelSocket *ioc,

                                      SocketAddressLegacy *addr,

                                      QIOTaskFunc callback,

                                      gpointer opaque,

                                      GDestroyNotify destroy)

{

    QIOTask *task = qio_task_new(

        OBJECT(ioc), callback, opaque, destroy);

    SocketAddressLegacy *addrCopy;



    addrCopy = QAPI_CLONE(SocketAddressLegacy, addr);



    

    trace_qio_channel_socket_connect_async(ioc, addr);

    qio_task_run_in_thread(task,

                           qio_channel_socket_connect_worker,

                           addrCopy,

                           (GDestroyNotify)qapi_free_SocketAddressLegacy);

}