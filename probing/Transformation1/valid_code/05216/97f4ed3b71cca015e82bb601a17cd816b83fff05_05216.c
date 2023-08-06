static int cmd_valid_while_locked(SDState *sd, SDRequest *req)

{

    

    if (sd->expecting_acmd) {

        return req->cmd == 41 || req->cmd == 42;

    }

    if (req->cmd == 16 || req->cmd == 55) {

        return 1;

    }

    return sd_cmd_class[req->cmd] == 0 || sd_cmd_class[req->cmd] == 7;

}