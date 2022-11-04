/** \file appl_ref_init_deinit.c
 * init and deinit functions to be used by the INIT_DNX command.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INITSEQDNX

/*
* INCLUDE FILES:
* {
*/

#include <soc/sand/shrextend/shrextend_debug.h>
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
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxc/dnxc_verify.h>

#ifdef BCM_ACCESS_SUPPORT
#include <bcm/rx.h>
#include <soc/access/pkt_rx.h>
#include <soc/access/pkt_dma.h>
#endif

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

    /*
     * Set verification control for application layer per soc property
     * Current design is constrcuted to init verification at the beggining of each module (BCM or APPL)
     * and enable it (assuiming it can be disabled) at the last stage.
     * (Assumptions appl_dnxf_device_params_init is first application step and appl_dnxf_init_done is last appl step) 
     */
    dnxc_verify_allowed_init(unit);

    APPL_DNXF_DEVICE_PARAMS(unit).modid =
        soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "module_id", APPL_DNXF_DEFAULT_MODID);

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

    return (shr_error_e) SHR_ERR_TO_SHELL;
}

static shr_error_e
appl_dnxf_deinit_start(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * do nothing 
     */

    return (shr_error_e) SHR_ERR_TO_SHELL;
}

static shr_error_e
appl_dnxf_init_done(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Mark soc control as done
     */
    SOC_FLAGS_SET(unit, SOC_FLAGS_GET(unit) | (SOC_F_INITED | SOC_F_ALL_MODULES_INITED));
    /*
     * Enable verification after init and applications are done.
     * That way we include also the tuning and the additional applications
     * beside the BCM init itself in the assumptions that the values
     * provided are correct and should pass the verification
     */
    dnxc_verify_allowed_set(unit, 1);

    return (shr_error_e) SHR_ERR_TO_SHELL;
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
    return (shr_error_e) SHR_ERR_TO_SHELL;
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

    return (shr_error_e) SHR_ERR_TO_SHELL;
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
    return (shr_error_e) SHR_ERR_TO_SHELL;
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

    return (shr_error_e) SHR_ERR_TO_SHELL;
}

#ifdef BCM_ACCESS_SUPPORT
/*
 *  \brief DNXF Packet DMA RX init.
 */
static shr_error_e
appl_dnxf_rx_init(
    int unit)
{
    bcm_rx_cfg_t cfg;
    int cosq, dma_channel, table_idx;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Application should be called only if packet DMA is supported for the device
     */
    if (!dnxf_data_fabric.rxtx.feature_get(unit, dnxf_data_fabric_rxtx_pkt_dma_supported))
    {
        SHR_EXIT();
    }

    if (bcm_rx_active(unit))
    {
        SHR_EXIT();
    }

    
    if (dnxf_data_fabric.rxtx.feature_get(unit, dnxf_data_fabric_rxtx_pkt_dma_supported))
    {
        SHR_EXIT();
    }

    /*
     * Initialize Dune RX Configuration Structure
     */
    bcm_rx_cfg_t_init(&cfg);

    /*
     * the size of RX pool = units# * pkt_size * pkts_per_chain * chains# * dma channels#, it must be less than the DMA buffer size.
     * pkt_size is 16K
     */
    cfg.pkt_size = ACCESS_RX_PKT_SIZE_DFLT;
    /*
     * Disable RX rate limitation
     */
    cfg.global_pps = 0;
    cfg.max_burst = 0;
    cfg.flags = 0;
    /*
     * the size of RX pool = units# * pkt_size * pkts_per_chain * chains# * dma channels#, it must be less than the DMA buffer size.
     * pkts_per_chain is 4
     */
    cfg.pkts_per_chain = 4;

    /*
     * Enable DMA cos on DMA channels per application
     */
    for (table_idx = 0; table_idx < dnxf_data_fabric.rxtx.rx_dma_channel_info_info_get(unit)->key_size[0]; table_idx++)
    {
        dma_channel = dnxf_data_fabric.rxtx.rx_dma_channel_info_get(unit, table_idx)->dma_channel;

        if (dma_channel != -1)
        {
            /*
             * the size of RX pool = units# * pkt_size * pkts_per_chain * chains# * dma channels#, it must be less than the DMA buffer size.
             * chains# is 2
             * dma channels# is 8
             */
            cfg.chan_cfg[dma_channel].chains = 2;
            cfg.chan_cfg[dma_channel].rate_pps = 0;
            cfg.chan_cfg[dma_channel].flags = 0;

            /* *INDENT-OFF* */
            /*
             *
             * Default distribution of application to cos to dma channel is like this
             * Application                   |  COS    |    DMA channels
             * Packets/Self routed cells        0-15        8
             * F1588                            16-30       9
             * Reserved                         31          10
             * Source routed cells              32          11
             * Mirror                           33          12
             * Events                           34          13
             */
            /* *INDENT-ON* */

            BCM_PBMP_ITER(dnxf_data_fabric.rxtx.rx_dma_channel_info_get(unit, table_idx)->cos, cosq)
            {
                SHR_IF_ERR_EXIT(bcm_rx_queue_channel_set(unit, cosq, dma_channel));
            }
        }
    }

    /*
     * Start RX
     */
    SHR_IF_ERR_EXIT(bcm_rx_start(unit, &cfg));

exit:
    return (shr_error_e) SHR_ERR_TO_SHELL;
}

/*
 *  \brief DNXF Packet DMA RX  deinit.
 */
static shr_error_e
appl_dnxf_rx_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Stop RX
     */
    SHR_IF_ERR_EXIT(bcm_rx_stop(unit, NULL));

exit:
    return (shr_error_e) SHR_ERR_TO_SHELL;
}
#endif

/* *INDENT-OFF* */

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
#ifdef BCM_ACCESS_SUPPORT
    {APPL_DNXF_STEP_RX,              "RX",             "RX",           appl_dnxf_rx_init,               appl_dnxf_rx_deinit,           NULL,     DEF_PROP_EN,           NULL},
#endif
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
appl_dnxf_per_dev_step_time_threshold_get(
    int unit,
    int step_id,
    utilex_step_time_dnx_data * step_id_dnx_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the per-device threshold for specified step
     */
    step_id_dnx_data->step_time = dnxf_data_dev_init.time.appl_step_time_get(unit, step_id)->time;
    step_id_dnx_data->down_deviation = dnxf_data_dev_init.time.appl_step_time_get(unit, step_id)->down_deviation;
    step_id_dnx_data->up_deviation = dnxf_data_dev_init.time.appl_step_time_get(unit, step_id)->up_deviation;
    step_id_dnx_data->test_sensitivity = dnxf_data_dev_init.time.test_sensitivity_get(unit);
    step_id_dnx_data->non_perc_up_deviation =
        dnxf_data_dev_init.time.appl_step_time_get(unit, step_id)->non_perc_up_deviation;

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnxf_per_dev_full_time_threshold_get(
    int unit,
    utilex_step_time_dnx_data * step_id_dnx_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the per-device threshold for specified step
     */
    step_id_dnx_data->step_time = dnxf_data_dev_init.time.appl_init_total_time_get(unit);
    step_id_dnx_data->down_deviation = dnxf_data_dev_init.time.down_deviation_total_get(unit);
    step_id_dnx_data->up_deviation = dnxf_data_dev_init.time.up_deviation_total_get(unit);
    step_id_dnx_data->test_sensitivity = dnxf_data_dev_init.time.test_sensitivity_get(unit);

    SHR_FUNC_EXIT;
}
