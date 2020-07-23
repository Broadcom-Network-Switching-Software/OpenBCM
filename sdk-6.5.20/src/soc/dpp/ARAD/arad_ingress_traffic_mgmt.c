#include <shared/bsl.h>

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

#define _ERR_MSG_MODULE_NAME BSL_SOC_COSQ



#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/arad_ingress_traffic_mgmt.h>

#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h>

#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_chip_tbls.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <soc/dpp/ARAD/arad_parser.h>
#include <soc/mem.h>
#include <soc/dpp/ARAD/arad_init.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>

#include <soc/dpp/SAND/Utils/sand_u64.h>

#include <soc/dpp/SAND/Utils/sand_conv.h>











#define ARAD_ITM_MAX_INST_Q_BDS_SIZE 0x3F80000


#define ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN 0
#define ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MAX(unit) (SOC_DPP_DEFS_GET(unit, nof_queues)-1) 



#define ARAD_ITM_WRED_QT_DP_INFO_MAX_PROBABILITY_MAX 100



#define ARAD_ITM_WRED_MAX_PACKET_SIZE SOC_TMC_ITM_WRED_MAX_PACKET_SIZE
#define ARAD_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC SOC_TMC_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC


#define ARAD_ITM_WRED_MAX_PROB 100



#define ARAD_ITM_PRIORITY_MAP_NDX_MAX 3

#define ARAD_ITM_PRIORITY_MAP_TMPLT_MAP_MAX SOC_SAND_U32_MAX


#define ARAD_ITM_SYS_RED_DROP_PROB_VAL_MAX 0xffff

#define ARAD_ITM_SYS_RED_QUEUE_TH_MAX 15


#define ARAD_ITM_SYS_RED_EG_INFO_AGING_TIMER_MAX 0x1FFFFF


#define ARAD_ITM_SYS_RED_GLOB_RCS_THS_UNICAST_RNG_THS_MAX 0x1FFFFF

#define ARAD_ITM_SYS_RED_GLOB_RCS_THS_MULTICAST_RNG_THS_MAX 0xFFFF

#define ARAD_ITM_SYS_RED_GLOB_RCS_THS_BDS_RNG_THS_MAX 0x3FFFF

#define ARAD_ITM_SYS_RED_GLOB_RCS_THS_OCB_RNG_THS_MAX 0x3FFFF


#define ARAD_ITM_SYS_RED_GLOB_RCS_VALS_RNG_VALS_MAX 15

#define ARAD_ITM_CR_DISCOUNT_MAX_VAL  127
#define ARAD_ITM_CR_DISCOUNT_MIN_VAL  -127


#define ARAD_ITM_HUNGRY_TH_MNT_MSB(unit)      (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 6 : 9)
#define ARAD_ITM_HUNGRY_TH_MNT_LSB            4
#define ARAD_ITM_HUNGRY_TH_MNT_NOF_BITS(unit) (ARAD_ITM_HUNGRY_TH_MNT_MSB(unit) - ARAD_ITM_HUNGRY_TH_MNT_LSB + 1)
#define ARAD_ITM_HUNGRY_TH_EXP_MSB            3
#define ARAD_ITM_HUNGRY_TH_EXP_LSB            0
#define ARAD_ITM_HUNGRY_TH_EXP_NOF_BITS       (ARAD_ITM_HUNGRY_TH_EXP_MSB - ARAD_ITM_HUNGRY_TH_EXP_LSB + 1)

#define ARAD_ITM_HUNGRY_TH_MNT_MAX(unit) ((1 << ARAD_ITM_HUNGRY_TH_MNT_NOF_BITS(unit)) - 1)
#define ARAD_ITM_HUNGRY_TH_EXP_MAX       ((1 << ARAD_ITM_HUNGRY_TH_EXP_NOF_BITS) - 1)
#define ARAD_ITM_HUNGRY_TH_MAX(unit)     (ARAD_ITM_HUNGRY_TH_MNT_MAX(unit) * (1 << ARAD_ITM_HUNGRY_TH_EXP_MAX))

#define ARAD_ITM_HUNGRY_TH_MULTIPLIER_VAL_TO_FIELD(val)             \
  ((val) < (1 << (ARAD_ITM_HUNGRY_TH_MULTIPLIER_OFFSET + 1)) ? 0 : \
  soc_sand_log2_round_down(val) - ARAD_ITM_HUNGRY_TH_MULTIPLIER_OFFSET)
#define ARAD_ITM_HUNGRY_TH_MULTIPLIER_FIELD_TO_VAL(field)          \
  (field ? SOC_SAND_BIT(field + ARAD_ITM_HUNGRY_TH_MULTIPLIER_OFFSET) : 0);

#define ARAD_ITM_SATISFIED_TH_MNT_MSB       7
#define ARAD_ITM_SATISFIED_TH_MNT_LSB       4
#define ARAD_ITM_SATISFIED_TH_MNT_NOF_BITS  (ARAD_ITM_SATISFIED_TH_MNT_MSB - ARAD_ITM_SATISFIED_TH_MNT_LSB + 1)
#define ARAD_ITM_SATISFIED_TH_EXP_MSB       3
#define ARAD_ITM_SATISFIED_TH_EXP_LSB       0
#define ARAD_ITM_SATISFIED_TH_EXP_NOF_BITS  (ARAD_ITM_SATISFIED_TH_EXP_MSB - ARAD_ITM_SATISFIED_TH_EXP_LSB + 1)

#define ARAD_ITM_SATISFIED_TH_MNT_MAX     ((1 << ARAD_ITM_SATISFIED_TH_MNT_NOF_BITS) - 1)
#define ARAD_ITM_SATISFIED_TH_EXP_MAX     ((1 << ARAD_ITM_SATISFIED_TH_EXP_NOF_BITS) - 1)
#define ARAD_ITM_SATISFIED_TH_MAX         \
  (ARAD_ITM_SATISFIED_TH_MNT_MAX * (1<<ARAD_ITM_SATISFIED_TH_EXP_MAX))

#define ARAD_ITM_WRED_GRANULARITY   SOC_TMC_ITM_WRED_GRANULARITY
        
#define ARAD_ITM_MIN_WRED_AVRG_TH_MNT_MSB(unit)       (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 6 : 7)
#define ARAD_ITM_MIN_WRED_AVRG_TH_MNT_LSB             0
#define ARAD_ITM_MIN_WRED_AVRG_TH_MNT_NOF_BITS(unit)  (ARAD_ITM_MIN_WRED_AVRG_TH_MNT_MSB(unit) - ARAD_ITM_MIN_WRED_AVRG_TH_MNT_LSB + 1)
#define ARAD_ITM_MIN_WRED_AVRG_TH_EXP_MSB(unit)       (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 11 : 12)
#define ARAD_ITM_MIN_WRED_AVRG_TH_EXP_LSB(unit)       (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 7  : 8 )
#define ARAD_ITM_MIN_WRED_AVRG_TH_EXP_NOF_BITS(unit)  (ARAD_ITM_MIN_WRED_AVRG_TH_EXP_MSB(unit) - ARAD_ITM_MIN_WRED_AVRG_TH_EXP_LSB(unit) + 1)

#define ARAD_ITM_MAX_WRED_AVRG_TH_MNT_MSB(unit)       (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 6 : 7)
#define ARAD_ITM_MAX_WRED_AVRG_TH_MNT_LSB             0
#define ARAD_ITM_MAX_WRED_AVRG_TH_MNT_NOF_BITS(unit)  (ARAD_ITM_MAX_WRED_AVRG_TH_MNT_MSB(unit) - ARAD_ITM_MAX_WRED_AVRG_TH_MNT_LSB + 1)
#define ARAD_ITM_MAX_WRED_AVRG_TH_EXP_MSB(unit)       (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 11 : 12)
#define ARAD_ITM_MAX_WRED_AVRG_TH_EXP_LSB(unit)       (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 7  : 8 )
#define ARAD_ITM_MAX_WRED_AVRG_TH_EXP_NOF_BITS(unit)  (ARAD_ITM_MAX_WRED_AVRG_TH_EXP_MSB(unit) - ARAD_ITM_MAX_WRED_AVRG_TH_EXP_LSB(unit) + 1)


#define ARAD_WRED_NORMALIZE_FACTOR  SOC_TMC_WRED_NORMALIZE_FACTOR

#define ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_MSB(unit)       SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_MSB(unit)
#define ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB             SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB     
#define ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_NOF_BITS(unit)  SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_NOF_BITS(unit)
#define ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_MSB             SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_MSB
#define ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB(unit)       SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB(unit)
#define ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_NOF_BITS(unit)  SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_NOF_BITS(unit)

                                                                                                 
#define ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_MSB(unit)       SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_MSB(unit)
#define ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB             SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB     
#define ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_NOF_BITS(unit)  SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_NOF_BITS(unit)
#define ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_MSB             SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_MSB     
#define ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB(unit)       SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB(unit)     
#define ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_NOF_BITS(unit)  SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_NOF_BITS(unit)


#define ARAD_ITM_WD_INFO_MIN_SCAN_CYCLE_PERIOD_MICRO_GRANULARITY(unit)   (SOC_IS_QAX(unit) ? 2048 : 4096)
#define ARAD_ITM_CREDIT_WATCHDOG_MAX_THRESHOLD_VALUE 15



#define ARAD_ITM_SYS_RED_Q_BOUNDARY_BYTES_RESOLUTION 16

#define ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_MSB   6
#define ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_LSB   0
#define ARAD_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_MNT_NOF_BITS (ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_MSB - ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_LSB + 1)

#define ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_MSB   11
#define ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_LSB   7
#define ARAD_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_EXP_NOF_BITS (ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_MSB - ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_LSB + 1)

#define ARAD_ITM_TEST_CTGRY_TEST_EN_BIT          0
#define ARAD_ITM_TEST_CTGRY_TRFFC_TEST_EN_BIT    1
#define ARAD_ITM_TEST_CTGRY2_3_CNCTN_TEST_EN_BIT 2
#define ARAD_ITM_TEST_STTSTCS_TAG_TEST_EN_BIT    3
#define ARAD_ITM_TEST_LLFC_TEST_EN_BIT           4
#define ARAD_ITM_TEST_PFC_TEST_EN_BIT            5


#define ARAD_ITM_VSQ_FC_BD_SIZE_MAX              0X3FFFFF
#define ARAD_ITM_VSQ_FC_Q_SIZE_MAX               0XFFFFFFFF





#define ARAD_Q_WRED_INFO_MIN_AVRG_TH_MAX(unit)  (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 0x80000000 : (3 * 0x80000000))
#define ARAD_Q_WRED_INFO_MAX_AVRG_TH_MAX(unit)  (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 0x80000000 : (3 * 0x80000000))



#define ARAD_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX     SOC_TMC_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX


#define ARAD_ITM_GLOB_RCS_DROP_BDS_SIZE_MAX      SOC_TMC_ITM_GLOB_RCS_DROP_BDS_SIZE_MAX


#define ARAD_ITM_GLOB_RCS_DROP_UC_SIZE_MAX       SOC_TMC_ITM_GLOB_RCS_DROP_UC_SIZE_MAX


#define ARAD_ITM_GLOB_RCS_DROP_MINI_MC_SIZE_MAX  SOC_TMC_ITM_GLOB_RCS_DROP_MINI_MC_SIZE_MAX


#define ARAD_ITM_GLOB_RCS_DROP_FMC_SIZE_MAX      SOC_TMC_ITM_GLOB_RCS_DROP_FMC_SIZE_MAX


#define ARAD_ITM_GLOB_RCS_DROP_OCB_SIZE_MAX      SOC_TMC_ITM_GLOB_RCS_DROP_OCB_SIZE_MAX

#define ARAD_ITM_COMMITTED_BYTES_MANTISSA_NOF_BITS               6
#define ARAD_ITM_COMMITTED_BYTES_EXPONENT_NOF_BITS               5
#define ARAD_ITM_COMMITTED_BYTES_RESOLUTION                      16
#define ARAD_ITM_COMMITTED_BDS_MANTISSA_NOF_BITS                 8
#define ARAD_ITM_COMMITTED_BDS_EXPONENT_NOF_BITS                 4


#define ARAD_ITM_MC_HP_FIFO_DP_0_THRESHOLD(limit)                6
#define ARAD_ITM_MC_HP_FIFO_DP_1_THRESHOLD(limit)                (6 + ((limit) - 6) / 4)
#define ARAD_ITM_MC_HP_FIFO_DP_2_THRESHOLD(limit)                (6 + ((limit) - 6) / 2)
#define ARAD_ITM_MC_HP_FIFO_DP_3_THRESHOLD(limit)                (6 + ((limit) - 6) * 3 / 4)
#define ARAD_ITM_MC_LP_FIFO_DP_0_THRESHOLD(limit)                6
#define ARAD_ITM_MC_LP_FIFO_DP_1_THRESHOLD(limit)                (6 + ((limit) - 6) / 4)
#define ARAD_ITM_MC_LP_FIFO_DP_2_THRESHOLD(limit)                (6 + ((limit) - 6) / 2)
#define ARAD_ITM_MC_LP_FIFO_DP_3_THRESHOLD(limit)                (6 + ((limit) - 6) * 3 / 4)

#define ARAD_ITM_NOF_REASSEMBLY_CONTEXTS                         (192)


#define ARAD_CREDIT_WATCHDOG_MIN_Q_SCAN_CYCLES 2
#define ARAD_CREDIT_WATCHDOG_MAX_Q_SCAN_CYCLES 255


#define NS_TO_MS_ROUND(nano) (((nano) + 50000) / 1000000); 























STATIC uint32
  arad_itm_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    start_prev,
    size_prev,
    i,
    pfc_tc,
    data,
    res;
  uint32
    reg_idx;
  char 
    *propkey,
    *propval;
  uint32 
    ftmh_stmping; 
  uint8
    is_mesh,
    is_single_cntxt;
  soc_field_t
      dqcq_field_1[ARAD_REGS_DBUFF_PTR_Q_THRESH_NOF_REGS] = 
      {DPQ_DQCQ_TH_1f, DPQ_DQCQ_TH_3f, DPQ_DQCQ_TH_11f, DPQ_DQCQ_TH_13f, DPQ_DQCQ_TH_15f, 
          DPQ_DQCQ_TH_5f, DPQ_DQCQ_TH_7f, DPQ_DQCQ_TH_9f};
   
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_REGS_INIT);

  

  

  
  if (SOC_DPP_CONFIG(unit)->arad->init.dram.pdm_mode == ARAD_INIT_PDM_MODE_REDUCED) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  9,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IQM_REG_0124r, REG_PORT_ANY, 0, FIELD_7_11f,  0xf));
  } else {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  9,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IQM_REG_0124r, REG_PORT_ANY, 0, FIELD_7_11f,  0xe));
  }

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,WRITE_IQM_REJECT_ADMISSION_Ar(unit, 0x3F3F3F3F));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,WRITE_IQM_REJECT_ADMISSION_Br(unit,  0x3F3F3F3F));

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR,READ_IQM_STE_ENABLERSr(unit, SOC_CORE_ALL, &data));
  data |= 0x3F; 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,WRITE_IQM_STE_ENABLERSr(unit, SOC_CORE_ALL,  data));

  
  for(i=0; i< ARAD_NOF_LOCAL_PORTS(unit); ++i)
  {
    pfc_tc = i & (ARAD_NOF_TRAFFIC_CLASSES - 1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, WRITE_IQM_NIFTCMm(unit, MEM_BLOCK_ANY, i, &pfc_tc));
  }

  
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  42,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, REG_PORT_ANY, 0, STAMP_FAP_PORTf,  0x1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  44,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, REG_PORT_ANY, 0, STAMP_FWDACTIONf,  0x1));
  if (SOC_DPP_CONFIG(unit)->arad->init.mirror_stamp_sys_dsp_ext) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  45,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, REG_PORT_ANY, 0, STAMP_DSP_EXTf,  0)); 
  }
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  46,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_STAMPING_FTMH_OUTLIF_ENABLEr, REG_PORT_ANY, 0, STAMP_FTMH_OUTLIF_TYPE_ENf,  0x7775));

  
    propkey = spn_XGS_COMPATABILITY_STAMP_CUD;
    propval = soc_property_get_str(unit, propkey);

    if (propval) {
        if (sal_strcmp(propval, "True") == 0) {
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  45,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_ARAD_PACKETS_FORMATr, REG_PORT_ANY, 0, STAMP_OUTLIF_XGS_USR_DEF_ENf,  0x1));
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  46,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, REG_PORT_ANY, 0, STAMP_USER_DEFINED_LSBf,  512-72-48));
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  47,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, REG_PORT_ANY, 0, STAMP_USER_DEFINED_MSBf,  16));
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  48,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_STAMPING_USR_DEF_OUTLIF_ENABLEr, REG_PORT_ANY, 0, STAMP_USR_DEF_OUTLIF_TYPE_ENf,  0x0));

            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  49,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_STAMPING_FTMH_OUTLIF_ENABLEr, REG_PORT_ANY, 0, STAMP_FTMH_OUTLIF_TYPE_ENf, &ftmh_stmping));
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_STAMPING_FTMH_OUTLIF_ENABLEr, REG_PORT_ANY, 0, STAMP_FTMH_OUTLIF_TYPE_ENf,  ftmh_stmping & ~0x3));            
          
        } else if (sal_strcmp(propval, "False") == 0) {
                    
        } else {            
            return SOC_E_FAIL;
        }
    }

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_MAPPING_QUEUE_TYPE_TO_SNOOP_PACKETr, REG_PORT_ANY, 0, SNOOP_TABLEf,  SOC_SAND_BIT(ARAD_ITM_FWD_ACTION_TYPE_SNOOP)));

  is_mesh = SOC_SAND_NUM2BOOL(SOC_DPP_IS_MESH((unit)));
  is_single_cntxt = SOC_SAND_NUM2BOOL(!is_mesh);

  if (is_single_cntxt)
  {
    

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG_1r, REG_PORT_ANY, 0, DEST_0_DEPTHf,  0x200));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  81,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG_1r, REG_PORT_ANY, 0, DEST_1_DEPTHf,  0x200));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  83,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG_2r, REG_PORT_ANY, 0, DEST_2_DEPTHf,  0x200));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  84,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG_2r, REG_PORT_ANY, 0, DEST_3_DEPTHf,  0x200));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  85,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG_3r, REG_PORT_ANY, 0, DEST_4_DEPTHf,  0x0));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  86,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG_3r, REG_PORT_ANY, 0, DEST_5_DEPTHf,  0x0));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  87,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG_4r, REG_PORT_ANY, 0, DEST_6_DEPTHf,  0x0));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  88,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG_4r, REG_PORT_ANY, 0, DEST_7_DEPTHf,  0x0));

    
    start_prev = 0x0;
    size_prev  = 0x190;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_TRANSMIT_DATA_QUEUE_START_ADRESS_0_1r, REG_PORT_ANY,  0, DTQ_START_0f + 0,  start_prev));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  111,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_TRANSMIT_DATA_QUEUE_SIZE_0_1r, REG_PORT_ANY,  0, DTQ_SIZE_0f + 0,  size_prev));

    start_prev = start_prev + size_prev + 1;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  112,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_TRANSMIT_DATA_QUEUE_START_ADRESS_0_1r, REG_PORT_ANY,  0, DTQ_START_0f + 1,  start_prev));
    size_prev = 0x10;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,   113,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_TRANSMIT_DATA_QUEUE_SIZE_0_1r, REG_PORT_ANY,  0, DTQ_SIZE_0f + 1,  size_prev));

    for (reg_idx = 1; reg_idx < ARAD_TRANSMIT_DATA_QUEUE_NOF_REGS; reg_idx++)
    {
        start_prev = start_prev + size_prev + 1;
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  114,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_TRANSMIT_DATA_QUEUE_START_ADRESS_0_1r + ((reg_idx != ARAD_TRANSMIT_DATA_QUEUE_NOF_REGS-1)?(reg_idx / 2):(-1)), REG_PORT_ANY,  0, DTQ_START_0f + reg_idx,  start_prev));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  116+reg_idx,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_TRANSMIT_DATA_QUEUE_SIZE_0_1r + ((reg_idx != ARAD_TRANSMIT_DATA_QUEUE_NOF_REGS-1)?(reg_idx / 2):(-1)), REG_PORT_ANY,  0, DTQ_SIZE_0f + reg_idx,  size_prev));
    }
  }
  else
  {
    if (is_mesh == TRUE)
    {
      for (reg_idx = 0; reg_idx < ARAD_REGS_DBUFF_PTR_Q_THRESH_NOF_REGS; reg_idx++)
      {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100+reg_idx,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_01r + reg_idx, REG_PORT_ANY, 0, dqcq_field_1[reg_idx],  0x7f)); 
      }
    }
  }

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_STORED_CREDITS_USAGE_CONFIGURATIONr, SOC_CORE_ALL, 0, MUL_PKT_DEQ_BYTESf,  16));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_2r, REG_PORT_ANY, 0, ISP_QNUM_LOWf,  0x17fef)); 
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  152, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_3r, REG_PORT_ANY, 0, ISP_QNUM_HIGHf,  0x17fef));


  
  data = 0;
  res = arad_fill_table_with_entry(unit, IQM_ITMPMm, MEM_BLOCK_ANY, &data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) 
  {
      uint32 
          mcr_limit_uc,
          mcr_limit_mc_hp,
          mcr_limit_mc_hp_size,
          mcr_limit_mc_lp_size;
      soc_reg_above_64_val_t 
          fld_above_64_val, 
          reg_above_64_val;
      SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
      SOC_REG_ABOVE_64_CLEAR(fld_above_64_val);

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRR_COMPATIBILITY_REGISTERr, REG_PORT_ANY, 0, SYS_CONFIG_2f,  0x1));

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  190,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IRR_MCR_FIFO_CONFIGr, REG_PORT_ANY, 0, MCR_LIMIT_UCf, &mcr_limit_uc));    
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  191,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IRR_MCR_FIFO_CONFIGr, REG_PORT_ANY, 0, MCR_LIMIT_MC_HPf, &mcr_limit_mc_hp));
      mcr_limit_mc_hp_size = mcr_limit_mc_hp - mcr_limit_uc;
      mcr_limit_mc_lp_size = (768 - mcr_limit_mc_hp);
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 200, exit, ARAD_REG_ACCESS_ERR, READ_IRR_MULTICAST_FIFO_THRESHOLDr(unit, reg_above_64_val));
      
      ARAD_FLD_FROM_REG_ABOVE_64(IRR_MULTICAST_FIFO_THRESHOLDr, MC_HP_FIFO_DP_0_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 200, exit);
      *fld_above_64_val = ARAD_ITM_MC_HP_FIFO_DP_0_THRESHOLD(mcr_limit_mc_hp_size);
      ARAD_FLD_TO_REG_ABOVE_64(  IRR_MULTICAST_FIFO_THRESHOLDr, MC_HP_FIFO_DP_0_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 201, exit);

      ARAD_FLD_FROM_REG_ABOVE_64(IRR_MULTICAST_FIFO_THRESHOLDr, MC_HP_FIFO_DP_1_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 210, exit);
      *fld_above_64_val = ARAD_ITM_MC_HP_FIFO_DP_1_THRESHOLD(mcr_limit_mc_hp_size);
      ARAD_FLD_TO_REG_ABOVE_64(  IRR_MULTICAST_FIFO_THRESHOLDr, MC_HP_FIFO_DP_1_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 211, exit);

      ARAD_FLD_FROM_REG_ABOVE_64(IRR_MULTICAST_FIFO_THRESHOLDr, MC_HP_FIFO_DP_2_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 220, exit);
      *fld_above_64_val = ARAD_ITM_MC_HP_FIFO_DP_2_THRESHOLD(mcr_limit_mc_hp_size);
      ARAD_FLD_TO_REG_ABOVE_64(  IRR_MULTICAST_FIFO_THRESHOLDr, MC_HP_FIFO_DP_2_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 221, exit);

      ARAD_FLD_FROM_REG_ABOVE_64(IRR_MULTICAST_FIFO_THRESHOLDr, MC_HP_FIFO_DP_3_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 230, exit);
      *fld_above_64_val = ARAD_ITM_MC_HP_FIFO_DP_3_THRESHOLD(mcr_limit_mc_hp_size);
      ARAD_FLD_TO_REG_ABOVE_64(  IRR_MULTICAST_FIFO_THRESHOLDr, MC_HP_FIFO_DP_3_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 231, exit);

      ARAD_FLD_FROM_REG_ABOVE_64(IRR_MULTICAST_FIFO_THRESHOLDr, MC_LP_FIFO_DP_0_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 240, exit);
      *fld_above_64_val = ARAD_ITM_MC_LP_FIFO_DP_0_THRESHOLD(mcr_limit_mc_lp_size);
      ARAD_FLD_TO_REG_ABOVE_64(  IRR_MULTICAST_FIFO_THRESHOLDr, MC_LP_FIFO_DP_0_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 241, exit);

      ARAD_FLD_FROM_REG_ABOVE_64(IRR_MULTICAST_FIFO_THRESHOLDr, MC_LP_FIFO_DP_1_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 250, exit);
      *fld_above_64_val = ARAD_ITM_MC_LP_FIFO_DP_1_THRESHOLD(mcr_limit_mc_lp_size);
      ARAD_FLD_TO_REG_ABOVE_64(  IRR_MULTICAST_FIFO_THRESHOLDr, MC_LP_FIFO_DP_1_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 251, exit);

      ARAD_FLD_FROM_REG_ABOVE_64(IRR_MULTICAST_FIFO_THRESHOLDr, MC_LP_FIFO_DP_2_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 260, exit);
      *fld_above_64_val = ARAD_ITM_MC_LP_FIFO_DP_2_THRESHOLD(mcr_limit_mc_lp_size);
      ARAD_FLD_TO_REG_ABOVE_64(  IRR_MULTICAST_FIFO_THRESHOLDr, MC_LP_FIFO_DP_2_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 261, exit);

      ARAD_FLD_FROM_REG_ABOVE_64(IRR_MULTICAST_FIFO_THRESHOLDr, MC_LP_FIFO_DP_3_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 270, exit);
      *fld_above_64_val = ARAD_ITM_MC_LP_FIFO_DP_3_THRESHOLD(mcr_limit_mc_lp_size);
      ARAD_FLD_TO_REG_ABOVE_64(  IRR_MULTICAST_FIFO_THRESHOLDr, MC_LP_FIFO_DP_3_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 271, exit);

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 300, exit, ARAD_REG_ACCESS_ERR, WRITE_IRR_MULTICAST_FIFO_THRESHOLDr(unit, reg_above_64_val));
  }
#endif 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_regs_init()",0,0);
}


