
/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */



#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <bcm/error.h>
#include <sal/appl/sal.h>
#include <sal/appl/config.h>

#include <shared/pbmp.h>
#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/drv.h>
#include <soc/dpp/error.h>
#include <soc/dpp/ARAD/arad_dram.h>
#include <soc/dpp/JER/jer_dram.h>
#include <soc/dpp/ARAD/arad_api_dram.h>
#include <soc/dpp/DRC/drc_combo28.h>
#include <soc/dpp/mbcm.h>

#ifdef BCM_DDR3_SUPPORT
#include <soc/shmoo_ddr40.h>
#include <soc/phy/ddr40.h>
#endif

#include <bcm_int/dpp/error.h>

char cmd_arad_ddr_phy_regs_usage[] =
    "\n\tRead/Write By Rbus to DDR Phy\n\t"
    "Usages:\n\t"
    "ddrphyregs write       <dram_ndx>          <reg_address> <value>\n\t"
    "ddrphyregs writebr     <last_dram_ndx>     <reg_address> <value>\n\t"
    "ddrphyregs read        <dram_ndx>          <reg_address>\n\t"
    "ddrphyregs modify      <dram_ndx>          <reg_address> <value> <mask>\n"
    ;

cmd_result_t
cmd_arad_ddr_phy_regs(int unit, args_t* a)
{
    char *function, *dram_ndx, *addr, *value, *mask;
    int strnlen_function;
    uint32 int_dram_ndx, int_addr, int_value, int_mask;

    if(!SOC_IS_ARAD(unit) && !SOC_IS_DPP_DRC_COMBO28(unit)) {
        cli_out("This function is not available in this device.\n");
        return CMD_FAIL;
    }

    function = ARG_GET(a);
    dram_ndx = ARG_GET(a);
    addr = ARG_GET(a);
    if ((!function) || (!dram_ndx) || (!addr)) {
        return CMD_USAGE;
    }
    int_dram_ndx = sal_ctoi(dram_ndx, 0);
    if (int_dram_ndx >= SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max)) {
        cli_out("Dram index is above max in device(%d)\n", SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max));
        return CMD_FAIL;
    }
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    strnlen_function = sal_strnlen(function,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (!sal_strncasecmp(function, "write", strnlen_function) || !sal_strncasecmp(function, "w", strnlen_function)) {
        int_addr = sal_ctoi(addr, 0);
       
        value = ARG_GET(a);
        if (!value) {
            return CMD_USAGE;
        }
        int_value = sal_ctoi(value, 0);
        
        if (SOC_IS_DPP_DRC_COMBO28(unit)) {
            if(SOC_SAND_FAILURE(soc_dpp_drc_combo28_phy_reg_write(unit, int_dram_ndx, int_addr, int_value))) {
                return CMD_FAIL;
            }
        } else {
            if(SOC_SAND_FAILURE(arad_dram_rbus_write(unit, int_dram_ndx, int_addr, int_value))) {
                return CMD_FAIL;
            }
        }

    } else if (!sal_strncasecmp(function, "writebr", strnlen_function) || !sal_strncasecmp(function, "wbr", strnlen_function)) {
        if (SOC_IS_DPP_DRC_COMBO28(unit)) {
            cli_out("invalid function 'writebr' for drc_combo28\n");
            return CMD_NFND;
        }
        int_addr = sal_ctoi(addr, 0);
           
        value = ARG_GET(a);
        if (!value) {
            return CMD_USAGE;
        }
        int_value = sal_ctoi(value, 0);
            
        if(SOC_SAND_FAILURE(arad_dram_rbus_write_br(unit, int_dram_ndx, int_addr, int_value))) {
                return CMD_FAIL;
        }
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    } else if (!sal_strncasecmp(function, "read", strnlen_function) || !sal_strncasecmp(function, "r", strnlen_function)) {

        int_addr = sal_ctoi(addr, 0);

        if (SOC_IS_DPP_DRC_COMBO28(unit)) {
            if(SOC_SAND_FAILURE(soc_dpp_drc_combo28_phy_reg_read(unit, int_dram_ndx, int_addr, &int_value))) {
                return CMD_FAIL;
            }
        } else {
        if(SOC_SAND_FAILURE(arad_dram_rbus_read(unit, int_dram_ndx, int_addr, &int_value))) {
                return CMD_FAIL;
            }
        }
        cli_out("data=0x%x\n", int_value);
    } else if (!sal_strcasecmp(function, "modify") || !sal_strcasecmp(function, "m")) {

        int_addr = sal_ctoi(addr, 0);

        value = ARG_GET(a);
        if (!value) {
            return CMD_USAGE;
        }
        int_value = sal_ctoi(value, 0);

        mask = ARG_GET(a);
        if (!mask) {
            return CMD_USAGE;
        }
        int_mask = sal_ctoi(mask, 0);

        if (SOC_IS_DPP_DRC_COMBO28(unit)){
            if(SOC_SAND_FAILURE(soc_dpp_drc_combo28_phy_reg_modify(unit, int_dram_ndx, int_addr, int_value, int_mask))) {
               return CMD_FAIL;
            }
        } else {
            if(SOC_SAND_FAILURE(arad_dram_rbus_modify(unit, int_dram_ndx, int_addr, int_value, int_mask))) {
               return CMD_FAIL;
            }
        }
    } 

    return CMD_OK;
}

