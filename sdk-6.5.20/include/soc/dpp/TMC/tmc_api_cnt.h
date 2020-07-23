/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_TMC_API_CNT_INCLUDED__

#define __SOC_TMC_API_CNT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/types.h>




#define  SOC_TMC_CNT_INVALID_DMA_CHANNEL (0xff)
#define  SOC_TMC_CNT_RESERVED_DMA_CHANNEL (0xfe)
#define  SOC_TMC_CNT_INVALID_FIFO (0xff)


#define  SOC_TMC_CNT_CACHE_LENGTH_PB (16)
#define  SOC_TMC_CNT_CACHE_LENGTH_ARAD (16*1024)  
#define  SOC_TMC_CNT_MAX_NUM_OF_FIFOS_PER_PROC (6)

#define SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL (7)

#define SOC_TMC_NOF_COUNTERS_IN_ENGINE(unit, proc_id) \
                                    ((proc_id < SOC_DPP_DEFS_GET(unit, nof_counter_processors)) ? \
                                     (SOC_DPP_DEFS_GET(unit, counters_per_counter_processor)) : \
                                     (SOC_DPP_DEFS_GET(unit, counters_per_small_counter_processor)))

#define SOC_TMC_COUNTER_NDX_MAX(unit, proc_id) (SOC_TMC_NOF_COUNTERS_IN_ENGINE(unit, proc_id) - 1)


#define EIGHT_BITS (8)
#define MAX_ING_COMP_LIF_NUMBER (32)
#define MAX_ING_COMP_DELTA_VALUE (127)
#define MIN_ING_COMP_DELTA_VALUE (-128)

#define SOC_TMC_CNT_QAX_OUT_LIF_CNT_PTR_SIZE (17)










typedef enum
{
    SOC_TMC_CNT_PROCESSOR_ID_A = 0,
    SOC_TMC_CNT_PROCESSOR_ID_B = 1,
   
    SOC_TMC_CNT_NOF_PROCESSOR_IDS_PETRA_B = 2,
    SOC_TMC_CNT_PROCESSOR_ID_C = 2,
    SOC_TMC_CNT_PROCESSOR_ID_D = 3,
    
    SOC_TMC_CNT_NOF_PROCESSOR_IDS_ARAD = 4,
    SOC_TMC_CNT_PROCESSOR_ID_E = 4,
    SOC_TMC_CNT_PROCESSOR_ID_F = 5,
    SOC_TMC_CNT_PROCESSOR_ID_G = 6,
    SOC_TMC_CNT_PROCESSOR_ID_H = 7,
    SOC_TMC_CNT_PROCESSOR_ID_I = 8,
    SOC_TMC_CNT_PROCESSOR_ID_J = 9,
    SOC_TMC_CNT_PROCESSOR_ID_K = 10,
    SOC_TMC_CNT_PROCESSOR_ID_L = 11,
    SOC_TMC_CNT_PROCESSOR_ID_M = 12,
    SOC_TMC_CNT_PROCESSOR_ID_N = 13,
    SOC_TMC_CNT_PROCESSOR_ID_O = 14,
    SOC_TMC_CNT_PROCESSOR_ID_P = 15,
    SOC_TMC_CNT_PROCESSOR_ID_Q = 16,
    SOC_TMC_CNT_PROCESSOR_ID_R = 17,

   
    SOC_TMC_CNT_NOF_PROCESSOR_IDS_JERICHO = 12
}SOC_TMC_CNT_PROCESSOR_ID;

#define SOC_TMC_CNT_NOF_PROCESSOR_IDS                           SOC_TMC_CNT_NOF_PROCESSOR_IDS_PETRA_B

