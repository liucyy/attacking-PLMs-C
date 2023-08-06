static coroutine_fn int nbd_negotiate(NBDClient *client, Error **errp)

{

    char buf[8 + 8 + 8 + 128];

    int ret;

    const uint16_t myflags = (NBD_FLAG_HAS_FLAGS | NBD_FLAG_SEND_TRIM |

                              NBD_FLAG_SEND_FLUSH | NBD_FLAG_SEND_FUA |

                              NBD_FLAG_SEND_WRITE_ZEROES);

    bool oldStyle;



    



    qio_channel_set_blocking(client->ioc, false, NULL);



    trace_nbd_negotiate_begin();

    memset(buf, 0, sizeof(buf));

    memcpy(buf, "NBDMAGIC", 8);



    oldStyle = client->exp != NULL && !client->tlscreds;

    if (oldStyle) {

        trace_nbd_negotiate_old_style(client->exp->size,

                                      client->exp->nbdflags | myflags);

        stq_be_p(buf + 8, NBD_CLIENT_MAGIC);

        stq_be_p(buf + 16, client->exp->size);

        stw_be_p(buf + 26, client->exp->nbdflags | myflags);



        if (nbd_write(client->ioc, buf, sizeof(buf), errp) < 0) {

            error_prepend(errp, "write failed: ");

            return -EINVAL;

        }

    } else {

        stq_be_p(buf + 8, NBD_OPTS_MAGIC);

        stw_be_p(buf + 16, NBD_FLAG_FIXED_NEWSTYLE | NBD_FLAG_NO_ZEROES);



        if (nbd_write(client->ioc, buf, 18, errp) < 0) {

            error_prepend(errp, "write failed: ");

            return -EINVAL;

        }

        ret = nbd_negotiate_options(client, myflags, errp);

        if (ret != 0) {

            if (ret < 0) {

                error_prepend(errp, "option negotiation failed: ");

            }

            return ret;

        }

    }



    trace_nbd_negotiate_success();



    return 0;

}