uint32
  arad_itm_init(
    SOC_SAND_IN  int  unit
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_INIT_PDM_MODE         
      pdm_mode;
  uint32
    pdm_nof_entries,
    pdm_mode_fld,
	mem_excess_size,
    idx;
  ARAD_ITM_GLOB_RCS_DROP_TH
    glbl_drop, glbl_drop_exact;
  ARAD_ITM_GLOB_RCS_FC_TH
    glbl_fc, glbl_fc_exact;
  ARAD_ITM_VSQ_GROUP
    vsq_group;
  ARAD_ITM_VSQ_FC_INFO
    vsq_fc_info,
    exact_vsq_fc_info;
  ARAD_MGMT_INIT *init = &(SOC_DPP_CONFIG(unit)->arad->init);

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_INIT);
  res = arad_itm_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  arad_ARAD_ITM_GLOB_RCS_DROP_TH_clear(&glbl_drop);
  arad_ARAD_ITM_GLOB_RCS_DROP_TH_clear(&glbl_drop_exact);
  arad_ARAD_ITM_GLOB_RCS_FC_TH_clear(&glbl_fc);
  arad_ARAD_ITM_GLOB_RCS_FC_TH_clear(&glbl_fc_exact);
  arad_ARAD_ITM_VSQ_FC_INFO_clear(&vsq_fc_info);
  arad_ARAD_ITM_VSQ_FC_INFO_clear(&exact_vsq_fc_info);

  
  if ((init->ocb.ocb_enable != OCB_ONLY) && ((init->ocb.ocb_enable != OCB_ENABLED) || (init->dram.nof_drams != 0))){
      glbl_fc.bdbs.hp.set       = 256;
      glbl_fc.bdbs.hp.clear     = 1024;
      glbl_fc.bdbs.lp.set       = 768;
      glbl_fc.bdbs.lp.clear     = 1500;
      glbl_fc.unicast.hp.set    = 672;
      glbl_fc.unicast.hp.clear  = 1344;
      glbl_fc.unicast.lp.set    = 10752;
      glbl_fc.unicast.lp.clear  = 43008;
      glbl_fc.full_mc.hp.set    = 416;
      glbl_fc.full_mc.hp.clear  = 832;
      glbl_fc.full_mc.lp.set    = 672;
      glbl_fc.full_mc.lp.clear  = 1344;
  }

  res = arad_itm_glob_rcs_fc_set_unsafe(
          unit,
          &glbl_fc,
          &glbl_fc_exact
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  glbl_drop.bdbs[0].set       = 128;
  glbl_drop.bdbs[0].clear     = 512;
  for (idx = 1; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
  {
    glbl_drop.bdbs[idx].set   = 256;
    glbl_drop.bdbs[idx].clear = 1024;
  }

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  36,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_IQM_ENABLERSr, SOC_CORE_ALL, 0, VSQ_CD_ENf, &pdm_mode_fld));
  pdm_mode = (pdm_mode_fld)? ARAD_INIT_PDM_MODE_REDUCED:ARAD_INIT_PDM_MODE_SIMPLE;
  res = arad_init_pdm_nof_entries_calc(
            unit,
            pdm_mode,
            &pdm_nof_entries
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);


  for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
  {
    glbl_drop.bds[idx].set   = 1008 * (pdm_nof_entries / 1024); 
    glbl_drop.bds[idx].clear =  928 * (pdm_nof_entries / 1024); 
  }

  
  if ((init->ocb.ocb_enable != OCB_ONLY) && ((init->ocb.ocb_enable != OCB_ENABLED) || (init->dram.nof_drams != 0))){

      for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
      {
        glbl_drop.unicast[idx].set   = 496;
        glbl_drop.unicast[idx].clear = 672;
      }

      for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
      {
        glbl_drop.full_mc[idx].set   = 160;
        glbl_drop.full_mc[idx].clear = 320;
      }

      for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
      {
        glbl_drop.mini_mc[idx].set   = 160;
        glbl_drop.mini_mc[idx].clear = 320;
      }

      for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
      {
          glbl_drop.mem_excess[idx].set   = ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX;
          glbl_drop.mem_excess[idx].clear = ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX - (ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX/8);
      }
  } else {
      mem_excess_size = (((SOC_DPP_DEFS_GET(unit, ocb_memory_size) * 1024 * 1024 ) / 8 ) / 128 );
      for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++) {
          glbl_drop.mem_excess[idx].set   = mem_excess_size;
          glbl_drop.mem_excess[idx].clear = mem_excess_size - (mem_excess_size / 8);
      }
  }

  res = arad_itm_glob_rcs_drop_set_unsafe(
          unit,
          &glbl_drop,
          &glbl_drop_exact
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  vsq_fc_info.bd_size_fc.set = ARAD_ITM_VSQ_FC_BD_SIZE_MAX;
  vsq_fc_info.bd_size_fc.clear = ARAD_ITM_VSQ_FC_BD_SIZE_MAX;
  vsq_fc_info.q_size_fc.set  = ARAD_ITM_VSQ_FC_Q_SIZE_MAX;
  vsq_fc_info.q_size_fc.clear = ARAD_ITM_VSQ_FC_Q_SIZE_MAX;

  for (vsq_group = 0; vsq_group < ARAD_ITM_VSQ_GROUP_LAST; vsq_group++) 
  {
       res = arad_itm_vsq_fc_set_unsafe(
          unit,
          vsq_group,
          0, 
          &vsq_fc_info,
          &exact_vsq_fc_info
       );
       SOC_SAND_CHECK_FUNC_RESULT(res, 50+vsq_group, exit);
  }
 
  

  res = arad_itm_setup_dp_map(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_init()",0,0);
}

uint32
arad_itm_vsq_in_group_size_get(
                SOC_SAND_IN  int                      unit, 
                SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx, 
                SOC_SAND_OUT uint32                   *vsq_in_group_size)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(vsq_in_group_size);
  switch(vsq_group_ndx)
  {
  case ARAD_ITM_VSQ_GROUP_CTGRY:
    *vsq_in_group_size = ARAD_ITM_VSQ_GROUPA_SZE(unit);
    break;
  case ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
    *vsq_in_group_size = ARAD_ITM_VSQ_GROUPB_SZE(unit);
    break;
  case ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
    *vsq_in_group_size = ARAD_ITM_VSQ_GROUPC_SZE(unit);
    break;
  case ARAD_ITM_VSQ_GROUP_STTSTCS_TAG:
    *vsq_in_group_size = ARAD_ITM_VSQ_GROUPD_SZE(unit);
    break;
  case ARAD_ITM_VSQ_GROUP_SRC_PORT:
    *vsq_in_group_size = ARAD_ITM_VSQ_GROUPE_SZE(unit);
    break;
  case ARAD_ITM_VSQ_GROUP_PG:
    *vsq_in_group_size = ARAD_ITM_VSQ_GROUPF_SZE(unit);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_vsq_idx_max_size()", vsq_group_ndx, 0);

}
uint32
  arad_itm_vsq_idx_verify(
     SOC_SAND_IN  int                      unit,
     SOC_SAND_IN  uint8                    is_cob_only,
     SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
     SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx
  )
{
  uint32
    res,
    vsq_in_group_size;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_IDX_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, ARAD_ITM_VSQ_GROUP_LAST - 1,
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 10, exit
  );
  SOC_SAND_ERR_IF_ABOVE_MAX(
    is_cob_only, SOC_DPP_DEFS_GET(unit, vsq_ocb_only_support),
    SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 15, exit
  );
  res = arad_itm_vsq_in_group_size_get(unit, vsq_group_ndx, &vsq_in_group_size);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  switch(vsq_group_ndx)
  {
  case ARAD_ITM_VSQ_GROUP_CTGRY:
    SOC_SAND_ERR_IF_ABOVE_MAX(vsq_in_group_ndx, vsq_in_group_size - 1, ARAD_ITM_VSQ_NDX_OUT_OF_RANGE_ERR, 30, exit);
    break;
  case ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
    SOC_SAND_ERR_IF_ABOVE_MAX(vsq_in_group_ndx, vsq_in_group_size - 1, ARAD_ITM_VSQ_NDX_OUT_OF_RANGE_ERR, 32, exit);
    break;
  case ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
    SOC_SAND_ERR_IF_ABOVE_MAX(vsq_in_group_ndx, vsq_in_group_size - 1, ARAD_ITM_VSQ_NDX_OUT_OF_RANGE_ERR, 34, exit);
    break;
  case ARAD_ITM_VSQ_GROUP_STTSTCS_TAG:
    SOC_SAND_ERR_IF_ABOVE_MAX(vsq_in_group_ndx, vsq_in_group_size - 1, ARAD_ITM_VSQ_NDX_OUT_OF_RANGE_ERR, 36, exit);
    break;
  case ARAD_ITM_VSQ_GROUP_SRC_PORT:
    SOC_SAND_ERR_IF_ABOVE_MAX(vsq_in_group_ndx, vsq_in_group_size - 1, ARAD_ITM_VSQ_NDX_OUT_OF_RANGE_ERR, 38, exit);
    break;
  case ARAD_ITM_VSQ_GROUP_PG:
    SOC_SAND_ERR_IF_ABOVE_MAX(vsq_in_group_ndx, vsq_in_group_size - 1, ARAD_ITM_VSQ_NDX_OUT_OF_RANGE_ERR, 40, exit);
    break;
  default:
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_vsq_idx_verify()",vsq_group_ndx,vsq_in_group_ndx);
}


uint32
  arad_itm_man_exp_buffer_set(
    SOC_SAND_IN  int32  value,
    SOC_SAND_IN  uint32 mnt_lsb,
    SOC_SAND_IN  uint32 mnt_nof_bits,
    SOC_SAND_IN  uint32 exp_lsb,
    SOC_SAND_IN  uint32 exp_nof_bits,
    SOC_SAND_IN  uint8 is_signed,
    SOC_SAND_OUT uint32  *output_field,
    SOC_SAND_OUT int32  *exact_value
  )
{
  uint32
    res,
    sign,
    rslt,
    mnt_lsb_field_aligned,
    exp_lsb_field_aligned,
    sign_bit_field_aligned,
    temp_val;
  uint32
    mnt,
    exp;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_MAN_EXP_BUFFER_SET);

  res = soc_sand_break_to_mnt_exp_round_up(
          soc_sand_abs(value),
          mnt_nof_bits,
          exp_nof_bits,
          0,
          &mnt,
          &exp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  rslt = 0;

  
  if (mnt_lsb < exp_lsb)
  {
    mnt_lsb_field_aligned = 0;
    exp_lsb_field_aligned = mnt_nof_bits;
    sign_bit_field_aligned = mnt_nof_bits + exp_nof_bits;
  }
  else
  {
    exp_lsb_field_aligned = 0;
    mnt_lsb_field_aligned = exp_nof_bits;
    sign_bit_field_aligned = mnt_nof_bits + exp_nof_bits;
  }

  temp_val = mnt;
  res = soc_sand_bitstream_set_any_field(
          &temp_val,
          mnt_lsb_field_aligned,
          mnt_nof_bits,
          &rslt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  temp_val = exp;
  res = soc_sand_bitstream_set_any_field(
          &temp_val,
          exp_lsb_field_aligned,
          exp_nof_bits,
          &rslt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  sign = (value < 0);
  if (is_signed)
  {
    res = soc_sand_bitstream_set_any_field(
            &sign,
            sign_bit_field_aligned,
            1,
            &rslt
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  *output_field = rslt;

  *exact_value = (mnt * (1<<exp)) * (sign ? -1 : 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_man_exp_buffer_set()",0,0);
}

uint32
  arad_itm_man_exp_buffer_get(
    SOC_SAND_IN  uint32  buffer,
    SOC_SAND_IN  uint32 mnt_lsb,
    SOC_SAND_IN  uint32 mnt_nof_bits,
    SOC_SAND_IN  uint32 exp_lsb,
    SOC_SAND_IN  uint32 exp_nof_bits,
    SOC_SAND_IN  uint8 is_signed,
    SOC_SAND_OUT int32  *value
  )
{
  uint32
    res,
    mnt    = 0,
    exp    = 0,
    sign   = 0,
    mnt_lsb_field_aligned,
    exp_lsb_field_aligned,
    sign_bit_field_aligned;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_MAN_EXP_BUFFER_GET);

  
  if (mnt_lsb < exp_lsb)
  {
    mnt_lsb_field_aligned = 0;
    exp_lsb_field_aligned = mnt_nof_bits;
    sign_bit_field_aligned = mnt_nof_bits + exp_nof_bits;
  }
  else
  {
    exp_lsb_field_aligned = 0;
    mnt_lsb_field_aligned = exp_nof_bits;
    sign_bit_field_aligned = mnt_nof_bits + exp_nof_bits;
  }

  res = soc_sand_bitstream_get_any_field(
          &buffer,
          mnt_lsb_field_aligned,
          mnt_nof_bits,
          &mnt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_get_any_field(
          &buffer,
          exp_lsb_field_aligned,
          exp_nof_bits,
          &exp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  sign = 0;
  if (is_signed)
  {
    res = soc_sand_bitstream_get_any_field(
            &buffer,
            sign_bit_field_aligned,
            1,
            &sign
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  *value = (mnt * (1<<exp)) * (sign ? -1 : 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_man_exp_buffer_get()",0,0);
}




uint32
  arad_itm_convert_admit_one_test_tmplt_to_u32(
    SOC_SAND_IN ARAD_ITM_ADMIT_ONE_TEST_TMPLT test,
    SOC_SAND_OUT uint32                       *test_in_sand_u32
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_CONVERT_ADMIT_ONE_TEST_TMPLT_TO_U32);
  SOC_SAND_CHECK_NULL_INPUT(test_in_sand_u32);

  *test_in_sand_u32 =
    (test.ctgry_test_en==TRUE ? 0:1) |
    ((test.ctgry_trffc_test_en==TRUE ? 0:1) << ARAD_ITM_TEST_CTGRY_TRFFC_TEST_EN_BIT)|
    ((test.ctgry2_3_cnctn_test_en==TRUE ? 0:1) << ARAD_ITM_TEST_CTGRY2_3_CNCTN_TEST_EN_BIT)|
    ((test.sttstcs_tag_test_en==TRUE ? 0:1) << ARAD_ITM_TEST_STTSTCS_TAG_TEST_EN_BIT)|
    ((test.pfc_test_en==TRUE ? 0:1) << ARAD_ITM_TEST_PFC_TEST_EN_BIT)|
    ((test.llfc_test_en==TRUE ? 0:1) << ARAD_ITM_TEST_LLFC_TEST_EN_BIT);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_convert_admit_one_test_tmplt_to_u32()",0,0);
}

uint32
  arad_itm_convert_u32_to_admit_one_test_tmplt(
    SOC_SAND_IN  uint32                       test_in_sand_u32,
    SOC_SAND_OUT ARAD_ITM_ADMIT_ONE_TEST_TMPLT *test
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_CONVERT_U32_TO_ADMIT_ONE_TEST_TMPLT);
  SOC_SAND_CHECK_NULL_INPUT(test);

  test->ctgry_test_en =
    ((test_in_sand_u32 >> ARAD_ITM_TEST_CTGRY_TEST_EN_BIT) & 1) ? FALSE : TRUE;
  test->ctgry_trffc_test_en =
    ((test_in_sand_u32 >> ARAD_ITM_TEST_CTGRY_TRFFC_TEST_EN_BIT) & 1) ? FALSE : TRUE;
  test->ctgry2_3_cnctn_test_en =
    ((test_in_sand_u32 >> ARAD_ITM_TEST_CTGRY2_3_CNCTN_TEST_EN_BIT) & 1) ? FALSE : TRUE;
  test->sttstcs_tag_test_en =
    ((test_in_sand_u32 >> ARAD_ITM_TEST_STTSTCS_TAG_TEST_EN_BIT) & 1) ? FALSE : TRUE;
  test->pfc_test_en =
    ((test_in_sand_u32 >> ARAD_ITM_TEST_PFC_TEST_EN_BIT) & 1) ? FALSE : TRUE;
  test->llfc_test_en =
    ((test_in_sand_u32 >> ARAD_ITM_TEST_LLFC_TEST_EN_BIT) & 1) ? FALSE : TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_convert_u32_to_admit_one_test_tmplt()",0,0);
}

STATIC uint32
  arad_itm_cr_request_info_hungry_table_field_set(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  int32 value,
    SOC_SAND_OUT uint32 *output_buffer,
    SOC_SAND_OUT int32 *exact_value,
    SOC_SAND_IN  uint8 resolution
  )
{
  uint32
    res;
  int32
    exact_value_var;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_REQUEST_INFO_HUNGRY_TABLE_FIELD_SET);

  res = arad_itm_man_exp_buffer_set(
          value / resolution,
          ARAD_ITM_HUNGRY_TH_MNT_LSB,
          ARAD_ITM_HUNGRY_TH_MNT_NOF_BITS(unit),
          ARAD_ITM_HUNGRY_TH_EXP_LSB,
          ARAD_ITM_HUNGRY_TH_EXP_NOF_BITS,
          TRUE,
          output_buffer,
          &exact_value_var
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *exact_value = resolution * (int32)exact_value_var;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_cr_request_info_hungry_table_field_set()", value, resolution);
}

STATIC uint32
  arad_itm_cr_request_info_satisfied_mnt_exp_table_field_set(
    SOC_SAND_IN  int32  value,
    SOC_SAND_OUT uint32  *output_buffer,
    SOC_SAND_OUT uint32 *exact_value,
    SOC_SAND_IN  uint8 resolution
  )
{
  uint32
    res;
  int32
    exact_value_var;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_CR_REQUEST_INFO_SATISFIED_MNT_EXP_TABLE_FIELD_SET);

  res = arad_itm_man_exp_buffer_set(
          value / resolution,
          ARAD_ITM_SATISFIED_TH_MNT_LSB,
          ARAD_ITM_SATISFIED_TH_MNT_NOF_BITS,
          ARAD_ITM_SATISFIED_TH_EXP_LSB,
          ARAD_ITM_SATISFIED_TH_EXP_NOF_BITS,
          FALSE,
          output_buffer,
          &exact_value_var
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *exact_value = resolution * (uint32)exact_value_var;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_cr_request_info_satisfied_mnt_exp_table_field_set()",0,0);
}

STATIC uint32
  arad_itm_cr_request_info_hungry_table_field_get(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  int32 buffer,
    SOC_SAND_OUT int32 *value,
    SOC_SAND_IN  int32 multiplier
  )
{
  uint32
    res;
  int32
    value_var;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_REQUEST_INFO_HUNGRY_TABLE_FIELD_GET);

  res = arad_itm_man_exp_buffer_get(
          buffer,
          ARAD_ITM_HUNGRY_TH_MNT_LSB,
          ARAD_ITM_HUNGRY_TH_MNT_NOF_BITS(unit),
          ARAD_ITM_HUNGRY_TH_EXP_LSB,
          ARAD_ITM_HUNGRY_TH_EXP_NOF_BITS,
          TRUE,
          &value_var
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *value = multiplier * (int32)value_var;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_cr_request_info_hungry_table_field_get()", buffer, 0);
}

STATIC uint32
  arad_itm_cr_request_info_satisfied_mnt_exp_table_field_get(
    SOC_SAND_IN  int32 buffer,
    SOC_SAND_OUT uint32 *value,
    SOC_SAND_IN  int32 multiplier
  )
{
  uint32
    res;
  int32
    value_var;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_CR_REQUEST_INFO_SATISFIED_MNT_EXP_TABLE_FIELD_GET);

  res = arad_itm_man_exp_buffer_get(
    buffer,
    ARAD_ITM_SATISFIED_TH_MNT_LSB,
    ARAD_ITM_SATISFIED_TH_MNT_NOF_BITS,
    ARAD_ITM_SATISFIED_TH_EXP_LSB,
    ARAD_ITM_SATISFIED_TH_EXP_NOF_BITS,
    FALSE,
    &value_var
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *value = multiplier * (uint32)value_var;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_cr_request_info_satisfied_mnt_exp_table_field_get()", buffer, 0);
}

STATIC uint32
  arad_itm_WRED_QT_DP_INFO_to_WRED_TBL_DATA(
     SOC_SAND_IN    int                                                  unit,
     SOC_SAND_IN    ARAD_ITM_WRED_QT_DP_INFO                            *wred_param,
     SOC_SAND_INOUT ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res,
    max_prob,
    calc,
    max_val_c1,
    max_avrg_th_16_byte;
  int32
    avrg_th_diff_wred_granular = 0;
  int32
    min_avrg_th_exact_wred_granular,
    max_avrg_th_exact_wred_granular;
  uint32
    trunced;
  SOC_SAND_U64
    u64_c2 = {{0}};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_WRED_QT_DP_INFO_TO_WRED_TBL_DATA);

  SOC_SAND_CHECK_NULL_INPUT(wred_param);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);

  trunced = FALSE;

  
  
  max_avrg_th_16_byte = SOC_SAND_DIV_ROUND_UP(wred_param->min_avrg_th,ARAD_ITM_WRED_GRANULARITY);
  tbl_data->pq_avrg_min_th = 0;
  res = arad_itm_man_exp_buffer_set(
          max_avrg_th_16_byte,
          ARAD_ITM_MIN_WRED_AVRG_TH_MNT_LSB,
          ARAD_ITM_MIN_WRED_AVRG_TH_MNT_NOF_BITS(unit),
          ARAD_ITM_MIN_WRED_AVRG_TH_EXP_LSB(unit),
          ARAD_ITM_MIN_WRED_AVRG_TH_EXP_NOF_BITS(unit),
          FALSE,
          &(tbl_data->pq_avrg_min_th),
          &min_avrg_th_exact_wred_granular
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  

  
  tbl_data->pq_avrg_max_th = 0;
  res = arad_itm_man_exp_buffer_set(
          SOC_SAND_DIV_ROUND_UP(wred_param->max_avrg_th,ARAD_ITM_WRED_GRANULARITY),
          ARAD_ITM_MAX_WRED_AVRG_TH_MNT_LSB,
          ARAD_ITM_MAX_WRED_AVRG_TH_MNT_NOF_BITS(unit),
          ARAD_ITM_MAX_WRED_AVRG_TH_EXP_LSB(unit),
          ARAD_ITM_MAX_WRED_AVRG_TH_EXP_NOF_BITS(unit),
          FALSE,
          &(tbl_data->pq_avrg_max_th),
          &max_avrg_th_exact_wred_granular
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  

  
  calc = wred_param->max_packet_size;
  if (calc > ARAD_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC)
  {
    calc = ARAD_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC;
  }
  calc = SOC_SAND_DIV_ROUND_UP(calc, ARAD_ITM_WRED_GRANULARITY);
  tbl_data->pq_c3 = (wred_param->max_avrg_th == 0 ? 0 : soc_sand_log2_round_up(calc));

  
  max_prob = (wred_param->max_probability);

  
  if(max_prob>=100)
  {
    max_prob = 99;
  }
  calc = ARAD_WRED_NORMALIZE_FACTOR * max_prob;

    
  max_val_c1 = 31; 

  avrg_th_diff_wred_granular =
    (max_avrg_th_exact_wred_granular - min_avrg_th_exact_wred_granular);

  if(avrg_th_diff_wred_granular == 0)
  {
    tbl_data->pq_c1 = max_val_c1;
  }
  else
  {
    calc = SOC_SAND_DIV_ROUND_DOWN(calc, avrg_th_diff_wred_granular);
    tbl_data->pq_c1 = soc_sand_log2_round_down(calc);
  }

  if(tbl_data->pq_c1 < max_val_c1)
  {
    
    uint32
      now     = 1 <<(tbl_data->pq_c1),
      changed = 1 <<(tbl_data->pq_c1+1),
      diff_with_now,
      diff_with_change;

    diff_with_change = changed-calc;
    diff_with_now    = calc-now;
    if( diff_with_change < diff_with_now)
    {
      tbl_data->pq_c1 += 1;
    }
  }

  SOC_SAND_LIMIT_FROM_ABOVE(tbl_data->pq_c1, max_val_c1);

  if (max_avrg_th_16_byte > 0)
  { 
    max_val_c1 = SOC_SAND_DIV_ROUND_DOWN(0xFFFFFFFF, max_avrg_th_16_byte);
    max_val_c1 = soc_sand_log2_round_down(max_val_c1);
    SOC_SAND_LIMIT_FROM_ABOVE(tbl_data->pq_c1, max_val_c1);
  }

  

  soc_sand_u64_multiply_longs(
          min_avrg_th_exact_wred_granular,
          (1 << tbl_data->pq_c1),
          &u64_c2
          );
  
  trunced = soc_sand_u64_to_long(&u64_c2, &tbl_data->pq_c2);

  if (trunced)
  {
    tbl_data->pq_c2 = 0xFFFFFFFF;
  }
  tbl_data->pq_c2 = (wred_param->max_avrg_th == 0 ? 1 : tbl_data->pq_c2);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_WRED_QT_DP_INFO_to_WRED_TBL_DATA()",0,0);
}


STATIC uint32
  arad_itm_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
     SOC_SAND_IN  int                                                   unit,
     SOC_SAND_IN  ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA  *tbl_data,
     SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO                             *wred_param
  )
{
  uint32
    res;
  uint32
    avrg_th_diff_wred_granular,
    two_power_c1,
    remainder;
  int32
    min_avrg_th_var,
    max_avrg_th_var;
  SOC_SAND_U64
    u64_1,
    u64_2; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_WRED_TBL_DATA_TO_WRED_QT_DP_INFO);

  SOC_SAND_CHECK_NULL_INPUT(wred_param);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);

  res = arad_itm_man_exp_buffer_get(
          tbl_data->pq_avrg_min_th,
          ARAD_ITM_MIN_WRED_AVRG_TH_MNT_LSB,
          ARAD_ITM_MIN_WRED_AVRG_TH_MNT_NOF_BITS(unit),
          ARAD_ITM_MIN_WRED_AVRG_TH_EXP_LSB(unit),
          ARAD_ITM_MIN_WRED_AVRG_TH_EXP_NOF_BITS(unit),
          FALSE,
          &min_avrg_th_var
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  wred_param->min_avrg_th = (uint32)min_avrg_th_var;

  wred_param->min_avrg_th *= ARAD_ITM_WRED_GRANULARITY;

  

  res = arad_itm_man_exp_buffer_get(
          tbl_data->pq_avrg_max_th,
          ARAD_ITM_MAX_WRED_AVRG_TH_MNT_LSB,
          ARAD_ITM_MAX_WRED_AVRG_TH_MNT_NOF_BITS(unit),
          ARAD_ITM_MAX_WRED_AVRG_TH_EXP_LSB(unit),
          ARAD_ITM_MAX_WRED_AVRG_TH_EXP_NOF_BITS(unit),
          FALSE,
          &max_avrg_th_var
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  wred_param->max_avrg_th = (uint32)max_avrg_th_var;

  wred_param->max_avrg_th *= ARAD_ITM_WRED_GRANULARITY;

  
  wred_param->max_packet_size = ((tbl_data->pq_c3 == 0) && (tbl_data->pq_c2 == 1)) ? 0 : (0x1 << (tbl_data->pq_c3)) * ARAD_ITM_WRED_GRANULARITY;

  
  avrg_th_diff_wred_granular =
      (wred_param->max_avrg_th - wred_param->min_avrg_th) / ARAD_ITM_WRED_GRANULARITY;
      
  two_power_c1 = 1<<tbl_data->pq_c1;
  
  
  
  soc_sand_u64_multiply_longs(two_power_c1, avrg_th_diff_wred_granular, &u64_1);
  remainder = soc_sand_u64_devide_u64_long(&u64_1, ARAD_WRED_NORMALIZE_FACTOR, &u64_2);
  soc_sand_u64_to_long(&u64_2, &wred_param->max_probability);
  
  if(remainder > (ARAD_WRED_NORMALIZE_FACTOR/2))
  {
      wred_param->max_probability++;
  }

  if(wred_param->max_probability > 100)
  {
    wred_param->max_probability = 100;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_WRED_TBL_DATA_to_WRED_QT_DP_INFO()",0,0);
}

STATIC uint32
  arad_itm_vsq_group_set_fc_info(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group,
    SOC_SAND_IN  uint32                 vsq_rt_cls,
    SOC_SAND_IN  ARAD_ITM_VSQ_FC_INFO     *info,
    SOC_SAND_OUT ARAD_ITM_VSQ_FC_INFO     *exact_info
  )
{
  uint32
    res,
    vsq_bds_th_clear_and_set_mnt_nof_bits,
    vsq_bds_th_clear_and_set_exp_nof_bits,
    vsq_words_th_clear_and_set_mnt_nof_bits,
    vsq_words_th_clear_and_set_exp_nof_bits;
  ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_flow_control_parameters_table_group_tbl_data;
  uint32
    set_threshold_words_mnt,
    set_threshold_words_exp,
    clear_threshold_words_mnt,
    clear_threshold_words_exp,
    set_threshold_bd_mnt,
    set_threshold_bd_exp,
    clear_threshold_bd_mnt,
    clear_threshold_bd_exp;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_GROUP_SET_FC_INFO);

 
 

  res = arad_iqm_vsq_flow_control_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group,
          vsq_rt_cls,
          &iqm_vsq_flow_control_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  vsq_bds_th_clear_and_set_mnt_nof_bits = 4;
  vsq_bds_th_clear_and_set_exp_nof_bits = soc_mem_field_length(unit, IQM_VQFCPR_MAm, SET_THRESHOLD_BDf) - vsq_bds_th_clear_and_set_mnt_nof_bits;

  vsq_words_th_clear_and_set_mnt_nof_bits = 5;
  vsq_words_th_clear_and_set_exp_nof_bits = soc_mem_field_length(unit, IQM_VQFCPR_MAm, SET_THRESHOLD_WORDSf) - vsq_words_th_clear_and_set_mnt_nof_bits;

  res = soc_sand_break_to_mnt_exp_round_up(
          info->bd_size_fc.clear,
          vsq_bds_th_clear_and_set_mnt_nof_bits,
          vsq_bds_th_clear_and_set_exp_nof_bits,
          0,
          &(clear_threshold_bd_mnt),
          &(clear_threshold_bd_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  iqm_vsq_flow_control_parameters_table_group_tbl_data.clear_threshold_bd_mnt = clear_threshold_bd_mnt;
  iqm_vsq_flow_control_parameters_table_group_tbl_data.clear_threshold_bd_exp = clear_threshold_bd_exp;

  res = soc_sand_break_to_mnt_exp_round_up(
          info->bd_size_fc.set,
          vsq_bds_th_clear_and_set_mnt_nof_bits,
          vsq_bds_th_clear_and_set_exp_nof_bits,
          0,
          &(set_threshold_bd_mnt),
          &(set_threshold_bd_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  iqm_vsq_flow_control_parameters_table_group_tbl_data.set_threshold_bd_mnt = set_threshold_bd_mnt;
  iqm_vsq_flow_control_parameters_table_group_tbl_data.set_threshold_bd_exp = set_threshold_bd_exp;

  res = soc_sand_break_to_mnt_exp_round_up(
          info->q_size_fc.clear,
          vsq_words_th_clear_and_set_mnt_nof_bits,
          vsq_words_th_clear_and_set_exp_nof_bits,
          0,
          &(clear_threshold_words_mnt),
          &(clear_threshold_words_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  iqm_vsq_flow_control_parameters_table_group_tbl_data.clear_threshold_words_mnt = clear_threshold_words_mnt;
  iqm_vsq_flow_control_parameters_table_group_tbl_data.clear_threshold_words_exp = clear_threshold_words_exp;

  res = soc_sand_break_to_mnt_exp_round_up(
          info->q_size_fc.set,
          vsq_words_th_clear_and_set_mnt_nof_bits,
          vsq_words_th_clear_and_set_exp_nof_bits,
          0,
          &(set_threshold_words_mnt),
          &(set_threshold_words_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  iqm_vsq_flow_control_parameters_table_group_tbl_data.set_threshold_words_mnt = set_threshold_words_mnt;
  iqm_vsq_flow_control_parameters_table_group_tbl_data.set_threshold_words_exp = set_threshold_words_exp;

  exact_info->bd_size_fc.clear =
      ((iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      clear_threshold_bd_mnt) *
      (1<<(iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      clear_threshold_bd_exp)));
  exact_info->bd_size_fc.set =
    ((iqm_vsq_flow_control_parameters_table_group_tbl_data.\
    set_threshold_bd_mnt) *
      (1<<(iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      set_threshold_bd_exp)));
  exact_info->q_size_fc.clear =
    ((iqm_vsq_flow_control_parameters_table_group_tbl_data.\
    clear_threshold_words_mnt) *
      (1<<(iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      clear_threshold_words_exp)));
  exact_info->q_size_fc.set =
    ((iqm_vsq_flow_control_parameters_table_group_tbl_data.\
    set_threshold_words_mnt) *
      (1<<(iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      set_threshold_words_exp)));

  res = arad_iqm_vsq_flow_control_parameters_table_group_tbl_set_unsafe(
          unit,
          vsq_group,
          vsq_rt_cls,
          &iqm_vsq_flow_control_parameters_table_group_tbl_data
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_group_set_fc_info()",0,0);
}

STATIC uint32
  arad_itm_vsq_group_get_fc_info(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   ARAD_ITM_VSQ_GROUP       vsq_group,
    SOC_SAND_IN   uint32   vsq_rt_cls,
    SOC_SAND_OUT  ARAD_ITM_VSQ_FC_INFO     *info
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_flow_control_parameters_table_group_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_GROUP_GET_FC_INFO);

  res = arad_iqm_vsq_flow_control_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group,
          vsq_rt_cls,
          &iqm_vsq_flow_control_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->bd_size_fc.clear =
      ((iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      clear_threshold_bd_mnt) *
      (1<<(iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      clear_threshold_bd_exp)));
  info->bd_size_fc.set =
    ((iqm_vsq_flow_control_parameters_table_group_tbl_data.\
    set_threshold_bd_mnt) *
      (1<<(iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      set_threshold_bd_exp)));
  info->q_size_fc.clear =
    ((iqm_vsq_flow_control_parameters_table_group_tbl_data.\
    clear_threshold_words_mnt) *
      (1<<(iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      clear_threshold_words_exp)));
  info->q_size_fc.set =
    ((iqm_vsq_flow_control_parameters_table_group_tbl_data.\
    set_threshold_words_mnt) *
      (1<<(iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      set_threshold_words_exp)));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_group_get_fc_info()",0,0);
}



STATIC uint32
  arad_itm_vsq_WRED_QT_DP_INFO_to_WRED_TBL_DATA(
    SOC_SAND_IN  int                                                   unit,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO                             *wred_param,
    SOC_SAND_INOUT ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA *tbl_data
  )
{
  uint32
    res,
    max_prob,
    calc,
    max_val_c1,
    max_avrg_th_16_byte;
  int32
    avrg_th_diff_wred_granular = 0;
  int32
    min_avrg_th_exact_wred_granular,
    max_avrg_th_exact_wred_granular;
  uint32
    trunced;
  SOC_SAND_U64
    u64_c2 = {{0}};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_WRED_QT_DP_INFO_TO_WRED_TBL_DATA);

  SOC_SAND_CHECK_NULL_INPUT(wred_param);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);

  trunced = FALSE;

  

  tbl_data->min_avrg_th = 0;
  max_avrg_th_16_byte = SOC_SAND_DIV_ROUND_UP(wred_param->min_avrg_th,ARAD_ITM_WRED_GRANULARITY);
  res = arad_itm_man_exp_buffer_set(
          max_avrg_th_16_byte,
          ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB,
          ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_NOF_BITS(unit),
          ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB(unit),
          ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_NOF_BITS(unit),
          FALSE,
          &(tbl_data->min_avrg_th),
          &min_avrg_th_exact_wred_granular
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  

  
  tbl_data->max_avrg_th = 0;
  res = arad_itm_man_exp_buffer_set(
          SOC_SAND_DIV_ROUND_UP(wred_param->max_avrg_th,ARAD_ITM_WRED_GRANULARITY),
          ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB,
          ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_NOF_BITS(unit),
          ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB(unit),
          ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_NOF_BITS(unit),
          FALSE,
          &(tbl_data->max_avrg_th),
          &max_avrg_th_exact_wred_granular
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  

  
  calc = wred_param->max_packet_size;
  if (calc > ARAD_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC)
  {
    calc = ARAD_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC;
  }
  calc = SOC_SAND_DIV_ROUND_UP(calc, ARAD_ITM_WRED_GRANULARITY);

  tbl_data->c3
    = soc_sand_log2_round_up(calc);

  
  tbl_data->vq_wred_pckt_sz_ignr = wred_param->ignore_packet_size;

  
  max_prob = (wred_param->max_probability);
  if(max_prob>=100)
  {
    max_prob = 99;
  }

  
  calc = ARAD_WRED_NORMALIZE_FACTOR * max_prob;
    
  max_val_c1 = 31; 

  avrg_th_diff_wred_granular =
    (max_avrg_th_exact_wred_granular - min_avrg_th_exact_wred_granular);

  if(avrg_th_diff_wred_granular == 0)
  {
    tbl_data->c1 = max_val_c1;
  }
  else
  {
    calc = SOC_SAND_DIV_ROUND_DOWN(calc, avrg_th_diff_wred_granular);
    tbl_data->c1 = soc_sand_log2_round_down(calc);
  }
  if(tbl_data->c1 < max_val_c1)
  {
    
    uint32
      now     = 1 <<(tbl_data->c1),
      changed = 1 <<(tbl_data->c1+1),
      diff_with_now,
      diff_with_change;

    diff_with_change = changed-calc;

    diff_with_now    = calc-now;
    if( diff_with_change < diff_with_now)
    {
      tbl_data->c1 += 1;
    }
  }

  SOC_SAND_LIMIT_FROM_ABOVE(tbl_data->c1, max_val_c1);

  if (max_avrg_th_16_byte > 0)
  {
    max_val_c1 = SOC_SAND_DIV_ROUND_DOWN(0xFFFFFFFF, max_avrg_th_16_byte);
    max_val_c1 = soc_sand_log2_round_down(max_val_c1);
    SOC_SAND_LIMIT_FROM_ABOVE(tbl_data->c1, max_val_c1);
  }

  

  soc_sand_u64_multiply_longs(
    min_avrg_th_exact_wred_granular,
    (1 << tbl_data->c1),
    &u64_c2
  );

  trunced = soc_sand_u64_to_long(&u64_c2, &tbl_data->c2);

  if (trunced)
  {
    tbl_data->c2 = 0xFFFFFFFF;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_WRED_QT_DP_INFO_to_WRED_TBL_DATA()",0,0);
}


STATIC uint32
  arad_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
    SOC_SAND_IN  int                                                  unit,
    SOC_SAND_IN  ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA   *tbl_data,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO                             *wred_param
  )
{
  uint32
    res;
  uint32
    avrg_th_diff_wred_granular,
    two_power_c1,
    remainder;
  SOC_SAND_U64
    u64_1,
    u64_2;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_WRED_TBL_DATA_TO_WRED_QT_DP_INFO);

  SOC_SAND_CHECK_NULL_INPUT(wred_param);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);



  res = arad_itm_man_exp_buffer_get(
          tbl_data->min_avrg_th,
          ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB,
          ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_NOF_BITS(unit),
          ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB(unit),
          ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_NOF_BITS(unit),
          FALSE,
          (int32*)&(wred_param->min_avrg_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  wred_param->min_avrg_th *= ARAD_ITM_WRED_GRANULARITY;

  

  res = arad_itm_man_exp_buffer_get(
          tbl_data->max_avrg_th,
          ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB,
          ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_NOF_BITS(unit),
          ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB(unit),
          ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_NOF_BITS(unit),
          FALSE,
          (int32*)&(wred_param->max_avrg_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  wred_param->max_avrg_th *= ARAD_ITM_WRED_GRANULARITY;

  

  wred_param->max_packet_size =
    (0x1<<(tbl_data->c3))*ARAD_ITM_WRED_GRANULARITY;

  
  wred_param->ignore_packet_size = SOC_SAND_NUM2BOOL(tbl_data->vq_wred_pckt_sz_ignr);


  avrg_th_diff_wred_granular =
    (wred_param->max_avrg_th - wred_param->min_avrg_th) / ARAD_ITM_WRED_GRANULARITY;

  two_power_c1 = 1<<tbl_data->c1;
  
  soc_sand_u64_multiply_longs(two_power_c1, avrg_th_diff_wred_granular, &u64_1);
  remainder = soc_sand_u64_devide_u64_long(&u64_1, ARAD_WRED_NORMALIZE_FACTOR, &u64_2);
  soc_sand_u64_to_long(&u64_2, &wred_param->max_probability);

  if(remainder > (ARAD_WRED_NORMALIZE_FACTOR/2))
  {
    wred_param->max_probability++;
  }

  if(wred_param->max_probability > 100)
  {
    wred_param->max_probability = 100;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO()",0,0);
}


uint32
  arad_itm_dram_buffs_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_DRAM_BUFFERS_INFO *dram_buffs
  )
{
  uint32
    uc_start,
    uc_end,
    uc_enable_val,
    uc_size,
    fmc_start,
    fmc_end,
    fmc_enable_val,
    fmc_size,
    mmc_start,
    mmc_end,
    mmc_enable_val,
    mmc_size,
    dbuff_size_internal,
    res;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_DRAM_BUFFS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(dram_buffs);

  

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_4r, REG_PORT_ANY, 0, UC_DB_PTR_STARTf, &uc_start));
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_5r, REG_PORT_ANY, 0, UC_DB_PTR_ENDf, &uc_end));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_6r, REG_PORT_ANY, 0, MN_MUL_DB_PTR_STARTf, &mmc_start));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_7r, REG_PORT_ANY, 0, MN_MUL_DB_PTR_ENDf, &mmc_end));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_8r, REG_PORT_ANY, 0, FL_MUL_DB_PTR_STARTf, &fmc_start));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit , ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_9r, REG_PORT_ANY, 0, FL_MUL_DB_PTR_ENDf, &fmc_end));

  if (uc_end > uc_start)
  {
    uc_size = uc_end - uc_start + 1;
  }
  else
  {
    if (uc_end == uc_start)
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit , ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_UNICAST_AUTOGEN_ENABLEf, &uc_enable_val));
      if (SOC_SAND_NUM2BOOL(uc_enable_val) == TRUE)
      {
        
        uc_size = uc_end - uc_start + 1;
      }
      else
      {
        uc_size = 0x0;
      }
    }
    else
    {
      uc_size = 0x0;
    }
  }

  if (mmc_end > mmc_start)
  {
    mmc_size = mmc_end - mmc_start + 1;
  }
  else
  {
    if (mmc_end == mmc_start)
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_MINI_MULTICAST_AUTOGEN_ENABLEf, &mmc_enable_val));
      if (SOC_SAND_NUM2BOOL(mmc_enable_val) == TRUE)
      {
        
        mmc_size = mmc_end - mmc_start + 1;
      }
      else
      {
        mmc_size = 0x0;
      }
    }
    else
    {
      mmc_size = 0x0;
    }
  }

  if (fmc_end > fmc_start)
  {
    fmc_size = fmc_end - fmc_start + 1;
  }
  else
  {
    if (fmc_end == fmc_start)
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_FULL_MULTICAST_AUTOGEN_ENABLEf, &fmc_enable_val));
      if (SOC_SAND_NUM2BOOL(fmc_enable_val) == TRUE)
      {
        
        fmc_size = fmc_end - fmc_start + 1;
      }
      else
      {
        fmc_size = 0x0;
      }
    }
    else
    {
      fmc_size = 0x0;
    }
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_1r, REG_PORT_ANY, 0, DBUFF_SIZEf, &dbuff_size_internal));

  res = arad_itm_dbuff_internal2size(
          dbuff_size_internal,
          &(dram_buffs->dbuff_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  dram_buffs->full_mc_nof_buffs = fmc_size;
  dram_buffs->mini_mc_nof_buffs = mmc_size;
  dram_buffs->uc_nof_buffs = uc_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_dram_buffs_get_unsafe()",0,0);
}

static uint32
  arad_itm_glob_res_fc_mnt_exp_field_set(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   soc_reg_t           reg,
    SOC_SAND_IN   soc_field_t         fld,
    SOC_SAND_IN   uint32              mnt_nof_bits,
    SOC_SAND_IN   uint32              exp_nof_bits,
    SOC_SAND_IN   uint32              value,
    SOC_SAND_OUT  uint32              *exact_value
  )
{
  uint32
    res;
  uint32
    exp_man,
    mnt_val,
    exp_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(exact_value);

  res = soc_sand_break_to_mnt_exp_round_up(
          value,
          mnt_nof_bits,
          exp_nof_bits,
          0,
          &mnt_val,
          &exp_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  (*exact_value) = (mnt_val) * (1<<(exp_val));

  arad_iqm_mantissa_exponent_set(
      unit,
      mnt_val,
      exp_val,
      mnt_nof_bits,
      &exp_man
    );

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, fld,  exp_man));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_res_fc_set_one()",0,0);
}

static uint32
  arad_itm_glob_res_fc_mnt_exp_mem_field_set(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   soc_mem_t           mem,
    SOC_SAND_IN   int                 index,
    SOC_SAND_IN   soc_field_t         fld,
    SOC_SAND_IN   uint32              mnt_nof_bits,
    SOC_SAND_IN   uint32              exp_nof_bits,
    SOC_SAND_IN   uint32              value,
    SOC_SAND_OUT  uint32              *exact_value
  )
{
  uint32
    res;
  uint32
    exp_man,
    mnt_val,
    exp_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(exact_value);

  res = soc_sand_break_to_mnt_exp_round_up(
          value,
          mnt_nof_bits,
          exp_nof_bits,
          0,
          &mnt_val,
          &exp_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  (*exact_value) = (mnt_val) * (1<<(exp_val));

  arad_iqm_mantissa_exponent_set(
      unit,
      mnt_val,
      exp_val,
      mnt_nof_bits,
      &exp_man
    );

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit, ARAD_REG_ACCESS_ERR, soc_mem_field32_modify(unit, mem, index, fld, exp_man));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_res_fc_set_one()",0,0);
}