typedef enum
{
  
  SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR = 0,
  
  SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_LOW = 1,
  
  SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_HI = 2,
  
  SOC_TMC_CNT_NOF_MODE_INGS_PETRA_B = 3,

  
  SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_ENQ_HI = 4,
  
  SOC_TMC_CNT_MODE_STATISTICS_FWD_NO_COLOR = 5,
  
  SOC_TMC_CNT_MODE_STATISTICS_DROP_NO_COLOR = 6,
  
  SOC_TMC_CNT_MODE_STATISTICS_ALL_NO_COLOR = 7,
  
  SOC_TMC_CNT_MODE_STATISTICS_FWD_SIMPLE_COLOR = 8,
  
  SOC_TMC_CNT_MODE_STATISTICS_DROP_SIMPLE_COLOR = 9,
  
  SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS = 10,
  
  SOC_TMC_CNT_NOF_MODE_INGS_ARAD = 10

}SOC_TMC_CNT_MODE_STATISTICS;
             
#define SOC_TMC_CNT_NOF_MODE_INGS                           SOC_TMC_CNT_NOF_MODE_INGS_PETRA_B

typedef enum
{
  
  SOC_TMC_CNT_MODE_EG_RES_NO_COLOR = 0,
  
  SOC_TMC_CNT_MODE_EG_RES_COLOR = 1,
  
  SOC_TMC_CNT_NOF_MODE_EGS = 2
}SOC_TMC_CNT_MODE_EG_RES;




typedef enum
{
  
  SOC_TMC_CNT_SRC_TYPE_ING_PP = 0,
  
  SOC_TMC_CNT_SRC_TYPE_VOQ = 1,
  
  SOC_TMC_CNT_SRC_TYPE_STAG = 2,
  
  SOC_TMC_CNT_SRC_TYPE_VSQ = 3,
  
  SOC_TMC_CNT_SRC_TYPE_CNM_ID = 4,
  
  SOC_TMC_CNT_SRC_TYPE_EGR_PP = 5,
  
  SOC_TMC_CNT_NOF_SRC_TYPES_PETRA_B = 6,
  
  SOC_TMC_CNT_SRC_TYPE_INGRESS_OAM = 10,
  
  
  SOC_TMC_CNT_SRC_TYPE_EPNI = 11,
   
  SOC_TMC_CNT_SRC_TYPE_EGRESS_OAM = 12,   
    
  SOC_TMC_CNT_NOF_SRC_TYPES_ARAD = 13,
  SOC_TMC_CNT_SRC_TYPES_EGQ_TM = 14,
  SOC_TMC_CNT_SRC_TYPES_IPT_LATENCY = 15,  
  SOC_TMC_CNT_SRC_TYPE_INGRESS_EXT_STAT = 16,
  SOC_TMC_CNT_NOF_SRC_TYPES_JERICHO = 17
}SOC_TMC_CNT_SRC_TYPE;

#define SOC_TMC_CNT_NOF_SRC_TYPES(unit)                 (SOC_IS_ARADPLUS_AND_BELOW(unit) ? SOC_TMC_CNT_NOF_SRC_TYPES_ARAD : SOC_TMC_CNT_NOF_SRC_TYPES_JERICHO)

#define SOC_TMC_CNT_SRC_IS_EGRESS_TM(src_type, cnt_mode) (((src_type == SOC_TMC_CNT_SRC_TYPE_EPNI) || \
                                                           (src_type == SOC_TMC_CNT_SRC_TYPE_EGR_PP)) && \
                                                          ((cnt_mode == SOC_TMC_CNT_MODE_EG_TYPE_TM) || \
                                                          (cnt_mode == SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT)))
typedef enum
{
  
  SOC_TMC_CNT_Q_SET_SIZE_1_Q = 1,
  
  SOC_TMC_CNT_Q_SET_SIZE_2_Q = 2,
  
  SOC_TMC_CNT_Q_SET_SIZE_4_Q = 4,
  
  SOC_TMC_CNT_Q_SET_SIZE_8_Q = 8,
  
  SOC_TMC_CNT_NOF_Q_SET_SIZES = 9
}SOC_TMC_CNT_Q_SET_SIZE;


