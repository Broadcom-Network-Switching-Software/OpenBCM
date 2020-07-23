/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPC

#include <shared/bsl.h>




#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/PPC/ppc_api_fp.h>

#include <soc/dpp/ARAD/arad_pmf_prog_select.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_fem_tag.h>
#include <soc/dpp/ARAD/arad_parser.h>

#include <bcm_int/dpp/field_int.h>




















void
  SOC_PPC_PMF_PFG_INFO_clear(
    SOC_SAND_OUT SOC_PPC_PMF_PFG_INFO *info
  )
{
    uint32
      ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_PMF_PFG_INFO));
  info->hdr_format_bmp = 0;
  info->vlan_tag_structure_bmp = 0xffffffff;
  info->is_array_qualifier = 0;
  info->stage = SOC_PPC_NOF_FP_DATABASE_STAGES;

  for (ind = 0; ind < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX; ++ind)
  {
    SOC_PPC_FP_QUAL_VAL_clear(&(info->qual_vals[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_clear(
    SOC_SAND_OUT SOC_PPC_FP_PACKET *info
  )
{
  uint32
    i;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->local_port_src = 0;
  info->size_bytes = 0;
   
  for(i = 0; i < SOC_PPC_FP_IRE_TRAFFIC_BUFFER_SIZE; ++i) {
    info->buffer[i] = 0;
  }
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_QUAL_VAL_clear(
    SOC_SAND_OUT SOC_PPC_FP_QUAL_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_QUAL_VAL));
  info->type = BCM_FIELD_ENTRY_INVALID;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_ETH_TAG_FORMAT_clear(
    SOC_SAND_OUT SOC_PPC_FP_ETH_TAG_FORMAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_ETH_TAG_FORMAT));
  info->tag_inner = SOC_SAND_PP_VLAN_TAG_TYPE_NONE;
  info->tag_outer = SOC_SAND_PP_VLAN_TAG_TYPE_NONE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_DATABASE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FP_DATABASE_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_DATABASE_INFO));
  info->db_type = SOC_PPC_NOF_FP_DATABASE_TYPES;
  info->supported_pfgs = 0;
  for (ind = 0; ind < SOC_PPC_FP_NOF_PFGS_IN_LONGS_ARAD; ++ind)
  {
      info->supported_pfgs_arad[ind] = 0;
  }
  for (ind = 0; ind < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++ind)
  {
    info->qual_types[ind] = BCM_FIELD_ENTRY_INVALID;
  }
  for (ind = 0; ind < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++ind)
  {
    info->action_types[ind] = SOC_PPC_FP_ACTION_TYPE_INVALID;
  }
  info->strength = 0;
  info->cascaded_coupled_db_id = SOC_PPC_FP_NOF_DBS;
  info->flags = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PPC_FP_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_ACTION_VAL));
  info->type = SOC_PPC_FP_ACTION_TYPE_INVALID;
  info->val = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FP_ENTRY_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_ENTRY_INFO));
  for (ind = 0; ind < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++ind)
  {
    SOC_PPC_FP_QUAL_VAL_clear(&(info->qual_vals[ind]));
  }
  for (ind = 0; ind < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++ind)
  {
    SOC_PPC_FP_ACTION_VAL_clear(&(info->actions[ind]));
  }
  info->priority = 0;
  info->is_for_update = 0;
  info->is_invalid = 0;
  info->is_inserted_top = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_DIR_EXTR_ACTION_LOC_clear(
    SOC_SAND_OUT SOC_PPC_FP_DIR_EXTR_ACTION_LOC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_DIR_EXTR_ACTION_LOC));
  info->type = BCM_FIELD_ENTRY_INVALID;
  info->fld_lsb = 0;
  info->cst_val = 0;
  info->nof_bits = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_DIR_EXTR_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PPC_FP_DIR_EXTR_ACTION_VAL *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_DIR_EXTR_ACTION_VAL));
  info->type = SOC_PPC_FP_ACTION_TYPE_INVALID;
  for (ind = 0; ind < SOC_PPC_FP_DIR_EXTR_MAX_NOF_FIELDS; ++ind)
  {
    SOC_PPC_FP_DIR_EXTR_ACTION_LOC_clear(&(info->fld_ext[ind]));
  }
  info->nof_fields = 0;
  info->base_val = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FP_DIR_EXTR_ENTRY_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_DIR_EXTR_ENTRY_INFO));
  for (ind = 0; ind < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++ind)
  {
    SOC_PPC_FP_QUAL_VAL_clear(&(info->qual_vals[ind]));
  }
  for (ind = 0; ind < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++ind)
  {
    SOC_PPC_FP_DIR_EXTR_ACTION_VAL_clear(&(info->actions[ind]));
  }
  info->priority = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_CONTROL_INDEX_clear(
    SOC_SAND_OUT SOC_PPC_FP_CONTROL_INDEX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_CONTROL_INDEX));
  info->db_id = 0;
  info->type = SOC_PPC_NOF_FP_CONTROL_TYPES;
  info->val_ndx = 0;
  info->clear_val = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_CONTROL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FP_CONTROL_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_CONTROL_INFO));
  for (ind = 0; ind < SOC_PPC_FP_NOF_CONTROL_VALS; ++ind)
  {
    info->val[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_DIAG_PP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FP_DIAG_PP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_DIAG_PP_PORT_INFO));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_PARSER_clear(
    SOC_SAND_OUT SOC_PPC_FP_PACKET_DIAG_PARSER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_PACKET_DIAG_PARSER));
  info->tm_port = 0;
  info->pp_port = 0;
  info->header_type = SOC_TMC_PORT_HEADER_TYPE_NONE;
  info->pfc_hw = -1;
  info->vlan_tag_structure = SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_PGM_clear(
    SOC_SAND_OUT SOC_PPC_FP_PACKET_DIAG_PGM *info
  )
{
    SOC_PPC_FP_DATABASE_STAGE 
        stage;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_PACKET_DIAG_PGM));
  SOC_PPC_FP_DIAG_PP_PORT_INFO_clear(&(info->pp_port_info));
  for (stage = 0; stage < SOC_PPC_NOF_FP_DATABASE_STAGES; stage ++) {
      info->pfg_id[stage] = 0;
      info->pgm_id[stage] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_QUAL_clear(
    SOC_SAND_OUT SOC_PPC_FP_PACKET_DIAG_QUAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_PACKET_DIAG_QUAL));
  info->type = BCM_FIELD_ENTRY_INVALID;
  info->val[0] = 0;
  info->val[1] = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_DB_QUAL_clear(
    SOC_SAND_OUT SOC_PPC_FP_PACKET_DIAG_DB_QUAL *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_PACKET_DIAG_DB_QUAL));
  info->db_id = 0;
  info->stage = SOC_PPC_NOF_FP_DATABASE_STAGES;
  for (ind = 0; ind < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++ind)
  {
    SOC_PPC_FP_PACKET_DIAG_QUAL_clear(&(info->qual[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FP_PACKET_DIAG_KEY *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_PACKET_DIAG_KEY));
  for (ind = 0; ind < SOC_PPC_FP_NOF_DBS; ++ind)
  {
    SOC_PPC_FP_PACKET_DIAG_DB_QUAL_clear(&(info->db_id_quals[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_TCAM_DT_clear(
    SOC_SAND_OUT SOC_PPC_FP_PACKET_DIAG_TCAM_DT *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_PACKET_DIAG_TCAM_DT));
  info->is_match = 0;
  info->db_id = 0;
  for (ind = 0; ind < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++ind)
  {
    SOC_PPC_FP_ACTION_VAL_clear(&(info->actions[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_MACRO_clear(
    SOC_SAND_OUT SOC_PPC_FP_PACKET_DIAG_MACRO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_PACKET_DIAG_MACRO));
  info->db_id = 0;
  info->entry_id = 0;
  for (ind = 0; ind < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++ind)
  {
    SOC_PPC_FP_ACTION_VAL_clear(&(info->input_actions[ind]));
  }
  SOC_PPC_FP_ACTION_VAL_clear(&(info->action));
  SOC_PPC_FP_PACKET_DIAG_QUAL_clear(&(info->qual_mask));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_MACRO_SIMPLE_clear(
    SOC_SAND_OUT SOC_PPC_FP_PACKET_DIAG_MACRO_SIMPLE *info
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(SOC_PPC_FP_PACKET_DIAG_MACRO_SIMPLE));
    info->db_id = 0;
    SOC_PPC_FP_ACTION_VAL_clear(&(info->action));
    SOC_SAND_MAGIC_NUM_SET;
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_ACTION_ELK_clear(
    SOC_SAND_OUT SOC_PPC_FP_PACKET_DIAG_ACTION_ELK *info
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(SOC_PPC_FP_PACKET_DIAG_ACTION_ELK));
    info->hit = 0;
    SOC_PPC_FP_ACTION_VAL_clear(&(info->action));
    SOC_SAND_MAGIC_NUM_SET;
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_FP_PACKET_DIAG_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FP_PACKET_DIAG_INFO *info
  )
{
  uint32
    ind2,
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_PACKET_DIAG_INFO));
  SOC_PPC_FP_PACKET_DIAG_PARSER_clear(&(info->parser));
  SOC_PPC_FP_PACKET_DIAG_PGM_clear(&(info->pgm));
  SOC_PPC_FP_PACKET_DIAG_KEY_clear(&(info->key));
  for (ind = 0; ind < SOC_PPC_FP_NOF_CYCLES; ++ind)
  {
    for (ind2 = 0; ind2 < SOC_PPC_FP_TCAM_NOF_BANKS; ++ind2)
    {
     SOC_PPC_FP_PACKET_DIAG_TCAM_DT_clear(&(info->tcam[ind][ind2]));
    }
  }
  for (ind = 0; ind < SOC_PPC_FP_NOF_CYCLES; ++ind)
  {
    SOC_PPC_FP_PACKET_DIAG_TCAM_DT_clear(&(info->dt[ind]));
  }
  for (ind = 0; ind < SOC_PPC_FP_NOF_CYCLES; ++ind)
  {
      for (ind2 = 0; ind2 < SOC_PPC_FP_NOF_MACRO_SIMPLES; ++ind2)
      {
        SOC_PPC_FP_PACKET_DIAG_MACRO_SIMPLE_clear(&(info->macro_simple[ind][ind2]));
      }

    for (ind2 = 0; ind2 < SOC_PPC_FP_NOF_MACROS; ++ind2)
    {
      SOC_PPC_FP_PACKET_DIAG_MACRO_clear(&(info->macro[ind][ind2]));
    }
  }
  for (ind = 0; ind < SOC_DPP_DEFS_MAX(NOF_EGRESS_PMF_ACTIONS); ++ind)
  {
    SOC_PPC_FP_ACTION_VAL_clear(&(info->egress_action[ind]));
  }
  for (ind = 0; ind < SOC_PPC_FP_NOF_ELK_ACTIONS; ++ind)
  {
    SOC_PPC_FP_PACKET_DIAG_ACTION_ELK_clear(&(info->elk_action[ind]));
  }

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_KEY_LSB_MSB_clear(
	SOC_SAND_OUT SOC_PPC_FP_RESOURCE_KEY_LSB_MSB *info
	  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(SOC_PPC_FP_RESOURCE_KEY_LSB_MSB));
    info->lsb = 0;
	info->msb = 0;
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_KEY *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(SOC_PPC_FP_RESOURCE_KEY));
    info->valid = 0;
	info->qual_type = BCM_FIELD_ENTRY_INVALID;
	info->is_second_key = 0;

	SOC_PPC_FP_RESOURCE_KEY_LSB_MSB_clear(&(info->key_loc));
	SOC_PPC_FP_RESOURCE_KEY_LSB_MSB_clear(&(info->qual_loc));
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_DB_BANK_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_DB_BANK *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(SOC_PPC_FP_RESOURCE_DB_BANK));
	info->valid = 0;
	info->entries_used = 0;
	info->entries_free = 0;
	info->action_tbl_bmp = 0;
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_ACTION_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_ACTION *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(SOC_PPC_FP_RESOURCE_ACTION));
	info->valid = 0;
	info->action_type = BCM_FIELD_ENTRY_INVALID;

	SOC_PPC_FP_RESOURCE_KEY_LSB_MSB_clear(&(info->action_loc));
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PPC_FP_RESOURCE_DB_TCAM_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_DB_TCAM *info
  )
{
    uint32 
        key_idx,
        idx;

    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(SOC_PPC_FP_RESOURCE_DB_TCAM));
    info->prefix_val = 0;
    info->prefix_len = 0;
    info->nof_actions = 0;
    info->nof_keys_per_db = 0;

    for(key_idx = 0; key_idx < SOC_PPC_FP_KEY_NOF_KEYS_PER_DB_MAX; key_idx++)
    {
        info->access_profile_id[key_idx] = 0;
    }
            
    for(idx = 0; idx < SOC_PPC_FP_TCAM_NOF_BANKS; idx++)
    {
        SOC_PPC_FP_RESOURCE_DB_BANK_clear(&(info->bank[idx]));
    }

    for(idx = 0; idx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; idx++)
    {
        SOC_PPC_FP_RESOURCE_ACTION_clear(&(info->action[idx]));
    }
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_DB_DE_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_DB_DE *info
  )
{
	uint32
        pgm,
		cycle, 
		idx;

	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

	sal_memset(info, 0x0, sizeof(SOC_PPC_FP_RESOURCE_DB_DE));

    for(pgm = 0; pgm < SOC_PPC_FP_NOF_FEM_PROGS_MAX; pgm++) {
        for(cycle = 0; cycle < SOC_PPC_FP_NOF_CYCLES ; cycle++)
        {
            for(idx = 0; idx < SOC_PPC_FP_NOF_MACROS; idx++)
            {
                info->valid[pgm][cycle][idx]=0;
                info->fem_entry_id[pgm][cycle][idx] = 0;
                info->de_entry_id[pgm][cycle][idx] = 0;
                SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_clear(&(info->de_entry[pgm][cycle][idx]));
            }
        }
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_DB_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_DB *info
  )
{
	uint32
		idx;

	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

	sal_memset(info, 0x0, sizeof(SOC_PPC_FP_RESOURCE_DB));
	info->valid = 0;
	info->type = SOC_PPC_NOF_FP_DATABASE_TYPES;
	info->stage = SOC_PPC_NOF_FP_DATABASE_STAGES;
	info->key_size = 0;
	info->nof_ces = 0;
  info->db_priority = 0;

	for(idx = 0; idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; idx++)
	{
		SOC_PPC_FP_RESOURCE_KEY_clear(&(info->key_qual[idx]));
	}

	SOC_PPC_FP_RESOURCE_DB_TCAM_clear(&(info->db_tcam));
	SOC_PPC_FP_RESOURCE_DB_DE_clear(&(info->db_de));
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_BANK_DB_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_BANK_DB *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(SOC_PPC_FP_RESOURCE_BANK_DB));
	info->db_id = 0;
	info->nof_entries = 0;

	SOC_TMC_TCAM_DB_PREFIX_clear(&(info->prefix));
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_BANK_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_BANK *info
  )
{
	uint32 
		idx;

	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

	sal_memset(info, 0x0, sizeof(SOC_PPC_FP_RESOURCE_BANK));
	info->is_used = 0;
	info->entry_size = 0;
	info->entries_free = 0;
	info->nof_dbs = 0;
	info->owner = SOC_TMC_NOF_TCAM_USERS;

	for(idx = 0; idx < SOC_PPC_FP_TCAM_NOF_BANKS; idx++)
	{
		SOC_PPC_FP_RESOURCE_BANK_DB_clear(&(info->db[idx]));
	}
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_PRESEL_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_PRESEL *info
  )
{
	uint32
		idx;

	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

	sal_memset(info, 0x0, sizeof(SOC_PPC_FP_RESOURCE_PRESEL));
	info->is_valid = 0;
	info->pmf_pgm = 0;
	
	for(idx = 0; idx < SOC_PPC_FP_NOF_PS_LINES_IN_LONGS; idx++)
	{
		info->presel_bmp[idx] = 0;
	}

	for(idx = 0; idx < SOC_PPC_FP_NOF_DBS_IN_LONGS; idx++)
	{
		info->db_bmp[idx] = 0;
	}
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_DIAG_ERROR_PARAM_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_DIAG_ERROR_PARAM *info
  )
{
	uint32
		idx;

	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

	sal_memset(info, 0x0, sizeof(SOC_PPC_FP_RESOURCE_DIAG_ERROR_PARAM));
	info->is_error = FALSE;

	for(idx = 0; idx < SOC_PPC_FP_RESOURCE_DIAG_NOF_ERROR_VALUES; idx++)
	{
		info->value[idx] = 0;
	}
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_DIAG_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_DIAG *info
  )
{
	uint32
		idx;

	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

	sal_memset(info, 0x0, sizeof(SOC_PPC_FP_RESOURCE_DIAG));
	
	for(idx = 0; idx < SOC_PPC_FP_RESOURCE_DIAG_NOF_PARAMS; idx++)
	{
		SOC_PPC_FP_RESOURCE_DIAG_ERROR_PARAM_clear(&(info->params[idx]));
	}
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_PMF_CE_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_PMF_CE *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

    info->is_used = FALSE;
    info->lsb = 0;
    info->msb = 0;
    info->is_msb = FALSE;
    info->is_second_key = FALSE;
    info->qual_lsb = 0;
    info->qual_type = BCM_FIELD_ENTRY_INVALID;
    info->key_id = 0;

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_FREE_INSTRUCTIONS_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_FREE_INSTRUCTIONS *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

    info->is_used = FALSE;
    info->lsb_16b = 0;
    info->lsb_32b = 0;
    info->msb_16b = 0;
    info->msb_32b = 0;
    
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_KEY_DB_DATA_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_KEY_DB_DATA *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

    info->is_used = FALSE;
    info->is_lsb_db = FALSE;
    info->is_msb_db = FALSE;
    
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_PMF_FES_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_PMF_FES *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

    info->is_used = FALSE;
    info->action_type = BCM_FIELD_ENTRY_INVALID;
    info->is_key_src = 0;
    info->key_tcam_id = 0;
    info->key_tcam_lsb = 0;
    info->action_size = 0;
    info->valid_bits = 0;
    info->is_action_always_valid = 0;
    
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_PMF_FES_FREE_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_PMF_FES_FREE *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

    info->is_used = FALSE;
    info->fes_free = 0;
    
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_PMF_FEM_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_PMF_FEM_ENTRY *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

    info->is_used = FALSE;
    info->is_for_entry = FALSE;
    info->db_strength = 0;
    info->entry_strength = 0;
    info->entry_id = 0;
    info->action_type = BCM_FIELD_ENTRY_INVALID;
    
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_AVAILABLE_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_AVAILABLE *info
  )
{
    uint32 stage;
    uint32 pgm_idx;
    uint32 db_id;
    uint32 cycle_idx;
    uint32 ce_idx;
    uint32 key;
    uint32 pfg_idx;
    uint32 i;
    uint32 presel_line_idx;
    uint32 fem_group_idx;    
            
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

    info->is_used = FALSE;

    for(cycle_idx = 0; cycle_idx < SOC_PPC_FP_NOF_CYCLES; ++cycle_idx) {
        for(pgm_idx = 0; pgm_idx < SOC_PPC_FP_NOF_PROGS_MAX; ++pgm_idx) {
            for(db_id = 0; db_id < SOC_PPC_FP_NOF_DBS; ++db_id) {
                for(ce_idx = 0; ce_idx < SOC_PPC_FP_NOF_CES; ++ce_idx) {
                    for(stage = 0; stage < SOC_PPC_NOF_FP_DATABASE_STAGES; ++stage) {
                        SOC_PPC_FP_RESOURCE_PMF_CE_clear(&info->pgm_ce[stage][pgm_idx][db_id][cycle_idx][ce_idx]);
                        if(db_id == 0 && ce_idx == 0) {
                            SOC_PPC_FP_RESOURCE_FREE_INSTRUCTIONS_clear(&info->free_instructions[stage][pgm_idx][cycle_idx]);
                            for(key = 0; key < SOC_PPC_FP_NOF_KEYS; ++key) {
                                SOC_PPC_FP_RESOURCE_KEY_DB_DATA_clear(&info->key[stage][pgm_idx][cycle_idx][key]);
                            }
                        }
                    }
                    SOC_PPC_FP_RESOURCE_PMF_FES_clear(&info->fes[pgm_idx][cycle_idx][db_id][ce_idx]);
                }
                if(pgm_idx < SOC_PPC_FP_NOF_FEM_PROGS_MAX) {
                    for(fem_group_idx = 0; fem_group_idx < SOC_PPC_FP_NOF_MACROS; ++fem_group_idx) {
                        SOC_PPC_FP_RESOURCE_PMF_FEM_ENTRY_clear(&info->fem[pgm_idx][cycle_idx][db_id][fem_group_idx]);
                    }
                }
            }
            SOC_PPC_FP_RESOURCE_PMF_FES_FREE_clear(&info->fes_free[pgm_idx][cycle_idx]);
        }
    }

    for(stage = 0; stage < SOC_PPC_NOF_FP_DATABASE_STAGES; ++stage) {
        for(pfg_idx = 0; pfg_idx < SOC_PPC_FP_NOF_PFGS_ARAD; ++pfg_idx) {
            for(i = 0; i < SOC_PPC_BIT_TO_U32(SOC_PPC_FP_NOF_DBS); ++i) {
                info->pfgs_db_pmb[stage][pfg_idx][i] = 0;
            }
            for(i = 0; i < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX; ++i) {
                SOC_PPC_FP_QUAL_VAL_clear(&info->pfgs_qualifiers[stage][pfg_idx][i]);
            }
        }
        for(presel_line_idx = 0; presel_line_idx < SOC_PPC_FP_NOF_PS_LINES; ++presel_line_idx) {
            for(pgm_idx = 0; pgm_idx < SOC_PPC_FP_NOF_PROGS_MAX; ++pgm_idx) {
                for(i = 0; i < SOC_PPC_BIT_TO_U32(SOC_PPC_FP_NOF_PFGS_ARAD); ++i) {
                    info->pfgs[stage][presel_line_idx][pgm_idx][i] = 0;
                }
                for(i = 0; i < SOC_PPC_BIT_TO_U32(SOC_PPC_NOF_FP_QUAL_TYPES); ++i) {
                    info->quals[stage][presel_line_idx][pgm_idx][i] = 0;
                }
            }
        }
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_DIAG_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_DIAG_INFO *info
  )
{
  uint32
	  stage,
	  idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_RESOURCE_DIAG_INFO));

  for(idx = 0; idx < SOC_PPC_FP_NOF_DBS; idx++)
  {
	  SOC_PPC_FP_RESOURCE_DB_clear(&(info->db[idx]));
  }
  for(idx = 0; idx < SOC_PPC_FP_TCAM_NOF_BANKS; idx++)
  {
	  SOC_PPC_FP_RESOURCE_BANK_clear(&(info->bank[idx]));
  }
  for(stage = 0; stage < SOC_PPC_NOF_FP_DATABASE_STAGES; stage++)
  {
	  for(idx = 0; idx < SOC_PPC_FP_NOF_PS_LINES; idx++)
	  {
		  SOC_PPC_FP_RESOURCE_PRESEL_clear(&(info->presel[stage][idx]));
	  }
  }
  for(idx = 0; idx < SOC_PPC_FP_RESOURCE_DIAG_ERROR_NOF_TYPE_LAST; idx++)
  {
	  SOC_PPC_FP_RESOURCE_DIAG_clear(&(info->diag[idx]));
  }
  
  SOC_PPC_FP_RESOURCE_AVAILABLE_clear(&info->available);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_LAST_FEM_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FP_LAST_FEM_INOUT_INFO  *info)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(SOC_PPC_FP_LAST_FEM_INOUT_INFO));
    SOC_PPC_FP_ACTION_VAL_clear(&(info->action));
    SOC_SAND_MAGIC_NUM_SET;

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_LAST_FES_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FP_LAST_FES_INOUT_INFO  *info)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(SOC_PPC_FP_LAST_FES_INOUT_INFO));
    SOC_PPC_FP_ACTION_VAL_clear(&(info->action));
    SOC_SAND_MAGIC_NUM_SET;

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_CE_INFO_CLEAR(
    SOC_SAND_OUT SOC_PPC_FP_CE_INFO *info)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(SOC_PPC_FP_CE_INFO));
    info->qual_type = BCM_FIELD_ENTRY_INVALID;
    SOC_SAND_MAGIC_NUM_SET;

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1
const char*
  SOC_PPC_FP_QUAL_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE enum_val
  )
{
CONST static char*
    Soc_ppc_fp_qual_hdr_user_def[SOC_PPC_FP_QUAL_HDR_USER_DEF_LAST - SOC_PPC_FP_QUAL_HDR_USER_DEF_0 + 1]= {
        "user_def_0",
        "user_def_1",
        "user_def_2",
        "user_def_3",
        "user_def_4",
        "user_def_5",
        "user_def_6",
        "user_def_7",
        "user_def_8",
        "user_def_9",
        "user_def_10",
        "user_def_11",
        "user_def_12",
        "user_def_13",
        "user_def_14",
        "user_def_15",
        "user_def_16",
        "user_def_17",
        "user_def_18",
        "user_def_19",
        "user_def_20",
        "user_def_21",
        "user_def_22",
        "user_def_23",
        "user_def_24",
        "user_def_25",
        "user_def_26",
        "user_def_27",
        "user_def_28",
        "user_def_29",
        "user_def_30",
        "user_def_31",
        "user_def_32",
        "user_def_33",
        "user_def_34",
        "user_def_35",
        "user_def_36",
        "user_def_37",
        "user_def_38",
        "user_def_39",
        "user_def_40",
        "user_def_41",
        "user_def_42",
        "user_def_43",
        "user_def_44",
        "user_def_45",
        "user_def_46",
        "user_def_47",
        "user_def_48",
        "user_def_49",
        "user_def_50",
        "user_def_51",
        "user_def_52",
        "user_def_53",
        "user_def_54",
        "user_def_55",
        "user_def_56",
        "user_def_57",
        "user_def_58",
        "user_def_59",
        "user_def_60",
        "user_def_61",
        "user_def_62",
        "user_def_63",
        "user_def_64",
        "user_def_65",
        "user_def_66",
        "user_def_67",
        "user_def_68",
        "user_def_69",
        "user_def_70",
        "user_def_71",
        "user_def_72",
        "user_def_73",
        "user_def_74",
        "user_def_75",
        "user_def_76",
        "user_def_77",
        "user_def_78",
        "user_def_79",
        "user_def_80",
        "user_def_81",
        "user_def_82",
        "user_def_83",
        "user_def_84",
        "user_def_85",
        "user_def_86",
        "user_def_87",
        "user_def_88",
        "user_def_89",
        "user_def_90",
        "user_def_91",
        "user_def_92",
        "user_def_93",
        "user_def_94",
        "user_def_95",
        "user_def_96",
        "user_def_97",
        "user_def_98",
        "user_def_99",
        "user_def_100",
        "user_def_101",
        "user_def_102",
        "user_def_103",
        "user_def_104",
        "user_def_105",
        "user_def_106",
        "user_def_107",
        "user_def_108",
        "user_def_109",
        "user_def_110",
        "user_def_111",
        "user_def_112",
        "user_def_113",
        "user_def_114",
        "user_def_115",
        "user_def_116",
        "user_def_117",
        "user_def_118",
        "user_def_119",
        "user_def_120",
        "user_def_121",
        "user_def_122",
        "user_def_123",
        "user_def_124",
        "user_def_125",
        "user_def_126",
        "user_def_127",
        "user_def_128",
        "user_def_129",
        "user_def_130",
        "user_def_131",
        "user_def_132",
        "user_def_133",
        "user_def_134",
        "user_def_135",
        "user_def_136",
        "user_def_137",
        "user_def_138",
        "user_def_139",
        "user_def_140",
        "user_def_141",
        "user_def_142",
        "user_def_143",
        "user_def_144",
        "user_def_145",
        "user_def_146",
        "user_def_147",
        "user_def_148",
        "user_def_149",
        "user_def_150",
        "user_def_151",
        "user_def_152",
        "user_def_153",
        "user_def_154",
        "user_def_155",
        "user_def_156",
        "user_def_157",
        "user_def_158",
        "user_def_159",
        "user_def_160",
        "user_def_161",
        "user_def_162",
        "user_def_163",
        "user_def_164",
        "user_def_165",
        "user_def_166",
        "user_def_167",
        "user_def_168",
        "user_def_169",
        "user_def_170",
        "user_def_171",
        "user_def_172",
        "user_def_173",
        "user_def_174",
        "user_def_175",
        "user_def_176",
        "user_def_177",
        "user_def_178",
        "user_def_179",
        "user_def_180",
        "user_def_181",
        "user_def_182",
        "user_def_183",
        "user_def_184",
        "user_def_185",
        "user_def_186",
        "user_def_187",
        "user_def_188",
        "user_def_189",
        "user_def_190",
        "user_def_191",
        "user_def_192",
        "user_def_193",
        "user_def_194",
        "user_def_195",
        "user_def_196",
        "user_def_197",
        "user_def_198",
        "user_def_199",
        "user_def_200",
        "user_def_201",
        "user_def_202",
        "user_def_203",
        "user_def_204",
        "user_def_205",
        "user_def_206",
        "user_def_207",
        "user_def_208",
        "user_def_209",
        "user_def_210",
        "user_def_211",
        "user_def_212",
        "user_def_213",
        "user_def_214",
        "user_def_215",
        "user_def_216",
        "user_def_217",
        "user_def_218",
        "user_def_219",
        "user_def_220",
        "user_def_221",
        "user_def_222",
        "user_def_223",
        "user_def_224",
        "user_def_225",
        "user_def_226",
        "user_def_227",
        "user_def_228",
        "user_def_229",
        "user_def_230",
        "user_def_231",
        "user_def_232",
        "user_def_233",
        "user_def_234",
        "user_def_235",
        "user_def_236",
        "user_def_237",
        "user_def_238",
        "user_def_239",
        "user_def_240",
        "user_def_241",
        "user_def_242",
        "user_def_243",
        "user_def_244",
        "user_def_245",
        "user_def_246",
        "user_def_247",
        "user_def_248",
        "user_def_249",
        "user_def_250",
        "user_def_251",
        "user_def_252",
        "user_def_253",
        "user_def_254",
        "user_def_255"
    };

  const char* str = NULL;

  
  if (SOC_PPC_FP_IS_QUAL_TYPE_USER_DEFINED(enum_val)) {
      str = Soc_ppc_fp_qual_hdr_user_def[enum_val - SOC_PPC_FP_QUAL_HDR_USER_DEF_0];
     return str;
  }

  switch(enum_val)
  {
  case SOC_PPC_FP_QUAL_OUT_LIF_RANGE:
    str = "out_lif_range";
    break;
  case SOC_PPC_FP_QUAL_HDR_FWD_VLAN_TAG:
    str = "fwd_vlan_tag";
    break;
  case SOC_PPC_FP_QUAL_HDR_FWD_VLAN_TAG_ID:
    str = "fwd_vlan_tag_id";
    break;
  case SOC_PPC_FP_QUAL_HDR_FWD_VLAN_2ND_TAG_ID:
    str = "fwd_inner_vlan_tag_id";
    break;
  case SOC_PPC_FP_QUAL_HDR_FWD_SA:
    str = "fwd_sa";
  break;
  case SOC_PPC_FP_QUAL_HDR_FWD_DA:
    str = "fwd_da";
  break;
  case SOC_PPC_FP_QUAL_HDR_FWD_INNERMOST_VLAN_TAG_ID:
    str = "fwd_inner_most_tag_id";
  break;
  case SOC_PPC_FP_QUAL_HDR_FWD_ETHERTYPE:
    str = "fwd_ethertype";
  break;
  case SOC_PPC_FP_QUAL_HDR_FWD_2ND_VLAN_TAG:
    str = "fwd_2nd_vlan_tag";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_VLAN_TAG:
    str = "inner_vlan_tag";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_VLAN_TAG_ID:
    str = "inner_vlan_tag_id";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_VLAN_TAG_CFI:
    str = "inner_vlan_tag_cfi";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_VLAN_TAG_PRI:
    str = "inner_vlan_tag_pri";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_SA:
    str = "inner_sa";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_DA:
    str = "inner_da";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_ETHERTYPE:
    str = "inner_ethertype";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_2ND_VLAN_TAG:
    str = "inner_2nd_vlan_tag";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_2ND_VLAN_TAG_ID:
    str = "inner_2nd_vlan_tag_id";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_2ND_VLAN_TAG_CFI:
    str = "inner_2nd_vlan_tag_cfi";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_2ND_VLAN_TAG_PRI:
    str = "inner_2nd_vlan_tag_pri";
  break;
  case SOC_PPC_FP_QUAL_HDR_VLAN_FORMAT:
    str = "vlan_format";
  break;
  case SOC_PPC_FP_QUAL_HDR_VLAN_TAG:
    str = "vlan_tag";
  break;
  case SOC_PPC_FP_QUAL_HDR_VLAN_TAG_ID:
    str = "vlan_tag_id";
  break;
  case SOC_PPC_FP_QUAL_HDR_VLAN_TAG_PRI:
    str = "vlan_tag_pri";
  break;
  case SOC_PPC_FP_QUAL_HDR_VLAN_TAG_CFI:
    str = "vlan_tag_cfi";
  break;
  case SOC_PPC_FP_QUAL_HDR_VLAN_TAG_PRI_CFI:
    str = "vlan_tag_pri_cfi";
  break;
  case SOC_PPC_FP_QUAL_HDR_VLAN_TAG_TPID:
    str = "vlan_tag_tpid";
  break;
  case SOC_PPC_FP_QUAL_HDR_SA:
    str = "sa";
  break;
  case SOC_PPC_FP_QUAL_HDR_DA:
    str = "da";
  break;
  case SOC_PPC_FP_QUAL_HDR_ETHERTYPE:
  case SOC_PPC_FP_QUAL_UNTAG_HDR_ETHERTYPE:
  case SOC_PPC_FP_QUAL_ONE_TAG_HDR_ETHERTYPE:
  case SOC_PPC_FP_QUAL_DOUBLE_TAG_HDR_ETHERTYPE:
    str = "ethertype";
  break;
  case SOC_PPC_FP_QUAL_HDR_PPPOE_SESSION_ID:
    str = "PPPoE session ID";
  break;
  case SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG:
    str = "2nd_vlan_tag";
  break;
  case SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG_TPID:
    str = "2nd_vlan_tag_tpid";
  break;
  case SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG_ID:
    str = "2nd_vlan_tag_id";
  break;
  case SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG_PRI:
    str = "2nd_vlan_tag_pri";
  break;
  case SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG_CFI:
    str = "2nd_vlan_tag_cfi";
  break;
  case SOC_PPC_FP_QUAL_HDR_FWD_IPV4_NEXT_PRTCL:
    str = "fwd_ipv4_next_prtcl";
  break;
  case SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DF:
    str = "fwd_ipv4_df";
  break;
  case SOC_PPC_FP_QUAL_HDR_FWD_IPV4_MF:
    str = "fwd_ipv4_mf";
  break;
  case SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP:
  case SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_SIP:
    str = "fwd_ipv4_sip";
  break;
  case SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP:
  case SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_DIP:
    str = "fwd_ipv4_dip";
  break;
  case SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SRC_PORT:
    str = "fwd_ipv4_src_port";
  break;
  case SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DEST_PORT:
    str = "fwd_ipv4_dest_port";
  break;
  case SOC_PPC_FP_QUAL_HDR_FWD_IPV4_TOS:
    str = "fwd_ipv4_tos";
  break;
  case SOC_PPC_FP_QUAL_HDR_FWD_IPV4_TCP_CTL:
    str = "fwd_ipv4_tcp_ctl";
  break;
  case SOC_PPC_FP_QUAL_HDR_FWD_IPV4_IN_VID:
    str = "fwd_ipv4_in_vid";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_IPV4_NEXT_PRTCL:
    str = "inner_ipv4_next_prtcl";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_IPV4_DF:
    str = "inner_ipv4_df";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_IPV4_MF:
    str = "inner_ipv4_mf";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_IPV4_SIP:
    str = "inner_ipv4_sip";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_IPV4_DIP:
    str = "inner_ipv4_dip";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_IPV4_SRC_PORT:
    str = "inner_ipv4_src_port";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_IPV4_DEST_PORT:
    str = "inner_ipv4_dest_port";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_IPV4_TOS:
    str = "inner_ipv4_tos";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_IPV4_TTL:
    str = "inner_ipv4_ttl";
  break;
  case SOC_PPC_FP_QUAL_HDR_INNER_IPV4_TCP_CTL:
    str = "inner_ipv4_tcp_ctl";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV4_NEXT_PRTCL:
    str = "ipv4_next_prtcl";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV4_FLAGS:
    str = "ipv4_flags";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV4_DF:
    str = "ipv4_df";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV4_MF:
    str = "ipv4_mf";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV4_SIP:
    str = "ipv4_sip";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV4_DIP:
    str = "ipv4_dip";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV4_SRC_PORT:
    str = "ipv4_src_port";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV4_DEST_PORT:
    str = "ipv4_dest_port";
  break;
  case SOC_PPC_FP_QUAL_HDR_ELK_RANGE_IPV4_SRC_PORT:
    str = "range_elk_ipv4_src_port";
  break;
  case SOC_PPC_FP_QUAL_HDR_ELK_RANGE_IPV4_DEST_PORT:
    str = "range_elk_ipv4_dest_port";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV4_SRC_DEST_PORT:
      str = "ipv4_dest_src_port";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV4_TOS:
    str = "ipv4_tos";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV4_TTL:
    str = "ipv4_ttl";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV4_TCP_CTL:
    str = "ipv4_tcp_ctl";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV4_L4OPS_HI:
    str = "ipv4_l4ops_hi";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV4_L4OPS_LOW:
    str = "ipv4_l4ops_low";
    break;
  case SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH:
    str = "ipv6_sip_high";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV6_SIP_LOW:
    str = "ipv6_sip_low";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH:
    str = "ipv6_dip_high";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW:
    str = "ipv6_dip_low";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV6_NEXT_PRTCL:
    str = "ipv6_next_prtcl";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV6_TCP_CTL:
    str = "ipv6_tcp_ctl";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV6_L4OPS:
    str = "ipv6_l4ops";
  break;
  case SOC_PPC_FP_QUAL_HDR_IPV6_FLOW_LABEL:
    str = "ipv6_flow_label";
    break;
  case SOC_PPC_FP_QUAL_HDR_IPV6_HOP_LIMIT:
    str = "ipv6_hop_limit";
    break;
  case SOC_PPC_FP_QUAL_HDR_IPV6_TC:
    str = "ipv6_tc";
    break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_FWD:
    str = "mpls_label_fwd";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD:
    str = "mpls_label_id_fwd";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_EXP_FWD:
    str = "mpls_exp_fwd";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_TTL_FWD:
    str = "mpls_ttl_fwd";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_BOS_FWD:
    str = "mpls_bos_fwd";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1:
    str = "mpls_label1";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1_ID:
    str = "mpls_label1_id";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_EXP1:
    str = "mpls_exp1";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_TTL1:
    str = "mpls_ttl1";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_BOS1:
    str = "mpls_bos1";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2:
    str = "mpls_label2";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2_ID:
    str = "mpls_label2_id";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_EXP2:
    str = "mpls_exp2";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_TTL2:
    str = "mpls_ttl2";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_BOS2:
    str = "mpls_bos2";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_LABEL3:
    str = "mpls_label3";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_LABEL3_ID:
    str = "mpls_label3_id";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_EXP3:
    str = "mpls_exp3";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_TTL3:
    str = "mpls_ttl3";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_BOS3:
    str = "mpls_bos3";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_LABEL4:
    str = "mpls_label4";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_LABEL4_ID:
    str = "mpls_label4_id";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_EXP4:
    str = "mpls_exp4";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_TTL4:
    str = "mpls_ttl4";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_BOS4:
    str = "mpls_bos4";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_LABEL5:
    str = "mpls_label5";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_EXP5:
    str = "mpls_exp5";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_TTL5:
    str = "mpls_ttl5";
  break;
  case SOC_PPC_FP_QUAL_HDR_MPLS_BOS5:
    str = "mpls_bos5";
  break;
  case SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT:
    str = "src_tm_port";
  break;
  case SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT:
    str = "src_syst_port";
  break;
  case SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT:
    str = "src_pp_port";
  break;
  case SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT:
    str = "PARSER_LEAF_CONTEXT";
  break;
  case SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE:
    str = "pkt_hdr_type";
  break;
  case SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT:
    str = "eth_tag_format";
  break;
  case SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST:
    str = "fwd_dec_dest";
  break;
  case SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC:
    str = "fwd_dec_tc";
  break;
  case SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP:
    str = "fwd_dec_dp";
  break;
  case SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE:
    str = "fwd_dec_cpu_trap_code";
  break;
  case SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_QUAL:
    str = "fwd_dec_cpu_trap_qual";
  break;
  case SOC_PPC_FP_QUAL_IRPP_UP:
    str = "up";
  break;
  case SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE:
    str = "snoop_code";
  break;
  case SOC_PPC_FP_QUAL_IRPP_LEARN_DECISION_DEST:
    str = "learn_decision_dest";
  break;
  case SOC_PPC_FP_QUAL_IRPP_LEARN_ADD_INFO:
    str = "learn_add_info";
  break;
  case SOC_PPC_FP_QUAL_IRPP_IN_LIF:
    str = "in_lif";
  break;
  case SOC_PPC_FP_QUAL_IRPP_LL_MIRROR_CMD:
    str = "ll_mirror_cmd";
  break;
  case SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI:
    str = "system_vsi";
  break;
  case SOC_PPC_FP_QUAL_IRPP_ORIENTATION_IS_HUB:
    str = "orientation_is_hub";
  break;
  case SOC_PPC_FP_QUAL_IRPP_VLAN_ID:
    str = "vlan_id";
  break;
  case SOC_PPC_FP_QUAL_IRPP_VLAN_PCP:
    str = "vlan_pcp";
  break;
  case SOC_PPC_FP_QUAL_IRPP_VLAN_DEI:
    str = "vlan_dei";
    break;
  case SOC_PPC_FP_QUAL_IRPP_STP_STATE:
    str = "stp_state";
  break;
  case SOC_PPC_FP_QUAL_IRPP_FWD_TYPE:
    str = "fwd_type";
  break;
  case SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX:
    str = "sub_header_ndx";
  break;
  case SOC_PPC_FP_QUAL_IRPP_KEY_CHANGED:
    str = "key_changed";
    break;
  case SOC_PPC_FP_QUAL_IRPP_IN_RIF:
    str = "in_rif";
    break;
  case SOC_PPC_FP_QUAL_IRPP_VRF:
    str = "vrf";
  break;
  case SOC_PPC_FP_QUAL_IRPP_PCKT_IS_COMP_MC:
    str = "pckt_is_comp_mc";
  break;
  case SOC_PPC_FP_QUAL_IRPP_MY_BMAC:
    str = "my_bmac";
  break;
  case SOC_PPC_FP_QUAL_IRPP_IN_TTL:
    str = "in_ttl";
  break;
  case SOC_PPC_FP_QUAL_IRPP_IN_DSCP_EXP:
    str = "in_dscp_exp";
  break;
  case SOC_PPC_FP_QUAL_IRPP_PACKET_SIZE_RANGE:
    str = "packet_size_range";
  break;
  case SOC_PPC_FP_QUAL_IRPP_TERM_TYPE:
    str = "term_type";
  break;
  case SOC_PPC_FP_QUAL_ERPP_PP_PORT_DATA:
    str = "erpp_pp_port_data";
  break;
  case SOC_PPC_FP_QUAL_ERPP_IPV4_NEXT_PROTOCOL:
    str = "erpp_ipv4_next_protocol";
  break;
  case SOC_PPC_FP_QUAL_ERPP_FTMH:
    str = "erpp_ftmh";
  break;
  case SOC_PPC_FP_QUAL_ERPP_PAYLOAD:
    str = "erpp_payload";
  break;
  case SOC_PPC_FP_QUAL_IRPP_LEM_2ND_LKUP_ASD:
    str = "2nd_lookup_asd";
    break;
  case SOC_PPC_FP_QUAL_IRPP_INVALID:
    str = "invalid";
    break;
  case SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR:
    str = "in_port_key_gen_var";
    break;
  case SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR_PS:
    str = "in_port_key_gen_var_ps";
    break;
  case SOC_PPC_FP_QUAL_IRPP_IN_PORT_BITMAP:
    str = "in_port_bitmap";
    break;
  case SOC_PPC_FP_QUAL_IRPP_PTC_KEY_GEN_VAR:
    str = "ptc_key_gen_var";
    break;
  case SOC_PPC_FP_QUAL_IRPP_PMF_PGM_KEY_GEN_VAR:
    str = "pmf_pgm_key_gen_var";
    break;
  case SOC_PPC_FP_QUAL_HDR_ITMH:
    str = "itmh";
    break;
  case SOC_PPC_FP_QUAL_HDR_ITMH_EXT:
    str = "itmh_ext";
    break;
  case SOC_PPC_FP_QUAL_HDR_ITMH_DEST_FWD:
    str = "itmh_dest_fwd";
    break;
  case SOC_PPC_FP_QUAL_HDR_ITMH_PMF_HDR_EXT:
    str = "itmh_pmf_hdr_ext";
    break;
  case SOC_PPC_FP_QUAL_HDR_FTMH:
    str = "ftmh";
    break;
  case SOC_PPC_FP_QUAL_HDR_FTMH_LB_KEY_EXT_AFTER_FTMH:
    str = "lb_key_ext_after_ftmh";
    break;
  case SOC_PPC_FP_QUAL_HDR_FTMH_LB_KEY_START_OF_PACKET:
    str = "lb_key_start_of_packet";
    break;
  case SOC_PPC_FP_QUAL_HDR_DSP_EXTENSION_AFTER_FTMH:
    str = "dsp_extension_after_ftmh";
    break;
  case SOC_PPC_FP_QUAL_HDR_STACKING_EXT_AFTER_DSP_EXT:
    str = "stacking_ext_after_dsp_ext";
    break;
  case SOC_PPC_FP_QUAL_HDR_STACKING_EXT_AFTER_DSP_EXT_PETRA:
    str = "stacking_ext_after_dsp_ext_petra";
    break;
  case SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES:
    str = "all_zeroes";
    break;
  case SOC_PPC_FP_QUAL_IRPP_ALL_ONES:
    str = "all_ones";
    break;
  case SOC_PPC_FP_QUAL_IRPP_PROG_VAR:
    str = "user_defined_bits";
    break;
  case SOC_PPC_FP_QUAL_IRPP_PEM_GENERAL_DATA:
    str = "pem_general_data";
    break;
  case SOC_PPC_FP_QUAL_PACKET_HEADER_SIZE:
    str = "packet_header_size";
    break;
  case SOC_PPC_FP_QUAL_HEADER_OFFSET0:
    str = "header_offset0";
    break;
  case SOC_PPC_FP_QUAL_HEADER_OFFSET1:
    str = "header_offset1";
    break;
  case SOC_PPC_FP_QUAL_HEADER_OFFSET2:
    str = "header_offset2";
    break;
  case SOC_PPC_FP_QUAL_HEADER_OFFSET3:
    str = "header_offset3";
    break;
  case SOC_PPC_FP_QUAL_HEADER_OFFSET4:
    str = "header_offset4";
    break;
  case SOC_PPC_FP_QUAL_HEADER_OFFSET5:
    str = "header_offset5";
    break;
  case SOC_PPC_FP_QUAL_HEADER_OFFSET_0_UNTIL_5:
    str = "header_offset_0_until_5";
    break;
  case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER0:
    str = "pfq0"; 
    break;
  case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1:
    str = "pfq1"; 
    break;
  case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_INNER_TAG:
    str = "pfq1_inner_tag"; 
    break;
  case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_PRIORITY:
    str = "pfq1_next_priority"; 
    break;
  case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_OUTER_TAG:
    str = "pfq1_outer_tag"; 
    break;
  case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL:
    str = "pfq1_next_protocol"; 
    break;
  case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_ENCAPSULATION:
    str = "pfq1_next_encapsulation"; 
    break;
  case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2:
    str = "pfq2"; 
    break;
  case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_BOS:
    str = "pfq2_bos"; 
    break;
  case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL:
    str = "pfq2_next_protocol"; 
    break;
  case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_FRAGMENTED:
    str = "pfq2_ip_fragmented"; 
    break;
  case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_HAS_OPTIONS:
    str = "pfq2_ip_has_options"; 
    break;
  case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3:
    str = "pfq3"; 
    break;
  case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3_IP_FRAGMENTED:
    str = "pfq3_ip_fragmented"; 
    break;
  case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER4:
    str = "pfq4"; 
    break;
  case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER5:
    str = "pfq5"; 
    break;
  case SOC_PPC_FP_QUAL_FORWARDING_ACTION_STRENGTH:
    str = "fwd_action_strength"; 
    break;
  case SOC_PPC_FP_QUAL_FORWARDING_ACTION_METER_TRAFFIC_CLASS:
    str = "fwd_action_meter_traffic_class"; 
    break;
  case SOC_PPC_FP_QUAL_SNOOP_STRENGTH:
    str = "snoop_strength";
    break;
  case SOC_PPC_FP_QUAL_VSI_PROFILE:
    str = "vsi_profile";
    break;
  case SOC_PPC_FP_QUAL_FID:
    str = "fid";
    break;
  case SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_COMMAND:
    str = "vlan_edit_cmd_command";
    break;
  case SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID2:
    str = "vlan_edit_cmd_vid2";
    break;
  case SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID1:
    str = "vlan_edit_cmd_vid1";
    break;
  case SOC_PPC_FP_QUAL_FORWARDING_OFFSET_EXTENSION:
    str = "fwd_offset_extension"; 
    break;
  case SOC_PPC_FP_QUAL_FORWARDING_HEADER_ENCAPSULATION:
    str = "fwd_header_encapsulation"; 
    break;
  case SOC_PPC_FP_QUAL_IGNORE_CP:
    str = "ignore_cp";
    break;
  case SOC_PPC_FP_QUAL_SEQUENCE_NUMBER_TAG:
    str = "sequence_number_tag";
    break;
  case SOC_PPC_FP_QUAL_EEI:
    str = "eei";
    break;
  case SOC_PPC_FP_QUAL_OUT_LIF:
    str = "out_lif";
    break;
  case SOC_PPC_FP_QUAL_RPF_DESTINATION:
    str = "rpf_destination";
    break;
  case SOC_PPC_FP_QUAL_RPF_DESTINATION_VALID:
    str = "rpf_destination_valid";
    break;
  case SOC_PPC_FP_QUAL_INGRESS_LEARN_ENABLE:
    str = "ingress_learn_enable";
    break;
  case SOC_PPC_FP_QUAL_EGRESS_LEARN_ENABLE:
    str = "egress_learn_enable";
    break;
  case SOC_PPC_FP_QUAL_LEARN_KEY:
    str = "learn_key";
    break;
  case SOC_PPC_FP_QUAL_LEARN_KEY_MAC:
    str = "learn_key_mac";
    break;
  case SOC_PPC_FP_QUAL_LEARN_KEY_VLAN:
    str = "learn_key_vlan";
    break;
  case SOC_PPC_FP_QUAL_IN_LIF_PROFILE:
    str = "in_lif_profile";
    break;
  case SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF:
    str = "in_lif_profile_same_if";
    break;
  case SOC_PPC_FP_QUAL_LEARN_OR_TRANSPLANT:
    str = "learn_or_transplant";
    break;
  case SOC_PPC_FP_QUAL_PACKET_IS_BOOTP_DHCP:
    str = "packet_is_bootp_dhcp";
    break;
  case SOC_PPC_FP_QUAL_UNKNOWN_ADDR:
    str = "unknown_addr";
    break;
  case SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE:
    str = "fwd_prcessing_profile";
    break;
  case SOC_PPC_FP_QUAL_ELK_ERROR:
    str = "elk_error";
    break;
  case SOC_PPC_FP_QUAL_ELK_LKP_PAYLOAD:
    str = "elk_lkp_payload";
    break;
  case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_0:
    str = "elk_lookup_found_0";
    break;
  case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_1:
    str = "elk_lookup_found_1";
    break;
  case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_2:
    str = "elk_lookup_found_2";
    break;
  case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_3:
    str = "elk_lookup_found_3";
    break;
  case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_4:
    str = "elk_lookup_found_4";
    break;
  case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_5:
    str = "elk_lookup_found_5";
    break;
  case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_6:
    str = "elk_lookup_found_6";
    break;
  case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_7:
    str = "elk_lookup_found_7";
    break;
  case SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_0:
    str = "elk_lookup_result_0";
    break;
  case SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_1:
    str = "elk_lookup_result_1";
    break;
  case SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_2:
    str = "elk_lookup_result_2";
    break;
  case SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_3:
    str = "elk_lookup_result_3";
    break;
  case SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_4:
    str = "elk_lookup_result_4";
    break;
  case SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_5:
    str = "elk_lookup_result_5";
    break;
  case SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_6:
    str = "elk_lookup_result_6";
    break;
  case SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_7:
    str = "elk_lookup_result_7";
    break;
  case SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND:
    str = "lem_1st_lookup_found";
    break;
  case SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_RESULT:
    str = "lem_1st_lookup_result";
    break;
  case SOC_PPC_FP_QUAL_COUNTER_UPDATE_A:
    str = "counter_update_a";
    break;
  case SOC_PPC_FP_QUAL_COUNTER_POINTER_A:
    str = "counter_pointer_a";
    break;
  case SOC_PPC_FP_QUAL_COUNTER_UPDATE_B:
    str = "counter_update_b";
    break;
  case SOC_PPC_FP_QUAL_COUNTER_POINTER_B:
    str = "counter_pointer_b";
    break;
  case SOC_PPC_FP_QUAL_PROGRAM_INDEX:
    str = "program_index";
    break;
  case SOC_PPC_FP_QUAL_LEARN_DATA:
    str = "learn_data";
    break;
  case SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_FOUND:
    str = "lem_2nd_lookup_found";
    break;
  case SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_RESULT:
    str = "lem_2nd_lookup_result";
    break;
  case SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_FOUND:
    str = "lpm_1st_lookup_found";
    break;
  case SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_RESULT:
    str = "lpm_1st_lookup_result";
    break;
  case SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_FOUND:
    str = "lpm_2nd_lookup_found";
    break;
  case SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_RESULT:
    str = "lpm_2nd_lookup_result";
    break;
  case SOC_PPC_FP_QUAL_TCAM_MATCH:
    str = "tcam_match";
    break;
  case SOC_PPC_FP_QUAL_TCAM_RESULT:
    str = "tcam_result";
    break;
  case SOC_PPC_FP_QUAL_TCAM_TRAPS0_MATCH:
    str = "tcam_traps0_match";
    break;
  case SOC_PPC_FP_QUAL_TCAM_TRAPS0_RESULT:
    str = "tcam_traps0_result";
    break;
  case SOC_PPC_FP_QUAL_TCAM_TRAPS1_MATCH:
    str = "tcam_traps1_match";
    break;
  case SOC_PPC_FP_QUAL_TCAM_TRAPS1_RESULT:
    str = "tcam_traps1_result";
    break;
  case SOC_PPC_FP_QUAL_TT_PROCESSING_PROFILE:
    str = "tt_processing_profile";
    break;
  case SOC_PPC_FP_QUAL_TT_LOOKUP0_FOUND:
    str = "tt_lookup0_found";
    break;
  case SOC_PPC_FP_QUAL_TT_LOOKUP0_PAYLOAD:
    str = "tt_lookup0_payload";
    break;
  case SOC_PPC_FP_QUAL_TT_LOOKUP1_FOUND:
    str = "tt_lookup1_found";
    break;
  case SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD:
    str = "tt_lookup1_payload";
    break;
  case SOC_PPC_FP_QUAL_VT_PROCESSING_PROFILE:
    str = "vt_processing_profile";
    break;
  case SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND:
    str = "vt_lookup0_found";
    break;
  case SOC_PPC_FP_QUAL_VT_LOOKUP0_PAYLOAD:
    str = "vt_lookup0_payload";
    break;
  case SOC_PPC_FP_QUAL_VT_LOOKUP1_FOUND:
    str = "vt_lookup1_found";
    break;
  case SOC_PPC_FP_QUAL_VT_LOOKUP1_PAYLOAD:
    str = "vt_lookup1_payload";
    break;
  case SOC_PPC_FP_QUAL_CPU_TRAP_CODE_PROFILE:
    str = "cpu_trap_code_profile";
    break;
  case SOC_PPC_FP_QUAL_VID_VALID:
    str = "vid_valid";
    break;
  case SOC_PPC_FP_QUAL_DA_IS_BPDU:
    str = "da_is_bpdu";
    break;
  case SOC_PPC_FP_QUAL_PACKET_IS_IEEE1588:
    str = "packet_is_ieee1588";
    break;
  case SOC_PPC_FP_QUAL_IEEE1588_ENCAPSULATION:
    str = "ieee1588_encapsulation";
    break;
  case SOC_PPC_FP_QUAL_IEEE1588_COMPENSATE_TIME_STAMP:
    str = "ieee1588_compensate_time_stamp";
    break;
  case SOC_PPC_FP_QUAL_IEEE1588_COMMAND:
    str = "ieee1588_command";
    break;
  case SOC_PPC_FP_QUAL_IEEE1588_HEADER_OFFSET:
    str = "ieee1588_header_offset";
    break;
  case SOC_PPC_FP_QUAL_OAM_UP_MEP:
    str = "oam_up_mep";
    break;
  case SOC_PPC_FP_QUAL_OAM_SUB_TYPE:
    str = "oam_sub_type";
    break;
  case SOC_PPC_FP_QUAL_OAM_OFFSET:
    str = "oam_offset";
    break;
  case SOC_PPC_FP_QUAL_OAM_STAMP_OFFSET:
    str = "oam_stamp_offset";
    break;
  case SOC_PPC_FP_QUAL_OAM_METER_DISABLE:
    str = "oam_meter_disable";
    break;
  case SOC_PPC_FP_QUAL_OAM_ID:
    str = "oam_id";
    break;
  case SOC_PPC_FP_QUAL_TUNNEL_ID:
    str = "tunnel_id";
    break;
  case SOC_PPC_FP_QUAL_ARP_SENDER_IP4:
    str = "arp_sender_ip4";
    break;
  case SOC_PPC_FP_QUAL_ARP_TARGET_IP4:
    str = "arp_target_ip4";
    break;
  case SOC_PPC_FP_QUAL_ARP_OPCODE_IP4:
    str = "arp_opcode_ip4";
    break;
  case SOC_PPC_FP_QUAL_STAMP_NATIVE_VSI:
    str = "stamp_native_vsi";
    break;
  case SOC_PPC_FP_QUAL_NATIVE_VSI:
    str = "native_vsi";
    break;
  case SOC_PPC_FP_QUAL_CPU2FLP_C_INTERNAL_FIELDS_DATA:
    str = "cpu2flp_c_internal_fields_data";
    break;
  case SOC_PPC_FP_QUAL_SERVICE_TYPE:
    str = "service_type";
    break;
  case SOC_PPC_FP_QUAL_VSI_UNKNOWN_DA_DESTINATION:
    str = "vsi_unknown_da_destination";
    break;
  case SOC_PPC_FP_QUAL_TT_LEARN_ENABLE:
    str = "tt_learn_enable";
    break;
  case SOC_PPC_FP_QUAL_TT_LEARN_DATA:
    str = "tt_learn_data";
    break;
  case SOC_PPC_FP_QUAL_IN_LIF_UNKNOWN_DA_PROFILE:
    str = "in_lif_unknown_da_profile";
    break;
  case SOC_PPC_FP_QUAL_IN_RIF_UC_RPF_ENABLE:
    str = "in_rif_uc_rpf_enable";
    break;
  case SOC_PPC_FP_QUAL_L3VPN_DEFAULT_ROUTING:
    str = "l3vpn_default_routing";
    break;
  case SOC_PPC_FP_QUAL_TERMINATED_TTL_VALID:
    str = "terminated_ttl_valid";
    break;
  case SOC_PPC_FP_QUAL_TERMINATED_TTL:
    str = "terminated_ttl";
    break;
  case SOC_PPC_FP_QUAL_TERMINATED_DSCP_EXP:
    str = "terminated_dscp_exp";
    break;
  case SOC_PPC_FP_QUAL_IPR2DSP_6EQ7_ESADI:
    str = "ipr2dsp_6eq7_esadi";
    break;
  case SOC_PPC_FP_QUAL_IPR2DSP_6EQ7_MPLS_EXP:
    str = "ipr2dsp_6eq7_mpls_exp";
    break;
  case SOC_PPC_FP_QUAL_TERMINATED_PROTOCOL:
    str = "terminated_protocol";
    break;
  case SOC_PPC_FP_QUAL_COS_PROFILE:
    str = "cos_profile";
    break;
  case SOC_PPC_FP_QUAL_VTT_OAM_LIF_VALID:
    str = "vtt_oam_lif_valid";
    break;
  case SOC_PPC_FP_QUAL_VTT_OAM_LIF:
    str = "vtt_oam_lif";
    break;
  case SOC_PPC_FP_QUAL_LL_LEM_1ST_LOOKUP_FOUND:
    str = "ll_lem_1st_lookup_found";
    break;
  case SOC_PPC_FP_QUAL_LL_LEM_1ST_LOOKUP_RESULT:
    str = "ll_lem_1st_lookup_result";
    break;
  case SOC_PPC_FP_QUAL_VT_ISA_KEY:
    str = "vt_isa_key";
    break;
  case SOC_PPC_FP_QUAL_KEY_AFTER_HASHING:
    str = "key_after_hashing";
    break;
  case SOC_PPC_FP_QUAL_IS_FEC_DEST_14_0:
    str = "is_fec_dest_14_0";
    break;
  case SOC_PPC_FP_QUAL_IRPP_ELK_LKP_PAYLOAD_LSB:
    str = "elk_lkp_payload_lsb";
    break;
  case SOC_PPC_FP_QUAL_IRPP_IN_RIF_PROFILE:
    str = "in_rif_profile";
    break;
  case SOC_PPC_FP_QUAL_IRPP_TCAM0_MATCH:
    str = "tcam0_match";
    break;
  case SOC_PPC_FP_QUAL_IRPP_TCAM0_RESULT:
    str = "tcam0_result";
    break;
  case SOC_PPC_FP_QUAL_IRPP_TCAM1_MATCH:
    str = "tcam1_match";
    break;
  case SOC_PPC_FP_QUAL_IRPP_TCAM1_RESULT:
    str = "tcam1_result";
    break;
  case SOC_PPC_FP_QUAL_IRPP_IN_LIF_DATA:
    str = "in_lif_data";
    break;
  case SOC_PPC_FP_QUAL_IRPP_RPF_STAMP_NATIVE_VSI:
    str = "rpf_stamp_native_vsi";
    break;
  case SOC_PPC_FP_QUAL_IRPP_RPF_NATIVE_VSI:
    str = "rpf_native_vsi";
    break;
  case SOC_PPC_FP_QUAL_IRPP_IN_PORT_MAPPED_PP_PORT:
    str = "in_port_mapped_pp_port";
    break;
  case SOC_PPC_FP_QUAL_IRPP_IN_LIF_DATA_INDEX:
    str = "in_lif_data_index";
    break;
  case SOC_PPC_FP_QUAL_IRPP_LOCAL_IN_LIF:
    str = "local_in_lif";
    break;
  case SOC_PPC_FP_QUAL_IRPP_CONSISTENT_HASHING_PGM_KEY_GEN_VAR:
    str = "consistent_hashing_pgm_key_gen_var";
    break;
  case SOC_PPC_FP_QUAL_IRPP_PACKET_FORMAT_CODE_ACL:
    str = "packet_format_code_acl";
    break;
  case SOC_PPC_FP_QUAL_IS_EQUAL:
    str = "is_equal";
    break;
  case SOC_PPC_FP_QUAL_EXTENSION_HEADER_TYPE:
    str = "extension_header_type";
    break;
  case SOC_PPC_FP_QUAL_HDR_HIGIG_FRC:
    str = "hdr_higig_frc";
    break;
  case SOC_PPC_FP_QUAL_HDR_HIGIG_PPD:
    str = "hdr_higig_ppd";
    break;
  case SOC_PPC_FP_QUAL_HDR_HIGIG_PPD_EXT:
    str = "hdr_higig_ppd_ext";
    break;
  case SOC_PPC_FP_QUAL_HDR_MH_FLOW:
    str = "hdr_mh_flow";
    break;
  case SOC_PPC_FP_QUAL_HDR_MH_TC2:
    str = "hdr_mh_tc2";
    break;
  case SOC_PPC_FP_QUAL_HDR_MH_DP0:
    str = "hdr_mh_d90";
    break;
  case SOC_PPC_FP_QUAL_HDR_MH_CAST:
    str = "hdr_mh_cast";
    break;
  case SOC_PPC_FP_QUAL_HDR_MH_DP1:
    str = "hdr_mh_dp1";
    break;
  case SOC_PPC_FP_QUAL_HDR_MH_TC10:
    str = "hdr_mh_tc10";
    break;
  case SOC_PPC_FP_QUAL_HDR_PTCH_RESERVE_LSB:
    str = "hdr_ptch_reserve_lsb";
    break;
  case SOC_PPC_FP_QUAL_ERPP_ONES:
    str = "erpp_ones";
    break;
  case SOC_PPC_FP_QUAL_ERPP_ZEROES:
    str = "erpp_zeroes";
    break;
  case SOC_PPC_FP_QUAL_ERPP_OAM_TS:
    str = "erpp_oam_ts";
    break;
  case SOC_PPC_FP_QUAL_ERPP_LEARN_EXT:
    str = "erpp_learn_ext";
    break;
  case SOC_PPC_FP_QUAL_ERPP_LEARN_EXT_SRC_PORT:
    str = "erpp_learn_ext_src_port";
    break;
  case SOC_PPC_FP_QUAL_ERPP_LEARN_EXT_IN_VPORT:
    str = "erpp_learn_ext_in_vport";
    break;
  case SOC_PPC_FP_QUAL_ERPP_FHEI:
    str = "erpp_fhei";
    break;
  case SOC_PPC_FP_QUAL_ERPP_FHEI_IPV4_TTL:
    str = "erpp_fhei_ipv4_ttl";
    break;
  case SOC_PPC_FP_QUAL_ERPP_FHEI_DSCP:
    str = "erpp_fhei_ipv4_dscp";
    break;
  case SOC_PPC_FP_QUAL_ERPP_FHEI_EXP:
    str = "erpp_fhei_ipv4_exp";
    break;
  case SOC_PPC_FP_QUAL_ERPP_OUT_TM_PORT_PMF_DATA:
    str = "erpp_out_tm_port_pmf_data";
    break;
  case SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA:
    str = "erpp_out_pp_port_pmf_data";
    break;
  case SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA_PS:
    str = "erpp_out_pp_port_pmf_data_ps";
    break;
  case SOC_PPC_FP_QUAL_ERPP_EEI:
    str = "erpp_eei";
    break;
  case SOC_PPC_FP_QUAL_ERPP_EXT_IN_LIF:
    str = "erpp_ext_in_lif";
    break;
  case SOC_PPC_FP_QUAL_ERPP_EXT_OUT_LIF:
    str = "erpp_ext_out_lif";
    break;
  case SOC_PPC_FP_QUAL_ERPP_STACKING_ROUTE_HISTORY_BITMAP:
    str = "erpp_stacking_route_history_bitmap";
    break;
  case SOC_PPC_FP_QUAL_ERPP_DSP_EXT:
    str = "erpp_dsp_ext";
    break;
  case SOC_PPC_FP_QUAL_ERPP_PACKET_SIZE:
    str = "erpp_packet_size";
    break;
  case SOC_PPC_FP_QUAL_ERPP_DST_SYSTEM_PORT:
    str = "erpp_dst_system_port";
    break;
  case SOC_PPC_FP_QUAL_ERPP_SRC_SYSTEM_PORT:
    str = "erpp_src_system_port";
    break;
  case SOC_PPC_FP_QUAL_ERPP_VSI_OR_VRF:
    str = "erpp_vsi_or_vrf";
    break;
  case SOC_PPC_FP_QUAL_ERPP_VSI_OR_VRF_ORIG:
    str = "erpp_vsi_or_vrf_orig";
    break;
  case SOC_PPC_FP_QUAL_ERPP_FWD_OFFSET:
    str = "erpp_fwd_offset";
    break;
  case SOC_PPC_FP_QUAL_ERPP_ETH_TAG_FORMAT:
    str = "erpp_eth_tag_format";
    break;
  case SOC_PPC_FP_QUAL_ERPP_SYS_VALUE1:
    str = "erpp_sys_value1";
    break;
  case SOC_PPC_FP_QUAL_ERPP_SYS_VALUE2:
    str = "erpp_sys_value2";
    break;
  case SOC_PPC_FP_QUAL_ERPP_DSP_PTR_ORIG:
    str = "erpp_dsp_ptr_orig";
    break;
  case SOC_PPC_FP_QUAL_ERPP_DSP_PTR:
    str = "erpp_dsp_ptr";
    break;
  case SOC_PPC_FP_QUAL_ERPP_OUT_TM_PORT:
    str = "erpp_out_tm_port";
    break;
  case SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT:
    str = "erpp_out_pp_port";
    break;
  case SOC_PPC_FP_QUAL_ERPP_LB_KEY:
    str = "erpp_lb_key";
    break;
  case SOC_PPC_FP_QUAL_ERPP_TC:
    str = "erpp_tc";
    break;
  case SOC_PPC_FP_QUAL_ERPP_FORMAT_CODE:
    str = "erpp_format_code";
    break;
  case SOC_PPC_FP_QUAL_ERPP_FWD_CODE:
    str = "erpp_fwd_code";
    break;
  case SOC_PPC_FP_QUAL_ERPP_FWD_CODE_ORIG:
    str = "erpp_fwd_code_orig";
    break;
  case SOC_PPC_FP_QUAL_ERPP_ACTION_PROFILE:
    str = "erpp_action_profile";
    break;
  case SOC_PPC_FP_QUAL_ERPP_HEADER_CODE:
    str = "erpp_header_code";
    break;
  case SOC_PPC_FP_QUAL_ERPP_ETH_TYPE_CODE:
    str = "erpp_eth_type_code";
    break;
  case SOC_PPC_FP_QUAL_ERPP_IN_LIF_ORIENTATION:
    str = "erpp_in_lif_orientation";
    break;
  case SOC_PPC_FP_QUAL_ERPP_SNOOP_CPU_CODE:
    str = "erpp_snoop_cpu_code";
    break;
  case SOC_PPC_FP_QUAL_ERPP_FTMH_RESERVED:
    str = "erpp_ftmh_reserved";
    break;
  case SOC_PPC_FP_QUAL_ERPP_ECN_CAPABLE:
    str = "erpp_ecn_capable";
    break;
  case SOC_PPC_FP_QUAL_ERPP_PPH_TYPE:
    str = "erpp_pph_type";
    break;
  case SOC_PPC_FP_QUAL_ERPP_TM_ACTION_TYPE:
    str = "erpp_tm_action_type";
    break;
  case SOC_PPC_FP_QUAL_ERPP_DP:
    str = "erpp_dp";
    break;
  case SOC_PPC_FP_QUAL_ERPP_FHEI_CODE:
    str = "erpp_fhei_code";
    break;
  case SOC_PPC_FP_QUAL_ERPP_LEARN_ALLOWED:
    str = "erpp_learn_allowed";
    break;
  case SOC_PPC_FP_QUAL_ERPP_UNKNOWN_ADDR:
    str = "erpp_unknown_addr";
    break;
  case SOC_PPC_FP_QUAL_ERPP_LEARN_EXT_VALID:
    str = "erpp_learn_ext_valid";
    break;
  case SOC_PPC_FP_QUAL_ERPP_BYPASS_FILTERING:
    str = "erpp_bypass_filtering";
    break;
  case SOC_PPC_FP_QUAL_ERPP_EEI_VALID:
    str = "erpp_eei_valid";
    break;
  case SOC_PPC_FP_QUAL_ERPP_CNI:
    str = "erpp_cni";
    break;
  case SOC_PPC_FP_QUAL_ERPP_DSP_EXT_VALID:
    str = "erpp_dsp_ext_valid";
    break;
  case SOC_PPC_FP_QUAL_ERPP_SYSTEM_MC:
    str = "erpp_system_mc";
    break;
  case SOC_PPC_FP_QUAL_ERPP_OUT_MIRROR_DISABLE:
    str = "erpp_out_mirror_disable";
    break;
  case SOC_PPC_FP_QUAL_ERPP_EXCLUDE_SRC:
    str = "erpp_exclude_src";
    break;
  case SOC_PPC_FP_QUAL_ERPP_DISCARD:
    str = "erpp_discard";
    break;
  case SOC_PPC_FP_QUAL_ERPP_FABRIC_OR_EGRESS_MC:
    str = "erpp_fabric_or_egress_mc";
    break;
  case SOC_PPC_FP_QUAL_ERPP_RESEVED:
    str = "erpp_reseved";
    break;
  case SOC_PPC_FP_QUAL_ERPP_FIRST_COPY:
    str = "erpp_first_copy";
    break;
  case SOC_PPC_FP_QUAL_ERPP_LAST_COPY:
    str = "erpp_last_copy";
    break;
  case SOC_PPC_FP_QUAL_ERPP_START_BUFFER:
    str = "erpp_start_buffer";
    break;
  case SOC_PPC_FP_QUAL_ERPP_CONTEXT:
    str = "erpp_context";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_DA:
    str = "erpp_nwk_rcrd_eth_da";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_SA:
    str = "erpp_nwk_rcrd_eth_sa";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_DATA:
    str = "erpp_nwk_rcrd_eth_data";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_ADDITIONAL_DATA:
    str = "erpp_nwk_rcrd_eth_additional_data";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_CPID0:
    str = "erpp_nwk_rcrd_eth_cpid0";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_CPID1:
    str = "erpp_nwk_rcrd_eth_cpid1";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_CPID2:
    str = "erpp_nwk_rcrd_eth_cpid2";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TPID:
    str = "erpp_nwk_rcrd_eth_outer_tpid";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TPID:
    str = "erpp_nwk_rcrd_eth_inner_tpid";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_ID:
    str = "erpp_nwk_rcrd_eth_outer_tag_id";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_CFI:
    str = "erpp_nwk_rcrd_eth_outer_tag_cfi";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_PRI:
    str = "erpp_nwk_rcrd_eth_outer_tag_pri";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_PRI_CFI:
    str = "erpp_nwk_rcrd_eth_outer_tag_pri_cfi";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG:
    str = "erpp_nwk_rcrd_eth_outer_tag";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG:
    str = "erpp_nwk_rcrd_eth_inner_tag";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_ID:
    str = "erpp_nwk_rcrd_eth_outer_tag_id";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_CFI:
    str = "erpp_nwk_rcrd_eth_outer_tag_cfi";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_PRI:
    str = "erpp_nwk_rcrd_eth_outer_tag_pri";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_ID:
    str = "erpp_nwk_rcrd_fhei_eth_outer_tag_id";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_CFI:
    str = "erpp_nwk_rcrd_fhei_eth_outer_tag_cfi";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_PRI:
    str = "erpp_nwk_rcrd_fhei_eth_outer_tag_pri";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG:
    str = "erpp_nwk_rcrd_fhei_eth_outer_tag";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG:
    str = "erpp_nwk_rcrd_fhei_eth_inner_tag";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_ID:
    str = "erpp_nwk_rcrd_fhei_eth_outer_tag_id";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_CFI:
    str = "erpp_nwk_rcrd_fhei_eth_outer_tag_cfi";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_FORWARDING_DATA_MSB:
    str = "erpp_nwk_rcrd_bridged_forwarding_data_msb";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_FORWARDING_DATA_LSB:
    str = "erpp_nwk_rcrd_bridged_forwarding_data_lsb";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_ACL_DATA_HIGH:
    str = "erpp_nwk_rcrd_bridged_acl_data_high";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_ACL_DATA_LOW:
    str = "erpp_nwk_rcrd_bridged_acl_data_low";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_PRI:
    str = "erpp_nwk_rcrd_fhei_eth_outer_tag_pri";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_OPTIONS_PRESENT:
    str = "erpp_nwk_rcrd_ipv4_options_present";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_TOTAL_LEN_ERROR:
    str = "erpp_nwk_rcrd_ipv4_total_len_error";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_HEADER_LEN_ERROR:
    str = "erpp_nwk_rcrd_ipv4_header_len_error";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_CHECKSUM_ERROR:
    str = "erpp_nwk_rcrd_ipv4_checksum_error";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_VERSION_ERROR:
    str = "erpp_nwk_rcrd_ipv4_version_error";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_TOS:
    str = "erpp_nwk_rcrd_ipv4_tos";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_SIP:
    str = "erpp_nwk_rcrd_ipv4_sip";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_DIP:
    str = "erpp_nwk_rcrd_ipv4_dip";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_PROTOCOL:
    str = "erpp_nwk_rcrd_ipv4_protocol";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_L4_SRC_PORT:
    str = "erpp_nwk_rcrd_ipv4_l4_src_port";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_L4_DEST_PORT:
    str = "erpp_nwk_rcrd_ipv4_l4_dest_port";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_TC:
    str = "erpp_nwk_rcrd_ipv6_msbs_tc";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_HOP_LIMIT:
    str = "erpp_nwk_rcrd_ipv6_msbs_tc_hop_limit";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_FLOW_LABEL:
    str = "erpp_nwk_rcrd_ipv6_msbs_flow_label";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_NEXT_PROTOCOL:
    str = "erpp_nwk_rcrd_ipv6_msbs_next_protocol";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MC_DST:
    str = "erpp_nwk_rcrd_ipv6_mc_dst";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_IPV4_MAPPED_DST:
    str = "erpp_nwk_rcrd_ipv6_ipv4_mapped_dst";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_IPV4_CMP_DST:
    str = "erpp_nwk_rcrd_ipv6_ipv4_cmp_dst";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SITE_LOCAL_SRC:
    str = "erpp_nwk_rcrd_ipv6_site_local_src";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_LINK_LOCAL_SRC:
    str = "erpp_nwk_rcrd_ipv6_link_local_src";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SITE_LOCAL_DST:
    str = "erpp_nwk_rcrd_ipv6_site_local_dst";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_LINK_LOCAL_DST:
    str = "erpp_nwk_rcrd_ipv6_link_local_dst";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_NEXT_HEADER_IS_ZERO:
    str = "erpp_nwk_rcrd_ipv6_next_header_is_zero";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_LOOPBACK_SRC_OR_DST:
    str = "erpp_nwk_rcrd_ipv6_loopback_src_or_dst";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SIP_IS_ALL_ZEROES:
    str = "erpp_nwk_rcrd_ipv6_sip_is_all_zeroes";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_DIP_IS_ALL_ZEROES:
    str = "erpp_nwk_rcrd_ipv6_dip_is_all_zeroes";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SIP_IS_MC:
    str = "erpp_nwk_rcrd_ipv6_sip_is_mc";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_VERSION_ERROR:
    str = "erpp_nwk_rcrd_ipv6_version_error";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_MPLS_EXTRA_DATA:
    str = "erpp_nwk_rcrd_mpls_extra_data";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_MPLS_HDR:
    str = "erpp_nwk_rcrd_mpls_hdr";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_EXTRA_DATA:
    str = "erpp_nwk_rcrd_trill_extra_data";
    break;
  case SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_HDR:
    str = "erpp_nwk_rcrd_trill_hdr";
    break;                                            
  case SOC_PPC_FP_QUAL_ETH_OAM_HEADER_BITS_0_31:
    str = "eth_oam_header_bits_0_31";
    break;     
  case SOC_PPC_FP_QUAL_ETH_OAM_HEADER_BITS_32_63:   
    str = "eth_oam_header_bits_32_63";
    break; 
  case SOC_PPC_FP_QUAL_MPLS_OAM_HEADER_BITS_0_31: 
    str = "mpls_oam_header_bits_0_31";
    break;   
  case SOC_PPC_FP_QUAL_MPLS_OAM_HEADER_BITS_32_63: 
    str = "mpls_oam_header_bits_32_63";
    break;  
  case SOC_PPC_FP_QUAL_MPLS_OAM_ACH:     
    str = "mpls_oam_ach";
    break;
  case SOC_PPC_FP_QUAL_OAM_OPCODE:    
    str = "oam_opcode";
    break;    
  case SOC_PPC_FP_QUAL_OAM_MD_LEVEL_UNTAGGED:    
    str = "oam_md_level";
    break;
  case SOC_PPC_FP_QUAL_OAM_MD_LEVEL_SINGLE_TAG:
	  str = "oam_md_level";
      break;
  case SOC_PPC_FP_QUAL_OAM_MD_LEVEL_DOUBLE_TAG:
	  str = "oam_md_level";
      break;
  case SOC_PPC_FP_QUAL_OAM_MD_LEVEL:
	  str = "oam_md_level";
      break;
  case SOC_PPC_FP_QUAL_TM_OUTER_TAG:
	  str = "tm_outer_tag";
      break;
  case SOC_PPC_FP_QUAL_TM_INNER_TAG:
	  str = "tm_inner_tag";
      break;
  case SOC_PPC_FP_QUAL_MY_DISCR_IPV4:
	  str = "my_discr_ipv4";
      break;
  case SOC_PPC_FP_QUAL_MY_DISCR_MPLS:
	  str = "my_discr_mpls";
      break;
  case SOC_PPC_FP_QUAL_MY_DISCR_PWE:
	  str = "my_discr_pwe";
      break;  
  case SOC_PPC_FP_QUAL_TRAP_QUALIFIER_FHEI:
      str = "trap qualifier from fhei";
      break;
  case SOC_PPC_FP_QUAL_EID:
	  str = "eid";
      break;
  case SOC_PPC_FP_QUAL_OAM_HEADER_BITS_32_63: 
    str = "oam_header_bits_32_63";
    break;     
  case SOC_PPC_FP_QUAL_ETH_HEADER_ISID:
      str = "eth_header_isid";
      break;
  case SOC_PPC_FP_QUAL_GRE_CRKS:
      str = "gre_crks";
      break;
  case SOC_PPC_FP_QUAL_GRE_KEY:
      str = "gre_key";
      break;
  case SOC_PPC_FP_QUAL_VXLAN_VNI:
      str = "vxlan_vni";
      break;
  case   SOC_PPC_FP_QUAL_VLAN_DOMAIN:
    str = "vlan_domain";
    break;
  case   SOC_PPC_FP_QUAL_INITIAL_VID:
    str = "initial_vid";
    break;
  case   SOC_PPC_FP_QUAL_MPLS_KEY3:
    str = "enh_mpls_key3";
    break;
  case   SOC_PPC_FP_QUAL_MPLS_KEY2:
    str = "enh_mpls_key2";
    break;
  case   SOC_PPC_FP_QUAL_MPLS_KEY1:
    str = "enh_mpls_key1";
    break;
  case   SOC_PPC_FP_QUAL_MPLS_KEY0:
    str = "enh_mpls_key0";
    break;
  case   SOC_PPC_FP_QUAL_CMPRSD_INNER_VID:
    str = "cmprsd_inner_vid";
    break;
  case   SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID:
    str = "cmprsd_outer_vid";
    break;
  case   SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF2:
    str = "mpls_label_range_base_lif2_lsb";
    break;
  case   SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF1:
    str = "mpls_label_range_base_lif1_lsb";
    break;
  case   SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF0:
    str = "mpls_label_range_base_lif0_lsb";
    break;
  case   SOC_PPC_FP_QUAL_LABEL3_IDX:
    str = "label3_idx";
    break;
  case   SOC_PPC_FP_QUAL_LABEL2_IDX:
    str = "label2_idx";
    break;
  case   SOC_PPC_FP_QUAL_LABEL1_IDX:
    str = "label1_idx";
    break;
  case   SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_DOMAIN:
    str = "bar_lem_1st_lookup_found_lem_domain_bits";
    break;
  case   SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_SA_DROP:
    str = "bar_lem_1st_lookup_found_lem_sa_drop_bit";
    break;
  case   SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_IS_LEARN_LIF:
    str = "bar_lem_1st_lookup_found_lem_is_learn_lif_bit";
    break;
  case   SOC_PPC_FP_QUAL_LEM_DYNAMIC_LEM_1ST_LOOKUP_FOUND_LEM:
    str = "lem_dynamic_bit_lem_1st_lookup_found";
    break;
  case   SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_DESTINATION:
    str = "bar_lem_1st_lookup_found_lem_destination";
    break;
  case   SOC_PPC_FP_QUAL_ISB_FOUND_IN_LIF_IDX:
    str = "vt_isb_in_lif_index_bits_isb_found";
    break;
  case   SOC_PPC_FP_QUAL_ISB_FOUND:
    str = "vt_isb_found";
    break;
  case   SOC_PPC_FP_QUAL_ISB_IN_LIF_IDX:
    str = "vt_isb_in_lif_index";
    break;
  case   SOC_PPC_FP_QUAL_ISA_FOUND_IN_LIF_IDX:
    str = "vt_isa_in_lif_index_bits_isa_found";
    break;
  case   SOC_PPC_FP_QUAL_ISA_FOUND:
    str = "vt_isa_found";
    break;
  case   SOC_PPC_FP_QUAL_ISA_IN_LIF_IDX:
    str = "vt_isa_in_lif_index";
    break;
  case   SOC_PPC_FP_QUAL_VT_TCAM_MATCH_IN_LIF_IDX:
    str = "vt_tcam_in_lif_index_bits_tcam_match";
    break;
  case   SOC_PPC_FP_QUAL_VT_TCAM_MATCH:
    str = "vt_tcam_match";
    break;
  case   SOC_PPC_FP_QUAL_VT_TCAM_IN_LIF_IDX:
    str = "vt_tcam_in_lif_index";
    break;
  case   SOC_PPC_FP_QUAL_INITIAL_VSI:
    str = "initial_vsi";
    break;
  case   SOC_PPC_FP_QUAL_IN_RIF_VALID_VRF:
    str = "in_rif_valid_vrf";
    break;
  case   SOC_PPC_FP_QUAL_IN_RIF_VALID_RIF_PROFILE:
    str = "in_rif_valid_rif_profile";
    break;
  case   SOC_PPC_FP_QUAL_IN_RIF_VALID_IN_RIF:
    str = "in_rif_valid_in_rif";
    break;
  case   SOC_PPC_FP_QUAL_IN_RIF_VALID:
    str = "in_rif_valid";
    break;
  case   SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_FOUND:
    str = "vt_lem_1st_lookup_found";
    break;
  case   SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND:
    str = "vt_lem_1st_lookup_not_found";
    break;
  case   SOC_PPC_FP_QUAL_MACT_DOMAIN:
    str = "mact_format1_domain";
    break;
  case   SOC_PPC_FP_QUAL_MACT_SA_DROP:
    str = "mact_format1_sa_drop";
    break;
  case   SOC_PPC_FP_QUAL_MACT_IS_LEARN:
    str = "mact_format1_is_learn_lif";
    break;
  case   SOC_PPC_FP_QUAL_MACT_DYNAMIC:
    str = "mact_format1_dynamic";
    break;
  case   SOC_PPC_FP_QUAL_MACT_DESTINATION:
    str = "mact_format1_destination";
    break;
  case   SOC_PPC_FP_QUAL_KEY3:
    str = "enh_mpls_key3";
    break;
  case   SOC_PPC_FP_QUAL_KEY3_16_INST0:
    str = "enh_mpls_key3_16_inst0";
    break;
  case   SOC_PPC_FP_QUAL_KEY3_16_INST1:
    str = "enh_mpls_key3_16_inst1";
    break;
  case   SOC_PPC_FP_QUAL_KEY3_16_INST2:
    str = "enh_mpls_key3_16_inst2";
    break;
  case   SOC_PPC_FP_QUAL_KEY3_32_INST0:
    str = "enh_mpls_key3_32_inst0";
    break;
  case   SOC_PPC_FP_QUAL_KEY3_32_INST1:
    str = "enh_mpls_key3_32_inst1";
    break;
  case   SOC_PPC_FP_QUAL_KEY3_LABEL:
    str = "enh_mpls_key3_label";
    break;
  case   SOC_PPC_FP_QUAL_KEY3_OUTER_VID_VALID:
    str = "enh_mpls_key3_outer_vid_valid";
    break;
  case   SOC_PPC_FP_QUAL_KEY3_OUTER_VID:
    str = "enh_mpls_key3_outer_vid";
    break;
  case   SOC_PPC_FP_QUAL_KEY3_INNER_VID_VALID:
    str = "enh_mpls_key3_inner_vid_valid";
    break;
  case   SOC_PPC_FP_QUAL_KEY3_INNER_VID:
    str = "enh_mpls_key3_inner_vid";
    break;
  case   SOC_PPC_FP_QUAL_KEY2:
    str = "enh_mpls_key2";
    break;
  case   SOC_PPC_FP_QUAL_KEY2_16_INST0:
    str = "enh_mpls_key2_16_inst0";
    break;
  case   SOC_PPC_FP_QUAL_KEY2_16_INST1:
    str = "enh_mpls_key2_16_inst1";
    break;
  case   SOC_PPC_FP_QUAL_KEY2_16_INST2:
    str = "enh_mpls_key2_16_inst2";
    break;
  case   SOC_PPC_FP_QUAL_KEY2_32_INST0:
    str = "enh_mpls_key2_32_inst0";
    break;
  case   SOC_PPC_FP_QUAL_KEY2_32_INST1:
    str = "enh_mpls_key2_32_inst1";
    break;
  case   SOC_PPC_FP_QUAL_KEY2_LABEL:
    str = "enh_mpls_key2_label";
    break;
  case   SOC_PPC_FP_QUAL_KEY2_OUTER_VID_VALID:
    str = "enh_mpls_key2_outer_vid_valid";
    break;
  case   SOC_PPC_FP_QUAL_KEY2_OUTER_VID:
    str = "enh_mpls_key2_outer_vid";
    break;
  case   SOC_PPC_FP_QUAL_KEY2_INNER_VID_VALID:
    str = "enh_mpls_key2_inner_vid_valid";
    break;
  case   SOC_PPC_FP_QUAL_KEY2_INNER_VID:
    str = "enh_mpls_key2_inner_vid";
    break;
  case   SOC_PPC_FP_QUAL_KEY1:
    str = "enh_mpls_key1";
    break;
  case   SOC_PPC_FP_QUAL_KEY1_16_INST0:
    str = "enh_mpls_key1_16_inst0";
    break;
  case   SOC_PPC_FP_QUAL_KEY1_16_INST1:
    str = "enh_mpls_key1_16_inst1";
    break;
  case   SOC_PPC_FP_QUAL_KEY1_16_INST2:
    str = "enh_mpls_key1_16_inst2";
    break;
  case   SOC_PPC_FP_QUAL_KEY1_32_INST0:
    str = "enh_mpls_key1_32_inst0";
    break;
  case   SOC_PPC_FP_QUAL_KEY1_32_INST1:
    str = "enh_mpls_key1_32_inst1";
    break;
  case   SOC_PPC_FP_QUAL_KEY1_LABEL:
    str = "enh_mpls_key1_label";
    break;
  case   SOC_PPC_FP_QUAL_KEY1_OUTER_VID_VALID:
    str = "enh_mpls_key1_outer_vid_valid";
    break;
  case   SOC_PPC_FP_QUAL_KEY1_OUTER_VID:
    str = "enh_mpls_key1_outer_vid";
    break;
  case   SOC_PPC_FP_QUAL_KEY1_INNER_VID_VALID:
    str = "enh_mpls_key1_inner_vid_valid";
    break;
  case   SOC_PPC_FP_QUAL_KEY1_INNER_VID:
    str = "enh_mpls_key1_inner_vid";
    break;
  case   SOC_PPC_FP_QUAL_KEY0:
    str = "enh_mpls_key0";
    break;
  case   SOC_PPC_FP_QUAL_KEY0_16_INST0:
    str = "enh_mpls_key0_16_inst0";
    break;
  case   SOC_PPC_FP_QUAL_KEY0_16_INST1:
    str = "enh_mpls_key0_16_inst1";
    break;
  case   SOC_PPC_FP_QUAL_KEY0_16_INST2:
    str = "enh_mpls_key0_16_inst2";
    break;
  case   SOC_PPC_FP_QUAL_KEY0_32_INST0:
    str = "enh_mpls_key0_32_inst0";
    break;
  case   SOC_PPC_FP_QUAL_KEY0_32_INST1:
    str = "enh_mpls_key0_32_inst1";
    break;
  case   SOC_PPC_FP_QUAL_KEY0_LABEL:
    str = "enh_mpls_key0_label";
    break;
  case   SOC_PPC_FP_QUAL_KEY0_OUTER_VID_VALID:
    str = "enh_mpls_key0_outer_vid_valid";
    break;
  case   SOC_PPC_FP_QUAL_KEY0_OUTER_VID:
    str = "enh_mpls_key0_outer_vid";
    break;
  case   SOC_PPC_FP_QUAL_KEY0_INNER_VID_VALID:
    str = "enh_mpls_key0_inner_vid_valid";
    break;
  case   SOC_PPC_FP_QUAL_KEY0_INNER_VID:
    str = "enh_mpls_key0_inner_vid";
    break;
  case  SOC_PPC_FP_QUAL_TRILL_INGRESS_NICK:
    str = "trill_ingress_nick";
    break;
  case  SOC_PPC_FP_QUAL_TRILL_EGRESS_NICK:
    str = "trill_egress_nick";
    break;
  case  SOC_PPC_FP_QUAL_TRILL_NATIVE_INNER_VLAN_VSI:
    str = "trill_native_inner_vlan_vsi";
    break;
  case  SOC_PPC_FP_QUAL_TRILL_NATIVE_VLAN_VSI:
    str = "trill_native_vlan_vsi";
    break;
  case SOC_PPC_FP_QUAL_TRILL_NATIVE_ETH_INNER_TPID:
    str = "trill_native_eth_inner_tpid";
    break;
  case SOC_PPC_FP_QUAL_IRPP_PTC_KEY_GEN_VAR_PS:  
    str = "IRPP_PTC_KEY_GEN_VAR_PS";     
    break;
  case SOC_PPC_FP_QUAL_KEY3_EVPN_BOS_EXPECTED:  
    str = "KEY3_EVPN_BOS_EXPECTED";     
    break;
  case SOC_PPC_FP_QUAL_KEY2_EVPN_BOS_EXPECTED:  
    str = "KEY2_EVPN_BOS_EXPECTED";     
    break;
  case SOC_PPC_FP_QUAL_KEY1_EVPN_BOS_EXPECTED:  
    str = "KEY1_EVPN_BOS_EXPECTED";     
    break;
  case SOC_PPC_FP_QUAL_KEY0_EVPN_BOS_EXPECTED:  
    str = "KEY0_EVPN_BOS_EXPECTED";     
    break;
  case SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH:
    str = "IPv6_dip_high";
    break;
  case SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW:
    str = "IPv6_dip_low";
    break;
  case SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH:
    str = "IPv6_sip_high";
    break;
  case SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW:
    str = "IPv6_sip_low";
    break;      
  case SOC_PPC_FP_QUAL_PORT_EXTENDER_ECID:
    str = "extender_channel_id";
    break;
  case SOC_PPC_FP_QUAL_GRE_PROTOCOL_TYPE:
    str = "GRE_protocol_type";
    break;
  case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_GRE_PARSED: 
      str = "is_ip_gre_tunnel";
      break;
  case SOC_PPC_FP_QUAL_IRPP_PRESEL_ID:
    str = "presel_id";
    break; 
  case SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE0:
    str = "presel_profile0";
    break;  
  case SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE1:
    str = "presel_profile1";
    break;  
  case SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE2:
    str = "presel_profile2";
    break;  
  case SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE3:
    str = "presel_profile3";
    break;  
  case SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE_KAPS:
    str = "presel_profile_kaps";
    break;       
  case SOC_PPC_FP_QUAL_CUSTOM_PP_HEADER_OUTPUT_FP:
    str = "customer tunnel 1";
    break;       
  case SOC_PPC_FP_QUAL_CUSTOM_PP_HEADER_OUTPUT_LSP_ECMP_FP:
    str = "customer tunnel 2";
    break;       
  case SOC_PPC_FP_QUAL_L2TP_TUNNEL:
	str = "l2tp_tunnel";
	break;
  case SOC_PPC_FP_QUAL_L2TP_SESSION:
	str = "l2tp_session";
	break;
  case SOC_PPC_FP_QUAL_L2TP_TUNNEL_SESSION:
	str = "l2tp_tunnel&session";
	break;
  case SOC_PPC_FP_QUAL_IPV6_COMPRESSED_SIP:
    str = "IPV6_COMPRESSED_SIP";
    break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FP_FWD_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FP_FWD_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FP_FWD_TYPE_BRIDGED:
    str = "bridged";
  break;
  case SOC_PPC_FP_FWD_TYPE_IPV4_UC:
    str = "ipv4_uc";
  break;
  case SOC_PPC_FP_FWD_TYPE_IPV4_MC:
    str = "ipv4_mc";
  break;
  case SOC_PPC_FP_FWD_TYPE_IPV6_UC:
    str = "ipv6_uc";
  break;
  case SOC_PPC_FP_FWD_TYPE_IPV6_MC:
    str = "ipv6_mc";
  break;
  case SOC_PPC_FP_FWD_TYPE_MPLS:
    str = "mpls";
  break;
  case SOC_PPC_FP_FWD_TYPE_TRILL:
    str = "trill";
  break;
  case SOC_PPC_FP_FWD_TYPE_BRIDGED_AFTER_TERM:
    str = "bridged_after_term";
  break;
  case SOC_PPC_FP_FWD_TYPE_CPU_TRAP:
    str = "cpu_trap";
  break;
  case SOC_PPC_FP_FWD_TYPE_TM:
    str = "cpu_tm";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FP_PROCESSING_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FP_PROCESSING_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FP_PROCESSING_TYPE_RAW:
    str = "raw";
  break;
  case SOC_PPC_FP_PROCESSING_TYPE_ETH:
    str = "eth";
  break;
  case SOC_PPC_FP_PROCESSING_TYPE_TM:
    str = "tm";
  break;
  case SOC_PPC_FP_PROCESSING_TYPE_FTMH:
    str = "ftmh";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FP_BASE_HEADER_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FP_BASE_HEADER_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_0:
    str = "header_0";
  break;
  case SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_1:
    str = "header_1";
  break;
  case SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_2:
    str = "header_2";
  break;
  case SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_3:
    str = "header_3";
  break;
  case SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_4:
    str = "header_4";
  break;
  case SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_5:
    str = "header_5";
  break;
  case SOC_PPC_FP_BASE_HEADER_TYPE_FWD:
    str = "fwd";
  break;
  case SOC_PPC_FP_BASE_HEADER_TYPE_FWD_POST:
    str = "fwd_post";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FP_ACTION_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FP_ACTION_TYPE_DEST:
    str = "dest";
  break;
    case SOC_PPC_FP_ACTION_TYPE_DEST_DROP:
    str = "dest_drop";
  break;
  case SOC_PPC_FP_ACTION_TYPE_DP:
    str = "dp";
  break;
  case SOC_PPC_FP_ACTION_TYPE_TC:
    str = "tc";
  break;
  case SOC_PPC_FP_ACTION_TYPE_TRAP:
    str = "trap";
  break;
  case SOC_PPC_FP_ACTION_TYPE_TRAP_REDUCED:
    str = "trap_reduced";
  break;
  case SOC_PPC_FP_ACTION_TYPE_SNP:
    str = "snp";
  break;
  case SOC_PPC_FP_ACTION_TYPE_MIRROR:
    str = "mirror";
  break;
  case SOC_PPC_FP_ACTION_TYPE_MIR_DIS:
    str = "mir_dis";
  break;
  case SOC_PPC_FP_ACTION_TYPE_EXC_SRC:
    str = "exc_src";
  break;
  case SOC_PPC_FP_ACTION_TYPE_IS:
    str = "is";
  break;
  case SOC_PPC_FP_ACTION_TYPE_METER:
    str = "meter";
  break;
  case SOC_PPC_FP_ACTION_TYPE_COUNTER:
    str = "counter";
  break;
  case SOC_PPC_FP_ACTION_TYPE_STAT:
    str = "stat";
  break;
  case SOC_PPC_FP_ACTION_TYPE_OUTLIF:
    str = "outlif";
  break;
  case SOC_PPC_FP_ACTION_TYPE_LAG_LB:
    str = "lag_lb";
  break;
  case SOC_PPC_FP_ACTION_TYPE_ECMP_LB:
    str = "ecmp_lb";
  break;
  case SOC_PPC_FP_ACTION_TYPE_STACK_RT_HIST:
    str = "stack_rt_hist";
  break;
  case SOC_PPC_FP_ACTION_TYPE_VSQ_PTR:
    str = "vsq_ptr";
  break;
  case SOC_PPC_FP_ACTION_TYPE_CHANGE_KEY:
    str = "change_key";
    break;
  case SOC_PPC_FP_ACTION_TYPE_NOP:
    str = "nop";
  break;
  case SOC_PPC_FP_ACTION_TYPE_EGR_TRAP:
    str = "egr_trap";
  break;
  case SOC_PPC_FP_ACTION_TYPE_EGR_OFP:
    str = "egr_ofp";
  break;
  case SOC_PPC_FP_ACTION_TYPE_EGR_TC_DP:
    str = "egr_tc_dp";
  break;
  case SOC_PPC_FP_ACTION_TYPE_EGR_OUTLIF:
    str = "egr_outlif";
  break;
  case SOC_PPC_FP_ACTION_TYPE_EEI:
    str = "eei";
    break;
  case SOC_PPC_FP_ACTION_TYPE_IN_PORT:
    str = "in_port";
    break;
  case SOC_PPC_FP_ACTION_TYPE_USER_PRIORITY:
    str = "user_priority";
    break;
  case SOC_PPC_FP_ACTION_TYPE_METER_B:
    str = "meter_b";
    break;
  case SOC_PPC_FP_ACTION_TYPE_COUNTER_B:
    str = "counter_b";
    break;
  case SOC_PPC_FP_ACTION_TYPE_DP_METER_COMMAND:
    str = "dp_meter_command";
    break;
  case SOC_PPC_FP_ACTION_TYPE_SRC_SYST_PORT:
    str = "src_syst_port";
    break;
  case SOC_PPC_FP_ACTION_TYPE_FWD_CODE:
    str = "fwd_code";
    break;
  case SOC_PPC_FP_ACTION_TYPE_FWD_OFFSET:
    str = "fwd_offset";
    break;
  case SOC_PPC_FP_ACTION_TYPE_BYTES_TO_REMOVE:
    str = "bytes_to_remove";
    break;
  case SOC_PPC_FP_ACTION_TYPE_SYSTEM_HEADER_PROFILE_ID:
    str = "system_header_profile_id";
    break;
  case SOC_PPC_FP_ACTION_TYPE_VSI:
    str = "vsi";
    break;
  case SOC_PPC_FP_ACTION_TYPE_ORIENTATION_IS_HUB:
    str = "orientation_is_hub";
    break;
  case SOC_PPC_FP_ACTION_TYPE_VLAN_EDIT_COMMAND:
    str = "vlan_edit_command";
    break;
  case SOC_PPC_FP_ACTION_TYPE_VLAN_EDIT_VID_1:
    str = "vlan_edit_vid_1";
    break;
  case SOC_PPC_FP_ACTION_TYPE_VLAN_EDIT_VID_2:
    str = "vlan_edit_vid_2";
    break;
  case SOC_PPC_FP_ACTION_TYPE_VLAN_EDIT_PCP_DEI:
    str = "vlan_edit_pcp_dei";
    break;
  case SOC_PPC_FP_ACTION_TYPE_IN_RIF:
    str = "in_rif";
    break;
  case SOC_PPC_FP_ACTION_TYPE_VRF:
    str = "vrf";
    break;
  case SOC_PPC_FP_ACTION_TYPE_IN_TTL:
    str = "in_ttl";
    break;
  case SOC_PPC_FP_ACTION_TYPE_IN_DSCP_EXP:
    str = "in_dscp_exp";
    break;
  case SOC_PPC_FP_ACTION_TYPE_RPF_DESTINATION_VALID:
    str = "rpf_destination_valid";
    break;
  case SOC_PPC_FP_ACTION_TYPE_RPF_DESTINATION:
    str = "rpf_destination";
    break;
  case SOC_PPC_FP_ACTION_TYPE_INGRESS_LEARN_ENABLE:
    str = "ingress_learn_enable";
    break;
  case SOC_PPC_FP_ACTION_TYPE_EGRESS_LEARN_ENABLE:
    str = "egress_learn_enable";
    break;
  case SOC_PPC_FP_ACTION_TYPE_LEARN_FID:
    str = "learn_fid";
    break;
  case SOC_PPC_FP_ACTION_TYPE_LEARN_SA_0_TO_15:
    str = "learn_sa_0_to_15";
    break;
  case SOC_PPC_FP_ACTION_TYPE_LEARN_SA_16_TO_47:
    str = "learn_sa_16_to_47";
    break;
  case SOC_PPC_FP_ACTION_TYPE_LEARN_DATA_0_TO_15:
    str = "learn_data_0_to_15";
    break;
  case SOC_PPC_FP_ACTION_TYPE_LEARN_DATA_16_TO_39:
    str = "learn_data_16_to_39";
    break;
  case SOC_PPC_FP_ACTION_TYPE_LEARN_OR_TRANSPLANT:
    str = "learn_or_transplant";
    break;
  case SOC_PPC_FP_ACTION_TYPE_IN_LIF:
    str = "in_lif";
    break;
  case SOC_PPC_FP_ACTION_TYPE_SEQUENCE_NUMBER_TAG:
    str = "sequence_number_tag";
    break;
  case SOC_PPC_FP_ACTION_TYPE_IGNORE_CP:
    str = "ignore_cp";
    break;
  case SOC_PPC_FP_ACTION_TYPE_PPH_TYPE:
    str = "pph_type";
    break;
  case SOC_PPC_FP_ACTION_TYPE_PACKET_IS_BOOTP_DHCP:
    str = "packet_is_bootp_dhcp";
    break;
  case SOC_PPC_FP_ACTION_TYPE_UNKNOWN_ADDR:
    str = "unknown_addr";
    break;
  case SOC_PPC_FP_ACTION_TYPE_FWD_HDR_ENCAPSULATION:
    str = "fwd_hdr_encapsulation";
    break;
  case SOC_PPC_FP_ACTION_TYPE_IEEE_1588:
    str = "ieee_1588";
    break;
  case SOC_PPC_FP_ACTION_TYPE_OAM:
    str = "oam";
    break;
  case SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1:
    str = "user_header_1";
    break;
  case SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2:
    str = "user_header_2";
    break;
  case SOC_PPC_FP_ACTION_TYPE_NATIVE_VSI:
    str = "native_vsi";
    break;
  case SOC_PPC_FP_ACTION_TYPE_IN_LIF_PROFILE:
    str = "in_lif_profile";
    break;
  case SOC_PPC_FP_ACTION_TYPE_IN_RIF_PROFILE:
    str = "in_rif_profile";
    break;
  case SOC_PPC_FP_ACTION_TYPE_COUNTER_AND_METER:
    str = "counter_and_meter";
    break;
  case SOC_PPC_FP_ACTION_TYPE_SNOOP_AND_TRAP:
    str = "snoop_and_trap";
    break;
  case SOC_PPC_FP_ACTION_TYPE_INVALID_NEXT:
    str = "invalid_next";
    break;
  case SOC_PPC_FP_ACTION_TYPE_COS_PROFILE:
    str = "cos_profile";
    break;
  case SOC_PPC_FP_ACTION_TYPE_ACE_TYPE:
    str = "ace_type";
    break;
  case SOC_PPC_FP_ACTION_TYPE_COUNTER_PROFILE:
    str = "counter_profile";
    break;
  case SOC_PPC_FP_ACTION_TYPE_ACE_POINTER:
    str = "ace_pointer";
    break;
  case SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0:
    str = "flp_action_0";
    break;
  case SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_1:
    str = "flp_action_1";
    break;
  case SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_2:
    str = "flp_action_2";
    break;
  case SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_3:
    str = "flp_action_3";
    break;
  case SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_4:
    str = "flp_action_4";
    break;
  case SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_5:
    str = "flp_action_5";
    break;
  case SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_6:
    str = "flp_action_7";
    break;
  case SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_7:
    str = "flp_action_6";
    break;
  case SOC_PPC_FP_ACTION_TYPE_FLP_LEM_1ST_RESULT_0:
    str = "update_flp_lem_0_MSB";
    break;
  case SOC_PPC_FP_ACTION_TYPE_FLP_LEM_1ST_RESULT_1:
    str = "update_flp_lem_0_LSB";
    break;
  case SOC_PPC_FP_ACTION_TYPE_FLP_LEM_2ND_RESULT_0:
    str = "update_flp_lem_1_MSB";
    break;
  case SOC_PPC_FP_ACTION_TYPE_FLP_LEM_2ND_RESULT_1:
    str = "update_flp_lem_1_LSB";
    break;
  case SOC_PPC_FP_ACTION_TYPE_SLB_HASH_VALUE:
    str = "slb_hash_value";
    break;
  case SOC_PPC_FP_ACTION_TYPE_USER_HEADER_3:
    str = "user_header_3";
    break;
  case SOC_PPC_FP_ACTION_TYPE_USER_HEADER_4:            
    str = "user_header_4";
    break;
  case SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1_TYPE:
    str = "user_header_1_type";
    break;
  case SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2_TYPE:       
    str = "user_header_2_type";
    break;
  case SOC_PPC_FP_ACTION_TYPE_USER_HEADER_3_TYPE:       
    str = "user_header_3_type";
    break;
  case SOC_PPC_FP_ACTION_TYPE_USER_HEADER_4_TYPE:       
    str = "user_header_4_type";
    break;
  case SOC_PPC_FP_ACTION_TYPE_ITPP_DELTA:
      str = "itpp_delta";
      break;
  case SOC_PPC_FP_ACTION_TYPE_STATISTICS_POINTER_0:
      str = "statistics_pointer_0";
      break;
  case SOC_PPC_FP_ACTION_TYPE_STATISTICS_POINTER_1:     
      str = "statistics_pointer_1";
      break;
  case SOC_PPC_FP_ACTION_TYPE_ADMIT_PROFILE:      
      str = "admit_profile";
      break;
  case SOC_PPC_FP_ACTION_TYPE_LATENCY_FLOW_ID:
      str = "latency_flow_id";
      break;
  case SOC_PPC_FP_ACTION_TYPE_PPH_RESERVE_VALUE:        
      str = "pph_reserve_value";
      break;
  case SOC_PPC_FP_ACTION_TYPE_PEM_CONTEXT:        
      str = "pem_context";
      break;
  case SOC_PPC_FP_ACTION_TYPE_PEM_GENERAL_DATA_0:
      str = "pem_general_data_0";
      break;
  case SOC_PPC_FP_ACTION_TYPE_PEM_GENERAL_DATA_1:       
      str = "pem_general_data_1";
      break;
  case SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_0:       
      str = "staggered_presel_result_0";
      break;
  case SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_1:       
      str = "staggered_presel_result_1";
      break;
  case SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_2:       
      str = "staggered_presel_result_2";
      break;
  case SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_3:       
      str = "staggered_presel_result_3";
      break;
  case SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_KAPS:       
      str = "staggered_presel_result_kaps";
      break;
 

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FP_DATABASE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FP_DB_TYPE_TCAM:
    str = "TCAM";
  break;
  case SOC_PPC_FP_DB_TYPE_DIRECT_TABLE:
    str = "DT"; 
  break;
  case SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION:
    str = "DE"; 
  break;
  case SOC_PPC_FP_DB_TYPE_EGRESS:
    str = "Egress";
  break;
  case SOC_PPC_FP_DB_TYPE_FLP:
    str = "FLP";
  break;
  case SOC_PPC_FP_DB_TYPE_SLB:
    str = "SLB";
  break;
  case SOC_PPC_FP_DB_TYPE_VT:
     str = "VT";
   break;
   case SOC_PPC_FP_DB_TYPE_TT:
     str = "TT";
   break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FP_DATABASE_STAGE_to_string(
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF:
    str = "ingress_pmf";
  break;
  case SOC_PPC_FP_DATABASE_STAGE_EGRESS:
    str = "egress";
  break;
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP:
    str = "ingress_flp";
  break;
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB:
    str = "ingress_slb";
  break;
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT:
    str = "ingress_vt";
  break;
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT:
    str = "ingress_tt";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FP_PREDEFINED_ACL_KEY_to_string(
    SOC_SAND_IN  SOC_PPC_FP_PREDEFINED_ACL_KEY  enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FP_PREDEFINED_ACL_KEY_L2:
    str = "l2";
  break;
  case SOC_PPC_FP_PREDEFINED_ACL_KEY_IPV4:
    str = "ipv4";
  break;
  case SOC_PPC_FP_PREDEFINED_ACL_KEY_IPV6:
    str = "ipv6";
  break;
  case SOC_PPC_FP_PREDEFINED_ACL_KEY_EGR_ETH:
    str = "egr_eth";
  break;
  case SOC_PPC_FP_PREDEFINED_ACL_KEY_EGR_IPV4:
    str = "egr_ipv4";
  break;
  case SOC_PPC_FP_PREDEFINED_ACL_KEY_EGR_MPLS:
    str = "egr_mpls";
  break;
  case SOC_PPC_FP_PREDEFINED_ACL_KEY_EGR_TM:
    str = "egr_tm";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FP_CONTROL_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FP_CONTROL_TYPE_L4OPS_RANGE:
    str = "l4ops_range";
  break;
  case SOC_PPC_FP_CONTROL_TYPE_PACKET_SIZE_RANGE:
    str = "packet_size_range";
  break;
  case SOC_PPC_FP_CONTROL_TYPE_ETHERTYPE:
    str = "ethertype";
  break;
  case SOC_PPC_FP_CONTROL_TYPE_NEXT_PROTOCOL_IP:
    str = "next_protocol_ip";
  break;
  case SOC_PPC_FP_CONTROL_TYPE_EGR_PP_PORT_DATA:
    str = "egr_pp_port_data";
  break;
  case SOC_PPC_FP_CONTROL_TYPE_EGR_TM_PORT_DATA:
    str = "egr_tm_port_data";
  break;
  case SOC_PPC_FP_CONTROL_TYPE_FLP_PP_PORT_DATA:
    str = "flp_pp_port_data";
  break;
  case SOC_PPC_FP_CONTROL_TYPE_ING_PP_PORT_DATA:
    str = "ing_pp_port_data";
  break;
  case SOC_PPC_FP_CONTROL_TYPE_ING_TM_PORT_DATA:
    str = "ing_tm_port_data";
  break;
  case SOC_PPC_FP_CONTROL_TYPE_EGR_L2_ETHERTYPES:
    str = "egr_l2_ethertypes";
    break;
  case SOC_PPC_FP_CONTROL_TYPE_EGR_IPV4_NEXT_PROTOCOL:
    str = "egr_ipv4_next_protocol";
  break;
  case SOC_PPC_FP_CONTROL_TYPE_PP_PORT_PROFILE:
    str = "pp_port_profile";
  break;
  case SOC_PPC_FP_CONTROL_TYPE_L2_L3_KEY_IN_LIF_ENABLE:
    str = "l2_l3_key_in_lif_enable";
  break;
  case SOC_PPC_FP_CONTROL_TYPE_L3_IPV6_TCP_CTL_ENABLE:
    str = "l3_ipv6_tcp_ctl_enable";
  break;
  case SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF:
    str = "hdr_user_def";
  break;
  case SOC_PPC_FP_CONTROL_TYPE_EGRESS_DP:
    str = "dp";
    break;
  case SOC_PPC_FP_CONTROL_TYPE_INNER_ETH_NOF_VLAN_TAGS:
    str = "inner_eth_nof_vlan_tags";
    break;
  case SOC_PPC_FP_CONTROL_TYPE_KEY_CHANGE_SIZE:
    str = "key_change_size";
    break;
  case SOC_PPC_FP_CONTROL_TYPE_IN_PORT_PROFILE:
    str = "in_port_profile";
    break;
  case SOC_PPC_FP_CONTROL_TYPE_OUT_PORT_PROFILE:
    str = "out_port_profile";
    break;
  case SOC_PPC_FP_CONTROL_TYPE_FLP_PGM_PROFILE:
    str = "flp_pgm_profile";
    break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_FP_PACKET_print(
    SOC_SAND_IN  SOC_PPC_FP_PACKET *info
  )
{
  uint32
    i;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  
  LOG_CLI((BSL_META_U(unit,
                      "Packet information:\n")));
  LOG_CLI((BSL_META_U(unit,
                      "Local Port Source: %d\n"), info->local_port_src));
  LOG_CLI((BSL_META_U(unit,
                      "Size in bytes: %d\n"), info->size_bytes));
  LOG_CLI((BSL_META_U(unit,
                      "Buffer (from MSB to LSB):\n")));
  for(i = 0; i < info->size_bytes; i += 32) {
    LOG_CLI((BSL_META_U(unit,
                        "0x%x\n"), info->buffer[i]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_PMF_PFG_INFO_print(
    SOC_SAND_IN  SOC_PPC_PMF_PFG_INFO *info
  )
{
    uint32
      ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "hdr_format_bmp: %x\n\r"),info->hdr_format_bmp));
  LOG_CLI((BSL_META_U(unit,
                      "vlan_tag_structure_bmp: %x\n\r"),info->vlan_tag_structure_bmp));
  LOG_CLI((BSL_META_U(unit,
                      "pp_ports_bmp[0]: %x\n\r"),info->pp_ports_bmp.arr[0]));
  LOG_CLI((BSL_META_U(unit,
                      "pp_ports_bmp[1]: %x\n\r"),info->pp_ports_bmp.arr[1]));
  LOG_CLI((BSL_META_U(unit,
                      "is_array_qualifier: %x\n\r"),info->is_array_qualifier));
  LOG_CLI((BSL_META_U(unit,
                      "stage: %s\n\r"),  SOC_PPC_FP_DATABASE_STAGE_to_string(info->stage)));

  for (ind = 0; ind < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX; ++ind)
  {
    if ((info->qual_vals[ind].type != SOC_PPC_NOF_FP_QUAL_TYPES) && (info->qual_vals[ind].type != BCM_FIELD_ENTRY_INVALID))
    {
      LOG_CLI((BSL_META_U(unit,
                          "qual_vals[%u]:"),ind));
      SOC_PPC_FP_QUAL_VAL_print(&(info->qual_vals[ind]));
    }
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_FP_QUAL_VAL_print(
    SOC_SAND_IN  SOC_PPC_FP_QUAL_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_FP_QUAL_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "val:")));
  soc_sand_u64_print(&(info->val), 0, 0);
  LOG_CLI((BSL_META_U(unit,
                      "valid_bits:")));
  soc_sand_u64_print(&(info->is_valid), 0, 0);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_DATABASE_INFO_print(
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Database type: %s\n\r"), SOC_PPC_FP_DATABASE_TYPE_to_string(info->db_type)));
  LOG_CLI((BSL_META_U(unit,
                      "Supported_pfgs: %u\n\r"),info->supported_pfgs));
  LOG_CLI((BSL_META_U(unit,
                      "Supported_pfgs - Arad: %x %x\n\r"),info->supported_pfgs_arad[0], info->supported_pfgs_arad[1]));

  LOG_CLI((BSL_META_U(unit,
                      "Qualifier types valid:\n\r")));
  for (ind = 0; ind < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++ind)
  {
    if (info->qual_types[ind] != BCM_FIELD_ENTRY_INVALID)
    {
      LOG_CLI((BSL_META_U(unit,
                          "qual_types[%u] %s "), ind,  SOC_PPC_FP_QUAL_TYPE_to_string(info->qual_types[ind])  ));
    }
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "Action types valid:\n\r")));
  for (ind = 0; ind < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++ind)
  {
    if (info->action_types[ind] != SOC_PPC_FP_ACTION_TYPE_INVALID)
    {
      LOG_CLI((BSL_META_U(unit,
                          "action_types[%u] %s "), ind,  SOC_PPC_FP_ACTION_TYPE_to_string(info->action_types[ind])  ));
    }
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "Strength: %u\n\r"),info->strength));
  LOG_CLI((BSL_META_U(unit,
                      "Cascaded coupled DB-Id: %u\n\r"),info->cascaded_coupled_db_id));
  LOG_CLI((BSL_META_U(unit,
                      "Use only small banks: %u\n\r"),info->flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_SMALL_BANKS));
  LOG_CLI((BSL_META_U(unit,
                      "No insertion order: %u\n\r"),info->flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_NO_INSERTION_PRIORITY_ORDER));
  LOG_CLI((BSL_META_U(unit,
                      "Sparse priorities: %u\n\r"),info->flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_SPARSE_PRIORITIES));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PPC_FP_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s, "), SOC_PPC_FP_ACTION_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "val: %u, "),info->val));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPC_FP_ENTRY_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++ind)
  {
    if ((info->qual_vals[ind].type != SOC_PPC_NOF_FP_QUAL_TYPES) && (info->qual_vals[ind].type != BCM_FIELD_ENTRY_INVALID))
    {
      LOG_CLI((BSL_META_U(unit,
                          "qual_vals[%u]:"),ind));
      SOC_PPC_FP_QUAL_VAL_print(&(info->qual_vals[ind]));
    }
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  for (ind = 0; ind < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++ind)
  {
    if (info->actions[ind].type != SOC_PPC_FP_ACTION_TYPE_INVALID)
    {
      LOG_CLI((BSL_META_U(unit,
                          "actions[%u]:"),ind));
      SOC_PPC_FP_ACTION_VAL_print(&(info->actions[ind]));
    }
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "priority: %u\n\r"),info->priority));
  LOG_CLI((BSL_META_U(unit,
                      "is_for_update: %u\n\r"),info->is_for_update));
  LOG_CLI((BSL_META_U(unit,
                      "is_invalid: %u\n\r"),info->is_invalid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_DIR_EXTR_ACTION_LOC_print(
    SOC_SAND_IN  SOC_PPC_FP_DIR_EXTR_ACTION_LOC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_FP_QUAL_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "fld_lsb: %u\n\r"),info->fld_lsb));
  LOG_CLI((BSL_META_U(unit,
                      "cst_val: %u\n\r"),info->cst_val));
  LOG_CLI((BSL_META_U(unit,
                      "nof_bits: %u\n\r"),info->nof_bits));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_DIR_EXTR_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PPC_FP_DIR_EXTR_ACTION_VAL *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_FP_ACTION_TYPE_to_string(info->type)));
  for (ind = 0; ind < info->nof_fields; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "fld_ext[%u]:"),ind));
    SOC_PPC_FP_DIR_EXTR_ACTION_LOC_print(&(info->fld_ext[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "nof_fields: %u\n\r"),info->nof_fields));
  LOG_CLI((BSL_META_U(unit,
                      "base_val: %u\n\r"),info->base_val));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO *info
  )
{
  uint32
    ind;
  int action_with_valid = 0;
  int field;
  int nof_qual = 0;
  uint32 fld_msb;
  const SOC_PPC_FP_DIR_EXTR_ACTION_LOC *fld_ext;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++ind)
  {
    if ((info->qual_vals[ind].type != SOC_PPC_NOF_FP_QUAL_TYPES) && (info->qual_vals[ind].type != BCM_FIELD_ENTRY_INVALID))
    {
      nof_qual++;
      if (soc_sand_u64_is_zero(&info->qual_vals[ind].is_valid)) {
        
        
        nof_qual--;
      }
      else {
        LOG_CLI((BSL_META_U(unit,
                            "\n\t        If (%s & 0x%X) == 0x%X then"), SOC_PPC_FP_QUAL_TYPE_to_string(info->qual_vals[ind].type),
                            info->qual_vals[ind].is_valid.arr[0], info->qual_vals[ind].val.arr[0]));
      }
    }
  }

  if (nof_qual == 0) {
      LOG_CLI((BSL_META_U(unit, "\n\t        ALWAYS")));
  }

  for (ind = 0; ind < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++ind)
  {
    if (info->actions[ind].type != SOC_PPC_FP_ACTION_TYPE_INVALID)
    {
      LOG_CLI((BSL_META_U(unit,
                          "\n\t          %s%s%s = {"), 
                          action_with_valid?"{":"",
                          SOC_PPC_FP_ACTION_TYPE_to_string(info->actions[ind].type),
                          action_with_valid?", valid}":""));
        
      for (field = info->actions[ind].nof_fields-1; field >= 0; --field)
      {
        fld_ext = &info->actions[ind].fld_ext[field];
        if (fld_ext->type == BCM_FIELD_ENTRY_INVALID) {
          
          LOG_CLI((BSL_META_U(unit,
                              "%s%u'h%x"), (field==(info->actions[ind].nof_fields-1))?"":",",
                              fld_ext->nof_bits, fld_ext->cst_val));
        }
        else {
            fld_msb = fld_ext->fld_lsb + fld_ext->nof_bits - 1;
            LOG_CLI((BSL_META_U(unit,
                                "%s%s[%u:%u]"), (field==(info->actions[ind].nof_fields-1))?"":",",
                                SOC_PPC_FP_QUAL_TYPE_to_string(fld_ext->type),
                                fld_msb, fld_ext->fld_lsb));
        }
      }

      LOG_CLI((BSL_META_U(unit, "}")));

      if (info->actions[ind].base_val != 0) {
        LOG_CLI((BSL_META_U(unit,
                            " + %u"), info->actions[ind].base_val));
      }
    }
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_CONTROL_INDEX_print(
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_INDEX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "db_id: %u\n\r"),info->db_id));
  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_FP_CONTROL_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "val_ndx: %u\n\r"),info->val_ndx));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_CONTROL_INFO_print(
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PPC_FP_NOF_CONTROL_VALS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "val[%u]: %u\n\r"),ind,info->val[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PPC_FP_DIAG_PP_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_FP_DIAG_PP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_PP_PORT_INFO_print(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_PARSER_print(
    int unit,
    SOC_SAND_IN  SOC_PPC_FP_PACKET_DIAG_PARSER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "  tm_port: %u, "),info->tm_port));
  LOG_CLI((BSL_META_U(unit,
                      "pp_port: %u, "),info->pp_port));
  LOG_CLI((BSL_META_U(unit,
                      "header_type: %s, "),SOC_TMC_PORT_HEADER_TYPE_to_string(info->header_type)));
  LOG_CLI((BSL_META_U(unit,
                      "hdr_format: %s, "),dpp_parser_pfc_string_by_hw(unit, info->pfc_hw)));
  LOG_CLI((BSL_META_U(unit,
                      "vlan_tag_structure: %s\n\r"),soc_sand_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_to_string(info->vlan_tag_structure)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_PGM_print_stage(
    SOC_SAND_IN  SOC_PPC_FP_PACKET_DIAG_PGM *info,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE   stage
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "  prog_sel_line_id: %u, "),info->pfg_id[stage]));
  LOG_CLI((BSL_META_U(unit,
                      "  pgm_id: %u, "),info->pgm_id[stage]));
  if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) {
      LOG_CLI((BSL_META_U(unit,
                          "pp_port_info:")));
      SOC_PPC_FP_DIAG_PP_PORT_INFO_print(&(info->pp_port_info));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_PGM_print(
    SOC_SAND_IN  SOC_PPC_FP_PACKET_DIAG_PGM *info
  )
{
    SOC_PPC_FP_DATABASE_STAGE stage;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (stage = 0; stage < SOC_PPC_NOF_FP_DATABASE_STAGES; stage ++) {
      SOC_PPC_FP_PACKET_DIAG_PGM_print_stage(info, stage);
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_FP_PACKET_DIAG_QUAL_print(
    SOC_SAND_IN  SOC_PPC_FP_PACKET_DIAG_QUAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->type != BCM_FIELD_ENTRY_INVALID)
  {
    LOG_CLI((BSL_META_U(unit,
                        "type: %s, "),SOC_PPC_FP_QUAL_TYPE_to_string(info->type)));
    LOG_CLI((BSL_META_U(unit,
                        "val[0]: 0x%x, "),info->val[0]));
    LOG_CLI((BSL_META_U(unit,
                        "val[1]: 0x%x\n\r"),info->val[1]));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "\n\r")));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_DB_QUAL_print(
    SOC_SAND_IN  uint32                  ind,
    SOC_SAND_IN  SOC_PPC_FP_PACKET_DIAG_DB_QUAL *info
  )
{
  uint32
    ind2;
  uint8
    is_to_print = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind2 = 0; ind2 < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++ind2)
  {
    if ((info->qual[ind2].type == SOC_PPC_NOF_FP_QUAL_TYPES) || (info->qual[ind2].type == BCM_FIELD_ENTRY_INVALID))
    {
      continue;
    }
    is_to_print = TRUE;
    break;
  }

  if (is_to_print == TRUE)
  {
    LOG_CLI((BSL_META_U(unit,
                        "db_id_quals[%u]:"),ind));
    LOG_CLI((BSL_META_U(unit,
                        "  db_id: %u\n\r"),info->db_id));
    for (ind2 = 0; ind2 < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++ind2)
    {
    if ((info->qual[ind2].type == SOC_PPC_NOF_FP_QUAL_TYPES) || (info->qual[ind2].type == BCM_FIELD_ENTRY_INVALID))
      {
        continue;
      }
      LOG_CLI((BSL_META_U(unit,
                          "    qual[%u]:"),ind2));
      SOC_PPC_FP_PACKET_DIAG_QUAL_print(&(info->qual[ind2]));
    }
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_KEY_print_stage(
    SOC_SAND_IN  SOC_PPC_FP_PACKET_DIAG_KEY *info,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE   stage
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PPC_FP_NOF_DBS; ++ind)
  {
    if (info->db_id_quals[ind].db_id == SOC_PPC_FP_NOF_DBS)
    {
      continue;
    }
    if (info->db_id_quals[ind].stage != stage)
    {
      continue;
    }
    SOC_PPC_FP_PACKET_DIAG_DB_QUAL_print(ind, &(info->db_id_quals[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_FP_PACKET_DIAG_KEY_print(
    SOC_SAND_IN  SOC_PPC_FP_PACKET_DIAG_KEY *info
  )
{
  SOC_PPC_FP_DATABASE_STAGE stage;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (stage = 0; stage < SOC_PPC_NOF_FP_DATABASE_STAGES; stage ++) {
      SOC_PPC_FP_PACKET_DIAG_KEY_print_stage(info, stage);
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_TCAM_DT_print(
    SOC_SAND_IN  SOC_PPC_FP_PACKET_DIAG_TCAM_DT *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "is_match: %u, "),info->is_match));
  LOG_CLI((BSL_META_U(unit,
                      "db_id: %u, "),info->db_id));
  for (ind = 0; ind < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++ind)
  {
    if (info->actions[ind].type == SOC_PPC_FP_ACTION_TYPE_INVALID)
    {
      continue;
    }
    SOC_PPC_FP_ACTION_VAL_print(&(info->actions[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_MACRO_print(
    SOC_SAND_IN  SOC_PPC_FP_PACKET_DIAG_MACRO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "db_id: %u, "),info->db_id));
  LOG_CLI((BSL_META_U(unit,
                      "entry_id: %u\n\r"),info->entry_id));
  for (ind = 0; ind < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++ind)
  {
    if (info->input_actions[ind].type == SOC_PPC_FP_ACTION_TYPE_INVALID)
    {
      continue;
    }
    LOG_CLI((BSL_META_U(unit,
                        "    Input actions[%u]: "), ind));
    SOC_PPC_FP_ACTION_VAL_print(&(info->input_actions[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "  Qual_mask:")));
  SOC_PPC_FP_PACKET_DIAG_QUAL_print(&(info->qual_mask));
  LOG_CLI((BSL_META_U(unit,
                      "  Action done:")));
  SOC_PPC_FP_ACTION_VAL_print(&(info->action));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_MACRO_SIMPLE_print(
    SOC_SAND_IN  SOC_PPC_FP_PACKET_DIAG_MACRO_SIMPLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "db_id: %u, "),info->db_id));
  LOG_CLI((BSL_META_U(unit,
                      "  Action done:")));
  SOC_PPC_FP_ACTION_VAL_print(&(info->action));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_ACTION_ELK_print(
    SOC_SAND_IN  SOC_PPC_FP_PACKET_DIAG_ACTION_ELK *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "hit: %u, "),info->hit));
  LOG_CLI((BSL_META_U(unit,
                      "  Action done:")));
  SOC_PPC_FP_ACTION_VAL_print(&(info->action));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_PACKET_DIAG_INFO_print(
    int unit,
    SOC_SAND_IN  SOC_PPC_FP_PACKET_DIAG_INFO *info
  )
{
  uint32
    ind2,
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->valid_stage[SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP]) {
      LOG_CLI((BSL_META_U(unit,
                          "INGRESS FLP (ELK): \n\r")));
      LOG_CLI((BSL_META_U(unit,
                          "Pgm:")));
      SOC_PPC_FP_PACKET_DIAG_PGM_print_stage(&(info->pgm), SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP);
      LOG_CLI((BSL_META_U(unit,
                          "Key:")));
      SOC_PPC_FP_PACKET_DIAG_KEY_print_stage(&(info->key), SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP);
      LOG_CLI((BSL_META_U(unit,
                          "ELK actions:")));
      for (ind = 0; ind < SOC_PPC_FP_NOF_ELK_ACTIONS; ++ind)
      {
        if (info->elk_action[ind].hit != 0)
        {
          SOC_PPC_FP_PACKET_DIAG_ACTION_ELK_print(&(info->elk_action[ind]));
        }
      }
      LOG_CLI((BSL_META_U(unit,
                          "\n\r")));
  }

  if (info->valid_stage[SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB]) {
      LOG_CLI((BSL_META_U(unit,
                          "INGRESS SLB (stateful load balancing): \n\r")));
      LOG_CLI((BSL_META_U(unit,
                          "Pgm:")));
      SOC_PPC_FP_PACKET_DIAG_PGM_print_stage(&(info->pgm), SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB);
      LOG_CLI((BSL_META_U(unit,
                          "Key:")));
      SOC_PPC_FP_PACKET_DIAG_KEY_print_stage(&(info->key), SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB);
      LOG_CLI((BSL_META_U(unit,
                          "\n\r")));
  }

  LOG_CLI((BSL_META_U(unit,
                      "INGRESS PMF: \n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "Parser:")));
  SOC_PPC_FP_PACKET_DIAG_PARSER_print(unit, &(info->parser));
  LOG_CLI((BSL_META_U(unit,
                      "Pgm:")));
  SOC_PPC_FP_PACKET_DIAG_PGM_print_stage(&(info->pgm), SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF);
  LOG_CLI((BSL_META_U(unit,
                      "Key:")));
  SOC_PPC_FP_PACKET_DIAG_KEY_print_stage(&(info->key), SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF);
  for (ind = 0; ind < SOC_PPC_FP_NOF_CYCLES; ++ind)
  {
    for (ind2 = 0; ind2 < SOC_PPC_FP_TCAM_NOF_BANKS; ++ind2)
    {
      if ((info->tcam[ind][ind2].is_match == FALSE) || (info->tcam[ind][ind2].db_id >= SOC_PPC_FP_NOF_DBS))
      {
        continue;
      }
      LOG_CLI((BSL_META_U(unit,
                          "TCAM[%u][%u]:"),ind, ind2));
      SOC_PPC_FP_PACKET_DIAG_TCAM_DT_print(&(info->tcam[ind][ind2]));
    }
  }
  for (ind = 0; ind < SOC_PPC_FP_NOF_CYCLES; ++ind)
  {
    if ((info->dt[ind].is_match == FALSE) || (info->dt[ind].db_id >= SOC_PPC_FP_NOF_DBS))
    {
      continue;
    }
    LOG_CLI((BSL_META_U(unit,
                        "DirectTable[%u]:"), ind));
    SOC_PPC_FP_PACKET_DIAG_TCAM_DT_print(&(info->dt[ind]));
  }
  for (ind = 0; ind < SOC_PPC_FP_NOF_CYCLES; ++ind)
  {
      for (ind2 = 0; ind2 < SOC_PPC_FP_NOF_MACRO_SIMPLES; ++ind2)
      {
        if (info->macro_simple[ind][ind2].action.type == SOC_PPC_FP_ACTION_TYPE_INVALID)
        {
          continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "Macro simple[%u][%u]:"),ind, ind2));
        SOC_PPC_FP_PACKET_DIAG_MACRO_SIMPLE_print(&(info->macro_simple[ind][ind2]));
        LOG_CLI((BSL_META_U(unit,
                            "\n\r")));
      }

    for (ind2 = 0; ind2 < SOC_PPC_FP_NOF_MACROS; ++ind2)
    {
      if (info->macro[ind][ind2].action.type == SOC_PPC_FP_ACTION_TYPE_INVALID)
      {
        continue;
      }
      LOG_CLI((BSL_META_U(unit,
                          "Macro[%u][%u]:"),ind, ind2));
      SOC_PPC_FP_PACKET_DIAG_MACRO_print(&(info->macro[ind][ind2]));
      LOG_CLI((BSL_META_U(unit,
                          "\n\r")));
    }
  }

  LOG_CLI((BSL_META_U(unit,
                      "EGRESS: \n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "Pgm:")));
  SOC_PPC_FP_PACKET_DIAG_PGM_print_stage(&(info->pgm), SOC_PPC_FP_DATABASE_STAGE_EGRESS);
  LOG_CLI((BSL_META_U(unit,
                      "Key:")));
  SOC_PPC_FP_PACKET_DIAG_KEY_print_stage(&(info->key), SOC_PPC_FP_DATABASE_STAGE_EGRESS);
  LOG_CLI((BSL_META_U(unit,
                      "Egress actions:")));
  for (ind = 0; ind < SOC_DPP_DEFS_MAX(NOF_EGRESS_PMF_ACTIONS); ++ind)
  {
    if (info->egress_action[ind].type != SOC_PPC_FP_ACTION_TYPE_INVALID)
    {
      SOC_PPC_FP_ACTION_VAL_print(&(info->egress_action[ind]));
    }
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));


exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_KEY_LSB_MSB_print(
	SOC_SAND_IN SOC_PPC_FP_RESOURCE_KEY_LSB_MSB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "msb=%u,lsb=%u"), info->msb, info->lsb));
	
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_KEY_print(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_KEY *info
  )
{
  uint32
      lost_bits;
  char
      qual_name[128];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  lost_bits = ((info->key_loc.msb - info->key_loc.lsb) - (info->qual_loc.msb - info->qual_loc.lsb));

  if(info->valid)
  {
      LOG_CLI((BSL_META_U(unit,
                          "\t%-10s"), info->is_second_key ? "Yes" : "No"));
      LOG_CLI((BSL_META_U(unit,
                          "%-10u"), info->key_loc.msb));
      LOG_CLI((BSL_META_U(unit,
                          "%-10u"), info->key_loc.lsb));
      LOG_CLI((BSL_META_U(unit,
                          "%-10u"), lost_bits));
      LOG_CLI((BSL_META_U(unit,
                          "%-10u"), info->qual_loc.msb));
      LOG_CLI((BSL_META_U(unit,
                          "%-10u"), info->qual_loc.lsb));
      if (SOC_PPC_FP_IS_QUAL_TYPE_USER_DEFINED(info->qual_type)) {
          qual_name[0] = '\0';
          _bcm_dpp_field_dump_single_qual_name(unit, info->qual_type, qual_name, sizeof(qual_name));
          LOG_CLI((BSL_META_U(unit,
                              "%s\n"), qual_name));
      } else {
          LOG_CLI((BSL_META_U(unit,
                              "%s\n"), SOC_PPC_FP_QUAL_TYPE_to_string(info->qual_type)));
      }
  }
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_DB_BANK_print(
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_DB_BANK *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    LOG_CLI((BSL_META_U(unit,
                        "%u\t\t"), info->entries_used));
    LOG_CLI((BSL_META_U(unit,
                        "%u\t\t"), info->entries_free));
    LOG_CLI((BSL_META_U(unit,
                        "0x%x\n"), info->action_tbl_bmp));

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_EGRESS_ACTION_print(
    int unit,
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_ACTION *info
  )
{
    uint32  action_lsb;
    uint32  action_size;
    int     action_valid;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    if(info->valid)
    {
        LOG_CLI((BSL_META_U(unit, "%-30s"), SOC_PPC_FP_ACTION_TYPE_to_string(info->action_type)));

        arad_egress_pmf_db_action_get_unsafe(unit, info->action_type, &action_lsb, &action_size, &action_valid);

        LOG_CLI((BSL_META_U(unit, "msb=%u,lsb=%u"), action_lsb + action_size -1, action_lsb));

        if(action_valid != -1)  {
          LOG_CLI((BSL_META_U(unit, ",valid=%u"), action_valid));
        }
        LOG_CLI((BSL_META_U(unit, "\n\r")));
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_ACTION_print(
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_ACTION *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    if(info->valid)
    {
        LOG_CLI((BSL_META_U(unit,
                            "%-30s"), SOC_PPC_FP_ACTION_TYPE_to_string(info->action_type)));

        SOC_PPC_FP_RESOURCE_KEY_LSB_MSB_print(&(info->action_loc));
        if(info->action_loc.lsb > 0)
        {
          LOG_CLI((BSL_META_U(unit,
                              ",valid=%u"), info->action_loc.lsb - 1));
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n\r")));
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PPC_FP_RESOURCE_DB_TCAM_print(
    SOC_SAND_IN int                         unit,
    SOC_PPC_FP_DATABASE_TYPE                type,
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_DB_TCAM *info
  )
{
    uint32 
        idx,
        idx_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    LOG_CLI((BSL_META_U(unit,
                        "\n\tTCAM DB: ")));
    LOG_CLI((BSL_META_U(unit,
                        "prefix_val = %u, "), info->prefix_val));
    LOG_CLI((BSL_META_U(unit,
                        "prefix_len = %u, "), info->prefix_len));

    for(idx = 0; (idx < info->nof_keys_per_db) && (idx < SOC_PPC_FP_KEY_NOF_KEYS_PER_DB_MAX); idx++)
    {
        LOG_CLI((BSL_META_U(unit,
                            "access_profile_id[%u] = %u  "), idx ,info->access_profile_id[idx]));
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n\n\tBank-ID\t")));
    LOG_CLI((BSL_META_U(unit,
                        "Entries-Used\t")));
    LOG_CLI((BSL_META_U(unit,
                        "Entries-Free\t")));
    LOG_CLI((BSL_META_U(unit,
                        "Action-Tbl-BMP\n\r")));
    LOG_CLI((BSL_META_U(unit,
                        "\t------------------------------------------------------\n\r")));

    for(idx = 0; idx < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); idx++)
    {
        if(info->bank[idx].valid)
        {
            idx_id = idx;
            if(info->nof_keys_per_db == SOC_PPC_FP_KEY_NOF_KEYS_PER_DB_MAX && 
               (idx % 2) == 0)
            {
                LOG_CLI((BSL_META_U(unit,
                                    "\t%u\\%u\t"), idx, idx + 1));
                idx++;
            }
            else
            {
                LOG_CLI((BSL_META_U(unit,
                                    "\t%u\t"), idx));
            }
            SOC_PPC_FP_RESOURCE_DB_BANK_print(&(info->bank[idx_id]));
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n\n\t%-10s"),"Action"));
    LOG_CLI((BSL_META_U(unit,
                        "%-30s"),"Action-Type"));
    LOG_CLI((BSL_META_U(unit,
                        "%s\n\r"),"Action-Location"));
    LOG_CLI((BSL_META_U(unit,
                        "\t-------------------------------------------------------------\n\r")));

    for(idx = 0; idx < info->nof_actions && idx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; idx++)
    {
            LOG_CLI((BSL_META_U(unit,
                                "\t%-10u"), idx));
            if(type == SOC_PPC_FP_DB_TYPE_EGRESS)
                SOC_PPC_FP_RESOURCE_EGRESS_ACTION_print(unit, &(info->action[idx]));
            else
                SOC_PPC_FP_RESOURCE_ACTION_print(&(info->action[idx]));
    }
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_DB_DE_print(
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_DB_DE *info
  )
{
    uint32 pgm_idx;
    uint32 cycle;
    uint32 idx;

    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    LOG_CLI((BSL_META_U(unit, "\n\tDB Direct Extraction:")));

    for (pgm_idx = 0; pgm_idx < SOC_PPC_FP_NOF_FEM_PROGS_MAX; pgm_idx++) {
        for(cycle = 0; cycle < SOC_PPC_FP_NOF_CYCLES; cycle++)
        {
            for(idx = 0; idx < SOC_PPC_FP_NOF_MACROS; idx++)
            {
                if(info->valid[pgm_idx][cycle][idx])
                {
                    LOG_CLI((BSL_META_U(unit, "\n\t    FEM pgm %u Cycle %u FEM ID %u Entry %u Pri %u"), 
                                pgm_idx, cycle, idx, info->de_entry_id[pgm_idx][cycle][idx],
                                info->de_entry[pgm_idx][cycle][idx].priority));
                    SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_print(&(info->de_entry[pgm_idx][cycle][idx]));
                }
            }
	    }
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_DB_print(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_DB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if(info->valid)
  {
    int32 
        idx,
        key_idx;

    LOG_CLI((BSL_META_U(unit,
                        "\tType = %s, "), SOC_PPC_FP_DATABASE_TYPE_to_string(info->type)));
    LOG_CLI((BSL_META_U(unit,
                        "Stage = %s, "), SOC_PPC_FP_DATABASE_STAGE_to_string(info->stage)));
    LOG_CLI((BSL_META_U(unit,
                        "Priority = %d, "), info->db_priority));
    LOG_CLI((BSL_META_U(unit,
                        "Key Size = %u, "), info->key_size));
    LOG_CLI((BSL_META_U(unit,
                        "NOF CEs = %u\n"), info->nof_ces));
    if(info->nof_ces)
    {
      LOG_CLI((BSL_META_U(unit,
                          "\tKey:\n")));
      LOG_CLI((BSL_META_U(unit,
                          "\t%-10s"),"Second"));
      LOG_CLI((BSL_META_U(unit,
                          "%-10s"),"Key msb"));
      LOG_CLI((BSL_META_U(unit,
                          "%-10s"),"Key lsb"));
      LOG_CLI((BSL_META_U(unit,
                          "%-10s"),"Lost Bits"));
      LOG_CLI((BSL_META_U(unit,
                          "%-10s"),"Qual msb"));
      LOG_CLI((BSL_META_U(unit,
                          "%-10s"),"Qual lsb"));
      LOG_CLI((BSL_META_U(unit,
                          "%-15s\n"),"Qual Type"));
      LOG_CLI((BSL_META_U(unit,
                          "\t----------------------------------------------------------------------------\n\r")));

      for(key_idx = 0; key_idx < SOC_PPC_FP_KEY_NOF_KEYS_PER_DB_MAX; key_idx++)
      {
          for(idx = 0; idx < info->nof_ces; idx++)
          {
            if((key_idx == 0 && !info->key_qual[idx].is_second_key) || 
               (key_idx == 1 && info->key_qual[idx].is_second_key))
            {
                SOC_PPC_FP_RESOURCE_KEY_print(unit, &(info->key_qual[idx]));
            }
          }
      }
    }

    if((SOC_PPC_FP_DB_TYPE_TCAM == info->type) 
       || (SOC_PPC_FP_DB_TYPE_FLP == info->type) 
       || (SOC_PPC_FP_DB_TYPE_EGRESS == info->type) 
       || (SOC_PPC_FP_DB_TYPE_DIRECT_TABLE == info->type))
    {
      SOC_PPC_FP_RESOURCE_DB_TCAM_print(unit, info->type, &(info->db_tcam));
    }
    else if(SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION == info->type)
    {
      SOC_PPC_FP_RESOURCE_DB_DE_print(&(info->db_de));
    }
  }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_DB_PREFIX_print(
    SOC_SAND_IN SOC_TMC_TCAM_DB_PREFIX *info
  )
{
  int32 idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for(idx = (info->nof_bits - 1); idx >= 0; idx--)
  {
	LOG_CLI((BSL_META_U(unit,
                            "%u"), info->val & (1 << (idx)) ));
  }
  for(idx = (4  - info->nof_bits); idx > 0; idx--)
  {
	LOG_CLI((BSL_META_U(unit,
                            "x")));
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n")));

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_BANK_DB_print(
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_BANK_DB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "%-10u"), info->db_id));
  LOG_CLI((BSL_META_U(unit,
                      "%-15u"), info->nof_entries));

  SOC_PPC_FP_DB_PREFIX_print(&(info->prefix));
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_BANK_ALL_DBS_print(
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_BANK *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

	if(info->is_used && (info->nof_dbs > 0))
	{
		uint32 
			idx;
		uint8 need_indent = 0;

		for(idx = 0; idx < info->nof_dbs && idx < SOC_PPC_FP_MAX_NOF_DBS_PER_BANK; idx++)
		{
			if(need_indent)
			{
				LOG_CLI((BSL_META_U(unit,
                                                    "          ")));
			}
			SOC_PPC_FP_RESOURCE_BANK_DB_print(&(info->db[idx]));
			need_indent = 1;
		}
	}
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_BANK_print(
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_BANK *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

	if(info->is_used)
	{
		LOG_CLI((BSL_META_U(unit,
                                    "%-16s"), SOC_TMC_TCAM_BANK_ENTRY_SIZE_ARAD_to_string(info->entry_size)));
		LOG_CLI((BSL_META_U(unit,
                                    "%-15u"), info->entries_free));
		LOG_CLI((BSL_META_U(unit,
                                    "%-10u"), info->nof_dbs));
		LOG_CLI((BSL_META_U(unit,
                                    "%s\n"), SOC_TMC_TCAM_BANK_OWNER_to_string(info->owner)));
	}
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_PRESEL_print(
    SOC_SAND_IN int                        unit,
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_PRESEL *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    if(info->is_valid)
    {
        int32
            idx;
        char
            presel_name[128];

        LOG_CLI((BSL_META_U(unit,
                            "%-10u"), info->pmf_pgm));

        for (idx = 0; idx < SOC_PPC_FP_MAX_SELECT_LINE; idx++) {
            if (SHR_BITGET(info->presel_bmp, idx)) {
                presel_name[0] = '\0';
                _bcm_dpp_field_dump_single_presel_name(unit, idx, presel_name, sizeof(presel_name));
                LOG_CLI((BSL_META_U(unit,
                                    "%-100s"), presel_name));
                break;
            }
        }

        if (idx == SOC_PPC_FP_MAX_SELECT_LINE) {
            LOG_CLI((BSL_META_U(unit, "%-100s"), "--"));
        }

        for(idx = (SOC_PPC_FP_NOF_PS_LINES_IN_LONGS - 1); idx >= 0; idx--)
        {
            LOG_CLI((BSL_META_U(unit,
                                "0x%-8x "), info->presel_bmp[idx]));
        }
        LOG_CLI((BSL_META_U(unit,
                            "   ")));

        for(idx = (SOC_PPC_FP_NOF_DBS_IN_LONGS - 1); idx >= 0 ; idx--)
        {
            LOG_CLI((BSL_META_U(unit,
                                "0x%-8x "), info->db_bmp[idx]));
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_DIAG_ERROR_PARAM_print(
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_DIAG_ERROR_TYPE err_type,
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE param_type,
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_DIAG_ERROR_PARAM *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

  switch(err_type)
  {
  case SOC_PPC_FP_RESOURCE_DIAG_ERROR_TYPE_TCAM_BANK:
    {
      switch(param_type)
      {
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_BANK_VALID:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: bank_valid\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   Bank-ID = %d, SW-valid = %d\n"), info->value[0], info->value[1]));
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_BANK_ENTRY_SIZE:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: bank_entry_size\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   Bank-ID = %d, TCAM-DB-Profile-0 = %d, Entry-Size-0 = %d, TCAM-DB-Profile-1 = %d, Entry-Size-1 = %d\n"), 
                   info->value[0], info->value[1], info->value[2], info->value[3], info->value[4]));
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_BANK_ENTRY_VALID:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: bank_entry_valid\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   TCAM-DB-ID = %d, DB-ID = %d, Profile-ID = %d, Bank-ID = %d, Entry-HW-Line = %d, "
                   "Entry-HW-Valid = %d, Entry-SW-Valid = %d\n"), 
                   info->value[0], info->value[1], info->value[2], info->value[3], info->value[4], info->value[5], info->value[6]));
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_BANK_NOF_ENTRIES:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: bank_nof_entries\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   Bank-ID = %d, SW-Nof-entries = %d, HW-Nof-Entries = %d\n"), 
                   info->value[0], info->value[1], info->value[2]));
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_BANK_DBS:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: bank_nof_dbs\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   Bank-ID = %d, DB-Not-Found = %d\n"), 
                   info->value[0], info->value[1]));
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_BANK_PREFIX_DB:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: bank_prefix_db\n")));
          if(info->value[2] < SOC_PPC_FP_NOF_DBS && info->value[5] < SOC_PPC_FP_NOF_DBS)
          {
            LOG_CLI((BSL_META_U(unit,
                                "   Bank-ID = %d, TCAM-DB-0 = %d, DB-0 = %d, Prefix-0 = %d, TCAM-DB-1 = %d, DB-1 = %d, Prefix-1 = %d\n"), 
                     info->value[0], info->value[1], info->value[2], info->value[3], info->value[4], info->value[5], info->value[6]));
          }
          else
          {
            LOG_CLI((BSL_META_U(unit,
                                "   Bank-ID = %d, TCAM-DB-0 = %d, Prefix-0 = %d, TCAM-DB-1 = %d, Prefix-1 = %d\n"), 
                     info->value[0], info->value[1], info->value[3], info->value[4], info->value[6]));
          }
        } break;
      default:
        break;
      }
    } break;

  case SOC_PPC_FP_RESOURCE_DIAG_ERROR_TYPE_TCAM_DB:
    {
      switch(param_type)
      {
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_TCAM_DB_UNIQUE_PROFILE:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: tcam_db_unique_profile\n")));
          if(info->value[2] < SOC_PPC_FP_NOF_DBS && info->value[4] < SOC_PPC_FP_NOF_DBS)
          {
            LOG_CLI((BSL_META_U(unit,
                                "   TCAM-DB-Profile = %d, TCAM-DB-0 = %d, DB-0 = %d, TCAM-DB-1 = %d, DB-1 = %d\n"), 
                     info->value[0], info->value[1], info->value[2], info->value[3], info->value[4]));
          }
          else
          {
            LOG_CLI((BSL_META_U(unit,
                                "   TCAM-DB-Profile = %d, TCAM-DB-0 = %d, TCAM-DB-1 = %d\n"), 
                     info->value[0], info->value[1], info->value[3]));
          }
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_TCAM_DB_PREFIX:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: tcam_db_prefix\n")));
          if(info->value[1] < SOC_PPC_FP_NOF_DBS)
          {
            LOG_CLI((BSL_META_U(unit,
                                "   TCAM-DB-ID = %d, DB-ID = %d, HW-DB-Profile = %d, SW-Value = %d, SW-Length = %d, "
                     "HW-Value = %d, HW-Length = %d\n"), 
                     info->value[0], info->value[1], info->value[2], info->value[3], info->value[4], info->value[5], info->value[6]));
          }
          else
          {
            LOG_CLI((BSL_META_U(unit,
                                "   TCAM-DB-ID = %d, HW-DB-Profile = %d, SW-Value = %d, SW-Length = %d, "
                     "HW-Value = %d, HW-Length = %d\n"), 
                     info->value[0], info->value[2], info->value[3], info->value[4], info->value[5], info->value[6]));
          }
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_TCAM_DB_ENTRY_SIZE:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: tcam_db_entry_size\n")));
          if(info->value[1] < SOC_PPC_FP_NOF_DBS)
          {
            LOG_CLI((BSL_META_U(unit,
                                "   TCAM-DB-ID = %d, DB-ID = %d, HW-DB-Profile = %d, Bank-ID = %d, SW-Value = %d, HW-Value = %d\n"), 
                     info->value[0], info->value[1], info->value[2], info->value[3], info->value[4], info->value[5]));
          }
          else
          {
            LOG_CLI((BSL_META_U(unit,
                                "   TCAM-DB-ID = %d, HW-DB-Profile = %d, Bank-ID = %d, SW-Value = %d, HW-Value = %d\n"), 
                     info->value[0], info->value[2], info->value[3], info->value[4], info->value[5]));
          }
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_TCAM_DB_BANKS:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: tcam_db_entry_banks\n")));
          if(info->value[1] < SOC_PPC_FP_NOF_DBS)
          {
            LOG_CLI((BSL_META_U(unit,
                                "   TCAM-DB-ID = %d, DB-ID = %d, Bank-ID = %d, HW-DB-Profile = %d, SW-Value = %d, HW-Value = %d\n"), 
                     info->value[0], info->value[1], info->value[2], info->value[3], info->value[4], info->value[5]));
          }
          else
          {
            LOG_CLI((BSL_META_U(unit,
                                "   TCAM-DB-ID = %d, Bank-ID = %d, HW-DB-Profile = %d, SW-Value = %d, HW-Value = %d\n"), 
                     info->value[0], info->value[2], info->value[3], info->value[4], info->value[5]));
          }
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_TCAM_DB_ACTION_BITMAP:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: tcam_db_action_bitmap\n")));
          if(info->value[1] < SOC_PPC_FP_NOF_DBS)
          {
            LOG_CLI((BSL_META_U(unit,
                                "   TCAM-DB-ID = %d, DB-ID = %d, HW-DB-Profile = %d, Bank-ID = %d, SW-Value = 0x%x, HW-Value = 0x%x\n"), 
                     info->value[0], info->value[1], info->value[2], info->value[3], info->value[4], info->value[5]));
          }
          else
          {
            LOG_CLI((BSL_META_U(unit,
                                "   TCAM-DB-ID = %d, HW-DB-Profile = %d, SW-Value = %d, HW-Value = %d\n"), 
                     info->value[0], info->value[2], info->value[3], info->value[4]));
          }
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_TCAM_DB_PRIORITY:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: tcam_db_priority\n")));
          if(info->value[1] < SOC_PPC_FP_NOF_DBS)
          {
            LOG_CLI((BSL_META_U(unit,
                                "   TCAM-DB-ID = %d, DB-ID = %d, Entry-0 = %d, Entry-1 = %d, Entry-Bank-0 = %d, Entry-Bank-1 = %d, "
                     "Entry-HW-Line-0 = %d, Entry-HW-Line-1 = %d, Entry-Priority-0 = %d, Entry-Priority-1 = %d\n"), 
                     info->value[0], info->value[1], info->value[2], info->value[3], info->value[4], info->value[5], 
                     info->value[6], info->value[7], info->value[8], info->value[9]));
          }
          else
          {
            LOG_CLI((BSL_META_U(unit,
                                "   TCAM-DB-ID = %d, Entry-0 = %d, Entry-1 = %d, Entry-Bank-0 = %d, Entry-Bank-1 = %d, "
                     "Entry-HW-Line-0 = %d, Entry-HW-Line-1 = %d, Entry-Priority-0 = %d, Entry-Priority-1 = %d\n"), 
                     info->value[0], info->value[2], info->value[3], info->value[4], info->value[5], 
                     info->value[6], info->value[7], info->value[8], info->value[9]));
          }
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_TCAM_DB_ENTRY_VALID:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: tcam_db_valid\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   DB-ID = %d, Entry-ID = %d, SW-Entry-Valid = %d, HW-Entry-Valid = %d, SW-Prefix = %d, HW-Prefix = %d\n"), 
                   info->value[0], info->value[1], info->value[2], info->value[3], info->value[4], info->value[5]));
        } break;
      default:
        break;
      }
    } break;

  case SOC_PPC_FP_RESOURCE_DIAG_ERROR_TYPE_KEY:
    {
      switch(param_type)
      {
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_KEY_COHERENCY:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: key_coherency\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   Program = %d, Key-ID = %d, Cycle-ID = %d, CE-ID-0 = %d, CE-ID-1 = %d, DB-ID = %d, TCAM-DB-ID = %d\n"), 
                   info->value[0], info->value[1], info->value[2], info->value[3], info->value[4], info->value[5], info->value[6]));
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_KEY_SW_HW:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: key_sw_hw\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   Program = %d, Qual-Type = %s, Key-ID = %d, Cycle-ID = %d, CE-ID = %d, SW-qual-lsb = %d, HW-lsb = %d, SW-qual-msb = %d, HW-Length = %d\n"), 
                   info->value[0], SOC_PPC_FP_QUAL_TYPE_to_string(info->value[1]), info->value[2], info->value[3], info->value[4], info->value[5], info->value[6], info->value[7], info->value[8]));
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_KEY_CE_USED:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: key_ce_used\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   Program = %d, Key-ID = %d, Cycle-ID = %d, CE-ID-0 = %d, SW-is-used = %d, HW-is-used = %d\n"), 
                   info->value[0], info->value[1], info->value[2], info->value[3], info->value[4], info->value[5]));
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_KEY_CE_BITMAP:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: key_ce_bitmap\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   Stage = %s, Program = %d, Cycle = %d, CE-Bitmap-0 = 0x%x, CE-Bitmap-1 = 0x%x\n"), 
                   SOC_PPC_FP_DATABASE_STAGE_to_string(info->value[0]), info->value[1], info->value[2], info->value[3], info->value[4]));
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_KEY_KEY_BITMAP:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: key_key_bitmap\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   Stage = %s, Program = %d, Cycle = %d, Key-Bitmap-0 = 0x%x, Key-Bitmap-1 = 0x%x\n"), 
                   SOC_PPC_FP_DATABASE_STAGE_to_string(info->value[0]), info->value[1], info->value[2], info->value[3], info->value[4]));
        } break;
      default:
        break;
      }
    } break;

  case SOC_PPC_FP_RESOURCE_DIAG_ERROR_TYPE_ACTION:
    {
      switch(param_type)
      {
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_ACTION_FEM:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: action_fem\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   Program = %d, FEM-ID = %d, DB-ID = %d, TCAM-DB-ID = %d, HW-Action-Type = %s, SW-Action-Type = %s\n"), 
                   info->value[0], info->value[1], info->value[2], info->value[3], SOC_PPC_FP_ACTION_TYPE_to_string(info->value[4]), 
                   SOC_PPC_FP_ACTION_TYPE_to_string(info->value[5])));
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_ACTION_FES:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: action_fes\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   Program = %d, FES-ID = %d, DB-ID = %d, TCAM-DB-ID = %d, HW-Action-Type = %s, SW-Action-Type = %s, "
                   "HW-LSB = %d, SW-LSB = %d\n"), info->value[0], info->value[1], info->value[2], info->value[3], 
                   SOC_PPC_FP_ACTION_TYPE_to_string(info->value[4]), SOC_PPC_FP_ACTION_TYPE_to_string(info->value[5]), info->value[6], info->value[7]));
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_ACTION_FEM_BASE:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: action_fem_base\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   Program = %d, FES-ID = %d, DB-ID = %d, TCAM-DB-ID = %d, HW-Base-Value = %d, SW-Base-Value = %d\n"), 
                   info->value[0], info->value[1], info->value[2], info->value[3], info->value[4], info->value[5]));
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_ACTION_PRIORITY:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: action_priority\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   Program = %d, Action-Type = %s, DB-ID-0 = %d, TCAM-DB-ID-0 = %d, DB-ID-1 = %d, "
                   "TCAM-DB-ID-1 = %d, is-fes-0 = %d, is-fes-1 = %d, fes/fem-id-0 = %d, fes/fem-id-1 = %d, "
                              "Priority-0 = %d, Priority-1 = %d, Entry-ID-0 = %d, Entry-ID-1 = %d\n"), 
                   info->value[0], SOC_PPC_FP_ACTION_TYPE_to_string(info->value[1]), info->value[2], info->value[3], info->value[4], info->value[5], info->value[6], 
                   info->value[7], info->value[8], info->value[9], info->value[10], info->value[11], info->value[12], info->value[13]));
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_ACTION_FES_BITMAP:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: action_fes_bitmap\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   Stage = %s, Program = %d, FES-Bitmap-0 = 0x%x, FES-Bitmap-1 = 0x%x\n"), 
                   SOC_PPC_FP_DATABASE_STAGE_to_string(info->value[0]), info->value[1], info->value[2], info->value[3]));
        } break;
      default:
        break;
      }
    } break;

  case SOC_PPC_FP_RESOURCE_DIAG_ERROR_TYPE_PRESEL:
    {
      switch(param_type)
      {
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_PRESEL_LINE_VALID:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: presel_line_valid\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   HW-Line = %d, SW-Valid = %d, HW-Valid = %d\n"), 
                   info->value[0], info->value[1], info->value[2]));
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_PRESEL_PGM_VALID:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: presel_pgm_valid\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   Program = %d, SW-Valid = %d, HW-Valid = %d, HW-Line-Pointing = %d\n"), 
                   info->value[0], info->value[1], info->value[2], info->value[3]));
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_PRESEL_DB_BMP:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: presel_db_bmp\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   Program = %d, SW-Database-bitmap-0 = %d, SW-Database-bitmap-1 = %d\n"), 
                   info->value[0], info->value[1], info->value[2]));
        } break;
      case SOC_PPC_FP_RESOURCE_DIAG_PARAM_TYPE_PRESEL_PROG_BITMAP:
        {
          LOG_CLI((BSL_META_U(unit,
                              " * Error type: presel_prog_bitmap\n")));
          LOG_CLI((BSL_META_U(unit,
                              "   Stage = %s, Prog-Bitmap-0 = 0x%x, Prog-Bitmap-1 = 0x%x\n"), 
                   SOC_PPC_FP_DATABASE_STAGE_to_string(info->value[0]), info->value[1], info->value[2]));
        } break;
      default:
        break;
      }
    } break;

  default:
    break;
  }
	
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_PMF_CE_print(
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_PMF_CE *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

    if(info->is_used) {
        LOG_CLI((BSL_META_U(unit,
                            "%-50s"),SOC_PPC_FP_QUAL_TYPE_to_string(info->qual_type)));
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"),info->lsb));
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"),info->msb));
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"),info->qual_lsb));
        if(info->is_msb) {
            LOG_CLI((BSL_META_U(unit,
                                "%-10s"),"Yes"));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "%-10s"),"No"));
        }
        if(info->is_second_key) {
            LOG_CLI((BSL_META_U(unit,
                                "%-15s"),"Yes"));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "%-15s"),"No"));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"),info->key_id));
        LOG_CLI((BSL_META_U(unit,
                            "\n\r")));
    }
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_FREE_INSTRUCTIONS_print(
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_FREE_INSTRUCTIONS *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);
    if(info->is_used) {
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"), info->lsb_16b));
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"), info->lsb_32b));
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"), info->msb_16b));
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"), info->msb_32b));
        LOG_CLI((BSL_META_U(unit,
                            "\n\r")));
    }
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_KEY_DB_DATA_print(
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_KEY_DB_DATA *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);
    if(info->is_used) {
        if(info->is_lsb_db) {
            LOG_CLI((BSL_META_U(unit,
                                "%-10s"), "Yes"));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "%-10s"), "No"));
        }        
        if(info->is_msb_db) {
            LOG_CLI((BSL_META_U(unit,
                                "%-10s"), "Yes"));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "%-10s"), "No"));
        }        
        LOG_CLI((BSL_META_U(unit,
                            "\n\r")));
    }
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
    
