
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __JER_PP_EG_ENCAP_INCLUDED__

#define __JER_PP_EG_ENCAP_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>





#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_SA_LSBS_NOF_BITS                   12
#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_SA_LSBS_MASK                       ((1 << JER_PP_EG_ENCAP_ROO_LINK_LAYER_SA_LSBS_NOF_BITS) -1)

#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_OUTER_TAG_LSB        0
#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_PCP_DEI_LSB          8
#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_PCP_DEI_NOF_BITS                   (SOC_SAND_PP_PCP_NOF_BITS + SOC_SAND_PP_CFI_NOF_BITS)
#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_PCP_DEI_MASK                      ((1 << JER_PP_EG_ENCAP_ROO_LINK_LAYER_PCP_DEI_NOF_BITS) - 1)


#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_LSB_NOF_BITS             JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_OUTER_VLAN_LSBS_NOF_BITS
#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_LSB_MASK                 ((1 << JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_LSB_NOF_BITS) -1)

#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_NOF_BITS             (SOC_SAND_PP_VID_NOF_BITS - JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_LSB_NOF_BITS)
#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_OUTER_TAG_MSB_MASK   ((1 << JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_NOF_BITS) -1)



#define JER_PP_EG_ENCAP_OVERLAY_ARP_DATA_ENTRY_OUT_VID_LSBS(vid, lsbs) (lsbs) = (vid & JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_LSB_MASK) 



 
#define ROO_LINK_LAYER_ENTRY_OUTER_TAG_MSB_SET(outer_tag_msb, outer_tag, pcp_dei) \
    (outer_tag_msb) = (((pcp_dei & JER_PP_EG_ENCAP_ROO_LINK_LAYER_PCP_DEI_MASK)     \
                             << JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_PCP_DEI_LSB)         \
                       | ((outer_tag >> (JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_LSB_NOF_BITS))           \
                          & JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_OUTER_TAG_MSB_MASK))


#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_ENTRY_OUTER_TAG_MSB_OUTER_TAG_GET(outer_tag_msb, outer_tag) \
   SHR_BITCOPY_RANGE(&outer_tag, JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_LSB_NOF_BITS,                     \
                     &outer_tag_msb, JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_OUTER_TAG_LSB,    \
                     JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_NOF_BITS) 


#define JER_PP_EG_ENCAP_ROO_LINK_LAYER_ENTRY_OUTER_TAG_MSB_PCP_DEI_GET(outer_tag_msb, pcp_dei) \
   SHR_BITCOPY_RANGE(&pcp_dei, 0,                                                              \
                     &outer_tag_msb, JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_MSB_PCP_DEI_LSB, \
                     JER_PP_EG_ENCAP_ROO_LINK_LAYER_PCP_DEI_NOF_BITS)



#define SOC_JER_PP_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_ENABLE (1)

                     














uint32
soc_jer_pp_eg_encap_overlay_arp_data_entry_add(
     SOC_SAND_IN  int                                    unit,
     SOC_SAND_IN  uint32                                 ll_eep_ndx, 
     SOC_SAND_INOUT  SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *ll_encap_info
   ); 



soc_error_t
soc_jer_eg_encap_roo_ll_entry_get(
     SOC_SAND_IN  int                         unit,
     SOC_SAND_IN  uint32                      eep_ndx, 
     SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_info
   ); 







soc_error_t
soc_jer_eg_encap_ether_type_index_set(
   SOC_SAND_IN  int                                        unit, 
   SOC_SAND_IN  int                                        eth_type_index,
   SOC_SAND_IN  SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *eth_type_index_entry
   ); 




soc_error_t
soc_jer_eg_encap_ether_type_index_get(
   SOC_SAND_IN  int                                        unit, 
   SOC_SAND_IN  int                                        eth_type_index,
   SOC_SAND_OUT  SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *eth_type_index_entry
   ); 


soc_error_t
soc_jer_eg_encap_ether_type_index_clear(
   SOC_SAND_IN  int unit, 
   SOC_SAND_IN  int eth_type_index); 



soc_error_t
soc_jer_eg_encap_direct_bank_set(int unit, int bank, uint8 is_mapped);



soc_error_t
soc_jer_eg_encap_extension_mapping_set(int unit, int bank, uint32 is_extended, uint32 extesnion_bank);


soc_error_t
soc_jer_eg_encap_extension_mapping_get(int unit, int bank, uint32 *is_extended, uint32 *extesnion_bank);


soc_error_t
soc_jer_eg_encap_extension_type_set(int unit, int bank, uint8 is_ext_data);

soc_error_t
soc_jer_eg_encap_extension_type_get(int unit, int bank, uint8 *is_ext_data);


soc_error_t
soc_jer_eg_encap_map_encap_intpri_color_set(int unit, int index, SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO *entry_info);

soc_error_t
soc_jer_eg_encap_map_encap_intpri_color_get(int unit, int index, SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO *entry_info);

soc_error_t 
soc_jer_eg_encap_ip_tunnel_size_protocol_template_set (int unit, int encapsulation_mode, SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO *ip_tunnel_size_protocol_template); 

soc_error_t 
soc_jer_eg_encap_ip_tunnel_size_protocol_template_init(int unit); 

soc_error_t soc_jer_eg_encap_null_value_set(int unit, uint32 *value);

soc_error_t soc_jer_eg_encap_null_value_get(int unit, uint32 *value);

soc_error_t soc_jer_eg_encap_push_2_swap_init(int unit);




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

