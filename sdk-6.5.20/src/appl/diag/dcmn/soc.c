/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        soc.c
 * Purpose:     SOC related CLI commands
 */



#include <shared/bsl.h>

#include <sal/appl/sal.h>
#include <sal/appl/pci.h>
#include <soc/mem.h>

#include <soc/drv.h>
#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/drv.h>
#ifdef BCM_JERICHO_SUPPORT
#include <soc/dpp/JER/jer_mgmt.h>
#endif
#endif
#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#endif
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_drv.h>
#endif

#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/dcmn/counter.h>

#include <appl/diag/dcmn/dcmn_patch_database.h>

#include <soc/defs.h>

#include <bcm/init.h>


static parse_key_t dpp_show_ctr_arg[] = {
    "Changed",                          /* 0 */
    "Same",                             /* 1 */
    "Zero",                             /* 2 */
    "NonZero",                          /* 3 */
    "Hex",                              /* 4 */
    "Raw",                              /* 5 */
    "All",                              /* 6 */
    "ErDisc",                           /* 7 */
    "Total",                            /* 8 */
    "Fabric",                           /* 9 */
    "Nif",                              /* 10 */
    "Compact",                          /* 11 */
    "Full"                              /* 12 */
};

static int dpp_show_ctr_arg_cnt = PARSE_ENTRIES(dpp_show_ctr_arg);

static parse_key_t dpp_show_interrupt_arg[] = {
    "Zero",                             /* 0 */
    "Mask",                             /* 1 */
    "UNMask",                           /* 2 */
    "All"                               /* 3 */
};

static int dpp_show_interrupt_arg_cnt = PARSE_ENTRIES(dpp_show_interrupt_arg);

char cmd_dpp_show_usage[] =
    "Usages:\n"
#ifdef BCM_PETRA_SUPPORT
    "  show Pci        - Show PCI Bus\n"
	"  show Counters [Z] [NZ] [FABRIC] [NIF] [Hex] [Compact/Full] [<reg>][.][<pbmp>] [ErDisc] [Total]\n"
    "\tDisplay all counters, or only specified regs and/or ports\n"
    "\t  Z       - include counters that are zero\n"
    "\t  Nz      - include counters that are not zero\n"
    "\t  All     - same as: Changed Same Z Nz\n"
    "\t  Fabric  - display FABRIC related counters\n"
    "\t  NIF     - display NIF related counters\n"
    "\t  Hex     - display counters as 64-bit hex values\n"
    "\t  ErDisc  - only show those counters marked with Error/Discard\n"
    "\t  Compact - display most popular counters\n"
    "\t  Full - display all available counters\n"
    "\t  Total   - dispaly total of all counters\n"
    "\tNOTES:\n"
#else
    "  show Counters [Changed] [Same] [Z] [NZ] [FABRIC] [NIF] [Hex] [Raw] [<reg>][.][<pbmp>] [Total]\n"
    "\tDisplay all counters, or only specified regs and/or ports\n"
    "\t  Changed - include counters that changed\n"
    "\t  Same    - include counters that did not change\n"
    "\t  Z       - include counters that are zero\n"
    "\t  Nz      - include counters that are not zero\n"
    "\t  All     - same as: Changed Same Z Nz\n"
    "\t  Fabric     - display FABRIC related counters\n"
    "\t  NIF     - display NIF related counters\n"
    "\t  Hex     - display counters as 64-bit hex values\n"
    "\t  Raw     - display raw 64-bit hex values, no register name(s)\n"
    "\t  ErDisc  - only show those counters marked with Error/Discard\n"
    "\t  Total   - dispaly total of all counters\n"
    "\tNOTES: If neither Changed or Same is specified, Changed is used.\n"
#endif
    "\t       If neither Z or Nz is specified, Nz is used.\n"
    "\t       If neither Fabric or NIF is specified, NIF is used.\n"
    "\t       Use All to display counters regardless of value.\n"
    "  show Statistics [pbm] [all] - SNMP accumulated statistics, "
    "all shows 0s\n"
    "  show Interrupts [Z] [Mask] [All] [UNMask] - Display interrupt Status/Statictics.\n"
    "\tDefault: Display asserted interrupts.\n"
    "\tZ      - Display cleared interrupts.\n"
    "\tAll    - Display All interrupts.\n"
    "\tMask   - Disaplsy Masked interrupts.\n"
    "\tUNMask - Disaplsy UnMasked interrupts.\n"
    "  show unit [<unit>]     - Unit list or unit parameters\n"
