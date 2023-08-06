static abi_long do_socketcall(int num, abi_ulong vptr)

{

    static const unsigned ac[] = { 

        [SOCKOP_socket] = 3,      

        [SOCKOP_bind] = 3,        

        [SOCKOP_connect] = 3,     

        [SOCKOP_listen] = 2,      

        [SOCKOP_accept] = 3,      

        [SOCKOP_accept4] = 4,     

        [SOCKOP_getsockname] = 3, 

        [SOCKOP_getpeername] = 3, 

        [SOCKOP_socketpair] = 4,  

        [SOCKOP_send] = 4,        

        [SOCKOP_recv] = 4,        

        [SOCKOP_sendto] = 6,      

        [SOCKOP_recvfrom] = 6,    

        [SOCKOP_shutdown] = 2,    

        [SOCKOP_sendmsg] = 3,     

        [SOCKOP_recvmsg] = 3,     

        [SOCKOP_sendmmsg] = 4,    

        [SOCKOP_recvmmsg] = 4,    

        [SOCKOP_setsockopt] = 5,  

        [SOCKOP_getsockopt] = 5,  

    };

    abi_long a[6]; 



    

    if (num >= 0 && num < ARRAY_SIZE(ac)) {

        unsigned i;

        assert(ARRAY_SIZE(a) >= ac[num]); 

        for (i = 0; i < ac[num]; ++i) {

            if (get_user_ual(a[i], vptr + i * sizeof(abi_long)) != 0) {

                return -TARGET_EFAULT;

            }

        }

    }



    

    switch (num) {

    case SOCKOP_socket: 

        return do_socket(a[0], a[1], a[2]);

    case SOCKOP_bind: 

        return do_bind(a[0], a[1], a[2]);

    case SOCKOP_connect: 

        return do_connect(a[0], a[1], a[2]);

    case SOCKOP_listen: 

        return get_errno(listen(a[0], a[1]));

    case SOCKOP_accept: 

        return do_accept4(a[0], a[1], a[2], 0);

    case SOCKOP_accept4: 

        return do_accept4(a[0], a[1], a[2], a[3]);

    case SOCKOP_getsockname: 

        return do_getsockname(a[0], a[1], a[2]);

    case SOCKOP_getpeername: 

        return do_getpeername(a[0], a[1], a[2]);

    case SOCKOP_socketpair: 

        return do_socketpair(a[0], a[1], a[2], a[3]);

    case SOCKOP_send: 

        return do_sendto(a[0], a[1], a[2], a[3], 0, 0);

    case SOCKOP_recv: 

        return do_recvfrom(a[0], a[1], a[2], a[3], 0, 0);

    case SOCKOP_sendto: 

        return do_sendto(a[0], a[1], a[2], a[3], a[4], a[5]);

    case SOCKOP_recvfrom: 

        return do_recvfrom(a[0], a[1], a[2], a[3], a[4], a[5]);

    case SOCKOP_shutdown: 

        return get_errno(shutdown(a[0], a[1]));

    case SOCKOP_sendmsg: 

        return do_sendrecvmsg(a[0], a[1], a[2], 1);

    case SOCKOP_recvmsg: 

        return do_sendrecvmsg(a[0], a[1], a[2], 0);

    case SOCKOP_sendmmsg: 

        return do_sendrecvmmsg(a[0], a[1], a[2], a[3], 1);

    case SOCKOP_recvmmsg: 

        return do_sendrecvmmsg(a[0], a[1], a[2], a[3], 0);

    case SOCKOP_setsockopt: 

        return do_setsockopt(a[0], a[1], a[2], a[3], a[4]);

    case SOCKOP_getsockopt: 

        return do_getsockopt(a[0], a[1], a[2], a[3], a[4]);

    default:

        gemu_log("Unsupported socketcall: %d\n", num);

        return -TARGET_ENOSYS;

    }

}