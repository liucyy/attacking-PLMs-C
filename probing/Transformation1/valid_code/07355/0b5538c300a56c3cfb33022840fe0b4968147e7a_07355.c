static void flush_trace_file(void)

{

    

    if (!trace_fp) {

        trace_fp = fopen(trace_file_name, "w");

        if (!trace_fp) {

            

            trace_file_enabled = false;

            return;

        }

        write_header(trace_fp);

    }



    if (trace_fp) {

        size_t unused; 

        unused = fwrite(trace_buf, trace_idx * sizeof(trace_buf[0]), 1, trace_fp);

    }

}