typedef enum
{
  
  SOC_TMC_CNT_MODE_EG_TYPE_TM = 0,
  
  SOC_TMC_CNT_MODE_EG_TYPE_VSI = 1,
  
  SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF = 2,
  
  SOC_TMC_CNT_NOF_MODE_EG_TYPES_PETRA_B = 3,
  
  SOC_TMC_CNT_MODE_EG_TYPE_PMF = 3,
  
  SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT = 4,
  
  SOC_TMC_CNT_NOF_MODE_EG_TYPES_ARAD = 5
}SOC_TMC_CNT_MODE_EG_TYPE;


typedef enum {
    SOC_TMC_CNT_TOTAL_PDS_THRESHOLD_VIOLATED, 
    SOC_TMC_CNT_TOTAL_PDS_UC_POOL_SIZE_THRESHOLD_VIOLATED,
    SOC_TMC_CNT_PER_PORT_UC_PDS_THRESHOLD_VIOLATED,
    SOC_TMC_CNT_PER_QUEUE_UC_PDS_THRESHOLD_VIOLATED,
    SOC_TMC_CNT_PER_PORT_UC_DBS_THRESHOLD_VIOLATED,
    SOC_TMC_CNT_PER_QUEUE_UC_DBS_THRESHOLD_VIOLATED,
    SOC_TMC_CNT_PER_QUEUE_DISABLE_BIT,
    SOC_TMC_CNT_TOTAL_PDS_MC_POOL_SIZE_THRESHOLD_VIOLATED,
    SOC_TMC_CNT_PER_INTERFACE_PDS_THREHOLD_VIOLATED,
    SOC_TMC_CNT_MC_SP_THRESHOLD_VIOLATED,
    SOC_TMC_CNT_PER_MC_TC_THRESHOLD_VIOLATED,
    SOC_TMC_CNT_MC_PDS_PER_PORT_THRESHOLD_VIOLATED,
    SOC_TMC_CNT_MC_PDS_PER_QUEUE_THRESHOLD_VIOLATED,
    SOC_TMC_CNT_MC_PER_PORT_SIZE_THRESHOLD_VIOLATED,
    SOC_TMC_CNT_MC_PER_QUEUE_SIZE_THRESHOLD_VIOLATED,
    SOC_TMC_CNT_GLOBAL_REJECT_DISCARDS,
    SOC_TMC_CNT_DRAM_REJECT_DISCARDS,
    SOC_TMC_CNT_VOQ_TAIL_DROP_DISCARDS,
    SOC_TMC_CNT_VOQ_STATISTICS_DISCARDS,
    SOC_TMC_CNT_VSQ_TAIL_DROP_DISCARDS,
    SOC_TMC_CNT_VSQ_STATISTICS_DISCARDS,
    SOC_TMC_CNT_QUEUE_NOT_VALID_DISCARD,
    SOC_TMC_CNT_OTHER_DISCARDS,
    SOC_TMC_CNT_DROP_REASON_COUNT
} SOC_TMC_CNT_FILTER_TYPE;

#define SOC_TMC_CNT_NOF_MODE_EG_TYPES                           SOC_TMC_CNT_NOF_MODE_EG_TYPES_PETRA_B

typedef enum
{
    SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES = 0,
    SOC_TMC_CNT_FORMAT_PKTS = 1,
    SOC_TMC_CNT_FORMAT_BYTES = 2,
    SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE = 3,
    SOC_TMC_CNT_FORMAT_IHB_COMMANDS = 4, 
    SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS = 5,
    SOC_TMC_CNT_FORMAT_IPT_LATENCY = 6,
    SOC_TMC_CNT_NOF_FORMATS_ARAD = 6,
    SOC_TMC_CNT_NOF_FORMATS_JERICHO = 7
} SOC_TMC_CNT_FORMAT;

#define SOC_TMC_CNT_FORMAT_IS_WIDE(format, src) (format == SOC_TMC_CNT_FORMAT_PKTS || \
                                            format == SOC_TMC_CNT_FORMAT_BYTES || \
                                            format == SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE || \
                                            format == SOC_TMC_CNT_FORMAT_IHB_COMMANDS || \
                                            src == SOC_TMC_CNT_SRC_TYPE_INGRESS_OAM || \
                                            src == SOC_TMC_CNT_SRC_TYPE_EGRESS_OAM || \
                                            format == SOC_TMC_CNT_FORMAT_IPT_LATENCY)                                           

