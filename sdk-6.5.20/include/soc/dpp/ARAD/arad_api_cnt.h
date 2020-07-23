/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_API_CNT_INCLUDED__

#define __ARAD_API_CNT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/TMC/tmc_api_cnt.h>

#include <soc/dpp/ARAD/arad_api_general.h>







#define  ARAD_CNT_CACHE_LENGTH(unit) (SOC_TMC_CNT_CACHE_LENGTH_ARAD)









#define ARAD_CNT_PROCESSOR_ID_A                              SOC_TMC_CNT_PROCESSOR_ID_A
#define ARAD_CNT_PROCESSOR_ID_B                              SOC_TMC_CNT_PROCESSOR_ID_B
#define ARAD_CNT_PROCESSOR_ID_C                              SOC_TMC_CNT_PROCESSOR_ID_C
#define ARAD_CNT_PROCESSOR_ID_D                              SOC_TMC_CNT_PROCESSOR_ID_D
#define ARAD_CNT_PROCESSOR_ID_E                              SOC_TMC_CNT_PROCESSOR_ID_E
#define ARAD_CNT_PROCESSOR_ID_F                              SOC_TMC_CNT_PROCESSOR_ID_F
#define ARAD_CNT_PROCESSOR_ID_G                              SOC_TMC_CNT_PROCESSOR_ID_G
#define ARAD_CNT_PROCESSOR_ID_H                              SOC_TMC_CNT_PROCESSOR_ID_H
#define ARAD_CNT_PROCESSOR_ID_I                              SOC_TMC_CNT_PROCESSOR_ID_I
#define ARAD_CNT_PROCESSOR_ID_J                              SOC_TMC_CNT_PROCESSOR_ID_J
#define ARAD_CNT_PROCESSOR_ID_K                              SOC_TMC_CNT_PROCESSOR_ID_K
#define ARAD_CNT_PROCESSOR_ID_L                              SOC_TMC_CNT_PROCESSOR_ID_L 
#define ARAD_CNT_PROCESSOR_ID_M                              SOC_TMC_CNT_PROCESSOR_ID_M 
#define ARAD_CNT_PROCESSOR_ID_N                              SOC_TMC_CNT_PROCESSOR_ID_N 
#define ARAD_CNT_PROCESSOR_ID_O                              SOC_TMC_CNT_PROCESSOR_ID_O 
#define ARAD_CNT_PROCESSOR_ID_P                              SOC_TMC_CNT_PROCESSOR_ID_P 
#define ARAD_CNT_PROCESSOR_ID_Q                              SOC_TMC_CNT_PROCESSOR_ID_Q 
#define ARAD_CNT_PROCESSOR_ID_R                              SOC_TMC_CNT_PROCESSOR_ID_R

#define ARAD_CNT_NOF_PROCESSOR_IDS(unit)                     (SOC_DPP_DEFS_GET(unit, nof_counter_processors) + SOC_DPP_DEFS_GET(unit, nof_small_counter_processors))
typedef SOC_TMC_CNT_PROCESSOR_ID                             ARAD_CNT_PROCESSOR_ID;

#define ARAD_CNT_MODE_STATISTICS_NO_COLOR                    SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR
#define ARAD_CNT_MODE_STATISTICS_COLOR_RES_LOW               SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_LOW
#define ARAD_CNT_MODE_STATISTICS_COLOR_RES_HI                SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_HI
#define ARAD_CNT_MODE_STATISTICS_COLOR_ENQ_HI                SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_ENQ_HI
#define ARAD_CNT_NOF_MODE_INGS                               SOC_TMC_CNT_NOF_MODE_INGS_ARAD
#define ARAD_CNT_MODE_STATISTICS_FWD_NO_COLOR                SOC_TMC_CNT_MODE_STATISTICS_FWD_NO_COLOR
#define ARAD_CNT_MODE_STATISTICS_DROP_NO_COLOR               SOC_TMC_CNT_MODE_STATISTICS_DROP_NO_COLOR
#define ARAD_CNT_MODE_STATISTICS_ALL_NO_COLOR                SOC_TMC_CNT_MODE_STATISTICS_ALL_NO_COLOR
#define ARAD_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS        SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS
typedef SOC_TMC_CNT_MODE_STATISTICS                          ARAD_CNT_MODE_STATISTICS;

