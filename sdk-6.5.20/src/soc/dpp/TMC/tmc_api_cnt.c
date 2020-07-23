/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/







#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_cnt.h>





















void
  SOC_TMC_CNT_CUSTOM_MODE_PARAMS_clear(
    SOC_SAND_OUT SOC_TMC_CNT_CUSTOM_MODE_PARAMS *info
  )
{
  int i;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->set_size = 0;
  for( i = 0 ; i < SOC_TMC_CNT_BMAP_OFFSET_COUNT ; i++){
      info->entries_bmaps[i] = 0;
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}




void
  SOC_TMC_CNT_OVERFLOW_clear(
    SOC_SAND_OUT SOC_TMC_CNT_OVERFLOW *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_CNT_OVERFLOW));
  info->is_overflow = 0;
  info->last_cnt_id = 0;
  info->is_pckt_overflow = 0;
  info->is_byte_overflow = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_CNT_STATUS_clear(
    SOC_SAND_OUT SOC_TMC_CNT_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_CNT_STATUS));
  info->is_cache_full = 0;
  SOC_TMC_CNT_OVERFLOW_clear(&(info->overflow_cnt));
  info->nof_active_cnts = 0;
  info->is_cnt_id_invalid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_CNT_RESULT_clear(
    SOC_SAND_OUT SOC_TMC_CNT_RESULT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_CNT_RESULT));
  info->counter_id = 0;
  COMPILER_64_ZERO(info->pkt_cnt);
  COMPILER_64_ZERO(info->byte_cnt);
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_CNT_RESULT_ARR_clear(
    SOC_SAND_OUT SOC_TMC_CNT_RESULT_ARR *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->nof_counters = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
SOC_TMC_CNT_MODE_EG_clear(
  SOC_SAND_OUT SOC_TMC_CNT_MODE_EG *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(SOC_TMC_CNT_MODE_EG));

    info->resolution = SOC_TMC_CNT_NOF_MODE_EGS;
    info->type = SOC_TMC_CNT_NOF_MODE_EG_TYPES;
    info->base_val = 0;

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_CNT_PROCESSOR_ID_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_PROCESSOR_ID enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_CNT_PROCESSOR_ID_A:
    str = "a";
  break;
  case SOC_TMC_CNT_PROCESSOR_ID_B:
    str = "b";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_CNT_MODE_STATISTICS_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_MODE_STATISTICS enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR:
    str = "no_color";
  break;
  case SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_LOW:
    str = "color_res_low";
  break;
  case SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_HI:
    str = "color_res_hi";
  break;
  case SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_ENQ_HI:
    str = "enqueue_hi(green,not_green)";
  break;
  case SOC_TMC_CNT_MODE_STATISTICS_FWD_NO_COLOR:
    str = "fwd_no_color";
  break;
  case SOC_TMC_CNT_MODE_STATISTICS_DROP_NO_COLOR:
    str = "drop_no_color";
  break;
  case SOC_TMC_CNT_MODE_STATISTICS_ALL_NO_COLOR:
    str = "all_no_color";
  break;
  case SOC_TMC_CNT_MODE_STATISTICS_FWD_SIMPLE_COLOR:
    str = "simple_no_color(fwd_green,fwd_not_green)";
    break;
  case SOC_TMC_CNT_MODE_STATISTICS_DROP_SIMPLE_COLOR:
    str = "simple_no_color(drop_green,drop_not_green)";
  break;
  case SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS:
    str = "configurable";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_CNT_MODE_EG_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_MODE_EG_RES enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_CNT_MODE_EG_RES_NO_COLOR:
    str = "no_color";
  break;
  case SOC_TMC_CNT_MODE_EG_RES_COLOR:
    str = "color";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_CNT_SRC_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_SRC_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_CNT_SRC_TYPE_ING_PP:
    str = "ing_pp";
  break;
  case SOC_TMC_CNT_SRC_TYPE_VOQ:
    str = "voq";
  break;
  case SOC_TMC_CNT_SRC_TYPE_INGRESS_EXT_STAT:
    str = "ingress_ext_stat";
  break; 
  case SOC_TMC_CNT_SRC_TYPE_STAG:
    str = "stag";
  break;
  case SOC_TMC_CNT_SRC_TYPE_VSQ:
    str = "vsq";
  break;
  case SOC_TMC_CNT_SRC_TYPE_CNM_ID:
    str = "cnm_id";
  break;
  case SOC_TMC_CNT_SRC_TYPE_EGR_PP:
    str = "egr_pp";
  break;
  case SOC_TMC_CNT_SRC_TYPE_INGRESS_OAM:
  case SOC_TMC_CNT_SRC_TYPE_EGRESS_OAM:    
    str = "oam";
  break;
  case SOC_TMC_CNT_SRC_TYPE_EPNI:
    str = "epni";
  break;
  case SOC_TMC_CNT_SRC_TYPES_EGQ_TM:
      str = "egq-tm";
  break;
  default:
    str = "Unknown";
  }
  return str;
}
const char*
  SOC_TMC_CNT_MODE_EG_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_MODE_EG_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_CNT_MODE_EG_TYPE_TM:
    str = "tm";
  break;
  case SOC_TMC_CNT_MODE_EG_TYPE_VSI:
    str = "vsi";
  break;
  case SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF:
    str = "outlif";
  break;
  case SOC_TMC_CNT_MODE_EG_TYPE_PMF:
    str = "pmf";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_CNT_Q_SET_SIZE_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_Q_SET_SIZE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_CNT_Q_SET_SIZE_1_Q:
    str = "size_1_q";
  break;
  case SOC_TMC_CNT_Q_SET_SIZE_2_Q:
    str = "size_2_q";
  break;
  case SOC_TMC_CNT_Q_SET_SIZE_4_Q:
    str = "size_4_q";
  break;
  case SOC_TMC_CNT_Q_SET_SIZE_8_Q:
    str = "size_8_q";
  break;
  default:
    str = " Unknown";
  }
  return str;
}
const char*
  SOC_TMC_CNT_FORMAT_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_FORMAT enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES:
    str = "PKTS_AND_BYTES";
  break;
  case SOC_TMC_CNT_FORMAT_PKTS:
    str = "PKTS";
  break;
  case SOC_TMC_CNT_FORMAT_BYTES:
    str = "BYTES";
  break;
  case SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE:
    str = "MAX_QUEUE_SIZE";
  break;
  case SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS:
	str = "PKTS_AND_PKTS";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_CNT_Q_SET_SIZE_print(
    SOC_SAND_IN  SOC_TMC_CNT_Q_SET_SIZE q_set_size
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  LOG_CLI((BSL_META_U(unit,
                      "q_set_size %s "), SOC_TMC_CNT_Q_SET_SIZE_to_string(q_set_size)));

  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  SOC_TMC_CNT_BMAP_OFFSET_MAPPING_to_string(
    SOC_SAND_IN  SOC_TMC_CNT_BMAP_OFFSET_MAPPING enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_CNT_BMAP_OFFSET_GREEN_FWD:
    str = "green fwd";
  break;
  case SOC_TMC_CNT_BMAP_OFFSET_GREEN_DROP:
    str = "green drop";
  break;
  case SOC_TMC_CNT_BMAP_OFFSET_YELLOW_FWD:
    str = "yellow fwd";
  break;
  case SOC_TMC_CNT_BMAP_OFFSET_YELLOW_DROP:
    str = "yellow drop";
  break;
  case SOC_TMC_CNT_BMAP_OFFSET_RED_FWD:
    str = "red fwd";
  break;
  case SOC_TMC_CNT_BMAP_OFFSET_RED_DROP:
    str = "red drop";
  break;
  case SOC_TMC_CNT_BMAP_OFFSET_BLACK_FWD:
    str = "black fwd";
  break;
  case SOC_TMC_CNT_BMAP_OFFSET_BLACK_DROP:
    str = "black drop";
  break;
  default:
    str = "Unknown";
  }
  return str;
}