typedef struct
{
    int                     command_id;     
    SOC_TMC_CNT_SRC_TYPE    source_type;    
} SOC_TMC_CNT_SOURCE;

typedef struct
{
  
  SOC_TMC_CNT_MODE_EG_RES resolution;
  
  SOC_TMC_CNT_MODE_EG_TYPE type;
  
  uint32 base_val;

} SOC_TMC_CNT_MODE_EG;

typedef enum {
	SOC_TMC_CNT_BMAP_OFFSET_GREEN_FWD = 0,
	SOC_TMC_CNT_BMAP_OFFSET_GREEN_DROP,
	SOC_TMC_CNT_BMAP_OFFSET_YELLOW_FWD,
	SOC_TMC_CNT_BMAP_OFFSET_YELLOW_DROP,
	SOC_TMC_CNT_BMAP_OFFSET_RED_FWD,
	SOC_TMC_CNT_BMAP_OFFSET_RED_DROP,
	SOC_TMC_CNT_BMAP_OFFSET_BLACK_FWD,
	SOC_TMC_CNT_BMAP_OFFSET_BLACK_DROP,
	SOC_TMC_CNT_BMAP_OFFSET_COUNT
}SOC_TMC_CNT_BMAP_OFFSET_MAPPING;


typedef struct
{
    
	uint32 entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL + 1];
	uint32 set_size;
}SOC_TMC_CNT_CUSTOM_MODE_PARAMS;

typedef uint32 SOC_TMC_CNT_COUNTER_WE_BITMAP;


typedef struct
{
  
  SOC_TMC_CNT_SRC_TYPE src_type;
  
  int src_core;
  
  int command_id;
  
  SOC_TMC_CNT_MODE_STATISTICS mode_statistics;
  
  SOC_TMC_CNT_MODE_EG mode_eg;
  
  SOC_TMC_CNT_Q_SET_SIZE q_set_size;
  
  uint32 stag_lsb;
  
  SOC_TMC_CNT_FORMAT            format;
  
  SOC_TMC_CNT_COUNTER_WE_BITMAP we_val;
  
  SOC_TMC_CNT_CUSTOM_MODE_PARAMS custom_mode_params;
  
  unsigned int num_counters;
  
  unsigned int num_sets;
  
  uint8 multiple_sources;
} SOC_TMC_CNT_COUNTERS_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_overflow;
  
  uint32 last_cnt_id;
  
  uint8 is_pckt_overflow;
  
  uint8 is_byte_overflow;

} SOC_TMC_CNT_OVERFLOW;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_cache_full;
  
  SOC_TMC_CNT_OVERFLOW overflow_cnt;
  
  uint32 nof_active_cnts;
  
  uint8 is_cnt_id_invalid;

} SOC_TMC_CNT_STATUS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 counter_id;
  
  uint32 engine_id;
  
  uint64 pkt_cnt;
  
  uint64 byte_cnt;

} SOC_TMC_CNT_RESULT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_CNT_RESULT* cnt_result;
  
  uint32 nof_counters;

} SOC_TMC_CNT_RESULT_ARR;

#define SOC_TMC_CNT_LIF_COUNTING_NUMBER_OF_STACK_IDS(unit, source) (((SOC_IS_JERICHO_B0_AND_ABOVE (unit) == TRUE) || (source == SOC_TMC_CNT_SRC_TYPE_ING_PP)) ? 4 : 3)
#define SOC_TMC_CNT_LIF_COUNTING_MASK_SINGLE_SIZE 2
#define SOC_TMC_CNT_LIF_COUNTING_MASK_SIZE(unit, source) (SOC_TMC_CNT_LIF_COUNTING_MASK_SINGLE_SIZE * SOC_TMC_CNT_LIF_COUNTING_NUMBER_OF_STACK_IDS(unit, source))
#define SOC_TMC_CNT_LIF_COUNTING_MASK_MAX(unit, source) ((1 << SOC_TMC_CNT_LIF_COUNTING_MASK_SIZE(unit, source)) - 1)

