int kvmppc_get_hypercall(CPUPPCState *env, uint8_t *buf, int buf_len)

{

    uint32_t *hc = (uint32_t*)buf;

    struct kvm_ppc_pvinfo pvinfo;



    if (!kvmppc_get_pvinfo(env, &pvinfo)) {

        memcpy(buf, pvinfo.hcall, buf_len);

        return 0;

    }



    



    hc[0] = cpu_to_be32(0x08000048);

    hc[1] = cpu_to_be32(0x3860ffff);

    hc[2] = cpu_to_be32(0x48000008);

    hc[3] = cpu_to_be32(bswap32(0x3860ffff));



    return 0;

}