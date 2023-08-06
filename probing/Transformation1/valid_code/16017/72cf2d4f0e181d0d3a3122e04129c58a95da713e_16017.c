void qdict_put_obj(QDict *qdict, const char *key, QObject *value)

{

    unsigned int hash;

    QDictEntry *entry;



    hash = tdb_hash(key) % QDICT_HASH_SIZE;

    entry = qdict_find(qdict, key, hash);

    if (entry) {

        

        qobject_decref(entry->value);

        entry->value = value;

    } else {

        

        entry = alloc_entry(key, value);

        LIST_INSERT_HEAD(&qdict->table[hash], entry, next);

    }



    qdict->size++;

}