#define SOC_TMC_CNT_LIF_COUNTING_NOF_SOURCES 2
#define SOC_TMC_CNT_LIF_COUNTING_MAX_NOF_RANGES_PER_SOURCE 64 
#define SOC_TMC_CNT_LIF_COUNTING_MIN_NOF_RANGES_PER_SOURCE 2 
#define SOC_TMC_CNT_LIF_COUNTING_NOF_RANGES_PER_SRC(_unit) (SOC_IS_JERICHO_PLUS(_unit) ? SOC_TMC_CNT_LIF_COUNTING_MAX_NOF_RANGES_PER_SOURCE : SOC_TMC_CNT_LIF_COUNTING_MIN_NOF_RANGES_PER_SOURCE) 
#define SOC_TMC_CNT_LIF_COUNTING_NOF_PROFILES(_unit) (SOC_IS_JERICHO_PLUS(_unit) ? \
        (SOC_TMC_CNT_LIF_COUNTING_MAX_NOF_RANGES_PER_SOURCE + SOC_TMC_CNT_LIF_COUNTING_MIN_NOF_RANGES_PER_SOURCE) : \
        (SOC_TMC_CNT_LIF_COUNTING_MIN_NOF_RANGES_PER_SOURCE*SOC_TMC_CNT_LIF_COUNTING_NOF_SOURCES))
#define SOC_TMC_CNT_LIF_COUNTING_MASK_MASK(unit, source, lif_stack_level) ((lif_stack_level >= SOC_TMC_CNT_LIF_COUNTING_NUMBER_OF_STACK_IDS(unit, source)) ? 0x0 : 0x3)
#define SOC_TMC_CNT_LIF_COUNTING_MASK_SHIFT(lif_stack_level) (lif_stack_level * SOC_TMC_CNT_LIF_COUNTING_MASK_SINGLE_SIZE)
#define SOC_TMC_CNT_LIF_COUNTING_MASK_SET(unit, source, lif_counting_mask, lif_range_stack_0, lif_range_stack_1, lif_range_stack_2, lif_range_stack_3) \
            (lif_counting_mask = \
            ((SOC_TMC_CNT_LIF_COUNTING_MASK_MASK(unit, source, 0) & lif_range_stack_0) << SOC_TMC_CNT_LIF_COUNTING_MASK_SHIFT(0)) | \
            ((SOC_TMC_CNT_LIF_COUNTING_MASK_MASK(unit, source, 1) & lif_range_stack_1) << SOC_TMC_CNT_LIF_COUNTING_MASK_SHIFT(1)) | \
            ((SOC_TMC_CNT_LIF_COUNTING_MASK_MASK(unit, source, 2) & lif_range_stack_2) << SOC_TMC_CNT_LIF_COUNTING_MASK_SHIFT(2)) | \
            ((SOC_TMC_CNT_LIF_COUNTING_MASK_MASK(unit, source, 3) & lif_range_stack_3) << SOC_TMC_CNT_LIF_COUNTING_MASK_SHIFT(3)))

#define SOC_TMC_CNT_LIF_COUNTING_MASK_GET(unit, source, lif_counting_mask, lif_stack_level) \
                                ((lif_counting_mask >> SOC_TMC_CNT_LIF_COUNTING_MASK_SHIFT(lif_stack_level)) & SOC_TMC_CNT_LIF_COUNTING_MASK_MASK(unit, source, lif_stack_level))

#define SOC_TMC_CNT_LIF_COUNTING_MASK_TO_FIELD_VAL(lif_counting_mask_i)  (((~lif_counting_mask_i) & 0x2) | (lif_counting_mask_i & 0x1))

#define SOC_TMC_CNT_LIF_COUNTING_MASK_FIELD_VAL_GET(unit, source, lif_counting_mask, lif_stack_level) \
            SOC_TMC_CNT_LIF_COUNTING_MASK_TO_FIELD_VAL(SOC_TMC_CNT_LIF_COUNTING_MASK_GET(unit, source, lif_counting_mask, lif_stack_level))


