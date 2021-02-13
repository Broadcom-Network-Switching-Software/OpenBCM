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
#include <shared/swstate/access/sw_state_exm_access.h>

#ifdef _ERR_MSG_MODULE_NAME
    #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

extern shr_sw_state_t *sw_state[BCM_MAX_NUM_UNITS];

#ifdef _SHR_SW_STATE_EXM
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_exm_alloc(int unit){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_ALLOC(
        unit,
        sw_state[unit]->exm,
        sizeof(two_levels_example_t),
        SW_STATE_IS_WB_CHECK | SW_STATE_ALLOW_AUTOSYNC,
        "sw_state_exm_alloc");
    SW_STATE_FUNC_RETURN;
}

#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_exm_is_allocated(int unit, uint8 *is_allocated){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_IS_ALLOC(
        unit,
        sw_state[unit]->exm,
        is_allocated,
        SW_STATE_NONE,
        "sw_state_exm_alloc");
    SW_STATE_FUNC_RETURN;
}

#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_exm_free(int unit){
    SW_STATE_INIT_FUNC_DEFS;
    SHR_SW_STATE_FREE(
        unit,
        sw_state[unit]->exm,
        SW_STATE_NONE,
        "sw_state_exm_alloc");
    SW_STATE_FUNC_RETURN;
}

#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* AUTO-GENERATED - DO NOT MODIFY */
int sw_state_exm_verify(int unit){
    SW_STATE_INIT_FUNC_DEFS;
    if (unit >= SOC_MAX_NUM_DEVICES || unit < 0 || sw_state[unit] == NULL) return _SHR_E_UNIT;
    SW_STATE_NULL_ACCESS_CHECK(sw_state[unit]->exm);
    SW_STATE_FUNC_RETURN;
}

#endif /* _SHR_SW_STATE_EXM*/ 


int
sw_state_exm_access_cb_init(int unit){
    sw_state_exm_twoLevelsArr_access_cb_init(unit);
    sw_state_exm_twoLevelsArrArr_access_cb_init(unit);
    sw_state_exm_twoLevelsArrPtr_access_cb_init(unit);
    sw_state_exm_twoLevelsPtr_access_cb_init(unit);
    sw_state_exm_twoLevelsRegular_access_cb_init(unit);
    sw_state_exm_twoLevelsRegularAllowWbAccess_access_cb_init(unit);
    sw_state_exm_twoLevelsSinglePtr_access_cb_init(unit);
    sw_state_exm_twoLevelsStaticArrArr_access_cb_init(unit);
    sw_state_exm_twoLevelsStaticArrInt_access_cb_init(unit);
#ifdef _SHR_SW_STATE_EXM
    sw_state_access[unit].exm.alloc = sw_state_exm_alloc;
#endif /* _SHR_SW_STATE_EXM*/ 
#ifdef _SHR_SW_STATE_EXM
    sw_state_access[unit].exm.is_allocated = sw_state_exm_is_allocated;
#endif /* _SHR_SW_STATE_EXM*/ 
#ifdef _SHR_SW_STATE_EXM
    sw_state_access[unit].exm.free = sw_state_exm_free;
#endif /* _SHR_SW_STATE_EXM*/ 
#ifdef _SHR_SW_STATE_EXM
    sw_state_access[unit].exm.verify = sw_state_exm_verify;
#endif /* _SHR_SW_STATE_EXM*/ 
    return _SHR_E_NONE;
}

#undef _ERR_MSG_MODULE_NAME
