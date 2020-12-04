/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/





#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_flow_control.h>
#include <soc/dpp/TMC/tmc_api_framework.h>


























void SOC_TMC_FC_PFC_GENERIC_BITMAP_clear(SOC_SAND_OUT SOC_TMC_FC_PFC_GENERIC_BITMAP *generic_bm)
{
  uint32
    i;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(generic_bm);

  sal_memset(generic_bm, 0x0, sizeof(SOC_TMC_FC_PFC_GENERIC_BITMAP));

  for(i = 0; i<SOC_TMC_FC_PFC_GENERIC_BITMAP_SIZE / 32; i++)
  {
    generic_bm->bitmap[i] = 0;
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void SOC_TMC_FC_CAL_IF_INFO_clear(SOC_SAND_OUT SOC_TMC_FC_CAL_IF_INFO *cal_info)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(cal_info);

  sal_memset(cal_info, 0x0, sizeof(SOC_TMC_FC_CAL_IF_INFO));

  cal_info->enable = 0;
  cal_info->cal_len = 0;
  cal_info->cal_reps = 0;


exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FC_GEN_INBND_CB_clear(
    SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_CB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_FC_GEN_INBND_CB));
  info->inherit = SOC_TMC_FC_NOF_INBND_CB_INHERITS;
  info->glbl_rcs_low = 0;
  info->cnm_intercept_enable = 0;
  info->nif_cls_bitmap = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_TMC_FC_GEN_INBND_LL_clear(
    SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_LL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_FC_GEN_INBND_LL));
  info->cnm_enable = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FC_REC_INBND_CB_clear(
    SOC_SAND_OUT SOC_TMC_FC_REC_INBND_CB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_FC_REC_INBND_CB));
  info->inherit = SOC_TMC_FC_NOF_INBND_CB_INHERITS;
  info->sch_hr_bitmap = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FC_GEN_INBND_INFO_clear(
    SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_FC_GEN_INBND_INFO));
  info->mode = SOC_TMC_FC_NOF_INBND_MODES;
  SOC_TMC_FC_GEN_INBND_CB_clear(&(info->cb));
  SOC_TMC_FC_GEN_INBND_LL_clear(&(info->ll));
  SOC_TMC_FC_GEN_INBND_PFC_clear(&(info->pfc));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FC_GEN_INBND_PFC_clear(
    SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_PFC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_TMC_FC_GEN_INBND_PFC));
  info->inherit = SOC_TMC_FC_NOF_INBND_PFC_INHERITS;
  info->glbl_rcs_low = 0;
  info->cnm_intercept_enable = 0;
  info->nif_cls_bitmap = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FC_REC_CALENDAR_clear(
    SOC_SAND_OUT SOC_TMC_FC_REC_CALENDAR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_FC_REC_CALENDAR));
  info->destination = SOC_TMC_FC_NOF_REC_CAL_DESTS;
  info->id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FC_GEN_CALENDAR_clear(
    SOC_SAND_OUT SOC_TMC_FC_GEN_CALENDAR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_FC_GEN_CALENDAR));
  info->source = SOC_TMC_FC_NOF_GEN_CAL_SRCS;
  info->id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FC_REC_INBND_INFO_clear(
    SOC_SAND_OUT SOC_TMC_FC_REC_INBND_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_FC_REC_INBND_INFO));
  info->mode = SOC_TMC_FC_NOF_INBND_MODES;
  SOC_TMC_FC_REC_INBND_CB_clear(&(info->cb));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FC_REC_OFP_MAP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_FC_REC_OFP_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_FC_REC_OFP_MAP_INFO));
  info->react_point = SOC_TMC_FC_NOF_REC_OFP_RPS;
  info->ofp_ndx = 0;
  info->priority = SOC_TMC_FC_NOF_OFP_PRIORITYS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FC_ILKN_LLFC_INFO_clear(
    SOC_SAND_OUT SOC_TMC_FC_ILKN_LLFC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_FC_ILKN_LLFC_INFO));
  info->multi_use_mask = 0;
  info->cal_channel = SOC_TMC_FC_NOF_ILKN_CAL_LLFCS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>
