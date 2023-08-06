static void *postcopy_ram_fault_thread(void *opaque)

{

    MigrationIncomingState *mis = opaque;

    struct uffd_msg msg;

    int ret;

    RAMBlock *rb = NULL;

    RAMBlock *last_rb = NULL; 



    trace_postcopy_ram_fault_thread_entry();

    qemu_sem_post(&mis->fault_thread_sem);



    while (true) {

        ram_addr_t rb_offset;

        struct pollfd pfd[2];



        

        pfd[0].fd = mis->userfault_fd;

        pfd[0].events = POLLIN;

        pfd[0].revents = 0;

        pfd[1].fd = mis->userfault_quit_fd;

        pfd[1].events = POLLIN; 

        pfd[1].revents = 0;



        if (poll(pfd, 2, -1 ) == -1) {

            error_report("%s: userfault poll: %s", __func__, strerror(errno));

            break;

        }



        if (pfd[1].revents) {

            trace_postcopy_ram_fault_thread_quit();

            break;

        }



        ret = read(mis->userfault_fd, &msg, sizeof(msg));

        if (ret != sizeof(msg)) {

            if (errno == EAGAIN) {

                

                continue;

            }

            if (ret < 0) {

                error_report("%s: Failed to read full userfault message: %s",

                             __func__, strerror(errno));

                break;

            } else {

                error_report("%s: Read %d bytes from userfaultfd expected %zd",

                             __func__, ret, sizeof(msg));

                break; 

            }

        }

        if (msg.event != UFFD_EVENT_PAGEFAULT) {

            error_report("%s: Read unexpected event %ud from userfaultfd",

                         __func__, msg.event);

            continue; 

        }



        rb = qemu_ram_block_from_host(

                 (void *)(uintptr_t)msg.arg.pagefault.address,

                 true, &rb_offset);

        if (!rb) {

            error_report("postcopy_ram_fault_thread: Fault outside guest: %"

                         PRIx64, (uint64_t)msg.arg.pagefault.address);

            break;

        }



        rb_offset &= ~(qemu_ram_pagesize(rb) - 1);

        trace_postcopy_ram_fault_thread_request(msg.arg.pagefault.address,

                                                qemu_ram_get_idstr(rb),

                                                rb_offset);



        

        if (rb != last_rb) {

            last_rb = rb;

            migrate_send_rp_req_pages(mis, qemu_ram_get_idstr(rb),

                                     rb_offset, qemu_ram_pagesize(rb));

        } else {

            

            migrate_send_rp_req_pages(mis, NULL,

                                     rb_offset, qemu_ram_pagesize(rb));

        }

    }

    trace_postcopy_ram_fault_thread_exit();

    return NULL;

}