#define ARAD_CNT_MODE_EG_TYPE_TM                             SOC_TMC_CNT_MODE_EG_TYPE_TM
#define ARAD_CNT_MODE_EG_TYPE_VSI                            SOC_TMC_CNT_MODE_EG_TYPE_VSI
#define ARAD_CNT_MODE_EG_TYPE_OUTLIF                         SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF
#define ARAD_CNT_MODE_EG_TYPE_PMF                            SOC_TMC_CNT_MODE_EG_TYPE_PMF
#define ARAD_CNT_MODE_EG_TYPE_TM_PORT                        SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT
#define ARAD_CNT_NOF_MODE_EGS                                SOC_TMC_CNT_NOF_MODE_EG_TYPES_ARAD
typedef SOC_TMC_CNT_MODE_EG_TYPE                             ARAD_CNT_MODE_EG_TYPES;
 
#define ARAD_CNT_MODE_EG_RES_NO_COLOR                        SOC_TMC_CNT_MODE_EG_RES_NO_COLOR
#define ARAD_CNT_MODE_EG_RES_COLOR                           SOC_TMC_CNT_MODE_EG_RES_COLOR
#define ARAD_CNT_NOF_MODE_EGS_RES                                SOC_TMC_CNT_NOF_MODE_EGS
typedef SOC_TMC_CNT_MODE_EG_RES                                     ARAD_CNT_MODE_EG_RES;

#define ARAD_CNT_MODE_STATISTICS_FWD_SIMPLE_COLOR                  SOC_TMC_CNT_MODE_STATISTICS_FWD_SIMPLE_COLOR
#define ARAD_CNT_MODE_STATISTICS_DROP_SIMPLE_COLOR                 SOC_TMC_CNT_MODE_STATISTICS_DROP_SIMPLE_COLOR

#define ARAD_CNT_SRC_TYPE_ING_PP                            SOC_TMC_CNT_SRC_TYPE_ING_PP
#define ARAD_CNT_SRC_TYPE_VOQ                               SOC_TMC_CNT_SRC_TYPE_VOQ
#define ARAD_CNT_SRC_TYPE_INGRESS_EXT_STAT                  SOC_TMC_CNT_SRC_TYPE_INGRESS_EXT_STAT
#define ARAD_CNT_SRC_TYPE_STAG                              SOC_TMC_CNT_SRC_TYPE_STAG
#define ARAD_CNT_SRC_TYPE_VSQ                               SOC_TMC_CNT_SRC_TYPE_VSQ
#define ARAD_CNT_SRC_TYPE_CNM_ID                            SOC_TMC_CNT_SRC_TYPE_CNM_ID
#define ARAD_CNT_SRC_TYPE_EGR_PP                            SOC_TMC_CNT_SRC_TYPE_EGR_PP
#define ARAD_CNT_SRC_TYPE_INGRESS_OAM                       SOC_TMC_CNT_SRC_TYPE_INGRESS_OAM
#define ARAD_CNT_SRC_TYPE_EGRESS_OAM                        SOC_TMC_CNT_SRC_TYPE_EGRESS_OAM
#define ARAD_CNT_SRC_TYPE_EPNI                              SOC_TMC_CNT_SRC_TYPE_EPNI
#define ARAD_CNT_SRC_TYPE_EGRESS_TM                         SOC_TMC_CNT_SRC_TYPE_EGRESS_TM
#define ARAD_CNT_SRC_TYPE_EGRESS_VSI                        SOC_TMC_CNT_SRC_TYPE_EGRESS_VSI
#define ARAD_CNT_SRC_TYPE_EGRESS_OUTLIF                     SOC_TMC_CNT_SRC_TYPE_EGRESS_OUTLIF
#define ARAD_CNT_SRC_TYPES_EGQ_TM                           SOC_TMC_CNT_SRC_TYPES_EGQ_TM
#define ARAD_CNT_SRC_TYPES_IPT_LATENCY                      SOC_TMC_CNT_SRC_TYPES_IPT_LATENCY

