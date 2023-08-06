static void ide_set_signature(IDEState *s)

{

    s->select &= 0xf0; 

    

    s->nsector = 1;

    s->sector = 1;

    if (s->drive_kind == IDE_CD) {

        s->lcyl = 0x14;

        s->hcyl = 0xeb;

    } else if (s->bs) {

        s->lcyl = 0;

        s->hcyl = 0;

    } else {

        s->lcyl = 0xff;

        s->hcyl = 0xff;

    }

}