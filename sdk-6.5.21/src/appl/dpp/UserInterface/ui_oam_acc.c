/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
#include <bcm_app/dpp/../H/usrApp.h>

/*
 * Utilities include file.
 */

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <appl/diag/dpp/utils_defi.h>
#include <appl/dpp/UserInterface/ui_defi.h>

#include <soc/dpp/OAM/oam_tbl_access.h>
#include <soc/dpp/OAM/oam_reg_access.h>
#include <soc/dpp/OAM/oam_api_framework.h>
#include <soc/dpp/OAM/oam_framework.h>
#include <soc/dpp/OAM/oam_chip_regs.h>

#include <appl/dpp/UserInterface/ui_pure_defi_oam_acc.h>

#define UI_OAM_ACC_OAM 1
#define UI_OAM_REG_ACCESS 1

uint32
  oam_get_default_unit();

void
  OAM_MOS_DB_TBL_DATA_clear(
    SOC_SAND_OUT OAM_MOS_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(OAM_MOS_DB_TBL_DATA));
  info->cv_ffd_enable = 0;
  info->is_ffd = 0;
  info->bdi = 0;
  info->fdi = 0;
  info->ffd_rate = 0;
  info->start = 0;
exit:
  return;
}
void
  OAM_MOR_DB_TBL_DATA_clear(
    SOC_SAND_OUT OAM_MOR_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(OAM_MOR_DB_TBL_DATA));
  info->ffd_rx_rate = 0;
  info->cnt_tm_1 = 0;
  info->cnt_tm_2 = 0;
  info->cnt_tm_3 = 0;
  info->d_locv_state = 0;
  info->d_excess_state = 0;
  info->last_fdi_receive_time = 0;
  info->last_bdi_receive_time = 0;
  info->mot_db_index = 0;
exit:
  return;
}
void
  OAM_MOR_DB_CPU_TBL_DATA_clear(
    SOC_SAND_OUT OAM_MOR_DB_CPU_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(OAM_MOR_DB_CPU_TBL_DATA));
  info->is_ffd = 0;
  info->reserved_1 = 0xf;
  info->reserved_2 = 0xf;
  info->enable = 0;
exit:
  return;
}
void
  OAM_SYS_PORT_TBL_DATA_clear(
    SOC_SAND_OUT OAM_SYS_PORT_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(OAM_SYS_PORT_TBL_DATA));
  info->sys_port = 0;
exit:
  return;
}
void
  OAM_LSR_ID_TBL_DATA_clear(
    SOC_SAND_OUT OAM_LSR_ID_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(OAM_LSR_ID_TBL_DATA));
  for (ind=0; ind<4; ++ind)
  {
    info->lsr_id[ind] = 0;
  }
exit:
  return;
}
void
  OAM_DEFECT_TYPE_TBL_DATA_clear(
    SOC_SAND_OUT OAM_DEFECT_TYPE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(OAM_DEFECT_TYPE_TBL_DATA));
  info->defect_type = 0;
exit:
  return;
}
void
  OAM_MEP_DB_TBL_DATA_clear(
    SOC_SAND_OUT OAM_MEP_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(OAM_MEP_DB_TBL_DATA));
  info->ccm_interval = 0;
  info->maid = 0;
  info->ccm_tx_start = 0;
  info->dm_enable = 0;
  info->dm_db_index = 0;
  info->port = 0;
  info->up1down0 = 0;
exit:
  return;
}
void
  OAM_MEP_INDEX_DB_TBL_DATA_clear(
    SOC_SAND_OUT OAM_MEP_INDEX_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(OAM_MEP_INDEX_DB_TBL_DATA));
  info->mep_db_ptr = 0;
  info->accelerated = 0;
exit:
  return;
}
void
  OAM_RMEP_DB_TBL_DATA_clear(
    SOC_SAND_OUT OAM_RMEP_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(OAM_RMEP_DB_TBL_DATA));
  info->last_ccmtime = 0;
  info->last_ccmtime_valid = 0;
  info->ccminterval = 0;
  info->ccmdefect = 0;
  info->rcvd_rdi = 0;
exit:
  return;
}
void
  OAM_RMEP_HASH_0_DB_TBL_DATA_clear(
    SOC_SAND_OUT OAM_RMEP_HASH_0_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(OAM_RMEP_HASH_0_DB_TBL_DATA));
  info->rmep_db_ptr0 = 0;
  info->verifier0 = 0;
  info->valid_hash0 = 0;
exit:
  return;
}
void
  OAM_RMEP_HASH_1_DB_TBL_DATA_clear(
    SOC_SAND_OUT OAM_RMEP_HASH_1_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(OAM_RMEP_HASH_1_DB_TBL_DATA));
  info->rmep_db_ptr1 = 0;
  info->verifier1 = 0;
  info->valid_hash1 = 0;
exit:
  return;
}
void
  OAM_LMDB_TBL_DATA_clear(
    SOC_SAND_OUT OAM_LMDB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(OAM_LMDB_TBL_DATA));
  info->tx_fcf_c = 0;
  info->tx_fcf_p = 0;
  info->rx_fcb_peer_c = 0;
  info->rx_fcb_peer_p = 0;
  info->rx_fcb_c = 0;
  info->rx_fcb_p = 0;
  info->tx_fcf_peer_c = 0;
  info->tx_fcf_peer_p = 0;
exit:
  return;
}
void
  OAM_MOT_DB_TBL_DATA_clear(
    SOC_SAND_OUT OAM_MOT_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(OAM_MOT_DB_TBL_DATA));
  info->sysport = 0;
  info->cos = 0;
  info->eep = 0;
  info->lsr_id_ptr = 0;
  info->lsp_id_lsb = 0;
  info->ttl_profile = 0;
  info->defect_type_ptr = 0;
  info->defect_location = 0;
exit:
  return;
}
void
  OAM_EXT_MEP_DB_TBL_DATA_clear(
    SOC_SAND_OUT OAM_EXT_MEP_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(OAM_EXT_MEP_DB_TBL_DATA));
  info->rdi_indicator = 0;
  info->mepid = 0;
  info->md_level = 0;
  info->vid = 0;
  info->priority = 0;
  info->lmenable = 0;
exit:
  return;
}

#if OAM_DEBUG