void
  SOC_TMC_CNT_CUSTOM_MODE_PARAMS_print(
    SOC_SAND_IN  SOC_TMC_CNT_CUSTOM_MODE_PARAMS *info
  )
{
  uint32 i, j;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "number of counters in set: %u\n\r"),info->set_size));
  for(i = 0 ; i < info->set_size ; i++){
      LOG_CLI((BSL_META_U(unit,
                          "counter %u(%u):"),i, info->entries_bmaps[i]));
      for(j = 0 ; j < SOC_TMC_CNT_BMAP_OFFSET_COUNT; j++){
          if(SHR_BITGET(&(info->entries_bmaps[i]),j)){
              LOG_CLI((BSL_META_U(unit,
                                  " %s"), SOC_TMC_CNT_BMAP_OFFSET_MAPPING_to_string(j)));
          }
      }
      LOG_CLI((BSL_META_U(unit,
                          "\r\n")));

  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void SOC_TMC_CNT_MODE_EG_print
(
 SOC_SAND_IN  SOC_TMC_CNT_MODE_EG *info
 )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Egress base_val: %u\n\r"),info->base_val));
  if (info->resolution == SOC_TMC_CNT_MODE_EG_RES_COLOR)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Egress resolution: COLOR\n\r")));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "Egress resolution: NO COLOR\n\r")));
  }
  switch (info->type)
  {


  case SOC_TMC_CNT_MODE_EG_TYPE_TM:

    LOG_CLI((BSL_META_U(unit,
                        "Egress type: TM\n\r")));
    break;

  case SOC_TMC_CNT_MODE_EG_TYPE_VSI:

    LOG_CLI((BSL_META_U(unit,
                        "Egress type: VSI\n\r")));

    break;

  case SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF :

    LOG_CLI((BSL_META_U(unit,
                        "Egress type: OUTLIF\n\r")));

    break;

  default:

    break;

  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_CNT_COUNTERS_INFO_print(
    SOC_SAND_IN  SOC_TMC_CNT_COUNTERS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "src_type %s "), SOC_TMC_CNT_SRC_TYPE_to_string(info->src_type)));
  LOG_CLI((BSL_META_U(unit,
                      "mode_ing %s "), SOC_TMC_CNT_MODE_STATISTICS_to_string(info->mode_statistics)));
  SOC_TMC_CNT_MODE_EG_print(&(info->mode_eg));
  LOG_CLI((BSL_META_U(unit,
                      "voq_cnt:")));
  SOC_TMC_CNT_Q_SET_SIZE_print(info->q_set_size);
  LOG_CLI((BSL_META_U(unit,
                      "stag_lsb: %u\n\r"),info->stag_lsb));
  LOG_CLI((BSL_META_U(unit,
                      "number of counters in engine: %u\n\r"),info->num_counters));
  LOG_CLI((BSL_META_U(unit,
                      "number of counters-sets in engine: %u\n\r"),info->num_sets));
  if(info->mode_statistics == SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS){
      SOC_TMC_CNT_CUSTOM_MODE_PARAMS_print(&info->custom_mode_params);
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_CNT_OVERFLOW_print(
    SOC_SAND_IN  SOC_TMC_CNT_OVERFLOW *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "is_overflow: %u\n\r"),info->is_overflow));
  LOG_CLI((BSL_META_U(unit,
                      "last_cnt_id: %u\n\r"),info->last_cnt_id));
  LOG_CLI((BSL_META_U(unit,
                      "is_pckt_overflow: %u\n\r"),info->is_pckt_overflow));
  LOG_CLI((BSL_META_U(unit,
                      "is_byte_overflow: %u\n\r"),info->is_byte_overflow));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_CNT_STATUS_print(
    SOC_SAND_IN  SOC_TMC_CNT_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "is_cache_full: %u\n\r"),info->is_cache_full));
  LOG_CLI((BSL_META_U(unit,
                      "overflow_cnt:")));
  SOC_TMC_CNT_OVERFLOW_print(&(info->overflow_cnt));
  LOG_CLI((BSL_META_U(unit,
                      "nof_active_cnts: %u\n\r"),info->nof_active_cnts));
  LOG_CLI((BSL_META_U(unit,
                      "is_cnt_id_invalid: %u\n\r"),info->is_cnt_id_invalid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_CNT_RESULT_print(
    SOC_SAND_IN  SOC_TMC_CNT_RESULT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "counter_id: %u\n\r"),info->counter_id));
  LOG_CLI((BSL_META_U(unit,
                      "pkt_cnt: 0x%08x%08x[Packets]\n\r"), COMPILER_64_HI(info->pkt_cnt),COMPILER_64_LO(info->pkt_cnt)));
  LOG_CLI((BSL_META_U(unit,
                      "byte_cnt: 0x%08x%08x[Bytes]\n\r"), COMPILER_64_HI(info->byte_cnt),COMPILER_64_LO(info->byte_cnt)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_CNT_RESULT_ARR_print(
    SOC_SAND_IN  SOC_TMC_CNT_RESULT_ARR *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_TMC_CNT_CACHE_LENGTH_ARAD; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "cnt_result[%u]:"),ind));
    SOC_TMC_CNT_RESULT_print(&(info->cnt_result[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "nof_counters: %u\n\r"),info->nof_counters));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
