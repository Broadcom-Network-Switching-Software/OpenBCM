/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_FP_INCLUDED__

#define __ARAD_PP_FP_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_fp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_parse.h>

#include <soc/dpp/ARAD/arad_pmf_low_level.h>
#include <soc/dpp/ARAD/arad_tcam_mgmt.h>






#define  ARAD_PP_FP_NOF_PDS        (8)


#define QUX_KAPS_DIRECT_LOOKUP_BANK_ID_MASK (~0x3FFF)
#define QUX_KAPS_DIRECT_LOOKUP_ENTRY_ID_MASK 0xFFF
#define QUX_KAPS_DIRECT_LOOKUP_UNUSED_BITS_MASK 0x3000
#define QUX_KAPS_DIRECT_LOOKUP_NOF_UNUSED_BITS 2
#define ARAD_PP_FP_DIRECT_TABLE_KAPS_KEY_LENGTH \
    ((SOC_IS_QUX(unit) && (SOC_DPP_JER_CONFIG(unit)->pp.kaps_large_db_bits > 12)) \
        ? (SOC_DPP_JER_CONFIG(unit)->pp.kaps_large_db_bits + QUX_KAPS_DIRECT_LOOKUP_NOF_UNUSED_BITS) \
            : SOC_IS_JERICHO(unit) ? SOC_DPP_JER_CONFIG(unit)->pp.kaps_large_db_bits : 0)