uint32
  arad_itm_glob_rcs_fc_set_unsafe(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   ARAD_ITM_GLOB_RCS_FC_TH  *info,
    SOC_SAND_OUT  ARAD_ITM_GLOB_RCS_FC_TH  *exact_info
  )
{
  uint32
    res;
  uint32
    bdbs_th_mnt_nof_bits,
    bdbs_th_exp_nof_bits,
    uc_th_mnt_nof_bits,
    uc_th_exp_nof_bits,
    fmc_th_mnt_nof_bits,
    fmc_th_exp_nof_bits,
    mnmc_th_mnt_nof_bits,
    mnmc_th_exp_nof_bits,
    ocb_th_mnt_nof_bits,
    ocb_th_exp_nof_bits,
    ocb_p0_th_mnt_nof_bits,
    ocb_p0_th_exp_nof_bits,
    ocb_p1_th_mnt_nof_bits,
    ocb_p1_th_exp_nof_bits,
    mix_p0_th_mnt_nof_bits,
    mix_p0_th_exp_nof_bits,
    mix_p1_th_mnt_nof_bits,
    mix_p1_th_exp_nof_bits,
    ocb_pdb_th_mnt_nof_bits,
    ocb_pdb_th_exp_nof_bits,
    pool_th_mnt_nof_bits,
    pool_th_exp_nof_bits,
    pool_pd_th_mnt_nof_bits,
    pool_pd_th_exp_nof_bits,
    pool_byte_th_mnt_nof_bits,
    pool_byte_th_exp_nof_bits,
    hdrm_th_mnt_nof_bits,
    hdrm_th_exp_nof_bits;
  soc_reg_t
    reg = INVALIDr;
  soc_field_t
    fld = INVALIDf;
  soc_mem_t
    mem = INVALIDm;
  int idx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_FC_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  
  if(!SOC_IS_QAX(unit)){
    reg = SOC_IS_JERICHO(unit) ? IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_BDBr : 
                                 IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_BDB_HIGH_PRIORITYr;
    fld = FC_SET_FR_BDB_TH_HPf;
    bdbs_th_mnt_nof_bits = 8;
    bdbs_th_exp_nof_bits = soc_reg_field_length(unit, reg, fld) - bdbs_th_mnt_nof_bits;

    fld = FC_CLR_FR_BDB_TH_HPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, bdbs_th_mnt_nof_bits, bdbs_th_exp_nof_bits, info->bdbs.hp.clear, &exact_info->bdbs.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    fld = FC_SET_FR_BDB_TH_HPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, bdbs_th_mnt_nof_bits, bdbs_th_exp_nof_bits, info->bdbs.hp.set, &exact_info->bdbs.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    reg = SOC_IS_JERICHO(unit) ? IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_BDBr : 
                                 IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_BDB_LOW_PRIORITYr;
    fld = FC_CLR_FR_BDB_TH_LPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, bdbs_th_mnt_nof_bits, bdbs_th_exp_nof_bits, info->bdbs.lp.clear, &exact_info->bdbs.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    fld = FC_SET_FR_BDB_TH_LPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, bdbs_th_mnt_nof_bits, bdbs_th_exp_nof_bits, info->bdbs.lp.set, &exact_info->bdbs.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  } else {
    reg = CGM_TOTAL_DRAM_RSRC_FLOW_CONTROL_THSr;
    fld = TOTAL_DRAM_BDBS_FC_LP_TH_SETf;
    bdbs_th_mnt_nof_bits = 8;
    bdbs_th_exp_nof_bits = soc_reg_field_length(unit, reg, fld) - bdbs_th_mnt_nof_bits;
	
    fld = TOTAL_DRAM_BDBS_FC_LP_TH_CLRf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, bdbs_th_mnt_nof_bits, bdbs_th_exp_nof_bits, info->bdbs.lp.clear, &exact_info->bdbs.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    fld = TOTAL_DRAM_BDBS_FC_LP_TH_SETf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, bdbs_th_mnt_nof_bits, bdbs_th_exp_nof_bits, info->bdbs.lp.set, &exact_info->bdbs.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    fld = TOTAL_DRAM_BDBS_FC_HP_TH_CLRf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, bdbs_th_mnt_nof_bits, bdbs_th_exp_nof_bits, info->bdbs.hp.clear, &exact_info->bdbs.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    fld = TOTAL_DRAM_BDBS_FC_HP_TH_SETf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, bdbs_th_mnt_nof_bits, bdbs_th_exp_nof_bits, info->bdbs.hp.set, &exact_info->bdbs.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
  

  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
    
    reg = IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr;
    fld = FC_CLR_FR_DB_UC_TH_HPf;
    uc_th_mnt_nof_bits = 7;
    uc_th_exp_nof_bits = soc_reg_field_length(unit, reg, fld) - uc_th_mnt_nof_bits;

    fld = FC_CLR_FR_DB_UC_TH_HPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, uc_th_mnt_nof_bits, uc_th_exp_nof_bits, info->unicast.hp.clear, &exact_info->unicast.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    reg = IQM_GENERAL_SET_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr;
    fld = FC_SET_FR_DB_UC_TH_HPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, uc_th_mnt_nof_bits, uc_th_exp_nof_bits, info->unicast.hp.set, &exact_info->unicast.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    reg = IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_LOW_PRIORITYr;
    fld = FC_CLR_FR_DB_UC_TH_LPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, uc_th_mnt_nof_bits, uc_th_exp_nof_bits, info->unicast.lp.clear, &exact_info->unicast.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    reg = IQM_GENERAL_SET_FLOW_CONTROL_CONFIGURATION_D_BS_LOW_PRIORITYr;
    fld = FC_SET_FR_DB_UC_TH_LPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, uc_th_mnt_nof_bits, uc_th_exp_nof_bits, info->unicast.lp.set, &exact_info->unicast.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    
  }

  
  if(!SOC_IS_QAX(unit)){
    reg = SOC_IS_JERICHO(unit) ? IQM_FLOW_CONTROL_CONFIGURATION_FULL_MC_D_BSr : 
                                 IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr;
    fld = FC_CLR_FR_DB_FLMC_TH_HPf;
    fmc_th_mnt_nof_bits = SOC_IS_JERICHO(unit) ? 8 : 7;
    fmc_th_exp_nof_bits = soc_reg_field_length(unit, reg, fld) - fmc_th_mnt_nof_bits;

    fld = FC_CLR_FR_DB_FLMC_TH_HPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, fmc_th_mnt_nof_bits, fmc_th_exp_nof_bits, info->full_mc.hp.clear, &exact_info->full_mc.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    reg = SOC_IS_JERICHO(unit) ? IQM_FLOW_CONTROL_CONFIGURATION_FULL_MC_D_BSr : 
                                 IQM_GENERAL_SET_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr;
    fld = FC_SET_FR_DB_FLMC_TH_HPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, fmc_th_mnt_nof_bits, fmc_th_exp_nof_bits, info->full_mc.hp.set, &exact_info->full_mc.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    reg = SOC_IS_JERICHO(unit) ? IQM_FLOW_CONTROL_CONFIGURATION_FULL_MC_D_BSr : 
                               IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_LOW_PRIORITYr;
    fld = FC_CLR_FR_DB_FLMC_TH_LPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, fmc_th_mnt_nof_bits, fmc_th_exp_nof_bits, info->full_mc.lp.clear, &exact_info->full_mc.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

    reg = SOC_IS_JERICHO(unit) ? IQM_FLOW_CONTROL_CONFIGURATION_FULL_MC_D_BSr : 
                                 IQM_GENERAL_SET_FLOW_CONTROL_CONFIGURATION_D_BS_LOW_PRIORITYr;
    fld = FC_SET_FR_DB_FLMC_TH_LPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, fmc_th_mnt_nof_bits, fmc_th_exp_nof_bits, info->full_mc.lp.set, &exact_info->full_mc.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  }
  

  if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
    
    reg = IQM_FLOW_CONTROL_CONFIGURATION_MINI_MC_D_BSr;
    fld = FC_CLR_FR_DB_MNMC_TH_HPf;
    mnmc_th_mnt_nof_bits = 8;
    mnmc_th_exp_nof_bits = soc_reg_field_length(unit, reg, fld) - mnmc_th_mnt_nof_bits;

    fld = FC_CLR_FR_DB_MNMC_TH_HPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, mnmc_th_mnt_nof_bits, mnmc_th_exp_nof_bits, info->mini_mc.hp.clear, &exact_info->mini_mc.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

    fld = FC_SET_FR_DB_MNMC_TH_HPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, mnmc_th_mnt_nof_bits, mnmc_th_exp_nof_bits, info->mini_mc.hp.set, &exact_info->mini_mc.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

    fld = FC_CLR_FR_DB_MNMC_TH_LPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, mnmc_th_mnt_nof_bits, mnmc_th_exp_nof_bits, info->mini_mc.lp.clear, &exact_info->mini_mc.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

    fld = FC_SET_FR_DB_MNMC_TH_LPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, mnmc_th_mnt_nof_bits, mnmc_th_exp_nof_bits, info->mini_mc.lp.set, &exact_info->mini_mc.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
    

    
    reg = IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_OCBr;
    fld = FC_CLR_FR_OCB_TH_HPf;
    ocb_th_mnt_nof_bits = 8;
    ocb_th_exp_nof_bits = soc_reg_field_length(unit, reg, fld) - ocb_th_mnt_nof_bits;

    fld = FC_CLR_FR_OCB_TH_HPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_th_mnt_nof_bits, ocb_th_exp_nof_bits, info->ocb.hp.clear, &exact_info->ocb.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

    fld = FC_SET_FR_OCB_TH_HPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_th_mnt_nof_bits, ocb_th_exp_nof_bits, info->ocb.hp.set, &exact_info->ocb.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

    fld = FC_CLR_FR_OCB_TH_LPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_th_mnt_nof_bits, ocb_th_exp_nof_bits, info->ocb.lp.clear, &exact_info->ocb.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

    fld = FC_SET_FR_OCB_TH_LPf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_th_mnt_nof_bits, ocb_th_exp_nof_bits, info->ocb.lp.set, &exact_info->ocb.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
    

    
    reg = IQM_OCB_SHRD_POOL_0_RSRC_FC_THr;
    fld = OCB_SHRD_POOL_0_HP_FC_CLR_THf;
    ocb_p0_th_mnt_nof_bits = 8;
    ocb_p0_th_exp_nof_bits = soc_reg_field_length(unit, reg, fld) - ocb_p0_th_mnt_nof_bits;

    fld = OCB_SHRD_POOL_0_HP_FC_CLR_THf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_p0_th_mnt_nof_bits, ocb_p0_th_exp_nof_bits, info->ocb_p0.hp.clear, &exact_info->ocb_p0.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);

    fld = OCB_SHRD_POOL_0_HP_FC_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_p0_th_mnt_nof_bits, ocb_p0_th_exp_nof_bits, info->ocb_p0.hp.set, &exact_info->ocb_p0.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);

    fld = OCB_SHRD_POOL_0_LP_FC_CLR_THf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_p0_th_mnt_nof_bits, ocb_p0_th_exp_nof_bits, info->ocb_p0.lp.clear, &exact_info->ocb_p0.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

    fld = OCB_SHRD_POOL_0_LP_FC_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_p0_th_mnt_nof_bits, ocb_p0_th_exp_nof_bits, info->ocb_p0.lp.set, &exact_info->ocb_p0.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
    

    
    reg = IQM_OCB_SHRD_POOL_1_RSRC_FC_THr;
    fld = OCB_SHRD_POOL_1_HP_FC_CLR_THf;
    ocb_p1_th_mnt_nof_bits = 8;
    ocb_p1_th_exp_nof_bits = soc_reg_field_length(unit, reg, fld) - ocb_p1_th_mnt_nof_bits;

    fld = OCB_SHRD_POOL_1_HP_FC_CLR_THf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_p1_th_mnt_nof_bits, ocb_p1_th_exp_nof_bits, info->ocb_p1.hp.clear, &exact_info->ocb_p1.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);

    fld = OCB_SHRD_POOL_1_HP_FC_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_p1_th_mnt_nof_bits, ocb_p1_th_exp_nof_bits, info->ocb_p1.hp.set, &exact_info->ocb_p1.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);

    fld = OCB_SHRD_POOL_1_LP_FC_CLR_THf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_p1_th_mnt_nof_bits, ocb_p1_th_exp_nof_bits, info->ocb_p1.lp.clear, &exact_info->ocb_p1.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);

    fld = OCB_SHRD_POOL_1_LP_FC_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_p1_th_mnt_nof_bits, ocb_p1_th_exp_nof_bits, info->ocb_p1.lp.set, &exact_info->ocb_p1.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);
    

    
    reg = IQM_MIX_SHRD_POOL_0_RSRC_FC_THr;
    fld = MIX_SHRD_POOL_0_HP_FC_CLR_THf;
    mix_p0_th_mnt_nof_bits = 8;
    mix_p0_th_exp_nof_bits = soc_reg_field_length(unit, reg, fld) - mix_p0_th_mnt_nof_bits;

    fld = MIX_SHRD_POOL_0_HP_FC_CLR_THf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, mix_p0_th_mnt_nof_bits, mix_p0_th_exp_nof_bits, info->mix_p0.hp.clear, &exact_info->mix_p0.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 290, exit);

    fld = MIX_SHRD_POOL_0_HP_FC_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, mix_p0_th_mnt_nof_bits, mix_p0_th_exp_nof_bits, info->mix_p0.hp.set, &exact_info->mix_p0.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 300, exit);

    fld = MIX_SHRD_POOL_0_LP_FC_CLR_THf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, mix_p0_th_mnt_nof_bits, mix_p0_th_exp_nof_bits, info->mix_p0.lp.clear, &exact_info->mix_p0.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 310, exit);

    fld = MIX_SHRD_POOL_0_LP_FC_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, mix_p0_th_mnt_nof_bits, mix_p0_th_exp_nof_bits, info->mix_p0.lp.set, &exact_info->mix_p0.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);
    

    
    reg = IQM_MIX_SHRD_POOL_1_RSRC_FC_THr;
    fld = MIX_SHRD_POOL_1_HP_FC_CLR_THf;
    mix_p1_th_mnt_nof_bits = 8;
    mix_p1_th_exp_nof_bits = soc_reg_field_length(unit, reg, fld) - mix_p1_th_mnt_nof_bits;

    fld = MIX_SHRD_POOL_1_HP_FC_CLR_THf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, mix_p1_th_mnt_nof_bits, mix_p1_th_exp_nof_bits, info->mix_p1.hp.clear, &exact_info->mix_p1.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);

    fld = MIX_SHRD_POOL_1_HP_FC_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, mix_p1_th_mnt_nof_bits, mix_p1_th_exp_nof_bits, info->mix_p1.hp.set, &exact_info->mix_p1.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 340, exit);

    fld = MIX_SHRD_POOL_1_LP_FC_CLR_THf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, mix_p1_th_mnt_nof_bits, mix_p1_th_exp_nof_bits, info->mix_p1.lp.clear, &exact_info->mix_p1.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 350, exit);

    fld = MIX_SHRD_POOL_1_LP_FC_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, mix_p1_th_mnt_nof_bits, mix_p1_th_exp_nof_bits, info->mix_p1.lp.set, &exact_info->mix_p1.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 360, exit);
    
  }

  if (SOC_IS_QAX(unit)) {
    
    reg = CGM_TOTAL_SRAM_RSRC_FLOW_CONTROL_THSr;
    fld = TOTAL_SRAM_PDBS_FC_LP_TH_CLRf;
    ocb_pdb_th_mnt_nof_bits = 8;
    ocb_pdb_th_exp_nof_bits = soc_reg_field_length(unit, reg, fld) - ocb_pdb_th_mnt_nof_bits;

    fld = TOTAL_SRAM_PDBS_FC_LP_TH_CLRf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_pdb_th_mnt_nof_bits, ocb_pdb_th_exp_nof_bits, info->ocb_pdb.lp.clear, &exact_info->ocb_pdb.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);

    fld = TOTAL_SRAM_PDBS_FC_LP_TH_SETf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_pdb_th_mnt_nof_bits, ocb_pdb_th_exp_nof_bits, info->ocb_pdb.lp.set, &exact_info->ocb_pdb.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 380, exit);

    fld = TOTAL_SRAM_PDBS_FC_HP_TH_CLRf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_pdb_th_mnt_nof_bits, ocb_pdb_th_exp_nof_bits, info->ocb_pdb.hp.clear, &exact_info->ocb_pdb.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 390, exit);
	
    fld = TOTAL_SRAM_PDBS_FC_HP_TH_SETf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_pdb_th_mnt_nof_bits, ocb_pdb_th_exp_nof_bits, info->ocb_pdb.hp.set, &exact_info->ocb_pdb.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 400, exit);
    

    
    reg = CGM_TOTAL_SRAM_RSRC_FLOW_CONTROL_THSr;
    fld = TOTAL_SRAM_BUFFERS_FC_LP_TH_SETf;
    ocb_th_mnt_nof_bits = 8;
    ocb_th_exp_nof_bits = soc_reg_field_length(unit, reg, fld) - ocb_th_mnt_nof_bits;

    fld = TOTAL_SRAM_BUFFERS_FC_LP_TH_CLRf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_th_mnt_nof_bits, ocb_th_exp_nof_bits, info->ocb.lp.clear, &exact_info->ocb.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 410, exit);

    fld = TOTAL_SRAM_BUFFERS_FC_LP_TH_SETf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_th_mnt_nof_bits, ocb_th_exp_nof_bits, info->ocb.lp.set, &exact_info->ocb.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 420, exit);

    fld = TOTAL_SRAM_BUFFERS_FC_HP_TH_CLRf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_th_mnt_nof_bits, ocb_th_exp_nof_bits, info->ocb.hp.clear, &exact_info->ocb.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 430, exit);
	
    fld = TOTAL_SRAM_BUFFERS_FC_HP_TH_SETf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, ocb_th_mnt_nof_bits, ocb_th_exp_nof_bits, info->ocb.hp.set, &exact_info->ocb.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 440, exit);
    

    
    mem = CGM_PB_VSQ_POOL_FC_THm;
    idx = 0;   
    pool_th_mnt_nof_bits = 8;
    fld = SRAM_BUFFERS_CLR_THf;
    pool_th_exp_nof_bits = soc_mem_field_length(unit, mem, fld) - pool_th_mnt_nof_bits;

    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_th_mnt_nof_bits, pool_th_exp_nof_bits, info->p0.lp.clear, &exact_info->p0.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);

    fld = SRAM_BUFFERS_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_th_mnt_nof_bits, pool_th_exp_nof_bits, info->p0.lp.set, &exact_info->p0.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 460, exit);

    idx = 2;   
    fld = SRAM_BUFFERS_CLR_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_th_mnt_nof_bits, pool_th_exp_nof_bits, info->p0.hp.clear, &exact_info->p0.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 470, exit);

    fld = SRAM_BUFFERS_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_th_mnt_nof_bits, pool_th_exp_nof_bits, info->p0.hp.set, &exact_info->p0.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 480, exit);
    

    
    idx = 1;   
    fld = SRAM_BUFFERS_CLR_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_th_mnt_nof_bits, pool_th_exp_nof_bits, info->p1.lp.clear, &exact_info->p1.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 490, exit);

    fld = SRAM_BUFFERS_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_th_mnt_nof_bits, pool_th_exp_nof_bits, info->p1.lp.set, &exact_info->p1.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 500, exit);

    idx = 3;   
    fld = SRAM_BUFFERS_CLR_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_th_mnt_nof_bits, pool_th_exp_nof_bits, info->p1.hp.clear, &exact_info->p1.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 510, exit);

    fld = SRAM_BUFFERS_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_th_mnt_nof_bits, pool_th_exp_nof_bits, info->p1.hp.set, &exact_info->p1.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 520, exit);
    

    
    pool_pd_th_mnt_nof_bits = 8;
    fld = SRAM_PDS_CLR_THf;
    pool_pd_th_exp_nof_bits = soc_mem_field_length(unit, mem, fld) - pool_pd_th_mnt_nof_bits;
    idx = 0;   

    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_pd_th_mnt_nof_bits, pool_pd_th_exp_nof_bits, info->p0_pd.lp.clear, &exact_info->p0_pd.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 530, exit);

    fld = SRAM_PDS_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_pd_th_mnt_nof_bits, pool_pd_th_exp_nof_bits, info->p0_pd.lp.set, &exact_info->p0_pd.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 540, exit);


    idx = 2;   
    fld = SRAM_PDS_CLR_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_pd_th_mnt_nof_bits, pool_pd_th_exp_nof_bits, info->p0_pd.hp.clear, &exact_info->p0_pd.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 550, exit);

    fld = SRAM_PDS_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_pd_th_mnt_nof_bits, pool_pd_th_exp_nof_bits, info->p0_pd.hp.set, &exact_info->p0_pd.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 560, exit);
    

    
    idx = 1;   
    fld = SRAM_PDS_CLR_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_pd_th_mnt_nof_bits, pool_pd_th_exp_nof_bits, info->p1_pd.lp.clear, &exact_info->p1_pd.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 570, exit);

    fld = SRAM_PDS_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_pd_th_mnt_nof_bits, pool_pd_th_exp_nof_bits, info->p1_pd.lp.set, &exact_info->p1_pd.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 580, exit);

    idx = 3;   
    fld = SRAM_PDS_CLR_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_pd_th_mnt_nof_bits, pool_pd_th_exp_nof_bits, info->p1_pd.hp.clear, &exact_info->p1_pd.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 590, exit);

    fld = SRAM_PDS_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_pd_th_mnt_nof_bits, pool_pd_th_exp_nof_bits, info->p1_pd.hp.set, &exact_info->p1_pd.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 600, exit);
    

    
    pool_byte_th_mnt_nof_bits = 8;
    fld = WORDS_CLR_THf;
    pool_byte_th_exp_nof_bits = soc_mem_field_length(unit, mem, fld) - pool_byte_th_mnt_nof_bits;

    idx = 0;   
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_byte_th_mnt_nof_bits, pool_byte_th_exp_nof_bits, info->p0_byte.lp.clear, &exact_info->p0_byte.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 610, exit);

    fld = WORDS_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_byte_th_mnt_nof_bits, pool_byte_th_exp_nof_bits, info->p0_byte.lp.set, &exact_info->p0_byte.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 620, exit);
	
    idx = 2;   
    fld = WORDS_CLR_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_byte_th_mnt_nof_bits, pool_byte_th_exp_nof_bits, info->p0_byte.hp.clear, &exact_info->p0_byte.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 630, exit);

    fld = WORDS_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_byte_th_mnt_nof_bits, pool_byte_th_exp_nof_bits, info->p0_byte.hp.set, &exact_info->p0_byte.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 640, exit);
    

    
    idx = 1;   
    fld = WORDS_CLR_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_byte_th_mnt_nof_bits, pool_byte_th_exp_nof_bits, info->p1_byte.lp.clear, &exact_info->p1_byte.lp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 650, exit);

    fld = WORDS_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_byte_th_mnt_nof_bits, pool_byte_th_exp_nof_bits, info->p1_byte.lp.set, &exact_info->p1_byte.lp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 660, exit);
    
    idx = 3;   
    fld = WORDS_CLR_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_byte_th_mnt_nof_bits, pool_byte_th_exp_nof_bits, info->p1_byte.hp.clear, &exact_info->p1_byte.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 670, exit);

    fld = WORDS_SET_THf;
    res = arad_itm_glob_res_fc_mnt_exp_mem_field_set(unit, mem, idx, fld, pool_byte_th_mnt_nof_bits, pool_byte_th_exp_nof_bits, info->p1_byte.hp.set, &exact_info->p1_byte.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 680, exit);
    

    
    reg = CGM_SRAM_HDRM_RSRC_FLOW_CONTROL_THSr;
    fld = HDRM_SRAM_BUFFERS_FC_TH_CLRf;
    hdrm_th_mnt_nof_bits = 8;
    hdrm_th_exp_nof_bits = soc_reg_field_length(unit, reg, fld) - hdrm_th_mnt_nof_bits;

    fld = HDRM_SRAM_BUFFERS_FC_TH_CLRf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, hdrm_th_mnt_nof_bits, hdrm_th_exp_nof_bits, info->hdrm.hp.clear, &exact_info->hdrm.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 690, exit);
	
    fld = HDRM_SRAM_BUFFERS_FC_TH_SETf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, hdrm_th_mnt_nof_bits, hdrm_th_exp_nof_bits, info->hdrm.hp.set, &exact_info->hdrm.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 700, exit);
    

    
    reg = CGM_SRAM_HDRM_RSRC_FLOW_CONTROL_THSr;
    fld = HDRM_SRAM_PDS_FC_TH_CLRf;
    hdrm_th_mnt_nof_bits = 8;
    hdrm_th_exp_nof_bits = soc_reg_field_length(unit, reg, fld) - hdrm_th_mnt_nof_bits;

    fld = HDRM_SRAM_PDS_FC_TH_CLRf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, hdrm_th_mnt_nof_bits, hdrm_th_exp_nof_bits, info->hdrm_pd.hp.clear, &exact_info->hdrm.hp.clear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 710, exit);

    fld = HDRM_SRAM_PDS_FC_TH_SETf;
    res = arad_itm_glob_res_fc_mnt_exp_field_set(unit, reg, fld, hdrm_th_mnt_nof_bits, hdrm_th_exp_nof_bits, info->hdrm_pd.hp.set, &exact_info->hdrm.hp.set);
    SOC_SAND_CHECK_FUNC_RESULT(res, 720, exit);
    
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_fc_set_unsafe()",0,0);
}

static uint32
  arad_itm_glob_res_fc_one_type_verify(
    SOC_SAND_IN   int                          unit,
    SOC_SAND_IN   SOC_TMC_ITM_GLOB_RCS_FC_TYPE fc_th,
    SOC_SAND_IN   uint32                       max_value
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    fc_th.hp.clear, max_value,
    ARAD_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    fc_th.hp.set, max_value,
    ARAD_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 20, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    fc_th.lp.clear, max_value,
    ARAD_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 30, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    fc_th.lp.set, max_value,
    ARAD_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 40, exit
    );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_res_fc_one_type_verify()",0,0);
}


uint32
  arad_itm_glob_rcs_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_GLOB_RCS_FC_TH  *info
  )
{
  uint32 
    res;
  uint32
    max_val;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_FC_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  
  max_val = SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_bdbs_size_max);
  res = arad_itm_glob_res_fc_one_type_verify(unit, info->bdbs, max_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->unicast, SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_uc_size_max));
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  
  if(!SOC_IS_QAX(unit)){
    max_val = SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_fmc_size_max);
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->full_mc, max_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
    
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->mini_mc, SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_mnmc_size_max));
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->ocb, SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_ocb_size_max));
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->ocb_p0, SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_ocb_p0_size_max));
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->ocb_p1, SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_ocb_p1_size_max));
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->mix_p0, SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_mix_p0_size_max));
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->mix_p1, SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_mix_p1_size_max));
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  }

  if (SOC_IS_QAX(unit)) {
    
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->ocb, SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_ocb_size_max));
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->ocb_pdb, SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_ocb_pdb_size_max));
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

    
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->p0, SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_size_max));
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

    
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->p1, SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p1_size_max));
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

    
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->p0_pd, SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_pd_size_max));
    SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

    
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->p1_pd, SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p1_pd_size_max));
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

    
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->p0_byte, SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_byte_size_max));
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

    
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->p1_byte, SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p1_byte_size_max));
    SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

    
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->hdrm, SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_headroom_size_max));
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

    
    res = arad_itm_glob_res_fc_one_type_verify(unit, info->hdrm_pd, SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_headroom_pd_size_max));
    SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_fc_verify()",0,0);
}


uint32
  arad_itm_glob_rcs_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_FC_TH  *info
  )
{
  uint32
      res,
      bdbs_th_mnt_nof_bits,
      uc_th_mnt_nof_bits,
      fmc_th_mnt_nof_bits,
      mnmc_th_mnt_nof_bits,
      ocb_th_mnt_nof_bits,
      ocb_p0_th_mnt_nof_bits,
      ocb_p1_th_mnt_nof_bits,
      mix_p0_th_mnt_nof_bits,
      mix_p1_th_mnt_nof_bits,
      ocb_pdb_th_mnt_nof_bits,
      hdrm_th_mnt_nof_bits,
      hdrm_pd_th_mnt_nof_bits,
      p0_th_mnt_nof_bits,
      p1_th_mnt_nof_bits,
      p0_pd_th_mnt_nof_bits,
      p1_pd_th_mnt_nof_bits,
      p0_byte_th_mnt_nof_bits,
      p1_byte_th_mnt_nof_bits,
      exp_man,
      mnt_val,
      exp_val;
  soc_reg_t
    reg = INVALIDr;
  soc_field_t
    fld = INVALIDf;
  soc_mem_t
    mem = INVALIDm;
  soc_reg_above_64_val_t
    fc_data;
  int idx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_FC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  
  if(!SOC_IS_QAX(unit)) {
    bdbs_th_mnt_nof_bits = 8;
    reg = SOC_IS_JERICHO(unit) ? IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_BDBr : 
                                 IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_BDB_HIGH_PRIORITYr;
    fld = FC_CLR_FR_BDB_TH_HPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, bdbs_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->bdbs.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = FC_SET_FR_BDB_TH_HPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, bdbs_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->bdbs.hp.set = (mnt_val) * (1<<(exp_val));

    reg = SOC_IS_JERICHO(unit) ? IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_BDBr : 
                                 IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_BDB_LOW_PRIORITYr;
    fld = FC_CLR_FR_BDB_TH_LPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, bdbs_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->bdbs.lp.clear = (mnt_val) * (1<<(exp_val));

    fld = FC_SET_FR_BDB_TH_LPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, bdbs_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->bdbs.lp.set = (mnt_val) * (1<<(exp_val));
  } else { 
    bdbs_th_mnt_nof_bits = 8;	 
    reg = CGM_TOTAL_DRAM_RSRC_FLOW_CONTROL_THSr;

    fld = TOTAL_DRAM_BDBS_FC_LP_TH_CLRf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, bdbs_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->bdbs.lp.clear = (mnt_val) * (1<<(exp_val));

    fld = TOTAL_DRAM_BDBS_FC_LP_TH_SETf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, bdbs_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->bdbs.lp.set = (mnt_val) * (1<<(exp_val));

    fld = TOTAL_DRAM_BDBS_FC_HP_TH_CLRf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, bdbs_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->bdbs.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = TOTAL_DRAM_BDBS_FC_HP_TH_SETf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, bdbs_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->bdbs.hp.set = (mnt_val) * (1<<(exp_val));
  }
  

  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
    
    uc_th_mnt_nof_bits = 7;
    reg = IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr;
    fld = FC_CLR_FR_DB_UC_TH_HPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, uc_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->unicast.hp.clear = (mnt_val) * (1<<(exp_val));

    reg = IQM_GENERAL_SET_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr;
    fld = FC_SET_FR_DB_UC_TH_HPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, uc_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->unicast.hp.set = (mnt_val) * (1<<(exp_val));

    reg = IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_LOW_PRIORITYr;
    fld = FC_CLR_FR_DB_UC_TH_LPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, uc_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->unicast.lp.clear = (mnt_val) * (1<<(exp_val));

    reg = IQM_GENERAL_SET_FLOW_CONTROL_CONFIGURATION_D_BS_LOW_PRIORITYr;
    fld = FC_SET_FR_DB_UC_TH_LPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, uc_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->unicast.lp.set = (mnt_val) * (1<<(exp_val));
    
  }

  
  if (!SOC_IS_QAX(unit)) {
    fmc_th_mnt_nof_bits = SOC_IS_JERICHO(unit) ? 8 : 7;
    reg = SOC_IS_JERICHO(unit) ? IQM_FLOW_CONTROL_CONFIGURATION_FULL_MC_D_BSr : 
                                 IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr;
    fld = FC_CLR_FR_DB_FLMC_TH_HPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, fmc_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->full_mc.hp.clear = (mnt_val) * (1<<(exp_val));

    reg = SOC_IS_JERICHO(unit) ? IQM_FLOW_CONTROL_CONFIGURATION_FULL_MC_D_BSr : 
                                 IQM_GENERAL_SET_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr;
    fld = FC_SET_FR_DB_FLMC_TH_HPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, fmc_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->full_mc.hp.set = (mnt_val) * (1<<(exp_val));

    reg = SOC_IS_JERICHO(unit) ? IQM_FLOW_CONTROL_CONFIGURATION_FULL_MC_D_BSr : 
                                 IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_LOW_PRIORITYr;
    fld = FC_CLR_FR_DB_FLMC_TH_LPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, fmc_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->full_mc.lp.clear = (mnt_val) * (1<<(exp_val));

    reg = SOC_IS_JERICHO(unit) ? IQM_FLOW_CONTROL_CONFIGURATION_FULL_MC_D_BSr : 
                                 IQM_GENERAL_SET_FLOW_CONTROL_CONFIGURATION_D_BS_LOW_PRIORITYr;
    fld = FC_SET_FR_DB_FLMC_TH_LPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, fmc_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->full_mc.lp.set = (mnt_val) * (1<<(exp_val));
  }
  

  if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
    
    mnmc_th_mnt_nof_bits = 8;
    reg = IQM_FLOW_CONTROL_CONFIGURATION_MINI_MC_D_BSr;

    fld = FC_CLR_FR_DB_MNMC_TH_HPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, mnmc_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->mini_mc.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = FC_SET_FR_DB_MNMC_TH_HPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, mnmc_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->mini_mc.hp.set = (mnt_val) * (1<<(exp_val));

    fld = FC_CLR_FR_DB_MNMC_TH_LPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, mnmc_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->mini_mc.lp.clear = (mnt_val) * (1<<(exp_val));

    fld = FC_SET_FR_DB_MNMC_TH_LPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  160,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, mnmc_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->mini_mc.lp.set = (mnt_val) * (1<<(exp_val));
    

    
    ocb_th_mnt_nof_bits = 8;   
    reg = IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_OCBr;

    fld = FC_CLR_FR_OCB_TH_HPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = FC_SET_FR_OCB_TH_HPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb.hp.set = (mnt_val) * (1<<(exp_val));

    fld = FC_CLR_FR_OCB_TH_LPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  190,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb.lp.clear = (mnt_val) * (1<<(exp_val));

    fld = FC_SET_FR_OCB_TH_LPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  200,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb.lp.set = (mnt_val) * (1<<(exp_val));
    

    
    ocb_p0_th_mnt_nof_bits = 8;
    reg = IQM_OCB_SHRD_POOL_0_RSRC_FC_THr;

    fld = OCB_SHRD_POOL_0_HP_FC_CLR_THf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_p0_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb_p0.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = OCB_SHRD_POOL_0_HP_FC_SET_THf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_p0_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb_p0.hp.set = (mnt_val) * (1<<(exp_val));

    fld = OCB_SHRD_POOL_0_LP_FC_CLR_THf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  190,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_p0_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb_p0.lp.clear = (mnt_val) * (1<<(exp_val));

    fld = OCB_SHRD_POOL_0_LP_FC_SET_THf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  200,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_p0_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb_p0.lp.set = (mnt_val) * (1<<(exp_val));
    

    
    ocb_p1_th_mnt_nof_bits = 8;
    reg = IQM_OCB_SHRD_POOL_1_RSRC_FC_THr;

    fld = OCB_SHRD_POOL_1_HP_FC_CLR_THf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  210,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_p1_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb_p1.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = OCB_SHRD_POOL_1_HP_FC_SET_THf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  220,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_p1_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb_p1.hp.set = (mnt_val) * (1<<(exp_val));

    fld = OCB_SHRD_POOL_1_LP_FC_CLR_THf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  230,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_p1_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb_p1.lp.clear = (mnt_val) * (1<<(exp_val));

    fld = OCB_SHRD_POOL_1_LP_FC_SET_THf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  240,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_p1_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb_p1.lp.set = (mnt_val) * (1<<(exp_val));
    

    
    mix_p0_th_mnt_nof_bits = 8;
    reg = IQM_MIX_SHRD_POOL_0_RSRC_FC_THr;

    fld = MIX_SHRD_POOL_0_HP_FC_CLR_THf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  250,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, mix_p0_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->mix_p0.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = MIX_SHRD_POOL_0_HP_FC_SET_THf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  260,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, mix_p0_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->mix_p0.hp.set = (mnt_val) * (1<<(exp_val));

    fld = MIX_SHRD_POOL_0_LP_FC_CLR_THf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  270,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, mix_p0_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->mix_p0.lp.clear = (mnt_val) * (1<<(exp_val));

    fld = MIX_SHRD_POOL_0_LP_FC_SET_THf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  280,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, mix_p0_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->mix_p0.lp.set = (mnt_val) * (1<<(exp_val));
    

    
    mix_p1_th_mnt_nof_bits = 8;
    reg = IQM_MIX_SHRD_POOL_1_RSRC_FC_THr;

    fld = MIX_SHRD_POOL_1_HP_FC_CLR_THf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  250,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, mix_p1_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->mix_p1.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = MIX_SHRD_POOL_1_HP_FC_SET_THf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  260,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, mix_p1_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->mix_p1.hp.set = (mnt_val) * (1<<(exp_val));

    fld = MIX_SHRD_POOL_1_LP_FC_CLR_THf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  270,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, mix_p1_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->mix_p1.lp.clear = (mnt_val) * (1<<(exp_val));

    fld = MIX_SHRD_POOL_1_LP_FC_SET_THf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  280,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, mix_p1_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->mix_p1.lp.set = (mnt_val) * (1<<(exp_val));
    
  }

  if (SOC_IS_QAX(unit)) {
    
    ocb_pdb_th_mnt_nof_bits = 8;   
    reg = CGM_TOTAL_SRAM_RSRC_FLOW_CONTROL_THSr;

    fld = TOTAL_SRAM_PDBS_FC_LP_TH_CLRf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_pdb_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb_pdb.lp.clear = (mnt_val) * (1<<(exp_val));

    fld = TOTAL_SRAM_PDBS_FC_LP_TH_SETf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_pdb_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb_pdb.lp.set = (mnt_val) * (1<<(exp_val));

    fld = TOTAL_SRAM_PDBS_FC_HP_TH_CLRf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  190,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_pdb_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb_pdb.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = TOTAL_SRAM_PDBS_FC_HP_TH_SETf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  200,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_pdb_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb_pdb.hp.set = (mnt_val) * (1<<(exp_val));
    
	
    
    ocb_th_mnt_nof_bits = 8;   
    reg = CGM_TOTAL_SRAM_RSRC_FLOW_CONTROL_THSr;

    fld = TOTAL_SRAM_BUFFERS_FC_LP_TH_CLRf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb.lp.clear = (mnt_val) * (1<<(exp_val));

    fld = TOTAL_SRAM_BUFFERS_FC_LP_TH_SETf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb.lp.set = (mnt_val) * (1<<(exp_val));

    fld = TOTAL_SRAM_BUFFERS_FC_HP_TH_CLRf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  190,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = TOTAL_SRAM_BUFFERS_FC_HP_TH_SETf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  200,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, ocb_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->ocb.hp.set = (mnt_val) * (1<<(exp_val));
    

    
    p0_th_mnt_nof_bits = 8;
    mem = CGM_PB_VSQ_POOL_FC_THm;
    idx = 0;   
    SOC_REG_ABOVE_64_CLEAR(fc_data);
    res = READ_CGM_PB_VSQ_POOL_FC_THm(unit, MEM_BLOCK_ANY, idx, &fc_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);

    fld = SRAM_BUFFERS_CLR_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p0_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p0.lp.clear = (mnt_val) * (1<<(exp_val));

    fld = SRAM_BUFFERS_SET_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p0_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p0.lp.set = (mnt_val) * (1<<(exp_val));

    idx = 2;   
    SOC_REG_ABOVE_64_CLEAR(fc_data);
    res = READ_CGM_PB_VSQ_POOL_FC_THm(unit, MEM_BLOCK_ANY, idx, &fc_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);

    fld = SRAM_BUFFERS_CLR_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p0_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p0.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = SRAM_BUFFERS_SET_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p0_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p0.hp.set = (mnt_val) * (1<<(exp_val));
    

    
    p1_th_mnt_nof_bits = 8;
    mem = CGM_PB_VSQ_POOL_FC_THm;
    idx = 1;   
    SOC_REG_ABOVE_64_CLEAR(fc_data);
    res = READ_CGM_PB_VSQ_POOL_FC_THm(unit, MEM_BLOCK_ANY, idx, &fc_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);

    fld = SRAM_BUFFERS_CLR_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p1_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p1.lp.clear = (mnt_val) * (1<<(exp_val));

    fld = SRAM_BUFFERS_SET_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p1_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p1.lp.set = (mnt_val) * (1<<(exp_val));

    idx = 3;   
    SOC_REG_ABOVE_64_CLEAR(fc_data);
    res = READ_CGM_PB_VSQ_POOL_FC_THm(unit, MEM_BLOCK_ANY, idx, &fc_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);

    fld = SRAM_BUFFERS_CLR_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p1_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p1.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = SRAM_BUFFERS_SET_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p1_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p1.hp.set = (mnt_val) * (1<<(exp_val));
    

    
    p0_pd_th_mnt_nof_bits = 8;
    mem = CGM_PB_VSQ_POOL_FC_THm;
    idx = 0;   
    SOC_REG_ABOVE_64_CLEAR(fc_data);
    res = READ_CGM_PB_VSQ_POOL_FC_THm(unit, MEM_BLOCK_ANY, idx, &fc_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);

    fld = SRAM_PDS_CLR_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p0_pd_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p0_pd.lp.clear = (mnt_val) * (1<<(exp_val));

    fld = SRAM_PDS_SET_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p0_pd_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p0_pd.lp.set = (mnt_val) * (1<<(exp_val));

    idx = 2;   
    SOC_REG_ABOVE_64_CLEAR(fc_data);
    res = READ_CGM_PB_VSQ_POOL_FC_THm(unit, MEM_BLOCK_ANY, idx, &fc_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);

    fld = SRAM_PDS_CLR_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p0_pd_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p0_pd.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = SRAM_PDS_SET_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p0_pd_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p0_pd.hp.set = (mnt_val) * (1<<(exp_val));
    

    
    p1_pd_th_mnt_nof_bits = 8;
    mem = CGM_PB_VSQ_POOL_FC_THm;
    idx = 1;   
    SOC_REG_ABOVE_64_CLEAR(fc_data);
    res = READ_CGM_PB_VSQ_POOL_FC_THm(unit, MEM_BLOCK_ANY, idx, &fc_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);

    fld = SRAM_PDS_CLR_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p1_pd_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p1_pd.lp.clear = (mnt_val) * (1<<(exp_val));

    fld = SRAM_PDS_SET_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p1_pd_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p1_pd.lp.set = (mnt_val) * (1<<(exp_val));

    idx = 3;   
    SOC_REG_ABOVE_64_CLEAR(fc_data);
    res = READ_CGM_PB_VSQ_POOL_FC_THm(unit, MEM_BLOCK_ANY, idx, &fc_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);

    fld = SRAM_PDS_CLR_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p1_pd_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p1_pd.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = SRAM_PDS_SET_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p1_pd_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p1_pd.hp.set = (mnt_val) * (1<<(exp_val));
    

    
    p0_byte_th_mnt_nof_bits = 8;
    mem = CGM_PB_VSQ_POOL_FC_THm;
    idx = 0;   	
    SOC_REG_ABOVE_64_CLEAR(fc_data);
    res = READ_CGM_PB_VSQ_POOL_FC_THm(unit, MEM_BLOCK_ANY, idx, &fc_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);

    fld = WORDS_CLR_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p0_byte_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p0_byte.lp.clear = (mnt_val) * (1<<(exp_val));

    fld = WORDS_SET_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p0_byte_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p0_byte.lp.set = (mnt_val) * (1<<(exp_val));

    idx = 2;   
    SOC_REG_ABOVE_64_CLEAR(fc_data);
    res = READ_CGM_PB_VSQ_POOL_FC_THm(unit, MEM_BLOCK_ANY, idx, &fc_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);

    fld = WORDS_CLR_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p0_byte_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p0_byte.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = WORDS_SET_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p0_byte_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p0_byte.hp.set = (mnt_val) * (1<<(exp_val));
    

    
    p1_byte_th_mnt_nof_bits = 8;
    mem = CGM_PB_VSQ_POOL_FC_THm;
    idx = 1;   
    SOC_REG_ABOVE_64_CLEAR(fc_data);
    res = READ_CGM_PB_VSQ_POOL_FC_THm(unit, MEM_BLOCK_ANY, idx, &fc_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);

    fld = WORDS_CLR_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p1_byte_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p1_byte.lp.clear = (mnt_val) * (1<<(exp_val));

    fld = WORDS_SET_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p1_byte_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p1_byte.lp.set = (mnt_val) * (1<<(exp_val));

    idx = 3;   
    SOC_REG_ABOVE_64_CLEAR(fc_data);
    res = READ_CGM_PB_VSQ_POOL_FC_THm(unit, MEM_BLOCK_ANY, idx, &fc_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);

    fld = WORDS_CLR_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p1_byte_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p1_byte.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = WORDS_SET_THf;
    exp_man = soc_mem_field32_get(unit, mem, &fc_data, fld);
    arad_iqm_mantissa_exponent_get(unit, exp_man, p1_byte_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->p1_byte.hp.set = (mnt_val) * (1<<(exp_val));
    

    
    hdrm_th_mnt_nof_bits = 8;   
    reg = CGM_SRAM_HDRM_RSRC_FLOW_CONTROL_THSr;

    fld = HDRM_SRAM_BUFFERS_FC_TH_CLRf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  190,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, hdrm_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->hdrm.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = HDRM_SRAM_BUFFERS_FC_TH_SETf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  200,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, hdrm_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->hdrm.hp.set = (mnt_val) * (1<<(exp_val));
    

    
    hdrm_pd_th_mnt_nof_bits = 8;   
    reg = CGM_SRAM_HDRM_RSRC_FLOW_CONTROL_THSr;

    fld = HDRM_SRAM_PDS_FC_TH_CLRf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, hdrm_pd_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->hdrm_pd.hp.clear = (mnt_val) * (1<<(exp_val));

    fld = HDRM_SRAM_PDS_FC_TH_SETf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, fld, &exp_man));
    arad_iqm_mantissa_exponent_get(unit, exp_man, hdrm_pd_th_mnt_nof_bits, &mnt_val, &exp_val);
    info->hdrm_pd.hp.set = (mnt_val) * (1<<(exp_val));
    
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_fc_get_unsafe()",0,0);
}


