/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_FRWRD_FEC_INCLUDED__

#define __SOC_PPD_API_FRWRD_FEC_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_frwrd_fec.h>

#include <soc/dpp/PPD/ppd_api_general.h>





#define SOC_PPD_FRWRD_FEC_NOF_FECS_PER_BANK(_unit)                              (SOC_DPP_DEFS_GET((_unit), nof_fecs) / SOC_DPP_DEFS_GET(_unit, nof_fec_banks))


#define SOC_PPD_FRWRD_FEC_BANK_ID(_unit, _fec_idx)                              ((_fec_idx) / SOC_PPD_FRWRD_FEC_NOF_FECS_PER_BANK(_unit))









typedef enum
{
  
  SOC_PPD_FRWRD_FEC_GLBL_INFO_SET = SOC_PPD_PROC_DESC_BASE_FRWRD_FEC_FIRST,
  SOC_PPD_FRWRD_FEC_GLBL_INFO_SET_PRINT,
  SOC_PPD_FRWRD_FEC_GLBL_INFO_GET,
  SOC_PPD_FRWRD_FEC_GLBL_INFO_GET_PRINT,
  SOC_PPD_FRWRD_FEC_ENTRY_ADD,
  SOC_PPD_FRWRD_FEC_ENTRY_ADD_PRINT,
  SOC_PPD_FRWRD_FEC_ECMP_ADD,
  SOC_PPD_FRWRD_FEC_ECMP_ADD_PRINT,
  SOC_PPD_FRWRD_FEC_ECMP_UPDATE,
  SOC_PPD_FRWRD_FEC_ECMP_UPDATE_PRINT,
  SOC_PPD_FRWRD_FEC_ENTRY_USE_INFO_GET,
  SOC_PPD_FRWRD_FEC_ENTRY_USE_INFO_GET_PRINT,
  SOC_PPD_FRWRD_FEC_ENTRY_GET,
  SOC_PPD_FRWRD_FEC_ENTRY_GET_PRINT,
  SOC_PPD_FRWRD_FEC_ECMP_GET,
  SOC_PPD_FRWRD_FEC_ECMP_GET_PRINT,
  SOC_PPD_FRWRD_FEC_REMOVE,
  SOC_PPD_FRWRD_FEC_REMOVE_PRINT,
  SOC_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET,
  SOC_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_PRINT,
  SOC_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET,
  SOC_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_PRINT,
  SOC_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET,
  SOC_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_PRINT,
  SOC_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET,
  SOC_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_PRINT,
  SOC_PPD_FRWRD_FEC_GET_BLOCK,
  SOC_PPD_FRWRD_FEC_GET_BLOCK_PRINT,
  SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET,
  SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_PRINT,
  SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET,
  SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_PRINT,
  SOC_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET,
  SOC_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_PRINT,
  SOC_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET,
  SOC_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET_PRINT,
  SOC_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET,
  SOC_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_PRINT,
  SOC_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET,
  SOC_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_PRINT,
  SOC_PPD_FRWRD_FEC_ECMP_INFO_SET,
  SOC_PPD_FRWRD_FEC_ECMP_INFO_GET,
  SOC_PPD_FRWRD_FEC_GET_PROCS_PTR,
  
  
  SOC_PPD_FRWRD_FEC_PROCEDURE_DESC_LAST
} SOC_PPD_FRWRD_FEC_PROCEDURE_DESC;










uint32
  soc_ppd_frwrd_fec_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_GLBL_INFO                 *glbl_info
  );


uint32
  soc_ppd_frwrd_fec_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_GLBL_INFO                 *glbl_info
  );


uint32
  soc_ppd_frwrd_fec_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_TYPE              protect_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_INFO              *protect_info,
    SOC_SAND_OUT uint8                               *success
  );


uint32
  soc_ppd_frwrd_fec_ecmp_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  uint32                                nof_entries,
    SOC_SAND_OUT uint8                               *success
  );


uint32
  soc_ppd_frwrd_fec_ecmp_update(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                          *fec_range
  );


uint32
  soc_ppd_frwrd_fec_ecmp_info_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PPC_FEC_ID ecmp_ndx,
    SOC_SAND_IN SOC_PPC_FRWRD_FEC_ECMP_INFO *ecmp_info
  ); 


uint32
  soc_ppd_frwrd_fec_ecmp_info_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PPC_FEC_ID ecmp_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_INFO *ecmp_info
  ); 


uint32
  soc_ppd_frwrd_fec_entry_use_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO            *fec_entry_info
  );


uint32
  soc_ppd_frwrd_fec_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_PROTECT_TYPE              *protect_type,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_PROTECT_INFO              *protect_info
  );


uint32
  soc_ppd_frwrd_fec_ecmp_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                          *fec_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_OUT uint32                                *nof_entries
  );


uint32
  soc_ppd_frwrd_fec_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx
  );


uint32
  soc_ppd_frwrd_fec_protection_oam_instance_status_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                oam_instance_ndx,
    SOC_SAND_IN  uint8                               up
  );


uint32
  soc_ppd_frwrd_fec_protection_oam_instance_status_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                oam_instance_ndx,
    SOC_SAND_OUT uint8                               *up
  );


uint32
  soc_ppd_frwrd_fec_protection_sys_port_status_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *sys_port_ndx,
    SOC_SAND_IN  uint8                               up
  );


uint32
  soc_ppd_frwrd_fec_protection_sys_port_status_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *sys_port_ndx,
    SOC_SAND_OUT uint8                               *up
  );


uint32
  soc_ppd_frwrd_fec_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_MATCH_RULE                *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range,
    SOC_SAND_OUT uint32                                *fec_array,
    SOC_SAND_OUT uint32                                *nof_entries
  );

#if SOC_PPC_DEBUG_IS_LVL1

uint32
  soc_ppd_frwrd_fec_print_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_MATCH_RULE                *rule,
    SOC_SAND_IN SOC_SAND_TABLE_BLOCK_RANGE                   *block_range,
    SOC_SAND_IN uint32                                 *fec_array,
    SOC_SAND_IN uint32                                 nof_entries
  );
#endif

uint32
  soc_ppd_frwrd_fec_entry_accessed_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  );


uint32
  soc_ppd_frwrd_fec_entry_accessed_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  uint8                               clear_access_stat,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  );


uint32
  soc_ppd_frwrd_fec_ecmp_hashing_global_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  );


uint32
  soc_ppd_frwrd_fec_ecmp_hashing_global_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  );


uint32
  soc_ppd_frwrd_fec_ecmp_hashing_port_info_set(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                                core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  );


uint32
  soc_ppd_frwrd_fec_ecmp_hashing_port_info_get(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

