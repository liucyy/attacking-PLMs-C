CPUState *cpu_generic_init(const char *typename, const char *cpu_model)

{

    

    const char *cpu_type = cpu_parse_cpu_model(typename, cpu_model);



    if (cpu_type) {

        return cpu_create(cpu_type);

    }

    return NULL;

}