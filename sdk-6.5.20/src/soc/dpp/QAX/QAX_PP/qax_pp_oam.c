/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_pp_oam.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_OAM
#include <shared/bsl.h>




#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>
#include <soc/dpp/dpp_config_defs.h>

#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_oam.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_oam_mep_db.h>
#include <soc/dpp/PPC/ppc_api_oam.h>






#define _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_MASK \
            ((~(_QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_KEY(0xf,0xff)))&((1<<(4+1+16+8))-1))






#define _QAX_PP_OAM_MP_TYPE_MAP_IS_BFD(mp_type_entry) ((mp_type_entry)>>4)
#define _QAX_PP_OAM_MP_TYPE_MAP_NOT_BELLOW_MDL_MEP_BITMAP_OR(mp_type_entry) (((mp_type_entry)>>3) & 0x1)
#define _QAX_PP_OAM_MP_TYPE_MAP_MDL_MP_TYPE(mp_type_entry) (((mp_type_entry)>>1) &0x3)
#define _QAX_PP_OAM_MP_TYPE_MAP_NOT_ABOVE_MDL_MEP_BITMAP_OR(mp_type_entry) ((mp_type_entry) & 0x1)

#define _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_KEY(_mep_type, _trap_code) \
            (((_mep_type)<<(8+16+1)) + (_trap_code))

#define _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_KEY_GET_TRAP_CODE(entry_key) (entry_key & 0xff)








extern ARAD_PP_INTERNAL_OPCODE_INIT internal_opcode_init[SOC_PPC_OAM_OPCODE_MAP_COUNT];








STATIC soc_error_t soc_qax_pp_oam_mp_type_config_init(int unit);


STATIC soc_error_t soc_qax_pp_oam_oem1_key_select_init(int unit);


STATIC soc_error_t soc_qax_pp_oam_oamp_mep_type_v_trap_code_tcam_init(int unit);


STATIC soc_error_t soc_qax_pp_oam_egress_sub_type_from_opcode_init(int unit);




soc_error_t soc_qax_pp_oam_classifier_init(int unit) {

    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_qax_pp_oam_mp_type_config_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_qax_pp_oam_oem1_key_select_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_qax_pp_oam_egress_sub_type_from_opcode_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);
    
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_1711_enable", 0) == 1) {
      
      rv = arad_pp_oam_tcam_y1711_lm_entry_add_unsafe(unit);
      SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_qax_pp_oam_oamp_init(int unit)
{
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_qax_pp_oam_oamp_mep_type_v_trap_code_tcam_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = qax_pp_oam_bfd_flexible_verification_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  soc_qax_pp_oam_oamp_punt_event_hendling_profile_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32                                 profile_ndx,
    SOC_SAND_IN SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA    *punt_profile_data
  )
{

    uint64 reg, field64;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    COMPILER_64_ZERO(reg);

    SOC_SAND_IF_ERR_EXIT(
       soc_reg_get(unit, OAMP_PUNT_EVENT_HENDLINGr, REG_PORT_ANY, profile_ndx, &reg));
    COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
    soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_N_PUNT_RATEf, field64);
    COMPILER_64_SET(field64, 0, punt_profile_data->punt_enable);
    soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_N_PUNT_ENABLEf, field64);
    COMPILER_64_SET(field64, 0, punt_profile_data->rx_state_update_enable);
    soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_N_RX_STATE_UPDATE_ENf, field64);
    COMPILER_64_SET(field64, 0, punt_profile_data->scan_state_update_enable);
    soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_N_SCAN_STATE_UPDATE_ENf, field64);
    COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_enable);
    soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_N_MEP_RDI_UPDATE_LOC_ENf, field64);
    COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_clear_enable);
    soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_N_MEP_RDI_UPDATE_LOC_CLEAR_ENf, field64);
    COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_rx_enable);
    soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_N_MEP_RDI_UPDATE_RX_ENf, field64);
    COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
    SOC_SAND_IF_ERR_EXIT(
       soc_reg_set(unit, OAMP_PUNT_EVENT_HENDLINGr, REG_PORT_ANY, profile_ndx, reg));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in qax_pp_oam_oamp_punt_event_hendling_profile_set()", profile_ndx, 0);
}


uint32
  soc_qax_pp_oam_oamp_punt_event_hendling_profile_get(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA    *punt_profile_data
  ) {
    uint64 reg, field64;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    COMPILER_64_ZERO(reg);

    SOC_SAND_IF_ERR_EXIT(
       soc_reg_get(unit, OAMP_PUNT_EVENT_HENDLINGr, REG_PORT_ANY, profile_ndx, &reg));
    field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_N_PUNT_RATEf);
    punt_profile_data->punt_rate = COMPILER_64_LO(field64);
    field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_N_PUNT_ENABLEf);
    punt_profile_data->punt_enable = COMPILER_64_LO(field64);
    field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_N_RX_STATE_UPDATE_ENf);
    punt_profile_data->rx_state_update_enable = COMPILER_64_LO(field64);
    field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_N_SCAN_STATE_UPDATE_ENf);
    punt_profile_data->scan_state_update_enable = COMPILER_64_LO(field64);
    field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_N_MEP_RDI_UPDATE_LOC_ENf);
    punt_profile_data->mep_rdi_update_loc_enable = COMPILER_64_LO(field64);
    field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_N_MEP_RDI_UPDATE_LOC_CLEAR_ENf);
    punt_profile_data->mep_rdi_update_loc_clear_enable = COMPILER_64_LO(field64);
    field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_N_MEP_RDI_UPDATE_RX_ENf);
    punt_profile_data->mep_rdi_update_rx_enable = COMPILER_64_LO(field64);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_punt_event_hendling_profile_get()", profile_ndx, 0);
}


STATIC
soc_error_t soc_qax_pp_oam_mp_type_config_init(int unit)
{
    int rv;
    uint32 mp_type_index;
    soc_reg_above_64_val_t reg_above_64;
    uint32 mp_type;

    SOCDNX_INIT_FUNC_DEFS;

    rv = READ_IHP_OAM_MP_TYPE_MAPr(unit, SOC_CORE_ALL, reg_above_64);
    SOCDNX_IF_ERR_EXIT(rv);

    for (mp_type_index=0; mp_type_index<(1<<5) ; ++mp_type_index) {
        if (_QAX_PP_OAM_MP_TYPE_MAP_IS_BFD(mp_type_index)) {
            mp_type = SOC_PPC_OAM_MP_TYPE_QAX_BFD;
        } else if (_QAX_PP_OAM_MP_TYPE_MAP_MDL_MP_TYPE(mp_type_index)==_QAX_PP_OAM_MDL_MP_TYPE_NO_MP) {
            if (_QAX_PP_OAM_MP_TYPE_MAP_NOT_ABOVE_MDL_MEP_BITMAP_OR(mp_type_index) &&
                _QAX_PP_OAM_MP_TYPE_MAP_NOT_BELLOW_MDL_MEP_BITMAP_OR(mp_type_index)) {
                if (_QAX_PP_HLM_BY_MDL(unit)) {
                    mp_type = SOC_PPC_OAM_MP_TYPE_QAX_BETWEEN_MEPS;
                } else {
                    mp_type = SOC_PPC_OAM_MP_TYPE_QAX_BELOW_ALL;
                }
            } else if (_QAX_PP_OAM_MP_TYPE_MAP_NOT_ABOVE_MDL_MEP_BITMAP_OR(mp_type_index)) {
                mp_type = SOC_PPC_OAM_MP_TYPE_QAX_BELOW_ALL;
            } else {
                mp_type = SOC_PPC_OAM_MP_TYPE_QAX_ABOVE_ALL;
            }
        } else {
            mp_type = _QAX_PP_OAM_MP_TYPE_FROM_MDL_MP_TYPE(
               _QAX_PP_OAM_MP_TYPE_MAP_MDL_MP_TYPE(mp_type_index));
        }
        SHR_BITCOPY_RANGE(reg_above_64, (3 * mp_type_index), &mp_type, 0, 3);
    }

    rv = WRITE_IHP_OAM_MP_TYPE_MAPr(unit,SOC_CORE_ALL,reg_above_64);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}