#if defined(BROADCOM_DEBUG)
    "  show features [all]    - Show enabled (or all) features for this unit\n"
#endif
    "  show PAtches           - Show all installed patches\n"
    "  show temp-PVT          - Show PVT temperature\n" 
    ;

#define DIAG_DCMN_SOC_PVT_MAX_NOF_MONITORS      (4)

cmd_result_t
cmd_dpp_show(int unit, args_t *a)
{
    char                *c;

    if (!(c = ARG_GET(a))) {    /* Nothing to do    */
        return(CMD_USAGE);      /* Print usage line */
    }

   if (!sal_strcasecmp(c, "unit") || !sal_strcasecmp(c, "units")) {
        int             u;
        soc_control_t   *usoc;

        c = ARG_GET(a);
        if (c != NULL) {        /* specific unit */
            u = sal_ctoi(c, 0);
            if (!SOC_UNIT_VALID(u)) {
                cli_out("Unit %d is not valid\n", u);
                return CMD_FAIL;
            }
            usoc = SOC_CONTROL(u);
            if (!(usoc->soc_flags & SOC_F_ATTACHED)) {
                cli_out("Unit %d (detached)\n", u);
                return CMD_OK;
            }
            cli_out("Unit %d chip %s%s\n",
                    u,
                    soc_dev_name(u),
                    u == unit ? " (current)" : "");
            
#ifdef BCM_PETRA_SUPPORT
            if(SOC_IS_DPP(u)) {
                soc_dpp_chip_dump(u, SOC_DRIVER(u));
            } else
#endif
#ifdef BCM_DFE_SUPPORT
            if(SOC_IS_DFE(u)) {
                soc_dfe_chip_dump(u, SOC_DRIVER(u));
            } else
#endif
            {
                cli_out("Unit %d can't show non dpp nor dfe device",u);
                return CMD_FAIL;
            }
        } else {                /* all units */
            for (u = 0; u < soc_ndev; u++) {
                if (!SOC_UNIT_VALID(SOC_NDEV_IDX2DEV(u))) {
                    continue;
                }
                usoc = SOC_CONTROL(SOC_NDEV_IDX2DEV(u));
                cli_out("Unit %d chip %s%s\n",
                        SOC_NDEV_IDX2DEV(u),
                        soc_dev_name(SOC_NDEV_IDX2DEV(u)),
                        SOC_NDEV_IDX2DEV(u) == unit ? " (current)" : "");
            }
        }
        return CMD_OK;
    } else if (!sal_strcasecmp(c, "Counters") || !sal_strcasecmp(c, "c")) {
        soc_pbmp_t          pbmp, pbmp_nif, temp_pbmp;
        const parse_key_t   *cmd;
        soc_reg_t           ctr_reg;
        int                 flags, break_loop;

        /*first -  sync counters*/
        bcm_stat_sync(unit);
        
        flags = 0;
        break_loop = 0;
        while (!break_loop && (c = ARG_GET(a)) != NULL) {
            cmd = parse_lookup(c, dpp_show_ctr_arg, 
                               sizeof(dpp_show_ctr_arg[0]), 
                               dpp_show_ctr_arg_cnt);

            switch (cmd - dpp_show_ctr_arg) {
            case 0:
                flags |= SHOW_CTR_CHANGED;
                break;
            case 1:
                flags |= SHOW_CTR_SAME;
                break;
            case 2:
                flags |= SHOW_CTR_Z;
                break;
            case 3:
                flags |= SHOW_CTR_NZ;
                break;
            case 4:
                flags |= SHOW_CTR_HEX;
                break;
            case 5:
                flags |= SHOW_CTR_RAW;
                break;
            case 6:
                flags |= (SHOW_CTR_CHANGED | SHOW_CTR_SAME |
                          SHOW_CTR_Z | SHOW_CTR_NZ);
                break;
            case 7:
                flags |= SHOW_CTR_ED;
                break;
            case 8:
                flags |= SHOW_TOTAL;
                break;
            case 9:
                flags |= SHOW_FABRIC;
                break;
            case 10:
                flags |= SHOW_NIF;
                break;
            case 11:
                flags |= SHOW_COMPACT;
                break;
            case 12:
                flags |= SHOW_FULL;
                break;
            default:
                break_loop=1;
            }
        }

        /*
         * Supply defaults
         */

        if (((flags & (SHOW_CTR_CHANGED | SHOW_CTR_SAME)) == 0)) {
            flags |= SHOW_CTR_CHANGED;
        }

        if ((flags & (SHOW_COMPACT | SHOW_FULL)) == 0) {
            flags |= SHOW_COMPACT;
        }

        /*default when counter thread is active: nz*/
        /*default when counter thread is not active: z*/
        if ((SOC_CONTROL(unit)->counter_interval != 0)) {
            if ((flags & (SHOW_CTR_Z | SHOW_CTR_NZ)) == 0) {
                flags |= SHOW_CTR_NZ;
            }
        } else {
            flags &= ~SHOW_CTR_NZ;
            flags |= SHOW_CTR_Z;
        }
        SOC_PBMP_CLEAR(pbmp);
        if (c == NULL) {
            ctr_reg = INVALIDr;         /* All registers */

            /*in case no pbmp input - choose default nif ports/fabric ports*/
            if ((flags & (SHOW_FABRIC | SHOW_NIF)) == 0) {
                if(SOC_IS_DFE(unit) || SOC_IS_DNXF(unit)) {
                    flags |= SHOW_FABRIC;
                } else {
                    flags |= SHOW_NIF;
                }
            }            
        } else if (parse_pbmp(unit, c, &pbmp) >= 0) {
            flags |= SHOW_SELECTED_PORTS;
            ctr_reg = INVALIDr;         /* All registers, selected ports */
        } else {
            return CMD_USAGE;
        }

        if (!soc_feature(unit, soc_feature_cpuport_stat_dma)) {
            SOC_PBMP_CLEAR(temp_pbmp);
            SOC_PBMP_OR(temp_pbmp, PBMP_PORT_ALL(unit));
            SOC_PBMP_OR(temp_pbmp, PBMP_CMIC(unit));
            SOC_PBMP_AND(pbmp, temp_pbmp);
        }
         
        
        SOC_PBMP_CLEAR(temp_pbmp);
        if (flags & SHOW_NIF) {
            SOC_PBMP_CLEAR(pbmp_nif);
            SOC_PBMP_NEGATE(pbmp_nif, PBMP_SFI_ALL(unit));
            SOC_PBMP_AND(pbmp_nif, PBMP_PORT_ALL(unit));
            SOC_PBMP_OR(temp_pbmp, pbmp_nif);
        }
        if(flags & SHOW_FABRIC) {
            SOC_PBMP_OR(temp_pbmp, PBMP_SFI_ALL(unit));
        }
        SOC_PBMP_OR(pbmp, temp_pbmp);
        

        return (do_show_dpp_counters(unit, ctr_reg, pbmp, flags));
    }  
    else if (!sal_strcasecmp(c, "Interrupts") || !sal_strcasecmp(c, "i")) {
        int i, blk, rc, val, mask_val, flags, break_loop, nof_interrupts;
        soc_interrupt_db_t* interrupts;
        soc_block_info_t* bi = NULL;
        int port;
        const parse_key_t   *cmd;
        
        flags = 0;
        break_loop = 0;
        while (!break_loop && (c = ARG_GET(a)) != NULL) {
            cmd = parse_lookup(c, dpp_show_interrupt_arg, 
                               sizeof(dpp_show_interrupt_arg[0]), 
                               dpp_show_interrupt_arg_cnt);
            
            switch (cmd - dpp_show_interrupt_arg) {
                case 0:
                    flags |= SHOW_INTR_Z;
                    break;
                case 1:
                    flags |= SHOW_INTR_MASK;
                    break;
                case 2:
                    flags |= SHOW_INTR_UNMASK;
                    break;
                case 3:
                    flags |= SHOW_INTR_ALL;
                    break;
                default:
                     break_loop = 1;
                }
        }

        interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
        if(NULL == interrupts) {
            cli_out("Not supported for unit %d \n",unit);
            return CMD_FAIL;
        }

        cli_out("                       Interrupt Name                      | ID  |Mask| On |Count  \n");
        cli_out("-----------------------------------------------------------------------------------\n");
                  
        rc = soc_nof_interrupts(unit, &nof_interrupts);
        if(SOC_FAILURE(rc)) {
            cli_out("Failed in soc_nof_interrupts()\n");
            return CMD_FAIL;
        }

        for(i=0 ; i < nof_interrupts ; i++) {

          if (!SOC_REG_IS_VALID(unit, interrupts[i].reg)) {
                continue;
            }

            SOC_BLOCKS_ITER(unit, blk, SOC_REG_INFO(unit,interrupts[i].reg).block) {  

                if (!SOC_INFO(unit).block_valid[blk]) {
                    continue;  
                }

                bi = &(SOC_BLOCK_INFO(unit, blk));

                port = 
                    (bi->type == SOC_BLK_CLP || bi->type == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit, blk) : bi->number;
                
                rc = soc_interrupt_get(unit, port, &(interrupts[i]), &val);
                if(SOC_FAILURE(rc)) {
                     cli_out("Failed to read counter: %s\n", interrupts[i].name);
                     continue;
                }
                
                rc = soc_interrupt_is_enabled(unit, port, &(interrupts[i]), &mask_val);
                if(SOC_FAILURE(rc)) {
                    cli_out("Failed to read interrupt mask: %s\n", interrupts[i].name);
                    continue;
                }

                if( (val && !(flags & SHOW_INTR_Z)) ||
                   (!val &&  (flags & SHOW_INTR_Z))||
                   (flags & SHOW_INTR_ALL)) {
                    
                    if((mask_val && !(flags & SHOW_INTR_MASK)) ||
                        (!mask_val && !(flags & SHOW_INTR_UNMASK)) ) {

                         cli_out("%55s[%d] |%4d |%3d |%3d |%3d  \n" ,
                                 interrupts[i].name, bi->number,i   , mask_val, val, interrupts[i].statistics_count[bi->number]);
                    }

                }
            }
        }
      
      return CMD_OK;
    }
    else if (!sal_strcasecmp(c, "Patches") || !sal_strcasecmp(c, "pa")) {
        int num_of_installed, index;
        appl_diag_dcmn_patch installed_patches[TOTAL_PATCHES];
        char *soc_dcmn_version;

        appl_diag_dcmn_patches_installed_get(unit, TOTAL_PATCHES, installed_patches, &num_of_installed);

        appl_diag_dcmn_version_get(unit, &soc_dcmn_version);
        cli_out("Current version installed: %s\n", soc_dcmn_version); 

        if(num_of_installed == 0) {
            cli_out("No patches have been installed\n");
        }
        else{
            cli_out("Patches installed:\n");
            for (index = 0; index < num_of_installed; index++) { 
                cli_out("Patch%2d: %s\n", installed_patches[index].id, installed_patches[index].str);
            }
        }
        return (CMD_OK);
    }

    else if (!sal_strcasecmp(c, "temp-PVT") || !sal_strcasecmp(c, "pvt")) {
        int temp_int, temp_remainder, rc = BCM_E_UNAVAIL;
        bcm_switch_temperature_monitor_t temp_monitor[DIAG_DCMN_SOC_PVT_MAX_NOF_MONITORS];
        int i, temp_monitor_count;
        int peak_int, peak_remainder;
#ifdef BCM_JERICHO_SUPPORT
        uint8 enable;
        int faulty_mon_counter = 0;
        int interval;
#endif
        rc = bcm_switch_temperature_monitor_get(unit, DIAG_DCMN_SOC_PVT_MAX_NOF_MONITORS, temp_monitor, &temp_monitor_count);
        if (BCM_E_NONE != rc) {
            cli_out("Failed to get PVT\n");
            return (CMD_FAIL);
        }

        for (i = 0; i < temp_monitor_count; i++)
        {
            temp_int = temp_monitor[i].curr / 10;
            temp_remainder = temp_monitor[i].curr % 10;
            temp_remainder *= temp_remainder < 0 ? -1 : 1;  /*fix sign*/
            cli_out("temp pvt[%d] curr: %d.%dC\n", i, temp_int, temp_remainder);
            if (temp_monitor[i].peak != -1)
            {
                peak_int = temp_monitor[i].peak / 10; 
                peak_remainder = temp_monitor[i].peak % 10;
                peak_remainder *= peak_remainder < 0 ? -1 : 1;  /*fix sign*/
                cli_out("temp pvt[%d] peak: %d.%dC\n", i, peak_int, peak_remainder);
            }
        }

#ifdef BCM_JERICHO_SUPPORT
        if(SOC_IS_JERICHO_PLUS_ONLY(unit)) {
            rc = jer_mgmt_pvt_monitor_correction_status_get(unit, &enable, &interval, &faulty_mon_counter);
            if (BCM_E_NONE != rc) {
                cli_out("Failed to get PVT correction status\n");
                return (CMD_FAIL);
            }
            if (faulty_mon_counter > 0) {
                cli_out("\nThe PVT monitor correction task %s, interval is %d.\n", (enable != 0)?"is running":"is not running", interval);
            }
        }
#endif
    } else if (!sal_strcasecmp(c, "pci")) {
#if !defined(BCM_ICS) && !defined(LINUX) && !defined(GHS)
        pci_print_all();
#endif
    } 
#if defined(BROADCOM_DEBUG)
    else if (!sal_strcasecmp(c, "feature") || !sal_strcasecmp(c, "features")) {
        int		all;
        soc_feature_t	f;

        c = ARG_GET(a);
        if (c == NULL) {
            all = 0;
        } else {
            all = 1;
        }
        cli_out("Unit %d features:\n", unit);

        for (f = 0; f < soc_feature_count; f++) {
            if (soc_feature(unit, f)) {
                cli_out("\t%s\n", soc_feature_name[f]);
            } else if (all) {
                cli_out("\t[%s]\n", soc_feature_name[f]);
            }
        }
        return CMD_OK;
    }
#endif /*BROADCOM_DEBUG*/
    else {
        cli_out(">>>>> cmd_dpp_show(): No such option for 'show': '%s' --- Hit 'show' for help\r\n\n", c);
        return(CMD_USAGE);
    }
    return (CMD_OK);
}

