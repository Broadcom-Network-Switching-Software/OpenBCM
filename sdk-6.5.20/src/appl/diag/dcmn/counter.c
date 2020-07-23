/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Counter CLI support
 */



#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#include <appl/diag/system.h>

#include <bcm/stat.h>
#include <bcm/error.h>
#include <soc/mem.h>
#include <soc/types.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/drv.h>
#include <bcm_int/dpp/counters.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/ARAD/arad_nif.h>
#endif
/*
 * Data structure for saving the previous value of counters so we can
 * tell which counters that have changed since last shown.
 */
static uint64 *dcmn_counter_val[SOC_MAX_NUM_DEVICES];
static int prev_dcmn_counter_interval[SOC_MAX_NUM_DEVICES];

/*
 * Deinit dcmn_counter_val, if it was allocated
 * in dpp_counter_val_set(), so we won't have memory leak, 
 * and if a different device will be loaded then new allocation
 * will be done otherwise we might have memory override.
 */
void
dcmn_counter_val_deinit(int unit)
{
    if (dcmn_counter_val[unit] != NULL) {
        sal_free(dcmn_counter_val[unit]);
        dcmn_counter_val[unit] = NULL;
    }
}

void
dpp_counter_val_set(int unit, soc_port_t port, soc_reg_t ctr_reg,
                    int ar_idx, uint64 val)
{
    int     ind, n;
 
    if (dcmn_counter_val[unit] == NULL) {
                n = SOC_CONTROL(unit)->counter_n32 + SOC_CONTROL(unit)->counter_n64 +
            SOC_CONTROL(unit)->counter_n64_non_dma;
        dcmn_counter_val[unit] = sal_alloc(n * sizeof (uint64), "save_ctrs");

        if (dcmn_counter_val[unit] == NULL) {
            return;
        }

        sal_memset(dcmn_counter_val[unit], 0, n * sizeof (uint64));
    }
    if (ctr_reg == INVALIDr) {
        /*set all port counters*/
        /*get number of counters per port*/
         n = SOC_CONTROL(unit)->counter_perport;
         /*set value*/
         sal_memset(dcmn_counter_val[unit] + port*n, COMPILER_64_LO(val), sizeof(uint64)*n);
         return;
    }
    ind = soc_counter_idx_get(unit, ctr_reg, ar_idx, port);

    if (ind != -1) {
        dcmn_counter_val[unit][ind] = val; 
    }
}

void
dpp_counter_val_get(int unit, soc_port_t port, soc_reg_t ctr_reg,
                int ar_idx, uint64 *val)
{
    int     ind;

    if (dcmn_counter_val[unit] == NULL) {
        COMPILER_64_SET(*val, 0, 0);
    } else {
        ind = soc_counter_idx_get(unit, ctr_reg, ar_idx, port);

        if (ind < 0) {
            COMPILER_64_SET(*val, 0, 0);
        } else {
            *val = dcmn_counter_val[unit][ind];
        }
    }
}

STATIC int
dpp_is_counter_error_discard(int unit, soc_reg_t reg)
{
    int rv = 0;

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        switch (reg) {
        case ARAD_NIF_RX_ERR_PACKETS:
        case ARAD_NIF_TX_ERR_PACKETS:
            rv = 1;
            break;
        default:
            break;
        }
    }
#endif /*BCM_PETRA_SUPPORT*/


    return rv;
}

/*
 * do_show_dpp_counters
 *
 *   Displays counters for specified register(s) and port(s).
 *
 *      SHOW_CTR_HEX displays values in hex instead of decimal.
 *
 *      SHOW_CTR_RAW displays only the counter value, not the register
 *      name or delta.
 *
 *      SHOW_CTR_NOT_CHANGED: includes counters that have not changed since
 *      the last call to do_show_dpp_counters().
 *
 *   NOTE: to get any output, you need at least one of the above two flags.
 *
 *      SHOW_CTR_ZERO: includes counters that are zero.
 *
 *      SHOW_CTR_NOT_ZERO: includes counters that are not zero.
 *
 *   NOTE: to get any output, you need at least one of the above two flags.
 *
 *   SHOW_CTR_ED:
 *      Show only those registers marked as Error/Discard.
 *
 *   Columns will remain lined up properly until the following maximum
 *   limits are exceeded:
 *
 *      Current count: 99,999,999,999,999,999
 *      Increment: +99,999,999,999,999
 *      Rate: 999,999,999,999/s
 */
/*
 * do_show_dpp_counters
 *
 *   Displays counters for specified register(s) and port(s).
 *
 *    SHOW_CTR_HEX displays values in hex instead of decimal.
 *
 *    SHOW_CTR_RAW displays only the counter value, not the register
 *    name or delta.
 *
 *    SHOW_CTR_NOT_CHANGED: includes counters that have not changed since
 *    the last call to do_show_dpp_counters().
 *
 *   NOTE: to get any output, you need at least one of the above two flags.
 *
 *    SHOW_CTR_ZERO: includes counters that are zero.
 *
 *    SHOW_CTR_NOT_ZERO: includes counters that are not zero.
 *
 *   NOTE: to get any output, you need at least one of the above two flags.
 *
 *   SHOW_CTR_ED:
 *      Show only those registers marked as Error/Discard.
 *
 *   Columns will remain lined up properly until the following maximum
 *   limits are exceeded:
 *
 *    Current count: 99,999,999,999,999,999
 *    Increment: +99,999,999,999,999
 *    Rate: 999,999,999,999/s
 */

