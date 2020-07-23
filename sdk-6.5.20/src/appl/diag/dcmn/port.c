/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     PORT CLI commands
 */



#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/util.h>
#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>


#if defined PORTMOD_SUPPORT
#include <appl/diag/phymod/phymod_sdk_diag.h>
#include <appl/diag/phymod/phymod_symop.h>
#include <soc/portmod/portmod.h>
#ifndef PHYMOD_EXCLUDE_CHIPLESS_TYPES
#define PHYMOD_EXCLUDE_CHIPLESS_TYPES
#include <phymod/chip/bcmi_tsce_xgxs_defs.h>
#include <phymod/chip/bcmi_tscf_xgxs_defs.h>
#undef PHYMOD_EXCLUDE_CHIPLESS_TYPES
#endif /* PHYMOD_EXCLUDE_CHIPLESS_TYPES */
#endif

#if defined(PORTMOD_SUPPORT)
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_chain.h>
#endif


#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/port.h>

#include <soc/phy.h>
#include <soc/eyescan.h>
#include <soc/phyreg.h>
#include <soc/dcmn/dcmn_port.h>
#include <../src/soc/phy/wcmod/include/wcmod.h>
#include <../src/soc/phy/wcmod/include/wcmod_extra.h>

#include <soc/dcmn/dcmn_defs.h>
#include <bcm_int/common/link.h>

#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#include <bcm_int/dfe/port.h>
#include <soc/dfe/cmn/mbcm.h>
#endif

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/ARAD/arad_defs.h>
#include <soc/dpp/drv.h>
#include <bcm_int/dpp/port.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/port_sw_db.h>
#endif
#ifdef BCM_JERICHO_SUPPORT
#include <soc/dpp/JER/jer_nif.h>
#endif


extern int soc_phy_set_verbose(int unit, int port, int verbose);
extern cmd_result_t port_phy_control_update(int u, bcm_port_t p, bcm_port_phy_control_t type,
                        uint32 val, uint32 flags, int *print_header);
#if defined PHYMOD_SUPPORT
extern int phymod_sym_access(int u, args_t *a, int intermediate, soc_pbmp_t *pbm); 
#endif

#define DUMP_PHY_COLS   4

#if defined PORTMOD_SUPPORT
#define MAX_CORE_ACCESS_STRUCTURES_PER_PORT (6)
#endif

/*local functions*/
STATIC cmd_result_t
_phy_prbs_dcmn(int unit, args_t *args);

STATIC cmd_result_t
_phy_power(int unit, args_t *args);
STATIC cmd_result_t
__phy_diag_phy_unit_get(int unit_value, int *phy_dev);

STATIC cmd_result_t
__phy_diag_phy_if_get(char *if_str, int *dev_if);

STATIC cmd_result_t
_phy_diag_dcmn(int unit, args_t *args);
STATIC cmd_result_t
_phy_diag_prbs_dcmn(int unit, bcm_pbmp_t pbmp,args_t *args);

STATIC cmd_result_t 
_phy_diag_loopback_dcmn(int unit, bcm_pbmp_t pbmp,args_t *args);

STATIC cmd_result_t
_phy_control(int unit, args_t *args);
STATIC cmd_result_t
_phy_dumpall(int unit, args_t *args);

STATIC cmd_result_t
_phy_measure(int unit, args_t *args);

STATIC cmd_result_t
_phy_cl72(int unit, args_t *args);

STATIC cmd_result_t
_phy_phyctrl_reg_write(int unit, soc_pbmp_t pbm, uint32 phy_reg, uint16 reg_data, int intermediate);

STATIC cmd_result_t
_phy_phyctrl_reg_read(int unit, soc_pbmp_t pbm, uint32 phy_reg, int intermediate);

STATIC cmd_result_t
_phy_phyctrl_regs_dump(int unit, soc_pbmp_t pbm, int intermediate);

#ifdef BCM_PETRA_SUPPORT
STATIC uint32 get_field_dcmn(uint32 in, int first, int last) {
   /*DESCRIPTION# Declaration    : uint32 get_field_dcmn(uint32 in, int first, int last);
     DESCRIPTION# Input          : in - input value; first - LSB bit of in; last - MSB field of in;
     DESCRIPTION# NOTE           : The field MUST be no longer than 32 bit.
     DESCRIPTION# Output         : NONE.
     DESCRIPTION# Return         : field value (uint32).*/
   if (last != 31) in &= (0x7FFFFFFF >> (30 - last));
   in = in >> first;
   in &= ~(0xFFFFFFFF << (last - first + 1));
   return (in);
}
#endif

char if_dpp_port_stat_usage[] =
    "Display info about port status in table format.\n"
    "    Link scan modes:\n"
    "        SW = software\n"
    "        HW = hardware\n"
    "    Learn operations (source lookup failure control):\n"
    "        F = SLF packets are forwarded\n"
    "        C = SLF packets are sent to the CPU\n"
    "        A = SLF packets are learned in L2 table\n"
    "        D = SLF packets are discarded.\n"
    "    Pause:\n"
    "        TX = Switch will transmit pause packets\n"
    "        RX = Switch will obey pause packets\n";

/*
 * Function:
 *     if_dpp_port_stat
 * Purpose:
 *     Table display of port status information.
 * Parameters:
 *     unit - Device number
 *     args - Pointer to args
 * Returns:
 *     CMD_OK   - Success
 *     CMD_FAIL - Failure
 */
cmd_result_t
if_dpp_port_stat(int unit, args_t *a)
{
    pbmp_t              pbm, tmp_pbm;
    bcm_port_info_t    *info_all;
    bcm_port_config_t   pcfg;
    soc_port_t          p;
    int                 r;
    char               *c;
    uint32              mask;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL) {
        BCM_PBMP_ASSIGN(pbm, pcfg.port);
    } else if (parse_bcm_pbmp(unit, c, &pbm) < 0) {
        cli_out("%s: Error: unrecognized port bitmap: %s\n",
                ARG_CMD(a), c);
        return CMD_FAIL;
    }
    if (!SOC_IS_ARDON(unit)) {
        BCM_PBMP_AND(pbm, pcfg.port); 
    } else {
        BCM_PBMP_AND(pbm, pcfg.sfi); 
    }
    if (BCM_PBMP_IS_NULL(pbm)) {
        cli_out("No ports specified.\n");
        return CMD_OK;
    }
    
    mask = BCM_PORT_ATTR_ALL_MASK;
    if (soc_property_get(unit, spn_BCM886XX_VLAN_TRANSLATE_MODE, 0)) {
        mask &= ~BCM_PORT_ATTR_DISCARD_MASK;
    }

    info_all = sal_alloc(SOC_MAX_NUM_PORTS * sizeof(bcm_port_info_t), 
                         "if_port_stat");
    if (info_all == NULL) {
        cli_out("Insufficient memory.\n");
        return CMD_FAIL;
    }

    sal_memset(info_all, 0, SOC_MAX_NUM_PORTS * sizeof(bcm_port_info_t));

    
    PBMP_ITER(pbm, p) {
        port_info_init(unit, p, &info_all[p], mask);
        if ((r = bcm_port_selective_get(unit, p, &info_all[p])) < 0) {
            cli_out("%s: Error: Could not get port %s information: %s\n",
                    ARG_CMD(a), BCM_PORT_NAME(unit, p), bcm_errmsg(r));
            sal_free(info_all);
            return (CMD_FAIL);
        }
    }

    brief_port_info_header(unit);

#define _call_bpi(pbm, pbm_mask) \
    tmp_pbm = pbm_mask; \
    BCM_PBMP_AND(tmp_pbm, pbm); \
    PBMP_ITER(tmp_pbm, p) { \
        brief_port_info(unit, p, &info_all[p], mask); \
    }

    if (soc_property_get(unit, spn_DPORT_MAP_ENABLE, TRUE)) {
        /* If port mapping is enabled, then use port order */
        _call_bpi(pbm, pcfg.port);
    } else {
        /* If no port mapping, ensure that ports are grouped by type */
        _call_bpi(pbm, pcfg.fe);
        _call_bpi(pbm, pcfg.ge);
        _call_bpi(pbm, pcfg.xe);
        _call_bpi(pbm, pcfg.hg);
    }

    sal_free(info_all);

    return CMD_OK;
}


char if_dpp_phy_usage[] = 
    "Subcommands:\n"
    "               int <ports>\n"
    "               info\n"
    "               list [pattern] print register matching the specified pattern/address\n"
    "               nz [pattern] print registers matching the pattern that the value is not zero\n"
    "               verbose Port=<ports> Verbose=<value>\n"
    "               power <ports> mode=<on|off>\n"
    "               prbs <ports> <set|get|clear> Mode=<phy|mac> [Polynomial=<value>]\n"
    "                       [Loopback=<true|false>] [Invert=<value>] [interval=<value>]\n"
    "               diag <ports> <eyescan|dsc|loopback|prbs|berproj> <parameters>\n"
    "               measure <ports> [<tx|rx>]\n"
    "               cl72 <ports> <on|off>\n"
    "               dumpall <c22|c45>\n"
    "               control <ports> [phy_control_type=value]\n"
    "               Direct Access (without symbol table):\n"
    "                      phy <port> direct [reg address] [value]\n"
    "NOTES: <ports> is a standard port bitmap,\n"
    "       e.g. fe for all 10/100 ports, fe5-fe7 for three FE's, etc. (see \"help pbmp\").\n"
    "       Add 'external' to the end of the cmd for external phy diagnostic (relevant for 'dsc').\n"
    ;

