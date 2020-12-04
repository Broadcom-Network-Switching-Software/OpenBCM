/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MPLS



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/utils.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>

#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mpls_term.h>

#include <soc/dpp/PPC/ppc_api_mpls_term.h>
#include <soc/dpp/JER/JER_PP/jer_pp_mpls_term.h>
#include <shared/swstate/access/sw_state_access.h>





#define _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_NOF_BITS           (9)
#define _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_TO_TERMINATE_WIDTH         (4)

#define _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_FORCE_IS_OAM_OFFSET (0)
#define _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_FORCE_FWD_CODE_VALUE_OFFSET (1)
#define _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_FORCE_FWD_CODE_OFFSET (5)
#define _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_HAS_CW_OFFSET (6)
#define _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_LABELS_TO_TERMINATE_OFFSET (7)

#define _SOC_JER_PP_MPLS_TERM_RANGE_PROFILE_NOF_FIELDS          (5)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_TAG_MODE_SHIFT        (0)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_HAS_CW_SHIFT          (1)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_OUTER_VID_SHIFT   (2)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_INNER_VID_SHIFT   (3)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_USE_BASE_SHIFT        (4)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_TAG_MODE_BIT          (1)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_HAS_CW_BIT            (2)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_OUTER_VID_BIT     (4)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_INNER_VID_BIT     (8)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_USE_BASE_BIT          (16)

#define _SOC_JER_PP_MPLS_TERM_LABEL_MAX  ((1 << 20) - 1)

#define _SOC_JER_PP_MPLS_TERM_LABEL_IN_RANGE(_label_)   \
        (_label_ <= _SOC_JER_PP_MPLS_TERM_LABEL_MAX)

#define _SOC_JER_PP_MPLS_TERM_RANGE_NDX_IML                    (1)











typedef enum {
    _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_IPV4_EXP = 0,
    _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_IPV6_EXP,
    _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_ELI,
    _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_GAL,
    _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_RA,
    _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_OAM_ALERT,
    _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_NOF
} _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX;











soc_error_t soc_jer_pp_mpls_termination_spacial_labels_init(int unit) {
    uint32 reg, field_val;
    soc_reg_above_64_val_t  reg_above_64; 
    uint64 reg_64;
    uint32 label, expected_ttl, expected_bos, check_bos, check_ttl;
    uint32 force_is_oam = 0, force_fwd_code = 0, force_fwd_code_value = 0, force_has_cw = 0;
    uint32 labels_to_terminate;
    uint32 special_label_profile;
    uint32 label_index;
    uint32 type;
    uint32 special_label_valid[_SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_NOF];
    uint32 special_label_bitmap = 0xFFFF; 
    uint32 forwarding_code = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Error, Only Jericho device supports this function\n")));
    }

#ifdef BCM_WARM_BOOT_SUPPORT
	if (SOC_WARM_BOOT(unit)) {
		SOCDNX_FUNC_RETURN;
    }
