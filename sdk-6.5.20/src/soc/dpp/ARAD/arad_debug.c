#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT



#include <shared/bsl.h>

#include <soc/dcmn/error.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_conv.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/dpp/ARAD/arad_debug.h>
#include <soc/dpp/ARAD/arad_api_debug.h>
#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/ARAD/arad_kbp_recover.h>
#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_scheduler_flow_converts.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_dram.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/mbcm.h>






#define ARAD_DBG_AUTOCREDIT_RATE_MIN_TH          (15)
#define ARAD_DBG_AUTOCREDIT_RATE_MAX_TH          (soc_sand_power_of_2(19))
#define ARAD_DBG_AUTOCREDIT_RATE_MIN_VALUE       (1)
#define ARAD_DBG_AUTOCREDIT_RATE_MAX_VALUE       (15)
#define ARAD_DBG_AUTOCREDIT_RATE_OFFSET          (3)
#define ARAD_DEBUG_RST_DOMAIN_MAX                (ARAD_DBG_NOF_RST_DOMAINS-1)

























STATIC uint32
  arad_dbg_rate2autocredit_rate(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   rate,
    SOC_SAND_OUT uint32   *autocr_rate_bits
  )
{
  uint32
    res,
    credit_worth,
    device_ticks_per_sec,
    autocr_rate = 0; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DBG_RATE2AUTOCREDIT_RATE);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(
    res,2,exit,ARAD_GET_ERR_TEXT_001,MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_credit_worth_get, (unit, &credit_worth))) ;

  if (rate == 0) 
  {
    *autocr_rate_bits = 0;
  }
  else
  {
    device_ticks_per_sec = arad_chip_ticks_per_sec_get(unit);

    res = soc_sand_kbits_per_sec_to_clocks(
            rate * 1000,
            credit_worth,
            device_ticks_per_sec,
            &autocr_rate
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (autocr_rate <= ARAD_DBG_AUTOCREDIT_RATE_MIN_TH)
    {
      *autocr_rate_bits = ARAD_DBG_AUTOCREDIT_RATE_MIN_VALUE;
    }
    else if (autocr_rate >= ARAD_DBG_AUTOCREDIT_RATE_MAX_TH)
    {
      *autocr_rate_bits = ARAD_DBG_AUTOCREDIT_RATE_MAX_VALUE;
    }
    else 
    {
      *autocr_rate_bits = soc_sand_log2_round_down(autocr_rate) - ARAD_DBG_AUTOCREDIT_RATE_OFFSET;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dbg_rate2autocredit_rate()",0,0);
}


STATIC uint32
  arad_dbg_autocredit_rate2rate(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   autocr_rate_bits,
    SOC_SAND_OUT uint32   *rate
  )
{
  uint32
    res,
    credit_worth,
    device_ticks_per_sec,
    autocr_rate = 0; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DBG_AUTOCREDIT_RATE2RATE);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(
    res,2,exit,ARAD_GET_ERR_TEXT_001,MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_credit_worth_get, (unit, &credit_worth))) ;

  if (autocr_rate_bits == 0) 
  {
    *rate = 0;
  }
  else
  {
    device_ticks_per_sec = arad_chip_ticks_per_sec_get(unit);

    autocr_rate = soc_sand_power_of_2(autocr_rate_bits + ARAD_DBG_AUTOCREDIT_RATE_OFFSET);

    res = soc_sand_clocks_to_kbits_per_sec(
            autocr_rate,
            credit_worth,
            device_ticks_per_sec,
            rate      
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    *rate /= 1000;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dbg_autocredit_rate2rate()",0,0);
}





uint32
  arad_dbg_autocredit_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_DBG_AUTOCREDIT_INFO *info,
    SOC_SAND_OUT uint32                  *exact_rate
  )
{
  uint32
    fld_val,
    fld_val2,
    autocr_rate = 0,
    res;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DBG_AUTOCREDIT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_rate);

  

  if (info->first_queue > info->last_queue)
  {
    fld_val = 0;
    fld_val2 = ARAD_MAX_QUEUE_ID(unit);
  }
  else
  {
    fld_val = info->first_queue;
    fld_val2 = info->last_queue;
  }
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_AUTO_CREDIT_MECHANISM_FIRST_QUEUEr, SOC_CORE_ALL, 0, AUTO_CR_FRST_QUEf,  fld_val));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_AUTO_CREDIT_MECHANISM_LAST_QUEUEr, SOC_CORE_ALL, 0, AUTO_CR_LAST_QUEf,  fld_val2));
  
  if (info->rate == 0) 
  {
    autocr_rate = 0;
    *exact_rate = 0;
  }
  else
  {
    res = arad_dbg_rate2autocredit_rate(
            unit,
            info->rate,
            &autocr_rate
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    res = arad_dbg_autocredit_rate2rate(
            unit,
            autocr_rate,
            exact_rate
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
  }
  fld_val = autocr_rate;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_AUTO_CREDIT_MECHANISM_RATE_CONFIGURATIONr, SOC_CORE_ALL, 0, AUTO_CR_RATEf,  fld_val));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dbg_autocredit_set_unsafe()",0,0);
}