#ifdef PORTMOD_SUPPORT
STATIC
int 
_phy_direct_access(int u, args_t *a, int intermediate, soc_pbmp_t *pbm)
{
    int rv, p, dport;
    int nof_phys_structs;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access[MAX_CORE_ACCESS_STRUCTURES_PER_PORT];
    uint8 is_write_op  = ARG_CNT(a) > 1 ;
    uint32 reg_addr;
    uint32 reg_val ;
    int i;
    char *c;

    if (ARG_CNT(a) == 0) {
        cli_out("please supply address\n"); 
        return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    reg_addr = soc_parse_integer(c);
    if (is_write_op) {
        c = ARG_GET(a);
        reg_val = soc_parse_integer(c);
    }


    rv = portmod_access_get_params_t_init(u, &params);
    if(SOC_FAILURE(rv)) {
        cli_out("portmod_access_get_params_t_init failed\n"); 
        return CMD_FAIL;
    }

    if(intermediate){
        params.phyn = 0;
    }
    DPORT_SOC_PBMP_ITER(u, *pbm, dport, p) {
        rv = portmod_port_phy_lane_access_get(u, p, &params, MAX_CORE_ACCESS_STRUCTURES_PER_PORT, phy_access, &nof_phys_structs, NULL);
        if (rv != CMD_OK) {
            return rv;
        }
        for(i = 0 ; i < nof_phys_structs ; i++){
            if (is_write_op) {
                rv = phymod_phy_reg_write(&(phy_access[i]), reg_addr, reg_val); 
            } else {
                rv = phymod_phy_reg_read(&(phy_access[i]), reg_addr, &reg_val);
                if (rv== CMD_OK) {
                    cli_out("Port %s%s(0x%02x):\n",
                      SOC_PORT_NAME(u, p),
                      intermediate ? " (int)" : "",
                      phy_access[i].access.addr);
                    cli_out("DIRECT [0x%x] = 0x%x\n",reg_addr,reg_val);

                }
            }
            if (rv!= CMD_OK) {
                cli_out("Failure access DIRECT [0x%x]\n",reg_addr);
                return rv;
            }
        }
    }

    return CMD_OK;

}

int
_phy_portmod_sym_access(int u, args_t *a, int intermediate, soc_pbmp_t *pbm)
{
    phymod_symbols_iter_t iter;
    phymod_symbols_t *symbols_table = NULL; 
    int rv, p, dport;
    int i = 0;
    char hdr[32];
    phymod_phy_access_t phy_access[MAX_CORE_ACCESS_STRUCTURES_PER_PORT];
    int nof_phys_structs;
    portmod_access_get_params_t params;
    
    rv = portmod_access_get_params_t_init(u, &params);
    if(SOC_FAILURE(rv)) {
        cli_out("portmod_access_get_params_t_init failed\n"); 
        return CMD_FAIL;
    }

    if(intermediate){
        params.phyn = 0;
    }
    rv = phymod_symop_init(&iter, a);
    if (rv != CMD_OK) {
        return rv;
    }

    DPORT_SOC_PBMP_ITER(u, *pbm, dport, p) {
        rv = portmod_port_phy_lane_access_get(u, p, &params, MAX_CORE_ACCESS_STRUCTURES_PER_PORT, phy_access, &nof_phys_structs, NULL);
        if (rv != CMD_OK) {
            phymod_symop_cleanup(&iter);
            return rv;
        }
        for(i = 0 ; i < nof_phys_structs ; i++){
            rv = sal_snprintf(hdr, sizeof(hdr), "Port %s%s(0x%02x):\n",
                  SOC_PORT_NAME(u, p),
                  intermediate ? " (int)" : "",
                  phy_access[i].access.addr);
            if (rv >= sizeof(hdr)) {
                continue;
            }
            rv = phymod_diag_symbols_table_get(&phy_access[i], &symbols_table);
            if (rv != CMD_OK) {
                phymod_symop_cleanup(&iter);
                return rv;
            }
            rv = phymod_symop_exec(&iter, symbols_table,
                                   &phy_access[i], hdr);
            if (rv != CMD_OK) {
                phymod_symop_cleanup(&iter);
                return rv;
            }
        }
    }

    return phymod_symop_cleanup(&iter);
}
#endif /* PORTMOD_SUPPORT */

#define MAX_PORT_CORES 7

/*
 * Function:     if_phy
 * Purpose:    Show/configure phy registers.
 * Parameters:    u - SOC unit #
 *        a - pointer to args
 * Returns:    CMD_OK/CMD_FAIL/
 */
cmd_result_t
if_dpp_phy(int unit, args_t *a)
{

    char *c;
    soc_port_t p, dport;
    soc_pbmp_t pbm, pbm_temp, pbm_phys;
    uint16 phy_data;
    uint32 phy_reg;
    int intermediate = 0;
    char pfmt[SOC_PBMP_FMT_LEN];
    uint16 phy_devad = 0;
    uint16 phy_addr;
    int ret = CMD_OK;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }
#ifdef PORTMOD_SUPPORT
    /*register print function*/
    phymod_diag_print_func = cli_out;
#endif

    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    /*******************************/
    /******    Verbose support    ********/
    /*******************************/

    if (sal_strcasecmp(c, "verbose") == 0) {
        parse_table_t    pt;
        int verbose = 10;

        if (SOC_USE_PORTCTRL(unit)) {
            cli_out("The unit does not support phy verbose\n");
            return CMD_FAIL;
        }

        p=0;

        if ((c = ARG_CUR(a)) != NULL) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Port", PQ_DFL | PQ_INT,
                            0, &p, 0);
            parse_table_add(&pt, "Verbose", PQ_DFL | PQ_INT,
                0, &verbose, 0);

            if (parse_arg_eq(a, &pt) < 0) {
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }

            if (ARG_CNT(a) > 0) {
                cli_out("%s: Error: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
            parse_arg_eq_done(&pt);
        }
        else {
            cli_out("        verbose: set PHY verbosity level.\n"
                    "        Example: phy verbose Port=1 Verbose=10\n");
            return CMD_FAIL;
        }

#if defined(BCM_88750_SUPPORT) || defined(BCM_PETRA_SUPPORT)
        if(SOC_E_NONE == soc_phy_set_verbose(unit, p, verbose))
            return CMD_OK;
#else
            return CMD_FAIL;
#endif
    }

    /*******************************/
    /******    Info support    ********/
    /*******************************/

    if (sal_strcasecmp(c, "info") == 0) {
        SOC_PBMP_ASSIGN(pbm, PBMP_PORT_ALL(unit));
        SOC_PBMP_REMOVE(pbm, PBMP_HG_SUBPORT_ALL(unit));
        SOC_PBMP_REMOVE(pbm, PBMP_REQ_ALL(unit));

        cli_out("Phy mapping dump:\n");
        cli_out("%11s %5s %5s %5s %5s %23s %10s\n",
                   "port", "id0", "id1", "addr", "iaddr", "name", "timeout");

        DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {

#ifdef PORTMOD_SUPPORT
            if (SOC_USE_PORTCTRL(unit)) {
                phymod_core_access_t internal_core[MAX_PORT_CORES];
                phymod_core_access_t core_acc[MAX_PORT_CORES];
                phymod_core_info_t   core_info;
                int nof_cores = 0 , an_timeout = -1;
                int phy = 0, is_legacy =0;
                int range_start = 0;
                int is_first_range;
                int core_num = 0;
                int port_base = 1;
                #ifdef BCM_JERICHO_SUPPORT
                uint32 first_phy_port = 0,phy_lane = 0;
                #endif
                portmod_port_diag_info_t diag_info;
                int rv, i;
                uint8   pcount=0;
                char    lnstr[32], *pname, namelen; 

                
                    for (i=0 ; i<MAX_PORT_CORES ; i++) {
                        rv = phymod_core_access_t_init(&core_acc[i]);
                        if (BCM_FAILURE(rv)) {
                            cli_out("Failed to init core access \n");
                            return CMD_FAIL;
                        }
                        rv = phymod_core_access_t_init(&internal_core[i]);
                        if (BCM_FAILURE(rv)) {
                            cli_out("Failed to init internal core access \n");
                            return CMD_FAIL;
                        }
                    }

                    rv = phymod_core_info_t_init(&core_info);
                    if (BCM_FAILURE(rv)) {
                        cli_out("Failed to init core info\n");
                        return CMD_FAIL;
                    }
                    
                    sal_memset(&diag_info, 0, sizeof(portmod_port_diag_info_t));
                    if (!SAL_BOOT_SIMULATION) {

                        rv = portmod_port_core_access_get(unit, p, -1/*most external phy*/, MAX_PORT_CORES, core_acc, &nof_cores, NULL);
                        if (BCM_FAILURE(rv)) {
                            cli_out("Failed to get core access\n");
                            return CMD_FAIL;
                        }
                        if(nof_cores == 0) {
                            continue;
                        }

                       /* check if the external phy is a legacy phy */
                        rv = portmod_port_is_legacy_ext_phy_present(unit, p, &is_legacy);
                        if (BCM_FAILURE(rv)) {
                            cli_out("Failed to get if port has legacy phy\n");
                            return CMD_FAIL;
                        }

                        rv = portmod_port_core_access_get(unit, p, 0, MAX_PORT_CORES, internal_core, &nof_cores, NULL);
                        if (BCM_FAILURE(rv)) {
                            cli_out("Failed to get internal core access\n");
                            return CMD_FAIL;
                        }
                        if(nof_cores == 0) {
                            continue;
                        }

                        rv = portmod_port_diag_info_get(unit, p, &diag_info);
                        if (BCM_FAILURE(rv)) {
                            cli_out("Failed to get port info\n");
                            return CMD_FAIL;
                        }

                        #ifdef BCM_JERICHO_SUPPORT
                        if(!(IS_SFI_PORT(unit, p) || IS_IL_PORT(unit, p))){
                            rv = soc_port_sw_db_first_phy_port_get(unit, p, &first_phy_port);
                            if (BCM_FAILURE(rv)) {
                                cli_out("Failed to get first phy port info\n");
                            }
                            rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, first_phy_port, &phy_lane));
                            if (BCM_FAILURE(rv)) {
                                cli_out("Failed to get  phy lane info\n");
                            }
                            core_num = (phy_lane - 1) / NUM_OF_LANES_IN_PM;
                        }
                        #endif

                        is_first_range = TRUE;
                        PORTMOD_PBMP_ITER(diag_info.phys, phy){
                            if( is_first_range ){
                                range_start = phy ;
                                is_first_range = FALSE;
                            }
                        }
                        an_timeout = soc_property_port_get(unit, p,
                                          spn_PHY_AUTONEG_TIMEOUT, 250000);
                        if ( !is_legacy ) {
                            rv = phymod_core_info_get(core_acc, &core_info);
                            if (BCM_FAILURE(rv)) {
                                cli_out("Failed to get core info\n");
                                return CMD_FAIL;
                            }
                        }
                    }
                     
                    PORTMOD_PBMP_COUNT(diag_info.phys, pcount);

                    pname = phymod_core_version_t_mapping[core_info.core_version].key;
                    namelen = strlen(pname);

                    sal_snprintf(lnstr, sizeof(lnstr), "%s", pname); 
                    sal_snprintf(lnstr+namelen-2, sizeof(lnstr)-(namelen-2), "-%s/%02d/", pname+namelen-2, core_num); 

                    pname = lnstr;
                    while (*pname != '-') {
                        *pname = sal_toupper(*pname); 
                         pname++;
                    }

                    pname = lnstr+strlen(lnstr); 
                    if (pcount == 4) {
                       sal_snprintf(pname,sizeof(lnstr), "%d", pcount);
                    } else if (pcount == 2) {
                       sal_snprintf(pname,sizeof(lnstr), "%d-%d", (range_start-1)%4, ((range_start-1)%4)+1);
                    } else {
                        /* Make sure that SFI ports will be pritned as 0-based and NIF-ports as 1-based*/
                       if(IS_SFI_PORT(unit, diag_info.original_port)) {
                           port_base = 0;
                       } else {
                           port_base = 1;
                       }
                       sal_snprintf(pname,sizeof(lnstr), "%d", (range_start-port_base)%4);
                    }

                    if ( !is_legacy ) {
                        cli_out("%5s(%3d) %5x %5x %5x %5x %23s %10d \n",
                                SOC_PORT_NAME(unit, p), p,
                                core_info.phy_id0,
                                core_info.phy_id1,
                                core_acc[0].access.addr,
                                internal_core[0].access.addr,
                                lnstr, an_timeout);
                    } else {
                        cli_out("%5s(%3d) %5x %5x %5x %5x %23s %10d \n",
                                SOC_PORT_NAME(unit, p), p,
                                soc_phy_id0reg_get(unit, p),
                                soc_phy_id1reg_get(unit, p),
                                soc_phy_addr_of_port(unit, p),
                                soc_phy_addr_int_of_port(unit, p),
                                soc_phy_name_get(unit, p),
                                soc_phy_an_timeout_get(unit, p));
                    }
                
           } else 
#endif
           {
                cli_out("%6s(%3d) %5x %5x %5x %5x %23s %10d\n",
                        SOC_PORT_NAME(unit, p), p,
                        soc_phy_id0reg_get(unit, p),
                        soc_phy_id1reg_get(unit, p),
                        soc_phy_addr_of_port(unit, p),
                        soc_phy_addr_int_of_port(unit, p),
                        soc_phy_name_get(unit, p),
                        soc_phy_an_timeout_get(unit, p));
           }
        }

        return CMD_OK;
    }

    /*******************************/
    /******    power support    ********/
    /*******************************/

    if (sal_strcasecmp(c, "power") == 0) {
        return _phy_power(unit, a);
    }

    /*******************************/
    /******    prbs support ********/
    /*******************************/

    if (sal_strcasecmp(c, "prbs") == 0) {
        return _phy_prbs_dcmn(unit, a);
    }

    if (c != NULL && sal_strcasecmp(c, "control") == 0) {
        return _phy_control(unit, a);
    }

    if (c != NULL && sal_strcasecmp(c, "dumpall") == 0) {
        return _phy_dumpall(unit, a);
    }
    /*******************************/
    /******measure  support*********/
    /*******************************/

    if (sal_strcasecmp(c, "measure") == 0) {
        return _phy_measure(unit, a);
    }

    if (sal_strcasecmp(c, "cl72") == 0) {
        return _phy_cl72(unit, a);
    }

    /*******************************/
    /******    diag support    ********/
    /*******************************/

    if (sal_strcasecmp(c, "diag") == 0) {
        return _phy_diag_dcmn(unit, a);
    }

    /*******************************/
    /******    raw support        ********/
    /*******************************/

    /* Raw access to an MII register */
    if (c != NULL && sal_strcasecmp(c, "raw") == 0) {
        int caluse = 22;
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
        if (sal_strcasecmp(c, "c45") == 0) {
            caluse = 45;
            if ((c = ARG_GET(a)) == NULL) {
                return CMD_USAGE;
            }
        }
        phy_addr = strtoul(c, NULL, 0);
        if ((c = ARG_GET(a)) == NULL) { /* Get register number */
            cli_out("%s: ERROR: Invalid register number\n", ARG_CMD(a));
            return CMD_FAIL;
        }

        /*when caluse is 45, get phy_devad*/
        if (45 == caluse) {
            phy_devad = strtoul(c, NULL, 0);
            if ((c = ARG_GET(a)) == NULL) {
                return CMD_USAGE;
            }
        }

        phy_reg = strtoul(c, NULL, 0);

        if ((c = ARG_GET(a)) == NULL) { /* Read register */
                if (22 == caluse) {
                ret = soc_dcmn_miim_read(unit, caluse, phy_addr, phy_reg, &phy_data);
             }
             else {
                  ret = soc_dcmn_miim_read(unit, caluse, phy_addr, SOC_PHY_CLAUSE45_ADDR(phy_devad, phy_reg), &phy_data);
            }

            if (ret < 0) {
                cli_out("ERROR: MII Addr %d: soc_miim_read failed: %s\n",
                       phy_addr, soc_errmsg(ret));
                return CMD_FAIL;
            }
            cli_out("%s\t0x%02x: 0x%04x\n",
                   "", phy_reg, phy_data);
        } else { /* write */
            phy_data = strtoul(c, NULL, 0);
            if (22 == caluse) {
                ret = soc_dcmn_miim_write(unit, caluse, phy_addr, phy_reg, phy_data);
             }
             else{
                ret = soc_dcmn_miim_write(unit, caluse ,phy_addr, SOC_PHY_CLAUSE45_ADDR(phy_devad, phy_reg), phy_data);
            }
            if (ret < 0) {
                cli_out("ERROR: MII Addr %d: soc_miim_write failed: %s\n",
                       phy_addr, soc_errmsg(ret));
                return CMD_FAIL;
            }
        }

        return CMD_OK;
    }

    /*******************************/
    /******    int support        ********/
    /*******************************/

    if (sal_strcasecmp(c, "int") == 0) {
        intermediate = 1;
        c = ARG_GET(a);
        if (c == NULL) {
            cli_out("        If the int option is given, the intermediate PHY for the port is\n"
                    "        used instead of the outer PHY.\n"
                    "        Example: phy int 6\n");
            return CMD_FAIL;
        }
    }

    /*******************************/
    /******read/write support********/
    /*******************************/

    /* Parse the bitmap. */
    if (parse_pbmp(unit, c, &pbm) < 0) {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
        return CMD_FAIL;
    }

    SOC_PBMP_ASSIGN(pbm_phys, pbm);
    SOC_PBMP_AND(pbm_phys, PBMP_PORT_ALL(unit));
    if (SOC_PBMP_IS_NULL(pbm_phys)) {
        cli_out("Ports specified do not have PHY drivers.\n");
    } else {
        SOC_PBMP_ASSIGN(pbm_temp, pbm);
        SOC_PBMP_REMOVE(pbm_temp, PBMP_PORT_ALL(unit));
        if (SOC_PBMP_NOT_NULL(pbm_temp)) {
            cli_out("Not all ports given have PHY drivers.  Using %s\n",
                    SOC_PBMP_FMT(pbm_phys, pfmt));
        }
    }
    SOC_PBMP_ASSIGN(pbm, pbm_phys);

    if (ARG_CNT(a) == 0) {    /*  show information for all registers */
        if (SOC_USE_PORTCTRL(unit) || SOC_IS_ARDON(unit)) {
            cli_out("for dump use *\n");
            return CMD_FAIL;
        }
       return _phy_phyctrl_regs_dump(unit, pbm , intermediate);
    } else {                /* get register argument */
#if defined(PORTMOD_SUPPORT)
        if (SOC_USE_PORTCTRL(unit)) {
            c = ARG_GET(a);
            if (sal_strcasecmp(c, "direct") == 0 ) {
                return _phy_direct_access(unit, a, intermediate, &pbm);
            }
            ARG_PREV(a);
            return _phy_portmod_sym_access(unit, a, intermediate, &pbm);
        }
#endif /* defined(PORTMOD_SUPPORT) */
#if defined(PHYMOD_SUPPORT)
        if (SOC_IS_ARDON(unit)) {
            phymod_sym_access(unit, a, intermediate, &pbm);
        }
#endif /* defined(PHYMOD_SUPPORT) */
        c = ARG_GET(a);
        phy_reg = sal_ctoi(c, 0);
        if (ARG_CNT(a) == 0) {        /* no more args; show this register */
            return _phy_phyctrl_reg_read(unit, pbm , phy_reg, intermediate);
        } else {    /* set the reg to given value for the indicated phys */
            c = ARG_GET(a);
            phy_data = sal_ctoi(c, 0);
            return _phy_phyctrl_reg_write(unit, pbm, phy_reg, phy_data, intermediate);
        }
    }
    return CMD_OK;
}


STATIC
cmd_result_t
_phy_power(int unit, args_t *args)
{

    parse_table_t    pt;
    char *c = NULL;
    char *modes[] = {"off", "on", NULL};
    int mode_value = -1;
    bcm_port_t       dport, p;
    bcm_pbmp_t pbm;
    
    if ((c = ARG_GET(args)) == NULL) {
        cli_out("        power: set power mode for the PHY devices implemented power control.\n"
                "        Example: phy power 1 mode=on\n");
            return CMD_FAIL;
    }

    if ((parse_bcm_pbmp(unit, c, &pbm) < 0)) {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(args), c);
        return CMD_FAIL;
    }


    parse_table_init(unit, &pt);
    parse_table_add(&pt, "mode", PQ_MULTI|PQ_DFL , (void *)(0), &mode_value, &modes);

   if (parse_arg_eq(args, &pt) < 0) {
       parse_arg_eq_done(&pt);
       return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

    if (ARG_CNT(args) > 0) {
        cli_out("%s: ERROR: Unknown argument %s\n", ARG_CMD(args), ARG_CUR(args));
        return CMD_FAIL;
    }

    if(mode_value == -1){
        cli_out("Missing mode parameter \n");
        return CMD_FAIL;
    }
    
    DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
        BCM_IF_ERROR_RETURN(bcm_port_enable_set(unit, p, mode_value));
    }
    return CMD_OK;
}


