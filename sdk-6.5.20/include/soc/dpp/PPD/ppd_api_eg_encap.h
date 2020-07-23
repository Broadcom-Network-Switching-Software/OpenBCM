/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/


#ifndef __SOC_PPD_API_EG_ENCAP_INCLUDED__

#define __SOC_PPD_API_EG_ENCAP_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_eg_encap.h>

#include <soc/dpp/PPD/ppd_api_general.h>






#define SOC_PPD_EG_ENCAP_BFD_IPV4_PROG_VAR        0xc

#define SOC_PPD_EG_ENCAP_TEST2_PROG_VAR          0x3











  
#define SOC_PPD_EG_ENCAP_DATA_RSPAN_FORMAT_SET(__unit, __type, __tpid, __pcp, __vid,__eg_encap_data_info)  \
      (__eg_encap_data_info)->data_entry[1] = ((__type & 0x3) << 8) | ((__tpid>>8) & 0xFF);  \
      (__eg_encap_data_info)->data_entry[0] = (((__tpid & 0xFF) << 24) | ((__pcp) << 21) | ((__vid) << 8) | SOC_PPC_PRGE_DATA_ENTRY_LSBS_RSPAN);


#define SOC_PPD_EG_ENCAP_DATA_RSPAN_FORMAT_GET(tpid, pcp, vid, __eg_encap_data_info) \
      tpid = (__eg_encap_data_info)->data_entry[0] >>24 | \
             ((__eg_encap_data_info)->data_entry[1] & 0xff) << 8;\
      pcp = ((__eg_encap_data_info)->data_entry[0] & 0xe00000) >> 21;\
      vid =  ((__eg_encap_data_info)->data_entry[0] & 0x1FFF00) >> 8;

#define SOC_PPD_EG_ENCAP_DATA_FORMAT_GET(__unit, __eg_encap_data_info)  ((__eg_encap_data_info)->data_entry[0] & 3)

  
#define SOC_PPD_EG_ENCAP_DATA_PON_TUNNEL_FORMAT_SET(__unit, __tpid, __tunnel_id, __eg_encap_data_info)  \
      (__eg_encap_data_info)->data_entry[1] = (__tpid>>8) & 0xFF;  \
      (__eg_encap_data_info)->data_entry[0] = ((((__tpid) & 0xFF) << 24) | ((__tunnel_id) << 8) | SOC_PPC_PRGE_DATA_ENTRY_LSBS_PON_TUNNEL);

#define SOC_PPD_EG_ENCAP_DATA_PON_TUNNEL_FORMAT_TPID_GET(__unit, __eg_encap_data_info)  \
      ((((__eg_encap_data_info)->data_entry[1]&0xFF) << 8) | (((__eg_encap_data_info)->data_entry[0] >> 24) & 0xFF))

#define SOC_PPD_EG_ENCAP_DATA_PON_TUNNEL_FORMAT_TUNNEL_ID_GET(__unit, __eg_encap_data_info)  \
      (((__eg_encap_data_info)->data_entry[0] >> 8) & 0xFFFF)

  
#define SOC_PPD_EG_ENCAP_DATA_BFD_IPV4_FORMAT_SET(__unit, __src_ip, __ip_hdr_checksum, __eg_encap_data_info)  \
      (__eg_encap_data_info)->data_entry[0]  = SOC_PPD_EG_ENCAP_BFD_IPV4_PROG_VAR;               \
      (__eg_encap_data_info)->data_entry[0] |= (__src_ip << 8);   \
      (__eg_encap_data_info)->data_entry[1]  = (__src_ip >> 24);  \
      (__eg_encap_data_info)->data_entry[1] |= (__ip_hdr_checksum << 8);

  
#define SOC_PPD_EG_ENCAP_DATA_BFD_IPV4_FORMAT_GET(__unit, __prg_var, __src_ip, __ip_hdr_checksum, __eg_encap_data_info)  \
      __prg_var = ((__eg_encap_data_info)->data_entry[0] & 0xff);                                                          \
      __src_ip = ((__eg_encap_data_info)->data_entry[0] >> 8) | (((__eg_encap_data_info)->data_entry[1] & 0xFF) << 24);           \
      __ip_hdr_checksum = ((__eg_encap_data_info)->data_entry[1] & 0xffff0) >> 4;

#define SOC_PPD_EG_ENCAP_DATA_BFD_IPV4_FORMAT_CHECK(__unit, __eg_encap_data_info)  (((__eg_encap_data_info)->data_entry[0] & 0xf) == SOC_PPD_EG_ENCAP_BFD_IPV4_PROG_VAR)

