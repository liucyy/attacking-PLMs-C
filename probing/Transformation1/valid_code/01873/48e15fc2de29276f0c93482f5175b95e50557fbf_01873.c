static int do_fork(CPUState *env, unsigned int flags, abi_ulong newsp,

                   abi_ulong parent_tidptr, target_ulong newtls,

                   abi_ulong child_tidptr)

{

    int ret;

    TaskState *ts;

    uint8_t *new_stack;

    CPUState *new_env;

#if defined(CONFIG_USE_NPTL)

    unsigned int nptl_flags;

    sigset_t sigmask;

#endif



    

    if (flags & CLONE_VFORK)

        flags &= ~(CLONE_VFORK | CLONE_VM);



    if (flags & CLONE_VM) {

        TaskState *parent_ts = (TaskState *)env->opaque;

#if defined(CONFIG_USE_NPTL)

        new_thread_info info;

        pthread_attr_t attr;

#endif

        ts = qemu_mallocz(sizeof(TaskState) + NEW_STACK_SIZE);

        init_task_state(ts);

        new_stack = ts->stack;

        

        new_env = cpu_copy(env);

#if defined(TARGET_I386) || defined(TARGET_SPARC) || defined(TARGET_PPC)

        cpu_reset(new_env);

#endif

        

        cpu_clone_regs(new_env, newsp);

        new_env->opaque = ts;

        ts->bprm = parent_ts->bprm;

        ts->info = parent_ts->info;

#if defined(CONFIG_USE_NPTL)

        nptl_flags = flags;

        flags &= ~CLONE_NPTL_FLAGS2;



        if (nptl_flags & CLONE_CHILD_CLEARTID) {

            ts->child_tidptr = child_tidptr;

        }



        if (nptl_flags & CLONE_SETTLS)

            cpu_set_tls (new_env, newtls);



        

        pthread_mutex_lock(&clone_lock);



        memset(&info, 0, sizeof(info));

        pthread_mutex_init(&info.mutex, NULL);

        pthread_mutex_lock(&info.mutex);

        pthread_cond_init(&info.cond, NULL);

        info.env = new_env;

        if (nptl_flags & CLONE_CHILD_SETTID)

            info.child_tidptr = child_tidptr;

        if (nptl_flags & CLONE_PARENT_SETTID)

            info.parent_tidptr = parent_tidptr;



        ret = pthread_attr_init(&attr);

        ret = pthread_attr_setstack(&attr, new_stack, NEW_STACK_SIZE);

        

        sigfillset(&sigmask);

        sigprocmask(SIG_BLOCK, &sigmask, &info.sigmask);



        ret = pthread_create(&info.thread, &attr, clone_func, &info);

        



        sigprocmask(SIG_SETMASK, &info.sigmask, NULL);

        pthread_attr_destroy(&attr);

        if (ret == 0) {

            

            pthread_cond_wait(&info.cond, &info.mutex);

            ret = info.tid;

            if (flags & CLONE_PARENT_SETTID)

                put_user_u32(ret, parent_tidptr);

        } else {

            ret = -1;

        }

        pthread_mutex_unlock(&info.mutex);

        pthread_cond_destroy(&info.cond);

        pthread_mutex_destroy(&info.mutex);

        pthread_mutex_unlock(&clone_lock);

#else

        if (flags & CLONE_NPTL_FLAGS2)

            return -EINVAL;

        

#ifdef __ia64__

        ret = __clone2(clone_func, new_stack, NEW_STACK_SIZE, flags, new_env);

#else

	ret = clone(clone_func, new_stack + NEW_STACK_SIZE, flags, new_env);

#endif

#endif

    } else {

        

        if ((flags & ~(CSIGNAL | CLONE_NPTL_FLAGS2)) != 0)

            return -EINVAL;

        fork_start();

        ret = fork();

        if (ret == 0) {

            

            cpu_clone_regs(env, newsp);

            fork_end(1);

#if defined(CONFIG_USE_NPTL)

            

            if (flags & CLONE_CHILD_SETTID)

                put_user_u32(gettid(), child_tidptr);

            if (flags & CLONE_PARENT_SETTID)

                put_user_u32(gettid(), parent_tidptr);

            ts = (TaskState *)env->opaque;

            if (flags & CLONE_SETTLS)

                cpu_set_tls (env, newtls);

            if (flags & CLONE_CHILD_CLEARTID)

                ts->child_tidptr = child_tidptr;

#endif

        } else {

            fork_end(0);

        }

    }

    return ret;

}