#define ARAD_CNT_NOF_SRC_TYPES(unit)                        SOC_TMC_CNT_NOF_SRC_TYPES(unit)
typedef SOC_TMC_CNT_SRC_TYPE                                   ARAD_CNT_SRC_TYPE;


#define ARAD_CNT_Q_SET_SIZE_1_Q                              SOC_TMC_CNT_Q_SET_SIZE_1_Q
#define ARAD_CNT_Q_SET_SIZE_2_Q                              SOC_TMC_CNT_Q_SET_SIZE_2_Q
#define ARAD_CNT_Q_SET_SIZE_4_Q                              SOC_TMC_CNT_Q_SET_SIZE_4_Q
#define ARAD_CNT_Q_SET_SIZE_8_Q                              SOC_TMC_CNT_Q_SET_SIZE_8_Q
#define ARAD_CNT_NOF_Q_SET_SIZES                             SOC_TMC_CNT_NOF_Q_SET_SIZES
typedef SOC_TMC_CNT_Q_SET_SIZE                                 ARAD_CNT_Q_SET_SIZE;

#define ARAD_CNT_FORMAT_PKTS_AND_BYTES                      SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES
#define ARAD_CNT_FORMAT_PKTS                                SOC_TMC_CNT_FORMAT_PKTS
#define ARAD_CNT_FORMAT_BYTES                               SOC_TMC_CNT_FORMAT_BYTES
#define ARAD_CNT_FORMAT_MAX_QUEUE_SIZE                      SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE
#define ARAD_CNT_FORMAT_PKTS_AND_PKTS                       SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS
#define ARAD_CNT_FORMAT_IPT_LATENCY                         SOC_TMC_CNT_FORMAT_IPT_LATENCY
typedef SOC_TMC_CNT_FORMAT                                      ARAD_CNT_FORMAT;

  
typedef SOC_TMC_CNT_MODE_EG                                    ARAD_CNT_MODE_EG;
typedef SOC_TMC_CNT_COUNTERS_INFO                              ARAD_CNT_COUNTERS_INFO;
typedef SOC_TMC_CNT_OVERFLOW                                   ARAD_CNT_OVERFLOW;
typedef SOC_TMC_CNT_STATUS                                     ARAD_CNT_STATUS;
typedef SOC_TMC_CNT_RESULT                                     ARAD_CNT_RESULT;
typedef SOC_TMC_CNT_RESULT_ARR                                 ARAD_CNT_RESULT_ARR;

typedef enum soc_tmc_cnt_adjust_type_e {
    SOC_TMC_CNT_ADJSUT_TYPE_GENERAL,
    SOC_TMC_CNT_ADJUST_TYPE_EGRESS_TM,
    SOC_TMC_CNT_ADJUST_TYPE_EGRESS_PP
} soc_tmc_cnt_adjust_type_t;










uint32
  arad_cnt_counters_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID                    processor_ndx,
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO                   *info
  );

uint32
  arad_cnt_dma_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   dma_ndx,
	SOC_SAND_IN  uint8                 channel,
    SOC_SAND_IN  int                   cache_length
  );
uint32
  arad_cnt_dma_unset(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   dma_ndx,
	SOC_SAND_IN  uint8                 channel
	);

uint32
  arad_cnt_counters_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID                    processor_ndx,
    SOC_SAND_OUT ARAD_CNT_COUNTERS_INFO                   *info
  );


uint32
  arad_cnt_status_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID                    processor_ndx,
    SOC_SAND_OUT ARAD_CNT_STATUS                          *proc_status
  );

uint32 
  arad_cnt_engine_to_fifo_dma_index(
     SOC_SAND_IN  int               unit,
     SOC_SAND_IN  soc_port_t        crps_index, 
     SOC_SAND_OUT int               *fifo_dma_index,
     SOC_SAND_OUT int               *cache_length
     );