uint32
  arad_dbg_autocredit_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_DBG_AUTOCREDIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DBG_AUTOCREDIT_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->first_queue, ARAD_MAX_QUEUE_ID(unit),
    ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 10, exit
   );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->last_queue, ARAD_MAX_QUEUE_ID(unit),
    ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 20, exit
   );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dbg_autocredit_verify()",0,0);
}



uint32
  arad_dbg_autocredit_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_DBG_AUTOCREDIT_INFO *info
  )
{
  uint32
    fld_val,
    res;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DBG_AUTOCREDIT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_AUTO_CREDIT_MECHANISM_FIRST_QUEUEr, SOC_CORE_ALL, 0, AUTO_CR_FRST_QUEf, &info->first_queue));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_AUTO_CREDIT_MECHANISM_FIRST_QUEUEr, SOC_CORE_ALL, 0, AUTO_CR_LAST_QUEf, &info->last_queue));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_AUTO_CREDIT_MECHANISM_RATE_CONFIGURATIONr, SOC_CORE_ALL, 0, AUTO_CR_RATEf, &fld_val));

  if (fld_val == 0) 
  {
    info->rate = 0;
  }
  else
  {
    res = arad_dbg_autocredit_rate2rate(
            unit,
            fld_val,
            &(info->rate)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dbg_autocredit_get_unsafe()",0,0);
}



uint32
  arad_dbg_egress_shaping_enable_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  )
{
  uint32
    res,
    fld_val,
    reg_val;
  int core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DBG_EGRESS_SHAPING_ENABLE_SET_UNSAFE);

  fld_val = SOC_SAND_BOOL2NUM(enable);

 SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1, exit, ARAD_REG_ACCESS_ERR, READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, &reg_val));

      soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &reg_val, OTM_SPR_ENAf, fld_val);
      soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &reg_val, QPAIR_SPR_ENAf, fld_val);
      soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &reg_val, TCG_SPR_ENAf, fld_val);

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 2, exit, ARAD_REG_ACCESS_ERR, WRITE_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, reg_val));
 }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dbg_egress_shaping_enable_set_unsafe()",0,0);
}

uint32
  arad_dbg_egress_shaping_enable_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  )
{
  uint32
    res,
    fld_val,
    reg_val;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DBG_EGRESS_SHAPING_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(enable);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1, exit, ARAD_REG_ACCESS_ERR, READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, 0, &reg_val));
  fld_val = soc_reg_field_get(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, OTM_SPR_ENAf);  

  *enable = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dbg_egress_shaping_enable_get_unsafe()",0,0);
}



uint32
  arad_dbg_flow_control_enable_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8  enable
  )
{
  uint32
    res,
    fld_val;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DBG_FLOW_CONTROL_ENABLE_SET_UNSAFE);

  fld_val = SOC_SAND_BOOL2NUM(enable);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, CFC_ENf,  fld_val));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dbg_flow_control_enable_set_unsafe()",0,0);
}

