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

#define _ERR_MSG_MODULE_NAME BSL_SOC_CNT



#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/mem.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/arad_cnt.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/QAX/qax_cnt.h>
#include <soc/dpp/JER/jer_cnt.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/JER/jer_mgmt.h>
#include <soc/dpp/QAX/qax_ingress_traffic_mgmt.h>
#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#endif

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif





#define ARAD_PROCESSOR_NDX_MAX(unit)                             (ARAD_CNT_NOF_PROCESSOR_IDS(unit)-1)
#define ARAD_CNT_PORT_NDX_MAX                                    (ARAD_PORT_NOF_PP_PORTS - 1)
#define ARAD_HDR_COMPENSATION_INGRESS_MIN                        (-31)
#define ARAD_HDR_COMPENSATION_INGRESS_MAX                        (31)
#define ARAD_HDR_COMPENSATION_EGRESS_MIN                         (-64)
#define ARAD_HDR_COMPENSATION_EGRESS_MAX                         (63)
#define ARAD_CNT_VOQ_PARAMS_Q_SET_SIZE_MAX                       (ARAD_CNT_NOF_Q_SET_SIZES-1)
#define ARAD_CNT_TYPE_MAX                                        (ARAD_CNT_NOF_MODE_EG_TYPES-1)
#define ARAD_CNT_COUNTERS_INFO_SRC_TYPE_MAX(unit)                (ARAD_CNT_NOF_SRC_TYPES(unit) - 1)
#define ARAD_CNT_COUNTERS_INFO_MODE_ING_MAX                      (ARAD_CNT_NOF_MODE_INGS-1)
#define ARAD_CNT_COUNTERS_INFO_MODE_EG_MAX                       (ARAD_CNT_NOF_MODE_EGS-1)
#define ARAD_CNT_BASE_VAL_MAX                                    (0)
#define ARAD_CNT_COUNTERS_INFO_STAG_MAX                          (15)
#define ARAD_CNT_RESULT_ARR_NOF_COUNTERS_MAX                     (16)


#define ARAD_CNT_MODE_STATISTICS_NO_COLOR_FLD_VAL                       (1)
#define ARAD_CNT_MODE_STATISTICS_COLOR_RES_LOW_FLD_VAL                  (3)
#define ARAD_CNT_MODE_STATISTICS_COLOR_RES_HI_FLD_VAL                   (4)
#define ARAD_CNT_MODE_STATISTICS_ONE_ENTRY_FLD_VAL                      (0)
#define ARAD_CNT_MODE_STATISTICS_COLOR_ENQ_HI_FLD_VAL                   (2)

#ifdef BCM_88660_A0
#define ARAD_CNT_MODE_STATISTICS_PKTS_AND_PKTS_FLD_VAL                  (5)
#endif

#ifdef BCM_88650_B0
#define ARAD_CNT_ONE_ENTRY_MODE_ALL_VAL                           (0)
#define ARAD_CNT_ONE_ENTRY_MODE_DROP_VAL                          (1)
#define ARAD_CNT_ONE_ENTRY_MODE_FWD_VAL                           (2)
#endif

#define ARAD_CNT_MODE_EG_TYPE_TM_FLD_VAL                           (1)
#define ARAD_CNT_MODE_EG_TYPE_OUTLIF_FLD_VAL                       (2)
#define ARAD_CNT_MODE_EG_TYPE_VSI_FLD_VAL                          (3)
#define ARAD_CNT_MODE_EG_TYPE_PMF_FLD_VAL                          (0)

#define ARAD_CNT_MODE_EG_TYPE_EGQ_PROFILE_TM_FLD_VAL               (5)
#define ARAD_CNT_MODE_EG_TYPE_EGQ_PROFILE_TM_PORT_FLD_VAL          (4)
#define ARAD_CNT_MODE_EG_TYPE_EGQ_PROFILE_VSI_FLD_VAL              (3)
#define ARAD_CNT_MODE_EG_TYPE_EGQ_PROFILE_OUTLIF_FLD_VAL           (2)
 

#define _LSB 0
#define _MSB 1





#define ARAD_CNT_FLD_VAL_TO_FORMAT(unit, fld_val, format)    \
  if (SOC_IS_JERICHO(unit) && (fld_val == 5)) {               \
      format = SOC_TMC_CNT_FORMAT_IPT_LATENCY;               \
  } else {                                                   \
      format = fld_val;                                      \
  }

#define ARAD_CNT_FORMAT_TO_FLD_VAL(unit, format, src, fld_val) \
  if (SOC_IS_JERICHO(unit)) {                                  \
      if (format == SOC_TMC_CNT_FORMAT_IPT_LATENCY) {          \
          fld_val = 5;                                         \
      } else if (format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS) { \
        fld_val = (src == ARAD_CNT_SRC_TYPE_INGRESS_OAM || src == ARAD_CNT_SRC_TYPE_EGRESS_OAM) ?             \
          SOC_TMC_CNT_FORMAT_IHB_COMMANDS : 1;                 \
      } else {                                                 \
          fld_val = format;                                    \
      }                                                        \
  } else {                                                     \
      fld_val = format;                                        \
  }


#define JER_CNT_PKT_CNT_NOF_BITS(format) ((format == SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES) ? 28 : (format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS) ? 32 : 64)
#define JER_CNT_PKT_CNT_SHIFT(format) 0
#define JER_CNT_BYTE_CNT_NOF_BITS(format) ((format == SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES) ? 36 : 64)
#define JER_CNT_BYTE_CNT_SHIFT(format) ((format == SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES) ? JER_CNT_PKT_CNT_NOF_BITS(format) : 0)

#define JER_CNT_PKT_CNT_MASK_HI(format) ((format == SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES || format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS) ? 0x0 : 0xffffffff)
#define JER_CNT_PKT_CNT_MASK_LO(format) ((format == SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES) ? 0xfffffff : 0xffffffff)

#define JER_CNT_BYTE_CNT_MASK_HI(format) ((format == SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES) ? 0xf : 0xffffffff)
#define JER_CNT_BYTE_CNT_MASK_LO(format) 0xffffffff


#define  _ING_PP_FLD_VAL_BASE(unit) (SOC_IS_JERICHO(unit) ? 0x4 : 0)
#define ARAD_CNT_SRC_TYPE_ING_PP_FLD_VAL(unit, src_core, command_id, msb) \
        (SOC_IS_JERICHO(unit) ? (_ING_PP_FLD_VAL_BASE(unit) + command_id + (2 * src_core)) : (_ING_PP_FLD_VAL_BASE(unit) + msb + (2 * command_id)))

#define ARAD_CNT_SRC_TYPE_IS_ING_PP_FLD(unit, fld_val) (_ING_PP_FLD_VAL_BASE(unit) <= fld_val && fld_val <= _ING_PP_FLD_VAL_BASE(unit) + 3)

#define ARAD_CNT_SRC_TYPE_ING_PP_FLD_CORE_GET(unit, fld_val) (SOC_IS_JERICHO(unit) ? (fld_val & 2) : 0)

#define ARAD_CNT_SRC_TYPE_ING_PP_FLD_CMD_GET(unit, fld_val) (SOC_IS_JERICHO(unit) ? (fld_val & 1) : (fld_val & 2))

#define ARAD_CNT_SRC_TYPE_ING_PP_FLD_LSB_GET(unit, fld_val) (SOC_IS_JERICHO(unit) ? -1 : (fld_val & 1))
 
#define  _IQM_FLD_VAL_BASE (SOC_IS_JERICHO(unit) ? 0 : 10)
#define ARAD_CNT_SRC_TYPE_IQM_FLD_VAL(unit, src_core, command_id, proc_id) \
        (SOC_IS_JERICHO(unit) ? (_IQM_FLD_VAL_BASE + command_id + (2 * src_core)) : (_IQM_FLD_VAL_BASE + proc_id))

#define ARAD_CNT_SRC_TYPE_IS_IQM_FLD(unit, fld_val) (_IQM_FLD_VAL_BASE <= fld_val && fld_val <= _IQM_FLD_VAL_BASE + 3)

#define ARAD_CNT_SRC_TYPE_IQM_FLD_CORE_GET(unit, fld_val) (SOC_IS_JERICHO(unit) ? (fld_val & 2) : 0)

#define ARAD_CNT_SRC_TYPE_IQM_FLD_CMD_GET(unit, fld_val) (SOC_IS_JERICHO(unit) ? (fld_val & 1) : 0)

#define ARAD_CNT_SRC_TYPE_IQM_FLD_PROC_ID_GET(unit, fld_val) (fld_val - _IQM_FLD_VAL_BASE)

#define  _EGR_PP_FLD_VAL_BASE (SOC_IS_JERICHO(unit) ? 8 : 4)
#define  _EGR_PP_FLD_VAL_NOF  (SOC_IS_JERICHO(unit) ? 4 : 2)
#define ARAD_CNT_SRC_TYPE_EGR_PP_FLD_VAL(unit, src_core, command_id, msb) \
        (SOC_IS_JERICHO(unit) ? (_EGR_PP_FLD_VAL_BASE + command_id + (2 * src_core)) : (_EGR_PP_FLD_VAL_BASE + msb))

#define ARAD_CNT_SRC_TYPE_IS_EGR_PP_FLD(unit, fld_val) (_EGR_PP_FLD_VAL_BASE <= fld_val && fld_val <= _EGR_PP_FLD_VAL_BASE + _EGR_PP_FLD_VAL_NOF - 1)

#define ARAD_CNT_SRC_TYPE_EGR_PP_FLD_CORE_GET(unit, fld_val) (SOC_IS_JERICHO(unit) ? (fld_val & 2) : 0)

#define ARAD_CNT_SRC_TYPE_EGR_PP_FLD_CMD_GET(unit, fld_val) (SOC_IS_JERICHO(unit) ? (fld_val & 1) : 0)

#define ARAD_CNT_SRC_TYPE_EGR_PP_FLD_LSB_GET(unit, fld_val) (SOC_IS_JERICHO(unit) ? -1 : (fld_val & 1))

#define _OAM_FLD_VAL_BASE (SOC_IS_JERICHO(unit) ? 0x10 : 14)
#define _OAM_FLD_VAL_NOF (SOC_IS_JERICHO(unit) ? 2 : 4)
#define ARAD_CNT_SRC_TYPE_OAM_FLD_VAL(unit, src_core, command_id) \
            (SOC_IS_JERICHO(unit) ? (_OAM_FLD_VAL_BASE + src_core) : (_OAM_FLD_VAL_BASE + command_id))

#define ARAD_CNT_SRC_TYPE_IS_OAM_FLD(unit, fld_val) (_OAM_FLD_VAL_BASE <= fld_val && fld_val <= _OAM_FLD_VAL_BASE + _OAM_FLD_VAL_NOF - 1)

#define ARAD_CNT_SRC_TYPE_OAM_FLD_CORE_GET(unit, fld_val) (SOC_IS_JERICHO(unit) ? (fld_val & 1) : 0)

#define ARAD_CNT_SRC_TYPE_OAM_FLD_CMD_GET(unit, fld_val) (SOC_IS_JERICHO(unit) ? 0 : (fld_val & 3))

#define _IPT_FLD_VAL_BASE (0x12)
#define ARAD_CNT_SRC_TYPE_IPT_FLD_VAL(unit, src_core) ((src_core & 1) + _IPT_FLD_VAL_BASE)

#define ARAD_CNT_SRC_TYPE_IS_IPT_FLD(unit, fld_val) (_IPT_FLD_VAL_BASE <= fld_val && fld_val <= _IPT_FLD_VAL_BASE + 1)

#define ARAD_CNT_SRC_TYPE_IPT_FLD_CORE_GET(unit, fld_val) (fld_val - _IPT_FLD_VAL_BASE)


#define _EGQ_TM_FLD_VAL_BASE (0x14)
#define ARAD_CNT_SRC_TYPE_EGQ_TM_FLD_VAL(unit, src_core)  ((src_core & 1) + _EGQ_TM_FLD_VAL_BASE)

#define ARAD_CNT_SRC_TYPE_IS_EGQ_TM_FLD(unit, fld_val) (_EGQ_TM_FLD_VAL_BASE <= fld_val && fld_val <= _EGQ_TM_FLD_VAL_BASE + 1)

#define ARAD_CNT_SRC_TYPE_EGQ_TM_FLD_CORE_GET(unit, fld_val) (fld_val - _EGQ_TM_FLD_VAL_BASE)

#define _EPNI_FLD_VAL_BASE (SOC_IS_JERICHO(unit) ? 0xc: 6)
#define ARAD_CNT_SRC_TYPE_EPNI_FLD_VAL(unit, src_core, command_id, msb) \
    (SOC_IS_JERICHO(unit) ? (_EPNI_FLD_VAL_BASE + command_id + (2 * src_core)) : (_EPNI_FLD_VAL_BASE + msb + (2 * command_id)))

#define ARAD_CNT_SRC_TYPE_IS_EPNI_FLD_VAL(unit, fld_val) (_EPNI_FLD_VAL_BASE <= fld_val && fld_val <= _EPNI_FLD_VAL_BASE + 3)

#define ARAD_CNT_SRC_TYPE_EPNI_FLD_CORE_GET(unit, fld_val) (SOC_IS_JERICHO(unit) ? (fld_val & 2) : 0)

#define ARAD_CNT_SRC_TYPE_EPNI_FLD_CMD_GET(unit, fld_val) (SOC_IS_JERICHO(unit) ? (fld_val & 1) : (fld_val & 2))

#define ARAD_CNT_SRC_TYPE_EPNI_FLD_LSB_GET(unit, fld_val) (SOC_IS_JERICHO(unit) ? -1 : (fld_val & 1))


#define ARAD_CNT_SRC_TYPE_CORE_GET(unit, fld_val, src_core)                 \
    if (ARAD_CNT_SRC_TYPE_IS_ING_PP_FLD(unit, fld_val)) {                   \
        src_core = ARAD_CNT_SRC_TYPE_ING_PP_FLD_CORE_GET(unit, fld_val);    \
    } else if (ARAD_CNT_SRC_TYPE_IS_EGR_PP_FLD(unit, fld_val)) {            \
        src_core = ARAD_CNT_SRC_TYPE_EGR_PP_FLD_CORE_GET(unit, fld_val);    \
    } else if (ARAD_CNT_SRC_TYPE_IS_OAM_FLD(unit, fld_val)) {               \
        src_core = ARAD_CNT_SRC_TYPE_OAM_FLD_CORE_GET(unit, fld_val);       \
    } else if (ARAD_CNT_SRC_TYPE_IS_IPT_FLD(unit, fld_val)) {               \
        src_core = ARAD_CNT_SRC_TYPE_IPT_FLD_CORE_GET(unit, fld_val);       \
    } else if (ARAD_CNT_SRC_TYPE_IS_EGQ_TM_FLD(unit,fld_val)) {             \
        src_core = ARAD_CNT_SRC_TYPE_EGQ_TM_FLD_CORE_GET(unit, fld_val);    \
    } else if (ARAD_CNT_SRC_TYPE_IS_EPNI_FLD_VAL(unit, fld_val)) {          \
        src_core = ARAD_CNT_SRC_TYPE_EPNI_FLD_CORE_GET(unit, fld_val);      \
    } else if (ARAD_CNT_SRC_TYPE_IS_IQM_FLD(unit, fld_val)) {               \
        src_core = ARAD_CNT_SRC_TYPE_IQM_FLD_CORE_GET(unit, fld_val);       \
    } else {                                                                \
        src_core = 0;                                                       \
    }
#define ARAD_CNT_SRC_TYPE_CMD_GET(unit, fld_val, command_id)                \
    if (ARAD_CNT_SRC_TYPE_IS_ING_PP_FLD(unit, fld_val)) {                   \
        command_id = ARAD_CNT_SRC_TYPE_ING_PP_FLD_CMD_GET(unit, fld_val);   \
    } else if (ARAD_CNT_SRC_TYPE_IS_EGR_PP_FLD(unit, fld_val)) {            \
        command_id = ARAD_CNT_SRC_TYPE_EGR_PP_FLD_CMD_GET(unit, fld_val);   \
    } else if (ARAD_CNT_SRC_TYPE_IS_OAM_FLD(unit, fld_val)) {               \
        command_id = ARAD_CNT_SRC_TYPE_OAM_FLD_CMD_GET(unit, fld_val);      \
    } else if (ARAD_CNT_SRC_TYPE_IS_EPNI_FLD_VAL(unit, fld_val)) {          \
        command_id = ARAD_CNT_SRC_TYPE_EPNI_FLD_CMD_GET(unit, fld_val);     \
    } else if (ARAD_CNT_SRC_TYPE_IS_IQM_FLD(unit,fld_val)) {                \
        command_id = ARAD_CNT_SRC_TYPE_IQM_FLD_CMD_GET(unit, fld_val);      \
    } else {                                                                \
        command_id = -1;                                                    \
    }

#define ARAD_CNT_SRC_TYPE_LSB_GET(unit, fld_val, lsb)                       \
    if (SOC_IS_JERICHO(unit)) {                                             \
        lsb = -1;                                                           \
    } else if (ARAD_CNT_SRC_TYPE_IS_ING_PP_FLD(unit, fld_val)) {            \
        lsb = ARAD_CNT_SRC_TYPE_ING_PP_FLD_LSB_GET(unit, fld_val);          \
    } else if (ARAD_CNT_SRC_TYPE_IS_EGR_PP_FLD(unit, fld_val)) {            \
        lsb = ARAD_CNT_SRC_TYPE_EGR_PP_FLD_LSB_GET(unit, fld_val);          \
    } else if (ARAD_CNT_SRC_TYPE_IS_EPNI_FLD_VAL(unit, fld_val)) {          \
        lsb = ARAD_CNT_SRC_TYPE_EPNI_FLD_LSB_GET(unit, fld_val);            \
    } else {                                                                \
        lsb = 0;                                                            \
    }



#define ARAD_CNT_PKT_TRESH(unit)     (SOC_IS_JERICHO(unit) ? (1 << 24) : (SOC_IS_ARDON(unit) ? (0x5803) : (0x5801)))

#define ARAD_CNT_OCT_TRESH(unit, field_val)                     \
            if (SOC_IS_JERICHO(unit)) {                         \
                COMPILER_64_SET(field_val ,1, 0);               \
            } else if (SOC_IS_ARDON(unit)) {                    \
                COMPILER_64_SET(field_val ,0, 0x7404);          \
            } else {                                            \
                COMPILER_64_SET(field_val ,0, 0x7401);          \
            }  

#define ARAD_EPNI_BASE_VAL_TM                                   (SOC_IS_JERICHO(unit) ? 0 : (0x8000))

#define ARAD_CNT_DO_NOT_COUNT_VAL                               (SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL)
#define ARAD_CNT_BMAP_OFFSET_COUNT                              (SOC_TMC_CNT_BMAP_OFFSET_COUNT)

  







typedef struct {
    int         reg_index;
    soc_reg_t   rd_processor_read_r;
    soc_field_t rd_req_type_f;
    soc_field_t rd_req_addr_f;
    soc_field_t rd_req_f;
} ARAD_CNT_RD_PROCESSOR_READ;

typedef struct {
    int         reg_index;
    soc_reg_t   rd_result_r;
    soc_field_t rd_data_valid_f;
    soc_field_t rd_pkt_cnt_f;
    soc_field_t rd_oct_cnt_f;
    soc_field_t rd_cnt_addr_f;
    soc_field_t rd_was_incremetal_f;
} ARAD_CNT_CRPS_DIRECT_RD_RESULT;

typedef struct {
    soc_field_t cache_rd_data_valid;
    soc_field_t cache_rd_pkt_cnt;
    soc_field_t cache_rd_oct_cnt;
    soc_field_t cache_rd_cnt_addr;
    soc_field_t cache_rd_engine_num;
    soc_field_t cache_rd_is_ecc_err;
} ARAD_CNT_CRPS_CACHE_RD_RESULT_FIELDS;








#define DMA_SRC_DEFAULT 0xf  
CONST soc_field_t sel[NOF_FIFO_DMA_CHANNELS] =  {FIFO_DMA_0_SELf, FIFO_DMA_1_SELf, FIFO_DMA_2_SELf, FIFO_DMA_3_SELf, 
                              FIFO_DMA_4_SELf, FIFO_DMA_5_SELf, FIFO_DMA_6_SELf, FIFO_DMA_7_SELf, 
                              FIFO_DMA_8_SELf, FIFO_DMA_9_SELf, FIFO_DMA_10_SELf, FIFO_DMA_11_SELf};




STATIC uint32 
  arad_cnt_fifo_dma_cache_length(
     SOC_SAND_IN  int           unit,
     SOC_SAND_IN  soc_port_t    fifo_dma_index, 
     SOC_SAND_OUT int           *cache_length
     );

uint32
    _arad_cnt_crps_pre_read_mem_get(int unit,int dma_ndx,SOC_SAND_OUT soc_mem_t *mem) 
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(mem);
    if (dma_ndx < 1) {
        *mem = CRPS_CRPS_0_TO_3_PRE_READ_MEMm;
    } else if (dma_ndx < 2) {
        *mem = CRPS_CRPS_4_TO_7_PRE_READ_MEMm;
    } else if (dma_ndx < 3) {
        *mem = CRPS_CRPS_8_TO_11_PRE_READ_MEMm;
    } else if (dma_ndx < 4) {
        *mem = CRPS_CRPS_12_TO_17_PRE_READ_MEMm;
    } else {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "FIFO DMA index %u out of range\n"), dma_ndx));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
exit:
  SOCDNX_FUNC_RETURN;
}
STATIC uint32
    _arad_cnt_crps_cache_rd_result_reg_get(int unit, int dma_ndx, soc_reg_t *reg) 
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(reg);
    if (SOC_IS_JERICHO(unit)) {
        if (dma_ndx == 0) {
            *reg = CRPS_CRPS_CACHE_RD_RESULTS_0_TO_3r;            
        } else if (dma_ndx == 1) {
            *reg = CRPS_CRPS_CACHE_RD_RESULTS_4_TO_7r;
        } else if (dma_ndx == 2) {
            *reg = CRPS_CRPS_CACHE_RD_RESULTS_8_TO_11r;
        } else if (dma_ndx < 4) {
            if(SOC_IS_QAX(unit))
            {
                *reg = CRPS_CRPS_CACHE_RD_RESULTS_12_TO_16r;
            }
            else
            {
                *reg = CRPS_CRPS_CACHE_RD_RESULTS_12_TO_17r;                
            }

        } else {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "FIFO DMA index %u out of range\n"), dma_ndx));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    } else {
      *reg = CRPS_CRPS_CACHE_RD_RESULTSr;
    }
exit:
  SOCDNX_FUNC_RETURN;
}