void
do_show_dpp_counter(int unit, soc_port_t port, soc_reg_t ctr_reg,
                int ar_idx, int flags)
{
    soc_counter_non_dma_t *non_dma;
    uint64        val, prev_val, diff, rate;
    int            changed;
    int         rv;
    int                 is_ed_cntr = FALSE, is_mmu_status = FALSE;
    int            tabwidth = soc_property_get(unit, spn_DIAG_TABS, 8);
    int            commachr = soc_property_get(unit, spn_DIAG_COMMA, ',');

    /* making sure that if we use val uninitialized, it'll contain all ones and we can catch it */
    COMPILER_64_ALLONES(val);
    /*dma counters support*/
    if (ctr_reg >= SOC_COUNTER_NON_DMA_START &&
        ctr_reg < SOC_COUNTER_NON_DMA_END) {
        if (SOC_CONTROL(unit)->counter_non_dma == NULL) {
            return;
        }
        is_ed_cntr = FALSE;
        is_mmu_status = FALSE;
        non_dma = &SOC_CONTROL(unit)->
            counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START];
        if (non_dma->flags &
            (_SOC_COUNTER_NON_DMA_PEAK | _SOC_COUNTER_NON_DMA_CURRENT)) {
            is_mmu_status = TRUE;
        }
        if (ar_idx < 0) {
            for (ar_idx = 0; ar_idx < non_dma->entries_per_port; ar_idx++) {
                do_show_dpp_counter(unit, port, ctr_reg, ar_idx, flags);
            }
            return;
        }
    } else if (!_SOC_CONTROLLED_COUNTER_USE(unit, port)) {
        /*regular counters -  non petrab*/
        is_ed_cntr = SOC_REG_INFO(unit, ctr_reg).flags & SOC_REG_FLAG_ED_CNTR;
        is_mmu_status = FALSE;

        if (!(SOC_REG_INFO(unit, ctr_reg).flags)) {
            ar_idx = 0;
        } else {
            if (ar_idx < 0) { /* Set all elts of array */
                for (ar_idx = 0; ar_idx < SOC_REG_INFO(unit, ctr_reg).numels;
                     ar_idx++) {
                    do_show_dpp_counter(unit, port, ctr_reg, ar_idx, flags);
                }
                return;
            }
        }
    } else {
        is_ed_cntr = dpp_is_counter_error_discard(unit, ctr_reg);
    }

   if(ctr_reg >= NUM_SOC_REG) {
        return;
    }

    /*get current counter value*/
    if (_SOC_CONTROLLED_COUNTER_USE(unit, port)) {
            /*controlled counters support*/
         if (SOC_CONTROL(unit)->counter_interval != 0 && (COUNTER_IS_COLLECTED(SOC_CONTROL(unit)->controlled_counters[ctr_reg]))) {
            rv = soc_counter_get(unit, port, ctr_reg, 0, &val); /*counter is collected by counter thread*/
        } else {
            /*counter isn't collected by counter thread*/
            if (SOC_CONTROL(unit)->controlled_counters[ctr_reg].controlled_counter_f != NULL) {
                rv = SOC_CONTROL(unit)->controlled_counters[ctr_reg].controlled_counter_f(unit, SOC_CONTROL(unit)->controlled_counters[ctr_reg].counter_id, port, &val, NULL);
            } else {
                return;
            }

        }
        if (rv != BCM_E_NONE) {
            return;
        }
    } else if (SOC_REG_IS_VALID(unit, ctr_reg) && SOC_REG_IS_COUNTER(unit, ctr_reg)) {
        if (SOC_CONTROL(unit)->counter_interval == 0 /*counter thread is off*/) { 
            rv = soc_reg_get(unit, ctr_reg, port, 0, &val);
        } else {
            rv = soc_counter_get(unit, port, ctr_reg, 
                                         0, &val);
        }
        if (rv != BCM_E_NONE) {
            return;
        }
    } else {
       return;
    }

    /*get prev counter value and rate*/
    if (_SOC_CONTROLLED_COUNTER_USE(unit, port) && (SOC_CONTROL(unit)->counter_interval == 0 || !(COUNTER_IS_COLLECTED(SOC_CONTROL(unit)->controlled_counters[ctr_reg])))) {
        /*controlled counters support while counter thread is off*/
        /*counter are clear on read therefore prev val and rate are irrelevant*/
        COMPILER_64_ZERO(prev_val);
        COMPILER_64_ZERO(rate);
   } else {
        dpp_counter_val_get(unit, port, ctr_reg, ar_idx, &prev_val);
        soc_counter_get_rate(unit, port, ctr_reg, ar_idx, &rate);
   }


    diff = val;
    COMPILER_64_SUB_64(diff, prev_val);

    if (COMPILER_64_IS_ZERO(diff)) {
        changed = 0;
    } else {
        dpp_counter_val_set(unit, port, ctr_reg, ar_idx, val);
        changed = 1;
    }

    /*print result*/
    if ((changed || (!changed && (flags & SHOW_CTR_SAME))) &&                                       /*changed flag*/
        ((COMPILER_64_IS_ZERO(val) && (flags & SHOW_CTR_Z)) ||  !COMPILER_64_IS_ZERO(val)) &&       /*zero flag*/
        ((!(flags & SHOW_CTR_ED) || ((flags & SHOW_CTR_ED) && is_ed_cntr))) &&                      /*error flag*/
        ((!(flags & SHOW_CTR_MS) || ((flags & SHOW_CTR_MS) && is_mmu_status)))                      /*ms flag*/
        ) 
    {
        /* size of line must be equal or more then sum of cname, buf_val, 
          buf_diff and buf_rate. */
        /* The size of tabby must be greater than size of line */

        char    tabby[130], line[120];
        char    cname[50];
        char    buf_val[32], buf_diff[32], buf_rate[32];

        if (_SOC_CONTROLLED_COUNTER_USE(unit, port)) {
            /*controlled counters support*/
            if (&SOC_CONTROL(unit)->controlled_counters != NULL) {
                            sal_sprintf(cname, "%-33s %s :", SOC_CONTROL(unit)->controlled_counters[ctr_reg].short_cname,
                                           SOC_PORT_NAME(unit, port));
            } 
        } else {
            /* DMA Counters */
            if (strlen(SOC_REG_NAME(unit, ctr_reg)) > 30) {
                sal_memcpy(cname, SOC_REG_NAME(unit, ctr_reg), 13);
                /* Truncate Name to fit in line */
                if (SOC_REG_INFO(unit, ctr_reg).flags & SOC_REG_FLAG_ARRAY) {
                    sal_sprintf(&cname[30], "(%d).%s", ar_idx, SOC_PORT_NAME(unit, port));
                } else {
                    sal_sprintf(&cname[33], ".%s", SOC_PORT_NAME(unit, port));
                }
            } else {
                if (SOC_REG_INFO(unit, ctr_reg).flags & SOC_REG_FLAG_ARRAY) {
                    sal_sprintf(cname, "%s(%d).%s", SOC_REG_NAME(unit, ctr_reg),
                                       ar_idx, SOC_PORT_NAME(unit, port));
                } else {
                    sal_sprintf(cname, "%s.%s", SOC_REG_NAME(unit, ctr_reg),
                                               SOC_PORT_NAME(unit, port));
                }
            }
        }

    if (flags & SHOW_CTR_RAW) {
       if (flags & SHOW_CTR_HEX) {
            sal_sprintf(line, "0x%08x%08x",
                COMPILER_64_HI(val), COMPILER_64_LO(val));
       } else {
            format_uint64_decimal(buf_val, val, 0);
            sal_sprintf(line, "%s", buf_val);
       }
    } else {
       if (flags & SHOW_CTR_HEX) {
                /* is_mnu_status may be changed from FALSE to TRUE */
                /* coveruty[dead_error_begin : FALSE] */
                if (is_mmu_status) {
                    sal_sprintf(line, "%-18s: 0x%08x%08x",
                                cname,
                                COMPILER_64_HI(val), COMPILER_64_LO(val));
                } else {
                    sal_sprintf(line, "%-18s: 0x%08x%08x +0x%08x%08x 0x%08x%08x/s",
                                cname,
                                COMPILER_64_HI(val), COMPILER_64_LO(val),
                                COMPILER_64_HI(diff), COMPILER_64_LO(diff),
                                COMPILER_64_HI(rate), COMPILER_64_LO(rate));
                }
         } else {
                if (is_mmu_status) {
                    /* is_mnu_status may be changed from FALSE to TRUE */
                    /* coverity[dead_error_begin : FALSE] */
                    format_uint64_decimal(buf_val, val, commachr);
                    sal_sprintf(line, "%-24s:%22s", cname, buf_val);
                } else {
                    format_uint64_decimal(buf_val, val, commachr);
                    buf_diff[0] = '+';
                    format_uint64_decimal(buf_diff + 1, diff, commachr);

                    sal_sprintf(line, "%-24s:%22s%20s",
                        cname, buf_val, buf_diff);
                    if (!COMPILER_64_IS_ZERO(rate)) {
                       format_uint64_decimal(buf_rate, rate, commachr);
                        sal_sprintf(line + strlen(line), "%16s/s", buf_rate);
                    }
                }
          }
    }

    /*
    * Display is tremendously faster with tabify.
    * Set diag_tab property to desired value, or 0 to turn it off.
    */

    tabify_line(tabby, line, tabwidth);

    cli_out("%s\n", tabby);
    }
}

