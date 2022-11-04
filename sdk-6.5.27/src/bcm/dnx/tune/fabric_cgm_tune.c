/** \file fabric_cgm_tune.c
 * Tuning of Fabric parameters
 * 
 * NOTE: as this code is invoked from iside SDK, 
 * API invocation is done via bcm_dnx_XXX functions.
 * When this code is taken outside SDK, 
 * these calls should be replaced by bcm_XXX functions.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
 * Include files.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <sal/types.h>
#include <bcm/fabric.h>
#include <bcm_int/dnx/auto_generated/dnx_fabric_dispatch.h>
#include <bcm_int/dnx/tdm/tdm.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tune.h>
#include "fabric_cgm_tune.h"
/*
 * }
 * Include files.
 */

/*
 * \brief
 *   Hold the params for bcm_fabric_cgm_control_set.
 */
typedef struct dnx_tune_fabric_cgm_control_s
{
    bcm_fabric_cgm_control_type_t control_type;
    bcm_fabric_cgm_control_id_t control_id;
    int value;
    uint32 flags;
} dnx_tune_fabric_cgm_control_t;

/**
 * \brief
 *   Tune Fabric CGM controls regarding RCI, which depends on number
 *   of pipes in the device.
 *   Since we limit unicast traffic to be only on pipe0, we need to
 *   disable other pipes threshold by setting it to maximum value.
 */
