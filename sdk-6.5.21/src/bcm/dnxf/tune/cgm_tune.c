 /** \file cgm_tune.c
 * Tuning of Ingress Congestion parameters
 * 
 * NOTE: as this code is invoked from iside SDK, 
 * API invocation is done via bcm_dnx_XXX functions.
 * When this code is taken outside SDK, 
 * these calls should be replaced by bcm_XXX functions.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/fabric.h>
#include <bcm/types.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnxf_dispatch.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_port.h>

#include <soc/error.h>

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/cmn/mbcm.h>

#include <soc/dnxf/ramon/ramon_cgm.h>

#include "cgm_tune.h"

/*
 * }
 * Include files.
 */

#define BCM_DNXF_CGM_TUNE_PROFILE_0              (0)
#define BCM_DNXF_CGM_TUNE_TH_DISABLE             (-1)
#define BCM_DNXF_CGM_TUNE_NUM_OF_RCI_LEVELS      (4)
#define BCM_DNXF_CGM_MAX_NOF_ROW_ELEMENTS        (4)
typedef struct threshold_descriptor_s
{
    bcm_fabric_threshold_type_t threshold_type; 
    bcm_fabric_threshold_id_t threshold_id;
    int  value;
    uint32 flags;
} threshold_descriptor_t;

/*
* { Single pipe mode
*/

/* Table with default CGM threshold values for Ramon in "single pipe" mode*/
static const threshold_descriptor_t cgm_init_thresholds_single_pipe_mode[] =
{
    /* Threshold Type                                  | Threshold ID                                                                  | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DCH / RX*/ {bcmFabricRxFragDropPipeTh,               BCM_FABRIC_PIPE_ALL,                                                            764,                              0x0},
             {bcmFabricRxMcLowPrioDropPipeTh,          BCM_FABRIC_PIPE_ALL,                                                            120,                              0x0},
             {bcmFabricRxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricRxLLFCFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            384,                              0x0},
/*DTM / MID*/{bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                1700,                             0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                2100,                             0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                2600,                             0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                4000,                             0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidFragDropClassTh,             BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastAll, BCM_FABRIC_PRIO_ALL),             BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     2000,                             0x0},
             {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     2200,                             0x0},
             {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     3050,                             0x0},
             {bcmFabricMidFragGciPipeTh,               BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     2420,                             0x0},
             {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     2620,                             0x0},
             {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     2830,                             0x0},
             {bcmFabricMidFragRciPipeTh,               BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidMcCopiesFcPrioTh,            BCM_FABRIC_PRIO_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
/*DTL / TX*/ {bcmFabricTxLinkLoadDropPipeTh,           BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragGuaranteedPipeTh,         BCM_FABRIC_PIPE_ALL,                                                            768,                              0x0},
             {bcmFabricTxFE1BypassLLFCFcPipeTh,        BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     480,                              0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     510,                              0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     540,                              0x0},
             {bcmFabricTxLinkLoadRciPipeTh,            BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                1700,                             0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                2100,                             0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                2600,                             0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                4000,                             0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastAll, BCM_FABRIC_PRIO_ALL),             BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     2000,                             0x0},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     2200,                             0x0},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     3000,                             0x0},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     2400,                             0x0},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     2600,                             0x0},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     2800,                             0x0},
             {bcmFabricTxFragRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagFcPipeTh,                  BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidTagFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidFragFcPipeTh,                BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
/*DFL / SHR*/{bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                6250,                             0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                7800,                             0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                9750,                             0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                14840,                            0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedMcCopiesDropPrioTh,       BCM_FABRIC_PRIO_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     7400,                             0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     8180,                             0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     11300,                            0x0},
             {bcmFabricSharedFragCopiesGciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     8950,                             0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     9750,                             0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     10500,                            0x0},
             {bcmFabricSharedBankMcDropPrioTh,         BCM_FABRIC_PRIO_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0}
};

