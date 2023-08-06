void qemu_co_rwlock_unlock(CoRwlock *lock)

{

    assert(qemu_in_coroutine());

    if (lock->writer) {

        lock->writer = false;

        while (!qemu_co_queue_empty(&lock->queue)) {

            

            qemu_co_queue_next(&lock->queue);

        }

    } else {

        lock->reader--;

        assert(lock->reader >= 0);

        

        if (!lock->reader) {

            qemu_co_queue_next(&lock->queue);

        }

    }

}