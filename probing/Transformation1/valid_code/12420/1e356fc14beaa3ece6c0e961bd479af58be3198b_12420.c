host_memory_backend_memory_complete(UserCreatable *uc, Error **errp)

{

    HostMemoryBackend *backend = MEMORY_BACKEND(uc);

    HostMemoryBackendClass *bc = MEMORY_BACKEND_GET_CLASS(uc);

    Error *local_err = NULL;

    void *ptr;

    uint64_t sz;



    if (bc->alloc) {

        bc->alloc(backend, &local_err);

        if (local_err) {

            goto out;

        }



        ptr = memory_region_get_ram_ptr(&backend->mr);

        sz = memory_region_size(&backend->mr);



        if (backend->merge) {

            qemu_madvise(ptr, sz, QEMU_MADV_MERGEABLE);

        }

        if (!backend->dump) {

            qemu_madvise(ptr, sz, QEMU_MADV_DONTDUMP);

        }

#ifdef CONFIG_NUMA

        unsigned long lastbit = find_last_bit(backend->host_nodes, MAX_NODES);

        

        unsigned long maxnode = (lastbit + 1) % (MAX_NODES + 1);

        

        unsigned flags = MPOL_MF_STRICT | MPOL_MF_MOVE;



        

        if (maxnode && backend->policy == MPOL_DEFAULT) {

            error_setg(errp, "host-nodes must be empty for policy default,"

                       " or you should explicitly specify a policy other"

                       " than default");

            return;

        } else if (maxnode == 0 && backend->policy != MPOL_DEFAULT) {

            error_setg(errp, "host-nodes must be set for policy %s",

                       HostMemPolicy_lookup[backend->policy]);

            return;

        }



        

        assert(sizeof(backend->host_nodes) >=

               BITS_TO_LONGS(MAX_NODES + 1) * sizeof(unsigned long));

        assert(maxnode <= MAX_NODES);

        if (mbind(ptr, sz, backend->policy,

                  maxnode ? backend->host_nodes : NULL, maxnode + 1, flags)) {

            if (backend->policy != MPOL_DEFAULT || errno != ENOSYS) {

                error_setg_errno(errp, errno,

                                 "cannot bind memory to host NUMA nodes");

                return;

            }

        }

#endif

        

        if (backend->prealloc) {

            os_mem_prealloc(memory_region_get_fd(&backend->mr), ptr, sz,

                            &local_err);

            if (local_err) {

                goto out;

            }

        }

    }

out:

    error_propagate(errp, local_err);

}