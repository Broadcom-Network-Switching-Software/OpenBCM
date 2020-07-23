/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INFO: this module is the entry poit for Crash Recovery feature
 * 
 * some design details:
 * - transaction starts at the beginning of an API and ends at the end of an API
 */

#include <soc/types.h>
#include <soc/error.h>
#include <shared/bsl.h>


#ifdef CRASH_RECOVERY_SUPPORT

#if defined(BCM_WARM_BOOT_API_TEST)
#include <soc/dcmn/dcmn_crash_recovery_test.h>
#endif

#include <soc/hwstate/hw_log.h>
#include <shared/swstate/sw_state_journal.h>
#include <soc/dcmn/dcmn_crash_recovery.h>
#include <soc/dcmn/dcmn_wb.h>
#include <soc/dpp/ARAD/arad_sim_em.h>
#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/scache.h>
#include <soc/dpp/drv.h>

#include <bcm_int/common/sat.h>
#include <soc/dpp/ARAD/arad_tcam_access_crash_recovery.h>
#include <soc/dpp/multicast_imp.h>

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

#define DCMN_CR_COULD_NOT_GUARANTEE 0
#define DCMN_CR_COMMITTED           1
#define DCMN_CR_ABORTED             2

#define DCMN_CR_UNIT_CHECK(unit) ((unit) >= 0 && (unit) < SOC_MAX_NUM_DEVICES)

#define DCMN_CR_DEFAULT_JOURNAL_SIZE (20000000) 

#if !defined(__KERNEL__) && defined (LINUX)
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/ha.h>
typedef enum {
    HA_CR_SUB_ID_0 = 0
} HA_CR_sub_id_tl;
#endif
#endif

uint8 dcmn_cr_suspect_crash[SOC_MAX_NUM_DEVICES];


STATIC int soc_dcmn_cr_journaling_unfreeze(int unit);
STATIC int soc_dcmn_cr_prepare_for_commit(int unit);

extern sw_state_journal_t *sw_state_journal[SOC_MAX_NUM_DEVICES];
soc_dcmn_cr_t *dcmn_cr_info[SOC_MAX_NUM_DEVICES];