static char        *_stat_names[] = BCM_STAT_NAME_INITIALIZER;

/* 
 * USE: print a table of snmmp counters of ports included in pbmp using bcm_stat_get api.
 * Supported Flags: SHOW_CTR_Z, SHOW_CTR_HEX
 */
cmd_result_t
cmd_dpp_counters_table_print(int unit, bcm_pbmp_t pbmp,bcm_stat_val_t *arr_snmp, int arr_count, int flags){
    bcm_stat_val_t counter;
    bcm_port_t port;
    uint64 val64;
    int rv, do_print;
    int not_supported = 0;

    cli_out("Port ||");
    for (counter=0; counter <arr_count; counter++) {
        cli_out(" %25s |", _stat_names[arr_snmp[counter]]);
    }
    cli_out("\n");
    cli_out("-------");
    for (counter=0; counter <arr_count; counter++) {
        cli_out("----------------------------");
    }
    cli_out("\n");

    PBMP_ITER(pbmp, port) {
        do_print = 0;

        if ((flags & SHOW_CTR_Z) == 0) {
            for (counter=0; counter<arr_count; counter++) {
                COMPILER_64_SET(val64, 0, 0);
                /*Check if the counter is supported*/
                if (!((IS_IL_PORT(unit, port) && arr_snmp[counter] ==  snmpIfHCInUcastPkts) ||
                    (IS_IL_PORT(unit, port) && arr_snmp[counter] ==  snmpIfHCOutUcastPkts)))
                {
                    rv = bcm_stat_get(unit, port, arr_snmp[counter], &val64);
                    if (rv != BCM_E_NONE) {
                        return CMD_FAIL;
                    }   
                }
                if (!COMPILER_64_IS_ZERO(val64)) {
                    do_print = 1;
                    break;
                }
            }
        } else {
            do_print = 1;
        }
        if (do_print) {
            cli_out("%4d ||", port);
            for (counter=0; counter<arr_count; counter++) {
                not_supported = 0;
                COMPILER_64_SET(val64, 0, 0);
                if ((IS_IL_PORT(unit, port) && arr_snmp[counter] ==  snmpIfHCInUcastPkts) ||
                    (IS_IL_PORT(unit, port) && arr_snmp[counter] ==  snmpIfHCOutUcastPkts))
                {
                    not_supported = 1;
                } else {
                    rv = bcm_stat_get(unit, port, arr_snmp[counter], &val64);
                    if (rv != BCM_E_NONE && rv != BCM_E_UNAVAIL) {
                        return CMD_FAIL;
                    }
                }
                if (not_supported) {
                    cli_out("|                      N/A |");
                } else if (flags & SHOW_CTR_HEX) {
                    cli_out("   0x%08x%08x    |", COMPILER_64_HI(val64), COMPILER_64_LO(val64));
                } else  {
                    char        buf_val[32];
                    int            commachr = soc_property_get(unit, spn_DIAG_COMMA, ',');

                    format_uint64_decimal(buf_val, val64, commachr);  
                    cli_out(" %25s |", buf_val);
                }
            }
            cli_out("\n");  
        }
    }

    return CMD_OK;
}
/*
 * USE: print counters information of ports included at pbmp according to ctr_reg and/or flags 
 * INPUT: 
 *      unit - unit #.
 *      ctr_reg - INVALIDr in case of all
 *      pbmp - ports bitmap.
 *      flags - supported flags:
 *                  SHOW_COMPACT - print table of main counters.
 *                  SHOW_CTR_Z, SHOW_CTR_HEX.
 */