#endif 

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);

    SOCDNX_SAND_IF_ERR_EXIT(soc_reg_above_64_get(unit,IHP_VTT_GENERAL_CONFIGS_1r, REG_PORT_ANY, 0, reg_above_64));
    field_val = 0x1;
    soc_reg_above_64_field32_set(unit, IHP_VTT_GENERAL_CONFIGS_1r, reg_above_64, MPLS_ENH_TT_ENABLEf, field_val);
    SOCDNX_SAND_IF_ERR_EXIT(soc_reg_above_64_set(unit,IHP_VTT_GENERAL_CONFIGS_1r, REG_PORT_ANY, 0, reg_above_64));

    if (SOC_IS_JERICHO_AND_BELOW(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_pp_mpls_termination_vccv_type_ttl1_oam_classification_set(unit, 0));
    }
    else {
        
        SOCDNX_IF_ERR_EXIT(soc_jer_pp_mpls_termination_vccv_type_ttl1_oam_classification_set(unit, 1));
    }

    
    
    if (SOC_IS_JERICHO_PLUS(unit) && !SOC_IS_QAX_A0(unit)) {
        SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_RESERVED_SPARE_0r(unit, SOC_CORE_ALL, &reg));
        SHR_BITCLR(&reg,0);
        soc_reg_field_set(unit, IHP_RESERVED_SPARE_0r, &reg, RESERVED_SPARE_0f, reg);
        SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_RESERVED_SPARE_0r(unit, SOC_CORE_ALL, reg));
    }

    
    for (label_index=0; label_index<_SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_NOF; label_index++) {
        special_label_valid[label_index] = 1;
    }

    
    
    if (!SOC_DPP_CONFIG(unit)->pp.oam_enable) {
        special_label_valid[_SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_RA] = 0;
    }

    if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "y1711_enabled",0) == 1)
      ||(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_1711_enable", 0) == 1)){
        special_label_valid[_SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_OAM_ALERT] = 0;
    }

    if ((SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode >= 20 && SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode <= 23)) {
        special_label_valid[_SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_IPV4_EXP] = 0;
        special_label_valid[_SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_IPV6_EXP] = 0;
    }

     
    for (label_index=0; label_index<_SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_NOF; label_index++) {

        
        
        if (special_label_valid[label_index]) {
            switch (label_index) {
            case _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_ELI:
                label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_ELI;
                expected_ttl = 0;
                expected_bos = 0;
                check_bos = 1;
                check_ttl = 0;
                labels_to_terminate = 2;
                force_is_oam = 0;
                force_fwd_code = 0;
                force_fwd_code_value = 0;
                force_has_cw = 0;
                break;
            case _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_GAL:
                label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_GAL;
                expected_ttl = 0;
                expected_bos = 0;
                check_bos = 0;
                check_ttl = 0;
                labels_to_terminate = 1;
                force_is_oam = 0;
                force_fwd_code = 1;
                force_fwd_code_value = ARAD_PP_FWD_CODE_TM; 
                force_has_cw = 1;
                break;
            case _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_IPV4_EXP:
                label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_IPV4_EXP;
                expected_ttl = 0;
                expected_bos = 0;
                check_bos = 0;
                check_ttl = 0;
                labels_to_terminate = 1;
                force_is_oam = 0;
                force_fwd_code = 0;
                force_fwd_code_value = 0;
                force_has_cw = 0;
                break;
            case _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_IPV6_EXP:
                label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_IPV6_EXP;
                expected_ttl = 0;
                expected_bos = 0;
                check_bos = 0;
                check_ttl = 0;
                labels_to_terminate = 1;
                force_is_oam = 0;
                force_fwd_code = SOC_DPP_CONFIG(unit)->pp.explicit_null_support ? 0 : 1;
                force_fwd_code_value = 0;
                force_has_cw = 0;
                break;
            case _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_RA: 
                label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_RA;
                expected_ttl = 0;
                expected_bos = 0;
                check_bos = 1;
                check_ttl = 0;
                labels_to_terminate = 1;
                force_is_oam = 1;
                force_fwd_code = SOC_DPP_CONFIG(unit)->pp.explicit_null_support ? 0 : 1;
                force_fwd_code_value = 2;
                force_has_cw = 0;
                break;
            case _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_OAM_ALERT: 
                label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_OAM_ALERT;
                expected_ttl = 0;
                expected_bos = 1;
                check_bos = 1;
                check_ttl = 0;
                labels_to_terminate = 1;
                force_is_oam = 0;
                force_fwd_code = 0;
                force_fwd_code_value = 0;
                force_has_cw = 0;
                break;
            }

            special_label_bitmap &= ~(1 << label);

            
            SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_VTT_MPLS_LABEL_TCAMm(unit, IHP_BLOCK(unit, SOC_BLOCK_ALL), label_index, reg_above_64));
            soc_IHP_VTT_MPLS_LABEL_TCAMm_field_set(unit, reg_above_64, KEYf, &label);
            field_val = 0;
            soc_IHP_VTT_MPLS_LABEL_TCAMm_field_set(unit, reg_above_64, KEY_MASKf, &field_val);
            soc_IHP_VTT_MPLS_LABEL_TCAMm_field_set(unit, reg_above_64, EXPECTED_TTLf, &expected_ttl);
            soc_IHP_VTT_MPLS_LABEL_TCAMm_field_set(unit, reg_above_64, EXPECTED_BOSf, &expected_bos);
            soc_IHP_VTT_MPLS_LABEL_TCAMm_field_set(unit, reg_above_64, CHECK_BOSf, &check_bos);
            soc_IHP_VTT_MPLS_LABEL_TCAMm_field_set(unit, reg_above_64, CHECK_TTLf, &check_ttl);
            field_val = 1;
            soc_IHP_VTT_MPLS_LABEL_TCAMm_field_set(unit, reg_above_64, VALIDf, &field_val);
            SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_VTT_MPLS_LABEL_TCAMm(unit, IHP_BLOCK(unit, SOC_BLOCK_ALL), label_index, reg_above_64));

            
            special_label_profile = (force_is_oam << _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_FORCE_IS_OAM_OFFSET) | 
                                    (force_fwd_code_value << _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_FORCE_FWD_CODE_VALUE_OFFSET) | 
                                    (force_fwd_code << _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_FORCE_FWD_CODE_OFFSET) | 
                                    (force_has_cw << _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_HAS_CW_OFFSET) | 
                                    (labels_to_terminate << _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_LABELS_TO_TERMINATE_OFFSET);
            SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_MPLS_SPECIAL_LABEL_PROFILE_TABLEr(unit, SOC_CORE_ALL, &reg_64));
            SOC_REG_ABOVE_64_CLEAR(reg_above_64);
            reg_above_64[0] = COMPILER_64_LO(reg_64);
            reg_above_64[1] = COMPILER_64_HI(reg_64);
            SHR_BITCOPY_RANGE(reg_above_64, _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_NOF_BITS*label_index, &special_label_profile, 0, _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_NOF_BITS);
            COMPILER_64_SET(reg_64, reg_above_64[1], reg_above_64[0]);
            SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_MPLS_SPECIAL_LABEL_PROFILE_TABLEr(unit, SOC_CORE_ALL, reg_64));

            
            SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_MPLS_SPECIAL_LABELS_TO_TERMINATEr(unit, SOC_CORE_ALL, &reg));
            SHR_BITCOPY_RANGE(&reg, label_index*_SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_TO_TERMINATE_WIDTH, &labels_to_terminate, 0, _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_TO_TERMINATE_WIDTH);
            soc_reg_field_set(unit, IHP_MPLS_SPECIAL_LABELS_TO_TERMINATEr, &reg, MPLS_SPECIAL_LABELS_TO_TERMINATEf, reg);
            SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_MPLS_SPECIAL_LABELS_TO_TERMINATEr(unit, SOC_CORE_ALL, reg));
        }
    }

    
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_VTT_MPLS_ENH_EXCLUDE_SPECIAL_LABEL_BIT_MAPr(unit, SOC_CORE_ALL, special_label_bitmap)); 

    
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    type = 2;
    if (SOC_IS_JERICHO_PLUS_A0(unit)) {
        soc_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm_field_set(unit, reg_above_64, TYPEf, &type);
        
        forwarding_code = 1; 
        soc_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm_field_set(unit, reg_above_64, FORWARDING_CODEf, &forwarding_code);
    }
    else {
        soc_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm_field_set(unit, reg_above_64, TYPEf, &type);
        if (SOC_IS_QUX(unit) || SOC_IS_QAX_B0(unit)) {
            
            forwarding_code = 1; 
            soc_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm_field_set(unit, reg_above_64, FORWARDING_CODEf, &forwarding_code);
        }
    }

    SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_VTT_MPLS_DUMMY_LIFr(unit, SOC_CORE_ALL, reg_above_64));

    
    reg = 0;
    soc_reg_field_set(unit, IHP_MPLS_LABEL_PROCEDURE_CONFIGr, &reg, MAX_NOF_LABELS_TO_TERMINATEf, 13);
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_MPLS_LABEL_PROCEDURE_CONFIGr(unit, SOC_CORE_ALL, reg));

    
    reg = 0;
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_VTT_MPLS_ENH_DUMMY_LIF_TERMINATION_PROFILEr(unit, SOC_CORE_ALL, reg)); 
    
    if (SOC_DPP_CONFIG(unit)->pp.mldp_support) {
        ARAD_PP_ISEM_ACCESS_KEY                  isem_key;
        ARAD_PP_ISEM_ACCESS_ENTRY                isem_entry;
        SOC_SAND_SUCCESS_FAILURE                 success;
        uint32                                   res = SOC_SAND_OK;
        SOC_PPC_LIF_ID local_mldp_dummy_lif_id[2];
        JER_MPLS_TERM.local_mldp_dummy_lif_id.get(unit, 0, &(local_mldp_dummy_lif_id[0]));
        JER_MPLS_TERM.local_mldp_dummy_lif_id.get(unit, 1, &(local_mldp_dummy_lif_id[1]));


        
        SOCDNX_CLEAR(&isem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
        SOCDNX_CLEAR(&isem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
        isem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS;
        isem_key.key_info.mpls.flags = SOC_PPC_MPLS_TERM_FLAG_DUMMY_LABEL;
        isem_key.key_info.mpls.is_bos = 0;
        isem_entry.sem_result_ndx = local_mldp_dummy_lif_id[1];
        res = arad_pp_isem_access_entry_add_unsafe(unit, &isem_key, &isem_entry, &success);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if (success != SOC_SAND_SUCCESS) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                                 (_BSL_SOCDNX_MSG("Error, isemb  addition fails dummy mpls lif1 on init, unexpected behavior.\n")));
        }

        SOCDNX_CLEAR(&isem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
        isem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS;
        isem_key.key_info.mpls.flags = SOC_PPC_MPLS_TERM_FLAG_DUMMY_LABEL;
        isem_key.key_info.mpls.is_bos = 1;
        isem_entry.sem_result_ndx = local_mldp_dummy_lif_id[0];
        res = arad_pp_isem_access_entry_add_unsafe(unit, &isem_key, &isem_entry, &success);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if (success != SOC_SAND_SUCCESS) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                                 (_BSL_SOCDNX_MSG("Error, isemb  addition fails dummy mpls lif2 on init, unexpected behavior.\n")));
        }

    }

