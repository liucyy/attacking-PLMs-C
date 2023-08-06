static void evaluate_flags_writeback(uint32_t flags)

{

	int x;



	

	x = env->cc_x;

	if ((x || env->cc_op == CC_OP_ADDC)

	    && flags & Z_FLAG)

		env->cc_mask &= ~Z_FLAG;



	

	env->pregs[PR_CCS] &= ~(env->cc_mask | X_FLAG);

	flags &= env->cc_mask;

	env->pregs[PR_CCS] |= flags;

}