
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_FRWRD_FEC_INCLUDED__

#define __ARAD_PP_FRWRD_FEC_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>

#include <soc/dpp/PPC/ppc_api_frwrd_fec.h>
#include <soc/dpp/PPC/ppc_api_general.h>




 
#define ARAD_PP_FEC_PROTECT_BITMAP_SIZE		                  (SOC_DPP_DEFS_MAX(NOF_FECS) / 2)
#define ARAD_PP_FEC_PROTECT_BITMAP_FEC_TO_IDX(__fec_indx)     (__fec_indx / 2)
#define ARAD_PP_FRWRD_FEC_HASH_FUNC_ID_MAX                    (15)
#define ARAD_PP_FRWRD_FEC_SEED_MAX                            (SOC_SAND_U16_MAX)
#define ARAD_PPC_FRWRD_FEC_NOF_HEADERS_MIN                    ((SOC_IS_JERICHO_PLUS_A0(unit) || SOC_IS_QUX(unit)) ? (0) : (1))
#define ARAD_PPC_FRWRD_FEC_NOF_HEADERS_MAX                    ((SOC_IS_JERICHO_PLUS_A0(unit) || SOC_IS_QUX(unit)) ? (3) : (2))









typedef enum
{
  
  SOC_PPC_FRWRD_FEC_GLBL_INFO_SET = ARAD_PP_PROC_DESC_BASE_FRWRD_FEC_FIRST,
  SOC_PPC_FRWRD_FEC_GLBL_INFO_SET_UNSAFE,
  SOC_PPC_FRWRD_FEC_GLBL_INFO_SET_VERIFY,
  SOC_PPC_FRWRD_FEC_GLBL_INFO_GET,
  SOC_PPC_FRWRD_FEC_GLBL_INFO_GET_VERIFY,
  SOC_PPC_FRWRD_FEC_GLBL_INFO_GET_UNSAFE,
  ARAD_PP_FRWRD_FEC_ENTRY_ADD,
  ARAD_PP_FRWRD_FEC_ENTRY_ADD_UNSAFE,
  ARAD_PP_FRWRD_FEC_ENTRY_ADD_VERIFY,
  ARAD_PP_FRWRD_FEC_ECMP_ADD,
  ARAD_PP_FRWRD_FEC_ECMP_ADD_UNSAFE,
  ARAD_PP_FRWRD_FEC_ECMP_ADD_VERIFY,
  ARAD_PP_FRWRD_FEC_ECMP_UPDATE,
  ARAD_PP_FRWRD_FEC_ECMP_UPDATE_UNSAFE,
  ARAD_PP_FRWRD_FEC_ECMP_UPDATE_VERIFY,
  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_GET,
  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_GET_UNSAFE,
  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_GET_VERIFY,
  ARAD_PP_FRWRD_FEC_ENTRY_GET,
  ARAD_PP_FRWRD_FEC_ENTRY_GET_UNSAFE,
  ARAD_PP_FRWRD_FEC_ENTRY_GET_VERIFY,
  ARAD_PP_FRWRD_FEC_ECMP_GET,
  ARAD_PP_FRWRD_FEC_ECMP_GET_UNSAFE,
  ARAD_PP_FRWRD_FEC_ECMP_GET_VERIFY,
  ARAD_PP_FRWRD_FEC_REMOVE,
  ARAD_PP_FRWRD_FEC_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_FEC_REMOVE_VERIFY,
  ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET,
  ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_UNSAFE,
  ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_VERIFY,
  ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET,
  ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_VERIFY,
  ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_UNSAFE,
  ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET,
  ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_UNSAFE,
  ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_VERIFY,
  ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET,
  ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_VERIFY,
  ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_UNSAFE,
  ARAD_PP_FRWRD_FEC_GET_BLOCK,
  ARAD_PP_FRWRD_FEC_GET_BLOCK_UNSAFE,
  ARAD_PP_FRWRD_FEC_GET_BLOCK_VERIFY,
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET,
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_UNSAFE,
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_VERIFY,
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET,
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_VERIFY,
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_UNSAFE,
  ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET,
  ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_UNSAFE,
  ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_VERIFY,
  ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET,
  ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET_VERIFY,
  ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET_UNSAFE,
  ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET,
  ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_UNSAFE,
  ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_VERIFY,
  ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET,
  ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_VERIFY,
  ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_UNSAFE,
  SOC_PPC_FRWRD_FEC_ECMP_INFO_SET,
  SOC_PPC_FRWRD_FEC_ECMP_INFO_SET_UNSAFE,
  SOC_PPC_FRWRD_FEC_ECMP_INFO_SET_VERIFY,
  SOC_PPC_FRWRD_FEC_ECMP_INFO_GET,
  SOC_PPC_FRWRD_FEC_ECMP_INFO_GET_VERIFY,
  SOC_PPC_FRWRD_FEC_ECMP_INFO_GET_UNSAFE,
  ARAD_PP_FRWRD_FEC_GET_PROCS_PTR,
  ARAD_PP_FRWRD_FEC_GET_ERRS_PTR,
  
   ARAD_PP_FRWRD_FEC_SIZE_TO_INDEX_GET,
   ARAD_PP_FRWRD_FEC_INDEX_TO_SIZE_GET,
   ARAD_PP_FRWRD_FEC_ONE_ENTRY_SET,
   ARAD_PP_FRWRD_FEC_ONE_ENTRY_GET,
   ARAD_PP_FRWRD_FEC_SIZE_GET,
   SOC_PPC_FRWRD_FEC_PROTECT_TYPE_GET,
   ARAD_PP_FRWRD_FEC_DEST_ENCODE,
   ARAD_PP_FRWRD_FEC_DEST_DECODE,

  
  ARAD_PP_FRWRD_FEC_PROCEDURE_DESC_LAST
} ARAD_PP_FRWRD_FEC_PROCEDURE_DESC;