void
  OAM_MOS_DB_TBL_DATA_print(
    SOC_SAND_IN  OAM_MOS_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "cv_ffd_enable: %lu\n\r",info->cv_ffd_enable);
  soc_sand_os_printf( "is_ffd: %lu\n\r",info->is_ffd);
  soc_sand_os_printf( "bdi: %lu\n\r",info->bdi);
  soc_sand_os_printf( "fdi: %lu\n\r",info->fdi);
  soc_sand_os_printf( "ffd_rate: %lu\n\r",info->ffd_rate);
  soc_sand_os_printf( "start: %lu\n\r",info->start);
exit:
  return;
}

void
  OAM_MOR_DB_TBL_DATA_print(
    SOC_SAND_IN  OAM_MOR_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "ffd_rx_rate           0x%lx\r\n", info->ffd_rx_rate             );
  soc_sand_os_printf( "cnt_tm_1              0x%lx\r\n", info->cnt_tm_1                );
  soc_sand_os_printf( "cnt_tm_2              0x%lx\r\n", info->cnt_tm_2                );
  soc_sand_os_printf( "cnt_tm_3              0x%lx\r\n", info->cnt_tm_3                );
  soc_sand_os_printf( "d_locv_state          0x%lx\r\n", info->d_locv_state            );
  soc_sand_os_printf( "d_excess_state        0x%lx\r\n", info->d_excess_state          );
  soc_sand_os_printf( "last_fdi_receive_time 0x%lx\r\n", info->last_fdi_receive_time   );
  soc_sand_os_printf( "last_bdi_receive_time 0x%lx\r\n", info->last_bdi_receive_time   );
  soc_sand_os_printf( "mot_db_index          0x%lx\r\n", info->mot_db_index            );
exit:
  return;
}

void
  OAM_MOR_DB_CPU_TBL_DATA_print(
    SOC_SAND_IN  OAM_MOR_DB_CPU_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "is_ffd      0x%lx\r\n", info->is_ffd        );
  soc_sand_os_printf( "reserved_1  0x%lx\r\n", info->reserved_1    );
  soc_sand_os_printf( "reserved_2  0x%lx\r\n", info->reserved_2    );
  soc_sand_os_printf( "enable      0x%lx\r\n", info->enable        );
exit:
  return;
}

void
  OAM_SYS_PORT_TBL_DATA_print(
    SOC_SAND_IN  OAM_SYS_PORT_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "sys_port: %lu\n\r",info->sys_port);
exit:
  return;
}

void
  OAM_LSR_ID_TBL_DATA_print(
    SOC_SAND_IN  OAM_LSR_ID_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<4; ++ind)
  {
    soc_sand_os_printf( "lsr_id[%u]: %lu\n\r",ind,info->lsr_id[ind]);
  }
exit:
  return;
}

void
  OAM_DEFECT_TYPE_TBL_DATA_print(
    SOC_SAND_IN  OAM_DEFECT_TYPE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "defect_type: %lu\n\r",info->defect_type);
exit:
  return;
}

void
  OAM_MEP_DB_TBL_DATA_print(
    SOC_SAND_IN  OAM_MEP_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "ccm_interval: %lu\n\r",info->ccm_interval);
  soc_sand_os_printf( "maid: %lu\n\r",info->maid);
  soc_sand_os_printf( "ccm_tx_start: %lu\n\r",info->ccm_tx_start);
  soc_sand_os_printf( "dm_enable: %lu\n\r",info->dm_enable);
  soc_sand_os_printf( "dm_db_index: %lu\n\r",info->dm_db_index);
  soc_sand_os_printf( "port: %lu\n\r",info->port);
  soc_sand_os_printf( "up1down0: %lu\n\r",info->up1down0);
exit:
  return;
}

void
  OAM_MEP_INDEX_DB_TBL_DATA_print(
    SOC_SAND_IN  OAM_MEP_INDEX_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "mep_db_ptr: %lu\n\r",info->mep_db_ptr);
  soc_sand_os_printf( "accelerated: %lu\n\r",info->accelerated);
exit:
  return;
}

void
  OAM_RMEP_DB_TBL_DATA_print(
    SOC_SAND_IN  OAM_RMEP_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "last_ccmtime: %lu\n\r",info->last_ccmtime);
  soc_sand_os_printf( "last_ccmtime_valid: %lu\n\r",info->last_ccmtime_valid);
  soc_sand_os_printf( "ccminterval: %lu\n\r",info->ccminterval);
  soc_sand_os_printf( "ccmdefect: %lu\n\r",info->ccmdefect);
  soc_sand_os_printf( "rcvd_rdi: %lu\n\r",info->rcvd_rdi);
exit:
  return;
}

void
  OAM_RMEP_HASH_0_DB_TBL_DATA_print(
    SOC_SAND_IN  OAM_RMEP_HASH_0_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "rmep_db_ptr0: %lu\n\r",info->rmep_db_ptr0);
  soc_sand_os_printf( "verifier0: %lu\n\r",info->verifier0);
  soc_sand_os_printf( "valid_hash0: %lu\n\r",info->valid_hash0);
exit:
  return;
}

void
  OAM_RMEP_HASH_1_DB_TBL_DATA_print(
    SOC_SAND_IN  OAM_RMEP_HASH_1_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "rmep_db_ptr1: %lu\n\r",info->rmep_db_ptr1);
  soc_sand_os_printf( "verifier1: %lu\n\r",info->verifier1);
  soc_sand_os_printf( "valid_hash1: %lu\n\r",info->valid_hash1);
exit:
  return;
}

void
  OAM_LMDB_TBL_DATA_print(
    SOC_SAND_IN  OAM_LMDB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "tx_fcf_c: %lu\n\r",info->tx_fcf_c);
  soc_sand_os_printf( "tx_fcf_p: %lu\n\r",info->tx_fcf_p);
  soc_sand_os_printf( "rx_fcb_peer_c: %lu\n\r",info->rx_fcb_peer_c);
  soc_sand_os_printf( "rx_fcb_peer_p: %lu\n\r",info->rx_fcb_peer_p);
  soc_sand_os_printf( "rx_fcb_c: %lu\n\r",info->rx_fcb_c);
  soc_sand_os_printf( "rx_fcb_p: %lu\n\r",info->rx_fcb_p);
  soc_sand_os_printf( "tx_fcf_peer_c: %lu\n\r",info->tx_fcf_peer_c);
  soc_sand_os_printf( "tx_fcf_peer_p: %lu\n\r",info->tx_fcf_peer_p);