/* 
 *  Table with CGM threshold values for Ramon in mode with one pipe and LR enable which should overwrite or add to the default tuning values
 */
static const threshold_descriptor_t cgm_init_thresholds_single_pipe_mode_lr[] =
{
    /* Threshold Type                                  | Threshold ID                                                                  | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DTL / TX*/ {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                850,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                850,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                1050,                             BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                1050,                             BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                1300,                             BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                1300,                             BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                2000,                             BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                2000,                             BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxFragDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastAll, BCM_FABRIC_PRIO_ALL),             BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxFragDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastAll, BCM_FABRIC_PRIO_ALL),             BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     1000,                             BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     1000,                             BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     1100,                             BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     1100,                             BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     1500,                             BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     1500,                             BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     1200,                             BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     1200,                             BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     1300,                             BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     1300,                             BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     1400,                             BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     1400,                             BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxFragRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxFragRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagFcPipeTh,                  BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagFcPipeTh,                  BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxFragFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxFragFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY}
};

/* Table with default CGM threshold values for Ramon in "single pipe" FE13 MULTISTAGE mode*/
static const threshold_descriptor_t cgm_init_thresholds_single_pipe_mode_fe13_multistage[] =
{
    /* Threshold Type                                  | Threshold ID                                                                  | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DCH / RX*/ {bcmFabricRxMcLowPrioDropPipeTh,           BCM_FABRIC_PIPE_ALL,                                                            220,                            BCM_FABRIC_LINK_TH_FE3_LINKS_ONLY}
};

/*
* } Single pipe mode
*/

/*
* { Dual pipes mode
*/

/* Table with default CGM threshold values for Ramon in "dual pipes" mode*/


/*bcmFabricSharedGciPipeTh,bcmFabricSharedRciPipeTh have to write different values for Method 1 and Method 2 According to SDD which is not supported ! */
static const threshold_descriptor_t cgm_init_thresholds_dual_pipes_mode[] =
{
    /* Threshold Type                                  | Threshold ID                                                                  | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DCH / RX*/ {bcmFabricRxFragDropPipeTh,               BCM_FABRIC_PIPE_ALL,                                                            380,                              0x0},
             {bcmFabricRxMcLowPrioDropPipeTh,          BCM_FABRIC_PIPE_ALL,                                                            120,                              0x0},
             {bcmFabricRxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricRxLLFCFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            192,                              0x0},
/*DTM / MID*/{bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                850,                              0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                1050,                             0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                1300,                             0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                2000,                             0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidFragDropClassTh,             BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastAll, BCM_FABRIC_PRIO_ALL),             BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                       1000,                             0x0},
             {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                       1100,                             0x0},
             {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                       1525,                             0x0},
             {bcmFabricMidFragGciPipeTh,               BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                       1210,                             0x0},
             {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                       1310,                             0x0},
             {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                       1415,                             0x0},
             {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidFragRciPipeTh,               BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidMcCopiesFcPrioTh,            BCM_FABRIC_PRIO_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
/*DTL / TX*/ {bcmFabricTxLinkLoadDropPipeTh,           BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFE1BypassLLFCFcPipeTh,        BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                       170,                              0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                       260,                              0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                       270,                              0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxLinkLoadRciPipeTh,            BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                850,                              0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                1050,                             0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                1300,                             0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                2000,                             0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastAll, BCM_FABRIC_PRIO_ALL),             BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                       1000,                             0x0},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                       1100,                             0x0},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                       1500,                             0x0},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                       1200,                             0x0},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                       1300,                             0x0},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                       1400,                             0x0},
             {bcmFabricTxFragRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagFcPipeTh,                  BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidTagFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidFragFcPipeTh,                BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
/*DFL / SHR*/{bcmFabricSharedMcCopiesDropPrioTh,       BCM_FABRIC_PRIO_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedFragCopiesGciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedBankMcDropPrioTh,         BCM_FABRIC_PRIO_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0}
};