typedef enum
{
  
  SOC_PPC_FP_PACKET_FORMAT_GROUP_SET = ARAD_PP_PROC_DESC_BASE_FP_FIRST,
  SOC_PPC_FP_PACKET_FORMAT_GROUP_SET_PRINT,
  SOC_PPC_FP_PACKET_FORMAT_GROUP_SET_UNSAFE,
  SOC_PPC_FP_PACKET_FORMAT_GROUP_SET_VERIFY,
  SOC_PPC_FP_PACKET_FORMAT_GROUP_GET,
  SOC_PPC_FP_PACKET_FORMAT_GROUP_GET_PRINT,
  SOC_PPC_FP_PACKET_FORMAT_GROUP_GET_VERIFY,
  SOC_PPC_FP_PACKET_FORMAT_GROUP_GET_UNSAFE,
  ARAD_PP_FP_DATABASE_CREATE,
  ARAD_PP_FP_DATABASE_CREATE_PRINT,
  ARAD_PP_FP_DATABASE_CREATE_UNSAFE,
  ARAD_PP_FP_DATABASE_CREATE_VERIFY,
  ARAD_PP_FP_DATABASE_GET,
  ARAD_PP_FP_DATABASE_GET_PRINT,
  ARAD_PP_FP_DATABASE_GET_UNSAFE,
  ARAD_PP_FP_DATABASE_GET_VERIFY,
  ARAD_PP_FP_DATABASE_DESTROY,
  ARAD_PP_FP_DATABASE_DESTROY_PRINT,
  ARAD_PP_FP_DATABASE_DESTROY_UNSAFE,
  ARAD_PP_FP_DATABASE_DESTROY_VERIFY,
  ARAD_PP_FP_ENTRY_ADD,
  ARAD_PP_FP_ENTRY_ADD_PRINT,
  ARAD_PP_FP_ENTRY_ADD_UNSAFE,
  ARAD_PP_FP_ENTRY_ADD_VERIFY,
  ARAD_PP_FP_ENTRY_GET,
  ARAD_PP_FP_ENTRY_GET_PRINT,
  ARAD_PP_FP_ENTRY_GET_UNSAFE,
  ARAD_PP_FP_ENTRY_GET_VERIFY,
  ARAD_PP_FP_ENTRY_REMOVE,
  ARAD_PP_FP_ENTRY_REMOVE_PRINT,
  ARAD_PP_FP_ENTRY_REMOVE_UNSAFE,
  ARAD_PP_FP_ENTRY_REMOVE_VERIFY,
  ARAD_PP_FP_DATABASE_ENTRIES_GET_BLOCK,
  ARAD_PP_FP_DATABASE_ENTRIES_GET_BLOCK_PRINT,
  ARAD_PP_FP_DATABASE_ENTRIES_GET_BLOCK_UNSAFE,
  ARAD_PP_FP_DATABASE_ENTRIES_GET_BLOCK_VERIFY,
  ARAD_PP_FP_DIRECT_EXTRACTION_ENTRY_ADD,
  ARAD_PP_FP_DIRECT_EXTRACTION_ENTRY_ADD_PRINT,
  ARAD_PP_FP_DIRECT_EXTRACTION_ENTRY_ADD_UNSAFE,
  ARAD_PP_FP_DIRECT_EXTRACTION_ENTRY_ADD_VERIFY,
  ARAD_PP_FP_DIRECT_EXTRACTION_ENTRY_GET,
  ARAD_PP_FP_DIRECT_EXTRACTION_ENTRY_GET_PRINT,
  ARAD_PP_FP_DIRECT_EXTRACTION_ENTRY_GET_UNSAFE,
  ARAD_PP_FP_DIRECT_EXTRACTION_ENTRY_GET_VERIFY,
  ARAD_PP_FP_DIRECT_EXTRACTION_ENTRY_REMOVE,
  ARAD_PP_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_PRINT,
  ARAD_PP_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_UNSAFE,
  ARAD_PP_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_VERIFY,
  ARAD_PP_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET,
  ARAD_PP_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET_PRINT,
  ARAD_PP_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET_UNSAFE,
  ARAD_PP_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET_VERIFY,
  ARAD_PP_FP_CONTROL_SET,
  ARAD_PP_FP_CONTROL_SET_PRINT,
  ARAD_PP_FP_CONTROL_SET_UNSAFE,
  ARAD_PP_FP_CONTROL_SET_VERIFY,
  ARAD_PP_FP_CONTROL_GET,
  ARAD_PP_FP_CONTROL_GET_PRINT,
  ARAD_PP_FP_CONTROL_GET_VERIFY,
  ARAD_PP_FP_CONTROL_GET_UNSAFE,
  ARAD_PP_FP_EGR_DB_MAP_SET,
  ARAD_PP_FP_EGR_DB_MAP_SET_PRINT,
  ARAD_PP_FP_EGR_DB_MAP_SET_UNSAFE,
  ARAD_PP_FP_EGR_DB_MAP_SET_VERIFY,
  ARAD_PP_FP_EGR_DB_MAP_GET,
  ARAD_PP_FP_EGR_DB_MAP_GET_PRINT,
  ARAD_PP_FP_EGR_DB_MAP_GET_VERIFY,
  ARAD_PP_FP_EGR_DB_MAP_GET_UNSAFE,
  SOC_PPC_FP_PACKET_DIAG_GET,
  SOC_PPC_FP_PACKET_DIAG_GET_PRINT,
  SOC_PPC_FP_PACKET_DIAG_GET_UNSAFE,
  SOC_PPC_FP_PACKET_DIAG_GET_VERIFY,
  ARAD_PP_FP_RESOURCE_DIAG_GET,
  ARAD_PP_FP_RESOURCE_DIAG_GET_PRINT,
  ARAD_PP_FP_RESOURCE_DIAG_GET_UNSAFE,
  ARAD_PP_FP_RESOURCE_DIAG_GET_VERIFY,
  ARAD_PP_FP_GET_PROCS_PTR,
  ARAD_PP_FP_GET_ERRS_PTR,
  
   ARAD_PP_FP_DB_TYPES_VERIFY_FIRST_STAGE,
   SOC_PPC_FP_QUAL_TYPES_VERIFY_FIRST_STAGE,
   SOC_PPC_FP_ACTION_TYPES_VERIFY_FIRST_STAGE,
   SOC_PPC_FP_ACTION_TYPE_TO_CE_COPY,
   ARAD_PP_FP_DB_STAGE_GET,
   ARAD_PP_FP_DB_CASCADED_CYCLE_GET,
   ARAD_PP_FP_DB_IS_EQUAL_PLACE_GET,
   ARAD_PP_FP_TCAM_CALLBACK,
   ARAD_PP_FP_INIT_UNSAFE,
   SOC_PPC_FP_ACTION_TYPE_MAX_SIZE_GET,
   SOC_PPC_FP_ACTION_TYPE_TO_PMF_CONVERT,
   ARAD_PP_FP_FEM_SET,
   ARAD_PP_FP_FEM_DB_FIND,
   SOC_PPC_FP_QUAL_TYPE_PREDEFINED_KEY_GET,
   ARAD_PP_FP_PREDEFINED_KEY_GET,
   ARAD_PP_FP_KEY_GET,
   SOC_PPC_FP_QUAL_TYPE_TO_TCAM_TYPE_CONVERT,
   SOC_PPC_FP_QUAL_TYPE_TO_KEY_FLD_CONVERT,
   ARAD_PP_FP_PREDEFINED_KEY_SIZE_AND_TYPE_GET,
   ARAD_PP_FP_ENTRY_VALIDITY_GET,
   SOC_PPC_FP_QUAL_TYPE_PRESET1,
   SOC_PPC_FP_QUAL_VAL_ENCODE,
   ARAD_PP_FP_ACTION_LSB_GET,
   ARAD_PP_FP_QUAL_LSB_AND_LENGTH_GET,
   ARAD_PP_FP_CE_KEY_LENGTH_MINIMAL_GET,
   ARAD_PP_FP_KEY_INPUT_GET,
   ARAD_PP_FP_ENTRY_NDX_DIRECT_TABLE_GET,
   SOC_PPC_FP_QUAL_TYPE_AND_LOCAL_LSB_GET,
   SOC_PPC_FP_ACTION_TYPE_FROM_PMF_CONVERT,
   ARAD_PP_FP_INGRESS_QUAL_VERIFY,
   ARAD_PP_FP_IPV4_HOST_EXTEND_ENABLE,
   ARAD_PP_FP_CYCLE_FOR_DB_INFO_VERIFY,
   ARAD_PP_FP_IRE_TRAFFIC_SEND_VERIFY,
   ARAD_PP_FP_IRE_TRAFFIC_SEND_UNSAFE,


  
  ARAD_PP_FP_PROCEDURE_DESC_LAST
} ARAD_PP_FP_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_FP_PFG_NDX_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_FP_FIRST,
  ARAD_PP_FP_DB_ID_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_SUCCESS_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_ENTRY_ID_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_IS_FOUND_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  SOC_PPC_FP_FWD_TYPE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_HDR_FORMAT_BMP_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_VLAN_TAG_STRUCTURE_BMP_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_DB_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_SUPPORTED_PFGS_OUT_OF_RANGE_ERR,
  SOC_PPC_FP_QUAL_TYPES_OUT_OF_RANGE_ERR,
  SOC_PPC_FP_DATABASE_TYPE_OUT_OF_RANGE_ERR,
  SOC_PPC_FP_ACTION_TYPES_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_STRENGTH_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_VAL_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_PRIORITY_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_FLD_LSB_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_CST_VAL_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_NOF_BITS_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_NOF_FIELDS_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_BASE_VAL_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_DB_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR,
  
   ARAD_PP_FP_DB_ID_ALREADY_EXIST_ERR,
   ARAD_PP_FP_DB_ID_NOT_DIRECT_EXTRACTION_ERR,
   ARAD_PP_FP_DB_ID_NOT_LOOKUP_ERR,
   ARAD_PP_FP_ENTRY_QUAL_TYPE_NOT_IN_DB_ERR,
   ARAD_PP_FP_ENTRY_ACTION_TYPE_NOT_IN_DB_ERR,
   ARAD_PP_FP_ENTRY_ALREADY_EXIST_ERR,
   SOC_PPC_FP_QUAL_TYPES_NOT_PREDEFINED_ACL_KEY_ERR,
   ARAD_PP_FP_DB_CREATION_FOR_IPV4_HOST_EXTEND_ERR,
   ARAD_PP_FP_EGR_DATABASE_NOT_ALREADY_ADDED_ERR,
   SOC_PPC_FP_QUAL_TYPES_END_OF_LIST_ERR,
   SOC_PPC_FP_ACTION_TYPES_END_OF_LIST_ERR,
   ARAD_PP_FP_DB_ID_DIRECT_TABLE_ALREADY_EXIST_ERR,
   ARAD_PP_FP_ACTION_LENGTHS_OUT_OF_RANGE_ERR,
   ARAD_PP_FP_QUALS_LENGTHS_OUT_OF_RANGE_ERR,
   ARAD_PP_FP_EGRESS_QUAL_USED_FOR_INGRESS_DB_ERR,
   ARAD_PP_FP_TAG_ACTION_ALREADY_EXIST_ERR,
   ARAD_PP_FP_QUAL_NOT_EGRESS_ERR,
   ARAD_PP_FP_ACTIONS_MIXED_WITH_TAG_ERR,
   SOC_PPC_FP_PACKET_SIZE_OUT_OF_RANGE_ERR,
   SOC_PPC_FP_NOF_DBS_PER_BANK_OUT_OF_RANGE_ERR,
   ARAD_PP_FP_NOF_QUALS_PER_DB_OUT_OF_RANGE_ERR,
   ARAD_PP_FP_NOF_ACTIONS_PER_DB_OUT_OF_RANGE_ERR, 
   ARAD_PP_FP_STAGE_OUT_OF_RANGE_ERR,
   ARAD_PP_FP_PORT_PROFILE_ALREADY_EXIST_ERR,

  
  ARAD_PP_FP_ERR_LAST
} ARAD_PP_FP_ERR;





typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 value;
  
  uint32 mask;

} ARAD_PP_FP_DT_KEY;



typedef union
{
  ARAD_TCAM_LOCATION location;
  ARAD_PP_FP_DT_KEY entry_dt_key;
}ARAD_PP_FP_LOCATION_HASH;

typedef struct
{
  
  uint8 size_in_bytes;

  
  uint8 offset;

  SOC_PPC_FP_QUAL_TYPE qual_type;  

} ARAD_PP_FP_RANGE_QUAL_INFO;


#define MAX_NOF_SHARED_QUALIFIERS_PER_PROGRAM   4

typedef enum 
{
    ARAD_PP_FP_DIP_SIP_CLUSTERS_IPV4,
    ARAD_PP_FP_DIP_SIP_CLUSTERS_IPV6,
    ARAD_PP_FP_DIP_SIP_CLUSTERS_L2,

    ARAD_PP_FP_NOF_DIP_SIP_CLUSTERS
} ARAD_PP_FP_DIP_SIP_CLUSTERS;



#define ARAD_PP_FP_MAX_NOF_PROGRAMS_IN_DIP_SIP_CLUSTERS   6

typedef struct
{

  char  name[20];

  uint32 size; 

  SOC_PPC_FP_QUAL_TYPE qual_type;  

} ARAD_PP_FP_SHARED_QUAL_INFO;


