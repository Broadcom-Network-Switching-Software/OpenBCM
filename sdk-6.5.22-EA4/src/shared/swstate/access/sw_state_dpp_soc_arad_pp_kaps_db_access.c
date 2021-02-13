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
#include <shared/swstate/access/sw_state_dpp_soc_arad_pp_kaps_db_access.h>

#ifdef _ERR_MSG_MODULE_NAME
    #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

extern shr_sw_state_t *sw_state[BCM_MAX_NUM_UNITS];

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_set(int unit, CONST JER_KAPS *kaps_db){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.kaps_db.verify( unit));
    SHR_SW_STATE_MEMSET(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->kaps_db[0],
        kaps_db,
        JER_KAPS,
        SW_STATE_ALLOW_AUTOSYNC,
        "sw_state_dpp_soc_arad_pp_kaps_db_set");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_get(int unit, JER_KAPS *kaps_db){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.kaps_db.verify( unit));
    *kaps_db = sw_state[unit]->dpp.soc.arad.pp->kaps_db[0];
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_alloc(int unit){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_ALLOC(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->kaps_db,
        sizeof(JER_KAPS),
        SW_STATE_ALLOW_AUTOSYNC,
        "sw_state_dpp_soc_arad_pp_kaps_db_alloc");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_is_allocated(int unit, uint8 *is_allocated){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_IS_ALLOC(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->kaps_db,
        is_allocated,
        SW_STATE_NONE,
        "sw_state_dpp_soc_arad_pp_kaps_db_alloc");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_free(int unit){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_FREE(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->kaps_db,
        SW_STATE_NONE,
        "sw_state_dpp_soc_arad_pp_kaps_db_alloc");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_verify(int unit){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.verify( unit);
    _SOC_IF_ERR_EXIT(_rv);
    SW_STATE_NULL_ACCESS_CHECK(sw_state[unit]->dpp.soc.arad.pp->kaps_db);
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_db_info_set(int unit, int db_info_idx_0, JER_KAPS_DB_HANDLES db_info){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.kaps_db.db_info.verify( unit, db_info_idx_0));
    SHR_SW_STATE_SET(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->kaps_db->db_info[db_info_idx_0],
        db_info,
        JER_KAPS_DB_HANDLES,
        SW_STATE_ALLOW_AUTOSYNC,
        "sw_state_dpp_soc_arad_pp_kaps_db_db_info_set");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_db_info_get(int unit, int db_info_idx_0, JER_KAPS_DB_HANDLES *db_info){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.kaps_db.db_info.verify( unit, db_info_idx_0));
    *db_info = sw_state[unit]->dpp.soc.arad.pp->kaps_db->db_info[db_info_idx_0];
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_db_info_verify(int unit, int db_info_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.kaps_db.verify( unit);
    _SOC_IF_ERR_EXIT(_rv);
    SW_STATE_OUT_OF_BOUND_CHECK( db_info_idx_0, JER_KAPS_IP_NOF_DB, "db_info");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_dma_db_info_set(int unit, int dma_db_info_idx_0, JER_KAPS_DMA_DB_HANDLES dma_db_info){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.kaps_db.dma_db_info.verify( unit, dma_db_info_idx_0));
    SHR_SW_STATE_SET(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->kaps_db->dma_db_info[dma_db_info_idx_0],
        dma_db_info,
        JER_KAPS_DMA_DB_HANDLES,
        SW_STATE_ALLOW_AUTOSYNC,
        "sw_state_dpp_soc_arad_pp_kaps_db_dma_db_info_set");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_dma_db_info_get(int unit, int dma_db_info_idx_0, JER_KAPS_DMA_DB_HANDLES *dma_db_info){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.kaps_db.dma_db_info.verify( unit, dma_db_info_idx_0));
    *dma_db_info = sw_state[unit]->dpp.soc.arad.pp->kaps_db->dma_db_info[dma_db_info_idx_0];
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_dma_db_info_verify(int unit, int dma_db_info_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.kaps_db.verify( unit);
    _SOC_IF_ERR_EXIT(_rv);
    SW_STATE_OUT_OF_BOUND_CHECK( dma_db_info_idx_0, JER_KAPS_NOF_MAX_DMA_DB, "dma_db_info");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_search_instruction_info_set(int unit, int search_instruction_info_idx_0, JER_KAPS_INSTRUCTION_HANDLES search_instruction_info){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.kaps_db.search_instruction_info.verify( unit, search_instruction_info_idx_0));
    SHR_SW_STATE_SET(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->kaps_db->search_instruction_info[search_instruction_info_idx_0],
        search_instruction_info,
        JER_KAPS_INSTRUCTION_HANDLES,
        SW_STATE_ALLOW_AUTOSYNC,
        "sw_state_dpp_soc_arad_pp_kaps_db_search_instruction_info_set");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_search_instruction_info_get(int unit, int search_instruction_info_idx_0, JER_KAPS_INSTRUCTION_HANDLES *search_instruction_info){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.kaps_db.search_instruction_info.verify( unit, search_instruction_info_idx_0));
    *search_instruction_info = sw_state[unit]->dpp.soc.arad.pp->kaps_db->search_instruction_info[search_instruction_info_idx_0];
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_search_instruction_info_verify(int unit, int search_instruction_info_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.kaps_db.verify( unit);
    _SOC_IF_ERR_EXIT(_rv);
    SW_STATE_OUT_OF_BOUND_CHECK( search_instruction_info_idx_0, JER_KAPS_NOF_SEARCHES, "search_instruction_info");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_table_handles_set(int unit, int table_handles_idx_0, JER_KAPS_TABLE_HANDLES table_handles){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.kaps_db.table_handles.verify( unit, table_handles_idx_0));
    SHR_SW_STATE_SET(
        unit,
        sw_state[unit]->dpp.soc.arad.pp->kaps_db->table_handles[table_handles_idx_0],
        table_handles,
        JER_KAPS_TABLE_HANDLES,
        SW_STATE_ALLOW_AUTOSYNC,
        "sw_state_dpp_soc_arad_pp_kaps_db_table_handles_set");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_table_handles_get(int unit, int table_handles_idx_0, JER_KAPS_TABLE_HANDLES *table_handles){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_VERIFY( sw_state_access[unit].dpp.soc.arad.pp.kaps_db.table_handles.verify( unit, table_handles_idx_0));
    *table_handles = sw_state[unit]->dpp.soc.arad.pp->kaps_db->table_handles[table_handles_idx_0];
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_dpp_soc_arad_pp_kaps_db_table_handles_verify(int unit, int table_handles_idx_0){
    SW_STATE_INIT_FUNC_DEFS;
    _rv = sw_state_access[unit].dpp.soc.arad.pp.kaps_db.verify( unit);
    _SOC_IF_ERR_EXIT(_rv);
    SW_STATE_OUT_OF_BOUND_CHECK( table_handles_idx_0, JER_KAPS_IP_NOF_TABLES, "table_handles");
    SW_STATE_FUNC_RETURN;
}

#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 


int
sw_state_dpp_soc_arad_pp_kaps_db_access_cb_init(int unit){
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.set = sw_state_dpp_soc_arad_pp_kaps_db_set;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.get = sw_state_dpp_soc_arad_pp_kaps_db_get;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.alloc = sw_state_dpp_soc_arad_pp_kaps_db_alloc;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.is_allocated = sw_state_dpp_soc_arad_pp_kaps_db_is_allocated;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.free = sw_state_dpp_soc_arad_pp_kaps_db_free;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.verify = sw_state_dpp_soc_arad_pp_kaps_db_verify;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.db_info.set = sw_state_dpp_soc_arad_pp_kaps_db_db_info_set;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.db_info.get = sw_state_dpp_soc_arad_pp_kaps_db_db_info_get;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.db_info.verify = sw_state_dpp_soc_arad_pp_kaps_db_db_info_verify;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.dma_db_info.set = sw_state_dpp_soc_arad_pp_kaps_db_dma_db_info_set;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.dma_db_info.get = sw_state_dpp_soc_arad_pp_kaps_db_dma_db_info_get;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.dma_db_info.verify = sw_state_dpp_soc_arad_pp_kaps_db_dma_db_info_verify;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.search_instruction_info.set = sw_state_dpp_soc_arad_pp_kaps_db_search_instruction_info_set;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.search_instruction_info.get = sw_state_dpp_soc_arad_pp_kaps_db_search_instruction_info_get;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.search_instruction_info.verify = sw_state_dpp_soc_arad_pp_kaps_db_search_instruction_info_verify;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.table_handles.set = sw_state_dpp_soc_arad_pp_kaps_db_table_handles_set;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.table_handles.get = sw_state_dpp_soc_arad_pp_kaps_db_table_handles_get;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_88675_A0)
#if defined(BCM_PETRA_SUPPORT)
    sw_state_access[unit].dpp.soc.arad.pp.kaps_db.table_handles.verify = sw_state_dpp_soc_arad_pp_kaps_db_table_handles_verify;
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* defined(BCM_88675_A0)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
    return _SHR_E_NONE;
}

#undef _ERR_MSG_MODULE_NAME
