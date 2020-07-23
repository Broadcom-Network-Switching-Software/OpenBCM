/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#ifndef __SOC_PPD_API_FP_INCLUDED__

#define __SOC_PPD_API_FP_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>

#include <soc/dpp/PPC/ppc_api_fp.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_FP_PACKET_FORMAT_GROUP_SET = SOC_PPD_PROC_DESC_BASE_FP_FIRST,
  SOC_PPD_FP_PACKET_FORMAT_GROUP_SET_PRINT,
  SOC_PPD_FP_PACKET_FORMAT_GROUP_GET,
  SOC_PPD_FP_PACKET_FORMAT_GROUP_GET_PRINT,
  SOC_PPD_FP_DATABASE_CREATE,
  SOC_PPD_FP_DATABASE_CREATE_PRINT,
  SOC_PPD_FP_DATABASE_GET,
  SOC_PPD_FP_DATABASE_GET_PRINT,
  SOC_PPD_FP_DATABASE_DESTROY,
  SOC_PPD_FP_DATABASE_DESTROY_PRINT,
  SOC_PPD_FP_ENTRY_ADD,
  SOC_PPD_FP_ENTRY_ADD_PRINT,
  SOC_PPD_FP_ENTRY_GET,
  SOC_PPD_FP_ENTRY_GET_PRINT,
  SOC_PPD_FP_ENTRY_REMOVE,
  SOC_PPD_FP_ENTRY_REMOVE_PRINT,
  SOC_PPD_FP_DATABASE_ENTRIES_GET_BLOCK,
  SOC_PPD_FP_DATABASE_ENTRIES_GET_BLOCK_PRINT,
  SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD,
  SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_PRINT,
  SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_GET,
  SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_GET_PRINT,
  SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_REMOVE,
  SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_PRINT,
  SOC_PPD_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET,
  SOC_PPD_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET_PRINT,
  SOC_PPD_FP_CONTROL_SET,
  SOC_PPD_FP_CONTROL_SET_PRINT,
  SOC_PPD_FP_CONTROL_GET,
  SOC_PPD_FP_CONTROL_GET_PRINT,
  SOC_PPD_FP_EGR_DB_MAP_SET,
  SOC_PPD_FP_EGR_DB_MAP_SET_PRINT,
  SOC_PPD_FP_EGR_DB_MAP_GET,
  SOC_PPD_FP_EGR_DB_MAP_GET_PRINT,
  SOC_PPD_FP_PACKET_DIAG_GET,
  SOC_PPD_FP_PACKET_DIAG_GET_PRINT,
  SOC_PPD_FP_RESOURCE_DIAG_GET,
  SOC_PPD_FP_RESOURCE_DIAG_GET_PRINT,
  SOC_PPD_FP_GET_PROCS_PTR,
  



  
  SOC_PPD_FP_PROCEDURE_DESC_LAST
} SOC_PPD_FP_PROCEDURE_DESC;










uint32
  soc_ppd_fp_packet_format_group_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  pfg_ndx,
    SOC_SAND_IN  SOC_PPC_PMF_PFG_INFO            *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE       *success
  );


uint32
  soc_ppd_fp_packet_format_group_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  pfg_ndx,
    SOC_SAND_OUT SOC_PPC_PMF_PFG_INFO            *info
  );


uint32
  soc_ppd_fp_database_create(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_INOUT  SOC_PPC_FP_DATABASE_INFO                    *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_fp_database_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_OUT SOC_PPC_FP_DATABASE_INFO                    *info
  );


uint32
  soc_ppd_fp_database_destroy(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx
  );


uint32
  soc_ppd_fp_entry_add(
    SOC_SAND_IN    int                               unit,
    SOC_SAND_IN    uint32                            db_id_ndx,
    SOC_SAND_IN    uint32                            entry_id_ndx,
    SOC_SAND_INOUT SOC_PPC_FP_ENTRY_INFO             *info,
    SOC_SAND_OUT   SOC_SAND_SUCCESS_FAILURE          *success
  );


uint32
  soc_ppd_fp_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_OUT uint8                                *is_found,
    SOC_SAND_INOUT SOC_PPC_FP_ENTRY_INFO              *info
  );

