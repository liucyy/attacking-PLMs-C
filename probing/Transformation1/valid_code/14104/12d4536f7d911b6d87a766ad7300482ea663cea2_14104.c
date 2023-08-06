static int kvm_check_many_ioeventfds(void)

{

    

#if defined(CONFIG_EVENTFD) && defined(CONFIG_IOTHREAD)

    int ioeventfds[7];

    int i, ret = 0;

    for (i = 0; i < ARRAY_SIZE(ioeventfds); i++) {

        ioeventfds[i] = eventfd(0, EFD_CLOEXEC);

        if (ioeventfds[i] < 0) {

            break;

        }

        ret = kvm_set_ioeventfd_pio_word(ioeventfds[i], 0, i, true);

        if (ret < 0) {

            close(ioeventfds[i]);

            break;

        }

    }



    

    ret = i == ARRAY_SIZE(ioeventfds);



    while (i-- > 0) {

        kvm_set_ioeventfd_pio_word(ioeventfds[i], 0, i, false);

        close(ioeventfds[i]);

    }

    return ret;

#else

    return 0;

#endif

}