void ARAD_PP_FP_SHARED_QUAL_INFO_clear(ARAD_PP_FP_SHARED_QUAL_INFO* shard_qualifiers_info);

typedef struct
{

  uint32 prog_id; 

  
  ARAD_PP_FP_SHARED_QUAL_INFO shared_quals[MAX_NOF_SHARED_QUALIFIERS_PER_PROGRAM];  

  uint32 nof_qualifiers; 

} ARAD_PP_FP_KEY_DIP_SIP_SHARING;


typedef struct
{

  char cluster_name[20];

  ARAD_PP_FP_DIP_SIP_CLUSTERS cluster_id;

  uint32 prog_id[ARAD_PP_FP_MAX_NOF_PROGRAMS_IN_DIP_SIP_CLUSTERS]; 

  
  ARAD_PP_FP_SHARED_QUAL_INFO shared_quals[MAX_NOF_SHARED_QUALIFIERS_PER_PROGRAM];  

  uint32 nof_qualifiers; 

} ARAD_PP_FP_DIP_SIP_CLUSTER_INFO;


void
  dip_sip_sharing_qual_info_get(int unit, int prog_id, SOC_PPC_FP_QUAL_TYPE qual_type, int* is_exists,  ARAD_PP_FP_SHARED_QUAL_INFO* shared_qual_info);

uint32
  arad_pp_fp_dip_sip_sharing_is_sharing_enabled_for_db(int unit, SOC_PPC_FP_DATABASE_STAGE stage, uint32 db_id_ndx, uint32* is_enabled, int* cluster_id);