static shr_error_e
dnx_tune_fabric_cgm_rci_pipes_set(
    int unit)
{
    uint32 nof_pipes = dnx_data_fabric.pipes.nof_pipes_get(unit);
    uint32 egress_rci_threshold = 0;
    uint32 egress_rci_threshold_offset = 0;
    uint32 total_egress_rci_threshold = 0;
    uint32 total_egress_rci_threshold_offset = 0;
    uint32 max_link_rci_threshold = 0;
    uint32 max_egress_rci_threshold = 0;
    uint32 max_total_egress_rci_threshold = 0;
    bcm_fabric_cgm_control_type_t control_type;
    bcm_fabric_cgm_control_id_t control_id;
    bcm_fabric_level_t level = 0;
    int value;
    uint32 flags;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get maximum value for thresholds.
     * These values will be used to disable the thresholds.
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                    (unit, DBAL_TABLE_FABRIC_CGM_RCI_THRESHOLDS, DBAL_FIELD_LINK_RCI_THRESHOLD, FALSE, 0, 0,
                     DBAL_PREDEF_VAL_MAX_VALUE, &max_link_rci_threshold));

    /*
     * Configure bcmFabricCgmRciLinkPipeLevelTh thresholds
     */
    /** Disable thresholds for all pipes by setting them to max */
    control_type = bcmFabricCgmRciLinkPipeLevelTh;
    control_id = BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(bcmFabricPipeAll, bcmFabricLevelAll);
    value = max_link_rci_threshold;
    flags = 0;
    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set(unit, flags, control_type, control_id, value));

    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {
        /*
         * Get maximum value for thresholds.
         * These values will be used to disable the thresholds.
         */
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                        (unit, DBAL_TABLE_FABRIC_CGM_RCI_THRESHOLDS, DBAL_FIELD_EGRESS_RCI_THRESHOLD, FALSE, 0, 0,
                         DBAL_PREDEF_VAL_MAX_VALUE, &max_egress_rci_threshold));
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                        (unit, DBAL_TABLE_FABRIC_CGM_RCI_THRESHOLDS, DBAL_FIELD_TOTAL_EGRESS_RCI_THRESHOLD, FALSE, 0, 0,
                         DBAL_PREDEF_VAL_MAX_VALUE, &max_total_egress_rci_threshold));

        /*
         * Configure bcmFabricCgmRciEgressPipeLevelTh thresholds
         */
        /** Set threshold for pipe0 */
        control_type = bcmFabricCgmRciEgressPipeLevelTh;
        flags = 0;

        /*
         * Here we will force new values provided by AEs and ignore the default ones.
         */
        if (dnx_data_tune.fabric.feature_get(unit, dnx_data_tune_fabric_cgm_rci_update_egress_th_values))
        {
            egress_rci_threshold = dnx_data_tune.fabric.cgm_rci_egress_pipe_level_th_base_get(unit);
            egress_rci_threshold_offset = dnx_data_tune.fabric.cgm_rci_egress_pipe_level_th_offset_get(unit);
            for (level = bcmFabricLevel0; level < bcmFabricLevel3; ++level)
            {
                control_id = BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(bcmFabricPipe0, level);
                value = egress_rci_threshold / nof_pipes;
                SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set(unit, flags, control_type, control_id, value));
                egress_rci_threshold += egress_rci_threshold_offset;
            }
        }
        else
        {
            /** Value of thresholds is dependent on number of pipes in the device. */
            egress_rci_threshold = max_egress_rci_threshold / nof_pipes;
            for (level = bcmFabricLevel0; level < bcmFabricLevel3; ++level)
            {
                control_id = BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(bcmFabricPipe0, level);
                value = egress_rci_threshold * (3 * (level + 1)) / 10;
                SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set(unit, flags, control_type, control_id, value));
            }
        }

        if (nof_pipes > 1)
        {
            /** Disable threshold for pipe1 by setting it to max */
            control_type = bcmFabricCgmRciEgressPipeLevelTh;
            control_id = BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(bcmFabricPipe1, bcmFabricLevelAll);
            value = max_egress_rci_threshold;
            flags = 0;
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set(unit, flags, control_type, control_id, value));
        }

        if (nof_pipes > 2)
        {
            /** Disable threshold for pipe2 by setting it to max */
            control_type = bcmFabricCgmRciEgressPipeLevelTh;
            control_id = BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(bcmFabricPipe2, bcmFabricLevelAll);
            value = max_egress_rci_threshold;
            flags = 0;
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set(unit, flags, control_type, control_id, value));
        }

        /*
         * Configure bcmFabricCgmRciTotalEgressPipeLevelTh thresholds
         */
        /** Set threshold for pipe0 */
        control_type = bcmFabricCgmRciTotalEgressPipeLevelTh;
        flags = 0;

        /*
         * Here we will force new values provided by AEs and ignore the default ones.
         */
        if (dnx_data_tune.fabric.feature_get(unit, dnx_data_tune_fabric_cgm_rci_update_egress_th_values))
        {
            total_egress_rci_threshold = dnx_data_tune.fabric.cgm_rci_total_egress_pipe_level_th_base_get(unit);
            total_egress_rci_threshold_offset =
                dnx_data_tune.fabric.cgm_rci_total_egress_pipe_level_th_offset_get(unit);
            for (level = bcmFabricLevel0; level < bcmFabricLevel3; ++level)
            {
                control_id = BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(bcmFabricPipe0, level);
                value = total_egress_rci_threshold / nof_pipes;
                SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set(unit, flags, control_type, control_id, value));
                total_egress_rci_threshold += total_egress_rci_threshold_offset;
            }
        }
        else
        {
            /** Value of thresholds is dependent on number of pipes in the device. */
            total_egress_rci_threshold = max_total_egress_rci_threshold / nof_pipes;
            for (level = bcmFabricLevel0; level < bcmFabricLevel3; ++level)
            {
                control_id = BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(bcmFabricPipe0, level);
                value = total_egress_rci_threshold * (3 * (level + 1)) / 10;
                SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set(unit, flags, control_type, control_id, value));
            }
        }
        if (nof_pipes > 1)
        {
            /** Disable threshold for pipe1 by setting it to max */
            control_type = bcmFabricCgmRciTotalEgressPipeLevelTh;
            control_id = BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(bcmFabricPipe1, bcmFabricLevelAll);
            value = max_total_egress_rci_threshold;
            flags = 0;
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set(unit, flags, control_type, control_id, value));
        }

        if (nof_pipes > 2)
        {
            /** Disable threshold for pipe2 by setting it to max */
            control_type = bcmFabricCgmRciTotalEgressPipeLevelTh;
            control_id = BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(bcmFabricPipe2, bcmFabricLevelAll);
            value = max_total_egress_rci_threshold;
            flags = 0;
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set(unit, flags, control_type, control_id, value));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Common function to tune Fabric CGM controls
 */