typedef enum
{
  
  SOC_PPC_FRWRD_FEC_PROTECT_TYPE_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_FRWRD_FEC_FIRST,
  ARAD_PP_FRWRD_FEC_SUCCESS_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_OAM_INSTANCE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_UP_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_ECMP_SIZES_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_ECMP_SIZES_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  SOC_PPC_FRWRD_FEC_RPF_MODE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_DIST_TREE_NICK_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_EEP_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_OAM_INSTANCE_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_VALUE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_SEED_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_HASH_FUNC_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_KEY_SHIFT_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_NOF_HEADERS_OUT_OF_RANGE_ERR,
  
  ARAD_PP_FRWRD_FEC_NO_MATCH_ECMP_SIZE_ERR,
  ARAD_PP_FRWRD_FEC_ECMP_SIZE_INDEX_OUT_OF_RANGE_ERR,

  ARAD_PP_FRWRD_FEC_PROTECTED_NOT_EVEN_INDEX_ERR,
  ARAD_PP_FRWRD_FEC_ECMP_NOT_EVEN_INDEX_ERR,
  ARAD_PP_FRWRD_FEC_UPDATE_RANGE_OUT_OF_ECMP_ERR,
  ARAD_PP_FRWRD_FEC_UPDATE_RANGE_ILLEGAL_ERR,
  ARAD_PP_FRWRD_FEC_DEST_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_IPMC_DEST_NOT_MC_ERR,
  ARAD_PP_FRWRD_FEC_IPMC_RPF_MODE_ILLEGAL_ERR,
  ARAD_PP_FRWRD_FEC_IPUC_RPF_MODE_ILLEGAL_ERR,
  ARAD_PP_FRWRD_FEC_EXPECT_NON_ECMP_ERR,
  ARAD_PP_FRWRD_FEC_EXPECT_NON_PROTECT_ERR,
  ARAD_PP_FRWRD_FEC_DEST_PHY_PORT_STATUS_ILLEGAL_ERR,
  ARAD_PP_FRWRD_FEC_TRAP_ACCESS_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_ACCESSED_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_ENTRY_TYPE_DEPRECATED_ROUTING_ERR,
  ARAD_PP_FRWRD_FEC_ENTRY_TYPE_DEPRECATED_TUNNEL_ERR,
  ARAD_PP_FRWRD_FEC_ECMP_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_ECMP_SIZE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_ECMP_RPF_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FEC_ECMP_LAG_HASH_MATCH_ERR,
  ARAD_PP_FRWRD_FEC_SELECTED_POLYNOMIAL_IN_USE,
  

  
  ARAD_PP_FRWRD_FEC_ERR_LAST
} ARAD_PP_FRWRD_FEC_ERR;