static const threshold_descriptor_t cgm_init_thresholds_pipe_map_dual_uc_mc[] =
{
    /* Threshold Type                                  | Threshold ID                                                                 | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DTL / TX*/ {bcmFabricTxFragGuaranteedPipeTh,         BCM_FABRIC_PIPE_ALL,                                                            384,                             0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                3100,                            0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                3900,                            0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                4870,                            0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                7420,                            0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     3700,                            0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     4100,                            0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     5650,                            0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     4480,                            0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     4870,                            0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     5250,                            0x0}


};

/* Table with CGM threshold values for Ramon in mode with two pipes which should overwrite or add to the default 2 pipe tuning values
   if the pipe mapping is:
   PIPE 0 - Trafic (UC+MC)
   PIPE 1 - TDM
  */
static const threshold_descriptor_t cgm_init_thresholds_pipe_map_dual_tdm_non_tdm[] =
{
    /* Threshold Type                                  | Threshold ID                                                                  | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DTL / TX*/ {bcmFabricTxFragGuaranteedPipeTh,         0,                                                                              128,                              0x0},
             {bcmFabricTxFragGuaranteedPipeTh,         1,                                                                              384,                              0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                5700,                             0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                7120,                             0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                8900,                             0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                13564,                            0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     6760,                             0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     7475,                             0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     10320,                            0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     8200,                             0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     8900,                             0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     9610,                             0x0}
};

/* 
 *  Table with CGM threshold values for Ramon in mode with two pipes and LR enable which should overwrite or add to the default 2 pipe tuning values
 */
static const threshold_descriptor_t cgm_init_thresholds_dual_pipes_mode_lr[] =
{
    /* Threshold Type                                  | Threshold ID                                                                  | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DTL / TX*/ {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                425,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                425,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                525,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                525,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                650,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                650,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                1000,                             BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                1000,                             BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxFragDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastAll, BCM_FABRIC_PRIO_ALL),             BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxFragDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastAll, BCM_FABRIC_PRIO_ALL),             BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                       500,                             BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                       500,                             BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                       550,                             BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                       550,                             BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                       750,                             BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                       750,                             BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                       600,                             BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                       600,                             BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                       650,                             BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                       650,                             BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                       700,                             BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                       700,                             BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxFragRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxFragRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagFcPipeTh,                  BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagFcPipeTh,                  BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxFragFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxFragFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY}
};

/*
* } Two pipes mode
*/

/*
* { Three pipes mode
*/

/* Table with default CGM threshold values for Ramon in "thriple pipes" mode*/


/*bcmFabricSharedGciPipeTh,bcmFabricSharedRciPipeTh have to write different values for Method 1 and Method 2 According to SDD which is not supported ! */
static const threshold_descriptor_t cgm_init_thresholds_thriple_pipes_mode[] =
{
    /* Threshold Type                                  | Threshold ID                                                                  | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DCH / RX*/ {bcmFabricRxFragDropPipeTh,               BCM_FABRIC_PIPE_ALL,                                                            252,                              0x0},
             {bcmFabricRxMcLowPrioDropPipeTh,          BCM_FABRIC_PIPE_ALL,                                                            120,                              0x0},
             {bcmFabricRxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricRxLLFCFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            128,                              0x0},
/*DTM / MID*/{bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                550,                              0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                700,                              0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                850,                              0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                1300,                             0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidFragDropClassTh,             BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastAll, BCM_FABRIC_PRIO_ALL),             BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidFragGciPipeTh,               BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidFragRciPipeTh,               BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidMcCopiesFcPrioTh,            BCM_FABRIC_PRIO_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
/*DTL / TX*/ {bcmFabricTxLinkLoadDropPipeTh,           BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFE1BypassLLFCFcPipeTh,        BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                       160,                              0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                       170,                              0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                       180,                              0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxLinkLoadRciPipeTh,            BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                550,                              0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                700,                              0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                850,                              0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                1300,                             0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastAll, BCM_FABRIC_PRIO_ALL),             BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagFcPipeTh,                  BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidTagFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidFragFcPipeTh,                BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
/*DFL / SHR*/{bcmFabricSharedMcCopiesDropPrioTh,       BCM_FABRIC_PRIO_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedFragCopiesGciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedBankMcDropPrioTh,         BCM_FABRIC_PRIO_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0}

};
/* Table with CGM threshold values for Ramon in mode with three pipes which should overwrite or add to the default 3 pipe tuning values
   if the pipe mapping is:
   PIPE 0  - UC
   PIPE 1  - MC
   PIPE 2  - TDM
*/
static const threshold_descriptor_t cgm_init_thresholds_pipe_map_triple_uc_mc_tdm[] =
{
    /* Threshold Type                                  | Threshold ID                                                                  | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DTM/MID*/ {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                         650,                             0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                         715,                             0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                         990,                             0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                         785,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                         850,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                         920,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                         BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 0),                                         785,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 1),                                         850,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 2),                                         920,                             0x0},