STATIC
soc_error_t soc_qax_pp_oam_oem1_key_select_ingress_init(int unit)
{
    int rv;

    uint32 key, payload;
    soc_reg_above_64_val_t reg_val = {0};

    uint8
        key_sel_inner,
        key_sel_outer,
        mp_profile_sel,
        your_disc_inner,
        your_disc_outer;

    SOCDNX_INIT_FUNC_DEFS;


    for (key=0; key<_QAX_PP_OAM_HLM_ING_KEY_SELECT_REG_KEY_SIZE; key++) {
        key_sel_inner   = 0;
        key_sel_outer   = 0;
        mp_profile_sel  = 0;
        your_disc_inner = 0;
        your_disc_outer = 0;

        if (!_QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_OAM_LIF_OUTER_VALID(key)) {
            if (!_QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_OAM_LIF_INNER_VALID(key)) {
                key_sel_inner =   _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF;
                key_sel_outer =   _QAX_PP_OAM_OEM1_KEY_SEL_NULL;
                mp_profile_sel =  1;
                your_disc_inner = _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_YOUR_DISC(key);
            }
        } else {
            if (!_QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_OAM_LIF_INNER_VALID(key)) {
                key_sel_inner =   _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF;
                key_sel_outer =   (_QAX_PP_HLM_BY_MDL(unit) || !_QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_LIF_EQ_TO_OAM_LIF_OUTER(key)) ?
                                   _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_OUTER :
                                  _QAX_PP_OAM_OEM1_KEY_SEL_NULL;
                mp_profile_sel =  1;

                

                if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
                    if (!_QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_LIF_EQ_TO_OAM_LIF_OUTER(key)) {
                        key_sel_inner = _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_OUTER;
                        mp_profile_sel =  0;
                    }
                }

                your_disc_inner = _QAX_PP_HLM_BY_MDL(unit) ? 0 : _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_YOUR_DISC(key);
                your_disc_outer = _QAX_PP_HLM_BY_MDL(unit) ? 1 : 0;
            } else {
                if (!_QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_LIF_EQ_TO_OAM_LIF_OUTER(key)) {
                    if (!_QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_YOUR_DISC(key)) {
                        key_sel_inner =   _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_INNER;
                        key_sel_outer =   _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_OUTER;
                        mp_profile_sel =  _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_LIF_EQ_TO_OAM_LIF_INNER(key);
                    } else {
                        key_sel_inner =   _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF;
                        key_sel_outer =   _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_OUTER;
                        mp_profile_sel =  1;
                        your_disc_inner = 1;
                    }
                }
            }
        }

        payload = _QAX_PP_OAM_OEM1_KEY_SEL_PAYLOAD(key_sel_inner,  key_sel_outer,
                                                   mp_profile_sel,
                                                   your_disc_inner, your_disc_outer);
        SHR_BITCOPY_RANGE(reg_val, key*7, &payload, 0, 7);
    }

    rv = WRITE_IHP_OAM_INGRESS_KEY_SELECTr(unit, reg_val);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC
soc_error_t soc_qax_pp_oam_oem1_key_select_egress_init(int unit)
{
    int rv;

    uint32 key, payload;
    soc_reg_above_64_val_t reg_val = {0};

    uint8
        key_sel_inner,
        key_sel_outer,
        mp_profile_sel,
        your_disc_inner,
        your_disc_outer;

    SOCDNX_INIT_FUNC_DEFS;


    for (key=0; key<_QAX_PP_OAM_HLM_EG_KEY_SELECT_REG_KEY_SIZE; key++) {
        key_sel_inner   = 0;
        key_sel_outer   = 0;
        mp_profile_sel  = 1; 
        your_disc_inner = 0;
        your_disc_outer = 0;

        if (!_QAX_PP_OAM_OEM1_KEY_SEL_EGRESS_KEY_OAM_LIF_OUTER_VALID(key)) {
            if (!_QAX_PP_OAM_OEM1_KEY_SEL_EGRESS_KEY_OAM_LIF_INNER_VALID(key)) {
                key_sel_inner =   _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF;
                key_sel_outer =   _QAX_PP_HLM_BY_MDL(unit) ?
                                  _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF :
                                  _QAX_PP_OAM_OEM1_KEY_SEL_NULL;
                your_disc_outer = _QAX_PP_HLM_BY_MDL(unit) ? 1 : 0;
            } else {
                key_sel_inner =   _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_INNER;
                key_sel_outer =   _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_OUTER;
            }
        } else {
            if (_QAX_PP_OAM_OEM1_KEY_SEL_EGRESS_KEY_OAM_LIF_INNER_VALID(key)) {
                key_sel_inner =   _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_INNER;
                key_sel_outer =   _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_OUTER;
            }
        }

        payload = _QAX_PP_OAM_OEM1_KEY_SEL_PAYLOAD(key_sel_inner,  key_sel_outer,
                                                   mp_profile_sel,
                                                   your_disc_inner, your_disc_outer);
        SHR_BITCOPY_RANGE(reg_val, key*7, &payload, 0, 7);
    }

    rv = WRITE_IHP_OAM_EGRESS_KEY_SELECTr(unit, reg_val);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC
soc_error_t soc_qax_pp_oam_oem1_key_select_init(int unit)
{
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_qax_pp_oam_oem1_key_select_ingress_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_qax_pp_oam_oem1_key_select_egress_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
soc_error_t soc_qax_pp_oam_examine_vector(int unit,
                                          uint16 mp_type_vector,
                                          uint16 matching_mp_type_vector,
                                          uint8  current_level,
                                          uint8* higher_mep_exists,
                                          uint8* lower_mep_exists)
{
    int mdl;
    uint8 mp_type, matching_mp_type, local_higher_mep_exists = 0, local_lower_mep_exists = 0;

    for (mdl=0; mdl<SOC_PPC_OAM_NOF_ETH_MP_LEVELS; mdl++) {
        mp_type = JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(mp_type_vector, mdl);
        if (mp_type == _JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH || mp_type == _JER_PP_OAM_MDL_MP_TYPE_PASSIVE_MATCH) {
            if (mp_type == _JER_PP_OAM_MDL_MP_TYPE_PASSIVE_MATCH) {
                
                matching_mp_type = JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(matching_mp_type_vector, mdl);
                if (matching_mp_type != _JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH) {
                    
                    break;
                }
            }

            if (mdl > current_level) {
                local_higher_mep_exists++;
            } else if (mdl < current_level) {
                local_lower_mep_exists++;
            }
        }
    }

    *higher_mep_exists = local_higher_mep_exists;
    *lower_mep_exists = local_lower_mep_exists;

    return SOC_E_NONE;
}


soc_error_t soc_qax_pp_oam_classifier_oem1_entry_set_for_hlm(
        int unit,
        const SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY * oem1_key,
        const SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD * oem1_new_payload,
        const SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry,
        uint8 operation ) {
    uint32 soc_sand_rv;
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY local_oem1_key, local_oem1_matching_key;
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD local_oem1_new_payload,
                                              local_oem1_prev_payload_yd0,
                                              local_oem1_prev_payload_yd1,
                                              local_oem1_matching_payload;
    uint8 found_yd0 = 0, found_yd1 = 0, found_matching = 0, pre_count_set = 0, new_count_set = 0;
    uint8 higher_mep_exists, lower_mep_exists;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    local_oem1_key = *oem1_key;

    
    local_oem1_matching_key.ingress = 1 - oem1_key->ingress;
    local_oem1_matching_key.oam_lif = (oem1_key->oam_lif == classifier_mep_entry->lif) ?
                                                            classifier_mep_entry->passive_side_lif :
                                                            classifier_mep_entry->lif;
    local_oem1_matching_key.your_discr = oem1_key->your_discr;

    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&local_oem1_prev_payload_yd0);
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&local_oem1_prev_payload_yd1);
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&local_oem1_matching_payload);

    local_oem1_new_payload = *oem1_new_payload;

    
    local_oem1_key.your_discr = 0;
    soc_sand_rv = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &local_oem1_key, &local_oem1_prev_payload_yd0, &found_yd0);
    SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 80, exit);

    
    local_oem1_key.your_discr = 1;
    soc_sand_rv = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &local_oem1_key, &local_oem1_prev_payload_yd1, &found_yd1);
    SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 80, exit);

    
    soc_sand_rv = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &local_oem1_matching_key, &local_oem1_matching_payload, &found_matching);
    SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 80, exit);

    
    if (!(classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT)) {
        
        local_oem1_key.your_discr = 0;
        local_oem1_new_payload.counter_ndx = local_oem1_prev_payload_yd0.counter_ndx;
        soc_sand_rv = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &local_oem1_key, &local_oem1_new_payload);
        SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 44, exit);

        if (found_yd1) {
            
            local_oem1_key.your_discr = 1;
            local_oem1_new_payload.counter_ndx = local_oem1_prev_payload_yd1.counter_ndx;
            soc_sand_rv = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &local_oem1_key, &local_oem1_new_payload);
            SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 44, exit);
        }

        SOC_SAND_EXIT_NO_ERROR;
    }

    

    
    soc_sand_rv = soc_qax_pp_oam_examine_vector(unit, oem1_new_payload->mp_type_vector, local_oem1_matching_payload.mp_type_vector,
                                                classifier_mep_entry->md_level, &higher_mep_exists, &lower_mep_exists);
    SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 37, exit);

    
    if(SOC_IS_QUX(unit)){
        if  ((local_oem1_prev_payload_yd0.counter_ndx != _QUX_PP_OAM_DIRECTION_BIT_ON_ADDITION) && (local_oem1_prev_payload_yd1.counter_ndx != _QUX_PP_OAM_DIRECTION_BIT_ON_ADDITION)
             &&(local_oem1_prev_payload_yd0.counter_ndx &&local_oem1_prev_payload_yd1.counter_ndx)){
            pre_count_set = 1;
        }
        if(oem1_new_payload->counter_ndx != _QUX_PP_OAM_DIRECTION_BIT_ON_ADDITION && oem1_new_payload->counter_ndx){
            new_count_set = 1;
        }
    }
    else {
        if(local_oem1_prev_payload_yd0.counter_ndx && local_oem1_prev_payload_yd1.counter_ndx){
            pre_count_set = 1;
        }
        if(oem1_new_payload->counter_ndx){
            new_count_set = 1;
        }
    }
    
    if (operation == _QAX_PP_OAM_HLM_EP_OPERATION_ADD &&
        pre_count_set &&
        (local_oem1_prev_payload_yd0.counter_ndx != local_oem1_prev_payload_yd1.counter_ndx)) {
        
        if (new_count_set) {
            if (local_oem1_prev_payload_yd0.counter_ndx != oem1_new_payload->counter_ndx &&
                local_oem1_prev_payload_yd1.counter_ndx != oem1_new_payload->counter_ndx) {
                
                SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("HLM: Only 2 different counters are supported on one LIF.")));
            }
        } else {
            
            if (!(lower_mep_exists && higher_mep_exists)) {
                SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("HLM: Valid counters on LIF must belong to the edge MEPs (in terms of level).")));
            }
        }
    }

    if (operation == _QAX_PP_OAM_HLM_EP_OPERATION_UPDATE) {
        
        if (higher_mep_exists) {
            
            
            local_oem1_key.your_discr = 0;
            local_oem1_new_payload.counter_ndx = local_oem1_prev_payload_yd0.counter_ndx;
            soc_sand_rv = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &local_oem1_key, &local_oem1_new_payload);
            SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 44, exit);

            
            if (found_yd1) {
                local_oem1_key.your_discr = 1;
                local_oem1_new_payload.counter_ndx = (SOC_IS_QUX(unit) ? (oem1_new_payload->counter_ndx != _QUX_PP_OAM_DIRECTION_BIT_ON_ADDITION && oem1_new_payload->counter_ndx) : oem1_new_payload->counter_ndx) ?
                                                     oem1_new_payload->counter_ndx :
                                                     local_oem1_prev_payload_yd1.counter_ndx;
                soc_sand_rv = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &local_oem1_key, &local_oem1_new_payload);
                SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 44, exit);
            }
        } else {
            
            
            local_oem1_key.your_discr = 0;
            local_oem1_new_payload.counter_ndx = oem1_new_payload->counter_ndx;
            soc_sand_rv = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &local_oem1_key, &local_oem1_new_payload);
            SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 44, exit);

            if (found_yd1) {
                
                
                local_oem1_key.your_discr = 1;
                local_oem1_new_payload.counter_ndx = local_oem1_prev_payload_yd1.counter_ndx;
                soc_sand_rv = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &local_oem1_key, &local_oem1_new_payload);
                SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 44, exit);
            }
        }
    } else if (operation == _QAX_PP_OAM_HLM_EP_OPERATION_ADD) {
        
        if (higher_mep_exists) {
            
            local_oem1_key.your_discr = 0;
            
            local_oem1_new_payload.counter_ndx = local_oem1_prev_payload_yd0.counter_ndx;
            
            soc_sand_rv = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &local_oem1_key, &local_oem1_new_payload);
            SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 44, exit);


            if (found_yd1 || new_count_set) {
                
                local_oem1_key.your_discr = 1;
                local_oem1_new_payload.counter_ndx = (SOC_IS_QUX(unit) ? (oem1_new_payload->counter_ndx  != _QUX_PP_OAM_DIRECTION_BIT_ON_ADDITION && oem1_new_payload->counter_ndx) : oem1_new_payload->counter_ndx) ?
                                                     oem1_new_payload->counter_ndx :
                                                     local_oem1_prev_payload_yd1.counter_ndx;
                soc_sand_rv = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &local_oem1_key, &local_oem1_new_payload);
                SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 44, exit);
            }
        } else {
            
            pre_count_set = 0;
            if(SOC_IS_QUX(unit)){
                if((local_oem1_prev_payload_yd0.counter_ndx != _QUX_PP_OAM_DIRECTION_BIT_ON_ADDITION) 
                   &&local_oem1_prev_payload_yd0.counter_ndx ){
                   pre_count_set = 1;
                }
            }
            else {
                if(local_oem1_prev_payload_yd0.counter_ndx){
                    pre_count_set = 1;
                }
            }
            if (new_count_set && pre_count_set &&
                oem1_new_payload->counter_ndx != local_oem1_prev_payload_yd0.counter_ndx) {
                
                local_oem1_key.your_discr = 0;
                
                local_oem1_new_payload.counter_ndx = oem1_new_payload->counter_ndx;
                soc_sand_rv = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &local_oem1_key, &local_oem1_new_payload);
                SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 44, exit);

                
                local_oem1_key.your_discr = 1;
                local_oem1_new_payload.counter_ndx = local_oem1_prev_payload_yd0.counter_ndx;
                soc_sand_rv = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &local_oem1_key, &local_oem1_new_payload);
                SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 44, exit);
            } else {
                local_oem1_key.your_discr = 0;
                
                local_oem1_new_payload.counter_ndx = (SOC_IS_QUX(unit) ? (oem1_new_payload->counter_ndx  != _QUX_PP_OAM_DIRECTION_BIT_ON_ADDITION && oem1_new_payload->counter_ndx) : oem1_new_payload->counter_ndx) ?
                                                     oem1_new_payload->counter_ndx :
                                                     local_oem1_prev_payload_yd0.counter_ndx;
                soc_sand_rv = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &local_oem1_key, &local_oem1_new_payload);
                SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 44, exit);

                
                if (found_yd1) {
                    local_oem1_key.your_discr = 1;
                    local_oem1_new_payload.counter_ndx = local_oem1_prev_payload_yd1.counter_ndx;
                    soc_sand_rv = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &local_oem1_key, &local_oem1_new_payload);
                    SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 44, exit);
                }
            }
        }
    } else {
        
        
        if (!higher_mep_exists) {
            
            
            local_oem1_key.your_discr = 0;
            local_oem1_new_payload.counter_ndx = (SOC_IS_QUX(unit) ? (local_oem1_prev_payload_yd1.counter_ndx  != _QUX_PP_OAM_DIRECTION_BIT_ON_ADDITION && local_oem1_prev_payload_yd1.counter_ndx) : local_oem1_prev_payload_yd1.counter_ndx)?
                                                    local_oem1_prev_payload_yd1.counter_ndx :
                                                     local_oem1_prev_payload_yd0.counter_ndx;
            soc_sand_rv = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &local_oem1_key, &local_oem1_new_payload);
            SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 44, exit);

            
            if (found_yd1) {
                local_oem1_key.your_discr = 1;
                soc_sand_rv = arad_pp_oam_classifier_oem1_entry_delete_unsafe(unit, &local_oem1_key);
                SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 100, exit);
            }
        } else {
            
            local_oem1_key.your_discr = 0;
            local_oem1_new_payload.counter_ndx = local_oem1_prev_payload_yd0.counter_ndx;
            soc_sand_rv = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &local_oem1_key, &local_oem1_new_payload);
            SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 44, exit);

            if (found_yd1) {
                local_oem1_key.your_discr = 1;
                if (!lower_mep_exists) {
                    
                    soc_sand_rv = arad_pp_oam_classifier_oem1_entry_delete_unsafe(unit, &local_oem1_key);
                    SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 100, exit);
                } else {
                    
                    
                    local_oem1_new_payload.counter_ndx = local_oem1_prev_payload_yd1.counter_ndx;

                    soc_sand_rv = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &local_oem1_key, &local_oem1_new_payload);
                    SOC_SAND_CHECK_FUNC_RESULT(soc_sand_rv, 44, exit);
                }
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_classifier_oem1_entry_set_for_hlm()", 0, 0);
}


