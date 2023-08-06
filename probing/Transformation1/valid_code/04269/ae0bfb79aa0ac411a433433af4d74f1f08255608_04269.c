static int vga_osi_call (CPUState *env)

{

    static int vga_vbl_enabled;

    int linesize;



#if 0

    printf("osi_call R5=%016" PRIx64 "\n", ppc_dump_gpr(env, 5));

#endif



    

    switch(env->gpr[5]) {

    case 4:

        break;

    case 28: 

        if (env->gpr[6] != 1 || env->gpr[7] != 0)

            env->gpr[3] = 1;

        else

            env->gpr[3] = 0;

        break;

    case 29: 

        if (env->gpr[6] != 0) {

            if (env->gpr[6] != 1 || env->gpr[7] != 0) {

                env->gpr[3] = 1;

                break;

            }

        }

        env->gpr[3] = 0;

        env->gpr[4] = (1 << 16) | 1; 

        env->gpr[5] = (1 << 16) | 0; 

        env->gpr[6] = (graphic_width << 16) | graphic_height; 

        env->gpr[7] = 85 << 16; 

        env->gpr[8] = (graphic_depth + 7) & ~7; 

        linesize = ((graphic_depth + 7) >> 3) * graphic_width;

        linesize = (linesize + 3) & ~3;

        env->gpr[9] = (linesize << 16) | 0; 

        break;

    case 31: 

        env->gpr[3] = 0;

        break;

    case 39: 

        if (env->gpr[6] == 0 || env->gpr[6] == 1)

            vga_vbl_enabled = env->gpr[6];

        env->gpr[3] = 0;

        break;

    case 47:

        break;

    case 59: 

        

        env->gpr[3] = 0;

        break;

    case 64: 

        

        env->gpr[3] = 0;

        break;

    case 116: 

        

        break;

    default:

        fprintf(stderr, "unsupported OSI call R5=%016" PRIx64 "\n",

                ppc_dump_gpr(env, 5));

        break;

    }



    return 1; 

}