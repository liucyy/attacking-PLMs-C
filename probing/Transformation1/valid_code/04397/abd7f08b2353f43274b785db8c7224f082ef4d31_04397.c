static void virgl_cmd_get_capset(VirtIOGPU *g,

                                 struct virtio_gpu_ctrl_command *cmd)

{

    struct virtio_gpu_get_capset gc;

    struct virtio_gpu_resp_capset *resp;

    uint32_t max_ver, max_size;

    VIRTIO_GPU_FILL_CMD(gc);



    virgl_renderer_get_cap_set(gc.capset_id, &max_ver,

                               &max_size);

    resp = g_malloc(sizeof(*resp) + max_size);



    resp->hdr.type = VIRTIO_GPU_RESP_OK_CAPSET;

    virgl_renderer_fill_caps(gc.capset_id,

                             gc.capset_version,

                             (void *)resp->capset_data);

    virtio_gpu_ctrl_response(g, cmd, &resp->hdr, sizeof(*resp) + max_size);

    g_free(resp);

}