char cmd_dpp_clear_usage[] =
    "Usages:\n"
    "  clear counters [PBMP]\n"
    ;


cmd_result_t
cmd_dpp_clear(int unit, args_t *a)
{
    char *parm = ARG_GET(a);
    int force_all = 0;
    uint64  val64;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (!parm) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(parm, "all")) {
        force_all = 1;
        parm = ARG_GET(a);
    }

    if (!parm) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(parm, "counters") || !sal_strcasecmp(parm, "c") || force_all) {
        int        rv = CMD_OK;
        soc_port_t    port;
        bcm_pbmp_t      bcm_pbmp;
        bcm_port_config_t pcfg;

        if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
            cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
            return CMD_FAIL;
        }

        if ((parm = ARG_GET(a)) == NULL) {
            SOC_PBMP_CLEAR(bcm_pbmp);
            if (soc_feature(unit, soc_feature_cpuport_stat_dma)) {
                SOC_PBMP_ASSIGN(bcm_pbmp, pcfg.all);
            } else {
                SOC_PBMP_ASSIGN(bcm_pbmp, pcfg.port);
            }
        } else if (parse_bcm_pbmp(unit, parm, &bcm_pbmp) < 0) {
            cli_out("%s: Invalid port bitmap: %s\n", ARG_CMD(a), parm);
            return CMD_FAIL;
        }
        SOC_PBMP_REMOVE(bcm_pbmp, PBMP_RCY_ALL(unit));
        if (soc_feature(unit, soc_feature_cpuport_stat_dma)) {
            SOC_PBMP_AND(bcm_pbmp, pcfg.all);
        } else {
            SOC_PBMP_AND(bcm_pbmp, pcfg.port);
        }

        SOC_PBMP_ITER(bcm_pbmp, port) {
            /*clear counters*/
            if ((rv = bcm_stat_clear(unit, port)) != BCM_E_NONE) {
                cli_out("%s: Unit %d Port %d failed to clear stats: %s\n",
                        ARG_CMD(a), unit, port, bcm_errmsg(rv));
                rv = CMD_FAIL;
            }
            /*clear diag counters data base*/
            COMPILER_64_ZERO(val64);
            dpp_counter_val_set(unit, port, INVALIDr, 0, val64);
    }
    
        if (!force_all) return (rv);
    }



    return CMD_OK;
}