STATIC uint32
    _arad_cnt_rd_processor_read(int unit, ARAD_CNT_PROCESSOR_ID proc_id, SOC_SAND_OUT ARAD_CNT_RD_PROCESSOR_READ* rd_processor_read) {
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(rd_processor_read);

    rd_processor_read->rd_processor_read_r = -1;
    rd_processor_read->rd_req_f = -1;
    rd_processor_read->rd_req_addr_f = -1;
    rd_processor_read->rd_req_type_f = -1;
    
    if ((proc_id < SOC_DPP_DEFS_GET(unit, nof_counter_processors)) ||
        (SOC_IS_QAX(unit) && proc_id == SOC_DPP_DEFS_GET(unit, nof_counter_processors))) {
        rd_processor_read->reg_index = proc_id;
        rd_processor_read->rd_processor_read_r = CRPS_CNT_PROCESSOR_READr;
        rd_processor_read->rd_req_f = CRPS_N_RD_REQf;
        rd_processor_read->rd_req_addr_f = CRPS_N_RD_REQ_ADDRf;
        rd_processor_read->rd_req_type_f = CRPS_N_RD_REQ_TYPEf;
    } else if (SOC_IS_JERICHO(unit) && 
               proc_id < SOC_DPP_DEFS_GET(unit, nof_counter_processors) + SOC_DPP_DEFS_GET(unit, nof_small_counter_processors)) {
        rd_processor_read->reg_index = 0;
        if (proc_id == SOC_DPP_DEFS_GET(unit, nof_counter_processors)) {
            rd_processor_read->rd_processor_read_r = CRPS_CNT_PROCESSOR_READ_16r;
            rd_processor_read->rd_req_f = CRPS_16_RD_REQf;
            rd_processor_read->rd_req_addr_f = CRPS_16_RD_REQ_ADDRf;
            rd_processor_read->rd_req_type_f = CRPS_16_RD_REQ_TYPEf;
        } else {
            rd_processor_read->rd_processor_read_r = CRPS_CNT_PROCESSOR_READ_17r;
            rd_processor_read->rd_req_f = CRPS_17_RD_REQf;
            rd_processor_read->rd_req_addr_f = CRPS_17_RD_REQ_ADDRf;
            rd_processor_read->rd_req_type_f = CRPS_17_RD_REQ_TYPEf;
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid processor ID %d\n"), proc_id));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
exit:
  SOCDNX_FUNC_RETURN;
}

STATIC uint32
    _arad_cnt_crps_direct_rd_result_get(int unit, ARAD_CNT_PROCESSOR_ID proc_id, SOC_SAND_OUT ARAD_CNT_CRPS_DIRECT_RD_RESULT* rd_result) {
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(rd_result);
    rd_result->rd_result_r = -1;
    rd_result->rd_data_valid_f = -1;
    rd_result->rd_pkt_cnt_f = -1;
    rd_result->rd_oct_cnt_f = -1;
    rd_result->rd_was_incremetal_f = -1;
    rd_result->rd_cnt_addr_f = -1;
    if (SOC_IS_JERICHO(unit)) {
        if ((proc_id < SOC_DPP_DEFS_GET(unit, nof_counter_processors)) ||
            ((SOC_IS_QAX(unit) == TRUE) && (proc_id == SOC_DPP_DEFS_GET(unit, nof_counter_processors))))
        {
            rd_result->reg_index = proc_id;
            rd_result->rd_result_r = CRPS_CRPS_DIRECT_RD_RESULTr;
            rd_result->rd_data_valid_f = CRPS_N_DIRECT_RD_DATA_VALIDf;
            rd_result->rd_pkt_cnt_f = CRPS_N_DIRECT_RD_CNTf;
            rd_result->rd_oct_cnt_f = -1;
            rd_result->rd_was_incremetal_f = CRPS_N_DIRECT_RD_WAS_INCREMENTALf;
            rd_result->rd_cnt_addr_f = CRPS_N_DIRECT_RD_CNT_ADDRf;
        } else if (proc_id < SOC_DPP_DEFS_GET(unit, nof_counter_processors) + SOC_DPP_DEFS_GET(unit, nof_small_counter_processors)) {
            rd_result->reg_index = 0;
            if (proc_id == SOC_DPP_DEFS_GET(unit, nof_counter_processors)) {
                rd_result->rd_result_r = CRPS_CRPS_DIRECT_RD_RESULT_16r;
                rd_result->rd_data_valid_f = CRPS_16_DIRECT_RD_DATA_VALIDf;
                rd_result->rd_pkt_cnt_f = CRPS_16_DIRECT_RD_CNTf;
                rd_result->rd_oct_cnt_f = -1;
                rd_result->rd_was_incremetal_f = CRPS_16_DIRECT_RD_WAS_INCREMENTALf;
                rd_result->rd_cnt_addr_f = CRPS_16_DIRECT_RD_CNT_ADDRf;
            } else {
                rd_result->rd_result_r = CRPS_CRPS_DIRECT_RD_RESULT_17r;
                rd_result->rd_data_valid_f = CRPS_17_DIRECT_RD_DATA_VALIDf;
                rd_result->rd_pkt_cnt_f = CRPS_17_DIRECT_RD_CNTf;
                rd_result->rd_oct_cnt_f = -1;
                rd_result->rd_was_incremetal_f = CRPS_17_DIRECT_RD_WAS_INCREMENTALf;
                rd_result->rd_cnt_addr_f = CRPS_17_DIRECT_RD_CNT_ADDRf;
            }
        } else {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid processor ID %d\n"), proc_id));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    } else {
        rd_result->reg_index = proc_id;
        rd_result->rd_result_r = CRPS_CRPS_DIRECT_RD_RESULTr;
        rd_result->rd_data_valid_f = CRPS_N_DIRECT_RD_DATA_VALIDf;
        rd_result->rd_pkt_cnt_f = CRPS_N_DIRECT_RD_PKT_CNTf;
        rd_result->rd_oct_cnt_f = CRPS_N_DIRECT_RD_OCT_CNTf;
        rd_result->rd_was_incremetal_f = CRPS_N_DIRECT_RD_WAS_INCREMENTALf;
        rd_result->rd_cnt_addr_f = -1;
    }
exit:
  SOCDNX_FUNC_RETURN;
}

STATIC uint32
    _arad_cnt_crps_cache_rd_result_fields_get(int unit, soc_reg_t reg, SOC_SAND_OUT ARAD_CNT_CRPS_CACHE_RD_RESULT_FIELDS* rd_result_fields) {
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(rd_result_fields);
    rd_result_fields->cache_rd_data_valid = -1;
    rd_result_fields->cache_rd_pkt_cnt    = -1;
    rd_result_fields->cache_rd_oct_cnt    = -1;
    rd_result_fields->cache_rd_cnt_addr   = -1;
    rd_result_fields->cache_rd_engine_num = -1;
    rd_result_fields->cache_rd_is_ecc_err = -1;
    if (reg == CRPS_CRPS_CACHE_RD_RESULTSr) {
        rd_result_fields->cache_rd_data_valid = CRPS_N_CACHE_RD_DATA_VALIDf;
        rd_result_fields->cache_rd_pkt_cnt    = CRPS_N_CACHE_RD_PKT_CNTf   ;
        rd_result_fields->cache_rd_oct_cnt    = CRPS_N_CACHE_RD_OCT_CNTf   ;
        rd_result_fields->cache_rd_cnt_addr   = CRPS_N_CACHE_RD_CNT_ADDRf  ;
        rd_result_fields->cache_rd_engine_num = -1                         ;
        rd_result_fields->cache_rd_is_ecc_err = -1                         ;
    } else if (reg == CRPS_CRPS_CACHE_RD_RESULTS_0_TO_3r) {
        rd_result_fields->cache_rd_data_valid = CRPS_0_TO_3_CACHE_RD_DATA_VALIDf;
        rd_result_fields->cache_rd_pkt_cnt    = CRPS_0_TO_3_CACHE_RD_CNTf       ;
        rd_result_fields->cache_rd_oct_cnt    = -1                              ;
        rd_result_fields->cache_rd_cnt_addr   = CRPS_0_TO_3_CACHE_RD_CNT_ADDRf  ;
        rd_result_fields->cache_rd_engine_num = CRPS_0_TO_3_CACHE_RD_ENGINE_NUMf;
        rd_result_fields->cache_rd_is_ecc_err = CRPS_0_TO_3_CACHE_RD_IS_ECC_ERRf;
    } else if (reg == CRPS_CRPS_CACHE_RD_RESULTS_4_TO_7r) {
        rd_result_fields->cache_rd_data_valid = CRPS_4_TO_7_CACHE_RD_DATA_VALIDf;
        rd_result_fields->cache_rd_pkt_cnt    = CRPS_4_TO_7_CACHE_RD_CNTf       ;
        rd_result_fields->cache_rd_oct_cnt    = -1                              ;
        rd_result_fields->cache_rd_cnt_addr   = CRPS_4_TO_7_CACHE_RD_CNT_ADDRf  ;
        rd_result_fields->cache_rd_engine_num = CRPS_4_TO_7_CACHE_RD_ENGINE_NUMf;
        rd_result_fields->cache_rd_is_ecc_err = CRPS_4_TO_7_CACHE_RD_IS_ECC_ERRf;
    } else if (reg == CRPS_CRPS_CACHE_RD_RESULTS_8_TO_11r) {
        rd_result_fields->cache_rd_data_valid = CRPS_8_TO_11_CACHE_RD_DATA_VALIDf;
        rd_result_fields->cache_rd_pkt_cnt    = CRPS_8_TO_11_CACHE_RD_CNTf       ;
        rd_result_fields->cache_rd_oct_cnt    = -1                               ;
        rd_result_fields->cache_rd_cnt_addr   = CRPS_8_TO_11_CACHE_RD_CNT_ADDRf  ;
        rd_result_fields->cache_rd_engine_num = CRPS_8_TO_11_CACHE_RD_ENGINE_NUMf;
        rd_result_fields->cache_rd_is_ecc_err = CRPS_8_TO_11_CACHE_RD_IS_ECC_ERRf;
    } else if (reg == CRPS_CRPS_CACHE_RD_RESULTS_12_TO_16r) {
        rd_result_fields->cache_rd_data_valid = CRPS_12_TO_16_CACHE_RD_DATA_VALIDf;
        rd_result_fields->cache_rd_pkt_cnt    = CRPS_12_TO_16_CACHE_RD_CNTf       ;
        rd_result_fields->cache_rd_oct_cnt    = -1                                ;
        rd_result_fields->cache_rd_cnt_addr   = CRPS_12_TO_16_CACHE_RD_CNT_ADDRf  ;
        rd_result_fields->cache_rd_engine_num = CRPS_12_TO_16_CACHE_RD_ENGINE_NUMf;
        rd_result_fields->cache_rd_is_ecc_err = CRPS_12_TO_16_CACHE_RD_IS_ECC_ERRf;        
    } else if (reg == CRPS_CRPS_CACHE_RD_RESULTS_12_TO_17r) {
        rd_result_fields->cache_rd_data_valid = CRPS_12_TO_17_CACHE_RD_DATA_VALIDf;
        rd_result_fields->cache_rd_pkt_cnt    = CRPS_12_TO_17_CACHE_RD_CNTf       ;
        rd_result_fields->cache_rd_oct_cnt    = -1                                ;
        rd_result_fields->cache_rd_cnt_addr   = CRPS_12_TO_17_CACHE_RD_CNT_ADDRf  ;
        rd_result_fields->cache_rd_engine_num = CRPS_12_TO_17_CACHE_RD_ENGINE_NUMf;
        rd_result_fields->cache_rd_is_ecc_err = CRPS_12_TO_17_CACHE_RD_IS_ECC_ERRf;
    } else {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid reg ID %u\n"), reg));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
exit:
  SOCDNX_FUNC_RETURN;
}
#if !ARAD_CNT_USE_DMA
STATIC uint32
 _arad_cnt_crps_pre_rd_fifo_sts_1_get(int unit, int dma_ndx, SOC_SAND_OUT soc_reg_t* reg) {
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(reg);
    if (SOC_IS_JERICHO(unit)) {
        if (dma_ndx == 0) {
            *reg = CRPS_CRPS_0_TO_3_PRE_RD_FIFO_STS_1r;
        } else if (dma_ndx == 1) {
            *reg = CRPS_CRPS_4_TO_7_PRE_RD_FIFO_STS_1r;
        } else if (dma_ndx == 2) {
            *reg = CRPS_CRPS_8_TO_11_PRE_RD_FIFO_STS_1r;
        } else if (dma_ndx == 3) {
            *reg = CRPS_CRPS_12_TO_17_PRE_RD_FIFO_STS_1r;
        } else {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "FIFO DMA index %u out of range\n"), dma_ndx));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    } else {
        *reg = CRPS_CRPS_PRE_RD_FIFO_STS_1r;                       
    }
exit:
    SOCDNX_FUNC_RETURN;
}
#endif 



uint32
    arad_cnt_crps_iqm_cmd_get(int unit, int proc_id, int command_id, SOC_SAND_OUT ARAD_CNT_CRPS_IQM_CMD *crps_iqm_cmd)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(crps_iqm_cmd);
    
    if (SOC_IS_JERICHO(unit)) {
        if (command_id == 0) {
            crps_iqm_cmd->cfg_1_r = IQM_IQM_CNT_CMD_CONFIG_Ar;
            crps_iqm_cmd->cfg_2_r = IQM_IQM_CNT_CMD_CONFIG_Ar;
            crps_iqm_cmd->src_cfg_r = IQM_IQM_CNT_CMD_CONFIG_Ar;
            crps_iqm_cmd->src_type_f = IQM_CNT_CMD_SRC_TYPE_Af;
            crps_iqm_cmd->queue_shift_f = IQM_CNT_CMD_QUEUE_SHIFT_Af;
            crps_iqm_cmd->base_q_f = IQM_CNT_CMD_BASE_Q_Af;
            crps_iqm_cmd->top_q_f = IQM_CNT_CMD_TOP_Q_Af;
        } else if (command_id == 1) {
            crps_iqm_cmd->cfg_1_r = IQM_IQM_CNT_CMD_CONFIG_Br;
            crps_iqm_cmd->cfg_2_r = IQM_IQM_CNT_CMD_CONFIG_Br;
            crps_iqm_cmd->src_cfg_r = IQM_IQM_CNT_CMD_CONFIG_Br;
            crps_iqm_cmd->src_type_f = IQM_CNT_CMD_SRC_TYPE_Bf;
            crps_iqm_cmd->queue_shift_f = IQM_CNT_CMD_QUEUE_SHIFT_Bf;
            crps_iqm_cmd->base_q_f = IQM_CNT_CMD_BASE_Q_Bf;
            crps_iqm_cmd->top_q_f = IQM_CNT_CMD_TOP_Q_Bf;
        } else {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid command ID %d\n"), command_id));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    } else {
        crps_iqm_cmd->src_cfg_r = IQM_CNT_PROCESSOR_CONFIG_2r;
        if (proc_id == 0) {
            crps_iqm_cmd->cfg_1_r = IQM_CNT_COMMAN_CFG_1_Ar;
            crps_iqm_cmd->cfg_2_r = IQM_CNT_COMMAN_CFG_2_Ar;
            crps_iqm_cmd->src_type_f = CRPS_IQM_CMD_SRC_TYPE_Af;
            crps_iqm_cmd->queue_shift_f = CRPS_IQM_CMD_QUEUE_SHIFT_Af;
            crps_iqm_cmd->base_q_f = CRPS_IQM_CMD_BASE_Q_Af;
            crps_iqm_cmd->top_q_f = CRPS_IQM_CMD_TOP_Q_Af;
        } else if (proc_id == 1) {        
            crps_iqm_cmd->cfg_1_r = IQM_CNT_COMMAN_CFG_1_Br;
            crps_iqm_cmd->cfg_2_r = IQM_CNT_COMMAN_CFG_2_Br;
            crps_iqm_cmd->src_type_f = CRPS_IQM_CMD_SRC_TYPE_Bf;
            crps_iqm_cmd->queue_shift_f = CRPS_IQM_CMD_QUEUE_SHIFT_Bf;
            crps_iqm_cmd->base_q_f = CRPS_IQM_CMD_BASE_Q_Bf;
            crps_iqm_cmd->top_q_f = CRPS_IQM_CMD_TOP_Q_Bf;
        } else if (proc_id == 2) {
            crps_iqm_cmd->cfg_1_r = IQM_CNT_COMMAN_CFG_1_Cr;
            crps_iqm_cmd->cfg_2_r = IQM_CNT_COMMAN_CFG_2_Cr;
            crps_iqm_cmd->src_type_f = CRPS_IQM_CMD_SRC_TYPE_Cf;
            crps_iqm_cmd->queue_shift_f = CRPS_IQM_CMD_QUEUE_SHIFT_Cf;
            crps_iqm_cmd->base_q_f = CRPS_IQM_CMD_BASE_Q_Cf;
            crps_iqm_cmd->top_q_f = CRPS_IQM_CMD_TOP_Q_Cf;
        } else if (proc_id == 3) {
            crps_iqm_cmd->cfg_1_r = IQM_CNT_COMMAN_CFG_1_Dr;
            crps_iqm_cmd->cfg_2_r = IQM_CNT_COMMAN_CFG_2_Dr;
            crps_iqm_cmd->src_type_f = CRPS_IQM_CMD_SRC_TYPE_Df;
            crps_iqm_cmd->queue_shift_f = CRPS_IQM_CMD_QUEUE_SHIFT_Df;
            crps_iqm_cmd->base_q_f = CRPS_IQM_CMD_BASE_Q_Df;
            crps_iqm_cmd->top_q_f = CRPS_IQM_CMD_TOP_Q_Df;
        } else {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid counter engine ID %d\n"), proc_id));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }
exit:
  SOCDNX_FUNC_RETURN;
}


STATIC
uint32 arad_cnt_convert_enum_val2_queue_shift(
        SOC_SAND_IN ARAD_CNT_Q_SET_SIZE enum_q_set_size
        ) { 
switch (enum_q_set_size)
  {
    case ARAD_CNT_Q_SET_SIZE_1_Q:
        return 0;
        break;       
    case ARAD_CNT_Q_SET_SIZE_2_Q:
        return 1;
        break;
    case ARAD_CNT_Q_SET_SIZE_4_Q:
        return 2;
      break;
    case ARAD_CNT_Q_SET_SIZE_8_Q:
        return 3;
        break;
    default:
        return 0;
        break;
  }
}


uint32
  arad_cnt_init(
    SOC_SAND_IN  int                 unit
  )
{
  ARAD_CNT_PROCESSOR_ID
      cnt_proc_ndx;
  uint32
    res;
  uint64 
      field_val;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_INIT);

  

  

  
  
  if(ARAD_CNT_NOF_PROCESSOR_IDS(unit) > (SOC_DPP_DEFS_MAX(NOF_COUNTER_PROCESSORS) + SOC_DPP_DEFS_MAX(NOF_SMALL_COUNTER_PROCESSORS))) {
	  SOC_SAND_SET_ERROR_CODE(ARAD_CNT_NOF_COUNTERS_OUT_OF_RANGE_ERR, 300, exit);
  }

  for (cnt_proc_ndx = 0; cnt_proc_ndx < ARAD_CNT_NOF_PROCESSOR_IDS(unit); cnt_proc_ndx++) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 101 + cnt_proc_ndx, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_GENERAL_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_CACHE_READ_ENf, 0x0));
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 111 + cnt_proc_ndx, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_THRESHOLDS_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_PKT_CNT_THRESHf,  ARAD_CNT_PKT_TRESH(unit)));
      ARAD_CNT_OCT_TRESH(unit, field_val);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 121 + cnt_proc_ndx, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CRPS_CRPS_THRESHOLDS_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_OCT_CNT_THRESHf, field_val));

       
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1241 + cnt_proc_ndx, exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, CRPS_CRPS_PREFETCH_TIMER_PERIOD_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_PREFETCH_PERIOD_CFGf,  ARAD_CNT_PREFETCH_TMR_CFG_MS)); 
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1201 + cnt_proc_ndx, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_PREFETCH_TIMERS_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_ALGORITHM_TMR_ENf, 0x0));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1211 + cnt_proc_ndx, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_PREFETCH_TIMERS_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_ALGORITHM_TMR_CFGf,  ARAD_CNT_PREFETCH_TMR_CFG_ALGO));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1221 + cnt_proc_ndx, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_PREFETCH_TIMERS_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_SEQUENTIAL_TMR_ENf, 0x0));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1231 + cnt_proc_ndx, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_PREFETCH_TIMERS_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_SEQUENTIAL_TMR_CFGf,  SOC_DPP_CONFIG(unit)->tm.crps_config.sequentialTimerInterval[cnt_proc_ndx]));

      if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1241 + cnt_proc_ndx, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_PREFETCH_TIMERS_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_OPPORTUNISTIC_TMR_ENf, 0x0));
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1251 + cnt_proc_ndx, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_PREFETCH_TIMERS_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_OPPORTUNISTIC_TMR_CFGf, 0x3));
      }
      
      if (SOC_IS_ARADPLUS(unit)) {
           
           SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1261 + cnt_proc_ndx, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_GENERAL_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_ACT_CNT_VALIDATE_ENf, 0x1));
      }
      if(SOC_IS_QAX(unit) && (cnt_proc_ndx < (SOC_DPP_DEFS_MAX(NOF_COUNTER_PROCESSORS))))
      {
        
        res = WRITE_CRPS_CRPS_SRC_MASKr(unit, cnt_proc_ndx,0);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1280, exit); 
      }
  }


  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
  
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 131, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_COUNTER_OUT_LIF_RANGE_0r, SOC_CORE_ALL, 0, COUNTER_OUT_LIF_RANGE_0_LOWf, 0));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 132, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_COUNTER_OUT_LIF_RANGE_0r, SOC_CORE_ALL, 0, COUNTER_OUT_LIF_RANGE_0_HIGHf, 0xFFFF));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 133, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_COUNTER_OUT_LIF_RANGE_1r, SOC_CORE_ALL, 0, COUNTER_OUT_LIF_RANGE_1_LOWf, 0));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 134, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_COUNTER_OUT_LIF_RANGE_1r, SOC_CORE_ALL, 0, COUNTER_OUT_LIF_RANGE_1_HIGHf, 0xFFFF));
  }
  
  if(!SOC_IS_QAX(unit))
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  135,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_IQM_ENABLERSr, SOC_CORE_ALL, 0, CRPS_CMD_ENf,  0x1));    
  }

  
  if (!SOC_IS_JERICHO(unit)) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_CNTPROCESSOR_CONFIG_1r, SOC_CORE_ALL, 0, CRPS_IQM_ACT_ON_QNVALIDf, 0x0));
  }  
  if (SOC_IS_JERICHO(unit)) {
     
     if(SOC_IS_QAX(unit))
     {
         res = WRITE_CRPS_CRPS_CNT_SRC_MODEr(unit, 0x1fff);
         SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 150, exit);             
     }
     else
     {
         res = WRITE_CRPS_CRPS_CNT_SRC_MODEr(unit, 0x3fffff);
         SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 151, exit);        
     }
  }

  if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 140, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQMT_IQMT_INITr, REG_PORT_ANY, 0, SIF_INITf, 0x0));
  }

    
    if(SOC_IS_QAX(unit))
    {
        
        
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1262,  exit, ARAD_REG_ACCESS_ERR,
          soc_reg_above_64_field32_modify(unit, CGM_IRPP_CTR_PTR_SETTINGSr, SOC_CORE_ALL, 0, IRPP_CTR_A_MC_ONCEf, 
            (SOC_DPP_CONFIG(unit)->tm.crps_config.ingress_replicate_packets_all == FALSE)));        
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1263,  exit, ARAD_REG_ACCESS_ERR,
          soc_reg_above_64_field32_modify(unit, CGM_IRPP_CTR_PTR_SETTINGSr, SOC_CORE_ALL, 0, IRPP_CTR_B_MC_ONCEf, 
            (SOC_DPP_CONFIG(unit)->tm.crps_config.ingress_replicate_packets_all == FALSE)));                        
    }
    else
    {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1260,  exit, ARAD_REG_ACCESS_ERR,
          soc_reg_above_64_field32_modify(unit, IRR_STATIC_CONFIGURATIONr, SOC_CORE_ALL, 0, COUNT_ALL_COPIESf, 
                        (SOC_DPP_CONFIG(unit)->tm.crps_config.ingress_replicate_packets_all == TRUE)));
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_cnt_init()", 0, 0);
}


uint32
ARAD_CNT_MODE_EG_verify(
  SOC_SAND_IN  ARAD_CNT_MODE_EG *info
   )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_ERR_IF_ABOVE_MAX(info->base_val, ARAD_CNT_BASE_VAL_MAX, ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_CNT_MODE_EG_verify()",0,0);
}
uint32
    arad_cnt_max_we_val_get( 
       SOC_SAND_IN  int                             unit,
       SOC_SAND_IN  ARAD_CNT_SRC_TYPE               src_type,
       SOC_SAND_OUT SOC_TMC_CNT_COUNTER_WE_BITMAP   *max_we_val)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(max_we_val);
    if (SOC_IS_JERICHO(unit)) {
        *max_we_val = (0x1 << soc_reg_field_length(unit, CRPS_CRPS_CNT_SRC_CFGr, CRPS_N_CNT_BIT_NUMf)) - 1;
    } else {
        switch(src_type) {
        case ARAD_CNT_SRC_TYPE_VOQ:
  		case ARAD_CNT_SRC_TYPE_INGRESS_EXT_STAT:			
        case ARAD_CNT_SRC_TYPE_STAG:
        case ARAD_CNT_SRC_TYPE_VSQ:
        case ARAD_CNT_SRC_TYPE_CNM_ID:
            *max_we_val = 0;
            break; 
        case ARAD_CNT_SRC_TYPE_ING_PP:      
        case ARAD_CNT_SRC_TYPE_EGR_PP:
        case ARAD_CNT_SRC_TYPE_INGRESS_OAM:
        case ARAD_CNT_SRC_TYPE_EGRESS_OAM:
        case ARAD_CNT_SRC_TYPE_EPNI:
            *max_we_val = 1;
            break;
        
        case ARAD_CNT_SRC_TYPES_EGQ_TM:
        case ARAD_CNT_SRC_TYPES_IPT_LATENCY:
        default:
          SOCDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }
    }
exit:
  SOCDNX_FUNC_RETURN;
}

STATIC
  uint32
    arad_cnt_get_source_id(
      SOC_SAND_IN  int               unit,
      SOC_SAND_IN  ARAD_CNT_SRC_TYPE src_type,
      SOC_SAND_IN  uint32       proc_id,
      SOC_SAND_IN  int          src_core,
      SOC_SAND_IN  int          command_id,
      SOC_TMC_CNT_COUNTER_WE_BITMAP we_val,
      SOC_SAND_OUT uint32       *src
    )
{
  uint32
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_GET_SOURCE_ID);
  
  switch(src_type)
  {
  case ARAD_CNT_SRC_TYPE_ING_PP:      
      fld_val = ARAD_CNT_SRC_TYPE_ING_PP_FLD_VAL(unit, src_core, command_id, we_val);
      break;
  case ARAD_CNT_SRC_TYPE_VOQ:
  case ARAD_CNT_SRC_TYPE_STAG:
  case ARAD_CNT_SRC_TYPE_VSQ:
  case ARAD_CNT_SRC_TYPE_CNM_ID:
  case ARAD_CNT_SRC_TYPE_INGRESS_EXT_STAT:
      fld_val = ARAD_CNT_SRC_TYPE_IQM_FLD_VAL(unit, src_core, command_id, proc_id);
      break; 
  case ARAD_CNT_SRC_TYPE_EGR_PP:
      fld_val = ARAD_CNT_SRC_TYPE_EGR_PP_FLD_VAL(unit, src_core, command_id, we_val);
      break;
  case ARAD_CNT_SRC_TYPE_INGRESS_OAM:
  case ARAD_CNT_SRC_TYPE_EGRESS_OAM:
    fld_val = ARAD_CNT_SRC_TYPE_OAM_FLD_VAL(unit, src_core, (command_id * 2) + we_val);
    break;
  case ARAD_CNT_SRC_TYPE_EPNI:
      fld_val = ARAD_CNT_SRC_TYPE_EPNI_FLD_VAL(unit, src_core, command_id, we_val);
      break;
  case ARAD_CNT_SRC_TYPES_EGQ_TM:
      fld_val = ARAD_CNT_SRC_TYPE_EGQ_TM_FLD_VAL(unit,src_core);
      break;
  case ARAD_CNT_SRC_TYPES_IPT_LATENCY:
      fld_val = ARAD_CNT_SRC_TYPE_IPT_FLD_VAL(unit,src_core);
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 23, exit);
  }

  *src = fld_val;

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("arad_cnt_get_source_id()", 0, 0);

}
  

  uint32
    arad_cnt_get_processor_id(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID processor_ndx,
      SOC_SAND_OUT uint32               *proc_id
    )

