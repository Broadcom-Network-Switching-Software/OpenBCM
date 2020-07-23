/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#ifndef __ARAD_PP_SW_DB_INCLUDED__

#define __ARAD_PP_SW_DB_INCLUDED__




#include <shared/swstate/sw_state.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <soc/dpp/SAND/Utils/sand_hashtable.h>
#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#include <soc/dpp/SAND/Utils/sand_sorted_list.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif_cos.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_vid_assign.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ilm.h>
#include <soc/dpp/PPC/ppc_api_frwrd_ipv4.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4_lpm_mngr.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_filter.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_fec.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_mirror.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_slb.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_extender.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_rif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_vsi.h>
#include <soc/dpp/ARAD/arad_parser.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_ptp.h>

#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#include <soc/dpp/SAND/Utils/sand_sorted_list.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_sw_db.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/port_map.h>

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#endif






#define ARAD_PP_SW_DB_TYPE_BITS (4)

#define ARAD_PP_SW_DB_HASH_KEY_TABLE_PART_LEN_BYTES             (4)
#define ARAD_PP_SW_DB_HASH_KEY_LEN_BYTES   (ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2)+4

typedef enum
{
  
  ARAD_PP_SW_DB_MULTI_SET_L3_PROTOCOL = ARAD_NOF_SW_DB_MULTI_SETS, 
  
  ARAD_PP_SW_DB_MULTI_SET_LLP_MIRROR_PROFILE,

  
  ARAD_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE,
  
   ARAD_PP_SW_DB_MULTI_SET_ETHER_TYPE,
  
   ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,

   ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_GLOBAL_METER_PROFILE,

#if defined(BCM_88675_A0)
   
  JER_PP_SW_DB_MULTI_SET_POLICER_SIZE_PROFILE,
#endif  

   ARAD_PP_SW_DB_MULTI_SET_L2_LIF_VLAN_COMPRESSION_RANGE,

 
  ARAD_PP_SW_DB_MULTI_SET_L2_LIF_PROFILE,

 
  ARAD_PP_SW_DB_MULTI_SET_EG_ENCAP_PROG_DATA_ENTRY, 

  
  ARAD_PP_SW_DB_MULTI_SET_ENTRY_OVERLAY_ARP_PROG_DATA_ENTRY

} ARAD_PP_SW_DB_MULTI_SET;





#define ARAD_PP_SW_DB_FIELD_SET(unit, field_name, val)        \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  if (Arad_pp_sw_db.device[unit] == NULL)        \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      &(Arad_pp_sw_db.device[unit]->field_name), \
      val,                                                        \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;

#define ARAD_PP_SW_DB_FIELD_GET(unit, field_name, val)        \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  if (Arad_pp_sw_db.device[unit] == NULL)        \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      val,                                                        \
      &(Arad_pp_sw_db.device[unit]->field_name), \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;

#define ARAD_PP_SW_DB_GLOBAL_FIELD_SET(field_name, val)            \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      &(Arad_pp_sw_db.field_name),                                 \
      val,                                                        \
      sizeof(*val)                                                \
    );                                                            \
                                                                  \
  return soc_sand_ret;

#define ARAD_PP_SW_DB_GLOBAL_FIELD_GET(field_name, val)            \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      val,                                                        \
      &(Arad_pp_sw_db.field_name),                                 \
      sizeof(*val)                                                \
    );                                                            \
                                                                  \
  return soc_sand_ret;


#define ARAD_PP_SW_DB_IPV4_FIELD_SET(unit, field_name, val)        \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  if (Arad_pp_sw_db.device[unit] == NULL || Arad_pp_sw_db.device[unit]->ipv4_info == NULL)        \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      &(Arad_pp_sw_db.device[unit]->field_name), \
      val,                                                        \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;