int
do_show_dpp_counters(int unit, soc_reg_t ctr_reg, soc_pbmp_t pbmp, int flags)
{
    soc_control_t   *soc = SOC_CONTROL(unit);
    
    SOC_PBMP_AND(pbmp, soc->counter_pbmp);

    if (flags & SHOW_COMPACT){
        /*compact table support*/

        bcm_pbmp_t pbmp_fabric;
        bcm_pbmp_t pbmp_rest;
        int rv;
        /*different table for nif ports and fabric ports*/
        BCM_PBMP_ASSIGN(pbmp_fabric, pbmp);
        BCM_PBMP_AND(pbmp_fabric, PBMP_SFI_ALL(unit));

        BCM_PBMP_NEGATE(pbmp_rest, PBMP_SFI_ALL(unit));
        BCM_PBMP_AND(pbmp_rest, pbmp);

        if (BCM_PBMP_NOT_NULL(pbmp_rest)) {
            bcm_stat_val_t arr_snmp_rx[] =  { snmpIfHCInUcastPkts, snmpIfInNUcastPkts, snmpIfInErrors, snmpEtherStatsRXNoErrors};
            bcm_stat_val_t arr_snmp_tx[] =  { snmpIfHCOutUcastPkts, snmpIfOutNUcastPkts, snmpIfOutErrors, snmpEtherStatsTXNoErrors};

            cli_out("NIF Ports\n");
            cli_out("---------\n\n");
            
            cli_out("RX\n");
            cli_out("---\n");
            rv = cmd_dpp_counters_table_print(unit, pbmp_rest, arr_snmp_rx, PARSE_ENTRIES(arr_snmp_rx), flags);
            if (rv != CMD_OK) {
                return rv;
            }

            cli_out("TX\n");
            cli_out("---\n");
            rv = cmd_dpp_counters_table_print(unit, pbmp_rest, arr_snmp_tx, PARSE_ENTRIES(arr_snmp_tx), flags);
            if (rv != CMD_OK) {
                return rv;
            }
        }

        if (BCM_PBMP_NOT_NULL(pbmp_fabric)) {
            bcm_stat_val_t arr_snmp_rx[] =  { snmpBcmRxControlCells, snmpBcmRxDataCells, snmpBcmRxCrcErrors};
            bcm_stat_val_t arr_snmp_tx[] =  { snmpBcmTxControlCells, snmpBcmTxDataCells};

            cli_out("Fabric Ports\n");
            cli_out("------------\n\n");
            
            cli_out("RX\n");
            cli_out("---\n");
            rv = cmd_dpp_counters_table_print(unit, pbmp_fabric, arr_snmp_rx, PARSE_ENTRIES(arr_snmp_rx), flags);
            if (rv != CMD_OK) {
                return rv;
            }

            cli_out("TX\n");
            cli_out("---\n");
            rv = cmd_dpp_counters_table_print(unit, pbmp_fabric, arr_snmp_tx, PARSE_ENTRIES(arr_snmp_tx), flags);
            if (rv != CMD_OK) {
                return rv;
            }
        }

    } else {
        /*full (non compact) support*/
        soc_control_t    *soc = SOC_CONTROL(unit);
        soc_port_t        port;
        int            i, index;
        soc_cmap_t          *cmap;
        soc_reg_t           reg;
        int                 numregs;
        char        pfmt[SOC_PBMP_FMT_LEN];

        PBMP_ITER(pbmp, port) {
            if (SOC_IS_ARAD(unit) && !IS_SFI_PORT(unit, port)) {
                bcm_port_t port_base;
                int phy_port;

                /*get port base for channelized case */
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                port_base = SOC_INFO(unit).port_p2l_mapping[phy_port];

                cmap = soc_port_cmap_get(unit, port_base);
            } else {    
                cmap = soc_port_cmap_get(unit, port);
            }
            
            if (cmap == NULL) {
                /*cmap mechanism is unavailable ==> using controlled counter mechanism*/
                for (index = 0; soc->controlled_counters[index].controlled_counter_f != NULL; index++) {
#ifdef BCM_PETRA_SUPPORT
                    if (SOC_IS_ARAD(unit)) {
                        soc_error_t rv;
                        int enable = 0, printable = 0;
                        rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_stat_controlled_counter_enable_get,(unit, port, index, &enable, &printable));
                        if (rv != (soc_error_t) BCM_E_NONE) {
                            return CMD_FAIL;
                        }
                        if (!enable || !printable) {
                            continue;
                        }
                    }
#endif  
                    if (soc->controlled_counters[index].flags == _SOC_CONTROLLED_COUNTER_FLAG_INVALID)
                    {
                        continue;
                    }
                    do_show_dpp_counter(unit, port, index, -1, flags);
                }

                continue;
            }
            numregs = cmap->cmap_size;

            if (ctr_reg != INVALIDr) {
                /*print specific ctr_reg*/
                do_show_dpp_counter(unit, port, ctr_reg, -1, flags);
            } else {
                /*print all counter regs*/
                for (i = 0; i < numregs; i++) {
                    reg = cmap->cmap_base[i].reg;
                    if (reg != INVALIDr) {
                        do_show_dpp_counter(unit, port, reg,
                                        cmap->cmap_base[i].index, flags);
                    }
                }
                for (i = SOC_COUNTER_NON_DMA_START; i < SOC_COUNTER_NON_DMA_END;
                     i++) {
                    do_show_dpp_counter(unit, port, i, -1, flags);
                }
            }
        }
        
        SOC_PBMP_REMOVE(pbmp, soc->counter_pbmp); 

        if (soc->counter_interval == 0) {
            cli_out("NOTE: counter collection is not running\n");
        } else if (SOC_PBMP_NOT_NULL(pbmp)) {
            cli_out("NOTE: counter collection is not active for ports %s\n", SOC_PBMP_FMT(pbmp, pfmt));
        }

    }



    return CMD_OK;
}

/*
 * Counter collection configuration
 */


char cmd_dpp_counter_usage[] =
    "\nParameters: [on] [off] [sync] [Interval=<usec>]\n\t"
    "\t[PortBitMap=<pbm>]\n\t"
    "Starts the counter collection task running every <usec>\n\t"
    "microseconds.  The task tallies software counters based on hardware\n\t"
    "values and must run often enough to avoid counter overflow.\n\t"
    "If <interval> is 0, stops the task.  If <interval> is omitted, prints\n\t"
    "the current INTERVAL.  sync reads in all the counters to synchronize\n\t"
    "'show counters' for the first time, and must be used alone.\n";