/*DTL / TX*/{bcmFabricTxFragGuaranteedPipeTh,         BCM_FABRIC_PIPE_ALL,                                                              256,                             0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                         650,                             0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                         700,                             0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                         1000,                            0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                         780,                             0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                         850,                             0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                         920,                             0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 0),                                         780,                             0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 1),                                         850,                             0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 2),                                         920,                             0x0},
/*DFL /SHR*/{bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                  2075,                            0x0},
            {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                  2600,                            0x0},
            {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                  3250,                            0x0},
            {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                  4950,                            0x0},
            {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),                BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                       2470,                            0x0},
            {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                       2730,                            0x0},
            {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                       3770,                            0x0},
            {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                       3000,                            0x0},
            {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                       3250,                            0x0},
            {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                       3500,                            0x0}

};

/* 
 *  Table with CGM threshold values for Ramon in mode with three pipes and LR enable which should overwrite or add to the default 3 pipe tuning values
 */
static const threshold_descriptor_t cgm_init_thresholds_pipe_map_triple_uc_mc_tdm_lr[] =
{
    /* Threshold Type                                  | Threshold ID                                                                  | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DTL / TX*/ {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                225,                              BCM_FABRIC_LINK_TH_LR_ONLY},
/*DTL / TX*/ {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                225,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                350,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                350,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                425,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                425,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                650,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                650,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxFragDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastAll, BCM_FABRIC_PRIO_ALL),             BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxFragDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastAll, BCM_FABRIC_PRIO_ALL),             BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                       325,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                       325,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                       350,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                       350,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                       500,                             BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                       500,                             BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                       390,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                       390,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                       425,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                       425,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                       460,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                       460,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 0),                                       390,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 0),                                       390,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 1),                                       425,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 1),                                       425,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 2),                                       460,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 2),                                       460,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxFragRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxFragRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagFcPipeTh,                  BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagFcPipeTh,                  BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxFragFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxFragFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY}
};

/* Table with CGM threshold values for Ramon in mode with three pipes which should overwrite or add to the default 3 pipe tuning values
   if the pipe mapping is:
   PIPE 0 - UC
   PIPE 1 - MC-High Prio
   PIPE 2 - MC-Low Prio
  */
static const threshold_descriptor_t cgm_init_thresholds_pipe_map_triple_uc_hp_mc_lp_mc[] =
{
    /* Threshold Type                                  | Threshold ID                                                                  | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DTM/MID*/ {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                         650,                             0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                         715,                             0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                         990,                             0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 0),                                         650,                             0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 1),                                         715,                             0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 2),                                         990,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                         785,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                         850,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                         920,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
/*DTL / TX*/{bcmFabricTxFragGuaranteedPipeTh,         0,                                                                                128,                             0x0},
            {bcmFabricTxFragGuaranteedPipeTh,         1,                                                                                256,                             0x0},
            {bcmFabricTxFragGuaranteedPipeTh,         2,                                                                                256,                             0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                         650,                             0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                         700,                             0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                         1000,                            0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 0),                                         650,                             0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 1),                                         700,                             0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 2),                                         1000,                            0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                         780,                             0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                         850,                             0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                         920,                             0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
