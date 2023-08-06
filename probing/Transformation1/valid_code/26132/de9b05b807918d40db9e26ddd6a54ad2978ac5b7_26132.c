void arm_cpu_realize(ARMCPU *cpu)

{

    

    CPUARMState *env = &cpu->env;

    

    if (arm_feature(env, ARM_FEATURE_V7)) {

        set_feature(env, ARM_FEATURE_VAPA);

        set_feature(env, ARM_FEATURE_THUMB2);

        set_feature(env, ARM_FEATURE_MPIDR);

        if (!arm_feature(env, ARM_FEATURE_M)) {

            set_feature(env, ARM_FEATURE_V6K);

        } else {

            set_feature(env, ARM_FEATURE_V6);



    if (arm_feature(env, ARM_FEATURE_V6K)) {

        set_feature(env, ARM_FEATURE_V6);

        set_feature(env, ARM_FEATURE_MVFR);


    if (arm_feature(env, ARM_FEATURE_V6)) {

        set_feature(env, ARM_FEATURE_V5);

        if (!arm_feature(env, ARM_FEATURE_M)) {

            set_feature(env, ARM_FEATURE_AUXCR);



    if (arm_feature(env, ARM_FEATURE_V5)) {

        set_feature(env, ARM_FEATURE_V4T);


    if (arm_feature(env, ARM_FEATURE_M)) {

        set_feature(env, ARM_FEATURE_THUMB_DIV);


    if (arm_feature(env, ARM_FEATURE_ARM_DIV)) {

        set_feature(env, ARM_FEATURE_THUMB_DIV);


    if (arm_feature(env, ARM_FEATURE_VFP4)) {

        set_feature(env, ARM_FEATURE_VFP3);


    if (arm_feature(env, ARM_FEATURE_VFP3)) {

        set_feature(env, ARM_FEATURE_VFP);







    register_cp_regs_for_features(cpu);