#define ARAD_PP_SW_DB_IPV4_FIELD_GET(unit, field_name, val)        \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  if (Arad_pp_sw_db.device[unit] == NULL || Arad_pp_sw_db.device[unit]->ipv4_info == NULL)        \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      val,                                                        \
      &(Arad_pp_sw_db.device[unit]->field_name), \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;
#define ARAD_PP_ALLOC_VAR(var, type, count)                                           \
  do                                                                                \
  {                                                                                 \
    if ((var = (type*)soc_sand_os_malloc(count * sizeof(type))) == NULL)                \
    {                                                                               \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 2, exit);                               \
    }                                                                               \
  } while(0)



#define ARAD_PP_VDC_MAP_ISID_GET(map_info) ((map_info) & 0xFFF)
#define ARAD_PP_VDC_MAP_PORT_VD_GET(map_info) ((map_info) >> 24)
#define ARAD_PP_VDC_MAP_INFO_SET(port_class, isid) ((port_class) << 24 | (isid))






typedef enum
{
  
  ARAD_PP_SW_DB_GET_PROCS_PTR = ARAD_PP_PROC_DESC_BASE_SW_DB_FIRST,
  ARAD_PP_SW_DB_GET_ERRS_PTR,
  

  ARAD_PP_SW_DB_INIT,
  ARAD_PP_SW_DB_DEVICE_INIT,
  ARAD_PP_SW_DB_DEVICE_CLOSE,

  
  ARAD_PP_SW_DB_PROCEDURE_DESC_LAST
} ARAD_PP_SW_DB_PROCEDURE_DESC;

typedef enum
{
  
  

  
  ARAD_PP_SW_DB_ERR_LAST
} ARAD_PP_SW_DB_ERR;

typedef struct
{
  SOC_SAND_MULTI_SET_PTR l3_protocols_multi_set;
} ARAD_PP_SW_DB_LLP_TRAP;

typedef struct
{
  uint8 llp_mirror_port_vlan[SOC_DPP_DEFS_MAX(NOF_CORES)][ARAD_PORT_NOF_PP_PORTS]; 
  SOC_SAND_MULTI_SET_PTR mirror_profile_multi_set;
} ARAD_PP_SW_DB_LLP_MIRROR;

typedef struct
{
  uint8 eg_mirror_port_vlan[SOC_DPP_DEFS_MAX(NOF_CORES)][ARAD_PORT_NOF_PP_PORTS]; 
  SOC_SAND_MULTI_SET_PTR mirror_profile_multi_set;
} ARAD_PP_SW_DB_EG_MIRROR;

typedef struct
{
  SOC_SAND_MULTI_SET_PTR prog_data;
} ARAD_PP_SW_DB_EG_ENCAP;

typedef struct
{
  SOC_SAND_MULTI_SET_PTR ether_type_multi_set;
} ARAD_PP_SW_DB_LLP_COS;

typedef struct
{
  SOC_SAND_MULTI_SET_PTR       eth_meter_profile_multi_set; 
  SOC_SAND_MULTI_SET_PTR       global_meter_profile_multi_set;
  SHR_BITDCL                   config_meter_status[ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BYTE];
} ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE;

typedef struct
{
  SHR_BITDCL lif_use[ARAD_BIT_TO_U32(SOC_DPP_DEFS_MAX(NOF_LOCAL_LIFS) * ARAD_PP_SW_DB_TYPE_BITS)];

  
  SHR_BITDCL lif_sub_use[ARAD_BIT_TO_U32(SOC_DPP_DEFS_MAX(NOF_LOCAL_LIFS) * ARAD_PP_SW_DB_TYPE_BITS)];

} ARAD_PP_SW_DB_LIF_TABLE;

typedef struct
{
  SOC_SAND_MULTI_SET_PTR       vlan_compression_range_multi_set;
} ARAD_PP_SW_DB_L2_LIF;

typedef struct
{
  SOC_SAND_MULTI_SET_PTR       ac_key_map_multi_set;
} ARAD_PP_SW_DB_L2_LIF_AC;

