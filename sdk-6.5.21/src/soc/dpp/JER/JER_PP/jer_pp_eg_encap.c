
 
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/sand/sand_mem.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>

#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap_access.h>
#include <soc/dpp/drv.h>






















static uint32
soc_jer_pp_eg_encap_overlay_arp_data_from_roo_link_layer_entry_buffer(
   SOC_SAND_IN  int                                    unit,
   SOC_SAND_IN JER_PP_EG_ENCAP_ACCESS_ROO_LL_ENTRY_FORMAT *roo_ll_entry, 
   SOC_SAND_OUT SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *overlay_arp_info 
   ) {
    uint32
        src_mac_lsbs[2]; 
    uint32 
        pcp_dei_32; 
    
    src_mac_lsbs[0] = 0; 
    src_mac_lsbs[1] = 0; 

    
    
    JER_PP_EG_ENCAP_ROO_LINK_LAYER_ENTRY_OUTER_TAG_MSB_OUTER_TAG_GET(roo_ll_entry->outer_tag_msb, overlay_arp_info->out_vid); 
    
    JER_PP_EG_ENCAP_ROO_LINK_LAYER_ENTRY_OUTER_TAG_MSB_PCP_DEI_GET(roo_ll_entry->outer_tag_msb, pcp_dei_32); 
    overlay_arp_info->pcp_dei = pcp_dei_32 & 0xFF; 

    
    SHR_BITCOPY_RANGE(src_mac_lsbs, 0, 
                      &roo_ll_entry->sa_lsb, 0, 
                      JER_PP_EG_ENCAP_ROO_LINK_LAYER_SA_LSBS_NOF_BITS); 
    soc_sand_pp_mac_address_long_to_struct(src_mac_lsbs, &overlay_arp_info->src_mac); 

    
    soc_sand_pp_mac_address_long_to_struct(roo_ll_entry->dest_mac, &overlay_arp_info->dest_mac);

    
    overlay_arp_info->pcp_dei_profile = roo_ll_entry->pcp_dei_profile; 

    
    overlay_arp_info->remark_profile= roo_ll_entry->remark_profile; 

    
    overlay_arp_info->nof_tags = roo_ll_entry->nof_tags; 

    
    overlay_arp_info->eth_type_index = roo_ll_entry->ether_type_index; 

    return SOC_SAND_OK;
}


static uint32
soc_jer_pp_eg_encap_overlay_arp_data_to_roo_link_layer_entry_buffer(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *overlay_arp_info, 
    SOC_SAND_OUT JER_PP_EG_ENCAP_ACCESS_ROO_LL_ENTRY_FORMAT *roo_ll_entry
   ) {

    uint32 src_mac[2];

    src_mac[0] =0; 
    src_mac[1] =0; 

    
    ROO_LINK_LAYER_ENTRY_OUTER_TAG_MSB_SET(
       roo_ll_entry->outer_tag_msb, overlay_arp_info->out_vid, overlay_arp_info->pcp_dei); 

    
    roo_ll_entry->remark_profile = overlay_arp_info->remark_profile; 

    
    roo_ll_entry->nof_tags = overlay_arp_info->nof_tags;  

    
    roo_ll_entry->ether_type_index = overlay_arp_info->eth_type_index; 

    
    soc_sand_pp_mac_address_struct_to_long(&overlay_arp_info->src_mac, src_mac); 
    roo_ll_entry->sa_lsb =  (src_mac[0] & JER_PP_EG_ENCAP_ROO_LINK_LAYER_SA_LSBS_MASK); 

    
    soc_sand_pp_mac_address_struct_to_long(&overlay_arp_info->dest_mac, roo_ll_entry->dest_mac);

    

    
    roo_ll_entry->pcp_dei_profile = overlay_arp_info->pcp_dei_profile; 

    return SOC_SAND_OK;
}


#define JER_PP_CFG_ETHER_TYPE_INDEX_TBL_ENTRY_SIZE    (2)




