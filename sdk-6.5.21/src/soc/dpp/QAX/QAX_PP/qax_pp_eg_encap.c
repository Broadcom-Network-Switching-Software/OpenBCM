
 
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
#include <soc/dcmn/dcmn_mem.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>


#include <soc/dpp/QAX/QAX_PP/qax_pp_eg_encap.h>



#include <soc/dpp/drv.h>





#define QAX_PP_EPNI_RESERVED_MPLS_PROFILE_TABLE_SIZE (2)


#define SOC_QAX_EG_ENCAP_PER_PKT_HDR_COMP_VALUE_LSBS_MASK   ((1 << PPC_API_EG_ENCAP_PER_PKT_HDR_COMP_NOF_VALUE_LSBS) - 1)
#define SOC_QAX_EG_ENCAP_PER_PKT_HDR_COMP_VALUE_MSB_MASK    (1 << PPC_API_EG_ENCAP_PER_PKT_HDR_COMP_NOF_VALUE_LSBS)
#define SOC_QAX_EG_ENCAP_PER_PKT_HDR_COMP_NOF_VALUE_BITS    (PPC_API_EG_ENCAP_PER_PKT_HDR_COMP_NOF_VALUE_LSBS + 1 ) 


















soc_error_t soc_qax_eg_encap_additional_label_profile_set(int unit, SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO *additional_label_profile_info, int profile_index){

    uint32
      data[QAX_PP_EPNI_RESERVED_MPLS_PROFILE_TABLE_SIZE];
    uint32 is_exp_set, is_ttl_set;
    SOCDNX_INIT_FUNC_DEFS;


    SOCDNX_IF_ERR_EXIT(soc_mem_read(
            unit,
            EPNI_RESERVED_MPLS_PROFILE_TABLEm,
            MEM_BLOCK_ANY,
            profile_index,
            data
          ));

    is_ttl_set = (additional_label_profile_info->ttl_model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_SET) ? 1 : 0;
    is_exp_set = (additional_label_profile_info->exp_model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_SET) ? 1 : 0;

    soc_mem_field32_set(unit, EPNI_RESERVED_MPLS_PROFILE_TABLEm, data, EXPf, additional_label_profile_info->exp);
    soc_mem_field32_set(unit, EPNI_RESERVED_MPLS_PROFILE_TABLEm, data, TTLf, additional_label_profile_info->ttl);
    soc_mem_field32_set(unit, EPNI_RESERVED_MPLS_PROFILE_TABLEm, data, EXP_MODELf, is_exp_set);
    soc_mem_field32_set(unit, EPNI_RESERVED_MPLS_PROFILE_TABLEm, data, TTL_MODELf, is_ttl_set);
    soc_mem_field32_set(unit, EPNI_RESERVED_MPLS_PROFILE_TABLEm, data, LABEL_TYPEf, additional_label_profile_info->label_type);
    soc_mem_field32_set(unit, EPNI_RESERVED_MPLS_PROFILE_TABLEm, data, LABELf, additional_label_profile_info->label);
    soc_mem_field32_set(unit, EPNI_RESERVED_MPLS_PROFILE_TABLEm, data, VALIDf, 1);

    SOCDNX_IF_ERR_EXIT(soc_mem_write(
            unit,
            EPNI_RESERVED_MPLS_PROFILE_TABLEm,
            MEM_BLOCK_ANY,
            profile_index,
            data
          ));

exit:
  SOCDNX_FUNC_RETURN;

}


