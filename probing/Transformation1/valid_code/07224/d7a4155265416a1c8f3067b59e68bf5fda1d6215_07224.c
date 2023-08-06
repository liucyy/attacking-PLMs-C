static void e1000_pre_save(void *opaque)

{

    E1000State *s = opaque;

    NetClientState *nc = qemu_get_queue(s->nic);



    

    if (s->mit_timer_on) {

        e1000_mit_timer(s);

    }



    

    if (nc->link_down &&

        s->compat_flags & E1000_FLAG_AUTONEG &&

        s->phy_reg[PHY_CTRL] & MII_CR_AUTO_NEG_EN &&

        s->phy_reg[PHY_CTRL] & MII_CR_RESTART_AUTO_NEG) {

         s->phy_reg[PHY_STATUS] |= MII_SR_AUTONEG_COMPLETE;

    }

}