exit:
  return;
}

void
  OAM_MOT_DB_TBL_DATA_print(
    SOC_SAND_IN  OAM_MOT_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "sysport: %lu\n\r",info->sysport);
  soc_sand_os_printf( "cos: %lu\n\r",info->cos);
  soc_sand_os_printf( "eep: %lu\n\r",info->eep);
  soc_sand_os_printf( "lsr_id_ptr: %lu\n\r",info->lsr_id_ptr);
  soc_sand_os_printf( "lsp_id_lsb: %lu\n\r",info->lsp_id_lsb);
  soc_sand_os_printf( "ttl_profile: %lu\n\r",info->ttl_profile);
  soc_sand_os_printf( "defect_type_ptr: %lu\n\r",info->defect_type_ptr);
  soc_sand_os_printf( "defect_location: %lu\n\r",info->defect_location);
exit:
  return;
}

void
  OAM_EXT_MEP_DB_TBL_DATA_print(
    SOC_SAND_IN  OAM_EXT_MEP_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "rdi_indicator: %lu\n\r",info->rdi_indicator);
  soc_sand_os_printf( "mepid: %lu\n\r",info->mepid);
  soc_sand_os_printf( "md_level: %lu\n\r",info->md_level);
  soc_sand_os_printf( "vid: %lu\n\r",info->vid);
  soc_sand_os_printf( "priority: %lu\n\r",info->priority);
  soc_sand_os_printf( "lmenable: %lu\n\r",info->lmenable);
exit:
  return;
}

#endif

#ifdef UI_OAM_ACC_OAM

