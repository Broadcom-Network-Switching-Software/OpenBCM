/** \file diag_dnxc_soc.c
 *
 * soc diagnostic pack for soc
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_SHELL

/* shared */
#include <shared/bsl.h>
/* appl */
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_utils.h>
/*soc*/

/*
 * { SOC Printout functions
 */
shr_error_e
cmd_dnxc_soc(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint16 dev_id;
    uint8 rev_id;
    soc_control_t *soc;
    soc_stat_t *stat;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    soc = SOC_CONTROL(unit);
    stat = &soc->stat;

    SHR_IF_ERR_EXIT(soc_cm_get_id(unit, &dev_id, &rev_id));

    /*
     * { Device general info table
     */
    PRT_TITLE_SET("Device General Info");
    PRT_INFO_ADD("UNIT %d Driver Control Structure", unit);

    PRT_COLUMN_ADD("Chip");
    PRT_COLUMN_ADD("Rev");
    PRT_COLUMN_ADD("Driver");
    PRT_COLUMN_ADD("Family");
    PRT_COLUMN_ADD("Board type");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("%s", soc_dev_name(unit));
    PRT_CELL_SET("0x%x", rev_id);
    PRT_CELL_SET("%s", SOC_CHIP_NAME(soc->chip_driver->type));
    PRT_CELL_SET("%s", SOC_CHIP_STRING(unit));
    PRT_CELL_SET("0x%x", soc->board_type);

    PRT_COMMITX;
    /*
     * } Device general info table
     */
    /*
     * { Device status table
     */
    PRT_TITLE_SET("Device Status");
    PRT_COLUMN_ADD("Flags = 0x%x", soc->soc_flags);

    if (soc->soc_flags & SOC_F_ATTACHED)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("attached");
    }
    if (soc->soc_flags & SOC_F_INITED)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("initialized");
    }
    if (soc->soc_flags & SOC_F_LSE)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("link-scan");
    }
    if (soc->soc_flags & SOC_F_SL_MODE)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("sl-mode");
    }
    if (soc->soc_flags & SOC_F_POLLED)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("polled");
    }
    if (soc->soc_flags & SOC_F_URPF_ENABLED)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("urpf");
    }
    if (soc->soc_flags & SOC_F_MEM_CLEAR_USE_DMA)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("mem-clear-use-dma");
    }
    if (soc->soc_flags & SOC_F_IPMCREPLSHR)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("ipmc-repl-shared");
    }
    if (soc->remote_cpu)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("rcpu");
    }

    PRT_COMMITX;
    /*
     * } Device status table
     */

    /*
     * { Device additional info
     */
    PRT_TITLE_SET("Device Additional Info");

    PRT_COLUMN_ADD("Feature");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Values");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("CM");
    PRT_CELL_SET("Base=%p", (void *) CMVEC(unit).base_address);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Disabled");
    PRT_CELL_SET("reg_flags=0x%x mem_flags=0x%x", soc->disabled_reg_flags, soc->disabled_mem_flags);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Debug");
    PRT_CELL_SET("SchanOps=%d", stat->schan_op);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Counter");
    PRT_CELL_SET("int=%dus", soc->counter_interval);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Timeout");
    PRT_CELL_SET("Schan=%dus MIIM=%d(%dus)", soc->schanTimeout, stat->err_mii_tmo, soc->miimTimeout);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Intr");
    PRT_CELL_SET("Total=%d Sc=%d ScErr=%d MMU/ARLErr=%d", stat->intr, stat->intr_sc, stat->intr_sce, stat->intr_mmu);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("PCI");
    PRT_CELL_SET("LinkStat=%d PCIfatal=%d PCIparity=%d", stat->intr_ls, stat->intr_pci_fe, stat->intr_pci_pe);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("ARL");
    PRT_CELL_SET("ARLdrop=%d ARLmbuf=%d ARLxfer=%d ARLcnt0=%d", stat->intr_arl_d, stat->intr_arl_m, stat->intr_arl_x,
                 stat->intr_arl_0);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("DMA");
    PRT_CELL_SET("TableDMA=%d TSLAM-DMA=%d", stat->intr_tdma, stat->intr_tslam);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Mem");
    PRT_CELL_SET("TMemCmd[BSE]=%d MemCmd[CSE]=%d MemCmd[HSE]=%d", stat->intr_mem_cmd[0], stat->intr_mem_cmd[1],
                 stat->intr_mem_cmd[2]);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("ChipFunc");
    PRT_CELL_SET("ChipFunc[0]=%d ChipFunc[1]=%d ChipFunc[2]=%d ChipFunc[3]=%d ChipFunc[4]=%d", stat->intr_chip_func[0],
                 stat->intr_chip_func[1], stat->intr_chip_func[2], stat->intr_chip_func[3], stat->intr_chip_func[4]);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("I2C");
    PRT_CELL_SET("I2C=%d MII=%d StatsDMA=%d Desc=%d Chain=%d", stat->intr_i2c, stat->intr_mii, stat->intr_stats,
                 stat->intr_desc, stat->intr_chain);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * } SOC Printout functions
 */

/*
 * } LOCAL DIAG PACK
 */

/* *INDENT-OFF* */
sh_sand_man_t sh_dnxc_soc_man = {
    .brief =    "Display SOC Info.",
    .full =     "Print internal SOC driver control information",
};


/* *INDENT-ON* */