static uint32 
soc_jer_pp_eg_encap_overlay_arp_data_from_roo_link_layer_quarter_entry_buffer(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN uint64                                 quarter_entry_buffer,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *overlay_arp_info
    ) {
    uint32
      res = SOC_SAND_OK;
    uint32 data[2], field32; 

    SOCDNX_INIT_FUNC_DEFS; 

   soc_sand_os_memset(data, 0x0, 2* sizeof(uint32));

    COMPILER_64_TO_32_LO(data[0], quarter_entry_buffer); 
    COMPILER_64_TO_32_HI(data[1], quarter_entry_buffer); 

    
    res = soc_sand_bitstream_get_any_field(
            data, 
            JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_INNER_VLAN_LSB, 
            JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_INNER_VLAN_NOF_BITS, 
            &(overlay_arp_info->inner_vid));
    SOCDNX_SAND_IF_ERR_EXIT(res);

    
    res = soc_sand_bitstream_get_any_field(
            data, 
            JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_PCP_DEI_INNER_VLAN_LSB, 
            JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_PCP_DEI_INNER_VLAN_NOF_BITS, 
            &(field32));
     SOCDNX_SAND_IF_ERR_EXIT(res);
     overlay_arp_info->inner_pcp_dei = field32 & 0xFF; 

     
     res = soc_sand_bitstream_get_any_field(
             data, 
             JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_OUTER_VLAN_LSBS_LSB, 
             JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_OUTER_VLAN_LSBS_NOF_BITS, 
             &(field32));
     SOCDNX_SAND_IF_ERR_EXIT(res);

    SHR_BITCOPY_RANGE(&(overlay_arp_info->out_vid), 0, 
                      &field32, 0, 
                      JER_PP_EG_ENCAP_ROO_LINK_LAYER_OUTER_TAG_LSB_NOF_BITS); 

exit:
  SOCDNX_FUNC_RETURN; 
}



static uint32 
soc_jer_pp_eg_encap_overlay_arp_data_to_roo_link_layer_quarter_entry_buffer(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *overlay_arp_info, 
    SOC_SAND_OUT uint64                                 *quarter_entry_buffer) {

    uint32
      res = SOC_SAND_OK;
    uint32 data[2], field32; 

    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_os_memset(data, 0x0, 2* sizeof(uint32));

    

    
    res = soc_sand_bitstream_set_any_field(&(overlay_arp_info->inner_vid), 
                                           JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_INNER_VLAN_LSB, 
                                           JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_INNER_VLAN_NOF_BITS, 
                                           data);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    
    field32 = overlay_arp_info->inner_pcp_dei; 
    res = soc_sand_bitstream_set_any_field(&(field32), 
                                           JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_PCP_DEI_INNER_VLAN_LSB, 
                                           JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_PCP_DEI_INNER_VLAN_NOF_BITS, 
                                           data);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    
    JER_PP_EG_ENCAP_OVERLAY_ARP_DATA_ENTRY_OUT_VID_LSBS(overlay_arp_info->out_vid, field32); 
    res = soc_sand_bitstream_set_any_field(&(field32), 
                                           JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_OUTER_VLAN_LSBS_LSB, 
                                           JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_OUTER_VLAN_LSBS_NOF_BITS, 
                                           data);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    COMPILER_64_SET(*quarter_entry_buffer,data[1],data[0]);

exit:
  SOCDNX_FUNC_RETURN; 
}