uint32
  soc_ppd_fp_entry_remove_by_key(
    SOC_SAND_IN     int                               unit,
    SOC_SAND_IN     uint32                               db_id_ndx,
    SOC_SAND_INOUT  SOC_PPC_FP_ENTRY_INFO                *info
  );


uint32
  soc_ppd_fp_entry_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_IN  uint32                               is_sw_remove_only
  );


uint32
  soc_ppd_fp_database_entries_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_PPC_FP_ENTRY_INFO                       *entries,
    SOC_SAND_OUT uint32                                *nof_entries
  );


uint32
  soc_ppd_fp_direct_extraction_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_IN  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO              *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_fp_direct_extraction_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_OUT uint8                               *is_found,
    SOC_SAND_OUT SOC_PPC_FP_DIR_EXTR_ENTRY_INFO              *info
  );


uint32
  soc_ppd_fp_direct_extraction_entry_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx
  );


uint32
  soc_ppd_fp_direct_extraction_db_entries_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE     *block_range,
    SOC_SAND_OUT SOC_PPC_FP_DIR_EXTR_ENTRY_INFO *entries,
    SOC_SAND_OUT uint32                   *nof_entries
  );


uint32
  soc_ppd_fp_control_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  int                  core_id,
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_INDEX      *control_ndx,
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_INFO        *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE       *success
  );


uint32
  soc_ppd_fp_control_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  int                  core_id,
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_INDEX       *control_ndx,
    SOC_SAND_OUT SOC_PPC_FP_CONTROL_INFO        *info
  );


uint32
  soc_ppd_fp_egr_db_map_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                  port_profile_ndx,
    SOC_SAND_IN  uint32                   db_id
  );


uint32
  soc_ppd_fp_egr_db_map_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                  port_profile_ndx,
    SOC_SAND_OUT uint32                   *db_id
  );


uint32
  soc_ppd_fp_database_compress(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  db_id_ndx
  );


uint32
  soc_ppd_fp_packet_diag_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core_id,
    SOC_SAND_OUT SOC_PPC_FP_PACKET_DIAG_INFO *info
  );


uint32
  soc_ppd_fp_resource_diag_get(
    SOC_SAND_IN 	int               		unit,
	SOC_SAND_IN	 	SOC_PPC_FP_RESOURCE_DIAG_MODE	mode,
    SOC_SAND_OUT 	SOC_PPC_FP_RESOURCE_DIAG_INFO	*info
  );

uint32
  soc_ppd_fp_dbs_action_info_show(
    SOC_SAND_IN 	int unit
  ) ;

uint32
  soc_ppd_fp_action_info_show(
    SOC_SAND_IN 	int unit
  ) ;

uint32
  soc_ppd_fp_print_all_fems_for_stage(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   uint32                 stage,
    SOC_SAND_IN   uint8                  is_for_tm
  ) ;

uint32
  soc_ppd_fp_print_fes_info_for_stage(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   uint32                 stage,
    SOC_SAND_IN   uint32                 pmf_pgm_ndx
  ) ;

uint32
    soc_ppd_fp_ire_traffic_send(
        SOC_SAND_IN int          unit,
        SOC_SAND_IN SOC_PPC_FP_PACKET  *packet,
        SOC_SAND_IN int          tx_count,
        SOC_SAND_IN int          core
    );


uint32
  soc_ppd_fp_select_line_clear(
    SOC_SAND_IN int                           unit,
    SOC_SAND_IN int                           core_id
  );


uint32
  soc_ppd_fp_select_line_get(
    SOC_SAND_IN     int                         unit,
    SOC_SAND_IN     int                         core_id,
    SOC_SAND_INOUT  SOC_PPC_FP_SELECT_LINE_INFO *line_info
  );


uint32
  soc_ppd_fp_last_fem_get(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  int                             core_id,
    SOC_SAND_IN  uint32                          fem_ndx,
    SOC_SAND_OUT SOC_PPC_FP_LAST_FEM_INOUT_INFO  *info
  );


uint32
  soc_ppd_fp_last_fes_get(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  int                             core_id,
    SOC_SAND_IN  uint32                          fes_ndx,
    SOC_SAND_OUT SOC_PPC_FP_LAST_FES_INOUT_INFO  *info
  );

uint32
  soc_ppd_fp_ce_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE         stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                             is_second_lookup,
    SOC_SAND_OUT SOC_PPC_FP_CE_INFO                *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