/*DFL /SHR*/{bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                  3370,                            0x0},
            {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                  4210,                            0x0},
            {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                  5260,                            0x0},
            {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                  8020,                            0x0},
            {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),                BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                       4000,                            0x0},
            {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                       4420,                            0x0},
            {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                       6100,                            0x0},
            {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                       4840,                            0x0},
            {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                       5260,                            0x0},
            {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                       5680,                            0x0}

};

/* 
 *  Table with CGM threshold values for Ramon in mode with three pipes and LR enable which should overwrite or add to the default 3 pipe tuning values
 */
static const threshold_descriptor_t cgm_init_thresholds_pipe_map_triple_uc_hp_mc_lp_mc_lr[] =
{
/* Threshold Type                                      | Threshold ID                                                                 | Value                            | Flags */
/* ================================================================================================================================================================================== */
/*DTL / TX*/ {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                225,                              BCM_FABRIC_LINK_TH_LR_ONLY},
/*DTL / TX*/ {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                225,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                350,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                350,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                425,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                425,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                650,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                650,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxFragDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastAll, BCM_FABRIC_PRIO_ALL),             BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxFragDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastAll, BCM_FABRIC_PRIO_ALL),             BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                       325,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                       325,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                       350,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                       350,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                       500,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                       500,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 0),                                       325,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 0),                                       325,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 1),                                       350,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 1),                                       350,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 2),                                       500,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 2),                                       500,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                       390,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                       390,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                       425,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                       425,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                       560,                              BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                       560,                              BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxFragRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxFragRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxTagFcPipeTh,                  BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxTagFcPipeTh,                  BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY},
             {bcmFabricTxFragFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_LR_ONLY},
             {bcmFabricTxFragFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     BCM_FABRIC_LINK_TH_NLR_ONLY}
};
/*
* } Three pipes mode
*/

static const uint32 bcm_dnxf_rci_map_single_pipe[BCM_DNXF_CGM_TUNE_NUM_OF_RCI_LEVELS][BCM_DNXF_CGM_TUNE_NUM_OF_RCI_LEVELS] =
             {  /*             grnt0    grnt1   grnt2   grnt3     */
                /* shr0 */     {0,      0,      0,      1},
                /* shr1 */     {1,      1,      1,      1},
                /* shr2 */     {2,      2,      2,      2},
                /* shr3 */     {3,      3,      3,      3},
             };

/**
 * \brief -  Tune congestion thresholds
 */

