void qemu_co_queue_init(CoQueue *queue)

{

    QTAILQ_INIT(&queue->entries);



    

    queue->ctx = qemu_get_aio_context();

}