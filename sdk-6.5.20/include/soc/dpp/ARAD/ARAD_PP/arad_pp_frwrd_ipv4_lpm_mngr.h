
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_PP_IPV4_LPM_INCLUDED__

#define __ARAD_PP_IPV4_LPM_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/SAND/Utils/sand_group_member_list.h>
#include <soc/dpp/SAND/Utils/sand_pat_tree.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_array_memory_allocator.h>
#include <soc/dpp/PPC/ppc_api_frwrd_ipv4.h>





                                                                   








typedef SOC_SAND_U64 ARAD_PP_IPV4_LPM_MNGR_INST;


typedef enum {
  ARAD_PP_IPV4_LPM_PXX_MODEL_UNKNOWN = 0,
  ARAD_PP_IPV4_LPM_PXX_MODEL_P4X = 1, 
  ARAD_PP_IPV4_LPM_PXX_MODEL_P6X = 2,  
  ARAD_PP_IPV4_LPM_PXX_MODEL_P6N = 3, 
  ARAD_PP_IPV4_LPM_PXX_MODEL_P4X_COMPLETE = 4, 
  ARAD_PP_IPV4_LPM_PXX_MODEL_P6M = 5, 
  ARAD_PP_IPV4_LPM_PXX_MODEL_LP4 = 6  
} ARAD_PP_IPV4_LPM_PXX_MODEL;



typedef
  uint32
    (*ARAD_PP_IPV4_LPM_MNGR_SW_DB_MEM_ALLOCATOR_GET)(
      SOC_SAND_IN  int                      prime_handle,
      SOC_SAND_IN  uint32                      sec_handle,
      SOC_SAND_IN  uint32                      memory_ndx,
      SOC_SAND_OUT ARAD_PP_ARR_MEM_ALLOCATOR_INFO   **mem_allocator
    );



typedef
  uint32
    (*ARAD_PP_IPV4_LPM_MNGR_SW_DB_PAT_TREE_GET)(
      SOC_SAND_IN  int                      prime_handle,
      SOC_SAND_IN  uint32                      sec_handle,
      SOC_SAND_IN  uint32                      vrf_ndx,
      SOC_SAND_OUT SOC_SAND_PAT_TREE_INFO            **pat_tree
    );

typedef
  int
    (*ARAD_PP_IPV4_LPM_MNGR_SW_DB_NOF_ENTRIES_FOR_HW_LPM_SET_FUN)(
      SOC_SAND_IN int                     unit,
      SOC_SAND_IN uint32                      nof_lpm_entries_in_lpm
   );

typedef
  int
    (*ARAD_PP_IPV4_LPM_MNGR_SW_DB_NOF_ENTRIES_FOR_HW_LPM_GET_FUN)(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_OUT  uint32                    *nof_lpm_entries_in_lpm
    );

typedef struct {
  SOC_SAND_MAGIC_NUM_VAR

  uint32 *bit_depth_per_bank;


} ARAD_PP_IPV4_LPM_MNGR_T;

#define ARAD_PP_LPV4_LPM_SUPPORT_CACHE  (1)
#define ARAD_PP_LPV4_LPM_SUPPORT_DEFRAG  (2)
#define ARAD_PP_LPV4_LPM_SHARED_MEM      (4)



typedef enum {
  ARAD_PP_IPV4_LPM_MEMORY_1,
  ARAD_PP_IPV4_LPM_MEMORY_2,
  ARAD_PP_IPV4_LPM_MEMORY_3,
  ARAD_PP_IPV4_LPM_MEMORY_4,
  ARAD_PP_IPV4_LPM_MEMORY_5,
  ARAD_PP_IPV4_LPM_MEMORY_6,

  ARAD_PP_IPV4_LPM_NOF_MEMS

}ARAD_PP_IPV4_LPM_MEMORY;