{
  uint32
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_GET_PROCESSOR_ID);

  switch (processor_ndx)
  {
      
  case ARAD_CNT_PROCESSOR_ID_A:
  case ARAD_CNT_PROCESSOR_ID_B:
  case ARAD_CNT_PROCESSOR_ID_C:
  case ARAD_CNT_PROCESSOR_ID_D:
      fld_val = processor_ndx;
      break;
  case ARAD_CNT_PROCESSOR_ID_E:
  case ARAD_CNT_PROCESSOR_ID_F:
  case ARAD_CNT_PROCESSOR_ID_G:
  case ARAD_CNT_PROCESSOR_ID_H:
  case ARAD_CNT_PROCESSOR_ID_I:
  case ARAD_CNT_PROCESSOR_ID_J:
  case ARAD_CNT_PROCESSOR_ID_K:
  case ARAD_CNT_PROCESSOR_ID_L:
  case ARAD_CNT_PROCESSOR_ID_M:
  case ARAD_CNT_PROCESSOR_ID_N:
  case ARAD_CNT_PROCESSOR_ID_O:
  case ARAD_CNT_PROCESSOR_ID_P:
  case ARAD_CNT_PROCESSOR_ID_Q:
  case ARAD_CNT_PROCESSOR_ID_R:
      if (SOC_IS_JERICHO(unit)) {
          fld_val = processor_ndx;
          break;
      }
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 30, exit);
  }

  *proc_id = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in processor_id_get_unsafe()", 0, 0);

}


uint32
  arad_cnt_ingress_params_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO *info,
    SOC_SAND_OUT uint32           *group_size,
    SOC_SAND_OUT uint32           *nof_counters,
    SOC_SAND_OUT uint32           *one_entry_mode_cnt
  )
{

  uint32
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
 
  
  if (SOC_IS_JERICHO(unit)) {
      *group_size = info->custom_mode_params.set_size;
      *nof_counters = info->custom_mode_params.set_size;
      *one_entry_mode_cnt = -1;
  } else {
      *one_entry_mode_cnt = ARAD_CNT_ONE_ENTRY_MODE_ALL_VAL;
      switch(info->mode_statistics)
      {
      case ARAD_CNT_MODE_STATISTICS_NO_COLOR:
        fld_val = ARAD_CNT_MODE_STATISTICS_NO_COLOR_FLD_VAL;
        *nof_counters = 2;
#ifdef BCM_88660_A0
        if (info->format == ARAD_CNT_FORMAT_PKTS_AND_PKTS){
            fld_val = ARAD_CNT_MODE_STATISTICS_PKTS_AND_PKTS_FLD_VAL;
            *nof_counters = 1;
        } 
#endif
        break;
      
      case ARAD_CNT_MODE_STATISTICS_COLOR_RES_LOW:
        fld_val = ARAD_CNT_MODE_STATISTICS_COLOR_RES_LOW_FLD_VAL;
        *nof_counters = 4;
        break;
       
      case ARAD_CNT_MODE_STATISTICS_COLOR_RES_HI:
        fld_val = ARAD_CNT_MODE_STATISTICS_COLOR_RES_HI_FLD_VAL;
        *nof_counters = 5;
        break;

      case ARAD_CNT_MODE_STATISTICS_ALL_NO_COLOR:
        fld_val = ARAD_CNT_MODE_STATISTICS_ONE_ENTRY_FLD_VAL;
        *nof_counters = 1;
#ifdef BCM_88650_B0
        *one_entry_mode_cnt = ARAD_CNT_ONE_ENTRY_MODE_ALL_VAL;
#endif
        break;

#ifdef BCM_88650_B0
      case ARAD_CNT_MODE_STATISTICS_DROP_NO_COLOR:
        fld_val = ARAD_CNT_MODE_STATISTICS_ONE_ENTRY_FLD_VAL;
        *nof_counters = 1;
        *one_entry_mode_cnt = ARAD_CNT_ONE_ENTRY_MODE_DROP_VAL;
        break;

      case ARAD_CNT_MODE_STATISTICS_FWD_NO_COLOR:
        fld_val = ARAD_CNT_MODE_STATISTICS_ONE_ENTRY_FLD_VAL;
        *nof_counters = 1;
        *one_entry_mode_cnt = ARAD_CNT_ONE_ENTRY_MODE_FWD_VAL;
        break;
#endif
      case ARAD_CNT_MODE_STATISTICS_COLOR_ENQ_HI:
        fld_val = ARAD_CNT_MODE_STATISTICS_COLOR_ENQ_HI_FLD_VAL;
        *nof_counters = 2;
#ifdef BCM_88660_A0
        *one_entry_mode_cnt = ARAD_CNT_ONE_ENTRY_MODE_ALL_VAL;
#endif
        break;
#ifdef BCM_88660_A0
      case ARAD_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS:
        fld_val = info->custom_mode_params.set_size - 1;
        *nof_counters = info->custom_mode_params.set_size;
        break;
    
      case ARAD_CNT_MODE_STATISTICS_FWD_SIMPLE_COLOR:
        fld_val = ARAD_CNT_MODE_STATISTICS_COLOR_ENQ_HI_FLD_VAL;
        *nof_counters = 2;
        *one_entry_mode_cnt = ARAD_CNT_ONE_ENTRY_MODE_FWD_VAL;
        break;
      case ARAD_CNT_MODE_STATISTICS_DROP_SIMPLE_COLOR:
        fld_val = ARAD_CNT_MODE_STATISTICS_COLOR_ENQ_HI_FLD_VAL;
        *nof_counters = 2;
        *one_entry_mode_cnt = ARAD_CNT_ONE_ENTRY_MODE_DROP_VAL;
        break;
#endif
      default:
        SOC_SAND_SET_ERROR_CODE(ARAD_CNT_ING_MODE_OUT_OF_RANGE_ERR, 10, exit);
      }
      *group_size = fld_val;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_ingress_params_get()", 0, 0);

}


STATIC
  uint32
    arad_cnt_id_compute(
      SOC_SAND_IN int             unit,
      SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO *counter_info,
      SOC_SAND_IN  uint32             counter_ndx,
      SOC_SAND_OUT uint32            *real_counter_id
      )
{
  uint32 nof_entries, group_size = 0, one_entry_cmd = 0;
  uint32 res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_cnt_ingress_params_get(unit, counter_info, &group_size, &nof_entries, &one_entry_cmd);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, fail);

  *real_counter_id = counter_ndx / nof_entries;

  ARAD_DO_NOTHING_AND_EXIT;

fail:
  *real_counter_id = 0;
  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_id_compute()", 0, 0);
}


uint32
  arad_cnt_channel_to_fifo_mapping_set(
     SOC_SAND_IN  int                             unit,
     SOC_SAND_IN  uint8                           channel,
     SOC_SAND_IN  uint8                           crps_fifo) {
    int rv = SOC_E_NONE;
    dma_fifo_channel_src_t selected_applicaion;
    SOCDNX_INIT_FUNC_DEFS;

    if (channel > NOF_FIFO_DMA_CHANNELS) {
        SOCDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }

    if (SOC_IS_JERICHO(unit)) {
      
        if (crps_fifo <= 0x3) {
            selected_applicaion = (dma_fifo_channel_src_t)crps_fifo;
        } else if (crps_fifo == SOC_TMC_CNT_INVALID_FIFO) {
            selected_applicaion = dma_fifo_channel_src_reserved; 
        } else {
            SOCDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }
        rv = jer_mgmt_dma_fifo_channel_set(unit, channel, selected_applicaion);
        SOCDNX_IF_ERR_EXIT(rv);
    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        if (crps_fifo != channel && crps_fifo != SOC_TMC_CNT_INVALID_FIFO) {
            
            SOCDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }
    } else {
        SOCDNX_IF_ERR_EXIT(BCM_E_UNAVAIL);
    }
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_cnt_channel_to_fifo_mapping_get(
     SOC_SAND_IN  int                             unit,
     SOC_SAND_IN  uint8                           channel,
     SOC_SAND_OUT uint8*                          crps_fifo) 
{
    int rv = SOC_E_NONE;
    uint64 fifo_dma_sel_data;
    uint32 selected_applicaion;
    SOCDNX_INIT_FUNC_DEFS;
    if (channel >= NOF_FIFO_DMA_CHANNELS) {
        SOCDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }
    SOCDNX_NULL_CHECK(crps_fifo);
    if (SOC_IS_JERICHO(unit)) {
        
        rv = READ_ECI_FIFO_DMA_SELr(unit, &fifo_dma_sel_data);
        SOCDNX_IF_ERR_EXIT(rv);

        selected_applicaion = soc_reg64_field32_get(unit, ECI_FIFO_DMA_SELr, fifo_dma_sel_data, sel[channel]);
        if (selected_applicaion <= 0x3) {
            
            *crps_fifo = selected_applicaion;
        } else if (selected_applicaion <= 0x7) {
            
            *crps_fifo = SOC_TMC_CNT_RESERVED_DMA_CHANNEL;
        } else {
            *crps_fifo = SOC_TMC_CNT_INVALID_FIFO;
        }
    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        
        *crps_fifo = channel;
    } else {
        SOCDNX_IF_ERR_EXIT(BCM_E_UNAVAIL);
    }
exit:
    SOCDNX_FUNC_RETURN;
}
         
uint32
  arad_cnt_dma_set_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  int                         dma_ndx,
    SOC_SAND_IN  uint8                       channel,
    SOC_SAND_IN  int                         cache_length
  )
{
#if ARAD_CNT_USE_DMA
    
    int cmc, ch;
    int entry_words;
    uint32 start_addr;
    int     blk;
    uint8   at;
    
    soc_error_t rv = SOC_E_NONE;
    uint32     *host_buff, buff_index; 
    soc_reg_t 
        reg_crps_cache_rd_results;
    uint32 reg_val;
    int dont, care, endian; 
#endif 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_DMA_SET_UNSAFE);

if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
    cmc = channel / NOF_DMA_FIFO_PER_CMC;
    ch = channel % NOF_DMA_FIFO_PER_CMC;    
} else {
    cmc = SOC_PCI_CMC(unit);
    ch = channel % NOF_DMA_FIFO_PER_CMC;  
} 

#if ARAD_CNT_USE_DMA
if (!SAL_BOOT_PLISIM ) {

  if (!SOC_UNIT_NUM_VALID(unit)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 3, exit);
  }
  if (dma_ndx < 0 || dma_ndx >= SOC_DPP_DEFS_GET(unit, nof_counter_fifos)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 4, exit);
  }
  if (channel > NOF_FIFO_DMA_CHANNELS) {
      SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 5, exit);
  } 
 
  arad_sw_db_cnt_buff_and_index_get(unit, dma_ndx, &host_buff, &buff_index);
  if (host_buff != NULL) {
      SOC_EXIT; 
  }
  rv = _arad_cnt_crps_cache_rd_result_reg_get(unit, dma_ndx, &reg_crps_cache_rd_results);
  if (SOC_FAILURE(rv)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 10, exit);   
  }
  entry_words = BYTES2WORDS(soc_reg_bytes((unit), reg_crps_cache_rd_results)); 
 
  host_buff = soc_cm_salloc((unit), cache_length * entry_words * sizeof(uint32),"Counters DMA Buffer"); 
                           
  if (host_buff == NULL) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 52, exit);   
  }
  sal_memset(host_buff, 0, cache_length * entry_words * sizeof(uint32));
  
  arad_sw_db_cnt_buff_and_index_set((unit), dma_ndx, host_buff, 0);

  rv = _soc_mem_sbus_fifo_dma_stop((unit), channel);
  if (SOC_FAILURE(rv)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 53, exit);   
  }

  rv = _soc_mem_sbus_fifo_dma_start_memreg((unit), channel, FALSE , 0, reg_crps_cache_rd_results, MEM_BLOCK_ANY, 0, ARAD_CNT_CACHE_LENGTH(unit), host_buff);
  if (SOC_FAILURE(rv)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 54, exit);   
  }
  
  start_addr = soc_reg_addr_get(unit, reg_crps_cache_rd_results, REG_PORT_ANY, 0, SOC_REG_ADDR_OPTION_NONE, &blk, &at);
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      
      start_addr = start_addr + dma_ndx * entry_words;
  }
  soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_SBUS_START_ADDRESS_OFFSET(cmc, ch), start_addr);

  
  reg_val = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch));
  soc_endian_get(unit, &dont,&care,&endian);
  soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &reg_val, ENDIANESSf, endian != 0);
  soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch), reg_val);

  
  soc_cmicm_cmcx_intr0_disable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));

}

#endif 
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_dma_set()", 0, 0);

}

uint32
  arad_cnt_dma_unset_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  int                         dma_ndx,
    SOC_SAND_IN  uint8                       channel
    )
{
#if ARAD_CNT_USE_DMA 
  uint32 *host_buff=NULL, buff_index; 
  soc_error_t rv = SOC_E_NONE; 
#endif  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_DMA_UNSET_UNSAFE); 
  if (dma_ndx < 0 || dma_ndx >= 4) {
      SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 5, exit);
  }

#if ARAD_CNT_USE_DMA 
  if (!SAL_BOOT_PLISIM ) { 
    rv = _soc_mem_sbus_fifo_dma_stop(unit, channel); 
    if (SOC_FAILURE(rv)) { 
      SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 10, exit); 
    } 
#ifdef BCM_CMICM_SUPPORT
    rv = _soc_mem_address_remap_dealloc(unit, (channel/NOF_DMA_FIFO_PER_CMC), (channel%NOF_DMA_FIFO_PER_CMC));
    if (SOC_FAILURE(rv)) { 
      SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 11, exit); 
    } 
#endif       
    arad_sw_db_cnt_buff_and_index_get(unit, dma_ndx, &host_buff, &buff_index); 
    if (host_buff != NULL) { 
      soc_cm_sfree(unit, host_buff); 
      arad_sw_db_cnt_buff_and_index_set(unit, dma_ndx, NULL, 0); 
    }
  }
#endif 

ARAD_DO_NOTHING_AND_EXIT; 
exit: 
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_dma_unset()", 0, 0); 
}
#define _BMAP_FIELDS(BLOCK, CORE, CMD_ID)                                \
        BLOCK ## _ ## CORE ## CMD_ID ## _GREEN_ADMITf,                   \
        BLOCK ## _ ## CORE ## CMD_ID ## _GREEN_DISCARDf,                 \
        BLOCK ## _ ## CORE ## CMD_ID ## _YELLOW_1_ADMITf,                \
        BLOCK ## _ ## CORE ## CMD_ID ## _YELLOW_1_DISCARDf,              \
        BLOCK ## _ ## CORE ## CMD_ID ## _YELLOW_2_ADMITf,                \
        BLOCK ## _ ## CORE ## CMD_ID ## _YELLOW_2_DISCARDf,              \
        BLOCK ## _ ## CORE ## CMD_ID ## _RED_ADMITf,                     \
        BLOCK ## _ ## CORE ## CMD_ID ## _RED_DISCARDf                    \


uint32
  _jer_cnt_counter_bmap_mem_by_src_type_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                proc_id,
    SOC_SAND_IN  int                   src_core,
    SOC_SAND_IN  ARAD_CNT_SRC_TYPE     src_type,
    SOC_SAND_IN  int                   command_id,
    SOC_SAND_OUT soc_reg_t             *reg, 
    SOC_SAND_OUT soc_field_t           *fields)
{
    soc_reg_t
        iqm_counter_bmap_reg[] =  {CRPS_IQM_0_OFFSET_BMAP_Ar, CRPS_IQM_0_OFFSET_BMAP_Br, CRPS_IQM_1_OFFSET_BMAP_Ar, CRPS_IQM_1_OFFSET_BMAP_Br},
        egq_counter_bmap_reg[] =  {CRPS_EGQ_0_OFFSET_BMAP_Ar, CRPS_EGQ_0_OFFSET_BMAP_Br, CRPS_EGQ_1_OFFSET_BMAP_Ar, CRPS_EGQ_1_OFFSET_BMAP_Br},
        irpp_counter_bmap_reg[] = {CRPS_IRPP_0_OFFSET_BMAP_Ar, CRPS_IRPP_0_OFFSET_BMAP_Br, CRPS_IRPP_1_OFFSET_BMAP_Ar, CRPS_IRPP_1_OFFSET_BMAP_Br},
        epni_counter_bmap_reg[] = {CRPS_EPNI_0_OFFSET_BMAP_Ar, CRPS_EPNI_0_OFFSET_BMAP_Br, CRPS_EPNI_1_OFFSET_BMAP_Ar, CRPS_EPNI_1_OFFSET_BMAP_Br},
        egq_tm_counter_bmap_reg[] =  {CRPS_EGQ_0_TM_OFFSET_BMAPr, CRPS_EGQ_1_TM_OFFSET_BMAPr},
        counter_bmap_reg = INVALIDf;
   int index, field_offset = 0;
    soc_field_t
       iqm_counter_bmap_fields[] =  {_BMAP_FIELDS(IQM, 0, A), _BMAP_FIELDS(IQM, 0, B), _BMAP_FIELDS(IQM, 1, A), _BMAP_FIELDS(IQM, 1, B)},
       egq_counter_bmap_fields[] =  {_BMAP_FIELDS(EGQ, 0, A), _BMAP_FIELDS(EGQ, 0, B), _BMAP_FIELDS(EGQ, 1, A), _BMAP_FIELDS(EGQ, 1, B)},
       irpp_counter_bmap_fields[] = {_BMAP_FIELDS(IRPP, 0, A), _BMAP_FIELDS(IRPP, 0, B), _BMAP_FIELDS(IRPP, 1, A), _BMAP_FIELDS(IRPP, 1, B)},
       epni_counter_bmap_fields[] = {_BMAP_FIELDS(EPNI, 0, A), _BMAP_FIELDS(EPNI, 0, B), _BMAP_FIELDS(EPNI, 1, A), _BMAP_FIELDS(EPNI, 1, B)},
       egq_tm_counter_bmap_fields[] = {_BMAP_FIELDS(EGQ, 0, _TM), _BMAP_FIELDS(EGQ, 1, _TM)},
       *counter_bmap_fields = NULL;
   SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   switch (src_type) {
    case ARAD_CNT_SRC_TYPE_ING_PP:
        counter_bmap_reg = irpp_counter_bmap_reg[command_id + (src_core * 2)];
        counter_bmap_fields = irpp_counter_bmap_fields;
        field_offset = (command_id + (src_core * 2)) * ARAD_CNT_BMAP_OFFSET_COUNT;
        break;      
    case ARAD_CNT_SRC_TYPE_VOQ:
    case ARAD_CNT_SRC_TYPE_INGRESS_EXT_STAT:		
    case ARAD_CNT_SRC_TYPE_STAG:
    case ARAD_CNT_SRC_TYPE_VSQ:
    case ARAD_CNT_SRC_TYPE_CNM_ID:
        counter_bmap_reg = iqm_counter_bmap_reg[command_id + (src_core * 2)];
        counter_bmap_fields = iqm_counter_bmap_fields;
        field_offset = (command_id + (src_core * 2)) * ARAD_CNT_BMAP_OFFSET_COUNT;
        break;  
    case ARAD_CNT_SRC_TYPE_EGR_PP:
        counter_bmap_reg = egq_counter_bmap_reg[command_id + (src_core * 2)];
        counter_bmap_fields = egq_counter_bmap_fields;
        field_offset = (command_id + (src_core * 2)) * ARAD_CNT_BMAP_OFFSET_COUNT;
        break;
    case ARAD_CNT_SRC_TYPE_INGRESS_OAM:
    case ARAD_CNT_SRC_TYPE_EGRESS_OAM:
    
      break;
    case ARAD_CNT_SRC_TYPES_IPT_LATENCY:
    
       break;
    case ARAD_CNT_SRC_TYPE_EPNI:
        counter_bmap_reg = epni_counter_bmap_reg[command_id + (src_core * 2)];
        counter_bmap_fields = epni_counter_bmap_fields;
        field_offset = (command_id + (src_core * 2)) * ARAD_CNT_BMAP_OFFSET_COUNT;
        break;
   case ARAD_CNT_SRC_TYPES_EGQ_TM:
       counter_bmap_reg = egq_tm_counter_bmap_reg[src_core];
       counter_bmap_fields = egq_tm_counter_bmap_fields;
       field_offset = src_core * ARAD_CNT_BMAP_OFFSET_COUNT;
       break;
   default:
        SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }
  *reg = counter_bmap_reg;
  if (counter_bmap_reg != INVALIDf && counter_bmap_fields) {
       for (index = 0; index  < ARAD_CNT_BMAP_OFFSET_COUNT; index++) {
           fields[index] = counter_bmap_fields[field_offset + index];
       }
   }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _jer_cnt_counter_bmap_mem_by_src_type_get()", 0, 0);
}

STATIC uint32
  _arad_cnt_counter_bmap_mem_by_src_type_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 proc_id,
    SOC_SAND_IN  ARAD_CNT_SRC_TYPE      src_type,
    SOC_SAND_IN  int                    commnad_id,
    SOC_SAND_OUT soc_mem_t              *mem,
    SOC_SAND_OUT soc_field_t            *fields
       )
{
  soc_mem_t
      iqm_counter_bmap_mem[] = {CRPS_IQM_OFFSET_BMAP_Am, CRPS_IQM_OFFSET_BMAP_Bm, CRPS_IQM_OFFSET_BMAP_Cm, CRPS_IQM_OFFSET_BMAP_Dm},
      counter_bmap_mem = INVALIDf;
  soc_field_t counter_bmap_fields[] = {GREEN_ADMITf, GREEN_DISCARDf, YELLOW_1_ADMITf, YELLOW_1_DISCARDf, YELLOW_2_ADMITf, YELLOW_2_DISCARDf, RED_ADMITf, RED_DISCARDf};
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  switch (src_type) {
    case ARAD_CNT_SRC_TYPE_ING_PP:
      if (commnad_id == 0) {
          counter_bmap_mem = CRPS_IRPP_OFFSET_BMAP_Am;
          break;    
      } else {
          counter_bmap_mem = CRPS_IRPP_OFFSET_BMAP_Bm;
          break;             
      }
    case ARAD_CNT_SRC_TYPE_VOQ:
    case ARAD_CNT_SRC_TYPE_INGRESS_EXT_STAT:		
    case ARAD_CNT_SRC_TYPE_STAG:
    case ARAD_CNT_SRC_TYPE_VSQ:
    case ARAD_CNT_SRC_TYPE_CNM_ID:
      counter_bmap_mem = iqm_counter_bmap_mem[proc_id];
      break;  
    case ARAD_CNT_SRC_TYPE_EGR_PP:
      counter_bmap_mem = CRPS_EGQ_OFFSET_BMAPm;
      break;
  case ARAD_CNT_SRC_TYPE_INGRESS_OAM:
  case ARAD_CNT_SRC_TYPE_EGRESS_OAM:
    
      break;
    case ARAD_CNT_SRC_TYPE_EPNI:
      if (commnad_id == 0) {
          counter_bmap_mem = CRPS_EPNI_OFFSET_BMAP_Am;
          break;
      } else {
          counter_bmap_mem = CRPS_EPNI_OFFSET_BMAP_Bm;
          break;
      }
    default:
        SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }
  
  *mem = counter_bmap_mem;
  if (counter_bmap_mem != INVALIDf) {
      sal_memcpy(fields, counter_bmap_fields, ARAD_CNT_BMAP_OFFSET_COUNT * sizeof(soc_field_t));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_cnt_counter_bmap_mem_by_src_type_get()", 0, 0);
}


uint32
  _arad_cnt_do_not_count_field_by_src_type_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 proc_id,
    SOC_SAND_IN  int                    src_core,
    SOC_SAND_IN  ARAD_CNT_SRC_TYPE      src_type,
    SOC_SAND_IN  int                    command_id,
    SOC_SAND_OUT soc_field_t            *field
    )
{
  soc_field_t       
      arad_iqm_do_not_count_entryf[] = {IQM_A_DO_NOT_COUNT_OFFSETf,  IQM_B_DO_NOT_COUNT_OFFSETf, IQM_C_DO_NOT_COUNT_OFFSETf, IQM_D_DO_NOT_COUNT_OFFSETf},
      iqm_do_not_count_entryf[] =      {IQM_0_A_DO_NOT_COUNT_OFFSETf,  IQM_0_B_DO_NOT_COUNT_OFFSETf,  IQM_1_A_DO_NOT_COUNT_OFFSETf,  IQM_1_B_DO_NOT_COUNT_OFFSETf},
      egq_do_not_count_entryf[] =      {EGQ_0_A_DO_NOT_COUNT_OFFSETf,  EGQ_0_B_DO_NOT_COUNT_OFFSETf,  EGQ_1_A_DO_NOT_COUNT_OFFSETf,  EGQ_1_B_DO_NOT_COUNT_OFFSETf},
      irpp_do_not_count_entryf[] =     {IRPP_0_A_DO_NOT_COUNT_OFFSETf, IRPP_0_B_DO_NOT_COUNT_OFFSETf, IRPP_1_A_DO_NOT_COUNT_OFFSETf, IRPP_1_B_DO_NOT_COUNT_OFFSETf},
      epni_do_not_count_entryf[] =     {EPNI_0_A_DO_NOT_COUNT_OFFSETf, EPNI_0_B_DO_NOT_COUNT_OFFSETf, EPNI_1_A_DO_NOT_COUNT_OFFSETf, EPNI_1_B_DO_NOT_COUNT_OFFSETf},
      egq_tm_do_not_count_entryf[] =   {EGQ_0_TM_DO_NOT_COUNT_OFFSETf, EGQ_1_TM_DO_NOT_COUNT_OFFSETf},
      do_not_count_fld = INVALIDf;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (SOC_IS_JERICHO(unit)) {
      switch (src_type) {
      case ARAD_CNT_SRC_TYPE_ING_PP:
          do_not_count_fld = irpp_do_not_count_entryf[command_id + (src_core * 2)];
          break;      
      case ARAD_CNT_SRC_TYPE_VOQ:
      case ARAD_CNT_SRC_TYPE_INGRESS_EXT_STAT:	  	
      case ARAD_CNT_SRC_TYPE_STAG:
      case ARAD_CNT_SRC_TYPE_VSQ:
      case ARAD_CNT_SRC_TYPE_CNM_ID:
          do_not_count_fld = iqm_do_not_count_entryf[command_id + (src_core * 2)];
          break;  
      case ARAD_CNT_SRC_TYPE_EGR_PP:
          do_not_count_fld = egq_do_not_count_entryf[command_id + (src_core * 2)];
          break;
      case ARAD_CNT_SRC_TYPE_INGRESS_OAM:
      case ARAD_CNT_SRC_TYPE_EGRESS_OAM:
        
          break;
      case ARAD_CNT_SRC_TYPE_EPNI:
          do_not_count_fld = epni_do_not_count_entryf[command_id + (src_core * 2)];
          break;
      case ARAD_CNT_SRC_TYPES_IPT_LATENCY:
          
          break;
      case ARAD_CNT_SRC_TYPES_EGQ_TM:
          do_not_count_fld = egq_tm_do_not_count_entryf[src_core];
          break;
      default:
          SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
      }
  } else {
     switch (src_type) {
     case ARAD_CNT_SRC_TYPE_ING_PP:
         if (command_id == 0) {
             do_not_count_fld = IRPP_A_DO_NOT_COUNT_OFFSETf;
             break;
         } else {
             do_not_count_fld = IRPP_B_DO_NOT_COUNT_OFFSETf;
             break;        
         }
     case ARAD_CNT_SRC_TYPE_VOQ:
     case ARAD_CNT_SRC_TYPE_INGRESS_EXT_STAT:	 	
     case ARAD_CNT_SRC_TYPE_STAG:
     case ARAD_CNT_SRC_TYPE_VSQ:
     case ARAD_CNT_SRC_TYPE_CNM_ID:
         do_not_count_fld = arad_iqm_do_not_count_entryf[proc_id];
         break;  
     case ARAD_CNT_SRC_TYPE_EGR_PP:
         do_not_count_fld = EGQ_DO_NOT_COUNT_OFFSETf;
         break;
     case ARAD_CNT_SRC_TYPE_INGRESS_OAM:
     case ARAD_CNT_SRC_TYPE_EGRESS_OAM:
         
         break;
     case ARAD_CNT_SRC_TYPE_EPNI:
         if (command_id == 0) {
             do_not_count_fld = EPNI_A_DO_NOT_COUNT_OFFSETf;
             break;
         } else {
             do_not_count_fld = EPNI_B_DO_NOT_COUNT_OFFSETf;
             break;
         }
     default:
         SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 20, exit);
     }
  }
  *field = do_not_count_fld;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_cnt_do_not_count_field_by_src_type_get()", 0, 0);
}