SOC_PB_CNT_MODE_EG_clear(
                     SOC_SAND_OUT SOC_TMC_CNT_MODE_EG *info
                     )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  info->type = SOC_TMC_CNT_NOF_MODE_EG_TYPES;
  info->resolution = SOC_TMC_CNT_NOF_MODE_EGS;
  info->base_val = 0;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
SOC_TMC_CNT_COUNTERS_INFO_clear(
                SOC_SAND_OUT int unit,
                SOC_SAND_OUT SOC_TMC_CNT_COUNTERS_INFO *info)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_CNT_COUNTERS_INFO));
  info->src_type = SOC_TMC_CNT_NOF_SRC_TYPES(unit);
  info->mode_statistics = SOC_TMC_CNT_NOF_MODE_INGS;
  info->mode_eg.base_val = 0;
  info->mode_eg.resolution = SOC_TMC_CNT_NOF_MODE_EGS;
  info->q_set_size = SOC_TMC_CNT_NOF_Q_SET_SIZES;
  info->command_id = -1;
  info->format = SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES;
  SOC_TMC_CNT_MODE_EG_clear(&info->mode_eg);
  info->src_core = -1;
  info->we_val = 0;
  SOC_TMC_CNT_CUSTOM_MODE_PARAMS_clear(&(info->custom_mode_params));
  info->stag_lsb = 0;
  info->num_counters = 0;
  info->num_sets = 0;
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}





#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

