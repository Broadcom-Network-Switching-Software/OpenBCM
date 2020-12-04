/** \file appl_ref_init_deinit.c
 * init and deinit functions to be used by the INIT_DNX command.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INITSEQDNX

/*
* INCLUDE FILES:
* {
*/

#include <shared/shrextend/shrextend_debug.h>
#include <appl/reference/dnxc/appl_ref_init_deinit.h>
#include <appl/reference/dnxc/appl_ref_init_utils.h>
#include <bcm/init.h>
#include <bcm/stack.h>
#include <bcm/fabric.h>
#include <soc/property.h>
#include <soc/drv.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include "appl_ref_init_deinit.h"
#include <appl/reference/dnxc/appl_ref_intr.h>
#include <appl/reference/dnxc/appl_ref_shell_threads.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_dev_init.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define APPL_DNXF_DEFAULT_MODID (5)
#define APPL_DNXF_DEFAULT_LINK_ISOLATE (0)

/*
 * }
 */

/*
 * MACROs:
 * {
 */

#define APPL_DNXF_DEVICE_PARAMS(unit)    (appl_dnxf_device_params[unit])

/*
 * }
 */

/*
 * Typedefs.
 * {  
 */

/**
 * \brief dnxf device parameters
 */
typedef struct appl_dnxf_device_params_s
{
    int modid;
    bcm_pbmp_t links_isolate_pbmp;      /* a bit per link denotes if to isolate this link */
} appl_dnxf_device_params_t;

/*
 * }
 */

/*
 * Globals.
 * {  
 */

appl_dnxf_device_params_t appl_dnxf_device_params[SOC_MAX_NUM_DEVICES];

/*
 * }
 */

/*
 *  \brief Parse Device Application Parameters
 */
static shr_error_e
appl_dnxf_device_params_init(
    int unit)
{
    int link;
    uint32 propval_num;
    SHR_FUNC_INIT_VARS(unit);

    APPL_DNXF_DEVICE_PARAMS(unit).modid =
        soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "module_id", APPL_DNXF_DEFAULT_MODID);
    LOG_INFO_EX(BSL_LOG_MODULE, "   module id = %d.\n%s%s%s", APPL_DNXF_DEVICE_PARAMS(unit).modid, EMPTY, EMPTY, EMPTY);

    BCM_PBMP_CLEAR(APPL_DNXF_DEVICE_PARAMS(unit).links_isolate_pbmp);
    PBMP_SFI_ITER(unit, link)
    {
        propval_num =
            soc_property_port_suffix_num_get(unit, link, -1, spn_APPL_PARAM, "link_isolate",
                                             APPL_DNXF_DEFAULT_LINK_ISOLATE);

        if (propval_num)
        {
            BCM_PBMP_PORT_ADD(APPL_DNXF_DEVICE_PARAMS(unit).links_isolate_pbmp, link);
        }
    }

    return SHR_ERR_TO_SHELL;
}

static shr_error_e
appl_dnxf_deinit_start(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * do nothing 
     */

    return SHR_ERR_TO_SHELL;
}

static shr_error_e
appl_dnxf_init_done(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Mark soc control as done
     */
    SOC_CONTROL(unit)->soc_flags |= SOC_F_INITED;
    SOC_CONTROL(unit)->soc_flags |= SOC_F_ALL_MODULES_INITED;

    return SHR_ERR_TO_SHELL;
}

/*
 *  \brief DNXF STK init.
 *          * Set Device Modid
 *          * Enable Traffic (Unisolate Device)
 */
static shr_error_e
appl_dnxf_ref_stk_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_stk_modid_set(unit, APPL_DNXF_DEVICE_PARAMS(unit).modid));
    SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, APPL_DNXF_DEVICE_PARAMS(unit).modid, -1, 0x1));

exit:
    return SHR_ERR_TO_SHELL;
}

/*
 *  \brief DNXF STK deinit.
 *          * Disable Traffic (Isolate Device)
 */
static shr_error_e
appl_dnxf_ref_stk_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Do Nothing 
     */

    return SHR_ERR_TO_SHELL;
}

/*
 *  \brief DNXF links isolate init.
 */
static shr_error_e
appl_dnxf_links_isolate_init(
    int unit)
{
    int link;
    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_ITER(APPL_DNXF_DEVICE_PARAMS(unit).links_isolate_pbmp, link)
    {
        SHR_IF_ERR_EXIT(bcm_fabric_link_control_set(unit, link, bcmFabricLinkIsolate, 1));
    }

exit:
    return SHR_ERR_TO_SHELL;
}

/*
 *  \brief DNXF links isolate deinit.
 */
static shr_error_e
appl_dnxf_links_isolate_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Do Nothing
     */

    return SHR_ERR_TO_SHELL;
}

/* *INDENT-OFF* */

/**
 * \brief Init-Deinit time thresholds
 *
 * This array is used for steps with APPL_INIT_ADVANCED_F_TIME_THRESH_EXPLICIT
 * flag in order to link the Step ID to the specific user defined time threshold.
 */