typedef struct
{
  uint16 vrrp_mac_use_bit_map[ARAD_PP_VRRP_NOF_MAC_ADDRESSES/16];
} ARAD_PP_SW_DB_VRRP_INFO;


typedef struct 
{
  SOC_SAND_GROUP_MEM_LL_INFO group_info;

} ARAD_PP_SW_DB_RIF_TO_LIF_GROUP_MAP;

typedef struct {
  PARSER_HINT_ARR uint32 *cur_size;
  PARSER_HINT_ARR uint32 *max_size;
} ARAD_PP_SW_DB_ECMP;


typedef struct
{
  uint8 bank_id;
  uint32 address;
} ARAD_PP_IPV4_LPM_FREE_LIST_ITEM_INFO;


typedef struct
{
  ARAD_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  uint32
    *cm_buf_array;
  uint32
    cm_buf_entry_words;
  ARAD_PP_IPV4_LPM_FREE_LIST_ITEM_INFO
    *free_list;
  uint32
    free_list_size;

}  ARAD_PP_SW_DB_IPV4_INFO;


typedef struct
{
  SOC_SAND_MULTI_SET_PTR prog_data;
} ARAD_PP_SW_OVERLAY_ARP;

typedef struct
{
  PARSER_HINT_ARR SOC_SAND_OCC_BM_PTR *occ_bm_hndl;
  int nof;
  PARSER_HINT_ALLOW_WB_ACCESS int counter;
} ARAD_PP_SW_DB_IPV4_PAT_TREE;

typedef struct
{
  ARAD_PP_SW_DB_IPV4_INFO *ipv4_info;


  ARAD_PP_SW_DB_RIF_TO_LIF_GROUP_MAP *rif_to_lif_group_map;

} ARAD_PP_SW_DB_DEVICE;

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
typedef struct
{
  JER_KAPS_DB_HANDLES          db_info[JER_KAPS_IP_NOF_DB];
  JER_KAPS_DMA_DB_HANDLES      dma_db_info[JER_KAPS_NOF_MAX_DMA_DB];
  JER_KAPS_INSTRUCTION_HANDLES search_instruction_info[JER_KAPS_NOF_SEARCHES];
  JER_KAPS_TABLE_HANDLES       table_handles[JER_KAPS_IP_NOF_TABLES];
} JER_KAPS;
#endif 

#if defined(BCM_88675_A0)
typedef struct
{
  SOC_SAND_MULTI_SET_PTR       policer_size_profile_multi_set; 
} JER_PP_SW_DB_POLICER_SIZE_PROFILE;
#endif 