STATIC uint32
    _arad_cnt_bmap_configurable_offset_format_set(
       SOC_SAND_IN  int                     unit,
       SOC_SAND_IN  int                     proc_id,
       SOC_SAND_IN  uint32                  group_size_val, 
       SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO *info)
{
    soc_field_t do_not_count_field = INVALIDf;
    uint64 do_not_count_val;
    soc_mem_t src_btmap_mem = INVALIDm;
    soc_reg_t src_btmap_reg = 0;
    soc_field_t bmap_fields[ARAD_CNT_BMAP_OFFSET_COUNT] = {0};
    uint32 reg_val = 0;
    uint32 res = SOC_SAND_OK;
    uint32 entry_index, counter_index;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


   if (info->src_type == ARAD_CNT_SRC_TYPE_INGRESS_OAM || info->src_type == ARAD_CNT_SRC_TYPE_EGRESS_OAM || info->src_type == ARAD_CNT_SRC_TYPES_IPT_LATENCY){
       
       
       SOC_SAND_EXIT_NO_ERROR;      
   }
   
       if(group_size_val != ARAD_CNT_BMAP_OFFSET_COUNT) {
            COMPILER_64_SET(do_not_count_val, 0, ARAD_CNT_DO_NOT_COUNT_VAL);
       } else {
            
            COMPILER_64_SET(do_not_count_val, 0, (ARAD_CNT_DO_NOT_COUNT_VAL+1));
       }
        
        res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_do_not_count_field_by_src_type_get,(unit, proc_id, info->src_core, info->src_type, info->command_id, &do_not_count_field));
        SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
        ARAD_DEVICE_CHECK(unit, exit);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(
           res,  18,  exit, ARAD_REG_ACCESS_ERR,
           soc_reg_above_64_field64_modify(unit, CRPS_CRPS_DO_NOT_COUNT_FILTERr, REG_PORT_ANY, 0, do_not_count_field, do_not_count_val));

    
    if (SOC_IS_JERICHO(unit)) {
        res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_counter_bmap_mem_by_src_type_get,(unit, proc_id, info->src_core, info->src_type, info->command_id, &src_btmap_reg, bmap_fields));                            
        SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);
        ARAD_DEVICE_CHECK(unit, exit);        
        
        for(entry_index = 0 ; entry_index < ARAD_CNT_BMAP_OFFSET_COUNT; entry_index++){
          
          for(counter_index = 0 ; counter_index < ARAD_CNT_BMAP_OFFSET_COUNT; counter_index++){
            if(SHR_BITGET(&(info->custom_mode_params.entries_bmaps[entry_index]), counter_index)){
              
              
              soc_reg_field_set(unit, src_btmap_reg, &reg_val , bmap_fields[counter_index], entry_index); 
            }
          }
        }
        res = soc_reg32_set(unit, src_btmap_reg, REG_PORT_ANY, 0, reg_val);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    } else if ((SOC_IS_ARADPLUS(unit)) && (info->mode_statistics == ARAD_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS)){
        res = _arad_cnt_counter_bmap_mem_by_src_type_get(unit, proc_id, info->src_type, info->command_id, &src_btmap_mem, bmap_fields);
        SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);
        
        for(entry_index = 0 ; entry_index < ARAD_CNT_BMAP_OFFSET_COUNT; entry_index++){
          
          for(counter_index = 0 ; counter_index < ARAD_CNT_BMAP_OFFSET_COUNT; counter_index++){
            if(SHR_BITGET(&(info->custom_mode_params.entries_bmaps[entry_index]), counter_index)){
              
              soc_mem_field_set(unit, src_btmap_mem, &reg_val , bmap_fields[counter_index], &entry_index); 
            }
          }
        }
        res = soc_mem_write(unit, src_btmap_mem, MEM_BLOCK_ALL, group_size_val, &reg_val);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_cnt_bmap_configurable_offset_format_set()", 0, 0);
}


#ifdef BCM_88650_B0
uint32
  _arad_cnt_one_entry_field_by_src_type_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                proc_id, 
    SOC_SAND_IN  ARAD_CNT_SRC_TYPE     src_type,
    SOC_SAND_IN  int                   commnad_id,
    SOC_SAND_OUT soc_field_t           *field
       )
{
  soc_field_t       
      iqm_one_entryf[] = {IQM_A_ONE_ENTRY_CNT_CMDf, IQM_B_ONE_ENTRY_CNT_CMDf, IQM_C_ONE_ENTRY_CNT_CMDf, IQM_D_ONE_ENTRY_CNT_CMDf},
      one_entry_mode_fld = INVALIDf;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch (src_type) {
    case ARAD_CNT_SRC_TYPE_ING_PP:
      if (commnad_id == 0) {
          one_entry_mode_fld = IRPP_A_ONE_ENTRY_CNT_CMDf;
          break;      
      } else {
          one_entry_mode_fld = IRPP_B_ONE_ENTRY_CNT_CMDf;
          break;    
      }
    case ARAD_CNT_SRC_TYPE_VOQ:
    case ARAD_CNT_SRC_TYPE_INGRESS_EXT_STAT:		
    case ARAD_CNT_SRC_TYPE_STAG:
    case ARAD_CNT_SRC_TYPE_VSQ:
    case ARAD_CNT_SRC_TYPE_CNM_ID:
      one_entry_mode_fld = iqm_one_entryf[proc_id]; 
      break;  
    case ARAD_CNT_SRC_TYPE_EGR_PP:
      one_entry_mode_fld = EGQ_ONE_ENTRY_CNT_CMDf;
      break;
    case ARAD_CNT_SRC_TYPE_INGRESS_OAM:
    case ARAD_CNT_SRC_TYPE_EGRESS_OAM:
    
      break;
    case ARAD_CNT_SRC_TYPE_EPNI:
      if (commnad_id == 0) {
          one_entry_mode_fld = EPNI_A_ONE_ENTRY_CNT_CMDf;
          break;
      } else {
          one_entry_mode_fld = EPNI_B_ONE_ENTRY_CNT_CMDf;
          break;
      }
    default:
        SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
    }
    *field = one_entry_mode_fld;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_cnt_one_entry_field_by_src_type_get()", 0, 0);
}
#endif

STATIC uint32
  _arad_cnt_src_mode_field_get(
     SOC_SAND_IN  int                    unit,
     SOC_SAND_IN  int                    src_core,
     SOC_SAND_IN  ARAD_CNT_SRC_TYPE      src_type,
     SOC_SAND_IN  int                    command_id,
     SOC_SAND_OUT soc_field_t            *field
     )
{
    soc_field_t irpp_fields[] = {IRPP_0_A_MODEf, IRPP_0_B_MODEf, IRPP_1_A_MODEf, IRPP_1_B_MODEf, IRPP_A_MODEf, IRPP_B_MODEf};
    soc_field_t egq_fields[] = {EGQ_0_A_MODEf, EGQ_0_B_MODEf, EGQ_1_A_MODEf, EGQ_1_B_MODEf, EGQ_A_MODEf, EGQ_B_MODEf};
    soc_field_t epni_fields[] = {EPNI_0_A_MODEf, EPNI_0_B_MODEf, EPNI_1_A_MODEf, EPNI_1_B_MODEf, EPNI_A_MODEf, EPNI_B_MODEf};
    soc_field_t iqm_fields[] = {IQM_0_A_MODEf, IQM_0_B_MODEf, IQM_1_A_MODEf, IQM_1_B_MODEf, CGM_A_MODEf, CGM_B_MODEf};
    soc_field_t egq_tm_fields[] ={EGQ_0_TM_MODEf, EGQ_1_TM_MODEf, EGQ_TM_MODEf};
    soc_field_t ihb_fields[] = {IHB_0_MODEf, IHB_1_MODEf, IHB_A_MODEf, IHB_B_MODEf};
    soc_field_t ipt_fields[] = {IPT_0_MODEf, IPT_1_MODEf, IDPS_MODEf, IDPD_MODEf};
    SOCDNX_INIT_FUNC_DEFS;

    if (src_core < 0 || src_core >= SOC_DPP_DEFS_GET(unit, nof_cores)) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid source core ID %d.\n"), src_core));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    switch (src_type) {
    case ARAD_CNT_SRC_TYPE_ING_PP:
        *field = (SOC_IS_QAX(unit) ? irpp_fields[4 + command_id] : irpp_fields[(src_core * 2) + command_id] );
        break;
    case ARAD_CNT_SRC_TYPE_VOQ:
    case ARAD_CNT_SRC_TYPE_INGRESS_EXT_STAT:		
    case ARAD_CNT_SRC_TYPE_STAG:
    case ARAD_CNT_SRC_TYPE_VSQ:
    case ARAD_CNT_SRC_TYPE_CNM_ID:
        *field = (SOC_IS_QAX(unit) ? iqm_fields[4 + command_id] : iqm_fields[(src_core * 2) + command_id]);
        break;
    case ARAD_CNT_SRC_TYPE_EGR_PP:
        *field = (SOC_IS_QAX(unit) ? egq_fields[4 + command_id] : egq_fields[(src_core * 2) + command_id]);
        break;
    case ARAD_CNT_SRC_TYPE_INGRESS_OAM:
    case ARAD_CNT_SRC_TYPE_EGRESS_OAM:
        *field = (SOC_IS_QAX(unit) ? ihb_fields[2 + command_id] : ihb_fields[src_core]);
        break;
    case ARAD_CNT_SRC_TYPES_IPT_LATENCY:
        *field = (SOC_IS_QAX(unit) ? ipt_fields[2 + command_id] : ipt_fields[src_core]);                        
        break;
    case ARAD_CNT_SRC_TYPE_EPNI:
        *field = (SOC_IS_QAX(unit) ? epni_fields[4 + command_id] : epni_fields[(src_core * 2) + command_id]);        
      break;
    case ARAD_CNT_SRC_TYPES_EGQ_TM:
        *field = (SOC_IS_QAX(unit) ? egq_tm_fields[2] : egq_tm_fields[src_core]);                        
      break;
    default:
        SOCDNX_IF_ERR_EXIT(BCM_E_PARAM);
      }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  _arad_cnt_group_size_field_by_src_type_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 proc_id, 
    SOC_SAND_IN  int                    src_core,
    SOC_SAND_IN  ARAD_CNT_SRC_TYPE      src_type,
    SOC_SAND_IN  int                    command_id,
    SOC_SAND_OUT soc_field_t            *field
    )
{
  soc_field_t       
      arad_iqm_group_sizef[] = {IQM_A_GROUP_SIZEf, IQM_B_GROUP_SIZEf, IQM_C_GROUP_SIZEf, IQM_D_GROUP_SIZEf},
      iqm_group_sizef[] =  {IQM_0_A_GROUP_SIZEf,  IQM_0_B_GROUP_SIZEf,  IQM_1_A_GROUP_SIZEf,  IQM_1_B_GROUP_SIZEf},
      egq_group_sizef[] =  {EGQ_0_A_GROUP_SIZEf,  EGQ_0_B_GROUP_SIZEf,  EGQ_1_A_GROUP_SIZEf,  EGQ_1_B_GROUP_SIZEf},
      irpp_group_sizef[] = {IRPP_0_A_GROUP_SIZEf, IRPP_0_B_GROUP_SIZEf, IRPP_1_A_GROUP_SIZEf, IRPP_1_B_GROUP_SIZEf},
      epni_group_sizef[] = {EPNI_0_A_GROUP_SIZEf, EPNI_0_B_GROUP_SIZEf, EPNI_1_A_GROUP_SIZEf, EPNI_1_B_GROUP_SIZEf},
      egq_tm_group_sizef[] = {EGQ_0_TM_GROUP_SIZEf, EGQ_1_TM_GROUP_SIZEf},
      group_size_fld = INVALIDf;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  if (SOC_IS_JERICHO(unit)) {
      switch (src_type) {
        case ARAD_CNT_SRC_TYPE_ING_PP:
            group_size_fld = irpp_group_sizef[command_id + (src_core * 2)];
            break;
        case ARAD_CNT_SRC_TYPE_VOQ:
        case ARAD_CNT_SRC_TYPE_INGRESS_EXT_STAT:			
        case ARAD_CNT_SRC_TYPE_STAG:
        case ARAD_CNT_SRC_TYPE_VSQ:
        case ARAD_CNT_SRC_TYPE_CNM_ID:
            group_size_fld = iqm_group_sizef[command_id + (src_core * 2)];
            break;
        case ARAD_CNT_SRC_TYPE_EGR_PP:
            group_size_fld = egq_group_sizef[command_id + (src_core * 2)];
            break;
        case ARAD_CNT_SRC_TYPE_INGRESS_OAM:
        case ARAD_CNT_SRC_TYPE_EGRESS_OAM:
          
          break;
        case ARAD_CNT_SRC_TYPES_IPT_LATENCY:
            
            break;
        case ARAD_CNT_SRC_TYPE_EPNI:
          group_size_fld = epni_group_sizef[command_id + (src_core * 2)];
          break;
        case ARAD_CNT_SRC_TYPES_EGQ_TM:
          group_size_fld = egq_tm_group_sizef[src_core];
          break;
        default:
            SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
        }
  } else {
      switch (src_type) {
        case ARAD_CNT_SRC_TYPE_ING_PP:
          if (command_id == 0) {
              group_size_fld = IRPP_A_GROUP_SIZEf;
              break;      
          } else {
              group_size_fld = IRPP_B_GROUP_SIZEf;
              break;
          }
        case ARAD_CNT_SRC_TYPE_VOQ:
        case ARAD_CNT_SRC_TYPE_INGRESS_EXT_STAT:			
        case ARAD_CNT_SRC_TYPE_STAG:
        case ARAD_CNT_SRC_TYPE_VSQ:
        case ARAD_CNT_SRC_TYPE_CNM_ID:
          group_size_fld = arad_iqm_group_sizef[proc_id];
          break;
        case ARAD_CNT_SRC_TYPE_EGR_PP:
          group_size_fld = EGQ_GROUP_SIZEf;
          break;
        case ARAD_CNT_SRC_TYPE_INGRESS_OAM:
        case ARAD_CNT_SRC_TYPE_EGRESS_OAM:
          
          break;
        case ARAD_CNT_SRC_TYPE_EPNI:
          if (command_id == 0) {
              group_size_fld = EPNI_A_GROUP_SIZEf;
              break;
          } else {
              group_size_fld = EPNI_B_GROUP_SIZEf;
              break;
          }
        default:
            SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 20, exit);
        }
  }
  *field = group_size_fld;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_cnt_group_size_field_by_src_type_get()", 0, 0);
}