uint32 
  arad_cnt_fifo_dma_offset_in_engine(
     SOC_SAND_IN  int               unit,
     SOC_SAND_IN  uint32            crps_index, 
     SOC_SAND_OUT uint32            *offset
     );
uint32
    arad_cnt_max_we_val_get( 
       SOC_SAND_IN  int               unit,
       SOC_SAND_IN  ARAD_CNT_SRC_TYPE src_type,
       SOC_SAND_OUT SOC_TMC_CNT_COUNTER_WE_BITMAP *max_we_val);

uint32
  arad_cnt_algorithmic_read(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID                    processor_ndx,
	SOC_SAND_IN  uint8                       		     channel,
    SOC_SAND_IN  SOC_TMC_CNT_COUNTERS_INFO                *counter_info_array,
    SOC_SAND_OUT ARAD_CNT_RESULT_ARR                      *result_arr
  );


uint32
  arad_cnt_direct_read(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID              processor_ndx,
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO             *counter_info,
    SOC_SAND_IN  uint32                             counter_ndx,
    SOC_SAND_OUT ARAD_CNT_RESULT                    *read_rslt
  );


uint32
  arad_cnt_q2cnt_id(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID                    processor_ndx,
    SOC_SAND_IN  uint32                              queue_ndx,
    SOC_SAND_OUT uint32                              *counter_ndx
  );



uint32
  arad_cnt_meter_hdr_compensation_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction,
    SOC_SAND_IN  int                        counter_adjust_type,
    SOC_SAND_IN  int32                    hdr_compensation
  );


uint32
  arad_cnt_meter_hdr_compensation_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction,
    SOC_SAND_IN  int                        counter_adjust_type,
    SOC_SAND_OUT int32                    *hdr_compensation
  );


void
  ARAD_CNT_MODE_EG_clear(
    SOC_SAND_OUT ARAD_CNT_MODE_EG *info
  );

void
  ARAD_CNT_COUNTERS_INFO_clear(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT ARAD_CNT_COUNTERS_INFO *info
  );

void
  ARAD_CNT_OVERFLOW_clear(
    SOC_SAND_OUT ARAD_CNT_OVERFLOW *info
  );

void
  ARAD_CNT_STATUS_clear(
    SOC_SAND_OUT ARAD_CNT_STATUS *info
  );

void
  ARAD_CNT_RESULT_clear(
    SOC_SAND_OUT ARAD_CNT_RESULT *info
  );

void
  ARAD_CNT_RESULT_ARR_clear(
    SOC_SAND_OUT ARAD_CNT_RESULT_ARR *info
  );

#if ARAD_DEBUG_IS_LVL1

const char*
  ARAD_CNT_PROCESSOR_ID_to_string(
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID enum_val
  );

const char*
  ARAD_CNT_MODE_STATISTICS_to_string(
    SOC_SAND_IN  ARAD_CNT_MODE_STATISTICS enum_val
  );

const char*
  ARAD_CNT_MODE_EG_to_string(
    SOC_SAND_IN  ARAD_CNT_MODE_EG enum_val
  );

const char*
  ARAD_CNT_SRC_TYPE_to_string(
    SOC_SAND_IN  ARAD_CNT_SRC_TYPE enum_val
  );

const char*
  ARAD_CNT_Q_SET_SIZE_to_string(
    SOC_SAND_IN  ARAD_CNT_Q_SET_SIZE enum_val
  );

void
  ARAD_CNT_MODE_EG_print(
    SOC_SAND_IN  ARAD_CNT_MODE_EG *info
  );
void
  ARAD_CNT_COUNTERS_INFO_print(
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO *info
  );

void
  ARAD_CNT_OVERFLOW_print(
    SOC_SAND_IN  ARAD_CNT_OVERFLOW *info
  );

void
  ARAD_CNT_STATUS_print(
    SOC_SAND_IN  ARAD_CNT_STATUS *info
  );

void
  ARAD_CNT_RESULT_print(
    SOC_SAND_IN  ARAD_CNT_RESULT *info
  );

void
  ARAD_CNT_RESULT_ARR_print(
    SOC_SAND_IN  ARAD_CNT_RESULT_ARR *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