soc_error_t soc_qax_pp_oam_oamp_rx_trap_codes_delete(
                 int                                 unit,
                 SOC_PPC_OAM_MEP_TYPE                mep_type,
                 uint32                              trap_code
    ) {
    uint32 res;
    int i;
    uint32 key=0 ;
    uint32 read_key=0 ;
    uint32 valid=0 ;
    soc_reg_above_64_val_t entbuf;
    uint32 internal_trap_code;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(entbuf);

    res = arad_pp_oam_oamp_rx_trap_codes_delete_verify(unit, mep_type, trap_code);
    SOCDNX_IF_ERR_EXIT(res); 

    res = _arad_pp_oam_trap_code_to_internal(unit, trap_code, &internal_trap_code);
    SOCDNX_IF_ERR_EXIT(res); 

    
    if (SOC_IS_QUX(unit)) {
        res = WRITE_OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1);
        SOCDNX_IF_ERR_EXIT(res);
    }

    key = _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_KEY(mep_type, internal_trap_code);

    
    for (i=0; i<=soc_mem_index_max(unit, OAMP_CLS_TRAP_CODE_TCAMm) ; ++i) {
        res = READ_OAMP_CLS_TRAP_CODE_TCAMm(unit, MEM_BLOCK_ANY, i, entbuf);
        SOCDNX_IF_ERR_EXIT(res); 
        valid  =     soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_get(unit, entbuf, VALIDf);
        read_key =   soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_get(unit, entbuf, KEYf);
        if (valid && (read_key == key)) {
            
            SOC_REG_ABOVE_64_CLEAR(entbuf);
            res = WRITE_OAMP_CLS_TRAP_CODE_TCAMm(unit, MEM_BLOCK_ANY, i, entbuf);
            SOCDNX_IF_ERR_EXIT(res); 
            break;
        }
    }

    if (SOC_IS_QUX(unit)) {
        res = WRITE_OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 0);
        SOCDNX_IF_ERR_EXIT(res);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_qax_pp_oam_oamp_rx_trap_codes_set(
                 int                                 unit,
                 SOC_PPC_OAM_MEP_TYPE                mep_type,
                 uint32                              trap_code
    ) {
    uint32 res;
    int i;
    soc_reg_above_64_val_t entbuf; 
    uint32 valid = 0;
    uint32 key = 0;
    uint32 read_key = 0;
    uint32 internal_trap_code;
    uint8 found = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(entbuf);

    res = arad_pp_oam_oamp_rx_trap_codes_set_verify(unit, mep_type, trap_code);
    SOCDNX_IF_ERR_EXIT(res); 

    res = _arad_pp_oam_trap_code_to_internal(unit, trap_code, &internal_trap_code);
    SOCDNX_IF_ERR_EXIT(res); 

    key = _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_KEY(mep_type, internal_trap_code);

    found = 0;
    
    for (i=0; i<=soc_mem_index_max(unit, OAMP_CLS_TRAP_CODE_TCAMm) ; ++i) {
        res = READ_OAMP_CLS_TRAP_CODE_TCAMm(unit, MEM_BLOCK_ANY, i, entbuf);
        SOCDNX_IF_ERR_EXIT(res);

        valid = soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_get(unit, entbuf, VALIDf);
        read_key = soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_get(unit, entbuf, KEYf);

        if (valid && (read_key == key)) {
            found = 1;
            break;
        }
    }

    
    if (SOC_IS_QUX(unit)) {
        res = WRITE_OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1);
        SOCDNX_IF_ERR_EXIT(res);
    }

    if (!found) {
        
        for (i=0; i<=soc_mem_index_max(unit, OAMP_CLS_TRAP_CODE_TCAMm) ; ++i) {
            res = READ_OAMP_CLS_TRAP_CODE_TCAMm(unit, MEM_BLOCK_ANY, i, entbuf);
            SOCDNX_IF_ERR_EXIT(res);

            valid = soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_get(unit, entbuf, VALIDf);
            if (valid == 0) {
                
                soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, KEYf, key);
                soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, MASKf, _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_MASK);
                soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, DATf, 1);
                soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, VALIDf, 1);
                res = WRITE_OAMP_CLS_TRAP_CODE_TCAMm(unit, MEM_BLOCK_ANY, i, entbuf);
                SOCDNX_IF_ERR_EXIT(res);
                break;
            }

        }
    }

    if (SOC_IS_QUX(unit)) {
        res = WRITE_OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 0);
        SOCDNX_IF_ERR_EXIT(res);
    }