void
  SOC_PPC_FP_RESOURCE_PMF_FES_print(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_PMF_FES *info
  )
{
    uint32 action_msb;
    
	SOC_SAND_INIT_ERROR_DEFINITIONS(0);
	SOC_SAND_CHECK_NULL_INPUT(info);
    if(info->is_used) {
        if (SOC_IS_JERICHO_PLUS(unit)) {
            action_msb = info->key_tcam_lsb + 32 - info->valid_bits - 1;
        } else {
            action_msb = info->key_tcam_lsb + info->action_size;
        }
        
        
        if (info->is_action_always_valid) {
            --action_msb;
        }

        if (info->is_key_src) {
            LOG_CLI((BSL_META_U(unit,
                                "{ %s%s } = Key %c[%d:%d]\n\r"), SOC_PPC_FP_ACTION_TYPE_to_string(info->action_type),
                                info->is_action_always_valid?"":", v",
                                'A'+info->key_tcam_id,
                                action_msb,
                                info->key_tcam_lsb
                                ));
        }
        else {
            if (info->use_kaps) {
                LOG_CLI((BSL_META_U(unit,
                                    "{ %s%s } = kaps[%d:%d]\n\r"), SOC_PPC_FP_ACTION_TYPE_to_string(info->action_type),
                                    info->is_action_always_valid?"":", v",
                                    action_msb,
                                    info->key_tcam_lsb
                                    ));
            }
            else {
                LOG_CLI((BSL_META_U(unit,
                                    "{ %s%s } = Action_table_%d[%d:%d]\n\r"), SOC_PPC_FP_ACTION_TYPE_to_string(info->action_type),
                                    info->is_action_always_valid?"":", v",
                                    info->key_tcam_id,
                                    action_msb,
                                    info->key_tcam_lsb
                                    ));
            }
        }
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_PMF_FES_FREE_print(
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_PMF_FES_FREE *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

    if(info->is_used) {
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"), info->fes_free));
        LOG_CLI((BSL_META_U(unit,
                            "\n\r")));
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_PMF_FEM_ENTRY_print(
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_PMF_FEM_ENTRY *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);
    if(info->is_used) {
        if(info->is_for_entry) {
            LOG_CLI((BSL_META_U(unit,
                                "%-19s"),"Yes"));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "%-19s"),"No"));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%-13d"),info->db_strength));
        LOG_CLI((BSL_META_U(unit,
                            "%-16d"),info->entry_strength));
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"),info->entry_id));
        LOG_CLI((BSL_META_U(unit,
                            "%-50s"),SOC_PPC_FP_ACTION_TYPE_to_string(info->action_type)));
        LOG_CLI((BSL_META_U(unit,
                            "\n\r")));
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_RESOURCE_AVAILABLE_print(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PPC_FP_RESOURCE_AVAILABLE *info
  )
{
    uint32 stage;
    uint32 pgm_idx;
    uint32 db_id;
    uint32 cycle_idx;
    uint32 ce_idx;
    uint32 fes_id;
    uint32 fem_group_idx;
    uint32 fem_idx;
    uint32 key;
    uint32 pfg;
    int32 idx;
    uint32 presel_line;
    uint32 qual_idx;
    uint8 is_header_printed = FALSE;
    char hexa_string[20];
    
	SOC_SAND_INIT_ERROR_DEFINITIONS(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

    if(info->is_used){
        LOG_CLI((BSL_META_U(unit,
                            "************************************* AVAILABLE RESOURCES INFO *****************************************\n\r\n\r")));
        for(stage = 0; stage < SOC_PPC_NOF_FP_DATABASE_STAGES; ++stage) {
            for(pgm_idx = 0; pgm_idx < SOC_PPC_FP_NOF_PROGS_MAX; ++pgm_idx) {
                for(db_id = 0; db_id < SOC_PPC_FP_NOF_DBS; ++db_id) {
                    for(cycle_idx = 0; cycle_idx < SOC_PPC_FP_NOF_CYCLES; ++cycle_idx) {
                        for(ce_idx = 0; ce_idx < SOC_PPC_FP_NOF_CES; ++ce_idx) {
                            if(info->pgm_ce[stage][pgm_idx][db_id][cycle_idx][ce_idx].is_used) {
                                if(!is_header_printed) {
                                    LOG_CLI((BSL_META_U(unit,
                                                        "RESOURCES\n\r\n\r")));
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-10s"),"Stage"));
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-10s"),"Program"));
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-10s"),"DB ID"));
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-10s"),"Cycle"));
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-10s"),"CE ID"));
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-50s"),"Qual Type"));
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-10s"),"LSB"));
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-10s"),"MSB"));
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-10s"),"Qual LSB"));
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-10s"),"Is In MSB"));
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-15s"),"Second Key"));
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-10s"),"Key ID"));
                                    LOG_CLI((BSL_META_U(unit,
                                                        "\n\r")));
                                    LOG_CLI((BSL_META_U(unit,
                                                        "---------------------------------------------------------------------------------------"
                                             "---------------------------------------------------------------------------\n\r")));
                                    is_header_printed = TRUE;
                                }
                                switch (stage) {
                                case SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF:
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-10s"),"Ing-PMF"));
                                    break;
                                case SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP:
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-10s"),"Ing-FLP"));
                                    break;
                                case SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB:
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-10s"),"Ing-SLB"));
                                    break;
                                case SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT:
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-10s"),"Ing-VT"));
                                    break;
                                case SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT:
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-10s"),"Ing-TT"));
                                    break;
                                case SOC_PPC_FP_DATABASE_STAGE_EGRESS:
                                default:
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%-10s"),"Egress"));
                                    break;
                                }
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10d"),pgm_idx));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10d"),db_id));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10d"),cycle_idx));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10d"),ce_idx));
                                SOC_PPC_FP_RESOURCE_PMF_CE_print(&info->pgm_ce[stage][pgm_idx][db_id][cycle_idx][ce_idx]);
                            }
                        }
                    }
                }
            }
        }
        is_header_printed = FALSE;
        for(stage = 0; stage < SOC_PPC_NOF_FP_DATABASE_STAGES; ++stage) {
            for(pgm_idx = 0; pgm_idx < SOC_PPC_FP_NOF_PROGS_MAX; ++pgm_idx) {
                for(cycle_idx = 0; cycle_idx < SOC_PPC_FP_NOF_CYCLES; ++cycle_idx) {
                    if(info->free_instructions[stage][pgm_idx][cycle_idx].is_used) {
                        if(!is_header_printed) {
                            LOG_CLI((BSL_META_U(unit,
                                                "\n\r\n\r")));
                            LOG_CLI((BSL_META_U(unit,
                                                "FREE INSTRUCTIONS\n\r\n\r")));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Stage"));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Program"));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Cycle"));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"16b-LSB"));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"32b-LSB"));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"16b-MSB"));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"32b-MSB"));
                            LOG_CLI((BSL_META_U(unit,
                                                "\n\r")));
                            LOG_CLI((BSL_META_U(unit,
                                                "--------------------------------------------------------------------\n\r")));
                            is_header_printed = TRUE;
                        }                        
                        switch (stage) {
                        case SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF:
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Ing-PMF"));
                            break;
                        case SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP:
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Ing-FLP"));
                            break;
                        case SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB:
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Ing-SLB"));
                            break;
                        case SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT:
                            LOG_CLI((BSL_META_U(unit,
                                                 "%-10s"),"Ing-VT"));
                             break;
                        case SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT:
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Ing-TT"));
                            break;
                        case SOC_PPC_FP_DATABASE_STAGE_EGRESS:
                        default:
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Egress"));
                            break;
                        }
                        LOG_CLI((BSL_META_U(unit,
                                            "%-10d"),pgm_idx));
                        LOG_CLI((BSL_META_U(unit,
                                            "%-10d"),cycle_idx));
                        SOC_PPC_FP_RESOURCE_FREE_INSTRUCTIONS_print(&info->free_instructions[stage][pgm_idx][cycle_idx]);
                    }
                }
            }
        }
        is_header_printed = FALSE;
        for(stage = 0; stage < SOC_PPC_NOF_FP_DATABASE_STAGES; ++stage) {
            for(pgm_idx = 0; pgm_idx < SOC_PPC_FP_NOF_PROGS_MAX; ++pgm_idx) {
                for(cycle_idx = 0; cycle_idx < SOC_PPC_FP_NOF_CYCLES; ++cycle_idx) {
                    for(key = 0; key < SOC_PPC_FP_NOF_KEYS; ++key) {
                        if(info->key[stage][pgm_idx][cycle_idx][key].is_used) {
                            if(!is_header_printed) {
                                LOG_CLI((BSL_META_U(unit,
                                                    "\n\r\n\r")));
                                LOG_CLI((BSL_META_U(unit,
                                                    "KEY RESOURCES\n\r\n\r")));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Stage"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Program"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Cycle"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Key"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"LSB-DB"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"MSB-DB"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "\n\r")));
                                LOG_CLI((BSL_META_U(unit,
                                                    "---------------------------------------------------------\n\r")));
                                is_header_printed = TRUE;
                            }
                            switch (stage) {
                            case SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF:
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Ing-PMF"));
                                break;
                            case SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP:
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Ing-FLP"));
                                break;
                            case SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB:
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Ing-SLB"));
                                break;
                            case SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT:
                                LOG_CLI((BSL_META_U(unit,
                                                   "%-10s"),"Ing-VT"));
                                break;
                            case SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT:
                                LOG_CLI((BSL_META_U(unit,
                                                   "%-10s"),"Ing-TT"));
                                 break;
                            case SOC_PPC_FP_DATABASE_STAGE_EGRESS:
                            default:
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Egress"));
                                break;
                            }
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10d"),pgm_idx));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10d"),cycle_idx));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10d"),key));
                            SOC_PPC_FP_RESOURCE_KEY_DB_DATA_print(&info->key[stage][pgm_idx][cycle_idx][key]);
                        }
                    }
                }
            }
        }
        is_header_printed = FALSE;
        for(pgm_idx = 0; pgm_idx < SOC_PPC_FP_NOF_PROGS_MAX; ++pgm_idx) {
            for(cycle_idx = 0; cycle_idx < SOC_PPC_FP_NOF_CYCLES; ++cycle_idx) {
                for(db_id = 0; db_id < SOC_PPC_FP_NOF_DBS; ++db_id) {
                    for(fes_id = 0; fes_id < 32; ++fes_id) {
                        if(info->fes[pgm_idx][cycle_idx][db_id][fes_id].is_used) {
                            if(!is_header_printed) {
                                LOG_CLI((BSL_META_U(unit,
                                                    "\n\r\n\r")));
                                LOG_CLI((BSL_META_U(unit,
                                                    "FES\n\r\n\r")));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Program"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Cycle"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"DB ID"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"FES ID"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-32s"),"Action"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "\n\r")));
                                LOG_CLI((BSL_META_U(unit,
                                                    "-----------------------------------------------------------------------------------------------------\n\r")));
                                is_header_printed = TRUE;
                            }
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10d"),pgm_idx));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10d"),cycle_idx));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10d"),db_id));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10d"),fes_id));
                            SOC_PPC_FP_RESOURCE_PMF_FES_print(unit, &info->fes[pgm_idx][cycle_idx][db_id][fes_id]);
                        }
                    }
                }
            }
        }
        is_header_printed = FALSE;
        for(pgm_idx = 0; pgm_idx < SOC_PPC_FP_NOF_PROGS_MAX; ++pgm_idx) {
            for(cycle_idx = 0; cycle_idx < SOC_PPC_FP_NOF_CYCLES; ++cycle_idx) {
                if(info->fes_free[pgm_idx][cycle_idx].is_used) {
                    if(!is_header_printed) {
                        LOG_CLI((BSL_META_U(unit,
                                            "\n\r\n\r")));
                        LOG_CLI((BSL_META_U(unit,
                                            "FES FREE\n\r\n\r")));
                        LOG_CLI((BSL_META_U(unit,
                                            "%-10s"),"Program"));
                        LOG_CLI((BSL_META_U(unit,
                                            "%-10s"),"Cycle"));
                        LOG_CLI((BSL_META_U(unit,
                                            "%-10s"),"FES Free"));
                        LOG_CLI((BSL_META_U(unit,
                                            "\n\r")));
                        LOG_CLI((BSL_META_U(unit,
                                            "-----------------------------\n\r")));
                        is_header_printed = TRUE;
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        "%-10d"),pgm_idx));
                    LOG_CLI((BSL_META_U(unit,
                                        "%-10d"),cycle_idx));
                    SOC_PPC_FP_RESOURCE_PMF_FES_FREE_print(&info->fes_free[pgm_idx][cycle_idx]);
                }
            }
        }
        is_header_printed = FALSE;
        for(pgm_idx = 0; pgm_idx < SOC_PPC_FP_NOF_FEM_PROGS_MAX; ++pgm_idx) {
            for(cycle_idx = 0; cycle_idx < SOC_PPC_FP_NOF_CYCLES; ++cycle_idx) {
                for(db_id = 0; db_id < SOC_PPC_FP_NOF_DBS; ++db_id) {
                    for(fem_group_idx = 0; fem_group_idx < SOC_PPC_FP_NOF_MACROS; ++fem_group_idx) {
                        if(info->fem[pgm_idx][cycle_idx][db_id][fem_group_idx].is_used) {
                            if(!is_header_printed) {
                                LOG_CLI((BSL_META_U(unit,
                                                    "\n\r\n\r")));
                                LOG_CLI((BSL_META_U(unit,
                                                    "FEM\n\r\n\r")));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-9s"),"FEM pgm"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-7s"),"Cycle"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-7s"),"DB ID"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-8s"),"Fem ID"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-19s"),"Direct Extraction"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-13s"),"DB Strength"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-16s"),"Entry Strength"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Entry ID"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-50s"),"Action Type"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "\n\r")));
                                LOG_CLI((BSL_META_U(unit,
                                                    "------------------------------------------------------------------------------------------------------\n\r")));
                                is_header_printed = TRUE;
                            }
                            LOG_CLI((BSL_META_U(unit,
                                                "%-9d"),pgm_idx));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-7d"),cycle_idx));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-7d"),db_id));
                            fem_idx = fem_group_idx + (cycle_idx * SOC_PPC_FP_NOF_MACROS);
                            LOG_CLI((BSL_META_U(unit,
                                                "%-8d"),fem_idx));
                            SOC_PPC_FP_RESOURCE_PMF_FEM_ENTRY_print(&info->fem[pgm_idx][cycle_idx][db_id][fem_group_idx]);
                        }
                    }
                }
            }
        }
        is_header_printed = FALSE;
        for(stage = 0; stage < SOC_PPC_NOF_FP_DATABASE_STAGES; ++stage) {
            for(presel_line = 0; presel_line < SOC_PPC_FP_NOF_PS_LINES; ++presel_line) {
                for(pgm_idx = 0; pgm_idx < SOC_PPC_FP_NOF_PROGS_MAX; ++pgm_idx) {
                    for(qual_idx = 0; qual_idx < SOC_PPC_NOF_FP_QUAL_TYPES; ++qual_idx) {
                        if(SHR_BITGET(info->quals[stage][presel_line][pgm_idx], qual_idx) ) {
                            if(!is_header_printed) {
                                LOG_CLI((BSL_META_U(unit,
                                                    "\n\r\n\r")));
                                LOG_CLI((BSL_META_U(unit,
                                                    "PRESELECTORS RESOURCE TABLES\n\r\n\r")));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Stage"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-15s"),"Presel Line"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Program"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-20s"),"Pfg Bitmap"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-50s"),"Qualify"));
                                LOG_CLI((BSL_META_U(unit,
                                                    "\n\r")));
                                LOG_CLI((BSL_META_U(unit,
                                                    "--------------------------------------------------------------------------------------------\n\r")));
                                is_header_printed = TRUE;
                            }
                            switch (stage) {
                            case SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF:
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Ing-PMF"));
                                break;
                            case SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP:
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Ing-FLP"));
                                break;
                            case SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB:
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Ing-SLB"));
                                break;
                            case SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT:
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Ing-VT"));
                                break;
                            case SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT:
                                LOG_CLI((BSL_META_U(unit,
                                                   "%-10s"),"Ing-TT"));
                                break;
                            case SOC_PPC_FP_DATABASE_STAGE_EGRESS:
                            default:
                                LOG_CLI((BSL_META_U(unit,
                                                    "%-10s"),"Egress"));
                                break;
                            }
                            LOG_CLI((BSL_META_U(unit,
                                                "%-15d"),presel_line));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10d"),pgm_idx));
                            LOG_CLI((BSL_META_U(unit,
                                                "0x%08x"),info->pfgs[stage][presel_line][pgm_idx][1]));
                            LOG_CLI((BSL_META_U(unit,
                                                "%08x"),info->pfgs[stage][presel_line][pgm_idx][0]));
                            LOG_CLI((BSL_META_U(unit,
                                                "  ")));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-50s"),SOC_PPC_FP_QUAL_TYPE_to_string(qual_idx)));
                            LOG_CLI((BSL_META_U(unit,
                                                "\n\r")));
                        }
                    }                                
                }
            }
        }
        is_header_printed = FALSE;
        for(stage = 0; stage < SOC_PPC_NOF_FP_DATABASE_STAGES; ++stage) {
            for(pfg = 0; pfg < SOC_PPC_FP_NOF_PFGS_ARAD; ++pfg) {
                for(db_id = 0; db_id < SOC_PPC_FP_NOF_DBS; ++db_id) {
                    if(SHR_BITGET(info->pfgs_db_pmb[stage][pfg], db_id) ) {
                        if(!is_header_printed) {
                            LOG_CLI((BSL_META_U(unit,
                                                "\n\r\n\r")));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Stage"));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Pfg"));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"DB ID"));
                            LOG_CLI((BSL_META_U(unit,
                                                "\n\r")));
                            LOG_CLI((BSL_META_U(unit,
                                                "--------------------------\n\r")));
                            is_header_printed = TRUE;
                        }
                        switch (stage) {
                        case SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF:
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Ing-PMF"));
                            break;
                        case SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP:
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Ing-FLP"));
                            break;
                        case SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB:
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Ing-SLB"));
                            break;
                        case SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT:
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Ing-VT"));
                            break;
                        case SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT:
                            LOG_CLI((BSL_META_U(unit,
                                               "%-10s"),"Ing-TT"));
                            break;
                        case SOC_PPC_FP_DATABASE_STAGE_EGRESS:
                        default:
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Egress"));
                            break;
                        }
                        LOG_CLI((BSL_META_U(unit,
                                            "%-10d"),pfg));
                        LOG_CLI((BSL_META_U(unit,
                                            "%-10d"),db_id));
                        LOG_CLI((BSL_META_U(unit,
                                            "\n\r")));
                    }                                
                }
            }
        }
        is_header_printed = FALSE;
        for(stage = 0; stage < SOC_PPC_NOF_FP_DATABASE_STAGES; ++stage) {
            for(pfg = 0; pfg < SOC_PPC_FP_NOF_PFGS_ARAD; ++pfg) {
                for(idx = 0; idx < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX; ++idx) {
                    if((info->pfgs_qualifiers[stage][pfg][idx].type != SOC_PPC_NOF_FP_QUAL_TYPES) 
					&& (info->pfgs_qualifiers[stage][pfg][idx].type != BCM_FIELD_ENTRY_INVALID)) {
                        if(!is_header_printed) {
                            LOG_CLI((BSL_META_U(unit,
                                                "\n\r\n\r")));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Stage"));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Pfg"));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-50s"),"Qualify"));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-20s"),"Data"));
                            LOG_CLI((BSL_META_U(unit,
                                                "%-20s"),"Mask"));
                            LOG_CLI((BSL_META_U(unit,
                                                "\n\r")));
                            LOG_CLI((BSL_META_U(unit,
                                                "-------------------------------------------------------------------------------------------------------------\n\r")));
                            is_header_printed = TRUE;
                        }
                        switch (stage) {
                        case SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF:
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Ing-PMF"));
                            break;
                        case SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP:
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Ing-FLP"));
                            break;
                        case SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB:
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Ing-SLB"));
                            break;
                        case SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT:
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Ing-VT"));
                            break;
                        case SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT:
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Ing-TT"));
                            break;
                        case SOC_PPC_FP_DATABASE_STAGE_EGRESS:
                        default:
                            LOG_CLI((BSL_META_U(unit,
                                                "%-10s"),"Egress"));
                            break;
                        }
                        LOG_CLI((BSL_META_U(unit,
                                            "%-10d"),pfg));
                        LOG_CLI((BSL_META_U(unit,
                                            "%-50s"),SOC_PPC_FP_QUAL_TYPE_to_string(info->pfgs_qualifiers[stage][pfg][idx].type)));
                        if(info->pfgs_qualifiers[stage][pfg][idx].val.arr[1]) {
                            sal_sprintf(hexa_string, "0x%x", info->pfgs_qualifiers[stage][pfg][idx].val.arr[1]);
                            sal_sprintf(&hexa_string[sal_strlen(hexa_string)], "%08x  ", info->pfgs_qualifiers[stage][pfg][idx].val.arr[0]);
                        } else {
                            sal_sprintf(hexa_string, "0x%x", info->pfgs_qualifiers[stage][pfg][idx].val.arr[0]);
                        }
                        LOG_CLI((BSL_META_U(unit,
                                            "%-20s"),hexa_string));
                        if(info->pfgs_qualifiers[stage][pfg][idx].is_valid.arr[1]) {
                            sal_sprintf(hexa_string, "0x%x", info->pfgs_qualifiers[stage][pfg][idx].is_valid.arr[1]);
                            sal_sprintf(&hexa_string[sal_strlen(hexa_string)], "%08x  ", info->pfgs_qualifiers[stage][pfg][idx].is_valid.arr[0]);
                        } else {
                            sal_sprintf(hexa_string, "0x%x", info->pfgs_qualifiers[stage][pfg][idx].is_valid.arr[0]);
                        }
                        LOG_CLI((BSL_META_U(unit,
                                            "%-20s"),hexa_string));
                        LOG_CLI((BSL_META_U(unit,
                                            "\n\r")));
                    }                                
                }
            }
        }
                
    }
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}  

