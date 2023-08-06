static void build_processor_devices(Aml *sb_scope, unsigned acpi_cpus,

                                    AcpiCpuInfo *cpu, AcpiPmInfo *pm)

{

    int i;

    Aml *dev;

    Aml *crs;

    Aml *pkg;

    Aml *field;

    Aml *ifctx;

    Aml *method;



    

    QEMU_BUILD_BUG_ON(ACPI_CPU_HOTPLUG_ID_LIMIT > 256);

    g_assert(acpi_cpus <= ACPI_CPU_HOTPLUG_ID_LIMIT);



    

    dev = aml_device("PCI0." stringify(CPU_HOTPLUG_RESOURCE_DEVICE));

    aml_append(dev, aml_name_decl("_HID", aml_eisaid("PNP0A06")));

    aml_append(dev,

        aml_name_decl("_UID", aml_string("CPU Hotplug resources"))

    );

    

    aml_append(dev, aml_name_decl("_STA", aml_int(0xB)));

    crs = aml_resource_template();

    aml_append(crs,

        aml_io(AML_DECODE16, pm->cpu_hp_io_base, pm->cpu_hp_io_base, 1,

               pm->cpu_hp_io_len)

    );

    aml_append(dev, aml_name_decl("_CRS", crs));

    aml_append(sb_scope, dev);

    

    aml_append(sb_scope, aml_operation_region(

        "PRST", AML_SYSTEM_IO, aml_int(pm->cpu_hp_io_base), pm->cpu_hp_io_len));

    field = aml_field("PRST", AML_BYTE_ACC, AML_NOLOCK, AML_PRESERVE);

    aml_append(field, aml_named_field("PRS", 256));

    aml_append(sb_scope, field);



    

    for (i = 0; i < acpi_cpus; i++) {

        dev = aml_processor(i, 0, 0, "CP%.02X", i);



        method = aml_method("_MAT", 0, AML_NOTSERIALIZED);

        aml_append(method,

            aml_return(aml_call1(CPU_MAT_METHOD, aml_int(i))));

        aml_append(dev, method);



        method = aml_method("_STA", 0, AML_NOTSERIALIZED);

        aml_append(method,

            aml_return(aml_call1(CPU_STATUS_METHOD, aml_int(i))));

        aml_append(dev, method);



        method = aml_method("_EJ0", 1, AML_NOTSERIALIZED);

        aml_append(method,

            aml_return(aml_call2(CPU_EJECT_METHOD, aml_int(i), aml_arg(0)))

        );

        aml_append(dev, method);



        aml_append(sb_scope, dev);

    }



    

    

    method = aml_method(AML_NOTIFY_METHOD, 2, AML_NOTSERIALIZED);

    for (i = 0; i < acpi_cpus; i++) {

        ifctx = aml_if(aml_equal(aml_arg(0), aml_int(i)));

        aml_append(ifctx,

            aml_notify(aml_name("CP%.02X", i), aml_arg(1))

        );

        aml_append(method, ifctx);

    }

    aml_append(sb_scope, method);



    

    pkg = acpi_cpus <= 255 ? aml_package(acpi_cpus) :

                             aml_varpackage(acpi_cpus);



    for (i = 0; i < acpi_cpus; i++) {

        uint8_t b = test_bit(i, cpu->found_cpus) ? 0x01 : 0x00;

        aml_append(pkg, aml_int(b));

    }

    aml_append(sb_scope, aml_name_decl(CPU_ON_BITMAP, pkg));

}