uint32 
    arad_cnt_lif_counting_set(
       SOC_SAND_IN int                  unit,
       SOC_SAND_IN SOC_TMC_CNT_SRC_TYPE source,
       SOC_SAND_IN int                  command_id,
       SOC_SAND_IN uint32               lif_counting_mask,
       SOC_SAND_IN int                  lif_stack_to_count
       )
{
    int rv = SOC_E_NONE;
    int counting_mask[4];
    uint32 data[20] = {0};
    int counter_0_mode_is_lif = 0, 
        counter_1_mode_is_lif = 0;
    uint32 
        counter_sources = 0;
    int index = 0;
    int stack_level = 0;
    uint32 lif_stack_to_count_fld_val = SOC_TMC_CNT_LIF_COUNTING_STACK_LEVEL_TO_FIELD_VAL_GET(lif_stack_to_count, source);
    SOCDNX_INIT_FUNC_DEFS;
    if (lif_counting_mask > SOC_TMC_CNT_LIF_COUNTING_MASK_MAX(unit, source)) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid LIF-counting-mask %d.\n"), lif_counting_mask));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (command_id != 0 && command_id != 1) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid command ID %d.\n"), command_id));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if ((lif_stack_to_count < -1 && source == SOC_TMC_CNT_SRC_TYPE_EPNI ) ||
        (lif_stack_to_count < 0  && source != SOC_TMC_CNT_SRC_TYPE_EPNI) || 
        (lif_stack_to_count >= SOC_TMC_CNT_LIF_COUNTING_NUMBER_OF_STACK_IDS(unit, source))) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid lif stack to count %d.\n"), lif_stack_to_count));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    for (stack_level = 0; stack_level < SOC_TMC_CNT_LIF_COUNTING_NUMBER_OF_STACK_IDS(unit, source); stack_level++) {
        counting_mask[stack_level] = SOC_TMC_CNT_LIF_COUNTING_MASK_FIELD_VAL_GET(unit, source, lif_counting_mask, stack_level);
        if (counting_mask[stack_level] < 0 || counting_mask[stack_level] > 3) {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid lif stack %d in lif counting stack level %d, .\n"), stack_level, counting_mask[stack_level]));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }
    if (source == SOC_TMC_CNT_SRC_TYPE_ING_PP) {
        
        index = (counting_mask[0] << 6) | (counting_mask[1] << 4) | (counting_mask[2] << 2) | (counting_mask[3]);
        rv = READ_IHP_DEFAULT_COUNTER_SOURCEm(unit, IHP_BLOCK(unit, SOC_CORE_ALL), 0x1f & (index >> 3) ,data);
        SOCDNX_IF_ERR_EXIT(rv);

        counter_sources = soc_IHP_DEFAULT_COUNTER_SOURCEm_field32_get(unit, data, COUNTER_SOURCESf);
        
        
        SHR_BITCOPY_RANGE(&counter_sources, ((index & 0x7) * 4) + (command_id * 2), &lif_stack_to_count_fld_val, 0, 2);

        soc_IHP_DEFAULT_COUNTER_SOURCEm_field32_set(unit, data, COUNTER_SOURCESf, counter_sources);

        rv = WRITE_IHP_DEFAULT_COUNTER_SOURCEm(unit, IHP_BLOCK(unit, SOC_CORE_ALL), 0x1f & (index >> 3) ,data);
        SOCDNX_IF_ERR_EXIT(rv);
    } else if (source == SOC_TMC_CNT_SRC_TYPE_EPNI) {
        for (counter_0_mode_is_lif = 0; counter_0_mode_is_lif < 2; counter_0_mode_is_lif++)
        {
            for (counter_1_mode_is_lif = 0; counter_1_mode_is_lif < 2; counter_1_mode_is_lif++)
            {
                
                if (SOC_IS_JERICHO_B0_AND_ABOVE(unit) == TRUE)
                {
                    index = ((counter_0_mode_is_lif << 9) | (counter_1_mode_is_lif << 8) | (counting_mask[0] << 6) | (counting_mask[1] << 4) | (counting_mask[2] << 2) | (counting_mask[3]));
                }
                else
                {
                    index = ((counter_0_mode_is_lif << 7) | (counter_1_mode_is_lif << 6) | (counting_mask[0] << 4) | (counting_mask[1] << 2) | (counting_mask[2]));
                }

                rv = READ_EPNI_COUNTER_SOURCE_MAPm(unit, EPNI_BLOCK(unit, SOC_CORE_ALL), index, data);
                SOCDNX_IF_ERR_EXIT(rv);

                if (command_id == 0) {
                    soc_EPNI_COUNTER_SOURCE_MAPm_field32_set(unit, data, COUNTER_0_SOURCEf, lif_stack_to_count_fld_val);
                } else if (command_id == 1) {
                    soc_EPNI_COUNTER_SOURCE_MAPm_field32_set(unit, data, COUNTER_1_SOURCEf, lif_stack_to_count_fld_val);
                }
                rv = WRITE_EPNI_COUNTER_SOURCE_MAPm(unit, EPNI_BLOCK(unit, SOC_CORE_ALL), index, data);
                SOCDNX_IF_ERR_EXIT(rv);

            }
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid source type %d.\n"), source));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
   arad_cnt_lif_counting_get(
      SOC_SAND_IN int                  unit,
      SOC_SAND_IN SOC_TMC_CNT_SRC_TYPE source,
      SOC_SAND_IN int                  command_id,
      SOC_SAND_IN uint32               lif_counting_mask,
      SOC_SAND_OUT int*                lif_stack_to_count
   )
{   
    int rv = SOC_E_NONE;
    int counting_mask[4];
    uint32 data[20] = {0};
    int counter_0_mode_is_lif = 0, 
        counter_1_mode_is_lif = 0;
    int index = 0;
    uint32 
        counter_sources;
    int 
        stack_level = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (command_id != 0 && command_id != 1) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid command ID %d.\n"), command_id));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (lif_counting_mask > SOC_TMC_CNT_LIF_COUNTING_MASK_MAX(unit, source)) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid LIF-counting-mask %d.\n"), lif_counting_mask));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    for (stack_level = 0; stack_level < SOC_TMC_CNT_LIF_COUNTING_NUMBER_OF_STACK_IDS(unit, source); stack_level++) {
        counting_mask[stack_level] = SOC_TMC_CNT_LIF_COUNTING_MASK_FIELD_VAL_GET(unit, source, lif_counting_mask, stack_level);
        if (counting_mask[stack_level] < 0 || counting_mask[stack_level] > 3) {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid lif stack %d in lif counting stack level %d, .\n"), stack_level, counting_mask[stack_level]));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }
    if (source == SOC_TMC_CNT_SRC_TYPE_ING_PP) {
        
        index = (counting_mask[0] << 6) | (counting_mask[1] << 4) | (counting_mask[2] << 2) | (counting_mask[3]);
        rv = READ_IHP_DEFAULT_COUNTER_SOURCEm(unit, IHP_BLOCK(unit, SOC_CORE_ALL), 0x1f & (index >> 3) ,data);
        SOCDNX_IF_ERR_EXIT(rv);

        counter_sources = soc_IHP_DEFAULT_COUNTER_SOURCEm_field32_get(unit, data, COUNTER_SOURCESf);
        counter_sources = ((counter_sources >> ((index & 7) * 4)) & 0xf);

        if (command_id == 0) {
            *lif_stack_to_count = (int)SOC_TMC_CNT_LIF_COUNTING_FIELD_VAL_TO_STACK_LEVEL_GET(soc_IHP_DEFAULT_COUNTER_SOURCEm_field32_get(unit, &counter_sources, COUNTER_0_SOURCEf), source);
        } else if (command_id == 1) {
            *lif_stack_to_count = (int)SOC_TMC_CNT_LIF_COUNTING_FIELD_VAL_TO_STACK_LEVEL_GET(soc_IHP_DEFAULT_COUNTER_SOURCEm_field32_get(unit, &counter_sources, COUNTER_1_SOURCEf), source);
        }
    } else if (source == SOC_TMC_CNT_SRC_TYPE_EPNI) {
        rv = READ_EPNI_PP_COUNTER_TABLEm(unit, EPNI_BLOCK(unit, SOC_CORE_ALL), 0 , data);
        SOCDNX_IF_ERR_EXIT(rv);
            
        counter_0_mode_is_lif = (soc_EPNI_PP_COUNTER_TABLEm_field32_get(unit, data, COUNTER_0_MODEf) == ARAD_CNT_MODE_EG_TYPE_PMF_FLD_VAL);
        counter_1_mode_is_lif = (soc_EPNI_PP_COUNTER_TABLEm_field32_get(unit, data, COUNTER_1_MODEf) == ARAD_CNT_MODE_EG_TYPE_PMF_FLD_VAL);
        
        
        if (SOC_IS_JERICHO_B0_AND_ABOVE(unit) == TRUE)
        {
            index = ((counter_0_mode_is_lif << 9) | (counter_1_mode_is_lif << 8) | (counting_mask[0] << 6) | (counting_mask[1] << 4) | (counting_mask[2] << 2) | (counting_mask[3]));
        }
        else
        {
            index = ((counter_0_mode_is_lif << 7) | (counter_1_mode_is_lif << 6) | (counting_mask[0] << 4) | (counting_mask[1] << 2) | (counting_mask[2]));
        }
        rv = READ_EPNI_COUNTER_SOURCE_MAPm(unit, EPNI_BLOCK(unit, SOC_CORE_ALL), index, data);
        SOCDNX_IF_ERR_EXIT(rv);

        if (command_id == 0) {
            *lif_stack_to_count = (int)SOC_TMC_CNT_LIF_COUNTING_FIELD_VAL_TO_STACK_LEVEL_GET(soc_EPNI_COUNTER_SOURCE_MAPm_field32_get(unit, data, COUNTER_0_SOURCEf), source);
        } else if (command_id == 1) {
            *lif_stack_to_count = (int)SOC_TMC_CNT_LIF_COUNTING_FIELD_VAL_TO_STACK_LEVEL_GET(soc_EPNI_COUNTER_SOURCE_MAPm_field32_get(unit, data, COUNTER_1_SOURCEf), source);
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid source type %d.\n"), source));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
   arad_cnt_lif_counting_range_set(
      SOC_SAND_IN int                  unit,
      SOC_SAND_IN SOC_TMC_CNT_SRC_TYPE source,
      SOC_SAND_IN int                  range_id,
      SOC_SAND_IN SOC_SAND_U32_RANGE*  range,
      SOC_SAND_IN int stif_counter_id,    
      SOC_SAND_IN int is_double_entry,
      SOC_SAND_IN int engine_range_offset
   )
{   
    int rv = SOC_E_NONE;
    soc_reg_t ihp_counter_in_lif_range_0r, 
              ihp_counter_in_lif_range_1r;
    soc_field_t ihp_counter_in_lif_range_low_0f, 
                ihp_counter_in_lif_range_high_0f,
                ihp_counter_in_lif_range_low_1f, 
                ihp_counter_in_lif_range_high_1f;
    SOCDNX_INIT_FUNC_DEFS;

    if(source == SOC_TMC_CNT_SRC_TYPE_EPNI && SOC_IS_JERICHO_PLUS(unit))
    {
        SOCDNX_IF_ERR_EXIT(jer_plus_cnt_out_lif_counting_range_set(unit, source, range_id, range, stif_counter_id, is_double_entry, engine_range_offset));
        SOCDNX_IF_ERR_EXIT(rv);      
    }    
    else
    {
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            
            ihp_counter_in_lif_range_0r = IHP_REG_01CCr;
            ihp_counter_in_lif_range_1r = IHP_REG_01CDr;
            ihp_counter_in_lif_range_low_0f = ihp_counter_in_lif_range_low_1f = FIELD_0_15f;
            ihp_counter_in_lif_range_high_0f = ihp_counter_in_lif_range_high_1f = FIELD_16_31f;
        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)){
            
            ihp_counter_in_lif_range_0r = IHP_REG_01CDr;
            ihp_counter_in_lif_range_1r = IHP_REG_01CEr;
            ihp_counter_in_lif_range_low_0f = ihp_counter_in_lif_range_low_1f = FIELD_0_15f;
            ihp_counter_in_lif_range_high_0f = ihp_counter_in_lif_range_high_1f = FIELD_16_31f;
        } else if (SOC_IS_JERICHO(unit)) {
              ihp_counter_in_lif_range_0r = IHP_COUNTER_IN_LIF_RANGE_0r;
              ihp_counter_in_lif_range_1r = IHP_COUNTER_IN_LIF_RANGE_1r;
              ihp_counter_in_lif_range_low_0f = COUNTER_IN_LIF_RANGE_0_LOWf;
              ihp_counter_in_lif_range_high_0f = COUNTER_IN_LIF_RANGE_0_HIGHf;
              ihp_counter_in_lif_range_low_1f = COUNTER_IN_LIF_RANGE_1_LOWf;
              ihp_counter_in_lif_range_high_1f = COUNTER_IN_LIF_RANGE_1_HIGHf;
        } else {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        if (range_id != 0 && range_id != 1) {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid range ID %d.\n"), range_id));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        if (source == SOC_TMC_CNT_SRC_TYPE_ING_PP) {
        
            if (range_id == 0) {
                if (range->start & ~((1 << soc_reg_field_length(unit, ihp_counter_in_lif_range_0r, ihp_counter_in_lif_range_low_0f)) - 1)) {
                    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                }
                if (range->end & ~((1 << soc_reg_field_length(unit, ihp_counter_in_lif_range_0r, ihp_counter_in_lif_range_high_0f)) - 1)) {
                    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                }            
                rv = soc_reg_above_64_field32_modify(unit, ihp_counter_in_lif_range_0r, SOC_CORE_ALL, 0, ihp_counter_in_lif_range_low_0f, range->start);
                SOCDNX_IF_ERR_EXIT(rv);
        
                rv = soc_reg_above_64_field32_modify(unit, ihp_counter_in_lif_range_0r, SOC_CORE_ALL, 0, ihp_counter_in_lif_range_high_0f, range->end);
                SOCDNX_IF_ERR_EXIT(rv);
        
            } else if (range_id == 1) {
                if (range->start & ~((1 << soc_reg_field_length(unit, ihp_counter_in_lif_range_1r, ihp_counter_in_lif_range_low_1f)) - 1)) {
                    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                }
                if (range->end & ~((1 << soc_reg_field_length(unit, ihp_counter_in_lif_range_1r, ihp_counter_in_lif_range_high_1f)) - 1)) {
                    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                }            
                rv = soc_reg_above_64_field32_modify(unit, ihp_counter_in_lif_range_1r, SOC_CORE_ALL, 0, ihp_counter_in_lif_range_low_1f, range->start);
                SOCDNX_IF_ERR_EXIT(rv);
        
                rv = soc_reg_above_64_field32_modify(unit, ihp_counter_in_lif_range_1r, SOC_CORE_ALL, 0, ihp_counter_in_lif_range_high_1f, range->end);
                SOCDNX_IF_ERR_EXIT(rv);
            }
        } else if (source == SOC_TMC_CNT_SRC_TYPE_EPNI) {
            uint64 data64;
            COMPILER_64_ZERO(data64);
            if (range_id == 0) {
                if (range->start & ~((1 << soc_reg_field_length(unit, EPNI_COUNTER_OUT_LIF_RANGE_0r, COUNTER_OUT_LIF_RANGE_0_LOWf)) - 1)) {
                    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                }
                if (range->end & ~((1 << soc_reg_field_length(unit, EPNI_COUNTER_OUT_LIF_RANGE_0r, COUNTER_OUT_LIF_RANGE_0_HIGHf)) - 1)) {
                    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                }            
        
                rv = READ_EPNI_COUNTER_OUT_LIF_RANGE_0r(unit, SOC_CORE_ALL, &data64);
                SOCDNX_IF_ERR_EXIT(rv);
        
                soc_reg64_field32_set(unit, EPNI_COUNTER_OUT_LIF_RANGE_0r, &data64, COUNTER_OUT_LIF_RANGE_0_LOWf,  range->start); 
                soc_reg64_field32_set(unit, EPNI_COUNTER_OUT_LIF_RANGE_0r, &data64, COUNTER_OUT_LIF_RANGE_0_HIGHf, range->end);
        
                rv = WRITE_EPNI_COUNTER_OUT_LIF_RANGE_0r(unit, SOC_CORE_ALL, data64);
                SOCDNX_IF_ERR_EXIT(rv);
            } else if (range_id == 1) {
                if (range->start & ~((1 << soc_reg_field_length(unit, EPNI_COUNTER_OUT_LIF_RANGE_1r, COUNTER_OUT_LIF_RANGE_1_LOWf)) - 1)) {
                    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                }
                if (range->end & ~((1 << soc_reg_field_length(unit, EPNI_COUNTER_OUT_LIF_RANGE_1r, COUNTER_OUT_LIF_RANGE_1_HIGHf)) - 1)) {
                    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                }            
        
                rv = READ_EPNI_COUNTER_OUT_LIF_RANGE_1r(unit, SOC_CORE_ALL, &data64);
                SOCDNX_IF_ERR_EXIT(rv);
        
                soc_reg64_field32_set(unit, EPNI_COUNTER_OUT_LIF_RANGE_1r, &data64, COUNTER_OUT_LIF_RANGE_1_LOWf,  range->start); 
                soc_reg64_field32_set(unit, EPNI_COUNTER_OUT_LIF_RANGE_1r, &data64, COUNTER_OUT_LIF_RANGE_1_HIGHf, range->end);
        
                rv = WRITE_EPNI_COUNTER_OUT_LIF_RANGE_1r(unit, SOC_CORE_ALL, data64);
                SOCDNX_IF_ERR_EXIT(rv);
            }
        } else {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid source type %d.\n"), source));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        SOCDNX_IF_ERR_EXIT(rv);    
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int 
   arad_cnt_lif_counting_range_get(
      SOC_SAND_IN int                       unit,
      SOC_SAND_IN SOC_TMC_CNT_SRC_TYPE      source,
      SOC_SAND_IN int                       range_id,
      SOC_SAND_INOUT SOC_SAND_U32_RANGE*    range     
      )
{   
    int rv = SOC_E_NONE;
    soc_reg_t ihp_counter_in_lif_range_0r, 
              ihp_counter_in_lif_range_1r;
    soc_field_t ihp_counter_in_lif_range_low_0f, 
                ihp_counter_in_lif_range_high_0f,
                ihp_counter_in_lif_range_low_1f, 
                ihp_counter_in_lif_range_high_1f;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        
        ihp_counter_in_lif_range_0r = IHP_REG_01CCr;
        ihp_counter_in_lif_range_1r = IHP_REG_01CDr;
        ihp_counter_in_lif_range_low_0f = ihp_counter_in_lif_range_low_1f = FIELD_0_15f;
        ihp_counter_in_lif_range_high_0f = ihp_counter_in_lif_range_high_1f = FIELD_16_31f;
    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)){
        
        ihp_counter_in_lif_range_0r = IHP_REG_01CDr;
        ihp_counter_in_lif_range_1r = IHP_REG_01CEr;
        ihp_counter_in_lif_range_low_0f = ihp_counter_in_lif_range_low_1f = FIELD_0_15f;
        ihp_counter_in_lif_range_high_0f = ihp_counter_in_lif_range_high_1f = FIELD_16_31f;
    } else if (SOC_IS_JERICHO(unit)) {
          ihp_counter_in_lif_range_0r = IHP_COUNTER_IN_LIF_RANGE_0r;
          ihp_counter_in_lif_range_1r = IHP_COUNTER_IN_LIF_RANGE_1r;
          ihp_counter_in_lif_range_low_0f = COUNTER_IN_LIF_RANGE_0_LOWf;
          ihp_counter_in_lif_range_high_0f = COUNTER_IN_LIF_RANGE_0_HIGHf;
          ihp_counter_in_lif_range_low_1f = COUNTER_IN_LIF_RANGE_1_LOWf;
          ihp_counter_in_lif_range_high_1f = COUNTER_IN_LIF_RANGE_1_HIGHf;
    } else {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (range_id != 0 && range_id != 1) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid range ID %d.\n"), range_id));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (source == SOC_TMC_CNT_SRC_TYPE_ING_PP) {
        if (range_id == 0) {
            rv = soc_reg_above_64_field32_read(unit, ihp_counter_in_lif_range_0r, SOC_CORE_ALL, 0, ihp_counter_in_lif_range_low_0f, &range->start);
            SOCDNX_IF_ERR_EXIT(rv);

            rv = soc_reg_above_64_field32_read(unit, ihp_counter_in_lif_range_0r, SOC_CORE_ALL, 0, ihp_counter_in_lif_range_high_0f, &range->end);
            SOCDNX_IF_ERR_EXIT(rv);

        } else if (range_id == 1) {
            rv = soc_reg_above_64_field32_read(unit, ihp_counter_in_lif_range_1r, SOC_CORE_ALL, 0, ihp_counter_in_lif_range_low_1f, &range->start);
            SOCDNX_IF_ERR_EXIT(rv);

            rv = soc_reg_above_64_field32_read(unit, ihp_counter_in_lif_range_1r, SOC_CORE_ALL, 0, ihp_counter_in_lif_range_high_1f, &range->end);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    } else if (source == SOC_TMC_CNT_SRC_TYPE_EPNI) {
        uint64 data64;
        COMPILER_64_ZERO(data64);
        if (range_id == 0) {
            rv = READ_EPNI_COUNTER_OUT_LIF_RANGE_0r(unit, SOC_CORE_ALL, &data64);
            SOCDNX_IF_ERR_EXIT(rv);

            range->start = soc_reg64_field32_get(unit, EPNI_COUNTER_OUT_LIF_RANGE_0r, data64, COUNTER_OUT_LIF_RANGE_0_LOWf); 
            range->end =   soc_reg64_field32_get(unit, EPNI_COUNTER_OUT_LIF_RANGE_0r, data64, COUNTER_OUT_LIF_RANGE_0_HIGHf);

        } else if (range_id == 1) {
            rv = READ_EPNI_COUNTER_OUT_LIF_RANGE_1r(unit, SOC_CORE_ALL, &data64);
            SOCDNX_IF_ERR_EXIT(rv);

            range->start = soc_reg64_field32_get(unit, EPNI_COUNTER_OUT_LIF_RANGE_1r, data64, COUNTER_OUT_LIF_RANGE_1_LOWf); 
            range->end =   soc_reg64_field32_get(unit, EPNI_COUNTER_OUT_LIF_RANGE_1r, data64, COUNTER_OUT_LIF_RANGE_1_HIGHf);
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid source type %d.\n"), source));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}




int arad_epni_counter_base_set(int unit, int src_core, int base_offset_field_val, int command_id, SOC_TMC_CNT_MODE_EG_TYPE type, SOC_TMC_CNT_SRC_TYPE source)
{
    int rv = SOC_E_NONE;
    
    SOCDNX_INIT_FUNC_DEFS

    if (command_id == 0) {
        switch (type) {
        case ARAD_CNT_MODE_EG_TYPE_PMF:
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "PMF belong to EGQ and not EPNI.\n")));
            rv = SOC_E_PARAM;
            break;
        case SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT:
        case SOC_TMC_CNT_MODE_EG_TYPE_TM:
            rv = soc_reg_above_64_field32_modify(unit, EPNI_COUNTER_0_BASEr, src_core, 0, COUNTER_0_MODE_1_BASEf, ARAD_EPNI_BASE_VAL_TM); 
            SOCDNX_IF_ERR_EXIT(rv);
            break;
        case ARAD_CNT_MODE_EG_TYPE_OUTLIF:
            rv = soc_reg_above_64_field32_modify(unit, EPNI_COUNTER_0_BASEr, src_core, 0, COUNTER_0_MODE_2_BASEf, base_offset_field_val);
            SOCDNX_IF_ERR_EXIT(rv);
            break;
        case ARAD_CNT_MODE_EG_TYPE_VSI:
            rv = soc_reg_above_64_field32_modify(unit, EPNI_COUNTER_0_BASEr, src_core, 0, COUNTER_0_MODE_3_BASEf, base_offset_field_val);
            SOCDNX_IF_ERR_EXIT(rv);
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid mode %s for source %s.\n"), SOC_TMC_CNT_MODE_EG_TYPE_to_string(type), SOC_TMC_CNT_SRC_TYPE_to_string(source)));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }

        if(rv != SOC_E_NONE)
        {
            SOCDNX_IF_ERR_EXIT(rv);
        }        
    } else {
        switch (type) {
        case ARAD_CNT_MODE_EG_TYPE_PMF:
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "PMF belong to EGQ and not EPNI.\n")));
            rv = SOC_E_PARAM;
            break;
        case SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT:
        case SOC_TMC_CNT_MODE_EG_TYPE_TM:
            rv = soc_reg_above_64_field32_modify(unit, EPNI_COUNTER_1_BASEr, src_core, 0, COUNTER_1_MODE_1_BASEf, ARAD_EPNI_BASE_VAL_TM); 
            SOCDNX_IF_ERR_EXIT(rv);
            break;
        case ARAD_CNT_MODE_EG_TYPE_OUTLIF:
            rv = soc_reg_above_64_field32_modify(unit, EPNI_COUNTER_1_BASEr, src_core, 0, COUNTER_1_MODE_2_BASEf, base_offset_field_val);
            SOCDNX_IF_ERR_EXIT(rv);
            break;
        case ARAD_CNT_MODE_EG_TYPE_VSI:
            rv = soc_reg_above_64_field32_modify(unit, EPNI_COUNTER_1_BASEr, src_core, 0, COUNTER_1_MODE_3_BASEf, base_offset_field_val);
            SOCDNX_IF_ERR_EXIT(rv);
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid mode %s for source %s.\n"), SOC_TMC_CNT_MODE_EG_TYPE_to_string(type), SOC_TMC_CNT_SRC_TYPE_to_string(source)));
            rv = SOC_E_PARAM;
            break;
        }

        if(rv != SOC_E_NONE)
        {
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

    
exit:
    SOCDNX_FUNC_RETURN;        
}



uint32 
   arad_cnt_base_val_set(
      SOC_SAND_IN int                       unit,
      SOC_SAND_IN SOC_TMC_CNT_PROCESSOR_ID  processor_ndx,
      SOC_SAND_IN SOC_TMC_CNT_COUNTERS_INFO *info
   )
{
    int                  rv = SOC_E_NONE;
    SOC_TMC_CNT_SRC_TYPE source;
    int                  src_core;
    int                  command_id;
    uint32               base_val;
    int base_offset_field_val;
    int base_counter_we_val;
    ARAD_CNT_CRPS_IQM_CMD crps_iqm_cmd;
    uint32 top_q;
    int format_scale = (info->format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS) ? 2 : 1;    
    
    SOCDNX_INIT_FUNC_DEFS;
    source = info->src_type;
    src_core = info->src_core;
    command_id = info->command_id;
    base_val = info->mode_eg.base_val;    
    
    if (command_id != 0 && command_id != 1) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid command ID %d.\n"), command_id));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (src_core < 0 || src_core >= SOC_DPP_DEFS_GET(unit, nof_cores)) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Invalid source core ID %d.\n"), src_core));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if (source == SOC_TMC_CNT_SRC_TYPE_EPNI || 
        source == SOC_TMC_CNT_SRC_TYPE_ING_PP) {
        
        base_offset_field_val =  
            (((info->num_counters*format_scale) - ((base_val * info->custom_mode_params.set_size) % (info->num_counters*format_scale))) % (info->num_counters*format_scale) ) / info->custom_mode_params.set_size;
                    
        base_counter_we_val = 
            ((base_val * info->custom_mode_params.set_size) / ((info->num_counters)*format_scale));

        if (source == SOC_TMC_CNT_SRC_TYPE_EPNI) 
        {
            if(SOC_IS_JERICHO_PLUS(unit)) 
            {
                SOCDNX_IF_ERR_EXIT(qax_cnt_epni_counter_base_set(unit, src_core, base_offset_field_val, command_id, info->mode_eg.type, source));
            }
            else
            {
                SOCDNX_IF_ERR_EXIT(arad_epni_counter_base_set(unit, src_core, base_offset_field_val, command_id, info->mode_eg.type, source));                
            }
            if((info->mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT) || (info->mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_TM))
            {
                base_offset_field_val = 0;
                base_counter_we_val = 0;
            }
        } else if (source == SOC_TMC_CNT_SRC_TYPE_ING_PP) {
            rv = arad_pmf_counter_set(unit, command_id, info->mode_eg.type, base_offset_field_val);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        if (SOC_IS_JERICHO(unit)) {
            uint32 max_we_val = 0;
            int cnt_bit_num = (base_offset_field_val == 0) ? (info->we_val + base_counter_we_val) : (info->we_val + base_counter_we_val + 1);
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_max_we_val_get, (unit, info->src_type, &max_we_val));
            SOCDNX_IF_ERR_EXIT(rv);
            if (cnt_bit_num > max_we_val) {
                LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "For counter processor %u, cnt_bit_num(%d) is greater than the max value(%d)\n"), processor_ndx, cnt_bit_num, max_we_val));
                SOCDNX_IF_ERR_EXIT(SOC_E_INTERNAL);
            }
            rv = soc_reg_above_64_field32_modify(unit, CRPS_CRPS_CNT_SRC_CFGr, REG_PORT_ANY, processor_ndx, CRPS_N_CNT_BIT_NUMf, cnt_bit_num);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    } 
    else 
    {    
        if (source == SOC_TMC_CNT_SRC_TYPE_VOQ) 
        {
            sal_memset(&crps_iqm_cmd, 0, sizeof(ARAD_CNT_CRPS_IQM_CMD));
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_crps_iqm_cmd_get,(unit, processor_ndx, command_id, &crps_iqm_cmd)));                            
            rv = soc_reg_above_64_field32_modify(unit, crps_iqm_cmd.src_cfg_r, src_core, 0, crps_iqm_cmd.queue_shift_f, arad_cnt_convert_enum_val2_queue_shift(info->q_set_size));
            SOCDNX_IF_ERR_EXIT(rv);
            
            rv = soc_reg_above_64_field32_modify(unit, crps_iqm_cmd.cfg_1_r, src_core, 0, crps_iqm_cmd.base_q_f, info->mode_eg.base_val);
            SOCDNX_IF_ERR_EXIT(rv);
            
            
            top_q = (0x1 << soc_reg_field_length(unit, crps_iqm_cmd.cfg_2_r, crps_iqm_cmd.top_q_f)) - 1;
            if (!SOC_IS_JERICHO(unit)) {
                                
                if (top_q > (info->mode_eg.base_val + (info->num_sets * info->q_set_size) - 1))
                    {
                        top_q = info->mode_eg.base_val + (info->num_sets * info->q_set_size) - 1;
                    }         
            }
            rv = soc_reg_above_64_field32_modify(unit, crps_iqm_cmd.cfg_2_r, src_core, 0, crps_iqm_cmd.top_q_f, top_q);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        if (SOC_IS_JERICHO(unit))
        {
            if (!SOC_IS_QAX(unit) || (SOC_IS_QAX(unit) && (processor_ndx < SOC_DPP_DEFS_GET(unit, nof_counter_processors))))
            {
                rv = soc_reg_above_64_field32_modify(unit, CRPS_CRPS_CNT_SRC_CFGr, REG_PORT_ANY, processor_ndx, CRPS_N_CNT_BIT_NUMf, info->we_val); 
                SOCDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 
   arad_cnt_epni_statistics_enable(
      SOC_SAND_IN int                     unit,
      SOC_SAND_IN int                     enable)
{
    uint32
      res = SOC_SAND_OK;
    int
        value;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    if(enable != 0 && enable != 1) {
        LOG_ERROR(BSL_LS_BCM_CNT,
                  (BSL_META_U(unit, "unit %d valid enable is 0 to disable or 1 to enable \n"),unit));
    }
    value = enable ? 0xD : 0x0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_GLOBAL_CONFIGr, SOC_CORE_ALL, 0, STATISTICS_ENAf, value));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_epni_statistics_enable()", 0, 0);
}


uint32 
   arad_cnt_epni_regs_set_unsafe(
      SOC_SAND_IN int                     unit,
      SOC_SAND_IN ARAD_CNT_SRC_TYPE       src_type,
      SOC_SAND_IN int                     src_core, 
      SOC_SAND_IN ARAD_CNT_MODE_EG_TYPES  eg_mode_type,
      SOC_SAND_IN int                     command_id,
      SOC_SAND_IN int                     crps_config)
{
    uint32
      data[20],
      tm_out_port_selector = 0,
      fld_val = 0,
      pp_port_ndx = 0,
      res = SOC_SAND_OK;
    soc_field_t 
        base_val_fld;
    uint32 is_counter_0 = FALSE;
    SOC_SAND_U32_RANGE range;
#ifdef BCM_88650_B0
       
#endif 
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    is_counter_0 = (command_id == 0);
    switch (eg_mode_type) {
    case ARAD_CNT_MODE_EG_TYPE_TM:
      fld_val = (SOC_IS_JERICHO_PLUS(unit) ? QAX_CNT_MODE_EG_TYPE_TM_FLD_VAL : ARAD_CNT_MODE_EG_TYPE_TM_FLD_VAL);
      tm_out_port_selector = 1; 
      break;
    case ARAD_CNT_MODE_EG_TYPE_VSI:
      fld_val = (SOC_IS_JERICHO_PLUS(unit) ? QAX_CNT_MODE_EG_TYPE_VSI_FLD_VAL : ARAD_CNT_MODE_EG_TYPE_VSI_FLD_VAL);
      break;
    case ARAD_CNT_MODE_EG_TYPE_OUTLIF:
      fld_val = (SOC_IS_JERICHO_PLUS(unit) ? QAX_CNT_MODE_EG_TYPE_OUTLIF_FLD_VAL : ARAD_CNT_MODE_EG_TYPE_OUTLIF_FLD_VAL);
      break;
    case ARAD_CNT_MODE_EG_TYPE_TM_PORT:
      fld_val = (SOC_IS_JERICHO_PLUS(unit) ? QAX_CNT_MODE_EG_TYPE_TM_FLD_VAL : ARAD_CNT_MODE_EG_TYPE_TM_FLD_VAL);
      tm_out_port_selector = 0; 
      break;
    default:
      break;
    }
    
    if (eg_mode_type == ARAD_CNT_MODE_EG_TYPE_TM || 
        eg_mode_type == ARAD_CNT_MODE_EG_TYPE_TM_PORT) {
        soc_reg_t counter_port_tm_reg;
        if ((SOC_IS_JERICHO(unit)) || (SOC_IS_ARAD_B0_AND_ABOVE(unit))) {
            counter_port_tm_reg = EPNI_STATISTICS_REPORTING_CONFIGURATIONr;
        } else {
            counter_port_tm_reg = EPNI_COUNTER_PORT_TMr;
        }
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, counter_port_tm_reg, src_core, 0, COUNTER_PORT_TMf,  tm_out_port_selector));
        if (SOC_IS_JERICHO(unit) && !SOC_IS_JERICHO_PLUS(unit)) {
            
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, counter_port_tm_reg, src_core, 0, TM_POINTER_MASKf,  0x5FFF));
        }
    }

    for(pp_port_ndx = 0;  pp_port_ndx < SOC_TMC_NOF_FAP_PORTS_PER_CORE;  pp_port_ndx++)
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 39, exit, READ_EPNI_PP_COUNTER_TABLEm(unit, EPNI_BLOCK(unit, src_core), pp_port_ndx, data));

        
        if (SOC_IS_JERICHO_PLUS(unit))
        {
            
            if(crps_config == FALSE && SOC_IS_QAX(unit))
            {
                base_val_fld = (is_counter_0) ? COUNTER_MODE_2f : COUNTER_MODE_3f;                        
            }
            else
            {
                base_val_fld = (is_counter_0) ? COUNTER_MODE_0f : COUNTER_MODE_1f;                        
            }
        }
        else
        {
            base_val_fld = (is_counter_0) ? COUNTER_0_MODEf : COUNTER_1_MODEf;        
        }        
        soc_EPNI_PP_COUNTER_TABLEm_field32_set(unit, data, base_val_fld, fld_val);
        if (SOC_IS_JERICHO_PLUS(unit))
        {
            if(crps_config == FALSE && SOC_IS_QAX(unit))
            {
                base_val_fld = PORT_OFFSET_2f;
            }
            else
            {
                base_val_fld = (is_counter_0) ? PORT_OFFSET_0f : PORT_OFFSET_1f;
            }
        }
        else
        {
            base_val_fld = (is_counter_0)? COUNTER_0_POINTER_BASEf:COUNTER_1_POINTER_BASEf;     
        }        
        soc_EPNI_PP_COUNTER_TABLEm_field32_set(unit, data, base_val_fld, 0);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_EPNI_PP_COUNTER_TABLEm(unit, EPNI_BLOCK(unit, src_core), pp_port_ndx, data));            

    }
    if (!SOC_IS_ARADPLUS_AND_BELOW(unit) && !SOC_IS_JERICHO_PLUS(unit)) {
        
        if (SOC_DPP_CONFIG(unit)->arad->init.stat_if.stat_if_etpp_mode != 1 && 
            SOC_DPP_CONFIG(unit)->arad->init.stat_if.stat_if_etpp_mode != -1 && 
            crps_config == TRUE)
        {
            LOG_ERROR(BSL_LS_BCM_CNT,
                  (BSL_META_U(unit, "unit %d not in JerichoModeA, but in mode %d - setting the mode to JerichoModeA \n"),
                                    unit, SOC_DPP_CONFIG(unit)->arad->init.stat_if.stat_if_etpp_mode));
        }
        if(crps_config == TRUE)
        {
            res = soc_reg_above_64_field32_modify(unit, EPNI_CFG_PP_LIF_COUNTER_MODEr, src_core, 0, CFG_PP_LIF_COUNTER_MODEf, 1);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);         
        }

        
        
        
        if((crps_config == FALSE) && (eg_mode_type == ARAD_CNT_MODE_EG_TYPE_OUTLIF))
        {        
            range.start = 0;
            range.end = 0x3FFFF;
            res = arad_cnt_lif_counting_range_set(unit, src_type, command_id, &range, TRUE, FALSE, 0);
            SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);
        }
    }
    
    if (crps_config == FALSE && (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit))) {
         res = arad_cnt_epni_statistics_enable(unit,TRUE);
         SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1257, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_epni_regs_set_unsafe()", 0, 0);
}


