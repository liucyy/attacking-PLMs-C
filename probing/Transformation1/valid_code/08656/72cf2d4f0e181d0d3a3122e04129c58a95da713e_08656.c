qemu_acl *qemu_acl_init(const char *aclname)

{

    qemu_acl *acl;



    acl = qemu_acl_find(aclname);

    if (acl)

        return acl;



    acl = qemu_malloc(sizeof(*acl));

    acl->aclname = qemu_strdup(aclname);

    

    acl->defaultDeny = 1;



    acl->nentries = 0;

    TAILQ_INIT(&acl->entries);



    acls = qemu_realloc(acls, sizeof(*acls) * (nacls +1));

    acls[nacls] = acl;

    nacls++;



    return acl;

}