exit:
    SOCDNX_FUNC_RETURN;

}



STATIC
soc_error_t soc_qax_pp_oam_oamp_mep_type_v_trap_code_tcam_init(int unit) {
    uint32 res;


    uint8 mep_types[] = { SOC_PPC_OAM_MEP_TYPE_ETH_OAM,
        SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP,
        SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE,
        SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP,
        SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP,
        SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS,
        SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE };

    uint8 trap_codes[] = {0xe0, 0xe1, 0xe2, 0xe3, 0xe3, 0xe4, 0xe5};

    int i;
    soc_reg_above_64_val_t entbuf;

    SOCDNX_INIT_FUNC_DEFS;
    
    
    if (SOC_IS_QUX(unit)) {
        res = WRITE_OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1);
        SOCDNX_IF_ERR_EXIT(res);
    }

    SOC_REG_ABOVE_64_CLEAR(entbuf);
    soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, MASKf, _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_MASK);
    soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, VALIDf, 1);
    soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, DATf, 1);
	

    for (i = 0; i < sizeof(mep_types); ++i) {
        soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, KEYf, _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_KEY(mep_types[i], trap_codes[i]));
        res = WRITE_OAMP_CLS_TRAP_CODE_TCAMm(unit, MEM_BLOCK_ANY, i, entbuf);
        SOCDNX_IF_ERR_EXIT(res);
    }

    if (SOC_IS_QUX(unit)) {
        res = WRITE_OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 0);
        SOCDNX_IF_ERR_EXIT(res);
    }

exit:
  SOCDNX_FUNC_RETURN;

}


STATIC
soc_error_t soc_qax_pp_oam_egress_sub_type_from_opcode_init(int unit) {

    soc_error_t res = SOC_E_NONE;
    soc_reg_above_64_val_t reg_data;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_data);

    res = READ_IHP_FLP_GENERAL_CFGr(unit, SOC_CORE_DEFAULT, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

    soc_reg_above_64_field32_set(unit, IHP_FLP_GENERAL_CFGr, reg_data, EGRESS_NON_OAM_PACKET_SUB_TYPE_FROM_OPCODEf, 1);

    res = WRITE_IHP_FLP_GENERAL_CFGr(unit, SOC_CORE_ALL, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_qax_pp_oam_oamp_1711_pe_profile_update(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN SOC_PPC_OAM_MEP_TYPE  mep_type,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  uint32                   cnt_enable
  )
{
    uint32 res = SOC_SAND_OK;
    soc_mem_t mem;
    soc_reg_above_64_val_t entry_above_64, entry_above_64_get;
    uint32     reg_val,reg32_field_get;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    mem = (mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ? OAMP_MEP_DB_Y_1731_ON_MPLSTPm : OAMP_MEP_DB_Y_1731_ON_PWEm;

    SOC_REG_ABOVE_64_CLEAR(entry_above_64);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ANY, mep_index, &entry_above_64));

    if(mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS){
        arad_pp_oamp_pe_program_profile_get(unit,(cnt_enable ? ARAD_PP_OAMP_PE_PROGS_CONSTRUCT_1711_MPLS_TP : ARAD_PP_OAMP_PE_PROGS_CONSTRUCT_1711_MPLS_TP_DIS_COUNT),&reg_val);
    }
    else if(mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE){
        arad_pp_oamp_pe_program_profile_get(unit,(cnt_enable ? ARAD_PP_OAMP_PE_PROGS_CONSTRUCT_1711_PWE : ARAD_PP_OAMP_PE_PROGS_CONSTRUCT_1711_PWE_DIS_COUNT),&reg_val);
    }
    soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ANY, mep_index, &entry_above_64));
    do
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ANY, mep_index, &entry_above_64));

        
        SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ANY, mep_index, &entry_above_64_get));
        soc_mem_field_get(unit, mem, entry_above_64_get, MEP_PE_PROFILEf, &reg32_field_get);

    } while (reg_val != reg32_field_get);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_1711_pe_profile_update()", mep_index, 0);
}


soc_error_t
 soc_qax_pp_oam_oamp_sd_sf_profile_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB     *sd_sf_profile_data
  )
{
    uint32 res = SOC_SAND_OK;
    uint32  entry[2];
    uint32 regv;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    ARAD_PP_CLEAR(entry, uint32, 2);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, READ_OAMP_SD_SF_PROFILEm(unit, MEM_BLOCK_ANY, profile_ndx, &entry));
    regv = sd_sf_profile_data->wnd_lngth;
    soc_OAMP_SD_SF_PROFILEm_field_set(unit, &entry, WND_LNGTHf,&regv);
    regv = sd_sf_profile_data->sd_set_thresh;
    soc_OAMP_SD_SF_PROFILEm_field_set(unit, &entry, SD_SET_THRESHf,&regv);
    regv = sd_sf_profile_data->sf_set_thresh;
    soc_OAMP_SD_SF_PROFILEm_field_set(unit, &entry, SF_SET_THRESHf,&regv);
    regv = sd_sf_profile_data->sd_clr_thresh;
    soc_OAMP_SD_SF_PROFILEm_field_set(unit, &entry, SD_CLR_THRESHf,&regv);
    
    regv = sd_sf_profile_data->sf_clr_thresh;
    soc_OAMP_SD_SF_PROFILEm_field_set(unit, &entry, SF_CLR_THRESHf,&regv);
    regv = sd_sf_profile_data->alert_method;
    soc_OAMP_SD_SF_PROFILEm_field_set(unit, &entry, ALERT_METHODf,&regv);
    
    regv = sd_sf_profile_data->supress_alerts;
    soc_OAMP_SD_SF_PROFILEm_field_set(unit, &entry, SUPRESS_ALERTSf,&regv);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_OAMP_SD_SF_PROFILEm(unit, MEM_BLOCK_ANY, profile_ndx, &entry));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_profile_set()", profile_ndx, 0);
}