typedef enum
{
    ARAD_PP_FRWRD_FEC_HASH_INDEX_ECMP,
    ARAD_PP_FRWRD_FEC_HASH_INDEX_LAG,
    ARAD_PP_FRWRD_FEC_HASH_INDEX_FLEXIBLE,
    ARAD_PP_FRWRD_FEC_HASH_INDEX_LAG_1,
    ARAD_PP_FRWRD_FEC_HASH_INDEX_ECMP_SECOND_HIER,
    ARAD_PP_FRWRD_FEC_HASH_INDEX_NOF
} ARAD_PP_FRWRD_FEC_HASH_INDEX_TYPE;

typedef struct
{
  SOC_PPC_FEC_TYPE fec_entry_type[SOC_DPP_DEFS_MAX(NOF_FECS)];
  uint8 flp_progs_mapping[SOC_DPP_DEFS_MAX(NOF_FLP_PROGRAMS)];
  uint8 lem_prefix_mapping[(1 << SOC_DPP_DEFS_MAX(NOF_LEM_PREFIXES))];
  SHR_BITDCL is_protected[_SHR_BITDCLSIZE(ARAD_PP_FEC_PROTECT_BITMAP_SIZE)];
  uint64 flp_hw_prog_id_bitmap;
  uint64 flp_prog_select_id_bitmap;
  int32  flp_ipv4mc_bridge_v4mc_cam_sel_id;
  ARAD_PP_FLP_PROG_NS_INFO name_space[ARAD_PP_FLP_PROG_NOF];
} ARAD_PP_FEC;









uint32
  arad_pp_frwrd_fec_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_frwrd_fec_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_GLBL_INFO                 *glbl_info
  );

uint32
  arad_pp_frwrd_fec_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_GLBL_INFO                 *glbl_info
  );

uint32
  arad_pp_frwrd_fec_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_frwrd_fec_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_GLBL_INFO                 *glbl_info
  );


uint32
  arad_pp_frwrd_fec_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_TYPE              protect_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_INFO              *protect_info,
    SOC_SAND_OUT uint8                                 *success
  );

uint32
  arad_pp_frwrd_fec_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_TYPE              protect_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_INFO              *protect_info
  );


uint32
  arad_pp_frwrd_fec_entry_uc_rpf_mode_set(
    int                                          unit,
    uint32                                       fec_ndx,
    uint32                                       uc_rpf_mode
  );


uint32
  arad_pp_frwrd_fec_ecmp_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  uint32                                  nof_entries,
    SOC_SAND_OUT uint8                                 *success
  );

uint32
  arad_pp_frwrd_fec_ecmp_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  uint32                                  nof_entries
  );


uint32
  arad_pp_frwrd_fec_ecmp_update_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range
  );

uint32
  arad_pp_frwrd_fec_ecmp_update_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range
  );


uint32
  arad_pp_frwrd_fec_ecmp_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                            ecmp_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_INFO               *ecmp_info
  );

uint32
  arad_pp_frwrd_fec_ecmp_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                            ecmp_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_INFO               *ecmp_info
  );

uint32
  arad_pp_frwrd_fec_ecmp_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                            ecmp_ndx
  );


uint32
  arad_pp_frwrd_fec_ecmp_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                            ecmp_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_INFO               *ecmp_info
  );
  


uint32
  arad_pp_frwrd_fec_entry_use_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO            *fec_entry_info
  );

uint32
  arad_pp_frwrd_fec_entry_use_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx
  );


