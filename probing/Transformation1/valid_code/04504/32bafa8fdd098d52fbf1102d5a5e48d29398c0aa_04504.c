int net_init_vde(const NetClientOptions *opts, const char *name,

                 NetClientState *peer, Error **errp)

{

    

    const NetdevVdeOptions *vde;



    assert(opts->type == NET_CLIENT_OPTIONS_KIND_VDE);

    vde = opts->u.vde;



    

    if (net_vde_init(peer, "vde", name, vde->sock, vde->port, vde->group,

                     vde->has_mode ? vde->mode : 0700) == -1) {

        return -1;

    }



    return 0;

}