uint32
  arad_dbg_flow_control_enable_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  )
{
  uint32
    res,
    fld_val;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DBG_FLOW_CONTROL_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(enable);

  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, CFC_ENf, &fld_val));
  *enable = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dbg_flow_control_enable_get_unsafe()",0,0);
}


uint32
  arad_dbg_ingr_reset_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DBG_INGR_RESET_UNSAFE);

  res = arad_dbg_dev_reset(
          unit,
          ARAD_DBG_RST_DOMAIN_INGR
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dbg_ingr_reset_unsafe()",0,0);
}


uint32 arad_dbg_dev_reset_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_DBG_RST_DOMAIN      rst_domain)
{
    uint32
        autogen_reg_val,
        fmc_scheduler_configs_reg_val_orig,
        fld32_val,
        res = SOC_SAND_OK;
    uint8
        is_traffic_enabled_orig,
        is_ctrl_cells_enabled_orig,
        dram_orig_enabled[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)];
    uint8
        is_ingr,
        is_egr,
        is_fabric = 0;
    uint64
        soft_init_reg_val,
        soft_init_reg_val_orig,
        soft_init_reg_val_orig_wo_clp,
        fld64_val;
#if defined(INCLUDE_KBP)
    ARAD_INIT_ELK *elk = &(SOC_DPP_CONFIG(unit)->arad->init.elk);
