/*! \file bcm56990_a0_fp_ing_action_db.c
 *
 *  IFP action configuration DB initialization function for Tomahawk-4(56990_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56990_a0_fp.h>
#include <bcmltd/chip/bcmltd_id.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_bcm56990_a0_ingress_action_cfg_db_init(int unit,
                                             bcmfp_stage_t *stage)
{
    uint32_t offset = 0;                           /* General variable to carry offset. */
    uint32_t profile_one_offset = 0;               /* Offset of PROFILE_ONE_SET. */
    uint32_t profile_two_offset = 0;               /* Offset of PROFILE_TWO_SET. */
    uint32_t redirect_offset = 0;                  /* Offset of REDIRECT_SET. */
    uint32_t l3swl2change_offset = 0;              /* Offset of L3SW_CHANGE_L2_SET. */
    uint32_t mirror_offset = 0;                    /* Offset of MIRROR_SET. */
    uint32_t ifp_multiple_mirror_dst_offset = 0;   /* Offset of IFP_MULTIPLE_MIRROR_DESTINATION_SET. */
    uint32_t ifp_mirror_on_drop_set = 0;           /* Offset of IFP_MIRROR_ON_DROP_SET. */
    uint32_t lb_offset = 0;                        /* Offset of LB_CONTROL_SET. */
    uint32_t copytocpu_offset = 0;                 /* Offset of COPY_TO_CPU_SET. */
    uint32_t ctr_offset = 0;                       /* Offset of IFP_COUNTER_SET. */
    uint32_t cutthrough_offset = 0;                /* Offset of CUT_THRU_OVERRIDE_SET. */
    uint32_t dcn_offset = 0;                       /* Offset of DCN_ACTION_SET. */
    uint32_t ttl_offset = 0;                       /* Offset of TTL_SET. */
    uint32_t cpucos_offset = 0;                    /* Offset of CHANGE_CPU_COS_SET. */
    uint32_t drop_offset = 0;                      /* Offset of DROP_SET. */
    uint32_t mirror_override_offset = 0;           /* Offset of MIRROR_OVERRIDE_SET. */
    uint32_t sflow_offset = 0;                     /* Offset of SFLOW_SET. */
    uint32_t debug_offset = 0;                     /* Offset of INSTRUMENTATION_TRIGGERS_ENABLE. */
    uint32_t timestamp_offset = 0;                 /* Offset of TIME_STAMP_SET. */
    uint32_t dlb_ecmp_monitor_offset = 0;          /* Offset of DLB_ECMP_MONITOR_SET. */
    uint32_t elephant_trap_offset = 0;             /* Offset of ELEPHANT_TRAP_SET. */
    uint32_t green_to_pid_offset = 0;              /* Offset of GREEN_TO_PID_SET. */
    uint32_t inband_telemetry_offset = 0;          /* Offset of INBAND_TELEMETRY_SET. */
    uint32_t ddrop_offset = 0;                     /* Offset of DELAYED_DROP_SET. */
    uint32_t dredirect_offset = 0;                 /* Offset of DELAYED_REDIRECT_SET. */
    uint32_t protect_sw_offset = 0;                /* Offset of PROTECTION_SWITCHING_SET. */
    uint32_t dlb_alt_path_offset = 0;              /* Offset of DLB_ALTERNATE_PATH_SET. */
    uint32_t dlb_ecmp_offset = 0;                  /* Ofsset of DLB_ECMP_SET. */
    uint32_t fid = 0;
    uint32_t conflict_fid = 0;
    BCMFP_ACTION_CFG_DECL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_KEY_TYPE_PRESEL)) {
        SHR_IF_ERR_EXIT(
               bcmfp_bcm56990_a0_ingress_presel_action_cfg_db_init(unit,
                                                                   stage));
    }

    redirect_offset        = 0;  /* Start of the IFP_POLICY_TABLE */
    cpucos_offset          = 38; /* REDIRECT_SET(38b) */
    copytocpu_offset       = 46; /* REDIRECT_SET(38b) +
                                  * CHANGE_CPU_COS_SET(8b) */
    ctr_offset             = 63; /* REDIRECT_SET(38b) +
                                  * CHANGE_CPU_COS_SET(8b) +
                                  * COPY_TO_CPU_SET(17b) */
    cutthrough_offset      = 88; /* REDIRECT_SET(38b) +
                                  * CHANGE_CPU_COS_SET(8b) +
                                  * COPY_TO_CPU_SET(17b) +
                                  * COUNTER_SET(25b) */
    ddrop_offset           = 91; /* REDIRECT_SET(38b) +
                                  * CHANGE_CPU_COS_SET(8b) +
                                  * COPY_TO_CPU_SET(17b) +
                                  * COUNTER_SET(25b) +
                                  * CUT_THRU_OVERRIDE_SET(1b) +
                                  * DCN_ACTION_SET (2b) */
    dredirect_offset       = 99; /* REDIRECT_SET(38b) +
                                  * CHANGE_CPU_COS_SET(8b) +
                                  * COPY_TO_CPU_SET(17b) +
                                  * COUNTER_SET(25b) +
                                  * CUT_THRU_OVERRIDE_SET(1b) +
                                  * DCN_ACTION_SET (2b)+
                                  * DELAYED_DROP_SET(8b) */
    dlb_alt_path_offset     = 116;
                                   /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) */
    dlb_ecmp_monitor_offset = 126; /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) */
    dlb_ecmp_offset         = 128;
                                   /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                    * DLB_ECMP_MONITOR_SET(2b) */

    drop_offset             = 130; /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                    * DLB_ECMP_MONITOR_SET(2b) +
                                    * DLB_ECMP_SET(2b) */
    elephant_trap_offset    = 136; /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                    * DLB_ECMP_MONITOR_SET(2b) +
                                    * DLB_ECMP_SET(2b) +
                                    * DROP_SET(6b) +
                                    * ELEPHANT_TRAP_SET(6b) +
                                    * GREEN_TO_PID_SET(1b) +
                                    * IFP_MIRROR_ON_DROP_SET (5b) +
                                    * DROP_SET(6b) */
    green_to_pid_offset     = 142; /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                    * DLB_ECMP_MONITOR_SET(2b) +
                                    * DLB_ECMP_SET(2b) +
                                    * DROP_SET(6b) +
                                    * ELEPHANT_TRAP_SET(6b) */
    ifp_mirror_on_drop_set  = 143; /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                    * DLB_ECMP_MONITOR_SET(2b) +
                                    * DLB_ECMP_SET(2b) +
                                    * DROP_SET(6b) +
                                    * ELEPHANT_TRAP_SET(6b) +
                                    * GREEN_TO_PID_SET(1b) */
    ifp_multiple_mirror_dst_offset = 148; /* REDIRECT_SET(38b) +
                                           * CHANGE_CPU_COS_SET(8b) +
                                           * COPY_TO_CPU_SET(17b) +
                                           * COUNTER_SET(25b) +
                                           * CUT_THRU_OVERRIDE_SET(1b) +
                                           * DCN_ACTION_SET (2b)+
                                           * DELAYED_DROP_SET(8b) +
                                           * DELAYED_REDIRECT_SET(17b) +
                                           * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                           * DLB_ECMP_MONITOR_SET(2b) +
                                           * DLB_ECMP_SET(2b) +
                                           * DROP_SET(6b) +
                                           * ELEPHANT_TRAP_SET(6b) +
                                           * GREEN_TO_PID_SET(1b) +
                                           * IFP_MIRROR_ON_DROP_SET (5b) +
                                           * ELEPHANT_TRAP_SET(6b) */
    inband_telemetry_offset = 155; /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                    * DLB_ECMP_MONITOR_SET(2b) +
                                    * DLB_ECMP_SET(2b) +
                                    * DROP_SET(6b) +
                                    * ELEPHANT_TRAP_SET(6b) +
                                    * GREEN_TO_PID_SET(1b) +
                                    * IFP_MIRROR_ON_DROP_SET (5b) +
                                    * IFP_MULTIPLE_MIRROR_DESTINATION_SET (7b) */
    debug_offset           = 187;  /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                    * DLB_ECMP_MONITOR_SET(2b) +
                                    * DLB_ECMP_SET(2b) +
                                    * DROP_SET(6b) +
                                    * ELEPHANT_TRAP_SET(6b) +
                                    * GREEN_TO_PID_SET(1b) +
                                    * IFP_MIRROR_ON_DROP_SET (5b) +
                                    * IFP_MULTIPLE_MIRROR_DESTINATION_SET (7b) +
                                    * INBAND_TELEMETRY_SET(2b) */
    l3swl2change_offset    = 188;  /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                    * DLB_ECMP_MONITOR_SET(2b) +
                                    * DLB_ECMP_SET(2b) +
                                    * DROP_SET(6b) +
                                    * ELEPHANT_TRAP_SET(6b) +
                                    * GREEN_TO_PID_SET(1b) +
                                    * IFP_MIRROR_ON_DROP_SET (5b) +
                                    * IFP_MULTIPLE_MIRROR_DESTINATION_SET (7b) +
                                    * INBAND_TELEMETRY_SET(32b) +
                                    * INSTRUMENTATION_SET(1b) */
    lb_offset              = 211;  /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                    * DLB_ECMP_MONITOR_SET(2b) +
                                    * DLB_ECMP_SET(2b) +
                                    * DROP_SET(6b) +
                                    * ELEPHANT_TRAP_SET(6b) +
                                    * GREEN_TO_PID_SET(1b) +
                                    * IFP_MIRROR_ON_DROP_SET (5b) +
                                    * IFP_MULTIPLE_MIRROR_DESTINATION_SET (7b) +
                                    * INBAND_TELEMETRY_SET(32b) +
                                    * INSTRUMENTATION_SET(1b) +
                                    * L3SW_CHANGE_L2_SET(23b) */
    mirror_override_offset = 229;  /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                    * DLB_ECMP_MONITOR_SET(2b) +
                                    * DLB_ECMP_SET(2b) +
                                    * DROP_SET(6b) +
                                    * ELEPHANT_TRAP_SET(6b) +
                                    * GREEN_TO_PID_SET(1b) +
                                    * IFP_MIRROR_ON_DROP_SET (5b) +
                                    * IFP_MULTIPLE_MIRROR_DESTINATION_SET (7b) +
                                    * INBAND_TELEMETRY_SET(32b) +
                                    * INSTRUMENTATION_SET(1b) +
                                    * L3SW_CHANGE_L2_SET(23b) +
                                    * LB_CONTROLS_SET(2b) +
                                    * METER_SET(16b) */
    mirror_offset          = 230;  /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                    * DLB_ECMP_MONITOR_SET(2b) +
                                    * DLB_ECMP_SET(2b) +
                                    * DROP_SET(6b) +
                                    * ELEPHANT_TRAP_SET(6b) +
                                    * GREEN_TO_PID_SET(1b) +
                                    * IFP_MIRROR_ON_DROP_SET (5b) +
                                    * IFP_MULTIPLE_MIRROR_DESTINATION_SET (7b) +
                                    * INBAND_TELEMETRY_SET(32b) +
                                    * INSTRUMENTATION_SET(1b) +
                                    * L3SW_CHANGE_L2_SET(23b) +
                                    * LB_CONTROLS_SET(2b) +
                                    * METER_SET(16b) +
                                    * MIRROR_OVERRIDE_SET(1b) */
    profile_one_offset     = 249;  /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                    * DLB_ECMP_MONITOR_SET(2b) +
                                    * DLB_ECMP_SET(2b) +
                                    * DROP_SET(6b) +
                                    * ELEPHANT_TRAP_SET(6b) +
                                    * GREEN_TO_PID_SET(1b) +
                                    * IFP_MIRROR_ON_DROP_SET (5b) +
                                    * IFP_MULTIPLE_MIRROR_DESTINATION_SET (7b) +
                                    * INBAND_TELEMETRY_SET(32b) +
                                    * INSTRUMENTATION_SET(1b) +
                                    * L3SW_CHANGE_L2_SET(23b) +
                                    * LB_CONTROLS_SET(2b) +
                                    * METER_SET(16b) +
                                    * MIRROR_OVERRIDE_SET(1b) +
                                    * MIRROR_SET(15b) */
    profile_two_offset     = 305;  /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                    * DLB_ECMP_MONITOR_SET(2b) +
                                    * DLB_ECMP_SET(2b) +
                                    * DROP_SET(6b) +
                                    * ELEPHANT_TRAP_SET(6b) +
                                    * GREEN_TO_PID_SET(1b) +
                                    * IFP_MIRROR_ON_DROP_SET (5b) +
                                    * IFP_MULTIPLE_MIRROR_DESTINATION_SET (7b) +
                                    * INBAND_TELEMETRY_SET(32b) +
                                    * INSTRUMENTATION_SET(1b) +
                                    * L3SW_CHANGE_L2_SET(23b) +
                                    * LB_CONTROLS_SET(2b) +
                                    * METER_SET(16b) +
                                    * MIRROR_OVERRIDE_SET(1b) +
                                    * MIRROR_SET(15b) +
                                    * PROFILE_SET_1(56b) */
    protect_sw_offset      = 357,   /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                    * DLB_ECMP_MONITOR_SET(2b) +
                                    * DLB_ECMP_SET(2b) +
                                    * DROP_SET(6b) +
                                    * ELEPHANT_TRAP_SET(6b) +
                                    * GREEN_TO_PID_SET(1b) +
                                    * IFP_MIRROR_ON_DROP_SET (5b) +
                                    * IFP_MULTIPLE_MIRROR_DESTINATION_SET (7b) +
                                    * INBAND_TELEMETRY_SET(32b) +
                                    * INSTRUMENTATION_SET(1b) +
                                    * L3SW_CHANGE_L2_SET(23b) +
                                    * LB_CONTROLS_SET(2b) +
                                    * METER_SET(16b) +
                                    * MIRROR_OVERRIDE_SET(1b) +
                                    * MIRROR_SET(15b) +
                                    * PROFILE_SET_1(56b) +
                                    * PROFILE_SET_2(52b) */
    sflow_offset           = 358;  /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                    * DLB_ECMP_MONITOR_SET(2b) +
                                    * DLB_ECMP_SET(2b) +
                                    * DROP_SET(6b) +
                                    * ELEPHANT_TRAP_SET(6b) +
                                    * GREEN_TO_PID_SET(1b) +
                                    * IFP_MIRROR_ON_DROP_SET (5b) +
                                    * IFP_MULTIPLE_MIRROR_DESTINATION_SET (7b) +
                                    * INBAND_TELEMETRY_SET(32b) +
                                    * INSTRUMENTATION_SET(1b) +
                                    * L3SW_CHANGE_L2_SET(23b) +
                                    * LB_CONTROLS_SET(2b) +
                                    * METER_SET(16b) +
                                    * MIRROR_OVERRIDE_SET(1b) +
                                    * MIRROR_SET(15b) +
                                    * PROFILE_SET_1(56b) +
                                    * PROFILE_SET_2(52b) +
                                    * PROTECTION_SWITCHING_SET(1b) */

    timestamp_offset       = 365;  /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                    * DLB_ECMP_MONITOR_SET(2b) +
                                    * DLB_ECMP_SET(2b) +
                                    * DROP_SET(6b) +
                                    * ELEPHANT_TRAP_SET(6b) +
                                    * GREEN_TO_PID_SET(1b) +
                                    * IFP_MIRROR_ON_DROP_SET (5b) +
                                    * IFP_MULTIPLE_MIRROR_DESTINATION_SET (7b) +
                                    * INBAND_TELEMETRY_SET(32b) +
                                    * INSTRUMENTATION_SET(1b) +
                                    * L3SW_CHANGE_L2_SET(23b) +
                                    * LB_CONTROLS_SET(2b) +
                                    * METER_SET(16b) +
                                    * MIRROR_OVERRIDE_SET(1b) +
                                    * MIRROR_SET(15b) +
                                    * PROFILE_SET_1(56b) +
                                    * PROFILE_SET_2(52b) +
                                    * PROTECTION_SWITCHING_SET(1b) +
                                    * SFLOW_SET(7b) */

    ttl_offset             = 369;  /* REDIRECT_SET(38b) +
                                    * CHANGE_CPU_COS_SET(8b) +
                                    * COPY_TO_CPU_SET(17b) +
                                    * COUNTER_SET(25b) +
                                    * CUT_THRU_OVERRIDE_SET(1b) +
                                    * DCN_ACTION_SET (2b)+
                                    * DELAYED_DROP_SET(8b) +
                                    * DELAYED_REDIRECT_SET(17b) +
                                    * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                    * DLB_ECMP_MONITOR_SET(2b) +
                                    * DLB_ECMP_SET(2b) +
                                    * DROP_SET(6b) +
                                    * ELEPHANT_TRAP_SET(6b) +
                                    * GREEN_TO_PID_SET(1b) +
                                    * IFP_MIRROR_ON_DROP_SET (5b) +
                                    * IFP_MULTIPLE_MIRROR_DESTINATION_SET (7b) +
                                    * INBAND_TELEMETRY_SET(32b) +
                                    * INSTRUMENTATION_SET(1b) +
                                    * L3SW_CHANGE_L2_SET(23b) +
                                    * LB_CONTROLS_SET(2b) +
                                    * METER_SET(16b) +
                                    * MIRROR_OVERRIDE_SET(1b) +
                                    * MIRROR_SET(15b) +
                                    * PROFILE_SET_1(56b) +
                                    * PROFILE_SET_2(52b) +
                                    * PROTECTION_SWITCHING_SET(1b) +
                                    * SFLOW_SET(7b) +
                                    * TIMESTAMP_SET(4b) */

    offset = redirect_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_REDIRECT_TO_PORTf;
    BCMFP_ACTION_CFG_ADD_FOUR(unit, stage,
                              fid,
                              0,
                              0,
                              offset + 6, 9, -1,
                              offset + 15 , 1, 0, offset + 3, 3, 0,
                              offset + 0, 3, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_REDIRECT_TO_TRUNKf;
    BCMFP_ACTION_CFG_ADD_FOUR(unit, stage,
                              fid,
                              0,
                              0,
                              offset + 6, 9, -1,
                              offset + 15, 1, 1, offset + 3, 3, 0,
                              offset + 0, 3, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_UNMODIFIED_REDIRECT_TO_PORTf;
    BCMFP_ACTION_CFG_ADD_FOUR(unit, stage,
                              fid,
                              0,
                              0,
                              offset + 6, 9, -1,
                              offset + 15 , 1, 0, offset + 3, 3, 1,
                              offset + 0, 3, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_UNMODIFIED_REDIRECT_TO_TRUNKf;
    BCMFP_ACTION_CFG_ADD_FOUR(unit, stage,
                              fid,
                              0,
                              0,
                              offset + 6, 9, -1,
                              offset + 15, 1, 1, offset + 3, 3, 1,
                              offset + 0, 3, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_REDIRECT_TO_NHOPf;
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage,
                               fid,
                               0,
                               0,
                               offset + 6, 16, -1,
                               offset + 3, 3, 2, offset + 0, 3, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_REDIRECT_TO_ECMPf;
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage,
                               fid,
                               0,
                               0,
                               offset + 6, 12, -1,
                               offset + 3, 3, 3, offset + 0, 3, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_REDIRECT_UC_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage,
                             fid,
                             0, 0, offset + 0, 3, 2);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_REDIRECT_PORTS_BROADCAST_PKTf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage,
                               fid,
                               0,
                               0,
                               offset + 23, 2, 0, offset + 0, 3, 3);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_REDIRECT_REMOVE_PORTS_BROADCASTf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                             0, 0,
                             offset + 0, 3, 4);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_REDIRECT_ADD_PORTS_BROADCASTf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                             0, 0,
                             offset + 0, 3, 5);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_REDIRECT_PORTS_VLAN_BROADCASTf;
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage,
                              fid,
                              0,
                              0,
                              offset + 17, 1, 0, offset + 23, 2, 1,
                              offset + 0, 3, 3);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_REDIRECT_PORTS_VLAN_BROADCAST_FP_INGf;
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage,
                               fid,
                               0,
                               0,
                               offset + 17, 1, 1,
                               offset + 23, 2, 1, offset + 0, 3, 3);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_REDIRECT_L2_MC_GROUP_IDf;
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage,
                               fid,
                               0,
                               0,
                               offset + 6, 10, -1,
                               offset + 23, 2, 2, offset + 0, 3, 3);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_REDIRECT_L3_MC_NHOP_IDf;
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage,
                               fid,
                               0,
                               0,
                               offset + 6, 9, -1,
                               offset + 23, 2, 3, offset + 0, 3, 3);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_FP_ING_ADD_REDIRECT_DATA_IDf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                             0, 0,
                             offset + 6, 8, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_FP_ING_REMOVE_REDIRECT_DATA_IDf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                             0, 0,
                             offset + 6, 8, -1);

    /* IFP_CHANGE_CPU_COS_SET(8b) */
    BCMFP_ACTION_CFG_INIT(unit);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_NEW_CPU_COSf;
    offset = cpucos_offset;
    BCMFP_ACTION_CFG_OFFSET_ADD(0, 0, offset + 2, 6, -1);
    BCMFP_ACTION_CFG_OFFSET_ADD(1, 0, offset + 0, 2, 1);
    conflict_fid = FP_ING_POLICY_TEMPLATEt_ACTION_NEW_SERVICE_POOL_IDf;
    BCMFP_ACTION_CFG_SINGLE_WIDE_CONFLICT_ACTION_ADD(0,conflict_fid);
    conflict_fid = FP_ING_POLICY_TEMPLATEt_ACTION_NEW_SERVICE_POOL_PRECEDENCEf;
    BCMFP_ACTION_CFG_SINGLE_WIDE_CONFLICT_ACTION_ADD(1,conflict_fid);
    BCMFP_ACTION_CFG_INSERT(unit, stage, fid, 0);

    BCMFP_ACTION_CFG_INIT(unit);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_NEW_SERVICE_POOL_IDf;
    BCMFP_ACTION_CFG_OFFSET_ADD(0, 0, offset + 4, 2, -1);
    BCMFP_ACTION_CFG_OFFSET_ADD(1, 0, offset + 6, 2, 2);
    BCMFP_ACTION_CFG_OFFSET_ADD(2, 0, offset + 0, 2, 2);
    conflict_fid = FP_ING_POLICY_TEMPLATEt_ACTION_NEW_CPU_COSf;
    BCMFP_ACTION_CFG_SINGLE_WIDE_CONFLICT_ACTION_ADD(0,conflict_fid);
    conflict_fid = FP_ING_POLICY_TEMPLATEt_ACTION_NEW_SERVICE_POOL_PRECEDENCEf;
    BCMFP_ACTION_CFG_SINGLE_WIDE_CONFLICT_ACTION_ADD(1,conflict_fid);
    BCMFP_ACTION_CFG_INSERT(unit, stage, fid, 0);

    BCMFP_ACTION_CFG_INIT(unit);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_NEW_SERVICE_POOL_PRECEDENCEf;
    BCMFP_ACTION_CFG_OFFSET_ADD(0, 0, offset + 2, 2, -1);
    BCMFP_ACTION_CFG_OFFSET_ADD(1, 0, offset + 6, 2, 1);
    BCMFP_ACTION_CFG_OFFSET_ADD(2, 0, offset + 0, 2, 2);
    conflict_fid = FP_ING_POLICY_TEMPLATEt_ACTION_NEW_CPU_COSf;
    BCMFP_ACTION_CFG_SINGLE_WIDE_CONFLICT_ACTION_ADD(0,conflict_fid);
    conflict_fid = FP_ING_POLICY_TEMPLATEt_ACTION_NEW_SERVICE_POOL_IDf;
    BCMFP_ACTION_CFG_SINGLE_WIDE_CONFLICT_ACTION_ADD(1,conflict_fid);
    BCMFP_ACTION_CFG_INSERT(unit, stage, fid, 0);
    /* IFP_COPY_TO_CPU_SET(17b) */
    offset = copytocpu_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_COPY_TO_CPUf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                             0, 0,
                             offset + 11, 3, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_MATCH_IDf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                             0, 0,
                             offset + 3, 8, -1);

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_COPY_TO_CPU_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 11, 3, 2);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_SWITCH_TO_CPU_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 11, 3, 3);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_SWITCH_TO_CPU_REINSATEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 11, 3, 4);

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_COPY_AND_SWITCH_TO_CPU_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 11, 3, 6);


    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_COPY_TO_CPU_WITH_TIMESTAMPf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                         offset + 11, 3, 5);

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_COPY_TO_CPUf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 14, 3, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_COPY_TO_CPU_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 14, 3, 2);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_SWITCH_TO_CPU_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 14, 3, 3);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_SWITCH_TO_CPU_REINSATEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 14, 3, 4);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_COPY_AND_SWITCH_TO_CPU_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 14, 3, 6);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_COPY_TO_CPU_WITH_TIMESTAMPf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 14, 3, 5);

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_COPY_TO_CPUf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                         offset + 0, 3, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_COPY_TO_CPU_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 0, 3, 2);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_SWITCH_TO_CPU_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 0, 3, 3);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_SWITCH_TO_CPU_REINSATEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 0, 3, 4);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_COPY_AND_SWITCH_TO_CPU_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 0, 3, 6);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_COPY_TO_CPU_WITH_TIMESTAMPf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                         offset + 0, 3, 5);

    /* IFP_CUT_THRU_OVERRIDE_SET(1b) */
    offset = cutthrough_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_DO_NOT_CUT_THROUGHf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                 0, 0, offset + 0, 1, 1);

    /* DLB_ALTERNATE_PATH_CONTROL_SET(10b) */
    offset = dlb_alt_path_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_DGMf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                 0, 0,
                                 offset + 0, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_DGM_THRESHOLDf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                 0, 0,
                                 offset + 7, 3, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_DGM_COSTf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                 0, 0,
                                 offset + 4, 3, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_DGM_BIASf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                 0, 0,
                                 offset + 1, 3, -1);

    /* DLB_ECMP_MONITOR_SET(2b) */
    offset = dlb_ecmp_monitor_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_DLB_ECMP_MONITOR_DISABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                             0, 0,
                             offset + 0, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_DLB_ECMP_MONITOR_ENABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                             0, 0,
                             offset + 1, 1, 1);

    /* DLB_ECMP_SET(2b) */
    offset = dlb_ecmp_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_DYNAMIC_ECMP_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                            0, 0, offset + 0, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_DYNAMIC_ECMP_ENABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                            0, 0, offset + 1, 1, 1);

    /* IFP_DROP_SET(6b) */
    offset = drop_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_DROPf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                 0, 0, offset + 2, 2, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_DROP_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                 0, 0, offset + 2, 2, 2);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_DROPf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                 0, 0, offset + 4, 2, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_DROP_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                 0, 0,  offset + 4, 2, 2);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_DROPf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                 0, 0, offset + 0, 2, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_DROP_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                 0, 0, offset + 0, 2, 2);

    /* ELEPHANT_TRAP_SET */
    offset = elephant_trap_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_ETRAP_COLOR_DISABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 0, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_ETRAP_COLOR_ENABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 1, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_ETRAP_LOOKUP_DISABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 2, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_ETRAP_LOOKUP_ENABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 3, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_ETRAP_QUEUE_DISABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 4, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_ETRAP_QUEUE_ENABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 5, 1, 1);
    /* IFP_MIRROR_ON_DROP_SET */
    offset = ifp_mirror_on_drop_set;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_MIRROR_ON_DROP_TM_ENABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 0, 1, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_MIRROR_ON_DROP_TM_DISABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 1, 1, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_MIRROR_ON_DROP_TM_PROFILE_IDf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 2, 3, -1);

    /* IFP_MULTIPLE_MIRROR_DESTINATION_SET */
    offset = ifp_multiple_mirror_dst_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_MIRROR_FLOW_CLASSf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 0, 6, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_MIRROR_FLOW_CLASS_ENABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                        0, 0,
                                        offset + 6, 1, 1);

    /* INBAND_TELEMETRY_SET */
    offset = inband_telemetry_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_ENCAP_DISABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                     0, 0,
                                     offset + 0, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_ENCAP_ENABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                     0, 0,
                                     offset + 1, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_PROFILE_IDXf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                     0, 0,
                                     offset + 2, 4, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_SAMPLE_ENABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                     0, 0,
                                     offset + 6, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_FLOW_CLASS_IDf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                     0, 0,
                                     offset + 7, 13, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_TERM_ACTION_PROFILE_IDXf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                     0, 0,
                                     offset + 20, 4, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_EGR_LOOPBACK_PROFILE_IDXf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                     0, 0,
                                     offset + 24, 2, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_MMU_STAT_REQ_0f;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                     0, 0,
                                     offset + 26, 3, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_MMU_STAT_REQ_1f;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                     0, 0,
                                     offset + 29, 3, -1);

    /* IFP_INSTRUMENTATION_TRIGGERS_ENABLE_SET(1b)*/
    offset = debug_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_VISIBILITY_ENABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                             0,
                             0,
                             offset + 0, 1, 1);

    /* IFP_L3SW_CHANGE_L2_SET(23b) */
    offset = l3swl2change_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_CHANGE_PKT_L2_FIELDSf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
                             0, 0,
                             offset + 0, 16, -1, offset + 19, 4, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_CHANGE_PKT_L2_FIELDS_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage,
                             fid,
                             0, 0,
                             offset + 19, 4, 2);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_SWITCH_TO_L3UCf;
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, fid,
                               0,
                               0,
                               offset + 0, 16, -1, offset + 17, 1, 0,
                               offset + 19, 4, 6);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_SWITCH_TO_ECMPf;
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, fid,
                               0,
                               0,
                               offset + 0, 12, -1, offset + 17, 1, 1,
                               offset + 19, 4, 6);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_ECMP_HASHf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
                             0, 0,
                             offset + 12, 3, -1, offset + 19, 4, 6);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_L3_SWITCH_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                             0, 0,
                             offset + 19, 4, 7);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_ING_CLASS_ID_SELECTf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage,
                             fid, 0, 0,
                             offset + 0, 4, -1, offset + 19, 4, 8);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_FP_ING_CLASS_IDf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage,
                             fid, 0, 0,
                             offset + 4, 12, -1, offset + 19, 4, 8);

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_NEW_BFD_SEESSION_IDXf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage,
                             fid, 0,
                             0,
                             offset + 0, 12, -1, offset + 19, 4, 9);

    /* IFP_LB_CONTROLS_SET(2b) */
    offset = lb_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_TRUNK_SPRAY_HASH_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage,
                       fid,
                       0, 0,
                       offset + 1, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_ECMP_SPRAY_HASH_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage,
                       fid,
                       0, 0,
                       offset + 0, 1, 1);

    /* IFP_MIRROR_OVERRIDE_SET(1b) */
    offset = mirror_override_offset;

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_MIRROR_OVERRIDEf;
     BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                              0, 0,
                              offset + 0, 1, 1);

    /* IFP_MIRROR_SET(12b) */
    offset = mirror_offset;

    fid = FP_ING_POLICY_TEMPLATEt_FIELD_COUNT;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage,
                       fid,
                       0, 0,
                       offset + 7, 3, -1);
    fid = FP_ING_POLICY_TEMPLATEt_FIELD_COUNT + 1;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage,
                       fid,
                       0, 0,
                       offset + 10, 3, -1);
    fid = FP_ING_POLICY_TEMPLATEt_FIELD_COUNT + 2;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage,
                       fid,
                       0, 0,
                       offset + 13, 3, -1);
    fid = FP_ING_POLICY_TEMPLATEt_FIELD_COUNT + 3;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage,
                       fid,
                       0, 0,
                       offset + 16, 3, -1);
    /* ACTION_MIRROR_INSTANCE_ENABLE[0] */
    fid = FP_ING_POLICY_TEMPLATEt_FIELD_COUNT + 4;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage,
                       fid,
                       0, 0,
                       offset + 0, 1, -1);
    /* ACTION_MIRROR_INSTANCE_ENABLE[1] */
    fid = FP_ING_POLICY_TEMPLATEt_FIELD_COUNT + 5;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage,
                       fid,
                       0, 0,
                       offset + 1, 1, -1);
    /* ACTION_MIRROR_INSTANCE_ENABLE[2] */
    fid = FP_ING_POLICY_TEMPLATEt_FIELD_COUNT + 6;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage,
                       fid,
                       0, 0,
                       offset + 2, 1, -1);
    /* ACTION_MIRROR_INSTANCE_ENABLE[3] */
    fid = FP_ING_POLICY_TEMPLATEt_FIELD_COUNT + 7;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage,
                       fid,
                       0, 0,
                       offset + 3, 1, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_MIRROR_EGR_ZERO_PAYLOAD_PROFILE_IDf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage,
                             fid,
                             0, 0,
                             offset + 4, 3, -1);

    /* IFP_PROFILE_SET_1(56b) */
    offset = profile_one_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_NEW_UNTAG_PKT_PRIORITYf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 18, 4, -1, offset + 0, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_NEW_COLORf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 35, 2, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_NEW_COLORf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 52, 2, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_NEW_COLORf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 14, 2, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_NEW_COSf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 27, 8, -1, offset + 22, 4, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_FP_ING_COS_Q_INT_PRI_MAP_IDf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 27, 2, -1, offset + 22, 4, 2);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_NEW_INTPRIf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 27, 8, -1, offset + 22, 4, 5);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_INTPRI_TO_TOSf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 22, 4, 6);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_INTPRI_UPDATES_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 22, 4, 7);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_NEW_UC_COSf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 27, 4, -1, offset + 22, 4, 8);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_NEW_MC_COSf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 31, 4, -1, offset + 22, 4, 9);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_NEW_COSf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 44, 8, -1, offset + 39, 4, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_FP_ING_COS_Q_INT_PRI_MAP_IDf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 44, 2, -1, offset + 39, 4, 2);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_NEW_INTPRIf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid ,
            0, 0, offset + 44, 8, -1, offset + 39, 4, 5);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_INTPRI_TO_TOSf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 39, 4, 6);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_INTPRI_UPDATES_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 39, 4, 7);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_NEW_UC_COSf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 44, 4, -1, offset + 39, 4, 8);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_NEW_MC_COSf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 48, 4, -1, offset + 39, 4, 9);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_NEW_COSf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 6, 8, -1, offset + 1, 4, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_FP_ING_COS_Q_INT_PRI_MAP_IDf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 6, 2, -1, offset + 1, 4, 2);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_NEW_INTPRIf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 6, 8, -1, offset + 1, 4, 5);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_INTPRI_TO_TOSf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 1, 4, 6);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_INTPRI_UPDATES_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 1, 4, 7);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_NEW_UC_COSf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 6, 4, -1, offset + 1, 4, 8);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_NEW_MC_COSf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 10, 4, -1, offset + 1, 4, 9);

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_NEW_INTCNf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage,
            fid,
            0,
            0, offset + 16, 2, -1, offset + 5, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_NEW_INTCNf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage,
            fid,
            0,
            0, offset + 54, 2, -1, offset + 43, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_NEW_INTCNf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage,
            fid,
            0,
            0, offset + 37, 2, -1, offset + 26, 1, 1);

    /* IFP_PROFILE_SET_2(52b) */
    offset = profile_two_offset;

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_NEW_ECNf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 30, 2, -1, offset + 20, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_NEW_ECNf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 47, 2, -1, offset + 37, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_NEW_ECNf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 13, 2, -1, offset + 3, 1, 1);

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_PRESERVE_DOT1Pf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 21, 3, 3);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_NEW_OUTER_DOT1Pf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 32, 3, -1, offset + 21, 3, 5);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_OUTER_DOT1P_TO_TOSf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 21, 3, 6);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_DOT1P_UPDATES_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 21, 3, 7);

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_PRESERVE_DOT1Pf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 38, 3, 3);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_NEW_OUTER_DOT1Pf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 49, 3, -1, offset + 38, 3, 5);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_OUTER_DOT1P_TO_TOSf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 38, 3, 6);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_DOT1P_UPDATES_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 38, 3, 7);

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_PRESERVE_DOT1Pf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 4, 3, 3);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_NEW_OUTER_DOT1Pf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 15, 3, -1, offset + 4, 3, 5);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_OUTER_DOT1P_TO_TOSf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 4, 3, 6);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_DOT1P_UPDATES_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 4, 3, 7);

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_NEW_DSCPf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 24, 6, -1, offset + 18, 2, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_DSCP_UPDATES_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 18, 2, 2);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_R_PRESERVE_DSCPf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 18, 2, 3);

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_NEW_DSCPf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 41, 6, -1, offset + 35, 2, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_DSCP_UPDATES_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 35, 2, 2);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_Y_PRESERVE_DSCPf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 35, 2, 3);

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_NEW_DSCPf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 7, 6, -1, offset + 0, 3, 3);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_DSCP_UPDATES_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 0, 3, 4);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_PRESERVE_DSCPf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 0, 3, 5);

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_NEW_TOSf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, offset + 7, 6, -1, offset + 0, 3, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_G_TOS_TO_PRE_FP_ING_OUTER_DOT1Pf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, offset + 0, 3, 2);

    /* IFP_PROTECTION_SWITCHING_SET(1b) */
    offset = protect_sw_offset;

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_PROTECTION_SWITCHING_DROP_OVERRIDEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                 0, 0, offset + 0, 1, 1);

    /* IFP_SFLOW_SET(1b) */
    offset = sflow_offset;

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_SFLOW_ENABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                 0, 0, offset + 0, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_MIRROR_ING_FLEX_SFLOW_IDf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                 0, 0, offset + 1, 6, -1);


    /* IFP_TIMESTAMP_SET(4b) */
    offset = timestamp_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_ING_TIMESTAMP_INSERTf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                             0, 0,
                             offset + 0, 2, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_ING_TIMESTAMP_INSERT_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                             0, 0,
                             offset + 0, 2, 2);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_EGR_TIMESTAMP_INSERTf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                             0, 0,
                             offset + 2, 2, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_EGR_TIMESTAMP_INSERT_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                             0, 0,
                             offset + 2, 2, 2);

    /* IFP_TTL_SET(1b) */
    offset = ttl_offset;

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_DO_NOT_CHANGE_TTLf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
                                 0, 0, offset + 0, 1, 1);


    /* IFP_COUNTER_SET */
    offset = ctr_offset;

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_FLEX_CTR_G_COUNTf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0,
            offset + 19, 2, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_FLEX_CTR_R_COUNTf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0,
            offset + 21, 2, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_FLEX_CTR_Y_COUNTf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0,
            offset + 23, 2, -1);
    fid = FP_ING_POLICY_TEMPLATEt_CTR_ING_EFLEX_OBJECTf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0,
            offset + 5, 14, -1);

    /* IFP_DELAYED_DROP_SET */
    offset = ddrop_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_FP_DELAYED_DROP_IDf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0,
            offset + 0, 7, -1);

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_DELAYED_DROP_ENABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0,
            offset + 7, 1, 1);

    /* IFP_DELAYED_REDIRECT_SET */
    offset = dredirect_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_FP_DELAYED_REDIRECT_IDf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0,
            offset + 0, 7, -1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_DELAYED_REDIRECT_ENABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0,
            offset + 7, 1, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_DELAYED_REDIRECT_PORTf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage,
                             fid,
                             0, 0,
                             offset + 8, 9, -1);

    /* IFP_DCN_ACTION_SET */
    offset = dcn_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_DCN_ENABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0,
            offset + 0, 2, 1);
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_DCN_DISABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0,
            offset + 0, 2, 2);

    /* GREEN_TO_PID ACTIONS */
    offset = green_to_pid_offset;
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_GREEN_TO_PIDf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0,
            offset + 0, 1, 1);

exit:
    SHR_FUNC_EXIT();
}
