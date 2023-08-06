ivshmem_client_parse_args(IvshmemClientArgs *args, int argc, char *argv[])

{

    int c;



    while ((c = getopt(argc, argv,

                       "h"  

                       "v"  

                       "S:" 

                      )) != -1) {



        switch (c) {

        case 'h': 

            ivshmem_client_usage(argv[0], 0);

            break;



        case 'v': 

            args->verbose = 1;

            break;



        case 'S': 

            args->unix_sock_path = strdup(optarg);

            break;



        default:

            ivshmem_client_usage(argv[0], 1);

            break;

        }

    }

}