typedef struct {
  SOC_SAND_MAGIC_NUM_VAR

 
  int prime_handle;
  
  uint32 sec_handle;
  
  uint32 nof_vrf_bits;
  
  uint32 nof_banks;
  
  uint32 *nof_bits_per_bank;
  
  uint32 *bank_to_mem;
  uint32 *mem_to_bank;
  uint32 flags;
  
  uint32 nof_mems;
  
  uint32 *nof_rows_per_mem;
  
  ARAD_PP_IPV4_LPM_PXX_MODEL pxx_model;
  
  ARAD_PP_ARR_MEM_ALLOCATOR_INFO *mem_allocators;

  
  SOC_SAND_GROUP_MEM_LL_INFO **rev_ptrs;

  
  SOC_SAND_PAT_TREE_INFO *lpms;
  uint32 nof_lpms;
  
  ARAD_PP_IPV4_LPM_MNGR_SW_DB_MEM_ALLOCATOR_GET mem_alloc_get_fun;

  ARAD_PP_IPV4_LPM_MNGR_SW_DB_PAT_TREE_GET pat_tree_get_fun;

  ARAD_PP_IPV4_LPM_MNGR_SW_DB_NOF_ENTRIES_FOR_HW_LPM_SET_FUN
    nof_entries_for_hw_lpm_set_fun;
  ARAD_PP_IPV4_LPM_MNGR_SW_DB_NOF_ENTRIES_FOR_HW_LPM_GET_FUN
    nof_entries_for_hw_lpm_get_fun;
  
  uint32
    max_nof_entries_for_hw_lpm;
} ARAD_PP_IPV4_LPM_MNGR_INIT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  ARAD_PP_IPV4_LPM_MNGR_INIT_INFO
  init_info;

  ARAD_PP_IPV4_LPM_MNGR_T
    data_info;
} ARAD_PP_IPV4_LPM_MNGR_INFO;



typedef struct {
  SOC_SAND_MAGIC_NUM_VAR
  uint32  total_nodes;
  uint32  prefix_nodes;
  uint32  free_nodes;
} ARAD_PP_IPV4_LPM_MNGR_STATUS;


uint32
  arad_pp_ipv4_lpm_mngr_create(
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr,
    SOC_SAND_IN  int                   unit
  );


uint32
  arad_pp_ipv4_lpm_mngr_vrf_init(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr,
    SOC_SAND_IN  uint32                    vrf_ndx,
    SOC_SAND_IN  SOC_SAND_PP_SYSTEM_FEC_ID       default_fec
  );


uint32
  arad_pp_ipv4_lpm_mngr_vrf_clear(
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr,
    SOC_SAND_IN  uint32                    vrf_ndx,
    SOC_SAND_IN  SOC_SAND_PP_SYSTEM_FEC_ID       default_fec,
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_ROUTE_STATUS   pending_type
  );


uint32
  arad_pp_ipv4_lpm_mngr_vrf_routes_clear(
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr,
    SOC_SAND_IN  uint32                    vrf_ndx,
    SOC_SAND_IN  SOC_SAND_PP_SYSTEM_FEC_ID       default_sys_fec,
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_ROUTE_STATUS   pending_type
  );


uint32
  arad_pp_ipv4_lpm_mngr_all_vrfs_clear(
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr,
    SOC_SAND_IN  uint32                    nof_vrfs,
    SOC_SAND_IN  SOC_SAND_PP_SYSTEM_FEC_ID       default_fec
  );

uint32 arad_pp_ipv4_lpm_mngr_clear(
  SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr
);


uint32
  arad_pp_ipv4_lpm_mngr_destroy(
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr
  );


uint32
  arad_pp_ipv4_lpm_mngr_hw_sync(
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr,
    SOC_SAND_IN  uint32                                      vrf_ndx
  );


uint32
  arad_pp_ipv4_lpm_mngr_mark_non_lpm_route_pending_type_set(
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr,
    SOC_SAND_IN  uint32                       vrf_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET            *key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_ROUTE_STATUS   pending_type
  );


uint32
  arad_pp_ipv4_lpm_mngr_prefix_add(
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO    *lpm_mngr,
    SOC_SAND_IN  uint32                      vrf_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET           *key,
    SOC_SAND_IN  SOC_SAND_PP_SYSTEM_FEC_ID         sys_fec_id,
    SOC_SAND_IN  uint8                    is_pending_op,
    SOC_SAND_IN SOC_PPC_FRWRD_IP_ROUTE_LOCATION hw_target_type,
    SOC_SAND_OUT  uint8                    *success
  );
    

uint32
  arad_pp_ipv4_lpm_mngr_sync(
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO    *lpm_mngr,
    SOC_SAND_IN  uint32                         vrf_ndx,
    SOC_SAND_IN  uint32                         *vrf_bitmap,
    SOC_SAND_IN  uint32                         nof_vrfs,
    SOC_SAND_IN  uint8                         update_lpm,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE            *success
  );

uint32
  arad_pp_ipv4_lpm_mngr_cache_set(
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO    *lpm_mngr,
    SOC_SAND_IN  uint32                         vrf_ndx,
    SOC_SAND_IN  uint32                          nof_vrfs,
    SOC_SAND_IN  uint8                         enable
  );