STATIC cmd_result_t
_phy_prbs_dcmn(int unit, args_t *args)
{
    parse_table_t       pt;
    bcm_port_t          port, dport;
    bcm_pbmp_t          pbmp;
    int                 rv, cmd, enable, mode = 0, mode_get = 0, lb_mode = BCM_PORT_LOOPBACK_PHY, interval=1;
    char               *cmd_str, *port_str, *mode_str;
    char *prbs_polys[] = {"X7_X6_1", "X15_X14_1", "X23_X18_1", "X31_X28_1", "X9_X5_1", "X11_X9_1", "X58_X31_1",
                         "0", "1", "2", "3", "4", "5", "6"};
    int                 poly = 0, lb = 0, invert=0;

    enum { _PHY_PRBS_SET_CMD, _PHY_PRBS_GET_CMD, _PHY_PRBS_CLEAR_CMD };
    enum { _PHY_PRBS_SI_MODE, _PHY_PRBS_HC_MODE };

    if ((port_str = ARG_GET(args)) == NULL) {
        cli_out("        prbs: set, get, or clear internal PRBS generator/checker.\n"
                "        Example: phy prbs 6 set mode=phy loopback=true\n"
                "                 phy prbs 6 get interval=5\n");
        return CMD_FAIL;
    }

    BCM_PBMP_CLEAR(pbmp);
    if (parse_bcm_pbmp(unit, port_str, &pbmp) < 0) {
        cli_out("ERROR: unrecognized port bitmap: %s\n", port_str);
        return CMD_FAIL;
    }

    if ((cmd_str = ARG_GET(args)) == NULL) {
        cli_out("ERROR: please insert a valid command: set|get|clear\n");
        return CMD_FAIL;
    }
    if (sal_strcasecmp(cmd_str, "set") == 0) {
        cmd = _PHY_PRBS_SET_CMD;
        enable = 1;
    } else if (sal_strcasecmp(cmd_str, "get") == 0) {
        cmd = _PHY_PRBS_GET_CMD;
        enable = 0;
    } else if (sal_strcasecmp(cmd_str, "clear") == 0) {
        cmd = _PHY_PRBS_CLEAR_CMD;
        enable = 0;
    } else{
        cli_out("ERROR: please insert a valid command: set|get|clear\n");
        return CMD_FAIL;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Mode", PQ_STRING, 0, &mode_str, NULL);
    if (cmd == _PHY_PRBS_SET_CMD) {
        parse_table_add(&pt, "Polynomial", PQ_DFL|PQ_MULTI,
                        (void *)(0), &poly, &prbs_polys);
        parse_table_add(&pt, "LoopBack", PQ_DFL|PQ_BOOL,
                        (void *)(0), &lb, NULL);
        parse_table_add(&pt, "Invert", PQ_DFL|PQ_BOOL,
                (void *)(0), &invert, NULL);
    } else if (cmd == _PHY_PRBS_GET_CMD) {
        parse_table_add(&pt, "interval", PQ_DFL|PQ_INT,
                (void *)(0), &interval, NULL);
    }
    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("ERROR: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
   
    if (mode_str) {
        if ((sal_strcasecmp(mode_str, "si") == 0) || (sal_strcasecmp(mode_str, "mac") == 0)) {
            mode = 1;
            lb_mode = BCM_PORT_LOOPBACK_MAC;
        } else if ((sal_strcasecmp(mode_str, "hc") == 0) || (sal_strcasecmp(mode_str, "phy") == 0)) {
            mode = 0;
            lb_mode = BCM_PORT_LOOPBACK_PHY;
        }
    }
    
    /* Now free allocated strings */
    parse_arg_eq_done(&pt);
        

    
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {

        int status;
        /* First set prbs mode */
        if(IS_SFI_PORT(unit, port) && (enable == 1)) {
            rv = bcm_port_control_set(unit, port, bcmPortControlPrbsMode,
                                      mode);
            if (rv != BCM_E_NONE) {
                cli_out("ERROR: Setting prbs mode failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }

        /**
         * If clear command is passed without mode parameter (as usual)
         * mode continues with its default value (0 - PHY PRBS mode)
         * However later the code rely on mode to be synced up with what was set
         */
        if (IS_SFI_PORT(unit, port) && cmd == _PHY_PRBS_CLEAR_CMD) {
            rv = bcm_port_control_get(unit, port, bcmPortControlPrbsMode,
                                      &mode_get);
            if (rv != BCM_E_NONE) {
                cli_out("ERROR: Getting prbs mode failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
            if (mode != mode_get) {
                cli_out("WARNING: CLEARing %s PRBS but was set %s PRBS.\n", mode ? "MAC": "PHY", mode_get ? "MAC": "PHY");
                cli_out("To avoid such cases use the 'mode' parameter \n");
            }

            mode = mode_get;
        }

        if (cmd == _PHY_PRBS_SET_CMD || cmd == _PHY_PRBS_CLEAR_CMD) {
            poly = poly % 7;                            
            /* Set polynomial*/
            if (mode && (SOC_IS_ARAD(unit) || (SOC_IS_DFE(unit)))){
                /*MAC PRBS polynomial set in arad and DFE is not supported*/
            } else {
                rv = bcm_port_control_set(unit, port,
                                          bcmPortControlPrbsPolynomial,
                                          poly);
                if (rv != BCM_E_NONE) {
                    cli_out("ERROR: Setting prbs polynomial failed: %s\n",
                            bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            }

            /*lb == 1 ==>internal prbs*/
            /*lb == 0 ==>external prbs*/
            if (lb) {
                rv = bcm_port_loopback_set(unit, port, lb_mode);

                if (rv != BCM_E_NONE) {                                     
                    cli_out("ERROR: loopback set failed: %s\n",           
                            bcm_errmsg(rv));                                 
                    return CMD_FAIL;                                        
                }    
            }

            /*Only for MAC PRBS when disabeling*/
            if (mode && cmd == _PHY_PRBS_CLEAR_CMD) {
                rv = bcm_port_control_set(unit, port,
                                          bcmPortControlPrbsTxEnable,
                                          enable);
            } else {
                rv = bcm_port_control_set(unit, port,
                                          bcmPortControlPrbsRxEnable,
                                          enable);
            }                            
            if (rv != BCM_E_NONE) {                                     
                cli_out("ERROR: Setting prbs enable failed: %s\n",           
                        bcm_errmsg(rv));                                 
                return CMD_FAIL;                                        
            }    

            /*invert*/
            if (!mode /*not mac prbs*/) {
                rv = bcm_port_control_set(unit, port,                       
                                      bcmPortControlPrbsTxInvertData,       
                                      invert);                            
                if (rv != BCM_E_NONE) {                                     
                    cli_out("ERROR: Invert prbs tx failed: %s\n",           
                            bcm_errmsg(rv));                                 
                    return CMD_FAIL;                                        
                }   
            }

            /*Only for MAC PRBS when disabeling*/
            if (mode && cmd == _PHY_PRBS_CLEAR_CMD) {
                rv = bcm_port_control_set(unit, port,
                                          bcmPortControlPrbsRxEnable,
                                          enable);
            } else {
                rv = bcm_port_control_set(unit, port,
                                          bcmPortControlPrbsTxEnable,
                                          enable);
            }                           
            if (rv != BCM_E_NONE) {                                     
                cli_out("ERROR: Setting prbs enable failed: %s\n",           
                        bcm_errmsg(rv));                                 
                return CMD_FAIL;                                        
            }                                                 
                                                                       
            if (cmd == _PHY_PRBS_SET_CMD ) {
                /* clear errors */
                rv = bcm_port_control_get(unit, port,
                                          bcmPortControlPrbsRxStatus,
                                          &status);
                if (rv != BCM_E_NONE) {
                    cli_out("ERROR: Getting prbs rx status failed: %s\n",
                            bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            }
        }
        
                
    }
    if (cmd == _PHY_PRBS_GET_CMD) {
        sal_sleep(interval);
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        
            int status;                                                 
            /*check prbs results*/
            rv = bcm_port_control_get(unit, port,                       
                              bcmPortControlPrbsRxStatus,       
                              &status);                         
            if (rv != BCM_E_NONE) {                                     
                cli_out("ERROR: Getting prbs rx status failed: %s\n",           
                        bcm_errmsg(rv));     
                return CMD_FAIL;                                        
            }

            switch (status) {
            case 0:
                cli_out("%s (%2d):  PRBS PASSED(LOCKED and returns no error)!\n", BCM_PORT_NAME(unit, port), port);
                break;
            case -1:
                cli_out("%s (%2d):  PRBS FAILED (not LOCKED)!\n", BCM_PORT_NAME(unit, port), port);
                break;
            case -2:
                cli_out("%s (%2d):  PRBS FAILED(currently LOCKED but was not LOCKED since last read)!\n", BCM_PORT_NAME(unit, port), port);
                break;
            default:
                cli_out("%s (%2d):  PRBS FAILED with %d errors!\n", BCM_PORT_NAME(unit, port), 
                        port, status);
                break;
            }

        }
    }
    return CMD_OK;
}


STATIC cmd_result_t
_phy_phyctrl_reg_write(int unit, soc_pbmp_t pbm, uint32 phy_reg, uint16 phy_data, int intermediate)
{
    soc_port_t p, dport;
    uint16 phy_addr;
    phy_ctrl_t* pc;
    cmd_result_t rv = CMD_OK;

    DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
        phy_addr = (intermediate ? PORT_TO_PHY_ADDR_INT(unit, p) : PORT_TO_PHY_ADDR(unit, p));
        if (phy_addr == 0xff) {
            cli_out("Port %s: No %sPHY address assigned\n", SOC_PORT_NAME(unit, p), intermediate ? "intermediate " : "");
            rv = CMD_FAIL;
            goto done;
        } 
        
        /*PC select*/
        if((!intermediate) && (EXT_PHY_SW_STATE(unit, p)) && (EXT_PHY_SW_STATE(unit, p)->read) && (EXT_PHY_SW_STATE(unit, p)->write)) {
            pc = EXT_PHY_SW_STATE(unit, p);
        } else if((INT_PHY_SW_STATE(unit, p)) && (INT_PHY_SW_STATE(unit, p)->read) && (INT_PHY_SW_STATE(unit, p)->write)){
            pc = INT_PHY_SW_STATE(unit, p);
        } else {
            cli_out("ERROR: Port %d isn't registered with MDIO functions\n", p);
            return CMD_FAIL;
        }
        /*write reg*/
        MIIM_LOCK(unit);                  
        rv = pc->write(unit, phy_addr, phy_reg, phy_data);
        MIIM_UNLOCK(unit);
        if (rv < 0) {
            cli_out("ERROR: Port %s: soc_miim_write failed: %s\n", SOC_PORT_NAME(unit, p), soc_errmsg(rv));
            rv = CMD_FAIL;
            goto done;
        }
    }
done:
    return rv;
}


STATIC cmd_result_t
_phy_phyctrl_reg_read(int unit, soc_pbmp_t pbm, uint32 phy_reg, int intermediate)
{
    soc_port_t p, dport;
    uint16 phy_addr;
    uint16 phy_data = 0;
    phy_ctrl_t* pc;
    cmd_result_t rv = CMD_OK;

    DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {

        phy_addr = (intermediate ? PORT_TO_PHY_ADDR_INT(unit, p) : PORT_TO_PHY_ADDR(unit, p));
        if (phy_addr == 0xff) {
            cli_out("Port %s: No %sPHY address assigned\n", SOC_PORT_NAME(unit, p), intermediate ? "intermediate " : "");
            continue;
        }

        cli_out("Port %s (PHY addr 0x%02x) Reg %d: ", SOC_PORT_NAME(unit, p), phy_addr, phy_reg);

        /*PC select*/
        if((!intermediate) && (EXT_PHY_SW_STATE(unit, p)) && (EXT_PHY_SW_STATE(unit, p)->read) && (EXT_PHY_SW_STATE(unit, p)->write)) {
            pc = EXT_PHY_SW_STATE(unit, p);
        } else if((INT_PHY_SW_STATE(unit, p)) && (INT_PHY_SW_STATE(unit, p)->read) && (INT_PHY_SW_STATE(unit, p)->write)){
            pc = INT_PHY_SW_STATE(unit, p);
        } else {
            cli_out("ERROR: Port %d isn't registered with MDIO functions\n", p);
            return CMD_FAIL;
        }

        MIIM_LOCK(unit);
        rv = pc->read(unit, phy_addr, phy_reg, &phy_data);
        MIIM_UNLOCK(unit);

        if (rv < 0) {
            cli_out("\nERROR: Port %s: soc_miim_read failed: %s\n",
                    SOC_PORT_NAME(unit, p), soc_errmsg(rv));
            rv = CMD_FAIL;
            goto done;
        }
        cli_out("0x%04x\n", phy_data);
     }
done:
    return rv;
}



/*
print all regs in the current block (Assume Clause 22) 
Output example: 
phy int xe13             
Port xe13 (intermediate PHY addr 0xec):
        0x00: 0x0000    0x01: 0x0000    0x02: 0x0000    0x03: 0x0000
        0x04: 0x0000    0x05: 0x0000    0x06: 0x0000    0x07: 0x0000
        0x08: 0x0000    0x09: 0x0000    0x0a: 0x0000    0x0b: 0x0000
        0x0c: 0x0000    0x0d: 0x0000    0x0e: 0x0100    0x0f: 0x0000
        0x10: 0x0000    0x11: 0x0000    0x12: 0x0000    0x13: 0x0000
        0x14: 0x0000    0x15: 0x0000    0x16: 0x0000    0x17: 0x0000
        0x18: 0x0000    0x19: 0x0000    0x1a: 0x0000    0x1b: 0x0000
        0x1c: 0x0000    0x1d: 0x0000    0x1e: 0x0003    0x1f: 0xffd0 
*/
STATIC cmd_result_t
_phy_phyctrl_regs_dump(int unit, soc_pbmp_t pbm, int intermediate){
    soc_port_t p, dport;
    uint16 phy_data = 0;
    uint16 phy_addr;
    uint32 phy_reg;
    phy_ctrl_t* pc;
    cmd_result_t rv = CMD_OK;

    DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {

        phy_addr = (intermediate ? PORT_TO_PHY_ADDR_INT(unit, p) : PORT_TO_PHY_ADDR(unit, p));
        if (phy_addr == 0xff) {
            cli_out("Port %s: No %sPHY address assigned\n",
                    SOC_PORT_NAME(unit, p),
                    intermediate ? "intermediate " : "");
            continue;
        }

        if (intermediate) {
            cli_out("Port %s (intermediate PHY addr 0x%02x):", SOC_PORT_NAME(unit, p), phy_addr);
        } else {     
            cli_out("Port %s (PHY addr 0x%02x): %s", SOC_PORT_NAME(unit, p), phy_addr, soc_phy_name_get(unit, p));
        }

        /*PC select*/
        if((!intermediate) && (EXT_PHY_SW_STATE(unit, p)) && (EXT_PHY_SW_STATE(unit, p)->read) && (EXT_PHY_SW_STATE(unit, p)->write)) {
            pc = EXT_PHY_SW_STATE(unit, p);
        } else if((INT_PHY_SW_STATE(unit, p)) && (INT_PHY_SW_STATE(unit, p)->read) && (INT_PHY_SW_STATE(unit, p)->write)){
            pc = INT_PHY_SW_STATE(unit, p);
        } else {
            cli_out("ERROR: Port %d isn't registered with MDIO functions\n", p);
            return CMD_FAIL;
        }

        MIIM_LOCK(unit);
        for (phy_reg = PHY_MIN_REG; phy_reg <= PHY_MAX_REG; phy_reg++) {
            rv = pc->read(unit, phy_addr, phy_reg, &phy_data);
            if (rv < 0) {
                MIIM_UNLOCK(unit);
                cli_out("\nERROR: Port %s: soc_miim_read failed: %s\n",
                        SOC_PORT_NAME(unit, p), soc_errmsg(rv));
                rv = CMD_FAIL;
                goto done;
            }
            cli_out("%s\t0x%02x: 0x%04x", ((phy_reg % DUMP_PHY_COLS) == 0) ? "\n" : "", phy_reg, phy_data);
        }
        MIIM_UNLOCK(unit);
        cli_out("\n");
    }
    cli_out("\n");
done:
    return rv;
}

STATIC cmd_result_t
__phy_diag_phy_unit_get(int unit_value, int *phy_dev)
{
    *phy_dev = PHY_DIAG_DEV_DFLT;
    if (unit_value == 0) {      /* internal PHY */
        *phy_dev = PHY_DIAG_DEV_INT;
    } else if ((unit_value > 0) && (unit_value < 4)) {
        *phy_dev = PHY_DIAG_DEV_EXT;
    } else if (unit_value != -1) {
        cli_out("unit is numeric value: 0,1,2, ...\n");
        return CMD_FAIL;
    }
    return CMD_OK;
}

STATIC cmd_result_t
__phy_diag_phy_if_get(char *if_str, int *dev_if)
{
    *dev_if = PHY_DIAG_INTF_DFLT;
    if (if_str) {
        if (sal_strcasecmp(if_str, "sys") == 0) {
            *dev_if = PHY_DIAG_INTF_SYS;
        } else if (sal_strcasecmp(if_str, "line") == 0) {
            *dev_if = PHY_DIAG_INTF_LINE;
        } else if (if_str[0] != 0) {
            cli_out("InterFace must be sys or line.\n");
            return CMD_FAIL;
        }
    } else {
        cli_out("Invalid Interface string\n");
        return CMD_FAIL;
    }
    return CMD_OK;
}

/* 
 * dsc diagnostics for devices that use phyctrl     
 */
STATIC cmd_result_t
_phy_diag_phyctrl_dsc(int unit, bcm_pbmp_t pbmp,args_t *args)
{
    bcm_port_t          port, dport;
    int                 rv, cmd; 
    soc_phy_wcmod_uc_desc_t *uc_desc = NULL;
    phy_ctrl_t  *pc;
    int i, size, num_core; 
    uint32 inst;
    parse_table_t pt;
    char *if_str;
    int phy_unit, phy_unit_value = 0, phy_unit_if;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "unit", PQ_DFL | PQ_INT, (void *)(0),
                    &phy_unit_value, NULL);
    parse_table_add(&pt, "if", PQ_STRING, 0, &if_str, NULL);
    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("Error: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    rv = __phy_diag_phy_if_get(if_str, &phy_unit_if);
    if (rv == CMD_OK) {
        rv = __phy_diag_phy_unit_get(phy_unit_value, &phy_unit);
    }
    if ((phy_unit_if != PHY_DIAG_INTF_SYS) && (phy_unit_if != PHY_DIAG_INTF_LINE)) {
        phy_unit_if = PHY_DIAG_INTF_LINE; /* set default value to line */
    }

    /* Now free allocated strings */
    parse_arg_eq_done(&pt);

    if (rv != CMD_OK) {
        return rv;
    }
    /* construct inst */
    inst = PHY_DIAG_INSTANCE(phy_unit, phy_unit_if, PHY_DIAG_LN_DFLT);

    rv = CMD_OK;
    cmd = PHY_DIAG_CTRL_DSC;

    uc_desc = sal_alloc(sizeof(soc_phy_wcmod_uc_desc_t)*MAX_NUM_LANES, "_phy_diag_dcmn.uc_desc");
    if(!uc_desc) {
        cli_out("Memory allocation failure.\n");
        return CMD_FAIL;
    }
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        MIIM_LOCK(unit);
        if (soc_phyctrl_diag_ctrl(unit, port, inst,
               PHY_DIAG_CTRL_CMD,cmd, uc_desc) != SOC_E_NONE) {
            rv = CMD_FAIL;
        }
        MIIM_UNLOCK(unit);

        /* get size */
        pc = INT_PHY_SW_STATE(unit, port);
        /*special ILKN*/
        if(SOC_IS_ARADPLUS(unit)){
#ifdef BCM_PETRA_SUPPORT
            if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (uc_desc[0].phy_ad == 0xe8)) {
                num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
            }else {
                num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
            }
#endif
        }
        else{
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }
        size = num_core * NUM_LANES;
        if (!SOC_IS_ARDON(unit)) {
            /* display */
            if (inst == PHY_DIAG_INT)
            {
                cli_out("\n\nDSC parameters for port %d\n\n", port);
                cli_out("PLL Range: %d\n\n", uc_desc[0].pll_range);
                cli_out("junction temprature:\n");
                for (i=0;i<num_core;i++)
                {   
                    cli_out("core%d %dc\n",i, uc_desc[i*4].temp);
                    
                }
                cli_out("\n");
                cli_out("AD   LN SD PMD(FW_MODE)   PPM clk90_ofs |clkp1_of p1_lvl m1_lvl PF  lowp |VGA  BIAS DFE1 DFE2 "
                        "DFE3 |DFE4 DFE5 TX_DRV |PREC MAIN POSTC MTRC PE  |ZE    ME  PO  ZO  MO |DSC_SM\n");
            
                for (i = 0; i < size; i++) {
                    
                    cli_out("0x%02x %d  %02d %-12s  %04d   %07d |%07d  %04d   %04d  %04d %04d |%04d %4s %04d %04d %04d |%04d "
                            "%04d 0x%04x |%04d %04d %04d %04d %04d |%04d %3d %3d %3d %3d |0x%04x\n",
                            uc_desc[i].phy_ad, uc_desc[i].lane ,uc_desc[i].sd,  uc_desc[i].pmd_mode_s, uc_desc[i].integ_reg, uc_desc[i].clk90_offset, uc_desc[i].clkp1_offset, uc_desc[i].p1_lvl,
                            uc_desc[i].m1_lvl,uc_desc[i].pf_ctrl,uc_desc[i].lopf, uc_desc[i].vga_sum,uc_desc[i].vga_bias_reduced,
                            uc_desc[i].dfe1_bin,uc_desc[i].dfe2_bin,uc_desc[i].dfe3_bin, uc_desc[i].dfe4_bin,
                            uc_desc[i].dfe5_bin,uc_desc[i].tx_drvr, uc_desc[i].tx_pre_cursor,uc_desc[i].tx_main,uc_desc[i].tx_post_cursor,
                            uc_desc[i].postc_metric,uc_desc[i].offset_pe,uc_desc[i].offset_ze,uc_desc[i].offset_me,
                            uc_desc[i].offset_po,uc_desc[i].offset_zo,uc_desc[i].offset_mo, uc_desc[i].dsc_sm);
                }
            }
        }
    }
    sal_free(uc_desc);
    return rv;
}

STATIC cmd_result_t
_phy_diag_portmod_dsc(int unit, bcm_pbmp_t pbmp,args_t *args)
{
#ifdef PORTMOD_SUPPORT
    bcm_port_t          port, dport;
    int                 rv = CMD_OK; 
    char               *cmd_str= NULL;
    parse_table_t pt;
    char *if_str;
    int phy_unit, phy_unit_value = 0, phy_unit_if;
    uint32 inst;
    int is_legacy_phy = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "unit", PQ_DFL | PQ_INT, (void *)(0),
                    &phy_unit_value, NULL);
    parse_table_add(&pt, "if", PQ_STRING, 0, &if_str, NULL);
    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("Error: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    rv = __phy_diag_phy_if_get(if_str, &phy_unit_if);
    if (rv == CMD_OK) {
        rv = __phy_diag_phy_unit_get(phy_unit_value, &phy_unit);
    }

    /* Now free allocated strings */
    parse_arg_eq_done(&pt);

    if (rv != CMD_OK) {
        return rv;
    }
    /* construct inst */
    inst = PHY_DIAG_INSTANCE(phy_unit, phy_unit_if, PHY_DIAG_LN_DFLT);

    if ((cmd_str = ARG_GET(args)) == NULL) {
        /* standard  */
        cmd_str = "STD";
    } else if (sal_strcasecmp(cmd_str, "external") == 0) {
        /*default phyn param is external- do nothing*/
        cmd_str = NULL;
    } else if (sal_strcasecmp(cmd_str, "config") == 0) {
        /* nothing need to do */
    }  else if (sal_strcasecmp(cmd_str, "state") == 0) {
        cmd_str = "STATE";
    } else {
         cli_out("valid sub option: external, config, state\n");
         return CMD_FAIL;
    }
   
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        rv = portmod_port_is_legacy_ext_phy_present(unit, port, &is_legacy_phy);
        if (rv < 0) {
            return rv;
        }
        if ((PHY_DIAG_DEV_DFLT == phy_unit || PHY_DIAG_DEV_EXT == phy_unit) && is_legacy_phy) {
            rv = soc_phyctrl_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_DSC, cmd_str);
        } else {
            rv = portmod_port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_DSC, cmd_str);
        }
        if (rv < 0) {
            return rv;
        }
    }
    return rv;
#else
    return CMD_FAIL;
#endif
}



STATIC cmd_result_t
_phy_diag_prbs_dcmn(int unit, bcm_pbmp_t pbmp, args_t *args)
{
    parse_table_t pt;
    bcm_port_t port, dport;
    int rv, cmd, enable;
    char *cmd_str, *if_str/*, *poly_str=NULL*/;
    int poly = 0, invert = 0;
    int phy_unit, phy_unit_value = -1, phy_unit_if;
    uint32 inst;
    char *prbs_polys[] = {"X7_X6_1", "X15_X14_1", "X23_X18_1", "X31_X28_1", "X9_X5_1", "X11_X9_1", "X58_X31_1",
                         "0", "1", "2", "3", "4", "5", "6"};


    enum { _PHY_PRBS_SET_CMD, _PHY_PRBS_GET_CMD, _PHY_PRBS_CLEAR_CMD };
    enum { _PHY_PRBS_SI_MODE, _PHY_PRBS_HC_MODE };

    if ((cmd_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    if (sal_strcasecmp(cmd_str, "set") == 0) {
        cmd = _PHY_PRBS_SET_CMD;
        enable = 1;
    } else if (sal_strcasecmp(cmd_str, "get") == 0) {
        cmd = _PHY_PRBS_GET_CMD;
        enable = 0;
    } else if (sal_strcasecmp(cmd_str, "clear") == 0) {
        cmd = _PHY_PRBS_CLEAR_CMD;
        enable = 0;
    } else
        return CMD_USAGE;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "unit", PQ_DFL | PQ_INT, (void *)(0),
                    &phy_unit_value, NULL);
    parse_table_add(&pt, "if", PQ_STRING, 0, &if_str, NULL);
    if (cmd == _PHY_PRBS_SET_CMD) {
        parse_table_add(&pt, "Polynomial", PQ_DFL|PQ_MULTI,
                        (void *)(0), &poly, &prbs_polys);
        parse_table_add(&pt, "Invert", PQ_DFL | PQ_INT,
                        (void *)(0), &invert, NULL);
    }
    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("Error: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    rv = (int)__phy_diag_phy_if_get(if_str, &phy_unit_if);
    if (rv == ((int)CMD_OK)) {
        rv = (int)__phy_diag_phy_unit_get(phy_unit_value, &phy_unit);
    }
    /* Now free allocated strings */
    parse_arg_eq_done(&pt);

    if (rv != CMD_OK) {
        return rv;
    }
    inst = PHY_DIAG_INSTANCE(phy_unit, phy_unit_if, PHY_DIAG_LN_DFLT);

    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        if (cmd == _PHY_PRBS_SET_CMD || cmd == _PHY_PRBS_CLEAR_CMD) {
            poly = poly % 7;
            /* Set polynomial */
            rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_SET,
                                       SOC_PHY_CONTROL_PRBS_POLYNOMIAL,
                                       INT_TO_PTR(poly));
            if (rv != ((int)BCM_E_NONE)) {
                cli_out("Setting prbs polynomial failed: %s\n",
                bcm_errmsg(rv));
                return CMD_FAIL;
            }

            /* Set invert */
            rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_SET,
                                       SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA,
                                       INT_TO_PTR(invert));
            if (rv != ((int)BCM_E_NONE)) {
                cli_out("Setting prbs invertion failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }

            rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_SET,
                                       SOC_PHY_CONTROL_PRBS_TX_ENABLE,
                                       INT_TO_PTR(enable));
            if (rv != ((int)BCM_E_NONE)) {
                cli_out("Setting prbs tx enable failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }

            rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_SET,
                                       SOC_PHY_CONTROL_PRBS_RX_ENABLE,
                                       INT_TO_PTR(enable));
            if (rv != ((int)BCM_E_NONE)) {
                cli_out("Setting prbs rx enable failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
        } else {                /* _PHY_PRBS_GET_CMD */
            int status;

            rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_GET,
                                       SOC_PHY_CONTROL_PRBS_RX_STATUS,
                                       (void *)&status);
            if (rv != ((int)BCM_E_NONE)) {
                cli_out("Getting prbs rx status failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }

            switch (status) {
                case 0:
                    cli_out("%s (%2d):  PRBS OK!\n", BCM_PORT_NAME(unit, port),
                            port);
                    break;
                case -1:
                    cli_out("%s (%2d):  PRBS Failed!\n",
                            BCM_PORT_NAME(unit, port), port);
                    break;
                default:
                    cli_out("%s (%2d):  PRBS has %d errors!\n",
                            BCM_PORT_NAME(unit, port), port, status);
                    break;
            }
        }
    }
    return CMD_OK;
}



/* 
 * Diagnostic utilities for serdes and PHY devices.
 *
 * Command format used in BCM diag shell:
 * phy diag <pbm> <sub_cmd> [sub cmd parameters]
 * All sub commands take two general parameters: unit and if. This identifies
 * the instance the command targets to.
 * unit = 0,1, ....  
 *   unit takes numeric values identifying the instance of the PHY devices 
 *   associated with the given port. A value 0 indicates the internal 
 *   PHY(serdes) the one directly connected to the MAC. A value 1 indicates
 *   the first external PHY.
 * if(interface) = [sys | line] 
 *   interface identifies the system side interface or line side interface of
 *   PHY device.
 * The list of sub commands:
 *   dsc - display tx/rx equalization information. Warpcore(WC) only.
 *   loopback - put the device in the given loopback mode 
 *              parameter: mode=[local | none]
 */
STATIC cmd_result_t
_phy_diag_dcmn(int unit, args_t *args)
{
    bcm_pbmp_t          pbmp;
    int                 rv;   
    char               *cmd_str, *port_str;

    rv = CMD_OK;
    if ((port_str = ARG_GET(args)) == NULL) {
        cli_out("        diag: Subcommands:\n"
                "              eyescan [vertical_max=<value>] [vertical_min=<value>]\n"
                "                 [horizontal_max=<value>] [horizontal_min=<value>]\n"
                "                 [sample_resolution=<value>] [sample_time=<value>]\n"
                "                 [counter=<value>] [flags=<value>] [error_threshold=<value>]\n"
                "                 [time_upper_bound=<value>] [nof_threshold_links=<value>] [lane=<value>]\n"
                "              loopback mode=<none|mac|phy>\n"
                "              berproj [hist_errcnt_threshold=<value>] [sample_time=<value>]\n"
                "              dsc\n"
                "        Example: phy diag 6 eyescan sample_time=15\n");
        return CMD_FAIL;
    }

    BCM_PBMP_CLEAR(pbmp);
    if (parse_bcm_pbmp(unit, port_str, &pbmp) < 0) {
        cli_out("ERROR: unrecognized port bitmap: %s\n", port_str);
        return CMD_FAIL;
    }

    if ((cmd_str = ARG_GET(args)) == NULL) {
        cli_out("ERROR: Invalid format. Please include sub option: eyescan|LoopBack|berproj|pcs\n");
        return CMD_FAIL;
    }

    /* linkscan should be disabled. soc_phyctrl_diag_ctrl() doesn't
     * assume exclusive access to the device.
     */
    BCM_IF_ERROR_RETURN(_bcm_linkscan_pause(unit));
    
    if (sal_strcasecmp(cmd_str, "dsc") == 0) {
        if (SOC_USE_PORTCTRL(unit)) {
            rv = _phy_diag_portmod_dsc(unit, pbmp, args);
        }else {
            rv = _phy_diag_phyctrl_dsc(unit,pbmp, args);
        }
    } else if (sal_strcasecmp(cmd_str, "pcs") == 0) {
        rv = _phy_diag_pcs(unit, pbmp, args);
    } else if (sal_strcasecmp(cmd_str, "eyescan") == 0) {
        rv = _phy_diag_eyescan(unit, pbmp, args);

    } else if (sal_strcasecmp(cmd_str, "LoopBack") == 0) {
        rv = _phy_diag_loopback_dcmn(unit, pbmp, args);

    } else if (sal_strcasecmp(cmd_str, "prbs") == 0) {
        rv = _phy_diag_prbs_dcmn(unit, pbmp, args);

    } else {
        cli_out("valid sub option: eyescan, LoopBack, dsc, prbs, pcs\n");
        rv = CMD_FAIL;
    }

    BCM_IF_ERROR_RETURN(_bcm_linkscan_continue(unit));

    return rv;
}


STATIC cmd_result_t 
_phy_diag_loopback_dcmn(int unit, bcm_pbmp_t pbmp,args_t *args) 
{
    parse_table_t       pt;
    bcm_port_t          port, dport;
    bcm_error_t         rv = BCM_E_NONE;
    char   *modes[] = {"none", "mac", "phy", "rmt", NULL};
    int    mode = -1;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "mode", PQ_MULTI | PQ_DFL , 0, &mode,  &modes);
    
    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("ERROR: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);
    if(mode == -1){
        cli_out("Missing mode parameter \n");
        return CMD_FAIL;
    }
    
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        rv = bcm_port_loopback_set(unit, port, mode);                 
        if (rv != BCM_E_NONE) {                                 
            cli_out("Setting loopback failed: %s\n",      
                    bcm_errmsg(rv));                    
            return CMD_FAIL;                                    
        }
    } 
    return CMD_OK;                                    
}


#ifdef BCM_PETRA_SUPPORT
void
phy_measure_ch0_calc(soc_reg_above_64_val_t reg_val, int *ch0_out_int, int *ch0_out_remainder) {
   int p_div, n_div, m0_div;
   p_div = get_field_dcmn(reg_val[1], 11, 13);
   n_div = get_field_dcmn(reg_val[0], 3, 10);
   if (p_div == 0) {p_div = 8;};
   if (n_div == 0) {n_div = 1024;};

   m0_div = get_field_dcmn(reg_val[0], 13, 20);
   if (m0_div == 0) {m0_div = 256;}
   *ch0_out_int = (125 / p_div * n_div) / m0_div;
   *ch0_out_remainder = (125000 / p_div * n_div) / m0_div - *ch0_out_int * 1000;
}

STATIC int
phy_diag_calc_os(int unit, soc_port_t port, int base_port, int* os_int, int* os_remainder){
    uint32 flags = 0x1, data16;
    int rv, osr_mode;

    MIIM_LOCK(unit);
    rv = bcm_port_phy_set(unit, port, flags, 0x1f, 0xffd0);
    if (rv != BCM_E_NONE) {
        MIIM_UNLOCK(unit);
        cli_out("ERROR: Failed in bcm_port_phy_set 0xffd0: %s\n", bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    rv = bcm_port_phy_set(unit, port, flags, 0x1e, base_port%4);
    if (rv != BCM_E_NONE) {
        MIIM_UNLOCK(unit);
        cli_out("ERROR: Failed in bcm_port_phy_set 0x1e: %s\n", bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    rv = bcm_port_phy_set(unit, port, flags, 0x1f, 0x8220);
    if (rv != BCM_E_NONE) {
        MIIM_UNLOCK(unit);
        cli_out("ERROR: Failed in bcm_port_phy_set 0x8220: %s\n", bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    rv = bcm_port_phy_get(unit, port, flags, 0x12, &data16);
    if (rv != BCM_E_NONE) {
        MIIM_UNLOCK(unit);
        cli_out("ERROR: Failed in bcm_port_phy_get 0x12: %s\n", bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    rv = bcm_port_phy_set(unit, port, flags, 0x1f, 0xffd0);
    if (rv != BCM_E_NONE) {
        MIIM_UNLOCK(unit);
        cli_out("ERROR: Failed in bcm_port_phy_set 0xffd0: %s\n", bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    rv = bcm_port_phy_set(unit, port, flags, 0x1e, 0x0);
    if (rv != BCM_E_NONE) {
        MIIM_UNLOCK(unit);
        cli_out("ERROR: Failed in bcm_port_phy_set 0x1e: %s\n", bcm_errmsg(rv));
        return BCM_E_FAIL;
    }
    MIIM_UNLOCK(unit);
       
    osr_mode = (data16 & 0x800) >> 11;
    osr_mode = (osr_mode * 8) + ((data16 & DSC3B0_CDR_STATUS2_OSCDR_MODE_MASK) >> DSC3B0_CDR_STATUS2_OSCDR_MODE_SHIFT);

    *os_remainder = 0;
    if (osr_mode == 0 ) {
        *os_int = 1;
    } else if (osr_mode == 1) {
        *os_int = 2;
    } else if (osr_mode == 2) {
        *os_int = 4;
    } else if (osr_mode == 3) {
        *os_int = 5;
    } else if (osr_mode == 6) {
        *os_int = 8;
        *os_remainder = 200;
    } else if (osr_mode == 7) {
        *os_int = 3;
        *os_remainder = 300;
    } else if (osr_mode == 8) {
        *os_int = 10;
    } else if (osr_mode == 9) {
        *os_int = 3;
    } else if (osr_mode == 10) {
        *os_int = 8;
    }
    else {
        return BCM_E_FAIL;
    }
    return BCM_E_NONE;
}


int
_phy_arad_nif_measure(int unit, bcm_port_t port, uint32 *type_of_bit_clk, int *one_clk_time_measured_int ,int *one_clk_time_measured_remainder, int *serdes_freq_int, int *serdes_freq_remainder, uint32 *lane) {
    uint32 sync_sth_cnt;        
    uint32 data_read;
    int serdes_freq_int_temp, serdes_freq_remainder_temp;
    int total_time_measured_int, total_time_measured_remainder;
    uint32 rx_wclk20_sel,rx_wclk33_sel;
    int number_of_gtimer_cycles = 10240;
    int clock_speed_int, clock_speed_remainder;
    int one_bit_clk_period_int, one_bit_clk_period_remainder;
    uint32 acc_port, flags;
    soc_dpp_config_arad_t *dpp_arad;
    uint32 reg_val, default_configuration, synce_prev_config = 0;
    int synce_read = 0;
    int counter;
    int os_int, os_remainder;
    int rv = BCM_E_NONE;
    uint16 lane_map;
    uint32 swapped_lane;

    rv = READ_NBI_GTIMER_CONFIGURATIONr(unit, &default_configuration);
    if (rv != BCM_E_NONE) {
        cli_out("ERROR: Failed to read NBI_GTIMER_CONFIGURATION: %s\n",   
                bcm_errmsg(rv));
        goto exit;
    }

    if(SOC_IS_DIRECT_PORT(unit, port)) {
        *lane = port - SOC_INFO(unit).physical_port_offset;
    } else {
        *lane = SOC_INFO(unit).port_l2p_mapping[port] - 1;
    }

    /* get swapped lane map. ex: 2301 */
    lane_map = soc_property_suffix_num_get(unit, (*lane)/4, spn_XGXS_RX_LANE_MAP, "quad", 0x3210) & 0xffff;
    /* find swapped number of lane (0-3). ex: if lane=14 -> swapped_lane=3 */                
    swapped_lane = ((lane_map >> (((*lane)%4)*4)) & 0xf);
    /* add base of lane to get swapped_lane. ex: if lane=14 -> swapped_lane=14-2+3=15 */
    swapped_lane = (*lane)-((*lane)%4)+swapped_lane;

    rv = READ_NBI_GTIMER_CONFIGURATIONr(unit, &reg_val);
    if (rv != BCM_E_NONE) {
        cli_out("ERROR: Failed to read NBI_GTIMER_CONFIGURATION: %s\n",   
                bcm_errmsg(rv));
        goto exit;
    }
    soc_reg_field_set(unit, NBI_GTIMER_CONFIGURATIONr, &reg_val, GTIMER_ENABLEf, 0x1);
    soc_reg_field_set(unit, NBI_GTIMER_CONFIGURATIONr, &reg_val, GTIMER_CYCLEf, 10240);
    soc_reg_field_set(unit, NBI_GTIMER_CONFIGURATIONr, &reg_val, GTIMER_RESET_ON_TRIGGERf, 0x0);
    rv = WRITE_NBI_GTIMER_CONFIGURATIONr(unit, reg_val);
    if (rv != BCM_E_NONE) {
        cli_out("ERROR: Failed to write NBI_GTIMER_CONFIGURATION: %s\n",   
                bcm_errmsg(rv));
        goto exit;
    }

    rv = READ_NBI_SYNC_ETH_CFGr(unit, 0, &reg_val);
    if (rv != BCM_E_NONE) {
        cli_out("ERROR: Failed to read NBI_SYNC_ETH_CFG: %s\n",   
                bcm_errmsg(rv));
        goto exit;
    }
    synce_read = 1;
    synce_prev_config = reg_val; /* Save synce configuration to restore at end of function */
    soc_reg_field_set(unit, NBI_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_CLOCK_SEL_Nf, swapped_lane);
    soc_reg_field_set(unit, NBI_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_CLOCK_DIV_Nf, 0x1);
    soc_reg_field_set(unit, NBI_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_SQUELCH_EN_Nf, 0x1);
    soc_reg_field_set(unit, NBI_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_LINK_VALID_SEL_Nf, 0x1);
    soc_reg_field_set(unit, NBI_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_GTIMER_MODE_Nf, 0x1);
    soc_reg_field_set(unit, NBI_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_NUM_OF_LANES_Nf, 0x0);
    rv = WRITE_NBI_SYNC_ETH_CFGr(unit, 0, reg_val);
    if (rv != BCM_E_NONE) {
        cli_out("ERROR: Failed to write NBI_SYNC_ETH_CFG: %s\n",   
                bcm_errmsg(rv));
        goto exit;
    }

    rv = READ_NBI_GTIMER_TRIGGERr(unit, &reg_val);
    if (rv != BCM_E_NONE) {
        cli_out("ERROR: Failed to read NBI_GTIMER_TRIGGER: %s\n",   
                bcm_errmsg(rv));
        goto exit;
    }
    soc_reg_field_set(unit, NBI_GTIMER_TRIGGERr, &reg_val, GTIMER_TRIGGERf, 0x0);
    rv = WRITE_NBI_GTIMER_TRIGGERr(unit, reg_val);
    if (rv != BCM_E_NONE) {
        cli_out("ERROR: Failed to write NBI_GTIMER_TRIGGER: %s\n",   
                bcm_errmsg(rv));
        goto exit;
    }

    sal_usleep(500000);

    soc_reg_field_set(unit, NBI_GTIMER_TRIGGERr, &reg_val, GTIMER_TRIGGERf, 0x1);
    rv = WRITE_NBI_GTIMER_TRIGGERr(unit, reg_val);
    if (rv != BCM_E_NONE) {
        cli_out("ERROR: Failed to write NBI_GTIMER_TRIGGER: %s\n",   
                bcm_errmsg(rv));
        goto exit;
    }
    
    rv = READ_NBI_GTIMER_TRIGGERr(unit, &reg_val);
    if (rv != BCM_E_NONE) {
        cli_out("ERROR: Failed to read NBI_GTIMER_TRIGGER: %s\n",   
                bcm_errmsg(rv));
        goto exit;
    }
    
    counter = 0;
    while (reg_val == 0x1) {
        sal_usleep(500000);
        rv = READ_NBI_GTIMER_TRIGGERr(unit, &reg_val);
        if (rv != BCM_E_NONE) {
            cli_out("ERROR: Failed to read NBI_GTIMER_TRIGGER: %s\n",   
                    bcm_errmsg(rv));
            goto exit;
        }
        if(10 == counter){
            cli_out("ERROR: Waiting for NBI_GTIMER_TRIGGER timeout\n");
            rv = BCM_E_TIMEOUT;
            goto exit;
        }
        counter++;
    }
    
    dpp_arad = SOC_DPP_CONFIG(unit)->arad;
    
    if(0 == dpp_arad->init.core_freq.frequency){
        cli_out("ERROR: dpp_arad->init.core_freq.frequency is 0\n");
        rv = BCM_E_INTERNAL;
        goto exit;
    }
    clock_speed_int = (1000000 / dpp_arad->init.core_freq.frequency);
    clock_speed_remainder = ((1000000 / (dpp_arad->init.core_freq.frequency / 10000))) % 10000;
                
    total_time_measured_int = (((clock_speed_int * 10000) + clock_speed_remainder) * number_of_gtimer_cycles) / 10000; 
    total_time_measured_remainder = ((int)(((clock_speed_int * 10000) + clock_speed_remainder) * (number_of_gtimer_cycles)) % 10000); 
    
    rv = READ_NBI_SYNC_ETH_COUNTERr(unit, 0, &sync_sth_cnt);
    if (rv != BCM_E_NONE) {
        cli_out("ERROR: Failed to read NBI_SYNC_ETH_COUNTER: %s\n",   
                bcm_errmsg(rv));
        goto exit;
    }

    if(SOC_IS_DIRECT_PORT(unit, port)) {
        int array_size;
        phyident_core_info_t core_info;
        uint32 phy_port= 1 + *lane;

        flags = BCM_PORT_PHY_INTERNAL | BCM_PORT_PHY_NOMAP;
        phyident_core_info_t_init(&core_info);
        rv = _dpp_phy_addr_multi_get(unit, phy_port, 0, 1, &array_size, &core_info);
        if (rv != BCM_E_NONE) {
            return rv;
        }
        acc_port = core_info.mdio_addr;
    } else {
        flags = BCM_PORT_PHY_INTERNAL;
        acc_port = port;
    }

    MIIM_LOCK(unit);
    /* reading rx_wclk20_sel from serdes 0x8141 */
    rv = bcm_port_phy_set(unit, acc_port, flags, 0x1f, 0xffd0);
    if (rv != BCM_E_NONE) {
        MIIM_UNLOCK(unit);
        cli_out("ERROR: Failed in bcm_port_phy_set 0xffd0: %s\n",   
                bcm_errmsg(rv));
        goto exit;
    }
    rv = bcm_port_phy_set(unit, acc_port, flags, 0x1e, 0x0);
    if (rv != BCM_E_NONE) {
        MIIM_UNLOCK(unit);
        cli_out("ERROR: Failed in bcm_port_phy_set 0x1e: %s\n",   
                bcm_errmsg(rv));
        goto exit;
    }
    rv = bcm_port_phy_set(unit, acc_port, flags, 0x1f, 0x8140);
    if (rv != BCM_E_NONE) {
        MIIM_UNLOCK(unit);
        cli_out("ERROR: Failed in bcm_port_phy_set 0x8140: %s\n",   
                bcm_errmsg(rv));
        goto exit;
    }
    rv = bcm_port_phy_get(unit, acc_port, flags, 0x11, &data_read);
    if (rv != BCM_E_NONE) {
        MIIM_UNLOCK(unit);
        cli_out("ERROR: Failed in bcm_port_phy_get 0x11: %s\n",   
                bcm_errmsg(rv));
        goto exit;
    }
    MIIM_UNLOCK(unit);

    rx_wclk20_sel = (data_read >> 12) & 0x00000001; /*bit12*/
    rx_wclk33_sel = (data_read >> 14) & 0x00000001; /*bit14*/
    if (rx_wclk33_sel) {
        *type_of_bit_clk = 33;
    } else {
        if (rx_wclk20_sel){
            *type_of_bit_clk = 40;
        } else {
            *type_of_bit_clk = 66;    
        }
    }

    rv = READ_NBI_GTIMER_CONFIGURATIONr(unit, &reg_val);
    if (rv != BCM_E_NONE) {
        cli_out("ERROR: Failed to read NBI_GTIMER_CONFIGURATION: %s\n",   
                bcm_errmsg(rv));
        goto exit;
    }
    soc_reg_field_set(unit, NBI_GTIMER_CONFIGURATIONr, &reg_val, GTIMER_ENABLEf, 0x0);
    rv = WRITE_NBI_GTIMER_CONFIGURATIONr(unit, reg_val);
    if (rv != BCM_E_NONE) {
        cli_out("ERROR: Failed to write NBI_GTIMER_CONFIGURATION: %s\n",   
                bcm_errmsg(rv));
        goto exit;
    }
    
    if(0 == sync_sth_cnt){
        cli_out("ERROR: sync_sth_cnt is 0\n");
        rv = BCM_E_INTERNAL;
        goto exit;
    }
    *one_clk_time_measured_int = (((total_time_measured_int*10000) + (total_time_measured_remainder)) / sync_sth_cnt) / 10000;
    *one_clk_time_measured_remainder = ((((total_time_measured_int*10000) + (total_time_measured_remainder)) / sync_sth_cnt) % 10000);
    if(0 == *type_of_bit_clk){
        cli_out("ERROR: *type_of_bit_clk is 0\n");
        rv = BCM_E_INTERNAL;
        goto exit;
    }
    one_bit_clk_period_int = ((*one_clk_time_measured_int*10000 + *one_clk_time_measured_remainder) / *type_of_bit_clk) / 10000;
    one_bit_clk_period_remainder = (((*one_clk_time_measured_int*10000 + *one_clk_time_measured_remainder) / *type_of_bit_clk) % 10000);
    if(0 == (one_bit_clk_period_int*10000 + one_bit_clk_period_remainder)){
        cli_out("ERROR: one_bit_clk_period is 0\n");
        rv = BCM_E_INTERNAL;
        goto exit;
    }

    serdes_freq_int_temp = (10000/(one_bit_clk_period_int*10000 + one_bit_clk_period_remainder));
    serdes_freq_remainder_temp = (10000000/(one_bit_clk_period_int*10000 + one_bit_clk_period_remainder)) % 1000;

    rv = phy_diag_calc_os(unit, port, *lane, &os_int, &os_remainder);
    if (rv != BCM_E_NONE) {
        goto exit;
    }

    *serdes_freq_int = ((((serdes_freq_int_temp * 1000) + serdes_freq_remainder_temp) * 1000) / ((os_int * 1000) + os_remainder)) / 1000;
    *serdes_freq_remainder = ((((serdes_freq_int_temp * 1000) + serdes_freq_remainder_temp) * 1000) / ((os_int * 1000) + os_remainder)) % 1000;
                          
exit:
    if (WRITE_NBI_GTIMER_CONFIGURATIONr(unit, default_configuration) != BCM_E_NONE) {
        cli_out("ERROR: Failed to write back the default configuration to NBI_GTIMER_CONFIGURATION.\n");
    }

    if (synce_read) {
        if (WRITE_NBI_SYNC_ETH_CFGr(unit, 0, synce_prev_config) != BCM_E_NONE) {
            cli_out("ERROR: Failed to write back the original configuration to NBI_SYNC_ETH_CFG.\n");
        }
    }

    return rv;
}
#endif

#ifdef BCM_JERICHO_SUPPORT
STATIC cmd_result_t
jer_phy_measure_pll_calc(soc_dcmn_init_serdes_ref_clock_t ref_clk, int n_div, int p_div, int m0_div, int *ch0_out_int, int *ch0_out_remainder)
{
    int ref_clk_speed_khz = 0, reminder_digits = 0, reminder_temp = 0;

    if (p_div == 0 || m0_div == 0) {
        return CMD_FAIL;
    }

    switch (ref_clk) {
    case soc_dcmn_init_serdes_ref_clock_125:
        ref_clk_speed_khz = 125000;
        break;
    case soc_dcmn_init_serdes_ref_clock_156_25:
        ref_clk_speed_khz = 156250;
        break;
    default:
        return CMD_FAIL;
    }
    /**
     * The output should be in MHz so konvert from Kilo to Mega
     */
    *ch0_out_int = ((ref_clk_speed_khz / p_div * n_div) / m0_div) / 1000;
    *ch0_out_remainder = ((ref_clk_speed_khz / p_div * n_div) / m0_div) % 1000;
    reminder_temp = *ch0_out_remainder;

    while (reminder_temp) {
        reminder_temp /= 10;
        reminder_digits++;
    }

    if (reminder_digits > SOC_DCMN_DIAG_NIF_NOF_REF_CLK_REMINDER_DIGITS) {
        *ch0_out_remainder /= 10 * (reminder_digits - SOC_DCMN_DIAG_NIF_NOF_REF_CLK_REMINDER_DIGITS);
    }

    return CMD_OK;
}

cmd_result_t
jer_phy_measure_nif_pll(int unit, bcm_port_t port, int *ch0_out_int, int *ch0_out_remainder)
{
    int rv;
    soc_dcmn_init_serdes_ref_clock_t ref_clk;
    int p_div, n_div, m0_div;

    rv = soc_jer_phy_nif_pll_div_get(unit, port, &ref_clk, &p_div, &n_div, &m0_div);
    if (rv != SOC_E_NONE) {
        return CMD_FAIL;
    }

    return jer_phy_measure_pll_calc(ref_clk, n_div, p_div, m0_div, ch0_out_int, ch0_out_remainder);
}

cmd_result_t
jer_phy_measure_fabric_pll(int unit, bcm_port_t port, int *ch0_out_int, int *ch0_out_remainder)
{
    int rv;
    bcm_port_t lane;
    soc_reg_above_64_val_t reg_val;
    soc_dcmn_init_serdes_ref_clock_t ref_clk;
    int p_div, n_div, m0_div;

    lane = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);

    if (lane / SOC_DCMN_NOF_LINKS_IN_MAC < 6) {
        rv = READ_ECI_FAB_0_PLL_CONFIGr(unit, reg_val);
        if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }
        n_div = soc_reg_above_64_field32_get(unit, ECI_FAB_0_PLL_CONFIGr, reg_val, FAB_0_PLL_CFG_NDIVf);
        p_div = soc_reg_above_64_field32_get(unit, ECI_FAB_0_PLL_CONFIGr, reg_val, FAB_0_PLL_CFG_PDIVf);
        m0_div = soc_reg_above_64_field32_get(unit, ECI_FAB_0_PLL_CONFIGr, reg_val, FAB_0_PLL_CFG_CH_0_MDIVf);
        ref_clk = SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_fabric_in[0];
    } else {
        rv = READ_ECI_FAB_1_PLL_CONFIGr(unit, reg_val);
        if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }
        n_div = soc_reg_above_64_field32_get(unit, ECI_FAB_1_PLL_CONFIGr, reg_val, FAB_1_PLL_CFG_NDIVf);
        p_div = soc_reg_above_64_field32_get(unit, ECI_FAB_1_PLL_CONFIGr, reg_val, FAB_1_PLL_CFG_PDIVf);
        m0_div = soc_reg_above_64_field32_get(unit, ECI_FAB_1_PLL_CONFIGr, reg_val, FAB_1_PLL_CFG_CH_0_MDIVf);
        ref_clk = SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_fabric_in[1];
    }

    return jer_phy_measure_pll_calc(ref_clk, n_div, p_div, m0_div, ch0_out_int, ch0_out_remainder);
}

#endif /*BCM_JERICHO_SUPPORT*/


int diag_dcmn_phy_measure_port(int unit, bcm_port_t port, int is_rx, uint32* rate_int, uint32* rate_remainder)
{
    int fabric_mac_bus_size = 0, counter_disabled = 0, 
        clock_speed_int = 0, clock_speed_remainder = 0,
        rv =  BCM_E_NONE;
    uint64 period, val, reg_val64,
           rate_start, rate_end;
    uint32 rate;
    bcm_port_t lane;
    bcm_stat_val_t type;
    int ilkn_o_fabric_port;
#ifdef BCM_PETRA_SUPPORT
    uint32 offset;
    int link_offset;
    uint32 first_phy_port;
#endif

    type = is_rx? snmpBcmRxAsynFifoRate:snmpBcmTxAsynFifoRate;

#ifdef BCM_PETRA_SUPPORT 
    if (SOC_IS_ARAD(unit)) {
        soc_dpp_config_arad_t *dpp_arad;
        dpp_arad = SOC_DPP_CONFIG(unit)->arad;
                                  
        clock_speed_int = (1000000 / dpp_arad->init.core_freq.frequency);
        clock_speed_remainder = ((1000000 / (dpp_arad->init.core_freq.frequency / 1000))) % 1000;
    }
#endif    
    
#ifdef BCM_DFE_SUPPORT
    if (SOC_IS_DFE(unit)) {
                                  
        clock_speed_int = ((1000000 / SOC_DFE_CONFIG(unit).core_clock_speed));
        clock_speed_remainder = ((1000000 / (SOC_DFE_CONFIG(unit).core_clock_speed / 1000)) % 1000);
    }
#endif   

#ifdef BCM_PETRA_SUPPORT
    if (!IS_SFI_PORT(unit, port)) {
        rv = soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset); 
        if (rv != SOC_E_NONE) {
            return CMD_FAIL;
        } 
        ilkn_o_fabric_port = SOC_DPP_IS_ILKN_PORT_OVER_FABRIC(unit,offset);
    } else 
#endif
    {
        ilkn_o_fabric_port = 0;
    }

    if(SOC_IS_DFE(unit) || (SOC_IS_ARAD(unit) && (IS_SFI_PORT(unit, port) || ilkn_o_fabric_port))) {
        lane = port;
        
#ifdef BCM_PETRA_SUPPORT
        if(SOC_IS_DPP(unit)) {
            if (ilkn_o_fabric_port) {
                rv =  soc_port_sw_db_first_phy_port_get(unit, port , &first_phy_port);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }
                lane = first_phy_port - SOC_DPP_FIRST_FABRIC_PHY_PORT(unit);
            } else {
                link_offset = SOC_DPP_DEFS_GET(unit,first_fabric_link_id);
                lane = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port - link_offset);
            }
        }
#endif
        COMPILER_64_SET(period, 0, 10000);
        COMPILER_64_SET(val, 0, 0);
        COMPILER_64_SET(reg_val64, 0, 0);
        rv = READ_FMAC_FMAL_STATISTICS_GTIMERr(unit, lane/SOC_DCMN_NOF_LINKS_IN_MAC, &reg_val64);
        if (rv != SOC_E_NONE) {
            return CMD_FAIL;
        }
        soc_reg64_field_set(unit, FMAC_FMAL_STATISTICS_GTIMERr, &reg_val64, GTIMER_PERIODf, period);
        soc_reg64_field_set(unit, FMAC_FMAL_STATISTICS_GTIMERr, &reg_val64, GTIMER_STARTf, val);
        rv = WRITE_FMAC_FMAL_STATISTICS_GTIMERr(unit, lane/SOC_DCMN_NOF_LINKS_IN_MAC, reg_val64);
        if (rv != SOC_E_NONE) {
            return CMD_FAIL;
        }

         /*clear stats*/
        rv = bcm_stat_sync(unit);
        /*if counter thread is disabled, no need to sync*/
        if (rv == BCM_E_DISABLED) {
            counter_disabled = 1;
        }
        else if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }
        
        rv = bcm_stat_get(unit, port, type, &rate_start);
        if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }

        /*start gtimer stats*/
        COMPILER_64_SET(val, 0, 1);
        rv = READ_FMAC_FMAL_STATISTICS_GTIMERr(unit, lane/SOC_DCMN_NOF_LINKS_IN_MAC, &reg_val64);
        if (rv != SOC_E_NONE) {
            return CMD_FAIL;
        }
        soc_reg64_field_set(unit, FMAC_FMAL_STATISTICS_GTIMERr, &reg_val64, GTIMER_STARTf, val);
        rv = WRITE_FMAC_FMAL_STATISTICS_GTIMERr(unit, lane/SOC_DCMN_NOF_LINKS_IN_MAC, reg_val64);
        if (rv != SOC_E_NONE) {
            return CMD_FAIL;
        }

        /*getting rate*/
        rv = bcm_stat_sync(unit);
        /*if counter thread is disabled, no need to sync*/
        if (rv == BCM_E_DISABLED) {
            counter_disabled = 1;
        }
        else if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }
        rv = bcm_stat_get(unit, port, type, &rate_end);
        if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }
        
        /* FifoRate counter collected in DFE, not collected in ARAD
         * if DFE, subtract collected rate_end from collected rate_start 
         * if ARAD, rate_start clears counter, and therefore rate_end is correct rate */
        if ((SOC_IS_DFE(unit) || SOC_IS_JERICHO(unit)) && !counter_disabled) {
            COMPILER_64_SUB_64(rate_end, rate_start);
        }
        rate = COMPILER_64_LO(rate_end);

#ifdef BCM_DFE_SUPPORT
        if (SOC_IS_DFE(unit))
        {
            fabric_mac_bus_size = SOC_DFE_DEFS_GET(unit, fabric_mac_bus_size);
        }
#endif
#ifdef BCM_PETRA_SUPPORT
        if (SOC_IS_DPP(unit))
        {
            fabric_mac_bus_size = SOC_DPP_DEFS_GET(unit, fabric_mac_bus_size);
            if (ilkn_o_fabric_port) {
                /*asyn fifo is in units of 40 (instead of 60) for ilkn over fabric*/
                fabric_mac_bus_size = 40;
            }
        }
#endif

        *rate_int = (rate * fabric_mac_bus_size * 1000)/ (COMPILER_64_LO(period) * (clock_speed_int * 1000 + (clock_speed_remainder%1000)));
        *rate_remainder = ((rate * fabric_mac_bus_size * 1000)/ ((COMPILER_64_LO(period)/1000) * (clock_speed_int * 1000 + (clock_speed_remainder%1000)))) % 1000;

        /* clear fmac_fmal_statistics_gtimer*/

        rv = READ_FMAC_FMAL_STATISTICS_GTIMERr(unit, lane/SOC_DCMN_NOF_LINKS_IN_MAC, &reg_val64);
        if (rv != SOC_E_NONE) {
            return CMD_FAIL;
        }
        soc_reg64_field32_set(unit, FMAC_FMAL_STATISTICS_GTIMERr, &reg_val64, GTIMER_STARTf, 0);
        rv = WRITE_FMAC_FMAL_STATISTICS_GTIMERr(unit, lane/SOC_DCMN_NOF_LINKS_IN_MAC, reg_val64);
        if (rv != SOC_E_NONE) {
            return CMD_FAIL;
        }
    }

return CMD_OK;
}

STATIC cmd_result_t
_phy_control(int unit, args_t *args)
{
    soc_pbmp_t pbm;
    soc_port_t p, dport;
    char *c;
    uint32 wan_mode, preemphasis, predriver_current, driver_current, dfe, lp_dfe, br_dfe, linkTraining;
    uint32 sw_rx_los_nval = 0;
    uint32 eq_boost;
    uint32 interface;
    uint32 interfacemax;
    uint32 flags;
    int print_header;
    cmd_result_t cmd_rv;
    bcm_port_config_t pcfg;
    int eq_tune = FALSE;
    int eq_status = FALSE;
    int dump = FALSE;
    int farEndEqValue = 0;
#ifdef INCLUDE_MACSEC
    uint32 macsec_switch_fixed, macsec_switch_fixed_speed;
    uint32 macsec_switch_fixed_duplex, macsec_switch_fixed_pause;
    uint32 macsec_pause_rx_fwd, macsec_pause_tx_fwd;
    uint32 macsec_line_ipg, macsec_switch_ipg;

    macsec_switch_fixed = 0;
    macsec_switch_fixed_speed = 0;
    macsec_switch_fixed_duplex = 0;
    macsec_switch_fixed_pause = 0;
    macsec_pause_rx_fwd = 0;
    macsec_pause_tx_fwd = 0;
    macsec_line_ipg = 0;
    macsec_switch_ipg = 0;
#endif

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(args));
        return CMD_FAIL;
    }

    wan_mode = 0;
    preemphasis = 0;
    predriver_current = 0;
    driver_current = 0;
    interface = 0;
    interfacemax = 0;
    eq_boost = 0;

    dfe = 0;
    lp_dfe = 0;
    br_dfe = 0;
    linkTraining = 0;

    if ((c = ARG_GET(args)) == NULL) {
        SOC_PBMP_ASSIGN(pbm, pcfg.port);
    } else if (parse_bcm_pbmp(unit, c, &pbm) < 0) {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(args), c);
        return CMD_FAIL;
    }

    BCM_PBMP_AND(pbm, pcfg.port);

    flags = 0;
    if ((c = ARG_CUR(args)) != NULL) {
        parse_table_t pt;
        int i;

        if (c[0] == '=') {
            return CMD_USAGE;       /* '=' unsupported */
        }
        if (sal_strcasecmp(c, "RxTune") == 0) {
            if (ARG_CNT(args) <= 1) { /*Current counter is at RxTune */
                ARG_NEXT(args);
                farEndEqValue = 0;
                cli_out("far end equalization value not input, using 0\n");
            } else {
                /* go to next argument */
                ARG_NEXT(args);
                /* Get far end equalization value  */
                if ((c = ARG_GET(args)) != NULL) {
                    farEndEqValue = sal_ctoi(c, 0);
                    cli_out("far end equalization value input (%d)\n",
                              farEndEqValue);
                }
            }
            eq_tune = TRUE;
        }
        if (sal_strcasecmp(c, "Dump") == 0) {
            c = ARG_GET(args);
            dump = TRUE;
        }

        if ((eq_tune == FALSE) && (dump == FALSE)) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "WanMode", PQ_DFL | PQ_BOOL,
                            0, &wan_mode, 0);
            parse_table_add(&pt, "Preemphasis", PQ_DFL | PQ_INT,
                            0, &preemphasis, 0);
            parse_table_add(&pt, "DriverCurrent", PQ_DFL | PQ_INT,
                            0, &driver_current, 0);
            parse_table_add(&pt, "PreDriverCurrent", PQ_DFL | PQ_INT,
                            0, &predriver_current, 0);
            parse_table_add(&pt, "EqualizerBoost", PQ_DFL | PQ_INT,
                            0, &eq_boost, 0);
            parse_table_add(&pt, "Interface", PQ_DFL | PQ_INT,
                            0, &interface, 0);
            parse_table_add(&pt, "InterfaceMax", PQ_DFL | PQ_INT,
                            0, &interfacemax, 0);
            parse_table_add(&pt, "SwRxLos", PQ_DFL | PQ_INT,
                            0, &sw_rx_los_nval, 0);
            parse_table_add(&pt, "Dfe", PQ_DFL | PQ_INT,
                            0, &dfe, 0);
            parse_table_add(&pt, "LpDfe", PQ_DFL | PQ_INT,
                            0, &lp_dfe, 0);
            parse_table_add(&pt, "BrDfe", PQ_DFL | PQ_INT,
                            0, &br_dfe, 0);
            parse_table_add(&pt, "LT", PQ_DFL | PQ_INT,
                            0, &linkTraining, 0);

#ifdef INCLUDE_MACSEC
            parse_table_add(&pt, "MacsecSwitchFixed", PQ_DFL | PQ_BOOL,
                            0, &macsec_switch_fixed, 0);
            parse_table_add(&pt, "MacsecSwitchFixedSpeed", PQ_DFL | PQ_INT,
                            0, &macsec_switch_fixed_speed, 0);
            parse_table_add(&pt, "MacsecSwitchFixedDuplex",
                            PQ_DFL | PQ_BOOL, 0,
                            &macsec_switch_fixed_duplex, 0);
            parse_table_add(&pt, "MacsecSwitchFixedPause", PQ_DFL | PQ_BOOL,
                            0, &macsec_switch_fixed_pause, 0);
            parse_table_add(&pt, "MacsecPauseRXForward", PQ_DFL | PQ_BOOL,
                            0, &macsec_pause_rx_fwd, 0);
            parse_table_add(&pt, "MacsecPauseTXForward", PQ_DFL | PQ_BOOL,
                            0, &macsec_pause_tx_fwd, 0);
            parse_table_add(&pt, "MacsecLineIPG", PQ_DFL | PQ_INT, 0,
                            &macsec_line_ipg, 0);
            parse_table_add(&pt, "MacsecSwitchIPG", PQ_DFL | PQ_INT, 0,
                            &macsec_switch_ipg, 0);
#endif

            if (parse_arg_eq(args, &pt) < 0) {
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
            if (ARG_CNT(args) > 0) {
                cli_out("%s: Unknown argument %s\n", ARG_CMD(args), ARG_CUR(args));
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }

            for (i = 0; i < pt.pt_cnt; i++) {
                if (pt.pt_entries[i].pq_type & PQ_PARSED) {
                    flags |= (1 << i);
                }
            }
            parse_arg_eq_done(&pt);
        }
    }
    DPORT_BCM_PBMP_ITER(unit, pbm, dport, p) {
        print_header = TRUE;

        if (eq_tune == TRUE) {

            cmd_rv =
                bcm_port_control_set(unit, p,
                                     bcmPortControlSerdesDriverEqualizationFarEnd,
                                     farEndEqValue);

            cmd_rv = bcm_port_control_set(unit, p,
                                          bcmPortControlSerdesDriverTune,
                                          1);
            if (cmd_rv != CMD_OK) {
                cli_out("unit %d port %d Tuning function not available\n",
                        unit, p);
                continue;
            }
            cli_out("Rx Equalization Tuning start\n");
            sal_usleep(1000000);
            cmd_rv = bcm_port_control_get(unit, p,
                                          bcmPortControlSerdesDriverEqualizationTuneStatusFarEnd,
                                          &eq_status);

            cli_out("unit %d port %d Tuning done, Status: %s\n",
                    unit, p, ((cmd_rv == CMD_OK) && eq_status) ? "OK" : "FAIL");
            continue;
        }
        if (dump == TRUE) {
            /* coverity[returned_value] */
            cmd_rv =
                bcm_port_phy_control_set(unit, p, BCM_PORT_PHY_CONTROL_DUMP,
                                         1);
            continue;
        }

        /* Read and set Preemphasis */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_PREEMPHASIS,
                                         preemphasis, flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set Driver Current */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_DRIVER_CURRENT,
                                         driver_current, flags,
                                         &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }


        /* Read and set DFE */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_FIRMWARE_DFE_ENABLE,
                                         dfe, flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set LP_DFE */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE,
                                         lp_dfe, flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set  BR_DFE */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_FIRMWARE_BR_DFE_ENABLE,
                                         br_dfe, flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set forced cl72/93 */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_CL72,
                                         linkTraining, flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }


        /* Read and set the interface */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_INTERFACE,
                                         interface, flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }
#ifdef INCLUDE_MACSEC

        /* Read and set the Switch fixed */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED,
                                         macsec_switch_fixed,
                                         flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set the Switch fixed Speed */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_SPEED,
                                         macsec_switch_fixed_speed,
                                         flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set the Switch fixed Duplex */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_DUPLEX,
                                         macsec_switch_fixed_duplex,
                                         flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set the Switch fixed Pause */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_PAUSE,
                                         macsec_switch_fixed_pause,
                                         flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set Pause Receive Forward */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_PAUSE_RX_FORWARD,
                                         macsec_pause_rx_fwd,
                                         flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set Pause transmit Forward */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_PAUSE_TX_FORWARD,
                                         macsec_pause_tx_fwd,
                                         flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set Line Side IPG */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_LINE_IPG,
                                         macsec_line_ipg,
                                         flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set Switch Side IPG */
        cmd_rv = port_phy_control_update(unit, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_IPG,
                                         macsec_switch_ipg,
                                         flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }
#endif
    }
    return CMD_OK;
}

STATIC cmd_result_t
_phy_dumpall(int unit, args_t *args)
{
    char *c;
    uint16 phy_data, phy_devad = 0;
    uint16 phy_addr;
    uint32 phy_reg;
    uint8 phy_addr_start = 0;
    uint8 phy_addr_end = 0xFF;
    int is_c45 = 0;
    int rv = 0;

    if ((c = ARG_GET(args)) == NULL) {
        cli_out("%s: Error: expecting \"c45\" or \"c22\"\n", ARG_CMD(args));
        return CMD_USAGE;
    }
    is_c45 = 0;
    if (sal_strcasecmp(c, "c45") == 0) {
        is_c45 = 1;
        if (!soc_feature(unit, soc_feature_phy_cl45)) {
            cli_out("%s: Error: Device does not support clause 45\n",
                    ARG_CMD(args));
            return CMD_USAGE;
        }
    } else if (sal_strcasecmp(c, "c22") != 0) {
        cli_out("%s: Error: expecting \"c45\" or \"c22\"\n", ARG_CMD(args));
        return CMD_USAGE;
    }
    if ((c = ARG_GET(args)) != NULL) {
        char *end;

        phy_addr_start = sal_strtoul(c, &end, 0);
        if (*end) {
            cli_out("%s: Error: Expecting PHY start address [%s]\n",
                    ARG_CMD(args), c);
            return CMD_USAGE;
        }
        if ((c = ARG_GET(args)) != NULL) {
            phy_addr_end = sal_strtoul(c, &end, 0);
            if (*end) {
                cli_out("%s: Error: Expecting PHY end address [%s]\n",
                        ARG_CMD(args), c);
                return CMD_USAGE;
            }
        } else {
            /* If start specified but no end, just print one phy address */
            phy_addr_end = phy_addr_start;
        }
    }
    if (is_c45) {
        cli_out("%4s%5s %5s %3s: %-6s\n", "", "PRTAD", "DEVAD", "REG",
                "VALUE");
        for (phy_addr = phy_addr_start; phy_addr <= phy_addr_end;
             phy_addr++) {
            /* Clause 45 supports 32 devices per phy. */
            for (phy_devad = 0; phy_devad <= 0x1f; phy_devad++) {
                /* Device ID is in registers 2 and 3 */
                for (phy_reg = 2; phy_reg <= 3; phy_reg++) {
                    rv = soc_miimc45_read(unit, phy_addr, phy_devad,
                                          phy_reg, &phy_data);
                    if (rv < 0) {
                        cli_out("ERROR: MII Addr %d: "
                                "soc_miim_read failed: %s\n",
                                phy_addr, soc_errmsg(rv));
                        return CMD_FAIL;
                    }
                    /* Assume device doesn't exist in phy if 
                       read back is all 0/1 */
                    if ((phy_data != 0xFFFF) && (phy_data != 0x0000) &&
                        (phy_data != 0x7FFF)&& (phy_data != 0x3FFF)) {
                        cli_out("%4s 0x%02X 0x%02X 0x%02X: 0x%04X\n",
                                "", phy_addr, phy_devad, phy_reg, phy_data);
                    }
                }
            }
        }
    } else {
        cli_out("%4s%5s %3s: %-6s\n", "", "PRTAD", "REG", "VALUE");
        for (phy_addr = phy_addr_start; phy_addr <= phy_addr_end;
             phy_addr++) {
            /* Device ID is in registers 2 and 3 */
            for (phy_reg = 2; phy_reg <= 3; phy_reg++) {
                rv = soc_miim_read(unit, phy_addr, phy_reg, &phy_data);
                if (rv < 0) {
                    cli_out("ERROR: MII Addr %d: soc_miim_read failed: %s\n",
                            phy_addr, soc_errmsg(rv));
                    return CMD_FAIL;
                }
                if ((phy_data != 0xFFFF) && (phy_data != 0x0000)) {
                    cli_out("%4s0x%02X 0x%02x: 0x%04x\n",
                            "", phy_addr, phy_reg, phy_data);
                }
            }
        }
    }
    return CMD_OK;
}