#define SOC_PPD_EG_ENCAP_DATA_L2_ENCAP_EXTERNAL_CPU_FORMAT_SET(__unit, __vlan, __ethertype, __dst_mac, __eg_encap_data_info)    \
      (__eg_encap_data_info)->data_entry[0] = (uint32)(__vlan)<<16 |(uint32)(__ethertype);                                      \
      (__eg_encap_data_info)->data_entry[1] = ((uint32)(__dst_mac[3])<<16)|((uint32)(__dst_mac[4])<<8)|((uint32)(__dst_mac[5]));

#define SOC_PPD_EG_ENCAP_DATA_L2_ENCAP_EXTERNAL_CPU_FORMAT_GET(__unit, __vlan, __ethertype, __dst_mac, __eg_encap_data_info) \
      __ethertype = (__eg_encap_data_info)->data_entry[0] & 0xFFFF;                                                         \
      __vlan = ((__eg_encap_data_info)->data_entry[0]>>16) & 0xFFFF;                                                        \
      __dst_mac[3]=((__eg_encap_data_info)->data_entry[1]>>16) & 0xFF;                                                      \
      __dst_mac[4]=((__eg_encap_data_info)->data_entry[1]>>8) & 0xFF;                                                       \
      __dst_mac[5]=((__eg_encap_data_info)->data_entry[1]) & 0xFF;                                                             

#ifdef BCM_88660_A0

#define SOC_PPD_EG_ENCAP_DATA_2ND_PON_TUNNEL_FORMAT_SET(__unit, __tpid, __tunnel_id, __eg_encap_data_info)  \
      (__eg_encap_data_info)->data[1] = (__tpid>>12) & 0x0F;  \
      (__eg_encap_data_info)->data[0] = ((((__tpid) & 0x0FFF) << 20) | (__tunnel_id << 4) | 0x04)



#define SOC_PPD_EG_ENCAP_DATA_2ND_PON_TUNNEL_FORMAT_TUNNEL_ID_GET(__unit, __eg_encap_data_info)  \
      (((__eg_encap_data_info)->data[0] >> 4) & 0xFFFF)
#endif


#define SOC_PPD_EG_ENCAP_DATA_2ND_PPPOE_FORMAT_SET(__unit, __session_id, __eg_encap_data_info)  \
      (__eg_encap_data_info)->data[0] = (((__session_id) & 0xFFFF) << 4 | 0x3)
#define SOC_PPD_EG_ENCAP_DATA_2ND_PPPOE_FORMAT_SESSION_ID_GET(__unit, __eg_encap_data_info)  \
          (((__eg_encap_data_info)->data[0] >> 4) & 0xFFFF)


#define SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_NOF_BITS (24)

#define SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_MASK ((1 << SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_NOF_BITS) - 1) 

#define SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_COMPLIMENT_ENTRTY_0_MASK ((1 << (32 - SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_NOF_BITS)) - 1) 

#define SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_COMPLIMENT_ENTRTY_1_MASK ((1 << ((SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_NOF_BITS*2) - 32 )) - 1) 

#define SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_NOF_BITS (28)

#define SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_MASK ((1 << SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_NOF_BITS) - 1) 

#define SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_COMPLIMENT_ENTRTY_0_MASK ((1 << (32 - SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_NOF_BITS)) - 1) 

#define SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_COMPLIMENT_ENTRTY_1_MASK ((1 << ((SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_NOF_BITS*2) - 32 )) - 1) 


#define SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_SET(__unit, __cud_1, __cud_2, __eg_encap_data_info) \
      (__eg_encap_data_info)->data_entry[0] = \
      ((((__cud_2) & SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_COMPLIMENT_ENTRTY_0_MASK) << SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_NOF_BITS) |\
      ((__cud_1) & SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_MASK)); \
      (__eg_encap_data_info)->data_entry[1] = \
      ((__cud_2  >> (32 - SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_NOF_BITS)) & SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_COMPLIMENT_ENTRTY_1_MASK);

#define SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_GET(__unit, __index, __eg_encap_data_info) \
      (((((__eg_encap_data_info)->data_entry[1] & (SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_COMPLIMENT_ENTRTY_1_MASK)) << (32 - SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_NOF_BITS)) * (__index)) | \
      (((__eg_encap_data_info)->data_entry[0] >> (__index * SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_NOF_BITS)) & SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_MASK));