typedef struct soc_arad_sw_state_pp_s{
    PARSER_HINT_PTR    ARAD_PP_SW_DB_ECMP                *ecmp_info;
    PARSER_HINT_PTR    SOC_DPP_DBAL_INFO                 *dbal_info;
    PARSER_HINT_PTR    ARAD_LLP_VID_ASSIGN_t             *llp_vid_assign;
    PARSER_HINT_PTR    ARAD_PP_LLP_FILTER_t              *llp_filter;
    PARSER_HINT_PTR    ARAD_PON_DOUBLE_LOOKUP            *pon_double_lookup;
    PARSER_HINT_PTR    ARAD_PP_SW_DB_VRRP_INFO           *vrrp_info;
    PARSER_HINT_PTR    ARAD_PP_SW_DB_LIF_TABLE           *lif_table;
    PARSER_HINT_PTR    ARAD_PP_HEADER_DATA               *header_data;
    PARSER_HINT_PTR    ARAD_PP_FWD_MACT                  *fwd_mact;
    PARSER_HINT_PTR    ARAD_PP_LAG                       *lag;
    PARSER_HINT_PTR    ARAD_PP_ILM_INFO                  *ilm_info;
    PARSER_HINT_PTR    ARAD_PP_OAM_MY_MAC_LSB            *oam_my_mac_lsb;
    PARSER_HINT_PTR    ARAD_PP_SW_DB_DIAG                *diag;
    PARSER_HINT_PTR    ARAD_PP_SW_DB_LIF_COS             *lif_cos;
    PARSER_HINT_PTR    ARAD_PP_OCCUPATION                *occupation;
    PARSER_HINT_PTR    ARAD_PP_FEC                       *fec;
    PARSER_HINT_PTR    ARAD_PP_SLB_CONFIG                *slb_config;
    PARSER_HINT_PTR    ARAD_SRC_BINDS                    *arad_sw_db_src_binds;
    PARSER_HINT_PTR    soc_dpp_pp_port_map               *pp_port_map;
    PARSER_HINT_PTR    arad_pp_extender_info_t           *extender_info;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    PARSER_HINT_PTR PARSER_HINT_ALLOW_WB_ACCESS JER_KAPS *kaps_db;
#endif 
    PARSER_HINT_PTR    ARAD_VTT                          *vtt;
    PARSER_HINT_PTR    ARAD_VTT                          *vtt_mask;
    PARSER_HINT_PTR    ARAD_FRWRD_IP                     *frwrd_ip;
    PARSER_HINT_PTR    ARAD_PP_SW_DB_L2_LIF              *l2_lif;
    PARSER_HINT_PTR    ARAD_PP_SW_DB_L2_LIF_AC           *l2_lif_ac;
    PARSER_HINT_ARR	   ARAD_PP_SW_DB_ETH_POLICER_MTR_PROFILE *eth_policer_mtr_profile;
    PARSER_HINT_PTR    ARAD_PP_SW_DB_LLP_COS             *llp_cos;
    PARSER_HINT_PTR    ARAD_PP_SW_DB_EG_ENCAP            *eg_encap;
    PARSER_HINT_PTR    ARAD_PP_SW_DB_LLP_MIRROR          *llp_mirror;
    PARSER_HINT_PTR    ARAD_PP_SW_DB_EG_MIRROR           *eg_mirror;
    PARSER_HINT_PTR    ARAD_PP_SW_DB_LLP_TRAP            *llp_trap;
    PARSER_HINT_PTR    ARAD_PP_SW_DB_IPV4_PAT_TREE       *ipv4_pat;
    PARSER_HINT_PTR    ARAD_PP_OAM_SW_STATE              *oam;
    PARSER_HINT_PTR    ARAD_PP_OAMP_PE_SW_STATE          *oamp_pe;
    PARSER_HINT_PTR    ARAD_PP_BFD_SW_STATE              *bfd;
    PARSER_HINT_PTR    ARAD_PP_MGMT_OPERATION_MODE       *oper_mode;
    PARSER_HINT_PTR    ARAD_PP_SW_OVERLAY_ARP            *overlay_arp;
    PARSER_HINT_PTR    ARAD_PP_IPV4_INFO                 *ipv4_info;
    PARSER_HINT_PTR    ARAD_PP_RIF_TO_LIF_GROUP_MAP      *rif_to_lif_group_map;
    PARSER_HINT_ARR    ARAD_PP_VDC_VSI_MAP               *vdc_vsi_map;
    PARSER_HINT_PTR    soc_dpp_vt_profile_info_t         *vt_profile_info;
    PARSER_HINT_PTR    dpp_parser_info_t                 *dpp_parser_info;
#if defined(BCM_88675_A0)
	PARSER_HINT_ARR	   JER_PP_SW_DB_POLICER_SIZE_PROFILE *policer_size_profile;
#endif 
    PARSER_HINT_PTR    ARAD_PP_PTP_SW_STATE      *ptp;
} soc_arad_sw_state_pp_t;

typedef struct
{
  uint8
    init;

  ARAD_PP_SW_DB_DEVICE*
    device[SOC_SAND_MAX_DEVICE];

} ARAD_PP_SW_DB;