static shr_error_e
dnx_tune_fabric_cgm_common_set(
    int unit,
    const dnx_tune_fabric_cgm_control_t * tune_fabric_cgm_control)
{
    uint32 flags = 0;
    bcm_fabric_cgm_control_type_t control_type = -1;
    bcm_fabric_cgm_control_id_t control_id = -1;
    int value = -1;
    int control = 0;
    SHR_FUNC_INIT_VARS(unit);

    while ((int) (tune_fabric_cgm_control[control].control_type) != -1)
    {
        control_type = tune_fabric_cgm_control[control].control_type;
        control_id = tune_fabric_cgm_control[control].control_id;
        flags = tune_fabric_cgm_control[control].flags;
        value = tune_fabric_cgm_control[control].value;

        SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set(unit, flags, control_type, control_id, value));

        ++control;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Tune additional default Fabric CGM controls params for RCI on
 *  multi-core devices.
 */
static shr_error_e
dnx_tune_fabric_cgm_rci_common_set(
    int unit)
{
    /**
     * Table with default Fabric CGM controls params for RCI.
     * Note that additional pipe specific RCI configurations can be found
     * in dnx_tune_fabric_cgm_rci_pipes_set.
     */
    dnx_tune_fabric_cgm_control_t dnx_tune_fabric_cgm_control_rci_common[] = {
        /* *INDENT-OFF* */
        /*
         * Control Type                           | Control ID             | Value | Flags
         */
        {bcmFabricCgmRciLocalLevelTh,               bcmFabricLevel0,         40,     0x0},
        {bcmFabricCgmRciLocalLevelTh,               bcmFabricLevel1,         60,     0x0},
        {bcmFabricCgmRciLocalLevelTh,               bcmFabricLevel2,         80,     0x0},
        {bcmFabricCgmRciLinkLevelFactor,            bcmFabricLevel0,         4,      0x0},
        {bcmFabricCgmRciLinkLevelFactor,            bcmFabricLevel1,         5,      0x0},
        {bcmFabricCgmRciLinkLevelFactor,            bcmFabricLevel2,         6,      0x0},
        {bcmFabricCgmRciHighSeverityMinLinksParam,  -1,                      dnx_data_tune.fabric.cgm_rci_high_sev_min_links_param_get(unit),     0x0},
        {bcmFabricCgmRciHighSeverityFactor,         -1,                      1000,   0x0},
        {bcmFabricCgmRciDeviceLevelMappingTh,       bcmFabricLevel0,         200,    0x0},
        {bcmFabricCgmRciDeviceLevelMappingTh,       bcmFabricLevel1,         300,    0x0},
        {bcmFabricCgmRciDeviceLevelMappingTh,       bcmFabricLevel2,         400,    0x0},
        {bcmFabricCgmRciDeviceLevelMappingTh,       bcmFabricLevel3,         500,    0x0},
        {bcmFabricCgmRciDeviceLevelMappingTh,       bcmFabricLevel4,         600,    0x0},
        {bcmFabricCgmRciDeviceLevelMappingTh,       bcmFabricLevel5,         700,    0x0},
        {bcmFabricCgmRciDeviceLevelMappingTh,       bcmFabricLevel6,         800,    0x0},
        {-1}
        /* *INDENT-ON* */
    };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_common_set(unit, dnx_tune_fabric_cgm_control_rci_common));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Tune additional default Fabric CGM controls params for RCI on
 *  multi-core devices.
 */
static shr_error_e
dnx_tune_fabric_cgm_rci_multi_core_set(
    int unit)
{
    /**
     * Table with additional default Fabric CGM controls params for RCI on
     * multi-core devices.
     * Note that these configurations are in addition to the configurations
     * in dnx_tune_fabric_cgm_control_rci_common.
     * Note that additional pipe specific RCI configurations can be found
     * in dnx_tune_fabric_cgm_rci_pipes_set.
     */
    dnx_tune_fabric_cgm_control_t dnx_tune_fabric_cgm_control_rci_multi_core[] = {
        /* *INDENT-OFF* */
        /*
         * Control Type                           | Control ID             | Value | Flags
         */
        {bcmFabricCgmRciTotalLocalLevelTh,          bcmFabricLevel0,         80,     0x0},
        {bcmFabricCgmRciTotalLocalLevelTh,          bcmFabricLevel1,         100,    0x0},
        {bcmFabricCgmRciTotalLocalLevelTh,          bcmFabricLevel2,         120,    0x0},
        {bcmFabricCgmRciEgressLevelFactor,          bcmFabricLevel0,         dnx_data_tune.fabric.cgm_rci_egress_level_factor_get(unit, bcmFabricLevel0)->score,    0x0},
        {bcmFabricCgmRciEgressLevelFactor,          bcmFabricLevel1,         dnx_data_tune.fabric.cgm_rci_egress_level_factor_get(unit, bcmFabricLevel1)->score,    0x0},
        {bcmFabricCgmRciEgressLevelFactor,          bcmFabricLevel2,         dnx_data_tune.fabric.cgm_rci_egress_level_factor_get(unit, bcmFabricLevel2)->score,    0x0},
        {bcmFabricCgmRciCoreLevelMappingTh,         bcmFabricLevel0,         200,    0x0},
        {bcmFabricCgmRciCoreLevelMappingTh,         bcmFabricLevel1,         300,    0x0},
        {bcmFabricCgmRciCoreLevelMappingTh,         bcmFabricLevel2,         400,    0x0},
        {bcmFabricCgmRciCoreLevelMappingTh,         bcmFabricLevel3,         500,    0x0},
        {bcmFabricCgmRciCoreLevelMappingTh,         bcmFabricLevel4,         600,    0x0},
        {bcmFabricCgmRciCoreLevelMappingTh,         bcmFabricLevel5,         700,    0x0},
        {bcmFabricCgmRciCoreLevelMappingTh,         bcmFabricLevel6,         800,    0x0},
        {-1}
        /* *INDENT-ON* */
    };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_common_set(unit, dnx_tune_fabric_cgm_control_rci_multi_core));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Tune LLFC and Drop Fabric CGM controls
 */
static shr_error_e
dnx_tune_fabric_cgm_llfc_and_drop_set(
    int unit)
{
    uint32 nof_pipes = dnx_data_fabric.pipes.nof_pipes_get(unit);
    /*
     * Table with default Fabric CGM controls params for LLFC and Drop thresholds
     */
    dnx_tune_fabric_cgm_control_t dnx_tune_fabric_cgm_control_llfc_and_drop[] = {
        /* *INDENT-OFF* */
        /*
         * Control Type                        | Control ID                                                                  | Value                                                    | Flags
         */
        {bcmFabricCgmLlfcPipeTh,                 bcmFabricPipeAll,                                                           dnx_data_tune.fabric.cgm_llfc_pipe_th_get(unit),           0x0},
        {bcmFabricCgmDropFabricClassTh,          BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmFabricCastUC, bcmFabricPriorityAll),   dnx_data_tune.fabric.cgm_drop_fabric_class_th_get(unit),   0x0},
        {bcmFabricCgmDropFabricClassTh,          BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmFabricCastMC, bcmFabricPriority0),     dnx_data_tune.fabric.cgm_drop_fabric_multicast_th_get(unit, bcmFabricPriority0)->threshold / nof_pipes,   0x0},
        {bcmFabricCgmDropFabricClassTh,          BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmFabricCastMC, bcmFabricPriority1),     dnx_data_tune.fabric.cgm_drop_fabric_multicast_th_get(unit, bcmFabricPriority1)->threshold / nof_pipes,   0x0},
        {bcmFabricCgmDropFabricClassTh,          BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmFabricCastMC, bcmFabricPriority2),     dnx_data_tune.fabric.cgm_drop_fabric_multicast_th_get(unit, bcmFabricPriority2)->threshold / nof_pipes,   0x0},
        {bcmFabricCgmDropFabricClassTh,          BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmFabricCastMC, bcmFabricPriority3),     dnx_data_tune.fabric.cgm_drop_fabric_multicast_th_get(unit, bcmFabricPriority3)->threshold / nof_pipes,   0x0},
        {bcmFabricCgmDropMeshMcPriorityTh,       bcmFabricPriorityAll,                                                       128,                                                       0x0},
        {bcmFabricCgmFcIngressTh,                -1,                                                                         4,                                                         0x0},
        {-1}
        /* *INDENT-ON* */
    };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_common_set(unit, dnx_tune_fabric_cgm_control_llfc_and_drop));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Tune Egress Drop Fabric CGM controls
 */