#define SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_SET(__unit, __cud_1, __cud_2, __eg_encap_data_info) \
      (__eg_encap_data_info)->data_entry[0] = \
      ((((__cud_2) & SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_COMPLIMENT_ENTRTY_0_MASK) << SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_NOF_BITS) |\
      ((__cud_1) & SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_MASK)); \
      (__eg_encap_data_info)->data_entry[1] = \
      ((__cud_2  >> (32 - SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_NOF_BITS)) & SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_COMPLIMENT_ENTRTY_1_MASK);

#define SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_GET(__unit, __index, __eg_encap_data_info) \
      (((((__eg_encap_data_info)->data_entry[1] & (SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_COMPLIMENT_ENTRTY_1_MASK)) << (32 - SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_NOF_BITS)) * (__index)) | \
      (((__eg_encap_data_info)->data_entry[0] >> (__index * SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_NOF_BITS)) & SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_MASK));






typedef enum
{
  
  SOC_PPD_EG_ENCAP_RANGE_INFO_SET = SOC_PPD_PROC_DESC_BASE_EG_ENCAP_FIRST,
  SOC_PPD_EG_ENCAP_RANGE_INFO_SET_PRINT,
  SOC_PPD_EG_ENCAP_RANGE_INFO_GET,
  SOC_PPD_EG_ENCAP_RANGE_INFO_GET_PRINT,
  SOC_PPD_EG_ENCAP_NULL_LIF_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_NULL_LIF_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_DATA_LIF_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_DATA_LIF_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_AC_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_AC_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_PWE_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_PWE_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_VSI_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_VSI_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_MPLS_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_MPLS_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_IPV4_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_IPV4_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_IPV6_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_IPV6_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_MIRROR_ENTRY_SET,
  SOC_PPD_EG_ENCAP_MIRROR_ENTRY_GET,
  SOC_PPD_EG_ENCAP_PORT_ERSPAN_SET,
  SOC_PPD_EG_ENCAP_PORT_ERSPAN_GET,
  SOC_PPD_EG_ENCAP_LL_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_LL_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_ENTRY_REMOVE,
  SOC_PPD_EG_ENCAP_ENTRY_REMOVE_PRINT,
  SOC_PPD_EG_ENCAP_ENTRY_GET,
  SOC_PPD_EG_ENCAP_ENTRY_GET_PRINT,
  SOC_PPD_EG_ENCAP_ENTRY_TYPE_GET,
  SOC_PPD_EG_ENCAP_ENTRY_TYPE_GET_PRINT,
  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET,
  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET_PRINT,
  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_GET,
  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_GET_PRINT,
  SOC_PPD_EG_ENCAP_PUSH_EXP_INFO_SET,
  SOC_PPD_EG_ENCAP_PUSH_EXP_INFO_SET_PRINT,
  SOC_PPD_EG_ENCAP_PUSH_EXP_INFO_GET,
  SOC_PPD_EG_ENCAP_PUSH_EXP_INFO_GET_PRINT,
  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_SET,
  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_SET_PRINT,
  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_GET,
  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_GET_PRINT,
  SOC_PPD_EG_ENCAP_GLBL_INFO_SET,
  SOC_PPD_EG_ENCAP_GLBL_INFO_SET_PRINT,
  SOC_PPD_EG_ENCAP_GLBL_INFO_GET,
  SOC_PPD_EG_ENCAP_GLBL_INFO_GET_PRINT,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_PRINT,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_PRINT,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_PRINT,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_PRINT,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_PRINT,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_PRINT,
  SOC_PPD_EG_ENCAP_GET_PROCS_PTR,
  SOC_PPD_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_INIT,
  SOC_PPD_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_SET,
  SOC_PPD_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_GET,
  
  
  SOC_PPD_EG_ENCAP_PROCEDURE_DESC_LAST
} SOC_PPD_EG_ENCAP_PROCEDURE_DESC;










uint32
  soc_ppd_eg_encap_entry_init(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               lif_eep_ndx
  );


uint32
  soc_ppd_eg_encap_range_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_RANGE_INFO                 *range_info
  );


uint32
  soc_ppd_eg_encap_range_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_RANGE_INFO                 *range_info
  );


uint32 soc_ppd_eg_encap_protection_info_set(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  int                                    out_lif_id,
    SOC_SAND_IN  uint8                                  is_full_entry_extension,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PROTECTION_INFO       *protection_info);


uint32
  soc_ppd_eg_encap_null_lif_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  uint32                                next_eep
  );


uint32
  soc_ppd_eg_encap_lif_field_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  uint32                                flags,
    SOC_SAND_IN  uint32                                val
  );


