static int memory_access_size(MemoryRegion *mr, unsigned l, hwaddr addr)

{

    unsigned access_size_min = mr->ops->impl.min_access_size;

    unsigned access_size_max = mr->ops->impl.max_access_size;



    

    if (access_size_min == 0) {

        access_size_min = 1;

    }

    if (access_size_max == 0) {

        access_size_max = 4;

    }



    

    if (!mr->ops->impl.unaligned) {

        unsigned align_size_max = addr & -addr;

        if (align_size_max != 0 && align_size_max < access_size_max) {

            access_size_max = align_size_max;

        }

    }



    

    if (l > access_size_max) {

        l = access_size_max;

    }

    

    assert(l >= access_size_min);



    return l;

}