#endif

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DBG_DEV_RESET_UNSAFE);

    
    SCHAN_LOCK(unit);

    is_ingr   = SOC_SAND_NUM2BOOL((rst_domain == ARAD_DBG_RST_DOMAIN_INGR)            || 
                                  (rst_domain == ARAD_DBG_RST_DOMAIN_INGR_AND_FABRIC) || 
                                  (rst_domain == ARAD_DBG_RST_DOMAIN_FULL)            || 
                                  (rst_domain == ARAD_DBG_RST_DOMAIN_FULL_AND_FABRIC));
    is_egr    = SOC_SAND_NUM2BOOL((rst_domain == ARAD_DBG_RST_DOMAIN_EGR)            || 
                                  (rst_domain == ARAD_DBG_RST_DOMAIN_EGR_AND_FABRIC) || 
                                  (rst_domain == ARAD_DBG_RST_DOMAIN_FULL)           || 
                                  (rst_domain == ARAD_DBG_RST_DOMAIN_FULL_AND_FABRIC));
    is_fabric = SOC_SAND_NUM2BOOL((rst_domain == ARAD_DBG_RST_DOMAIN_INGR_AND_FABRIC) || 
                                  (rst_domain == ARAD_DBG_RST_DOMAIN_EGR_AND_FABRIC)  || 
                                  (rst_domain == ARAD_DBG_RST_DOMAIN_FULL_AND_FABRIC));

   LOG_VERBOSE(BSL_LS_SOC_INIT,
               (BSL_META_U(unit,
                           "%s(): Start. is_ingr=%d, is_egr=%d, is_fabric=%d\n"), FUNCTION_NAME(), is_ingr, is_egr, is_fabric));

    
    
    
    
    res = arad_mgmt_enable_traffic_get(unit, &is_traffic_enabled_orig);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   LOG_VERBOSE(BSL_LS_SOC_INIT,
               (BSL_META_U(unit,
                           "%s(): Disable Traffic. is_traffic_enabled_orig=%d\n"), FUNCTION_NAME(), is_traffic_enabled_orig));

    res = arad_mgmt_enable_traffic_set(unit, FALSE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    if (is_fabric == TRUE) {
        
        res = arad_mgmt_all_ctrl_cells_enable_get(unit, &is_ctrl_cells_enabled_orig);
        SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

        
        res = arad_mgmt_all_ctrl_cells_enable_set_unsafe(unit, FALSE, ARAD_MGMT_ALL_CTRL_CELLS_FLAGS_SOFT_RESET);
        SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
    }

    
    
    

    if (is_egr) {    

       LOG_VERBOSE(BSL_LS_SOC_INIT,
                   (BSL_META_U(unit,
                               "%s(): Validate Data Path is clean- egr.\n"), FUNCTION_NAME()));

        res = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, CGM_TOTAL_PD_CNTr , REG_PORT_ANY, 0, TOTAL_PD_CNTf, 0x0);
        if (soc_sand_update_error_code(res, &ex ) != no_err) {
           LOG_ERROR(BSL_LS_SOC_INIT,
                     (BSL_META_U(unit,
                                 "%s(): Error Validating Dtat Path is clean: CGM_TOTAL_PD_CNTr, TOTAL_PD_CNTf.\n"), FUNCTION_NAME()));
        }

        res = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, CGM_TOTAL_DB_CNTr, REG_PORT_ANY, 0, TOTAL_DB_CNTf, 0x0);
        if (soc_sand_update_error_code(res, &ex ) != no_err) {
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit,
                                  "%s(): Error Validating Dtat Path is clean: CGM_TOTAL_DB_CNTr, TOTAL_DB_CNTf.\n"), FUNCTION_NAME()));
         }
    }

    
    
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,READ_ECI_BLOCKS_SOFT_INITr_REG64(unit,&soft_init_reg_val));
    soft_init_reg_val_orig = soft_init_reg_val;
    soft_init_reg_val_orig_wo_clp = soft_init_reg_val;

   LOG_VERBOSE(BSL_LS_SOC_INIT,
               (BSL_META_U(unit,
                           "%s(): Read original configuration. soft_init_reg_val_orig=0x%x,0x%x\n"), FUNCTION_NAME(), COMPILER_64_HI(soft_init_reg_val_orig), COMPILER_64_LO(soft_init_reg_val_orig)));
    
    sal_memcpy(dram_orig_enabled, SOC_DPP_CONFIG(unit)->arad->init.dram.is_valid, sizeof(uint8) * SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max));

    
    
    
   LOG_VERBOSE(BSL_LS_SOC_INIT,
               (BSL_META_U(unit,
                           "%s(): IN-RESET\n"), FUNCTION_NAME()));
    COMPILER_64_SET(fld64_val,0,0x1);  
    if (is_ingr) {

        if (!SOC_IS_ARDON(unit)) {
            
            if (soc_mem_cache_get(unit, IHB_OPCODE_MAP_RXm, SOC_MEM_BLOCK_MIN(unit, IHB_OPCODE_MAP_RXm)) == FALSE) {
                if (soc_mem_cache_set(unit, IHB_OPCODE_MAP_RXm, COPYNO_ALL, TRUE)) {
                    LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Caching IHB_OPCODE_MAP_RXm for rewrite after soft reset not succeeded\n")));
                }
            }
            if (soc_mem_cache_get(unit, IHB_OPCODE_MAP_TXm, SOC_MEM_BLOCK_MIN(unit, IHB_OPCODE_MAP_TXm))== FALSE) {
                if (soc_mem_cache_set(unit, IHB_OPCODE_MAP_TXm, COPYNO_ALL, TRUE)) {
                    LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Caching IHB_OPCODE_MAP_TXm for rewrite after soft reset not succeeded\n")));
                }
            }
            if (SOC_IS_ARADPLUS(unit)) {
                
                if (soc_mem_cache_get(unit, OAMP_PE_PROG_TCAMm, SOC_MEM_BLOCK_MIN(unit, OAMP_PE_PROG_TCAMm)) == FALSE) {
                    if (soc_mem_cache_set(unit, OAMP_PE_PROG_TCAMm, COPYNO_ALL, TRUE)) {
                        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Caching OAMP_PE_PROG_TCAMm for rewrite after soft reset not succeeded\n")));
                    }
                }
            }
        }

        
        if (soc_mem_cache_get(unit, IRR_SMOOTH_DIVISIONm, SOC_MEM_BLOCK_MIN(unit, IRR_SMOOTH_DIVISIONm)) == FALSE) {
            if (soc_mem_cache_set(unit, IRR_SMOOTH_DIVISIONm, COPYNO_ALL, TRUE)) {
                LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Caching IRR_SMOOTH_DIVISIONm for rewrite after soft reset not succeeded\n")));
            }
        }

        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  22,  exit, ARAD_REG_ACCESS_ERR,READ_IPS_FMC_SCHEDULER_CONFIGSr(unit, &fmc_scheduler_configs_reg_val_orig));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  25,  exit, ARAD_REG_ACCESS_ERR,WRITE_IPS_FMC_SCHEDULER_CONFIGSr(unit,  0x04000000));