uint32
  soc_ppd_eg_encap_lif_field_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  uint32                                flags,
    SOC_SAND_OUT  uint32                               *val
  );



uint32
  soc_ppd_eg_encap_data_local_to_global_cud_init(
    SOC_SAND_IN  int                                 unit
  );


uint32
  soc_ppd_eg_encap_data_local_to_global_cud_set(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  in_cud,
    SOC_SAND_IN  uint32                                  out_cud
  );


uint32
  soc_ppd_eg_encap_data_local_to_global_cud_get(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  in_cud,
    SOC_SAND_OUT uint32*                                 out_cud
  );


uint32
  soc_ppd_eg_encap_data_lif_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_DATA_INFO                 *data_info,
    SOC_SAND_IN  uint8                                   next_eep_valid,
    SOC_SAND_IN  uint32                                  next_eep
  );


uint32
  soc_ppd_eg_encap_swap_command_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_SWAP_INFO                  *swap_info,
    SOC_SAND_IN  uint32                                next_eep
  );


uint32
  soc_ppd_eg_encap_pwe_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_INFO                   *pwe_info,
    SOC_SAND_IN  uint32                                next_eep
  );


uint32
  soc_ppd_eg_encap_pop_command_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_POP_INFO                   *pop_info,
    SOC_SAND_IN  uint32                                next_eep
  );


uint32
  soc_ppd_eg_encap_vsi_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO             *vsi_info, 
    SOC_SAND_IN uint8                                  next_eep_valid, 
    SOC_SAND_IN  uint32                                  next_eep
  );


uint32
  soc_ppd_eg_encap_mpls_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO            *mpls_encap_info,
    SOC_SAND_IN  uint32                                ll_eep
  );


uint32
  soc_ppd_eg_encap_ipv4_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO            *ipv4_encap_info,
    SOC_SAND_IN  uint32                                ll_eep
  );


uint32
  soc_ppd_eg_encap_ipv6_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tunnel_eep_ndx,
    SOC_SAND_INOUT  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO   *ipv6_encap_info,
    SOC_SAND_IN  uint32                                ll_eep
  );




uint32
  soc_ppd_eg_encap_overlay_arp_data_entry_add(
     SOC_SAND_IN  int                                    unit,
     SOC_SAND_IN  uint32                                 overlay_ll_eep_ndx, 
     SOC_SAND_INOUT  SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *ll_encap_info
     );



uint32
  soc_ppd_eg_encap_mirror_entry_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              mirror_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO *mirror_encap_info
  );



uint32
  soc_ppd_eg_encap_mirror_entry_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              mirror_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO *mirror_encap_info
  );


uint32
  soc_ppd_eg_encap_port_erspan_set(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  is_erspan
  );


uint32
  soc_ppd_eg_encap_port_erspan_get(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_OUT uint8                                  *is_erspan
  );


uint32
  soc_ppd_eg_encap_ll_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                ll_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_LL_INFO                    *ll_encap_info
  );


uint32
  soc_ppd_eg_encap_entry_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                eep_ndx
  );


uint32
  soc_ppd_eg_encap_entry_type_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 eep_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_TYPE                *entry_type
  );



uint32
  soc_ppd_eg_encap_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                eep_ndx,
    SOC_SAND_IN  uint32                                depth,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO                 encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX],
    SOC_SAND_OUT uint32                                next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX],
    SOC_SAND_OUT uint32                                *nof_entries
  );



uint32
soc_ppd_eg_encap_entry_data_info_to_overlay_arp_encap_info(
   SOC_SAND_IN  int                               unit,
   SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO                 *encap_entry_info
  );


uint32
  soc_ppd_eg_encap_push_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info
  );


uint32
  soc_ppd_eg_encap_push_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info
  );


uint32
  soc_ppd_eg_encap_push_exp_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                        exp
  );


uint32
  soc_ppd_eg_encap_push_exp_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_EXP                        *exp
  );


uint32
  soc_ppd_eg_encap_pwe_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  );


uint32
  soc_ppd_eg_encap_pwe_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  );


uint32
  soc_ppd_eg_encap_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_GLBL_INFO              *glbl_info
  );


uint32
  soc_ppd_eg_encap_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_GLBL_INFO              *glbl_info
  );


uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  uint32                                src_ip
  );


uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_OUT uint32                                *src_ip
  );


uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                          ttl
  );


uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                          *ttl
  );


uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                        tos
  );


uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_TOS                        *tos
  );


uint32
  soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_set(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint8                                is_exp_copy
  );


uint32
  soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_get(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_OUT uint8                                *is_exp_copy
  );




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