static shr_error_e
dnx_tune_fabric_cgm_egress_drop_set(
    int unit)
{
    /*
     * Table with default Fabric CGM controls params for Egress Drop TDM thresholds
     */
    dnx_tune_fabric_cgm_control_t dnx_tune_fabric_cgm_control_egress_drop_tdm[] = {
        /* *INDENT-OFF* */
        /*
         * Control Type                        | Control ID | Value  | Flags
         */
        {bcmFabricCgmEgressDropTdmLatencyEn,     -1,          0,       0x0},
        {bcmFabricCgmEgressDropTdmLatencyTh,     -1,          13652,   0x0},
        {-1}
        /* *INDENT-ON* */
    };
    /*
     * Table with default Fabric CGM controls params for Egress Drop mc low priority thresholds
     */
    dnx_tune_fabric_cgm_control_t dnx_tune_fabric_cgm_control_egress_drop_mc_low_prio[] = {
        /* *INDENT-OFF* */
        /*
         * Control Type                        | Control ID         | Value                  | Flags
         */
        {bcmFabricCgmEgressDropMcLowPrioEn,      -1,                  0,                       0x0},
        {bcmFabricCgmEgressDropMcLowPrioTh,      -1,                  376,                     0x0},
        {bcmFabricCgmEgressDropMcLowPrioSelect,  -1,                  bcmFabricPriority0,      0x0},
        {-1}
        /* *INDENT-ON* */
    };
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.cgm.feature_get(unit, dnx_data_fabric_cgm_is_egress_drop_tdm_latency_supported))
    {
        SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_common_set(unit, dnx_tune_fabric_cgm_control_egress_drop_tdm));
    }

    if (dnx_data_fabric.cgm.feature_get(unit, dnx_data_fabric_cgm_egress_drop_low_prio_mc))
    {
        SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_common_set(unit, dnx_tune_fabric_cgm_control_egress_drop_mc_low_prio));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Tune common CGM controls for GCI
 */