uint32
  arad_cnt_counters_set_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID          processor_ndx,
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO         *info
  )
{
  int 
      command_id, second_cmd_id, i,
      src_core;
  uint32
      data[20],
    proc_id,
    group_size_val=0,
      nof_counters = 0,
      one_entry_mode_cnt = 0,
    src_type=0,
     
    fld_val = 0,
    res = SOC_SAND_OK;
  uint32
    pp_port_ndx;
  ARAD_CNT_CRPS_IQM_CMD 
      crps_iqm_cmd;
  soc_field_t
      group_size_field = INVALIDf;
  uint32 
      format_field_val;
  uint64 data64;
  soc_field_t src_field_name = INVALIDf;
  soc_field_t src_bubble_field_name;
  soc_field_t crps_cnt_src_mode_fiels = -1;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_COUNTERS_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);
  sal_memset(&crps_iqm_cmd, 0, sizeof(ARAD_CNT_CRPS_IQM_CMD));

  COMPILER_64_ZERO(data64);
#ifdef BCM_88650_B0
 if (!SOC_UNIT_NUM_VALID(unit)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 3, exit);
  }
#endif

 res = arad_cnt_get_processor_id(
         unit,
         processor_ndx,
         &proc_id
       );
 SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

  src_core = info->src_core;
  command_id = info->command_id;

  res = arad_cnt_get_source_id(
          unit,
          info->src_type,
          proc_id,      
          src_core,
          command_id,
          info->we_val,
          &src_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);
  res = arad_cnt_ingress_params_get(unit, info, &group_size_val, &nof_counters, &one_entry_mode_cnt);
  SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);

  if (SOC_IS_QAX(unit))
  {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 9, exit, ARAD_REG_ACCESS_ERR, qax_cnt_get_field_name_per_src_type(unit, info->src_type, info->command_id, &src_field_name, &src_bubble_field_name, &group_size_field));
      if(src_field_name != INVALIDf)
      {
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg_set(unit, CRPS_CRPS_SRC_MASKr, REG_PORT_ANY, proc_id, data64));
      }

      if(src_bubble_field_name != INVALIDf)
      {
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 112, exit, soc_reg_set(unit, CRPS_CRPS_CNT_SRC_CFGr, REG_PORT_ANY, proc_id, data64));                      
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 12, exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, CRPS_CRPS_CNT_SRC_CFGr, REG_PORT_ANY, proc_id, src_bubble_field_name, TRUE));    
      }      
  }
  else
  {
      res = _arad_cnt_group_size_field_by_src_type_get(unit, proc_id, src_core, info->src_type, command_id, &group_size_field);
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);   
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 15, exit, ARAD_REG_ACCESS_ERR, arad_cnt_base_val_set(unit, proc_id, info));

  if (group_size_field != INVALIDf) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 16, exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, CRPS_CRPS_CNT_SRC_GROUP_SIZESr, REG_PORT_ANY, 0, group_size_field, group_size_val));
            
  }
  
  if ((info->src_type == ARAD_CNT_SRC_TYPE_VOQ) || (info->src_type == ARAD_CNT_SRC_TYPE_STAG) || 
      (info->src_type == ARAD_CNT_SRC_TYPE_INGRESS_EXT_STAT) ||
      (info->src_type == ARAD_CNT_SRC_TYPE_VSQ) || (info->src_type == ARAD_CNT_SRC_TYPE_CNM_ID)){
      res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_crps_iqm_cmd_get,(unit, proc_id, command_id, &crps_iqm_cmd));                            
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 17, exit);
      
      if(SOC_IS_QAX(unit) && info->src_type == SOC_TMC_CNT_SRC_TYPE_INGRESS_EXT_STAT)
      {
        fld_val = 1;
      }   
      else
      {
        fld_val = info->src_type - ARAD_CNT_SRC_TYPE_VOQ;
      }
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 18, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, crps_iqm_cmd.src_cfg_r, src_core, 0, crps_iqm_cmd.src_type_f,  fld_val));
  }
  
  if (info->src_type == ARAD_CNT_SRC_TYPE_STAG) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1240,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_TAG_CONFIGURATIONr, src_core, 0, STAT_TAG_CRPS_CMD_LSBf, info->stag_lsb));
  }
  
#ifdef BCM_88650_B0
  if (SOC_IS_ARAD_B0_AND_ABOVE(unit) && !SOC_IS_JERICHO(unit)) {
      soc_field_t one_entry_mode_field = INVALIDf;
      res = _arad_cnt_one_entry_field_by_src_type_get(unit, proc_id, info->src_type, info->command_id, &one_entry_mode_field);
      SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);
      if (one_entry_mode_field != INVALIDf){
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 20, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_ONE_ENTRY_MODE_CNT_CMDr, REG_PORT_ANY, 0, one_entry_mode_field, one_entry_mode_cnt));
      }
  }
#endif
  
  if (SOC_IS_JERICHO(unit) || (SOC_IS_ARADPLUS(unit) && (info->mode_statistics == ARAD_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS))) {
      res = _arad_cnt_bmap_configurable_offset_format_set(unit, proc_id, group_size_val, info);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 21, exit);
  }
  if (info->src_type == SOC_TMC_CNT_SRC_TYPE_EPNI){
      res = arad_cnt_epni_regs_set_unsafe(unit, info->src_type, info->src_core, info->mode_eg.type, info->command_id, TRUE);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  }

  
  if(info->src_type == SOC_TMC_CNT_SRC_TYPE_EGR_PP || info->src_type == SOC_TMC_CNT_SRC_TYPES_EGQ_TM) {
    switch (info->mode_eg.type)
    {
    case ARAD_CNT_MODE_EG_TYPE_TM:
      fld_val = ARAD_CNT_MODE_EG_TYPE_EGQ_PROFILE_TM_FLD_VAL;
        break;
    
    case ARAD_CNT_MODE_EG_TYPE_VSI:
      fld_val = ARAD_CNT_MODE_EG_TYPE_EGQ_PROFILE_VSI_FLD_VAL;
      break;

    case ARAD_CNT_MODE_EG_TYPE_OUTLIF:
      fld_val = ARAD_CNT_MODE_EG_TYPE_EGQ_PROFILE_OUTLIF_FLD_VAL;
      break;

    case ARAD_CNT_MODE_EG_TYPE_PMF:
        fld_val = ARAD_CNT_MODE_EG_TYPE_PMF_FLD_VAL;
        break;
    case ARAD_CNT_MODE_EG_TYPE_TM_PORT:
        fld_val = ARAD_CNT_MODE_EG_TYPE_EGQ_PROFILE_TM_PORT_FLD_VAL;
    default:
      break;
    }

    for(pp_port_ndx = 0;  pp_port_ndx < ARAD_PORT_NOF_PP_PORTS;  pp_port_ndx++)
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_EGQ_PP_PPCTm(unit, EGQ_BLOCK(unit, src_core), pp_port_ndx, data));
        if (info->src_type == SOC_TMC_CNT_SRC_TYPES_EGQ_TM) {
            soc_EGQ_PP_PPCTm_field32_set(unit, data, ENABLE_TM_COUNTER_PTRf, 0x1);
        }
        else
        {
            soc_EGQ_PP_PPCTm_field32_set(unit, data, COUNTER_PROFILEf, fld_val);           
        }
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_EGQ_PP_PPCTm(unit, EGQ_BLOCK(unit, src_core), pp_port_ndx, data));
    }
  }

  ARAD_CNT_FORMAT_TO_FLD_VAL(unit, info->format, info->src_type, format_field_val);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1250, exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, CRPS_CRPS_GENERAL_CFGr, REG_PORT_ANY,  processor_ndx, CRPS_N_COUNT_MODEf, format_field_val));
  if (SOC_IS_JERICHO(unit)) {
      
      res = _arad_cnt_src_mode_field_get(unit, src_core, info->src_type, command_id, &crps_cnt_src_mode_fiels);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1253, exit);
      
      res = soc_reg_above_64_field32_modify(unit, CRPS_CRPS_CNT_SRC_MODEr, REG_PORT_ANY, 0, crps_cnt_src_mode_fiels, (info->format != SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS));
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1255, exit);
  }

  
  if(SOC_IS_JERICHO(unit) && info->src_type == SOC_TMC_CNT_SRC_TYPES_IPT_LATENCY) {
      if (soc_property_get(unit, spn_PHY_1588_DPLL_FREQUENCY_LOCK, 0) != 1) {
          LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit, "unit %d Soc property PHY_1588_DPLL_FREQUENCY_LOCK should be configured in order to config Latency feature \n"),unit));
      }
      
      if(info->src_core == 0) {
          if(SOC_IS_QAX(unit)) {
              for(i=0; i<QAX_ITM_NOF_MAX_LATENCY_MEASUREMENTS; i++) {
                  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1201, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ITE_PACKET_LATENCY_MEASUREMENTr, REG_PORT_ANY,  i, CRPS_LATENCY_ENf, 1));
              }
              
              SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 11, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_SRC_MASKr, REG_PORT_ANY, processor_ndx, CRPS_N_IDP_S_MASKf, 1));
          } else {
              SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1201, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_PACKET_LATENCY_MEASURE_CFGr, REG_PORT_ANY,  0, CRPS_0_LATENCY_WEf, 1));
          }
      } else { 
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1202, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_PACKET_LATENCY_MEASURE_CFGr, REG_PORT_ANY,  0, CRPS_1_LATENCY_WEf, 1));      
      }
  }

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1203, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify
      (unit, CRPS_CRPS_PREFETCH_TIMERS_CFGr, REG_PORT_ANY,  processor_ndx, CRPS_N_ALGORITHM_TMR_ENf,  !SOC_TMC_CNT_FORMAT_IS_WIDE(info->format, info->src_type)));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1221, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify
        (unit, CRPS_CRPS_PREFETCH_TIMERS_CFGr, REG_PORT_ANY,  processor_ndx, CRPS_N_SEQUENTIAL_TMR_ENf,  !SOC_TMC_CNT_FORMAT_IS_WIDE(info->format, info->src_type)));
  if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1222, exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify
        (unit, CRPS_CRPS_PREFETCH_TIMERS_CFGr, REG_PORT_ANY,  processor_ndx, CRPS_N_OPPORTUNISTIC_TMR_ENf, !SOC_TMC_CNT_FORMAT_IS_WIDE(info->format, info->src_type))); 
  }
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1223, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify
    (unit, CRPS_CRPS_GENERAL_CFGr, REG_PORT_ANY,  processor_ndx, CRPS_N_CACHE_READ_ENf,  !SOC_TMC_CNT_FORMAT_IS_WIDE(info->format, info->src_type)));

  
  if (info->src_type == ARAD_CNT_SRC_TYPE_INGRESS_OAM || info->src_type == ARAD_CNT_SRC_TYPE_EGRESS_OAM)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 11, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_GENERAL_CFGr, REG_PORT_ANY, proc_id, CRPS_N_CNT_STUCK_AT_ONE_ENf, FALSE));        
  }
  else
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 11, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_GENERAL_CFGr, REG_PORT_ANY, proc_id, CRPS_N_CNT_STUCK_AT_ONE_ENf, TRUE));            
  }

  
  if (SOC_IS_QAX(unit))
  {
      if(src_field_name != INVALIDf)
      {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 11, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_SRC_MASKr, REG_PORT_ANY, proc_id, src_field_name, TRUE));    
      }
  }
  else
  {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 13, exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, CRPS_CRPS_CNT_SRC_CFGr, REG_PORT_ANY, proc_id, CRPS_N_CNT_SRCf, src_type));
  }

  
  if(SOC_IS_QAX(unit) && (info->src_type == ARAD_CNT_SRC_TYPE_INGRESS_OAM || info->src_type == ARAD_CNT_SRC_TYPE_EGRESS_OAM) && (info->multiple_sources == TRUE))
  {
    second_cmd_id = command_id == 0 ? 1 : 0;  
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 9, exit, ARAD_REG_ACCESS_ERR, qax_cnt_get_field_name_per_src_type(unit, info->src_type, second_cmd_id, &src_field_name, &src_bubble_field_name, &group_size_field));    
    if(src_field_name != INVALIDf)
    {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 11, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_SRC_MASKr, REG_PORT_ANY, proc_id, src_field_name, TRUE));    
    }
    if (group_size_field != INVALIDf) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 16, exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, CRPS_CRPS_CNT_SRC_GROUP_SIZESr, REG_PORT_ANY, 0, group_size_field, group_size_val));
              
    }    
    res = _arad_cnt_src_mode_field_get(unit, src_core, info->src_type, second_cmd_id, &crps_cnt_src_mode_fiels);    
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1253, exit);
    
    res = soc_reg_above_64_field32_modify(unit, CRPS_CRPS_CNT_SRC_MODEr, REG_PORT_ANY, 0, crps_cnt_src_mode_fiels, (info->format != SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS));
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1255, exit);                
  }
  
  if (info->src_type == SOC_TMC_CNT_SRC_TYPE_EPNI && (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit))) {
        res = arad_cnt_epni_statistics_enable(unit,TRUE);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1257, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_counters_set_unsafe()", 0, 0);
}

uint32
  arad_cnt_dma_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    dma_ndx,
    SOC_SAND_IN  uint8                  channel,
    SOC_SAND_IN  int                    cache_length
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_DMA_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(dma_ndx, SOC_DPP_DEFS_GET(unit, nof_counter_fifos) - 1, ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(channel, NOF_FIFO_DMA_CHANNELS, ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 20, exit);
  if (dma_ndx < SOC_DPP_DEFS_GET(unit, nof_counter_fifos) - 1) {
      SOC_SAND_ERR_IF_ABOVE_MAX(dma_ndx, SOC_DPP_DEFS_GET(unit, counter_fifo_depth), ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 20, exit);
  } else {
      SOC_SAND_ERR_IF_ABOVE_MAX(dma_ndx, 
                                SOC_DPP_DEFS_GET(unit, counter_fifo_depth) + 
                                (SOC_DPP_DEFS_GET(unit, nof_small_counter_processors) * SOC_DPP_DEFS_GET(unit, counters_per_small_counter_processor)), 
                                ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 30, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_dma_set_verify()", 0, 0);
}


uint32
  arad_cnt_dma_unset_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   dma_ndx,
    SOC_SAND_IN  uint8                 channel

  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_DMA_UNSET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(dma_ndx, SOC_DPP_DEFS_GET(unit, nof_counter_fifos) - 1, ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(channel, NOF_FIFO_DMA_CHANNELS, ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 21, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_dma_unset_verify()", 0, 0);
}

uint32
  arad_cnt_counters_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO        *info
  )
{
  uint32 res = SOC_SAND_OK;
  uint32 max_we_val;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_COUNTERS_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(processor_ndx, ARAD_PROCESSOR_NDX_MAX(unit), ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->src_type, ARAD_CNT_COUNTERS_INFO_SRC_TYPE_MAX(unit), ARAD_CNT_SRC_TYPE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->src_core, SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores, ARAD_CNT_SRC_TYPE_OUT_OF_RANGE_ERR, 12, exit);

  if(info->src_type == ARAD_CNT_SRC_TYPE_EPNI)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->mode_eg.type, ARAD_CNT_COUNTERS_INFO_MODE_EG_MAX, ARAD_CNT_MODE_EG_OUT_OF_RANGE_ERR, 13, exit);
  }

  if(info->src_type == ARAD_CNT_SRC_TYPE_VOQ) 
  {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->q_set_size, ARAD_CNT_VOQ_PARAMS_Q_SET_SIZE_MAX, ARAD_CNT_Q_SET_SIZE_OUT_OF_RANGE_ERR , 23, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->mode_eg.base_val, SOC_DPP_DEFS_GET(unit, nof_queues) - 1, ARAD_CNT_START_Q_OUT_OF_RANGE_ERR, 29, exit);
  }

  if(info->src_type == ARAD_CNT_SRC_TYPE_STAG) 
  {
     SOC_SAND_ERR_IF_ABOVE_MAX(info->stag_lsb, ARAD_CNT_COUNTERS_INFO_STAG_MAX, ARAD_CNT_STAG_LSB_OUT_OF_RANGE_ERR, 37 , exit);
  }
  else if(info->stag_lsb != 0) 
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_CNT_DEFINED_LSB_STAG_ERR, 43, exit);
  }

  res = arad_cnt_max_we_val_get(unit, info->src_type, &max_we_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 44, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->we_val, max_we_val, ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_counters_set_verify()", 0, 0);
}

uint32
  arad_cnt_counters_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_COUNTERS_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(processor_ndx, ARAD_PROCESSOR_NDX_MAX(unit), ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_counters_get_verify()", 0, 0);
}


uint32
  arad_cnt_counters_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_OUT ARAD_CNT_COUNTERS_INFO        *info
  )
{
  int 
      src_core;
  uint32
      data,
    res = SOC_SAND_OK,
    proc_id,
    fld_val2 = 0,
    is_configurable_mode = FALSE,
#ifdef BCM_88650_B0
    one_entry_cmd = 0,
#endif
    reg_val,
    fld_val;
  soc_reg_above_64_val_t
    reg_above_64;
  ARAD_CNT_CRPS_IQM_CMD 
      crps_iqm_cmd;
  soc_reg_t
      base_val_reg;
  soc_field_t
      base_val_fld,
      size_fld_source;
  uint64
      field64;
  uint32 
      format_fld_val,
      queue_shift;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_COUNTERS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  ARAD_CNT_COUNTERS_INFO_clear(unit, info);
  sal_memset(&crps_iqm_cmd, 0, sizeof(ARAD_CNT_CRPS_IQM_CMD));

#ifdef BCM_88650_B0
 if (!SOC_UNIT_NUM_VALID(unit)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 10, exit);
  }