#if defined(INCLUDE_KBP)
        if (SOC_DPP_IS_ELK_ENABLE(unit)) {
            res = arad_kbp_recover_rx_shut_down(unit, elk->kbp_user_data[0].kbp_mdio_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);
        }
#endif
        
        
         
        COMPILER_64_TO_32_LO(fld32_val, fld64_val);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_IQM_ENABLERSr, SOC_CORE_ALL, 0, PDM_INIT_ENf,  fld32_val));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  34,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, ECI_BLOCKS_SOFT_RESETr, REG_PORT_ANY, 0, PDM_RESETf,  fld64_val));
        
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, IPS_INITf, fld64_val, soft_init_reg_val, 36, exit);
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, IQM_INITf, fld64_val, soft_init_reg_val, 38, exit);
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, IPT_INITf, fld64_val, soft_init_reg_val, 40, exit);
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, MMU_INITf, fld64_val, soft_init_reg_val, 42, exit);
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, OCB_INITf, fld64_val, soft_init_reg_val, 44, exit);
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, IRE_INITf, fld64_val, soft_init_reg_val, 46, exit);
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, IDR_INITf, fld64_val, soft_init_reg_val, 48, exit);
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, IRR_INITf, fld64_val, soft_init_reg_val, 50, exit);
        if (is_fabric == 0x1) {
            ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, FDT_INITf, fld64_val, soft_init_reg_val, 52, exit);
            ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, FCT_INITf, fld64_val, soft_init_reg_val, 54, exit);
        }
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, OAMP_INITf, fld64_val, soft_init_reg_val, 56, exit);
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, IHP_INITf, fld64_val, soft_init_reg_val, 58, exit);
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, IHB_INITf, fld64_val, soft_init_reg_val, 60, exit);
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, CFC_INITf, fld64_val, soft_init_reg_val, 62, exit);
     
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR, WRITE_ECI_BLOCKS_SOFT_INITr_REG64(unit, soft_init_reg_val));

        res = arad_mgmt_dram_init_drc_soft_init(unit, dram_orig_enabled, 0x1);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

         
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_CMIC_TXBUF_IPINTF_INTERFACE_CREDITSr(unit, 0x40));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_CMIC_TXBUF_IPINTF_INTERFACE_CREDITSr(unit, 0x0));
        
    } 

    if (is_egr) {

        
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1003,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_FL_STSf,  0x1));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1004,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_CRD_FCRf,  0x1));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1005,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_SRf,  0x1));
        
        
        res = arad_dbg_sch_reset_unsafe(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 1070, exit);

        COMPILER_64_SET(fld64_val,0,0x1);  

        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, EGQ_INITf, fld64_val, soft_init_reg_val, 1030, exit);
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, EPNI_INITf, fld64_val, soft_init_reg_val, 1031, exit);
        if (is_fabric == 0x1) {
            ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, FDR_INITf, fld64_val, soft_init_reg_val, 1032, exit);
            ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, FCR_INITf, fld64_val, soft_init_reg_val, 1034, exit);
        }
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, OLP_INITf, fld64_val, soft_init_reg_val, 1034, exit);
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, CFC_INITf, fld64_val, soft_init_reg_val, 39, exit);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1040,  exit, ARAD_REG_ACCESS_ERR,WRITE_ECI_BLOCKS_SOFT_INITr_REG64(unit, soft_init_reg_val));

          
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_CMIC_RXBUF_EPINTF_RELEASE_ALL_CREDITSr(unit, 0x0));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_CMIC_RXBUF_EPINTF_RELEASE_ALL_CREDITSr(unit, 0x1));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1026,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_INIT_TXI_CONFIGr, SOC_CORE_ALL, 0, INIT_TXI_CMICMf,  0x1));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1026,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_INIT_FQP_TXI_CMICMr, SOC_CORE_ALL, 0, INIT_FQP_TXI_CMICMf,  0x1));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1026,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_INIT_FQP_TXI_OLPr, SOC_CORE_ALL, 0, INIT_FQP_TXI_OLPf,  0x1));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1026,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_INIT_FQP_TXI_OAMr, SOC_CORE_ALL, 0, INIT_FQP_TXI_OAMf,  0x1));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1026,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_INIT_FQP_TXI_RCYr, SOC_CORE_ALL, 0, INIT_FQP_TXI_RCYf,  0x1));

    } 

    
    if (is_ingr && is_egr) {
        
        
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, CLP_0_INITf, fld64_val, soft_init_reg_val, 24, exit);
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, CLP_1_INITf, fld64_val, soft_init_reg_val, 25, exit);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  28,  exit, ARAD_REG_ACCESS_ERR, WRITE_ECI_BLOCKS_SOFT_INITr_REG64(unit, soft_init_reg_val));

        
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, NBI_INITf, fld64_val, soft_init_reg_val, 27, exit); 
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  22,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, ECI_BLOCKS_SOFT_INITr, REG_PORT_ANY, 0, NBI_INITf,  0x1));
    }

    
    
    
   LOG_VERBOSE(BSL_LS_SOC_INIT,
               (BSL_META_U(unit,
                           "%s(): OUT-OF-RESET.\n"), FUNCTION_NAME()));
    COMPILER_64_ZERO(fld64_val);
    if (is_ingr) {

         
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1026,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, ECI_BLOCKS_SOFT_RESETr, REG_PORT_ANY, 0, PDM_RESETf,  fld64_val));

        
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, IDR_INITf, fld64_val, soft_init_reg_val, 50, exit);
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, MMU_INITf, fld64_val, soft_init_reg_val, 52, exit);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,WRITE_ECI_BLOCKS_SOFT_INITr_REG64(unit, soft_init_reg_val));

      
        SOC_SAND_CHECK_FUNC_RESULT( arad_mgmt_set_mru_by_dbuff_size(unit), 120, exit) ;

        
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,READ_IDR_STATIC_CONFIGURATIONr(unit, &autogen_reg_val));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  62,  exit, ARAD_REG_ACCESS_ERR,WRITE_IDR_STATIC_CONFIGURATIONr(unit,  0x4));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  64,  exit, ARAD_REG_ACCESS_ERR,WRITE_IDR_STATIC_CONFIGURATIONr(unit,  autogen_reg_val));

        
        res = arad_mgmt_dram_init_drc_soft_init(unit, dram_orig_enabled, 0x0);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  2160,  exit, ARAD_REG_ACCESS_ERR,WRITE_IPS_FMC_SCHEDULER_CONFIGSr(unit,  fmc_scheduler_configs_reg_val_orig));
    } 

    
    if (is_ingr && is_egr) { 

        COMPILER_64_SET(fld64_val, 0, 0x1);
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, CLP_0_INITf, fld64_val, soft_init_reg_val_orig_wo_clp, 24, exit);
        ARAD_FLD_TO_REG64(ECI_BLOCKS_SOFT_INITr, CLP_1_INITf, fld64_val, soft_init_reg_val_orig_wo_clp, 25, exit);
    }

    
    
    
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,WRITE_ECI_BLOCKS_SOFT_INITr_REG64(unit, soft_init_reg_val_orig_wo_clp));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,WRITE_ECI_BLOCKS_SOFT_INITr_REG64(unit, soft_init_reg_val_orig));


    if (is_ingr) {
        
        if (!SOC_IS_ARDON(unit)) {
            arad_update_table_with_cache(unit, IHB_OPCODE_MAP_RXm);
            arad_update_table_with_cache(unit, IHB_OPCODE_MAP_TXm);
            if (SOC_IS_ARADPLUS(unit)) {
                arad_update_table_with_cache(unit, OAMP_PE_PROG_TCAMm);
            }
        }

        arad_update_table_with_cache(unit, IRR_SMOOTH_DIVISIONm);

#if defined(INCLUDE_KBP)
        
        if (SOC_DPP_IS_ELK_ENABLE(unit)) {
            res = arad_kbp_recover_rx_enable(unit, elk->kbp_user_data[0].kbp_mdio_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 29, exit);
            
            if (elk->kbp_recover_enable) {
                res = arad_kbp_recover_run_recovery_sequence(unit, 0, elk->kbp_user_data, elk->kbp_recover_iter, NULL, 1);
            }
            SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
        }
#endif
    }

    
    
    
    sal_usleep(1000);

   LOG_VERBOSE(BSL_LS_SOC_INIT,
               (BSL_META_U(unit,
                           "%s(): Validate/Poll for out-of-reset/init-done indications.\n"), FUNCTION_NAME()));
    res = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IPS_IPS_GENERAL_CONFIGURATIONSr, REG_PORT_ANY, 0, IPS_INIT_TRIGGERf, 0x0);
    if (soc_sand_update_error_code(res, &ex ) != no_err) {
       LOG_ERROR(BSL_LS_SOC_INIT,
                 (BSL_META_U(unit,
                             "%s(): Error Validate out-of-reset done indications: IPS_IPS_GENERAL_CONFIGURATIONSr, IPS_INIT_TRIGGERf.\n"), FUNCTION_NAME()));
    }

    res = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, REG_PORT_ANY, 0, PDM_INITf, 0x0);
    if (soc_sand_update_error_code(res, &ex ) != no_err) {
       LOG_ERROR(BSL_LS_SOC_INIT,
                 (BSL_META_U(unit,
                             "%s(): Error Validate out-of-reset done indications: IQM_IQM_INITr, IQC_INITf.\n"), FUNCTION_NAME()));
    }

    res = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, EGQ_EGQ_BLOCK_INIT_STATUSr, REG_PORT_ANY, 0, EGQ_BLOCK_INITf, 0x0);
    if (soc_sand_update_error_code(res, &ex ) != no_err) {
       LOG_ERROR(BSL_LS_SOC_INIT,
                 (BSL_META_U(unit,
                             "%s(): Error Validate out-of-reset done indications: EGQ_EGQ_BLOCK_INIT_STATUSr, EGQ_BLOCK_INITf.\n"), FUNCTION_NAME()));
    }

    if(is_egr) {

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  2003,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_FL_STSf,  0x0));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  2004,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_CRD_FCRf,  0x0));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  2005,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_SRf,  0x0));

       
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1026,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_INIT_TXI_CONFIGr, SOC_CORE_ALL, 0, INIT_TXI_CMICMf,  0x1));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1026,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_INIT_FQP_TXI_CMICMr, SOC_CORE_ALL, 0, INIT_FQP_TXI_CMICMf,  0x1));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1026,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_INIT_FQP_TXI_OLPr, SOC_CORE_ALL, 0, INIT_FQP_TXI_OLPf,  0x1));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1026,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_INIT_FQP_TXI_OAMr, SOC_CORE_ALL, 0, INIT_FQP_TXI_OAMf,  0x1));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1026,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_INIT_FQP_TXI_RCYr, SOC_CORE_ALL, 0, INIT_FQP_TXI_RCYf,  0x1));

    } 

    
    
    
    if (is_fabric == 0x1) {
        res = arad_mgmt_all_ctrl_cells_enable_set_unsafe(unit, is_ctrl_cells_enabled_orig, ARAD_MGMT_ALL_CTRL_CELLS_FLAGS_SOFT_RESET);
        SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
    }

    
    
    
    LOG_VERBOSE(BSL_LS_SOC_INIT,
                (BSL_META_U(unit,
                            "%s(): Restore traffic.\n"), FUNCTION_NAME()));
    res = arad_mgmt_enable_traffic_set(unit, is_traffic_enabled_orig);
    SOC_SAND_CHECK_FUNC_RESULT(res, 3000, exit);

    
    
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  3001,  exit, ARAD_REG_ACCESS_ERR,WRITE_IQM_INTERRUPT_REGISTERr(unit, SOC_CORE_ALL,  0xffffffff));