exit:
    SOCDNX_FUNC_RETURN;
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


soc_error_t soc_jer_pp_mpls_termination_l4b_l5l_init(int unit) {
    ARAD_PP_ISEM_ACCESS_KEY                  isem_key;
    ARAD_PP_ISEM_ACCESS_ENTRY                isem_entry;
    SOC_SAND_SUCCESS_FAILURE                 success;
    uint32                                   res = SOC_SAND_OK;
    uint32                                   index = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_WARM_BOOT(unit)) {
        SOCDNX_FUNC_RETURN;
    }

    for (index=0;index < 3;index++) {
        
        SOCDNX_CLEAR(&isem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
        SOCDNX_CLEAR(&isem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
        isem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_L4B_L5G;
        if (index == 0) {
        	isem_key.key_info.mpls.is_bos = 0x1;
            isem_key.key_info.mpls.label = 0x0; 
        } else if (index == 1){
        	isem_key.key_info.mpls.is_bos = 0x0;
        	isem_key.key_info.mpls.label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_GAL;
        } else {
        	isem_key.key_info.mpls.is_bos = 0x0;
        	isem_key.key_info.mpls.label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_RA;
        }

        res = arad_pp_isem_access_entry_add_unsafe(unit, &isem_key, &isem_entry, &success);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if(success != SOC_SAND_SUCCESS)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                             (_BSL_SOCDNX_MSG("Error, TCAM additions fails Label 4 BOS and label 5 GAL on init, unexpected behavior.\n")));
        }

    }



exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_jer_pp_mpls_termination_range_action_set(int unit, uint32 entry_index, SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO * range_action_info,uint32 flags) {
    soc_reg_above_64_val_t  reg_above_64; 

    SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO
        mpls_label_range_info;
    uint32
        res = SOC_SAND_OK;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Error, Only Jericho device supports this function\n")));
    }

    SOCDNX_NULL_CHECK(range_action_info);

    if (!_SOC_JER_PP_MPLS_TERM_LABEL_IN_RANGE(range_action_info->label_high) || \
        !_SOC_JER_PP_MPLS_TERM_LABEL_IN_RANGE(range_action_info->label_low)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error, One of the given labels is out of (0,2^20-1)\n")));
    }

    SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_VTT_MPLS_LABEL_RANGE_TABLEm(unit, IHP_BLOCK(unit, SOC_BLOCK_ALL), entry_index, reg_above_64));
    soc_IHP_VTT_MPLS_LABEL_RANGE_TABLEm_field_set(unit, reg_above_64, RANGE_MINf, &(range_action_info->label_low));
    soc_IHP_VTT_MPLS_LABEL_RANGE_TABLEm_field_set(unit, reg_above_64, RANGE_MAXf, &(range_action_info->label_high));
    soc_IHP_VTT_MPLS_LABEL_RANGE_TABLEm_field_set(unit, reg_above_64, BOS_VALUEf, &(range_action_info->bos_value));
    soc_IHP_VTT_MPLS_LABEL_RANGE_TABLEm_field_set(unit, reg_above_64, BOS_VALUE_MASKf, &(range_action_info->bos_value_mask));
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_VTT_MPLS_LABEL_RANGE_TABLEm(unit, IHP_BLOCK(unit, SOC_BLOCK_ALL), entry_index, reg_above_64));

    if ((SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode >= 20 && SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode <= 23) && (flags & BCM_MPLS_RANGE_ACTION_EVPN_IML)) {
        SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_clear(&mpls_label_range_info);
        mpls_label_range_info.range.first_label = range_action_info->label_low;
        mpls_label_range_info.range.last_label  = range_action_info->label_high;
        res = arad_pp_mpls_term_label_range_set_unsafe(unit,_SOC_JER_PP_MPLS_TERM_RANGE_NDX_IML,&mpls_label_range_info);
        SOCDNX_SAND_IF_ERR_EXIT(res);

    }
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_jer_pp_mpls_termination_range_action_get(int unit, uint32 entry_index, SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO * range_action_info) {
    soc_reg_above_64_val_t  reg_above_64; 

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Error, Only Jericho device supports this function\n")));
    }

    SOCDNX_NULL_CHECK(range_action_info);

    SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_VTT_MPLS_LABEL_RANGE_TABLEm(unit, IHP_BLOCK(unit, SOC_BLOCK_ALL), entry_index, reg_above_64));
    soc_IHP_VTT_MPLS_LABEL_RANGE_TABLEm_field_get(unit, reg_above_64, RANGE_MINf, &(range_action_info->label_low));
    soc_IHP_VTT_MPLS_LABEL_RANGE_TABLEm_field_get(unit, reg_above_64, RANGE_MAXf, &(range_action_info->label_high));
    soc_IHP_VTT_MPLS_LABEL_RANGE_TABLEm_field_get(unit, reg_above_64, BOS_VALUEf, &(range_action_info->bos_value));
    soc_IHP_VTT_MPLS_LABEL_RANGE_TABLEm_field_get(unit, reg_above_64, BOS_VALUE_MASKf, &(range_action_info->bos_value_mask));
    

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t soc_jer_pp_mpls_termination_range_profile_set(int unit, uint32 entry_index, SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO * range_profile_info) {
    soc_reg_above_64_val_t  reg_above_64; 
    uint64 reg_64;
    uint32 range_profile = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Error, Only Jericho device supports this function\n")));
    }

    SOCDNX_NULL_CHECK(range_profile_info);

    SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_VTT_MPLS_LABEL_RANGE_PROFILEr(unit, SOC_CORE_ALL, &reg_64));

    range_profile = (range_profile_info->mpls_label_range_tag_mode << _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_TAG_MODE_SHIFT) |
                    (range_profile_info->mpls_label_range_has_cw << _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_HAS_CW_SHIFT) |
                    (range_profile_info->mpls_label_range_set_outer_vid << _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_OUTER_VID_SHIFT) |
                    (range_profile_info->mpls_label_range_set_inner_vid << _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_INNER_VID_SHIFT) |
                    (range_profile_info->mpls_label_range_use_base << _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_USE_BASE_SHIFT);    
    
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    reg_above_64[0] = COMPILER_64_LO(reg_64);
    reg_above_64[1] = COMPILER_64_HI(reg_64);
    SHR_BITCOPY_RANGE(reg_above_64, _SOC_JER_PP_MPLS_TERM_RANGE_PROFILE_NOF_FIELDS*entry_index, &range_profile, 0, _SOC_JER_PP_MPLS_TERM_RANGE_PROFILE_NOF_FIELDS);
    COMPILER_64_SET(reg_64, reg_above_64[1], reg_above_64[0]);
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_VTT_MPLS_LABEL_RANGE_PROFILEr(unit, SOC_CORE_ALL, reg_64));

 

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_jer_pp_mpls_termination_range_profile_get(int unit, uint32 entry_index, SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO *range_profile_info) {
    uint64 reg_64;
    soc_reg_above_64_val_t  reg_above_64; 
    uint32 range_profile = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Error, Only Jericho device supports this function\n")));
    }

    SOCDNX_NULL_CHECK(range_profile_info);

    SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_VTT_MPLS_LABEL_RANGE_PROFILEr(unit, SOC_CORE_ALL, &reg_64));    
    
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    reg_above_64[0] = COMPILER_64_LO(reg_64);
    reg_above_64[1] = COMPILER_64_HI(reg_64);
    SHR_BITCOPY_RANGE(&range_profile, 0,reg_above_64 , _SOC_JER_PP_MPLS_TERM_RANGE_PROFILE_NOF_FIELDS*entry_index, _SOC_JER_PP_MPLS_TERM_RANGE_PROFILE_NOF_FIELDS);
    
    range_profile_info->mpls_label_range_tag_mode = (range_profile & _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_TAG_MODE_BIT) \
                                                    >> _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_TAG_MODE_SHIFT; 
    range_profile_info->mpls_label_range_has_cw = (range_profile & _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_HAS_CW_BIT) \
                                                    >> _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_HAS_CW_SHIFT;
    range_profile_info->mpls_label_range_set_outer_vid = (range_profile & _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_OUTER_VID_BIT) \
                                                    >> _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_OUTER_VID_SHIFT;
    range_profile_info->mpls_label_range_set_inner_vid = (range_profile & _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_INNER_VID_BIT) \
                                                    >> _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_INNER_VID_SHIFT ;
    range_profile_info->mpls_label_range_use_base = (range_profile & _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_USE_BASE_BIT) \
                                                    >> _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_USE_BASE_SHIFT;

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_jer_pp_mpls_termination_vccv_type_ttl1_oam_classification_set(int unit, uint8 vccv_type_ttl1_oam_classification_enabled) {
    soc_reg_above_64_val_t  reg_above_64; 
    uint32 field_val;

    SOCDNX_INIT_FUNC_DEFS;


    SOCDNX_SAND_IF_ERR_EXIT(soc_reg_above_64_get(unit,IHP_VTT_GENERAL_CONFIGS_1r, REG_PORT_ANY, 0, reg_above_64));
    field_val = vccv_type_ttl1_oam_classification_enabled ? 0 : 1;
    soc_reg_above_64_field32_set(unit, IHP_VTT_GENERAL_CONFIGS_1r, reg_above_64, VTT_MPLS_DISABLE_TTL_EXPIRY_VCCVf, field_val);
    SOCDNX_SAND_IF_ERR_EXIT(soc_reg_above_64_set(unit,IHP_VTT_GENERAL_CONFIGS_1r, REG_PORT_ANY, 0, reg_above_64));


exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_jer_pp_mpls_termination_vccv_type_ttl1_oam_classification_get(int unit, uint8 * vccv_type_ttl1_oam_classification_enabled) {
    soc_reg_above_64_val_t  reg_above_64; 
    uint32 field_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(soc_reg_above_64_get(unit,IHP_VTT_GENERAL_CONFIGS_1r, REG_PORT_ANY, 0, reg_above_64));
    field_val = soc_reg_above_64_field32_get(unit, IHP_VTT_GENERAL_CONFIGS_1r, reg_above_64, VTT_MPLS_DISABLE_TTL_EXPIRY_VCCVf);
    *vccv_type_ttl1_oam_classification_enabled = field_val ? 0 : 1;
    SOCDNX_SAND_IF_ERR_EXIT(soc_reg_above_64_set(unit,IHP_VTT_GENERAL_CONFIGS_1r, REG_PORT_ANY, 0, reg_above_64));


exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_jer_pp_mpls_termination_first_label_bit_map_set(int unit, uint32 termination_bit_map) {
	uint32 reg_val32 = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(soc_reg32_get(unit, IHP_VTT_MPLS_TERMINATE_FIRST_LABEL_BIT_MAPr, REG_PORT_ANY, 0, &reg_val32));
    reg_val32 = reg_val32 | termination_bit_map;
    SOCDNX_SAND_IF_ERR_EXIT(soc_reg32_set(unit, IHP_VTT_MPLS_TERMINATE_FIRST_LABEL_BIT_MAPr, REG_PORT_ANY, 0, reg_val32));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_jer_pp_mpls_termination_first_label_bit_map_get(int unit, uint32 * termination_bit_map) {


    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(soc_reg32_get(unit, IHP_VTT_MPLS_TERMINATE_FIRST_LABEL_BIT_MAPr, REG_PORT_ANY, 0, termination_bit_map));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_jer_pp_mpls_termination_first_label_bit_map_init(int unit, uint32 termination_bit_map) {
	uint32 reg_val32 = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(soc_reg32_get(unit, IHP_VTT_MPLS_TERMINATE_FIRST_LABEL_BIT_MAPr, REG_PORT_ANY, 0, &reg_val32));
    reg_val32 = reg_val32 & termination_bit_map;
    SOCDNX_SAND_IF_ERR_EXIT(soc_reg32_set(unit, IHP_VTT_MPLS_TERMINATE_FIRST_LABEL_BIT_MAPr, REG_PORT_ANY, 0, reg_val32));

exit:
    SOCDNX_FUNC_RETURN;
}