static shr_error_e
dnx_tune_fabric_cgm_gci_common_set(
    int unit)
{
    /*
     * Table with default Fabric CGM controls params for GCI
     */
    dnx_tune_fabric_cgm_control_t dnx_tune_fabric_cgm_control_gci[] = {
        /* *INDENT-OFF* */
        /*
         * Control Type                               | Control ID                | Value | Flags
         */
        {bcmFabricCgmGciLeakyBucketEn,                  -1,                         1,      0x0},
        {bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh,   bcmFabricLeakyBucketAll,    128,    0x0},
        {bcmFabricCgmGciLeakyBucketFCLeakyBucketTh,     bcmFabricLeakyBucketAll,    4,      0x0},
        {bcmFabricCgmGciBackoffEn,                      -1,                         1,      0x0},
        {bcmFabricCgmGciBackoffLevelTh,                 bcmFabricLevel0,            256,    0x0},
        {bcmFabricCgmGciBackoffLevelTh,                 bcmFabricLevel1,            512,    0x0},
        {bcmFabricCgmGciBackoffLevelTh,                 bcmFabricLevel2,            768,    0x0},
        {-1}
        /* *INDENT-ON* */
    };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_common_set(unit, dnx_tune_fabric_cgm_control_gci));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Tune CGM Slow Start controls
 */