soc_error_t soc_qax_eg_encap_additional_label_profile_get(int unit, SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO *additional_label_profile_info, int profile_index){

  uint32
    data[QAX_PP_EPNI_RESERVED_MPLS_PROFILE_TABLE_SIZE];
  uint32 is_exp_set, is_ttl_set;
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_IF_ERR_EXIT(soc_mem_read(
          unit,
          EPNI_RESERVED_MPLS_PROFILE_TABLEm,
          MEM_BLOCK_ANY,
          profile_index,
          data
        ));



  additional_label_profile_info->exp  = soc_mem_field32_get(unit, EPNI_RESERVED_MPLS_PROFILE_TABLEm, data, EXPf);
  additional_label_profile_info->ttl  = soc_mem_field32_get(unit, EPNI_RESERVED_MPLS_PROFILE_TABLEm, data, TTLf);

  is_ttl_set = soc_mem_field32_get(unit, EPNI_RESERVED_MPLS_PROFILE_TABLEm, data, TTL_MODELf);;
  is_exp_set = soc_mem_field32_get(unit, EPNI_RESERVED_MPLS_PROFILE_TABLEm, data, EXP_MODELf);;
  additional_label_profile_info->ttl_model = is_ttl_set ? SOC_SAND_PP_MPLS_TUNNEL_MODEL_SET : SOC_SAND_PP_MPLS_TUNNEL_MODEL_COPY;
  additional_label_profile_info->exp_model = is_exp_set ? SOC_SAND_PP_MPLS_TUNNEL_MODEL_SET : SOC_SAND_PP_MPLS_TUNNEL_MODEL_COPY;

  additional_label_profile_info->label_type = soc_mem_field32_get(unit, EPNI_RESERVED_MPLS_PROFILE_TABLEm, data, LABEL_TYPEf);
  additional_label_profile_info->label = soc_mem_field32_get(unit, EPNI_RESERVED_MPLS_PROFILE_TABLEm, data, LABELf);
  
exit:
  SOCDNX_FUNC_RETURN;
}


uint32
soc_qax_eg_encap_header_compensation_per_cud_set(int unit, int cud_msb, int value){
    uint32 res;
    soc_reg_above_64_val_t reg_val;
    uint32 field_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    field_val  = (value < 0) ? -(value) : (value);
    field_val &= SOC_QAX_EG_ENCAP_PER_PKT_HDR_COMP_VALUE_LSBS_MASK;
    field_val |= (value < 0) ? SOC_QAX_EG_ENCAP_PER_PKT_HDR_COMP_VALUE_MSB_MASK : 0;


    
    res = READ_EGQ_PER_PACKET_HDR_COMPr(unit, reg_val);
    SOCDNX_IF_ERR_EXIT(res);

    res = soc_sand_bitstream_set_any_field(&field_val,
                                     cud_msb * SOC_QAX_EG_ENCAP_PER_PKT_HDR_COMP_NOF_VALUE_BITS,
                                     SOC_QAX_EG_ENCAP_PER_PKT_HDR_COMP_NOF_VALUE_BITS,
                                     reg_val);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = WRITE_EGQ_PER_PACKET_HDR_COMPr(unit, reg_val);
    SOCDNX_IF_ERR_EXIT(res);
  
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
soc_qax_eg_encap_header_compensation_per_cud_get(int unit, int cud_msb, int *value){
    uint32 res;
    soc_reg_above_64_val_t reg_val;
    uint32 field_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    res = READ_EGQ_PER_PACKET_HDR_COMPr(unit, reg_val);
    SOCDNX_IF_ERR_EXIT(res);

    res = soc_sand_bitstream_get_any_field(reg_val,
                                     cud_msb * SOC_QAX_EG_ENCAP_PER_PKT_HDR_COMP_NOF_VALUE_BITS,
                                     SOC_QAX_EG_ENCAP_PER_PKT_HDR_COMP_NOF_VALUE_BITS,
                                     &field_val);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    
    *value = field_val & SOC_QAX_EG_ENCAP_PER_PKT_HDR_COMP_VALUE_LSBS_MASK; 

    *value *= (field_val & SOC_QAX_EG_ENCAP_PER_PKT_HDR_COMP_VALUE_MSB_MASK) ? -1 : 1;

exit:
    SOCDNX_FUNC_RETURN;
}




#include <soc/dpp/SAND/Utils/sand_footer.h>

