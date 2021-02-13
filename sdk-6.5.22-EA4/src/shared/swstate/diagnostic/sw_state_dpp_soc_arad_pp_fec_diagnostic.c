/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 * 
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 */

#include <sal/core/libc.h>
#include <shared/swstate/sw_state.h>
#include <shared/error.h>
#include <shared/swstate/sw_state_defs.h>
#include <shared/bsl.h>
#include <shared/swstate/sw_state_dump.h>
#include <shared/swstate/diagnostic/sw_state_diagnostic.h>
#include <shared/swstate/diagnostic/sw_state_dpp_soc_arad_pp_fec_diagnostic.h>

#ifdef BCM_WARM_BOOT_API_TEST
#ifdef _ERR_MSG_MODULE_NAME
    #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

extern shr_sw_state_t *sw_state[BCM_MAX_NUM_UNITS];

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_fec_dump(int unit, int pp_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    {
        uint32 j0, J0;
        sw_state_get_nof_elements(unit, sw_state[unit]->dpp.soc.arad.pp->fec, sizeof(*sw_state[unit]->dpp.soc.arad.pp->fec), &j0 ,&J0);
        for (; j0 < J0; j0++) {
            shr_sw_state_dump_update_current_idx(unit, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.fec_entry_type.dump(unit, pp_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.dump(unit, pp_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.lem_prefix_mapping.dump(unit, pp_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.is_protected.dump(unit, pp_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.flp_hw_prog_id_bitmap.dump(unit, pp_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.flp_prog_select_id_bitmap.dump(unit, pp_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.flp_ipv4mc_bridge_v4mc_cam_sel_id.dump(unit, pp_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.name_space.dump(unit, pp_idx_0, j0);
        }
        shr_sw_state_dump_end_of_stride(unit);
    }
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_fec_fec_entry_type_dump(int unit, int pp_idx_0, int fec_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    {
        int i0;
        i0 = shr_sw_state_dump_check_all_the_same(unit, sw_state[unit]->dpp.soc.arad.pp->fec->fec_entry_type
            , sizeof(*sw_state[unit]->dpp.soc.arad.pp->fec->fec_entry_type), SOC_DPP_DEFS_MAX(NOF_FECS)) ? SOC_DPP_DEFS_MAX(NOF_FECS) - 1 : 0;
        for (; i0 < SOC_DPP_DEFS_MAX(NOF_FECS); i0++) {
            shr_sw_state_dump_update_current_idx(unit, i0);
            SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.pp->fec->fec_entry_type[i0],
                "sw_state[%d]->dpp.soc.arad.pp->fec->fec_entry_type[%d]: ", unit, i0);
        }
        shr_sw_state_dump_end_of_stride(unit);
    }
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_fec_flp_progs_mapping_dump(int unit, int pp_idx_0, int fec_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    {
        int i0;
        i0 = shr_sw_state_dump_check_all_the_same(unit, sw_state[unit]->dpp.soc.arad.pp->fec->flp_progs_mapping
            , sizeof(*sw_state[unit]->dpp.soc.arad.pp->fec->flp_progs_mapping), SOC_DPP_DEFS_MAX(NOF_FLP_PROGRAMS)) ? SOC_DPP_DEFS_MAX(NOF_FLP_PROGRAMS) - 1 : 0;
        for (; i0 < SOC_DPP_DEFS_MAX(NOF_FLP_PROGRAMS); i0++) {
            shr_sw_state_dump_update_current_idx(unit, i0);
            SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.pp->fec->flp_progs_mapping[i0],
                "sw_state[%d]->dpp.soc.arad.pp->fec->flp_progs_mapping[%d]: ", unit, i0);
        }
        shr_sw_state_dump_end_of_stride(unit);
    }
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_fec_lem_prefix_mapping_dump(int unit, int pp_idx_0, int fec_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    {
        int i0;
        i0 = shr_sw_state_dump_check_all_the_same(unit, sw_state[unit]->dpp.soc.arad.pp->fec->lem_prefix_mapping
            , sizeof(*sw_state[unit]->dpp.soc.arad.pp->fec->lem_prefix_mapping), (1<<SOC_DPP_DEFS_MAX(NOF_LEM_PREFIXES))) ? (1<<SOC_DPP_DEFS_MAX(NOF_LEM_PREFIXES)) - 1 : 0;
        for (; i0 < (1<<SOC_DPP_DEFS_MAX(NOF_LEM_PREFIXES)); i0++) {
            shr_sw_state_dump_update_current_idx(unit, i0);
            SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.pp->fec->lem_prefix_mapping[i0],
                "sw_state[%d]->dpp.soc.arad.pp->fec->lem_prefix_mapping[%d]: ", unit, i0);
        }
        shr_sw_state_dump_end_of_stride(unit);
    }
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_fec_is_protected_dump(int unit, int pp_idx_0, int fec_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    {
        int i0;
        i0 = shr_sw_state_dump_check_all_the_same(unit, sw_state[unit]->dpp.soc.arad.pp->fec->is_protected
            , sizeof(*sw_state[unit]->dpp.soc.arad.pp->fec->is_protected), _SHR_BITDCLSIZE(ARAD_PP_FEC_PROTECT_BITMAP_SIZE)) ? _SHR_BITDCLSIZE(ARAD_PP_FEC_PROTECT_BITMAP_SIZE) - 1 : 0;
        for (; i0 < _SHR_BITDCLSIZE(ARAD_PP_FEC_PROTECT_BITMAP_SIZE); i0++) {
            shr_sw_state_dump_update_current_idx(unit, i0);
            SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.pp->fec->is_protected[i0],
                "sw_state[%d]->dpp.soc.arad.pp->fec->is_protected[%d]: ", unit, i0);
        }
        shr_sw_state_dump_end_of_stride(unit);
    }
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_fec_flp_hw_prog_id_bitmap_dump(int unit, int pp_idx_0, int fec_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.pp->fec->flp_hw_prog_id_bitmap,
        "sw_state[%d]->dpp.soc.arad.pp->fec->flp_hw_prog_id_bitmap: ", unit);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_fec_flp_prog_select_id_bitmap_dump(int unit, int pp_idx_0, int fec_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.pp->fec->flp_prog_select_id_bitmap,
        "sw_state[%d]->dpp.soc.arad.pp->fec->flp_prog_select_id_bitmap: ", unit);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_fec_flp_ipv4mc_bridge_v4mc_cam_sel_id_dump(int unit, int pp_idx_0, int fec_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.pp->fec->flp_ipv4mc_bridge_v4mc_cam_sel_id,
        "sw_state[%d]->dpp.soc.arad.pp->fec->flp_ipv4mc_bridge_v4mc_cam_sel_id: ", unit);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_fec_name_space_dump(int unit, int pp_idx_0, int fec_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    {
        int i0;
        i0 = shr_sw_state_dump_check_all_the_same(unit, sw_state[unit]->dpp.soc.arad.pp->fec->name_space
            , sizeof(*sw_state[unit]->dpp.soc.arad.pp->fec->name_space), ARAD_PP_FLP_PROG_NOF) ? ARAD_PP_FLP_PROG_NOF - 1 : 0;
        for (; i0 < ARAD_PP_FLP_PROG_NOF; i0++) {
            shr_sw_state_dump_update_current_idx(unit, i0);
            sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.name_space.count.dump(unit, pp_idx_0, fec_idx_0, i0);
            sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.name_space.hw_prog.dump(unit, pp_idx_0, fec_idx_0, i0);
        }
        shr_sw_state_dump_end_of_stride(unit);
    }
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_fec_name_space_count_dump(int unit, int pp_idx_0, int fec_idx_0, int name_space_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.pp->fec->name_space[name_space_idx_0].count,
        "sw_state[%d]->dpp.soc.arad.pp->fec->name_space[%d].count: ", unit, name_space_idx_0);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_fec_name_space_hw_prog_dump(int unit, int pp_idx_0, int fec_idx_0, int name_space_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    {
        int i0;
        i0 = shr_sw_state_dump_check_all_the_same(unit, sw_state[unit]->dpp.soc.arad.pp->fec->name_space[name_space_idx_0].hw_prog
            , sizeof(*sw_state[unit]->dpp.soc.arad.pp->fec->name_space[name_space_idx_0].hw_prog), MAX_PROG_IN_NS) ? MAX_PROG_IN_NS - 1 : 0;
        for (; i0 < MAX_PROG_IN_NS; i0++) {
            shr_sw_state_dump_update_current_idx(unit, i0);
            SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.pp->fec->name_space[name_space_idx_0].hw_prog[i0],
                "sw_state[%d]->dpp.soc.arad.pp->fec->name_space[%d].hw_prog[%d]: ", unit, name_space_idx_0, i0);
        }
        shr_sw_state_dump_end_of_stride(unit);
    }
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 


int
sw_state_dpp_soc_arad_pp_fec_diagnostic_cb_init(int unit){
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.dump = sw_state_dpp_soc_arad_pp_fec_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.fec_entry_type.dump = sw_state_dpp_soc_arad_pp_fec_fec_entry_type_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.dump = sw_state_dpp_soc_arad_pp_fec_flp_progs_mapping_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.lem_prefix_mapping.dump = sw_state_dpp_soc_arad_pp_fec_lem_prefix_mapping_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.is_protected.dump = sw_state_dpp_soc_arad_pp_fec_is_protected_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.flp_hw_prog_id_bitmap.dump = sw_state_dpp_soc_arad_pp_fec_flp_hw_prog_id_bitmap_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.flp_prog_select_id_bitmap.dump = sw_state_dpp_soc_arad_pp_fec_flp_prog_select_id_bitmap_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.flp_ipv4mc_bridge_v4mc_cam_sel_id.dump = sw_state_dpp_soc_arad_pp_fec_flp_ipv4mc_bridge_v4mc_cam_sel_id_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.name_space.dump = sw_state_dpp_soc_arad_pp_fec_name_space_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.name_space.count.dump = sw_state_dpp_soc_arad_pp_fec_name_space_count_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.pp.fec.name_space.hw_prog.dump = sw_state_dpp_soc_arad_pp_fec_name_space_hw_prog_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
    return _SHR_E_NONE;
}

#undef _ERR_MSG_MODULE_NAME
#endif /* BCM_WARM_BOOT_API_TEST */