STATIC cmd_result_t
_phy_measure(int unit, args_t *args){

    char *port_str, *cmd_str;
    bcm_pbmp_t          pbmp;
    int is_rx = 1;
    bcm_port_t port, dport, lane;
    uint32  rate_int, rate_remainder;
    bcm_error_t rv =  BCM_E_NONE;
    cmd_result_t result;
    int ilkn_o_fabric_port;
#ifdef BCM_PETRA_SUPPORT
    int ch0_out_int = 0, ch0_out_remainder = 0;
    soc_reg_above_64_val_t reg_val;
    uint32 offset;
#endif /*BCM_PETRA_SUPPORT*/
    

    if ((port_str = ARG_GET(args)) == NULL) {
        cli_out("        Measured rate of chosen port\n");
        cli_out("        measure <ports> [<tx|rx>]\n");
        return CMD_FAIL;
    }

    BCM_PBMP_CLEAR(pbmp);
    if (parse_bcm_pbmp(unit, port_str, &pbmp) < 0) {
        cli_out("ERROR: unrecognized port bitmap: %s\n", port_str);
        return CMD_FAIL;
    }

    if ((cmd_str = ARG_GET(args)) != NULL) {
        if (sal_strcasecmp(cmd_str, "tx") == 0) {
            is_rx = 0;
        } else if (sal_strcasecmp(cmd_str, "rx") == 0) {
            is_rx = 1;
        } else {
            cli_out("ERROR: valid values are tx or rx\n");
            return CMD_FAIL;
        }
    }
                
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {

#ifdef BCM_PETRA_SUPPORT
    if (!IS_SFI_PORT(unit, port)) {
        result = soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset); 
        if (result != CMD_OK) {
            return CMD_FAIL;
        } 
        ilkn_o_fabric_port = SOC_DPP_IS_ILKN_PORT_OVER_FABRIC(unit,offset);
    } else 
#endif 
    {
        ilkn_o_fabric_port = 0;
    }

    if(SOC_IS_DFE(unit) || (SOC_IS_ARAD(unit) && (IS_SFI_PORT(unit, port) || ilkn_o_fabric_port))) {
            lane = port;
            
#ifdef BCM_PETRA_SUPPORT
            if(SOC_IS_DPP(unit)) {
                lane = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
            }
#endif         
            result = diag_dcmn_phy_measure_port(unit, port, is_rx, &rate_int, &rate_remainder);
            if (result != CMD_OK)
            {
                cli_out("diag_dcmn_phy_measure_port failed %d\n", rv);
                return result;
            }
            cli_out("Measured rate for port %1d (QUAD%1d,L-%1d) is %d.%d\n", port, lane / SOC_DCMN_NOF_LINKS_IN_MAC, lane % SOC_DCMN_NOF_LINKS_IN_MAC, rate_int, rate_remainder);

#ifdef BCM_PETRA_SUPPORT
#ifdef BCM_JERICHO_SUPPORT
            if (SOC_IS_JERICHO(unit)) {
                result = jer_phy_measure_fabric_pll(unit, port, &ch0_out_int, &ch0_out_remainder);
                if(CMD_OK != result) return result;

                cli_out("PLL is %d.%d\n", ch0_out_int, ch0_out_remainder);
            } else
#endif /*BCM_JERICHO_SUPPORT*/
            if (SOC_IS_ARAD(unit)) {
                /*SRD0 PLL - ch0 out print*/
                if (READ_ECI_SRD_0_PLL_CONFIGr(unit, reg_val) != BCM_E_NONE) {
                    return CMD_FAIL;
                }
                phy_measure_ch0_calc(reg_val, &ch0_out_int, &ch0_out_remainder);
                cli_out("PLL is %d.%d\n", ch0_out_int, ch0_out_remainder);
            }
#endif /*BCM_PETRA_SUPPORT*/
           
        }
#ifdef BCM_PETRA_SUPPORT
        else if (SOC_IS_ARAD(unit)){
            /* nif port */         
            int one_clk_time_measured_int, one_clk_time_measured_remainder;
            int serdes_freq_int, serdes_freq_remainder;
            uint32 type_of_bit_clk, port_lane;

#ifdef BCM_JERICHO_SUPPORT
            if (SOC_IS_JERICHO(unit)) {
                rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_phy_nif_measure, (unit, port, &type_of_bit_clk, &one_clk_time_measured_int, &one_clk_time_measured_remainder, &serdes_freq_int, &serdes_freq_remainder, &port_lane));
                if(BCM_E_NONE != rv) { return CMD_FAIL; }

                result = jer_phy_measure_nif_pll(unit, port, &ch0_out_int, &ch0_out_remainder);
                if(CMD_OK != result) { return result; }
            } else
#endif /*BCM_JERICHO_SUPPORT*/
            {
                rv = _phy_arad_nif_measure(unit, port, &type_of_bit_clk, &one_clk_time_measured_int, &one_clk_time_measured_remainder, &serdes_freq_int, &serdes_freq_remainder, &port_lane);
                if(BCM_E_NONE != rv) { return CMD_FAIL; }

                /*SRD1 PLL - ch0 out print*/
                if (READ_ECI_SRD_1_PLL_CONFIGr(unit, reg_val) != BCM_E_NONE) {
                   return CMD_FAIL;
                }
                phy_measure_ch0_calc(reg_val, &ch0_out_int, &ch0_out_remainder);
            }

            cli_out("Measured rate for port %1d (WC%1d,L-%1d) is %d.%03d and PLL is %3d.%-3d\n", port, port_lane / SOC_DCMN_NOF_LINKS_IN_MAC,
                   port_lane % SOC_DCMN_NOF_LINKS_IN_MAC, serdes_freq_int, serdes_freq_remainder, ch0_out_int, ch0_out_remainder);

        }
