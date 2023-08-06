static int vmstate_subsection_load(QEMUFile *f, const VMStateDescription *vmsd,

                                   void *opaque)

{

    while (qemu_peek_byte(f, 0) == QEMU_VM_SUBSECTION) {

        char idstr[256];

        int ret;

        uint8_t version_id, len, size;

        const VMStateDescription *sub_vmsd;



        len = qemu_peek_byte(f, 1);

        if (len < strlen(vmsd->name) + 1) {

            

            return 0;

        }

        size = qemu_peek_buffer(f, (uint8_t *)idstr, len, 2);

        if (size != len) {

            return 0;

        }

        idstr[size] = 0;



        if (strncmp(vmsd->name, idstr, strlen(vmsd->name)) != 0) {

            

            return 0;

        }

        sub_vmsd = vmstate_get_subsection(vmsd->subsections, idstr);

        if (sub_vmsd == NULL) {

            return -ENOENT;

        }

        qemu_file_skip(f, 1); 

        qemu_file_skip(f, 1); 

        qemu_file_skip(f, len); 

        version_id = qemu_get_be32(f);



        ret = vmstate_load_state(f, sub_vmsd, opaque, version_id);

        if (ret) {

            return ret;

        }

    }

    return 0;

}