void
  SOC_PPC_FP_RESOURCE_DIAG_INFO_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FP_RESOURCE_DIAG_INFO *info
  )
{
	uint32
		idx,
    idx2,
		idx_id,
    nof_lines;
    char db_desc[128];

	SOC_PPC_FP_DATABASE_STAGE
		stage;

  uint8 is_error[SOC_PPC_FP_RESOURCE_DIAG_ERROR_NOF_TYPE_LAST];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "************************************* RESOURCE INFO *****************************************\n\r")));
  for(idx = 0; idx < SOC_PPC_FP_NOF_DBS; idx++)
  {
    if (info->db[idx].valid)
    {
      db_desc[0] = '\0';
      _bcm_dpp_field_group_hw_handle_to_name(unit, idx, db_desc, sizeof(db_desc));
      LOG_CLI((BSL_META_U(unit, "\nResource DB %u [%s]\n\n\r"), idx, db_desc));
      SOC_PPC_FP_RESOURCE_DB_print(unit, &(info->db[idx]));
    }
  }

  LOG_CLI((BSL_META_U(unit,
                      "\n\n************************************** BANK INFO ********************************************\n\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "%-10s"), "Bank-ID"));
  LOG_CLI((BSL_META_U(unit,
                      "%-16s"), "Entry-Size"));
  LOG_CLI((BSL_META_U(unit,
                      "%-15s"), "Entries-Free"));
  LOG_CLI((BSL_META_U(unit,
                      "%-10s"), "NOF-DBs"));
  LOG_CLI((BSL_META_U(unit,
                      "%s"), "Owner"));

  LOG_CLI((BSL_META_U(unit,
                      "\n-------------------------------------------------------------\n\r")));

  for(idx = 0; idx < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); idx++)
  {
	if(info->bank[idx].is_used)
	{
		idx_id = idx;

		if(info->bank[idx].entry_size == SOC_TMC_TCAM_BANK_ENTRY_SIZE_320_BITS && 
		   (idx % 2) == 0)
		{
		  LOG_CLI((BSL_META_U(unit,
                                      "%u\\%-*u"), idx, idx > 9 ? 7 : 8, idx + 1));
		  idx++;
		}
		else
		{
			LOG_CLI((BSL_META_U(unit,
                                            "%-10u"), idx));
		}
		SOC_PPC_FP_RESOURCE_BANK_print(&(info->bank[idx_id]));
	}
  }
	LOG_CLI((BSL_META_U(unit,
                            "\n%-10s"), "Bank-ID"));
	LOG_CLI((BSL_META_U(unit,
                            "%-10s"), "DB-ID"));
	LOG_CLI((BSL_META_U(unit,
                            "%-15s"), "NOF-Entries"));
	LOG_CLI((BSL_META_U(unit,
                            "%-15s"), "Prefix"));
	LOG_CLI((BSL_META_U(unit,
                            "\n----------------------------------------------------------------\n\r")));

  for(idx = 0; idx < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); idx++)
  {
    if(info->bank[idx].is_used && (info->bank[idx].nof_dbs > 0))
    {
      idx_id = idx;

      if(info->bank[idx].entry_size == SOC_TMC_TCAM_BANK_ENTRY_SIZE_320_BITS && 
         (idx % 2) == 0)
      {
        LOG_CLI((BSL_META_U(unit,
                            "%u\\%-*u"), idx, idx > 9 ? 7 : 8, idx + 1));
        idx++;
      }
      else
      {
        LOG_CLI((BSL_META_U(unit,
                            "%-10u"), idx));
      }
      SOC_PPC_FP_RESOURCE_BANK_ALL_DBS_print(&(info->bank[idx_id]));
    }
  }

  LOG_CLI((BSL_META_U(unit,
                      "\n\n************************************** PRESEL INFO ******************************************\n\n\r")));

  LOG_CLI((BSL_META_U(unit,
                      "%-15s"), "Stage"));
  LOG_CLI((BSL_META_U(unit,
                      "%-8s"), "Presel"));
  LOG_CLI((BSL_META_U(unit,
                      "%-10s"), "PMF-Pgm"));
  LOG_CLI((BSL_META_U(unit,
                      "%-100s"), "Presel-Name"));
  LOG_CLI((BSL_META_U(unit,
                      "%-25s"), "Presel-Bitmap (msb-lsb)"));
  LOG_CLI((BSL_META_U(unit,
                      "%s\n"), "Database-Bitmap (msb-lsb)"));
  LOG_CLI((BSL_META_U(unit,
                      "---------------------------------------------------------------------------------------------------\n\r")));

  for(stage = 0; stage < SOC_PPC_NOF_FP_DATABASE_STAGES; stage++)
  {
      nof_lines = ARAD_PMF_NOF_LINES;
 	  for(idx = 0; idx < nof_lines; idx++)
	  {
			if(info->presel[stage][idx].is_valid)
			{
				LOG_CLI((BSL_META_U(unit,
                                                    "%-15s"), SOC_PPC_FP_DATABASE_STAGE_to_string(stage)));
				LOG_CLI((BSL_META_U(unit,
                                                    "%-8u"), idx));
				SOC_PPC_FP_RESOURCE_PRESEL_print(unit, &(info->presel[stage][idx]));
			}
	  }
	}
	LOG_CLI((BSL_META_U(unit,
                            "\n")));

  SOC_PPC_FP_RESOURCE_AVAILABLE_print(unit, &info->available);
  LOG_CLI((BSL_META_U(unit,
                      "\n\n************************************** ERROR INFO *******************************************\n\n\r")));

  for(idx = 0; idx < SOC_PPC_FP_RESOURCE_DIAG_ERROR_NOF_TYPE_LAST; idx++)
  {
    is_error[idx] = FALSE;

    for(idx2 = 0; idx2 < SOC_PPC_FP_RESOURCE_DIAG_NOF_PARAMS; idx2++)
    {
      if(info->diag[idx].params[idx2].is_error)
      {
        if(!is_error[idx])
        {
          is_error[idx] = TRUE;
          switch(idx)
          {
          case SOC_PPC_FP_RESOURCE_DIAG_ERROR_TYPE_TCAM_BANK:
            LOG_CLI((BSL_META_U(unit,
                                "Errors in TCAM Bank:\n")));
            break;
          case SOC_PPC_FP_RESOURCE_DIAG_ERROR_TYPE_TCAM_DB:
            LOG_CLI((BSL_META_U(unit,
                                "Errors in TCAM DB:\n")));
            break;
          case SOC_PPC_FP_RESOURCE_DIAG_ERROR_TYPE_KEY:
            LOG_CLI((BSL_META_U(unit,
                                "Errors in Key:\n")));
            break;
          case SOC_PPC_FP_RESOURCE_DIAG_ERROR_TYPE_ACTION:
            LOG_CLI((BSL_META_U(unit,
                                "Errors in Action:\n")));
            break;
          case SOC_PPC_FP_RESOURCE_DIAG_ERROR_TYPE_PRESEL:
            LOG_CLI((BSL_META_U(unit,
                                "Errors in Presel:\n")));
            break;
          default:
            break;
          }
        }
        SOC_PPC_FP_RESOURCE_DIAG_ERROR_PARAM_print(idx, idx2, &(info->diag[idx].params[idx2]));
      }
    }
	}
	LOG_CLI((BSL_META_U(unit,
                            "\n")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

