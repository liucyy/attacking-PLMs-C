void qmp_migrate_set_capabilities(MigrationCapabilityStatusList *params,

                                  Error **errp)

{

    MigrationState *s = migrate_get_current();

    MigrationCapabilityStatusList *cap;

    bool old_postcopy_cap = migrate_postcopy_ram();



    if (migration_is_setup_or_active(s->state)) {

        error_setg(errp, QERR_MIGRATION_ACTIVE);

        return;

    }



    for (cap = params; cap; cap = cap->next) {

#ifndef CONFIG_LIVE_BLOCK_MIGRATION

        if (cap->value->capability == MIGRATION_CAPABILITY_BLOCK

            && cap->value->state) {

            error_setg(errp, "QEMU compiled without old-style (blk/-b, inc/-i) "

                       "block migration");

            error_append_hint(errp, "Use drive_mirror+NBD instead.\n");

            continue;

        }

#endif

        s->enabled_capabilities[cap->value->capability] = cap->value->state;

    }



    if (migrate_postcopy_ram()) {

        if (migrate_use_compression()) {

            

            error_report("Postcopy is not currently compatible with "

                         "compression");

            s->enabled_capabilities[MIGRATION_CAPABILITY_POSTCOPY_RAM] =

                false;

        }

        

        if (!old_postcopy_cap && runstate_check(RUN_STATE_INMIGRATE) &&

            !postcopy_ram_supported_by_host()) {

            

            error_report("Postcopy is not supported");

            s->enabled_capabilities[MIGRATION_CAPABILITY_POSTCOPY_RAM] =

                false;

        }

    }

}