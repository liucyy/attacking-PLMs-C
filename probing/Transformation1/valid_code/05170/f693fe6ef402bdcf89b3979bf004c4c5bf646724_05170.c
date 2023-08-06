GuestMemoryBlockList *qmp_guest_get_memory_blocks(Error **errp)

{

    GuestMemoryBlockList *head, **link;

    Error *local_err = NULL;

    struct dirent *de;

    DIR *dp;



    head = NULL;

    link = &head;



    dp = opendir("/sys/devices/system/memory/");

    if (!dp) {

        error_setg_errno(errp, errno, "Can't open directory"

                         "\"/sys/devices/system/memory/\"\n");

        return NULL;

    }



    

    while ((de = readdir(dp)) != NULL) {

        GuestMemoryBlock *mem_blk;

        GuestMemoryBlockList *entry;



        if ((strncmp(de->d_name, "memory", 6) != 0) ||

            !(de->d_type & DT_DIR)) {

            continue;

        }



        mem_blk = g_malloc0(sizeof *mem_blk);

        

        mem_blk->phys_index = strtoul(&de->d_name[6], NULL, 10);

        mem_blk->has_can_offline = true; 

        transfer_memory_block(mem_blk, true, NULL, &local_err);



        entry = g_malloc0(sizeof *entry);

        entry->value = mem_blk;



        *link = entry;

        link = &entry->next;

    }



    closedir(dp);

    if (local_err == NULL) {

        

        if (head == NULL) {

            error_setg(errp, "guest reported zero memory blocks!");

        }

        return head;

    }



    qapi_free_GuestMemoryBlockList(head);

    error_propagate(errp, local_err);

    return NULL;

}