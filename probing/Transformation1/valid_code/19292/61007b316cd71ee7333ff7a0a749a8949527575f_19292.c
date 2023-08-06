static bool tracked_request_overlaps(BdrvTrackedRequest *req,

                                     int64_t offset, unsigned int bytes)

{

    

    if (offset >= req->overlap_offset + req->overlap_bytes) {

        return false;

    }

    

    if (req->overlap_offset >= offset + bytes) {

        return false;

    }

    return true;

}