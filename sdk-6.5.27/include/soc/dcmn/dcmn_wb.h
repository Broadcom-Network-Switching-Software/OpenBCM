/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * SOC DCMN CMICD IPROC
 */

#ifndef _SOC_DCMN_WB_H_
#define _SOC_DCMN_WB_H_




#include <soc/defs.h>
#include <soc/drv.h>
#include <shared/bsl.h>

#include <soc/dcmn/dcmn_defs.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <appl/diag/shell.h>
#endif 





#define SOC_WB_API_RESET_ERR_CHECK(disp_rv)\
    if (disp_rv != SOC_E_NONE) {return SOC_E_INTERNAL;}


#ifdef BCM_WARM_BOOT_SUPPORT
#define WB_TEST(op) op
#else
#define WB_TEST(op)
#endif 

#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_NONE                          (0) 
#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API               (1) 
#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_END_OF_DVAPIS                 (2) 
#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY                (3) 
#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_ISSU_SYNC                     (4) 
#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_ISSU_DUMP                     (5) 



#define DCMN_WARM_EXP_PARAM 10

 
typedef enum _dcmn_wb_field_falgs_s {
    _dcmnWbFieldFlagDirectExtraction=0,
    _dcmnWbFieldFlagDataQual=1, 
    _dcmnWbFieldFlagNOF=2
} _dcmn_wb_field_falgs_e;


extern int    _soc_dcmn_wb_warmboot_test_mode[SOC_MAX_NUM_DEVICES];

extern int    _soc_dcmn_wb_override_wb_test[SOC_MAX_NUM_DEVICES];

extern int    _soc_dcmn_wb_disable_once_wb_test[SOC_MAX_NUM_DEVICES];






void soc_dcmn_wb_field_test_mode_set_or_suppress(int unit, _dcmn_wb_field_falgs_e type, int wb_flag);

#ifdef BCM_WARM_BOOT_SUPPORT
extern void soc_dcmn_wb_test_mode_set(int unit, int mode);
extern void soc_dcmn_wb_test_mode_get(int unit, int *mode);
extern void soc_dcmn_wb_default_dump_directory_set(int unit, char* directory);
extern void soc_dcmn_wb_default_dump_directory_get(int unit, char** directory);
extern void soc_dcmn_wb_issu_dump_directory_set(int unit, char* directory);
extern void soc_dcmn_wb_issu_dump_directory_get(int unit, char** directory);
extern void soc_dcmn_wb_no_wb_test_set(int unit, int wb_flag);
extern void soc_dcmn_wb_no_wb_test_get(int unit, int *wb_flag);
extern void soc_dcmn_wb_field_test_mode_set(int unit, _dcmn_wb_field_falgs_e type, int wb_flag);
extern void soc_dcmn_wb_field_test_mode_get(int unit, _dcmn_wb_field_falgs_e type, int *wb_flag);
extern void soc_dcmn_wb_disable_once_set(int unit, int wb_flag);
extern void soc_dcmn_wb_disable_once_get(int unit, int *wb_flag);
extern void soc_dcmn_wb_test_counter_set(int unit, int counter);
extern void soc_dcmn_wb_test_counter_get(int unit, int *counter);
extern void soc_dcmn_wb_test_counter_plus_plus(int unit);
extern void dcmn_bcm_warm_boot_api_test_enable_if_disabled_once(int unit);
extern void soc_dcmn_wb_test_counter_reset(int unit);
extern int  dcmn_bcm_warm_boot_api_test_mode_skip_wb_sequence(int unit);
extern int  dcmn_wb_exp_backoff(int test_counter, int *nof_skip);
extern int dcmn_wb_all_reset_test_utils_preconditions(int unit);
extern int dcmn_wb_common_api_preconditions(int unit);
extern int dcmn_wb_api_counter_increase(int unit);
extern int dcmn_wb_api_counter_decrease(int unit);
extern int dcmn_wb_api_counter_get(int unit);
extern int dcmn_wb_api_counter_reset(int unit);
extern int dcmn_wb_api_counter_is_top_level(int unit);
extern void soc_dcmn_wb_main_thread_set(int unit);
#endif 




