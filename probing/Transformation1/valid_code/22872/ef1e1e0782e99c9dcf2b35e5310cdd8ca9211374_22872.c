void xen_pt_config_delete(XenPCIPassthroughState *s)

{

    struct XenPTRegGroup *reg_group, *next_grp;

    struct XenPTReg *reg, *next_reg;



    

    if (s->msix) {

        xen_pt_msix_delete(s);

    }

    if (s->msi) {

        g_free(s->msi);

    }



    

    QLIST_FOREACH_SAFE(reg_group, &s->reg_grps, entries, next_grp) {

        

        QLIST_FOREACH_SAFE(reg, &reg_group->reg_tbl_list, entries, next_reg) {

            QLIST_REMOVE(reg, entries);

            g_free(reg);

        }



        QLIST_REMOVE(reg_group, entries);

        g_free(reg_group);

    }

}