char cmd_dpp_soc_usage[] =
    "Parameters: [<unit #>] ... \n\t"
    "Print internal SOC driver control information IF compiled as a \n\t"
    "debug version. If not compiled as a debug version, a warning is\n\t"
    "printed and the command completes successfully with no further\n\t"
    "output\n";

/*
 * Function:    sh_soc
 * Purpose:     Print soc control information if compiled in debug mode.
 * Parameters:  u - unit #
 *              a - pointer to args, expects <unit> ...., if none passed,
 *                      default unit # used.
 * Returns:     CMD_OK/CMD_FAIL
 */
cmd_result_t
cmd_dpp_soc(int unit, args_t *a)
{
#if defined(BROADCOM_DEBUG)
    char        *c;
    cmd_result_t rv = CMD_OK;

    if (!sh_check_attached("soc", unit)) {
        return(CMD_FAIL);
    }

    if (!ARG_CNT(a)) {
#ifdef BCM_PETRA_SUPPORT
        if(SOC_IS_DPP(unit)) {
            rv = soc_dpp_dump(unit, "  ") ? CMD_FAIL : CMD_OK;
        } else
#endif
#ifdef BCM_DFE_SUPPORT
        if(SOC_IS_DFE(unit)) {
            rv = soc_dfe_dump(unit, "  ") ? CMD_FAIL : CMD_OK;
        } else
#endif
#ifdef BCM_DNXF_SUPPORT
        if(SOC_IS_DNXF(unit)) {
            rv = soc_dnxf_dump(unit, "  ") ? CMD_FAIL : CMD_OK;
        } else
#endif
        {
            cli_out("Unit %d can't handle non dpp nor dfe device",unit);
            rv = CMD_FAIL;
        }   
    } else {
        while ((CMD_OK == rv) && (c = ARG_GET(a))) {
            if (!isint(c)) {
                cli_out("%s: Invalid unit identifier: %s\n", ARG_CMD(a), c);
                rv = CMD_FAIL;
            } else {
#ifdef BCM_PETRA_SUPPORT
                if(SOC_IS_DPP(unit)) {
                    rv = soc_dpp_dump(parse_integer(c), "  ") ? CMD_FAIL : CMD_OK;
                } else
#endif
#ifdef BCM_DFE_SUPPORT
                if(SOC_IS_DFE(unit)) {
                        rv = soc_dfe_dump(parse_integer(c), "  ") ? CMD_FAIL : CMD_OK;
                } else
#endif
#ifdef BCM_DNXF_SUPPORT
                if(SOC_IS_DNXF(unit)) {
                        rv = soc_dnxf_dump(parse_integer(c), "  ") ? CMD_FAIL : CMD_OK;
                } else
#endif
                {
                    cli_out("Unit %d can't handle non dpp nor dfe device",unit);
                    rv = CMD_FAIL;
                } 
            }
        }
    }
    return(rv);
#else /* !defined(BROADCOM_DEBUG) */
    cli_out("%s: Warning: Not compiled with BROADCOM_DEBUG enabled\n",
            ARG_CMD(a));
    return(CMD_OK);
#endif /* defined(BROADCOM_DEBUG) */
}