static shr_error_e
dnx_tune_fabric_cgm_slow_start_set(
    int unit)
{
    /*
     * Table with default Fabric CGM controls params for Slow Start
     */
    dnx_tune_fabric_cgm_control_t dnx_tune_fabric_cgm_control_slow_start[] = {
        /* *INDENT-OFF* */
        /*
         * Control Type                            | Control ID                                                                              | Value | Flags
         */
        {bcmFabricCgmSlowStartEn,                    bcmFabricMcShaperAll,                                                                     0,      0x0},
        {bcmFabricCgmSlowStartRateShaperPhaseParam,  BCM_FABRIC_TH_INDEX_FMC_SLOW_START_PHASE_SET(bcmFabricMcShaperAll, bcmFabricPhaseAll),    100,    0x0},
        {-1}
        /* *INDENT-ON* */
    };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_common_set(unit, dnx_tune_fabric_cgm_control_slow_start));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Tune CGM Egress dynamic WFQ controls
 */
static shr_error_e
dnx_tune_fabric_cgm_egress_dynamic_wfq_common_set(
    int unit)
{
    /*
     * Table with default Fabric CGM controls params for Egress Dynamic WFQ
     */
    dnx_tune_fabric_cgm_control_t dnx_tune_fabric_cgm_control_egress_dynamic_wfq[] = {
        /* *INDENT-OFF* */
        /*
         * Control Type                   | Control ID | Value | Flags
         */
        {bcmFabricCgmEgressDynamicWfqTh,    -1,          160,    0x0},
        {-1}
        /* *INDENT-ON* */
    };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_common_set(unit, dnx_tune_fabric_cgm_control_egress_dynamic_wfq));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Tune CGM Static Shaper common controls
 */
static shr_error_e
dnx_tune_fabric_cgm_static_shaper_common_set(
    int unit)
{
    /*
     * Table with default Fabric CGM controls params for Static Shaper
     */
    dnx_tune_fabric_cgm_control_t dnx_tune_fabric_cgm_control_static_shaper[] = {
        /* *INDENT-OFF* */
        /*
         * Control Type                        | Control ID | Value                                  | Flags
         */
        {bcmFabricCgmLciShaperMode,              -1,          BCM_FABRIC_CGM_LCI_SHAPER_MODE_DYNAMIC,  0x0},
        {bcmFabricCgmLciShaperStaticRateInGbps,  -1,          1200,                                    0x0},
        {-1}
        /* *INDENT-ON* */
    };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_common_set(unit, dnx_tune_fabric_cgm_control_static_shaper));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Tune FC between FDT(DQTs) and IPT(PDQs)
 */