soc_error_t
 soc_qax_pp_oam_oamp_sd_sf_profile_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB     *sd_sf_profile_data
  )
{
    uint32 res = SOC_SAND_OK;
    uint32  entry[2];
    uint32 regv;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    ARAD_PP_CLEAR(entry, uint32, 2);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, READ_OAMP_SD_SF_PROFILEm(unit, MEM_BLOCK_ANY, profile_ndx, &entry));

    soc_OAMP_SD_SF_PROFILEm_field_get(unit, &entry, WND_LNGTHf,&regv);
    sd_sf_profile_data->wnd_lngth = regv ; 
    
    soc_OAMP_SD_SF_PROFILEm_field_get(unit, &entry, SD_SET_THRESHf,&regv);
    sd_sf_profile_data->sd_set_thresh = regv ; 
    
    soc_OAMP_SD_SF_PROFILEm_field_get(unit, &entry, SF_SET_THRESHf,&regv);
    sd_sf_profile_data->sf_set_thresh = regv ; 
    
    soc_OAMP_SD_SF_PROFILEm_field_get(unit, &entry, SD_CLR_THRESHf,&regv);
    sd_sf_profile_data->sd_clr_thresh = regv ; 
    
    soc_OAMP_SD_SF_PROFILEm_field_get(unit, &entry, SF_CLR_THRESHf,&regv);
    sd_sf_profile_data->sf_clr_thresh = regv ; 
    
    soc_OAMP_SD_SF_PROFILEm_field_get(unit, &entry, ALERT_METHODf,&regv);
    sd_sf_profile_data->alert_method = regv ; 
    
    soc_OAMP_SD_SF_PROFILEm_field_get(unit, &entry, SUPRESS_ALERTSf,&regv);
    sd_sf_profile_data->supress_alerts = regv ; 


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_profile_get()", profile_ndx, 0);
}

soc_error_t
 soc_qax_pp_oam_oamp_sd_sf_1711_config_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                 d_excess_thresh,
    SOC_SAND_IN  uint8                                 clr_low_thresh,
    SOC_SAND_IN  uint8                                 clr_high_thresh,
    SOC_SAND_IN  uint8                                 num_entry
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 reg64_val;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,READ_OAMP_SD_SF_Y_1711_CONFr(unit, &reg64_val));
    soc_reg_field_set(unit, OAMP_SD_SF_Y_1711_CONFr, &reg64_val, D_EXCESS_THRESHf, (uint32)d_excess_thresh);
    soc_reg_field_set(unit, OAMP_SD_SF_Y_1711_CONFr, &reg64_val, IND_CLR_LOW_THRESHf, (uint32)clr_low_thresh);
    soc_reg_field_set(unit, OAMP_SD_SF_Y_1711_CONFr, &reg64_val, IND_CLR_HIGH_THRESHf, (uint32)clr_high_thresh);
    soc_reg_field_set(unit, OAMP_SD_SF_Y_1711_CONFr, &reg64_val, NUM_OF_1711_ENTRIESf, (uint32)num_entry);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,WRITE_OAMP_SD_SF_Y_1711_CONFr(unit, reg64_val));
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_1711_config_set()", 0, 0);
}

  soc_error_t
 soc_qax_pp_oam_oamp_sd_sf_1711_config_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT  uint8                                 *d_excess_thresh,
    SOC_SAND_INOUT  uint8                                 *clr_low_thresh,
    SOC_SAND_INOUT  uint8                                 *clr_high_thresh,
    SOC_SAND_INOUT  uint8                                 *num_entry
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 reg_val;
    uint32 field;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_SD_SF_Y_1711_CONFr(unit, &reg_val));
    
    field = soc_reg_field_get(unit, OAMP_SD_SF_Y_1711_CONFr, reg_val, D_EXCESS_THRESHf);
    *d_excess_thresh = field;
    field = soc_reg_field_get(unit, OAMP_SD_SF_Y_1711_CONFr, reg_val, IND_CLR_LOW_THRESHf);
    *clr_low_thresh = field;
    field = soc_reg_field_get(unit, OAMP_SD_SF_Y_1711_CONFr, reg_val, IND_CLR_HIGH_THRESHf);
    *clr_high_thresh = field;
    field = soc_reg_field_get(unit, OAMP_SD_SF_Y_1711_CONFr, reg_val, NUM_OF_1711_ENTRIESf);
    *num_entry = field;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_1711_config_get()", 0, 0);
}

soc_error_t
 soc_qax_pp_oam_oamp_sd_sf_scanner_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                            scan_index
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 num_clocks_per_mep_scan;
    soc_reg_above_64_val_t reg_above_64_val;
    soc_field_t scan_fields[] = {SD_SF_NUM_SCANS_1f,SD_SF_NUM_SCANS_2f,SD_SF_NUM_SCANS_3f,SD_SF_NUM_SCANS_4f,SD_SF_NUM_SCANS_5f,SD_SF_NUM_SCANS_6f,SD_SF_NUM_SCANS_7f};
    uint32 scan_value[] = {1,3,30,300,3000,18000,180000}; 
    uint32  reg_val;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    
    
    if((scan_index >7) || (scan_index<1)){
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(" scan index error \n")));
    }
    
     
    num_clocks_per_mep_scan = ( 333*arad_chip_kilo_ticks_per_sec_get(unit))/100;
        
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,READ_OAMP_SD_SF_SCANNER_CFGr(unit, reg_above_64_val));

    
    soc_reg_above_64_field32_set(unit, OAMP_SD_SF_SCANNER_CFGr, reg_above_64_val, SD_SF_SCAN_ENf, 1);
    reg_val = scan_value[scan_index-1];
    soc_reg_above_64_field32_set(unit, OAMP_SD_SF_SCANNER_CFGr, reg_above_64_val, scan_fields[scan_index-1], reg_val);
    
    
    soc_reg_above_64_field32_set(unit, OAMP_SD_SF_SCANNER_CFGr, reg_above_64_val,  NUM_CLOCKS_SD_SF_DB_SCANf,num_clocks_per_mep_scan);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,WRITE_OAMP_SD_SF_SCANNER_CFGr(unit, reg_above_64_val));
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_scanner_set()", 0, 0);
}


soc_error_t
  soc_qax_pp_oam_oamp_sd_sf_1711_db_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  uint32                   y1711_sd_sf_id,
    SOC_SAND_IN  uint32                   sd_sf_1711_db_format,
    SOC_SAND_IN  uint8                   ccm_tx_rate,
    SOC_SAND_IN  uint8                   alert_method
  )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t entry;
    uint32  reg_val;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_REG_ABOVE_64_CLEAR(entry);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, READ_OAMP_SD_SF_DB_Y_1711m(unit, MEM_BLOCK_ANY, y1711_sd_sf_id, &entry));
    
    reg_val = rmep_index; 
    soc_OAMP_SD_SF_DB_Y_1711m_field_set(unit, entry, RMEP_DB_PTRf, &reg_val);
    
    reg_val = ccm_tx_rate; 
    soc_OAMP_SD_SF_DB_Y_1711m_field_set(unit, entry, CCM_TX_RATEf, &reg_val);
    
    reg_val = alert_method; 
    soc_OAMP_SD_SF_DB_Y_1711m_field_set(unit, entry, ALLERT_METHODf, &reg_val);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_OAMP_SD_SF_DB_Y_1711m(unit, MEM_BLOCK_ANY, y1711_sd_sf_id, entry));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_1711_db_set()", 0, 0);

}

soc_error_t
  soc_qax_pp_oam_oamp_sd_sf_1711_db_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   y1711_sd_sf_id,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_SD_SF_Y_1711_DB_ENTRY  *sd_sf_1711_entry
  )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t entry;
    uint32  reg_val;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_REG_ABOVE_64_CLEAR(entry);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, READ_OAMP_SD_SF_DB_Y_1711m(unit, MEM_BLOCK_ANY, y1711_sd_sf_id, &entry));
    
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, D_EXCESSf, &reg_val);
    sd_sf_1711_entry->d_excess = reg_val;
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, D_MISMERGEf, &reg_val);
    sd_sf_1711_entry->d_mismatch = reg_val;
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, D_MISMATCHf, &reg_val);
    sd_sf_1711_entry->d_mismerge = reg_val;
    
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, PRD_PKT_CNTR_0f, &reg_val);
    sd_sf_1711_entry->prd_pkt_cnt_0 = reg_val;
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, PRD_PKT_CNTR_1f, &reg_val);
    sd_sf_1711_entry->prd_pkt_cnt_1 = reg_val;
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, PRD_PKT_CNTR_2f, &reg_val);
    sd_sf_1711_entry->prd_pkt_cnt_2 = reg_val;
    
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, PRD_ERR_IND_0f, &reg_val);
    sd_sf_1711_entry->prd_err_ind_0 = reg_val;
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, PRD_ERR_IND_1f, &reg_val);
    sd_sf_1711_entry->prd_err_ind_1 = reg_val;
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, PRD_ERR_IND_2f, &reg_val);
    sd_sf_1711_entry->prd_err_ind_2 = reg_val;

    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, CCM_TX_RATEf, &reg_val);
    sd_sf_1711_entry->ccm_tx_rate= reg_val;
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, ALLERT_METHODf, &reg_val);
    sd_sf_1711_entry->allert_method= reg_val;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_1711_db_get()", 0, 0);

}