char cmd_dpp_pbmp_usage[] =
    "Parameters: <pbmp>\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tConverts a pbmp string into a hardware port bitmap.  A pbmp string\n\t"
    "is a single port, or a group of ports specified in a list using ','\n\t"
    "to separate them and '-' for ranges, e.g. 1-8,25,cpu.  Ports may be\n\t"
    "specified using the one-based port number (1-29) or port type and\n\t"
    "zero-based number (fe0-fe23,ge0-ge7).  'cpu' is the CPU port,\n\t"
    "'fe' is all FE ports, 'ge' is all GE ports, 'e' is all ethernet\n\t"
    "ports, 'all' is all ports, and 'none' is no ports (0x0).\n\t"
    "A '~' may be used to exclude port previously given (e.g. e,~fe19)\n\t"
    "Acceptable strings and values also depend on the chip being used.\n\t"
    "A pbmp may also be given as a raw hex (0x) number, e.g. 0xbffffff.\n"
#endif
    ;

cmd_result_t
cmd_dpp_pbmp(int unit, args_t *a)
{
    pbmp_t              pbmp;
    char                *c;
    soc_port_t          port;
    char                pbmp_str[FORMAT_PBMP_MAX];
    char                pfmt[SOC_PBMP_FMT_LEN];

    COMPILER_REFERENCE(unit);

    c = ARG_GET(a);

    if (!c) {
        cli_out("Current bitmaps:\n");
        cli_out("     GE     ==> %s\n",
                SOC_PBMP_FMT(PBMP_GE_ALL(unit), pfmt));
        cli_out("     XE     ==> %s\n",
                SOC_PBMP_FMT(PBMP_XE_ALL(unit), pfmt));
        cli_out("     CE     ==> %s\n",
                SOC_PBMP_FMT(PBMP_CE_ALL(unit), pfmt));
        cli_out("     E      ==> %s\n",
                SOC_PBMP_FMT(PBMP_E_ALL(unit), pfmt));
        cli_out("     IL     ==> %s\n",
                SOC_PBMP_FMT(PBMP_IL_ALL(unit), pfmt));
        cli_out("     XL     ==> %s\n",
                SOC_PBMP_FMT(PBMP_XL_ALL(unit), pfmt));
        cli_out("     ST     ==> %s\n",
                SOC_PBMP_FMT(PBMP_ST_ALL(unit), pfmt));
        cli_out("     RCY    ==> %s\n",
                SOC_PBMP_FMT(PBMP_RCY_ALL(unit), pfmt));
        cli_out("     PORT   ==> %s\n",
                SOC_PBMP_FMT(PBMP_PORT_ALL(unit), pfmt));
        cli_out("     CMIC   ==> %s\n",
                SOC_PBMP_FMT(PBMP_CMIC(unit), pfmt));
        cli_out("     LB     ==> %s\n",
                SOC_PBMP_FMT(PBMP_LB(unit), pfmt));
        cli_out("     SFI    ==> %s\n",
                SOC_PBMP_FMT(PBMP_SFI_ALL(unit), pfmt));
        cli_out("     PON    ==> %s\n",
                SOC_PBMP_FMT(PBMP_PON_ALL(unit), pfmt));
        cli_out("     QSGMII ==> %s\n",
                SOC_PBMP_FMT(PBMP_QSGMII_ALL(unit), pfmt));
        cli_out("     LBG    ==> %s\n",
                SOC_PBMP_FMT(PBMP_LBG_ALL(unit), pfmt));
        cli_out("     TDM    ==> %s\n",
                SOC_PBMP_FMT(PBMP_TDM_ALL(unit), pfmt));
        cli_out("     ALL    ==> %s\n",
                SOC_PBMP_FMT(PBMP_ALL(unit), pfmt));
        return CMD_OK;
    }

    if (sal_strcasecmp(c, "bcm") == 0) {
        /* return cmd_esw_bcm_pbmp(unit, a); */
    }

    if (sal_strcasecmp(c, "port") == 0) {
        if ((c = ARG_GET(a)) == NULL) {
            cli_out("ERROR: missing port string\n");
            return CMD_FAIL;
        }
        if (parse_port(unit, c, &port) < 0) {
            cli_out("%s: Invalid port string: %s\n", ARG_CMD(a), c);
            return CMD_FAIL;
        }
        cli_out("    port %s ==> %s (%d)\n",
                c, SOC_PORT_NAME(unit, port), port);
        return CMD_OK;
    }

    if (parse_pbmp(unit, c, &pbmp) < 0) {
        cli_out("%s: Invalid pbmp string (%s); use 'pbmp ?' for more info.\n",
                ARG_CMD(a), c);
        return CMD_FAIL;
    }

    format_pbmp(unit, pbmp_str, sizeof (pbmp_str), pbmp);

    cli_out("    %s ==> %s\n", SOC_PBMP_FMT(pbmp, pfmt), pbmp_str);

    return CMD_OK;
}