uint32
soc_jer_pp_eg_encap_overlay_arp_data_entry_add(
     SOC_SAND_IN  int                                    unit,
     SOC_SAND_IN  uint32                                 overlay_ll_eep_ndx, 
     SOC_SAND_INOUT  SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *ll_encap_info
   ) 
{   
  uint32 
      res = SOC_SAND_OK;

  JER_PP_EG_ENCAP_ACCESS_ROO_LL_ENTRY_FORMAT         tbl_data; 

  uint64 quarter_entry_buffer = COMPILER_64_INIT(0,0); 
  JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_FORMAT  quarter_entry; 
  
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(ll_encap_info);

  

  soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));

  

  

  soc_sand_os_memset(&quarter_entry, 0x0, sizeof(JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_FORMAT)); 

  
  res = soc_jer_pp_eg_encap_overlay_arp_data_to_roo_link_layer_quarter_entry_buffer(
     unit, ll_encap_info, &quarter_entry_buffer); 
  SOCDNX_IF_ERR_EXIT(res);

  
  res = arad_pp_lif_additional_data_set(unit, overlay_ll_eep_ndx, 0, quarter_entry_buffer, FALSE);
  SOCDNX_IF_ERR_EXIT(res);

  
  
  res = soc_jer_pp_eg_encap_overlay_arp_data_to_roo_link_layer_entry_buffer(
     unit, ll_encap_info, &tbl_data);
  SOCDNX_IF_ERR_EXIT(res);

   
  res = soc_jer_eg_encap_access_roo_link_layer_format_tbl_set(
     unit, overlay_ll_eep_ndx, &tbl_data); 
  SOCDNX_IF_ERR_EXIT(res);


exit:
  SOCDNX_FUNC_RETURN; 

}



