static void migrate_fd_cancel(MigrationState *s)
{
    int old_state ;
    QEMUFile *f = migrate_get_current()->to_dst_file;
    trace_migrate_fd_cancel();
    if (s->rp_state.from_dst_file) {
        
        qemu_file_shutdown(s->rp_state.from_dst_file);
    do {
        old_state = s->state;
        if (!migration_is_setup_or_active(old_state)) {
            break;
        migrate_set_state(&s->state, old_state, MIGRATION_STATUS_CANCELLING);
    } while (s->state != MIGRATION_STATUS_CANCELLING);
    
    if (s->state == MIGRATION_STATUS_CANCELLING && f) {
        qemu_file_shutdown(f);