uint32
  arad_itm_glob_rcs_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_GLOB_RCS_DROP_TH *info,
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_DROP_TH *exact_info
  )
{
  uint32
    res,
      exp_man,
    bdbs_th_mnt_nof_bits,
    bdbs_th_exp_nof_bits,
    bds_th_mnt_nof_bits,
    bds_th_exp_nof_bits,
    uc_th_mnt_nof_bits,
    uc_th_exp_nof_bits,
    mini_mc_mnt_nof_bits,
    mini_mc_exp_nof_bits,
    fmc_th_mnt_nof_bits,
    fmc_th_exp_nof_bits;
  uint32
    mnt_val,
    exp_val,
    fld_val,
    indx;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_DROP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  

  

  bdbs_th_mnt_nof_bits = 8;
  bdbs_th_exp_nof_bits = soc_reg_field_length(unit, IQM_GENERAL_REJECT_CONFIGURATION_BD_BS_0r, RJCT_CLR_FR_BDB_TH_0f) - bdbs_th_mnt_nof_bits;

  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              info->bdbs[indx].clear,
              bdbs_th_mnt_nof_bits,
              bdbs_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      exact_info->bdbs[indx].clear = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          bdbs_th_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_CONFIGURATION_BD_BS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_FR_BDB_TH_0f + indx,  exp_man));

      res = soc_sand_break_to_mnt_exp_round_up(
              info->bdbs[indx].set,
              bdbs_th_mnt_nof_bits,
              bdbs_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      exact_info->bdbs[indx].set = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          bdbs_th_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_CONFIGURATION_BD_BS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_FR_BDB_TH_0f + indx,  exp_man));
  }

  
  bds_th_mnt_nof_bits = 12;
  bds_th_exp_nof_bits = soc_reg_field_length(unit, IQM_GENERAL_REJECT_CONFIGURATION_B_DS_0r, RJCT_CLR_OC_BD_TH_0f) - bds_th_mnt_nof_bits;

  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              info->bds[indx].clear,
              bds_th_mnt_nof_bits,
              bds_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      exact_info->bds[indx].clear = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          bds_th_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_CONFIGURATION_B_DS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_OC_BD_TH_0f + indx,  exp_man));

      res = soc_sand_break_to_mnt_exp_round_up(
              info->bds[indx].set,
              bds_th_mnt_nof_bits,
              bds_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

      exact_info->bds[indx].set = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          bds_th_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_CONFIGURATION_B_DS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_OC_BD_TH_0f + indx,  exp_man));
  }

  
  uc_th_mnt_nof_bits = 7;
  uc_th_exp_nof_bits = soc_reg_field_length(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r, RJCT_CLR_FR_DB_UC_TH_0f) - uc_th_mnt_nof_bits;

  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              info->unicast[indx].clear,
              uc_th_mnt_nof_bits,
              uc_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

      exact_info->unicast[indx].clear = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          uc_th_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_FR_DB_UC_TH_0f + indx,  exp_man));

      res = soc_sand_break_to_mnt_exp_round_up(
              info->unicast[indx].set,
              uc_th_mnt_nof_bits,
              uc_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

      exact_info->unicast[indx].set = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          uc_th_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_SET_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_FR_DB_UC_TH_0f + indx,  exp_man));
  }

  

  mini_mc_mnt_nof_bits = 6;
  mini_mc_exp_nof_bits = soc_reg_field_length(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r, RJCT_CLR_FR_DB_MNMC_TH_0f) - mini_mc_mnt_nof_bits;

  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              info->mini_mc[indx].clear,
              mini_mc_mnt_nof_bits,
              mini_mc_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

      exact_info->mini_mc[indx].clear = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          mini_mc_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_FR_DB_MNMC_TH_0f + indx,  exp_man));

      res = soc_sand_break_to_mnt_exp_round_up(
              info->mini_mc[indx].set,
              mini_mc_mnt_nof_bits,
              mini_mc_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

      exact_info->mini_mc[indx].set = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          mini_mc_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  160,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_SET_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_FR_DB_MNMC_TH_0f + indx,  exp_man));
  }

  

  fmc_th_mnt_nof_bits = 7;
  fmc_th_exp_nof_bits = soc_reg_field_length(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r, RJCT_CLR_FR_DB_FLMC_TH_0f) - fmc_th_mnt_nof_bits;

  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              info->full_mc[indx].clear,
              fmc_th_mnt_nof_bits,
              fmc_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

      exact_info->full_mc[indx].clear = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          fmc_th_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_FR_DB_FLMC_TH_0f + indx,  exp_man));

      res = soc_sand_break_to_mnt_exp_round_up(
              info->full_mc[indx].set,
              fmc_th_mnt_nof_bits,
              fmc_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

      exact_info->full_mc[indx].set = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          fmc_th_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  200,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_SET_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_FR_DB_FLMC_TH_0f + indx,  exp_man));
  }

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
      
      for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++) {
          fld_val = info->ocb_uc[indx].set;
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  450,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_RJCT_UC_OCB_TH_0r + indx, REG_PORT_ANY, 0, RJCT_SET_FR_UC_OCB_TH_0f + indx,  fld_val));
          fld_val = info->ocb_uc[indx].clear;
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  460,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_RJCT_UC_OCB_TH_0r + indx, REG_PORT_ANY, 0, RJCT_CLR_FR_UC_OCB_TH_0f + indx,  fld_val));
      }

      
      for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++) {
          fld_val = info->ocb_mc[indx].set;
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  470,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_RJCT_MC_OCB_TH_0r + indx, REG_PORT_ANY, 0, RJCT_SET_FR_MC_OCB_TH_0f + indx,  fld_val));
          fld_val = info->ocb_mc[indx].clear;
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  480,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_RJCT_MC_OCB_TH_0r + indx, REG_PORT_ANY, 0, RJCT_CLR_FR_MC_OCB_TH_0f + indx,  fld_val));
      }
  }
#endif

  
    res = arad_b_itm_glob_rcs_drop_set_unsafe(
            unit,
            info->mem_excess,
            exact_info->mem_excess
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_drop_set_unsafe()",0,0);
}


uint32
  arad_itm_glob_rcs_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_GLOB_RCS_DROP_TH *info
  )
{
  uint32
    res = SOC_SAND_OK,
    index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_DROP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  

  for (index = 0 ; index < ARAD_NOF_DROP_PRECEDENCE; index++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->bdbs[index].clear, ARAD_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 10, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->bdbs[index].set, ARAD_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 20, exit
      );
  }

  

  for (index = 0 ; index < ARAD_NOF_DROP_PRECEDENCE; index++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->bdbs[index].clear, ARAD_ITM_GLOB_RCS_DROP_BDS_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 10, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->bdbs[index].set, ARAD_ITM_GLOB_RCS_DROP_BDS_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 20, exit
      );
  }

  

  for (index = 0 ; index < ARAD_NOF_DROP_PRECEDENCE; index++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->unicast[index].clear, ARAD_ITM_GLOB_RCS_DROP_UC_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 30, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->unicast[index].set, ARAD_ITM_GLOB_RCS_DROP_UC_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 40, exit
      );
  }

  

  for (index = 0 ; index < ARAD_NOF_DROP_PRECEDENCE; index++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->mini_mc[index].clear, ARAD_ITM_GLOB_RCS_DROP_MINI_MC_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 50, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->mini_mc[index].set, ARAD_ITM_GLOB_RCS_DROP_MINI_MC_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 60, exit
      );
  }

  

  for (index = 0 ; index < ARAD_NOF_DROP_PRECEDENCE; index++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->full_mc[index].clear, ARAD_ITM_GLOB_RCS_DROP_FMC_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 70, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->full_mc[index].set, ARAD_ITM_GLOB_RCS_DROP_FMC_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 80, exit
      );

  }

  

  for (index = 0 ; index < ARAD_NOF_DROP_PRECEDENCE; index++)
  {
    
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->ocb_uc[index].clear, ARAD_ITM_GLOB_RCS_DROP_OCB_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 84, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->ocb_uc[index].set, ARAD_ITM_GLOB_RCS_DROP_OCB_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 85, exit
      );

    
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->ocb_mc[index].clear, 0x7FFF,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 84, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->ocb_mc[index].set, 0x7FFF,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 85, exit
      );
  }

  
    res = arad_b_itm_glob_rcs_drop_verify(
            unit,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_drop_verify()",0,0);
}


uint32
  arad_itm_glob_rcs_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_DROP_TH *info
  )
{
  uint32
    res,
      exp_man = 0,
      bdbs_th_mnt_nof_bits,
      bds_th_mnt_nof_bits,
      uc_th_mnt_nof_bits,
      mini_mc_mnt_nof_bits,
      fmc_th_mnt_nof_bits,
    mnt_val,
    exp_val,
    fld_val;
  uint32
    indx;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  

  

  bdbs_th_mnt_nof_bits = 8;
  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_CONFIGURATION_BD_BS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_FR_BDB_TH_0f + indx, &exp_man));

      arad_iqm_mantissa_exponent_get(
          unit,
          exp_man,
          bdbs_th_mnt_nof_bits,
          &mnt_val,
          &exp_val
      );

     info->bdbs[indx].clear = (mnt_val) * (1<<(exp_val));

     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_CONFIGURATION_BD_BS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_FR_BDB_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         bdbs_th_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->bdbs[indx].set = (mnt_val) * (1<<(exp_val));
  }

  

  bds_th_mnt_nof_bits = 12;
  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_CONFIGURATION_B_DS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_OC_BD_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         bds_th_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->bds[indx].clear = (mnt_val) * (1<<(exp_val));

     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_CONFIGURATION_B_DS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_OC_BD_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         bds_th_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->bds[indx].set = (mnt_val) * (1<<(exp_val));
  }

  

  uc_th_mnt_nof_bits = 7;
  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  210,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_FR_DB_UC_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         uc_th_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->unicast[indx].clear = (mnt_val) * (1<<(exp_val));

     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  230,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_SET_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_FR_DB_UC_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         uc_th_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->unicast[indx].set = (mnt_val) * (1<<(exp_val));
  }

  

  mini_mc_mnt_nof_bits = 6;
  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  310,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_FR_DB_MNMC_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         mini_mc_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->mini_mc[indx].clear = (mnt_val) * (1<<(exp_val));

     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  340,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_SET_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_FR_DB_MNMC_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         mini_mc_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->mini_mc[indx].set = (mnt_val) * (1<<(exp_val));
  }

  

  fmc_th_mnt_nof_bits = 7;
  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  410,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_FR_DB_FLMC_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         fmc_th_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->full_mc[indx].clear = (mnt_val) * (1<<(exp_val));

     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  440,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_SET_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_FR_DB_FLMC_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         fmc_th_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->full_mc[indx].set = (mnt_val) * (1<<(exp_val));
  }

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
      
      for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  450,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_RJCT_UC_OCB_TH_0r + indx, REG_PORT_ANY, 0, RJCT_SET_FR_UC_OCB_TH_0f + indx, &fld_val));
          info->ocb_uc[indx].set = fld_val;
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  460,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_RJCT_UC_OCB_TH_0r + indx, REG_PORT_ANY, 0, RJCT_CLR_FR_UC_OCB_TH_0f + indx, &fld_val));
          info->ocb_uc[indx].clear= fld_val;
      }

      
      for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  470,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_RJCT_MC_OCB_TH_0r + indx, REG_PORT_ANY, 0, RJCT_SET_FR_MC_OCB_TH_0f + indx, &fld_val));
          info->ocb_mc[indx].set = fld_val;
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  480,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_RJCT_MC_OCB_TH_0r + indx, REG_PORT_ANY, 0, RJCT_CLR_FR_MC_OCB_TH_0f + indx, &fld_val));
          info->ocb_mc[indx].clear= fld_val;
      }
  }
#endif

  
    res = arad_b_itm_glob_rcs_drop_get_unsafe(
            unit,
            info->mem_excess
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 490, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_drop_get_unsafe()",0,0);
}




uint32
  arad_itm_category_rngs_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CATEGORY_RNGS *info
  )
{
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CATEGORY_RNGS_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 0, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_PACKET_QUEUES_CATEGORIES_0r, REG_PORT_ANY, 0, TOP_PKT_Q_CAT_0f,  info->vsq_ctgry0_end));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_PACKET_QUEUES_CATEGORIES_1r, REG_PORT_ANY, 0, TOP_PKT_Q_CAT_1f,  info->vsq_ctgry1_end));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 2, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_PACKET_QUEUES_CATEGORIES_2r, REG_PORT_ANY, 0, TOP_PKT_Q_CAT_2f,  info->vsq_ctgry2_end));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_category_rngs_set_unsafe()",0,0);
}


uint32
  arad_itm_category_rngs_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CATEGORY_RNGS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CATEGORY_RNGS_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if ((info->vsq_ctgry0_end > info->vsq_ctgry1_end) ||
      (info->vsq_ctgry1_end > info->vsq_ctgry2_end) ||
      (info->vsq_ctgry0_end > info->vsq_ctgry2_end))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_CATEGORY_END_OUT_OF_ORDER_ERR, 10, exit);
  }
  
  
  if ((info->vsq_ctgry0_end < ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN) ||
    (info->vsq_ctgry0_end > ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MAX(unit)))
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_CATEGORY_END_OUT_OF_RANGE_ERR, 20, exit);

  
  
  if ((info->vsq_ctgry1_end < ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN) ||
    (info->vsq_ctgry1_end > ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MAX(unit)))
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_CATEGORY_END_OUT_OF_RANGE_ERR, 30, exit);

  
  
  if ((info->vsq_ctgry2_end < ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN) ||
    (info->vsq_ctgry2_end > ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MAX(unit)))
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_CATEGORY_END_OUT_OF_RANGE_ERR, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_category_rngs_verify()",0,0);
}


uint32
  arad_itm_category_rngs_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_CATEGORY_RNGS *info
  )
{
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CATEGORY_RNGS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_PACKET_QUEUES_CATEGORIES_0r, REG_PORT_ANY, 0, TOP_PKT_Q_CAT_0f, &info->vsq_ctgry0_end));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_PACKET_QUEUES_CATEGORIES_1r, REG_PORT_ANY, 0, TOP_PKT_Q_CAT_1f, &info->vsq_ctgry1_end));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_PACKET_QUEUES_CATEGORIES_2r, REG_PORT_ANY, 0, TOP_PKT_Q_CAT_2f, &info->vsq_ctgry2_end));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_category_rngs_get_unsafe()",0,0);
}


uint32
  arad_itm_admit_test_tmplt_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_IN  ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  uint32
    res,
    test_a_in_sand_u32,
    test_b_in_sand_u32;
  uint32
    test_a_index,
    test_b_index;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_ADMIT_TEST_TMPLT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  

  res = arad_itm_convert_admit_one_test_tmplt_to_u32(
          info->test_a,
          &test_a_in_sand_u32
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,0,exit);

  test_a_index = admt_tst_ndx;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_REJECT_ADMISSION_Ar, SOC_CORE_ALL, 0, RJCT_TMPLTA_SET_0f + test_a_index,  test_a_in_sand_u32));

  res =
    arad_itm_convert_admit_one_test_tmplt_to_u32(
      info->test_b,
      &test_b_in_sand_u32
    );
  SOC_SAND_CHECK_FUNC_RESULT(res,2,exit);

  test_b_index = admt_tst_ndx;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  3, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_REJECT_ADMISSION_Br, REG_PORT_ANY, 0, RJCT_TMPLTB_SET_0f + test_b_index,  test_b_in_sand_u32));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_admit_test_tmplt_set_unsafe()",0,0);
}


uint32
  arad_itm_admit_test_tmplt_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_IN  ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_ADMIT_TEST_TMPLT_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if (admt_tst_ndx > (ARAD_ITM_ADMIT_TSTS_LAST-1))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_ADMT_TEST_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_admit_test_tmplt_verify()",0,0);
}


uint32
  arad_itm_admit_test_tmplt_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_OUT ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  uint32
    res,
    test_a_in_sand_u32,
    test_b_in_sand_u32;
  uint32
    test_a_index,
    test_b_index;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_ADMIT_TEST_TMPLT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  

  test_a_index = admt_tst_ndx;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_REJECT_ADMISSION_Ar, SOC_CORE_ALL, 0, RJCT_TMPLTA_SET_0f + test_a_index, &test_a_in_sand_u32));

  res =
    arad_itm_convert_u32_to_admit_one_test_tmplt(
      test_a_in_sand_u32,
      &(info->test_a)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  test_b_index = admt_tst_ndx;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_REJECT_ADMISSION_Br, REG_PORT_ANY, 0, RJCT_TMPLTB_SET_0f + test_b_index, &test_b_in_sand_u32));
    res =
      arad_itm_convert_u32_to_admit_one_test_tmplt(
        test_b_in_sand_u32,
        &(info->test_b)
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_admit_test_tmplt_get_unsafe()",0,0);
}

CONST uint32 mode2scan_time[] = ARAD_MIN_SCAN_CYCLE_PERIOD_MICRO_PER_MODE;

STATIC uint32
  arad_itm_get_set_cr_watch_dog_scan_calc_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 scan_time_in_micro, 
    SOC_SAND_OUT uint32 *out_scan_time,
    SOC_SAND_OUT uint32 *out_field_value
  )
{
  uint32 exact_credit_watchdog_scan_time_nano;
  uint32 res, tmp=0;
  int8 mode;
  uint32 scan_ms = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = GET_EXACT_CREDIT_WATCHDOG_SCAN_TIME_NANO(unit, exact_credit_watchdog_scan_time_nano);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2, exit);

  if (scan_time_in_micro || 
      out_field_value || 
      !exact_credit_watchdog_scan_time_nano) { 
    
    res = GET_CREDIT_WATCHDOG_MODE(unit, mode);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
    scan_ms = scan_time_in_micro ? scan_time_in_micro : mode2scan_time[GET_CREDIT_WATCHDOG_MODE_BASE(mode)];

    res = arad_chip_time_to_ticks( unit, scan_ms, FALSE,
      ARAD_ITM_WD_INFO_MIN_SCAN_CYCLE_PERIOD_MICRO_GRANULARITY(unit), TRUE, &tmp); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (out_field_value) {
      *out_field_value = tmp;
    }

    res = arad_ticks_to_time(unit, tmp, TRUE, ARAD_ITM_WD_INFO_MIN_SCAN_CYCLE_PERIOD_MICRO_GRANULARITY(unit), &tmp); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit, mode)) { 
        res = SET_EXACT_CREDIT_WATCHDOG_SCAN_TIME_NANO(unit, (scan_ms * 1000));
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
    } else {
        res = SET_EXACT_CREDIT_WATCHDOG_SCAN_TIME_NANO(unit, tmp);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
    }
    if (out_scan_time) {
      *out_scan_time = tmp;
    }
  } else if (out_scan_time) {
      *out_scan_time = exact_credit_watchdog_scan_time_nano;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_get_set_cr_watch_dog_scan_calc_unsafe()", exact_credit_watchdog_scan_time_nano, tmp);
}



static uint8 _credit_balance_resolution_to_exp[5] = {-1, 0, 1, -1, 2};


#define ARAD_ITM_MAX_CRBL_RESOLUTION_EXPONENT 2
uint32
  arad_itm_cr_request_call_needed_credit_balance_resolution(
    SOC_SAND_IN  uint32  value,       
    SOC_SAND_IN  uint32  max_allowed, 
    SOC_SAND_OUT uint8*  credit_balance_resolution 
  )
{
  uint32 resolution_exp, current_max_allowed = max_allowed; 
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(credit_balance_resolution);

  for (resolution_exp = 0; resolution_exp <= ARAD_ITM_MAX_CRBL_RESOLUTION_EXPONENT; ++resolution_exp) {
    if (value <= current_max_allowed) {
      break;
    }
    current_max_allowed *= 2;
    if (current_max_allowed <= max_allowed) {
      SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 100, exit); 
    }
  }
  *credit_balance_resolution = 1 << resolution_exp;
  if (resolution_exp > ARAD_ITM_MAX_CRBL_RESOLUTION_EXPONENT) {
      SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 220, exit); 
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_cr_request_call_needed_credit_balance_resolution()", value, max_allowed);
}


uint32
  arad_itm_cr_request_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_QT_NDX          qt_ndx,
    SOC_SAND_IN  ARAD_ITM_CR_REQUEST_INFO *info,
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_INFO *exact_info
  )
{
  uint32
    mul_pckt_deq_fld_val,
    cycle_time = 0,
    tmp;
  uint64 
      tmp64;
  uint8
    hw_q_type = ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE, cr_bal_resolution, tmp_resolution;
  int32
    int32_min, int32_max;
  uint32 
      field_value,
      credit_selection_field_value[1] = {0};
  uint32
    res, uint32_max;
  uint32
      reg32 = 0;
  ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA
    ips_queue_size_based_thresholds_table_tbl_data;
  ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA
    ips_credit_balance_based_thresholds_table_tbl_data;
  ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA
    ips_empty_queue_credit_balance_table_tbl_data;
  ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA
    ips_credit_watchdog_thresholds_table_tbl_data;
  soc_reg_t
      resolution_reg, credit_selection_reg;
  soc_field_t 
      credit_selection_field;
  uint32 
      exact_credit_watchdog_scan_time_nano;
  int8
      credit_watchdog_mode;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_REQUEST_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  
  SOC_SAND_CHECK_FUNC_RESULT(arad_sw_db_queue_type_map_get_alloc(unit, qt_ndx, &hw_q_type), 10, exit);

  
  if (hw_q_type != SOC_TMC_ITM_QT_PUSH_Q_NDX) {
      
      int32_min = int32_max = info->hungry_th.off_to_slow_th;
      if (int32_max < info->hungry_th.off_to_normal_th) {
        int32_max = info->hungry_th.off_to_normal_th;
      } else if (int32_min > info->hungry_th.off_to_normal_th) {
        int32_min = info->hungry_th.off_to_normal_th;
      }

      if (int32_max < info->hungry_th.slow_to_normal_th) {
        int32_max = info->hungry_th.slow_to_normal_th;
      } else if (int32_min > info->hungry_th.slow_to_normal_th) {
        int32_min = info->hungry_th.slow_to_normal_th;
      }

      if (int32_max < info->hungry_th.normal_to_slow_th) {
        int32_max = info->hungry_th.normal_to_slow_th;
      } else if (int32_min > info->hungry_th.normal_to_slow_th) {
        int32_min = info->hungry_th.normal_to_slow_th;
      }

      if (int32_min < 0) {
        int32_min = -int32_min;
        if (int32_max < int32_min) {
          int32_max = int32_min;
        }
      } 

      SOC_SAND_CHECK_FUNC_RESULT(arad_itm_cr_request_call_needed_credit_balance_resolution(
        int32_max, 7 * (1 << 12), &cr_bal_resolution), 20, exit); 

      
      uint32_max = info->satisfied_th.backoff_th.backoff_enter_th;
      if (uint32_max < info->satisfied_th.backoff_th.backoff_exit_th) {
        uint32_max = info->satisfied_th.backoff_th.backoff_exit_th;
      }
      if (uint32_max < info->satisfied_th.backlog_th.backlog_enter_th) {
        uint32_max = info->satisfied_th.backlog_th.backlog_enter_th;
      }
      if (uint32_max < info->satisfied_th.backlog_th.backlog_exit_th) {
        uint32_max = info->satisfied_th.backlog_th.backlog_exit_th;
      } 
      if (uint32_max < info->satisfied_th.backslow_th.backslow_exit_th) {
        uint32_max = info->satisfied_th.backslow_th.backslow_exit_th;
      }
      if (uint32_max < info->satisfied_th.backslow_th.backslow_enter_th) {
        uint32_max = info->satisfied_th.backslow_th.backslow_enter_th;
      } 

      SOC_SAND_CHECK_FUNC_RESULT(arad_itm_cr_request_call_needed_credit_balance_resolution(
        uint32_max, 15 * (1 << 11), &tmp_resolution), 30, exit); 
      if (cr_bal_resolution < tmp_resolution) {
        cr_bal_resolution = tmp_resolution;
      }

      
      int32_min = int32_max = info->satisfied_th.empty_queues.satisfied_empty_q_th;
      if (int32_max < info->satisfied_th.empty_queues.max_credit_balance_empty_q) {
        int32_max = info->satisfied_th.empty_queues.max_credit_balance_empty_q;
      } else if (int32_min > info->satisfied_th.empty_queues.max_credit_balance_empty_q) {
        int32_min = info->satisfied_th.empty_queues.max_credit_balance_empty_q;
      }
      if (int32_min < 0) {
        SOC_SAND_CHECK_FUNC_RESULT(arad_itm_cr_request_call_needed_credit_balance_resolution(
          -int32_min, (1 << 15) - 1, &tmp_resolution), 40, exit); 
        if (cr_bal_resolution < tmp_resolution) {
          cr_bal_resolution = tmp_resolution;
        }
      }
      if (int32_max > 0) {
        SOC_SAND_CHECK_FUNC_RESULT(arad_itm_cr_request_call_needed_credit_balance_resolution(
          int32_max, (1 << 15) - 1, &tmp_resolution), 50, exit);
        if (cr_bal_resolution < tmp_resolution) {
          cr_bal_resolution = tmp_resolution;
        }
      } 

      
      res = arad_itm_cr_request_info_hungry_table_field_set(
              unit,
              info->hungry_th.off_to_normal_th,
              &(ips_queue_size_based_thresholds_table_tbl_data.off_to_norm_msg_th),
              &(exact_info->hungry_th.off_to_normal_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

      res = arad_itm_cr_request_info_hungry_table_field_set(
              unit,
              info->hungry_th.off_to_slow_th,
              &(ips_queue_size_based_thresholds_table_tbl_data.off_to_slow_msg_th),
              &(exact_info->hungry_th.off_to_slow_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

      res = arad_itm_cr_request_info_hungry_table_field_set(
              unit,
              info->hungry_th.slow_to_normal_th,
              &(ips_queue_size_based_thresholds_table_tbl_data.slow_to_norm_msg_th),
              &(exact_info->hungry_th.slow_to_normal_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

      res = arad_itm_cr_request_info_hungry_table_field_set(
              unit,
              info->hungry_th.normal_to_slow_th,
              &(ips_queue_size_based_thresholds_table_tbl_data.norm_to_slow_msg_th),
              &(exact_info->hungry_th.normal_to_slow_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

      ips_queue_size_based_thresholds_table_tbl_data.fsm_th_mul =
        ARAD_ITM_HUNGRY_TH_MULTIPLIER_VAL_TO_FIELD(info->hungry_th.multiplier);

      
      res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_set(
              info->satisfied_th.backlog_th.backlog_enter_th,
              &(ips_credit_balance_based_thresholds_table_tbl_data.backlog_enter_qcr_bal_th),
              &(exact_info->satisfied_th.backlog_th.backlog_enter_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

      res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_set(
              info->satisfied_th.backlog_th.backlog_exit_th,
              &(ips_credit_balance_based_thresholds_table_tbl_data.backlog_exit_qcr_bal_th),
              &(exact_info->satisfied_th.backlog_th.backlog_exit_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);

      res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_set(
              info->satisfied_th.backoff_th.backoff_enter_th,
              &(ips_credit_balance_based_thresholds_table_tbl_data.backoff_enter_qcr_bal_th),
              &(exact_info->satisfied_th.backoff_th.backoff_enter_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);

      res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_set(
              info->satisfied_th.backoff_th.backoff_exit_th,
              &(ips_credit_balance_based_thresholds_table_tbl_data.backoff_exit_qcr_bal_th),
              &(exact_info->satisfied_th.backoff_th.backoff_exit_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

      res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_set(
              info->satisfied_th.backslow_th.backslow_enter_th,
              &(ips_credit_balance_based_thresholds_table_tbl_data.backslow_enter_qcr_bal_th),
              &(exact_info->satisfied_th.backslow_th.backslow_enter_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 232, exit);

      res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_set(
              info->satisfied_th.backslow_th.backslow_exit_th,
              &(ips_credit_balance_based_thresholds_table_tbl_data.backslow_exit_qcr_bal_th),
              &(exact_info->satisfied_th.backslow_th.backslow_exit_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 234, exit);

      
      if (info->satisfied_th.empty_queues.satisfied_empty_q_th < 0)
      {
        ips_empty_queue_credit_balance_table_tbl_data.empty_qsatisfied_cr_bal =
          (SOC_SAND_U16_MAX + 1) + info->satisfied_th.empty_queues.satisfied_empty_q_th / cr_bal_resolution;
      }
      else
      {
        ips_empty_queue_credit_balance_table_tbl_data.empty_qsatisfied_cr_bal =
        info->satisfied_th.empty_queues.satisfied_empty_q_th / cr_bal_resolution;
      }

      exact_info->satisfied_th.empty_queues.satisfied_empty_q_th =
        (info->satisfied_th.empty_queues.satisfied_empty_q_th / cr_bal_resolution) * cr_bal_resolution;

      if (info->satisfied_th.empty_queues.max_credit_balance_empty_q < 0)
      {
        ips_empty_queue_credit_balance_table_tbl_data.max_empty_qcr_bal =
          (SOC_SAND_U16_MAX + 1) + info->satisfied_th.empty_queues.max_credit_balance_empty_q / cr_bal_resolution;
      }
      else
      {
        ips_empty_queue_credit_balance_table_tbl_data.max_empty_qcr_bal =
        info->satisfied_th.empty_queues.max_credit_balance_empty_q / cr_bal_resolution;
      }
      exact_info->satisfied_th.empty_queues.max_credit_balance_empty_q =
        (info->satisfied_th.empty_queues.max_credit_balance_empty_q / cr_bal_resolution) * cr_bal_resolution;

      ips_empty_queue_credit_balance_table_tbl_data.exceed_max_empty_qcr_bal =
        exact_info->satisfied_th.empty_queues.exceed_max_empty_q =
        info->satisfied_th.empty_queues.exceed_max_empty_q;

      res = GET_EXACT_CREDIT_WATCHDOG_SCAN_TIME_NANO(unit, exact_credit_watchdog_scan_time_nano);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 240, exit);

      
      res = GET_CREDIT_WATCHDOG_MODE(unit, credit_watchdog_mode);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 245, exit);
      if (IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit, credit_watchdog_mode)) { 
        if (info->wd_th.cr_wd_dlt_q_th) {
          uint32 mnt, exp;
          tmp = (info->wd_th.cr_wd_dlt_q_th + (ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_MS - 1)) /
            ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_MS; 
          SOC_SAND_CHECK_FUNC_RESULT(soc_sand_break_to_mnt_exp_round_up(tmp, 4, 4, 0, &mnt, &exp), 330, exit);
          if (exp > ARAD_CREDIT_WATCHDOG_COMMON_MAX_DELETE_EXP - ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES) { 
            SOC_SAND_SET_ERROR_CODE(ARAD_CR_WD_DEL_TH_OUT_OF_RANGE, 250, exit);
          }
          exact_info->wd_th.cr_wd_dlt_q_th = ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_MS * (mnt << exp);
          exact_info->wd_th.cr_wd_stts_msg_gen = 0;
          
          switch(exact_credit_watchdog_scan_time_nano) {
            case ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS:     
              tmp = ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES;           
              break;
            case 2 * ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS: 
              tmp = ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES - 1;       
              break;
            case 4 * ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS: 
              tmp = ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES - 2;       
              break;
            case 8 * ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS: 
              tmp = ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES - 3;       
              break;
            case ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_NS:     
              tmp = 0;
              break;
            default:
              SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 260, exit); 
          }
          ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth = mnt;
          ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period = exp + tmp;
        } else {
          ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth = 
          ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period = 0;
        }

      } else { 

        res = arad_itm_get_set_cr_watch_dog_scan_calc_unsafe(unit, 0, &cycle_time, 0);
        SOC_SAND_CHECK_FUNC_RESULT(res, 300, exit);

        res = GET_CREDIT_WATCHDOG_MODE(unit, credit_watchdog_mode);
        SOC_SAND_CHECK_FUNC_RESULT(res, 310, exit);
        if ((cycle_time != 0) && (info->wd_th.cr_wd_dlt_q_th != 0) && !IS_CREDIT_WATCHDOG_FAST_STATUS_MESSAGE_MODE(unit, credit_watchdog_mode)) {
          tmp = info->wd_th.cr_wd_dlt_q_th * 1000000;
          ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth = SOC_SAND_DIV_ROUND_UP(tmp, cycle_time);
          SOC_SAND_LIMIT_FROM_BELOW(ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth, 1);
          SOC_SAND_LIMIT_FROM_ABOVE(ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth, ARAD_ITM_CREDIT_WATCHDOG_MAX_THRESHOLD_VALUE);
          exact_info->wd_th.cr_wd_dlt_q_th = NS_TO_MS_ROUND(cycle_time * ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth);
        }
        else
        {
          ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth = 0;
          exact_info->wd_th.cr_wd_dlt_q_th = 0;
        }

        if ((cycle_time != 0) && (info->wd_th.cr_wd_stts_msg_gen != 0))
        {
          tmp = info->wd_th.cr_wd_stts_msg_gen * 1000000;
          ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period = SOC_SAND_DIV_ROUND_UP(tmp, cycle_time);
          SOC_SAND_LIMIT_FROM_ABOVE(ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period, ARAD_ITM_CREDIT_WATCHDOG_MAX_THRESHOLD_VALUE);
          if (ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth > 0)
          {
            if (ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth == ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period &&
                ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth > 1)
            {
              
              ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period--;
            }
            else if (ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth <= ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period)
            {
              SOC_SAND_SET_ERROR_CODE(ARAD_CR_WD_DELETE_BEFORE_STATUS_MSG_ERR, 400, exit);
            }
          }

          exact_info->wd_th.cr_wd_stts_msg_gen = NS_TO_MS_ROUND(cycle_time * ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period);
        }
        else
        {
          ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period = 0;
          exact_info->wd_th.cr_wd_stts_msg_gen = 0;
        }

      }
  }
  if (hw_q_type == ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE) {
      exact_info->wd_th.cr_wd_dlt_q_th = SOC_TMC_ITM_CR_WD_Q_TH_OPERATION_FAILED;
  } else {
      if (hw_q_type >= SOC_DPP_DEFS_GET(unit ,nof_credit_request_profiles)) {
          SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 405, exit); 
      }
      if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {

          uint32 queue_ptiority_data, is_ocb_only = 0, ref_count = 0;

          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 410, exit, ARAD_REG_ACCESS_ERR,
                                         READ_IPS_QPRIm(unit, MEM_BLOCK_ANY, hw_q_type, &queue_ptiority_data));
          
          if (!SOC_IS_QAX(unit)) {

              res = sw_state_access[unit].dpp.soc.arad.tm.q_type_ref_count.get(unit, hw_q_type, &ref_count);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res ,415, exit);

              
              if (ref_count) {
                  soc_mem_field_get(unit, IPS_QPRIm, &queue_ptiority_data, QUEUE_IS_OCB_ONLYf, &is_ocb_only);
                  if (is_ocb_only != info->is_ocb_only) {
                      SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_OCB_ONLY_ERR, 420, exit);
                  }
              }

              field_value = info->is_ocb_only;
              soc_mem_field_set(unit, IPS_QPRIm, &queue_ptiority_data, QUEUE_IS_OCB_ONLYf, &field_value);
              exact_info->is_ocb_only = info->is_ocb_only;
          }

          field_value = info->is_high_priority;
          soc_mem_field_set(unit, IPS_QPRIm, &queue_ptiority_data, QUEUE_IS_HIGH_PRIORITYf, &field_value);
          exact_info->is_high_priority  = info->is_high_priority;
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 430, exit, ARAD_REG_ACCESS_ERR,
                                           WRITE_IPS_QPRIm(unit, MEM_BLOCK_ANY, hw_q_type, &queue_ptiority_data));

          
          credit_selection_reg = (SOC_IS_QAX(unit)) ? (IPS_MAX_S_DEQ_COMMAND_CREDIT_SELECTIONr) : (IPS_MAX_DEQ_COMMAND_CREDIT_SELECTIONr);
          credit_selection_field = (SOC_IS_QAX(unit)) ? (MAX_SDQ_CMD_SELf) : (MAX_DQ_CMD_SELf);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 440, exit, ARAD_REG_ACCESS_ERR, 
                  soc_reg32_get(unit, credit_selection_reg, REG_PORT_ANY, 0, &reg32));
          credit_selection_field_value[0] = soc_reg_field_get(unit, credit_selection_reg, reg32, credit_selection_field);
          SHR_BITWRITE(credit_selection_field_value, hw_q_type, info->is_high_bandwidth_profile);
          soc_reg_field_set(unit, credit_selection_reg, &reg32, credit_selection_field, credit_selection_field_value[0]);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 450, exit, ARAD_REG_ACCESS_ERR, 
                  soc_reg32_set(unit, credit_selection_reg, SOC_CORE_ALL, 0, reg32));
      }

      if (hw_q_type != SOC_TMC_ITM_QT_PUSH_Q_NDX) {
          if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
              uint32
                  slow_thresholds_data[ARAD_IPS_SLOW_FACTOR_THRESHOLDS_TABLE_TBL_ENTRY_SIZE];
              int slow_level;
              const int slow_level_thresh_up_fields[SOC_TMC_ITM_CR_SLOW_LEVELS] = {
                  SF_TH_01f,
                  SF_TH_12f,
                  SF_TH_23f,
                  SF_TH_34f,
                  SF_TH_45f,
                  SF_TH_56f,
                  SF_TH_67f};
              const int slow_level_thresh_down_fields[SOC_TMC_ITM_CR_SLOW_LEVELS] = {
                  SF_TH_10f,
                  SF_TH_21f,
                  SF_TH_32f,
                  SF_TH_43f,
                  SF_TH_54f,
                  SF_TH_65f,
                  SF_TH_76f};

              sal_memset(slow_thresholds_data, 0x0, sizeof(slow_thresholds_data));
              for (slow_level = 0; slow_level < SOC_TMC_ITM_CR_SLOW_LEVELS; slow_level++) {
                  soc_mem_field_set(unit,
                                    IPS_SFTHm,
                                    slow_thresholds_data,
                                    slow_level_thresh_up_fields[slow_level],
                                    (uint32*)&info->slow_level_thresholds.slow_level_thresh_up[slow_level]);
                  soc_mem_field_set(unit,
                                    IPS_SFTHm,
                                    slow_thresholds_data,
                                    slow_level_thresh_down_fields[slow_level],
                                    (uint32*)&info->slow_level_thresholds.slow_level_thresh_down[slow_level]);
              }
              SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 530, exit, ARAD_REG_ACCESS_ERR,
                                                   WRITE_IPS_SFTHm(unit, MEM_BLOCK_ANY, hw_q_type, slow_thresholds_data));
  
          }
          
          tmp_resolution = _credit_balance_resolution_to_exp[cr_bal_resolution]; 
          if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
              resolution_reg = IPS_CREDIT_BALANCE_RESOLUTION_REGISTERr;
          } else if (SOC_IS_ARADPLUS(unit)) {
              resolution_reg = IPS_REG_014Er;
          } else {
              resolution_reg = IPS_REG_014Br;
          }
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 540,  exit, ARAD_REG_ACCESS_ERR, soc_reg_get(unit, resolution_reg, REG_PORT_ANY, 0, &tmp64));
          tmp = (hw_q_type >= 16) ? COMPILER_64_HI(tmp64) : COMPILER_64_LO(tmp64);

          if ((3 & (tmp >> (2 * (hw_q_type & 0xf)))) != tmp_resolution) { 
              tmp &= ~(3 << (2 * (hw_q_type & 0xf)));
              tmp |= tmp_resolution << (2 * (hw_q_type & 0xf));
            
              if (hw_q_type >= 16) {
                  COMPILER_64_SET(tmp64, tmp, COMPILER_64_LO(tmp64));
              } else {
                  COMPILER_64_SET(tmp64, COMPILER_64_HI(tmp64), tmp);
              }
              SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  550,  exit, ARAD_REG_ACCESS_ERR, soc_reg_set(unit, resolution_reg, REG_PORT_ANY, 0, tmp64));
          }

          SOC_SAND_CHECK_FUNC_RESULT(arad_ips_queue_size_based_thresholds_table_tbl_set_unsafe( 
            unit, hw_q_type, &ips_queue_size_based_thresholds_table_tbl_data), 580, exit);
          exact_info->hungry_th.multiplier = ARAD_ITM_HUNGRY_TH_MULTIPLIER_FIELD_TO_VAL(
            ips_queue_size_based_thresholds_table_tbl_data.fsm_th_mul);

          SOC_SAND_CHECK_FUNC_RESULT( arad_ips_credit_balance_based_thresholds_table_tbl_set_unsafe( 
            unit, hw_q_type, &ips_credit_balance_based_thresholds_table_tbl_data), 600, exit);

          SOC_SAND_CHECK_FUNC_RESULT(arad_ips_empty_queue_credit_balance_table_tbl_set_unsafe( 
            unit, hw_q_type, &ips_empty_queue_credit_balance_table_tbl_data), 620, exit);

          SOC_SAND_CHECK_FUNC_RESULT(arad_ips_credit_watchdog_thresholds_table_tbl_set_unsafe( 
            unit, hw_q_type, &ips_credit_watchdog_thresholds_table_tbl_data), 640, exit);

          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  660,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_STORED_CREDITS_USAGE_CONFIGURATIONr, SOC_CORE_ALL, 0, MUL_PKT_DEQf, &mul_pckt_deq_fld_val));
          tmp = SOC_SAND_BOOL2NUM(info->is_low_latency);
          SOC_SAND_SET_BIT(mul_pckt_deq_fld_val, tmp, hw_q_type);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  670,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_STORED_CREDITS_USAGE_CONFIGURATIONr, SOC_CORE_ALL, 0, MUL_PKT_DEQf,  mul_pckt_deq_fld_val));
          exact_info->is_low_latency = info->is_low_latency;     

          
          if (SOC_IS_ARADPLUS(unit) && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            
            
            uint32 reg, field, mask = 1 << hw_q_type;
            exact_info->is_remote_credit_value = info->is_remote_credit_value ? TRUE : FALSE;
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 700, exit, ARAD_REG_ACCESS_ERR, READ_IPS_IPS_CREDIT_CONFIG_4r(unit, &reg));
            field = soc_reg_field_get(unit, IPS_IPS_CREDIT_CONFIG_4r, reg, CREDIT_VALUE_Q_TYPE_BITMAPf);
            if (info->is_remote_credit_value) {
              field |= mask;
            } else {
              field &= ~mask;
            }
            soc_reg_field_set(unit, IPS_IPS_CREDIT_CONFIG_4r, &reg, CREDIT_VALUE_Q_TYPE_BITMAPf, field);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 710, exit, ARAD_REG_ACCESS_ERR, WRITE_IPS_IPS_CREDIT_CONFIG_4r(unit, reg));
          } else {
            exact_info->is_remote_credit_value = FALSE;
          }
        }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_request_set_unsafe()", qt_ndx, hw_q_type);
}


