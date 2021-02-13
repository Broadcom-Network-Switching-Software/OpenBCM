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
#include <shared/swstate/diagnostic/sw_state_dpp_soc_arad_tm_arad_em_blocks_diagnostic.h>

#ifdef BCM_WARM_BOOT_API_TEST
#ifdef _ERR_MSG_MODULE_NAME
    #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

extern shr_sw_state_t *sw_state[BCM_MAX_NUM_UNITS];

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_tm_arad_em_blocks_dump(int unit, int tm_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    {
        uint32 j0, J0;
        sw_state_get_nof_elements(unit, sw_state[unit]->dpp.soc.arad.tm->arad_em_blocks, sizeof(*sw_state[unit]->dpp.soc.arad.tm->arad_em_blocks), &j0 ,&J0);
        for (; j0 < J0; j0++) {
            shr_sw_state_dump_update_current_idx(unit, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.dump(unit, tm_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.offset.dump(unit, tm_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.table_size.dump(unit, tm_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.dump(unit, tm_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.dump(unit, tm_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.dump(unit, tm_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.dump(unit, tm_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.multi_set.dump(unit, tm_idx_0, j0);
            sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.base.dump(unit, tm_idx_0, j0);
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
int sw_state_dpp_soc_arad_tm_arad_em_blocks_read_result_address_dump(int unit, int tm_idx_0, int arad_em_blocks_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.tm->arad_em_blocks[arad_em_blocks_idx_0].read_result_address,
        "sw_state[%d]->dpp.soc.arad.tm->arad_em_blocks[%d].read_result_address: ", unit, arad_em_blocks_idx_0);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_tm_arad_em_blocks_offset_dump(int unit, int tm_idx_0, int arad_em_blocks_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.tm->arad_em_blocks[arad_em_blocks_idx_0].offset,
        "sw_state[%d]->dpp.soc.arad.tm->arad_em_blocks[%d].offset: ", unit, arad_em_blocks_idx_0);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_tm_arad_em_blocks_table_size_dump(int unit, int tm_idx_0, int arad_em_blocks_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.tm->arad_em_blocks[arad_em_blocks_idx_0].table_size,
        "sw_state[%d]->dpp.soc.arad.tm->arad_em_blocks[%d].table_size: ", unit, arad_em_blocks_idx_0);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_tm_arad_em_blocks_key_size_dump(int unit, int tm_idx_0, int arad_em_blocks_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.tm->arad_em_blocks[arad_em_blocks_idx_0].key_size,
        "sw_state[%d]->dpp.soc.arad.tm->arad_em_blocks[%d].key_size: ", unit, arad_em_blocks_idx_0);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_tm_arad_em_blocks_data_nof_bytes_dump(int unit, int tm_idx_0, int arad_em_blocks_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.tm->arad_em_blocks[arad_em_blocks_idx_0].data_nof_bytes,
        "sw_state[%d]->dpp.soc.arad.tm->arad_em_blocks[%d].data_nof_bytes: ", unit, arad_em_blocks_idx_0);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_tm_arad_em_blocks_start_address_dump(int unit, int tm_idx_0, int arad_em_blocks_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.tm->arad_em_blocks[arad_em_blocks_idx_0].start_address,
        "sw_state[%d]->dpp.soc.arad.tm->arad_em_blocks[%d].start_address: ", unit, arad_em_blocks_idx_0);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_tm_arad_em_blocks_end_address_dump(int unit, int tm_idx_0, int arad_em_blocks_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.tm->arad_em_blocks[arad_em_blocks_idx_0].end_address,
        "sw_state[%d]->dpp.soc.arad.tm->arad_em_blocks[%d].end_address: ", unit, arad_em_blocks_idx_0);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_tm_arad_em_blocks_multi_set_dump(int unit, int tm_idx_0, int arad_em_blocks_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.tm->arad_em_blocks[arad_em_blocks_idx_0].multi_set,
        "sw_state[%d]->dpp.soc.arad.tm->arad_em_blocks[%d].multi_set: ", unit, arad_em_blocks_idx_0);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_tm_arad_em_blocks_base_dump(int unit, int tm_idx_0, int arad_em_blocks_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    {
        uint32 j0, J0;
        sw_state_get_nof_elements(unit, sw_state[unit]->dpp.soc.arad.tm->arad_em_blocks[arad_em_blocks_idx_0].base, sizeof(*sw_state[unit]->dpp.soc.arad.tm->arad_em_blocks[arad_em_blocks_idx_0].base), &j0 ,&J0);
        for (; j0 < J0; j0++) {
            shr_sw_state_dump_update_current_idx(unit, j0);
            SW_STATE_PRINT_OPAQUE(unit, sw_state[unit]->dpp.soc.arad.tm->arad_em_blocks[arad_em_blocks_idx_0].base[j0],
                "sw_state[%d]->dpp.soc.arad.tm->arad_em_blocks[%d].base[%d]: ", unit, arad_em_blocks_idx_0, j0);
        }
        shr_sw_state_dump_end_of_stride(unit);
    }
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 


int
sw_state_dpp_soc_arad_tm_arad_em_blocks_diagnostic_cb_init(int unit){
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.dump = sw_state_dpp_soc_arad_tm_arad_em_blocks_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.dump = sw_state_dpp_soc_arad_tm_arad_em_blocks_read_result_address_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.offset.dump = sw_state_dpp_soc_arad_tm_arad_em_blocks_offset_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.table_size.dump = sw_state_dpp_soc_arad_tm_arad_em_blocks_table_size_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.dump = sw_state_dpp_soc_arad_tm_arad_em_blocks_key_size_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.dump = sw_state_dpp_soc_arad_tm_arad_em_blocks_data_nof_bytes_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.dump = sw_state_dpp_soc_arad_tm_arad_em_blocks_start_address_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.dump = sw_state_dpp_soc_arad_tm_arad_em_blocks_end_address_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.multi_set.dump = sw_state_dpp_soc_arad_tm_arad_em_blocks_multi_set_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
    sw_state_diagnostic[unit].dpp.soc.arad.tm.arad_em_blocks.base.dump = sw_state_dpp_soc_arad_tm_arad_em_blocks_base_dump;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
    return _SHR_E_NONE;
}

#undef _ERR_MSG_MODULE_NAME
#endif /* BCM_WARM_BOOT_API_TEST */
