static void ich9_apm_ctrl_changed(uint32_t val, void *arg)

{

    ICH9LPCState *lpc = arg;



    

    acpi_pm1_cnt_update(&lpc->pm.acpi_regs,

                        val == ICH9_APM_ACPI_ENABLE,

                        val == ICH9_APM_ACPI_DISABLE);

    if (val == ICH9_APM_ACPI_ENABLE || val == ICH9_APM_ACPI_DISABLE) {

        return;

    }



    

    if (lpc->pm.smi_en & ICH9_PMIO_SMI_EN_APMC_EN) {

        cpu_interrupt(current_cpu, CPU_INTERRUPT_SMI);

    }

}