soc_error_t
  soc_qax_pp_oam_oamp_sd_sf_db_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  uint32                   sd_sf_db_index,
    SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY       *sd_sf_entry
  )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t entry;
    uint8 dbflag=0;
    uint32 num_sdsfdb1 = SOC_DPP_DEFS_GET(unit,oamp_number_of_sd_sf_db_1);
    uint32  reg_val = 0;
    uint32 sd_sf_entry_index = 0;
    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_above_64_val_t reg_val_db_2;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_REG_ABOVE_64_CLEAR(entry);
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SOC_REG_ABOVE_64_CLEAR(reg_val_db_2);

   if((sd_sf_entry->ccm_tx_rate> 0x7)||(sd_sf_entry->thresh_profile> 15)){
       SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(" ccm rate or thresh_profile error \n")));
   }

    
    if(sd_sf_db_index < num_sdsfdb1){
      dbflag=1;
      sd_sf_entry_index =sd_sf_db_index;
    }
    else{
      dbflag=0;
      sd_sf_entry_index = sd_sf_db_index - num_sdsfdb1;
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, dbflag? soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, OAMP_SD_SF_DB_1m, 0, MEM_BLOCK_ANY, sd_sf_entry_index, &entry)
                                                      : READ_OAMP_SD_SF_DB_2m(unit, MEM_BLOCK_ANY, sd_sf_entry_index, &entry));


    if(dbflag == 1){
        reg_val = sd_sf_entry->entry_format;
        soc_OAMP_SD_SF_DB_1m_field_set(unit, &entry, ENTRY_FORMATf, &reg_val);
        reg_val = rmep_index;
        soc_OAMP_SD_SF_DB_1m_field_set(unit, &entry, RMEP_DB_PTRf, &reg_val) ;
        reg_val = sd_sf_entry->ccm_tx_rate;
        soc_OAMP_SD_SF_DB_1m_field_set(unit, &entry, CCM_TX_RATEf, &reg_val) ;
        reg_val = sd_sf_entry->thresh_profile;
        soc_OAMP_SD_SF_DB_1m_field_set(unit, &entry, THRESH_PROFILEf, &reg_val) ;
        reg_val = sd_sf_entry->sd;
        soc_OAMP_SD_SF_DB_1m_field_set(unit, &entry, SD_INDICATIONf, &reg_val) ;
        reg_val = sd_sf_entry->sf;
        soc_OAMP_SD_SF_DB_1m_field_set(unit, &entry, SF_INDICATIONf, &reg_val) ;
        
        reg_val = 0;
        soc_OAMP_SD_SF_DB_1m_field_set(unit, &entry, SUM_OF_CNTRSf, &reg_val);
        soc_OAMP_SD_SF_DB_1m_field_set(unit, &entry, SLIDING_WND_CNTRf, &reg_val);
    }
    else{
        reg_val = sd_sf_entry->entry_format;
        SHR_BITCOPY_RANGE(entry, 0, &reg_val, 0, 2);
         
        
        reg_val = 0; 
        SHR_BITCOPY_RANGE(reg_val_db_2, 514, &reg_val, 0, 10);
        reg_val = sd_sf_entry->thresh_profile;
        SHR_BITCOPY_RANGE(reg_val_db_2, 524, &reg_val, 0, 4);
        reg_val = sd_sf_entry->sf;
        SHR_BITCOPY_RANGE(reg_val_db_2, 528, &reg_val, 0, 1);
        reg_val = sd_sf_entry->sd;
        SHR_BITCOPY_RANGE(reg_val_db_2, 529, &reg_val, 0, 1);
        reg_val = rmep_index;
        SHR_BITCOPY_RANGE(reg_val_db_2, 530, &reg_val, 0, 15);
        reg_val = sd_sf_entry->ccm_tx_rate;
        SHR_BITCOPY_RANGE(reg_val_db_2, 545, &reg_val, 0, 3);
        soc_OAMP_SD_SF_DB_2m_field_set(unit, &entry, SD_SF_DATAf, reg_val_db_2);
    }

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,READ_OAMP_SD_SF_SCANNER_CFGr(unit, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, OAMP_SD_SF_SCANNER_CFGr, reg_above_64_val, SD_SF_SCAN_ENf, 0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,WRITE_OAMP_SD_SF_SCANNER_CFGr(unit, reg_above_64_val));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, dbflag ? WRITE_OAMP_SD_SF_DB_1m(unit, MEM_BLOCK_ANY, sd_sf_entry_index, entry) :  WRITE_OAMP_SD_SF_DB_2m(unit, MEM_BLOCK_ANY, sd_sf_entry_index, entry));

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,READ_OAMP_SD_SF_SCANNER_CFGr(unit, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, OAMP_SD_SF_SCANNER_CFGr, reg_above_64_val, SD_SF_SCAN_ENf, 1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,WRITE_OAMP_SD_SF_SCANNER_CFGr(unit, reg_above_64_val));


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_db_set()", 0, 0);
}


soc_error_t
  soc_qax_pp_oam_oamp_sd_sf_db_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                sd_sf_db_index,    
    SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY    *sd_sf_entry
  )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t entry;
    soc_reg_above_64_val_t reg_val_db_2;
    soc_reg_above_64_val_t  slinding_count;
    uint32 num_sdsfdb1 = 4096;
    uint8 dbflag=0;
    uint32  reg_val;
    uint32 sd_sf_entry_index = 0;
    int index=0; 
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_REG_ABOVE_64_CLEAR(entry);
    SOC_REG_ABOVE_64_CLEAR(slinding_count);
    SOC_REG_ABOVE_64_CLEAR(reg_val_db_2);

    if(sd_sf_db_index < num_sdsfdb1){
      dbflag=1;
      sd_sf_entry_index =sd_sf_db_index;
    }
    else{
      dbflag=0;
      sd_sf_entry_index = sd_sf_db_index - 4096;
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, dbflag? soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, OAMP_SD_SF_DB_1m, 0, MEM_BLOCK_ANY, sd_sf_entry_index, &entry)
                                                      : READ_OAMP_SD_SF_DB_2m(unit, MEM_BLOCK_ANY, sd_sf_entry_index, &entry));

    if(dbflag){
         soc_OAMP_SD_SF_DB_1m_field_get(unit, &entry, SD_INDICATIONf, &reg_val);
         sd_sf_entry->sd=reg_val;
         soc_OAMP_SD_SF_DB_1m_field_get(unit, &entry, SF_INDICATIONf, &reg_val);
         sd_sf_entry->sf=reg_val;
         soc_OAMP_SD_SF_DB_1m_field_get(unit, &entry, SUM_OF_CNTRSf, &reg_val);
         sd_sf_entry->sum_cnt=reg_val;
         soc_OAMP_SD_SF_DB_1m_field_get(unit, &entry, SLIDING_WND_CNTRf, slinding_count);
         for( index=0; index<256; index++){
             SHR_BITCOPY_RANGE(&(sd_sf_entry->sliding_wnd_cntr[index]),0,slinding_count,index*2,2);
         }
         soc_OAMP_SD_SF_DB_1m_field_get(unit, &entry, CCM_TX_RATEf,&reg_val) ;
         sd_sf_entry->ccm_tx_rate=reg_val;
         soc_OAMP_SD_SF_DB_1m_field_get(unit, &entry, THRESH_PROFILEf, &reg_val) ;
         sd_sf_entry->thresh_profile=reg_val;
         soc_OAMP_SD_SF_DB_1m_field_get(unit, &entry, ENTRY_FORMATf, &reg_val);
         sd_sf_entry->entry_format = reg_val;
    }
    else{
        soc_OAMP_SD_SF_DB_2m_field_get(unit, &entry, SD_SF_DATAf, reg_val_db_2);
        SHR_BITCOPY_RANGE(&reg_val, 0, reg_val_db_2, 0, 2);
        sd_sf_entry->entry_format = reg_val;
        SHR_BITCOPY_RANGE(slinding_count, 0, reg_val_db_2, 2, 512);
        for( index=0; index<256; index++){
            SHR_BITCOPY_RANGE(&(sd_sf_entry->sliding_wnd_cntr[index]),0,slinding_count,index*2,2);
        }
        SHR_BITCOPY_RANGE(&reg_val, 0, reg_val_db_2, 514, 10);
        sd_sf_entry->sum_cnt=reg_val;
        SHR_BITCOPY_RANGE(&reg_val, 0, reg_val_db_2, 524, 4);
        sd_sf_entry->thresh_profile=reg_val;
        SHR_BITCOPY_RANGE(&reg_val, 0, reg_val_db_2, 528, 1);
        sd_sf_entry->sd=reg_val;
        SHR_BITCOPY_RANGE(&reg_val, 0, reg_val_db_2, 529, 1);
        sd_sf_entry->sf=reg_val;
        SHR_BITCOPY_RANGE(&reg_val, 0, reg_val_db_2, 545, 3);
        sd_sf_entry->ccm_tx_rate=reg_val;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_db_get()", 0, 0);
}


soc_error_t
  soc_qax_pp_oam_oamp_rmep_db_ext_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_RMEP_DB_EXT_ENTRY  *rmep_db_ext_entry
  )
{
    uint32  res;
    uint32  rmep_db_ext_index;
    uint32  entry_index;
    uint64  entry_64 ,val64;
    uint32  val32 = 0;
    
    soc_field_t db_entrys[] = {ENTRY_0f,ENTRY_1f,ENTRY_2f,ENTRY_3f,ENTRY_4f,ENTRY_5f,ENTRY_6f,ENTRY_7f,ENTRY_8f,ENTRY_9f,
        ENTRY_10f,ENTRY_11f,ENTRY_12f,ENTRY_13f,ENTRY_14f,ENTRY_15f};
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    rmep_db_ext_index = (rmep_index/8) + 1;
    entry_index = rmep_index%8;
    
      
    COMPILER_64_ZERO(entry_64);
    COMPILER_64_ZERO(val64);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_OAMP_RMEP_DB_EXTm(unit, MEM_BLOCK_ANY, rmep_db_ext_index, &entry_64));

    soc_mem_field64_get(unit, OAMP_RMEP_DB_EXTm, &entry_64, db_entrys[entry_index], &val64);
    val32 = COMPILER_64_LO(val64);
    rmep_db_ext_entry->last_prd_pkt_cnt_1731 = val32&0x3; 
    rmep_db_ext_entry->last_prd_pkt_cnt_1711 = (val32&0x1c)>>2; 
    rmep_db_ext_entry->rx_err = (val32&0x20)>>5; 
    rmep_db_ext_entry->loc= (val32&0x40)>>6; 