#ifdef BCM_DDR3_SUPPORT
char cmd_arad_ddr_phy_tune_usage[] = "\n"  
"  DDRPhyTune <drc_ndx>\n"  
"  Usage options:\n"
"  \t<drc_ndx>              Decimal number will be taken as index, and hexadecimal as bit map of indexes.\n"
"  \t                           for example: dram_ndx =\"0x3,7\" - will tune 0,1 and 7\n"  
"  \tCtlType=<value>        Specify a control selector for the DRAM PHY tuning procedure. Relevant only for ddr40. \n"
"  \tPhyType=<value>        Relevant only for ddr40\n"
"  \tShmooType=<value>      Selects Shmoo sub-section to be performs. Set '-1' for full Shmoo. Default: '-1'.\n"
"  \t                           Relevant only for drc_combo28. \n"
"  \tStat=<value>           Execute statistics only. Default: '0'. Relevant only for ddr40.\n"
"  \tPlot=<value>           Plot tune results. Default: '0'.\n"
"  \tExtVrefRange=<value>   Relevant only for drc_combo28\n"
"  \tSaveCfg=<value>        Relevant only for ddr40\n"
"  \tRestoreCfg=<value>     Relevant only for ddr40\n"
"  \tAction=<value>         Relevant only for drc_combo28. Select the Run/Save/restore action functionality. Default: '1'. Options:\n"
"  \t                           0 - Restore tuning parameters from saved parameters.\n"
"  \t                           1 - Run tuning. Default Action.\n"
"  \t                           2 - Save tuning parameters.\n"
"  \t                           3 - Run tuning and save tuning parameters.\n"
"\n"
"  \tExamples: 'DDRPhyTune 0 PhyType=2 Plot=1'\n"
"              'DDRPhyTune 0x3,6 ShmooType=2 Plot=1'\n";