#ifdef BCM_DPP_SUPPORT
#include <bcm_int/petra_dispatch.h>
#else 
#ifdef BCM_DFE_SUPPORT
#include <bcm_int/dfe_dispatch.h>
#endif 
#endif 

#define TR_141_COMMAND(is_cr) ((is_cr) ? "tr 141 w=1 NoSync=1 NoDump=1" : "tr 141 W=1 NoDump=2")

#ifdef BCM_WARM_BOOT_SUPPORT	
int dcmn_bcm_warm_boot_api_test_reset(int unit);


#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit)\
    dcmn_bcm_warm_boot_api_test_mode_skip_wb_sequence(unit)


 
 
 
#define _DCMN_BCM_WARM_BOOT_API_TEST_OVERRIDE_WB_TEST_MODE(unit)        \
    soc_dcmn_wb_no_wb_test_set(unit, 1)
#define _DCMN_BCM_WARM_BOOT_API_TEST_RETRACT_OVERRIDEN_WB_TEST_MODE(unit)\
    soc_dcmn_wb_no_wb_test_set(unit, 0)

#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE_NO_SANITY_CHECK(unit)        \
    dcmn_wb_api_counter_increase(unit);\
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);\
    dcmn_wb_api_counter_decrease(unit)

#define DCMN_WB_API_COUNTER_INCREASE(unit) \
    do { \
        dcmn_wb_api_counter_increase(unit); \
    } while(0)


#define DCMN_WB_API_COUNTER_DECREASE(unit) \
    do { \
        SOC_DISP_ERR_CHECK(dcmn_bcm_warm_boot_api_test_reset(unit)); \
        dcmn_wb_api_counter_decrease(unit); \
    } while(0)

#define DCMN_WB_DOCUMENT_API_NAME(unit)\
    if(SOC_UNIT_VALID(unit)){      \
    sal_strcpy(warmboot_api_function_name[unit],__FUNCTION__);                            \
    }                                   

#else 


#define _DCMN_BCM_WARM_BOOT_API_TEST_OVERRIDE_WB_TEST_MODE(unit)            do {} while(0)
#define _DCMN_BCM_WARM_BOOT_API_TEST_RETRACT_OVERRIDEN_WB_TEST_MODE(unit)   do {} while(0)

#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit)            do {} while(0)

#define _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE_NO_SANITY_CHECK(unit)        \
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit

#define DCMN_WB_API_COUNTER_INCREASE(unit)        do {} while(0)
#define DCMN_WB_API_COUNTER_DECREASE(unit)        do {} while(0)
#define DCMN_WB_DOCUMENT_API_NAME(unit)           do {} while(0)
#endif 

#define _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_DIR_EXT_WB_TEST_MODE_STOP(unit);      \
    soc_dcmn_wb_field_test_mode_set_or_suppress(unit, _dcmnWbFieldFlagDirectExtraction, 1)
#define _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_DIR_EXT_WB_TEST_MODE_CONTINUE(unit);      \
    soc_dcmn_wb_field_test_mode_set_or_suppress(unit, _dcmnWbFieldFlagDirectExtraction, 0)
#define _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_QUAL_WB_TEST_MODE_STOP(unit);     \
    soc_dcmn_wb_field_test_mode_set_or_suppress(unit, _dcmnWbFieldFlagDataQual, 1)
#define _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_QUAL_WB_TEST_MODE_CONTINUE(unit);     \
    soc_dcmn_wb_field_test_mode_set_or_suppress(unit, _dcmnWbFieldFlagDataQual, 0)

#define SOC_DISP_ERR_CHECK(disp_rv)\
    if (disp_rv != SOC_E_NONE) {return SOC_E_INTERNAL;}
    

#endif 