void
  dip_sip_sharing_is_shared_qualifier_qset(int unit, int prog_id, SOC_PPC_FP_QUAL_TYPE qual_type, int* is_exists, ARAD_PP_FP_SHARED_QUAL_INFO* shared_qual_info);





uint32
  arad_pp_fp_key_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint8                     is_for_egress,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE           *qual_types,
    SOC_SAND_OUT SOC_PPC_FP_PREDEFINED_ACL_KEY  *predefined_acl_key
  );

uint32
  arad_pp_fp_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
    arad_pp_fp_db_stage_get(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32  db_id_ndx,
      SOC_SAND_OUT SOC_PPC_FP_DATABASE_STAGE  *stage
    );

uint32
    arad_pp_fp_db_cascaded_cycle_get(
          SOC_SAND_IN int unit,
          SOC_SAND_IN uint32  db_id_ndx,
          SOC_SAND_OUT uint8  *is_cascaded,
          SOC_SAND_OUT uint8  *lookup_id 
    );

uint32
    arad_pp_fp_db_is_equal_place_get(
          SOC_SAND_IN int unit,
          SOC_SAND_IN uint32  db_id_ndx,
          SOC_SAND_OUT uint8  *is_equal,
          SOC_SAND_OUT uint32 *place 
    );

uint32
  arad_pp_fp_key_length_get_unsafe(
    SOC_SAND_IN    int              unit,
    SOC_SAND_IN     SOC_PPC_FP_DATABASE_STAGE    stage,
    SOC_SAND_IN    SOC_PPC_FP_QUAL_TYPE     qual_type,
    SOC_SAND_IN    uint8              with_padding,
    SOC_SAND_OUT   uint32              *length
  );



uint32
  arad_pp_fp_packet_format_group_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_IN  SOC_PPC_PMF_PFG_INFO            *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  );

uint32
  arad_pp_fp_packet_format_group_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_IN  SOC_PPC_PMF_PFG_INFO            *info
  );

uint32
  arad_pp_fp_packet_format_group_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx
  );


uint32
  arad_pp_fp_packet_format_group_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_INOUT SOC_PPC_PMF_PFG_INFO            *info
  );
uint32
    arad_pp_fp_tcam_callback(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32  user_data
    );


uint32
  arad_pp_fp_action_bitmap_ndx_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                              db_id_ndx,
    SOC_SAND_OUT ARAD_TCAM_ACTION_SIZE               *action_bitmap_ndx
  );


uint32
  arad_pp_fp_database_create_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT  SOC_PPC_FP_DATABASE_INFO                    *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_fp_database_create_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_INFO                    *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );


uint32
  arad_pp_fp_database_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_OUT SOC_PPC_FP_DATABASE_INFO                    *info
  );

uint32
  arad_pp_fp_database_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx
  );


uint32
  arad_pp_fp_database_is_tm_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_INFO               *info,
    SOC_SAND_OUT uint8                                  *is_for_tm,
    SOC_SAND_OUT uint8                                  *is_default_tm
  );


uint32
  arad_pp_fp_database_destroy_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx
  );

uint32
  arad_pp_fp_database_destroy_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx
  );


uint32
  arad_pp_fp_entry_add_unsafe(
    SOC_SAND_IN     int                                 unit,
    SOC_SAND_IN     uint32                              db_id_ndx,
    SOC_SAND_IN     uint32                              entry_id_ndx,
    SOC_SAND_INOUT  SOC_PPC_FP_ENTRY_INFO               *info,
    SOC_SAND_OUT    SOC_SAND_SUCCESS_FAILURE            *success
  );

uint32
  arad_pp_fp_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  SOC_PPC_FP_ENTRY_INFO                       *info
  );


uint32
  arad_pp_fp_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_OUT uint8                                  *is_found,
    SOC_SAND_INOUT SOC_PPC_FP_ENTRY_INFO                *info
  );

uint32
  arad_pp_fp_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  );


uint32
  arad_pp_fp_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  uint32                                 is_sw_remove_only
  );

uint32
  arad_pp_fp_entry_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  );