int soc_dcmn_cr_init(int unit){

    uint32 hw_journal_size;
    uint32 sw_journal_size;
    uint32 mode;
    uint32 size;

    SOC_INIT_FUNC_DEFS;

    
    if (!SOC_IS_JERICHO(unit)) SOC_EXIT;
    
    if(!(mode = soc_property_get(unit, spn_HA_HW_JOURNAL_MODE, DCMN_CR_JOURNALING_MODE_DISABLED))) {
        SOC_EXIT;
    }

    _SOC_IF_ERR_EXIT(soc_stable_sanity(unit));

    
    size = sizeof(soc_dcmn_cr_t);
    if (ha_mem_alloc(unit, HA_CR_Mem_Pool, HA_CR_SUB_ID_0, &size, (void**)&dcmn_cr_info[unit]) != SOC_E_NONE) {
        return SOC_E_INTERNAL;
    }
    if (!dcmn_cr_info[unit])
    {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                       (BSL_META_U(unit, "CR shared mem Allocation failed for unit %d.\n"), unit));
        return SOC_E_MEMORY;
    }

    
    soc_dcmn_cr_journaling_mode_set(unit, mode);

    
    dcmn_cr_info[unit]->remove_suppression_in_next_commit = TRUE;

    _SOC_IF_ERR_EXIT(soc_dcmn_cr_utils_journaling_thread_set(unit));

    
    SOC_CR_ENABLE(unit);

    
    hw_journal_size = soc_property_get(unit, spn_HA_HW_JOURNAL_SIZE, 0);
    _SOC_IF_ERR_EXIT(soc_hw_log_init(unit, hw_journal_size));

    
    sw_journal_size = soc_property_get(unit, spn_HA_SW_JOURNAL_SIZE, 0);
    _SOC_IF_ERR_EXIT(sw_state_journal_init(unit, sw_journal_size));

    
    _SOC_IF_ERR_EXIT(soc_dcmn_cr_utils_logging_stop(unit));

    if (SOC_WARM_BOOT(unit)) {
        
        dcmn_cr_suspect_crash[unit] = TRUE;
    }

    _SOC_IF_ERR_EXIT(soc_dcmn_cr_transaction_api_count_reset(unit));
    _SOC_IF_ERR_EXIT(soc_dcmn_cr_traverse_flag_reset(unit));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_transaction_start(int unit) {
    
    SOC_INIT_FUNC_DEFS;

    
    if (!SOC_IS_JERICHO(unit)) SOC_EXIT;

    if (SOC_CR_ENABALED(unit)) {

#ifdef DRCM_CR_MULTIPLE_THREAD_JOURNALING
        
        _SOC_IF_ERR_EXIT(soc_dcmn_cr_utils_journaling_thread_set(unit));
#else
        
        DCMN_CR_EXIT_IF_NOT_JOURNALING_THREAD(unit);
#endif 

        
        if(soc_dcmn_cr_is_journaling_per_api(unit)) {
            if(!soc_dcmn_cr_api_is_top_level(unit)) {
                SOC_EXIT;
            }
        }

        
        _SOC_IF_ERR_EXIT(soc_hw_ensure_immediate_hw_access_disabled(unit));

        
        if (!dcmn_cr_info[unit]->remove_suppression_in_next_commit) SOC_EXIT;

        
        _SOC_IF_ERR_EXIT(soc_hw_log_ensure_not_suppressed(unit));

        if (SOC_DPP_CONFIG(unit)->arad->init.pp_enable && SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
            
            _SOC_IF_ERR_EXIT(chip_sim_exact_match_cr_shadow_clear(unit));

        }

        
        dcmn_cr_info[unit]->transaction_mode = DCMN_TRANSACTION_MODE_LOGGING;

        
        _SOC_IF_ERR_EXIT(soc_dcmn_cr_utils_logging_start(unit));

        
        arad_tcam_access_cr_dictionary_clear(unit);
    }

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

STATIC int 
soc_dcmn_cr_prepare_for_commit(int unit){
    SOC_INIT_FUNC_DEFS;

    
    _SOC_IF_ERR_EXIT(soc_dcmn_cr_utils_logging_stop(unit));

    
    dcmn_cr_info[unit]->transaction_mode = DCMN_TRANSACTION_MODE_COMMITTING;

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_commit(int unit){

#if defined(INCLUDE_KBP) && !defined(BCM_88030)        
    uint8                   kbp_dirty,kaps_dirty;
    uint32                  kbp_tbl_id,kaps_tbl_id;
#endif
    DCMN_TRANSACTION_MODE  transaction_mode;
    
    SOC_INIT_FUNC_DEFS;
    
    
    if (!SOC_IS_JERICHO(unit)) SOC_EXIT;

    
    if (!dcmn_cr_info[unit]->remove_suppression_in_next_commit) SOC_EXIT;

    
    if (!SOC_CR_ENABALED(unit)) {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                       (BSL_META_U(unit,
                          "unit:%d Crash Recovery ERROR: trying to commit while Crash Recovery feature is disabled\n"), unit));
    }

    
    DCMN_CR_EXIT_IF_NOT_JOURNALING_THREAD(unit);

     
    _SOC_IF_ERR_EXIT(soc_hw_ensure_immediate_hw_access_disabled(unit));

    
    
    transaction_mode = dcmn_cr_info[unit]->transaction_mode;
    if (transaction_mode != DCMN_TRANSACTION_MODE_COMMITTING) {
        _SOC_IF_ERR_EXIT(soc_dcmn_cr_prepare_for_commit(unit));
    }

    
    bcm_common_sat_wb_sync(unit, TRUE);

    if (SOC_DPP_CONFIG(unit)->arad->init.pp_enable && SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
        
        _SOC_IF_ERR_EXIT(chip_sim_exact_match_cr_shadow_clear(unit));
    }

    
    arad_tcam_access_cr_dictionary_clear(unit);

    
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    if (ARAD_KBP_IS_CR_MODE(unit)) {
        kbp_dirty = dcmn_cr_info[unit]->kbp_dirty;
        if (kbp_dirty) {
            kbp_tbl_id = dcmn_cr_info[unit]->kbp_tbl_id;
            if (arad_kbp_cr_db_commit(unit,kbp_tbl_id)) {
                _SOC_IF_ERR_EXIT(soc_dcmn_cr_abort(unit));
                _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                   (BSL_META_U(unit,
                      "unit:%d Crash Recovery ERROR: transaction commit failed to install KBP\n"), unit));
            }
            _SOC_IF_ERR_EXIT(arad_kbp_cr_transaction_cmd(unit,FALSE));
        }
        
        dcmn_cr_info[unit]->kbp_dirty = 0;
    }
    if (JER_KAPS_IS_CR_MODE(unit)) {
        
        kaps_dirty = dcmn_cr_info[unit]->kaps_dirty;
        if (kaps_dirty) {
            kaps_tbl_id = dcmn_cr_info[unit]->kaps_tbl_id;
            if (jer_kaps_cr_db_commit(unit,kaps_tbl_id)) {
                _SOC_IF_ERR_EXIT(soc_dcmn_cr_abort(unit));
                _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                   (BSL_META_U(unit,
                      "unit:%d Crash Recovery ERROR: transaction commit failed to install KAPS\n"), unit));
            }
            _SOC_IF_ERR_EXIT(jer_kaps_cr_transaction_cmd(unit,FALSE));
        }
        
        dcmn_cr_info[unit]->kaps_dirty = 0;
    }
