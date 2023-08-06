void migration_bitmap_extend(ram_addr_t old, ram_addr_t new)

{

    

    if (migration_bitmap) {

        unsigned long *old_bitmap = migration_bitmap, *bitmap;

        bitmap = bitmap_new(new);



        

        qemu_mutex_lock(&migration_bitmap_mutex);

        bitmap_copy(bitmap, old_bitmap, old);

        bitmap_set(bitmap, old, new - old);

        atomic_rcu_set(&migration_bitmap, bitmap);

        qemu_mutex_unlock(&migration_bitmap_mutex);

        migration_dirty_pages += new - old;

        synchronize_rcu();

        g_free(old_bitmap);

    }

}