/* Counter collection configuration */
cmd_result_t
cmd_dpp_counter(int unit, args_t *a)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    soc_port_t          p;
    soc_pbmp_t          pbmp;
    int                 usec, r;
    parse_table_t       pt;
    uint32              flags;
    int                 sync = 0;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }


    flags = soc->counter_flags;
    usec = soc->counter_interval;
    SOC_PBMP_ASSIGN(pbmp, soc->counter_pbmp);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Interval",    PQ_DFL|PQ_INT,
                    (void *)( 0), &usec, NULL);
    parse_table_add(&pt, "PortBitMap",  PQ_DFL|PQ_PBMP,
                    (void *)(0), &pbmp, NULL);

    if (!ARG_CNT(a)) {                  /* Display settings */
        cli_out("Current settings:\n");
        parse_eq_format(&pt);
        parse_arg_eq_done(&pt);
        return(CMD_OK);
    }

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return(CMD_FAIL);
    }
    parse_arg_eq_done(&pt);

    if (ARG_CNT(a) > 0 && !sal_strcasecmp(_ARG_CUR(a), "on")) {
        usec = prev_dcmn_counter_interval[unit];
        ARG_NEXT(a);
    }

    if (ARG_CNT(a) > 0 && !sal_strcasecmp(_ARG_CUR(a), "off")) {
        ARG_NEXT(a);
        usec = 0;
    }

    if (ARG_CNT(a) > 0 && !sal_strcasecmp(_ARG_CUR(a), "sync")) {
        ARG_NEXT(a);
        sync = 1;
    }

    if (sync) {
        /* Sync driver info to HW */
        if ((r = soc_counter_sync(unit)) < 0) {
            cli_out("%s: Error: Could not sync counters: %s\n",
                    ARG_CMD(a), soc_errmsg(r));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    if (soc_feature(unit, soc_feature_cpuport_stat_dma)) {
        soc_pbmp_t temp_pbmp;
        SOC_PBMP_CLEAR(temp_pbmp);
        SOC_PBMP_OR(temp_pbmp, PBMP_PORT_ALL(unit));
        SOC_PBMP_OR(temp_pbmp, PBMP_CMIC(unit));
        SOC_PBMP_AND(pbmp, temp_pbmp);
    } else {
        SOC_PBMP_AND(pbmp, PBMP_PORT_ALL(unit));
    }

    if (SOC_IS_ARAD(unit)) {
        /*manage ports bitmap*/
        bcm_port_t p;
        bcm_pbmp_t ports_to_remove;
        bcm_port_t port_base;
        int phy_port;

        SOC_PBMP_CLEAR(ports_to_remove);
        /*remove channelized ports which is not port base*/
        PBMP_ITER(pbmp, p) {
            char *prop;
            /*get port base for channelized case */
            phy_port = SOC_INFO(unit).port_l2p_mapping[p];
            port_base = SOC_INFO(unit).port_p2l_mapping[phy_port];
            if (p != port_base /*nchannelized and not port_Base*/) {
                prop = soc_property_get_str(unit, spn_ILKN_COUNTERS_MODE);
                if ((IS_IL_PORT(unit, p)) && (sal_strcmp(prop, "PACKET_PER_CHANNEL")))
                {
                    continue;
                }
                else
                {
                    SOC_PBMP_PORT_ADD(ports_to_remove, p);
                }
            }
        }

        SOC_PBMP_REMOVE(pbmp, ports_to_remove);
    }

    /* counters are non-dmable */
    flags &= ~SOC_COUNTER_F_DMA;

    if (usec > 0) {
        r = soc_counter_start(unit, flags, usec, pbmp);
    } else {
        uint32 flags;
        pbmp_t pbmp;
        int prev_interval;

        r = soc_counter_status(unit, &flags,
                               &prev_interval, &pbmp);
        if (r < 0 || prev_interval <= 0) {
            prev_interval = SOC_COUNTER_INTERVAL_DEFAULT;
        }
        prev_dcmn_counter_interval[unit] = prev_interval;
        r = soc_counter_stop(unit);
    }

    if (r < 0) {
        cli_out("%s: Error: Could not set counter mode: %s\n",
                ARG_CMD(a), soc_errmsg(r));
        return CMD_FAIL;
    }

    if (usec == 0) {
         /*update counters bitmap in case counter thread is not start*/
        SOC_PBMP_ASSIGN(soc->counter_pbmp, pbmp);
        PBMP_ITER(soc->counter_pbmp, p) {
            if ((SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), p))) {
                SOC_PBMP_PORT_REMOVE(soc->counter_pbmp, p);
            }
            if (IS_LB_PORT(unit, p)) {
                SOC_PBMP_PORT_REMOVE(soc->counter_pbmp, p);
            }
        }
    }


    return CMD_OK;
}

#if defined (BCM_PETRA_SUPPORT)
/*
 *  Counter processor manipulation and access
 */
char cmd_dpp_ctr_proc_usage[] =
    "\n"
    "  INIT\n"
    "  DETACH\n"
    "  SUSPEND\n"
    "  RESUME\n"
    "  INFO\n"
    "  GET [CAched] Processor=<proc> Set=<set>\n"
    "  SET [CAched] Processor=<proc> Set=<set> Counter=<counter> [Value=<value>]\n"
    "  ALLOC SouRCe=<source>\n"
    "  ALLOC WITHProc SouRCe=<source> Processor=<proc>\n"
    "  ALLOC WITHID SouRCe=<source> Processor=<proc> Set=<set>\n"
    "  FREE Processor=<proc> Set=<set>\n"
    "Note: Behaviour of SET on an emulated counter is not defined.  See which\n"
    "counters of a set are native and which are emulated by getting that set.\n";

cmd_result_t
cmd_dpp_ctr_proc_get(int unit,
                     int cached,
                     unsigned int proc,
                     unsigned int ctrset)
{
    bcm_dpp_counter_set_t ctrall;
    bcm_dpp_counter_set_t ctrnat;
    uint64 counters[bcm_dpp_counter_count];
    bcm_dpp_counter_t types[bcm_dpp_counter_count];
    unsigned int index;
    unsigned int value;
    unsigned int limit;
    int result;

    result = bcm_dpp_counter_avail_get(unit, proc, &ctrall, &ctrnat);
    if (BCM_E_NONE != result) {
        cli_out("unable to get supported counters: %d\n", result);
        return CMD_FAIL;
    }
    for (index = 0; index < bcm_dpp_counter_count; index++) {
        if (SHR_BITGET(ctrnat, index)) {
            SHR_BITCLR(ctrall, index);
        }
    }
    value = 0;
    for (index = 0; index < bcm_dpp_counter_count; index++) {
        if (SHR_BITGET(ctrnat, index)) {
            types[value] = index;
            value++;
        }
    }
    limit = value;
    for (index = 0; index < bcm_dpp_counter_count; index++) {
        if (SHR_BITGET(ctrall, index)) {
            types[value] = index;
            value++;
        }
    }
    if (value) {
        if (cached) {
            result = bcm_dpp_counter_multi_get_cached(unit,
                                                      proc,
                                                      ctrset,
                                                      value,
                                                      &(types[0]),
                                                      &(counters[0]));
        } else {
            result = bcm_dpp_counter_multi_get(unit,
                                               proc,
                                               ctrset,
                                               value,
                                               &(types[0]),
                                               &(counters[0]));
        }
        if (BCM_E_NONE == result) {
            cli_out("native statistics for unit %d counter"
                    " processor %u set %u\n",
                    unit,
                    proc,
                    ctrset);
            for (index = 0; index < limit; index++) {
                cli_out("  0x%08x%08x : %s (%d)\n",
                        COMPILER_64_HI(counters[index]),
                        COMPILER_64_LO(counters[index]),
                        bcm_dpp_counter_t_names[types[index]],
                        types[index]);
            }
            if (value > limit) {
                cli_out("emulated statistics for unit %d counter"
                        " processor %u set %u\n",
                        unit,
                        proc,
                        ctrset);
                for (index = limit; index < value; index++) {
                    cli_out("  0x%08x%08x : %s (%d)\n",
                            COMPILER_64_HI(counters[index]),
                            COMPILER_64_LO(counters[index]),
                            bcm_dpp_counter_t_names[types[index]],
                            types[index]);
                }
            } /* if (value > limit) */
            return CMD_OK;
        } else { /* if (BCM_E_NONE == result) */
            cli_out("unable to read unit %d proc %u set %u: %d\n",
                    unit,
                    proc,
                    ctrset,
                    result);
            return CMD_FAIL;
        } /* if (BCM_E_NONE == result) */
    } else { /* if (value */
        cli_out("current mode does not support any statistics\n");
        return CMD_FAIL;
    } /* if (value */
}

