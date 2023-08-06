static BlockReopenQueue *bdrv_reopen_queue_child(BlockReopenQueue *bs_queue,

                                                 BlockDriverState *bs,

                                                 QDict *options,

                                                 int flags,

                                                 const BdrvChildRole *role,

                                                 QDict *parent_options,

                                                 int parent_flags)

{

    assert(bs != NULL);



    BlockReopenQueueEntry *bs_entry;

    BdrvChild *child;

    QDict *old_options, *explicit_options;



    if (bs_queue == NULL) {

        bs_queue = g_new0(BlockReopenQueue, 1);

        QSIMPLEQ_INIT(bs_queue);

    }



    if (!options) {

        options = qdict_new();

    }



    



    if (!parent_options) {

        

        update_options_from_flags(options, flags);

    }



    

    old_options = qdict_clone_shallow(bs->explicit_options);

    bdrv_join_options(bs, options, old_options);

    QDECREF(old_options);



    explicit_options = qdict_clone_shallow(options);



    

    if (parent_options) {

        assert(!flags);

        role->inherit_options(&flags, options, parent_flags, parent_options);

    }



    

    old_options = qdict_clone_shallow(bs->options);

    bdrv_join_options(bs, options, old_options);

    QDECREF(old_options);



    

    flags &= ~BDRV_O_PROTOCOL;



    QLIST_FOREACH(child, &bs->children, next) {

        QDict *new_child_options;

        char *child_key_dot;



        

        if (child->bs->inherits_from != bs) {

            continue;

        }



        child_key_dot = g_strdup_printf("%s.", child->name);

        qdict_extract_subqdict(options, &new_child_options, child_key_dot);

        g_free(child_key_dot);



        bdrv_reopen_queue_child(bs_queue, child->bs, new_child_options, 0,

                                child->role, options, flags);

    }



    bs_entry = g_new0(BlockReopenQueueEntry, 1);

    QSIMPLEQ_INSERT_TAIL(bs_queue, bs_entry, entry);



    bs_entry->state.bs = bs;

    bs_entry->state.options = options;

    bs_entry->state.explicit_options = explicit_options;

    bs_entry->state.flags = flags;



    return bs_queue;

}