exit:
    SCHAN_UNLOCK(unit);
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_dbg_dev_reset_unsafe()", rst_domain, 0);
}

uint32 arad_dbg_dev_reset_verify(
    SOC_SAND_IN  ARAD_DBG_RST_DOMAIN      rst_domain)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_DBG_DEV_RESET_VERIFY);

    SOC_SAND_ERR_IF_ABOVE_MAX(rst_domain, ARAD_DEBUG_RST_DOMAIN_MAX, ARAD_DEBUG_RST_DOMAIN_OUT_OF_RANGE_ERR, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_dbg_dev_reset_verify()", rst_domain, 0);
}


uint32 arad_dbg_sch_reset_unsafe(
    SOC_SAND_IN  int unit)
{
    uint32
        mc_conf_0_fld_val,
        mc_conf_1_fld_val,
        ingr_shp_en_fld_val,
        res = SOC_SAND_OK;
    ARAD_SCH_SCHEDULER_INIT_TBL_DATA
        init_tbl;
    uint32 
        tbl_data[ARAD_SCH_SCHEDULER_INIT_TBL_ENTRY_SIZE] = {0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DBG_SCH_RESET_UNSAFE);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_DVS_CONFIG_1r, REG_PORT_ANY, 0, FORCE_PAUSEf,  0x1));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, SOC_CORE_ALL, 0, DISABLE_FABRIC_MSGSf,  0x1));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1006,  exit, ARAD_REG_ACCESS_ERR,READ_SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r(unit, SOC_CORE_ALL, &mc_conf_0_fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1006,  exit, ARAD_REG_ACCESS_ERR,READ_SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r(unit, SOC_CORE_ALL, &mc_conf_1_fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1026,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r, SOC_CORE_ALL, 0, MULTICAST_GFMC_ENABLEf,  0x0));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1026,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r, SOC_CORE_ALL, 0, MULTICAST_BFMC_1_ENABLEf,  0x0));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1026,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r, SOC_CORE_ALL, 0, MULTICAST_BFMC_2_ENABLEf,  0x0));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1026,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r, SOC_CORE_ALL, 0, MULTICAST_BFMC_3_ENABLEf,  0x0));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, SOC_CORE_ALL, 0, INGRESS_SHAPING_ENABLEf, &ingr_shp_en_fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  72,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, SOC_CORE_ALL, 0, INGRESS_SHAPING_ENABLEf,  0x0));

    
    init_tbl.schinit = 0x0;

    res = arad_sch_scheduler_init_tbl_set_unsafe(unit, 0x0,&init_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    soc_mem_field32_set(unit, SCH_MEM_01F00000m, tbl_data, ITEM_0_0f, 0x1);

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_DVS_CONFIG_1r, REG_PORT_ANY, 0, FORCE_PAUSEf,  0x0));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, SOC_CORE_ALL, 0, DISABLE_FABRIC_MSGSf,  0x0));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  2022,  exit, ARAD_REG_ACCESS_ERR,WRITE_SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r(unit, SOC_CORE_ALL,  mc_conf_0_fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  2022,  exit, ARAD_REG_ACCESS_ERR,WRITE_SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r(unit, SOC_CORE_ALL,  mc_conf_1_fld_val));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, SOC_CORE_ALL, 0, INGRESS_SHAPING_ENABLEf,  ingr_shp_en_fld_val));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_dbg_sch_reset_unsafe()", 0, 0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 