cmd_result_t
cmd_arad_ddr_phy_tune(int unit, args_t *a)
{
    parse_table_t pt;
    int rv;
    int cdr_is_enabled;
    char  *c;
    uint32 ci_ndx, i, shmoo_flags = 0;
    uint32 max_nof_dram_interfaces = SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max);
    uint32 first_word = 0;
    soc_pbmp_t ci_pbm;
    combo28_shmoo_config_param_t *config_param;
    combo16_shmoo_config_param_t *config_param_16;
    soc_dpp_drc_combo28_info_t *drc_info;
    SHR_BITDCLNAME(dram_bmp, SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX));

    int phyType = 0;
    int shmoo_type = -1;
    int ctlType = 1;
    int stat    = 0;
    int plot    = 0;
    int ext_vref_range  = 0;
    int save    = 0;
    int restore = 0;
    int action  = 0;
    char print_buf[_SHR_PBMP_WORD_MAX*8 + 3];

    if(!SOC_IS_ARAD(unit) && !SOC_IS_DPP_DRC_COMBO28(unit)) {
        cli_out("This function is not available in this device.\n");
        return CMD_FAIL;
    }

    if (((c = ARG_GET(a)) == NULL) || (parse_small_integers(unit, c, &ci_pbm) < 0)) {
        return CMD_USAGE;
    } 

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);
        parse_table_add(&pt,"CtlType",PQ_INT, (void *) (SHMOO_COMBO28_CTL_TYPE_1), &ctlType, NULL);
        parse_table_add(&pt,"ShmooType",PQ_INT,(void *) (-1), &shmoo_type, NULL); 
        parse_table_add(&pt,"PhyType",PQ_INT,(void *) (0), &phyType, NULL);
        parse_table_add(&pt,"Stat",PQ_INT, (void *) (0), &stat, NULL);
        parse_table_add(&pt,"Plot", PQ_BOOL|PQ_DFL, 0, &plot, NULL);
        parse_table_add(&pt,"ExtVrefRange", PQ_BOOL|PQ_DFL, 0, &ext_vref_range, NULL);
        parse_table_add(&pt,"SaveCfg", PQ_BOOL|PQ_DFL, 0, &save, NULL);
        parse_table_add(&pt,"RestoreCfg", PQ_BOOL|PQ_DFL, 0, &restore, NULL);
        parse_table_add(&pt,"Action",PQ_INT, (void *) (SHMOO_COMBO28_ACTION_RUN), &action, NULL);
        if (!parseEndOk(a,&pt,&rv)) {
            return rv;
        }
    }

    _shr_pbmp_format(ci_pbm, print_buf);
    cli_out("%s(): shmoo_type=%d, Stat=%d, Plot=%d, ext_vref_range=%d, Action=%d, ci_pbm=%s\n", FUNCTION_NAME(), shmoo_type, stat, plot, ext_vref_range, action, print_buf);

    first_word = SOC_PBMP_WORD_GET(ci_pbm, 0);
    SHR_BITCOPY_RANGE(dram_bmp, 0, &first_word, 0, max_nof_dram_interfaces);

    SHR_BIT_ITER(dram_bmp, max_nof_dram_interfaces, i) {
        if (SOC_IS_DPP_DRC_COMBO28(unit)) {
        
            drc_info = &(SOC_DPP_CONFIG(unit)->arad->init.drc_info);

            /* if dram interface is not used get out with error */
            if (!SHR_BITGET(drc_info->dram_bitmap, i)) 
            {
                cli_out("error: dram_ndx:%d is not used, tuning failed\n", i);
                return CMD_FAIL;
            }

            config_param = &(SOC_DPP_CONFIG(unit)->arad->init.drc_info.shmoo_config_param[i]);
             
            if (stat == 0x1) {
               shmoo_flags |= SHMOO_COMBO28_CTL_FLAGS_STAT_BIT;
            }
            if (plot == 0x1) {
               shmoo_flags |= SHMOO_COMBO28_CTL_FLAGS_PLOT_BIT;
            }
            if (ext_vref_range == 0x1) {
               shmoo_flags |= SHMOO_COMBO28_CTL_FLAGS_EXT_VREF_RANGE_BIT;
            }            
                         
            if( soc_dpp_drc_combo28_cdr_monitor_enable_get(unit, i, &cdr_is_enabled)) {
                cli_out("soc_dpp_drc_combo28_cdr_monitor_enable_get() error: dram_ndx:%d failed\n", i);
                return CMD_FAIL;
            }
                                     
            if (cdr_is_enabled) {
                if (soc_dpp_drc_combo28_cdr_monitor_enable_set(unit, i, 0)) {
                    cli_out("soc_dpp_drc_combo28_cdr_monitor_enable_set() error: dram_ndx:%d failed\n", i);
                    return CMD_FAIL;
                }

                if (soc_dpp_drc_combo28_cdr_ctl(unit, i, 1, 0, 0)) {
                    cli_out("soc_dpp_drc_combo28_cdr_ctl() error: dram_ndx:%d failed\n", i);
                    return CMD_FAIL;
                }
            }
            
            if (SOC_IS_SINAI(unit))
            {
                cli_out("%s(): DRC_COMBO16. drc_ndx=%d\n", FUNCTION_NAME(), i); 
                config_param_16 = (combo16_shmoo_config_param_t*)config_param;
                if(soc_combo16_shmoo_ctl(unit, i, shmoo_type, shmoo_flags, action, config_param_16)){
                    cli_out("soc_combo16_shmoo_ctl() error: Cfg dram_ndx:%d failed\n", i);
                    return CMD_FAIL;
                }
            }
            else 
            {
                cli_out("%s(): DRC_COMBO28. drc_ndx=%d\n", FUNCTION_NAME(), i); 
                if(soc_combo28_shmoo_ctl(unit, i, shmoo_type, shmoo_flags, action, config_param)){
                    cli_out("soc_combo28_shmoo_ctl() error: Cfg dram_ndx:%d failed\n", i);
                    return CMD_FAIL;
                }
            }
            
            if (cdr_is_enabled) {
                if (soc_dpp_drc_combo28_cdr_ctl(unit, i, 1, 1, 1)) {
                    cli_out("soc_dpp_drc_combo28_cdr_ctl() error: dram_ndx:%d failed\n", i);
                    return CMD_FAIL;
                }

                if (soc_dpp_drc_combo28_cdr_monitor_enable_set(unit, i, cdr_is_enabled)) {
                    cli_out("soc_dpp_drc_combo28_cdr_monitor_enable_set() error: dram_ndx:%d failed\n", i);
                    return CMD_FAIL;
                }
            }

            if( soc_dpp_drc_combo28_gddr5_restore_dbi_and_crc(unit, drc_info, i)) {
                cli_out("soc_dpp_drc_combo28_gddr5_restore_dbi_and_crc() error: dram_ndx:%d failed\n", i);
                return CMD_FAIL;
            }

            if ((action == SHMOO_COMBO28_ACTION_SAVE) || (action == SHMOO_COMBO28_ACTION_RUN_AND_SAVE)) {
                if(soc_dpp_drc_combo28_shmoo_cfg_set(unit, i, config_param)){
                    cli_out("soc_combo28_shmoo_ctl() error: Cfg dram_ndx:%d failed\n", i);
                    return CMD_FAIL;
                }   
            }

        } else {
            ci_ndx = 2*i;
            cli_out("%s(): CtlType=%d, PhyType=%d, SaveCfg=%d\n", FUNCTION_NAME(), ctlType, phyType, save);
            if (restore) {
                if (soc_ddr40_shmoo_restorecfg(unit, ci_ndx)) {
                    cli_out(" RestoreCfg dram_ndx:%d failed\n", ci_ndx);
                    return CMD_FAIL;
                }
            } else {
                if(soc_ddr40_shmoo_ctl(unit, ci_ndx, phyType, ctlType, stat , plot)) {
                    cli_out(" ci_ndx=%d failed\n", ci_ndx);
                    return CMD_FAIL;
                }
                if (save) {
                    if (soc_ddr40_shmoo_savecfg(unit, ci_ndx)) {
                        cli_out(" SaveCfg ci_ndx:%d failed\n", ci_ndx);
                    }
                }
        
            }
        }
    }

    return CMD_OK;
}

char cmd_arad_ddr_phy_cdr_usage[] = "\n"  
"  DDRPhyCDR <drc_ndx>\n"  
"  Usage options:\n"
"  \t<drc_ndx> - decimal number will be taken as index, and hexadecimal as bit map of indexes.\n"
"  \t            for example: dram_ndx =\"0x3,7\" - will tune 0,1 and 7\n"
"  \tStat=<value>                   Read current configuration; otherwise skip\n"
"  \tByte=<value>                   Skip for all bytes\n"
"  \tResetN=<value>                 Skip to keep present value\n"
"  \tEnable=<value>                 Skip to keep present value\n"
"  \tP=<value>                      Skip to keep present value\n"
"  \tN=<value>                      Skip to keep present value\n"
"  \tI=<value>                      Skip to keep present value\n"
"  \tQ=<value>                      Skip to keep present value\n"
"  \tIB=<value>                     Skip to keep present value\n"
"  \tDataSource=<value>             Skip to keep present value\n"
"  \tInitLockTransition=<value>     Skip to keep present value\n"
"  \tAccuPosThreshold=<value>       Skip to keep present value\n"
"  \tUpdateGap=<value>              Skip to keep present value\n"
"  \tUpdateMode=<value>             Skip to keep present value\n"
"  \tAutoCopy=<value>               Default=0\n"
"  eg.  DDRPhyCDR 0 Enable=1\n"
"       DDRPhyCDR 0x3,6 Stat=1\n";

