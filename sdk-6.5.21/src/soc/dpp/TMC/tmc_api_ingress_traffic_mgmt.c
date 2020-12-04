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

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>
#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>





#define SOC_TMC_ITM_WORDS_RESOLUTION 16
















int  tmc_itm_mantissa_exp_field_set(
    int                  unit,
    soc_mem_t mem_id,
    soc_field_t field_id,
    int mantissa_bits,
    int exp_bits,
    int factor,
    int round_up,
    void *data,
    uint32 threshold, 
    uint32* result_threshold
    )
{
    uint32    exp_man, mnt = 0, exp = 0;
    
    SOCDNX_INIT_FUNC_DEFS;

    if (round_up) {
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_break_to_mnt_exp_round_up(
                                    SOC_SAND_DIV_ROUND_UP(threshold, factor),
                                    mantissa_bits,
                                    exp_bits,
                                    0, &mnt, &exp
                                    ));
    } else {
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_break_to_mnt_exp_round_down(
                                    threshold/factor,
                                    mantissa_bits,
                                    exp_bits,
                                    0, &mnt, &exp
                                    ));
    }

    arad_iqm_mantissa_exponent_set(unit, mnt, exp, mantissa_bits, &exp_man);
    soc_mem_field32_set(unit, mem_id, data, field_id, exp_man);
    *result_threshold = mnt * (factor << exp);

exit:
    SOCDNX_FUNC_RETURN;
}

void  tmc_itm_mantissa_exp_field_get(
    int                  unit,
    soc_mem_t mem_id,
    soc_field_t field_id,
    int mantissa_bits,
    int exp_bits,
    int factor,
    void *data,
    uint32* result_threshold
    )
{
    uint32    exp_man, mnt = 0, exp = 0;
    
    exp_man = soc_mem_field32_get(unit, mem_id, data, field_id);
    arad_iqm_mantissa_exponent_get(unit, exp_man, mantissa_bits, &mnt, &exp);
    *result_threshold = mnt * (factor << exp);
}

int soc_tmc_itm_voq_threshold_resolution_get(int unit, SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type, int* resolution)
{

    SOCDNX_INIT_FUNC_DEFS;
    
    if (SOC_IS_QAX(unit))
    {
        
        *resolution = (rsrc_type == SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS ? 1: SOC_TMC_ITM_WORDS_RESOLUTION);
    }
    else
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("tmc_itm_voq_threshold_resolution_get is not implemeted for this device\n")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



void
  SOC_TMC_ITM_DRAM_BUFFERS_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_DRAM_BUFFERS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_DRAM_BUFFERS_INFO));
  info->dbuff_size = SOC_TMC_ITM_NOF_DBUFF_SIZES;
  info->uc_nof_buffs = 0;
  info->full_mc_nof_buffs = 0;
  info->mini_mc_nof_buffs = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE_clear(
    SOC_SAND_OUT SOC_TMC_ITM_GLOB_RCS_FC_TYPE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_GLOB_RCS_FC_TYPE));
  SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->hp));
  SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->lp));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_GLOB_RCS_FC_TH_clear(
    SOC_SAND_OUT SOC_TMC_ITM_GLOB_RCS_FC_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_GLOB_RCS_FC_TH));
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE_clear(&(info->bdbs));
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE_clear(&(info->unicast));
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE_clear(&(info->full_mc));
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE_clear(&(info->mini_mc));
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE_clear(&(info->ocb));
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE_clear(&(info->ocb_p0));
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE_clear(&(info->ocb_p1));
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE_clear(&(info->mix_p0));
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE_clear(&(info->mix_p1));
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_GLOB_RCS_DROP_TH_clear(
    SOC_SAND_OUT SOC_TMC_ITM_GLOB_RCS_DROP_TH *info
  )
{
  uint32
    ind, ind1;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_GLOB_RCS_DROP_TH));
  for (ind=0; ind<SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->bdbs[ind]));
  }
  for (ind=0; ind<SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
      SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->ocb_bdbs[ind]));
  }
  for (ind=0; ind<SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->bds[ind]));
  }
  for (ind=0; ind<SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->ocb_bds[ind]));
  }
  for (ind=0; ind<SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->unicast[ind]));
  }
  for (ind=0; ind<SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->full_mc[ind]));
  }
  for (ind=0; ind<SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->mini_mc[ind]));
  }
  for (ind = 0; ind < SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->mem_excess[ind]));
  }
    for (ind = 0; ind < SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->ocb_mem_excess[ind]));
  }
  for (ind = 0; ind < SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->ocb_uc[ind]));
  }
  for (ind = 0; ind < SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->ocb_mc[ind]));
  }
  for (ind = 0; ind < SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->ocb_shrd_pool[0][ind]));
  }
  for (ind = 0; ind < SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->ocb_shrd_pool[1][ind]));
  }
  for (ind = 0; ind < SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->mix_shrd_pool[0][ind]));
  }
  for (ind = 0; ind < SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->mix_shrd_pool[1][ind]));
  }
  for (ind1 = 0; ind1 < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++ind1)
  {
      for (ind = 0; ind < SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
      {
        SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->global_shrd_pool_0[ind1][ind]));
        SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->global_shrd_pool_1[ind1][ind]));
      }
  }
  for (ind1 = 0; ind1 < SOC_TMC_INGRESS_THRESHOLD_NOF_SRAM_TYPES; ++ind1)
  {
      for (ind = 0; ind < SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
      {
        SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->global_free_sram[ind1][ind]));
        SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->global_free_sram_only[ind1][ind]));
      }
  }
  for (ind = 0; ind < SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->global_free_dram[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_QUEUE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_QUEUE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_QUEUE_INFO));
  info->cr_req_type_ndx = SOC_TMC_ITM_NOF_QT_NDXS;
  info->credit_cls = 0;
  info->rate_cls = 0;
  info->vsq_connection_cls = 0;
  info->vsq_traffic_cls = 0;
  info->signature = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CATEGORY_RNGS_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CATEGORY_RNGS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_CATEGORY_RNGS));
  info->vsq_ctgry0_end = 0;
  info->vsq_ctgry1_end = 0;
  info->vsq_ctgry2_end = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT_clear(
    SOC_SAND_OUT SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT));
  info->ctgry_test_en = 0;
  info->ctgry_trffc_test_en = 0;
  info->ctgry2_3_cnctn_test_en = 0;
  info->sttstcs_tag_test_en = 0;
  info->pfc_test_en = 0;
  info->llfc_test_en = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO));
  SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT_clear(&(info->test_a));
  SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT_clear(&(info->test_b));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_REQUEST_HUNGRY_TH_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_REQUEST_HUNGRY_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_CR_REQUEST_HUNGRY_TH));
  info->off_to_slow_th = 0;
  info->off_to_normal_th = 0;
  info->slow_to_normal_th = 0;
  info->normal_to_slow_th = 0;
  info->multiplier = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_REQUEST_BACKOFF_TH_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_REQUEST_BACKOFF_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_CR_REQUEST_BACKOFF_TH));
  info->backoff_enter_th = 0;
  info->backoff_exit_th = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_REQUEST_BACKLOG_TH_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_REQUEST_BACKLOG_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_CR_REQUEST_BACKLOG_TH));
  info->backlog_enter_th = 0;
  info->backlog_exit_th = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_REQUEST_EMPTY_Q_TH_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_REQUEST_EMPTY_Q_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_CR_REQUEST_EMPTY_Q_TH));
  info->satisfied_empty_q_th = 0;
  info->max_credit_balance_empty_q = 0;
  info->exceed_max_empty_q = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_REQUEST_SATISFIED_TH_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_REQUEST_SATISFIED_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_CR_REQUEST_SATISFIED_TH));
  SOC_TMC_ITM_CR_REQUEST_BACKOFF_TH_clear(&(info->backoff_th));
  SOC_TMC_ITM_CR_REQUEST_BACKLOG_TH_clear(&(info->backlog_th));
  SOC_TMC_ITM_CR_REQUEST_EMPTY_Q_TH_clear(&(info->empty_queues));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_WD_Q_TH_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_WD_Q_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_CR_WD_Q_TH));
  info->cr_wd_stts_msg_gen = 0;
  info->cr_wd_dlt_q_th = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_REQUEST_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_REQUEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_CR_REQUEST_INFO));
  SOC_TMC_ITM_CR_WD_Q_TH_clear(&(info->wd_th));
  SOC_TMC_ITM_CR_REQUEST_HUNGRY_TH_clear(&(info->hungry_th));
  SOC_TMC_ITM_CR_REQUEST_SATISFIED_TH_clear(&(info->satisfied_th));
  info->is_low_latency = FALSE;
  info->is_remote_credit_value = FALSE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_DISCOUNT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_DISCOUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_CR_DISCOUNT_INFO));
  info->discount = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_WRED_QT_DP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_WRED_QT_DP_INFO));
  info->ignore_packet_size = 1;
  info->min_avrg_th = 0;
  info->max_avrg_th = 0;
  info->max_packet_size = 0;
  info->max_probability = 100;
  info->wred_en = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_TAIL_DROP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_TAIL_DROP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_TAIL_DROP_INFO));
  info->max_inst_q_size = 0;
  info->max_inst_q_size_bds = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_FADT_DROP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_FADT_DROP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_FADT_DROP_INFO));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_DRAM_BOUND_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_DRAM_BOUND_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_DRAM_BOUND_INFO));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