uint32
  arad_pp_frwrd_fec_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_PROTECT_TYPE              *protect_type,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_PROTECT_INFO              *protect_info
  );

uint32
  arad_pp_frwrd_fec_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx
  );


uint32
  arad_pp_frwrd_fec_ecmp_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_frwrd_fec_ecmp_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range
  );


uint32
  arad_pp_frwrd_fec_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx
  );

uint32
  arad_pp_frwrd_fec_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx
  );


uint32
  arad_pp_frwrd_fec_protection_oam_instance_status_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx,
    SOC_SAND_IN  uint8                                 up
  );

uint32
  arad_pp_frwrd_fec_protection_oam_instance_status_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx,
    SOC_SAND_IN  uint8                                 up
  );

uint32
  arad_pp_frwrd_fec_protection_oam_instance_status_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx
  );


uint32
  arad_pp_frwrd_fec_protection_oam_instance_status_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx,
    SOC_SAND_OUT uint8                                 *up
  );


uint32
  arad_pp_frwrd_fec_protection_sys_port_status_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_IN  uint8                                 up
  );

uint32
  arad_pp_frwrd_fec_protection_sys_port_status_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_IN  uint8                                 up
  );

uint32
  arad_pp_frwrd_fec_protection_sys_port_status_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx
  );


uint32
  arad_pp_frwrd_fec_protection_sys_port_status_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_OUT uint8                                 *up
  );


uint32
  arad_pp_frwrd_fec_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_MATCH_RULE                *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT uint32                                  *fec_array,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_frwrd_fec_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_MATCH_RULE                *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  );


uint32
  arad_pp_frwrd_fec_entry_accessed_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  );

uint32
  arad_pp_frwrd_fec_entry_accessed_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  );

uint32
  arad_pp_frwrd_fec_entry_accessed_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id_ndx
  );


uint32
  arad_pp_frwrd_fec_entry_accessed_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  uint8                                 clear_access_stat,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  );


uint32
  arad_pp_frwrd_fec_ecmp_hashing_global_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  );

uint32
  arad_pp_frwrd_fec_ecmp_hashing_global_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  );

uint32
  arad_pp_frwrd_fec_ecmp_hashing_global_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

uint32
arad_pp_frwrd_fec_is_protected_get(
    int                         unit,
	uint32                      fec_ndx,
	uint8						*is_protected
);


uint32
  arad_pp_frwrd_fec_ecmp_hashing_global_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  );


uint32
  arad_pp_frwrd_fec_ecmp_hashing_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  );

uint32
  arad_pp_frwrd_fec_ecmp_hashing_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  );

uint32
  arad_pp_frwrd_fec_ecmp_hashing_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx
  );

uint32
  arad_pp_frwrd_fec_unique_polynomial_check(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  int                                         new_hw_val,
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_HASH_INDEX_TYPE           hash_index_type
  );


uint32
  arad_pp_frwrd_fec_ecmp_hashing_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_fec_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_fec_get_errs_ptr(void);

uint32
  SOC_PPC_FRWRD_FEC_GLBL_INFO_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_GLBL_INFO *info
  );

uint32
  SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO *info
  );

uint32
  SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO *info,
    SOC_SAND_IN  SOC_PPC_FEC_TYPE                 entry_type
  );

uint32
  SOC_PPC_FRWRD_FEC_ENTRY_INFO_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO *info
  );

uint32
  SOC_PPC_FRWRD_FEC_ECMP_INFO_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_INFO *info
  );

uint32
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO *info
  );

uint32
  SOC_PPC_FRWRD_FEC_PROTECT_INFO_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_INFO *info
  );

uint32
  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO *info
  );

uint32
  SOC_PPC_FRWRD_FEC_MATCH_RULE_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_MATCH_RULE *info
  );

uint32
  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO *info
  );

uint32
  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO *info
  );

uint32
  arad_pp_frwrd_fec_hash_index_to_hw_val(
    SOC_SAND_IN  uint8                   hash_index
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