cmd_result_t
cmd_arad_ddr_phy_cdr(int unit, args_t *a)
{
    parse_table_t pt;
    int rv;
    char  *c;
    uint32 i;
    uint32 max_nof_dram_interfaces = SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max);
    uint32 first_word = 0;
    soc_pbmp_t ci_pbm;
    combo28_cdr_config_param_t config_param;
    combo16_cdr_config_param_t config_param_16;
    SHR_BITDCLNAME(dram_bmp, SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX));    
    
        
    int stat    = 0;
/*    char print_buf[256]; */

    if (((c = ARG_GET(a)) == NULL) || (parse_small_integers(unit, c, &ci_pbm) < 0)) {
        return CMD_USAGE;
    } 

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);
        parse_table_add(&pt, "Byte", PQ_INT, (void *) (-1), &(config_param.byte), NULL);
        parse_table_add(&pt, "ResetN", PQ_INT, (void *) (SHMOO_COMBO28_CDR_UNDEFINED_VALUE), &(config_param.reset_n), NULL);
        parse_table_add(&pt, "Enable", PQ_INT, (void *) (SHMOO_COMBO28_CDR_UNDEFINED_VALUE), &(config_param.enable), NULL);
        parse_table_add(&pt, "Stat", PQ_INT, (void *) (0), &stat, NULL);
        parse_table_add(&pt, "P", PQ_INT, (void *) (SHMOO_COMBO28_CDR_UNDEFINED_VALUE), &(config_param.p), NULL);
        parse_table_add(&pt, "N", PQ_INT, (void *) (SHMOO_COMBO28_CDR_UNDEFINED_VALUE), &(config_param.n), NULL);
        parse_table_add(&pt, "I", PQ_INT, (void *) (SHMOO_COMBO28_CDR_UNDEFINED_VALUE), &(config_param.i), NULL);
        parse_table_add(&pt, "Q", PQ_INT, (void *) (SHMOO_COMBO28_CDR_UNDEFINED_VALUE), &(config_param.q), NULL);
        parse_table_add(&pt, "IB", PQ_INT, (void *) (SHMOO_COMBO28_CDR_UNDEFINED_VALUE), &(config_param.ib), NULL);
        parse_table_add(&pt, "DataSource", PQ_INT, (void *) (SHMOO_COMBO28_CDR_UNDEFINED_VALUE), &(config_param.data_source), NULL);
        parse_table_add(&pt, "InitLockTransition", PQ_INT, (void *) (SHMOO_COMBO28_CDR_UNDEFINED_VALUE), &(config_param.init_lock_transition), NULL);
        parse_table_add(&pt, "AccuPosThreshold", PQ_INT, (void *) (SHMOO_COMBO28_CDR_UNDEFINED_VALUE), &(config_param.accu_pos_threshold), NULL);
        parse_table_add(&pt, "UpdateGap", PQ_INT, (void *) (SHMOO_COMBO28_CDR_UNDEFINED_VALUE), &(config_param.update_gap), NULL);
        parse_table_add(&pt, "UpdateMode", PQ_INT, (void *) (SHMOO_COMBO28_CDR_UNDEFINED_VALUE), &(config_param.update_mode), NULL);
        parse_table_add(&pt, "AutoCopy", PQ_INT, (void *) (0), &(config_param.auto_copy), NULL);
        if (!parseEndOk(a,&pt,&rv)) {
            return rv;
        }
    }

    first_word = SOC_PBMP_WORD_GET(ci_pbm, 0);
    SHR_BITCOPY_RANGE(dram_bmp, 0, &first_word, 0, max_nof_dram_interfaces);

    SHR_BIT_ITER(dram_bmp, max_nof_dram_interfaces, i) {
      if (SOC_IS_SINAI(unit))
      {
          sal_memcpy(&config_param_16,&config_param,sizeof(config_param_16));
          if (soc_combo16_cdr_ctl(unit, i, stat, &config_param_16))
          {
              return CMD_FAIL;
          }
      }
      else
      {
          if (soc_combo28_cdr_ctl(unit, i, stat, &config_param))
          {
              return CMD_FAIL;
          }
      }
    }

    return CMD_OK;
}

char cmd_arad_dram_buf_usage[] =
    "Usage:\n"
    "\tDRAMBuf [options]\n" 
    "Usage options:\n"
    "\tcrc_del_enable <max_error> -        Enable/Disable quarantine of fault DRAM buffers (both MMU and IPT CRC).\n"
    "\t                                    max_error - max num of error allowed per DRAM buffer,\n"
    "\t                                    if max_error == 0 quarantine of fault DRAM buffers is disabled.\n"
    "\tread_del_fifo -                     Print details on the buffers in the CRC deleted buffers FIFO.\n"
    "\tlist_err_cntr -                     List all buffers that was detected with error.\n"
    "\tinit_err_cntr <buf> -               Zero the error counter of a buffer (Use buf=-1 to Zero all the error counters).\n"
    "\ttest_buf <buf> -                    Perform test on deleted buffer (doesn't supported under traffic).\n"
    "\tdelete_buf <buf> -                  Delete  buffer.\n"
    "\trelease_buf <buf> -                 Release  buffer (if it was quarantined).\n"
    ; 


