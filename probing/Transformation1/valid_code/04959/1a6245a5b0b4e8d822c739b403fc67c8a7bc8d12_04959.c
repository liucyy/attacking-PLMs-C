static int nbd_receive_options(NBDClient *client)

{

    int csock = client->sock;

    uint32_t flags;



    



    if (read_sync(csock, &flags, sizeof(flags)) != sizeof(flags)) {

        LOG("read failed");

        return -EIO;

    }

    TRACE("Checking client flags");

    be32_to_cpus(&flags);

    if (flags != 0 && flags != NBD_FLAG_C_FIXED_NEWSTYLE) {

        LOG("Bad client flags received");

        return -EIO;

    }



    while (1) {

        int ret;

        uint32_t tmp, length;

        uint64_t magic;



        if (read_sync(csock, &magic, sizeof(magic)) != sizeof(magic)) {

            LOG("read failed");

            return -EINVAL;

        }

        TRACE("Checking opts magic");

        if (magic != be64_to_cpu(NBD_OPTS_MAGIC)) {

            LOG("Bad magic received");

            return -EINVAL;

        }



        if (read_sync(csock, &tmp, sizeof(tmp)) != sizeof(tmp)) {

            LOG("read failed");

            return -EINVAL;

        }



        if (read_sync(csock, &length, sizeof(length)) != sizeof(length)) {

            LOG("read failed");

            return -EINVAL;

        }

        length = be32_to_cpu(length);



        TRACE("Checking option");

        switch (be32_to_cpu(tmp)) {

        case NBD_OPT_LIST:

            ret = nbd_handle_list(client, length);

            if (ret < 0) {

                return ret;

            }

            break;



        case NBD_OPT_ABORT:

            return -EINVAL;



        case NBD_OPT_EXPORT_NAME:

            return nbd_handle_export_name(client, length);



        default:

            tmp = be32_to_cpu(tmp);

            LOG("Unsupported option 0x%x", tmp);

            nbd_send_rep(client->sock, NBD_REP_ERR_UNSUP, tmp);

            return -EINVAL;

        }

    }

}