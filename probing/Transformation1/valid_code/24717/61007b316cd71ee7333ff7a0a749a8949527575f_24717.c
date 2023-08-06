static int multiwrite_req_compare(const void *a, const void *b)

{

    const BlockRequest *req1 = a, *req2 = b;



    

    if (req1->sector > req2->sector) {

        return 1;

    } else if (req1->sector < req2->sector) {

        return -1;

    } else {

        return 0;

    }

}