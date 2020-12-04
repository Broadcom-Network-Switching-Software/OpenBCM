/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_L3




#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/utils.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_extender.h>
#include <soc/dpp/PPC/ppc_api_extender.h>
#include <soc/dpp/TMC/tmc_api_ports.h>
#include <soc/dpp/JER/jer_ports.h>
#include <soc/dpp/QAX/qax_ports.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/PPC/ppc_api_fp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_vtt.h>





#define EPNI_CFG_KEEP_ING_ECID_REG_SIZE 8





















soc_error_t 
arad_pp_extender_init(int unit){
    int rv;
    SOC_TMC_PORT_SWAP_GLOBAL_INFO global_swap_info;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_reg_field32_modify(unit, EPNI_CFG_ENABLE_ETAGr, REG_PORT_ANY, CFG_ENABLE_ETAGf, 1);
    SOCDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_JERICHO(unit)) {
        sal_memset(&global_swap_info, 0, sizeof(SOC_TMC_PORT_SWAP_GLOBAL_INFO));
        if (SOC_DPP_CONFIG(unit)->pp.custom_feature_vn_tag_port_termination == 1) {
            global_swap_info.global_tag_swap_n_size = 0x1; 
        } else {
            global_swap_info.global_tag_swap_n_size = 0x2;  
        }
        global_swap_info.tag_swap_n_offset_0 = 0xc;     
        global_swap_info.tag_swap_n_offset_1 = 0xc;     
        
        if (SOC_IS_QAX(unit)) {
            rv = soc_qax_port_swap_global_info_set(unit, &global_swap_info);
        }
        else if (SOC_IS_JERICHO(unit)){
            rv = soc_jer_port_swap_global_info_set(unit, &global_swap_info);
        }
        SOCDNX_IF_ERR_EXIT(rv);
    }


    rv = sw_state_access[unit].dpp.soc.arad.pp.extender_info.alloc(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
arad_pp_extender_deinit(int unit){
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_reg_field32_modify(unit, EPNI_CFG_ENABLE_ETAGr, REG_PORT_ANY, CFG_ENABLE_ETAGf, 0);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
arad_pp_extender_port_info_set(int unit, SOC_PPC_PORT port, SOC_PPC_EXTENDER_PORT_INFO *port_info){
    int rv;
    soc_reg_above_64_val_t data;
    uint32 tbl_ndx, tbl_offset, port_mask;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(port_info);

    tbl_ndx = port / SOC_SAND_NOF_BITS_IN_UINT32;

    tbl_offset = port % SOC_SAND_NOF_BITS_IN_UINT32;

    port_mask = 1 << tbl_offset;

    
    rv = READ_EPNI_CFG_KEEP_ING_ECIDr(unit, REG_PORT_ANY, data);
    SOCDNX_IF_ERR_EXIT(rv);

    
    switch (port_info->ing_ecid_mode) {
    case SOC_PPC_EXTENDER_PORT_ING_ECID_NOP:
        data[tbl_ndx] &= ~port_mask;
        break;
    case SOC_PPC_EXTENDER_PORT_ING_ECID_KEEP:
        data[tbl_ndx] |= port_mask;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong ing ecid mode.")));
    }

    rv = WRITE_EPNI_CFG_KEEP_ING_ECIDr(unit, REG_PORT_ANY, data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
arad_pp_extender_port_info_get(int unit, SOC_PPC_PORT port, SOC_PPC_EXTENDER_PORT_INFO *port_info){
    int rv;
    soc_reg_above_64_val_t data;
    uint32 tbl_ndx, tbl_offset, port_mask;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(port_info);

    sal_memset(port_info, 0, sizeof(SOC_PPC_EXTENDER_PORT_INFO));

    tbl_ndx = port / SOC_SAND_NOF_BITS_IN_UINT32;

    tbl_offset = port % SOC_SAND_NOF_BITS_IN_UINT32;

    port_mask = 1 << tbl_offset;

    
    rv = READ_EPNI_CFG_KEEP_ING_ECIDr(unit, REG_PORT_ANY, data);
    SOCDNX_IF_ERR_EXIT(rv);

    port_info->ing_ecid_mode = (data[tbl_ndx] & port_mask) ? SOC_PPC_EXTENDER_PORT_ING_ECID_KEEP : SOC_PPC_EXTENDER_PORT_ING_ECID_NOP;

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC soc_error_t 
arad_pp_extender_global_etag_ethertype_add_remove(int unit, 
                                     uint16 etag_tpid,
                                     uint8 add
                                     ){
    int rv;
    ARAD_PP_ISEM_ACCESS_KEY sem_key;
    uint32  nof_tables;
    SOC_DPP_DBAL_SW_TABLE_IDS  dbal_tables_id[ARAD_PP_ISEM_ACCESS_NOF_TABLES];
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_SAND_SUCCESS_FAILURE success;
    ARAD_PP_ISEM_ACCESS_ENTRY sem_entry;
    uint8 is_duplicate = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
    SOCDNX_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);

    sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_EXTENDER_UNTAG_CHECK;

    
    rv = arad_pp_dbal_vtt_sw_db_get(unit, &sem_key, &nof_tables, dbal_tables_id, &is_duplicate);
    qual_vals[0].type = SOC_PPC_FP_QUAL_PORT_EXTENDER_ETAG;
    qual_vals[0].val.arr[0] = etag_tpid;

    if (add) {
        
        
        
        rv = arad_pp_dbal_entry_add(unit, dbal_tables_id[0], qual_vals, 0,  &sem_entry, &success);
        SOCDNX_IF_ERR_EXIT(rv);
    } else { 
        
        rv = arad_pp_dbal_entry_delete(unit, dbal_tables_id[0], qual_vals, &success);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    if (success != SOC_SAND_SUCCESS){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Failed to %s old etag tpid %s dbal."), (add)?"add":"remove", (add)?"to":"from"));
    }


exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
arad_pp_extender_global_etag_ethertype_set(int unit, uint16 etag_tpid){
    int rv;
    uint16 old_etag_tpid;

    SOCDNX_INIT_FUNC_DEFS;
    
    
    rv = sw_state_access[unit].dpp.soc.arad.pp.extender_info.etag_ethertype.get(unit, &old_etag_tpid);
    SOCDNX_IF_ERR_EXIT(rv);

    
    if (old_etag_tpid != 0) {
        rv = arad_pp_extender_global_etag_ethertype_add_remove(unit, old_etag_tpid, FALSE);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    rv = sw_state_access[unit].dpp.soc.arad.pp.extender_info.etag_ethertype.set(unit, etag_tpid);
    SOCDNX_IF_ERR_EXIT(rv);

    
    if (etag_tpid != 0) { 
        
        rv = arad_pp_extender_global_etag_ethertype_add_remove(unit, etag_tpid, TRUE);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}





soc_error_t 
arad_pp_extender_global_etag_ethertype_get(int unit, uint16 *etag_tpid){
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    rv = sw_state_access[unit].dpp.soc.arad.pp.extender_info.etag_ethertype.get(unit, etag_tpid);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
arad_pp_extender_eve_etag_format_set(int unit, uint32 edit_profile, uint8 is_extender){
    int rv;
    uint32 etag_format[1];

    SOCDNX_INIT_FUNC_DEFS;

    
    rv = READ_EPNI_CFG_EVE_ETAG_FORMATr(unit, SOC_CORE_ALL, etag_format);
    SOCDNX_IF_ERR_EXIT(rv);

    
    SHR_BITWRITE(etag_format, edit_profile, is_extender);

    
    rv = WRITE_EPNI_CFG_EVE_ETAG_FORMATr(unit, SOC_CORE_ALL, *etag_format);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
arad_pp_extender_eve_etag_format_get(int unit, uint32 edit_profile, uint8 *is_extender){
    int rv;
    uint32 etag_format[1];

    SOCDNX_INIT_FUNC_DEFS;

    
    rv = READ_EPNI_CFG_EVE_ETAG_FORMATr(unit, SOC_CORE_ALL, etag_format);
    SOCDNX_IF_ERR_EXIT(rv);

    
    (*is_extender) = SHR_BITGET(etag_format, edit_profile);

exit:
    SOCDNX_FUNC_RETURN;

}
#include <soc/dpp/SAND/Utils/sand_footer.h>