cmd_result_t
cmd_arad_dram_buf(int unit, args_t *a)
{
    char *param, *func;


    if(!SOC_IS_ARAD(unit)) {
        cli_out("This function is not available in this device.\n");
        return CMD_FAIL;
    }

    if(!SOC_IS_ARAD(unit)) {
        cli_out("This function is not available in this device.\n");
        return CMD_FAIL;
    }

    if (!(func = ARG_GET(a))) {    /* Nothing to do    */
        return(CMD_USAGE);      /* Print usage line */
    }
    
    

    if(!sal_strcasecmp(func, "crc_del_enable")) {

        uint32 max_err;

        if (!(param = ARG_GET(a))) {    /* Nothing to do    */
            return(CMD_USAGE);      /* Print usage line */
        }

        max_err = (uint32)sal_ctoi(param, NULL);
        /* set num of max error */
        if(SOC_SAND_FAILURE(arad_dram_crc_del_buffer_max_reclaims_set(unit, max_err))) {
            cli_out("Error: arad_dram_crc_del_buffer_max_reclaims_set(%d, 1 )\n", unit);
            return CMD_FAIL;
        }
        /* enable/disable DRAM crc delete buffers */
        if(max_err != 0) {
            if(SOC_SAND_FAILURE(arad_dram_crc_delete_buffer_enable(unit, 1))) {
                cli_out("Error: arad_dram_crc_delete_buffer_enable(%d, 1 )\n", unit);
                return CMD_FAIL;
            }
        } else {
            if(SOC_SAND_FAILURE(arad_dram_crc_delete_buffer_enable(unit, 0))) {
                cli_out("Error: arad_dram_crc_delete_buffer_enable(%d, 0 )\n", unit);
                return CMD_FAIL;
            }
        }

    } else if (!sal_strcasecmp(func, "read_del_fifo")){ 
        
        int i;
        uint32 del_buf[16];
        uint32 del_buf_count;

        if (SOC_SAND_FAILURE(arad_dram_delete_buffer_read_fifo(unit, 16, del_buf, &del_buf_count))) {
            cli_out("Error: arad_dram_delete_buffer_read_fifo(%d, 16, &del_buf, &del_buf_count) failed\n", unit);
            return CMD_FAIL;    
        }

        cli_out("read from CRC dram del buffers FIFO:\n");
        if ( del_buf_count == 0 ) {
            cli_out("The FIFO is empty\n");
        }

        for(i=0; i<del_buf_count; ++i) {
            cli_out("\tbuffer: %u\n", del_buf[i]);
        }

    } else if (!sal_strcasecmp(func, "list_err_cntr")){  
        
        int i;
        uint32 is_buf, buf;
        arad_dram_buffer_info_t buf_info;
        
        for(i=0; i<ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++i) {
            
            if (SOC_SAND_FAILURE(arad_dram_get_buffer_error_cntr_in_list_index(unit, i, &is_buf, &buf))) {
   
                cli_out("Error: arad_dram_get_buffer_error_cntr_in_list_index(%d, %d, &is_buf, &buf) failed\n", unit, i);
                return CMD_FAIL;
                   
            } else if (is_buf){
                if(SOC_SAND_FAILURE(arad_dram_buffer_get_info(unit, buf, &buf_info))) {
                    cli_out("Error:  arad_dram_buffer_get_info(%d, %u, &buf_info)", unit, buf);
                    return CMD_FAIL;
                }

                cli_out("Buf: %u\n"
                        "\tbuf num: %u ,bank: %u, channel: %c, err cntr: %u, is deleted: %u\n" , buf, buf_info.buf_num, buf_info.bank, buf_info.channel, buf_info.err_cntr, buf_info.is_deleted);

            }
            
        }

    } else if ((param = ARG_GET(a))){
            
            int buf;
            buf =  sal_ctoi(param, NULL); 

            if(!sal_strcasecmp(func, "test_buf")){

                uint32 is_pass;

                if(SOC_SAND_FAILURE(arad_dram_delete_buffer_test(unit, buf, &is_pass))) { 
                    cli_out("Error: arad_dram_delete_buffer_test(%d, %u, &is_pass)", unit, buf);
                    return CMD_FAIL;
                } else {
                    cli_out("is_pass = %u\n", is_pass);
                }

            } else if(!sal_strcasecmp(func, "delete_buf")){

                if(SOC_SAND_FAILURE(arad_dram_delete_buffer_action(unit, buf, 1))) { 
                    cli_out("Error: arad_dram_delete_buffer_action(%d, %u, 1)", unit, buf);
                    return CMD_FAIL;
                }

            }  else if(!sal_strcasecmp(func, "release_buf")){

                if(SOC_SAND_FAILURE(arad_dram_delete_buffer_action(unit, buf, 0))) { 
                    cli_out("Error: arad_dram_delete_buffer_action(%d, %u, 0)", unit, buf);
                    return CMD_FAIL;
                }

            } else if (!sal_strcasecmp(func, "init_err_cntr")){

                 if(SOC_SAND_FAILURE(arad_dram_init_buffer_error_cntr(unit, (uint32)buf))) { 
                    cli_out("Error: arad_dram_init_buffer_error_cntr(%d, %u)", unit, buf);
                    return CMD_FAIL;
                }

            } else {
                return(CMD_USAGE);      /* Print usage line */
            } 
        
    } else {
        return(CMD_USAGE);      /* Print usage line */
    }
    return (CMD_OK);

}