uint32
  arad_pp_sw_db_init(void);

uint32
  arad_pp_sw_db_device_init(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE    *oper_mode
  );

uint32
  arad_pp_sw_db_device_close(
    SOC_SAND_IN int unit
  );




int
  arad_pp_sw_db_multiset_by_type_get(
    SOC_SAND_IN  int                       unit,
	SOC_SAND_IN	 int					   core_id,
    SOC_SAND_IN  ARAD_PP_SW_DB_MULTI_SET   multiset,
    SOC_SAND_OUT SOC_SAND_MULTI_SET_PTR*   multi_set_info
  );




uint32
  arad_pp_sw_db_llp_filter_initialize(
    SOC_SAND_IN  int unit
    );






uint32
  arad_pp_sw_db_ipv4_initialize(
   SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  nof_vrfs,
    SOC_SAND_IN uint32  *max_nof_routes,
    SOC_SAND_IN uint32  nof_vrf_bits,
    SOC_SAND_IN uint32  nof_banks,
    SOC_SAND_IN uint32  *nof_bits_per_bank,
    SOC_SAND_IN uint32  *bank_to_mem,
    SOC_SAND_IN uint32  *mem_to_bank,
    SOC_SAND_IN uint32  nof_mems,
    SOC_SAND_IN uint32  *nof_rows_per_mem, 
    SOC_SAND_IN ARAD_PP_IPV4_LPM_PXX_MODEL pxx_model,
    SOC_SAND_IN uint32  flags,
    SOC_SAND_IN SOC_SAND_PP_SYSTEM_FEC_ID  default_sys_fec
  );

uint32
  arad_pp_sw_db_ipv4_terminate(
    SOC_SAND_IN  int unit
  );

uint32
  arad_pp_sw_db_ipv4_cache_mode_for_ip_type_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32                                     type,
    SOC_SAND_OUT uint8	                          	     *pending_op
  );

uint32
  arad_pp_sw_db_ipv4_cache_vrf_modified_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN  uint32                                    vrf_ndx,
    SOC_SAND_IN uint32                                     is_modified
  );

uint32
  arad_pp_sw_db_ipv4_lpm_dma_buff_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    **dma_buff
  );

uint32
  arad_pp_sw_db_free_list_commit(
    SOC_SAND_IN  int            unit
  );

uint32
  arad_pp_sw_db_free_list_add(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            bank_id,
    SOC_SAND_IN  uint32             address
  );

int
  arad_pp_sw_db_ipv4_is_vrf_exist(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                    vrf_ndx,
    SOC_SAND_OUT uint8                    *is_vrf_exist
  );

int
  arad_pp_sw_db_ipv4_is_routing_enabled(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT uint8                  *is_routing_enabled
  );


uint32
  arad_pp_sw_db_ipv4_is_shared_lpm_memory(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN uint32                   vrf_ndx,
    SOC_SAND_OUT uint8                  *shared
  );

uint32
  arad_pp_sw_db_ipv4_lpm_mngr_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN ARAD_PP_IPV4_LPM_MNGR_INFO    *lpm_mngr
  );

uint32
  arad_pp_sw_db_ipv4_lpm_mngr_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr
  );





uint32
  arad_pp_sw_db_llp_trap_initialize(
    SOC_SAND_IN  int unit
  );




uint32
  arad_pp_sw_db_llp_mirror_initialize(
    SOC_SAND_IN  int unit
  );





uint32
  arad_pp_sw_db_llp_vid_assign_initialize(
    SOC_SAND_IN  int unit
  );



#define ARAD_PP_PON_DOUBLE_LOOKUP_VD_SET(_vlan_domain, _enable)\
  if(_enable) {\
    sw_state_access[unit].dpp.soc.arad.pp.pon_double_lookup.pon_double_lookup_enable.bit_set(unit,_vlan_domain);\
  } else {\
    sw_state_access[unit].dpp.soc.arad.pp.pon_double_lookup.pon_double_lookup_enable.bit_clear(unit,_vlan_domain);\
  }


