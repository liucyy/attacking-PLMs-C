void coroutine_fn qemu_co_mutex_unlock(CoMutex *mutex)

{

    Coroutine *self = qemu_coroutine_self();



    trace_qemu_co_mutex_unlock_entry(mutex, self);



    assert(mutex->locked);

    assert(mutex->holder == self);

    assert(qemu_in_coroutine());



    mutex->holder = NULL;

    self->locks_held--;

    if (atomic_fetch_dec(&mutex->locked) == 1) {

        

        return;

    }



    for (;;) {

        CoWaitRecord *to_wake = pop_waiter(mutex);

        unsigned our_handoff;



        if (to_wake) {

            Coroutine *co = to_wake->co;

            aio_co_wake(co);

            break;

        }



        

        if (++mutex->sequence == 0) {

            mutex->sequence = 1;

        }



        our_handoff = mutex->sequence;

        atomic_mb_set(&mutex->handoff, our_handoff);

        if (!has_waiters(mutex)) {

            

            break;

        }



        

        if (atomic_cmpxchg(&mutex->handoff, our_handoff, 0) != our_handoff) {

            break;

        }

    }



    trace_qemu_co_mutex_unlock_return(mutex, self);

}