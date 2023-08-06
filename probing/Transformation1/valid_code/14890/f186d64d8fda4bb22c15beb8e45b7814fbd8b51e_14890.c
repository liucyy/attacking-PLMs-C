void replay_account_executed_instructions(void)

{

    if (replay_mode == REPLAY_MODE_PLAY) {

        replay_mutex_lock();

        if (replay_state.instructions_count > 0) {

            int count = (int)(replay_get_current_step()

                              - replay_state.current_step);

            replay_state.instructions_count -= count;

            replay_state.current_step += count;

            if (replay_state.instructions_count == 0) {

                assert(replay_data_kind == EVENT_INSTRUCTION);

                replay_finish_event();

                

                qemu_notify_event();

            }

        }

        replay_mutex_unlock();

    }

}