soc_error_t
soc_jer_eg_encap_roo_ll_entry_get(
     SOC_SAND_IN  int                         unit,
     SOC_SAND_IN  uint32                      eep_ndx, 
     SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_info
   ) {

    uint32
       res = SOC_SAND_OK; 

    JER_PP_EG_ENCAP_ACCESS_ROO_LL_ENTRY_FORMAT tbl_data; 

    JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_FORMAT  quarter_entry; 
    uint64 quarter_entry_buffer; 


    SOC_PPC_EG_ENCAP_EEP_TYPE                  
        eep_type_ndx = SOC_PPC_EG_ENCAP_EEP_TYPE_ROO_LL;
    ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
        cur_eep_type = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(encap_info);

    soc_sand_os_memset(&tbl_data, 0x0, sizeof(tbl_data));
    soc_sand_os_memset(&quarter_entry, 0x0, sizeof(JER_PP_EG_ENCAP_ACCESS_ROO_LL_QUARTER_ENTRY_FORMAT)); 

    SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(encap_info);

    

    res = arad_pp_eg_encap_access_key_prefix_type_get_unsafe(
       unit, eep_ndx, &cur_eep_type);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    ARAD_PP_EG_ENCAP_VERIFY_EEP_TYPE_COMPATIBLE_TO_ACCESS_TYPE(
       eep_ndx,eep_type_ndx, cur_eep_type);

    

    
    res = soc_jer_eg_encap_access_roo_link_layer_format_tbl_get(
       unit, eep_ndx, &tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    encap_info->entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_ROO_LL_ENCAP; 
    res = soc_jer_pp_eg_encap_overlay_arp_data_from_roo_link_layer_entry_buffer(
       unit, &tbl_data, &(encap_info->entry_val.overlay_arp_encap_info)); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    
    
    res = arad_pp_lif_additional_data_get(unit, eep_ndx, 0, &quarter_entry_buffer); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    
    res = soc_jer_pp_eg_encap_overlay_arp_data_from_roo_link_layer_quarter_entry_buffer(
       unit, quarter_entry_buffer, &(encap_info->entry_val.overlay_arp_encap_info)); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_jer_eg_encap_roo_ll_entry_get()",0,0);
}







uint32
soc_jer_pp_eg_encap_eth_type_index_to_cfg_ether_type_index_tbl_entry(
   SOC_SAND_IN  int                                     unit,
   SOC_SAND_IN  SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *eth_type_index_entry, 
   SOC_SAND_OUT uint32                                  *cfg_ether_type_index_tbl_data
   ) {

  uint32 tmp; 
  uint32 fld_offset; 

  uint32 res = SOC_SAND_OK;

  fld_offset = 0; 

  
  tmp = eth_type_index_entry->tpid_1;   
  SHR_BITCOPY_RANGE(
     cfg_ether_type_index_tbl_data, fld_offset, &tmp, 0, SOC_SAND_PP_TPID_NOF_BITS);

  fld_offset += SOC_SAND_PP_TPID_NOF_BITS; 

  
  tmp =  eth_type_index_entry->tpid_0;
  SHR_BITCOPY_RANGE(
     cfg_ether_type_index_tbl_data, fld_offset, &tmp, 0, SOC_SAND_PP_TPID_NOF_BITS); 

  fld_offset += SOC_SAND_PP_TPID_NOF_BITS; 

  
  tmp = eth_type_index_entry->ether_type; 
  SHR_BITCOPY_RANGE(
     cfg_ether_type_index_tbl_data, fld_offset, &tmp, 0, SOC_SAND_PP_ETHER_TYPE_NOF_BITS);

  return res; 
}



uint32
soc_jer_pp_eg_encap_eth_type_index_from_cfg_ether_type_index_tbl_entry(
   SOC_SAND_IN  int                                     unit,
   SOC_SAND_OUT  SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *eth_type_index_entry, 
   SOC_SAND_IN uint32                                  *cfg_ether_type_index_tbl_entry
   ) {
  uint32 tmp; 
  uint32 fld_offset; 
  uint32 res = SOC_SAND_OK;


  fld_offset = 0; 

  
  SHR_BITCOPY_RANGE(
     &tmp, 0, cfg_ether_type_index_tbl_entry, fld_offset, SOC_SAND_PP_TPID_NOF_BITS); 
  eth_type_index_entry->tpid_1 = (uint16) (tmp & 0xFFFF); 

  fld_offset += SOC_SAND_PP_TPID_NOF_BITS; 

  
  SHR_BITCOPY_RANGE(
     &tmp, 0, cfg_ether_type_index_tbl_entry, fld_offset, SOC_SAND_PP_TPID_NOF_BITS); 
  eth_type_index_entry->tpid_0 = (uint16) (tmp & 0xFFFF); 

  fld_offset += SOC_SAND_PP_TPID_NOF_BITS; 

  
  SHR_BITCOPY_RANGE(
     &tmp, 0, cfg_ether_type_index_tbl_entry, fld_offset, SOC_SAND_PP_ETHER_TYPE_NOF_BITS); 
  eth_type_index_entry->ether_type = (uint16) (tmp & 0xFFFF); 

  return res; 
}




soc_error_t
soc_jer_eg_encap_ether_type_index_set(
   SOC_SAND_IN  int                                        unit, 
   SOC_SAND_IN  int                                        eth_type_index,
   SOC_SAND_IN  SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *eth_type_index_entry
   ) {

    uint32
       res = SOC_SAND_OK; 

    uint32 cfg_ether_type_index_tbl_data[JER_PP_CFG_ETHER_TYPE_INDEX_TBL_ENTRY_SIZE];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    soc_sand_os_memset(&cfg_ether_type_index_tbl_data, 0x0, sizeof(uint32)*JER_PP_CFG_ETHER_TYPE_INDEX_TBL_ENTRY_SIZE);

  
  res = soc_jer_pp_eg_encap_eth_type_index_to_cfg_ether_type_index_tbl_entry(
     unit, eth_type_index_entry, cfg_ether_type_index_tbl_data); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  res = WRITE_EPNI_CFG_ETHER_TYPE_INDEXm(unit, MEM_BLOCK_ANY, eth_type_index, cfg_ether_type_index_tbl_data); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_jer_eg_encap_ether_type_index_set()", 0, 0); 
}
soc_error_t
soc_jer_eg_encap_ether_type_index_get(
   SOC_SAND_IN  int                                        unit, 
   SOC_SAND_IN  int                                        eth_type_index,
   SOC_SAND_OUT  SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *eth_type_index_entry
   ) {

    uint32
       res = SOC_SAND_OK; 

    uint32 cfg_ether_type_index_tbl_data[JER_PP_CFG_ETHER_TYPE_INDEX_TBL_ENTRY_SIZE];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    soc_sand_os_memset(&cfg_ether_type_index_tbl_data, 0x0, sizeof(uint32)*JER_PP_CFG_ETHER_TYPE_INDEX_TBL_ENTRY_SIZE);

    
    res = READ_EPNI_CFG_ETHER_TYPE_INDEXm(
       unit, MEM_BLOCK_ANY, 
       eth_type_index, 
       cfg_ether_type_index_tbl_data); 
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

     
    res = soc_jer_pp_eg_encap_eth_type_index_from_cfg_ether_type_index_tbl_entry(
       unit, eth_type_index_entry, cfg_ether_type_index_tbl_data); 
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_jer_eg_encap_ether_type_index_set()", 0, 0); 
}

soc_error_t
soc_jer_eg_encap_ether_type_index_clear(
   SOC_SAND_IN  int unit, 
   SOC_SAND_IN  int eth_type_index) {

    uint32
       res = SOC_SAND_OK; 

     uint64 reg_64_val; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

     COMPILER_64_ZERO(reg_64_val); 
  
    res = WRITE_EPNI_CFG_ETHER_TYPE_INDEXm(
      unit, MEM_BLOCK_ANY, eth_type_index, &reg_64_val); 
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_jer_pp_eg_encap_ether_type_index_clear()", 0, 0); 
}

soc_error_t
soc_jer_eg_encap_direct_bank_set(int unit, int bank, uint8 is_mapped){
    int rv;
    uint64 buffer;
    SOCDNX_INIT_FUNC_DEFS;


    

    rv = soc_reg_above_64_field64_read(unit, EPNI_CFG_OUTLIF_MAPPING_IS_REQUIREDr, REG_PORT_ANY, 0, CFG_OUTLIF_MAPPING_IS_REQUIREDf, &buffer);
    SOCDNX_IF_ERR_EXIT(rv);

    if (is_mapped) {
        COMPILER_64_BITSET(buffer, bank);
    } else {
        COMPILER_64_BITCLR(buffer, bank);
    }

    rv = soc_reg_above_64_field64_modify(unit, EPNI_CFG_OUTLIF_MAPPING_IS_REQUIREDr, REG_PORT_ANY, 0, CFG_OUTLIF_MAPPING_IS_REQUIREDf, buffer);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_reg_above_64_field64_modify(unit, EPNI_CFG_EEI_MAPPING_IS_REQUIREDr, REG_PORT_ANY, 0, CFG_EEI_MAPPING_IS_REQUIREDf, buffer);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_jer_eg_encap_extension_mapping_set(int unit, int bank, uint32 is_extended, uint32 extesnion_bank){
    int res;
    uint32 tbl_data;
    uint32
      table_entry[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_EDB_EEDB_MAP_TO_PROTECTION_PTRm(unit, MEM_BLOCK_ANY, bank, &tbl_data));

    
    soc_mem_field_set(unit, EDB_EEDB_MAP_TO_PROTECTION_PTRm , &tbl_data, PROTECTION_PTR_ENABLEf, &is_extended);

    
    soc_mem_field_set(unit, EDB_EEDB_MAP_TO_PROTECTION_PTRm , &tbl_data, PROTECTION_PTR_TABLE_ADDR_MSBf, &extesnion_bank);

    
    SOCDNX_IF_ERR_EXIT(WRITE_EDB_EEDB_MAP_TO_PROTECTION_PTRm(unit, MEM_BLOCK_ANY, bank, &tbl_data));

    
    if (is_extended) {
        sal_memset(table_entry, 0, sizeof(table_entry));
    } else {
        res = arad_pp_eg_encap_access_create_none_entry_in_buffer(unit, table_entry);
        SOCDNX_IF_ERR_EXIT(res);
    }

     
      
    res = sand_fill_partial_table_with_entry(unit, EDB_EEDB_TOP_BANKm, extesnion_bank, extesnion_bank, MEM_BLOCK_ANY, \
                                             0, ARAD_PP_EG_ENCAP_NOF_ENTRIES_PER_TOP_BANK(unit), table_entry);
    SOCDNX_IF_ERR_EXIT(res);


exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_jer_eg_encap_extension_mapping_get(int unit, int bank, uint32 *is_extended, uint32 *extesnion_bank){
   
    uint32 tbl_data;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_EDB_EEDB_MAP_TO_PROTECTION_PTRm(unit, MEM_BLOCK_ANY, bank, &tbl_data));

      
      soc_mem_field_get(unit, EDB_EEDB_MAP_TO_PROTECTION_PTRm, &tbl_data, PROTECTION_PTR_ENABLEf, is_extended);

      
      soc_mem_field_get(unit, EDB_EEDB_MAP_TO_PROTECTION_PTRm, &tbl_data, PROTECTION_PTR_TABLE_ADDR_MSBf, extesnion_bank);


exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_jer_eg_encap_extension_type_set(int unit, int bank, uint8 is_ext_data){

    uint32 reg[1];
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_SAND_IF_ERR_EXIT(READ_EPNI_USE_PROTECTION_AS_DATA_PER_BANKr(unit, REG_PORT_ANY, reg));
    if (is_ext_data) {
        SHR_BITSET(reg,bank);
    }
    else {
         SHR_BITCLR(reg,bank);
    }
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_EPNI_USE_PROTECTION_AS_DATA_PER_BANKr(unit, REG_PORT_ANY, *reg));


exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_jer_eg_encap_extension_type_get(int unit, int bank, uint8 *is_ext_data){
   
    uint32 reg[1];
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_SAND_IF_ERR_EXIT(READ_EPNI_USE_PROTECTION_AS_DATA_PER_BANKr(unit, REG_PORT_ANY, reg));

    
     *is_ext_data =  (SHR_BITGET(reg, bank)) ? TRUE : FALSE;


exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_jer_eg_encap_map_encap_intpri_color_set(int unit, int index, SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO *entry_info)
{

    uint32 tbl_data;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_EPNI_COS_PROFILE_TABLEm(unit, MEM_BLOCK_ANY, index, &tbl_data));

    
    soc_mem_field_set(unit, EPNI_COS_PROFILE_TABLEm , &tbl_data, TC_VALIDf, &(entry_info->int_pri_valid));
    soc_mem_field_set(unit, EPNI_COS_PROFILE_TABLEm , &tbl_data, TCf, &(entry_info->int_pri));

    
    soc_mem_field_set(unit, EPNI_COS_PROFILE_TABLEm , &tbl_data, DP_VALIDf, &(entry_info->color_valid));
    soc_mem_field_set(unit, EPNI_COS_PROFILE_TABLEm , &tbl_data, DPf, &(entry_info->color));

    
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_COS_PROFILE_TABLEm(unit, MEM_BLOCK_ANY, index, &tbl_data));

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_jer_eg_encap_map_encap_intpri_color_get(int unit, int index, SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO *entry_info)
{
   
    uint32 tbl_data;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_EPNI_COS_PROFILE_TABLEm(unit, MEM_BLOCK_ANY, index, &tbl_data));

    
    soc_mem_field_get(unit, EPNI_COS_PROFILE_TABLEm , &tbl_data, TC_VALIDf, &(entry_info->int_pri_valid));
    soc_mem_field_get(unit, EPNI_COS_PROFILE_TABLEm , &tbl_data, TCf, &(entry_info->int_pri));

    
    soc_mem_field_get(unit, EPNI_COS_PROFILE_TABLEm , &tbl_data, DP_VALIDf, &(entry_info->color_valid));
    soc_mem_field_get(unit, EPNI_COS_PROFILE_TABLEm , &tbl_data, DPf, &(entry_info->color));

exit:
  SOCDNX_FUNC_RETURN;
} 


#define JER_PP_EG_ENCAP_MAP_ENC_MODE_TO_TEMPLATE_TEMPLATE_NOF_BITS (3)
#define JER_PP_EG_ENCAP_MAP_ENC_MODE_TO_SIZE_SIZE_NOF_BITS (6)

#define JER_PP_EG_ENCAP_MAP_ENC_MODE_TO_PROTOCOL_AND_PROTOCOL_ENABLED_NOF_BITS (SOC_SAND_PP_IP_PROTOCOL_NOF_BITS + 1)
#define JER_PP_EG_ENCAP_MAP_ENC_MODE_TO_PROTOCOL_AND_PROTOCOL_ENABLED_PROTOCOL_SET(protocol_enabled, protocol, protocol_and_protocol_enabled) \
            (protocol_and_protocol_enabled = ((protocol << 1) | protocol_enabled))


soc_error_t soc_jer_eg_encap_ip_tunnel_size_protocol_template_set(
   int                                                    unit, 
   int                                                    encapsulation_mode, 
   SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO *ip_tunnel_size_protocol_template) {

    uint32 reg32; 
    uint64 reg64; 
    uint32 reg64_array[2];  
    soc_reg_above_64_val_t reg_above_64; 
    uint32 value; 
    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_os_memset(reg64_array, 0x0, sizeof(reg64_array));

    
    if (SOC_IS_JERICHO_B0_AND_ABOVE(unit)) {
        
        SOCDNX_IF_ERR_EXIT(READ_EPNI_CFG_MAP_ENC_MODE_TO_TEMPLATEr(unit, REG_PORT_ANY, &reg32)); 
        SHR_BITCOPY_RANGE(&reg32, 
                          (encapsulation_mode * JER_PP_EG_ENCAP_MAP_ENC_MODE_TO_TEMPLATE_TEMPLATE_NOF_BITS), 
                          &ip_tunnel_size_protocol_template->ip_tunnel_template, 
                          0, 
                          JER_PP_EG_ENCAP_MAP_ENC_MODE_TO_TEMPLATE_TEMPLATE_NOF_BITS
                          ); 
        SOCDNX_IF_ERR_EXIT(WRITE_EPNI_CFG_MAP_ENC_MODE_TO_TEMPLATEr(unit, REG_PORT_ANY, reg32)); 

        
        SOCDNX_IF_ERR_EXIT(READ_EPNI_CFG_MAP_ENC_MODE_TO_SIZEr(unit, REG_PORT_ANY, &reg64)); 
        COMPILER_64_TO_32_LO(reg64_array[0], reg64); 
        COMPILER_64_TO_32_HI(reg64_array[1], reg64); 
        SHR_BITCOPY_RANGE(reg64_array, 
                          (encapsulation_mode * JER_PP_EG_ENCAP_MAP_ENC_MODE_TO_SIZE_SIZE_NOF_BITS), 
                          &ip_tunnel_size_protocol_template->encapsulation_size, 
                          0,
                          JER_PP_EG_ENCAP_MAP_ENC_MODE_TO_SIZE_SIZE_NOF_BITS
                          ); 
        COMPILER_64_SET(reg64 , reg64_array[1],reg64_array[0]);
        SOCDNX_IF_ERR_EXIT(WRITE_EPNI_CFG_MAP_ENC_MODE_TO_SIZEr(unit, REG_PORT_ANY, reg64)); 

        
        SOCDNX_IF_ERR_EXIT(READ_EPNI_CFG_MAP_ENC_MODE_TO_PROTOCOLr(unit, REG_PORT_ANY, reg_above_64)); 
        JER_PP_EG_ENCAP_MAP_ENC_MODE_TO_PROTOCOL_AND_PROTOCOL_ENABLED_PROTOCOL_SET(ip_tunnel_size_protocol_template->protocol_enable, ip_tunnel_size_protocol_template->protocol, value); 
        SHR_BITCOPY_RANGE(reg_above_64,
                          (encapsulation_mode * JER_PP_EG_ENCAP_MAP_ENC_MODE_TO_PROTOCOL_AND_PROTOCOL_ENABLED_NOF_BITS), 
                          &value, 
                          0, 
                          JER_PP_EG_ENCAP_MAP_ENC_MODE_TO_PROTOCOL_AND_PROTOCOL_ENABLED_NOF_BITS); 
        SOCDNX_IF_ERR_EXIT(WRITE_EPNI_CFG_MAP_ENC_MODE_TO_PROTOCOLr(unit, REG_PORT_ANY, reg_above_64)); 
    }

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t soc_jer_eg_encap_ip_tunnel_size_protocol_template_init(int unit) {

    uint32 reg32; 
    uint64 reg64; 
    SOCDNX_INIT_FUNC_DEFS;

    
    COMPILER_64_ZERO(reg64); 
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_CFG_MAP_OUTLIF_PROFILE_TO_ENC_MODEr(unit, REG_PORT_ANY, reg64)); 

    
    reg32 = 0; 
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_CFG_MAP_ENC_MODE_TO_TEMPLATEr(unit, REG_PORT_ANY, reg32));

    
    COMPILER_64_ZERO(reg64); 
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_CFG_MAP_ENC_MODE_TO_SIZEr(unit, REG_PORT_ANY, reg64)); 

    


exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_jer_eg_encap_null_value_set(int unit, uint32 *value) {

    uint32 reg; 
    SOCDNX_INIT_FUNC_DEFS;

    
    if (SOC_IS_JERICHO_B0_AND_ABOVE(unit)) {
        SOCDNX_SAND_IF_ERR_EXIT(READ_EPNI_CFG_MPLS_NULL_VALUESr(unit, SOC_CORE_ALL, &reg));
        soc_reg_field_set(unit, EPNI_CFG_MPLS_NULL_VALUESr, &reg, CFG_MPLS_NULL_LABELf, *value);
        SOCDNX_SAND_IF_ERR_EXIT(WRITE_EPNI_CFG_MPLS_NULL_VALUESr(unit, SOC_CORE_ALL, reg));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_jer_eg_encap_null_value_get(int unit, uint32 *value) {

    uint32 reg; 
    SOCDNX_INIT_FUNC_DEFS;

    
    if (SOC_IS_JERICHO_B0_AND_ABOVE(unit)) {
        SOCDNX_SAND_IF_ERR_EXIT(READ_EPNI_CFG_MPLS_NULL_VALUESr(unit, SOC_CORE_ALL, &reg));
        *value = soc_reg_field_get(unit, EPNI_CFG_MPLS_NULL_VALUESr, reg, CFG_MPLS_NULL_LABELf);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_jer_eg_encap_push_2_swap_init(int unit) {

    uint64 reg; 
    uint32 sh_mask=0, entry;
    soc_reg_above_64_val_t profile_to_push2swap_val;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_JERICHO_B0_AND_ABOVE(unit) && !SOC_IS_QAX_A0(unit)) {
        
         
        
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_occ_mgmt_get_app_mask(unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_MPLS_PUSH_OR_SWAP, &sh_mask));
           
        SOC_REG_ABOVE_64_CLEAR(profile_to_push2swap_val);
        for(entry = 0; entry < 64; entry++){   
            
            if (entry & sh_mask) {
                if (entry >= 32) {
                    profile_to_push2swap_val[1] |= (1 << (entry - 32));
                }
                else {
                    profile_to_push2swap_val[0] |= (1 << entry);
                }
            }
        }

        COMPILER_64_SET(reg, profile_to_push2swap_val[1], profile_to_push2swap_val[0]);
        SOCDNX_SAND_IF_ERR_EXIT(soc_reg64_set(unit, EPNI_MPLS_OUTLIF_PROFILE_PUSH_TO_SWAP_ENABLEDr, REG_PORT_ANY,  0,  reg));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



#include <soc/dpp/SAND/Utils/sand_footer.h>
