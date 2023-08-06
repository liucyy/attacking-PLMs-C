static int nbd_negotiate_options(NBDClient *client, uint16_t myflags,
                                 Error **errp)
{
    uint32_t flags;
    bool fixedNewstyle = false;
    bool no_zeroes = false;
    
    if (nbd_read(client->ioc, &flags, sizeof(flags), errp) < 0) {
        error_prepend(errp, "read failed: ");
        return -EIO;
    be32_to_cpus(&flags);
    trace_nbd_negotiate_options_flags(flags);
    if (flags & NBD_FLAG_C_FIXED_NEWSTYLE) {
        fixedNewstyle = true;
        flags &= ~NBD_FLAG_C_FIXED_NEWSTYLE;
    if (flags & NBD_FLAG_C_NO_ZEROES) {
        no_zeroes = true;
        flags &= ~NBD_FLAG_C_NO_ZEROES;
    if (flags != 0) {
        error_setg(errp, "Unknown client flags 0x%" PRIx32 " received", flags);
    while (1) {
        int ret;
        uint32_t option, length;
        uint64_t magic;
        if (nbd_read(client->ioc, &magic, sizeof(magic), errp) < 0) {
            error_prepend(errp, "read failed: ");
        magic = be64_to_cpu(magic);
        trace_nbd_negotiate_options_check_magic(magic);
        if (magic != NBD_OPTS_MAGIC) {
            error_setg(errp, "Bad magic received");
        if (nbd_read(client->ioc, &option,
                     sizeof(option), errp) < 0) {
            error_prepend(errp, "read failed: ");
        option = be32_to_cpu(option);
        if (nbd_read(client->ioc, &length, sizeof(length), errp) < 0) {
            error_prepend(errp, "read failed: ");
        length = be32_to_cpu(length);
        trace_nbd_negotiate_options_check_option(option,
                                                 nbd_opt_lookup(option));
        if (client->tlscreds &&
            client->ioc == (QIOChannel *)client->sioc) {
            QIOChannel *tioc;
            if (!fixedNewstyle) {
                error_setg(errp, "Unsupported option 0x%" PRIx32, option);
            switch (option) {
            case NBD_OPT_STARTTLS:
                if (length) {
                    
                    return nbd_reject_length(client, length, option, true,
                                             errp);
                tioc = nbd_negotiate_handle_starttls(client, errp);
                if (!tioc) {
                    return -EIO;
                ret = 0;
                object_unref(OBJECT(client->ioc));
                client->ioc = QIO_CHANNEL(tioc);
                break;
            case NBD_OPT_EXPORT_NAME:
                
                error_setg(errp, "Option 0x%x not permitted before TLS",
                           option);
            default:
                if (nbd_drop(client->ioc, length, errp) < 0) {
                    return -EIO;
                ret = nbd_negotiate_send_rep_err(client->ioc,
                                                 NBD_REP_ERR_TLS_REQD,
                                                 option, errp,
                                                 "Option 0x%" PRIx32
                                                 "not permitted before TLS",
                                                 option);
                
                if (option == NBD_OPT_ABORT) {
                    return 1;
                break;
        } else if (fixedNewstyle) {
            switch (option) {
            case NBD_OPT_LIST:
                if (length) {
                    ret = nbd_reject_length(client, length, option, false,
                                            errp);
                } else {
                    ret = nbd_negotiate_handle_list(client, errp);
                break;
            case NBD_OPT_ABORT:
                
                nbd_negotiate_send_rep(client->ioc, NBD_REP_ACK, option, NULL);
                return 1;
            case NBD_OPT_EXPORT_NAME:
                return nbd_negotiate_handle_export_name(client, length,
                                                        myflags, no_zeroes,
                                                        errp);
            case NBD_OPT_INFO:
            case NBD_OPT_GO:
                ret = nbd_negotiate_handle_info(client, length, option,
                                                myflags, errp);
                if (ret == 1) {
                    assert(option == NBD_OPT_GO);
                    return 0;
                break;
            case NBD_OPT_STARTTLS:
                if (length) {
                    ret = nbd_reject_length(client, length, option, false,
                                            errp);
                } else if (client->tlscreds) {
                    ret = nbd_negotiate_send_rep_err(client->ioc,
                                                     NBD_REP_ERR_INVALID,
                                                     option, errp,
                                                     "TLS already enabled");
                } else {
                    ret = nbd_negotiate_send_rep_err(client->ioc,
                                                     NBD_REP_ERR_POLICY,
                                                     option, errp,
                                                     "TLS not configured");
                break;
            case NBD_OPT_STRUCTURED_REPLY:
                if (length) {
                    ret = nbd_reject_length(client, length, option, false,
                                            errp);
                } else if (client->structured_reply) {
                    ret = nbd_negotiate_send_rep_err(
                        client->ioc, NBD_REP_ERR_INVALID, option, errp,
                        "structured reply already negotiated");
                } else {
                    ret = nbd_negotiate_send_rep(client->ioc, NBD_REP_ACK,
                                                 option, errp);
                    client->structured_reply = true;
                    myflags |= NBD_FLAG_SEND_DF;
                break;
            default:
                if (nbd_drop(client->ioc, length, errp) < 0) {
                    return -EIO;
                ret = nbd_negotiate_send_rep_err(client->ioc,
                                                 NBD_REP_ERR_UNSUP,
                                                 option, errp,
                                                 "Unsupported option 0x%"
                                                 PRIx32 " (%s)", option,
                                                 nbd_opt_lookup(option));
                break;
        } else {
            
            switch (option) {
            case NBD_OPT_EXPORT_NAME:
                return nbd_negotiate_handle_export_name(client, length,
                                                        myflags, no_zeroes,
                                                        errp);
            default:
                error_setg(errp, "Unsupported option 0x%" PRIx32 " (%s)",
                           option, nbd_opt_lookup(option));
        if (ret < 0) {
            return ret;