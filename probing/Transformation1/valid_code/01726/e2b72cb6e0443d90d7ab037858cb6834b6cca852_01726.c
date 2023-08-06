static bool vnc_should_update(VncState *vs)

{

    switch (vs->update) {

    case VNC_STATE_UPDATE_NONE:

        break;

    case VNC_STATE_UPDATE_INCREMENTAL:

        

        if (!vs->output.offset || vs->audio_cap) {

            return true;

        }

        break;

    case VNC_STATE_UPDATE_FORCE:

        return true;

    }

    return false;

}