SOC_TMC_ITM_DRAM_BOUND_THRESHOLD* SOC_TMC_ITM_DRAM_BOUND_INFO_thresh_get(
    int                 unit,
    SOC_TMC_ITM_DRAM_BOUND_INFO* info,
    SOC_TMC_INGRESS_DRAM_BOUND_TYPE_E dram_thresh,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E resource_type)
{
    SOC_TMC_ITM_DRAM_BOUND_THRESHOLD* thresh = NULL;

    switch (resource_type) {
        case SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES:
            thresh = &info->sram_words_dram_threshold[dram_thresh];
            break;
        case SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS:
            thresh = &info->sram_pds_dram_threshold[dram_thresh];
            break;
        default:
            break;
    }

    return thresh;
}

void
  SOC_TMC_ITM_VSQ_PG_PRM_clear (
     SOC_TMC_ITM_VSQ_PG_PRM  *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);
    sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_VSQ_PG_PRM));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);

}

void
  SOC_TMC_ITM_VSQ_SRC_PORT_INFO_clear (
     SOC_TMC_ITM_VSQ_SRC_PORT_INFO  *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);
    sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_VSQ_SRC_PORT_INFO));
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_VSQ_PG_INFO_clear (
     SOC_TMC_ITM_VSQ_PG_INFO  *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);
    sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_VSQ_PG_INFO));
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_WD_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_WD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_CR_WD_INFO));
  info->min_scan_cycle_period_micro = 0;
  info->max_flow_msg_gen_rate_nano = 0;
  info->bottom_queue = 0;
  info->top_queue = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_VSQ_FC_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO *info
  )
{
  int ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_VSQ_FC_INFO));
  SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->q_size_fc));
  SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->bd_size_fc));
  for (ind = 0; ind < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++ind) {
      SOC_TMC_THRESH_WITH_HYST_INFO_clear(&(info->size_fc[ind]));
      SOC_TMC_THRESH_WITH_FADT_HYST_INFO_clear(&(info->fadt_size_fc[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_VSQ_TAIL_DROP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_VSQ_TAIL_DROP_INFO));
  info->max_inst_q_size = 0;
  info->max_inst_q_size_bds = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_VSQ_WRED_GEN_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_WRED_GEN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_VSQ_WRED_GEN_INFO));
  info->wred_en = 0;
  info->exp_wq = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_STAG_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_STAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_STAG_INFO));
  info->enable_mode = SOC_TMC_ITM_STAG_ENABLE_MODE_DISABLED;
  info->vsq_index_msb = 0;
  info->vsq_index_lsb = 0;
  info->dropp_en = 0;
  info->dropp_lsb = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_INGRESS_SHAPE_Q_RANGE_clear(
    SOC_SAND_OUT SOC_TMC_ITM_INGRESS_SHAPE_Q_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_INGRESS_SHAPE_Q_RANGE));
  info->q_num_low = 0;
  info->q_num_high = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_INGRESS_SHAPE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_INGRESS_SHAPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_INGRESS_SHAPE_INFO));
  info->enable = 0;
  SOC_TMC_ITM_INGRESS_SHAPE_Q_RANGE_clear(&(info->q_range));
  info->rate = 0;
  info->sch_port = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_PRIORITY_MAP_TMPLT_clear(
    SOC_SAND_OUT SOC_TMC_ITM_PRIORITY_MAP_TMPLT *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_PRIORITY_MAP_TMPLT));
  for (ind=0; ind<SOC_TMC_ITM_PRIO_MAP_SIZE_IN_UINT32S; ++ind)
  {
    info->map[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_SYS_RED_DROP_PROB_clear(
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_DROP_PROB *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_SYS_RED_DROP_PROB));
  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_DRP_PROBS; ++ind)
  {
    info->drop_probs[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_SYS_RED_QT_DP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_QT_DP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_SYS_RED_QT_DP_INFO));
  info->enable = 0;
  info->adm_th = 0;
  info->prob_th = 0;
  info->drp_th = 0;
  info->drp_prob_low = 0;
  info->drp_prob_high = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_SYS_RED_QT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_QT_INFO *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_SYS_RED_QT_INFO));
  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_Q_SIZE_RANGES; ++ind)
  {
    info->queue_size_boundaries[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_SYS_RED_EG_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_EG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_SYS_RED_EG_INFO));
  info->enable = 0;
  info->aging_timer = 0;
  info->reset_expired_q_size = 0;
  info->aging_only_dec_q_size = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_THS_clear(
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_GLOB_RCS_THS *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_SYS_RED_GLOB_RCS_THS));
  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_BUFFS_RNGS-1; ++ind)
  {
    info->unicast_rng_ths[ind] = 0;
  }
  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_BUFFS_RNGS-1; ++ind)
  {
    info->multicast_rng_ths[ind] = 0;
  }
  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_BUFFS_RNGS-1; ++ind)
  {
    info->bds_rng_ths[ind] = 0;
  }
  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_BUFFS_RNGS-1; ++ind)
  {
    info->ocb_rng_ths[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_VALS_clear(
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_GLOB_RCS_VALS *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_SYS_RED_GLOB_RCS_VALS));
  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_BUFFS_RNGS; ++ind)
  {
    info->unicast_rng_vals[ind] = 0;
  }
  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_BUFFS_RNGS; ++ind)
  {
    info->multicast_rng_vals[ind] = 0;
  }
  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_BUFFS_RNGS; ++ind)
  {
    info->bds_rng_vals[ind] = 0;
  }
  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_BUFFS_RNGS; ++ind)
  {
    info->ocb_rng_vals[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_GLOB_RCS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_SYS_RED_GLOB_RCS_INFO));
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_THS_clear(&(info->thresholds));
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_VALS_clear(&(info->values));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
SOC_TMC_ITM_TC_MAPPING_clear(
		SOC_SAND_OUT SOC_TMC_ITM_TC_MAPPING *info
	)
{
  uint32
    i;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ITM_TC_MAPPING));

  for(i=0; i<SOC_TMC_NOF_TRAFFIC_CLASSES; i++)
  {
    info->new_tc[i] = 0;
  }
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_ITM_QT_NDX_to_string(
    SOC_SAND_IN  SOC_TMC_ITM_QT_NDX enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_ITM_QT_NDX_00:
    str = "ndx_00";
  break;
  case SOC_TMC_ITM_NOF_QT_NDXS:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_to_string(
    SOC_SAND_IN  SOC_TMC_ITM_CR_DISCNT_CLS_NDX enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_ITM_CR_DISCNT_CLS_NDX_00:
    str = "ndx_00";
  break;
  case SOC_TMC_ITM_CR_DISCNT_CLS_NDX_01:
    str = "ndx_01";
  break;
  case SOC_TMC_ITM_CR_DISCNT_CLS_NDX_02:
    str = "ndx_02";
  break;
  case SOC_TMC_ITM_CR_DISCNT_CLS_NDX_03:
    str = "ndx_03";
  break;
  case SOC_TMC_ITM_CR_DISCNT_CLS_NDX_04:
    str = "ndx_04";
  break;
  case SOC_TMC_ITM_CR_DISCNT_CLS_NDX_05:
    str = "ndx_05";
  break;
  case SOC_TMC_ITM_CR_DISCNT_CLS_NDX_06:
    str = "ndx_06";
  break;
  case SOC_TMC_ITM_CR_DISCNT_CLS_NDX_07:
    str = "ndx_07";
  break;
  case SOC_TMC_ITM_CR_DISCNT_CLS_NDX_08:
    str = "ndx_08";
  break;
  case SOC_TMC_ITM_CR_DISCNT_CLS_NDX_09:
    str = "ndx_09";
  break;
  case SOC_TMC_ITM_CR_DISCNT_CLS_NDX_10:
    str = "ndx_10";
  break;
  case SOC_TMC_ITM_CR_DISCNT_CLS_NDX_11:
    str = "ndx_11";
  break;
  case SOC_TMC_ITM_CR_DISCNT_CLS_NDX_12:
    str = "ndx_12";
  break;
  case SOC_TMC_ITM_CR_DISCNT_CLS_NDX_13:
    str = "ndx_13";
  break;
  case SOC_TMC_ITM_CR_DISCNT_CLS_NDX_14:
    str = "ndx_14";
  break;
  case SOC_TMC_ITM_CR_DISCNT_CLS_NDX_15:
    str = "ndx_15";
  break;
  case SOC_TMC_ITM_NOF_CR_DISCNT_CLS_NDXS:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_ITM_DBUFF_SIZE_BYTES_to_string(
    SOC_SAND_IN SOC_TMC_ITM_DBUFF_SIZE_BYTES enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_ITM_DBUFF_SIZE_BYTES_256:
    str = "bytes_256";
  break;
  case SOC_TMC_ITM_DBUFF_SIZE_BYTES_512:
    str = "bytes_512";
  break;
  case SOC_TMC_ITM_DBUFF_SIZE_BYTES_1024:
    str = "bytes_1024";
  break;
  case SOC_TMC_ITM_DBUFF_SIZE_BYTES_2048:
    str = "bytes_2048";
  break;
  case SOC_TMC_ITM_NOF_DBUFF_SIZES:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_ITM_VSQ_GROUP_SIZE_to_string(
     SOC_SAND_IN int unit,
     SOC_SAND_IN SOC_TMC_ITM_VSQ_GROUP_SIZE enum_val
  )
{
  const char* str = NULL;
  if(enum_val == SOC_DPP_DEFS_GET(unit, nof_vsq_a))
  {
    str = "vsq_groupa_sze";
  } else if (enum_val == SOC_DPP_DEFS_GET(unit, nof_vsq_b)) {
    str = "vsq_groupb_sze";
  } else if (enum_val == SOC_DPP_DEFS_GET(unit, nof_vsq_c)) {
    str = "vsq_groupc_sze";
  } else if (enum_val == SOC_DPP_DEFS_GET(unit, nof_vsq_d)) {
    str = "vsq_groupd_sze";
  } else {
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_ITM_VSQ_NDX_RNG_to_string(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_NDX_RNG enum_val
  )
{
  const char* str = NULL;
  if (enum_val == SOC_TMC_ITM_VSQ_NDX_MIN(unit))
  {
      str = "vsq_ndx_min";
  } 
  else if (enum_val == SOC_TMC_ITM_VSQ_NDX_MAX(unit)) 
  {
      str = "vsq_ndx_max";
  } 
  else 
  {
      str = " Unknown";
  }   
  return str;
}

const char*
  SOC_TMC_ITM_ADMIT_TSTS_to_string(
    SOC_SAND_IN  SOC_TMC_ITM_ADMIT_TSTS enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_ITM_ADMIT_TST_00:
    str = "admit_tst_00";
  break;
  case SOC_TMC_ITM_ADMIT_TST_01:
    str = "admit_tst_01";
  break;
  case SOC_TMC_ITM_ADMIT_TST_02:
    str = "admit_tst_02";
  break;
  case SOC_TMC_ITM_ADMIT_TST_03:
    str = "admit_tst_03";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_ITM_VSQ_GROUP_to_string(
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_ITM_VSQ_GROUP_CTGRY:
    str = "ctgry";
  break;
  case SOC_TMC_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
    str = "ctgry_traffic_cls";
  break;
  case SOC_TMC_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
    str = "ctgry_2_3_cnctn_cls";
  break;
  case SOC_TMC_ITM_VSQ_GROUP_STTSTCS_TAG:
    str = "sttstcs_tag";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_ITM_STAG_ENABLE_MODE_to_string(
    SOC_SAND_IN SOC_TMC_ITM_STAG_ENABLE_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_ITM_STAG_ENABLE_MODE_DISABLED:
    str = "DISABLED";
  break;

  case SOC_TMC_ITM_STAG_ENABLE_MODE_STAT_IF_NO_DEQ:
    str = "STAT_IF_NO_DEQ";
  break;

  case SOC_TMC_ITM_STAG_ENABLE_MODE_ENABLED_WITH_DEQ:
    str = "ENABLED_WITH_DEQ";
  break;

  case SOC_TMC_ITM_NOF_STAG_ENABLE_MODES:
    str = "NOF_ENABLE_MODES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_ITM_DRAM_BUFFERS_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_DRAM_BUFFERS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Dbuff_size %s [Bytes] \n\r"),
           SOC_TMC_ITM_DBUFF_SIZE_BYTES_to_string(info->dbuff_size)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "Uc_nof_buffs: %u[Buffers]\n\r"),info->uc_nof_buffs));
  LOG_CLI((BSL_META_U(unit,
                      "Full_mc_nof_buffs: %u[Buffers]\n\r"),info->full_mc_nof_buffs));
  LOG_CLI((BSL_META_U(unit,
                      "Mini_mc_nof_buffs: %u[Buffers]\n\r"),info->mini_mc_nof_buffs));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE_print(
    SOC_SAND_IN SOC_TMC_ITM_GLOB_RCS_FC_TYPE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Hp: ")));
  SOC_TMC_THRESH_WITH_HYST_INFO_print(&(info->hp));
  LOG_CLI((BSL_META_U(unit,
                      "Lp: ")));
  SOC_TMC_THRESH_WITH_HYST_INFO_print(&(info->lp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_GLOB_RCS_FC_TH_print(
    SOC_SAND_IN SOC_TMC_ITM_GLOB_RCS_FC_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      " +---------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " |  Resource |   SET    |  CLEAR   |\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " +---------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | BDB-HP    | %8u | %8u |\n\r"), info->bdbs.hp.set, info->bdbs.hp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " | BDB-LP    | %8u | %8u |\n\r"), info->bdbs.lp.set, info->bdbs.lp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " +---------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | UC-HP     | %8u | %8u |\n\r"), info->unicast.hp.set, info->unicast.hp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " | UC-LP     | %8u | %8u |\n\r"), info->unicast.lp.set, info->unicast.lp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " +---------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | FullMC-HP | %8u | %8u |\n\r"), info->full_mc.hp.set, info->full_mc.hp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " | FullMC-LP | %8u | %8u |\n\r"), info->full_mc.lp.set, info->full_mc.lp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " +---------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | MiniMC-HP | %8u | %8u |\n\r"), info->mini_mc.hp.set, info->mini_mc.hp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " | MiniMC-LP | %8u | %8u |\n\r"), info->mini_mc.lp.set, info->mini_mc.lp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " +---------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | OCB-HP | %8u | %8u |\n\r"), info->ocb.hp.set, info->ocb.hp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " | OCB-LP | %8u | %8u |\n\r"), info->ocb.lp.set, info->ocb.lp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " +---------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | OCB_P0-HP | %8u | %8u |\n\r"), info->ocb_p0.hp.set, info->ocb_p0.hp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " | OCB_P0-LP | %8u | %8u |\n\r"), info->ocb_p0.lp.set, info->ocb_p0.lp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " +---------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | OCB_P1-HP | %8u | %8u |\n\r"), info->ocb_p1.hp.set, info->ocb_p1.hp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " | OCB_P1-LP | %8u | %8u |\n\r"), info->ocb_p1.lp.set, info->ocb_p1.lp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " +---------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | MIX_P0-HP | %8u | %8u |\n\r"), info->mix_p0.hp.set, info->mix_p0.hp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " | MIX_P0-LP | %8u | %8u |\n\r"), info->mix_p0.lp.set, info->mix_p0.lp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " +---------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | MIX_P1-HP | %8u | %8u |\n\r"), info->mix_p1.hp.set, info->mix_p1.hp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " | MIX_P1-LP | %8u | %8u |\n\r"), info->mix_p1.lp.set, info->mix_p1.lp.clear));
  LOG_CLI((BSL_META_U(unit,
                      " +---------------------------------+\n\r")));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_GLOB_RCS_DROP_TH_print_no_table(
    SOC_SAND_IN SOC_TMC_ITM_GLOB_RCS_DROP_TH *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Bdbs[%u]: "),ind));
    SOC_TMC_THRESH_WITH_HYST_INFO_print(&(info->bdbs[ind]));
  }
  for (ind=0; ind<SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Bds[%u]: "),ind));
    SOC_TMC_THRESH_WITH_HYST_INFO_print(&(info->bds[ind]));
  }
  for (ind=0; ind<SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Unicast[%u]: "),ind));
    SOC_TMC_THRESH_WITH_HYST_INFO_print(&(info->unicast[ind]));
  }
  for (ind=0; ind<SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Full_mc[%u]: "),ind));
    SOC_TMC_THRESH_WITH_HYST_INFO_print(&(info->full_mc[ind]));
  }
  for (ind=0; ind<SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Mini_mc[%u]: "),ind));
    SOC_TMC_THRESH_WITH_HYST_INFO_print(&(info->mini_mc[ind]));
  }
  for (ind=0; ind<SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Mem_size[%u]: "),ind));
    SOC_TMC_THRESH_WITH_HYST_INFO_print(&(info->mem_excess[ind]));
  }
  for (ind=0; ind<SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "OCB_UC[%u]: "),ind));
    SOC_TMC_THRESH_WITH_HYST_INFO_print(&(info->ocb_uc[ind]));
  }
  for (ind=0; ind<SOC_TMC_NOF_DROP_PRECEDENCE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "OCB_MC[%u]: "),ind));
    SOC_TMC_THRESH_WITH_HYST_INFO_print(&(info->ocb_mc[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
   SOC_TMC_ITM_GLOB_RCS_DROP_TH_print(
    SOC_SAND_IN SOC_TMC_ITM_GLOB_RCS_DROP_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      " +-------------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | Resource| DP|   SET     |  CLEAR    |\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " +-------------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | BDB     | 0 | %9u | %9u |\n\r"), info->bdbs[0].set, info->bdbs[0].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | BDB     | 1 | %9u | %9u |\n\r"), info->bdbs[1].set, info->bdbs[1].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | BDB     | 2 | %9u | %9u |\n\r"), info->bdbs[2].set, info->bdbs[2].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | BDB     | 3 | %9u | %9u |\n\r"), info->bdbs[3].set, info->bdbs[3].clear));
  LOG_CLI((BSL_META_U(unit,
                      " +-------------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | BD      | 0 | %9u | %9u |\n\r"), info->bds[0].set, info->bds[0].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | BD      | 1 | %9u | %9u |\n\r"), info->bds[1].set, info->bds[1].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | BD      | 2 | %9u | %9u |\n\r"), info->bds[2].set, info->bds[2].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | BD      | 3 | %9u | %9u |\n\r"), info->bds[3].set, info->bds[3].clear));
  LOG_CLI((BSL_META_U(unit,
                      " +-------------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | UC      | 0 | %9u | %9u |\n\r"), info->unicast[0].set, info->unicast[0].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | UC      | 1 | %9u | %9u |\n\r"), info->unicast[1].set, info->unicast[1].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | UC      | 2 | %9u | %9u |\n\r"), info->unicast[2].set, info->unicast[2].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | UC      | 3 | %9u | %9u |\n\r"), info->unicast[3].set, info->unicast[3].clear));
  LOG_CLI((BSL_META_U(unit,
                      " +-------------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | Full MC | 0 | %9u | %9u |\n\r"), info->full_mc[0].set, info->full_mc[0].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | Full MC | 1 | %9u | %9u |\n\r"), info->full_mc[1].set, info->full_mc[1].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | Full MC | 2 | %9u | %9u |\n\r"), info->full_mc[2].set, info->full_mc[2].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | Full MC | 3 | %9u | %9u |\n\r"), info->full_mc[3].set, info->full_mc[3].clear));
  LOG_CLI((BSL_META_U(unit,
                      " +-------------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | Mini-MC | 0 | %9u | %9u |\n\r"), info->mini_mc[0].set, info->mini_mc[0].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | Mini-MC | 1 | %9u | %9u |\n\r"), info->mini_mc[1].set, info->mini_mc[1].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | Mini-MC | 2 | %9u | %9u |\n\r"), info->mini_mc[2].set, info->mini_mc[2].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | Mini-MC | 3 | %9u | %9u |\n\r"), info->mini_mc[3].set, info->mini_mc[3].clear));
  LOG_CLI((BSL_META_U(unit,
                      " +-------------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | MemSize | 0 | %9u | %9u |\n\r"), info->mem_excess[0].set, info->mem_excess[0].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | MemSize | 1 | %9u | %9u |\n\r"), info->mem_excess[1].set, info->mem_excess[1].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | MemSize | 2 | %9u | %9u |\n\r"), info->mem_excess[2].set, info->mem_excess[2].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | MemSize | 3 | %9u | %9u |\n\r"), info->mem_excess[3].set, info->mem_excess[3].clear));
  LOG_CLI((BSL_META_U(unit,
                      " +-------------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | OCB UC  | 0 | %9u | %9u |\n\r"), info->ocb_uc[0].set, info->ocb_uc[0].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | OCB UC  | 1 | %9u | %9u |\n\r"), info->ocb_uc[1].set, info->ocb_uc[1].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | OCB UC  | 2 | %9u | %9u |\n\r"), info->ocb_uc[2].set, info->ocb_uc[2].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | OCB UC  | 3 | %9u | %9u |\n\r"), info->ocb_uc[3].set, info->ocb_uc[3].clear));
  LOG_CLI((BSL_META_U(unit,
                      " +-------------------------------------+\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " | OCB MC  | 0 | %9u | %9u |\n\r"), info->ocb_mc[0].set, info->ocb_mc[0].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | OCB MC  | 1 | %9u | %9u |\n\r"), info->ocb_mc[1].set, info->ocb_mc[1].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | OCB MC  | 2 | %9u | %9u |\n\r"), info->ocb_mc[2].set, info->ocb_mc[2].clear));
  LOG_CLI((BSL_META_U(unit,
                      " | OCB MC  | 3 | %9u | %9u |\n\r"), info->ocb_mc[3].set, info->ocb_mc[3].clear));
  LOG_CLI((BSL_META_U(unit,
                      " +-------------------------------------+\n\r")));


exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_QUEUE_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_QUEUE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Cr_req_type_ndx:    %u\n\r"),info->cr_req_type_ndx));
  LOG_CLI((BSL_META_U(unit,
                      "Credit_cls:         %u\n\r"),info->credit_cls));
  LOG_CLI((BSL_META_U(unit,
                      "Rate_cls:           %u\n\r"),info->rate_cls));
  LOG_CLI((BSL_META_U(unit,
                      "Vsq_connection_cls: %u\n\r"),info->vsq_connection_cls));
  LOG_CLI((BSL_META_U(unit,
                      "Vsq_traffic_cls:    %u\n\r"),info->vsq_traffic_cls));
  LOG_CLI((BSL_META_U(unit,
                      "Signature:          %u\n\r"),info->signature));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CATEGORY_RNGS_print(
    SOC_SAND_IN SOC_TMC_ITM_CATEGORY_RNGS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Vsq_ctgry0_end: %u\n\r"),info->vsq_ctgry0_end));
  LOG_CLI((BSL_META_U(unit,
                      "Vsq_ctgry1_end: %u\n\r"),info->vsq_ctgry1_end));
  LOG_CLI((BSL_META_U(unit,
                      "Vsq_ctgry2_end: %u\n\r"),info->vsq_ctgry2_end));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT_print(
    SOC_SAND_IN SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "  Ctgry_test_en:          %d\n\r"),info->ctgry_test_en));
  LOG_CLI((BSL_META_U(unit,
                      "  Ctgry_trffc_test_en:    %d\n\r"),info->ctgry_trffc_test_en));
  LOG_CLI((BSL_META_U(unit,
                      "  Ctgry2_3_cnctn_test_en: %d\n\r"),info->ctgry2_3_cnctn_test_en));
  LOG_CLI((BSL_META_U(unit,
                      "  Sttstcs_tag_test_en:    %d\n\r"),info->sttstcs_tag_test_en));
  LOG_CLI((BSL_META_U(unit,
                      "  Pfc_test_en:            %d\n\r"),info->pfc_test_en));
  LOG_CLI((BSL_META_U(unit,
                      "  Llfc_test_en:           %d\n\r"),info->llfc_test_en));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Test_a:\n\r")));
  SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT_print(&(info->test_a));
  LOG_CLI((BSL_META_U(unit,
                      "Test_b:\n\r")));
  SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT_print(&(info->test_b));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_REQUEST_HUNGRY_TH_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_REQUEST_HUNGRY_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "  Off_to_slow_th: %d[Bytes]\n\r"),info->off_to_slow_th));
  LOG_CLI((BSL_META_U(unit,
                      "  Off_to_normal_th: %d[Bytes]\n\r"),info->off_to_normal_th));
  LOG_CLI((BSL_META_U(unit,
                      "  Slow_to_normal_th: %d[Bytes]\n\r"),info->slow_to_normal_th));
  LOG_CLI((BSL_META_U(unit,
                      "  Normal_to_slow_th: %d[Bytes]\n\r"),info->normal_to_slow_th));
  LOG_CLI((BSL_META_U(unit,
                      "  Multiplier: %u[Bytes]\n\r"),info->multiplier));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_REQUEST_BACKOFF_TH_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_REQUEST_BACKOFF_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "    Backoff_enter_th: %d[Bytes]\n\r"),info->backoff_enter_th));
  LOG_CLI((BSL_META_U(unit,
                      "    Backoff_exit_th: %d[Bytes]\n\r"),info->backoff_exit_th));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_REQUEST_BACKLOG_TH_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_REQUEST_BACKLOG_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "    Backlog_enter_th: %d[Bytes]\n\r"),info->backlog_enter_th));
  LOG_CLI((BSL_META_U(unit,
                      "    Backlog_exit_th: %d[Bytes]\n\r"),info->backlog_exit_th));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_REQUEST_EMPTY_Q_TH_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_REQUEST_EMPTY_Q_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "    Satisfied_empty_q_th: %d[Bytes]\n\r"),info->satisfied_empty_q_th));
  LOG_CLI((BSL_META_U(unit,
                      "    Max_credit_balance_empty_q: %d[Bytes]\n\r"),info->max_credit_balance_empty_q));
  LOG_CLI((BSL_META_U(unit,
                      "    Exceed_max_empty_q: %d\n\r"),info->exceed_max_empty_q));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_REQUEST_SATISFIED_TH_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_REQUEST_SATISFIED_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "  Backoff_th:\n\r")));
  SOC_TMC_ITM_CR_REQUEST_BACKOFF_TH_print(&(info->backoff_th));
  LOG_CLI((BSL_META_U(unit,
                      "  Backlog_th:\n\r")));
  SOC_TMC_ITM_CR_REQUEST_BACKLOG_TH_print(&(info->backlog_th));
  LOG_CLI((BSL_META_U(unit,
                      "  Empty_queues:\n\r")));
  SOC_TMC_ITM_CR_REQUEST_EMPTY_Q_TH_print(&(info->empty_queues));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_WD_Q_TH_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_WD_Q_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "  Cr_wd_stts_msg_gen: %u[msec]\n\r"),info->cr_wd_stts_msg_gen));
  LOG_CLI((BSL_META_U(unit,
                      "  Cr_wd_dlt_q_th: %u[msec]\n\r"),info->cr_wd_dlt_q_th));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_REQUEST_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_REQUEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Wd_th:\n\r")));
  SOC_TMC_ITM_CR_WD_Q_TH_print(&(info->wd_th));
  LOG_CLI((BSL_META_U(unit,
                      "Hungry_th:\n\r")));
  SOC_TMC_ITM_CR_REQUEST_HUNGRY_TH_print(&(info->hungry_th));
  LOG_CLI((BSL_META_U(unit,
                      "Satisfied_th:\n\r")));
  SOC_TMC_ITM_CR_REQUEST_SATISFIED_TH_print(&(info->satisfied_th));
  LOG_CLI((BSL_META_U(unit,
                      "Is_low_latency: %s\n\r"), info->is_low_latency?"TRUE":"FALSE"));
  LOG_CLI((BSL_META_U(unit,
                      "Is_remote_credit_value: %s\n\r"), info->is_remote_credit_value?"TRUE":"FALSE"));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_DISCOUNT_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_DISCOUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Discount: %d[Bytes]\n\r"),info->discount));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_WRED_QT_DP_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_WRED_QT_DP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Wred_en:            %d\n\r"),info->wred_en));
  LOG_CLI((BSL_META_U(unit,
                      "Ignore_packet_size: %d\n\r"),info->ignore_packet_size));
  LOG_CLI((BSL_META_U(unit,
                      "Min_avrg_th:        %u[Bytes]\n\r"),info->min_avrg_th));
  LOG_CLI((BSL_META_U(unit,
                      "Max_avrg_th:        %u[Bytes]\n\r"),info->max_avrg_th));
  LOG_CLI((BSL_META_U(unit,
                      "Max_packet_size:    %u[Bytes]\n\r"),info->max_packet_size));
  LOG_CLI((BSL_META_U(unit,
                      "Max_probability:    %u[%%]\n\r"),info->max_probability));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_TAIL_DROP_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_TAIL_DROP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Max_inst_q_size: %u[Bytes]\n\r"),info->max_inst_q_size));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_CR_WD_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_WD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Min_scan_cycle_period_micro: %u[ms]\n\r"),info->min_scan_cycle_period_micro));
  LOG_CLI((BSL_META_U(unit,
                      "Max_flow_msg_gen_rate_nano:  %u[ns]\n\r"),info->max_flow_msg_gen_rate_nano));
  LOG_CLI((BSL_META_U(unit,
                      "Bottom_queue:                %u\n\r"),info->bottom_queue));
  LOG_CLI((BSL_META_U(unit,
                      "Top_queue:                   %u\n\r"),info->top_queue));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_VSQ_FC_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_VSQ_FC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Q_size_fc: ")));
  SOC_TMC_THRESH_WITH_HYST_INFO_print(&(info->q_size_fc));
  LOG_CLI((BSL_META_U(unit,
                      "Bd_size_fc: ")));
  SOC_TMC_THRESH_WITH_HYST_INFO_print(&(info->bd_size_fc));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_VSQ_TAIL_DROP_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_VSQ_TAIL_DROP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Max_inst_q_size: %u[Bytes]\n\r"),info->max_inst_q_size));
  LOG_CLI((BSL_META_U(unit,
                      "Max_inst_q_size_bds: %u[Buffer descriptors]\n\r"),info->max_inst_q_size_bds));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_VSQ_WRED_GEN_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_VSQ_WRED_GEN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Wred_en: %d\n\r"),info->wred_en));
  LOG_CLI((BSL_META_U(unit,
                      "Exp_wq: %u\n\r"),info->exp_wq));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_STAG_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_STAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "enable_mode %s \n\r"),
           SOC_TMC_ITM_STAG_ENABLE_MODE_to_string(info->enable_mode)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "  Vsq_index_msb: %u\n\r"),info->vsq_index_msb));
  LOG_CLI((BSL_META_U(unit,
                      "  Vsq_index_lsb: %u\n\r"),info->vsq_index_lsb));
  LOG_CLI((BSL_META_U(unit,
                      "  Dropp_en: %u\n\r"),info->dropp_en));
  LOG_CLI((BSL_META_U(unit,
                      "  Dropp_lsb: %u\n\r"),info->dropp_lsb));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_INGRESS_SHAPE_Q_RANGE_print(
    SOC_SAND_IN SOC_TMC_ITM_INGRESS_SHAPE_Q_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "  Q_num_low: %u\n\r"),info->q_num_low));
  LOG_CLI((BSL_META_U(unit,
                      "  Q_num_high: %u\n\r"),info->q_num_high));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_INGRESS_SHAPE_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_INGRESS_SHAPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Enable: %u\n\r"),info->enable));
  LOG_CLI((BSL_META_U(unit,
                      "Q_range:\n\r")));
  SOC_TMC_ITM_INGRESS_SHAPE_Q_RANGE_print(&(info->q_range));
  LOG_CLI((BSL_META_U(unit,
                      "Rate: %u[Kbps]\n\r"),info->rate));
  LOG_CLI((BSL_META_U(unit,
                      "Sch_port: %u\n\r"),info->sch_port));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_PRIORITY_MAP_TMPLT_print(
    SOC_SAND_IN SOC_TMC_ITM_PRIORITY_MAP_TMPLT *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_TMC_ITM_PRIO_MAP_SIZE_IN_UINT32S; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Map[%u]: %u\n\r"),ind,info->map[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_SYS_RED_DROP_PROB_print(
    SOC_SAND_IN SOC_TMC_ITM_SYS_RED_DROP_PROB *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_DRP_PROBS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Drop_probs[%u]: %u[.01%%]\n\r"),ind,info->drop_probs[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_SYS_RED_QT_DP_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_SYS_RED_QT_DP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Enable:        %d\n\r"),info->enable));
  LOG_CLI((BSL_META_U(unit,
                      "Adm_th:        %u\n\r"),info->adm_th));
  LOG_CLI((BSL_META_U(unit,
                      "Prob_th:       %u\n\r"),info->prob_th));
  LOG_CLI((BSL_META_U(unit,
                      "Drp_th:        %u\n\r"),info->drp_th));
  LOG_CLI((BSL_META_U(unit,
                      "Drp_prob_low:  %u\n\r"),info->drp_prob_low));
  LOG_CLI((BSL_META_U(unit,
                      "Drp_prob_high: %u\n\r"),info->drp_prob_high));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_SYS_RED_QT_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_SYS_RED_QT_INFO *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_Q_SIZE_RANGES; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Queue_size_boundaries[%u]: %u[Bytes]\n\r"),ind,info->queue_size_boundaries[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_SYS_RED_EG_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_SYS_RED_EG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Enable:                %d\n\r"),info->enable));
  LOG_CLI((BSL_META_U(unit,
                      "Aging_timer:           %u[ms]\n\r"),info->aging_timer));
  LOG_CLI((BSL_META_U(unit,
                      "Reset_expired_q_size:  %d\n\r"),info->reset_expired_q_size));
  LOG_CLI((BSL_META_U(unit,
                      "Aging_only_dec_q_size: %d\n\r"),info->aging_only_dec_q_size));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_THS_print(
    SOC_SAND_IN SOC_TMC_ITM_SYS_RED_GLOB_RCS_THS *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_BUFFS_RNGS-1; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "  Unicast_rng_ths[%u]: %u\n\r"),ind,info->unicast_rng_ths[ind]));
  }
  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_BUFFS_RNGS-1; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "  Multicast_rng_ths[%u]: %u\n\r"),ind,info->multicast_rng_ths[ind]));
  }
  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_BUFFS_RNGS-1; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "  Bds_rng_ths[%u]: %u\n\r"),ind,info->bds_rng_ths[ind]));
  }
  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_BUFFS_RNGS-1; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "  Ocb_rng_ths[%u]: %u\n\r"),ind,info->ocb_rng_ths[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_VALS_print(
    SOC_SAND_IN SOC_TMC_ITM_SYS_RED_GLOB_RCS_VALS *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_BUFFS_RNGS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "  Unicast_rng_vals[%u]: %u\n\r"),ind,info->unicast_rng_vals[ind]));
  }
  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_BUFFS_RNGS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "  Multicast_rng_vals[%u]: %u\n\r"),ind,info->multicast_rng_vals[ind]));
  }
  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_BUFFS_RNGS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "  Bds_rng_vals[%u]: %u\n\r"),ind,info->bds_rng_vals[ind]));
  }
  for (ind=0; ind<SOC_TMC_ITM_SYS_RED_BUFFS_RNGS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "  Ocb_rng_vals[%u]: %u\n\r"),ind,info->ocb_rng_vals[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_SYS_RED_GLOB_RCS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Thresholds:\n\r")));
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_THS_print(&(info->thresholds));
  LOG_CLI((BSL_META_U(unit,
                      "Values:\n\r")));
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_VALS_print(&(info->values));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

