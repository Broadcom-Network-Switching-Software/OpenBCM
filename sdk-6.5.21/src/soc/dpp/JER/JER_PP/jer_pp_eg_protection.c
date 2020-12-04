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
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_protection.h>

#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>






#define JER_PP_EGRESS_PROTECTION_PATH_STATE_BITS                (1)

#define JER_PP_EGRESS_PROTECTION_PATH_NOF_ENTRY_OFFSET_BITS     (6)
#define JER_PP_EGRESS_PROTECTION_PATH_NOF_ENTRY_IDX_BITS        (9)

#define JER_PP_EGRESS_PROTECTION_PATH_ENTRY_OFFSET_MASK         (0x3F)
#define JER_PP_EGRESS_PROTECTION_PATH_ENTRY_IDX_MASK            (0x7FC0)


















 


soc_error_t soc_jer_egress_protection_state_verify(
   int unit,
   uint32 *protection_ndx,
   uint8 *path_state)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    if (protection_ndx) {
        if (*protection_ndx >= SOC_DPP_DEFS_GET(unit, nof_failover_egress_ids)) {
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


 


soc_error_t soc_jer_pp_eg_protection_init(int unit)
{
    uint32 field_val;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    
    field_val = (SOC_DPP_IS_PROTECTION_EGRESS_COUPLED(unit)) ? 1 : 0;
    rv = soc_reg_field32_modify(unit, EPNI_CFG_PROTECTION_PTR_MODEr, REG_PORT_ANY, CFG_PROTECTION_PTR_MODEf, field_val);
    SOCDNX_IF_ERR_EXIT(rv);

    


    


exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t soc_jer_egress_protection_state_set(
   int unit,
   uint32 protection_ndx,
   uint8 path_state)
{
    soc_error_t rv;
    uint32 tbl_idx, formated_path_state, epni_outlif_protection_path_entry_data[2];
    uint64 outlif_protection_val64_entry, outlif_protection_val64_field;
    uint32 entry_offset;
    SOCDNX_INIT_FUNC_DEFS;

    
    rv = soc_jer_egress_protection_state_verify(unit, &protection_ndx, &path_state);
    SOCDNX_IF_ERR_EXIT(rv);

    
    entry_offset = protection_ndx & JER_PP_EGRESS_PROTECTION_PATH_ENTRY_OFFSET_MASK;
    tbl_idx = (protection_ndx & JER_PP_EGRESS_PROTECTION_PATH_ENTRY_IDX_MASK) >>
              JER_PP_EGRESS_PROTECTION_PATH_NOF_ENTRY_OFFSET_BITS;

    
    rv = READ_EPNI_PROTECTION_PTR_TABLEm(unit, MEM_BLOCK_ANY, tbl_idx, &outlif_protection_val64_entry);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_mem_field64_get(unit, EPNI_PROTECTION_PTR_TABLEm, &outlif_protection_val64_entry, PROTECTION_PTR_TABLEf, &outlif_protection_val64_field);

    epni_outlif_protection_path_entry_data[0] = COMPILER_64_LO(outlif_protection_val64_field);
    epni_outlif_protection_path_entry_data[1] = COMPILER_64_HI(outlif_protection_val64_field);

    
    formated_path_state = SOC_SAND_BOOL2NUM_INVERSE(path_state);
    SHR_BITCOPY_RANGE(epni_outlif_protection_path_entry_data, entry_offset,
                      &formated_path_state, 0, JER_PP_EGRESS_PROTECTION_PATH_STATE_BITS);
    COMPILER_64_SET(outlif_protection_val64_field, epni_outlif_protection_path_entry_data[1], epni_outlif_protection_path_entry_data[0]);    

    
    soc_mem_field64_set(unit, EPNI_PROTECTION_PTR_TABLEm, &outlif_protection_val64_entry, PROTECTION_PTR_TABLEf, outlif_protection_val64_field);  
    rv = WRITE_EPNI_PROTECTION_PTR_TABLEm(unit, MEM_BLOCK_ALL, tbl_idx, &outlif_protection_val64_entry);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t soc_jer_egress_protection_state_get(
   int unit,
   uint32 protection_ndx,
   uint8 *path_state)
{
    soc_error_t rv;
    uint32 tbl_idx, formated_path_state = 0;
    uint32 epni_outlif_protection_path_entry_data[6];
    uint64 outlif_protection_val64_entry, outlif_protection_val64_field;
    uint32 entry_offset;
    SOCDNX_INIT_FUNC_DEFS;

    
    rv = soc_jer_egress_protection_state_verify(unit, &protection_ndx, NULL);
    SOCDNX_IF_ERR_EXIT(rv);

    
    entry_offset = protection_ndx & JER_PP_EGRESS_PROTECTION_PATH_ENTRY_OFFSET_MASK;
    tbl_idx = (protection_ndx & JER_PP_EGRESS_PROTECTION_PATH_ENTRY_IDX_MASK) >>
              JER_PP_EGRESS_PROTECTION_PATH_NOF_ENTRY_OFFSET_BITS;

    
    rv = READ_EPNI_PROTECTION_PTR_TABLEm(unit, MEM_BLOCK_ANY, tbl_idx, &outlif_protection_val64_entry);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_mem_field64_get(unit, EPNI_PROTECTION_PTR_TABLEm, &outlif_protection_val64_entry, PROTECTION_PTR_TABLEf, &outlif_protection_val64_field);

    epni_outlif_protection_path_entry_data[0] = COMPILER_64_LO(outlif_protection_val64_field);
    epni_outlif_protection_path_entry_data[1] = COMPILER_64_HI(outlif_protection_val64_field);

    
    SHR_BITCOPY_RANGE(&formated_path_state, 0,
                      epni_outlif_protection_path_entry_data, entry_offset, JER_PP_EGRESS_PROTECTION_PATH_STATE_BITS);

    *path_state = SOC_SAND_NUM2BOOL_INVERSE(formated_path_state);

exit:
    SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>