uint32
  arad_pp_fp_entry_remove_by_key_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_PPC_FP_ENTRY_INFO                *info
  );


uint32
  arad_pp_fp_dip_sip_sharing_handle(SOC_SAND_IN int unit, 
                                    SOC_SAND_IN uint32 db_id_ndx, 
                                    SOC_SAND_IN SOC_PPC_FP_ENTRY_INFO* info,
                                    uint8*   elk_data, 
                                    uint8*   elk_mask);
uint32
  arad_pp_fp_entry_remove_by_key_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_PPC_FP_ENTRY_INFO                *info
  );


uint32
  arad_pp_fp_database_entries_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PPC_FP_ENTRY_INFO                       *entries,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_fp_database_entries_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  );


uint32
  arad_pp_fp_direct_extraction_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO              *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_fp_direct_extraction_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO              *info
  );


uint32
  arad_pp_fp_direct_extraction_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_OUT uint8                                 *is_found,
    SOC_SAND_OUT uint8                                 *fem_count,
    SOC_SAND_OUT SOC_PPC_FP_DIR_EXTR_ENTRY_INFO              *info
  );

uint32
  arad_pp_fp_direct_extraction_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  );


uint32
  arad_pp_fp_direct_extraction_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  );

uint32
  arad_pp_fp_direct_extraction_entry_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  );


uint32
  arad_pp_fp_direct_extraction_db_entries_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PPC_FP_DIR_EXTR_ENTRY_INFO              *entries,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_fp_direct_extraction_db_entries_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  );


uint32
  arad_pp_fp_control_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_INDEX       *control_ndx,
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_INFO        *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  );

uint32
  arad_pp_fp_control_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_INDEX      * control_ndx,
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_INFO        *info
  );

uint32
  arad_pp_fp_control_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_INDEX      * control_ndx
  );


uint32
  arad_pp_fp_control_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_INDEX      * control_ndx,
    SOC_SAND_OUT SOC_PPC_FP_CONTROL_INFO        *info
  );


uint32
  arad_pp_fp_egr_db_map_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx,
    SOC_SAND_IN  uint32                     db_id
  );

uint32
  arad_pp_fp_egr_db_map_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx,
    SOC_SAND_IN  uint32                     db_id
  );

uint32
  arad_pp_fp_egr_db_map_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx
  );


uint32
  arad_pp_fp_egr_db_map_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx,
    SOC_SAND_OUT uint32                     *db_id
  );


uint32
  arad_pp_fp_database_compress_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  db_id_ndx
  );


uint32
  arad_pp_fp_packet_diag_pgm_bmp_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE stage,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_OUT  uint32                *pmf_progs_bmp_final,
    SOC_SAND_OUT  uint32                pfg_bmp[ARAD_PMF_NOF_LINES_MAX_ALL_STAGES]
  );


uint32
  arad_pp_fp_packet_diag_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT SOC_PPC_FP_PACKET_DIAG_INFO *info
  );


uint32
  arad_pp_fp_get_dbs_for_action(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                      flags,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE      in_action_type,
    SOC_SAND_IN  uint32                      db_identifier_size,
    SOC_SAND_OUT uint32                      *db_identifier_p,
    SOC_SAND_OUT uint32                      *db_identifier_size_actual
  ) ;


uint32
  arad_pp_fp_is_action_on_any_db(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE      in_action_type,
    SOC_SAND_OUT uint32                      *db_identifier_p
  ) ;


uint32
  arad_pp_fp_dbs_action_info_show_unsafe(
    SOC_SAND_IN  int unit
  ) ;

uint32
  arad_pp_fp_action_info_show_unsafe(
    SOC_SAND_IN  int unit
  ) ;


uint32
  arad_pp_fp_resource_diag_get_verify(
    SOC_SAND_IN  int                 		    unit,
    SOC_SAND_IN  SOC_PPC_FP_RESOURCE_DIAG_MODE	mode,
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_DIAG_INFO	*info
  );

