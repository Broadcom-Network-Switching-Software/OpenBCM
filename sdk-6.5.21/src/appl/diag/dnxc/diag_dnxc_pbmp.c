/** \file diag_dnxc_pbmp.c
 *
 * Diagnostic pack for PBMP
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_FABRIC

/* shared */
#include <shared/bsl.h>
/* appl */
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_utils.h>
/*soc*/
#ifdef BCM_DNXF_SUPPORT
#include <appl/diag/dnxf/diag_dnxf_fabric.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#endif

#define PRINT_PORT_PBMP_CELLS(name, pbmp) \
    do { \
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE); \
        PRT_CELL_SET(name); \
        sal_memset(buf, 0, (sizeof(char) * FORMAT_PBMP_MAX)); \
        PRT_CELL_SET("%s", shr_pbmp_range_format(pbmp, buf, sizeof(buf))); \
        print_counter++;\
    } while (0)

#define PRINT_PORT_PBMP_CELLS_CHECK(...) \
    do { \
        if (print_counter == 0) { \
            SHR_ERR_EXIT(_SHR_E_PARAM,"PBMP type not valid!\n"); \
        } \
    } while (0)

/* *INDENT-OFF* */
sh_sand_option_t dnxc_pbmp_options[] = {
    {"port", SAL_FIELD_TYPE_STR, "A port/pbmp string.", "", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

/* *INDENT-ON* */
static shr_error_e
diag_dnxc_pbmp_print(
    int unit,
    sh_sand_control_t * sand_control,
    char *port,
    int print_all)
{
    char buf[FORMAT_PBMP_MAX];
    int print_counter = 0;
    bcm_port_config_t port_config;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

     /* coverity[suspicious_sizeof:FALSE]  */
    PRT_TITLE_SET("Current Bitmaps");
    PRT_COLUMN_ADD("Ports");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Bit(s)");

    if (SOC_PBMP_NOT_NULL(port_config.fe))
    {
        if (sal_strcmp("fe", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("FE", port_config.fe);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.ge))
    {
        if (sal_strcmp("ge", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("GE", port_config.ge);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.xe))
    {
        if (sal_strcmp("xe", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("XE", port_config.xe);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.ce))
    {
        if (sal_strcmp("ce", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("CE", port_config.ce);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.cd))
    {
        if (sal_strcmp("cd", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("CD", port_config.cd);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.cc))
    {
        if (sal_strcmp("cc", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("CC", port_config.cc);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.le))
    {
        if (sal_strcmp("le", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("LE", port_config.le);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.e))
    {
        if (sal_strcmp("e", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("E", port_config.e);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.xl))
    {
        if (sal_strcmp("xl", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("XL", port_config.xl);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.il))
    {
        if (sal_strcmp("il", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("IL", port_config.il);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.rcy))
    {
        if (sal_strcmp("rcy", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("RCY", port_config.rcy);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.port))
    {
        if (sal_strcmp("port", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("PORT", port_config.port);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.cpu))
    {
        if (sal_strcmp("cpu", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("CPU", port_config.cpu);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.sci))
    {
        if (sal_strcmp("sci", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("SCI", port_config.sci);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.sfi))
    {
        if (sal_strcmp("sfi", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("SFI", port_config.sfi);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.spi))
    {
        if (sal_strcmp("spi", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("SPI", port_config.spi);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.spi_subport))
    {
        if (sal_strcmp("spi_subport", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("SPI_SUB", port_config.spi_subport);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.stack_ext))
    {
        if (sal_strcmp("stack_ext", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("STK", port_config.stack_ext);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.hg))
    {
        if (sal_strcmp("hg", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("HiG", port_config.hg);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.tdm))
    {
        if (sal_strcmp("tdm", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("TDM", port_config.tdm);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.pon))
    {
        if (sal_strcmp("pon", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("PON", port_config.pon);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.llid))
    {
        if (sal_strcmp("llid", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("LLID", port_config.llid);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.sat))
    {
        if (sal_strcmp("sat", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("SAT", port_config.sat);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.ipsec))
    {
        if (sal_strcmp("ipsec", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("IPSEC", port_config.ipsec);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.control))
    {
        if (sal_strcmp("control", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("CTRL", port_config.control);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.eventor))
    {
        if (sal_strcmp("eventor", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("EVENTOR", port_config.eventor);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.olp))
    {
        if (sal_strcmp("olp", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("OLP", port_config.olp);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.oamp))
    {
        if (sal_strcmp("oamp", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("OAMP", port_config.oamp);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.erp))
    {
        if (sal_strcmp("erp", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("ERP", port_config.erp);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.crps))
    {
        if (sal_strcmp("crps", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("CRPS", port_config.crps);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.nif))
    {
        if (sal_strcmp("nif", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("NIF", port_config.nif);
        }
    }

    if (SOC_PBMP_NOT_NULL(port_config.all))
    {
        if (sal_strcmp("all", port) == 0 || print_all)
        {
            PRINT_PORT_PBMP_CELLS("ALL", port_config.all);
        }
    }

    PRINT_PORT_PBMP_CELLS_CHECK();
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * { PBMP Printout functions
 */
shr_error_e
cmd_dnxc_pbmp(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *ports = NULL;
    int is_port_present = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("port", is_port_present);
    if (TRUE == is_port_present)
    {
        SH_SAND_GET_STR("port", ports);
        diag_dnxc_pbmp_print(unit, sand_control, ports, FALSE);
    }
    else
    {
        diag_dnxc_pbmp_print(unit, sand_control, "", TRUE);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * } PBMP Printout functions
 */

/* *INDENT-OFF* */
sh_sand_man_t sh_dnxc_pbmp_man = {
    .brief =    "Convert port bitmap string to hex.",
    .full =     "Parameters: <pbmp>\n"
                "        Converts a pbmp string into a hardware port bitmap.  A pbmp string\n"
                "        is a single port, or a group of ports specified in a list using ','\n"
                "        to separate them and '-' for ranges, e.g. 1-8,25,cpu.  Ports may be\n"
                "        specified using the one-based port number (1-29) or port type and\n"
                "        zero-based number (fe0-fe23,ge0-ge7).  'cpu' is the CPU port,\n"
                "        'fe' is all FE ports, 'ge' is all GE ports, 'e' is all ethernet\n"
                "        ports, 'all' is all ports, and 'none' is no ports (0x0).\n"
                "        A '~' may be used to exclude port previously given (e.g. e,~fe19)\n"
                "        Acceptable strings and values also depend on the chip being used.\n"
                "        A pbmp may also be given as a raw hex (0x) number, e.g. 0xbffffff.\n",
    .synopsis = "<pbmp>",
    .examples = "cpu\n"
};


/* *INDENT-ON* */
