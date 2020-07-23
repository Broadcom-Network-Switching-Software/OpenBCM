/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INGRESS




#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/JER/JER_PP/jer_pp_ing_protection.h>

#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>






#define JER_PP_INGRESS_PROTECTION_PATH_STATE_BITS               (1)

#define JER_PP_INGRESS_PROTECTION_PATH_NOF_ENTRY_OFFSET_BITS    (3)
#define JER_PP_INGRESS_PROTECTION_PATH_NOF_ENTRY_IDX_BITS       (13)

#define JER_PP_INGRESS_PROTECTION_PATH_ENTRY_OFFSET_MASK        (0x7)
#define JER_PP_INGRESS_PROTECTION_PATH_ENTRY_IDX_MASK           (0x7FF8)


















 


soc_error_t soc_jer_ingress_protection_state_verify(
   int unit,
   uint32 *protection_ndx,
   uint8 *path_state)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    if (protection_ndx) {
        if (*protection_ndx >= SOC_DPP_DEFS_GET(unit, nof_failover_ingress_ids)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Protection index out of range")));
        }
    }

    
    if (path_state) {
        if (*path_state > 1) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Path state out of range")));
        }
    }

    SOC_EXIT;
exit:
    SOCDNX_FUNC_RETURN;
}


 


soc_error_t soc_jer_pp_ing_protection_init(int unit)
{
    uint32 field_val;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    
    field_val = (SOC_DPP_IS_PROTECTION_INGRESS_COUPLED(unit)) ? 1 : 0;
    if (SOC_IS_JERICHO_PLUS_A0(unit)) {
        rv = soc_reg_above_64_field32_modify(unit, IHP_VTT_GENERAL_CONFIGS_1r, REG_PORT_ANY,  0, COUPLED_PATH_SELECT_POINTERf, field_val);
    }
    else {
        rv = soc_reg_field32_modify(unit, IHP_VTT_GENERAL_CONFIGS_1r, REG_PORT_ANY, COUPLED_PATH_SELECT_POINTERf, field_val);
    }
    SOCDNX_IF_ERR_EXIT(rv);

    
    field_val = (SOC_DPP_IS_PROTECTION_FEC_ACCELERATED_REROUTE_MODE(unit)) ? 1 : 0;
    rv = soc_reg_field32_modify(unit, IHB_FER_GENERAL_CONFIGURATIONSr, REG_PORT_ANY, ENABLE_PATH_AND_FACILITY_PROTECTIONf, field_val);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t soc_jer_ingress_protection_state_set(
   int unit,
   uint32 protection_ndx,
   uint8 path_state)
{
    soc_error_t rv;
    uint32 tbl_idx, formated_path_state;
    uint32 ihp_vtt_path_select_entry_data;
    uint8 entry_offset;
    SOCDNX_INIT_FUNC_DEFS;

    
    rv = soc_jer_ingress_protection_state_verify(unit, &protection_ndx, &path_state);
    SOCDNX_IF_ERR_EXIT(rv);

    
    ihp_vtt_path_select_entry_data = 0;

    
    entry_offset = protection_ndx & JER_PP_INGRESS_PROTECTION_PATH_ENTRY_OFFSET_MASK;
    tbl_idx = (protection_ndx & JER_PP_INGRESS_PROTECTION_PATH_ENTRY_IDX_MASK) >>
              JER_PP_INGRESS_PROTECTION_PATH_NOF_ENTRY_OFFSET_BITS;

    
    rv = READ_IHP_VTT_PATH_SELECTm(unit, MEM_BLOCK_ANY, tbl_idx, &ihp_vtt_path_select_entry_data);
    SOCDNX_IF_ERR_EXIT(rv);

    
    formated_path_state = SOC_SAND_BOOL2NUM_INVERSE(path_state);
    SHR_BITCOPY_RANGE(&ihp_vtt_path_select_entry_data, entry_offset,
                      &formated_path_state, 0, JER_PP_INGRESS_PROTECTION_PATH_STATE_BITS);

    
    if (SOC_IS_JERICHO_PLUS_A0(unit)) {
    	 
    	SOCDNX_IF_ERR_EXIT(WRITE_IHP_INDIRECT_WR_MASKr(unit,SOC_CORE_ALL, 0));
    }
    rv = WRITE_IHP_VTT_PATH_SELECTm(unit, MEM_BLOCK_ALL, tbl_idx, &ihp_vtt_path_select_entry_data);
    SOCDNX_IF_ERR_EXIT(rv);
    if (SOC_IS_JERICHO_PLUS_A0(unit)) {
    	SOCDNX_IF_ERR_EXIT(WRITE_IHP_INDIRECT_WR_MASKr(unit,SOC_CORE_ALL, 0xffffffff));
    }    

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t soc_jer_ingress_protection_state_get(
   int unit,
   uint32 protection_ndx,
   uint8 *path_state)
{
    soc_error_t rv;
    uint32 tbl_idx, formated_path_state;
    uint32 ihp_vtt_path_select_entry_data;
    uint8 entry_offset;
    SOCDNX_INIT_FUNC_DEFS;

    
    rv = soc_jer_ingress_protection_state_verify(unit, &protection_ndx, NULL);
    SOCDNX_IF_ERR_EXIT(rv);

    
    ihp_vtt_path_select_entry_data = 0;
    formated_path_state = 0;

    
    entry_offset = protection_ndx & JER_PP_INGRESS_PROTECTION_PATH_ENTRY_OFFSET_MASK;
    tbl_idx = (protection_ndx & JER_PP_INGRESS_PROTECTION_PATH_ENTRY_IDX_MASK) >>
              JER_PP_INGRESS_PROTECTION_PATH_NOF_ENTRY_OFFSET_BITS;

    
    rv = READ_IHP_VTT_PATH_SELECTm(unit, MEM_BLOCK_ANY, tbl_idx, &ihp_vtt_path_select_entry_data);
    SOCDNX_IF_ERR_EXIT(rv);

    
    SHR_BITCOPY_RANGE(&formated_path_state, 0,
                      &ihp_vtt_path_select_entry_data, entry_offset, JER_PP_INGRESS_PROTECTION_PATH_STATE_BITS);
    *path_state = SOC_SAND_NUM2BOOL_INVERSE(formated_path_state);

exit:
    SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>