#define ARAD_PP_PON_DOUBLE_LOOKUP_VD_GET(_vlan_domain, _res)\
  sw_state_access[unit].dpp.soc.arad.pp.pon_double_lookup.pon_double_lookup_enable.bit_get(unit,_vlan_domain, &_res);



uint32
  arad_pp_sw_db_eg_mirror_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  arad_pp_sw_db_eg_encap_initialize(
    SOC_SAND_IN  int unit
  );





uint32
  arad_pp_sw_db_llp_cos_initialize(
    SOC_SAND_IN  int unit
  );





uint32
  arad_pp_sw_db_eth_policer_initialize(
    SOC_SAND_IN  int unit
  );

int
  arad_pp_sw_db_eth_policer_config_status_bit_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                	  core_id, 
    SOC_SAND_IN  uint32                   bit_offset,
    SOC_SAND_IN  uint8                    status
  );




uint32
  arad_pp_sw_db_mact_flush_db_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 entry_index,
    SOC_SAND_IN uint32 flush_db_data[7]
  );

uint32
  arad_pp_sw_db_mact_flush_db_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 entry_index,
    SOC_SAND_OUT uint32 flush_db_data[7]
  );

uint32
  arad_pp_sw_db_mact_traverse_flush_entry_use_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 entry_offset,
    SOC_SAND_IN uint8  in_use
  );

uint32
  arad_pp_sw_db_mact_traverse_flush_entry_use_get(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  entry_offset,
    SOC_SAND_OUT uint8   *status
  );




uint32
  arad_pp_sw_db_lif_table_entry_use_set(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID         lif_id,
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_TYPE   entry_type,
    SOC_SAND_IN    uint32                 sub_type
  );


uint32
  arad_pp_sw_db_lif_table_entry_use_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID         lif_id,
    SOC_SAND_OUT  SOC_PPC_LIF_ENTRY_TYPE   *entry_type,
    SOC_SAND_OUT  uint32                   *sub_type
  );





uint32
  arad_pp_sw_db_flp_prog_app_to_index_get(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint8    app_id,
    SOC_SAND_OUT uint32 *prog_index
  );


uint32
  arad_pp_sw_db_vrrp_mac_entry_use_set(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            reg_ndx,
    SOC_SAND_IN  uint32            bit_ndx,
    SOC_SAND_IN  uint8             entry_in_use
  );

uint32
  arad_pp_sw_db_vrrp_mac_entry_use_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            reg_ndx,
    SOC_SAND_IN  uint32            bit_ndx,
    SOC_SAND_OUT uint8             *entry_in_use
  );


uint32
  arad_pp_sw_db_mac_limit_per_tunnel_set(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint16            port,
    SOC_SAND_IN  uint16            tunnel,
    SOC_SAND_IN  int               mac_limit
  );

uint32
  arad_pp_sw_db_mac_limit_per_tunnel_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint16            port,
    SOC_SAND_IN  uint16            tunnel,
    SOC_SAND_OUT int              *mac_limit
  );




uint32
  arad_pp_sw_db_vdc_vsi_mapping_set(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32            vsi,
    SOC_SAND_IN  int               port_class,
    SOC_SAND_IN  uint32            isid
  );

uint32
  arad_pp_sw_db_vdc_vsi_mapping_get(
    SOC_SAND_IN   int               unit,
    SOC_SAND_IN   uint32            vsi,
    SOC_SAND_OUT  int               *port_class,
    SOC_SAND_OUT  uint32            *isid
  );



uint32 
  arad_pp_sw_db_rif_to_lif_group_map_add_lif_to_rif(
     SOC_SAND_IN int            unit,
     SOC_SAND_IN uint32            rif,
     SOC_SAND_IN bcm_port_t        lif
     );