#endif 

    
    DCMN_CR_TEST(_SOC_IF_ERR_EXIT(dcmn_bcm_crash_recovery_api_test_reset(unit, 1, NULL)));

    
    _SOC_IF_ERR_EXIT(soc_hw_log_commit(unit));

    _SOC_IF_ERR_EXIT(soc_dcmn_cr_transaction_api_count_reset(unit));
    _SOC_IF_ERR_EXIT(soc_dcmn_cr_traverse_flag_reset(unit));

    
    dcmn_cr_info[unit]->transaction_mode = DCMN_TRANSACTION_MODE_IDLE;

    
    _SOC_IF_ERR_EXIT(sw_state_journal_clear(unit));

    
    soc_hw_log_purge(unit);

    
    _SOC_IF_ERR_EXIT(soc_dcmn_cr_journaling_unfreeze(unit));

    
    if (dcmn_cr_info[unit]->set_journaling_mode_after_commit) {
        dcmn_cr_info[unit]->set_journaling_mode_after_commit = FALSE;
        soc_dcmn_cr_journaling_mode_set(unit,dcmn_cr_info[unit]->journaling_mode_after_commit);
    }

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

STATIC int soc_dcmn_cr_journaling_unfreeze(int unit){
    SOC_INIT_FUNC_DEFS;

    
    if (!SOC_CR_ENABALED(unit)) SOC_EXIT;

    
    DCMN_CR_EXIT_IF_NOT_JOURNALING_THREAD(unit);

    
    _SOC_IF_ERR_EXIT(soc_hw_log_unsuppress(unit));

    
    _SOC_IF_ERR_EXIT(soc_hw_reset_immediate_hw_access_counter(unit));

    
    dcmn_cr_info[unit]->is_recovarable = TRUE;
    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_dispatcher_commit(int unit){

    SOC_INIT_FUNC_DEFS;

    if (!SOC_CR_ENABALED(unit)) SOC_EXIT;

    
    DCMN_CR_EXIT_IF_NOT_JOURNALING_THREAD(unit);

    
    if(!soc_dcmn_cr_api_is_top_level(unit)) {
        SOC_EXIT;
    }
    
    if (!dcmn_cr_info[unit]->remove_suppression_in_next_commit) SOC_EXIT;

    _SOC_IF_ERR_EXIT(soc_dcmn_cr_commit(unit));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_abort(int unit){
    SOC_INIT_FUNC_DEFS;

    
    _SOC_IF_ERR_EXIT(sw_state_journal_roll_back(unit));

    
    _SOC_IF_ERR_EXIT(sw_state_journal_clear(unit));

    
#if defined(INCLUDE_KBP) && !defined(BCM_88030) 
    if (SOC_DPP_IS_ELK_ENABLE(unit) && ARAD_KBP_IS_CR_MODE(unit) && SOC_IS_DONE_INIT(unit)) {
        _SOC_IF_ERR_EXIT(arad_kbp_cr_clear_restore_status(unit));
    }
    if (JER_KAPS_ENABLE(unit) && JER_KAPS_IS_CR_MODE(unit) && SOC_IS_DONE_INIT(unit)) {
        _SOC_IF_ERR_EXIT(jer_kaps_cr_clear_restore_status(unit));
    }
#endif

    
    _SOC_IF_ERR_EXIT(soc_hw_log_purge(unit));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_recover(int unit){
    DCMN_TRANSACTION_MODE commit_status;
#if defined(INCLUDE_KBP) && !defined(BCM_88030)        
    uint8 kaps_commited = FALSE, kbp_commited = FALSE;
    uint8 need_abort = FALSE;
#endif
    int is_recovarable;
    SOC_INIT_FUNC_DEFS;

    
    if (!dcmn_cr_suspect_crash[unit]) SOC_EXIT;

    
    commit_status = dcmn_cr_info[unit]->transaction_mode;
    if (commit_status == DCMN_TRANSACTION_MODE_COMMITTING) {
        
#if defined(INCLUDE_KBP) && !defined(BCM_88030)        
        if (SOC_DPP_IS_ELK_ENABLE(unit) && ARAD_KBP_IS_CR_MODE(unit) && SOC_IS_DONE_INIT(unit)) {
            kbp_commited = arad_kbp_cr_query_restore_status(unit);
            if (!kbp_commited) need_abort = TRUE;
        }
        if (JER_KAPS_ENABLE(unit) && JER_KAPS_IS_CR_MODE(unit) && SOC_IS_DONE_INIT(unit)) {
            kaps_commited = jer_kaps_cr_query_restore_status(unit);
            if (!kaps_commited) need_abort = TRUE;
        }
        if (need_abort) {
            _SOC_IF_ERR_EXIT(soc_dcmn_cr_abort(unit));
            dcmn_cr_info[unit]->transaction_status = DCMN_CR_ABORTED;
        }
        else
#endif
        {
            _SOC_IF_ERR_EXIT(soc_dcmn_cr_api_counter_reset(unit));
            _SOC_IF_ERR_EXIT(soc_dcmn_cr_commit(unit));
            
            soc_hw_set_immediate_hw_access(unit);
            _SOC_IF_ERR_EXIT(dpp_mcds_multicast_init2(unit));
            soc_hw_restore_immediate_hw_access(unit);

        }
    } else if (commit_status == DCMN_TRANSACTION_MODE_LOGGING) {
        _SOC_IF_ERR_EXIT(soc_dcmn_cr_abort(unit));
        dcmn_cr_info[unit]->transaction_status = DCMN_CR_ABORTED;
    } else if (commit_status == DCMN_TRANSACTION_MODE_IDLE) {
        dcmn_cr_info[unit]->transaction_status = DCMN_CR_COULD_NOT_GUARANTEE;
        dcmn_cr_info[unit]->not_recoverable_reason = dcmn_cr_no_support_not_in_api;
    }

    is_recovarable = dcmn_cr_info[unit]->is_recovarable;
    if (!is_recovarable) {
        dcmn_cr_info[unit]->transaction_status = DCMN_CR_COULD_NOT_GUARANTEE;
    }

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_suppress(int unit, dcmn_cr_no_support reason){

    SOC_INIT_FUNC_DEFS;

    
    if (!SOC_CR_ENABALED(unit)) SOC_EXIT;

    
    if (!soc_dcmn_cr_utils_is_logging(unit)) SOC_EXIT;

    
    DCMN_CR_EXIT_IF_NOT_JOURNALING_THREAD(unit);

    
    _SOC_IF_ERR_EXIT(soc_hw_ensure_immediate_hw_access_disabled(unit));

    
    dcmn_cr_info[unit]->is_recovarable = FALSE;
    dcmn_cr_info[unit]->not_recoverable_reason = reason;

    if (reason == dcmn_cr_no_support_qualifier || reason == dcmn_cr_no_support_external_directory) {
        dcmn_cr_info[unit]->remove_suppression_in_next_commit = FALSE;
    }
    else {
        dcmn_cr_info[unit]->remove_suppression_in_next_commit = TRUE;
    }

    _SOC_IF_ERR_EXIT(soc_dcmn_cr_commit(unit));
    _SOC_IF_ERR_EXIT(soc_hw_log_suppress(unit));
    
    dcmn_cr_info[unit]->is_recovarable = FALSE;
    
    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_journaling_mode_get(int unit)
{
    return (SOC_CONTROL(unit)->journaling_mode);
}

int soc_dcmn_cr_is_journaling_per_api(int unit)
{
    return (SOC_CR_ENABALED(unit) &&
            (DCMN_CR_JOURNALING_MODE_AFTER_EACH_API == SOC_CONTROL(unit)->journaling_mode));
}

int soc_dcmn_cr_is_journaling_on_demand(int unit)
{
    return (SOC_CR_ENABALED(unit) &&
            (DCMN_CR_JOURNALING_MODE_ON_DEMAND == SOC_CONTROL(unit)->journaling_mode));
}

int soc_dcmn_cr_journaling_mode_set(int unit, DCMN_CR_JOURNALING_MODE mode)
{
    SOC_INIT_FUNC_DEFS;

    
    if(DCMN_CR_JOURNALING_MODE_DISABLED == mode){
        SOC_CR_DISABLE(unit);
    }

    SOC_CONTROL(unit)->journaling_mode = mode;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_dispatcher_transaction_start(int unit) {

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(soc_dcmn_cr_api_counter_increase(unit));

    if (soc_dcmn_cr_is_journaling_per_api(unit)) {
        _SOC_IF_ERR_EXIT(soc_dcmn_cr_transaction_start(unit));
    }

    
    _SOC_IF_ERR_EXIT(soc_dcmn_cr_ondemand_sanity_check(unit));
    _SOC_IF_ERR_EXIT(soc_dcmn_cr_transaction_api_count_increase(unit));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}


int soc_dcmn_cr_dispatcher_transaction_end(int unit) {

    int skip_commit = 0;
    SOC_INIT_FUNC_DEFS;

    
    DCMN_CR_TEST(_SOC_IF_ERR_EXIT(dcmn_bcm_crash_recovery_api_test_reset(unit, 0, &skip_commit)));

    
    if (soc_dcmn_cr_is_journaling_per_api(unit) && !skip_commit) {
        soc_dcmn_cr_dispatcher_commit(unit);
    }

    _SOC_IF_ERR_EXIT(soc_dcmn_cr_api_counter_decrease(unit));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_ondemand_em_traverse_hit(int unit)
{
    SOC_INIT_FUNC_DEFS;

    
    _SOC_IF_ERR_EXIT(soc_dcmn_cr_traverse_flag_set(unit));
    _SOC_IF_ERR_EXIT(soc_dcmn_cr_ondemand_sanity_check(unit));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;

}

int soc_dcmn_cr_ondemand_sanity_check(int unit)
{
    SOC_INIT_FUNC_DEFS;

    if (!SOC_CR_ENABALED(unit) || !soc_dcmn_cr_utils_is_logging(unit)) SOC_EXIT;

    
    if((soc_dcmn_cr_transaction_api_count_get(unit) > 1) &&
        soc_dcmn_cr_traverse_flag_get(unit)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                       (BSL_META_U(unit, "CR ON-DEMAND: APIs containg EM traverse operations must be in a dadicated transaction, unit %d.\n"), unit));
        return SOC_E_FAIL;
    }
    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

#endif