#endif
  res = arad_cnt_get_processor_id(
          unit,
          processor_ndx,
          &proc_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  READ_CRPS_CRPS_CNT_SRC_CFGr(unit, proc_id, &reg_val);
  fld_val = soc_reg_field_get(unit, CRPS_CRPS_CNT_SRC_CFGr, reg_val, CRPS_N_CNT_SRCf);
  ARAD_CNT_SRC_TYPE_CORE_GET(unit, fld_val, info->src_core);
  src_core = info->src_core;
  ARAD_CNT_SRC_TYPE_CMD_GET(unit, fld_val, info->command_id);
  if (SOC_IS_JERICHO(unit)) { 
      info->we_val = soc_reg_field_get(unit, CRPS_CRPS_CNT_SRC_CFGr, reg_val, CRPS_N_CNT_BIT_NUMf);
  } else {
      ARAD_CNT_SRC_TYPE_LSB_GET(unit, fld_val, info->we_val);
  }

  READ_CRPS_CRPS_GENERAL_CFGr(unit, proc_id, &reg_val);
  format_fld_val = soc_reg_field_get(unit, CRPS_CRPS_GENERAL_CFGr, reg_val, CRPS_N_COUNT_MODEf);
  ARAD_CNT_FLD_VAL_TO_FORMAT(unit, format_fld_val, info->format);
  if (SOC_IS_JERICHO(unit)) {
      soc_field_t crps_cnt_src_mode_field = -1;
      uint32 mode = 0;
      
      res = _arad_cnt_src_mode_field_get(unit, src_core, info->src_type, info->command_id, &crps_cnt_src_mode_field);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1253, exit);
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(
         res, 1255, exit, ARAD_REG_ACCESS_ERR, 
         soc_reg_above_64_field32_read(unit, CRPS_CRPS_CNT_SRC_MODEr, REG_PORT_ANY, 0, crps_cnt_src_mode_field, &mode));

      if (!mode) {
          info->format = SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS;
      }
  }

  if (ARAD_CNT_SRC_TYPE_IS_EGR_PP_FLD(unit,fld_val)) {
      info->src_type = ARAD_CNT_SRC_TYPE_EGR_PP;
  } else if (ARAD_CNT_SRC_TYPE_IS_IQM_FLD(unit, fld_val)) {
      res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_crps_iqm_cmd_get,(unit, proc_id, info->command_id, &crps_iqm_cmd));                            
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, crps_iqm_cmd.src_cfg_r, src_core, 0, crps_iqm_cmd.src_type_f, &fld_val2));
      info->src_type = ARAD_CNT_SRC_TYPE_VOQ + fld_val2;
      
      if(SOC_IS_QAX(unit) && fld_val2 == 1)
      {
        info->src_type = SOC_TMC_CNT_SRC_TYPE_INGRESS_EXT_STAT;
      }
  } else if (ARAD_CNT_SRC_TYPE_IS_ING_PP_FLD(unit, fld_val)) {
      info->src_type = ARAD_CNT_SRC_TYPE_ING_PP;
  } else if (ARAD_CNT_SRC_TYPE_IS_OAM_FLD(unit, fld_val)) {
      
      info->src_type = ARAD_CNT_SRC_TYPE_INGRESS_OAM;
  } else if (ARAD_CNT_SRC_TYPE_IS_EPNI_FLD_VAL(unit, fld_val)) {
      info->src_type = ARAD_CNT_SRC_TYPE_EPNI;
  } else if (ARAD_CNT_SRC_TYPE_IS_EGQ_TM_FLD(unit, fld_val)) {
      info->src_type = ARAD_CNT_SRC_TYPES_EGQ_TM;
  } else if (ARAD_CNT_SRC_TYPE_IS_IPT_FLD(unit, fld_val)) {
      info->src_type = SOC_TMC_CNT_SRC_TYPES_IPT_LATENCY;
  } else {
      SOC_SAND_SET_ERROR_CODE(ARAD_CNT_SRC_TYPE_OUT_OF_RANGE_ERR, 60, exit);
  }

  if (info->src_type == ARAD_CNT_SRC_TYPE_EPNI) {
    
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, READ_EPNI_PP_COUNTER_TABLEm(unit, EPNI_BLOCK(unit, src_core), 0, &data));

      if (SOC_IS_JERICHO_PLUS(unit))
      {
          base_val_fld = (info->command_id == 0) ? COUNTER_MODE_0f : COUNTER_MODE_1f;        
      }
      else
      {
          base_val_fld = (info->command_id == 0) ? COUNTER_0_MODEf : COUNTER_1_MODEf;        
      }

      fld_val = soc_EPNI_PP_COUNTER_TABLEm_field32_get(unit, &data, base_val_fld);

      if(SOC_IS_JERICHO_PLUS(unit))
      {
          
          switch (fld_val)
          {
          case QAX_CNT_MODE_EG_TYPE_TM_FLD_VAL:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  71,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_COUNTERS_BASEr, src_core, 0, PORT_COUNTER_BASE_POINTERf, &fld_val2));
            info->mode_eg.type = ARAD_CNT_MODE_EG_TYPE_TM;
            info->mode_eg.base_val = fld_val2;
          break;                   
          case QAX_CNT_MODE_EG_TYPE_VSI_FLD_VAL:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  73,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EPNI_COUNTERS_BASEr, src_core, 0, VSI_COUNTER_BASE_POINTERf, &fld_val2));
            info->mode_eg.type = ARAD_CNT_MODE_EG_TYPE_VSI;
            info->mode_eg.base_val = fld_val2*2048; 
            break;
          }                
      }
      else
      {
          base_val_reg = (info->command_id == 0) ? EPNI_COUNTER_0_BASEr : EPNI_COUNTER_1_BASEr;
          base_val_fld = (info->command_id == 0) ? COUNTER_0_MODE_0_BASEf : COUNTER_1_MODE_0_BASEf;
          switch (fld_val)
          {
          case ARAD_CNT_MODE_EG_TYPE_TM_FLD_VAL:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  71,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, base_val_reg, src_core, 0, base_val_fld + (COUNTER_0_MODE_1_BASEf - COUNTER_0_MODE_0_BASEf), &field64));
            info->mode_eg.type = ARAD_CNT_MODE_EG_TYPE_TM;
            COMPILER_64_TO_32_LO(info->mode_eg.base_val, field64);
          break;
          
          case ARAD_CNT_MODE_EG_TYPE_OUTLIF_FLD_VAL:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  72,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, base_val_reg, src_core, 0, base_val_fld + (COUNTER_0_MODE_3_BASEf - COUNTER_0_MODE_0_BASEf), &field64));
            info->mode_eg.type = ARAD_CNT_MODE_EG_TYPE_OUTLIF;
            info->mode_eg.base_val = fld_val;
          break;
          
          case ARAD_CNT_MODE_EG_TYPE_VSI_FLD_VAL:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  73,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, base_val_reg, src_core, 0, base_val_fld + (COUNTER_0_MODE_2_BASEf - COUNTER_0_MODE_0_BASEf), &field64));
            info->mode_eg.type = ARAD_CNT_MODE_EG_TYPE_VSI;
            COMPILER_64_TO_32_LO(info->mode_eg.base_val, field64);
            break;
          }        
      }
  }

  
  res = _arad_cnt_group_size_field_by_src_type_get(unit, proc_id, src_core, info->src_type, info->command_id, &size_fld_source);
  SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);


  if (size_fld_source != INVALIDf) {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 76, exit, READ_CRPS_CRPS_CNT_SRC_GROUP_SIZESr(unit, reg_above_64));
    fld_val = soc_reg_above_64_field32_get(unit, CRPS_CRPS_CNT_SRC_GROUP_SIZESr, reg_above_64, size_fld_source);
  } else {
      
      switch(info->src_type){
        case ARAD_CNT_SRC_TYPE_INGRESS_OAM:
        case ARAD_CNT_SRC_TYPE_EGRESS_OAM:
            fld_val = ARAD_CNT_MODE_STATISTICS_NO_COLOR_FLD_VAL;
            break;
        default:
          SOC_SAND_SET_ERROR_CODE(ARAD_CNT_ING_MODE_OUT_OF_RANGE_ERR, 77, exit);
      }
  }


#ifdef BCM_88650_B0
  if (SOC_IS_ARAD_B0_AND_ABOVE(unit) && !SOC_IS_JERICHO(unit)){
    soc_field_t one_entry_fld_source = INVALIDf;

    res = _arad_cnt_one_entry_field_by_src_type_get(unit, proc_id, info->src_type, info->command_id, &one_entry_fld_source);
    SOC_SAND_CHECK_FUNC_RESULT(res, 232, exit);
    if(one_entry_fld_source != INVALIDf){
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CRPS_CRPS_ONE_ENTRY_MODE_CNT_CMDr, REG_PORT_ANY, 0, one_entry_fld_source, &one_entry_cmd));
    }
  }
#endif


  if (SOC_IS_ARADPLUS(unit)){
    soc_field_t do_not_count_field = INVALIDf;
    uint64 do_not_count_val;

    COMPILER_64_ZERO(do_not_count_val);
    
    res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_do_not_count_field_by_src_type_get,(unit, proc_id, info->src_core, info->src_type, info->command_id, &do_not_count_field));                                
    SOC_SAND_CHECK_FUNC_RESULT(res, 233, exit); 
    if(do_not_count_field != INVALIDf){
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  234,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, CRPS_CRPS_DO_NOT_COUNT_FILTERr, REG_PORT_ANY, 0, do_not_count_field, &do_not_count_val));
        
        if (COMPILER_64_LO(do_not_count_val) == ARAD_CNT_DO_NOT_COUNT_VAL || (fld_val+1) == ARAD_CNT_BMAP_OFFSET_COUNT) {
          is_configurable_mode = TRUE;
          info->mode_statistics = ARAD_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS;
          info->custom_mode_params.set_size = fld_val + 1;
    }
  }
  }

  if(!is_configurable_mode){
    
    switch(fld_val)
    {
    case ARAD_CNT_MODE_STATISTICS_NO_COLOR_FLD_VAL:
      info->mode_statistics = ARAD_CNT_MODE_STATISTICS_NO_COLOR;
      break;

    case ARAD_CNT_MODE_STATISTICS_COLOR_RES_LOW_FLD_VAL:
      info->mode_statistics = ARAD_CNT_MODE_STATISTICS_COLOR_RES_LOW;
      break;

    case ARAD_CNT_MODE_STATISTICS_COLOR_RES_HI_FLD_VAL:
      info->mode_statistics = ARAD_CNT_MODE_STATISTICS_COLOR_RES_HI;
      break;

    case ARAD_CNT_MODE_STATISTICS_ONE_ENTRY_FLD_VAL:
      info->mode_statistics = ARAD_CNT_MODE_STATISTICS_ALL_NO_COLOR;
#ifdef BCM_88650_B0
      if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
          if (one_entry_cmd == ARAD_CNT_ONE_ENTRY_MODE_ALL_VAL) {
              info->mode_statistics = ARAD_CNT_MODE_STATISTICS_ALL_NO_COLOR;
          } else if (one_entry_cmd == ARAD_CNT_ONE_ENTRY_MODE_DROP_VAL) {
              info->mode_statistics = ARAD_CNT_MODE_STATISTICS_DROP_NO_COLOR;
          } else if (one_entry_cmd == ARAD_CNT_ONE_ENTRY_MODE_FWD_VAL) {
              info->mode_statistics = ARAD_CNT_MODE_STATISTICS_FWD_NO_COLOR;
          } else {
              info->mode_statistics = ARAD_CNT_MODE_STATISTICS_ALL_NO_COLOR;
          }
      }
#endif
      break;

    case ARAD_CNT_MODE_STATISTICS_COLOR_ENQ_HI_FLD_VAL:
      info->mode_statistics = ARAD_CNT_MODE_STATISTICS_COLOR_ENQ_HI;
#ifdef BCM_88660_A0
      
      if(SOC_IS_ARADPLUS(unit)){
        if (one_entry_cmd == ARAD_CNT_ONE_ENTRY_MODE_DROP_VAL) {
          info->mode_statistics = ARAD_CNT_MODE_STATISTICS_DROP_SIMPLE_COLOR;
        } else if (one_entry_cmd == ARAD_CNT_ONE_ENTRY_MODE_FWD_VAL) {
            info->mode_statistics = ARAD_CNT_MODE_STATISTICS_FWD_SIMPLE_COLOR;
        }
      }
      break;
    
    case ARAD_CNT_MODE_STATISTICS_PKTS_AND_PKTS_FLD_VAL:
      if(SOC_IS_ARAD_B1_AND_BELOW(unit)){
        SOC_SAND_SET_ERROR_CODE(ARAD_CNT_ING_MODE_OUT_OF_RANGE_ERR, 105, exit);
      }
      info->mode_statistics = ARAD_CNT_MODE_STATISTICS_NO_COLOR;
#endif
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_CNT_ING_MODE_OUT_OF_RANGE_ERR, 110, exit);
    }
  }

  if (info->src_type == ARAD_CNT_SRC_TYPE_VOQ)
  {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, crps_iqm_cmd.src_cfg_r, src_core, 0, crps_iqm_cmd.queue_shift_f, &queue_shift));
      info->q_set_size = 1 << queue_shift;

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, crps_iqm_cmd.cfg_1_r, src_core, 0, crps_iqm_cmd.base_q_f, &info->mode_eg.base_val));
  }

  if( info->src_type == ARAD_CNT_SRC_TYPE_STAG )
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_STATISTICS_TAG_CONFIGURATIONr, src_core, 0, STAT_TAG_CRPS_CMD_LSBf, &info->stag_lsb));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_counters_get_unsafe()", 0, 0);
}


uint32
  arad_cnt_status_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_OUT ARAD_CNT_STATUS               *proc_status
  )
{
  uint32
    res = SOC_SAND_OK;
   
    
  uint32
    reg_val,
    fld_val;
  ARAD_CNT_COUNTERS_INFO
    counter_info;
  uint32
    cnt_ndx;
  soc_field_t
      invalid_ad_acc_fld[SOC_DPP_DEFS_MAX(NOF_COUNTER_PROCESSORS) + SOC_DPP_DEFS_MAX(NOF_SMALL_COUNTER_PROCESSORS)],
      preread_fifo_full_fld[SOC_DPP_DEFS_MAX(NOF_COUNTER_PROCESSORS) + SOC_DPP_DEFS_MAX(NOF_SMALL_COUNTER_PROCESSORS)];
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(proc_status);

  ARAD_CNT_STATUS_clear(proc_status);
  if (SOC_IS_JERICHO(unit)) {
      invalid_ad_acc_fld[0]  = CRPS_0_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[1]  = CRPS_1_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[2]  = CRPS_2_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[3]  = CRPS_3_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[4]  = CRPS_4_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[5]  = CRPS_5_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[6]  = CRPS_6_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[7]  = CRPS_7_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[8]  = CRPS_8_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[9]  = CRPS_9_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[10] = CRPS_10_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[11] = CRPS_11_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[12] = CRPS_12_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[13] = CRPS_13_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[14] = CRPS_14_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[15] = CRPS_15_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[16] = CRPS_16_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[17] = CRPS_17_INVLD_PTR_ACCf;

      preread_fifo_full_fld[0]  = CRPS_0_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[1]  = CRPS_1_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[2]  = CRPS_2_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[3]  = CRPS_3_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[4]  = CRPS_4_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[5]  = CRPS_5_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[6]  = CRPS_6_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[7]  = CRPS_7_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[8]  = CRPS_8_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[9]  = CRPS_9_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[10] = CRPS_10_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[11] = CRPS_11_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[12] = CRPS_12_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[13] = CRPS_13_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[14] = CRPS_14_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[15] = CRPS_15_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[16] = CRPS_16_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[17] = CRPS_17_PRE_READ_FIFO_FULLf;
  } else {
      invalid_ad_acc_fld[0] = CRPS_0_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[1] = CRPS_1_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[2] = CRPS_2_INVLD_PTR_ACCf;
      invalid_ad_acc_fld[3] = CRPS_3_INVLD_PTR_ACCf;

      preread_fifo_full_fld[0] = CRPS_0_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[1] = CRPS_1_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[2] = CRPS_2_PRE_READ_FIFO_FULLf;
      preread_fifo_full_fld[3] = CRPS_3_PRE_READ_FIFO_FULLf;
  }

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CRPS_INTERRUPT_REGISTERr, REG_PORT_ANY, 0, invalid_ad_acc_fld[processor_ndx], &fld_val));
  proc_status->is_cnt_id_invalid = SOC_SAND_NUM2BOOL(fld_val);
  
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CRPS_INTERRUPT_REGISTERr, REG_PORT_ANY, 0, preread_fifo_full_fld[processor_ndx], &fld_val));
  proc_status->is_cache_full = SOC_SAND_NUM2BOOL(fld_val);
  
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CRPS_CRPS_COUNTER_STS_1r, REG_PORT_ANY,  processor_ndx, FIELD_16_30f, &fld_val));
  proc_status->nof_active_cnts = fld_val;

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  45,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CRPS_CRPS_COUNTER_STS_1r, REG_PORT_ANY,  processor_ndx, CRPS_N_OVTH_CNTRS_CNTf, &fld_val));
  proc_status->overflow_cnt.is_overflow = SOC_SAND_NUM2BOOL(fld_val);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  46,  exit, ARAD_REG_ACCESS_ERR,READ_CRPS_CRPS_CNT_OVRF_STSr(unit,  processor_ndx, &reg_val));
  ARAD_FLD_FROM_REG(CRPS_CRPS_CNT_OVRF_STSr, CRPS_N_COUNTER_OVF_ADDRf, fld_val, reg_val, 50, exit);

  ARAD_CNT_COUNTERS_INFO_clear(unit, &counter_info);
  res = arad_cnt_counters_get_unsafe(
    unit,
    processor_ndx,
    &counter_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = arad_cnt_id_compute(
    unit,
    &counter_info,
    fld_val,
    &cnt_ndx
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  proc_status->overflow_cnt.last_cnt_id = cnt_ndx;

  ARAD_FLD_FROM_REG(CRPS_CRPS_CNT_OVRF_STSr, CRPS_N_IS_OVF_CNT_OCTSf, fld_val, reg_val, 50, exit);
  proc_status->overflow_cnt.is_byte_overflow = SOC_SAND_NUM2BOOL(fld_val);

  ARAD_FLD_FROM_REG(CRPS_CRPS_CNT_OVRF_STSr, CRPS_N_IS_OVF_CNT_PKTSf, fld_val, reg_val, 60, exit);
  proc_status->overflow_cnt.is_pckt_overflow = SOC_SAND_NUM2BOOL(fld_val);
  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_status_get_unsafe()", 0, 0);
}

uint32
  arad_cnt_status_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_STATUS_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(processor_ndx, ARAD_PROCESSOR_NDX_MAX(unit), ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_status_get_verify()", 0, 0);
}

uint32 
  arad_cnt_engine_to_fifo_dma_index(
     SOC_SAND_IN  int               unit,
     SOC_SAND_IN  soc_port_t        crps_index, 
     SOC_SAND_OUT int               *fifo_dma_index,
     SOC_SAND_OUT int               *cache_length
     )
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(fifo_dma_index);
    SOCDNX_NULL_CHECK(cache_length);
    *fifo_dma_index = 0;

    if (SOC_IS_JERICHO(unit)) {
        if (crps_index < SOC_DPP_DEFS_GET(unit, nof_counter_processors)) {
            *fifo_dma_index = crps_index / SOC_DPP_DEFS_GET(unit, nof_counter_fifos);
        } else if (crps_index < SOC_DPP_DEFS_GET(unit, nof_counter_processors) + SOC_DPP_DEFS_GET(unit, nof_small_counter_processors)){
            *fifo_dma_index = SOC_DPP_DEFS_GET(unit, nof_counter_fifos) - 1;
        } else {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "counter processor %u out of range\n"), crps_index));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    } else {
        if (crps_index < 0 || crps_index > SOC_DPP_DEFS_GET(unit, nof_counter_processors) + SOC_DPP_DEFS_GET(unit, nof_small_counter_processors)) {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "counter processor %u out of range\n"), crps_index));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        *fifo_dma_index = crps_index;
    }
    SOCDNX_IF_ERR_EXIT(arad_cnt_fifo_dma_cache_length(unit, *fifo_dma_index, cache_length));

exit:
  SOCDNX_FUNC_RETURN;

}

STATIC uint32 
  arad_cnt_fifo_dma_cache_length(
     SOC_SAND_IN  int           unit,
     SOC_SAND_IN  soc_port_t    fifo_dma_index, 
     SOC_SAND_OUT int           *cache_length
     )
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(cache_length);
    
    if (fifo_dma_index < SOC_DPP_DEFS_GET(unit, nof_counter_fifos)) {
        if (SOC_IS_JERICHO(unit) && (fifo_dma_index == SOC_DPP_DEFS_GET(unit, nof_counter_fifos) - 1)) {
                *cache_length = SOC_DPP_DEFS_GET(unit, counter_fifo_depth) + 
                              (SOC_DPP_DEFS_GET(unit, nof_small_counter_processors) * SOC_DPP_DEFS_GET(unit, counters_per_small_counter_processor));
        } else {
            *cache_length = SOC_DPP_DEFS_GET(unit, counter_fifo_depth);  
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "DMA index %d out of range\n"), fifo_dma_index));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    
    *cache_length = ARAD_CNT_CACHE_LENGTH(unit);
exit:
  SOCDNX_FUNC_RETURN;

}

uint32 
  arad_cnt_fifo_dma_offset_in_engine(
     SOC_SAND_IN  int               unit,
     SOC_SAND_IN  uint32            crps_index, 
     SOC_SAND_OUT uint32            *offset
     )
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(offset);
    *offset = 0;
    if (SOC_IS_JERICHO(unit)) {
        if (crps_index < SOC_DPP_DEFS_GET(unit, nof_counter_processors)) {
            *offset = crps_index % SOC_DPP_DEFS_GET(unit, nof_counter_fifos);
        } else if (crps_index < SOC_DPP_DEFS_GET(unit, nof_counter_processors) + SOC_DPP_DEFS_GET(unit, nof_small_counter_processors)){
            *offset = (crps_index % SOC_DPP_DEFS_GET(unit, nof_counter_fifos)) + 4;
        } else {
            LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "counter processor %u out of range\n"), crps_index));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    } else {
        offset = 0;
    }
exit:
  SOCDNX_FUNC_RETURN;

}
STATIC uint32 
jer_cnt_counter_field_val_parse(
   SOC_SAND_IN  int                 unit,
   SOC_SAND_IN  SOC_TMC_CNT_FORMAT  format,
   SOC_SAND_IN  uint64              field_val,
   SOC_SAND_OUT uint64              *pkt_cnt,
   SOC_SAND_OUT uint64              *byte_cnt
)
{
    SOCDNX_INIT_FUNC_DEFS;
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "jer_cnt_counter_field_val_parse() should not be called for Arad plus and below devices.\n")));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    COMPILER_64_ZERO(*pkt_cnt);
    if (format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS) {
        COMPILER_64_SET(*pkt_cnt, 0, COMPILER_64_HI(field_val));
    } else if (format != SOC_TMC_CNT_FORMAT_BYTES){
        COMPILER_64_SET(*pkt_cnt, JER_CNT_PKT_CNT_MASK_HI(format) & COMPILER_64_HI(field_val), JER_CNT_PKT_CNT_MASK_LO(format) & COMPILER_64_LO(field_val));
    }

    COMPILER_64_ZERO(*byte_cnt);
    if (format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS) {
        COMPILER_64_SET(*byte_cnt, 0, COMPILER_64_LO(field_val)); 
    } else if (format != SOC_TMC_CNT_FORMAT_PKTS){
        COMPILER_64_ADD_64(*byte_cnt, field_val);
        COMPILER_64_SHR(*byte_cnt, JER_CNT_BYTE_CNT_SHIFT(format));
    }

exit:
  SOCDNX_FUNC_RETURN;
}


STATIC uint32 
arad_cnt_counter_field_val_parse(
   SOC_SAND_IN  int                 unit,
   SOC_SAND_IN  SOC_TMC_CNT_FORMAT  format,
   SOC_SAND_IN  uint64              pkt_field_val,
   SOC_SAND_IN  uint64              byte_field_val,
   SOC_SAND_OUT uint64              *pkt_cnt,
   SOC_SAND_OUT uint64              *byte_cnt
)
{
    int shift_size = 26;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "arad_cnt_counter_field_val_parse() should not be called for Jericho and above devices.\n")));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    COMPILER_64_ZERO(*pkt_cnt);
    COMPILER_64_ZERO(*byte_cnt);
    if (format == SOC_TMC_CNT_FORMAT_PKTS) {
        COMPILER_64_ADD_64(*pkt_cnt, byte_field_val);
        COMPILER_64_SHL(*pkt_cnt, shift_size);
        COMPILER_64_ADD_64(*pkt_cnt, pkt_field_val);
    } else if (format == SOC_TMC_CNT_FORMAT_BYTES || format == SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE) {
        COMPILER_64_ADD_64(*byte_cnt, byte_field_val);
        COMPILER_64_SHL(*byte_cnt, shift_size);
        COMPILER_64_ADD_64(*byte_cnt, pkt_field_val);
    } else {
        COMPILER_64_ADD_64(*pkt_cnt, pkt_field_val);
        COMPILER_64_ADD_64(*byte_cnt, byte_field_val);
    }
exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_cnt_algorithmic_read_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  int                           dma_ndx,
    SOC_SAND_IN  uint8                         channel,
    SOC_SAND_IN  SOC_TMC_CNT_COUNTERS_INFO     *counter_info_array,
    SOC_SAND_OUT ARAD_CNT_RESULT_ARR           *result_arr
  )
{
  soc_error_t
    rv = SOC_E_NONE;
  uint32
    nof_read_cnt,
    counters_2read=0,
    valid = 0,
    indx = 0;
  soc_reg_above_64_val_t
      fld_val,
      reg_val;
  ARAD_CNT_CRPS_CACHE_RD_RESULT_FIELDS rd_result_fields;
 
#if ARAD_CNT_USE_DMA
  uint32 *orig_entry;
  int entry_words=0;
  int j, cnt;
  uint32    *host_buff, buff_index;
  soc_reg_t reg;
#endif
  int cache_length = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_ALGORITHMIC_READ_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(result_arr);

  SOC_REG_ABOVE_64_CLEAR(reg_val);
  SOC_REG_ABOVE_64_CLEAR(fld_val);
  if (!SOC_UNIT_NUM_VALID(unit)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(dma_ndx, SOC_DPP_DEFS_GET(unit, nof_counter_fifos), ARAD_CNT_START_FIFO_DMA_ERR, 5, exit);

  
#if ARAD_CNT_USE_DMA
if (!SAL_BOOT_PLISIM ) {


   arad_sw_db_cnt_buff_and_index_get(unit, dma_ndx, &host_buff, &buff_index);    
   _soc_mem_sbus_fifo_dma_get_num_entries(unit, channel, &cnt);

   rv = _arad_cnt_crps_cache_rd_result_reg_get(unit, dma_ndx, &reg);
   if (SOC_FAILURE(rv)) {
       SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 7, exit);   
   }

   entry_words = BYTES2WORDS(soc_reg_bytes(unit, reg));

   counters_2read = cnt;
}
#else
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(rv, 7, exit, ARAD_CNT_START_FIFO_DMA_ERR, _arad_cnt_crps_pre_rd_fifo_sts_1_get(unit, dma_ndx, &reg));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(rv, 8, exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY,  dma_ndx, CRPS_N_CACHE_STATUS_CNTf, &counters_2read));
#endif

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(rv, 9, exit, ARAD_CNT_START_FIFO_DMA_ERR, arad_cnt_fifo_dma_cache_length(unit, dma_ndx, &cache_length));
  if (counters_2read > (cache_length + 1))
  {
      int cmc, ch;
      uint32 val;
      if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
          cmc = channel / NOF_DMA_FIFO_PER_CMC;
          ch = channel % NOF_DMA_FIFO_PER_CMC;    
      } else {
          cmc = SOC_PCI_CMC(unit);
          ch = channel % NOF_DMA_FIFO_PER_CMC;  
      } 

      val = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch));
      LOG_VERBOSE(BSL_LS_SOC_FIFODMA,(BSL_META_U(unit, "CMIC_CMCx_FIFO_CHy_RD_DMA_CFG(%d, %d) val:%x\n"), cmc, ch, val));
      
      val = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CUR_HOSTMEM_WRITE_PTR_OFFSET(cmc, ch));
      LOG_VERBOSE(BSL_LS_SOC_FIFODMA,(BSL_META_U(unit, "CMIC_CMCx_FIFO_CHy_RD_DMA_CUR_HOSTMEM_WRITE_PTR(%d, %d) val:%x\n"), cmc, ch, val));
      
      val = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_ECCERR_ADDRESS_OFFSET(cmc, ch));
      LOG_VERBOSE(BSL_LS_SOC_FIFODMA,(BSL_META_U(unit, "CMIC_CMCx_FIFO_CHy_RD_DMA_ECCERR_ADDRESS(%d, %d) val:%x\n"), cmc, ch, val));
      
      val = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_ECCERR_CONTROL_OFFSET(cmc, ch));
      LOG_VERBOSE(BSL_LS_SOC_FIFODMA,(BSL_META_U(unit, "CMIC_CMCx_FIFO_CHy_RD_DMA_ECCERR_CONTROL(%d, %d) val:%x\n"), cmc, ch, val));
      
      val = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_START_ADDRESS_OFFSET(cmc, ch));
      LOG_VERBOSE(BSL_LS_SOC_FIFODMA,(BSL_META_U(unit, "CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_START_ADDRESS(%d, %d) val:%x\n"), cmc, ch, val));
      
      val = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(cmc, ch));
      LOG_VERBOSE(BSL_LS_SOC_FIFODMA,(BSL_META_U(unit, "CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD(%d, %d) val:%x\n"), cmc, ch, val));
      
      val = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEM_OFFSET(cmc, ch));
      LOG_VERBOSE(BSL_LS_SOC_FIFODMA,(BSL_META_U(unit, "CMIC_CMCx_FIFO_CHy_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEM(%d, %d) val:%x\n"), cmc, ch, val));
      
      val = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_NUM_OF_ENTRIES_VALID_IN_HOSTMEM_OFFSET(cmc, ch));
      LOG_VERBOSE(BSL_LS_SOC_FIFODMA,(BSL_META_U(unit, "CMIC_CMCx_FIFO_CHy_RD_DMA_NUM_OF_ENTRIES_VALID_IN_HOSTMEM(%d, %d) val:%x\n"), cmc, ch, val));
      
      val = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_OPCODE_OFFSET(cmc, ch));
      LOG_VERBOSE(BSL_LS_SOC_FIFODMA,(BSL_META_U(unit, "CMIC_CMCx_FIFO_CHy_RD_DMA_OPCODE(%d, %d) val:%x\n"), cmc, ch, val));
      
      val = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_SBUS_START_ADDRESS_OFFSET(cmc, ch));
      LOG_VERBOSE(BSL_LS_SOC_FIFODMA,(BSL_META_U(unit, "CMIC_CMCx_FIFO_CHy_RD_DMA_SBUS_START_ADDRESS(%d, %d) val:%x\n"), cmc, ch, val));
      
      val = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, ch));
      LOG_VERBOSE(BSL_LS_SOC_FIFODMA,(BSL_META_U(unit, "CMIC_CMCx_FIFO_CHy_RD_DMA_STAT(%d, %d) val:%x\n"), cmc, ch, val));
      
      LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "counters_2read=%d, cache_length=%d \n"), counters_2read, cache_length));
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(counters_2read, cache_length + 1, ARAD_CNT_CACHE_LENGTH_OUT_OF_RANGE_ERR, 10, exit); 

  rv = _arad_cnt_crps_cache_rd_result_reg_get(unit, dma_ndx, &reg);
  if (SOC_FAILURE(rv)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 11, exit);   
  }
  nof_read_cnt = 0; 
  for(indx = 0;  indx < counters_2read;  indx++) {
#if ARAD_CNT_USE_DMA
    if (!SAL_BOOT_PLISIM ) {
      orig_entry = host_buff + (buff_index * entry_words);
      for(j = 0; j < 3; j++) {
        reg_val[j] = orig_entry[j];
      }
      LOG_VERBOSE(BSL_LS_SOC_STAT,
            (BSL_META_U(unit, "algorithmic_read: fifo=%d, curr_line=%x val=%.8x %.8x %.8x\n"),
                dma_ndx, buff_index, (unsigned int)(reg_val[0]), (unsigned int)reg_val[1], (unsigned int)reg_val[2]));
      buff_index++;      
      if (buff_index >= ARAD_CNT_CACHE_LENGTH(unit)) {
         buff_index = 0;
      }
    }
#else
    soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_val);
#endif

    rv = _arad_cnt_crps_cache_rd_result_fields_get(unit, reg, &rd_result_fields);
    if (SOC_FAILURE(rv)) {
        SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 12, exit);   
    }

    ARAD_FLD_FROM_REG_ABOVE_64(reg, rd_result_fields.cache_rd_data_valid, fld_val, reg_val, 13, exit);
    valid = fld_val[0];

    
    
    if(valid){
        if (SOC_IS_JERICHO(unit)) {
            uint64
                cnt_val;
            fld_val[0] = 0;
            ARAD_FLD_FROM_REG_ABOVE_64(reg, rd_result_fields.cache_rd_engine_num, fld_val, reg_val, 14, exit);
            rv = arad_cnt_fifo_dma_offset_in_engine(unit, fld_val[0], &result_arr->cnt_result[nof_read_cnt].engine_id);
            if (SOC_FAILURE(rv)) {
                SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 18, exit);   
            }
            result_arr->cnt_result[nof_read_cnt].engine_id = (SOC_DPP_DEFS_GET(unit, nof_counter_fifos) * dma_ndx) + result_arr->cnt_result[nof_read_cnt].engine_id;

            if (result_arr->cnt_result[nof_read_cnt].engine_id >= SOC_DPP_DEFS_GET(unit, nof_counter_processors) + SOC_DPP_DEFS_GET(unit, nof_small_counter_processors)) {
                SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 16, exit);
            }
            cnt_val = soc_reg_above_64_field64_get(unit, reg, reg_val, rd_result_fields.cache_rd_pkt_cnt);
            rv = jer_cnt_counter_field_val_parse(
                    unit,
                    counter_info_array[result_arr->cnt_result[nof_read_cnt].engine_id].format, 
                    cnt_val,
                    &result_arr->cnt_result[nof_read_cnt].pkt_cnt,
                    &result_arr->cnt_result[nof_read_cnt].byte_cnt);
            if (SOC_FAILURE(rv)) {
                SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 18, exit);   
            }
        } else {
            uint64
                pkt_field_val,
                byte_field_val;

            
            result_arr->cnt_result[nof_read_cnt].engine_id = dma_ndx;
            pkt_field_val = soc_reg_above_64_field64_get(unit, reg, reg_val, rd_result_fields.cache_rd_pkt_cnt);
            byte_field_val = soc_reg_above_64_field64_get(unit, reg, reg_val, rd_result_fields.cache_rd_oct_cnt);

            rv = arad_cnt_counter_field_val_parse(
                    unit, 
                    counter_info_array[result_arr->cnt_result[nof_read_cnt].engine_id].format,
                    pkt_field_val,
                    byte_field_val,
                    &result_arr->cnt_result[nof_read_cnt].pkt_cnt,
                    &result_arr->cnt_result[nof_read_cnt].byte_cnt);
            if (SOC_FAILURE(rv)) {
                SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 19, exit);   
            }

        }
        ARAD_FLD_FROM_REG_ABOVE_64(reg, rd_result_fields.cache_rd_cnt_addr, fld_val, reg_val, 20, exit);
        result_arr->cnt_result[nof_read_cnt].counter_id = fld_val[0];
        if (result_arr->cnt_result[nof_read_cnt].counter_id >= 
            SOC_TMC_NOF_COUNTERS_IN_ENGINE(unit, result_arr->cnt_result[nof_read_cnt].engine_id))
        {
            SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 20, exit);  
        }
        nof_read_cnt++;
    }
  }
  
  result_arr->nof_counters = nof_read_cnt;
  
#if ARAD_CNT_USE_DMA
if (!SAL_BOOT_PLISIM ) {
  (void)_soc_mem_sbus_fifo_dma_set_entries_read(unit, channel, nof_read_cnt);
  arad_sw_db_cnt_buff_and_index_set(unit, dma_ndx, host_buff, buff_index);
}
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_algorithmic_read_unsafe()", 0, 0);
}


uint32
  arad_cnt_algorithmic_read_verify(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  int                                        dma_ndx,
    SOC_SAND_IN  uint8                                      channel
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_ALGORITHMIC_READ_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(dma_ndx, SOC_DPP_DEFS_GET(unit, nof_counter_fifos), ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(channel, NOF_FIFO_DMA_CHANNELS, ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_algorithmic_read_verify()", 0, 0);
}


uint32
  arad_cnt_direct_read_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID          processor_ndx,
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO         *counter_info,
    SOC_SAND_IN  uint32                         counter_ndx,
    SOC_SAND_OUT ARAD_CNT_RESULT                *read_rslt
  )
{
  uint32
    res = SOC_SAND_OK;
  soc_error_t
    rv = SOC_E_NONE;
  uint32
     cnt_ndx = 0;
  ARAD_CNT_RD_PROCESSOR_READ 
      rd_processor_read;
  ARAD_CNT_CRPS_DIRECT_RD_RESULT 
      rd_result;
 soc_reg_above_64_val_t 
      reg_val, valid_bit;
 uint32 reg32_val = 0;          
 uint64
     cnt_val, oct_cnt, pkt_cnt;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_DIRECT_READ_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(counter_info);
  SOC_SAND_CHECK_NULL_INPUT(read_rslt);

  SOC_REG_ABOVE_64_CLEAR(reg_val);  
  ARAD_CNT_RESULT_clear(read_rslt);
  sal_memset(&rd_result,0,sizeof(rd_result));
  sal_memset(&rd_processor_read,0,sizeof(rd_processor_read));

  if (!SOC_UNIT_NUM_VALID(unit)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
  }

   res = arad_cnt_counters_get_verify(
    unit,
    processor_ndx
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  res = _arad_cnt_rd_processor_read(unit, processor_ndx, &rd_processor_read);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);

#ifdef CRASH_RECOVERY_SUPPORT
  soc_hw_set_immediate_hw_access(unit);
#endif 
  soc_reg_field_set(unit, rd_processor_read.rd_processor_read_r, &reg32_val, rd_processor_read.rd_req_f, 0x1);
  soc_reg_field_set(unit, rd_processor_read.rd_processor_read_r, &reg32_val, rd_processor_read.rd_req_type_f, ARAD_CNT_READ_DIRECT_READ);
  soc_reg_field_set(unit, rd_processor_read.rd_processor_read_r, &reg32_val, rd_processor_read.rd_req_addr_f, counter_ndx);  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg32_set(unit, rd_processor_read.rd_processor_read_r, REG_PORT_ANY, rd_processor_read.reg_index, reg32_val));
  
#ifdef CRASH_RECOVERY_SUPPORT
  soc_hw_restore_immediate_hw_access(unit);
#endif 

 
  res = _arad_cnt_crps_direct_rd_result_get(unit, processor_ndx, &rd_result);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg_above_64_get(unit, rd_result.rd_result_r, REG_PORT_ANY, rd_result.reg_index, reg_val));
  valid_bit[0] = 0;
  soc_reg_above_64_field_get(unit, rd_result.rd_result_r, reg_val, rd_result.rd_data_valid_f, valid_bit);

  if (valid_bit[0]) {
      if (SOC_IS_JERICHO(unit)) {
          cnt_val = soc_reg_above_64_field64_get(unit, rd_result.rd_result_r, reg_val, rd_result.rd_pkt_cnt_f);
          
          rv = jer_cnt_counter_field_val_parse(
                    unit,
                    counter_info->format,
                    cnt_val,
                    &read_rslt->pkt_cnt,
                    &read_rslt->byte_cnt);
          if (SOC_FAILURE(rv)) {
              SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 25, exit);   
          }
      } else {
          COMPILER_64_ZERO(oct_cnt);
          COMPILER_64_ZERO(pkt_cnt);
          pkt_cnt = soc_reg_above_64_field64_get(unit, rd_result.rd_result_r, reg_val, rd_result.rd_pkt_cnt_f);
          oct_cnt = soc_reg_above_64_field64_get(unit, rd_result.rd_result_r, reg_val, rd_result.rd_oct_cnt_f);

          rv = arad_cnt_counter_field_val_parse(
                    unit, 
                    counter_info->format,
                    pkt_cnt,
                    oct_cnt,
                    &read_rslt->pkt_cnt,
                    &read_rslt->byte_cnt);
          if (SOC_FAILURE(rv)) {
              SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 26, exit);   
          }
      }
  }
  res = arad_cnt_id_compute(
    unit,
    counter_info,
    counter_ndx,
    &cnt_ndx
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  read_rslt->counter_id = cnt_ndx;
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_direct_read_unsafe()", 0, counter_ndx);
}

uint32
  arad_cnt_direct_read_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                    counter_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_DIRECT_READ_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(processor_ndx, ARAD_PROCESSOR_NDX_MAX(unit), ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(counter_ndx, ARAD_COUNTER_NDX_MAX(unit, counter_ndx), ARAD_COUNTER_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_direct_read_verify()", 0, counter_ndx);
}


uint32
  arad_cnt_q2cnt_id_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                   queue_ndx,
    SOC_SAND_OUT uint32                   *counter_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_CNT_COUNTERS_INFO
    info;
  uint32
    counter_id,
    fld_val;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_Q2CNT_ID_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(counter_ndx);
  
  res = arad_cnt_counters_get_unsafe(
          unit,
          processor_ndx,
          &info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if(info.src_type != ARAD_CNT_SRC_TYPE_VOQ) {
    SOC_SAND_SET_ERROR_CODE(ARAD_CNT_PROC_SRC_TYPE_NOT_VOQ_ERR, 20, exit);
  }
  counter_id = ((queue_ndx - info.mode_eg.base_val) / info.q_set_size);
  
  res = arad_cnt_id_compute(
     unit,
     &info,
     counter_id,
     &fld_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  *counter_ndx = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_q2cnt_id_unsafe()", queue_ndx, *counter_ndx);
}

uint32
  arad_cnt_q2cnt_id_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                   queue_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_Q2CNT_ID_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(queue_ndx, SOC_DPP_DEFS_GET(unit, nof_queues) - 1, ARAD_QUEUE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_q2cnt_id_verify()", queue_ndx, 0);
}



uint32
  arad_cnt_meter_hdr_compensation_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction,
    SOC_SAND_IN  int                        counter_adjust_type,
    SOC_SAND_IN  int32                    hdr_compensation
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_IHB_PINFO_LBP_TBL_DATA
    tbl_data_ingress;
  
  int32
    fld_val;
  uint32
    data[20];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_METER_HDR_COMPENSATION_SET_UNSAFE);

  if (direction == SOC_TMC_PORT_DIRECTION_INCOMING) {
      if (hdr_compensation < 0)
      {
        fld_val = hdr_compensation + 64;
      }
      else
      {
        fld_val = hdr_compensation;
      }
      res = arad_ihb_pinfo_lbp_tbl_get_unsafe(
              unit,
              core_id,
              port_ndx,
              &tbl_data_ingress
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);

      tbl_data_ingress.counter_compension = fld_val;
      
      res = arad_ihb_pinfo_lbp_tbl_set_unsafe(
              unit,
              core_id,
              port_ndx,
              &tbl_data_ingress
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);
  } else {
      if (hdr_compensation < 0)
      {
        fld_val = hdr_compensation + 128;
      }
      else
      {
        fld_val = hdr_compensation;
      }
      
      if (counter_adjust_type == SOC_TMC_CNT_ADJUST_TYPE_EGRESS_TM || counter_adjust_type == SOC_TMC_CNT_ADJSUT_TYPE_GENERAL)
      {
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_EGQ_PP_PPCTm(unit, EGQ_BLOCK(unit, core_id), port_ndx, data));

          soc_EGQ_PP_PPCTm_field32_set(unit, data, COUNTER_COMPENSATIONf, fld_val);

          SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_EGQ_PP_PPCTm(unit, EGQ_BLOCK(unit, core_id), port_ndx, data));
      }

      if (counter_adjust_type == SOC_TMC_CNT_ADJUST_TYPE_EGRESS_PP || counter_adjust_type == SOC_TMC_CNT_ADJSUT_TYPE_GENERAL)
      {
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_EPNI_PP_COUNTER_TABLEm(unit, EPNI_BLOCK(unit, core_id), port_ndx, data));

          soc_EPNI_PP_COUNTER_TABLEm_field32_set(unit, data, COUNTER_COMPENSATIONf, fld_val);

          SOC_SAND_SOC_IF_ERROR_RETURN(res, 1030, exit, WRITE_EPNI_PP_COUNTER_TABLEm(unit, EPNI_BLOCK(unit, core_id), port_ndx, data));
      }

  }

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_meter_hdr_compensation_set_unsafe()", port_ndx, 0);
}

uint32
  arad_cnt_meter_hdr_compensation_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction,
    SOC_SAND_IN  int32                    hdr_compensation
  )
{
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_METER_HDR_COMPENSATION_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_CNT_PORT_NDX_MAX, ARAD_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  if (direction == SOC_TMC_PORT_DIRECTION_INCOMING) {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(hdr_compensation, ARAD_HDR_COMPENSATION_INGRESS_MIN, ARAD_HDR_COMPENSATION_INGRESS_MAX, ARAD_HDR_IN_COMPENSATION_OUT_OF_RANGE_ERR, 20, exit);
  } else {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(hdr_compensation, ARAD_HDR_COMPENSATION_EGRESS_MIN, ARAD_HDR_COMPENSATION_EGRESS_MAX, ARAD_HDR_OUT_COMPENSATION_OUT_OF_RANGE_ERR, 20, exit);
  }
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_meter_hdr_compensation_set_verify()", port_ndx, 0);
}

uint32
  arad_cnt_meter_hdr_compensation_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction

  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_METER_HDR_COMPENSATION_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_CNT_PORT_NDX_MAX, ARAD_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_meter_hdr_compensation_get_verify()", port_ndx, 0);
}


uint32
  arad_cnt_meter_hdr_compensation_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction,
    SOC_SAND_IN  int                        counter_adjust_type,
    SOC_SAND_OUT int32                    *hdr_compensation
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_IHB_PINFO_LBP_TBL_DATA
    tbl_data;
  int32
    fld_val;
  uint32
    data[20];

 SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_METER_HDR_COMPENSATION_GET_UNSAFE);

 SOC_SAND_CHECK_NULL_INPUT(hdr_compensation);
  
 if (direction == SOC_TMC_PORT_DIRECTION_INCOMING) {

     res = arad_ihb_pinfo_lbp_tbl_get_unsafe(
          unit,
          core_id,
          port_ndx,
          &tbl_data
        );
     SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);

     if (tbl_data.counter_compension >= 32)
     {
         fld_val = tbl_data.counter_compension - 64;
     }
     else
     {
         fld_val = tbl_data.counter_compension;
     }
 } else {
     
     if (counter_adjust_type != SOC_TMC_CNT_ADJUST_TYPE_EGRESS_PP)
     {
         SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_EGQ_PP_PPCTm(unit, EGQ_BLOCK(unit, core_id), port_ndx, data));

         fld_val = soc_EGQ_PP_PPCTm_field32_get(unit, data, COUNTER_COMPENSATIONf);
     }
     
     else
     {
         SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_EPNI_PP_COUNTER_TABLEm(unit, EPNI_BLOCK(unit, core_id), port_ndx, data));

         fld_val = soc_EPNI_PP_COUNTER_TABLEm_field32_get(unit, data, COUNTER_COMPENSATIONf);
     }

     if (fld_val >= 64)
     {
         fld_val = fld_val - 128;
     }
    
 }
  *hdr_compensation = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_meter_hdr_compensation_get_unsafe()", port_ndx, 0);
}

#if ARAD_DEBUG_IS_LVL1


uint32
  ARAD_CNT_COUNTERS_INFO_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->src_type, ARAD_CNT_COUNTERS_INFO_SRC_TYPE_MAX(unit), ARAD_CNT_SRC_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mode_statistics, ARAD_CNT_COUNTERS_INFO_MODE_ING_MAX, ARAD_CNT_MODE_STATISTICS_OUT_OF_RANGE_ERR, 11, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->stag_lsb, ARAD_CNT_COUNTERS_INFO_STAG_MAX, ARAD_CNT_STAG_LSB_OUT_OF_RANGE_ERR , 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->q_set_size, ARAD_CNT_VOQ_PARAMS_Q_SET_SIZE_MAX, ARAD_CNT_Q_SET_SIZE_OUT_OF_RANGE_ERR, 14, exit);

  ARAD_STRUCT_VERIFY(ARAD_CNT_MODE_EG, &(info->mode_eg), 15, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_CNT_COUNTERS_INFO_verify()",0,0);
}

uint32
  ARAD_CNT_OVERFLOW_verify(
    SOC_SAND_IN  ARAD_CNT_OVERFLOW *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_CNT_OVERFLOW_verify()",0,0);
}

uint32
  ARAD_CNT_STATUS_verify(
    SOC_SAND_IN  ARAD_CNT_STATUS *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_STRUCT_VERIFY(ARAD_CNT_OVERFLOW, &(info->overflow_cnt), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_CNT_STATUS_verify()",0,0);
}

uint32
  ARAD_CNT_RESULT_verify(
    SOC_SAND_IN  ARAD_CNT_RESULT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_CNT_RESULT_verify()",0,0);
}

uint32
  ARAD_CNT_RESULT_ARR_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_CNT_RESULT_ARR *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < ARAD_CNT_CACHE_LENGTH(unit); ++ind)
  {
    ARAD_STRUCT_VERIFY(ARAD_CNT_RESULT, &(info->cnt_result[ind]), 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_counters, ARAD_CNT_RESULT_ARR_NOF_COUNTERS_MAX, ARAD_CNT_NOF_COUNTERS_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_CNT_RESULT_ARR_verify()",0,0);
}

#endif 



int arad_stat_counter_erpp_offset_set(
    SOC_SAND_IN int                         unit, 
    SOC_SAND_IN SOC_TMC_CNT_SRC_TYPE        src_type,
    SOC_SAND_IN SOC_TMC_CNT_MODE_EG_TYPE    eg_type,
    SOC_SAND_IN int                         counter_profile_offset_value
    )
{
    int rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    if(counter_profile_offset_value < 0) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Not allowed to configure counter profile offset with negative value %d \n"), counter_profile_offset_value));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if(src_type != SOC_TMC_CNT_SRC_TYPE_EGR_PP) {
        LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Not allowed to configure counter profile offset on this source type %d \n"), src_type));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    else {
        if(eg_type == SOC_TMC_CNT_MODE_EG_TYPE_PMF) { 
            rv = soc_reg_above_64_field32_modify(unit, EGQ_COUNTER_PROFILE_OFFSETr, REG_PORT_ANY,  0, COUNTER_PROFILE_OFFSET_1f,  counter_profile_offset_value);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        else if(eg_type == SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF) { 
            rv = soc_reg_above_64_field32_modify(unit, EGQ_COUNTER_PROFILE_OFFSETr, REG_PORT_ANY,  0, COUNTER_PROFILE_OFFSET_2f,  counter_profile_offset_value);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        else if(eg_type == SOC_TMC_CNT_MODE_EG_TYPE_VSI){ 
            rv = soc_reg_above_64_field32_modify(unit, EGQ_COUNTER_PROFILE_OFFSETr, REG_PORT_ANY,  0, COUNTER_PROFILE_OFFSET_3f,  counter_profile_offset_value);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        else {
            if(counter_profile_offset_value != 0) {
                LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Not allowed to configure counter profile offset with engine source %d \n"), eg_type));
                SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
            }
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


int arad_stat_counter_erpp_offset_get(
    SOC_SAND_IN int                         unit, 
    SOC_SAND_IN SOC_TMC_CNT_SRC_TYPE        src_type,
    SOC_SAND_IN SOC_TMC_CNT_MODE_EG_TYPE    eg_type,
    SOC_SAND_IN int*                        counter_profile_offset_value
    )
{
    int rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    if(src_type == SOC_TMC_CNT_SRC_TYPE_EGR_PP && eg_type == SOC_TMC_CNT_MODE_EG_TYPE_PMF) { 
        rv = soc_reg_above_64_field32_read(unit, EGQ_COUNTER_PROFILE_OFFSETr, REG_PORT_ANY,  0, COUNTER_PROFILE_OFFSET_1f,  (uint32 *)counter_profile_offset_value);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    else if(src_type == SOC_TMC_CNT_SRC_TYPE_EGR_PP && eg_type == SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF) { 
        rv = soc_reg_above_64_field32_read(unit, EGQ_COUNTER_PROFILE_OFFSETr, REG_PORT_ANY,  0, COUNTER_PROFILE_OFFSET_2f,  (uint32 *)counter_profile_offset_value);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    else if(src_type == SOC_TMC_CNT_SRC_TYPE_EGR_PP && eg_type == SOC_TMC_CNT_MODE_EG_TYPE_VSI){ 
        rv = soc_reg_above_64_field32_read(unit, EGQ_COUNTER_PROFILE_OFFSETr, REG_PORT_ANY,  0, COUNTER_PROFILE_OFFSET_3f,  (uint32 *)counter_profile_offset_value);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    else {
        counter_profile_offset_value = 0;
    }

    
exit:
    SOCDNX_FUNC_RETURN;
}


int arad_cnt_stif_ingress_pp_source_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int command_id, 
    SOC_SAND_IN SOC_TMC_CNT_MODE_EG_TYPE eg_type, 
    SOC_SAND_IN int counter_base
    )
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pmf_counter_set(unit, command_id, eg_type, counter_base));
exit:
    SOCDNX_FUNC_RETURN;
}


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