uint32
  arad_pp_fp_resource_diag_get_unsafe(
    SOC_SAND_IN  int                 		unit,
	SOC_SAND_IN  SOC_PPC_FP_RESOURCE_DIAG_MODE	mode,
    SOC_SAND_OUT SOC_PPC_FP_RESOURCE_DIAG_INFO 	*info
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_fp_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_fp_get_errs_ptr(void);

uint32
  SOC_PPC_FP_QUAL_VAL_verify(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  SOC_PPC_FP_QUAL_VAL *info,
      SOC_SAND_IN     SOC_PPC_FP_DATABASE_STAGE    stage
  );

uint32
  SOC_PPC_PMF_PFG_INFO_verify(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  SOC_PPC_PMF_PFG_INFO *info
  );

uint32
  SOC_PPC_FP_DATABASE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_INFO *info
  );

uint32
  SOC_PPC_FP_ACTION_VAL_verify(
    SOC_SAND_IN  SOC_PPC_FP_ACTION_VAL *info
  );

uint32
  SOC_PPC_FP_ENTRY_INFO_verify(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  SOC_PPC_FP_ENTRY_INFO *info,
      SOC_SAND_IN     SOC_PPC_FP_DATABASE_STAGE    stage
  );

uint32
  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_verify(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO *info,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE    stage,
      SOC_SAND_IN  uint8                       alloc_fes_flag
  );

uint32
  SOC_PPC_FP_CONTROL_INDEX_verify(
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_INDEX *info
  );

uint32
  SOC_PPC_FP_CONTROL_INFO_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_TYPE type,
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_INFO *info
  );


soc_error_t
    arad_pp_fp_presel_max_id_get(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE   stage,
      SOC_SAND_OUT int                   *presel_max_id 
    );

uint32
arad_pp_fp_pp_test1(
    SOC_SAND_IN  int                           unit);

uint32
    arad_pp_fp_ire_traffic_send_verify(
        SOC_SAND_IN int          unit,
        SOC_SAND_IN SOC_PPC_FP_PACKET  *packet
    );

uint32
    arad_pp_fp_ire_traffic_send_unsafe(
        SOC_SAND_IN int          unit,
        SOC_SAND_IN SOC_PPC_FP_PACKET  *packet,
        SOC_SAND_IN int          core
    );

uint32
  arad_pp_fp_rewrite_entry(
     SOC_SAND_IN  int                        unit,
     SOC_SAND_IN  uint8                      entry_exists,
     SOC_SAND_IN  ARAD_TCAM_GLOBAL_LOCATION  *global_location,
     SOC_SAND_IN  ARAD_TCAM_LOCATION         *location
  );

uint32
  arad_pp_fp_qual_type_preset(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_PREDEFINED_ACL_KEY  predefined_key,
    SOC_SAND_OUT SOC_PPC_FP_QUAL_TYPE           qual_types[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]
  );

uint32
  arad_pp_fp_qual_val_encode(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_VAL_ENCODE_INFO *qual_val_encode,
    SOC_SAND_OUT SOC_PPC_FP_QUAL_VAL             *qual_val
  );

uint32
  arad_pp_fp_select_line_clear_unsafe(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN int                    core_id
    );

uint32
  arad_pp_fp_select_line_get_unsafe(
    SOC_SAND_IN     int                         unit,
    SOC_SAND_IN     int                         core_id,
    SOC_SAND_OUT    SOC_PPC_FP_SELECT_LINE_INFO *line_info
    );

uint32
  arad_pp_fp_last_fem_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                         fem_ndx
  );

uint32
  arad_pp_fp_last_fem_get_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  int                           core_id,
    SOC_SAND_IN  uint32                        fem_ndx,
    SOC_SAND_OUT SOC_PPC_FP_LAST_FEM_INOUT_INFO  *info
  );

uint32
  arad_pp_fp_last_fes_get_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  int                           core_id,
    SOC_SAND_IN  uint32                        fes_ndx,
    SOC_SAND_OUT SOC_PPC_FP_LAST_FES_INOUT_INFO  *info
  );

uint32
  arad_pp_fp_ce_info_get_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE         stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                             is_second_lookup);

uint32
  arad_pp_fp_ce_info_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE         stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                             is_second_lookup,
    SOC_SAND_OUT SOC_PPC_FP_CE_INFO                *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