#define SOC_TMC_CNT_LIF_COUNTING_STACK_LEVEL_TO_FIELD_VAL_GET(lif_stack_level, source) ((source == SOC_TMC_CNT_SRC_TYPE_EPNI) ? (lif_stack_level + 1) : lif_stack_level)
#define SOC_TMC_CNT_LIF_COUNTING_FIELD_VAL_TO_STACK_LEVEL_GET(lif_stack_level, source) ((source == SOC_TMC_CNT_SRC_TYPE_EPNI) ? (lif_stack_level - 1) : lif_stack_level)








void
  SOC_TMC_CNT_CUSTOM_MODE_PARAMS_clear(
    SOC_SAND_OUT SOC_TMC_CNT_CUSTOM_MODE_PARAMS *info
  );

void
  SOC_TMC_CNT_COUNTERS_INFO_clear(
     SOC_SAND_OUT int unit,
     SOC_SAND_OUT SOC_TMC_CNT_COUNTERS_INFO *info
  );

void
  SOC_TMC_CNT_OVERFLOW_clear(
    SOC_SAND_OUT SOC_TMC_CNT_OVERFLOW *info
  );

void
  SOC_TMC_CNT_STATUS_clear(
    SOC_SAND_OUT SOC_TMC_CNT_STATUS *info
  );

void
  SOC_TMC_CNT_RESULT_clear(
    SOC_SAND_OUT SOC_TMC_CNT_RESULT *info
  );

void
  SOC_TMC_CNT_RESULT_ARR_clear(
    SOC_SAND_OUT SOC_TMC_CNT_RESULT_ARR *info
  );

void
SOC_TMC_CNT_MODE_EG_clear(
  SOC_SAND_OUT SOC_TMC_CNT_MODE_EG *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_CNT_PROCESSOR_ID_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_PROCESSOR_ID enum_val
  );

const char*
  SOC_TMC_CNT_MODE_STATISTICS_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_MODE_STATISTICS enum_val
  );

const char*
  SOC_TMC_CNT_MODE_EG_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_MODE_EG_RES enum_val
  );

const char*
  SOC_TMC_CNT_SRC_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_SRC_TYPE enum_val
  );

const char*
  SOC_TMC_CNT_Q_SET_SIZE_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_Q_SET_SIZE enum_val
  );
const char*
SOC_TMC_CNT_MODE_EG_TYPE_to_string(
  SOC_SAND_IN  SOC_TMC_CNT_MODE_EG_TYPE enum_val
  );

const char*
SOC_TMC_CNT_FORMAT_to_string(
  SOC_SAND_IN  SOC_TMC_CNT_FORMAT enum_val
  );

const char*
  SOC_TMC_CNT_BMAP_OFFSET_MAPPING_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_BMAP_OFFSET_MAPPING enum_val
  );

void
  SOC_TMC_CNT_CUSTOM_MODE_PARAMS_print(
    SOC_SAND_IN  SOC_TMC_CNT_CUSTOM_MODE_PARAMS *info
  );


void
  SOC_TMC_CNT_COUNTERS_INFO_print(
    SOC_SAND_IN  SOC_TMC_CNT_COUNTERS_INFO *info
  );

void
  SOC_TMC_CNT_OVERFLOW_print(
    SOC_SAND_IN  SOC_TMC_CNT_OVERFLOW *info
  );

void
  SOC_TMC_CNT_STATUS_print(
    SOC_SAND_IN  SOC_TMC_CNT_STATUS *info
  );

void
  SOC_TMC_CNT_RESULT_print(
    SOC_SAND_IN  SOC_TMC_CNT_RESULT *info
  );
void SOC_TMC_CNT_MODE_EG_print
    (
    SOC_SAND_IN  SOC_TMC_CNT_MODE_EG *info
    );
void
  SOC_TMC_CNT_RESULT_ARR_print(
    SOC_SAND_IN  SOC_TMC_CNT_RESULT_ARR *info
  );


#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
