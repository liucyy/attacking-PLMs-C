void bdrv_error_action(BlockDriverState *bs, BlockErrorAction action,

                       bool is_read, int error)

{

    assert(error >= 0);



    if (action == BLOCK_ERROR_ACTION_STOP) {

        

        bdrv_iostatus_set_err(bs, error);



        

        qemu_system_vmstop_request_prepare();

        send_qmp_error_event(bs, action, is_read, error);

        qemu_system_vmstop_request(RUN_STATE_IO_ERROR);

    } else {

        send_qmp_error_event(bs, action, is_read, error);

    }

}