uint32 
  arad_pp_sw_db_rif_to_lif_group_map_remove_lif_from_rif(
     SOC_SAND_IN int            unit,
     SOC_SAND_IN bcm_port_t        lif
     );

typedef SOC_SAND_GROUP_MEM_LL_ITER_FUN_POINTER_PARAM ARAD_PP_SW_DB_RIF_TO_LIF_GROUP_MAP_LIF_VISITOR;


uint32
  arad_pp_sw_db_rif_to_lif_group_map_visit_lif_group(
     SOC_SAND_IN int unit,
     SOC_SAND_IN uint32 rif,
     SOC_SAND_IN ARAD_PP_SW_DB_RIF_TO_LIF_GROUP_MAP_LIF_VISITOR visitor,
     SOC_SAND_INOUT void *opaque
     );


uint32
  arad_pp_sw_db_rif_to_lif_group_map_get_rif_urpf_mode(
     SOC_SAND_IN int unit,
     SOC_SAND_IN uint32 rif,
     SOC_SAND_OUT SOC_PPC_FRWRD_FEC_RPF_MODE *mode
     );


uint32
  arad_pp_sw_db_authentication_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 authentication_enable
  );

uint8
  arad_pp_sw_db_isem_ext_key_enabled_get(
    SOC_SAND_IN int unit
  );

void
  arad_pp_sw_db_isem_ext_key_enabled_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 ext_key_enabled
  );


uint32
  arad_pp_sw_db_llp_mirror_port_vlan_is_exist_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  int           core_id,
    SOC_SAND_IN  SOC_PPC_PORT  pp_port_ndx,
    SOC_SAND_IN  uint8         internal_vid_ndx,
    SOC_SAND_IN  uint8         is_exist
  );

uint32
  arad_pp_sw_db_llp_mirror_port_vlan_is_exist_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  int           core_id,
    SOC_SAND_IN  SOC_PPC_PORT  pp_port_ndx,
    SOC_SAND_IN  uint8         internal_vid_ndx,
    SOC_SAND_OUT uint8         *is_exist
  );

uint32
  arad_pp_sw_db_eg_mirror_port_vlan_is_exist_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  int           core_id,
    SOC_SAND_IN  SOC_PPC_PORT  pp_port_ndx,
    SOC_SAND_IN  uint8         internal_vid_ndx,
    SOC_SAND_IN  uint8         is_exist
  );

uint32
  arad_pp_sw_db_eg_mirror_port_vlan_is_exist_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  int           core_id,
    SOC_SAND_IN  SOC_PPC_PORT  pp_port_ndx,
    SOC_SAND_IN  uint8         internal_vid_ndx,
    SOC_SAND_OUT uint8         *is_exist
  );

uint32
  arad_pp_sw_db_ecmp_cur_size_set(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       ecmp_fec_idx,
    SOC_SAND_IN  uint32       cur_size
  );

uint32 
  arad_pp_sw_db_ecmp_cur_size_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       ecmp_fec_idx,
    SOC_SAND_OUT uint32       *size
  );

uint32
  arad_pp_sw_db_ecmp_max_size_set(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       ecmp_fec_idx,
    SOC_SAND_IN  uint32       max_size
  );

uint32
  arad_pp_sw_db_ecmp_max_size_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       ecmp_fec_idx,
    SOC_SAND_OUT uint32       *max_size
  );

uint32
arad_pp_sw_db_test_defragmet(SOC_SAND_IN  int unit);

CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_sw_db_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_sw_db_get_errs_ptr(void);







uint32
arad_pp_sw_db_overlay_arp_initialize(SOC_SAND_IN  int unit);


int
  arad_pp_sw_db_src_bind_spoof_id_ref_count_set(
    int               unit,
    uint16            spoof_id,
    uint8             increase
  );

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