static const appl_init_time_thresh_info_t appl_dnxf_init_time_thresh_expl[] = {
  /*STEP_ID,                          TIME THRESHOLD */
  {APPL_DNXF_STEP_LAST_STEP,          0}
};

/**
 * \brief DNX reference applications sequence 
 *        * Each refernce application in the init sequence should be added here.
 *        * Each application can be enabled or disabled using a dedicated
 *          soc property which uses the "suffix" as an argument: 'appl_enable_<suffix>=0/1'
 *        * The last step must be all NULLs
 */
static const appl_dnxc_init_step_t appl_dnxf_ref_apps_steps[] = {
   /*STEP_ID                         NAME              SUFFIX          INIT_FUNC                        DEINIT_FUNC                    FLAGS_CB  STEP_FLAGS             SUB_LIST*/
    {APPL_DNXF_STEP_DEVICE_PARAMS,   "Device Params",  "params",       appl_dnxf_device_params_init,    NULL,                          NULL,     DEF_PROP_EN,           NULL},
    {APPL_DNXF_LINKS_ISOLATE,        "Links Isolate",  "links_isolate",appl_dnxf_links_isolate_init,    appl_dnxf_links_isolate_deinit,NULL,     DEF_PROP_EN | WB_SKIP, NULL},
    {APPL_DNXF_STEP_STK_INIT,        "STK Init",       "stk_init",     appl_dnxf_ref_stk_init,          appl_dnxf_ref_stk_deinit,      NULL,     DEF_PROP_EN | WB_SKIP, NULL},
    {APPL_DNXF_STEP_INTR_INIT,       "Interrupt Init", "intr_init",    appl_dnxc_interrupt_init,        appl_dnxc_interrupt_deinit,    NULL,     DEF_PROP_EN,           NULL},
    {APPL_DNXF_STEP_SHELL_THREADS,   "Shell threads",  "shell_threads",NULL,                            appl_dnxc_shell_threads_deinit,NULL,     DEF_PROP_EN,           NULL},
    {APPL_DNXF_STEP_INIT_DONE,       "Init Done",      "init_done",    appl_dnxf_init_done,             appl_dnxf_deinit_start,        NULL,     0,                     NULL},
    {APPL_DNXF_STEP_LAST_STEP,        NULL,             NULL,          NULL,                            NULL,                          NULL,     0,                     NULL} /* Last step must be all NULLs*/
};


/**
 * \brief DNX init sequence 
 *        * Each step in the init sequence should be added here
 *        * The init sequence has 2 main phases:
 *          1. Initialize SDK (create unit and init bcm)
 *          2. Run default reference applications - each application can be enabled or disabled using a dedicated
 *             soc property which uses the "suffix" as an argument: 'appl_enable_<suffix>=0/1'
 *        * The last step must be all NULLs
 */
static const appl_dnxc_init_step_t appl_dnxf_init_steps[] = {
    /*STEP_ID                    NAME            SUFFIX      INIT_FUNC             DEINIT_FUNC             FLAGS_CB STEP_FLAGS   SUB_LIST*/
    {APPL_DNXF_STEP_SDK,         "SDK",          "",         appl_dnxc_sdk_init,   appl_dnxc_sdk_deinit,   NULL,    STANDALONE | TIME_PER_DEV,  NULL},
    {APPL_DNXF_STEP_APPS,        "Applications", "",         NULL,                 NULL,                   NULL,    DEF_PROP_EN | TIME_PER_DEV, appl_dnxf_ref_apps_steps},
    {APPL_DNXF_STEP_LAST_STEP,   NULL,           NULL,       NULL,                 NULL,                   NULL,    0,                       NULL} /* Last step must be all NULLs*/
};

/* *INDENT-ON* */

/*
 * See .h file
 */
shr_error_e
appl_dnxf_step_list_get(
    int unit,
    const appl_dnxc_init_step_t ** appl_steps)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * return pointer to the list of DNXF application init steps
     */
    *appl_steps = appl_dnxf_init_steps;

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnxf_step_time_thresh_list_get(
    int unit,
    const appl_init_time_thresh_info_t ** time_thresh_list)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * return pointer to the list of DNXF application
     * init steps' time thresholds
     */
    *time_thresh_list = appl_dnxf_init_time_thresh_expl;

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnxf_per_dev_step_time_threshold_get(
    int unit,
    int step_id,
    sal_usecs_t * time_thresh)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the per-device threshold for specified step
     */
    *time_thresh = dnxf_data_dev_init.time.appl_step_thresh_get(unit, step_id)->value;;

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnxf_per_dev_full_time_threshold_get(
    int unit,
    sal_usecs_t * time_thresh)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the per-device threshold for specified step
     */
    *time_thresh = dnxf_data_dev_init.time.appl_init_total_thresh_get(unit);

    SHR_FUNC_EXIT;
}