shr_error_e
dnxf_cgm_tune_init(
    int unit)
{
    uint32 flags, nof_table_entries, nof_table_entries_lr;
    int table_entry_idx, value, nof_pipes, links_count = 0;
    bcm_fabric_threshold_id_t threshold_id;
    bcm_fabric_threshold_type_t threshold_type;
    bcm_port_t links[DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS], link;
    uint32 profile_attach_flags = 0x0;
    uint32 shared_rci, guaranteed_rci;
    bcm_fabric_rci_resolution_key_t key;
    bcm_fabric_rci_resolution_config_t config;
    const threshold_descriptor_t * cgm_init_values_table;
    const threshold_descriptor_t * cgm_init_values_table_lr;

    SHR_FUNC_INIT_VARS(unit);

    nof_pipes = dnxf_data_fabric.pipes.nof_pipes_get(unit);

    /* Get the correct init vlaues table in dependace of the operational mode of the  device*/
    switch(nof_pipes)
    {
        case 1: nof_table_entries = sizeof(cgm_init_thresholds_single_pipe_mode)/sizeof(threshold_descriptor_t);
                cgm_init_values_table = cgm_init_thresholds_single_pipe_mode;
                break;

        case 2: nof_table_entries = sizeof(cgm_init_thresholds_dual_pipes_mode)/sizeof(threshold_descriptor_t);
                cgm_init_values_table = cgm_init_thresholds_dual_pipes_mode;
                break;

        case 3: nof_table_entries = sizeof(cgm_init_thresholds_thriple_pipes_mode)/sizeof(threshold_descriptor_t);
                cgm_init_values_table = cgm_init_thresholds_thriple_pipes_mode;
                break;
        default: SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid number of pipes received!");
                break;
    }

    /* Configure CGM threhold for each entry in the Deafult init values table*/
    for (table_entry_idx=0; table_entry_idx < nof_table_entries; table_entry_idx++)
    {
        threshold_type = cgm_init_values_table[table_entry_idx].threshold_type;
        threshold_id = cgm_init_values_table[table_entry_idx].threshold_id;
        flags = cgm_init_values_table[table_entry_idx].flags;
        value = cgm_init_values_table[table_entry_idx].value;

        SHR_IF_ERR_EXIT(bcm_dnxf_fabric_profile_threshold_set(unit, BCM_DNXF_CGM_TUNE_PROFILE_0, threshold_id, threshold_type, flags, value));
    }

    if (nof_pipes  == 1) {
        /*For FE13 multistage there is a possibility to configure additional threshold */
        cgm_init_values_table = cgm_init_thresholds_single_pipe_mode_fe13_multistage;
        /**
         * set the table entries only if FE in FE13 MULTISTAGE mode
         */
        if (dnxf_data_fabric.general.device_mode_get(unit) ==  soc_dnxf_fabric_device_mode_multi_stage_fe13)
        {
            nof_table_entries = sizeof(cgm_init_thresholds_single_pipe_mode_fe13_multistage)/sizeof(threshold_descriptor_t);
        }
        else
        {
            nof_table_entries = 0;
        }

        nof_table_entries_lr = sizeof(cgm_init_thresholds_single_pipe_mode_lr)/sizeof(threshold_descriptor_t);
        cgm_init_values_table_lr = cgm_init_thresholds_single_pipe_mode_lr;
    } else {
        /* Get additional init values specific for each of the pipe mapping possibilities */
        switch (dnxf_data_fabric.pipes.map_get(unit)->type)
        {
            case soc_dnxc_fabric_pipe_map_triple_uc_mc_tdm:
                nof_table_entries = sizeof(cgm_init_thresholds_pipe_map_triple_uc_mc_tdm)/sizeof(threshold_descriptor_t);
                cgm_init_values_table = cgm_init_thresholds_pipe_map_triple_uc_mc_tdm;

                nof_table_entries_lr = sizeof(cgm_init_thresholds_pipe_map_triple_uc_mc_tdm_lr)/sizeof(threshold_descriptor_t);
                cgm_init_values_table_lr = cgm_init_thresholds_pipe_map_triple_uc_mc_tdm_lr;
                break;
            case soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc:
                nof_table_entries = sizeof(cgm_init_thresholds_pipe_map_triple_uc_hp_mc_lp_mc)/sizeof(threshold_descriptor_t);
                cgm_init_values_table = cgm_init_thresholds_pipe_map_triple_uc_hp_mc_lp_mc;

                nof_table_entries_lr = sizeof(cgm_init_thresholds_pipe_map_triple_uc_hp_mc_lp_mc_lr)/sizeof(threshold_descriptor_t);
                cgm_init_values_table_lr = cgm_init_thresholds_pipe_map_triple_uc_hp_mc_lp_mc_lr;
                break;
            case soc_dnxc_fabric_pipe_map_dual_tdm_non_tdm:
                nof_table_entries = sizeof(cgm_init_thresholds_pipe_map_dual_tdm_non_tdm)/sizeof(threshold_descriptor_t);
                cgm_init_values_table = cgm_init_thresholds_pipe_map_dual_tdm_non_tdm;

                nof_table_entries_lr = sizeof(cgm_init_thresholds_dual_pipes_mode_lr)/sizeof(threshold_descriptor_t);
                cgm_init_values_table_lr = cgm_init_thresholds_dual_pipes_mode_lr;
                break;
            case soc_dnxc_fabric_pipe_map_dual_uc_mc:
                nof_table_entries = sizeof(cgm_init_thresholds_pipe_map_dual_uc_mc)/sizeof(threshold_descriptor_t);
                cgm_init_values_table = cgm_init_thresholds_pipe_map_dual_uc_mc;

                nof_table_entries_lr = sizeof(cgm_init_thresholds_dual_pipes_mode_lr)/sizeof(threshold_descriptor_t);
                cgm_init_values_table_lr = cgm_init_thresholds_dual_pipes_mode_lr;
                break;
            case soc_dnxc_fabric_pipe_map_single:
                /*No need for additional CGM tuning*/
                nof_table_entries = 0;

                nof_table_entries_lr = sizeof(cgm_init_thresholds_single_pipe_mode_lr)/sizeof(threshold_descriptor_t);
                cgm_init_values_table_lr = cgm_init_thresholds_single_pipe_mode_lr;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid pipe mapping vlaue received!");
                break;
        };
    }


    for (table_entry_idx=0; table_entry_idx < nof_table_entries; table_entry_idx++)
    {
        threshold_type = cgm_init_values_table[table_entry_idx].threshold_type;
        threshold_id = cgm_init_values_table[table_entry_idx].threshold_id;
        flags = cgm_init_values_table[table_entry_idx].flags;
        value = cgm_init_values_table[table_entry_idx].value;

        SHR_IF_ERR_EXIT(bcm_dnxf_fabric_profile_threshold_set(unit, BCM_DNXF_CGM_TUNE_PROFILE_0, threshold_id, threshold_type, flags, value));
    }

    /*
     * IF LR is enabled overwrite with LR/NLR specific values
     */
    if (dnxf_data_fabric.general.local_routing_enable_uc_get(unit) || dnxf_data_fabric.general.local_routing_enable_mc_get(unit))
    {
        for (table_entry_idx=0; table_entry_idx < nof_table_entries_lr; table_entry_idx++)
        {
            threshold_type = cgm_init_values_table_lr[table_entry_idx].threshold_type;
            threshold_id = cgm_init_values_table_lr[table_entry_idx].threshold_id;
            flags = cgm_init_values_table_lr[table_entry_idx].flags;
            value = cgm_init_values_table_lr[table_entry_idx].value;

            SHR_IF_ERR_EXIT(bcm_dnxf_fabric_profile_threshold_set(unit, BCM_DNXF_CGM_TUNE_PROFILE_0, threshold_id, threshold_type, flags, value));
        }

    }

    /*Configure all link to CGM profile 0*/
    SOC_PBMP_ITER(SOC_INFO(unit).sfi.bitmap, link)
    {
        links[links_count] = link;
        links_count++;
    }

    SHR_IF_ERR_EXIT(bcm_dnxf_fabric_link_profile_set(unit, BCM_DNXF_CGM_TUNE_PROFILE_0, profile_attach_flags, links_count, links));

    /* Map RCI levels */
    for (shared_rci = 0 ; shared_rci < BCM_DNXF_CGM_TUNE_NUM_OF_RCI_LEVELS ; shared_rci++)
    {
        for (guaranteed_rci = 0 ; guaranteed_rci < BCM_DNXF_CGM_TUNE_NUM_OF_RCI_LEVELS ; guaranteed_rci++)
        {
            key.pipe = BCM_FABRIC_PIPE_ALL;
            key.shared_rci = shared_rci;
            key.guaranteed_rci = guaranteed_rci;
            config.resolved_rci = bcm_dnxf_rci_map_single_pipe[shared_rci][guaranteed_rci];
            SHR_IF_ERR_EXIT(bcm_dnxf_fabric_rci_resolution_set(unit, 0x0, &key, &config));
        }
    }

exit:
    SHR_FUNC_EXIT;
}