uint32
  arad_itm_cr_request_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_QT_NDX          qt_ndx,
    SOC_SAND_IN  ARAD_ITM_CR_REQUEST_INFO *info
  )
{
  int slow_level;
  int8 credit_watchdog_mode;
  uint32
      res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_REQUEST_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if ((qt_ndx < SOC_TMC_ITM_QT_NDX_02 || qt_ndx >= ARAD_ITM_IPS_QT_MAX(unit)) &&
      ((qt_ndx < SOC_TMC_ITM_PREDEFIEND_OFFSET) || (qt_ndx >= SOC_TMC_ITM_PREDEFIEND_OFFSET + SOC_TMC_ITM_NOF_QT_STATIC)) && 
       (qt_ndx != SOC_TMC_ITM_QT_PUSH_Q_NDX + SOC_TMC_ITM_PREDEFIEND_OFFSET)) {
        SOC_SAND_SET_ERROR_CODE(ARAD_ITM_IPS_QT_RNG_OUT_OF_RANGE_ERR, 5, exit);
  }
  if (qt_ndx != SOC_TMC_ITM_QT_PUSH_Q_NDX + SOC_TMC_ITM_PREDEFIEND_OFFSET) {
      if (soc_sand_abs(info->hungry_th.normal_to_slow_th) > \
        ARAD_ITM_HUNGRY_TH_MAX(unit))
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_HUNGRY_TH_OUT_OF_RANGE_ERR, 10, exit);
      }
      if (soc_sand_abs(info->hungry_th.off_to_normal_th) > \
        ARAD_ITM_HUNGRY_TH_MAX(unit))
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_HUNGRY_TH_OUT_OF_RANGE_ERR, 20, exit);
      }
      if (soc_sand_abs(info->hungry_th.off_to_slow_th) > \
        ARAD_ITM_HUNGRY_TH_MAX(unit))
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_HUNGRY_TH_OUT_OF_RANGE_ERR, 30, exit);
      }
      if (soc_sand_abs(info->hungry_th.slow_to_normal_th) > \
        ARAD_ITM_HUNGRY_TH_MAX(unit))
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_HUNGRY_TH_OUT_OF_RANGE_ERR, 40, exit);
      }

      if (info->hungry_th.multiplier && soc_sand_log2_round_down(info->hungry_th.multiplier) <= ARAD_ITM_HUNGRY_TH_MULTIPLIER_OFFSET)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_HUNGRY_TH_MULTIPLIER_OUT_OF_RANGE_ERR, 45, exit);
      }

      if (info->satisfied_th.backlog_th.backlog_exit_th > ARAD_ITM_SATISFIED_TH_MAX)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 50, exit);
      }

      if (info->satisfied_th.backlog_th.backlog_enter_th > ARAD_ITM_SATISFIED_TH_MAX)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 60, exit);
      }
      if (info->satisfied_th.backoff_th.backoff_exit_th > ARAD_ITM_SATISFIED_TH_MAX)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 70, exit);
      }

      if (info->satisfied_th.backoff_th.backoff_enter_th > ARAD_ITM_SATISFIED_TH_MAX)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 80, exit);
      }

      if (info->satisfied_th.backslow_th.backslow_exit_th > ARAD_ITM_SATISFIED_TH_MAX)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 82, exit);
      }
      if (info->satisfied_th.backslow_th.backslow_enter_th > ARAD_ITM_SATISFIED_TH_MAX)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 84, exit);
      }

      res = GET_CREDIT_WATCHDOG_MODE(unit, credit_watchdog_mode);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 85, exit);
      if (info->wd_th.cr_wd_dlt_q_th != 0 && !IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit, credit_watchdog_mode))
      {
          SOC_SAND_ERR_IF_OUT_OF_RANGE(
            info->wd_th.cr_wd_dlt_q_th, ARAD_ITM_CR_WD_Q_TH_MIN_MSEC, ARAD_ITM_CR_WD_Q_TH_MAX_MSEC,
            ARAD_CR_WD_DEL_TH_OUT_OF_RANGE, 86, exit
          );
      }
      
      if (SOC_IS_JERICHO(unit) && info->is_remote_credit_value) {
              SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 90, exit);
      }

      if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          for (slow_level = 0; slow_level < SOC_TMC_ITM_CR_SLOW_LEVELS; slow_level++) {
              SOC_SAND_ERR_IF_OUT_OF_RANGE(info->slow_level_thresholds.slow_level_thresh_up[slow_level], 0, SOC_TMC_ITM_CR_SLOW_LEVEL_VAL_MAX, ARAD_CR_WD_DEL_TH_OUT_OF_RANGE, 110, exit);
              SOC_SAND_ERR_IF_OUT_OF_RANGE(info->slow_level_thresholds.slow_level_thresh_down[slow_level], 0, SOC_TMC_ITM_CR_SLOW_LEVEL_VAL_MAX, ARAD_CR_WD_DEL_TH_OUT_OF_RANGE, 120, exit);
          }
          for (slow_level = 0; slow_level < SOC_TMC_ITM_CR_SLOW_LEVELS - 1; slow_level++) {
              
              if (info->slow_level_thresholds.slow_level_thresh_up[slow_level + 1] < info->slow_level_thresholds.slow_level_thresh_up[slow_level]) {
                  SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 130, exit);
              }
          }
          for (slow_level = 0; slow_level < SOC_TMC_ITM_CR_SLOW_LEVELS - 1; slow_level++) {
              
              if (info->slow_level_thresholds.slow_level_thresh_down[slow_level + 1] < info->slow_level_thresholds.slow_level_thresh_down[slow_level]) {
                  SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 140, exit);
              }
          }
          for (slow_level = 0; slow_level < SOC_TMC_ITM_CR_SLOW_LEVELS; slow_level++) {
              
              if (info->slow_level_thresholds.slow_level_thresh_up[slow_level] < info->slow_level_thresholds.slow_level_thresh_down[slow_level]) {
                  SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 150, exit);
              }
          }
      }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_request_verify()", qt_ndx, 0);
}


uint32
  arad_itm_cr_request_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_QT_NDX          qt_ndx,
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_INFO *info
  )
{
  uint32
    mul_pckt_deq_fld_val,
    tmp;
  uint32 
      field_value = 0,
      credit_selection_field_value[1] = {0};
  uint64
      tmp64;
  uint8
    hw_q_type = ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE;
  int32
    cr_bal_resolution = 0; 
  uint32
    res;
  uint32
    reg32 = 0;
  ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA
    ips_queue_size_based_thresholds_table_tbl_data;
  ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA
    ips_credit_balance_based_thresholds_table_tbl_data;
  ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA
    ips_empty_queue_credit_balance_table_tbl_data;
  ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA
    ips_credit_watchdog_thresholds_table_tbl_data;
  soc_reg_t
      resolution_reg, credit_selection_reg;
  soc_field_t 
      credit_selection_field;
  uint32 
      exact_credit_watchdog_scan_time_nano;
  int8 
      credit_watchdog_mode;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_REQUEST_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  
  
  SOC_SAND_CHECK_FUNC_RESULT(arad_sw_db_queue_type_map_get(unit, qt_ndx, &hw_q_type), 10, exit);
  if (hw_q_type == ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE) { 
    info->wd_th.cr_wd_dlt_q_th = SOC_TMC_ITM_CR_WD_Q_TH_OPERATION_FAILED;
    goto exit;
  }
  if (qt_ndx == SOC_TMC_ITM_QT_NDX_INVALID || hw_q_type >= SOC_DPP_DEFS_GET(unit ,nof_credit_request_profiles)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 400, exit); 
  }

  if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      uint32
          queue_ptiority_data;

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 420, exit, ARAD_REG_ACCESS_ERR,
                                             READ_IPS_QPRIm(unit, MEM_BLOCK_ANY, hw_q_type, &queue_ptiority_data));
      if (!SOC_IS_QAX(unit)) {
          field_value = 0;
          soc_mem_field_get(unit, IPS_QPRIm, &queue_ptiority_data, QUEUE_IS_OCB_ONLYf, &field_value);
          info->is_ocb_only = field_value;
      }
      field_value = 0;
      soc_mem_field_get(unit, IPS_QPRIm, &queue_ptiority_data, QUEUE_IS_HIGH_PRIORITYf, &field_value);
      info->is_high_priority = field_value;

      
      credit_selection_reg = (SOC_IS_QAX(unit)) ? (IPS_MAX_S_DEQ_COMMAND_CREDIT_SELECTIONr) : (IPS_MAX_DEQ_COMMAND_CREDIT_SELECTIONr);
      credit_selection_field = (SOC_IS_QAX(unit)) ? (MAX_SDQ_CMD_SELf) : (MAX_DQ_CMD_SELf);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 440, exit, ARAD_REG_ACCESS_ERR, 
              soc_reg32_get(unit, credit_selection_reg, REG_PORT_ANY, 0, &reg32));
      credit_selection_field_value[0] = soc_reg_field_get(unit, credit_selection_reg, reg32, credit_selection_field);
      field_value = SHR_BITGET(credit_selection_field_value, hw_q_type);
      info->is_high_bandwidth_profile = (field_value) ? TRUE : FALSE;
  }
  if (hw_q_type != SOC_TMC_ITM_QT_PUSH_Q_NDX) {
      if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          uint32
              slow_thresholds_data[ARAD_IPS_SLOW_FACTOR_THRESHOLDS_TABLE_TBL_ENTRY_SIZE];
          int slow_level;
          const int slow_level_thresh_up_fields[SOC_TMC_ITM_CR_SLOW_LEVELS] = {
              SF_TH_01f,
              SF_TH_12f,
              SF_TH_23f,
              SF_TH_34f,
              SF_TH_45f,
              SF_TH_56f,
              SF_TH_67f};
          const int slow_level_thresh_down_fields[SOC_TMC_ITM_CR_SLOW_LEVELS] = {
              SF_TH_10f,
              SF_TH_21f,
              SF_TH_32f,
              SF_TH_43f,
              SF_TH_54f,
              SF_TH_65f,
              SF_TH_76f};
          sal_memset(slow_thresholds_data, 0x0, sizeof(slow_thresholds_data));

          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 430, exit, ARAD_REG_ACCESS_ERR,
                                               READ_IPS_SFTHm(unit, MEM_BLOCK_ANY, hw_q_type, slow_thresholds_data));
          for (slow_level = 0; slow_level < SOC_TMC_ITM_CR_SLOW_LEVELS; slow_level++) {
              soc_mem_field_get(unit,
                                IPS_SFTHm,
                                slow_thresholds_data,
                                slow_level_thresh_up_fields[slow_level],
                                (uint32*)&info->slow_level_thresholds.slow_level_thresh_up[slow_level]);
              soc_mem_field_get(unit,
                                IPS_SFTHm,
                                slow_thresholds_data,
                                slow_level_thresh_down_fields[slow_level],
                                (uint32*)&info->slow_level_thresholds.slow_level_thresh_down[slow_level]);
          }
      }

      
      if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          resolution_reg = IPS_CREDIT_BALANCE_RESOLUTION_REGISTERr;
      } else if (SOC_IS_ARADPLUS(unit) ) {
          resolution_reg = IPS_REG_014Er;
      } else {
          resolution_reg = IPS_REG_014Br;
      }
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR, soc_reg_get(unit, resolution_reg, REG_PORT_ANY,  0, &tmp64));
      tmp = (hw_q_type >= 16) ? COMPILER_64_HI(tmp64) : COMPILER_64_LO(tmp64);
      tmp = 3 & (tmp >> (2 * (hw_q_type & 0xf)));

      cr_bal_resolution = 1 << (3 & (tmp >> (2 * (hw_q_type & 0xf))));
      if (tmp > ARAD_ITM_MAX_CRBL_RESOLUTION_EXPONENT) {
        SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 14, exit); 
      }
      cr_bal_resolution = 1 << tmp;

      
      res = arad_ips_queue_size_based_thresholds_table_tbl_get_unsafe(
                unit,
                hw_q_type,
                &ips_queue_size_based_thresholds_table_tbl_data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = arad_itm_cr_request_info_hungry_table_field_get(
              unit,
              ips_queue_size_based_thresholds_table_tbl_data.off_to_norm_msg_th,
              &(info->hungry_th.off_to_normal_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      res = arad_itm_cr_request_info_hungry_table_field_get(
              unit,
              ips_queue_size_based_thresholds_table_tbl_data.off_to_slow_msg_th,
              &(info->hungry_th.off_to_slow_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      res = arad_itm_cr_request_info_hungry_table_field_get(
              unit,
              ips_queue_size_based_thresholds_table_tbl_data.slow_to_norm_msg_th,
              &(info->hungry_th.slow_to_normal_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      res = arad_itm_cr_request_info_hungry_table_field_get(
              unit,
              ips_queue_size_based_thresholds_table_tbl_data.norm_to_slow_msg_th,
              &(info->hungry_th.normal_to_slow_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

      info->hungry_th.multiplier =
        ARAD_ITM_HUNGRY_TH_MULTIPLIER_FIELD_TO_VAL(
          ips_queue_size_based_thresholds_table_tbl_data.fsm_th_mul);
      

      

      res = arad_ips_credit_balance_based_thresholds_table_tbl_get_unsafe(
              unit,
              hw_q_type,
              &ips_credit_balance_based_thresholds_table_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

      res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_get(
              ips_credit_balance_based_thresholds_table_tbl_data.backlog_enter_qcr_bal_th,
              &(info->satisfied_th.backlog_th.backlog_enter_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

      res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_get(
              ips_credit_balance_based_thresholds_table_tbl_data.backlog_exit_qcr_bal_th,
              &(info->satisfied_th.backlog_th.backlog_exit_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

      res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_get(
              ips_credit_balance_based_thresholds_table_tbl_data.backoff_enter_qcr_bal_th,
              &(info->satisfied_th.backoff_th.backoff_enter_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

      res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_get(
              ips_credit_balance_based_thresholds_table_tbl_data.backoff_exit_qcr_bal_th,
              &(info->satisfied_th.backoff_th.backoff_exit_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

      res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_get(
              ips_credit_balance_based_thresholds_table_tbl_data.backslow_enter_qcr_bal_th,
              &(info->satisfied_th.backslow_th.backslow_enter_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 112, exit);

      res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_get(
              ips_credit_balance_based_thresholds_table_tbl_data.backslow_exit_qcr_bal_th,
              &(info->satisfied_th.backslow_th.backslow_exit_th), cr_bal_resolution
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 114, exit);

      

      

      res = arad_ips_empty_queue_credit_balance_table_tbl_get_unsafe(
              unit,
              hw_q_type,
              &ips_empty_queue_credit_balance_table_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);


      if (ips_empty_queue_credit_balance_table_tbl_data.empty_qsatisfied_cr_bal > SOC_SAND_I16_MAX )
      {
        info->satisfied_th.empty_queues.satisfied_empty_q_th = cr_bal_resolution *
          (int32)(ips_empty_queue_credit_balance_table_tbl_data.empty_qsatisfied_cr_bal -
          (SOC_SAND_U16_MAX + 1));
      }
      else
      {
        info->satisfied_th.empty_queues.satisfied_empty_q_th = cr_bal_resolution *
          (int32)(ips_empty_queue_credit_balance_table_tbl_data.empty_qsatisfied_cr_bal);
      }

      if(ips_empty_queue_credit_balance_table_tbl_data.max_empty_qcr_bal > SOC_SAND_I16_MAX )
      {
        info->satisfied_th.empty_queues.max_credit_balance_empty_q = cr_bal_resolution *
          (int32)(ips_empty_queue_credit_balance_table_tbl_data.max_empty_qcr_bal -
          (SOC_SAND_U16_MAX + 1));
      }
      else
      {
        info->satisfied_th.empty_queues.max_credit_balance_empty_q = cr_bal_resolution *
        (int32)(ips_empty_queue_credit_balance_table_tbl_data.max_empty_qcr_bal);
      }

      info->satisfied_th.empty_queues.exceed_max_empty_q =
        (uint8)ips_empty_queue_credit_balance_table_tbl_data.exceed_max_empty_qcr_bal;

      

      
      res = arad_ips_credit_watchdog_thresholds_table_tbl_get_unsafe(
              unit,
              hw_q_type,
              &ips_credit_watchdog_thresholds_table_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

      res = GET_EXACT_CREDIT_WATCHDOG_SCAN_TIME_NANO(unit, exact_credit_watchdog_scan_time_nano);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 152, exit);

      res = GET_CREDIT_WATCHDOG_MODE(unit, credit_watchdog_mode);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 154, exit);
      if (IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit, credit_watchdog_mode)) {
        
        
        info->wd_th.cr_wd_dlt_q_th = ((ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth <<
          ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period) *
          (exact_credit_watchdog_scan_time_nano / ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS)) /
          (1000000 / ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS);
        info->wd_th.cr_wd_stts_msg_gen = 0;
      } else {
        uint32 cycle_time = 0;
        SOC_SAND_CHECK_FUNC_RESULT(arad_itm_get_set_cr_watch_dog_scan_calc_unsafe(
          unit, 0, &cycle_time, 0), 160, exit);
        info->wd_th.cr_wd_dlt_q_th = NS_TO_MS_ROUND(cycle_time * ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth);
        info->wd_th.cr_wd_stts_msg_gen = NS_TO_MS_ROUND(cycle_time * ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period);
      }

      

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_STORED_CREDITS_USAGE_CONFIGURATIONr, SOC_CORE_ALL, 0, MUL_PKT_DEQf, &mul_pckt_deq_fld_val));
      tmp = SOC_SAND_GET_BIT(mul_pckt_deq_fld_val, hw_q_type);
      info->is_low_latency = SOC_SAND_NUM2BOOL(tmp);
      
      
      if (SOC_IS_ARADPLUS(unit) && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 180, exit, ARAD_REG_ACCESS_ERR, READ_IPS_IPS_CREDIT_CONFIG_4r(unit, &tmp));
        info->is_remote_credit_value = ((soc_reg_field_get(unit, IPS_IPS_CREDIT_CONFIG_4r, tmp, CREDIT_VALUE_Q_TYPE_BITMAPf)
          >> hw_q_type) & 1) ? TRUE : FALSE;
      } else {
        info->is_remote_credit_value = FALSE;
      }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_request_get_unsafe()", unit, hw_q_type);
}


uint32
  arad_itm_cr_discount_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_CR_DISCOUNT_INFO *info
  )
{
  uint32
    res;
  ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA
    iqm_credit_discount_table_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_DISCOUNT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  iqm_credit_discount_table_tbl_data.crdt_disc_val = (uint32)soc_sand_abs(info->discount);
  iqm_credit_discount_table_tbl_data.discnt_sign = (info->discount < 0);

  res = arad_iqm_credit_discount_table_tbl_set_unsafe(
          unit,
          cr_cls_ndx,
          &iqm_credit_discount_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_discount_set_unsafe()",0,0);
}


uint32
  arad_itm_cr_discount_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_CR_DISCOUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_DISCOUNT_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    cr_cls_ndx, ARAD_ITM_QT_CR_CLS_MAX,
    ARAD_ITM_CR_CLS_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    info->discount, ARAD_ITM_CR_DISCOUNT_MIN_VAL, ARAD_ITM_CR_DISCOUNT_MAX_VAL,
    ARAD_ITM_CR_DISCOUNT_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_discount_verify()",cr_cls_ndx,info->discount);
}


uint32
  arad_itm_cr_discount_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_CR_DISCOUNT_INFO *info
  )
{
  uint32
    res;
  ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA
    iqm_credit_discount_table_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_DISCOUNT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_iqm_credit_discount_table_tbl_get_unsafe(
          unit,
          cr_cls_ndx,
          &iqm_credit_discount_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->discount = iqm_credit_discount_table_tbl_data.crdt_disc_val;
  if (iqm_credit_discount_table_tbl_data.discnt_sign)
  {
    info->discount *= -1;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_discount_get_unsafe()",0,0);
}


uint32
  arad_itm_queue_test_tmplt_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_ADMIT_TSTS      test_tmplt
  )
{
  uint32
    res;
  ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_QUEUE_TEST_TMPLT_SET_UNSAFE);

  res = arad_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  iqm_packet_queue_red_parameters_table_tbl_data.addmit_logic =
    (uint32)test_tmplt;

  res = arad_iqm_packet_queue_red_parameters_table_tbl_set_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_test_tmplt_set_unsafe()",0,0);
}


uint32
  arad_itm_queue_test_tmplt_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_ADMIT_TSTS      test_tmplt
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_QUEUE_TEST_TMPLT_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, ARAD_MAX_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  if (test_tmplt > (ARAD_ITM_ADMIT_TSTS_LAST-1))
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_ADMT_TEST_ID_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_test_tmplt_verify()",0,0);
}


uint32
  arad_itm_queue_test_tmplt_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_ADMIT_TSTS      *test_tmplt
  )
{
  uint32
    res;
  ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_QUEUE_TEST_TMPLT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(test_tmplt);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, ARAD_MAX_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  res = arad_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *test_tmplt =
    iqm_packet_queue_red_parameters_table_tbl_data.addmit_logic;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_test_tmplt_get_unsafe()",0,0);
}


uint32
  arad_itm_wred_exp_wq_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                  exp_wq,
    SOC_SAND_IN  uint8                   enable
  )
{
  uint32
    res;
  ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA
    iqm_packet_queue_red_weight_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_WRED_EXP_WQ_SET_UNSAFE);

  res = arad_iqm_packet_queue_red_weight_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          &iqm_packet_queue_red_weight_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  iqm_packet_queue_red_weight_table_tbl_data.pq_weight =
      exp_wq;
  iqm_packet_queue_red_weight_table_tbl_data.avrg_en = 
      enable ? TRUE : FALSE;
  res = arad_iqm_packet_queue_red_weight_table_tbl_set_unsafe(
          unit,
          rt_cls_ndx,
          &iqm_packet_queue_red_weight_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_wred_exp_wq_set_unsafe()",0,0);
}


uint32
  arad_itm_wred_exp_wq_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                  exp_wq
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_WRED_EXP_WQ_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    exp_wq, ARAD_ITM_WQ_MAX,
    ARAD_ITM_WRED_EXP_WT_PARAMETER_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_wred_exp_wq_verify()",0,0);
}


uint32
  arad_itm_wred_exp_wq_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT  uint32                  *exp_wq
  )
{
  uint32
    res;
  ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA
    iqm_packet_queue_red_weight_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_WRED_EXP_WQ_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  res = arad_iqm_packet_queue_red_weight_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          &iqm_packet_queue_red_weight_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *exp_wq =
    (uint8)iqm_packet_queue_red_weight_table_tbl_data.pq_weight;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_wred_exp_wq_get_unsafe()",0,0);
}


uint32
  arad_itm_wred_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *exact_info
  )
{
  uint32
    res;
  ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_WRED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  res = arad_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  iqm_packet_queue_red_parameters_table_tbl_data.pq_wred_en =
    exact_info->wred_en = SOC_SAND_BOOL2NUM(info->wred_en);

  iqm_packet_queue_red_parameters_table_tbl_data.pq_wred_pckt_sz_ignr =
    exact_info->ignore_packet_size = SOC_SAND_BOOL2NUM(info->ignore_packet_size);

  res = arad_itm_WRED_QT_DP_INFO_to_WRED_TBL_DATA(
          unit,
          info,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_itm_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
          unit,
          &iqm_packet_queue_red_parameters_table_tbl_data,
          exact_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = arad_iqm_packet_queue_red_parameters_table_tbl_set_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_wred_set_unsafe()",0,0);
}


uint32
  arad_itm_wred_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_WRED_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, SOC_TMC_NOF_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  if (info->min_avrg_th > info->max_avrg_th)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_WRED_MIN_TH_HIGHER_THEN_MAX_TH_ERR, 50, exit);
  }

  if (info->min_avrg_th > ARAD_Q_WRED_INFO_MIN_AVRG_TH_MAX(unit))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_WRED_MIN_AVRG_TH_OUT_OF_RANGE_ERR, 20, exit);
  }

  if (info->max_avrg_th > ARAD_Q_WRED_INFO_MAX_AVRG_TH_MAX(unit))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_WRED_MAX_AVRG_TH_OUT_OF_RANGE_ERR, 30, exit);
  }

  if (info->max_probability > ARAD_ITM_WRED_QT_DP_INFO_MAX_PROBABILITY_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_WRED_PROB_OUT_OF_RANGE_ERR, 40, exit);
  }

  if (info->max_packet_size > ARAD_ITM_WRED_MAX_PACKET_SIZE)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_WRED_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_wred_verify()",0,0);
}


uint32
  arad_itm_wred_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *info
  )
{
  uint32
    res;
  ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_WRED_GET_UNSAFE);

  sal_memset(&iqm_packet_queue_red_parameters_table_tbl_data,0x0,sizeof(ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA));

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, SOC_TMC_NOF_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  res = arad_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_itm_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
          unit,
          &iqm_packet_queue_red_parameters_table_tbl_data,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  info->wred_en = SOC_SAND_NUM2BOOL(iqm_packet_queue_red_parameters_table_tbl_data.pq_wred_en);

  info->ignore_packet_size =
    SOC_SAND_NUM2BOOL(iqm_packet_queue_red_parameters_table_tbl_data.pq_wred_pckt_sz_ignr);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_wred_get_unsafe()",0,0);
}


uint32
  arad_itm_tail_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT ARAD_ITM_TAIL_DROP_INFO  *exact_info
  )
{
  uint32
    res,
    max_inst_q_siz_mnt_nof_bits,
    max_inst_q_siz_bds_mnt_nof_bits,
    max_inst_q_siz_exp_nof_bits;
  ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;
  uint32
    pq_max_que_size_mnt,
    pq_max_que_size_exp,
    max_q_size = info->max_inst_q_size,
    resolution = ARAD_ITM_DROP_TAIL_SIZE_RESOLUTION;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_TAIL_DROP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  
 
  res = arad_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  res = arad_iqm_packet_queue_red_parameters_tail_drop_mantissa_nof_bits(
             unit,
             drop_precedence_ndx,
             &max_inst_q_siz_mnt_nof_bits,
             &max_inst_q_siz_bds_mnt_nof_bits
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

  max_inst_q_siz_exp_nof_bits = soc_mem_field_length(unit, IQM_PQREDm, PQ_MAX_QUE_SIZEf) - max_inst_q_siz_mnt_nof_bits;

  if ((drop_precedence_ndx == SOC_TMC_NOF_DROP_PRECEDENCE) && (!SOC_IS_ARADPLUS(unit))) {
    resolution = 1;
    if (max_q_size > ARAD_B0_MAX_ECN_QUEUE_BYTES) {
      if (max_q_size < ARAD_MAX_QUEUE_SIZE_BYTES) { 
        SOC_SAND_SET_ERROR_CODE(ARAD_QUEUE_SIZE_OUT_OF_RANGE_ERR, 24, exit);
      }
      max_q_size = ARAD_MAX_QUEUE_SIZE_BYTES; 
    }
  }
  res = soc_sand_break_to_mnt_exp_round_up(
        SOC_SAND_DIV_ROUND_UP(info->max_inst_q_size, resolution),
        max_inst_q_siz_mnt_nof_bits,
        max_inst_q_siz_exp_nof_bits,
        0,
        &(pq_max_que_size_mnt),
        &(pq_max_que_size_exp)
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_mnt = pq_max_que_size_mnt;
  iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_exp = pq_max_que_size_exp;

  exact_info->max_inst_q_size = iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_mnt << iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_exp;

  exact_info->max_inst_q_size *= resolution;

  
  max_inst_q_siz_exp_nof_bits = soc_mem_field_length(unit, IQM_PQREDm, PQ_MAX_QUE_BUFF_SIZEf) - max_inst_q_siz_bds_mnt_nof_bits;
  res = soc_sand_break_to_mnt_exp_round_up(
          info->max_inst_q_size_bds,
          max_inst_q_siz_bds_mnt_nof_bits,
          max_inst_q_siz_exp_nof_bits,
          0,
          &(pq_max_que_size_mnt),
          &(pq_max_que_size_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_bds_mnt = pq_max_que_size_mnt;
  iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_bds_exp = pq_max_que_size_exp;

  exact_info->max_inst_q_size_bds = iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_bds_mnt << iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_bds_exp;

  res = arad_iqm_packet_queue_red_parameters_table_tbl_set_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_tail_drop_set_unsafe()",rt_cls_ndx,drop_precedence_ndx);
}


uint32
  arad_itm_tail_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_TAIL_DROP_INFO  *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_TAIL_DROP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, SOC_TMC_NOF_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->max_inst_q_size, ARAD_MAX_QUEUE_SIZE_BYTES,
    ARAD_QUEUE_SIZE_OUT_OF_RANGE_ERR, 9, exit
  )

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->max_inst_q_size_bds, ARAD_MAX_QUEUE_SIZE_BDS,
    ARAD_QUEUE_SIZE_OUT_OF_RANGE_ERR, 10, exit
  )

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_tail_drop_verify()", rt_cls_ndx, drop_precedence_ndx);
}


uint32
  arad_itm_tail_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_TAIL_DROP_INFO  *info
  )
{
  uint32
    res;
  ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_TAIL_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(drop_precedence_ndx, SOC_TMC_NOF_DROP_PRECEDENCE, ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit);

  res = arad_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->max_inst_q_size =
    ((iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_mnt) *
    (1<<(iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_exp)));

  if ((drop_precedence_ndx != SOC_TMC_NOF_DROP_PRECEDENCE) || SOC_IS_ARADPLUS(unit)) {
    info->max_inst_q_size *= ARAD_ITM_DROP_TAIL_SIZE_RESOLUTION;
  }

  info->max_inst_q_size_bds =
    ((iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_bds_mnt) *
    (1<<(iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_bds_exp)));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_tail_drop_get_unsafe()",rt_cls_ndx,drop_precedence_ndx);
}


