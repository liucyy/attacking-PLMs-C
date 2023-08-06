void qemu_acl_reset(qemu_acl *acl)

{

    qemu_acl_entry *entry;



    

    acl->defaultDeny = 1;

    TAILQ_FOREACH(entry, &acl->entries, next) {

        TAILQ_REMOVE(&acl->entries, entry, next);

        free(entry->match);

        free(entry);

    }

    acl->nentries = 0;

}