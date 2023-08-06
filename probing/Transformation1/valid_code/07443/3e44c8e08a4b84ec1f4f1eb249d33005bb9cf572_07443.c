static void change_parent_backing_link(BlockDriverState *from,

                                       BlockDriverState *to)

{

    BdrvChild *c, *next, *to_c;



    QLIST_FOREACH_SAFE(c, &from->parents, next_parent, next) {

        if (c->role->stay_at_node) {

            continue;

        }

        if (c->role == &child_backing) {

            

            QLIST_FOREACH(to_c, &to->children, next) {

                if (to_c == c) {

                    break;

                }

            }

            if (to_c) {

                continue;

            }

        }



        assert(c->role != &child_backing);

        bdrv_ref(to);

        

        bdrv_replace_child(c, to, true);

        bdrv_unref(from);

    }

}