uint32
  arad_itm_cr_wd_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  ARAD_ITM_CR_WD_INFO      *info,
    SOC_SAND_OUT ARAD_ITM_CR_WD_INFO      *exact_info
  )
{
  uint32 res, reg_val, conf_reg = 0;
  uint64 conf_reg64;
  uint32 scan_time_us = info->min_scan_cycle_period_micro;
  uint32 exact_credit_watchdog_scan_time_nano;
  int8 credit_watchdog_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_WD_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  if (info->bottom_queue == ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP && 
      info->top_queue == ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP &&
      info == exact_info) {

      res = GET_EXACT_CREDIT_WATCHDOG_SCAN_TIME_NANO(unit, exact_credit_watchdog_scan_time_nano);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 240, exit);

      res = GET_CREDIT_WATCHDOG_MODE(unit, credit_watchdog_mode);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 250, exit);

      
      if (info->max_flow_msg_gen_rate_nano == ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP) {
          if (info->min_scan_cycle_period_micro == ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP) {

              
              if (IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit, credit_watchdog_mode)) {
                  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 3100, exit, ARAD_REG_ACCESS_ERR,
                    READ_IPS_CREDIT_WATCHDOG_CONFIGURATIONr(unit, core_id, &conf_reg64)); 
                  conf_reg = soc_reg64_field32_get(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, conf_reg64, CR_WD_FSM_MODEf);
              }
              exact_info->min_scan_cycle_period_micro = conf_reg == 3 ? 1 : (conf_reg != 2 ? 0 : 2); 
          } else {

              
              res = SET_EXACT_CREDIT_WATCHDOG_SCAN_TIME_NANO(unit, 0);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3272, exit);
              SOC_SAND_CHECK_FUNC_RESULT(arad_itm_get_set_cr_watch_dog_scan_calc_unsafe(unit,
                info->min_scan_cycle_period_micro, &exact_info->min_scan_cycle_period_micro, 0), 3150, exit);
          }
      } else if (IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit, credit_watchdog_mode)) {

          
          int i;
          if (info->min_scan_cycle_period_micro > ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES ||
              info->max_flow_msg_gen_rate_nano > ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES ||
              (info->min_scan_cycle_period_micro && info->max_flow_msg_gen_rate_nano)) {
              SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3200, exit); 
          }
          

          for (i = 0; i < SOC_DPP_DEFS_GET(unit ,nof_credit_request_profiles); ++i) {
              SOC_SAND_SOC_IF_ERROR_RETURN(res, 3210, exit, READ_IPS_CRWDTHm(unit, MEM_BLOCK_ANY, i, &reg_val));
              if (soc_IPS_CRWDTHm_field32_get(unit, &reg_val, WD_DELETE_Q_THf)) {
                  conf_reg = soc_IPS_CRWDTHm_field32_get(unit, &reg_val, WD_STATUS_MSG_GEN_PERIODf);
                  if (info->min_scan_cycle_period_micro) {
                      conf_reg -= info->min_scan_cycle_period_micro;
                  } else {
                      conf_reg += info->max_flow_msg_gen_rate_nano;
                  }
                  if (conf_reg > ARAD_CREDIT_WATCHDOG_COMMON_MAX_DELETE_EXP) {
                      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3220, exit); 
                  }
                  soc_IPS_CRWDTHm_field32_set(unit, &reg_val, WD_STATUS_MSG_GEN_PERIODf, conf_reg);
                  SOC_SAND_SOC_IF_ERROR_RETURN(res, 3230, exit, WRITE_IPS_CRWDTHm(unit, MEM_BLOCK_ANY, i, &reg_val));
              }
          }
      } else if (IS_CREDIT_WATCHDOG_UNINITIALIZED(unit, credit_watchdog_mode)) { 
          
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 3250, exit, ARAD_REG_ACCESS_ERR,
            READ_IPS_CREDIT_WATCHDOG_CONFIGURATIONr(unit, core_id, &conf_reg64)); 
          reg_val = soc_reg64_field32_get(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, conf_reg64, CR_WD_MIN_SCAN_CYCLE_PERIODf);
          if (reg_val) { 
              uint32 scan_ns, i;
              SOC_SAND_CHECK_FUNC_RESULT(arad_ticks_to_time(unit, reg_val, TRUE,
                ARAD_ITM_WD_INFO_MIN_SCAN_CYCLE_PERIOD_MICRO_GRANULARITY(unit), &scan_ns) , 3260, exit);
              if (SOC_IS_ARADPLUS(unit) &&
                  soc_reg64_field32_get(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, conf_reg64, CR_WD_DELETE_Q_EXP_MODEf)) {
                  
                  for (i = ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_NS; ;)  {
                      if (scan_ns >= i) {
                          if ((scan_ns / 100) * 99 >= i) {
                              SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3270, exit); 
                          }
                          res = SET_EXACT_CREDIT_WATCHDOG_SCAN_TIME_NANO(unit, i);
                          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3272, exit);
                          break;
                      }
                      i /= 2;
                      if (i < ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS) {
                          SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3275, exit); 
                      }
                  }
                  res = SET_CREDIT_WATCHDOG_MODE(unit, soc_reg64_field32_get(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, conf_reg64, CR_WD_FSM_MODEf) != 2 ? 
                                                        CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE : CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE + 1);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3280, exit);
              } else {
                  if (scan_ns >= ARAD_MIN_SCAN_CYCLE_PERIOD_MICRO * 1000 &&
                      scan_ns <= ARAD_MIN_SCAN_CYCLE_PERIOD_MICRO * 1010) {
                      res = SET_CREDIT_WATCHDOG_MODE(unit, CREDIT_WATCHDOG_NORMAL_MODE);
                      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3282, exit);
                  } else if (scan_ns >= ARAD_MIN_SCAN_CYCLE_PERIOD_MICRO_AGGRESSIVE_WD_STATUS_MSG * 1000 &&
                             scan_ns <= ARAD_MIN_SCAN_CYCLE_PERIOD_MICRO_AGGRESSIVE_WD_STATUS_MSG * 1010) {
                      res = SET_CREDIT_WATCHDOG_MODE(unit, CREDIT_WATCHDOG_FAST_STATUS_MESSAGE_MODE);
                      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3285, exit);
                  } else {
                      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3290, exit); 
                  }
                  res = SET_EXACT_CREDIT_WATCHDOG_SCAN_TIME_NANO(unit, scan_ns);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3125, exit);
              }
          } else { 
              reg_val = soc_reg_field_get(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, conf_reg, CR_WD_MAX_FLOW_MSG_GEN_RATEf);
              if (reg_val <= CREDIT_WATCHDOG_FAST_STATUS_MESSAGE_MODE) {
                  res =  SET_CREDIT_WATCHDOG_MODE(unit, reg_val == CREDIT_WATCHDOG_UNINITIALIZED ? 
                                                  (SOC_IS_ARADPLUS(unit) ?  CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE : CREDIT_WATCHDOG_NORMAL_MODE) : 
                                                   reg_val); 
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3300, exit);
                  
                  SOC_SAND_CHECK_FUNC_RESULT(arad_itm_get_set_cr_watch_dog_scan_calc_unsafe(unit, 0, 0, 0), 3300, exit);
                  if (!SOC_WARM_BOOT(unit)) {  
                      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  3310,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_CR_WD_BOTTOM_Qr, core_id, 0, CR_WD_BOTTOM_Qf,  0));
                      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  3320,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_CR_WD_TOP_Qr, core_id, 0, CR_WD_TOP_Qf,  SOC_DPP_DEFS_GET(unit, max_queues)-1));
                  }
              } else if (reg_val >= CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE && SOC_IS_ARADPLUS(unit) &&
                         (reg_val -= CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE) <= (ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES + 1)) {
                  if (reg_val > ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES) {
                      res = SET_CREDIT_WATCHDOG_MODE(unit, CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE + 1);
                      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3330, exit);
                      res = SET_EXACT_CREDIT_WATCHDOG_SCAN_TIME_NANO(unit, ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_NS);
                      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3332, exit);
                  } else {
                      res = SET_CREDIT_WATCHDOG_MODE(unit, CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE);
                      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3335, exit);
                      res = SET_EXACT_CREDIT_WATCHDOG_SCAN_TIME_NANO(unit, ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS << reg_val);
                      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3337, exit);
                  }
              } else {
                  SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3340, exit); 
              }
          }
      } else {
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3990, exit);
      }
      SOC_EXIT;
  }

  SOC_SAND_CHECK_NULL_INPUT(info);

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_CR_WD_BOTTOM_Qr, core_id, 0, CR_WD_BOTTOM_Qf, info->bottom_queue));
  exact_info->bottom_queue = info->bottom_queue;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_CR_WD_TOP_Qr, core_id, 0, CR_WD_TOP_Qf, info->top_queue));
  exact_info->top_queue = info->top_queue;

  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 30, exit, ARAD_REG_ACCESS_ERR,
    READ_IPS_CREDIT_WATCHDOG_CONFIGURATIONr(unit, core_id, &conf_reg64)); 
  conf_reg = COMPILER_64_LO(conf_reg64);
  

  if (SOC_IS_ARADPLUS(unit)) {
      res = GET_CREDIT_WATCHDOG_MODE(unit, credit_watchdog_mode);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 35, exit);

      if (IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit, credit_watchdog_mode)) {
          
          uint32 hw_mode;
          switch (scan_time_us) {
            case 0:
              hw_mode = 0;
              break;
            case ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS     / 1000: 
            case 2 * ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS / 1000: 
            case 4 * ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS / 1000: 
            case 8 * ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS / 1000: 
            case ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_NS     / 1000: 
                hw_mode = credit_watchdog_mode == CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE + 1 ? 2 : 3;
                break;
            default:
              SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 32, exit); 
          }
          exact_info->min_scan_cycle_period_micro = scan_time_us;
          soc_reg64_field32_set(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, &conf_reg64, CR_WD_FSM_MODEf, hw_mode);
          soc_reg64_field32_set(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, &conf_reg64, CR_WD_DELETE_Q_EXP_MODEf, 1);
      } else {
          soc_reg64_field32_set(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, &conf_reg64, CR_WD_FSM_MODEf, 1);
          soc_reg64_field32_set(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, &conf_reg64, CR_WD_DELETE_Q_EXP_MODEf, 0);
      }
  }

  
  if (!scan_time_us) { 
    exact_info->max_flow_msg_gen_rate_nano = reg_val = info->max_flow_msg_gen_rate_nano;
  } else if (info->max_flow_msg_gen_rate_nano) { 
    res = arad_chip_time_to_ticks( unit, info->max_flow_msg_gen_rate_nano, TRUE, 1, TRUE, &reg_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit); 

    res = arad_ticks_to_time( unit, reg_val, TRUE, 1, &(exact_info->max_flow_msg_gen_rate_nano));
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit); 

    SOC_SAND_LIMIT_FROM_BELOW(reg_val, ARAD_CREDIT_WATCHDOG_MIN_Q_SCAN_CYCLES);
    SOC_SAND_LIMIT_FROM_ABOVE(reg_val, ARAD_CREDIT_WATCHDOG_MAX_Q_SCAN_CYCLES);
  } else {
    exact_info->max_flow_msg_gen_rate_nano = reg_val = 0;
  }

  soc_reg_field_set(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, &conf_reg, CR_WD_MAX_FLOW_MSG_GEN_RATEf, reg_val);

  
  if (scan_time_us) { 
    res = arad_itm_get_set_cr_watch_dog_scan_calc_unsafe(unit, scan_time_us, 0, &reg_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    res = GET_EXACT_CREDIT_WATCHDOG_SCAN_TIME_NANO(unit, exact_credit_watchdog_scan_time_nano);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);

    exact_info->min_scan_cycle_period_micro = exact_credit_watchdog_scan_time_nano / 1000;
  } else {
    exact_info->min_scan_cycle_period_micro = reg_val = 0;
  }

  soc_reg_field_set(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, &conf_reg, CR_WD_MIN_SCAN_CYCLE_PERIODf, reg_val);
  COMPILER_64_SET(conf_reg64, COMPILER_64_HI(conf_reg64), conf_reg);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 80, exit, ARAD_REG_ACCESS_ERR, WRITE_IPS_CREDIT_WATCHDOG_CONFIGURATIONr(unit, core_id, conf_reg64));
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_wd_set_unsafe()",0,0);
}


uint32
  arad_itm_cr_wd_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  ARAD_ITM_CR_WD_INFO      *info
  )
{
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_WD_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if (info->bottom_queue != ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP || 
      info->top_queue != ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP) {
      if (info->bottom_queue > SOC_DPP_DEFS_GET(unit, nof_queues))
      {
          SOC_SAND_SET_ERROR_CODE(ARAD_QUEUE_ID_OUT_OF_RANGE_ERR, 10, exit);
      }

      if (info->top_queue > SOC_DPP_DEFS_GET(unit, nof_queues))
      {
          SOC_SAND_SET_ERROR_CODE(ARAD_QUEUE_ID_OUT_OF_RANGE_ERR, 15, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_wd_verify()",0,0);
}


uint32
  arad_itm_cr_wd_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_OUT ARAD_ITM_CR_WD_INFO      *info
  )
{
  uint32
    res,
    reg_val = 0;
  uint64
    reg_val64;
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_WD_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_SET;

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_CR_WD_BOTTOM_Qr, core_id, 0, CR_WD_BOTTOM_Qf, &info->bottom_queue));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_CR_WD_TOP_Qr, core_id, 0, CR_WD_TOP_Qf, &info->top_queue));
 

  

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 20, exit, ARAD_REG_ACCESS_ERR,
    READ_IPS_CREDIT_WATCHDOG_CONFIGURATIONr(unit, REG_PORT_ANY, &reg_val64));
  reg_val = COMPILER_64_LO(reg_val64);
  res =
    arad_ticks_to_time(
      unit,
      soc_reg_field_get(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, reg_val, CR_WD_MAX_FLOW_MSG_GEN_RATEf),
      TRUE,
      1,
      &(info->max_flow_msg_gen_rate_nano)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  res =
    arad_ticks_to_time(
      unit,
      soc_reg_field_get(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, reg_val, CR_WD_MIN_SCAN_CYCLE_PERIODf),
      FALSE,
      ARAD_ITM_WD_INFO_MIN_SCAN_CYCLE_PERIOD_MICRO_GRANULARITY(unit),
      &(info->min_scan_cycle_period_micro)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_wd_get_unsafe()", unit, 0);
}


uint32
  arad_itm_vsq_qt_rt_cls_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls
  )
{
  uint32
    res;
  uint32
    data[ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ENTRY_SIZE];
  soc_mem_t 
      mem = 0;
  soc_field_t 
      field = 0; 
  uint32 entry_offset;
  uint32 vsq_in_group_size;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_QT_RT_CLS_SET_UNSAFE);

  res = soc_sand_os_memset(&(data[0]), 0x0, sizeof(data));
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) { 
      field = VSQ_RATE_CLASSf;
      switch(vsq_group_ndx) {
      case ARAD_ITM_VSQ_GROUP_CTGRY:
          mem = IQM_VSQA_RCm;
          break;
      case ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
          mem = IQM_VSQB_RCm;
          break;
      case ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
          mem = IQM_VSQC_RCm;
          break;
      case ARAD_ITM_VSQ_GROUP_STTSTCS_TAG:
          mem = IQM_VSQD_RCm;
          break;
      case ARAD_ITM_VSQ_GROUP_SRC_PORT:
          mem = IQM_VSQE_RCm;
          break;
      case ARAD_ITM_VSQ_GROUP_PG:
          mem = IQM_VSQF_RCm;
          break;
      default:
        break;
      }
  } else {
      switch(vsq_group_ndx) { 
      case ARAD_ITM_VSQ_GROUP_CTGRY:
          mem = IQM_VSQDRC_Am;
          field = VSQ_RC_Af;
          break;
      case ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
          mem = IQM_VSQDRC_Bm;
          field = VSQ_RC_Bf;
          break;
      case ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
          mem = IQM_VSQDRC_Cm;
          field = VSQ_RC_Cf;
          break;
      case ARAD_ITM_VSQ_GROUP_STTSTCS_TAG:
          mem = IQM_VSQDRC_Dm;
          field = VSQ_RC_Df;
          break;
      case ARAD_ITM_VSQ_GROUP_SRC_PORT:
          mem = IQM_VSQDRC_Em;
          field = VSQ_RC_Ef;
          break;
      case ARAD_ITM_VSQ_GROUP_PG:
          mem = IQM_VSQDRC_Fm;
          field = VSQ_RC_Ff;
          break;
      default:
        break;
      }
  }
  entry_offset = vsq_in_group_ndx;
  if (vsq_group_ndx != ARAD_ITM_VSQ_GROUP_PG) {
      res = arad_itm_vsq_in_group_size_get(unit, vsq_group_ndx, &vsq_in_group_size);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

      entry_offset += is_ocb_only * vsq_in_group_size;
  }
  res = soc_mem_read(unit, mem, IQM_BLOCK(unit, core_id), entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  soc_mem_field32_set(unit, mem, data, field, vsq_rt_cls);

  res = soc_mem_write(unit, mem, IQM_BLOCK(unit, core_id), entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_qt_rt_cls_set_unsafe()",0,0);
}


uint32
  arad_itm_vsq_qt_rt_cls_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_QT_RT_CLS_VERIFY);

  res = arad_itm_vsq_idx_verify(
          unit,
          is_ocb_only,
          vsq_group_ndx,
          vsq_in_group_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (vsq_rt_cls > (ARAD_ITM_VSQ_QT_RT_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 40, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_qt_rt_cls_verify()",0,0);
}


uint32
  arad_itm_vsq_qt_rt_cls_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx,
    SOC_SAND_OUT uint32                 *vsq_rt_cls
  )
{
  uint32
    res;
  uint32
    data[ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ENTRY_SIZE];
  soc_mem_t 
      mem = 0;
  soc_field_t 
      field = 0; 
  uint32 entry_offset;
  uint32 vsq_in_group_size;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_QT_RT_CLS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vsq_rt_cls);

  res = arad_itm_vsq_idx_verify(
          unit,
          is_ocb_only,
          vsq_group_ndx,
          vsq_in_group_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_os_memset(&(data[0]), 0x0, sizeof(data));
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) { 
      field = VSQ_RATE_CLASSf;
      switch(vsq_group_ndx) {
      case ARAD_ITM_VSQ_GROUP_CTGRY:
          mem = IQM_VSQA_RCm;
          break;
      case ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
          mem = IQM_VSQB_RCm;
          break;
      case ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
          mem = IQM_VSQC_RCm;
          break;
      case ARAD_ITM_VSQ_GROUP_STTSTCS_TAG:
          mem = IQM_VSQD_RCm;
          break;
      case ARAD_ITM_VSQ_GROUP_SRC_PORT:
          mem = IQM_VSQE_RCm;
          break;
      case ARAD_ITM_VSQ_GROUP_PG:
          mem = IQM_VSQF_RCm;
          break;
      default:
        break;
      }
  } else {
      switch(vsq_group_ndx) { 
      case ARAD_ITM_VSQ_GROUP_CTGRY:
          mem = IQM_VSQDRC_Am;
          field = VSQ_RC_Af;
          break;
      case ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
          mem = IQM_VSQDRC_Bm;
          field = VSQ_RC_Bf;
          break;
      case ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
          mem = IQM_VSQDRC_Cm;
          field = VSQ_RC_Cf;
          break;
      case ARAD_ITM_VSQ_GROUP_STTSTCS_TAG:
          mem = IQM_VSQDRC_Dm;
          field = VSQ_RC_Df;
          break;
      case ARAD_ITM_VSQ_GROUP_SRC_PORT:
          mem = IQM_VSQDRC_Em;
          field = VSQ_RC_Ef;
          break;
      case ARAD_ITM_VSQ_GROUP_PG:
          mem = IQM_VSQDRC_Fm;
          field = VSQ_RC_Ff;
          break;
      default:
        break;
      }
  }
  entry_offset = vsq_in_group_ndx;
  if (vsq_group_ndx != ARAD_ITM_VSQ_GROUP_PG) {
      res = arad_itm_vsq_in_group_size_get(unit, vsq_group_ndx, &vsq_in_group_size);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

      entry_offset += is_ocb_only * vsq_in_group_size;
  }
  res = soc_mem_read(unit, mem, IQM_BLOCK(unit, core_id), entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  *vsq_rt_cls = soc_mem_field32_get(unit, mem, data, field);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_qt_rt_cls_get_unsafe()",0,0);
}


uint32
  arad_itm_vsq_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_FC_INFO     *info,
    SOC_SAND_OUT ARAD_ITM_VSQ_FC_INFO     *exact_info
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_FC_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);


  res = arad_itm_vsq_group_set_fc_info(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          info,
          exact_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_fc_set_unsafe()",0,0);
}


uint32
  arad_itm_vsq_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_FC_INFO     *info
  )
{
  uint32
    max_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_FC_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, ARAD_ITM_VSQ_GROUP_LAST-1,
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, ARAD_ITM_VSQ_QT_RT_CLS_MAX,
    ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  max_val = ARAD_ITM_VSQ_FC_BD_SIZE_MAX;
  if (info->bd_size_fc.clear > max_val)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_VSQ_FC_PARAMETER_OUT_OF_RANGE_ERR, 50, exit);
  }

  if (info->bd_size_fc.set > max_val)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_VSQ_FC_PARAMETER_OUT_OF_RANGE_ERR, 60, exit);
  }

  max_val = ARAD_ITM_VSQ_FC_Q_SIZE_MAX;
  if (info->q_size_fc.clear > max_val)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_VSQ_FC_PARAMETER_OUT_OF_RANGE_ERR, 70, exit);
  }

  if (info->q_size_fc.set > max_val)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_VSQ_FC_PARAMETER_OUT_OF_RANGE_ERR, 80, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_fc_verify()",0,0);
}


uint32
  arad_itm_vsq_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_VSQ_FC_INFO     *info
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_FC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (ARAD_ITM_VSQ_GROUP_LAST-1),
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, ARAD_ITM_VSQ_QT_RT_CLS_MAX,
    ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  res = arad_itm_vsq_group_get_fc_info(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_fc_get_unsafe()",0,0);
}


uint32
  arad_itm_vsq_tail_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT  ARAD_ITM_VSQ_TAIL_DROP_INFO  *exact_info
  )
{
  uint32
    res,
    max_inst_q_siz_words_mnt_nof_bits,
    max_inst_q_siz_words_exp_nof_bits,
    max_inst_q_siz_bds_mnt_nof_bits,
    max_inst_q_siz_bds_exp_nof_bits;
  ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_queue_parameters_table_group_tbl_data;
  uint32
    vq_max_size_words_mnt,
    vq_max_size_words_exp,
    vq_max_szie_bds_mnt,
    vq_max_szie_bds_exp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_TAIL_DROP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

 
 

  res = arad_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_vsq_queue_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  max_inst_q_siz_words_mnt_nof_bits = 7;
  max_inst_q_siz_words_exp_nof_bits = soc_mem_field_length(unit, IQM_VQPR_MAm, VQ_MAX_SIZE_WORDSf) - max_inst_q_siz_words_mnt_nof_bits;

    res = soc_sand_break_to_mnt_exp_round_up(
            SOC_SAND_DIV_ROUND_UP(info->max_inst_q_size,ARAD_ITM_DROP_TAIL_SIZE_RESOLUTION),
            max_inst_q_siz_words_mnt_nof_bits,
            max_inst_q_siz_words_exp_nof_bits,
            0,
            &(vq_max_size_words_mnt),
            &(vq_max_size_words_exp)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_mnt = vq_max_size_words_mnt;
    iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_exp = vq_max_size_words_exp;

    max_inst_q_siz_bds_mnt_nof_bits = 7;
    max_inst_q_siz_bds_exp_nof_bits = soc_mem_field_length(unit, IQM_VQPR_MAm, VQ_MAX_SIZE_BDSf) - max_inst_q_siz_bds_mnt_nof_bits;

    res = soc_sand_break_to_mnt_exp_round_up(
            info->max_inst_q_size_bds,
            max_inst_q_siz_bds_mnt_nof_bits,
            max_inst_q_siz_bds_exp_nof_bits,
            0,
            &(vq_max_szie_bds_mnt),
            &(vq_max_szie_bds_exp)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_szie_bds_mnt = vq_max_szie_bds_mnt;
    iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_szie_bds_exp = vq_max_szie_bds_exp;

    if (iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_mnt == 0)
    {
      exact_info->max_inst_q_size = 0;
    }
    else
    {
      exact_info->max_inst_q_size =
        ((iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_mnt) *
        (1<<(iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_exp)));
      exact_info->max_inst_q_size *= ARAD_ITM_DROP_TAIL_SIZE_RESOLUTION;
    }

    if(iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_szie_bds_mnt == 0)
    {
      exact_info->max_inst_q_size_bds = 0;
    }
    else
    {
      exact_info->max_inst_q_size_bds =
        ((iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_szie_bds_mnt) *
        (1<<(iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_szie_bds_exp)));
    }

    res = arad_iqm_vsq_queue_parameters_table_group_tbl_set_unsafe(
            unit,
            vsq_group_ndx,
            vsq_rt_cls_ndx,
            drop_precedence_ndx,
            &iqm_vsq_queue_parameters_table_group_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_tail_drop_set_unsafe()",0,0);
}


uint32
  arad_itm_vsq_tail_drop_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP           vsq_group_ndx,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                     drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_TAIL_DROP_INFO  *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_TAIL_DROP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (ARAD_ITM_VSQ_GROUP_LAST-1),
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, ARAD_ITM_VSQ_QT_RT_CLS_MAX,
    ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, ARAD_MAX_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 9, exit
  );

  

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->max_inst_q_size_bds,ARAD_ITM_MAX_INST_Q_BDS_SIZE,
    ARAD_ITM_VSQ_MAX_INST_Q_SIZ_PARAMETER_OUT_OF_RANGE_ERR, 50, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_tail_drop_verify()",0,0);
}


uint32
  arad_itm_vsq_tail_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_VSQ_TAIL_DROP_INFO  *info
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_queue_parameters_table_group_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_TAIL_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (ARAD_ITM_VSQ_GROUP_LAST-1),
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, ARAD_ITM_VSQ_QT_RT_CLS_MAX,
    ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, ARAD_MAX_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 9, exit
  );

 
 

  res = arad_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_vsq_queue_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


    if (iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_mnt == 0)
    {
      info->max_inst_q_size = 0;
    }
    else
    {
      info->max_inst_q_size =
        ((iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_mnt) *
        (1<<(iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_exp)));
      info->max_inst_q_size *= ARAD_ITM_DROP_TAIL_SIZE_RESOLUTION;
    }

    if(iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_szie_bds_mnt == 0)
    {
      info->max_inst_q_size_bds = 0;
    }
    else
    {
      info->max_inst_q_size_bds =
        ((iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_szie_bds_mnt) *
        (1<<(iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_szie_bds_exp)));
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_tail_drop_get_unsafe()",0,0);
}


uint32
  arad_itm_vsq_tail_drop_get_default_unsafe(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_OUT ARAD_ITM_VSQ_TAIL_DROP_INFO  *info
  )
{
    uint32 max_size_exp = 0, max_size_mnt = 0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_TAIL_DROP_DEFAULT_GET_UNSAFE);
    SOC_SAND_CHECK_NULL_INPUT(info);

    arad_iqm_mantissa_exponent_get(unit, (0x1 << soc_mem_field_length(unit, IQM_VQPR_MAm, VQ_MAX_SIZE_WORDSf)) - 1, 7, &max_size_mnt, &max_size_exp);
    info->max_inst_q_size = (max_size_mnt * (1 << max_size_exp));

    arad_iqm_mantissa_exponent_get(unit, (0x1 << soc_mem_field_length(unit, IQM_VQPR_MAm, VQ_MAX_SIZE_BDSf)) - 1, 7, &max_size_mnt, &max_size_exp);
    info->max_inst_q_size_bds = (max_size_mnt * (1 << max_size_exp));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_tail_drop_get_unsafe()",0,0);
}


uint32
  arad_itm_vsq_wred_gen_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_WRED_GEN_INFO *info
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_flow_control_parameters_table_group_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_WRED_GEN_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_iqm_vsq_flow_control_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          &iqm_vsq_flow_control_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  iqm_vsq_flow_control_parameters_table_group_tbl_data.red_weight_q =
    info->exp_wq;
  iqm_vsq_flow_control_parameters_table_group_tbl_data.wred_en =
    info->wred_en;

  res = arad_iqm_vsq_flow_control_parameters_table_group_tbl_set_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          &iqm_vsq_flow_control_parameters_table_group_tbl_data
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_wred_gen_set_unsafe()",0,0);
}


uint32
  arad_itm_vsq_wred_gen_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_WRED_GEN_INFO *info
  )
{
  uint32
    max_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_WRED_GEN_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);
 

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (ARAD_ITM_VSQ_GROUP_LAST-1),
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, ARAD_ITM_VSQ_QT_RT_CLS_MAX,
    ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  max_val = (1 << soc_mem_field_length(unit, IQM_VQFCPR_MAm, RED_WEIGHT_Qf)) - 1;
  if (info->exp_wq > max_val)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_WRED_EXP_WT_PARAMETER_OUT_OF_RANGE_ERR, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_wred_gen_verify()",0,0);
}


uint32
  arad_itm_vsq_wred_gen_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_VSQ_WRED_GEN_INFO *info
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_flow_control_parameters_table_group_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_WRED_GEN_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (ARAD_ITM_VSQ_GROUP_LAST-1),
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, ARAD_ITM_VSQ_QT_RT_CLS_MAX,
    ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  res = arad_iqm_vsq_flow_control_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          &iqm_vsq_flow_control_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->exp_wq =
    iqm_vsq_flow_control_parameters_table_group_tbl_data.red_weight_q;
  info->wred_en =
    (uint8)iqm_vsq_flow_control_parameters_table_group_tbl_data.wred_en;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_wred_gen_get_unsafe()",0,0);
}


uint32
  arad_itm_vsq_wred_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *exact_info
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_queue_parameters_table_group_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_WRED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = arad_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_vsq_queue_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_itm_vsq_WRED_QT_DP_INFO_to_WRED_TBL_DATA(
          unit,
          info,
          &iqm_vsq_queue_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
          unit,
          &iqm_vsq_queue_parameters_table_group_tbl_data,
          exact_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  exact_info->wred_en = info->wred_en;

  res = arad_iqm_vsq_queue_parameters_table_group_tbl_set_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_vsq_queue_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_wred_set_unsafe()",0,0);
}


uint32
  arad_itm_vsq_wred_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_WRED_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (ARAD_ITM_VSQ_GROUP_LAST-1),
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, ARAD_ITM_VSQ_QT_RT_CLS_MAX,
    ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, ARAD_MAX_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 9, exit
  );

  if (info->min_avrg_th > info->max_avrg_th)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_WRED_MIN_TH_HIGHER_THEN_MAX_TH_ERR, 60, exit);
  }

  

  if (info->max_probability > ARAD_ITM_WRED_MAX_PROB)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_WRED_PROB_OUT_OF_RANGE_ERR, 90, exit);
  }

  if (info->max_packet_size > ARAD_ITM_WRED_MAX_PACKET_SIZE)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_WRED_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR, 100, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_wred_verify()",0,0);
}


uint32
  arad_itm_vsq_wred_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *info
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_queue_parameters_table_group_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_WRED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (ARAD_ITM_VSQ_GROUP_LAST-1),
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, ARAD_ITM_VSQ_QT_RT_CLS_MAX,
    ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, ARAD_MAX_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 9, exit
  );

  res = arad_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_vsq_queue_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
          unit,
          &iqm_vsq_queue_parameters_table_group_tbl_data,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_wred_get_unsafe()",0,0);
}



uint32
  arad_itm_vsq_counter_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx
  )
{
  uint32
    res,
    fld_val,
    reg_val,
    vsq_in_group_size;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_COUNTER_SET_UNSAFE);

  reg_val = 0;
  fld_val = vsq_in_group_ndx;
  if (vsq_group_ndx != ARAD_ITM_VSQ_GROUP_PG) {
      res = arad_itm_vsq_in_group_size_get(unit, vsq_group_ndx, &vsq_in_group_size);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

      fld_val += is_ocb_only * vsq_in_group_size;
  }  
  ARAD_FLD_TO_REG(IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr, VSQ_PRG_CNT_Qf, fld_val, reg_val, 20, exit);
  fld_val = 0x0;
  ARAD_FLD_TO_REG(IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr, VSQ_PRG_CNT_MSKf, fld_val, reg_val, 22, exit);
  fld_val = vsq_group_ndx;
  ARAD_FLD_TO_REG(IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr, VSQ_PRG_GRP_SELf, fld_val, reg_val, 24, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,WRITE_IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr(unit, core_id,  reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_count_get_unsafe()",0,0);
}



uint32
  arad_itm_vsq_counter_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_cob_only,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP  vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX    vsq_in_group_ndx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_COUNTER_VERIFY);
  if (core_id != SOC_CORE_ALL && (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
  }
   res = arad_itm_vsq_idx_verify(
          unit,
          is_cob_only,
          vsq_group_ndx,
          vsq_in_group_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_counter_verify()",0,0);
}



uint32
  arad_itm_vsq_counter_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT  uint8                    *is_ocb_only,
    SOC_SAND_OUT ARAD_ITM_VSQ_GROUP       *vsq_group_ndx,
    SOC_SAND_OUT ARAD_ITM_VSQ_NDX         *vsq_in_group_ndx
  )
{
  uint32
    res,
    fld_val = 0,
    reg_val, 
    vsq_in_group_size;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_COUNTER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vsq_group_ndx);
  SOC_SAND_CHECK_NULL_INPUT(vsq_in_group_ndx);
  SOC_SAND_CHECK_NULL_INPUT(is_ocb_only);

  if (core_id != SOC_CORE_ALL && (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
  } 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,READ_IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr(unit, SOC_CORE_ALL, &reg_val));

  ARAD_FLD_FROM_REG(IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr, VSQ_PRG_GRP_SELf, fld_val, reg_val, 50, exit);
  *vsq_group_ndx = fld_val;

  res = arad_itm_vsq_in_group_size_get(unit, *vsq_group_ndx, &vsq_in_group_size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);


  ARAD_FLD_FROM_REG(IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr, VSQ_PRG_CNT_Qf, fld_val, reg_val, 30, exit);

  if (*vsq_group_ndx != ARAD_ITM_VSQ_GROUP_PG) {
      *is_ocb_only = fld_val / vsq_in_group_size;
      *vsq_in_group_ndx = fld_val % vsq_in_group_size;
  } else {
      if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          *is_ocb_only = 0;
      } else {
          *is_ocb_only = 0;
          
          
      }
      *vsq_in_group_ndx = fld_val;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_counter_get_unsafe()",0,0);
}



uint32
  arad_itm_vsq_counter_read_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT uint32                  *pckt_count
  )
{
  uint32
    res,
    fld_val = 0,
    reg_val = 0,
    cntr_val = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_COUNTER_READ_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pckt_count);

  if (core_id != SOC_CORE_ALL && (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 5, exit);
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,READ_IQM_VSQ_ENQUEUE_PACKET_COUNTERr(unit, core_id, &reg_val));
  ARAD_FLD_FROM_REG(IQM_VSQ_ENQUEUE_PACKET_COUNTERr, VSQ_ENQ_PKT_CNT_OVFf, fld_val, reg_val, 12, exit);

  if (fld_val != 0x0)
  {
    cntr_val = 0xFFFFFFFF;
  }
  else
  {
    ARAD_FLD_FROM_REG(IQM_VSQ_ENQUEUE_PACKET_COUNTERr, VSQ_ENQ_PKT_CNTf, cntr_val, reg_val, 14, exit);
  }

  *pckt_count = cntr_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_counter_read_unsafe()",0,0);
}


uint32
  arad_itm_queue_is_ocb_only_set(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  int                 core,
     SOC_SAND_IN  uint32              queue_ndx,
     SOC_SAND_IN  uint8               enable
     ) {
    uint32  res = SOC_E_NONE;
    uint32  is_ocb_only_line[2] = {0}, is_ocb_only_field = 0;
    uint32  line_index;
    uint32  line_shift;
    uint32  line_length;
    int     core_index;
    SOCDNX_INIT_FUNC_DEFS;
    if (SOC_IS_ARADPLUS_AND_BELOW(unit) || SOC_IS_QAX(unit)) {
        SOC_EXIT;
    }
    if ((core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) && core != SOC_CORE_ALL) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Core ID %d out of range\n"), core));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (queue_ndx >= SOC_DPP_DEFS_GET(unit, nof_queues)) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Base Queue ID %d out of range\n"), queue_ndx));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }


    line_length = soc_mem_field_length(unit, IRR_QUEUE_IS_OCB_COMMITTEDm, COMMITMENTf);
    if (line_length == 0) {
        SOCDNX_IF_ERR_EXIT(SOC_E_INTERNAL);
    }
    line_index = queue_ndx / line_length;
    line_shift = queue_ndx % line_length;
    SOC_DPP_CORES_ITER(core, core_index) {
        res = READ_IRR_QUEUE_IS_OCB_COMMITTEDm(unit, core_index, MEM_BLOCK_ANY, line_index, is_ocb_only_line);
        SOCDNX_IF_ERR_EXIT(res);
        soc_IRR_QUEUE_IS_OCB_COMMITTEDm_field_get(unit, is_ocb_only_line, COMMITMENTf, &is_ocb_only_field);

        if (enable) {
            SHR_BITSET(&is_ocb_only_field, line_shift);
        } else {
            SHR_BITCLR(&is_ocb_only_field, line_shift);
        }
        soc_IRR_QUEUE_IS_OCB_COMMITTEDm_field_set(unit, is_ocb_only_line, COMMITMENTf, &is_ocb_only_field);
        res = WRITE_IRR_QUEUE_IS_OCB_COMMITTEDm(unit, core_index, MEM_BLOCK_ANY, line_index, is_ocb_only_line);
        SOCDNX_IF_ERR_EXIT(res);
    }
exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_itm_queue_is_ocb_only_get(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  int                 core,
     SOC_SAND_IN  uint32              queue_ndx,
     SOC_SAND_OUT uint8               *enable
     ) {
    uint32  res = SOC_E_NONE;
    uint32  is_ocb_only_line[2] = {0}, is_ocb_only_field = 0, is_ocb_only = 0;
    uint32  line_index;
    uint32  line_shift;
    uint32  line_length;
    int core_index;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(enable);
    if (SOC_IS_ARADPLUS_AND_BELOW(unit) || SOC_IS_QAX(unit)) {
        *enable = 0;
        SOC_EXIT;
    }
    if ((core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) && core != SOC_CORE_ALL) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Core ID %d out of range\n"), core));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (queue_ndx >= SOC_DPP_DEFS_GET(unit, nof_queues)) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Base Queue ID %d out of range\n"), queue_ndx));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    line_length = soc_mem_field_length(unit, IRR_QUEUE_IS_OCB_COMMITTEDm, COMMITMENTf);
    if (line_length == 0) {
        SOCDNX_IF_ERR_EXIT(SOC_E_INTERNAL);
    }
    line_index = queue_ndx / line_length;
    line_shift = queue_ndx % line_length;
    SOC_DPP_CORES_ITER(core, core_index) {
        res = READ_IRR_QUEUE_IS_OCB_COMMITTEDm(unit, core_index, MEM_BLOCK_ANY, line_index, is_ocb_only_line);
        SOCDNX_IF_ERR_EXIT(res);
        soc_IRR_QUEUE_IS_OCB_COMMITTEDm_field_get(unit, is_ocb_only_line, COMMITMENTf, &is_ocb_only_field);

        is_ocb_only = SHR_BITGET(&is_ocb_only_field, line_shift);
        *enable = (is_ocb_only) ? TRUE : FALSE;
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}
uint32
  arad_itm_qytpe_is_ocb_only_set (
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  int                 core,
     SOC_SAND_IN  uint32              hw_q_type,
     SOC_SAND_OUT uint8               enable)
{
    uint32
        queue_ptiority_data;
    uint32  
        res = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;
    if ((core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) && core != SOC_CORE_ALL) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Core ID %d out of range\n"), core));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (hw_q_type >= SOC_DPP_DEFS_GET(unit ,nof_credit_request_profiles)) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Credit request profile %d is out of range\n"), hw_q_type));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (!SOC_IS_QAX(unit)) {
        res = READ_IPS_QPRIm(unit, IPS_BLOCK(unit, core), hw_q_type, &queue_ptiority_data);
        SOCDNX_IF_ERR_EXIT(res);

        soc_mem_field32_set(unit, IPS_QPRIm, &queue_ptiority_data, QUEUE_IS_OCB_ONLYf, enable ? TRUE : FALSE);

        res = WRITE_IPS_QPRIm(unit, IPS_BLOCK(unit, core), hw_q_type, &queue_ptiority_data);
        SOCDNX_IF_ERR_EXIT(res);
    }

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  arad_itm_qytpe_is_ocb_only_get (
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  int                 core,
     SOC_SAND_IN  uint32              hw_q_type,
     SOC_SAND_OUT uint8               *enable)
{
    uint32 
      is_ocb_only;
    uint32
        queue_ptiority_data;
    uint32 
        res = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(enable);
    if ((core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) && core != SOC_CORE_ALL) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Core ID %d out of range\n"), core));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (hw_q_type >= SOC_DPP_DEFS_GET(unit ,nof_credit_request_profiles)) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Credit request profile %d is out of range\n"), hw_q_type));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (!SOC_IS_QAX(unit)) {
        res = READ_IPS_QPRIm(unit, IPS_BLOCK(unit, core), hw_q_type, &queue_ptiority_data);
        SOCDNX_IF_ERR_EXIT(res);

        soc_mem_field_get(unit, IPS_QPRIm, &queue_ptiority_data, QUEUE_IS_OCB_ONLYf, &is_ocb_only);
        *enable = is_ocb_only ? TRUE : FALSE;
    }
exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  arad_itm_queue_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_IN  ARAD_ITM_QUEUE_INFO      *old_info,
    SOC_SAND_IN  ARAD_ITM_QUEUE_INFO      *info
  )
{
  uint32
    res;
  ARAD_IQM_STATIC_TBL_DATA
    iqm_static_tbl_data;
  ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA
    ips_queue_type_lookup_table_tbl_data;
  uint8
    hw_q_type, old_hw_q_type;
  uint8 
    is_ocb_only = FALSE;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_QUEUE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  iqm_static_tbl_data.traffic_class =
    info->vsq_traffic_cls;
  iqm_static_tbl_data.credit_class =
    info->credit_cls;
  iqm_static_tbl_data.connection_class =
    info->vsq_connection_cls;
  iqm_static_tbl_data.rate_class =
    info->rate_cls;
  iqm_static_tbl_data.que_signature =
    info->signature;

  
  SOC_SAND_CHECK_FUNC_RESULT(arad_sw_db_queue_type_map_get(unit, info->cr_req_type_ndx, &hw_q_type), 10, exit);
  if (hw_q_type == ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE) {
    SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 15, exit); 
  }

  res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_per_queue_info_set, (unit, core, queue_ndx, &iqm_static_tbl_data));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  ips_queue_type_lookup_table_tbl_data.queue_type_lookup_table = hw_q_type;
  res = arad_ips_queue_type_lookup_table_tbl_set_unsafe(
          unit,
          core,
          queue_ndx,
          &ips_queue_type_lookup_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
  if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
      
      SOC_SAND_CHECK_FUNC_RESULT(arad_sw_db_queue_type_map_get(unit, old_info->cr_req_type_ndx, &old_hw_q_type), 44 ,exit);

      res = arad_sw_db_queue_type_ref_count_exchange(unit, core, old_hw_q_type, hw_q_type, 1);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 46, exit);
      
      
      res = arad_itm_qytpe_is_ocb_only_get(unit, core, hw_q_type, &is_ocb_only);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

      
      res = arad_itm_queue_is_ocb_only_set(unit, core, queue_ndx, is_ocb_only);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);
    }
  
  res = arad_sw_db_tm_queue_to_rate_class_mapping_ref_count_exchange(unit, core, is_ocb_only, old_info->rate_cls, info->rate_cls, 1);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 65, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_info_set_unsafe()",0,0);
}


uint32
  arad_itm_queue_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_IN  ARAD_ITM_QUEUE_INFO      *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_QUEUE_INFO_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_ndx, ARAD_MAX_QUEUE_ID(unit),
    ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 5, exit
  );

  if (info->vsq_traffic_cls > (ARAD_TR_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_TRAFFIC_CLS_OUT_OF_RANGE_ERR, 10, exit);
  }
  if (info->credit_cls > (ARAD_ITM_QT_CR_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CREDIT_CLS_OUT_OF_RANGE_ERR, 20, exit);
  }
  if (info->vsq_connection_cls > (ARAD_ITM_QT_CC_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_CONNECTION_CLS_OUT_OF_RANGE_ERR, 30, exit);
  }
  if (info->rate_cls > (ARAD_ITM_QT_RT_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_RATE_CLS_OUT_OF_RANGE_ERR, 40, exit);
  }

  if ((info->cr_req_type_ndx < SOC_TMC_ITM_QT_NDX_02 || info->cr_req_type_ndx >= ARAD_ITM_IPS_QT_MAX(unit)) &&
    (info->cr_req_type_ndx < SOC_TMC_ITM_PREDEFIEND_OFFSET || info->cr_req_type_ndx >= SOC_TMC_ITM_PREDEFIEND_OFFSET + SOC_TMC_ITM_NOF_QT_STATIC) &&
    info->cr_req_type_ndx != SOC_TMC_ITM_PREDEFIEND_OFFSET + SOC_TMC_ITM_QT_NDX_15)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQ_TYPE_NDX_OUT_OF_RANGE_ERR, 50, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->signature, ARAD_MAX_SIGNATURE,
    ARAD_SIGNATURE_OUT_OF_RANGE_ERR, 80, exit
  );
  if ((core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) && core != SOC_CORE_ALL) {
      SOC_SAND_SET_ERROR_CODE(ARAD_CORE_INDEX_OUT_OF_RANGE_ERR, 70, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_info_verify()", queue_ndx, info ? info->cr_req_type_ndx : -1);
}


uint32
  arad_itm_queue_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT ARAD_ITM_QUEUE_INFO      *info
  )
{
  uint32
    res;
  ARAD_IQM_STATIC_TBL_DATA
    iqm_static_tbl_data;
  ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA
    ips_queue_type_lookup_table_tbl_data;
  uint8
    tolerance_level;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_QUEUE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_ndx, ARAD_MAX_QUEUE_ID(unit),
    ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 5, exit
  );

  res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_per_queue_info_get, (unit, core, queue_ndx, &iqm_static_tbl_data));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);

  res = arad_ips_queue_type_lookup_table_tbl_get_unsafe(
          unit,
          core,
          queue_ndx,
          &ips_queue_type_lookup_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

  info->vsq_traffic_cls =
    iqm_static_tbl_data.traffic_class;
  info->credit_cls =
    iqm_static_tbl_data.credit_class;
  info->vsq_connection_cls =
    iqm_static_tbl_data.connection_class;
  info->rate_cls =
    iqm_static_tbl_data.rate_class;
  info->signature =
    iqm_static_tbl_data.que_signature;

  SOC_SAND_CHECK_FUNC_RESULT(arad_sw_db_queue_type_map_reverse_get(unit, ips_queue_type_lookup_table_tbl_data.queue_type_lookup_table, &tolerance_level), 40, exit);
  info->cr_req_type_ndx = tolerance_level;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_info_get_unsafe()",0,0);
}


uint32
  arad_itm_queue_dyn_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT ARAD_ITM_QUEUE_DYN_INFO      *info
  )  
{
  uint32
    res;
  ARAD_IQM_DYNAMIC_TBL_DATA iqm_dynamic_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_ndx, ARAD_MAX_QUEUE_ID(unit),
    ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 5, exit
  );

  res = arad_iqm_dynamic_tbl_get_unsafe(
          unit,
          core,
          queue_ndx,
          &iqm_dynamic_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->pq_inst_que_size[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES] =
    iqm_dynamic_tbl.pq_inst_que_size;
  info->pq_inst_que_buff_size =
    iqm_dynamic_tbl.pq_inst_que_buff_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_dyn_info_get_unsafe()",0,0);
}


uint32
  arad_itm_dyn_total_thresh_set_unsafe(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  int32    reservation_increase 
  )
{
  uint32
    res;
  int 
    core_id = 0;
  soc_dpp_guaranteed_q_resource_t guaranteed_q_resource;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_DYN_TOTAL_THRESH_SET_UNSAFE);

  res = sw_state_access[unit].dpp.soc.arad.tm.guaranteed_q_resource.get(unit, core_id, &guaranteed_q_resource);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  if (reservation_increase < 0 &&  ((uint32)(-reservation_increase)) > guaranteed_q_resource.dram.used) {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 110, exit); 
  }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 115, exit);
  
  if (reservation_increase) {
    int32 resource_left_calc =  ((int32)(guaranteed_q_resource.dram.total -
            (guaranteed_q_resource.dram.used))) - reservation_increase;
    uint32 resource_left =  resource_left_calc;
    if (resource_left_calc < 0) { 
      SOC_SAND_SET_ERROR_CODE(ARAD_ITM_GRNT_OUT_OF_RESOURCE_ERR, 120, exit);
    }

    if (SOC_DPP_CONFIG(unit)->tm.guaranteed_q_mode == SOC_DPP_GUARANTEED_Q_RESOURCE_MEMORY) {

      

    } else {
      uint32 reg_dp0 = ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED;
      uint32 reg_dp1 = ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED;
      uint32 reg_dp2 = ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED;
      uint32 reg_dp3 = ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED;
      if (resource_left) { 
        
        uint32 mantissa, exponent, field_val;
        SOC_SAND_CHECK_FUNC_RESULT( soc_sand_break_to_mnt_exp_round_down( resource_left,
          ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS, ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS,
          0, &mantissa, &exponent), 110, exit);
        field_val = mantissa | (exponent << ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS);
        soc_reg_field_set(unit, IQM_BUFF_DYN_SIZE_TH_CFG_1r, &reg_dp0, BUFF_DYN_SIZE_RJCT_SET_TH_0f, field_val);
        soc_reg_field_set(unit, IQM_BUFF_DYN_SIZE_TH_CFG_1r, &reg_dp0, BUFF_DYN_SIZE_RJCT_CLR_TH_0f, field_val);

        SOC_SAND_CHECK_FUNC_RESULT( soc_sand_break_to_mnt_exp_round_down( (resource_left / 20) * 17, 
          ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS, ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS,
          0, &mantissa, &exponent), 120, exit);
        field_val = mantissa | (exponent << ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS);
        soc_reg_field_set(unit, IQM_BUFF_DYN_SIZE_TH_CFG_2r, &reg_dp1, BUFF_DYN_SIZE_RJCT_SET_TH_1f, field_val);
        soc_reg_field_set(unit, IQM_BUFF_DYN_SIZE_TH_CFG_2r, &reg_dp1, BUFF_DYN_SIZE_RJCT_CLR_TH_1f, field_val);

        SOC_SAND_CHECK_FUNC_RESULT( soc_sand_break_to_mnt_exp_round_down( (resource_left / 20) * 15, 
          ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS, ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS,
          0, &mantissa, &exponent), 130, exit);
        field_val = mantissa | (exponent << ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS);
        soc_reg_field_set(unit, IQM_BUFF_DYN_SIZE_TH_CFG_3r, &reg_dp2, BUFF_DYN_SIZE_RJCT_SET_TH_2f, field_val);
        soc_reg_field_set(unit, IQM_BUFF_DYN_SIZE_TH_CFG_3r, &reg_dp2, BUFF_DYN_SIZE_RJCT_CLR_TH_2f, field_val);

        SOC_SAND_CHECK_FUNC_RESULT( soc_sand_break_to_mnt_exp_round_down(0, 
          ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS, ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS,
          0, &mantissa, &exponent), 140, exit);
        field_val = mantissa | (exponent << ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS);
        soc_reg_field_set(unit, IQM_BUFF_DYN_SIZE_TH_CFG_4r, &reg_dp3, BUFF_DYN_SIZE_RJCT_SET_TH_3f, field_val);
        soc_reg_field_set(unit, IQM_BUFF_DYN_SIZE_TH_CFG_4r, &reg_dp3, BUFF_DYN_SIZE_RJCT_CLR_TH_3f, field_val);
      }

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 210, exit, ARAD_REG_ACCESS_ERR,
        WRITE_IQM_BUFF_DYN_SIZE_TH_CFG_1r(unit, reg_dp0));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 220, exit, ARAD_REG_ACCESS_ERR,
        WRITE_IQM_BUFF_DYN_SIZE_TH_CFG_2r(unit, reg_dp1));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 230, exit, ARAD_REG_ACCESS_ERR,
        WRITE_IQM_BUFF_DYN_SIZE_TH_CFG_3r(unit, reg_dp2));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 240, exit, ARAD_REG_ACCESS_ERR,
        WRITE_IQM_BUFF_DYN_SIZE_TH_CFG_4r(unit, reg_dp3));

    }
    guaranteed_q_resource.dram.used += reservation_increase;
    
    res = sw_state_access[unit].dpp.soc.arad.tm.guaranteed_q_resource.set(unit, core_id, &guaranteed_q_resource);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 250, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_dyn_total_thresh_set_unsafe()",0,0);
}


int
  arad_ingress_drop_status(
    SOC_SAND_IN int   unit,
    SOC_SAND_OUT uint32 *is_max_size 
  )
{
    uint64 val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_IQM_REJECT_STATUS_BMPr(unit, SOC_CORE_ALL, &val));
    (*is_max_size) = soc_reg64_field32_get(unit, IQM_REJECT_STATUS_BMPr, val, PKT_Q_MAX_SIZE_RJCTf);

    
exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  arad_itm_ingress_shape_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_INGRESS_SHAPE_INFO *info
  )
{
  uint32
    fld_val,
    q_hi,
    q_low,
    q_base,
    ihp_reg_low,
    ihp_reg_hi,
    base_port_tc,
    rate_internal,
    res;
  ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO
    explicit_info;  
  int 
      core = 0; 
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(info);
  

  res = arad_ipq_explicit_mapping_mode_info_get_unsafe(
          unit,
          &explicit_info
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

  q_base = explicit_info.base_queue_id;

  if (info->enable)
  {
    q_low = info->q_range.q_num_low;
    q_hi  = info->q_range.q_num_high;
  }
  else
  {
    
    if (
        !(explicit_info.queue_id_add_not_decrement)
        && (q_base < 2)
       )
    {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    q_low = (explicit_info.queue_id_add_not_decrement?q_base + 2:q_base-1);
    q_hi = q_low - 1;
  }


  
  res = soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_2r, REG_PORT_ANY, 0, ISP_QNUM_LOWf,  q_low);
  SOCDNX_IF_ERR_EXIT(res);

  res = soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_3r, REG_PORT_ANY, 0, ISP_QNUM_HIGHf,  q_hi);
  SOCDNX_IF_ERR_EXIT(res);

  
  if (explicit_info.queue_id_add_not_decrement)
  {
    ihp_reg_low = q_low  - q_base;
    ihp_reg_hi  = q_hi   - q_base;
  }
  else
  {
    ihp_reg_low = q_base - q_hi;
    ihp_reg_hi  = q_base - q_low;
  }

  res = arad_parser_ingress_shape_state_set(
            unit,
            info->enable,
            ihp_reg_low,
            ihp_reg_hi
          );
  SOCDNX_SAND_IF_ERR_EXIT(res);
  
  fld_val = info->enable ? 0x1 : 0x0;
  res = soc_reg_above_64_field32_modify(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, SOC_CORE_ALL, 0, INGRESS_SHAPING_ENABLEf,  fld_val);
  SOCDNX_IF_ERR_EXIT(res);

  if (info->enable)
  {
    res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, info->sch_port, &base_port_tc);
    SOCDNX_IF_ERR_EXIT(res);

    res = soc_reg_above_64_field32_modify(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, SOC_CORE_ALL, 0, INGRESS_SHAPING_PORT_IDf,  base_port_tc);
    SOCDNX_IF_ERR_EXIT(res);

    res = arad_intern_rate2clock(unit, info->rate, TRUE,  &rate_internal);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = soc_reg_above_64_field32_modify(unit, IPS_INGRESS_SHAPE_SCHEDULER_CONFIGr, SOC_CORE_ALL, 0, ISS_MAX_CR_RATEf,  rate_internal);
    SOCDNX_IF_ERR_EXIT(res);
  }

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  arad_itm_ingress_shape_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_INGRESS_SHAPE_INFO *info
  )
{
  uint32
    res;
  ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO
    explicit_info;
  uint8
    is_base_q_set;
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(info);

  if (info->enable) 
  {
      res = arad_ipq_base_q_is_valid_get_unsafe(
                unit,
                &is_base_q_set
              );
      SOCDNX_SAND_IF_ERR_EXIT(res);

      if (!is_base_q_set) {
          SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
      }
        arad_ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(&explicit_info);

        res = arad_ipq_explicit_mapping_mode_info_get_unsafe(
              unit,
              &explicit_info
            );
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if (explicit_info.base_queue_id > ARAD_MAX_QUEUE_ID(unit)) {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Base Queue ID out %d of range\n"), explicit_info.base_queue_id));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }

        if (info->q_range.q_num_low > (info->q_range.q_num_high + 1)) {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Min Queue %d is higher than max queue %d\n"), info->q_range.q_num_low, info->q_range.q_num_high));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }

        if (info->q_range.q_num_high > ARAD_MAX_QUEUE_ID(unit)) {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Max Queue ID out %d of range\n"), info->q_range.q_num_high));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }

        if (explicit_info.queue_id_add_not_decrement)
        {
            if (explicit_info.base_queue_id > info->q_range.q_num_low + 1) {
                LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Base Queue ID %d higher than Mix Queue Id %d\n"), explicit_info.base_queue_id, info->q_range.q_num_low));
                SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
            }

            if (explicit_info.base_queue_id > info->q_range.q_num_high) {
                LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Base Queue ID %d higher than Max Queue Id %d\n"), explicit_info.base_queue_id, info->q_range.q_num_high));
                SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
            }
        }
        else
        {
            if (info->q_range.q_num_high > explicit_info.base_queue_id + 1) {
                LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Base Queue ID %d lower than Max Queue Id %d\n"), explicit_info.base_queue_id, info->q_range.q_num_high));
                SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
            }
            if (info->q_range.q_num_low > explicit_info.base_queue_id + 1) {
                LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Base Queue ID %d lower than Min Queue Id %d\n"), explicit_info.base_queue_id, info->q_range.q_num_low));
                SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
            }
        }
    }
exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_itm_ingress_shape_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_INGRESS_SHAPE_INFO *info
  )
{
  uint32
    rate_internal,
    fld_val,
    res,
    dsp_pp,
    base_port_tc,
    nof_prio,
    flags;
  soc_port_t
    port;
  soc_pbmp_t
    ports_bm;
  int
    core; 
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(info);

  res = soc_reg_above_64_field32_read(unit, ECI_GLOBAL_2r, REG_PORT_ANY, 0, ISP_QNUM_LOWf, &info->q_range.q_num_low);
  SOCDNX_IF_ERR_EXIT(res);

  res = soc_reg_above_64_field32_read(unit, ECI_GLOBAL_3r, REG_PORT_ANY, 0, ISP_QNUM_HIGHf, &info->q_range.q_num_high);
  SOCDNX_IF_ERR_EXIT(res);

  res = soc_reg_above_64_field32_read(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, SOC_CORE_ALL, 0, INGRESS_SHAPING_PORT_IDf, &base_port_tc);
  SOCDNX_IF_ERR_EXIT(res);

  
  res = soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm);
  SOCDNX_IF_ERR_EXIT(res);

  SOC_PBMP_ITER(ports_bm, port) 
  {
      res = soc_port_sw_db_flags_get(unit, port, &flags);
      SOCDNX_IF_ERR_EXIT(res);

      if (!(SOC_PORT_IS_ELK_INTERFACE(flags) || SOC_PORT_IS_STAT_INTERFACE(flags))) {

          res = soc_port_sw_db_local_to_tm_port_get(unit, port, &dsp_pp, &core);
          SOCDNX_IF_ERR_EXIT(res);

          res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, dsp_pp, &nof_prio);
          SOCDNX_IF_ERR_EXIT(res);

          if (base_port_tc == nof_prio) {
              info->sch_port = dsp_pp;
          }
      }
  }
  
  res = soc_reg_above_64_field32_read(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, SOC_CORE_ALL, 0, INGRESS_SHAPING_ENABLEf, &fld_val);
  SOCDNX_IF_ERR_EXIT(res);
  info->enable = SOC_SAND_NUM2BOOL(fld_val);

  res = soc_reg_above_64_field32_read(unit, IPS_INGRESS_SHAPE_SCHEDULER_CONFIGr, SOC_CORE_ALL, 0, ISS_MAX_CR_RATEf, &rate_internal);
  SOCDNX_IF_ERR_EXIT(res);

  res = arad_intern_clock2rate(
          unit,
          rate_internal,
          TRUE, 
          &(info->rate)
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_itm_priority_map_tmplt_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 map_ndx,
    SOC_SAND_IN  ARAD_ITM_PRIORITY_MAP_TMPLT *info
  )
{
  uint32
    res;
  ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA
    ips_queue_priority_maps_table_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PRIORITY_MAP_TMPLT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_sand_os_memcpy(
          ips_queue_priority_maps_table_tbl_data.queue_priority_maps_table,
          info->map,
          ARAD_ITM_PRIO_MAP_SIZE_IN_UINT32S * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_ips_queue_priority_maps_table_tbl_set_unsafe(
          unit,
          map_ndx,
          &ips_queue_priority_maps_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_priority_map_tmplt_set_unsafe()",0,0);
}


uint32
  arad_itm_priority_map_tmplt_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 map_ndx,
    SOC_SAND_IN  ARAD_ITM_PRIORITY_MAP_TMPLT *info
  )
{
  uint32
    index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PRIORITY_MAP_TMPLT_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    map_ndx, ARAD_ITM_PRIORITY_MAP_NDX_MAX,
    ARAD_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 10, exit
  );

  for (index = 0 ; index < ARAD_ITM_PRIO_MAP_SIZE_IN_UINT32S; ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->map[index], ARAD_ITM_PRIORITY_MAP_TMPLT_MAP_MAX,
      ARAD_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 20, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_priority_map_tmplt_verify()",0,0);
}


uint32
  arad_itm_priority_map_tmplt_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 map_ndx,
    SOC_SAND_OUT ARAD_ITM_PRIORITY_MAP_TMPLT *info
  )
{
  uint32
    res;
  ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA
    ips_queue_priority_maps_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PRIORITY_MAP_TMPLT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    map_ndx, ARAD_ITM_PRIORITY_MAP_NDX_MAX,
    ARAD_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 5, exit
  );

  res = arad_ips_queue_priority_maps_table_tbl_get_unsafe(
          unit,
          map_ndx,
          &ips_queue_priority_maps_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_os_memcpy(
          info->map,
          ips_queue_priority_maps_table_tbl_data.queue_priority_maps_table,
          ARAD_ITM_PRIO_MAP_SIZE_IN_UINT32S * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_priority_map_tmplt_get_unsafe()",0,0);
}


uint32
  arad_itm_priority_map_tmplt_select_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_64_ndx,
    SOC_SAND_IN  uint32                 priority_map
  )
{
  uint32
    res;
  ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA
    ips_queue_priority_map_select_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PRIORITY_MAP_TMPLT_SELECT_SET_UNSAFE);

  ips_queue_priority_map_select_tbl_data.queue_priority_map_select = priority_map;

  res = arad_ips_queue_priority_map_select_tbl_set_unsafe(unit, queue_64_ndx, &ips_queue_priority_map_select_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_priority_map_tmplt_select_set_unsafe()",0,0);
}


uint32
  arad_itm_priority_map_tmplt_select_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_64_ndx,
    SOC_SAND_IN  uint32                 priority_map
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PRIORITY_MAP_TMPLT_SELECT_VERIFY);


  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_64_ndx, (ARAD_ITM_PRIO_NOF_SEGMENTS(unit) - 1),
    ARAD_ITM_PRIORITY_MAP_SEGMENT_NDX_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    priority_map, ARAD_ITM_PRIORITY_MAP_NDX_MAX,
    ARAD_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_priority_map_tmplt_select_verify()",0,0);
}


uint32
  arad_itm_priority_map_tmplt_select_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_64_ndx,
    SOC_SAND_OUT uint32                 *priority_map
  )
{
  uint32
    res;
  ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA
    ips_queue_priority_map_select_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PRIORITY_MAP_TMPLT_SELECT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(priority_map);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_64_ndx, (ARAD_ITM_PRIO_NOF_SEGMENTS(unit) - 1),
    ARAD_ITM_PRIORITY_MAP_SEGMENT_NDX_OUT_OF_RANGE_ERR, 10, exit
  );

  res = arad_ips_queue_priority_map_select_tbl_get_unsafe(
          unit,
          queue_64_ndx,
          &ips_queue_priority_map_select_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *priority_map = ips_queue_priority_map_select_tbl_data.queue_priority_map_select;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_priority_map_tmplt_select_get_unsafe()",0,0);
}


uint32
  arad_itm_sys_red_drop_prob_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_DROP_PROB *info
  )
{
  uint32
    res,
    indx;
  ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA
    iqm_system_red_drop_probability_values_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_DROP_PROB_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  for (indx = 0; indx<ARAD_ITM_SYS_RED_DRP_PROBS; ++indx)
  {
    iqm_system_red_drop_probability_values_tbl_data.drp_prob =
      info->drop_probs[indx];

    res= arad_iqm_system_red_drop_probability_values_tbl_set_unsafe(
           unit,
           indx,
           &iqm_system_red_drop_probability_values_tbl_data
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20+indx, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_drop_prob_set_unsafe()",0,0);
}


uint32
  arad_itm_sys_red_drop_prob_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_DROP_PROB *info
  )
{
  uint32
    index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_DROP_PROB_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  for (index = 0 ; index < ARAD_ITM_SYS_RED_DRP_PROBS; ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->drop_probs[index], ARAD_ITM_SYS_RED_DROP_PROB_VAL_MAX,
      ARAD_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 10, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_drop_prob_verify()",0,0);
}


uint32
  arad_itm_sys_red_drop_prob_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_DROP_PROB *info
  )
{
  uint32
    res,
    indx;
  ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA
    iqm_system_red_drop_probability_values_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_DROP_PROB_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  for (indx = 0; indx<ARAD_ITM_SYS_RED_DRP_PROBS; ++indx)
  {
    res= arad_iqm_system_red_drop_probability_values_tbl_get_unsafe(
           unit,
           indx,
           &iqm_system_red_drop_probability_values_tbl_data
         );
    info->drop_probs[indx] =
      iqm_system_red_drop_probability_values_tbl_data.drp_prob;

    SOC_SAND_CHECK_FUNC_RESULT(res, 20+indx, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_drop_prob_get_unsafe()",0,0);
}


uint32
  arad_itm_sys_red_queue_size_boundaries_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_INFO *info,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *exact_info
    )
{
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_UNSAFE);

  res = x_itm_sys_red_queue_size_boundaries_set_unsafe(
    unit,
    rt_cls_ndx,
    info,
    exact_info,
    ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_LSB,
    ARAD_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_MNT_NOF_BITS,
    ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_MSB,
    ARAD_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_EXP_NOF_BITS
  );

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_queue_size_boundaries_set_unsafe()",0,0);
}


uint32
  x_itm_sys_red_queue_size_boundaries_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_INFO *info,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *exact_info,
    SOC_SAND_IN int mnt_lsb,
    SOC_SAND_IN int mnt_nof_bits,
    SOC_SAND_IN int exp_lsb,
    SOC_SAND_IN int exp_nof_bits
  )
{
  uint32
    res,
    boundary_i;
  ARAD_IQM_SYSTEM_RED_TBL_DATA
    iqm_system_red_tbl_data;
  int32
    exact_value_var;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  for (boundary_i = 0; boundary_i < (ARAD_ITM_SYS_RED_Q_SIZE_RANGES-1); boundary_i++)
  {

    res = arad_itm_man_exp_buffer_set(
            info->queue_size_boundaries[boundary_i] / ARAD_ITM_SYS_RED_Q_BOUNDARY_BYTES_RESOLUTION,
            mnt_lsb,
            mnt_nof_bits,
            exp_lsb,
            exp_nof_bits,
            FALSE,
            &iqm_system_red_tbl_data.qsz_rng_th[boundary_i],
            &exact_value_var
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    exact_info->queue_size_boundaries[boundary_i] = (uint32)exact_value_var * ARAD_ITM_SYS_RED_Q_BOUNDARY_BYTES_RESOLUTION;
  }

  res = arad_iqm_system_red_tbl_set_unsafe(
    unit,
    rt_cls_ndx,
    &iqm_system_red_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in x_itm_sys_red_queue_size_boundaries_set_unsafe()",0,0);
}


uint32
  arad_itm_sys_red_queue_size_boundaries_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_INFO *info
  )
{
  uint32
    index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 10, exit
  );

  for (index = 0 ; index < (ARAD_ITM_SYS_RED_Q_SIZE_RANGES-1); ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->queue_size_boundaries[index], ARAD_MAX_QUEUE_SIZE_BYTES,
      ARAD_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 20, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_queue_size_boundaries_verify()",0,0);
}


uint32
  arad_itm_sys_red_queue_size_boundaries_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *info
  )
{
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_GET_UNSAFE);

  res = x_itm_sys_red_queue_size_boundaries_get_unsafe(
    unit,
    rt_cls_ndx,
    info,
    ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_LSB,
    ARAD_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_MNT_NOF_BITS,
    ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_LSB,
    ARAD_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_EXP_NOF_BITS
  );

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_queue_size_boundaries_get_unsafe()",0,0);
}


uint32
  x_itm_sys_red_queue_size_boundaries_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *info,
    SOC_SAND_IN int mnt_lsb,
    SOC_SAND_IN int mnt_nof_bits,
    SOC_SAND_IN int exp_lsb,
    SOC_SAND_IN int exp_nof_bits
  )
{
  uint32
    res,
    boundary_i;
  ARAD_IQM_SYSTEM_RED_TBL_DATA
    iqm_system_red_tbl_data;
  int32
    exact_value_var;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 10, exit
  );

  res = arad_iqm_system_red_tbl_get_unsafe(
    unit,
    rt_cls_ndx,
    &iqm_system_red_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (boundary_i = 0; boundary_i < (ARAD_ITM_SYS_RED_Q_SIZE_RANGES-1); boundary_i++)
  {
    res = arad_itm_man_exp_buffer_get(
            iqm_system_red_tbl_data.qsz_rng_th[boundary_i],
            mnt_lsb,
            mnt_nof_bits,
            exp_lsb,
            exp_nof_bits,
            FALSE,
            &exact_value_var
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    info->queue_size_boundaries[boundary_i] = (uint32)exact_value_var * ARAD_ITM_SYS_RED_Q_BOUNDARY_BYTES_RESOLUTION;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in x_itm_sys_red_queue_size_boundaries_get_unsafe()",0,0);
}


uint32
  arad_itm_sys_red_q_based_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_DP_INFO *info
  )
{
  uint32
    res;
  ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_system_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_Q_BASED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_iqm_system_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          sys_red_dp_ndx,
          &iqm_system_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  iqm_system_red_parameters_table_tbl_data.adm_th =
    info->adm_th;
  iqm_system_red_parameters_table_tbl_data.prob_th =
    info->prob_th;
  iqm_system_red_parameters_table_tbl_data.drp_th =
    info->drp_th;
  iqm_system_red_parameters_table_tbl_data.drp_prob_indx1 =
    info->drp_prob_low;
  iqm_system_red_parameters_table_tbl_data.drp_prob_indx2 =
    info->drp_prob_high;
  iqm_system_red_parameters_table_tbl_data.sys_red_en =
    info->enable;

  res = arad_iqm_system_red_parameters_table_tbl_set_unsafe(
          unit,
          rt_cls_ndx,
          sys_red_dp_ndx,
          &iqm_system_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_q_based_set_unsafe()",0,0);
}


uint32
  arad_itm_sys_red_q_based_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_DP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_Q_BASED_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    sys_red_dp_ndx, ARAD_MAX_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  if (info->adm_th > ARAD_ITM_SYS_RED_QUEUE_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_SYS_RED_QUEUE_TH_OUT_OF_RANGE_ERR, 10, exit);
  }
  if(info->prob_th > ARAD_ITM_SYS_RED_QUEUE_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_SYS_RED_QUEUE_TH_OUT_OF_RANGE_ERR, 20, exit);
  }
  if (info->drp_th > ARAD_ITM_SYS_RED_QUEUE_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_SYS_RED_QUEUE_TH_OUT_OF_RANGE_ERR, 30, exit);
  }
  if (info->drp_prob_low > (ARAD_ITM_SYS_RED_DRP_PROBS-1))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_SYS_RED_DRP_PROB_INDEX_OUT_OF_RANGE_ERR, 40, exit);
  }
  if (info->drp_prob_high > (ARAD_ITM_SYS_RED_DRP_PROBS-1))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_SYS_RED_DRP_PROB_INDEX_OUT_OF_RANGE_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_q_based_verify()",0,0);
}


uint32
  arad_itm_sys_red_q_based_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_DP_INFO *info
  )
{
  uint32
    res;
  ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_system_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_Q_BASED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    sys_red_dp_ndx, ARAD_MAX_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  res = arad_iqm_system_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          sys_red_dp_ndx,
          &iqm_system_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->adm_th =
    iqm_system_red_parameters_table_tbl_data.adm_th;
  info->prob_th =
    iqm_system_red_parameters_table_tbl_data.prob_th;
  info->drp_th =
    iqm_system_red_parameters_table_tbl_data.drp_th;
  info->drp_prob_low =
    iqm_system_red_parameters_table_tbl_data.drp_prob_indx1;
  info->drp_prob_high =
    iqm_system_red_parameters_table_tbl_data.drp_prob_indx2;
  info->enable =
    (uint8)iqm_system_red_parameters_table_tbl_data.sys_red_en;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_q_based_get_unsafe()",0,0);
}


uint32
  arad_itm_sys_red_eg_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_EG_INFO *info,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_EG_INFO *exact_info
  )
{
  uint32
    res,
    aging_timer,
    device_mega_ticks_per_sec;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_EG_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  device_mega_ticks_per_sec = arad_chip_mega_ticks_per_sec_get(unit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  0, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_SYSTEM_RED_CONFIGURATIONr, SOC_CORE_ALL, 0, ENABLE_SYS_REDf,  info->enable));

  
  if(info->aging_timer > SOC_SAND_DIV_ROUND_DOWN(SAL_UINT32_MAX,1000000))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_SYS_RED_EG_INFO_AGING_TIMER_OUT_OF_RANGE_ERR, 10, exit);
  }

  
  aging_timer = info->aging_timer * device_mega_ticks_per_sec * 1000;
  aging_timer = SOC_SAND_DIV_ROUND_UP(aging_timer, 1024);
  
  
  if (aging_timer > ARAD_ITM_SYS_RED_EG_INFO_AGING_TIMER_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_SYS_RED_EG_INFO_AGING_TIMER_OUT_OF_RANGE_ERR, 30, exit);
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_SYSTEM_RED_CONFIGURATIONr, SOC_CORE_ALL, 0, AGING_TIMER_CFGf,  info->aging_timer));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_SYSTEM_RED_CONFIGURATIONr, SOC_CORE_ALL, 0, RESET_XPIRED_QSZf,  info->reset_expired_q_size));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_SYSTEM_RED_CONFIGURATIONr, SOC_CORE_ALL, 0, AGING_ONLY_DEC_PQSf,  info->aging_only_dec_q_size));

  exact_info->enable = info->enable;
  exact_info->reset_expired_q_size = info->reset_expired_q_size;
  exact_info->aging_only_dec_q_size = info->aging_only_dec_q_size;
  exact_info->aging_timer = aging_timer * 1024 / device_mega_ticks_per_sec;
  exact_info->aging_timer /= 1000;

  if (SOC_IS_ARAD_A0(unit)) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_SYSTEM_RED_AGING_CONFIGURATIONr, SOC_CORE_ALL, 0, SYSTEM_RED_AGE_PERIODf,  0x0));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_eg_set_unsafe()",0,0);
}