/*
 * Utility routine for BIST command as well as BIST diagnostic
 */

STATIC void
dpp_bist_add_mem(soc_mem_t *mems, int *num_mems, soc_mem_t mem)
{
    int     i;

    for (i = 0; i < *num_mems; i++) {
  if (mems[i] == mem) {
      return;   /* Already there */
  }
    }

    mems[(*num_mems)++] = mem;
}

int
dpp_bist_args_to_mems(int unit, args_t *a, soc_mem_t *mems, int *num_mems)
{
    char    *parm;
    soc_mem_t   mem;
    int     rv = 0;

    *num_mems = 0;

    while ((parm = ARG_GET(a)) != 0) {
  if (sal_strcasecmp(parm, "all") == 0) {
      for (mem = 0; mem < NUM_SOC_MEM; mem++) {
    if (soc_mem_is_valid(unit, mem) &&
        (soc_mem_is_bistepic(unit, mem) ||
         soc_mem_is_bistcbp(unit, mem) ||
         soc_mem_is_bistffp(unit, mem))) {
        dpp_bist_add_mem(mems, num_mems, mem);
    }
      }
  } else if (sal_strcasecmp(parm, "cbp") == 0) {
      for (mem = 0; mem < NUM_SOC_MEM; mem++) {
    if (soc_mem_is_valid(unit, mem) &&
        soc_mem_is_cbp(unit, mem)) {
        dpp_bist_add_mem(mems, num_mems, mem);
    }
      }
  } else if (parse_memory_name(unit, &mem, parm, 0, 0) < 0) {
      cli_out("BIST ERROR: Unknown memory name: %s\n", parm);
      rv = -1;
  } else {
      dpp_bist_add_mem(mems, num_mems, mem);
  }
    }

    if (*num_mems == 0) {
  cli_out("BIST ERROR: No memories specified\n");
  rv = -1;
    }

    return rv;
}

