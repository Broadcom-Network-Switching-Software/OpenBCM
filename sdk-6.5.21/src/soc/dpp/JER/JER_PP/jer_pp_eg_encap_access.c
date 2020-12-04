
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#include <soc/mcm/memregs.h>

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS






#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap_access.h>
#include <soc/dpp/drv.h>









 
 








soc_error_t 
  soc_jer_eg_encap_access_roo_link_layer_format_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32          outlif,
    SOC_SAND_IN  JER_PP_EG_ENCAP_ACCESS_ROO_LL_ENTRY_FORMAT  *tbl_data
  ) {
  uint32
    res = SOC_SAND_OK;
  uint32
    tmp = 0,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  uint64 mac64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);

  

  
  tmp = ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_LINK_LAYER;
  res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  soc_mem_field32_set(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_TAGS_MSBf, tbl_data->outer_tag_msb); 
  soc_mem_field32_set(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_ETHER_TYPE_INDEXf, tbl_data->ether_type_index); 
  soc_mem_field32_set(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_REMARK_PROFILEf, tbl_data->remark_profile); 
  soc_mem_field32_set(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_NUMBER_OF_TAGSf, tbl_data->nof_tags); 
  soc_mem_field32_set(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_ACTION_SAf, tbl_data->sa_lsb); 
  COMPILER_64_SET(mac64, tbl_data->dest_mac[1], tbl_data->dest_mac[0]);
  soc_mem_field64_set(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_ACTION_DAf, mac64); 
  
  soc_mem_field32_set(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_ACTION_IDENTIFIERf, 0x1); 
  soc_mem_field32_set(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_DROPf, tbl_data->drop); 
  soc_mem_field32_set(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_PCP_DEI_PROFILEf, tbl_data->pcp_dei_profile); 

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1100, exit, arad_pp_eg_encap_access_entry_write(unit, outlif, data));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_jer_eg_encap_access_roo_link_layer_format_tbl_set()", outlif, 0);
}


soc_error_t
  soc_jer_eg_encap_access_roo_link_layer_format_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT JER_PP_EG_ENCAP_ACCESS_ROO_LL_ENTRY_FORMAT  *tbl_data
  ) {
  uint32
    res = SOC_SAND_OK;
  uint32
    tmp = 0,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE], 
    roo_link_format_identifier;
  uint64 mac64;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_FORMAT, 1);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, arad_pp_eg_encap_access_entry_read(unit, outlif, data));

  
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  if ((tmp & ARAD_PP_EG_ENCAP_ACCESS_LL_ENTRY_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_LINK_LAYER)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
  }

  
  roo_link_format_identifier = soc_mem_field32_get(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_ACTION_IDENTIFIERf); 
  
  
  if (roo_link_format_identifier  != 1) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 20, exit);
  }

  tbl_data->outer_tag_msb = soc_mem_field32_get(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_TAGS_MSBf); 
  tbl_data->ether_type_index = soc_mem_field32_get(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_ETHER_TYPE_INDEXf); 
  tbl_data->remark_profile = soc_mem_field32_get(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_REMARK_PROFILEf); 
  tbl_data->nof_tags = soc_mem_field32_get(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_NUMBER_OF_TAGSf); 
  tbl_data->sa_lsb = soc_mem_field32_get(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_ACTION_SAf); 
  soc_mem_field64_get(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_ACTION_DAf, &mac64); 
  COMPILER_64_TO_32_HI(tbl_data->dest_mac[1], mac64);
  COMPILER_64_TO_32_LO(tbl_data->dest_mac[0], mac64);
  tbl_data->drop = soc_mem_field32_get(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_DROPf); 
  tbl_data->pcp_dei_profile = soc_mem_field32_get(unit, EDB_LINK_LAYER_OR_ARP_NEW_FORMATm, data, ARP_LL_ACTION_PCP_DEI_PROFILEf); 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_jer_eg_encap_access_roo_link_layer_format_tbl_get_unsafe()", outlif, 0);

}
 
soc_error_t 
  jer_pp_eg_encap_access_init_outrif_max(
        SOC_SAND_IN  int             unit
   )
{
    uint32 nof_outrifs = SOC_DPP_CONFIG(unit)->l3.nof_rifs;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_EPNI_OUTRIF_MAX_VALUEr(unit, REG_PORT_ANY, nof_outrifs));
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_EGQ_OUTRIF_MAX_VALUEr(unit, REG_PORT_ANY, nof_outrifs));
    if (SOC_IS_JERICHO_PLUS(unit)) {
        SOCDNX_SAND_IF_ERR_EXIT(WRITE_EDB_CFG_OUTRIF_MAXr(unit, nof_outrifs));
    }
    if (SOC_IS_QAX(unit)) {
        nof_outrifs = (nof_outrifs ? (nof_outrifs - 1) : 0); 
        SOCDNX_SAND_IF_ERR_EXIT(
            soc_reg_above_64_field32_modify(unit, ITE_ITPP_GENERAL_CFGr, REG_PORT_ANY, 0, ITPP_OUTRIF_RANGEf, nof_outrifs));
    }


exit:
    SOCDNX_FUNC_RETURN;
}




#include <soc/dpp/SAND/Utils/sand_footer.h>