exit:
      SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_rmep_db_ext_get()", 0, 0);
}


soc_error_t soc_qax_pp_bfd_oamp_udp_src_port_check_enable(int unit, uint8 enable) 
{
    soc_reg_above_64_val_t profile_entry;
    uint8 profile_indx = _QAX_PP_BFD_UDP_SRC_PORT_CHECK_DIS_OAMP_MEP_PROFILE;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_REG_ABOVE_64_CLEAR(profile_entry);

    
    SOCDNX_IF_ERR_EXIT(READ_OAMP_MEP_PROFILEm(unit, MEM_BLOCK_ANY, profile_indx, profile_entry)); 
    
    soc_OAMP_MEP_PROFILEm_field32_set(unit, profile_entry, UDP_SRC_PORT_CHECK_DISf, !enable);
    
    SOCDNX_IF_ERR_EXIT(WRITE_OAMP_MEP_PROFILEm(unit, MEM_BLOCK_ANY, profile_indx, profile_entry)); 
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_qax_pp_oam_slm_set(int unit, int is_slm) {

    
    soc_error_t res = SOC_E_NONE;
    soc_reg_above_64_val_t reg_data;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_data);

    res = READ_IHP_FLP_GENERAL_CFGr(unit, SOC_CORE_DEFAULT, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

    if (is_slm) {
        
        soc_reg_above_64_field32_set(unit, IHP_FLP_GENERAL_CFGr, reg_data, EGRESS_NON_OAM_PACKET_SUB_TYPE_FROM_OPCODEf, 0);
        soc_reg_above_64_field32_set(unit, IHP_FLP_GENERAL_CFGr, reg_data, EGRESS_NON_OAM_PACKET_SUB_TYPEf, _ARAD_PP_OAM_SUBTYPE_LM);
    }
    else {
        soc_reg_above_64_field32_set(unit, IHP_FLP_GENERAL_CFGr, reg_data, EGRESS_NON_OAM_PACKET_SUB_TYPE_FROM_OPCODEf, 1);
        soc_reg_above_64_field32_set(unit, IHP_FLP_GENERAL_CFGr, reg_data, EGRESS_NON_OAM_PACKET_SUB_TYPEf, 0);
    }

    res = WRITE_IHP_FLP_GENERAL_CFGr(unit, SOC_CORE_ALL, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_qax_pp_oam_classifier_oam1_2_entries_insert_egress_inject( SOC_SAND_IN    int unit,
                                                                           SOC_SAND_IN    SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data,
                                                                           SOC_SAND_INOUT _oam_oam_a_b_table_buffer_t  *oama_buffer,
                                                                           SOC_SAND_INOUT _oam_oam_a_b_table_buffer_t  *oamb_buffer)
{
    uint32 res;
    int mep_prof;
    uint32 is_my_cfm_mac;
    uint8 internal_opcode;
    int cur_mp_type;
    uint32 possible_mp_types_qax[] = { SOC_PPC_OAM_MP_TYPE_QAX_ACTIVE_MATCH, SOC_PPC_OAM_MP_TYPE_QAX_PASSIVE_MATCH };

    SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY    oam1_key;
    SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY    oam2_key;
    SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD oam_payload;


    SOCDNX_INIT_FUNC_DEFS;

    SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(&oam1_key);
    SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY_clear(&oam2_key);
    SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(&oam_payload);

    oam1_key.inject = 1;
    oam1_key.is_bfd = 0;
    oam1_key.ingress = FALSE; 

    oam_payload.forward_disable = 0;
    oam_payload.mirror_profile = 0;
    oam_payload.mirror_enable = 0;
    oam_payload.forwarding_strength = 7;
    oam_payload.mirror_strength=0;

    for (is_my_cfm_mac = 0; is_my_cfm_mac <=1; is_my_cfm_mac++) {
        oam1_key.my_cfm_mac = is_my_cfm_mac;
        for (internal_opcode = 1; internal_opcode < SOC_PPC_OAM_OPCODE_MAP_COUNT; internal_opcode++) {
            oam1_key.opcode = internal_opcode;
            oam_payload.sub_type = internal_opcode_init[internal_opcode].sub_type;

            
            if (oam_payload.sub_type == _ARAD_PP_OAM_SUBTYPE_CCM) {
                if (SOC_IS_QUX(unit)) {
                    if (SOC_DPP_CONFIG(unit)->pp.qux_ccm_counting_enable == TRUE) {
                        oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_DEFAULT;
                    }
                } else {
                    oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_DEFAULT;
                }
            }

            
            if ((oam_payload.sub_type == _ARAD_PP_OAM_SUBTYPE_LM ||
                 oam_payload.sub_type == _ARAD_PP_OAM_SUBTYPE_SLM) &&
                !(profile_data->flags & SOC_PPC_OAM_LIF_PROFILE_FLAG_COUNTED) &&
                (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_lm_enable", 1) == 1)) {
                oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_DEFAULT_OAM_MESSAGE;
            }
            res =_arad_pp_oam_set_counter_disable(unit,internal_opcode,0, (&oam_payload), &profile_data->mep_profile_data, profile_data->is_piggybacked);
            SOCDNX_SAND_IF_ERR_EXIT(res);

            
            for (cur_mp_type = 0; cur_mp_type < sizeof(possible_mp_types_qax) / sizeof(uint32); ++cur_mp_type) {
                oam1_key.mp_type_jr = possible_mp_types_qax[cur_mp_type];
                oam1_key.mp_type = possible_mp_types_qax[cur_mp_type];
                
                for (mep_prof = 0; mep_prof < SOC_PPC_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM; ++mep_prof) {
                    oam1_key.mp_profile = mep_prof;
                    res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, oama_buffer);
                    SOCDNX_SAND_IF_ERR_EXIT(res);
                }
            }
            
            for (mep_prof = 0; mep_prof < SOC_PPC_OAM_ACC_PROFILES_NUM; ++mep_prof) {
                oam2_key.mp_profile = mep_prof;
                oam2_key.ingress = 0;
                oam2_key.inject = 1;
                oam2_key.is_bfd = 0;
                oam2_key.my_cfm_mac = oam1_key.my_cfm_mac;
                oam2_key.opcode = oam1_key.opcode;
                res = arad_pp_oam_classifier_oam2_entry_set_on_buffer(unit, &oam2_key, &oam_payload, oamb_buffer);
                SOCDNX_SAND_IF_ERR_EXIT(res);
            }
        }
    }
exit:
    SOCDNX_FUNC_RETURN;

}



soc_error_t soc_qax_diag_oamp_counter_set(int unit, uint8 type, uint8 filter, uint16 value)
{
    uint64 reg;
    int rv = 0;
    soc_reg_above_64_val_t rx_pkt_reg = {0};

    SOCDNX_INIT_FUNC_DEFS;

    switch(type) {
        case RX:
            switch (filter) {
                case ALL:
                    
                    break;

                case MEP:
                    soc_reg_above_64_field32_set(unit, OAMP_RX_PKT_COUNTERr,
                                                 rx_pkt_reg, RX_PKT_CNTR_MODEf, 1);

                    soc_reg_above_64_field32_set(unit, OAMP_RX_PKT_COUNTERr,
                                                 rx_pkt_reg, RX_PKT_CNTR_MEP_ID_FLTRf, value);
                    break;

                case RMEP:
                    soc_reg_above_64_field32_set(unit, OAMP_RX_PKT_COUNTERr,
                                                 rx_pkt_reg, RX_PKT_CNTR_MODEf, 2);

                    soc_reg_above_64_field32_set(unit, OAMP_RX_PKT_COUNTERr,
                                                 rx_pkt_reg, RX_PKT_CNTR_RMEP_ID_FLTRf, value);
                    break;

                case OPCODE:
                    soc_reg_above_64_field32_set(unit, OAMP_RX_PKT_COUNTERr,
                                                 rx_pkt_reg, RX_PKT_CNTR_MODEf, 3);

                    soc_reg_above_64_field32_set(unit, OAMP_RX_PKT_COUNTERr,
                                                 rx_pkt_reg, RX_PKT_CNTR_OPCODE_FLTRf, value);
                    break;

                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                                         (_BSL_SOCDNX_MSG("Error: Invalid counter type")));
            }
            rv = WRITE_OAMP_RX_PKT_COUNTERr(unit, rx_pkt_reg);
            SOCDNX_IF_ERR_EXIT(rv);
            break;

        case TX:
            COMPILER_64_ZERO(reg);
            switch (filter) {
                case ALL:
                    
                    break;

                case MEP:
                    soc_reg64_field32_set(unit, OAMP_TXM_PKT_COUNTERr, &reg, TXM_PKT_CNTR_MODEf, 1);
                    soc_reg64_field32_set(unit, OAMP_TXM_PKT_COUNTERr, &reg, TXM_PKT_CNTR_MEP_ID_FLTRf, value);
                    break;

                case CCM:
                    soc_reg64_field32_set(unit, OAMP_TXM_PKT_COUNTERr, &reg, TXM_PKT_CNTR_MODEf, 2);
                    break;

                case LMM:
                    soc_reg64_field32_set(unit, OAMP_TXM_PKT_COUNTERr, &reg, TXM_PKT_CNTR_MODEf, 3);
                    break;

                case DMM:
                    soc_reg64_field32_set(unit, OAMP_TXM_PKT_COUNTERr, &reg, TXM_PKT_CNTR_MODEf, 4);
                    break;

                case SLM:
                    soc_reg64_field32_set(unit, OAMP_TXM_PKT_COUNTERr, &reg, TXM_PKT_CNTR_MODEf, 5);
                    break;

                case BFD:
                    soc_reg64_field32_set(unit, OAMP_TXM_PKT_COUNTERr, &reg, TXM_PKT_CNTR_MODEf, 6);
                    break;

                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                                         (_BSL_SOCDNX_MSG("Error: Invalid filter type")));

            }
            rv = WRITE_OAMP_TXM_PKT_COUNTERr(unit, reg);
            SOCDNX_IF_ERR_EXIT(rv);
            break;

        case PUNT_RESPONSE:
            
            COMPILER_64_ZERO(reg);
            switch (filter) {
                case ALL:
                    
                    break;

                case PUNT:
                    soc_reg64_field32_set(unit, OAMP_RXB_PKT_COUNTERr, &reg, RXB_PKT_CNTR_MODEf, 1);
                    break;

                case RESPONSE:
                    soc_reg64_field32_set(unit, OAMP_RXB_PKT_COUNTERr, &reg, RXB_PKT_CNTR_MODEf, 2);
                    break;

                case RESPONSE_MEP:
                    soc_reg64_field32_set(unit, OAMP_RXB_PKT_COUNTERr, &reg, RXB_PKT_CNTR_MODEf, 3);
                    soc_reg64_field32_set(unit, OAMP_RXB_PKT_COUNTERr, &reg, RXB_PKT_CNTR_MEP_ID_FLTRf, value);
                    break;

                case RESPONSE_OPCODE:
                    soc_reg64_field32_set(unit, OAMP_RXB_PKT_COUNTERr, &reg, RXB_PKT_CNTR_MODEf, 4);
                    soc_reg64_field32_set(unit, OAMP_RXB_PKT_COUNTERr, &reg, RXB_PKT_CNTR_OPCODE_FLTRf, value);
                    break;

                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                                         (_BSL_SOCDNX_MSG("Error: Invalid filter type")));

            }
            rv = WRITE_OAMP_RXB_PKT_COUNTERr(unit, reg);
            SOCDNX_IF_ERR_EXIT(rv);
            break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                                 (_BSL_SOCDNX_MSG("Error: Invalid counter type")));

    }