uint32
  arad_pp_ipv4_lpm_mngr_prefix_remove(
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr,
    SOC_SAND_IN  uint32                     vrf_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET          *key,
    SOC_SAND_IN  uint8                    is_pending_op,
    SOC_SAND_OUT  uint8                   *success
  );


uint32
  arad_pp_ipv4_lpm_mngr_prefix_lookup(
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr,
    SOC_SAND_IN  uint32                     vrf_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET          *key,
    SOC_SAND_OUT  SOC_SAND_PP_IPV4_SUBNET         *found_key,
    SOC_SAND_OUT  uint8                   *exact_match,
    SOC_SAND_OUT  uint8                   *found
  );


uint32
  arad_pp_ipv4_lpm_mngr_prefix_is_exist(
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr,
    SOC_SAND_IN  uint32                     vrf_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET          *key,
    SOC_SAND_OUT  uint8                   *exist
  );


uint32 arad_pp_ipv4_lpm_mngr_sys_fec_get(
  SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr,
  SOC_SAND_IN  uint32                     vrf_ndx,
  SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET           *key,
  SOC_SAND_IN  uint8                     exact_match,
  SOC_SAND_OUT  SOC_SAND_PP_SYSTEM_FEC_ID        *sys_fec_id,
  SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS *pending_type,
  SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION *hw_target_type,
  SOC_SAND_OUT  uint8                    *found
);


uint32
  arad_pp_ipv4_lpm_mngr_get_stat(
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO   *lpm_mngr,
    SOC_SAND_IN  uint32                      vrf_ndx,
    SOC_SAND_OUT  ARAD_PP_IPV4_LPM_MNGR_STATUS  *lpm_stat
  );

uint32
  arad_pp_ipv4_lpm_mngr_lookup0(
    SOC_SAND_IN ARAD_PP_IPV4_LPM_MNGR_INFO    *lpm_mngr,
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32         vrf_ndx,
    SOC_SAND_IN  uint32         addr,
    SOC_SAND_OUT uint32        *lookup
  );

uint32
  arad_pp_ipv4_lpm_mngr_get_block(
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr,
    SOC_SAND_IN  uint32                     vrf_ndx,
    SOC_SAND_IN SOC_SAND_PAT_TREE_ITER_TYPE       iter_type,
    SOC_SAND_INOUT  SOC_SAND_U64                  *iter,
    SOC_SAND_IN  uint32                     entries_to_scan,
    SOC_SAND_IN  uint32                     entries_to_get,
    SOC_SAND_OUT  SOC_SAND_PP_IPV4_SUBNET         *route_table,
    SOC_SAND_OUT  uint32                    *sys_fec_ids,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS *pending_type,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION *hw_target_type,
    SOC_SAND_OUT uint32                     *nof_entries
  );
 
void
  arad_pp_ipv4_lpm_entry_decode(
    SOC_SAND_IN ARAD_PP_IPV4_LPM_MNGR_INFO      *lpm_mngr,
    SOC_SAND_IN  uint32                       depth,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY   *data,
    SOC_SAND_IN  uint32                       data_len,
    SOC_SAND_OUT  uint32                       *raw
  ) ;

uint32
  arad_pp_ipv4_lpm_mngr_row_to_base_addr(
    SOC_SAND_IN  ARAD_PP_IPV4_LPM_MNGR_INFO *lpm_mngr,
    SOC_SAND_IN ARAD_PP_IPV4_LPM_MNGR_INST *inst,
    SOC_SAND_IN uint32 depth ,
    SOC_SAND_OUT int32 *result
  );

int32
  arad_pp_ipv4_lpm_mngr_update_base_addr(
    SOC_SAND_IN  ARAD_PP_IPV4_LPM_MNGR_INFO *lpm_mngr,
    SOC_SAND_INOUT ARAD_PP_IPV4_LPM_MNGR_INST *inst,
    SOC_SAND_IN uint32 depth,
    SOC_SAND_IN uint32 new_ptr
  );

void
  arad_pp_ARAD_PP_IPV4_LPM_MNGR_INFO_clear(
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *info
  );

void
  arad_pp_ARAD_PP_IPV4_LPM_MNGR_STATUS_clear(
    SOC_SAND_INOUT  ARAD_PP_IPV4_LPM_MNGR_STATUS  *info
  );

uint8
  arad_pp_ipv4_lpm_mngr_pat_tree_node_data_is_identical_fun(
  SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info_0,
  SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info_1
  );

uint8
  arad_pp_ipv4_lpm_mngr_pat_node_is_skip_in_lpm_identical_data_query_fun(
    SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info
  );

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif

