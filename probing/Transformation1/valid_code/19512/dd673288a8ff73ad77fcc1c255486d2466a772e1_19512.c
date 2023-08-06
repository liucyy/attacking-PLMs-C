int cpu_is_bsp(CPUX86State *env)

{

    

    return env->cpu_index == 0;

}