/*
 * BIST (non-diagnostic version)
 */

char dpp_bist_usage[] =
    "Parameters: [table] ...\n\t"
    "Runs raw BIST (built-in self-test) on list of BISTable memories.\n\t"
    "Use \"listmem\" to get a list of BISTable memories.\n\t"
    "The special name CBP can be used to refer to all CBP memories.\n";

cmd_result_t
dpp_cmd_bist(int unit, args_t *a)
{
    soc_mem_t   *mems;
    int     num_mems;
#if defined (BCM_ESW_SUPPORT)
    int     rv;
#endif

    /*BIST is not supported on XGSIII devices*/
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM56601, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM56602, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM56504, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM56102, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM56304, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM56112, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM56314, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM56514, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM5650, a);

    if (!sh_check_attached(ARG_CMD(a), unit)) {
  return CMD_FAIL;
    }

    mems = sal_alloc(sizeof(soc_mem_t) * NUM_SOC_MEM,
                                  "BIST mem list");
    if (NULL == mems) {
  cli_out("Insufficient memory for BIST\n");
        return BCM_E_MEMORY;
    }

    if (dpp_bist_args_to_mems(unit, a, mems, &num_mems) < 0) {
        sal_free(mems);
  return CMD_FAIL;
    }
#if defined (BCM_ESW_SUPPORT)
    if ((rv = soc_bist(unit,
           mems, num_mems,
           SOC_CONTROL(unit)->bistTimeout)) < 0) {
  cli_out("BIST failed: %s\n", soc_errmsg(rv));
        sal_free(mems);
  return CMD_FAIL;
    }
#endif

    sal_free(mems);
    return CMD_OK;
}