void dpp_mmu_ind_access_print(int unit, int index, soc_reg_above_64_val_t* data)
{
    if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit))
    {
        cli_out("%d:\t0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n"
                   "\t0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n"
                   "\t0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n"
                   "\t0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", 
            index, data[0][0],  data[0][1],  data[0][2],  data[0][3],  data[0][4],  data[0][5],  data[0][6],  data[0][7],  data[0][8],  data[0][9],  data[0][10], data[0][11], data[0][12], data[0][13], data[0][14], data[0][15],
                   data[0][16], data[0][17], data[0][18], data[0][19], data[1][0],  data[1][1],  data[1][2],  data[1][3],  data[1][4],  data[1][5],  data[1][6],  data[1][7],  data[1][8],  data[1][9],  data[1][10], data[1][11],
                   data[1][12], data[1][13], data[1][14], data[1][15], data[1][16], data[1][17], data[1][18], data[1][19], data[2][0],  data[2][1],  data[2][2],  data[2][3],  data[2][4],  data[2][5],  data[2][6],  data[2][7],
                   data[2][8],  data[2][9],  data[2][10], data[2][11], data[2][12], data[2][13], data[2][14], data[2][15], data[2][16], data[2][17], data[2][18], data[2][19], data[3][0],  data[3][1],  data[3][2],  data[3][3]);
    } else if (SOC_IS_ARAD(unit)) {
        cli_out("%d: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", 
            index, data[0][0], data[0][1], data[0][2],  data[0][3],  data[0][4],  data[0][5],  data[0][6],  data[0][7], data[0][8], data[0][9], data[0][10], data[0][11], data[0][12], data[0][13], data[0][14], data[0][15]);
    }
}


char cmd_arad_dram_mmu_ind_access_usage[] =
    "Usage:\n"
    "\tDramMmuIndAccess read/write <Options>- mmu indirect read/write\n"
    "Usage options:\n"
    "\t<LogicalMod> -                       Determine access method Logical/Physical\n"
    "\t<ADdr> -                             Physical address (Physical mode)\n"
    "\t<INDex> -                            Index of the 64(Arad) or 256(Jericho) bytes of the buffer, Index=-1 will read/write entire buffer (Logical mode).\n"
    "\t<BufNum> -                           Buff number (Logical mode).\n"
    "\t<Data0>, <Data1>, ... <Data63> -     User configures Data Pattern (for arad only Data0 - Data15 are used). By default data will be zero (Used when patternMod=0)\n"
    "\t<PatternMod> -                       Pattern mode (Default Value '0').\n"
    "\t       PatternMod=0 -                Use Data parameter (given by user)\n"
    "\t       PatternMod=1 -                Fill data with 0101.. Pattern to all the buffer\n"
    "\t       PatternMod=2 -                Fill data with 1111.. Pattern to all the buffer\n"
    "\t       PatternMod=3 -                Fill data with 0000.. Pattern to all the buffer\n"
    "\t       PatternMod=4 -                Fill data with 0x1, 0x2... 0x3f Pattern to all the buffer\n"
    "Examples:\n"
    "\tDramMmuIndAccess read   LogicalMod=0  ADdr=0x1268389\n"
    "\tDramMmuIndAccess read   LogicalMod=1 INDex=-1 BufNum=134862\n"
    "\tDramMmuIndAccess write  PatternMod=0 LogicalMod=0  ADdr=0x10 Data0=0x1 Data1=0x77 Data2=0x1 Data3=0x2\n"
    "\tDramMmuIndAccess write  PatternMod=0 LogicalMod=1  INDex=0 BufNum=134862 Data0=0x1 Data1=0x2 Data2=0x3\n"
    "\tDramMmuIndAccess write  PatternMod=1 LogicalMod=1  INDex=0 BufNum=134862\n"
    ;