#endif /* BCM_PETRA_SUPPORT */
    }

    return CMD_OK;
}

STATIC cmd_result_t
_phy_cl72(int unit, args_t *args){

    char *port_str, *cmd_str;
    bcm_pbmp_t      pbmp;
    soc_port_t      port, dport;
    int             rv;
    uint32 cl72_configured, cl72_status;

    if ((port_str = ARG_GET(args)) == NULL) {
        cli_out("        Enable|Disable link training (either cl72 or cl93)\n");
        cli_out("        cl72 <ports> <on|off>\n");
        return CMD_FAIL;
    }

    BCM_PBMP_CLEAR(pbmp);
    if (parse_bcm_pbmp(unit, port_str, &pbmp) < 0) {
        cli_out("Error: unrecognized port bitmap: %s\n", port_str);
        return CMD_FAIL;
    }

    cmd_str = ARG_GET(args);

    if(NULL == cmd_str) {
        /*print cl72 status*/
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            rv = bcm_port_phy_control_get(unit, port, BCM_PORT_PHY_CONTROL_CL72, &cl72_configured);
            if (BCM_FAILURE(rv)) {
                cli_out("%s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
            rv = bcm_port_phy_control_get(unit, port, BCM_PORT_PHY_CONTROL_CL72_STATUS , &cl72_status);
            if (BCM_FAILURE(rv)) {
                cli_out("%s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
            cli_out("CL72 configured %d\n", cl72_configured);
            cli_out("CL72 status %d\n", cl72_status);
        }
    } else if (sal_strcasecmp(cmd_str, "off") == 0) {
        /*turn off cl72*/
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            rv = bcm_port_phy_control_set(unit, port, BCM_PORT_PHY_CONTROL_CL72 , 0);
            if (BCM_FAILURE(rv)) {
                cli_out("%s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }
    } else if (sal_strcasecmp(cmd_str, "on") == 0) {
        /*turn on cl72*/
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            rv = bcm_port_phy_control_set(unit, port, BCM_PORT_PHY_CONTROL_CL72 , 1);
            if (BCM_FAILURE(rv)) {
                cli_out("%s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }
    } else {
        cli_out("ERROR: valid values are on or off\n");
        return CMD_FAIL;
    }

    return CMD_OK;
}

char if_dpp_port_usage[] =
    "Parameters: <pbmp>\n\t"
    "[LinkScan=on|off|hw|sw] [SPeed=10|100|1000] [FullDuplex=true|false]\n\t"
    "[SpanningTreeProtocol=Disable|Block|LIsten|LEarn|Forward]\n\t"
    "[VlanFilter=<value>] [Enable=true|false] [InterFace=<interface>]\n\t"
    "[LoopBack=NONE|MAC|PHY]] [AutoNeg=on|off]\n\t"
    "[TxPAUse=on|off] [RxPAUse=on|off]\n\t"
    "If only <ports> is specified, characteristics for that port are\n\t"
    "displayed. <ports> is a standard port bitmap (see \"help pbmp\").\n\t"
    "If AutoNeg is on, SPeed and DUPlex are inoperative.\n\t"
    "If AutoNeg is off, SPeed and DUPlex are the FORCED values.\n\t"
    "SPeed of zero indicates maximum speed.\n\t"
    "LinkScan enables automatic scanning for link changes with updating\n\t"
    "of MAC registers, and EPC_LINK (or equivalent)\n\t"
    "PAUse enables send/receive of pause frames in full duplex mode.\n\t"
    "VlanFilter drops input packets that not tagged with a valid VLAN\n\t"
    "that contains the port.\n"
    ;

/* Iterate thru a port bitmap with the given mask; display info */
STATIC int
_dpp_port_disp_iter(int unit, pbmp_t pbm, pbmp_t pbm_mask, uint32 seen)
{
    bcm_port_info_t info;
    soc_port_t port, dport;
    int r;

    BCM_PBMP_AND(pbm, pbm_mask);
    DPORT_BCM_PBMP_ITER(unit, pbm, dport, port) {

        sal_memset(&info, 0, sizeof(bcm_port_info_t));
        port_info_init(unit, port, &info, seen);

        if ((r = bcm_port_selective_get(unit, port, &info)) < 0) {
            cli_out("Error: Could not get port %s information: %s\n",
                    BCM_PORT_NAME(unit, port), bcm_errmsg(r));
            return (CMD_FAIL);
        }

        disp_port_info(unit, BCM_PORT_NAME(unit, port), port, &info, 
                       IS_ST_PORT(unit, port), seen);
    }

    return CMD_OK;
}


/*
 * Function:
 *  if_port
 * Purpose:
 *  Configure port specific parameters.
 * Parameters:
 *  u   - SOC unit #
 *  a   - pointer to args
 * Returns:
 *  CMD_OK/CMD_FAIL
 */

cmd_result_t
if_dpp_port(int unit, args_t *a)
{
    pbmp_t              pbm;
    bcm_port_config_t   pcfg;
    bcm_port_info_t     *info_all;
    bcm_port_info_t     info_given;
    bcm_port_ability_t  *ability_all;       /* Abilities for all ports */
    bcm_port_ability_t  ability_port;       /* Ability for current port */
    bcm_port_ability_t  ability_given;  
    char                *c;
    int                 r, rv = 0, cmd_rv = CMD_OK;
    soc_port_t          p, dport;
    parse_table_t       pt;
    uint32              seen = 0;
    uint32              parsed = 0, parsed_adj;
    char                pfmt[SOC_PBMP_FMT_LEN];

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL) {
        return(CMD_USAGE);
    }

    info_all = sal_alloc(SOC_MAX_NUM_PORTS * sizeof(bcm_port_info_t), 
                         "if_port");

    if (info_all == NULL) {
        cli_out("Insufficient memory.\n");
        return CMD_FAIL;
    }

    ability_all = sal_alloc(SOC_MAX_NUM_PORTS * sizeof(bcm_port_ability_t), 
        "if_port");

    if (ability_all == NULL) {
        cli_out("Insufficient memory.\n");
        sal_free(info_all);
        return CMD_FAIL;
    }

    sal_memset(&info_given, 0, sizeof(bcm_port_info_t));
    sal_memset(info_all, 0, SOC_MAX_NUM_PORTS * sizeof(bcm_port_info_t));
    sal_memset(&ability_given, 0, sizeof(bcm_port_ability_t));
    sal_memset(&ability_port, 0, sizeof(bcm_port_ability_t));
    sal_memset(ability_all, 0, SOC_MAX_NUM_PORTS * sizeof(bcm_port_ability_t));

    if (parse_bcm_pbmp(unit, c, &pbm) < 0) {
        cli_out("%s: Error: unrecognized port bitmap: %s\n",
                ARG_CMD(a), c);
        sal_free(info_all);
        sal_free(ability_all);
        return CMD_FAIL;
    }

    BCM_PBMP_AND(pbm, pcfg.port);

    if (BCM_PBMP_IS_NULL(pbm)) {
        ARG_DISCARD(a);
        sal_free(info_all);
        sal_free(ability_all);
        return CMD_OK;
    }

    if (ARG_CNT(a) == 0) {

        seen = BCM_PORT_ATTR_ALL_MASK;
        
        if (SOC_IS_ARAD(unit)) {
#ifdef BCM_PETRA_SUPPORT
            seen = _BCM_DPP_PORT_ATTRS;
#endif
        }
        else if (SOC_IS_DFE(unit)) {
#ifdef BCM_DFE_SUPPORT
            seen = _BCM_DFE_PORT_ATTRS;
#endif
        }

    } else {
        /*
         * Otherwise, arguments are given.  Use them to determine which
         * properties need to be gotten/set.
         *
         * Probe and detach, hidden commands.
         */
        if (!sal_strcasecmp(_ARG_CUR(a), "detach")) {
            pbmp_t detached;
            bcm_port_detach(unit, pbm, &detached);
            cli_out("Detached port bitmap %s\n", SOC_PBMP_FMT(detached, pfmt));
            ARG_GET(a);
            sal_free(info_all);
            sal_free(ability_all);
            return CMD_OK;
        } else if ((!sal_strcasecmp(_ARG_CUR(a), "probe")) ||
                   (!sal_strcasecmp(_ARG_CUR(a), "attach"))) {
            pbmp_t probed;
            bcm_port_probe(unit, pbm, &probed);
            cli_out("Probed port bitmap %s\n", SOC_PBMP_FMT(probed, pfmt));
            ARG_GET(a);
            sal_free(info_all);
            sal_free(ability_all);
            return CMD_OK;
        } 

        if (!sal_strcmp(_ARG_CUR(a), "=")) {
            /*
             * For "=" where the user is prompted to enter all the parameters,
             * use the parameters from the first selected port as the defaults.
             */
            if (ARG_CNT(a) != 1) {
                sal_free(info_all);
                sal_free(ability_all);
                return CMD_USAGE;
            }
            DPORT_BCM_PBMP_ITER(unit, pbm, dport, p) {
                break;    /* Find first port in bitmap */
            }

            port_info_init(unit,p,&info_given,0);
            if ((rv = bcm_port_info_get(unit, p, &info_given)) < 0) {
                cli_out("%s: Error: Failed to get port info\n", ARG_CMD(a));
                sal_free(info_all);
                sal_free(ability_all);
                return CMD_FAIL;
            }
        } else {
            DPORT_BCM_PBMP_ITER(unit, pbm, dport, p) {
                break;    /* Find first port in bitmap */
            }
            port_info_init(unit,p,&info_given,0);
        }

        /*
         * Parse the arguments.  Determine which ones were actually given.
         */
        port_parse_setup(unit, &pt, &info_given);

        if (parse_arg_eq(a, &pt) < 0) {
            parse_arg_eq_done(&pt);
            sal_free(info_all);
            sal_free(ability_all);
            return(CMD_FAIL);
        }

        /* Translate port_info into port abilities. */

        if (ARG_CNT(a) > 0) {
            cli_out("%s: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            sal_free(info_all);
            sal_free(ability_all);
            return(CMD_FAIL);
        }

        /*
         * Find out what parameters specified.  Record values specified.
         */
        port_parse_mask_get(&pt, &seen, &parsed);

        parse_arg_eq_done(&pt);
    }

    if (seen && parsed) {
        cli_out("%s: Cannot get and set "
                "port properties in one command\n", ARG_CMD(a));
        sal_free(info_all);
        sal_free(ability_all);
        return CMD_FAIL;
    } else if (seen) { /* Show selected information */
        cli_out("%s: Status (* indicates PHY link up)\n", ARG_CMD(a));
        /* Display the information by port type */
#define _call_pdi(unit, p, mp, s) \
        if (_dpp_port_disp_iter(unit, p, mp, s) != CMD_OK) { \
             sal_free(info_all); \
             sal_free(ability_all); \
             return CMD_FAIL; \
        }
        _call_pdi(unit, pbm, pcfg.fe, seen);
        _call_pdi(unit, pbm, pcfg.ge, seen);
        _call_pdi(unit, pbm, pcfg.xe, seen);
        _call_pdi(unit, pbm, pcfg.ce, seen);
        _call_pdi(unit, pbm, pcfg.hg, seen);
        _call_pdi(unit, pbm, pcfg.sfi, seen);
        sal_free(info_all);
        sal_free(ability_all);
        return(CMD_OK);
    }

    /* Some set information was given */

    if (parsed & BCM_PORT_ATTR_LINKSCAN_MASK) {
        /* Map ON --> S/W, OFF--> None */
        if (info_given.linkscan > 2) {
            info_given.linkscan -= 3;
        }
    }

    parsed_adj = parsed;
    if (parsed & (BCM_PORT_ATTR_SPEED_MASK | BCM_PORT_ATTR_DUPLEX_MASK)) {
        parsed_adj |= BCM_PORT_ATTR_SPEED_MASK | BCM_PORT_ATTR_DUPLEX_MASK;
    }

    /*
     * Retrieve all requested port information first, then later write
     * back all port information.  That prevents a problem with loopback
     * cables where setting one port's info throws another into autoneg
     * causing it to return info in flux (e.g. suddenly go half duplex).
     */
    DPORT_BCM_PBMP_ITER(unit, pbm, dport, p) {
        port_info_init(unit, p, &info_all[p], parsed_adj);
        if ((r = bcm_port_selective_get(unit, p, &info_all[p])) < 0) {
            cli_out("%s: Error: Could not get port %s information: %s\n",
                    ARG_CMD(a), BCM_PORT_NAME(unit, p), bcm_errmsg(r));
            sal_free(info_all);
            sal_free(ability_all);
            return (CMD_FAIL);
        }
    }

    /*
     * Loop through all the specified ports, changing whatever field
     * values were actually given.  This avoids copying unaffected
     * information from one port to another and prevents having to
     * re-parse the arguments once per port.
     */
    DPORT_BCM_PBMP_ITER(unit, pbm, dport, p) {
        if ((rv = bcm_port_speed_max(unit, p, &info_given.speed_max)) < 0) {
            cli_out("port parse: Error: Could not get port %s max speed: %s\n",
                    BCM_PORT_NAME(unit, p), bcm_errmsg(rv));
            continue;
        }
        /* bcm_port_ability_local_get() is not implemented for QSGMII ports. */
        if (!SOC_IS_DFE(unit) && !IS_SFI_PORT(unit, p) && !IS_QSGMII_PORT(unit, p)){
            if ((rv = bcm_port_ability_local_get(unit, p, &info_given.port_ability)) < 0) {
                cli_out("port parse: Error: Could not get port %s ability: %s\n",
                        BCM_PORT_NAME(unit, p), bcm_errmsg(rv));
                continue;
            }
            if ((rv = soc_port_ability_to_mode(&info_given.port_ability,
                                               &info_given.ability)) < 0) {
                cli_out("port parse: Error: Could not transform port %s ability to mode: %s\n",
                        BCM_PORT_NAME(unit, p), bcm_errmsg(rv));
                continue;
            }
        }
        if ((r = port_parse_port_info_set(parsed,
                                          &info_given, &info_all[p])) < 0) {
            cli_out("%s: Error: Could not parse port %s info: %s\n",
                    ARG_CMD(a), BCM_PORT_NAME(unit, p), bcm_errmsg(r));
            cmd_rv = CMD_FAIL;
            continue;
        }

        /* If AN is on, do not set speed, duplex, pause */
        if (info_all[p].autoneg) {
            info_all[p].action_mask &= ~BCM_PORT_AN_ATTRS;
        }

#ifdef BCM_PETRA_SUPPORT
        if(SOC_IS_ARAD(unit) && !BCM_PBMP_MEMBER(pcfg.sfi, p)) {
            uint32              sw_db_flags;

            if((r = soc_port_sw_db_flags_get(unit, p, &sw_db_flags)) < 0){
                cli_out("%s: Error: Could not get flags port %s info: %s\n",
                        ARG_CMD(a), BCM_PORT_NAME(unit, p), bcm_errmsg(r));
                cmd_rv = CMD_FAIL;
                continue;
            }

            if(SOC_PORT_IS_STAT_INTERFACE(sw_db_flags)) {
                info_all[p].action_mask &= ~(BCM_PORT_ATTR_LINKSCAN_MASK | BCM_PORT_ATTR_LINKSTAT_MASK);
            }
        }
#endif

        if ((r = bcm_port_selective_set(unit, p, &info_all[p])) < 0) {
            cli_out("%s: Error: Could not set port %s information: %s\n",
                    ARG_CMD(a), BCM_PORT_NAME(unit, p), bcm_errmsg(r));
            cmd_rv = CMD_FAIL;
            continue;
        }
    }

    sal_free(info_all);
    sal_free(ability_all);

    return(cmd_rv);
}