uint32
  arad_itm_sys_red_eg_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_EG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_EG_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_eg_verify()",0,0);
}


uint32
  arad_itm_sys_red_eg_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_EG_INFO *info
  )
{
  uint32
    res,
    sys_red_enable,
    reset_expired_q_size_en,
    aging_only_dec_q_size_en;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_EG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_SYSTEM_RED_CONFIGURATIONr, SOC_CORE_ALL, 0, ENABLE_SYS_REDf, &sys_red_enable));
  info->enable = (uint8)sys_red_enable;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_SYSTEM_RED_CONFIGURATIONr, SOC_CORE_ALL, 0, AGING_TIMER_CFGf, &info->aging_timer));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  14,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_SYSTEM_RED_CONFIGURATIONr, SOC_CORE_ALL, 0, RESET_XPIRED_QSZf, &reset_expired_q_size_en));
  info->reset_expired_q_size = (uint8)reset_expired_q_size_en;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  16,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_SYSTEM_RED_CONFIGURATIONr, SOC_CORE_ALL, 0, AGING_ONLY_DEC_PQSf, &aging_only_dec_q_size_en));
  info->aging_only_dec_q_size = (uint8)aging_only_dec_q_size_en;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_eg_get_unsafe()",0,0);
}


uint32
    arad_itm_sys_red_glob_rcs_set_unsafe(
        SOC_SAND_IN  int                 unit,
        SOC_SAND_IN  ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info)
{
    uint32
        res,
        reg_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_GLOB_RCS_SET_UNSAFE);

    SOC_SAND_CHECK_NULL_INPUT(info);


    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  5,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_UNICAST_DBUFF_THRESHOLD_0r, REG_PORT_ANY, 0, FR_UN_DB_TH_0f,  info->thresholds.unicast_rng_ths[0]));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_UNICAST_DBUFF_THRESHOLD_1r, REG_PORT_ANY, 0, FR_UN_DB_TH_1f,  info->thresholds.unicast_rng_ths[1]));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_UNICAST_DBUFF_THRESHOLD_2r, REG_PORT_ANY, 0, FR_UN_DB_TH_2f,  info->thresholds.unicast_rng_ths[2]));
    reg_val = 0x0;
    ARAD_FLD_TO_REG(IQM_FREE_UNICAST_DBUFF_RANGE_VALUESr, FR_UN_DB_RNG_VAL_0f, info->values.unicast_rng_vals[0], reg_val, 25, exit);
    ARAD_FLD_TO_REG(IQM_FREE_UNICAST_DBUFF_RANGE_VALUESr, FR_UN_DB_RNG_VAL_1f, info->values.unicast_rng_vals[1], reg_val, 30, exit);
    ARAD_FLD_TO_REG(IQM_FREE_UNICAST_DBUFF_RANGE_VALUESr, FR_UN_DB_RNG_VAL_2f, info->values.unicast_rng_vals[2], reg_val, 35, exit);
    ARAD_FLD_TO_REG(IQM_FREE_UNICAST_DBUFF_RANGE_VALUESr, FR_UN_DB_RNG_VAL_3f, info->values.unicast_rng_vals[3], reg_val, 40, exit);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,WRITE_IQM_FREE_UNICAST_DBUFF_RANGE_VALUESr(unit,  reg_val));

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  55,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_FULL_MULTICAST_DBUFF_THRESHOLD_0r, REG_PORT_ANY, 0, FR_FL_MUL_DB_TH_0f,  info->thresholds.multicast_rng_ths[0]));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_FULL_MULTICAST_DBUFF_THRESHOLD_1r, REG_PORT_ANY, 0, FR_FL_MUL_DB_TH_1f,  info->thresholds.multicast_rng_ths[1]));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  65,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_FULL_MULTICAST_DBUFF_THRESHOLD_2r, REG_PORT_ANY, 0, FR_FL_MUL_DB_TH_2f,  info->thresholds.multicast_rng_ths[2]));
    reg_val = 0x0;
    ARAD_FLD_TO_REG(IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr, FR_FL_ML_DB_RNG_VAL_0f, info->values.multicast_rng_vals[0], reg_val, 75, exit);
    ARAD_FLD_TO_REG(IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr, FR_FL_ML_DB_RNG_VAL_1f, info->values.multicast_rng_vals[1], reg_val, 80, exit);
    ARAD_FLD_TO_REG(IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr, FR_FL_ML_DB_RNG_VAL_2f, info->values.multicast_rng_vals[2], reg_val, 85, exit);
    ARAD_FLD_TO_REG(IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr, FR_FL_ML_DB_RNG_VAL_3f, info->values.multicast_rng_vals[3], reg_val, 90, exit);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,WRITE_IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr(unit, SOC_CORE_ALL,  reg_val));

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  55,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_BDB_THRESHOLD_0r, REG_PORT_ANY, 0, FR_BDB_TH_0f,  info->thresholds.bds_rng_ths[0]));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_BDB_THRESHOLD_1r, REG_PORT_ANY, 0, FR_BDB_TH_1f,  info->thresholds.bds_rng_ths[1]));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  65,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_BDB_THRESHOLD_2r, REG_PORT_ANY, 0, FR_BDB_TH_2f,  info->thresholds.bds_rng_ths[2]));
    reg_val = 0x0;
    ARAD_FLD_TO_REG(IQM_FREE_BDB_RANGE_VALUESr, FR_BDB_RNG_VAL_0f, info->values.bds_rng_vals[0], reg_val, 75, exit);
    ARAD_FLD_TO_REG(IQM_FREE_BDB_RANGE_VALUESr, FR_BDB_RNG_VAL_1f, info->values.bds_rng_vals[1], reg_val, 80, exit);
    ARAD_FLD_TO_REG(IQM_FREE_BDB_RANGE_VALUESr, FR_BDB_RNG_VAL_2f, info->values.bds_rng_vals[2], reg_val, 85, exit);
    ARAD_FLD_TO_REG(IQM_FREE_BDB_RANGE_VALUESr, FR_BDB_RNG_VAL_3f, info->values.bds_rng_vals[3], reg_val, 90, exit);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,WRITE_IQM_FREE_BDB_RANGE_VALUESr(unit, SOC_CORE_ALL,  reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_glob_rcs_set_unsafe()",0,0);
}


uint32
  arad_itm_sys_red_glob_rcs_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  )
{
  uint32
    index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_GLOB_RCS_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  for (index = 0 ; index < (ARAD_ITM_SYS_RED_BUFFS_RNGS-1); ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->thresholds.unicast_rng_ths[index], ARAD_ITM_SYS_RED_GLOB_RCS_THS_UNICAST_RNG_THS_MAX,
      ARAD_ITM_SYS_RED_GLOB_RCS_RNG_THS_OUT_OF_RANGE_ERR, 10 + index, exit
    );
  }
  for (index = 0 ; index < (ARAD_ITM_SYS_RED_BUFFS_RNGS-1); ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->thresholds.multicast_rng_ths[index], ARAD_ITM_SYS_RED_GLOB_RCS_THS_MULTICAST_RNG_THS_MAX,
      ARAD_ITM_SYS_RED_GLOB_RCS_RNG_THS_OUT_OF_RANGE_ERR, 30 + index, exit
    );
  }
  for (index = 0 ; index < (ARAD_ITM_SYS_RED_BUFFS_RNGS-1); ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->thresholds.bds_rng_ths[index], ARAD_ITM_SYS_RED_GLOB_RCS_THS_BDS_RNG_THS_MAX,
      ARAD_ITM_SYS_RED_GLOB_RCS_THS_OUT_OF_RANGE_ERR, 50 + index, exit
      );
  }

  for (index = 0 ; index < ARAD_ITM_SYS_RED_BUFFS_RNGS; ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->values.unicast_rng_vals[index], ARAD_ITM_SYS_RED_GLOB_RCS_VALS_RNG_VALS_MAX,
      ARAD_ITM_SYS_RED_GLOB_RCS_RNG_VALS_OUT_OF_RANGE_ERR, 70 + index, exit
    );
  }
  for (index = 0 ; index < ARAD_ITM_SYS_RED_BUFFS_RNGS; ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->values.multicast_rng_vals[index], ARAD_ITM_SYS_RED_GLOB_RCS_VALS_RNG_VALS_MAX,
      ARAD_ITM_SYS_RED_GLOB_RCS_RNG_VALS_OUT_OF_RANGE_ERR, 90 + index, exit
    );
  }
  for (index = 0 ; index < ARAD_ITM_SYS_RED_BUFFS_RNGS; ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->values.bds_rng_vals[index], ARAD_ITM_SYS_RED_GLOB_RCS_VALS_RNG_VALS_MAX,
      ARAD_ITM_SYS_RED_GLOB_RCS_RNG_VALS_OUT_OF_RANGE_ERR, 110 + index, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_glob_rcs_verify()",0,0);
}


uint32
  arad_itm_sys_red_glob_rcs_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  )
{
  uint32
    res,
    indx;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_GLOB_RCS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  

   
  for (indx = 0 ; indx < (ARAD_ITM_SYS_RED_BUFFS_RNGS-1) ; ++indx)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_UNICAST_DBUFF_THRESHOLD_0r + indx, REG_PORT_ANY, 0, FR_UN_DB_TH_0f + indx, &info->thresholds.unicast_rng_ths[indx]));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_UNICAST_DBUFF_RANGE_VALUESr, REG_PORT_ANY, 0, FR_UN_DB_RNG_VAL_0f + indx, &info->values.unicast_rng_vals[indx]));
  }
   
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  14,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_UNICAST_DBUFF_RANGE_VALUESr, REG_PORT_ANY, 0, FR_UN_DB_RNG_VAL_0f + indx, &info->values.unicast_rng_vals[indx]));

    
  for (indx = 0 ; indx < ARAD_ITM_SYS_RED_BUFFS_RNGS-1 ; ++indx)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  16,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_FULL_MULTICAST_DBUFF_THRESHOLD_0r + indx, REG_PORT_ANY, 0, FR_FL_MUL_DB_TH_0f + indx, &info->thresholds.multicast_rng_ths[indx]));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  18,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr, SOC_CORE_ALL, 0, FR_FL_ML_DB_RNG_VAL_0f + indx, &info->values.multicast_rng_vals[indx]));
  }

   
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr, SOC_CORE_ALL, 0, FR_FL_ML_DB_RNG_VAL_0f + indx, &info->values.multicast_rng_vals[indx]));

   

  for (indx = 0 ; indx < ARAD_ITM_SYS_RED_BUFFS_RNGS-1 ; ++indx)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  22,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_BDB_THRESHOLD_0r + indx, REG_PORT_ANY, 0, FR_BDB_TH_0f + indx, &info->thresholds.bds_rng_ths[indx]));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  24,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_BDB_RANGE_VALUESr, SOC_CORE_ALL, 0, FR_BDB_RNG_VAL_0f + indx, &info->values.bds_rng_vals[indx]));
  }

   
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  26,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_BDB_RANGE_VALUESr, SOC_CORE_ALL, 0, FR_BDB_RNG_VAL_0f + indx, &info->values.bds_rng_vals[indx]));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_glob_rcs_get_unsafe()",0,0);
}

uint32
  arad_itm_dbuff_size2internal(
    SOC_SAND_IN  ARAD_ITM_DBUFF_SIZE_BYTES dbuff_size,
    SOC_SAND_OUT uint32                   *dbuff_size_internal
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_DBUFF_SIZE2INTERNAL);

  switch(dbuff_size) {
  case ARAD_ITM_DBUFF_SIZE_BYTES_256:
    *dbuff_size_internal = 0x0;
    break;
  case ARAD_ITM_DBUFF_SIZE_BYTES_512:
    *dbuff_size_internal = 0x1;
    break;
  case ARAD_ITM_DBUFF_SIZE_BYTES_1024:
    *dbuff_size_internal = 0x2;
    break;
  case ARAD_ITM_DBUFF_SIZE_BYTES_2048:
    *dbuff_size_internal = 0x3;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_DBUFF_SIZE_INVALID_ERR, 10, exit)
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_dbuff_size2internal()",0,0);
}

uint32
  arad_itm_dbuff_internal2size(
    SOC_SAND_IN  uint32                   dbuff_size_internal,
    SOC_SAND_OUT ARAD_ITM_DBUFF_SIZE_BYTES *dbuff_size
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_DBUFF_INTERNAL2SIZE);

  switch(dbuff_size_internal) {
  case 0x0:
    *dbuff_size = ARAD_ITM_DBUFF_SIZE_BYTES_256;
    break;
  case 0x1:
    *dbuff_size = ARAD_ITM_DBUFF_SIZE_BYTES_512;
    break;
  case 0x2:
    *dbuff_size = ARAD_ITM_DBUFF_SIZE_BYTES_1024;
    break;
  case 0x3:
    *dbuff_size = ARAD_ITM_DBUFF_SIZE_BYTES_2048;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_DBUFF_SIZE_INVALID_ERR, 10, exit)
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_dbuff_internal2size()",0,0);
}


uint32
  arad_b_itm_glob_rcs_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_GLOB_RCS_DROP_TH *info
  )
{
  uint32
    index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_B_ITM_GLOB_RCS_DROP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  

  for (index = 0 ; index < ARAD_NOF_DROP_PRECEDENCE; index++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->mem_excess[index].clear, ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 10, exit
    );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->mem_excess[index].set, ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 20, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_b_itm_glob_rcs_drop_verify()",0,0);
}

uint32
  arad_b_itm_glob_rcs_drop_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_THRESH_WITH_HYST_INFO mem_size[ARAD_NOF_DROP_PRECEDENCE],
    SOC_SAND_OUT ARAD_THRESH_WITH_HYST_INFO exact_mem_size[ARAD_NOF_DROP_PRECEDENCE]
  )
{
  uint32
      exp_man,
    res,
    mnt_nof_bits,
    exp_nof_bits;
  uint32
    mnt_val,
    exp_val,
    indx = 0;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_DROP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mem_size);
  SOC_SAND_CHECK_NULL_INPUT(exact_mem_size);

  

  
  mnt_nof_bits = 12;
  exp_nof_bits = soc_reg_field_length(unit, IQM_DRAM_DYN_SIZE_TH_CFG_0r, DRAM_DYN_SIZE_RJCT_SET_TH_0f) - mnt_nof_bits;

  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              mem_size[indx].clear,
              mnt_nof_bits,
              exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      exact_mem_size[indx].clear = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_DRAM_DYN_SIZE_TH_CFG_0r + indx, REG_PORT_ANY,  0, DRAM_DYN_SIZE_RJCT_CLR_TH_0f + indx,  exp_man));

      res = soc_sand_break_to_mnt_exp_round_up(
              mem_size[indx].set,
              mnt_nof_bits,
              exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      exact_mem_size[indx].set = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_DRAM_DYN_SIZE_TH_CFG_0r + indx, REG_PORT_ANY,  0, DRAM_DYN_SIZE_RJCT_SET_TH_0f + indx,  exp_man));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_drop_set_unsafe()", indx, 0);
}

uint32
  arad_b_itm_glob_rcs_drop_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT ARAD_THRESH_WITH_HYST_INFO mem_size[ARAD_NOF_DROP_PRECEDENCE]
  )
{
  uint32
    res,
    exp_man = 0,
    mnt_nof_bits,
    mnt_val,
    exp_val;
   uint32
    indx;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mem_size);

  

  
  mnt_nof_bits = 12;

  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_DRAM_DYN_SIZE_TH_CFG_0r + indx, REG_PORT_ANY,  0, DRAM_DYN_SIZE_RJCT_CLR_TH_0f + indx, &exp_man));

      arad_iqm_mantissa_exponent_get(
          unit,
          exp_man,
          mnt_nof_bits,
          &mnt_val,
          &exp_val
      );

    mem_size[indx].clear = (mnt_val) * (1<<(exp_val));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_DRAM_DYN_SIZE_TH_CFG_0r + indx, REG_PORT_ANY,  0, DRAM_DYN_SIZE_RJCT_SET_TH_0f + indx, &exp_man));
    arad_iqm_mantissa_exponent_get(
        unit,
        exp_man,
        mnt_nof_bits,
        &mnt_val,
        &exp_val
    );

    mem_size[indx].set = (mnt_val) * (1<<(exp_val));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_drop_get_unsafe()",0,0);

}


uint32
  arad_itm_committed_q_size_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  rt_cls_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_GUARANTEED_INFO *info, 
    SOC_SAND_OUT SOC_TMC_ITM_GUARANTEED_INFO *exact_info 
  )
{
  uint32
    data,
    exp_man,
    res = SOC_SAND_OK;
  uint32
    mnt = 0,
    exp = 0;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_COMMITTED_Q_SIZE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IQM_GRSPRMm(unit, MEM_BLOCK_ANY, rt_cls_ndx, &data));

  if (info->guaranteed_size[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES] != SOC_TMC_ITM_GUARANTEED_INFO_DO_NOT_SET) {
      
      res = soc_sand_break_to_mnt_exp_round_up(
          info->guaranteed_size[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES] / ARAD_ITM_COMMITTED_BYTES_RESOLUTION,
          ARAD_ITM_COMMITTED_BYTES_MANTISSA_NOF_BITS,
          ARAD_ITM_COMMITTED_BYTES_EXPONENT_NOF_BITS,
          0, &mnt, &exp
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
      
      
      arad_iqm_mantissa_exponent_set(unit, mnt, exp, ARAD_ITM_COMMITTED_BYTES_MANTISSA_NOF_BITS, &exp_man);
      soc_IQM_GRSPRMm_field32_set(unit, &data, GRNT_BYTES_THf, exp_man);
      exact_info->guaranteed_size[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES] = mnt * (ARAD_ITM_COMMITTED_BYTES_RESOLUTION << exp);
  } else {
      exp_man = soc_IQM_GRSPRMm_field32_get(unit, &data, GRNT_BYTES_THf);
      arad_iqm_mantissa_exponent_get(unit, exp_man, ARAD_ITM_COMMITTED_BYTES_MANTISSA_NOF_BITS, &mnt, &exp);
      exact_info->guaranteed_size[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES] = mnt * (ARAD_ITM_COMMITTED_BYTES_RESOLUTION << exp);
  }
  
  if (info->guaranteed_size_bds != SOC_TMC_ITM_GUARANTEED_INFO_DO_NOT_SET) {
      
      res = soc_sand_break_to_mnt_exp_round_up(
          info->guaranteed_size_bds,
          ARAD_ITM_COMMITTED_BDS_MANTISSA_NOF_BITS,
          ARAD_ITM_COMMITTED_BDS_EXPONENT_NOF_BITS,
          0, &mnt, &exp
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 300, exit);
      
      
      arad_iqm_mantissa_exponent_set(unit, mnt, exp, ARAD_ITM_COMMITTED_BDS_MANTISSA_NOF_BITS, &exp_man);
      soc_IQM_GRSPRMm_field32_set(unit, &data, GRNT_BUFFS_THf, exp_man);
      exact_info->guaranteed_size_bds = mnt << exp;
  } else {
      exp_man = soc_IQM_GRSPRMm_field32_get(unit, &data, GRNT_BUFFS_THf);
      arad_iqm_mantissa_exponent_get(unit, exp_man, ARAD_ITM_COMMITTED_BDS_MANTISSA_NOF_BITS, &mnt, &exp);
      exact_info->guaranteed_size_bds = mnt << exp;
  }
  
  if ((info->guaranteed_size[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES] != SOC_TMC_ITM_GUARANTEED_INFO_DO_NOT_SET) ||
      (info->guaranteed_size_bds != SOC_TMC_ITM_GUARANTEED_INFO_DO_NOT_SET)) {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 400, exit, WRITE_IQM_GRSPRMm(unit, MEM_BLOCK_ANY, rt_cls_ndx, &data));
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_committed_q_size_set_unsafe()", 0, 0);
}

uint32
  arad_itm_committed_q_size_set_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  rt_cls_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_GUARANTEED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_COMMITTED_Q_SIZE_SET_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_ERR_IF_ABOVE_MAX(rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX, ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 10, exit);
  if (info->guaranteed_size[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES] != SOC_TMC_ITM_GUARANTEED_INFO_DO_NOT_SET) {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->guaranteed_size[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES], ARAD_ITM_GRNT_BYTES_MAX, ARAD_ITM_GRNT_BYTES_OUT_OF_RANGE_ERR, 11, exit);
  }
  if (info->guaranteed_size_bds != SOC_TMC_ITM_GUARANTEED_INFO_DO_NOT_SET) {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->guaranteed_size_bds, ARAD_ITM_GRNT_BDS_MAX, ARAD_ITM_GRNT_BDS_OUT_OF_RANGE_ERR, 12, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_committed_q_size_set_verify()", rt_cls_ndx, 0);
}

uint32
  arad_itm_committed_q_size_get_verify(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        rt_cls_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_COMMITTED_Q_SIZE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX, ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_committed_q_size_get_verify()", rt_cls_ndx, 0);
}


uint32
  arad_itm_committed_q_size_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  rt_cls_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_GUARANTEED_INFO *exact_info
  )
{
  uint32
      exp_man,
      data,
    mnt = 0,
    exp = 0;
  uint32 res;
   
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_COMMITTED_Q_SIZE_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1210, exit, READ_IQM_GRSPRMm(unit, MEM_BLOCK_ANY, rt_cls_ndx, &data));

  exp_man = soc_IQM_GRSPRMm_field32_get(unit, &data, GRNT_BYTES_THf);
  arad_iqm_mantissa_exponent_get(unit, exp_man, ARAD_ITM_COMMITTED_BYTES_MANTISSA_NOF_BITS, &mnt, &exp);
  exact_info->guaranteed_size[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES] = mnt * (ARAD_ITM_COMMITTED_BYTES_RESOLUTION << exp);
  
  exp_man = soc_IQM_GRSPRMm_field32_get(unit, &data, GRNT_BUFFS_THf);
  arad_iqm_mantissa_exponent_get(unit, exp_man, ARAD_ITM_COMMITTED_BDS_MANTISSA_NOF_BITS, &mnt, &exp);
  exact_info->guaranteed_size_bds = mnt << exp;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_committed_q_size_get_unsafe()", 0, 0);
}

soc_error_t
arad_itm_pfc_tc_map_set_unsafe(const int unit, const int tc_in, const int port_id, const int tc_out)
{
  int line_id;
  uint32 is_valid;
  uint32 nif_port;
  uint32 mem_val;
  int temp_tc_out = tc_out;
  SOCDNX_INIT_FUNC_DEFS;

  if(port_id < 0 || port_id >= ARAD_ITM_NOF_REASSEMBLY_CONTEXTS) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid port index %d, it should be between 0 to 191\n"),port_id));
  }
  if(tc_in <0 || tc_in > 7) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid tc_in index %d, it should be between 0 to 7\n"),tc_in));
  }
  if(tc_out <0 || tc_out > 7) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid tc_out %d, it should be between 0 to 7\n"),tc_out));
  }

  SOCDNX_IF_ERR_EXIT(READ_IQM_ITMPMm(unit, MEM_BLOCK_ANY, port_id, &mem_val));
  soc_mem_field_get(unit, IQM_ITMPMm, &mem_val, NIF_PORT_VALIDf, &is_valid);
  if(!is_valid) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid port %d\n"),port_id));
  }
  soc_mem_field_get(unit, IQM_ITMPMm, &mem_val, NIF_PORTf, &nif_port);
  line_id = (nif_port <<3) | tc_in;
  SOCDNX_IF_ERR_EXIT(WRITE_IQM_NIFTCMm(unit, MEM_BLOCK_ANY, line_id, &temp_tc_out));

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
arad_itm_pfc_tc_map_get_unsafe(const int unit, const int tc_in, const int port_id, int *tc_out)
{
  int line_id;
  uint32 is_valid;
  uint32 nif_port;
  uint32 mem_val;
  SOCDNX_INIT_FUNC_DEFS;

  if(port_id < 0 || port_id >= ARAD_ITM_NOF_REASSEMBLY_CONTEXTS) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid port index %d, it should be between 0 to 191\n"),port_id));
  }
  if(tc_in <0 || tc_in > 7) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid tc_in index %d, it should be between 0 to 7\n"),tc_in));
  }
  
  SOCDNX_IF_ERR_EXIT(READ_IQM_ITMPMm(unit, MEM_BLOCK_ANY, port_id, &mem_val));
  soc_mem_field_get(unit, IQM_ITMPMm, &mem_val, NIF_PORT_VALIDf, &is_valid);
  if(!is_valid) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid port %d\n"),port_id));
  }
  soc_mem_field_get(unit, IQM_ITMPMm, &mem_val, NIF_PORTf, &nif_port);
  line_id = (nif_port <<3) | tc_in;
  SOCDNX_IF_ERR_EXIT(READ_IQM_NIFTCMm(unit, MEM_BLOCK_ANY, line_id, tc_out));

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  arad_itm_dp_discard_set_verify(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        discard_dp
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_DISCARD_DP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(discard_dp, ARAD_ITM_DISCARD_DP_MAX, ARAD_ITM_DP_DISCARD_OUT_OF_RANGE_ERR, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_dp_discard_set_verify()", discard_dp, 0);
}


uint32
  arad_itm_enable_ecn_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint32   enabled 
  )
{
  uint32 res, enable_bit = enabled ? 1 : 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_IQM_ENABLERSr, SOC_CORE_ALL, 0, ECN_ENABLEf,  enable_bit)); 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_ECN_REMARKr, SOC_CORE_ALL, 0, ECN_REMARKf,  enable_bit)); 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_enable_ecn_unsafe()", unit, enabled);
}


uint32
  arad_itm_get_ecn_enabled_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint32   *enabled 
  )
{
  uint32 res, val;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(enabled);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit, ARAD_REG_ACCESS_ERR, READ_EPNI_ECN_REMARKr(unit, REG_PORT_ANY, &val));
  *enabled = soc_reg_field_get(unit, EPNI_ECN_REMARKr, val, ECN_REMARKf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_enable_ecn_unsafe()", unit, 0);
}


int
  arad_itm_dp_discard_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  discard_dp
  )
{     
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQM_IQM_ENABLERSr, SOC_CORE_ALL, 0, DSCRD_DPf,  discard_dp));

exit:
    SOCDNX_FUNC_RETURN;
}


int
  arad_itm_dp_discard_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT uint32                  *discard_dp
  )
{     
    uint32 fld_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IQM_IQM_ENABLERSr, SOC_CORE_ALL, 0, DSCRD_DPf, &fld_val));

    *discard_dp = fld_val;

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
  arad_itm_setup_dp_map(
    SOC_SAND_IN  int unit
  )
{
  uint32
    dp_cmnd_indx,
    iqm_dp_indx,
    mtr_res_indx,
    ingress_mapped_mtr_res_indx=0,
    egress_mapped_mtr_res_indx=0,
    entry_indx=0,
    iqm_dp,
    etm_de,
    tbl_data,
    egress_divisor,
	color_resolution_mode,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  color_resolution_mode = SOC_DPP_CONFIG(unit)->meter.color_resolution_mode;

  
  egress_divisor = SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system ? 2 : 1;
     
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit) && color_resolution_mode != SOC_PPC_MTR_COLOR_RESOLUTION_MODE_DEFAULT) {
    
    egress_divisor = 1;
  }
#endif 

  
  for (dp_cmnd_indx = 0; dp_cmnd_indx < SOC_PPC_NOF_MTR_RES_USES; ++dp_cmnd_indx)  {
    
    for (iqm_dp_indx = 0; iqm_dp_indx <= ARAD_PP_DP_MAX_VAL; ++iqm_dp_indx)  {
      
      for (mtr_res_indx = 0; mtr_res_indx <= ARAD_PP_DP_MAX_VAL; ++mtr_res_indx) {
        
        if (SOC_DPP_CONFIG(unit)->arad->init.pp_enable) {
          ingress_mapped_mtr_res_indx = egress_mapped_mtr_res_indx = mtr_res_indx;

          if (soc_property_suffix_num_get(unit, 0, spn_CUSTOM_FEATURE, "always_map_result_dp_2_to_1", 0) == 1) {
            if (mtr_res_indx == 2) {
              ingress_mapped_mtr_res_indx = egress_mapped_mtr_res_indx = 1;
            }
          }

          if (soc_property_suffix_num_get(unit, 0, spn_CUSTOM_FEATURE, "always_map_ingress_result_dp_2_to_1", 0) == 1) {
            if (mtr_res_indx == 2) {
              ingress_mapped_mtr_res_indx = 1;
            }
          }

          if (soc_property_suffix_num_get(unit, 0, spn_CUSTOM_FEATURE, "always_map_egress_result_dp_2_to_1", 0) == 1) {
            if (mtr_res_indx == 2) {
              egress_mapped_mtr_res_indx = 1;
            }
          }

#ifdef BCM_88660_A0
          if (SOC_IS_ARADPLUS(unit) && color_resolution_mode == SOC_PPC_MTR_COLOR_RESOLUTION_MODE_RED_DIFF){
			  
			  if (mtr_res_indx == 1) {
				  ingress_mapped_mtr_res_indx = egress_mapped_mtr_res_indx = 2;
			  }

			  
			  if (mtr_res_indx == 2) {
				  ingress_mapped_mtr_res_indx = egress_mapped_mtr_res_indx = 1;
			  }

			  
			  if (mtr_res_indx == 3) {
				  ingress_mapped_mtr_res_indx = egress_mapped_mtr_res_indx = 3;
			  }
		}		 
#endif 

          
          if (dp_cmnd_indx == SOC_PPC_MTR_RES_USE_NONE) {
             
    	      
            iqm_dp = iqm_dp_indx;
            etm_de = iqm_dp_indx/egress_divisor;
          } else if (dp_cmnd_indx == SOC_PPC_MTR_RES_USE_OW_DP) {
            
            iqm_dp = ingress_mapped_mtr_res_indx; 
            etm_de = iqm_dp_indx/egress_divisor; 
          } else if (dp_cmnd_indx == SOC_PPC_MTR_RES_USE_OW_DE) {
            
    	      iqm_dp = iqm_dp_indx; 
            etm_de = egress_mapped_mtr_res_indx/egress_divisor; 
          } else {
            
            
            iqm_dp = ingress_mapped_mtr_res_indx;
            etm_de = egress_mapped_mtr_res_indx/egress_divisor;
          }
        } else {
          
           
          iqm_dp = iqm_dp_indx;
          etm_de = iqm_dp_indx/egress_divisor;
        }

        res = READ_IDR_DROP_PRECEDENCE_MAPPINGm(unit, MEM_BLOCK_ANY, entry_indx, &tbl_data);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        soc_mem_field_set(unit, IDR_DROP_PRECEDENCE_MAPPINGm, &tbl_data, INGRESS_DROP_PRECEDENCEf, &iqm_dp);
        soc_mem_field_set(unit, IDR_DROP_PRECEDENCE_MAPPINGm, &tbl_data, EGRESS_DROP_PRECEDENCEf, &etm_de);

        res = WRITE_IDR_DROP_PRECEDENCE_MAPPINGm(unit, MEM_BLOCK_ANY, entry_indx, &tbl_data);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        
        ++entry_indx;
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_setup_dp_map()", 0, 0);
}


uint32
    arad_plus_itm_alpha_set_unsafe(
      SOC_SAND_IN  int       unit,
      SOC_SAND_IN  uint32       rt_cls_ndx,
      SOC_SAND_IN  uint32       drop_precedence_ndx,
      SOC_SAND_IN  int32        alpha 
    )
{
    uint32 data = 0, reg_val = 0, temp_val = 0;
    uint32 res;
    int need_workaround = (SOC_IS_ARADPLUS_A0(unit) || SOC_IS_ARDON(unit));
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    if (!SOC_IS_QAX(unit)) {
        if (alpha >= 0) {
            soc_mem_field32_set(unit, IQM_FADTTm, &data, EXP_VALf, alpha);
        } else {
            soc_mem_field32_set(unit, IQM_FADTTm, &data, EXP_VALf, -alpha);
            soc_mem_field32_set(unit, IQM_FADTTm, &data, EXP_SIGNf, 1);
        }

        if (need_workaround) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, READ_IQM_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, REG_PORT_ANY, &reg_val));
            temp_val = reg_val;
            soc_reg_field_set(unit, IQM_ENABLE_DYNAMIC_MEMORY_ACCESSr, &temp_val, ENABLE_DYNAMIC_MEMORY_ACCESSf, 1);
            if (reg_val == temp_val) {
                need_workaround = 0;
            } else {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, WRITE_IQM_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, REG_PORT_ANY, temp_val));
            }
        }

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, WRITE_IQM_FADTTm(unit, MEM_BLOCK_ANY, rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE + drop_precedence_ndx, &data));

        if (need_workaround) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, WRITE_IQM_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, REG_PORT_ANY, reg_val));
        }
    
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_plus_itm_alpha_set_unsafe()", rt_cls_ndx, drop_precedence_ndx);
}


uint32
    arad_plus_itm_alpha_get_unsafe(
      SOC_SAND_IN  int       unit,
      SOC_SAND_IN  uint32       rt_cls_ndx,
      SOC_SAND_IN  uint32       drop_precedence_ndx,
      SOC_SAND_OUT int32        *alpha 
    )
{
    uint32 data = 0, res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(alpha);
    if (!SOC_IS_QAX(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IQM_FADTTm(unit, MEM_BLOCK_ANY, rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE + drop_precedence_ndx, &data));
        res = soc_mem_field32_get(unit, IQM_FADTTm, &data, EXP_VALf);
        *alpha = soc_mem_field32_get(unit, IQM_FADTTm, &data, EXP_SIGNf) ? -res : res;
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_plus_itm_alpha_get_unsafe()", rt_cls_ndx, drop_precedence_ndx);
}




uint32
  arad_plus_itm_fair_adaptive_tail_drop_enable_set_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint8    enabled 
  )
{
    uint32 res, reg_val;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 150, exit, READ_IQM_FR_RSRC_DYN_TH_SETTINGSr(unit, REG_PORT_ANY, &reg_val));
    soc_reg_field_set(unit, IQM_FR_RSRC_DYN_TH_SETTINGSr, &reg_val, FR_RSRC_DYN_TH_ENABLEf, enabled ? 1 : 0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 160, exit, WRITE_IQM_FR_RSRC_DYN_TH_SETTINGSr(unit, REG_PORT_ANY, reg_val));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_fair_adaptive_tail_drop_enable_set_unsafe()", unit, enabled);
}


uint32
  arad_plus_itm_fair_adaptive_tail_drop_enable_get_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint8    *enabled 
  )
{
    uint32 res, reg_val;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(enabled);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 150, exit, READ_IQM_FR_RSRC_DYN_TH_SETTINGSr(unit, REG_PORT_ANY, &reg_val));
    *enabled = soc_reg_field_get(unit, IQM_FR_RSRC_DYN_TH_SETTINGSr, reg_val, FR_RSRC_DYN_TH_ENABLEf);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_fair_adaptive_tail_drop_enable_get_unsafe()", unit, 0);
}





int arad_itm_congestion_statistics_get(
  SOC_SAND_IN int unit,
  SOC_SAND_IN int core,
  SOC_SAND_OUT ARAD_ITM_CGM_CONGENSTION_STATS *stats 
  )
{
  uint32 fld_val;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(stats);
  if (!SOC_UNIT_NUM_VALID(unit)) {
      SOCDNX_IF_ERR_EXIT(SOC_E_UNIT);
  } 
  if (((core < 0) || (core > SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) && (core != SOC_CORE_ALL)) { 
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  }

    
  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IQM_OCCUPIED_UNICAST_DBUFFS_COUNTERr, REG_PORT_ANY, 0, OC_DB_COUNT_UCf, &fld_val));
  stats->bdb_free = fld_val; 

  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IQM_GLOBAL_RESOURCE_COUNTERS_BDr, REG_PORT_ANY, 0, OC_BD_COUNTf, &fld_val));
  stats->bd_occupied = fld_val; 	

  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IQM_GLOBAL_RESOURCE_COUNTERS_BD_2r, REG_PORT_ANY, 0, FR_BD_COUNTf, &fld_val));
  stats->bd2_free = fld_val; 		

  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IQM_OCCUPIED_UNICAST_DBUFFS_COUNTERr, REG_PORT_ANY, 0, OC_DB_COUNT_UCf, &fld_val));
  stats->db_uni_occupied = fld_val; 		

  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IQM_FREE_UNICAST_DBUFFS_COUNTERr, REG_PORT_ANY, 0, FR_DB_COUNT_UCf, &fld_val));
  stats->db_uni_free = fld_val; 		

  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IQM_FREE_FULL_MULTICAST_DBUFFS_COUNTERr, REG_PORT_ANY, 0, FR_DB_COUNT_FLMCf, &fld_val));
  stats->db_full_mul_free = fld_val; 	

  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IQM_FREE_MINI_MULTICAST_DBUFFS_COUNTERr, REG_PORT_ANY, 0, FR_DB_COUNT_MNMCf, &fld_val));
  stats->db_mini_mul_free = fld_val; 		

  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IQM_GLOBAL_RESOURCE_MINIMUM_OCCUPANCYr, REG_PORT_ANY, 0, FREE_BDB_MIN_OCf, &fld_val));
  stats->free_bdb_mini_occu = fld_val; 		

  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IQM_FREE_UNICAST_DBUFFS_MINIMUM_OCCUPANCYr, REG_PORT_ANY, 0, FR_DB_MIN_OCf, &fld_val));
  stats->free_db_uni_mini_occu = fld_val; 		
	
  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IQM_FREE_FULL_MULTICAST_DBUFFS_MINIMUM_OCCUPANCYr, REG_PORT_ANY, 0, FR_DB_FLMC_MIN_OCf, &fld_val));
  stats->free_bdb_full_mul_mini_occu = fld_val; 		

  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IQM_FREE_MINI_MULTICAST_DBUFFS_MINIMUM_OCCUPANCYr, REG_PORT_ANY, 0, FR_DB_MNMC_MIN_OCf, &fld_val));
  stats->free_bdb_mini_mul_mini_occu = fld_val; 		
  
exit:
  SOCDNX_FUNC_RETURN;
}
#undef _ERR_MSG_MODULE_NAME

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 