cmd_result_t
cmd_arad_dram_mmu_ind_access(int unit, args_t *a)
{
    int rv, max_index, mmu_dram_transaction_size; 
    uint32  i, addr=0, logical_mod=0, index=0, buf_num=0, pattern_mod=0, addr_full, read_flag, write_flag;
    char *func;
    soc_reg_above_64_val_t data[SOC_DPP_DRAM_MMU_IND_ACCESS_MAX_BUFF_SIZE];
    parse_table_t pt;   
    uint32 reg_val=0,original_ovrd_ocb_value=0;

    if(!SOC_IS_ARAD(unit)) {
        cli_out("This function is not available in this device.\n");
        return CMD_FAIL;
    }

    if (!(func = ARG_GET(a))) {    /* Nothing to do    */
        return(CMD_USAGE);      /* Print usage line */
    }

    mmu_dram_transaction_size = (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) ? 256 : 64 ;
    max_index = SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size / mmu_dram_transaction_size ;

    sal_memset(data, 0x0, sizeof(data));

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);
        parse_table_add(&pt, "LogicalMod", PQ_INT, (void *)(0), &logical_mod, NULL);
        parse_table_add(&pt, "ADdr", PQ_INT, (void *)(0), &addr, NULL);
        parse_table_add(&pt, "INDex", PQ_INT, (void *)(0), &index, NULL);
        parse_table_add(&pt, "BufNum", PQ_INT, (void *)(0), &buf_num, NULL);
        parse_table_add(&pt, "PatternMod", PQ_INT, (void *)(0), &pattern_mod, NULL);
        parse_table_add(&pt, "Data0",  PQ_INT, (void *)(0), &data[0][0],  NULL);
        parse_table_add(&pt, "Data1",  PQ_INT, (void *)(0), &data[0][1],  NULL);
        parse_table_add(&pt, "Data2",  PQ_INT, (void *)(0), &data[0][2],  NULL);
        parse_table_add(&pt, "Data3",  PQ_INT, (void *)(0), &data[0][3],  NULL);
        parse_table_add(&pt, "Data4",  PQ_INT, (void *)(0), &data[0][4],  NULL);
        parse_table_add(&pt, "Data5",  PQ_INT, (void *)(0), &data[0][5],  NULL);
        parse_table_add(&pt, "Data6",  PQ_INT, (void *)(0), &data[0][6],  NULL);
        parse_table_add(&pt, "Data7",  PQ_INT, (void *)(0), &data[0][7],  NULL);
        parse_table_add(&pt, "Data8",  PQ_INT, (void *)(0), &data[0][8],  NULL);
        parse_table_add(&pt, "Data9",  PQ_INT, (void *)(0), &data[0][9],  NULL);
        parse_table_add(&pt, "Data10", PQ_INT, (void *)(0), &data[0][10], NULL);
        parse_table_add(&pt, "Data11", PQ_INT, (void *)(0), &data[0][11], NULL);
        parse_table_add(&pt, "Data12", PQ_INT, (void *)(0), &data[0][12], NULL);
        parse_table_add(&pt, "Data13", PQ_INT, (void *)(0), &data[0][13], NULL);
        parse_table_add(&pt, "Data14", PQ_INT, (void *)(0), &data[0][14], NULL);
        parse_table_add(&pt, "Data15", PQ_INT, (void *)(0), &data[0][15], NULL);
        parse_table_add(&pt, "Data16", PQ_INT, (void *)(0), &data[0][16], NULL);
        parse_table_add(&pt, "Data17", PQ_INT, (void *)(0), &data[0][17], NULL);
        parse_table_add(&pt, "Data18", PQ_INT, (void *)(0), &data[0][18], NULL);
        parse_table_add(&pt, "Data19", PQ_INT, (void *)(0), &data[0][19], NULL);
        parse_table_add(&pt, "Data20", PQ_INT, (void *)(0), &data[1][0],  NULL);
        parse_table_add(&pt, "Data21", PQ_INT, (void *)(0), &data[1][1],  NULL);
        parse_table_add(&pt, "Data22", PQ_INT, (void *)(0), &data[1][2],  NULL);
        parse_table_add(&pt, "Data23", PQ_INT, (void *)(0), &data[1][3],  NULL);
        parse_table_add(&pt, "Data24", PQ_INT, (void *)(0), &data[1][4],  NULL);
        parse_table_add(&pt, "Data25", PQ_INT, (void *)(0), &data[1][5],  NULL);
        parse_table_add(&pt, "Data26", PQ_INT, (void *)(0), &data[1][6],  NULL);
        parse_table_add(&pt, "Data27", PQ_INT, (void *)(0), &data[1][7],  NULL);
        parse_table_add(&pt, "Data28", PQ_INT, (void *)(0), &data[1][8],  NULL);
        parse_table_add(&pt, "Data29", PQ_INT, (void *)(0), &data[1][9],  NULL);
        parse_table_add(&pt, "Data30", PQ_INT, (void *)(0), &data[1][10], NULL);
        parse_table_add(&pt, "Data31", PQ_INT, (void *)(0), &data[1][11], NULL);
        parse_table_add(&pt, "Data32", PQ_INT, (void *)(0), &data[1][12], NULL);
        parse_table_add(&pt, "Data33", PQ_INT, (void *)(0), &data[1][13], NULL);
        parse_table_add(&pt, "Data34", PQ_INT, (void *)(0), &data[1][14], NULL);
        parse_table_add(&pt, "Data35", PQ_INT, (void *)(0), &data[1][15], NULL);
        parse_table_add(&pt, "Data36", PQ_INT, (void *)(0), &data[1][16], NULL);
        parse_table_add(&pt, "Data37", PQ_INT, (void *)(0), &data[1][17], NULL);
        parse_table_add(&pt, "Data38", PQ_INT, (void *)(0), &data[1][18], NULL);
        parse_table_add(&pt, "Data39", PQ_INT, (void *)(0), &data[1][19], NULL);
        parse_table_add(&pt, "Data40", PQ_INT, (void *)(0), &data[2][0],  NULL);
        parse_table_add(&pt, "Data41", PQ_INT, (void *)(0), &data[2][1],  NULL);
        parse_table_add(&pt, "Data42", PQ_INT, (void *)(0), &data[2][2],  NULL);
        parse_table_add(&pt, "Data43", PQ_INT, (void *)(0), &data[2][3],  NULL);
        parse_table_add(&pt, "Data44", PQ_INT, (void *)(0), &data[2][4],  NULL);
        parse_table_add(&pt, "Data45", PQ_INT, (void *)(0), &data[2][5],  NULL);
        parse_table_add(&pt, "Data46", PQ_INT, (void *)(0), &data[2][6],  NULL);
        parse_table_add(&pt, "Data47", PQ_INT, (void *)(0), &data[2][7],  NULL);
        parse_table_add(&pt, "Data48", PQ_INT, (void *)(0), &data[2][8],  NULL);
        parse_table_add(&pt, "Data49", PQ_INT, (void *)(0), &data[2][9],  NULL);
        parse_table_add(&pt, "Data50", PQ_INT, (void *)(0), &data[2][10], NULL);
        parse_table_add(&pt, "Data51", PQ_INT, (void *)(0), &data[2][11], NULL);
        parse_table_add(&pt, "Data52", PQ_INT, (void *)(0), &data[2][12], NULL);
        parse_table_add(&pt, "Data53", PQ_INT, (void *)(0), &data[2][13], NULL);
        parse_table_add(&pt, "Data54", PQ_INT, (void *)(0), &data[2][14], NULL);
        parse_table_add(&pt, "Data55", PQ_INT, (void *)(0), &data[2][15], NULL);
        parse_table_add(&pt, "Data56", PQ_INT, (void *)(0), &data[2][16], NULL);
        parse_table_add(&pt, "Data57", PQ_INT, (void *)(0), &data[2][17], NULL);
        parse_table_add(&pt, "Data58", PQ_INT, (void *)(0), &data[2][18], NULL);
        parse_table_add(&pt, "Data59", PQ_INT, (void *)(0), &data[2][19], NULL);
        parse_table_add(&pt, "Data60", PQ_INT, (void *)(0), &data[3][0],  NULL);
        parse_table_add(&pt, "Data61", PQ_INT, (void *)(0), &data[3][1],  NULL);
        parse_table_add(&pt, "Data62", PQ_INT, (void *)(0), &data[3][2],  NULL);
        parse_table_add(&pt, "Data63", PQ_INT, (void *)(0), &data[3][3],  NULL); 
        if (!parseEndOk(a,&pt,&rv)) {
            return rv;
        }
    }

    read_flag = !sal_strcasecmp(func, "read");
    write_flag = !sal_strcasecmp(func, "write");

    /* Check usage */
    if((buf_num && addr)            ||
       (buf_num && !logical_mod)    ||
       (addr && logical_mod)        || 
       (read_flag && pattern_mod)   || 
       (read_flag && write_flag)    ||
       (!read_flag && !write_flag)      )
    {
        /* Print usage line */ 
        return(CMD_USAGE);
    }

    /* Set pattern to write */
    if(write_flag == 1) 
    {
        switch(pattern_mod) 
        {
            case 0:
                break;
            case 1:
                sal_memset(data, 0x01, sizeof(data));
                break;
            case 2:
                sal_memset(data, 0x11, sizeof(data));
                break; 
            case 3:
                sal_memset(data, 0x0, sizeof(data));
                break;
            case 4:
                for(i = 0; i < SOC_DPP_DRAM_MMU_IND_ACCESS_MAX_BUFF_SIZE * SOC_REG_ABOVE_64_MAX_SIZE_U32; ++i)
                {
                    data[i / SOC_REG_ABOVE_64_MAX_SIZE_U32][i % SOC_REG_ABOVE_64_MAX_SIZE_U32] = i;
                }
                break;          
            default:
                /* Print usage line */
                return(CMD_USAGE);
        }
    }

    if(logical_mod && SOC_IS_ARADPLUS_A0(unit)) {
        if(SOC_SAND_FAILURE(soc_reg32_get(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, REG_PORT_ANY, 0, &reg_val))) {
            return CMD_FAIL;
        }
        original_ovrd_ocb_value = soc_reg_field_get(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, reg_val, OVRD_OCB_INDCICATION_DURING_INDIRECTf);
        soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, &reg_val, OVRD_OCB_INDCICATION_DURING_INDIRECTf, 1);
        if(SOC_SAND_FAILURE(soc_reg32_set(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, REG_PORT_ANY, 0, reg_val))) {
            return CMD_FAIL;
        }
    }
    /* Iterate over Indices and Perform Read/Write Action */    
    for(i=0; i<max_index; ++i) 
    {
        if( (i==index) || (index==-1) ) 
        {
            /* Set read/write addr_full */
            if(logical_mod) 
            {
                if(SOC_SAND_FAILURE(arad_dram_mmu_indirect_get_logical_address_full(unit,buf_num, i, &addr_full))) 
                {
                    cli_out("Error:  arad_dram_mmu_indirect_get_logical_address_full(%d, %u, %d, &addr)", unit, buf_num, index);
                    return CMD_FAIL;
                } 
            } else {
                if(SOC_SAND_FAILURE(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_dram_validate_address,(unit, addr))))
                {
                    cli_out("Error:  mbcm_dpp_dram_validate_address - invalid physical address:0x%x", addr);
                    return CMD_FAIL;
                }
                addr_full = addr;
            }

            if(read_flag == 1) 
            {
                if (SOC_SAND_FAILURE(arad_dram_mmu_indirect_read(unit, logical_mod, addr_full, data))) 
                {
                    cli_out("Error: arad_dram_mmu_indirect_read(%d, 1, %u, &data)", unit, addr);
                    return CMD_FAIL;
                } else {
                    dpp_mmu_ind_access_print(unit, i, data);
                }
            } else { /* (write_flag == 1) */
                if (SOC_SAND_FAILURE(arad_dram_mmu_indirect_write(unit, logical_mod, addr_full, data)))
                {
                    cli_out("Error: arad_dram_mmu_indirect_write(%d, 1, %u, &data)", unit, addr);
                    return CMD_FAIL;
                }
            }
        }
    }

    if(logical_mod && SOC_IS_ARADPLUS_A0(unit)) {
        if(SOC_SAND_FAILURE(soc_reg32_get(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, REG_PORT_ANY, 0, &reg_val))) {
            return CMD_FAIL;
        }
        soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, &reg_val, OVRD_OCB_INDCICATION_DURING_INDIRECTf, original_ovrd_ocb_value);
        if(SOC_SAND_FAILURE(soc_reg32_set(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, REG_PORT_ANY, 0, reg_val))) {
            return CMD_FAIL;
        }
    }

    return (CMD_OK);

}


#endif /* BCM_DDR3_SUPPORT */
#endif /* BCM_PETRA_SUPPORT */
