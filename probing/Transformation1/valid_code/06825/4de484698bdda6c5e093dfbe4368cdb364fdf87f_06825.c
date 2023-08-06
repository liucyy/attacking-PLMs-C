void ahci_command_wait(AHCIQState *ahci, AHCICommand *cmd)

{

    

    while (BITSET(ahci_px_rreg(ahci, cmd->port, AHCI_PX_TFD),

                  AHCI_PX_TFD_STS_BSY) ||

           BITSET(ahci_px_rreg(ahci, cmd->port, AHCI_PX_CI), (1 << cmd->slot))) {

        usleep(50);

    }

}