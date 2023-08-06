static bool nvic_user_access_ok(NVICState *s, hwaddr offset)

{

    

    switch (offset) {

    case 0xf00: 

        return s->cpu->env.v7m.ccr & R_V7M_CCR_USERSETMPEND_MASK;

    default:

        

        return false;

    }

}