/********************************************************************
 *  Function handler: oamp_mos_db_tbl_set (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_mos_db_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_MOS_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_mos_db_tbl_set";

  unit = oam_get_default_unit();
  OAM_MOS_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_mos_db_tbl_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = oam_mos_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mos_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mos_db_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_START_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_START_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.start = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_FFD_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_FFD_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ffd_rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_FDI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_FDI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.fdi = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_BDI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_BDI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.bdi = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_CV0_FFD1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_CV0_FFD1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.is_ffd = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_CV_FFD_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_CV_FFD_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.cv_ffd_enable = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = oam_mos_db_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mos_db_tbl_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_mos_db_tbl_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_mos_db_tbl_get (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_mos_db_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_MOS_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_mos_db_tbl_get";

  unit = oam_get_default_unit();
  OAM_MOS_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOS_DB_TBL_GET_OAMP_MOS_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOS_DB_TBL_GET_OAMP_MOS_DB_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_mos_db_tbl_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = oam_mos_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mos_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mos_db_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  OAM_MOS_DB_TBL_DATA_print(&prm_tbl_data);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_mor_db_tbl_set (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_mor_db_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_MOR_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_mor_db_tbl_set";

  unit = oam_get_default_unit();
  OAM_MOR_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_mor_db_tbl_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = oam_mor_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mor_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mor_db_tbl_get");
    goto exit;
  }
                                                       
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_FFD_RX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_FFD_RX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ffd_rx_rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_CNT_TM_1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_CNT_TM_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.cnt_tm_1 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_CNT_TM_2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_CNT_TM_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.cnt_tm_2 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_CNT_TM_3_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_CNT_TM_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.cnt_tm_3 = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_D_LOCV_STATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_D_LOCV_STATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.d_locv_state = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_D_EXCESS_STATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_D_EXCESS_STATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.d_excess_state = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_LAST_FDI_RECEIVE_TIME_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_LAST_FDI_RECEIVE_TIME_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.last_fdi_receive_time = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_LAST_BDI_RECEIVE_TIME_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_LAST_BDI_RECEIVE_TIME_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.last_bdi_receive_time = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_MOT_DB_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_MOT_DB_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mot_db_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mot_db_index = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = oam_mor_db_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mor_db_tbl_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_mor_db_tbl_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_mor_db_tbl_get (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_mor_db_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_MOR_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_mor_db_tbl_get";

  unit = oam_get_default_unit();
  OAM_MOR_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_TBL_GET_OAMP_MOR_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_TBL_GET_OAMP_MOR_DB_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_mor_db_tbl_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = oam_mor_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mor_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mor_db_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  OAM_MOR_DB_TBL_DATA_print(&prm_tbl_data);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_mor_db_tbl_set (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_mor_db_cpu_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_MOR_DB_CPU_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_mor_db_tbl_set";

  unit = oam_get_default_unit();
  OAM_MOR_DB_CPU_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_CPU_TBL_SET_OAMP_MOR_DB_CPU_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_CPU_TBL_SET_OAMP_MOR_DB_CPU_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_mor_db_tbl_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = oam_mor_db_cpu_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mor_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mor_db_tbl_get");
    goto exit;
  }
                                                       
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_CPU_TBL_SET_OAMP_MOR_DB_CPU_TBL_SET_TBL_DATA_IS_FFD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_CPU_TBL_SET_OAMP_MOR_DB_CPU_TBL_SET_TBL_DATA_IS_FFD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.is_ffd = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_CPU_TBL_SET_OAMP_MOR_DB_CPU_TBL_SET_TBL_DATA_LAST_FDI_DEFECT_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_CPU_TBL_SET_OAMP_MOR_DB_CPU_TBL_SET_TBL_DATA_LAST_FDI_DEFECT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.reserved_1 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_CPU_TBL_SET_OAMP_MOR_DB_CPU_TBL_SET_TBL_DATA_LAST_BDI_DEFECT_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_CPU_TBL_SET_OAMP_MOR_DB_CPU_TBL_SET_TBL_DATA_LAST_BDI_DEFECT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.reserved_2 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_CPU_TBL_SET_OAMP_MOR_DB_CPU_TBL_SET_TBL_DATA_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_CPU_TBL_SET_OAMP_MOR_DB_CPU_TBL_SET_TBL_DATA_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.enable = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = oam_mor_db_cpu_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mor_db_tbl_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_mor_db_tbl_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_mor_db_tbl_get (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_mor_db_cpu_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_MOR_DB_CPU_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_mor_db_tbl_get";

  unit = oam_get_default_unit();
  OAM_MOR_DB_CPU_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_CPU_TBL_GET_OAMP_MOR_DB_CPU_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOR_DB_CPU_TBL_GET_OAMP_MOR_DB_CPU_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_mor_db_tbl_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = oam_mor_db_cpu_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mor_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mor_db_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  OAM_MOR_DB_CPU_TBL_DATA_print(&prm_tbl_data);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_sys_port_tbl_set (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_sys_port_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_SYS_PORT_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_sys_port_tbl_set";

  unit = oam_get_default_unit();
  OAM_SYS_PORT_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_SYS_PORT_TBL_SET_OAMP_SYS_PORT_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_SYS_PORT_TBL_SET_OAMP_SYS_PORT_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_sys_port_tbl_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = oam_sys_port_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_sys_port_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_sys_port_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_SYS_PORT_TBL_SET_OAMP_SYS_PORT_TBL_SET_TBL_DATA_SYS_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_SYS_PORT_TBL_SET_OAMP_SYS_PORT_TBL_SET_TBL_DATA_SYS_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.sys_port = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = oam_sys_port_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_sys_port_tbl_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_sys_port_tbl_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_sys_port_tbl_get (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_sys_port_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_SYS_PORT_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_sys_port_tbl_get";

  unit = oam_get_default_unit();
  OAM_SYS_PORT_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_SYS_PORT_TBL_GET_OAMP_SYS_PORT_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_SYS_PORT_TBL_GET_OAMP_SYS_PORT_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_sys_port_tbl_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = oam_sys_port_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_sys_port_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_sys_port_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  OAM_SYS_PORT_TBL_DATA_print(&prm_tbl_data);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_lsr_id_tbl_set (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_lsr_id_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  uint32
    prm_lsr_id_index = 0xFFFFFFFF;
  OAM_LSR_ID_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_lsr_id_tbl_set";

  unit = oam_get_default_unit();
  OAM_LSR_ID_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LSR_ID_TBL_SET_OAMP_LSR_ID_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_LSR_ID_TBL_SET_OAMP_LSR_ID_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_lsr_id_tbl_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = oam_lsr_id_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_lsr_id_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_lsr_id_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LSR_ID_TBL_SET_OAMP_LSR_ID_TBL_SET_TBL_DATA_LSR_ID_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_LSR_ID_TBL_SET_OAMP_LSR_ID_TBL_SET_TBL_DATA_LSR_ID_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lsr_id_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_lsr_id_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LSR_ID_TBL_SET_OAMP_LSR_ID_TBL_SET_TBL_DATA_LSR_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_LSR_ID_TBL_SET_OAMP_LSR_ID_TBL_SET_TBL_DATA_LSR_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.lsr_id[ prm_lsr_id_index] = (uint32)param_val->value.ulong_value;
  }

  }

  /* Call function */
  ret = oam_lsr_id_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_lsr_id_tbl_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_lsr_id_tbl_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_lsr_id_tbl_get (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_lsr_id_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_LSR_ID_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_lsr_id_tbl_get";

  unit = oam_get_default_unit();
  OAM_LSR_ID_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LSR_ID_TBL_GET_OAMP_LSR_ID_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_LSR_ID_TBL_GET_OAMP_LSR_ID_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_lsr_id_tbl_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = oam_lsr_id_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_lsr_id_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_lsr_id_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  OAM_LSR_ID_TBL_DATA_print(&prm_tbl_data);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_defect_type_tbl_set (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_defect_type_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_DEFECT_TYPE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_defect_type_tbl_set";

  unit = oam_get_default_unit();
  OAM_DEFECT_TYPE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_DEFECT_TYPE_TBL_SET_OAMP_DEFECT_TYPE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_DEFECT_TYPE_TBL_SET_OAMP_DEFECT_TYPE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_defect_type_tbl_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = oam_defect_type_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_defect_type_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_defect_type_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_DEFECT_TYPE_TBL_SET_OAMP_DEFECT_TYPE_TBL_SET_TBL_DATA_DEFECT_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_DEFECT_TYPE_TBL_SET_OAMP_DEFECT_TYPE_TBL_SET_TBL_DATA_DEFECT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.defect_type = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = oam_defect_type_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_defect_type_tbl_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_defect_type_tbl_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_defect_type_tbl_get (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_defect_type_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_DEFECT_TYPE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_defect_type_tbl_get";

  unit = oam_get_default_unit();
  OAM_DEFECT_TYPE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_DEFECT_TYPE_TBL_GET_OAMP_DEFECT_TYPE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_DEFECT_TYPE_TBL_GET_OAMP_DEFECT_TYPE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_defect_type_tbl_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = oam_defect_type_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_defect_type_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_defect_type_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  OAM_DEFECT_TYPE_TBL_DATA_print(&prm_tbl_data);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_mep_db_tbl_set (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_mep_db_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_MEP_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_mep_db_tbl_set";

  unit = oam_get_default_unit();
  OAM_MEP_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_mep_db_tbl_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = oam_mep_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mep_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mep_db_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_UP1DOWN0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_UP1DOWN0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.up1down0 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.port = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_DM_DB_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_DM_DB_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.dm_db_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_DM_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_DM_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.dm_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_CCM_TX_START_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_CCM_TX_START_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ccm_tx_start = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_MAID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_MAID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.maid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_CCM_INTERVAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_CCM_INTERVAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ccm_interval = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = oam_mep_db_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mep_db_tbl_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_mep_db_tbl_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_mep_db_tbl_get (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_mep_db_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_MEP_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_mep_db_tbl_get";

  unit = oam_get_default_unit();
  OAM_MEP_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MEP_DB_TBL_GET_OAMP_MEP_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MEP_DB_TBL_GET_OAMP_MEP_DB_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_mep_db_tbl_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = oam_mep_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mep_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mep_db_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  OAM_MEP_DB_TBL_DATA_print(&prm_tbl_data);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_mep_index_db_tbl_set (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_mep_index_db_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_MEP_INDEX_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_mep_index_db_tbl_set";

  unit = oam_get_default_unit();
  OAM_MEP_INDEX_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MEP_INDEX_DB_TBL_SET_OAMP_MEP_INDEX_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MEP_INDEX_DB_TBL_SET_OAMP_MEP_INDEX_DB_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_mep_index_db_tbl_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = oam_mep_index_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mep_index_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mep_index_db_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MEP_INDEX_DB_TBL_SET_OAMP_MEP_INDEX_DB_TBL_SET_TBL_DATA_ACCELERATED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MEP_INDEX_DB_TBL_SET_OAMP_MEP_INDEX_DB_TBL_SET_TBL_DATA_ACCELERATED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.accelerated = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MEP_INDEX_DB_TBL_SET_OAMP_MEP_INDEX_DB_TBL_SET_TBL_DATA_MEP_DB_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MEP_INDEX_DB_TBL_SET_OAMP_MEP_INDEX_DB_TBL_SET_TBL_DATA_MEP_DB_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mep_db_ptr = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = oam_mep_index_db_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mep_index_db_tbl_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_mep_index_db_tbl_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_mep_index_db_tbl_get (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_mep_index_db_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_MEP_INDEX_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_mep_index_db_tbl_get";

  unit = oam_get_default_unit();
  OAM_MEP_INDEX_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MEP_INDEX_DB_TBL_GET_OAMP_MEP_INDEX_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MEP_INDEX_DB_TBL_GET_OAMP_MEP_INDEX_DB_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_mep_index_db_tbl_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = oam_mep_index_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mep_index_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mep_index_db_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  OAM_MEP_INDEX_DB_TBL_DATA_print(&prm_tbl_data);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_rmep_db_tbl_set (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_rmep_db_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_RMEP_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_rmep_db_tbl_set";

  unit = oam_get_default_unit();
  OAM_RMEP_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_rmep_db_tbl_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = oam_rmep_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_rmep_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_rmep_db_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_RCVD_RDI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_RCVD_RDI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.rcvd_rdi = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_CCMDEFECT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_CCMDEFECT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ccmdefect = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_CCMINTERVAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_CCMINTERVAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ccminterval = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_LAST_CCMTIME_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_LAST_CCMTIME_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.last_ccmtime_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_LAST_CCMTIME_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_LAST_CCMTIME_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.last_ccmtime = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = oam_rmep_db_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_rmep_db_tbl_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_rmep_db_tbl_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_rmep_db_tbl_get (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_rmep_db_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_RMEP_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_rmep_db_tbl_get";

  unit = oam_get_default_unit();
  OAM_RMEP_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_DB_TBL_GET_OAMP_RMEP_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_RMEP_DB_TBL_GET_OAMP_RMEP_DB_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_rmep_db_tbl_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = oam_rmep_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_rmep_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_rmep_db_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  OAM_RMEP_DB_TBL_DATA_print(&prm_tbl_data);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_rmep_hash_0_db_tbl_set (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_rmep_hash_0_db_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_RMEP_HASH_0_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_rmep_hash_0_db_tbl_set";

  unit = oam_get_default_unit();
  OAM_RMEP_HASH_0_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_rmep_hash_0_db_tbl_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = oam_rmep_hash_0_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_rmep_hash_0_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_rmep_hash_0_db_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_TBL_DATA_VALID_HASH0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_TBL_DATA_VALID_HASH0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.valid_hash0 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_TBL_DATA_VERIFIER0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_TBL_DATA_VERIFIER0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.verifier0 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_TBL_DATA_RMEP_DB_PTR0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_TBL_DATA_RMEP_DB_PTR0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.rmep_db_ptr0 = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = oam_rmep_hash_0_db_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_rmep_hash_0_db_tbl_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_rmep_hash_0_db_tbl_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_rmep_hash_0_db_tbl_get (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_rmep_hash_0_db_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_RMEP_HASH_0_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_rmep_hash_0_db_tbl_get";

  unit = oam_get_default_unit();
  OAM_RMEP_HASH_0_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_HASH_0_DB_TBL_GET_OAMP_RMEP_HASH_0_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_RMEP_HASH_0_DB_TBL_GET_OAMP_RMEP_HASH_0_DB_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_rmep_hash_0_db_tbl_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = oam_rmep_hash_0_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_rmep_hash_0_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_rmep_hash_0_db_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  OAM_RMEP_HASH_0_DB_TBL_DATA_print(&prm_tbl_data);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_rmep_hash_1_db_tbl_set (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_rmep_hash_1_db_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_RMEP_HASH_1_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_rmep_hash_1_db_tbl_set";

  unit = oam_get_default_unit();
  OAM_RMEP_HASH_1_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_rmep_hash_1_db_tbl_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = oam_rmep_hash_1_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_rmep_hash_1_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_rmep_hash_1_db_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_TBL_DATA_VALID_HASH1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_TBL_DATA_VALID_HASH1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.valid_hash1 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_TBL_DATA_VERIFIER1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_TBL_DATA_VERIFIER1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.verifier1 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_TBL_DATA_RMEP_DB_PTR1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_TBL_DATA_RMEP_DB_PTR1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.rmep_db_ptr1 = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = oam_rmep_hash_1_db_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_rmep_hash_1_db_tbl_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_rmep_hash_1_db_tbl_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_rmep_hash_1_db_tbl_get (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_rmep_hash_1_db_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_RMEP_HASH_1_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_rmep_hash_1_db_tbl_get";

  unit = oam_get_default_unit();
  OAM_RMEP_HASH_1_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_HASH_1_DB_TBL_GET_OAMP_RMEP_HASH_1_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_RMEP_HASH_1_DB_TBL_GET_OAMP_RMEP_HASH_1_DB_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_rmep_hash_1_db_tbl_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = oam_rmep_hash_1_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_rmep_hash_1_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_rmep_hash_1_db_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  OAM_RMEP_HASH_1_DB_TBL_DATA_print(&prm_tbl_data);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_lmdb_tbl_set (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_lmdb_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_LMDB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_lmdb_tbl_set";

  unit = oam_get_default_unit();
  OAM_LMDB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_lmdb_tbl_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = oam_lmdb_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_lmdb_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_lmdb_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_TX_FCF_PEER_P_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_TX_FCF_PEER_P_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tx_fcf_peer_p = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_TX_FCF_PEER_C_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_TX_FCF_PEER_C_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tx_fcf_peer_c = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_RX_FCB_P_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_RX_FCB_P_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.rx_fcb_p = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_RX_FCB_C_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_RX_FCB_C_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.rx_fcb_c = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_RX_FCB_PEER_P_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_RX_FCB_PEER_P_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.rx_fcb_peer_p = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_RX_FCB_PEER_C_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_RX_FCB_PEER_C_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.rx_fcb_peer_c = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_TX_FCF_P_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_TX_FCF_P_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tx_fcf_p = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_TX_FCF_C_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_TX_FCF_C_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tx_fcf_c = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = oam_lmdb_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_lmdb_tbl_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_lmdb_tbl_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_lmdb_tbl_get (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_lmdb_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_LMDB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_lmdb_tbl_get";

  unit = oam_get_default_unit();
  OAM_LMDB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LMDB_TBL_GET_OAMP_LMDB_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_LMDB_TBL_GET_OAMP_LMDB_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_lmdb_tbl_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = oam_lmdb_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_lmdb_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_lmdb_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  OAM_LMDB_TBL_DATA_print(&prm_tbl_data);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_mot_db_tbl_set (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_mot_db_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_MOT_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_mot_db_tbl_set";

  unit = oam_get_default_unit();
  OAM_MOT_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_mot_db_tbl_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = oam_mot_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mot_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mot_db_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_DEFECT_LOCATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_DEFECT_LOCATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.defect_location = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_DEFECT_TYPE_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_DEFECT_TYPE_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.defect_type_ptr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_TTL_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_TTL_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ttl_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_LSP_ID_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_LSP_ID_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.lsp_id_lsb = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_LSR_ID_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_LSR_ID_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.lsr_id_ptr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_EEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_EEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.eep = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_CO_S_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_CO_S_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.cos = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_SYSPORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_SYSPORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.sysport = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = oam_mot_db_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mot_db_tbl_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_mot_db_tbl_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_mot_db_tbl_get (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_mot_db_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_MOT_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_mot_db_tbl_get";

  unit = oam_get_default_unit();
  OAM_MOT_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOT_DB_TBL_GET_OAMP_MOT_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MOT_DB_TBL_GET_OAMP_MOT_DB_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_mot_db_tbl_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = oam_mot_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mot_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mot_db_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  OAM_MOT_DB_TBL_DATA_print(&prm_tbl_data);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_ext_mep_db_tbl_set (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_ext_mep_db_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_EXT_MEP_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_ext_mep_db_tbl_set";

  unit = oam_get_default_unit();
  OAM_EXT_MEP_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_ext_mep_db_tbl_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = oam_ext_mep_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_ext_mep_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_ext_mep_db_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_LMENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_LMENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.lmenable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_PRIORITY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_PRIORITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.priority = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_MD_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_MD_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.md_level = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_MEPID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_MEPID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mepid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_RDI_INDICATOR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_RDI_INDICATOR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.rdi_indicator = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = oam_ext_mep_db_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_ext_mep_db_tbl_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_ext_mep_db_tbl_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: oamp_ext_mep_db_tbl_get (section oam_acc_oam)
 */
int
  ui_oam_acc_oam_ext_mep_db_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  OAM_EXT_MEP_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");
  soc_sand_proc_name = "oam_ext_mep_db_tbl_get";

  unit = oam_get_default_unit();
  OAM_EXT_MEP_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_EXT_MEP_DB_TBL_GET_OAMP_EXT_MEP_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_EXT_MEP_DB_TBL_GET_OAMP_EXT_MEP_DB_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after oamp_ext_mep_db_tbl_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = oam_ext_mep_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_ext_mep_db_tbl_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_ext_mep_db_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  OAM_EXT_MEP_DB_TBL_DATA_print(&prm_tbl_data);

  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_OAM_REG_ACCESS

STATIC uint32
  oam_ui_read_fld(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  OAM_REG_FIELD*  field,
    SOC_SAND_OUT uint32*         val
  )
{
  uint32
    res = SOC_SAND_OK,
    offset = 0,
    reg_val = 0,
    fld_val = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(OAM_READ_FLD_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(val);

  offset = 0x200 * sizeof(uint32) + field->base;

  res = soc_sand_mem_read_unsafe(
          unit,
          &(reg_val),
          offset,
          sizeof(uint32),
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = oam_field_from_reg_get(
          &(reg_val),
          field,
          &(fld_val)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *val = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in oam_ui_read_fld",0,0);
}

STATIC uint32
  oam_ui_write_fld(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  OAM_REG_FIELD*  field,
    SOC_SAND_IN  uint32          val
  )
{
  uint32
    max_fld_val,
    res = SOC_SAND_OK;
  uint32
    offset = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(OAM_WRITE_FLD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(field);

  max_fld_val = OAM_FLD_MAX(*field);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    val, max_fld_val,
    OAM_VAL_IS_OUT_OF_RANGE_ERR, 10, exit
 );

  offset = 0x200 * sizeof(uint32) + field->base;

  res = soc_sand_read_modify_write(
          soc_sand_get_chip_descriptor_base_addr(unit),
          offset,
          field->lsb,
          SOC_SAND_BITS_MASK(field->msb, field->lsb),
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in oam_ui_write_fld",offset,val);
}

/********************************************************************
 *  Function handler: read_fld (section reg_access)
 */
int
  ui_oam_acc_reg_access_read_fld(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  OAM_REG_FIELD
    prm_field;
  uint32
    prm_val = 0;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_reg_access");
  soc_sand_proc_name = "oam_read_fld";

  unit = oam_get_default_unit();
  prm_field.step = 0;
  prm_field.lsb = 0;
  prm_field.msb = 31;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_READ_FLD_READ_FLD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_READ_FLD_READ_FLD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.base = (uint32)param_val->value.ulong_value;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_READ_FLD_READ_FLD_FIELD_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_READ_FLD_READ_FLD_FIELD_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.lsb = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_READ_FLD_READ_FLD_FIELD_MSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_READ_FLD_READ_FLD_FIELD_MSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.msb = (uint8)param_val->value.ulong_value;
  }

  prm_field.base *= 4;

  /* Call function */
  ret = oam_ui_read_fld(
          unit,
          &prm_field,
          &prm_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    oam_disp_result(ret, "oam_read_fld");
    goto exit;
  }

  soc_sand_os_printf( "val: 0x%.8lX\n\r",prm_val);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: write_fld (section reg_access)
 */
int
  ui_oam_acc_reg_access_write_fld(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  OAM_REG_FIELD
    prm_field;
  uint32
    prm_val = 0;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_reg_access");
  soc_sand_proc_name = "oam_write_fld";

  unit = oam_get_default_unit();
  prm_field.step = 0;
  prm_field.lsb = 0;
  prm_field.msb = 31;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_WRITE_FLD_WRITE_FLD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_WRITE_FLD_WRITE_FLD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.base = (uint32)param_val->value.ulong_value;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_WRITE_FLD_WRITE_FLD_FIELD_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_WRITE_FLD_WRITE_FLD_FIELD_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.lsb = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_WRITE_FLD_WRITE_FLD_FIELD_MSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_WRITE_FLD_WRITE_FLD_FIELD_MSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.msb = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_WRITE_FLD_WRITE_FLD_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_WRITE_FLD_WRITE_FLD_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_val = (uint32)param_val->value.ulong_value;
  }

  prm_field.base *= 4;

  /* Call function */
  ret = oam_ui_write_fld(
          unit,
          &prm_field,
          prm_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    oam_disp_result(ret, "oam_write_fld");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: read_reg (section reg_access)
 */
int
  ui_oam_acc_reg_access_read_reg(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  OAM_REG_FIELD
    prm_field;
  uint32
    prm_val,
    do_ndx,
    nof_occurs = 1,
    wait_time=1,
    sec[3],
    nano[3];

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_reg_access");
  soc_sand_proc_name = "oam_read_reg";

  unit = oam_get_default_unit();
  prm_field.step = 0;
  prm_field.lsb = 0;
  prm_field.msb = 31;
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_READ_REG_READ_REG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_READ_REG_READ_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.base = (uint32)param_val->value.ulong_value;
  }

  /* Get parameters */
  prm_field.base *= 4;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_DO_TWICE_ID, 1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_DO_TWICE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_occurs = 2;
    wait_time = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  for(do_ndx=0;do_ndx<nof_occurs;do_ndx++)
  {
    ret = oam_ui_read_fld(
            unit,
            &prm_field,
            &prm_val
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      oam_disp_result(ret, "oam_read_reg");
      goto exit;
    }

    if(nof_occurs==2)
    {
      soc_sand_os_get_time(&sec[do_ndx],&nano[do_ndx]);
      if(do_ndx == 0)
      {
        sal_msleep(wait_time);
      }
      else
      {
        soc_sand_os_get_time_diff(sec[0],nano[0],sec[1],nano[1],&sec[2],&nano[2]);
        soc_sand_os_printf( " Waited %d seconds and %d nanoseconds\n\r", sec[2],nano[2]);
      }
    }
    soc_sand_os_printf( "val: 0x%.8lX\n\r",prm_val);
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: write_reg (section reg_access)
 */
int
  ui_oam_acc_reg_access_write_reg(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  OAM_REG_FIELD
    prm_field;
  uint32
    prm_val = 0;

  UI_MACROS_INIT_FUNCTION("ui_oam_acc_reg_access");
  soc_sand_proc_name = "oam_write_reg";

  unit = oam_get_default_unit();
  prm_field.step = 0;
  prm_field.lsb = 0;
  prm_field.msb = 31;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_WRITE_REG_WRITE_REG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_WRITE_REG_WRITE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.base = (uint32)param_val->value.ulong_value;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_WRITE_REG_WRITE_REG_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_WRITE_REG_WRITE_REG_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_val = (uint32)param_val->value.ulong_value;
  }

  prm_field.base *= 4;

  /* Call function */
  ret = oam_ui_write_fld(
          unit,
          &prm_field,
          prm_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_write_reg - FAIL", TRUE);
    oam_disp_result(ret, "oam_write_reg");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

int
  ui_oam_iread(
    CURRENT_LINE *current_line
  )
{
  uint32
    addr=0,
    data[3],
    ret=0,
    size=0,
    do_ndx,
    nof_occurs = 1,
    wait_time=0,
    sec[3],
    nano[3],
    module_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_oam_iread");
  soc_sand_proc_name = "ui_oam_iread";

  unit = oam_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_IRW_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_IRW_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    addr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    size = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_DO_TWICE_ID, 1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_DO_TWICE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_occurs = 2;
    wait_time = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_IACC_MODULE_ID_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_OAM_IACC_MODULE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    module_id = param_val->numeric_equivalent;
  }

  for(do_ndx=0;do_ndx<nof_occurs;do_ndx++)
  {
    ret = soc_sand_tbl_read(
      unit,
      data,
      addr,
      size * sizeof(uint32),
      module_id,
      size * sizeof(uint32)
      );
    if(nof_occurs==2)
    {
      soc_sand_os_get_time(&sec[do_ndx],&nano[do_ndx]);
      if(do_ndx == 0)
      {
        sal_msleep(wait_time);
      }
      else
      {
        soc_sand_os_get_time_diff(sec[0],nano[0],sec[1],nano[1],&sec[2],&nano[2]);
        soc_sand_os_printf( " Waited %d: %d \n\r", sec[2],nano[2]);
      }
    }
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_sand_mem_iread - FAIL", TRUE);
      oam_disp_result(ret, "soc_sand_mem_iread");
      goto exit;
    }
    soc_sand_os_printf( "addr 0x%x data 0x%x 0x%x 0x%x", addr, data[2], data[1], data[0]);
  }

  goto exit;
exit:
  return ui_ret;
}

int
  ui_oam_iwrite(
    CURRENT_LINE *current_line
  )
{
  uint32
    addr=0,
    param_i,
    data[3],
    ret,
    vals[3],
    size=0,
    do_ndx,
    nof_occurs = 1,
    wait_time=0,
    sec[3],
    nano[3],
    module_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_oam_iwrite");
  soc_sand_proc_name = "ui_oam_iwrite";

  unit = oam_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_IRW_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_IRW_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    addr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    size = (uint32)param_val->value.ulong_value;
  }

  for (param_i=1; param_i<=3; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_OAM_ACC_IWRITE_VAL_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_OAM_ACC_IWRITE_VAL_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    vals[param_i-1] = param_val->value.ulong_value;
  }

  if(size == 1)
  {
    data[0] = vals[0];
  }
  else if(size == 2)
  {
    data[1] = vals[0];
    data[0] = vals[1];
  }
 else if(size == 3)
  {
    data[2] = vals[0];
    data[1] = vals[1];
    data[0] = vals[2];
  }
  else
  {
    soc_sand_os_printf( "size %d is invalid\n\r", size);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_DO_TWICE_ID, 1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_DO_TWICE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_occurs = 2;
    wait_time = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_IACC_MODULE_ID_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_OAM_IACC_MODULE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    module_id = param_val->numeric_equivalent;
  }

  for(do_ndx=0;do_ndx<nof_occurs;do_ndx++)
  {
    ret = soc_sand_tbl_write(
      unit,
      data,
      addr,
      size * sizeof(uint32),
      module_id,
      size * sizeof(uint32)
      );
    if(nof_occurs==2)
    {
      soc_sand_os_get_time(&sec[do_ndx],&nano[do_ndx]);
      if(do_ndx == 0)
      {
        sal_msleep(wait_time);
      }
      else
      {
        soc_sand_os_get_time_diff(sec[0],nano[0],sec[1],nano[1],&sec[2],&nano[2]);
        soc_sand_os_printf( " Waited %d: %d \n\r", sec[2],nano[2]);
      }
    }
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_sand_mem_iwrite - FAIL", TRUE);
      oam_disp_result(ret, "soc_sand_mem_iwrite");
      goto exit;
    }
  }

  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_OAM_ACC_OAM/* { oam_acc_oam*/
/********************************************************************
 *  Section handler: oam_acc_oam
 */
int
  ui_oam_acc_oam(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_oam_acc_oam");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_mos_db_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOS_DB_TBL_GET_OAMP_MOS_DB_TBL_GET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_mos_db_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_CPU_TBL_SET_OAMP_MOR_DB_CPU_TBL_SET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_mor_db_cpu_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_CPU_TBL_GET_OAMP_MOR_DB_CPU_TBL_GET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_mor_db_cpu_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_mor_db_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOR_DB_TBL_GET_OAMP_MOR_DB_TBL_GET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_mor_db_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_SYS_PORT_TBL_SET_OAMP_SYS_PORT_TBL_SET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_sys_port_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_SYS_PORT_TBL_GET_OAMP_SYS_PORT_TBL_GET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_sys_port_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LSR_ID_TBL_SET_OAMP_LSR_ID_TBL_SET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_lsr_id_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LSR_ID_TBL_GET_OAMP_LSR_ID_TBL_GET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_lsr_id_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_DEFECT_TYPE_TBL_SET_OAMP_DEFECT_TYPE_TBL_SET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_defect_type_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_DEFECT_TYPE_TBL_GET_OAMP_DEFECT_TYPE_TBL_GET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_defect_type_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_mep_db_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MEP_DB_TBL_GET_OAMP_MEP_DB_TBL_GET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_mep_db_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MEP_INDEX_DB_TBL_SET_OAMP_MEP_INDEX_DB_TBL_SET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_mep_index_db_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MEP_INDEX_DB_TBL_GET_OAMP_MEP_INDEX_DB_TBL_GET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_mep_index_db_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_rmep_db_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_DB_TBL_GET_OAMP_RMEP_DB_TBL_GET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_rmep_db_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_rmep_hash_0_db_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_HASH_0_DB_TBL_GET_OAMP_RMEP_HASH_0_DB_TBL_GET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_rmep_hash_0_db_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_rmep_hash_1_db_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_RMEP_HASH_1_DB_TBL_GET_OAMP_RMEP_HASH_1_DB_TBL_GET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_rmep_hash_1_db_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_lmdb_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_LMDB_TBL_GET_OAMP_LMDB_TBL_GET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_lmdb_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_mot_db_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MOT_DB_TBL_GET_OAMP_MOT_DB_TBL_GET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_mot_db_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_ext_mep_db_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_EXT_MEP_DB_TBL_GET_OAMP_EXT_MEP_DB_TBL_GET_ID,1))
  {
    ui_ret = ui_oam_acc_oam_ext_mep_db_tbl_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after oam_acc_oam***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */oam_acc_oam

#ifdef UI_OAM_REG_ACCESS/* { reg_access*/
/********************************************************************
 *  Section handler: reg_access
 */
int
  ui_oam_acc_reg_access(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_oam_acc_reg_access");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_READ_FLD_READ_FLD_ID,1))
  {
    ui_ret = ui_oam_acc_reg_access_read_fld(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_WRITE_FLD_WRITE_FLD_ID,1))
  {
    ui_ret = ui_oam_acc_reg_access_write_fld(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_READ_REG_READ_REG_ID,1))
  {
    ui_ret = ui_oam_acc_reg_access_read_reg(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_WRITE_REG_WRITE_REG_ID,1))
  {
    ui_ret = ui_oam_acc_reg_access_write_reg(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_IREAD_ID,1))
  {
    ui_ret = ui_oam_iread(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_IWRITE_ID,1))
  {
    ui_ret = ui_oam_iwrite(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after reg_access***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif

/*****************************************************
*NAME
*  subject_oam_acc
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'subject_oam_acc' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_oam_acc(current_line,current_line_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to process.
*    CURRENT_LINE **current_line_ptr -
*      Pointer to prompt line to be displayed after
*      this procedure finishes execution. Caller
*      points this variable to the pointer to
*      the next line to display. If called function wishes
*      to set the next line to display, it replaces
*      the pointer to the next line to display.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Processing results. See 'current_line_ptr'.
*REMARKS:
*  This procedure should be carried out under 'task_safe'
*  state (i.e., task can not be deleted while this
*  procedure is being carried out).
*SEE ALSO:
 */
int
  subject_oam_acc(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  unsigned int
    match_index;
  int
    ui_ret;
  unsigned int
    ;
  char
    *proc_name ;

  proc_name = "subject_oam_acc" ;
  ui_ret = FALSE ;
  unit = 0;

  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\r\n",FALSE) ;
    send_string_to_screen("'subject_oam_acc()' function was called with no parameters.\r\n",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_oam_acc').
   */

  send_array_to_screen("\r\n",2) ;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ACC_OAM_ID,1))
  {
    ui_ret = ui_oam_acc_oam(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ACC_REG_ACCESS_ID,1))
  {
    ui_ret = ui_oam_acc_reg_access(current_line);
  }
  else
  {
    /*
     * Enter if an unknown request.
     */
    send_string_to_screen(
      "\r\n"
      "*** oam_acc command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return (ui_ret);
}