static shr_error_e
dnx_tune_fabric_cgm_dtqs_fc_thresholds_set(
    int unit)
{

    dnx_tune_fabric_cgm_control_t dnx_tune_fabric_cgm_dtqs_fc_config[] = {
        /* *INDENT-OFF* */
        /*
         * Control Type                 | Control ID         | Value                                                                                     | Flags
         */

        /** Set FC indications*/
        /** Total Non OCB traffic*/
        {bcmFabricCgmFcSetTotalTh,        bcmFabricLevel0,   dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 0)->set_total_th,          0x0},
        {bcmFabricCgmFcSetTotalTh,        bcmFabricLevel1,   dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 1)->set_total_th,          0x0},
        {bcmFabricCgmFcSetTotalTh,        bcmFabricLevel2,   dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 2)->set_total_th,          0x0},
        {bcmFabricCgmFcSetTotalTh,        bcmFabricLevel3,   dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 3)->set_total_th,          0x0},
        /** Total OCB traffic*/
        {bcmFabricCgmFcSetTotalOcbTh,     bcmFabricLevel0,   dnx_data_fabric.transmit.dtq_to_pdq_ocb_thresholds_get(unit, 0)->set_total_th,              0x0},
        {bcmFabricCgmFcSetTotalOcbTh,     bcmFabricLevel1,   dnx_data_fabric.transmit.dtq_to_pdq_ocb_thresholds_get(unit, 1)->set_total_th,              0x0},
        /** Non OCB Local only traffic*/
        {bcmFabricCgmFcSetLocalTh,        bcmFabricLevel0,   dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 0)->set_total_local_th,    0x0},
        {bcmFabricCgmFcSetLocalTh,        bcmFabricLevel1,   dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 1)->set_total_local_th,    0x0},
        {bcmFabricCgmFcSetLocalTh,        bcmFabricLevel2,   dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 2)->set_total_local_th,    0x0},
        {bcmFabricCgmFcSetLocalTh,        bcmFabricLevel3,   dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 3)->set_total_local_th,    0x0},
        /** Non OCB Fabriconly traffic*/
        {bcmFabricCgmFcSetFabricTh,       bcmFabricLevel0,   dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 0)->set_total_fabric_th,   0x0},
        {bcmFabricCgmFcSetFabricTh,       bcmFabricLevel1,   dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 1)->set_total_fabric_th,   0x0},
        {bcmFabricCgmFcSetFabricTh,       bcmFabricLevel2,   dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 2)->set_total_fabric_th,   0x0},
        {bcmFabricCgmFcSetFabricTh,       bcmFabricLevel3,   dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 3)->set_total_fabric_th,   0x0},

        /** Clear FC indications*/
        /** DTQ0 */
        {bcmFabricCgmFcClearFabricTh,     BCM_FABRIC_TH_INDEX_CAST_LEVEL_SET(bcmFabricCastUC,bcmFabricLevel0), dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 0)->mask_dtq_0_th,  0x0},
        {bcmFabricCgmFcClearFabricTh,     BCM_FABRIC_TH_INDEX_CAST_LEVEL_SET(bcmFabricCastUC,bcmFabricLevel1), dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 1)->mask_dtq_0_th,  0x0},
        {bcmFabricCgmFcClearFabricTh,     BCM_FABRIC_TH_INDEX_CAST_LEVEL_SET(bcmFabricCastUC,bcmFabricLevel2), dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 2)->mask_dtq_0_th,  0x0},
        {bcmFabricCgmFcClearFabricTh,     BCM_FABRIC_TH_INDEX_CAST_LEVEL_SET(bcmFabricCastUC,bcmFabricLevel3), dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 3)->mask_dtq_0_th,  0x0},

        {bcmFabricCgmFcClearFabricOcbTh,  BCM_FABRIC_TH_INDEX_CAST_LEVEL_SET(bcmFabricCastUC,bcmFabricLevel0), dnx_data_fabric.transmit.dtq_to_pdq_ocb_thresholds_get(unit, 0)->mask_dtq_0_th,      0x0},
        {bcmFabricCgmFcClearFabricOcbTh,  BCM_FABRIC_TH_INDEX_CAST_LEVEL_SET(bcmFabricCastUC,bcmFabricLevel1), dnx_data_fabric.transmit.dtq_to_pdq_ocb_thresholds_get(unit, 1)->mask_dtq_0_th,      0x0},
        /** DTQ1 */
        {bcmFabricCgmFcClearFabricTh,     BCM_FABRIC_TH_INDEX_CAST_LEVEL_SET(bcmFabricCastMC,bcmFabricLevel0), dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 0)->mask_dtq_1_th,  0x0},
        {bcmFabricCgmFcClearFabricTh,     BCM_FABRIC_TH_INDEX_CAST_LEVEL_SET(bcmFabricCastMC,bcmFabricLevel1), dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 1)->mask_dtq_1_th,  0x0},
        {bcmFabricCgmFcClearFabricTh,     BCM_FABRIC_TH_INDEX_CAST_LEVEL_SET(bcmFabricCastMC,bcmFabricLevel2), dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 2)->mask_dtq_1_th,  0x0},
        {bcmFabricCgmFcClearFabricTh,     BCM_FABRIC_TH_INDEX_CAST_LEVEL_SET(bcmFabricCastMC,bcmFabricLevel3), dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 3)->mask_dtq_1_th,  0x0},

        {bcmFabricCgmFcClearFabricOcbTh,  BCM_FABRIC_TH_INDEX_CAST_LEVEL_SET(bcmFabricCastMC,bcmFabricLevel0), dnx_data_fabric.transmit.dtq_to_pdq_ocb_thresholds_get(unit, 0)->mask_dtq_1_th,      0x0},
        {bcmFabricCgmFcClearFabricOcbTh,  BCM_FABRIC_TH_INDEX_CAST_LEVEL_SET(bcmFabricCastMC,bcmFabricLevel1), dnx_data_fabric.transmit.dtq_to_pdq_ocb_thresholds_get(unit, 1)->mask_dtq_1_th,      0x0},
        /** Local traffic */
        {bcmFabricCgmFcClearLocalTh,      bcmFabricLevel0,   dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 0)->mask_local_th,  0x0},
        {bcmFabricCgmFcClearLocalTh,      bcmFabricLevel1,   dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 1)->mask_local_th,  0x0},
        {bcmFabricCgmFcClearLocalTh,      bcmFabricLevel2,   dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 2)->mask_local_th,  0x0},
        {bcmFabricCgmFcClearLocalTh,      bcmFabricLevel3,   dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get(unit, 3)->mask_local_th,  0x0},

        {bcmFabricCgmFcClearLocalOcbTh,   bcmFabricLevel0,   dnx_data_fabric.transmit.dtq_to_pdq_ocb_thresholds_get(unit, 0)->mask_local_th,      0x0},
        {bcmFabricCgmFcClearLocalOcbTh,   bcmFabricLevel1,   dnx_data_fabric.transmit.dtq_to_pdq_ocb_thresholds_get(unit, 1)->mask_local_th,      0x0},
        {-1}
        /* *INDENT-ON* */
    };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_common_set(unit, dnx_tune_fabric_cgm_dtqs_fc_config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Tune Fabric CGM controls
 */
shr_error_e
dnx_tune_fabric_cgm_set(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Tune RCI */
    SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_rci_pipes_set(unit));
    SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_rci_common_set(unit));
    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {
        SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_rci_multi_core_set(unit));
    }

    /** Tune GCI */
    SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_gci_common_set(unit));

    /** Tune Slow Start */
    SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_slow_start_set(unit));

    /** Tune LLFC and Drop */
    SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_llfc_and_drop_set(unit));

    /** Tune Egress Drop */
    SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_egress_drop_set(unit));

    /** Tune Egress Dynamic WFQ */
    SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_egress_dynamic_wfq_common_set(unit));

    if (dnx_data_fabric.cgm.feature_get(unit, dnx_data_fabric_cgm_is_static_shaper_supported))
    {
        /** Tune Static Shaper */
        SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_static_shaper_common_set(unit));
    }

    if (dnx_data_fabric.transmit.feature_get(unit, dnx_data_fabric_transmit_dtq_fc_torwards_pdq))
    {
        /** Tune FC between FDT(DQTs) and IPT(PDQs) */
        SHR_IF_ERR_EXIT(dnx_tune_fabric_cgm_dtqs_fc_thresholds_set(unit));
    }

exit:
    SHR_FUNC_EXIT;
}
