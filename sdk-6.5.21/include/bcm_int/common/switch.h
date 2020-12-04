/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * Common internal definitions for BCM switch module
 */

#ifndef _BCM_INT_SWITCH_H_
#define _BCM_INT_SWITCH_H_

#include <soc/dnxc/multithread_analyzer.h>
#include <assert.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/dcmn/dcmn_crash_recovery.h>
#include <soc/dcmn/dcmn_crash_recovery_test.h>
#include <soc/dcmn/dcmn_crash_recovery_utils.h>
#else
#if defined(BCM_DNX_SUPPORT)
#include <soc/dnx/dnx_err_recovery_manager.h>
#endif
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <soc/dnxc/dnxc_wb_test_utils.h>
#endif
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/control.h>


#include <soc/defs.h>
extern char warmboot_api_function_name[SOC_MAX_NUM_DEVICES][100];
#if defined(BCM_WARM_BOOT_API_TEST) && (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))
extern char dnxc_warmboot_api_function_name[SOC_MAX_NUM_DEVICES][DNXC_WARMBOOT_API_FUNCTION_NAME_MAX_LEN];
#endif

extern int _bcm_switch_state_sync(int unit, bcm_dtype_t dtype);

#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_WARM_BOOT_API_TEST)

#include <soc/dcmn/dcmn_wb.h>
#endif /* BCM_PETRA_SUPPORT && BCM_WARM_BOOT_API_TEST */

#define BCM_STATE_SYNC(_u) \
    /* next line actually call sync only in autosync mode */ \
    _bcm_switch_state_sync(_u, dtype);

#else /* !BCM_WARM_BOOT_SUPPORT */
#define BCM_STATE_SYNC(_u)
#endif /* BCM_WARM_BOOT_SUPPORT */


/************** WARM BOOT API TEST ********************/

#if defined(BCM_WARM_BOOT_API_TEST)
#define DNXC_WARM_BOOT_API_TEST_RESET(unit) \
    do { \
        if (dnxc_warm_boot_api_test_reset(unit) != 0) \
        {\
            assert(0);\
        }\
    } while(0)

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#define DNXC_WARM_BOOT_API_TEST_FUNCTION_NAME_UPDATE(unit)                                                                  \
    do {                                                                                                                    \
        if (dnxc_wb_all_reset_test_utils_preconditions(unit))                                                               \
        {                                                                                                                   \
            sal_strncpy(dnxc_warmboot_api_function_name[unit], __func__, DNXC_WARMBOOT_API_FUNCTION_NAME_MAX_LEN - 1);  \
        }                                                                                                                   \
    } while(0)
#else /* defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) */
#define DNXC_WARM_BOOT_API_TEST_FUNCTION_NAME_UPDATE(unit) \
    sal_strncpy(dnxc_warmboot_api_function_name[unit], __func__, DNXC_WARMBOOT_API_FUNCTION_NAME_MAX_LEN - 1)
#endif /* defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) */


#define DNXC_WB_API_TEST_COUNTER_INCREASE(unit) \
    do { \
        dnxc_wb_api_test_counter_increase(unit); \
    } while(0)
#define DNXC_WB_API_TEST_COUNTER_DECREASE(unit) \
    do { \
        dnxc_wb_api_test_counter_decrease(unit); \
    } while(0)
#else
#define DNXC_WARM_BOOT_API_TEST_RESET(unit)
#define DNXC_WARM_BOOT_API_TEST_FUNCTION_NAME_UPDATE(unit)
#define DNXC_WB_API_TEST_COUNTER_INCREASE(unit)
#define DNXC_WB_API_TEST_COUNTER_DECREASE(unit)
#endif

/******************************************************/



/****************** CRASH RECOVERY **************************/

#ifdef CRASH_RECOVERY_SUPPORT
#define BCM_PRE_API_LOGIC_HOOK(unit)\
    SOC_CR_DOCUMENT_API_NAME(unit);\
    BCM_CR_TEST_TRANSACTION_START(unit);\
    SOC_CR_DISP_ERR_CHECK(soc_dcmn_cr_dispatcher_transaction_start(unit))
#define BCM_POST_API_LOGIC_HOOK(unit)\
    SOC_CR_DOCUMENT_API_NAME(unit);\
    SOC_CR_DISP_ERR_CHECK(soc_dcmn_cr_dispatcher_transaction_end(unit));\
    BCM_CR_TEST_TRANSACTION_END(unit)
#elif !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#define BCM_PRE_API_LOGIC_HOOK(unit)
#define BCM_POST_API_LOGIC_HOOK(unit)
#endif

/*************************************************************/



/******************* ERROR RECOVERY ***************************/
#if defined(BCM_DNX_SUPPORT)
#define DNXC_ERR_RECOVERY_TRANSACTION_START(_unit) DNX_ERR_RECOVERY_TRANSACTION_START(_unit)
#define DNXC_ERR_RECOVERY_TRANSACTION_END(_unit) DNX_ERR_RECOVERY_TRANSACTION_END(_unit)
#else
#define DNXC_ERR_RECOVERY_TRANSACTION_START(_unit)
#define DNXC_ERR_RECOVERY_TRANSACTION_END(_unit)
#endif
/**************************************************************/


/* API TRANSACTION START and API TRANSACTION END */

#if (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)) && ! defined (CRASH_RECOVERY_SUPPORT)
#define BCM_PRE_API_LOGIC_HOOK(unit)                 \
    DNXC_WARM_BOOT_API_TEST_FUNCTION_NAME_UPDATE(unit);\
    DNXC_WB_API_TEST_COUNTER_INCREASE(unit);           \
    DNXC_ERR_RECOVERY_TRANSACTION_START(unit);         \
    DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, __func__, MTA_FLAG_DISPATCHER, TRUE));\

#define BCM_POST_API_LOGIC_HOOK(unit)        \
    DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, __func__, MTA_FLAG_DISPATCHER, FALSE));\
    DNXC_ERR_RECOVERY_TRANSACTION_END(unit);                                                                \
    DNXC_WARM_BOOT_API_TEST_RESET(unit);                                                                    \
    DNXC_WB_API_TEST_COUNTER_DECREASE(unit)
#endif

#endif /* _BCM_INT_SWITCH_H_ */