exit:
    SOCDNX_FUNC_RETURN;

}



soc_error_t soc_qax_diag_oamp_counter_get(int unit, uint8 type, uint8 *filter, uint16 *value, uint32 *counter_val)
{
    uint64 reg;
    int rv = 0;
    soc_reg_above_64_val_t rx_pkt_reg = {0};
    uint32 filter_field = 0;

    SOCDNX_INIT_FUNC_DEFS;

    switch(type) {
        case RX:
            rv = READ_OAMP_RX_PKT_COUNTERr(unit, rx_pkt_reg);
            SOCDNX_IF_ERR_EXIT(rv);
            filter_field = soc_reg_above_64_field32_get(unit, OAMP_RX_PKT_COUNTERr,
                    rx_pkt_reg, RX_PKT_CNTR_MODEf);
            *counter_val = soc_reg_above_64_field32_get(unit, OAMP_RX_PKT_COUNTERr,
                    rx_pkt_reg, RX_PKT_CNTRf); 
            switch (filter_field) {
                case 0:
                    
                    *filter = ALL;
                    break;

                case 1:
                    *filter = MEP;
                    *value = soc_reg_above_64_field32_get(unit, OAMP_RX_PKT_COUNTERr,
                                                 rx_pkt_reg, RX_PKT_CNTR_MEP_ID_FLTRf);
                    break;

                case 2:
                    *filter = RMEP;
                    
                    *value = soc_reg_above_64_field32_get(unit, OAMP_RX_PKT_COUNTERr,
                                                 rx_pkt_reg, RX_PKT_CNTR_RMEP_ID_FLTRf);
                    break;

                case 3:
                    *filter = OPCODE;
                    *value = soc_reg_above_64_field32_get(unit, OAMP_RX_PKT_COUNTERr,
                                                 rx_pkt_reg, RX_PKT_CNTR_OPCODE_FLTRf);
                    break;

                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                                         (_BSL_SOCDNX_MSG("Error: Invalid filter type")));
            }
            break;

        case TX:
            COMPILER_64_ZERO(reg);
            rv = READ_OAMP_TXM_PKT_COUNTERr(unit, &reg);
            SOCDNX_IF_ERR_EXIT(rv);
            filter_field = soc_reg64_field32_get(unit, OAMP_TXM_PKT_COUNTERr, reg, TXM_PKT_CNTR_MODEf);
            *counter_val = soc_reg64_field32_get(unit, OAMP_TXM_PKT_COUNTERr, reg, TXM_PKT_CNTRf);
            switch (filter_field) {
                case 0:
                    *filter = ALL;
                    break;

                case 1:
                    *filter = MEP;
                    *value = soc_reg64_field32_get(unit, OAMP_TXM_PKT_COUNTERr, reg, TXM_PKT_CNTR_MEP_ID_FLTRf);
                    break;

                case 2:
                    *filter = CCM;
                    break;

                case 3:
                    *filter = LMM;
                    break;

                case 4:
                    *filter = DMM;
                    break;

                case 5:
                    *filter = SLM;
                    break;

                case 6:
                    *filter = BFD;
                    break;

                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                                         (_BSL_SOCDNX_MSG("Error: Invalid filter type")));

            }
            break;

        case PUNT_RESPONSE:
            
            COMPILER_64_ZERO(reg);
            rv = READ_OAMP_RXB_PKT_COUNTERr(unit, &reg);
            SOCDNX_IF_ERR_EXIT(rv);
            filter_field = soc_reg64_field32_get(unit, OAMP_RXB_PKT_COUNTERr, reg, RXB_PKT_CNTR_MODEf);
            *counter_val = soc_reg64_field32_get(unit, OAMP_RXB_PKT_COUNTERr, reg, RXB_PKT_CNTRf);
            switch (filter_field) {
                case 0:
                    *filter = ALL;
                    break;

                case 1:
                    *filter = PUNT;
                    break;

                case 2:
                    *filter = RESPONSE;
                    break;

                case 3:
                    *filter = RESPONSE_MEP;
                    *value = soc_reg64_field32_get(unit, OAMP_RXB_PKT_COUNTERr, reg, RXB_PKT_CNTR_MEP_ID_FLTRf);
                    break;

                case 4:
                    *filter = RESPONSE_OPCODE;
                    *value = soc_reg64_field32_get(unit, OAMP_RXB_PKT_COUNTERr, reg, RXB_PKT_CNTR_OPCODE_FLTRf);
                    break;

                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                                         (_BSL_SOCDNX_MSG("Error: Invalid filter type")));

            }
            break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                                 (_BSL_SOCDNX_MSG("Error: Invalid counter type")));

    }
exit:
    SOCDNX_FUNC_RETURN;

}



soc_error_t soc_maid_48_qax_mep_pe_profile(
    SOC_SAND_IN      int            unit,
    SOC_SAND_IN      uint32            is_upmep,
    SOC_SAND_IN      uint32            mep_type,
    SOC_SAND_IN      uint32            out_lif,
    SOC_SAND_OUT     uint32            *mep_pe_profile)
{


    uint32 res=SOC_SAND_OK;
    uint32 reg_val = 0;
    uint32 user_header_size, user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_pmf_db_fes_user_header_sizes_get( unit,
                                                     &user_header_0_size,
                                                     &user_header_1_size,
                                                     &user_header_egress_pmf_offset_0_dummy,
                                                     &user_header_egress_pmf_offset_1_dummy);
    user_header_size = (user_header_0_size + user_header_1_size) / 8;
    SOCDNX_IF_ERR_EXIT(res);


    
    if(out_lif != 0) {
        
        if (mep_type ==  SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) {
            arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_CCM_COUNT_48_MAID_QAX, &reg_val);
        
        }else if ((mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM)){
            arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_EGR_INJ_48_MAID_QAX, &reg_val);
        }
        
        reg_val |= ((out_lif & 0x30000) >> 16);
        *mep_pe_profile = reg_val;
    }else{
        if(is_upmep) {
            arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_MAID_48, &reg_val);
        }else{
            
            if(user_header_size){
                 arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_MAID_48_UDH, &reg_val);
            } else{
                 arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_MAID_48, &reg_val);
            }
        }
        *mep_pe_profile = reg_val;
    }

exit:
    SOCDNX_FUNC_RETURN;

}




#include <soc/dpp/SAND/Utils/sand_footer.h>

