static int setfsugid(int uid, int gid)

{

    

    cap_value_t cap_list[] = {

        CAP_DAC_OVERRIDE,

    };



    setfsgid(gid);

    setfsuid(uid);



    if (uid != 0 || gid != 0) {

        return do_cap_set(cap_list, ARRAY_SIZE(cap_list), 0);

    }

    return 0;

}