cmd_result_t
cmd_dpp_ctr_proc_set(int unit,
                     int cached,
                     unsigned int proc,
                     unsigned int ctrset,
                     unsigned int ctrsel,
                     uint64 counter)
{
    int result;

    if (cached) {
        result = bcm_dpp_counter_set_cached(unit,
                                            proc,
                                            ctrset,
                                            ctrsel,
                                            counter);
    } else {
        result = bcm_dpp_counter_set(unit,
                                     proc,
                                     ctrset,
                                     ctrsel,
                                     counter);
    }
    if (BCM_E_NONE == result) {
        cli_out("successfully set unit %d proc %u set %u counter %u"
                " to 0x%08x%08x\n",
                unit,
                proc,
                ctrset,
                ctrsel,
                COMPILER_64_HI(counter),
                COMPILER_64_LO(counter));
        return CMD_OK;
    } else {
        cli_out("unable to set unit %d proc %u set %u counter %u"
                " to 0x%08x%08x: %d\n",
                unit,
                proc,
                ctrset,
                ctrsel,
                COMPILER_64_HI(counter),
                COMPILER_64_LO(counter),
                result);
        return CMD_FAIL;
    }
}

cmd_result_t
cmd_dpp_ctr_proc(int unit, args_t *a)
{
    unsigned int index;
    unsigned int limit;
    unsigned int value;
    unsigned int mode;
    uint64 counter;
    int cached;
    int result;
    int proc;
    int ctrset;
    int ctrsel;
    int source;
    parse_table_t pt;

    if (ARG_CNT(a)) {
        if (!sal_strcasecmp(_ARG_CUR(a), "info")) {
            ARG_NEXT(a);
            cli_out("unit %d counter processor information:\n", unit);
            /* display information */
            limit = 0;
            result = bcm_dpp_counter_diag_info_get(unit,
                                                   bcm_dpp_counter_diag_info_num_proc,
                                                   0,
                                                   &value);
            if ((BCM_E_INIT == result) || (BCM_E_UNAVAIL == result)) {
                cli_out("unit %d counter processor state not available\n", unit);
                return CMD_FAIL;
            }
            cli_out("unit %d counter processor information:\n", unit);
            if (BCM_E_NONE == result) {
                cli_out("  counter processors = %12d\n", value);
                limit = value;
            } else {
                cli_out("  counter processors = [error %d reading]\n", result);
            }
            result = bcm_dpp_counter_diag_info_get(unit,
                                                   bcm_dpp_counter_diag_info_bg_active,
                                                   0,
                                                   &value);
            if (BCM_E_NONE == result) {
                cli_out("  background active  = %12s\n", value?"TRUE":"FALSE");
            } else {
                cli_out("  background active  = [error %d reading]\n", result);
            }
            result = bcm_dpp_counter_diag_info_get(unit,
                                                   bcm_dpp_counter_diag_info_bg_suspend,
                                                   0,
                                                   &value);
            if (BCM_E_NONE == result) {
                cli_out("  background suspend = %12s\n", value?"TRUE":"FALSE");
            } else {
                cli_out("  background suspend = [error %d reading]\n", result);
            }
            result = bcm_dpp_counter_diag_info_get(unit,
                                                   bcm_dpp_counter_diag_info_bg_defer,
                                                   0,
                                                   &value);
            if (BCM_E_NONE == result) {
                cli_out("  background defer   = %12s\n", value?"TRUE":"FALSE");
            } else {
                cli_out("  background defer   = [error %d reading]\n", result);
            }
            result = bcm_dpp_counter_diag_info_get(unit,
                                                   bcm_dpp_counter_diag_info_fg_hit,
                                                   0,
                                                   &value);
            if (BCM_E_NONE == result) {
                cli_out("  foreground hit     = %12s\n", value?"TRUE":"FALSE");
            } else {
                cli_out("  foreground hit     = [error %d reading]\n", result);
            }
            result = bcm_dpp_counter_diag_info_get(unit,
                                                   bcm_dpp_counter_diag_info_fifo_read_background,
                                                   0,
                                                   &value);
            if (BCM_E_NONE == result) {
                cli_out("  background reads   = %12d\n", value);
            } else {
                cli_out("  background reads   = [error %d reading]\n", result);
            }
            result = bcm_dpp_counter_diag_info_get(unit,
                                                   bcm_dpp_counter_diag_info_fifo_read_deferred,
                                                   0,
                                                   &value);
            if (BCM_E_NONE == result) {
                cli_out("  background defers  = %12d\n", value);
            } else {
                cli_out("  background defers  = [error %d reading]\n", result);
            }
            for (index = 0; index < limit; index++) {
                cli_out("  counter processor %u information:\n", index);
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_source,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    counter source   = %12d (%s)\n", value, SOC_TMC_CNT_SRC_TYPE_to_string(value));
                    mode = value;
                    switch (mode) {
                    case SOC_TMC_CNT_SRC_TYPE_VOQ:
                        result = bcm_dpp_counter_diag_info_get(unit,
                                                               bcm_dpp_counter_diag_info_voq_base,
                                                               index,
                                                               &value);
                        if (BCM_E_NONE == result) {
                            cli_out("      VOQ base       = %12d\n", value);
                        } else {
                            cli_out("      VOQ base       = [error %d reading]\n", result);
                        }
                        result = bcm_dpp_counter_diag_info_get(unit,
                                                               bcm_dpp_counter_diag_info_voq_per_set,
                                                               index,
                                                               &value);
                        if (BCM_E_NONE == result) {
                            cli_out("      VOQ per set    = %12d\n", value);
                        } else {
                            cli_out("      VOQ per set    = [error %d reading]\n", result);
                        }
                        break;
                    case SOC_TMC_CNT_SRC_TYPE_STAG:
                        result = bcm_dpp_counter_diag_info_get(unit,
                                                               bcm_dpp_counter_diag_info_stat_tag_low_bit,
                                                               index,
                                                               &value);
                        if (BCM_E_NONE == result) {
                            cli_out("      stats tag LSb  = %12d\n", value);
                        } else {
                            cli_out("      stats tag LSb  = [error %d reading]\n", result);
                        }
                        break;

                    }
                } else {
                    cli_out("    counter source   = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_msb_patten,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    pointer MSBs     = %12d\n", value);
                } else {
                    cli_out("    counters         = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_counters,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    counters         = %12d\n", value);
                } else {
                    cli_out("    counters         = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_format,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    counter format   = %12d (%s)\n", value, SOC_TMC_CNT_MODE_STATISTICS_to_string(value));
                } else {
                    cli_out("    counter format   = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_sets,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    counter sets     = %12d\n", value);
                } else {
                    cli_out("    counter sets     = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_alloc_local,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    if (value) {
                        result = bcm_dpp_counter_diag_info_get(unit,
                                                               bcm_dpp_counter_diag_info_alloc_inuse,
                                                               index,
                                                               &value);
                        if (BCM_E_NONE == result) {
                            cli_out("    counter sets use = %12d\n", value);
                        } else {
                            cli_out("    counter sets use = [error %d reading]\n", result);
                        }
                    }
                } else {
                    cli_out("    counter sets use = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_fifo_read_passes,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    fifo read pass   = %12d\n", value);
                } else {
                    cli_out("    fifo read pass   = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_fifo_read_fails,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    fifo read fail   = %12d\n", value);
                } else {
                    cli_out("    fifo read fail   = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_fifo_read_items,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    fifo read items  = %12d\n", value);
                } else {
                    cli_out("    fifo read items  = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_fifo_read_max,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    fifo read max    = %12d\n", value);
                } else {
                    cli_out("    fifo read max    = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_fifo_read_last,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    fifo read last   = %12d\n", value);
                } else {
                    cli_out("    fifo read last   = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_direct_read_passes,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    direct read pass = %12d\n", value);
                } else {
                    cli_out("    direct read pass = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_direct_read_fails,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    direct read fail = %12d\n", value);
                } else {
                    cli_out("    direct read fail = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_cache_updates,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    cache updates    = %12d\n", value);
                } else {
                    cli_out("    cache updates    = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_cache_reads,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    cache reads      = %12d\n", value);
                } else {
                    cli_out("    cache reads      = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_cache_writes,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    cache writes     = %12d\n", value);
                } else {
                    cli_out("    cache writes     = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_api_reads,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    requested reads  = %12d\n", value);
                } else {
                    cli_out("    requested reads  = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_api_miss_reads,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    req read fails   = %12d\n", value);
                } else {
                    cli_out("    req read fails   = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_api_writes,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    requested writes = %12d\n", value);
                } else {
                    cli_out("    requested writes = [error %d reading]\n", result);
                }
                result = bcm_dpp_counter_diag_info_get(unit,
                                                       bcm_dpp_counter_diag_info_api_miss_writes,
                                                       index,
                                                       &value);
                if (BCM_E_NONE == result) {
                    cli_out("    req write fails  = %12d\n", value);
                } else {
                    cli_out("    req write fails  = [error %d reading]\n", result);
                }
            } /* for (index = 0; index < limit; index++) */
            return CMD_OK;
        } else if (!sal_strcasecmp(_ARG_CUR(a), "get")) {
            ARG_NEXT(a);
            if (!ARG_CNT(a)) {
                return CMD_USAGE;
            }
            /*
             * Make sure at least one of the strings is NULL terminated.
             */
            if ((strlen(_ARG_CUR(a)) > 1) &&
                (!sal_strncasecmp(_ARG_CUR(a), "cached", sal_strnlen(_ARG_CUR(a),SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) ) )) {
                cached = TRUE;
                ARG_NEXT(a);
            } else {
                cached = FALSE;
            }
            parse_table_init(unit, &pt);
            proc = 0;
            ctrset = 0;
            parse_table_add(&pt,
                            "Processor",
                            PQ_DFL|PQ_INT,
                            (void *)(0),
                            &proc,
                            NULL);
            parse_table_add(&pt,
                            "Set",
                            PQ_DFL|PQ_INT,
                            (void *)(0),
                            &ctrset,
                            NULL);
            if (parse_arg_eq(a, &pt) != 2) {
                cli_out("%s %s: Error: Unknown option: %s\n",
                        ARG_CMD(a),
                        "get",
                        ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            } else {
                parse_arg_eq_done(&pt);
            }
            return cmd_dpp_ctr_proc_get(unit, cached, proc, ctrset);
        } else if (!sal_strcasecmp(_ARG_CUR(a), "set")) {
            ARG_NEXT(a);
            if (!ARG_CNT(a)) {
                return CMD_USAGE;
            }
            /*
             * Make sure at least one of the strings is NULL terminated.
             */
            if ((strlen(_ARG_CUR(a)) > 1) &&
                (!sal_strncasecmp(_ARG_CUR(a), "cached", sal_strnlen(_ARG_CUR(a),SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)))) {
                cached = TRUE;
                ARG_NEXT(a);
            } else {
                cached = FALSE;
            }
            parse_table_init(unit, &pt);
            proc = 0;
            ctrset = 0;
            ctrsel = 0;
            COMPILER_64_SET(counter, 0,0);
            parse_table_add(&pt,
                            "Processor",
                            PQ_DFL|PQ_INT,
                            (void *)(0),
                            &proc,
                            NULL);
            parse_table_add(&pt,
                            "Set",
                            PQ_DFL|PQ_INT,
                            (void *)(0),
                            &ctrset,
                            NULL);
            parse_table_add(&pt,
                            "Counter",
                            PQ_DFL|PQ_INT,
                            (void *)(0),
                            &ctrsel,
                            NULL);
            parse_table_add(&pt,
                            "Value",
                            PQ_DFL|PQ_INT64,
                            (void *)(0),
                            &counter,
                            NULL);
            if (parse_arg_eq(a, &pt) != 4) {
                cli_out("%s %s: Error: Unknown option: %s\n",
                        ARG_CMD(a),
                        "set",
                        ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            } else {
                parse_arg_eq_done(&pt);
            }
            return cmd_dpp_ctr_proc_set(unit,
                                        cached,
                                        proc,
                                        ctrset,
                                        ctrsel,
                                        counter);
        } else if (!sal_strcasecmp(_ARG_CUR(a), "alloc")) {
            ARG_NEXT(a);
            if (!ARG_CNT(a)) {
                return CMD_USAGE;
            }
            cached = 0;
            source = 0;
            proc = 0;
            ctrset = 0;
            index = 1;
            /*
             * Make sure at least one of the strings is NULL terminated.
             */
            if (strlen(_ARG_CUR(a)) > 4) {
                if (!sal_strncasecmp(_ARG_CUR(a), "withproc",sal_strnlen(_ARG_CUR(a),SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))) {
                    cached = BCM_DPP_COUNTER_WITH_PROC;
                    ARG_NEXT(a);
                } else if (!sal_strncasecmp(_ARG_CUR(a), "withid",sal_strnlen(_ARG_CUR(a),SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))) {
                    cached = BCM_DPP_COUNTER_WITH_ID;
                    ARG_NEXT(a);
                }
            }
            parse_table_init(unit, &pt);
            parse_table_add(&pt,
                            "SouRCe",
                            PQ_DFL|PQ_INT,
                            (void *)(0),
                            &source,
                            NULL);
            if (cached & (BCM_DPP_COUNTER_WITH_PROC | BCM_DPP_COUNTER_WITH_ID)) {
                parse_table_add(&pt,
                                "Proc",
                                PQ_DFL|PQ_INT,
                                (void*)(0),
                                &proc,
                                NULL);
                index++;
            }
            if (cached & BCM_DPP_COUNTER_WITH_ID) {
                parse_table_add(&pt,
                                "Set",
                                PQ_DFL|PQ_INT,
                                (void*)(0),
                                &ctrset,
                                NULL);
                index++;
            }
            if (parse_arg_eq(a, &pt) != index) {
                cli_out("%s %s: Error: Unknown option: %s\n",
                        ARG_CMD(a),
                        "alloc",
                        ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            } else {
                parse_arg_eq_done(&pt);
            }
            index = proc;
            value = ctrset;
            result = bcm_dpp_counter_alloc(unit,
                                           cached,
                                           source,
                                           &index,
                                           &value);
            proc = index;
            ctrset = value;
            if (BCM_E_NONE == result) {
                cli_out("allocated unit %d processor %d counter set %d\n",
                        unit,
                        proc,
                        ctrset);
                return CMD_OK;
            } else {
                cli_out("failed to allocate a counter set (%d,%08X,%d,&(%d),&(%d)): %d (%s)\n",
                        unit,
                        cached,
                        source,
                        proc,
                        ctrset,
                        result,
                        _SHR_ERRMSG(result));
                return CMD_FAIL;
            }
        } else if (!sal_strcasecmp(_ARG_CUR(a), "free")) {
            ARG_NEXT(a);
            if (!ARG_CNT(a)) {
                return CMD_USAGE;
            }
            parse_table_init(unit, &pt);
            proc = 0;
            ctrset = 0;
            parse_table_add(&pt,
                            "Processor",
                            PQ_DFL|PQ_INT,
                            (void *)(0),
                            &proc,
                            NULL);
            parse_table_add(&pt,
                            "Set",
                            PQ_DFL|PQ_INT,
                            (void *)(0),
                            &ctrset,
                            NULL);
            if (parse_arg_eq(a, &pt) != 2) {
                cli_out("%s %s: Error: Unknown option: %s\n",
                        ARG_CMD(a),
                        "free",
                        ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            } else {
                parse_arg_eq_done(&pt);
            }
            result = bcm_dpp_counter_free(unit, proc, ctrset);
            if (BCM_E_NONE == result) {
                cli_out("freed unit %d processor %d set %d\n",
                        unit,
                        proc,
                        ctrset);
                return CMD_OK;
            } else {
                cli_out("unable to free unit %d processor %d set %d: %d (%s)\n",
                        unit,
                        proc,
                        ctrset,
                        result,
                        _SHR_ERRMSG(result));
                return CMD_FAIL;
            }
        } else if (!sal_strcasecmp(_ARG_CUR(a), "init")) {
            ARG_NEXT(a);
            result = bcm_dpp_counter_init(unit);
            if (BCM_E_NONE == result) {
                cli_out("unit %d counter processor init success\n", unit);
                return CMD_OK;
            } else {
                cli_out("unit %d counter processor init failed: %d (%s)\n",
                        unit,
                        result,
                        _SHR_ERRMSG(result));
                return CMD_FAIL;
            }
        } else if (!sal_strcasecmp(_ARG_CUR(a), "detach")) {
            ARG_NEXT(a);
            result = bcm_dpp_counter_background_collection_enable_set(unit, FALSE);
            if (BCM_E_NONE == result) {
                cli_out("unit %d counter processor detach success\n", unit);
                return CMD_OK;
            } else {
                cli_out("unit %d counter processor detach failed: %d (%s)\n",
                        unit,
                        result,
                        _SHR_ERRMSG(result));
                return CMD_FAIL;
            }
        } else if (!sal_strcasecmp(_ARG_CUR(a), "suspend")) {
            ARG_NEXT(a);
            result = bcm_dpp_counter_bg_enable_set(unit, FALSE);
            if (BCM_E_NONE == result) {
                cli_out("unit %d counter processor background accesses suspended\n", unit);
                return CMD_OK;
            } else {
                cli_out("unit %d counter processor background access suspend failed: %d (%s)\n",
                        unit,
                        result,
                        _SHR_ERRMSG(result));
                return CMD_FAIL;
            }
        } else if (!sal_strcasecmp(_ARG_CUR(a), "resume")) {
            ARG_NEXT(a);
            result = bcm_dpp_counter_bg_enable_set(unit, TRUE);
            if (BCM_E_NONE == result) {
                cli_out("unit %d counter processor background accesses resumed\n", unit);
                return CMD_OK;
            } else {
                cli_out("unit %d counter processor background access resume failed: %d (%s)\n",
                        unit,
                        result,
                        _SHR_ERRMSG(result));
                return CMD_FAIL;
            }
        }
    } /* if (ARG_CNT(a)) */
    return CMD_USAGE;
}


#endif /* BCM_PETRA_SUPPORT */

