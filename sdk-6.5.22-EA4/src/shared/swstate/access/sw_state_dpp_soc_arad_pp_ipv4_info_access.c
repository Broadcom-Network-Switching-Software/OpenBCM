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
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/access/sw_state_dpp_soc_arad_pp_ipv4_info_access.h>

#ifdef _ERR_MSG_MODULE_NAME
    #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

extern shr_sw_state_t *sw_state[BCM_MAX_NUM_UNITS];

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_set(int unit, CONST ARAD_PP_IPV4_INFO *ipv4_info){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.verify( unit));
    SHR_SW_STATE_MEMSET(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->ipv4_info[0],
        ipv4_info,
        ARAD_PP_IPV4_INFO,
        SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
        "sw_state_dpp_soc_arad_pp_ipv4_info_set");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_get(int unit, ARAD_PP_IPV4_INFO *ipv4_info){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.verify( unit));
    *ipv4_info = sw_state[unit]->dpp.soc.arad.pp->ipv4_info[0];
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_alloc(int unit){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_ALLOC(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->ipv4_info,
        sizeof(ARAD_PP_IPV4_INFO),
        SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
        "sw_state_dpp_soc_arad_pp_ipv4_info_alloc");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_is_allocated(int unit, uint8 *is_allocated){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_IS_ALLOC(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->ipv4_info,
        is_allocated,
        SW_STATE_NONE,
        "sw_state_dpp_soc_arad_pp_ipv4_info_alloc");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_free(int unit){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_FREE(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->ipv4_info,
        SW_STATE_NONE,
        "sw_state_dpp_soc_arad_pp_ipv4_info_alloc");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_verify(int unit){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.verify( unit);
    _SOC_IF_ERR_EXIT(_rv);
    SW_STATE_NULL_ACCESS_CHECK(sw_state[unit]->dpp.soc.arad.pp->ipv4_info);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_default_fec_set(int unit, uint32 default_fec){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.default_fec.verify( unit));
    SHR_SW_STATE_SET(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->ipv4_info->default_fec,
        default_fec,
        uint32,
        SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
        "sw_state_dpp_soc_arad_pp_ipv4_info_default_fec_set");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_default_fec_get(int unit, uint32 *default_fec){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.default_fec.verify( unit));
    *default_fec = sw_state[unit]->dpp.soc.arad.pp->ipv4_info->default_fec;
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_default_fec_verify(int unit){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.verify( unit);
    _SOC_IF_ERR_EXIT(_rv);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_nof_lpm_entries_in_lpm_set(int unit, uint32 nof_lpm_entries_in_lpm){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_lpm_entries_in_lpm.verify( unit));
    SHR_SW_STATE_SET(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->ipv4_info->nof_lpm_entries_in_lpm,
        nof_lpm_entries_in_lpm,
        uint32,
        SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
        "sw_state_dpp_soc_arad_pp_ipv4_info_nof_lpm_entries_in_lpm_set");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_nof_lpm_entries_in_lpm_get(int unit, uint32 *nof_lpm_entries_in_lpm){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_lpm_entries_in_lpm.verify( unit));
    *nof_lpm_entries_in_lpm = sw_state[unit]->dpp.soc.arad.pp->ipv4_info->nof_lpm_entries_in_lpm;
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_nof_lpm_entries_in_lpm_verify(int unit){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.verify( unit);
    _SOC_IF_ERR_EXIT(_rv);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_nof_vrfs_set(int unit, uint32 nof_vrfs){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_vrfs.verify( unit));
    SHR_SW_STATE_SET(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->ipv4_info->nof_vrfs,
        nof_vrfs,
        uint32,
        SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
        "sw_state_dpp_soc_arad_pp_ipv4_info_nof_vrfs_set");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_nof_vrfs_get(int unit, uint32 *nof_vrfs){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_vrfs.verify( unit));
    *nof_vrfs = sw_state[unit]->dpp.soc.arad.pp->ipv4_info->nof_vrfs;
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_nof_vrfs_verify(int unit){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.verify( unit);
    _SOC_IF_ERR_EXIT(_rv);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_set(int unit, int _bit_num){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.verify(unit, _bit_num/SHR_BITWID);
   _SOC_IF_ERR_EXIT(_rv);
        SHR_SW_STATE_BIT_SET(
        unit,
         SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
         sw_state[unit]->dpp.soc.arad.pp->ipv4_info->vrf_modified_bitmask,
         _bit_num);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_clear(int unit, int _bit_num){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.verify(unit, _bit_num/SHR_BITWID);
   _SOC_IF_ERR_EXIT(_rv);
        SHR_SW_STATE_BIT_CLEAR(
        unit,
         SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
         sw_state[unit]->dpp.soc.arad.pp->ipv4_info->vrf_modified_bitmask,
         _bit_num);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_get(int unit, int _bit_num, uint8* result){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.verify(unit, _bit_num/SHR_BITWID);
   _SOC_IF_ERR_EXIT(_rv);
        SHR_SW_STATE_BIT_GET(
        unit,
         SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
         sw_state[unit]->dpp.soc.arad.pp->ipv4_info->vrf_modified_bitmask,
         _bit_num,
         result);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_read(int unit, int sw_state_bmp_first, int result_first, int _range, SHR_BITDCL *result){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.verify(unit, (sw_state_bmp_first + _range - 1)/SHR_BITWID);
    _SOC_IF_ERR_EXIT(_rv);
        SHR_SW_STATE_BIT_RANGE_READ(
        unit,
         SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
         sw_state[unit]->dpp.soc.arad.pp->ipv4_info->vrf_modified_bitmask,
         sw_state_bmp_first,
         result_first,
         _range,
         result);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_write(int unit, int sw_state_bmp_first, int input_bmp_first, int _range, SHR_BITDCL *input_bmp){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.verify(unit, (sw_state_bmp_first + _range - 1)/SHR_BITWID);
    _SOC_IF_ERR_EXIT(_rv);
        SHR_SW_STATE_BIT_RANGE_WRITE(
        unit,
         SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
         sw_state[unit]->dpp.soc.arad.pp->ipv4_info->vrf_modified_bitmask,
         sw_state_bmp_first,
         input_bmp_first,
          _range,
         input_bmp);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_and(int unit, int _first, int _count, SHR_BITDCL *input_bmp){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.verify(unit, (_first + _count - 1)/SHR_BITWID);
    _SOC_IF_ERR_EXIT(_rv);
        SHR_SW_STATE_BIT_RANGE_AND(
        unit,
         SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
         sw_state[unit]->dpp.soc.arad.pp->ipv4_info->vrf_modified_bitmask,
         input_bmp,
         _first,
         _count);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_or(int unit, int _first, int _count, SHR_BITDCL *input_bmp){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.verify(unit, (_first + _count - 1)/SHR_BITWID);
    _SOC_IF_ERR_EXIT(_rv);
        SHR_SW_STATE_BIT_RANGE_OR(
        unit,
         SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
         sw_state[unit]->dpp.soc.arad.pp->ipv4_info->vrf_modified_bitmask,
         input_bmp,
         _first,
         _count);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_xor(int unit, int _first, int _count, SHR_BITDCL *input_bmp){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.verify(unit, (_first + _count - 1)/SHR_BITWID);
    _SOC_IF_ERR_EXIT(_rv);
        SHR_SW_STATE_BIT_RANGE_XOR(
        unit,
         SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
         sw_state[unit]->dpp.soc.arad.pp->ipv4_info->vrf_modified_bitmask,
         input_bmp,
         _first,
         _count);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_remove(int unit, int _first, int _count, SHR_BITDCL *input_bmp){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.verify(unit, (_first + _count - 1)/SHR_BITWID);
    _SOC_IF_ERR_EXIT(_rv);
        SHR_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
         sw_state[unit]->dpp.soc.arad.pp->ipv4_info->vrf_modified_bitmask,
         input_bmp,
         _first,
         _count);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_negate(int unit, int _first, int _count){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.verify(unit, (_first + _count - 1)/SHR_BITWID);
    _SOC_IF_ERR_EXIT(_rv);
        SHR_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
         sw_state[unit]->dpp.soc.arad.pp->ipv4_info->vrf_modified_bitmask,
         _first,
         _count);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_clear(int unit, int _first, int _count){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.verify(unit, (_first + _count - 1)/SHR_BITWID);
    _SOC_IF_ERR_EXIT(_rv);
        SHR_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
         sw_state[unit]->dpp.soc.arad.pp->ipv4_info->vrf_modified_bitmask,
         _first,
         _count);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_set(int unit, int _first, int _count){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.verify(unit, (_first + _count - 1)/SHR_BITWID);
    _SOC_IF_ERR_EXIT(_rv);
        SHR_SW_STATE_BIT_RANGE_SET(
        unit,
         SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
         sw_state[unit]->dpp.soc.arad.pp->ipv4_info->vrf_modified_bitmask,
         _first,
         _count);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_null(int unit, int _first, int _count, uint8 *result){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.verify(unit, (_first + _count - 1)/SHR_BITWID);
    _SOC_IF_ERR_EXIT(_rv);
        SHR_SW_STATE_BIT_RANGE_NULL(
        unit,
         SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
         sw_state[unit]->dpp.soc.arad.pp->ipv4_info->vrf_modified_bitmask,
         _first,
         _count,
         result);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_test(int unit, int _first, int _count, uint8 *result){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.verify(unit, (_first + _count - 1)/SHR_BITWID);
    _SOC_IF_ERR_EXIT(_rv);
        SHR_SW_STATE_BIT_RANGE_TEST(
        unit,
         SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
         sw_state[unit]->dpp.soc.arad.pp->ipv4_info->vrf_modified_bitmask,
         _first,
         _count,
         result);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_eq(int unit, SHR_BITDCL *input_bmp, int _first, int _count, uint8 *result){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.verify(unit, (_first + _count - 1)/SHR_BITWID);
    _SOC_IF_ERR_EXIT(_rv);
        SHR_SW_STATE_BIT_RANGE_EQ(
        unit,
         SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
         sw_state[unit]->dpp.soc.arad.pp->ipv4_info->vrf_modified_bitmask,
         _first,
         _count,
         input_bmp,
         result);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_count(int unit, int _first, int _range, int *result){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.verify(unit, (_first + _range - 1)/SHR_BITWID);
    _SOC_IF_ERR_EXIT(_rv);
        SHR_SW_STATE_BIT_RANGE_COUNT(
        unit,
         SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
         sw_state[unit]->dpp.soc.arad.pp->ipv4_info->vrf_modified_bitmask,
         _first,
         _range,
         result);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_verify(int unit, int vrf_modified_bitmask_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.verify( unit);
    _SOC_IF_ERR_EXIT(_rv);
    SW_STATE_OUT_OF_BOUND_CHECK( vrf_modified_bitmask_idx_0, ARAD_PP_IPV4_VRF_BITMAP_SIZE, "vrf_modified_bitmask");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_cache_modified_set(int unit, uint8 cache_modified){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.cache_modified.verify( unit));
    SHR_SW_STATE_SET(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->ipv4_info->cache_modified,
        cache_modified,
        uint8,
        SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
        "sw_state_dpp_soc_arad_pp_ipv4_info_cache_modified_set");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_cache_modified_get(int unit, uint8 *cache_modified){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.cache_modified.verify( unit));
    *cache_modified = sw_state[unit]->dpp.soc.arad.pp->ipv4_info->cache_modified;
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_cache_modified_verify(int unit){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.verify( unit);
    _SOC_IF_ERR_EXIT(_rv);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_cache_mode_set(int unit, uint32 cache_mode){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.cache_mode.verify( unit));
    SHR_SW_STATE_SET(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->ipv4_info->cache_mode,
        cache_mode,
        uint32,
        SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
        "sw_state_dpp_soc_arad_pp_ipv4_info_cache_mode_set");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_cache_mode_get(int unit, uint32 *cache_mode){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.cache_mode.verify( unit));
    *cache_mode = sw_state[unit]->dpp.soc.arad.pp->ipv4_info->cache_mode;
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_cache_mode_verify(int unit){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.verify( unit);
    _SOC_IF_ERR_EXIT(_rv);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#ifdef SOC_SAND_DEBUG
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_lem_add_fail_set(int unit, uint8 lem_add_fail){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.lem_add_fail.verify( unit));
    SHR_SW_STATE_SET(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->ipv4_info->lem_add_fail,
        lem_add_fail,
        uint8,
        SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
        "sw_state_dpp_soc_arad_pp_ipv4_info_lem_add_fail_set");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* SOC_SAND_DEBUG*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#ifdef SOC_SAND_DEBUG
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_lem_add_fail_get(int unit, uint8 *lem_add_fail){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.lem_add_fail.verify( unit));
    *lem_add_fail = sw_state[unit]->dpp.soc.arad.pp->ipv4_info->lem_add_fail;
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* SOC_SAND_DEBUG*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#ifdef SOC_SAND_DEBUG
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_ipv4_info_lem_add_fail_verify(int unit){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.verify( unit);
    _SOC_IF_ERR_EXIT(_rv);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* SOC_SAND_DEBUG*/ 
#endif /* BCM_PETRA_SUPPORT*/ 


int
sw_state_dpp_soc_arad_pp_ipv4_info_access_cb_init(int unit){
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.set = sw_state_dpp_soc_arad_pp_ipv4_info_set;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.get = sw_state_dpp_soc_arad_pp_ipv4_info_get;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.alloc = sw_state_dpp_soc_arad_pp_ipv4_info_alloc;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.is_allocated = sw_state_dpp_soc_arad_pp_ipv4_info_is_allocated;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.free = sw_state_dpp_soc_arad_pp_ipv4_info_free;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.verify = sw_state_dpp_soc_arad_pp_ipv4_info_verify;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.default_fec.set = sw_state_dpp_soc_arad_pp_ipv4_info_default_fec_set;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.default_fec.get = sw_state_dpp_soc_arad_pp_ipv4_info_default_fec_get;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.default_fec.verify = sw_state_dpp_soc_arad_pp_ipv4_info_default_fec_verify;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_lpm_entries_in_lpm.set = sw_state_dpp_soc_arad_pp_ipv4_info_nof_lpm_entries_in_lpm_set;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_lpm_entries_in_lpm.get = sw_state_dpp_soc_arad_pp_ipv4_info_nof_lpm_entries_in_lpm_get;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_lpm_entries_in_lpm.verify = sw_state_dpp_soc_arad_pp_ipv4_info_nof_lpm_entries_in_lpm_verify;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_vrfs.set = sw_state_dpp_soc_arad_pp_ipv4_info_nof_vrfs_set;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_vrfs.get = sw_state_dpp_soc_arad_pp_ipv4_info_nof_vrfs_get;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_vrfs.verify = sw_state_dpp_soc_arad_pp_ipv4_info_nof_vrfs_verify;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_set = sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_set;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_clear = sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_clear;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_get = sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_get;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_range_read = sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_read;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_range_write = sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_write;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_range_and = sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_and;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_range_or = sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_or;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_range_xor = sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_xor;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_range_remove = sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_remove;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_range_negate = sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_negate;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_range_clear = sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_clear;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_range_set = sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_set;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_range_null = sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_null;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_range_test = sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_test;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_range_eq = sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_eq;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_range_count = sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_bit_range_count;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.verify = sw_state_dpp_soc_arad_pp_ipv4_info_vrf_modified_bitmask_verify;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.cache_modified.set = sw_state_dpp_soc_arad_pp_ipv4_info_cache_modified_set;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.cache_modified.get = sw_state_dpp_soc_arad_pp_ipv4_info_cache_modified_get;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.cache_modified.verify = sw_state_dpp_soc_arad_pp_ipv4_info_cache_modified_verify;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.cache_mode.set = sw_state_dpp_soc_arad_pp_ipv4_info_cache_mode_set;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.cache_mode.get = sw_state_dpp_soc_arad_pp_ipv4_info_cache_mode_get;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.cache_mode.verify = sw_state_dpp_soc_arad_pp_ipv4_info_cache_mode_verify;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#ifdef SOC_SAND_DEBUG
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.lem_add_fail.set = sw_state_dpp_soc_arad_pp_ipv4_info_lem_add_fail_set;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* SOC_SAND_DEBUG*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#ifdef SOC_SAND_DEBUG
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.lem_add_fail.get = sw_state_dpp_soc_arad_pp_ipv4_info_lem_add_fail_get;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* SOC_SAND_DEBUG*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#ifdef SOC_SAND_DEBUG
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.lem_add_fail.verify = sw_state_dpp_soc_arad_pp_ipv4_info_lem_add_fail_verify;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* SOC_SAND_DEBUG*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
    return _SHR_E_NONE;
}

#undef _ERR_MSG_MODULE_NAME
