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
#include <shared/swstate/diagnostic/sw_state_dpp_soc_arad_pp_bfd_diagnostic.h>

#ifdef BCM_WARM_BOOT_API_TEST
#ifdef _ERR_MSG_MODULE_NAME
    #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

extern shr_sw_state_t *sw_state[BCM_MAX_NUM_UNITS];

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_bfd_dump(int unit, int pp_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    {
        uint32 j0, J0;
        sw_state_get_nof_elements(unit, sw_state[unit]->dpp.soc.arad.pp->bfd, sizeof(*sw_state[unit]->dpp.soc.arad.pp->bfd), &j0 ,&J0);
        for (; j0 < J0; j0++) {
            shr_sw_state_dump_update_current_idx(unit, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.pp.bfd.trap_code_trap_to_cpu.dump(unit, pp_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.pp.bfd.trap_code_oamp_bfd_cc_mpls_tp.dump(unit, pp_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.pp.bfd.trap_code_oamp_bfd_ipv4.dump(unit, pp_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.pp.bfd.trap_code_oamp_bfd_mpls.dump(unit, pp_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.pp.bfd.trap_code_oamp_bfd_pwe.dump(unit, pp_idx_0, j0);
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
int sw_state_dpp_soc_arad_pp_bfd_trap_code_trap_to_cpu_dump(int unit, int pp_idx_0, int bfd_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.pp->bfd->trap_code_trap_to_cpu,
        "sw_state[%d]->dpp.soc.arad.pp->bfd->trap_code_trap_to_cpu: ", unit);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_bfd_trap_code_oamp_bfd_cc_mpls_tp_dump(int unit, int pp_idx_0, int bfd_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.pp->bfd->trap_code_oamp_bfd_cc_mpls_tp,
        "sw_state[%d]->dpp.soc.arad.pp->bfd->trap_code_oamp_bfd_cc_mpls_tp: ", unit);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_bfd_trap_code_oamp_bfd_ipv4_dump(int unit, int pp_idx_0, int bfd_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.pp->bfd->trap_code_oamp_bfd_ipv4,
        "sw_state[%d]->dpp.soc.arad.pp->bfd->trap_code_oamp_bfd_ipv4: ", unit);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_bfd_trap_code_oamp_bfd_mpls_dump(int unit, int pp_idx_0, int bfd_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.pp->bfd->trap_code_oamp_bfd_mpls,
        "sw_state[%d]->dpp.soc.arad.pp->bfd->trap_code_oamp_bfd_mpls: ", unit);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_bfd_trap_code_oamp_bfd_pwe_dump(int unit, int pp_idx_0, int bfd_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.pp->bfd->trap_code_oamp_bfd_pwe,
        "sw_state[%d]->dpp.soc.arad.pp->bfd->trap_code_oamp_bfd_pwe: ", unit);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 


int
sw_state_dpp_soc_arad_pp_bfd_diagnostic_cb_init(int unit){
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.pp.bfd.dump = sw_state_dpp_soc_arad_pp_bfd_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.pp.bfd.trap_code_trap_to_cpu.dump = sw_state_dpp_soc_arad_pp_bfd_trap_code_trap_to_cpu_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.pp.bfd.trap_code_oamp_bfd_cc_mpls_tp.dump = sw_state_dpp_soc_arad_pp_bfd_trap_code_oamp_bfd_cc_mpls_tp_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.pp.bfd.trap_code_oamp_bfd_ipv4.dump = sw_state_dpp_soc_arad_pp_bfd_trap_code_oamp_bfd_ipv4_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.pp.bfd.trap_code_oamp_bfd_mpls.dump = sw_state_dpp_soc_arad_pp_bfd_trap_code_oamp_bfd_mpls_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.pp.bfd.trap_code_oamp_bfd_pwe.dump = sw_state_dpp_soc_arad_pp_bfd_trap_code_oamp_bfd_pwe_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
    return _SHR_E_NONE;
}

#undef _ERR_MSG_MODULE_NAME
#endif /* BCM_WARM_BOOT_API_TEST */
