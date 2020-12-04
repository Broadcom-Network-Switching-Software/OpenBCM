/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_LIF



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_mem.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/register.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_lif.h>
#include  <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/PPC/ppc_api_lif.h>


#define QAX_PP_LIF_NOF_DEFAULT_NATIVE_OUTLIFS (4)

soc_error_t qax_pp_lif_default_native_ac_outlif_init(int unit) {
    soc_reg_above_64_val_t reg_above_64_val; 
    int default_outlif_id = 0;

    
    uint32 out_lif_null_entry = ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, SOC_PPC_LIF_NULL_LOCAL_OUTLIF_ID); 
    SOCDNX_INIT_FUNC_DEFS;
     
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val); 

    for (default_outlif_id = 0; default_outlif_id < QAX_PP_LIF_NOF_DEFAULT_NATIVE_OUTLIFS; default_outlif_id++) {
        SOC_REG_ABOVE_64_RANGE_COPY(reg_above_64_val, default_outlif_id * SOC_DPP_DEFS_GET(unit, out_lif_nof_bits), &out_lif_null_entry, 0, SOC_DPP_DEFS_GET(unit, out_lif_nof_bits)); 
    }
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_CFG_NATIVE_DEFAULT_AC_POINTERSr(unit, reg_above_64_val)); 
exit:
  SOCDNX_FUNC_RETURN;

}

soc_error_t qax_pp_lif_default_native_ac_outlif_set(int unit, uint32 local_out_lif_id) {

    soc_reg_above_64_val_t reg_above_64_val; 
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val); 

    
    SOCDNX_VERIFY(SOC_DPP_DEFS_GET(unit, out_lif_nof_bits) <= 8*sizeof(local_out_lif_id)); 

    SOC_REG_ABOVE_64_RANGE_COPY(reg_above_64_val, 0, &local_out_lif_id, 0, SOC_DPP_DEFS_GET(unit, out_lif_nof_bits)); 

    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_CFG_NATIVE_DEFAULT_AC_POINTERSr(unit, reg_above_64_val)); 
                                      

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t qax_pp_lif_default_native_ac_outlif_get(int unit, uint32* local_out_lif_id) {

    soc_reg_above_64_val_t reg_above_64_val; 
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_VERIFY(SOC_DPP_DEFS_GET(unit, out_lif_nof_bits) <= 8*sizeof(*local_out_lif_id)); 

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val); 
    SOCDNX_IF_ERR_EXIT(READ_EPNI_CFG_NATIVE_DEFAULT_AC_POINTERSr(unit, reg_above_64_val)); 

    SOC_REG_ABOVE_64_RANGE_COPY(local_out_lif_id, 0, reg_above_64_val, 0, SOC_DPP_DEFS_GET(unit, out_lif_nof_bits)); 

exit:
  SOCDNX_FUNC_RETURN;
}


