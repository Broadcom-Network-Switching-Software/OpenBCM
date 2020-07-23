 /*
  * 
  *
  * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
  * 
  * Copyright 2007-2020 Broadcom Inc. All rights reserved.
  *
  * File:        field.c
  * Purpose:     Field Processor module routines specific to BCM56980(Tomahawk3)
 */
#include <shared/bsl.h>

#include <soc/defs.h>
#include <bcm_int/esw/tomahawk.h>

#if defined(BCM_TOMAHAWK3_SUPPORT) && defined(BCM_FIELD_SUPPORT)
#include <bcm_int/esw/field.h>
#include <bcm/field.h>
#include <bcm_int/esw/tomahawk3.h>
#if defined(BCM_ESW_SUPPORT)
#include <bcm_int/esw/xgs4.h>
#endif



/* UDF Chunk to Qualifier Mapping */
uint32 _bcm_th3_udf_chunk_to_qual_mapping[UDF_OFFSET_CHUNKS16] =
                        {
                           _bcmFieldQualifyData0,  /* Chunk 0  */
                           _bcmFieldQualifyData1,  /* Chunk 1  */
                           _bcmFieldQualifyData2,  /* Chunk 2  */
                           _bcmFieldQualifyData3,  /* Chunk 3  */
                           _bcmFieldQualifyData4,  /* Chunk 4  */
                           _bcmFieldQualifyData5,  /* Chunk 5  */
                           _bcmFieldQualifyData6,  /* Chunk 6  */
                           _bcmFieldQualifyData7,  /* Chunk 7  */
                           _bcmFieldQualifyData8,  /* Chunk 8  */
                           _bcmFieldQualifyData9,  /* Chunk 9  */
                           _bcmFieldQualifyData10, /* Chunk 10 */
                           _bcmFieldQualifyData11, /* Chunk 11 */
                           _bcmFieldQualifyData12, /* Chunk 12 */
                           _bcmFieldQualifyData13, /* Chunk 13 */
                           _bcmFieldQualifyData14, /* Chunk 14 */
                           _bcmFieldQualifyData15  /* Chunk 15 */
                        };

/*
 * Function:
 *    _field_th3_presel_qualifiers_init
 *
 * Purpose:
 *    Initialize Field Ingress Stage Preselector Qualifiers.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN/OUT) Field Processor stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_MEMORY      - Allocation failure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_th3_presel_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    /*
     * All the qualifiers offset mentioned are the actual individual HW field
     * offset in IFP_LOGICAL_TABLE_SELECT Memory which are starting from bit
     * position 1. "KEY" overlap field in the memory is used during write,
     * hence to accomidate in the field, the offsets of the qualifiers has to
     * be substracted by 1.
     */
    uint8 mixed_src_class_offset = 0;
    uint8 em_lt_id_offset = 0;
    uint8 cpu_tx_offset = 0;
    
    uint8 drop_offset = 0;
    uint8 pkt_res_offset = 0;
    uint8 fwd_type_offset = 0;
    uint8 lookup_status_offset = 0;
    uint8 mystation_hit_offset = 0;
    uint8 tunnel_type_offset = 0;
    uint8 lb_type_offset = 0;
    uint8 lb_offset = 0;
    uint8 l4pkt_offset = 0;
    uint8 ip_type_offset = 0;
    _FP_QUAL_DECL;


    /* Exact match stage qualifier offset are not same as in IFP presel.
     * Hence during init here need to take care of offset position
     * accordingly.
     */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        mixed_src_class_offset = 0;
        
        drop_offset = 33;
        cpu_tx_offset = 34;
        fwd_type_offset = 42;
        lookup_status_offset = 46;
        mystation_hit_offset = 65;
        tunnel_type_offset = 66;
        lb_type_offset = 66;
        lb_offset = 69;
        l4pkt_offset = 72;
        ip_type_offset = 73;
    } else if(stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        mixed_src_class_offset = 0;
        em_lt_id_offset = 32;
        
        cpu_tx_offset = 36;
        drop_offset = 47;
        pkt_res_offset = 48;
        fwd_type_offset = 54;
        lookup_status_offset = 58;
        mystation_hit_offset = 77;
        tunnel_type_offset = 78;
        lb_type_offset = 78;
        lb_offset = 81;
        l4pkt_offset = 84;
        ip_type_offset = 85;
    } else {
        return BCM_E_NONE;
    }

    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_presel_qual_arr,
        (_bcmFieldQualifyCount * sizeof(_bcm_field_qual_info_t *)),
        "IFP Preselector qualifiers");
    if (stage_fc->f_presel_qual_arr == 0) {
        return (BCM_E_MEMORY);
    }

    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMixedSrcClassId,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        mixed_src_class_offset, 32);

    _FP_PRESEL_QUAL_ADD(unit, stage_fc,
        _bcmFieldQualifyExactMatchGroupClassIdLookup0,
        _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
        _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        em_lt_id_offset, 4);

#if 0
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmfFieldQualifyCpuTxDestType,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        cpu_tx_dest_type_offset, 4);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmfFieldQualifyCpuTxFlowType,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        cpu_tx_flow_type, 2);
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCpuPktProfileCtrl,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        cpu_pkt_prof_ctrl_offset, 1);
#endif

    _FP_PRESEL_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyCpuTxCtrl,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        cpu_tx_offset, 6);

    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        drop_offset, 1);

    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
        _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
        _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        pkt_res_offset, 6);

    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        fwd_type_offset, 4);

    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        mystation_hit_offset, 1);

    
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopbackType,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        tunnel_type_offset, 4);

    
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelType,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        lb_type_offset, 4);

    
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopback,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        lb_offset, 1);

    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        l4pkt_offset, 1);

    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        ip_type_offset, 4);

    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        lookup_status_offset, 2);

    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStpState,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        lookup_status_offset, 2);

    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        lookup_status_offset + 2, 1);

    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        lookup_status_offset + 3, 1);

    
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
        _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
        _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        lookup_status_offset + 4, 1);

    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
        _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
        _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        lookup_status_offset + 5, 1);

   
#if 0
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsBosTerminated,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        lookup_status_offset + 6, 1);
#endif

    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        lookup_status_offset + 7, 1);

    return (BCM_E_NONE);
}

int
_bcm_field_th3_common_actions_init(int unit, _field_stage_t *stage_fc)
{
    _FP_ACTION_DECL
    uint32 offset = 0;                  /* General variable to carry offset. */
    uint32 profile_one_offset = 0;      /* Offset of PROFILE_ONE_SET. */
    uint32 profile_two_offset = 0;      /* Offset of PROFILE_TWO_SET. */
    uint32 redirect_offset = 0;         /* Offset of REDIRECT_SET. */
    uint32 l3swl2change_offset = 0;     /* Offset of L3SW_CHANGE_L2_SET. */
    uint32 mirror_offset = 0;           /* Offset of MIRROR_SET. */
    uint32 lb_offset = 0;               /* Offset of LB_CONTROL_SET. */
    uint32 copytocpu_offset = 0;        /* Offset of COPY_TO_CPU_SET. */
    uint32 cutthrough_offset = 0;       /* Offset of CUT_THRU_OVERRIDE_SET. */
    uint32 ttl_offset = 0;              /* Offset of TTL_SET. */
    uint32 cpucos_offset = 0;           /* Offset of CHANGE_CPU_COS_SET. */
    uint32 drop_offset = 0;             /* Offset of DROP_SET. */
    uint32 mirror_override_offset = 0;  /* Offset of MIRROR_OVERRIDE_SET. */
    uint32 sflow_offset = 0;            /* Offset of SFLOW_SET. */
    uint32 debug_offset = 0;            /* Offset of INSTRUMENTATION_TRIGGERS_ENABLE. */
    uint32 ddrop_offset = 0;            /* Offset of DELAYED_DROP_SET. */
    uint32 dredirect_offset = 0;        /* Offset of DELAYED_REDIRECT_SET. */
    uint32 dlb_alt_path_offset = 0;     /* Offset of DLB_ALTERNATE_PATH_CONTROL_SET. */
    uint32 dlb_ecmp_offset = 0;         /* Offset of DLB_ECMP_SET. */
    uint32 dlb_ecmp_monitor_offset = 0; /* Offset of DLB_ECMP_MONITOR_SET. */
    uint32 timestamp_offset = 0;        /* Offset of TIMESTAMP_SET. */
    uint32 etrap_offset = 0;            /* Offset of ELEPHANT_TRAP_SET. */
    uint32 int_offset = 0;              /* Offset of INBAND_TELEMETRY_SET. */


    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        redirect_offset        = 0;  /* Start of the IFP_POLICY_TABLE */
        cpucos_offset          = 38; /* REDIRECT_SET(38b) */
        copytocpu_offset       = 46; /* REDIRECT_SET(38b) +
                                      * CHANGE_CPU_COS_SET(8b) */
        cutthrough_offset      = 88; /* REDIRECT_SET(38b) +
                                      * CHANGE_CPU_COS_SET(8b) +
                                      * COPY_TO_CPU_SET(17b) +
                                      * COUNTER_SET(25b) */
        ddrop_offset           = 89; /* REDIRECT_SET(38b) +
                                      * CHANGE_CPU_COS_SET(8b) +
                                      * COPY_TO_CPU_SET(17b) +
                                      * COUNTER_SET(25b) +
                                      * CUT_THRU_OVERRIDE_SET(1b) */
        dredirect_offset       = 97; /* REDIRECT_SET(38b) +
                                      * CHANGE_CPU_COS_SET(8b) +
                                      * COPY_TO_CPU_SET(17b) +
                                      * COUNTER_SET(25b) +
                                      * CUT_THRU_OVERRIDE_SET(1b) +
                                      * DELAYED_DROP_SET(8b) */
        dlb_alt_path_offset     = 113; /* REDIRECT_SET(38b) +
                                        * CHANGE_CPU_COS_SET(8b) +
                                        * COPY_TO_CPU_SET(17b) +
                                        * COUNTER_SET(25b) +
                                        * CUT_THRU_OVERRIDE_SET(1b) +
                                        * DELAYED_DROP_SET(8b) +
                                        * DELAYED_REDIRECT_SET(16b) */
        dlb_ecmp_monitor_offset = 123; /* REDIRECT_SET(38b) +
                                        * CHANGE_CPU_COS_SET(8b) +
                                        * COPY_TO_CPU_SET(17b) +
                                        * COUNTER_SET(25b) +
                                        * CUT_THRU_OVERRIDE_SET(1b) +
                                        * DELAYED_DROP_SET(8b) +
                                        * DELAYED_REDIRECT_SET(16b) +
                                        * DLB_ALTERNATE_PATH_CONTROL_SET(10b) */
        dlb_ecmp_offset         = 125; /* REDIRECT_SET(38b) +
                                        * CHANGE_CPU_COS_SET(8b) +
                                        * COPY_TO_CPU_SET(17b) +
                                        * COUNTER_SET(25b) +
                                        * CUT_THRU_OVERRIDE_SET(1b) +
                                        * DELAYED_DROP_SET(8b) +
                                        * DELAYED_REDIRECT_SET(16b) +
                                        * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                        * DLB_ECMP_MONITOR_SET(2b) */
        drop_offset             = 127; /* REDIRECT_SET(38b) +
                                        * CHANGE_CPU_COS_SET(8b) +
                                        * COPY_TO_CPU_SET(17b) +
                                        * COUNTER_SET(25b) +
                                        * CUT_THRU_OVERRIDE_SET(1b) +
                                        * DELAYED_DROP_SET(8b) +
                                        * DELAYED_REDIRECT_SET(16b) +
                                        * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                        * DLB_ECMP_MONITOR_SET(2b) +
                                        * DLB_ECMP_SET(2b) */
        etrap_offset            = 133; /* REDIRECT_SET(38b) +
                                        * CHANGE_CPU_COS_SET(8b) +
                                        * COPY_TO_CPU_SET(17b) +
                                        * COUNTER_SET(25b) +
                                        * CUT_THRU_OVERRIDE_SET(1b) +
                                        * DELAYED_DROP_SET(8b) +
                                        * DELAYED_REDIRECT_SET(16b) +
                                        * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                        * DLB_ECMP_MONITOR_SET(2b) +
                                        * DLB_ECMP_SET(2b) +
                                        * DROP_SET(6b) */
        int_offset              = 140; /* REDIRECT_SET(38b) +
                                        * CHANGE_CPU_COS_SET(8b) +
                                        * COPY_TO_CPU_SET(17b) +
                                        * COUNTER_SET(25b) +
                                        * CUT_THRU_OVERRIDE_SET(1b) +
                                        * DELAYED_DROP_SET(8b) +
                                        * DELAYED_REDIRECT_SET(16b) +
                                        * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                        * DLB_ECMP_MONITOR_SET(2b) +
                                        * DLB_ECMP_SET(2b) +
                                        * DROP_SET(6b) +
                                        * ELEPHANT_TRAP_SET(6b) +
                                        * GREEN_TO_PID_SET(1b) */
        debug_offset           = 142;  /* REDIRECT_SET(38b) +
                                        * CHANGE_CPU_COS_SET(8b) +
                                        * COPY_TO_CPU_SET(17b) +
                                        * COUNTER_SET(25b) +
                                        * CUT_THRU_OVERRIDE_SET(1b) +
                                        * DELAYED_DROP_SET(8b) +
                                        * DELAYED_REDIRECT_SET(16b) +
                                        * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                        * DLB_ECMP_MONITOR_SET(2b) +
                                        * DLB_ECMP_SET(2b) +
                                        * DROP_SET(6b) +
                                        * ELEPHANT_TRAP_SET(6b) +
                                        * GREEN_TO_PID_SET(1b) +
                                        * INBAND_TELEMETRY_SET(2b) */
        l3swl2change_offset    = 143;  /* REDIRECT_SET(38b) +
                                        * CHANGE_CPU_COS_SET(8b) +
                                        * COPY_TO_CPU_SET(17b) +
                                        * COUNTER_SET(25b) +
                                        * CUT_THRU_OVERRIDE_SET(1b) +
                                        * DELAYED_DROP_SET(8b) +
                                        * DELAYED_REDIRECT_SET(16b) +
                                        * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                        * DLB_ECMP_MONITOR_SET(2b) +
                                        * DLB_ECMP_SET(2b) +
                                        * DROP_SET(6b) +
                                        * ELEPHANT_TRAP_SET(6b) +
                                        * GREEN_TO_PID_SET(1b) +
                                        * INBAND_TELEMETRY_SET(2b) +
                                        * INSTRUMENTATION_SET(1b) */
        lb_offset              = 166;  /* REDIRECT_SET(38b) +
                                        * CHANGE_CPU_COS_SET(8b) +
                                        * COPY_TO_CPU_SET(17b) +
                                        * COUNTER_SET(25b) +
                                        * CUT_THRU_OVERRIDE_SET(1b) +
                                        * DELAYED_DROP_SET(8b) +
                                        * DELAYED_REDIRECT_SET(16b) +
                                        * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                        * DLB_ECMP_MONITOR_SET(2b) +
                                        * DLB_ECMP_SET(2b) +
                                        * DROP_SET(6b) +
                                        * ELEPHANT_TRAP_SET(6b) +
                                        * GREEN_TO_PID_SET(1b) +
                                        * INBAND_TELEMETRY_SET(2b) +
                                        * INSTRUMENTATION_SET(1b) +
                                        * L3SW_CHANGE_L2_SET(23b) */
        mirror_override_offset = 184;  /* REDIRECT_SET(38b) +
                                        * CHANGE_CPU_COS_SET(8b) +
                                        * COPY_TO_CPU_SET(17b) +
                                        * COUNTER_SET(25b) +
                                        * CUT_THRU_OVERRIDE_SET(1b) +
                                        * DELAYED_DROP_SET(8b) +
                                        * DELAYED_REDIRECT_SET(16b) +
                                        * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                        * DLB_ECMP_MONITOR_SET(2b) +
                                        * DLB_ECMP_SET(2b) +
                                        * DROP_SET(6b) +
                                        * ELEPHANT_TRAP_SET(6b) +
                                        * GREEN_TO_PID_SET(1b) +
                                        * INBAND_TELEMETRY_SET(2b) +
                                        * INSTRUMENTATION_SET(1b) +
                                        * L3SW_CHANGE_L2_SET(23b) +
                                        * LB_CONTROLS_SET(2b) +
                                        * METER_SET(16b) */
        mirror_offset          = 185;  /* REDIRECT_SET(38b) +
                                        * CHANGE_CPU_COS_SET(8b) +
                                        * COPY_TO_CPU_SET(17b) +
                                        * COUNTER_SET(25b) +
                                        * CUT_THRU_OVERRIDE_SET(1b) +
                                        * DELAYED_DROP_SET(8b) +
                                        * DELAYED_REDIRECT_SET(16b) +
                                        * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                        * DLB_ECMP_MONITOR_SET(2b) +
                                        * DLB_ECMP_SET(2b) +
                                        * DROP_SET(6b) +
                                        * ELEPHANT_TRAP_SET(6b) +
                                        * GREEN_TO_PID_SET(1b) +
                                        * INBAND_TELEMETRY_SET(2b) +
                                        * INSTRUMENTATION_SET(1b) +
                                        * L3SW_CHANGE_L2_SET(23b) +
                                        * LB_CONTROLS_SET(2b) +
                                        * METER_SET(16b) +
                                        * MIRROR_OVERRIDE_SET(1b) */
        profile_one_offset     = 200;  /* REDIRECT_SET(38b) +
                                        * CHANGE_CPU_COS_SET(8b) +
                                        * COPY_TO_CPU_SET(17b) +
                                        * COUNTER_SET(25b) +
                                        * CUT_THRU_OVERRIDE_SET(1b) +
                                        * DELAYED_DROP_SET(8b) +
                                        * DELAYED_REDIRECT_SET(16b) +
                                        * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                        * DLB_ECMP_MONITOR_SET(2b) +
                                        * DLB_ECMP_SET(2b) +
                                        * DROP_SET(6b) +
                                        * ELEPHANT_TRAP_SET(6b) +
                                        * GREEN_TO_PID_SET(1b) +
                                        * INBAND_TELEMETRY_SET(2b) +
                                        * INSTRUMENTATION_SET(1b) +
                                        * L3SW_CHANGE_L2_SET(23b) +
                                        * LB_CONTROLS_SET(2b) +
                                        * METER_SET(16b) +
                                        * MIRROR_OVERRIDE_SET(1b) +
                                        * MIRROR_SET(15b) */
        profile_two_offset     = 256;  /* REDIRECT_SET(38b) +
                                        * CHANGE_CPU_COS_SET(8b) +
                                        * COPY_TO_CPU_SET(17b) +
                                        * COUNTER_SET(25b) +
                                        * CUT_THRU_OVERRIDE_SET(1b) +
                                        * DELAYED_DROP_SET(8b) +
                                        * DELAYED_REDIRECT_SET(16b) +
                                        * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                        * DLB_ECMP_MONITOR_SET(2b) +
                                        * DLB_ECMP_SET(2b) +
                                        * DROP_SET(6b) +
                                        * ELEPHANT_TRAP_SET(6b) +
                                        * GREEN_TO_PID_SET(1b) +
                                        * INBAND_TELEMETRY_SET(2b) +
                                        * INSTRUMENTATION_SET(1b) +
                                        * L3SW_CHANGE_L2_SET(23b) +
                                        * LB_CONTROLS_SET(2b) +
                                        * METER_SET(16b) +
                                        * MIRROR_OVERRIDE_SET(1b) +
                                        * MIRROR_SET(15b) +
                                        * PROFILE_SET_1(56b) */
        sflow_offset           = 313;  /* REDIRECT_SET(38b) +
                                        * CHANGE_CPU_COS_SET(8b) +
                                        * COPY_TO_CPU_SET(17b) +
                                        * COUNTER_SET(25b) +
                                        * CUT_THRU_OVERRIDE_SET(1b) +
                                        * DELAYED_DROP_SET(8b) +
                                        * DELAYED_REDIRECT_SET(16b) +
                                        * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                        * DLB_ECMP_MONITOR_SET(2b) +
                                        * DLB_ECMP_SET(2b) +
                                        * DROP_SET(6b) +
                                        * ELEPHANT_TRAP_SET(6b) +
                                        * GREEN_TO_PID_SET(1b) +
                                        * INBAND_TELEMETRY_SET(2b) +
                                        * INSTRUMENTATION_SET(1b) +
                                        * L3SW_CHANGE_L2_SET(23b) +
                                        * LB_CONTROLS_SET(2b) +
                                        * METER_SET(16b) +
                                        * MIRROR_OVERRIDE_SET(1b) +
                                        * MIRROR_SET(15b) +
                                        * PROFILE_SET_1(56b) +
                                        * PROFILE_SET_2(52b) +
                                        * PROTECTION_SWITCHING_SET(1b) +
                                        * RESERVED_SET(4b) */

        timestamp_offset       = 314;  /* REDIRECT_SET(38b) +
                                        * CHANGE_CPU_COS_SET(8b) +
                                        * COPY_TO_CPU_SET(17b) +
                                        * COUNTER_SET(25b) +
                                        * CUT_THRU_OVERRIDE_SET(1b) +
                                        * DELAYED_DROP_SET(8b) +
                                        * DELAYED_REDIRECT_SET(16b) +
                                        * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                        * DLB_ECMP_MONITOR_SET(2b) +
                                        * DLB_ECMP_SET(2b) +
                                        * DROP_SET(6b) +
                                        * ELEPHANT_TRAP_SET(6b) +
                                        * GREEN_TO_PID_SET(1b) +
                                        * INBAND_TELEMETRY_SET(2b) +
                                        * INSTRUMENTATION_SET(1b) +
                                        * L3SW_CHANGE_L2_SET(23b) +
                                        * LB_CONTROLS_SET(2b) +
                                        * METER_SET(16b) +
                                        * MIRROR_OVERRIDE_SET(1b) +
                                        * MIRROR_SET(15b) +
                                        * PROFILE_SET_1(56b) +
                                        * PROFILE_SET_2(52b) +
                                        * PROTECTION_SWITCHING_SET(1b) +
                                        * RESERVED_SET(4b) +
                                        * SFLOW_SET(1b) */

        ttl_offset             = 318;  /* REDIRECT_SET(38b) +
                                        * CHANGE_CPU_COS_SET(8b) +
                                        * COPY_TO_CPU_SET(17b) +
                                        * COUNTER_SET(25b) +
                                        * CUT_THRU_OVERRIDE_SET(1b) +
                                        * DELAYED_DROP_SET(8b) +
                                        * DELAYED_REDIRECT_SET(16b) +
                                        * DLB_ALTERNATE_PATH_CONTROL_SET(10b) +
                                        * DLB_ECMP_MONITOR_SET(2b) +
                                        * DLB_ECMP_SET(2b) +
                                        * DROP_SET(6b) +
                                        * ELEPHANT_TRAP_SET(6b) +
                                        * GREEN_TO_PID_SET(1b) +
                                        * INBAND_TELEMETRY_SET(2b) +
                                        * INSTRUMENTATION_SET(1b) +
                                        * L3SW_CHANGE_L2_SET(23b) +
                                        * LB_CONTROLS_SET(2b) +
                                        * METER_SET(16b) +
                                        * MIRROR_OVERRIDE_SET(1b) +
                                        * MIRROR_SET(15b) +
                                        * PROFILE_SET_1(56b) +
                                        * PROFILE_SET_2(52b) +
                                        * PROTECTION_SWITCHING_SET(1b) +
                                        * RESERVED_SET(4b) +
                                        * SFLOW_SET(1b) +
                                        * TIMESTAMP_SET(4b) */
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        soc_field_info_t * f_info;
        f_info = soc_mem_fieldinfo_get(unit, EXACT_MATCH_QOS_ACTIONS_PROFILEm, PROFILE_SET_2f);
        profile_two_offset = f_info->bp;
    }

    offset = redirect_offset;
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc,
                                  bcmFieldActionRedirect,
                                  _BCM_FIELD_ACTION_REDIRECT_DGLP,
                                  _FieldActionRedirectSet,
                                  offset + 6, 8, -1,
                                  offset + 14 , 1, 0, offset + 3, 3, 0,
                                  offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc,
                                  bcmFieldActionUnmodifiedPacketRedirectPort,
                                  _BCM_FIELD_ACTION_REDIRECT_DGLP,
                                  _FieldActionRedirectSet,
                                  offset + 6, 8, -1,
                                  offset + 14 , 1, 0, offset + 3, 3, 1,
                                  offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc,
                                  bcmFieldActionUnmodifiedPacketRedirectPort,
                                  _BCM_FIELD_ACTION_REDIRECT_TRUNK,
                                  _FieldActionRedirectSet,
                                  offset + 6, 8, -1,
                                  offset + 14 , 1, 0, offset + 3, 3, 1,
                                  offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc, bcmFieldActionRedirect,
                                  _BCM_FIELD_ACTION_REDIRECT_TRUNK,
                                  _FieldActionRedirectSet,
                                  offset + 6, 8, -1, offset + 14, 1, 1,
                                  offset + 3, 3, 0, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc,
                                  bcmFieldActionRedirectTrunk,
                                  _BCM_FIELD_ACTION_REDIRECT_TRUNK,
                                  _FieldActionRedirectSet,
                                  offset + 6, 8, -1, offset + 14, 1, 1,
                                  offset + 3, 3, 0, offset + 0, 3, 1);

    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc,
                                   bcmFieldActionRedirectEgrNextHop,
                                   _BCM_FIELD_ACTION_REDIRECT_NEXT_HOP,
                                   _FieldActionRedirectSet,
                                   offset + 6, 16, -1,
                                   offset + 3, 3, 2, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc,
                                   bcmFieldActionRedirectEgrNextHop,
                                   _BCM_FIELD_ACTION_REDIRECT_ECMP,
                                   _FieldActionRedirectSet,
                                   offset + 6, 12, -1,
                                   offset + 3, 3, 3, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRedirectCancel,
                                 0, _FieldActionRedirectSet, offset + 0, 3, 2);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc,
                                   bcmFieldActionRedirectPbmp, 0,
                                   _FieldActionRedirectSet,
                                   offset + 6, 8, -1, offset + 23, 2, 0,
                                   offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc,
                                   bcmFieldActionRedirectVlan, 0,
                                   _FieldActionRedirectSet,
                                   offset + 17, 1, 0, offset + 23, 2, 1,
                                   offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc,
                                  bcmFieldActionRedirectBcastPbmp, 0,
                                  _FieldActionRedirectSet,
                                  offset + 6, 8, -1, offset + 17, 1, 1,
                                  offset + 23, 2, 1, offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc,
                                   bcmFieldActionRedirectMcast, 0,
                                   _FieldActionRedirectSet,
                                   offset + 6, 10, -1, offset + 23, 2, 2,
                                   offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc,
                                   bcmFieldActionRedirectIpmc, 0,
                                   _FieldActionRedirectSet,
                                   offset + 6, 9, -1, offset + 23, 2, 3,
                                   offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc,
                                 bcmFieldActionEgressMask, 0,
                                 _FieldActionRedirectSet,
                                 offset + 6, 8, -1, offset + 0, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc,
                                 bcmFieldActionEgressPortsAdd, 0,
                                 _FieldActionRedirectSet,
                                 offset + 6, 8, -1, offset + 0, 3, 5);

    /* IFP_CHANGE_CPU_COS_SET(8b) */
    offset = cpucos_offset;
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc,
                                 bcmFieldActionCosQCpuNew, 0,
                                 _FieldActionChangeCpuCosSet,
                                 offset + 2, 6, -1, offset + 0, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc,
                                   bcmFieldActionServicePoolIdNew, 0,
                                   _FieldActionChangeCpuCosSet,
                                   offset + 4, 2, -1, offset + 6, 2, 2,
                                   offset + 0, 2, 2);
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc,
                                  bcmFieldActionServicePoolIdPrecedenceNew, 0,
                                  _FieldActionChangeCpuCosSet,
                                  offset + 4, 2, -1, offset + 2, 2, -1,
                                  offset + 6, 2, 3, offset + 0, 2, 2);

    /* IFP_COPY_TO_CPU_SET(17b) */
    offset = copytocpu_offset;

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpCopyToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 3, 8, -1, offset + 11, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpCopyToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 11, 3, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpSwitchToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 11, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpSwitchToCpuReinstate,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 11, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpTimeStampToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 3, 8, -1, offset + 11, 3, 5);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpCopyToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 3, 8, -1, offset + 14, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpCopyToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 14, 3, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpSwitchToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 14, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpSwitchToCpuReinstate,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 14, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpTimeStampToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 3, 8, -1, offset + 14, 3, 5);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpCopyToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 3, 8, -1, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpCopyToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 0, 3, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpSwitchToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpSwitchToCpuReinstate,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 0, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpTimeStampToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 3, 8, -1, offset + 0, 3, 5);

    /* IFP_CUT_THRU_OVERRIDE_SET(1b) */
    offset = cutthrough_offset;
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDoNotCutThrough,
                                 0, _FieldActionCutThrOverrideSet, offset + 0, 1, 1);


    /* DLB_ALTERNATE_PATH_CONTROL_SET(10b) */
    offset = dlb_alt_path_offset;
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDgm,
                                 0, _FieldActionIfpDlbAlternatePathControlSet,
                                 offset + 0, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDgmThreshold,
                                 0, _FieldActionIfpDlbAlternatePathControlSet,
                                 offset + 7, 3, -1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDgmCost,
                                 0, _FieldActionIfpDlbAlternatePathControlSet,
                                 offset + 4, 3, -1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDgmBias,
                                 0, _FieldActionIfpDlbAlternatePathControlSet,
                                 offset + 1, 3, -1);

    /* DLB_ECMP_MONITOR_SET(2b) */
    offset = dlb_ecmp_monitor_offset;
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDlbEcmpMonitorEnable,
                                 0, _FieldActionEcmpDlbMonitorSet,
                                 offset + 0, 2, 0);

    /* DLB_ECMP_SET(2b) */
    offset = dlb_ecmp_offset;
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDynamicEcmpCancel,
                            0, _FieldActionEcmpDlbActionSet, offset + 0, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDynamicEcmpEnable,
                            0, _FieldActionEcmpDlbActionSet, offset + 1, 1, 1);

    /* IFP_DROP_SET(6b) */
    offset = drop_offset;
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpDrop,
                                 0, _FieldActionDropSet, offset + 2, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpDropCancel,
                                 0, _FieldActionDropSet, offset + 2, 2, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpDrop,
                                 0, _FieldActionDropSet, offset + 4, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpDropCancel,
                                 0, _FieldActionDropSet,  offset + 4, 2, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpDrop,
                                 0, _FieldActionDropSet, offset + 0, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpDropCancel,
                                 0, _FieldActionDropSet, offset + 0, 2, 2);

    /* IFP_INSTRUMENTATION_TRIGGERS_ENABLE_SET(1b)*/
    offset = debug_offset;
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPacketTraceEnable,
                                       _BCM_FIELD_ACTION_NO_EM_SUPPORT,
                                       _FieldActionInstrumentationSet,
                                       offset + 0, 1, 1);

    /* IFP_L3SW_CHANGE_L2_SET(23b) */
    offset = l3swl2change_offset;
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionChangeL2Fields,
                                 0, _FieldActionL3SwChangeL2Set,
                                 offset + 0, 16, -1, offset + 19, 4, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
                                 bcmFieldActionChangeL2FieldsCancel,
                                 0, _FieldActionL3SwChangeL2Set,
                                 offset + 19, 4, 2);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionL3Switch,
                                   _BCM_FIELD_ACTION_L3SWITCH_NEXT_HOP,
                                   _FieldActionL3SwChangeL2Set,
                                   offset + 0, 16, -1, offset + 17, 1, 0,
                                   offset + 19, 4, 6);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionL3Switch,
                                   _BCM_FIELD_ACTION_L3SWITCH_ECMP,
                                   _FieldActionL3SwChangeL2Set,
                                   offset + 0, 12, -1, offset + 17, 1, 1,
                                   offset + 19, 4, 6);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionMultipathHash,
                                 0, _FieldActionL3SwChangeL2Set,
                                 offset + 12, 3, -1, offset + 19, 4, 6);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionL3SwitchCancel,
                                 0, _FieldActionL3SwChangeL2Set,
                                 offset + 19, 4, 7);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc,
                                 bcmFieldActionEgressClassSelect,
                                 0, _FieldActionL3SwChangeL2Set,
                                 offset + 0, 4, -1, offset + 19, 4, 8);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc,
                                 bcmFieldActionNewClassId, 0,
                                 _FieldActionL3SwChangeL2Set,
                                 offset + 7, 9, -1, offset + 19, 4, 8);

    
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc,
                                 bcmFieldActionBFDSessionIdNew, 0,
                                 _FieldActionL3SwChangeL2Set,
                                 offset + 0, 12, -1, offset + 19, 4, 9);

    /* IFP_LB_CONTROLS_SET(3b) */
    offset = lb_offset;
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
                       bcmFieldActionTrunkRandomRoundRobinHashCancel,
                       0, _FieldActionLbControlSet,
                       offset + 0, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
                       bcmFieldActionEcmpRandomRoundRobinHashCancel,
                       0, _FieldActionLbControlSet,
                       offset + 1, 1, 1);

    /* IFP_MIRROR_OVERRIDE_SET(1b) */
    offset = mirror_override_offset;

     _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionMirrorOverride,
                                 0, _FieldActionMirrorOverrideSet, offset + 0, 1, 1);

    /* IFP_MIRROR_SET(12b) */
    offset = mirror_offset;

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionMirrorIngress,
                                          0, _FieldActionMirrorSet,
                                          offset, 4, 0, offset + 7 , 8, 0);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionMirrorEgress,
                                          0, _FieldActionMirrorSet,
                                          offset, 4, 0, offset + 7, 8, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionMirrorZeroingEnable,
                                          0, _FieldActionMirrorSet,
                                          offset + 4, 3, -1);

    /* ExactMatch Action Class Id (12b) */
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionExactMatchClassId,
                       _BCM_FIELD_ACTION_NO_IFP_SUPPORT, 0, 12, 0);

    /* IFP_PROFILE_SET_1(56b) */
    offset = profile_one_offset;
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionUntaggedPacketPriorityNew,
                                      0, offset + 18, 4, -1, offset + 0, 1, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpDropPrecedence,
                                                         0, offset + 35, 2, -1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpDropPrecedence,
                                                         0, offset + 52, 2, -1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpDropPrecedence,
                                                         0, offset + 14, 2, -1);

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionRpCosQNew,
                                     0, offset + 27, 8, -1, offset + 22, 4, 1);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionRpCosMapNew,
                                     0, offset + 27, 2, -1, offset + 22, 4, 2);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionRpPrioIntNew,
                                     0, offset + 27, 8, -1, offset + 22, 4, 5);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioIntTos,
                                                         0, offset + 22, 4, 6);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioIntCancel,
                                                         0, offset + 22, 4, 7);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionRpUcastCosQNew,
                                     0, offset + 27, 4, -1, offset + 22, 4, 8);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionRpMcastCosQNew,
                                     0, offset + 31, 4, -1, offset + 22, 4, 9);

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionYpCosQNew,
                                     0, offset + 44, 8, -1, offset + 39, 4, 1);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionYpCosMapNew,
                                     0, offset + 44, 2, -1, offset + 39, 4, 2);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionYpPrioIntNew,
                                     0, offset + 44, 8, -1, offset + 39, 4, 5);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioIntTos,
                                                         0, offset + 39, 4, 6);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioIntCancel,
                                                         0, offset + 39, 4, 7);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionYpUcastCosQNew,
                                     0, offset + 44, 4, -1, offset + 39, 4, 8);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionYpMcastCosQNew,
                                     0, offset + 48, 4, -1, offset + 39, 4, 9);

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpCosQNew,
                                       0, offset + 6, 8, -1, offset + 1, 4, 1);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpCosMapNew,
                                       0, offset + 6, 2, -1, offset + 1, 4, 2);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpPrioIntNew,
                                       0, offset + 6, 8, -1, offset + 1, 4, 5);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioIntTos,
                                                          0, offset + 1, 4, 6);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioIntCancel,
                                                          0, offset + 1, 4, 7);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpUcastCosQNew,
                                       0, offset + 6, 4, -1, offset + 1, 4, 8);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpMcastCosQNew,
                                      0, offset + 10, 4, -1, offset + 1, 4, 9);

    if (soc_feature(unit, soc_feature_ecn_wred)) {
        _FP_ACTION_ADD_TWO(unit, stage_fc,
                           bcmFieldActionGpIntCongestionNotificationNew,
                           _BCM_FIELD_ACTION_NO_EM_SUPPORT,
                           offset + 16, 2, -1, offset + 5, 1, 1);
        _FP_ACTION_ADD_TWO(unit, stage_fc,
                           bcmFieldActionYpIntCongestionNotificationNew,
                           _BCM_FIELD_ACTION_NO_EM_SUPPORT,
                           offset + 54, 2, -1, offset + 43, 1, 1);
       _FP_ACTION_ADD_TWO(unit, stage_fc,
                          bcmFieldActionRpIntCongestionNotificationNew,
                          _BCM_FIELD_ACTION_NO_EM_SUPPORT,
                          offset + 37, 2, -1, offset + 26, 1, 1);
    }

    if (soc_feature(unit, soc_feature_field_action_pfc_class)) {
       _FP_ACTION_ADD_TWO(unit, stage_fc,
                          bcmFieldActionPfcClassNew,
                          0, offset + 18, 4, -1, offset + 0, 1, 1);
    }

    /* IFP_PROFILE_SET_2(52b) */
    offset = profile_two_offset;

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionRpEcnNew,
                                      0, offset + 30, 2, -1, offset + 20, 1, 1);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionYpEcnNew,
                                      0, offset + 47, 2, -1, offset + 37, 1, 1);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpEcnNew,
                                       0, offset + 13, 2, -1, offset + 3, 1, 1);

    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpDot1pPreserve,
                                                          0, offset + 21, 3, 3);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionRpPrioPktNew,
                                      0, offset + 32, 3, -1, offset + 21, 3, 5);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktTos,
                                                          0, offset + 21, 3, 6);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktCancel,
                                                          0, offset + 21, 3, 7);

    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpDot1pPreserve,
                                                          0, offset + 38, 3, 3);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionYpPrioPktNew,
                                      0, offset + 49, 3, -1, offset + 38, 3, 5);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktTos,
                                                          0, offset + 38, 3, 6);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktCancel,
                                                          0, offset + 38, 3, 7);

    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpDot1pPreserve,
                                                           0, offset + 4, 3, 3);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpPrioPktNew,
                                       0, offset + 15, 3, -1, offset + 4, 3, 5);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktTos,
                                                           0, offset + 4, 3, 6);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktCancel,
                                                           0, offset + 4, 3, 7);

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionRpDscpNew,
                                      0, offset + 24, 6, -1, offset + 18, 2, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpDscpCancel,
                                                          0, offset + 18, 2, 2);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpDscpPreserve,
                                                          0, offset + 18, 2, 3);

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionYpDscpNew,
                                      0, offset + 41, 6, -1, offset + 35, 2, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpDscpCancel,
                                                          0, offset + 35, 2, 2);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpDscpPreserve,
                                                          0, offset + 35, 2, 3);

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpDscpNew,
                                      0, offset + 7, 6, -1, offset + 0, 3, 3);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpDscpCancel,
                                                          0, offset + 0, 3, 4);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpDscpPreserve,
                                                          0, offset + 0, 3, 5);

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpTosPrecedenceNew,
                                      0, offset + 7, 6, -1, offset + 0, 3, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpTosPrecedenceCopy,
                                                          0, offset + 0, 3, 2);
    /* IFP_SFLOW_SET(1b) */
    offset = sflow_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionIngSampleEnable,
                                 0, _FieldActionSflowSet, offset + 0, 1, 1);

    /* TIMESTAMP_SET(4b) */
    offset = timestamp_offset;
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionIngressTimeStampInsert,
                                        0, _FieldActionTimestampInsertionSet,
                                        offset + 0, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionIngressTimeStampInsertCancel,
                                        0, _FieldActionTimestampInsertionSet,
                                        offset + 0, 2, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionEgressTimeStampInsert,
                                        0, _FieldActionTimestampInsertionSet,
                                        offset + 2, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionEgressTimeStampInsertCancel,
                                        0, _FieldActionTimestampInsertionSet,
                                        offset + 2, 2, 2);

    /* IFP_TTL_SET(1b) */
    offset = ttl_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDoNotChangeTtl,
                                 0, _FieldActionTtlOverrideSet, offset + 0, 1, 1);


    /* All actions which have corresponding Colored(Gp/Yp/Rp) Actions needs to be
     * initialized with some default configuration(offset = 0, width = 0, value = 0).
     */
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionCosQNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioIntNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioIntTos, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioIntCancel, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionUcastCosQNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionMcastCosQNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionDropPrecedence, 0, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionColorIndependent,
                                 0, _FieldActionGreenToPidSet, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionEcnNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktTos, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktCancel, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionDscpNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionDscpCancel, 0, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionCopyToCpu,
                                 0, _FieldActionCopyToCpuSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionCopyToCpuCancel,
                                 0, _FieldActionCopyToCpuSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionSwitchToCpuCancel,
                                 0, _FieldActionCopyToCpuSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionSwitchToCpuReinstate,
                                 0, _FieldActionCopyToCpuSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionTimeStampToCpu,
                                 0, _FieldActionCopyToCpuSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDrop,
                                 0, _FieldActionDropSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDropCancel,
                                 0, _FieldActionDropSet, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionCosMapNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionDscpPreserve, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionDot1pPreserve, 0, 0, 0, 0);

    /* ELEPHANT_TRAP_SET */
    offset = etrap_offset;
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionElephantColorDisable,
                                        0, _FieldActionElephantTrapSet,
                                        offset + 0, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionElephantColorEnable,
                                        0, _FieldActionElephantTrapSet,
                                        offset + 1, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionElephantLookupDisable,
                                        0, _FieldActionElephantTrapSet,
                                        offset + 2, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionElephantLookupEnable,
                                        0, _FieldActionElephantTrapSet,
                                        offset + 3, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionElephantQueueDisable,
                                        0, _FieldActionElephantTrapSet,
                                        offset + 4, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionElephantQueueEnable,
                                        0, _FieldActionElephantTrapSet,
                                        offset + 5, 1, 1);
    /* INBAND_TELEMETRY_SET */
    offset = int_offset;
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionIntEncapDisable,
                                     0, _FieldActionInbandTelemetrySet,
                                     offset + 0, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionIntEncapEnable,
                                     0, _FieldActionInbandTelemetrySet,
                                     offset + 1, 1, 1);

    /* DELAYED ACTIONS */
    offset = ddrop_offset;
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionMatchPbmpDrop,
                                 0, _FieldActionDelayedDropSet,
                                 offset + 0, 7, -1, offset + 7, 1, 1);
    offset = dredirect_offset;
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc,
                                   bcmFieldActionMatchPbmpRedirect,
                                   0, _FieldActionDelayedRedirectSet,
                                   offset + 0, 7, -1, offset + 8, 8, -1,
                                   offset + 7, 1, 1);

    return BCM_E_NONE;
}

int
_bcm_field_th3_extractors_init(int unit, _field_stage_t *stage_fc)
{
    int level = 0;                   /* Extractor hierarchy level.          */
    int gran = 0;                    /* Extractor granularity.              */
    int ext_num = 0;                 /* Extractor number.                   */
    int part;                        /* Entry part number.                  */
    bcmi_keygen_ext_attrs_t ext_attrs;
    bcmi_keygen_ext_attrs_t temp_attrs;
    bcmi_keygen_ext_attrs_t *attrs = &temp_attrs;
    uint32 ext_attrs_size = 0;
    BCMI_KEYGEN_EXT_CFG_DECL;              /* Extractors config declaration.      */
    bcmi_keygen_ext_cfg_db_t *ext_cfg_db = NULL;
    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(stage_fc, BCM_E_PARAM);

    ext_attrs_size = sizeof(bcmi_keygen_ext_attrs_t);
    sal_memset(attrs, 0, ext_attrs_size);

    _FP_XGS3_ALLOC(stage_fc->ext_cfg_db_arr,
                   (BCMI_KEYGEN_MODE_COUNT * sizeof(bcmi_keygen_ext_cfg_db_t *)),
                    "Field Extractors Data base Array");
    if (NULL == stage_fc->ext_cfg_db_arr) {
        return (BCM_E_MEMORY);
    }

    _FP_XGS3_ALLOC(ext_cfg_db,
                   sizeof(bcmi_keygen_ext_cfg_db_t), "Field Extractors DB");
    if (NULL == ext_cfg_db) {
        return (BCM_E_MEMORY);
    }
    stage_fc->ext_cfg_db_arr[BCMI_KEYGEN_MODE_DBLINTRA] = ext_cfg_db;
    /* Initialize Level_1 32-bit extractor section. */
    level = 1;
    gran = 32;
    ext_num = 0;
    part = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2S1, 0, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2S1, 32, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2S2, 0, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2S2, 32, attrs);

    /* Initialize Level_1 16-bit extractors section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2S1, 64, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2S1, 80, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2S1, 96, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2S1, 112, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2S1, 128, attrs);

    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2S2, 64, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2S2, 80, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2S2, 96, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2S2, 112, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2S2, 128, attrs);

    /* Initialize Level_1 8-bit extractors section. */
    level = 1;
    gran = 8;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2S2, 144, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2S2, 152, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2S2, 160, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2S2, 168, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2S2, 176, attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LTID);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2S2,
        184, &ext_attrs);

    /* Initialize Level_1 4-bit extractors section. */
    level = 1;
    gran = 4;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2S2, 192, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2S2, 196, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2S2, 200, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2S2, 204, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2S2, 208, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2S2, 212, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2S2, 216, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2S2, 220, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2S2, 224, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2S2, 228, attrs);

    /* Initialize Level_2 16-bit extractors section. */
    level = 2;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2S1, BCMI_KEYGEN_EXT_SECTION_L3S1, 0, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_MODE_SINGLE_ASET_WIDE);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2S1, BCMI_KEYGEN_EXT_SECTION_L3S1, 20, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_MODE_SINGLE_ASET_WIDE);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2S1, BCMI_KEYGEN_EXT_SECTION_L3S1, 36, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_C);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2S1, BCMI_KEYGEN_EXT_SECTION_L3S1, 52, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_D);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2S1, BCMI_KEYGEN_EXT_SECTION_L3S1, 68, &ext_attrs);

    /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 16, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_0_3);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 84, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_4_7);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 88, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_8_11);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 92, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 96, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 100, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 104, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 108, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IFP_DROP_VECTOR);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 112, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_PRI);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 116, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_MH_OPCODE);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 10),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 120, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LTID);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num +11),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 124, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 12),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 128, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 13),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 132, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 14),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 136, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 15),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 140, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 144, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 146, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 148, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 150, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 152, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 154, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 156, &ext_attrs);

    /* Initialize Level2 1-bit extractors section. */
    gran = 1;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
         BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_HIT);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 158, &ext_attrs);

    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DROP);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2S2, BCMI_KEYGEN_EXT_SECTION_L3S1, 159, &ext_attrs);

    level = 3;
    gran = 160;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, 0,
        BCMI_KEYGEN_EXT_SECTION_L3S1, BCMI_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);

    /* 320 bits mode - Initialize Level_1 32-bit extractor section. */
    ext_cfg_db = NULL;
    _FP_XGS3_ALLOC(ext_cfg_db,
                   sizeof(bcmi_keygen_ext_cfg_db_t), "Field Extractors DB");
    if (NULL == ext_cfg_db) {
        return (BCM_E_MEMORY);
    }
    stage_fc->ext_cfg_db_arr[BCMI_KEYGEN_MODE_DBLINTER] = ext_cfg_db;

    /* Initialize Level_1 32-bit extractor section. */
    level = 1;
    gran = 32;
    ext_num = 0;
    part = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2AS1, 0, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2AS1, 32, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2AS2, 0, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2AS2, 32, attrs);

    /* Initialize Level_1 16-bit extractors section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS1, 64, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS1, 80, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS1, 96, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS1, 112, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS1, 128, attrs);

    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS2, 64, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS2, 80, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS2, 96, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS2, 112, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS2, 128, attrs);

    /* Initialize Level_1 8-bit extractors section. */
    level = 1;
    gran = 8;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2AS2, 144, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2AS2, 152, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2AS2, 160, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2AS2, 168, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2AS2, 176, attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LTID);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2AS2,
        184, &ext_attrs);

    /* Initialize Level_1 4-bit extractors section. */
    level = 1;
    gran = 4;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 192, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 196, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 200, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 204, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 208, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 212, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 216, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 220, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 224, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 228, attrs);

    /* Initialize Level_2 16-bit extractors section. */
    level = 2;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2AS1, BCMI_KEYGEN_EXT_SECTION_L3AS1, 0, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2AS1, BCMI_KEYGEN_EXT_SECTION_L3AS1, 20, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2AS1, BCMI_KEYGEN_EXT_SECTION_L3AS1, 36, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_C);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2AS1, BCMI_KEYGEN_EXT_SECTION_L3AS1, 52, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_D);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2AS1, BCMI_KEYGEN_EXT_SECTION_L3AS1, 68, &ext_attrs);

    /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 16, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_0_3);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 84, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_4_7);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 88, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_8_11);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 92, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 96, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 100, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 104, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 108, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IFP_DROP_VECTOR);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 112, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_PRI);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 116, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_MH_OPCODE);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 10),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 120, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LTID);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 11) ,
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 124, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 12),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 128, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 13),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 132, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 14),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 136, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 15),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 140, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 144, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 146, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 148, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 150, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 152, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 154, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 156, &ext_attrs);

    /* Initialize Level2 1-bit extractors section. */
    gran = 1;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
         BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_HIT);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 158, &ext_attrs);

    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DROP);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 159, &ext_attrs);


    /* Initialize Level_1 32-bit extractor section. */
    level = 1;
    gran = 32;
    ext_num = 0;
    part = 1;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2BS1, 0, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2BS1, 32, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2BS2, 0, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2BS2, 32, attrs);

    /* Initialize Level_1 16-bit extractors section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS1, 64, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS1, 80, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS1, 96, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS1, 112, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS1, 128, attrs);

    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS2, 64, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS2, 80, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS2, 96, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS2, 112, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS2, 128, attrs);

    /* Initialize Level_1 8-bit extractors section. */
    level = 1;
    gran = 8;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2BS2, 144, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2BS2, 152, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2BS2, 160, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2BS2, 168, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2BS2, 176, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2BS2, 184, attrs);

    /* Initialize Level_1 4-bit extractors section. */
    level = 1;
    gran = 4;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 192, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 196, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 200, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 204, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 208, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 212, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 216, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 220, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 224, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 228, attrs);

    /* Initialize Level_2 16-bit extractors section. */
    level = 2;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2BS1, BCMI_KEYGEN_EXT_SECTION_L3BS1, 0, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2BS1, BCMI_KEYGEN_EXT_SECTION_L3BS1, 20, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2BS1, BCMI_KEYGEN_EXT_SECTION_L3BS1, 36, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_C);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2BS1, BCMI_KEYGEN_EXT_SECTION_L3BS1, 52, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_D);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2BS1, BCMI_KEYGEN_EXT_SECTION_L3BS1, 68, &ext_attrs);

    /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 16, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 84, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 88, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 92, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 96, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 100, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 104, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 108, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 112, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 116, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 10),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 120, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 11),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 124, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 12),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 128, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 13),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 132, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 14),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 136, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 15),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 140, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 144, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 146, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 148, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 150, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 152, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 154, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 156, &ext_attrs);

    /* Initialize Level2 1-bit extractors section. */
    gran = 1;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 158, &ext_attrs);

    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 159, &ext_attrs);


    level = 3;
    gran = 160;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, 0,
        BCMI_KEYGEN_EXT_SECTION_L3AS1, BCMI_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, 0,
        BCMI_KEYGEN_EXT_SECTION_L3BS1, BCMI_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);


    /* 480 bit mode - Initialize Level_1 32-bit extractor section. */
    ext_cfg_db = NULL;
    _FP_XGS3_ALLOC(ext_cfg_db,
                   sizeof(bcmi_keygen_ext_cfg_db_t), "Field Extractors DB");
    if (NULL == ext_cfg_db) {
        return (BCM_E_MEMORY);
    }
    stage_fc->ext_cfg_db_arr[BCMI_KEYGEN_MODE_TRIPLE] = ext_cfg_db;

    /* Initialize Level_1 32-bit extractor section. */
    level = 1;
    gran = 32;
    ext_num = 0;
    part = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2AS1, 0, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2AS1, 32, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2AS2, 0, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2AS2, 32, attrs);

    /* Initialize Level_1 16-bit extractors section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS1, 64, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS1, 80, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS1, 96, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS1, 112, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS1, 128, attrs);

    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS2, 64, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS2, 80, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS2, 96, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS2, 112, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2AS2, 128, attrs);

    /* Initialize Level_1 8-bit extractors section. */
    level = 1;
    gran = 8;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2AS2, 144, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2AS2, 152, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2AS2, 160, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2AS2, 168, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2AS2, 176, attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LTID);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2AS2,
        184, &ext_attrs);

    /* Initialize Level_1 4-bit extractors section. */
    level = 1;
    gran = 4;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 192, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 196, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 200, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 204, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 208, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 212, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 216, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 220, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 224, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2AS2, 228, attrs);

    /* Initialize Level_2 16-bit extractors section. */
    level = 2;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2AS1, BCMI_KEYGEN_EXT_SECTION_L3AS1, 0, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2AS1, BCMI_KEYGEN_EXT_SECTION_L3AS1, 20, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2AS1, BCMI_KEYGEN_EXT_SECTION_L3AS1, 36, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_C);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2AS1, BCMI_KEYGEN_EXT_SECTION_L3AS1, 52, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_D);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2AS1, BCMI_KEYGEN_EXT_SECTION_L3AS1, 68, &ext_attrs);

    /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 16, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_0_3);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 84, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_4_7);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 88, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_8_11);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 92, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 96, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 100, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 104, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 108, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IFP_DROP_VECTOR);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 112, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_PRI);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 116, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_MH_OPCODE);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 10),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 120, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LTID);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 11),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1,
        124, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 12),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 128, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 13),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 132, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 14),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 136, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 15),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 140, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 144, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 146, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 148, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 150, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 152, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 154, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 156, &ext_attrs);

    /* Initialize Level2 1-bit extractors section. */
    gran = 1;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
         BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_HIT);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 158, &ext_attrs);

    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DROP);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2AS2, BCMI_KEYGEN_EXT_SECTION_L3AS1, 159, &ext_attrs);

    /* Initialize Level_1 32-bit extractor section. */
    level = 1;
    gran = 32;
    ext_num = 0;
    part = 1;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2BS1, 0, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2BS1, 32, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2BS2, 0, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2BS2, 32, attrs);

    /* Initialize Level_1 16-bit extractors section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS1, 64, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS1, 80, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS1, 96, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS1, 112, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS1, 128, attrs);

    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS2, 64, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS2, 80, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS2, 96, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS2, 112, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2BS2, 128, attrs);

    /* Initialize Level_1 8-bit extractors section. */
    level = 1;
    gran = 8;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2BS2, 144, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2BS2, 152, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2BS2, 160, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2BS2, 168, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2BS2, 176, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2BS2, 184, attrs);

    /* Initialize Level_1 4-bit extractors section. */
    level = 1;
    gran = 4;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 192, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 196, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 200, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 204, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 208, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 212, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 216, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 220, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 224, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2BS2, 228, attrs);

    /* Initialize Level_2 16-bit extractors section. */
    level = 2;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2BS1, BCMI_KEYGEN_EXT_SECTION_L3BS1, 0, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2BS1, BCMI_KEYGEN_EXT_SECTION_L3BS1, 20, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2BS1, BCMI_KEYGEN_EXT_SECTION_L3BS1, 36, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_C);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2BS1, BCMI_KEYGEN_EXT_SECTION_L3BS1, 52, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_D);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2BS1, BCMI_KEYGEN_EXT_SECTION_L3BS1, 68, &ext_attrs);

    /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 16, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 84, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 88, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 92, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 96, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 100, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 104, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 108, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 112, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 116, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 10),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 120, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 11),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 124, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 12),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 128, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 13),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 132, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 14),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 136, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 15),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 140, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 144, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 146, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 148, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 150, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 152, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 154, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 156, &ext_attrs);

    /* Initialize Level2 1-bit extractors section. */
    gran = 1;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 158, &ext_attrs);

    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2BS2, BCMI_KEYGEN_EXT_SECTION_L3BS1, 159, &ext_attrs);


    /* Initialize Level_1 32-bit extractor section. */
    level = 1;
    gran = 32;
    ext_num = 0;
    part = 2;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2CS1, 0, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2CS1, 32, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2CS2, 0, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E32, BCMI_KEYGEN_EXT_SECTION_L2CS2, 32, attrs);

    /* Initialize Level_1 16-bit extractors section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2CS1, 64, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2CS1, 80, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2CS1, 96, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2CS1, 112, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2CS1, 128, attrs);

    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2CS2, 64, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2CS2, 80, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2CS2, 96, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2CS2, 112, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L1E16, BCMI_KEYGEN_EXT_SECTION_L2CS2, 128, attrs);

    /* Initialize Level_1 8-bit extractors section. */
    level = 1;
    gran = 8;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2CS2, 144, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2CS2, 152, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2CS2, 160, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2CS2, 168, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2CS2, 176, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E8, BCMI_KEYGEN_EXT_SECTION_L2CS2, 184, attrs);

    /* Initialize Level_1 4-bit extractors section. */
    level = 1;
    gran = 4;
    ext_num = 0;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2CS2, 192, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2CS2, 196, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2CS2, 200, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2CS2, 204, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2CS2, 208, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2CS2, 212, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2CS2, 216, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2CS2, 220, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2CS2, 224, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L1E4, BCMI_KEYGEN_EXT_SECTION_L2CS2, 228, attrs);

    /* Initialize Level_2 16-bit extractors section. */
    level = 2;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2CS1, BCMI_KEYGEN_EXT_SECTION_L3CS1, 0, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2CS1, BCMI_KEYGEN_EXT_SECTION_L3CS1, 20, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2CS1, BCMI_KEYGEN_EXT_SECTION_L3CS1, 36, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_C);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2CS1, BCMI_KEYGEN_EXT_SECTION_L3CS1, 52, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_D);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2CS1, BCMI_KEYGEN_EXT_SECTION_L3CS1, 68, &ext_attrs);

    /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 16, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 84, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 88, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 92, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 96, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 100, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 104, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A);
    SHR_BITSET(ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 108, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 112, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 116, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 10),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 120, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 11),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 124, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 12),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 128, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 13),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 132, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 14),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 136, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 15),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 140, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 144, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 146, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 148, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 150, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 152, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 154, &ext_attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 156, &ext_attrs);

    /* Initialize Level2 1-bit extractors section. */
    gran = 1;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 158, &ext_attrs);

    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMI_KEYGEN_EXT_SECTION_L2CS2, BCMI_KEYGEN_EXT_SECTION_L3CS1, 159, &ext_attrs);

    level = 3;
    gran = 160;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, 0,
        BCMI_KEYGEN_EXT_SECTION_L3AS1, BCMI_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, 0,
        BCMI_KEYGEN_EXT_SECTION_L3BS1, BCMI_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, 0,
        BCMI_KEYGEN_EXT_SECTION_L3CS1, BCMI_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);
exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

/*
 * Function:
 *  _bcm_field_th3_em_key_attributes_init
 * Purpose:
 *  Initialize Exact Match Key Attribute memory.
 * Parameters:
 *  unit     - (IN) BCM device number.
 *  stage_fc - (IN) Field Processor stage control structure.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 */
int
_bcm_field_th3_em_key_attributes_init(int unit, _field_stage_t *stage_fc,
                                      _field_control_t *fc)
{
    exact_match_key_attributes_entry_t em_entry;
                                      /* key attributes Entry buffer.  */
    uint32 *em_key_attr_entry = NULL; /* Hardware Buffer Ptr.      */
    int idx;
    soc_mem_t mem;                    /* Memory Identifier.        */
    int mem_inst = 0;
    static soc_mem_t em_key_attributes_mem;

    mem = EXACT_MATCH_KEY_ATTRIBUTESm;

    /* Assign exact match key attribute entry buffer. */
    em_key_attr_entry = (uint32 *)&em_entry;

    for (idx = 0; idx < stage_fc->num_instances; idx++) {
        if (!(fc->pipe_map & (1 << idx))) {
             continue;
        }
        if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
            mem_inst = _FP_GLOBAL_INST;
        } else if (stage_fc->oper_mode == bcmFieldGroupOperModePipeLocal) {
            mem_inst = idx;
        } else {
            return BCM_E_INTERNAL;
        }

        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                            EXACT_MATCH_KEY_ATTRIBUTESm,
                            mem_inst, &em_key_attributes_mem));

        /* Initialize buffer to 0 */
        sal_memcpy(em_key_attr_entry, soc_mem_entry_null(unit, mem),
                   soc_mem_entry_words(unit, mem) * sizeof(uint32));

        /* Mode 128: entry 0 */
        /* Read entry into SW buffer. */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, em_key_attributes_mem,
                            MEM_BLOCK_ANY, 0, em_key_attr_entry));

        /* Mode 128: Set BUCKET_MODE. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, BUCKET_MODEf, 0);
        /* Mode 128: Set KEY_BASE_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, KEY_BASE_WIDTHf, 1);
        /* Mode 128: Set KEY_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, KEY_WIDTHf, 3);
        /* Mode 128: Set DATA_BASE_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, DATA_BASE_WIDTHf, 0);

        /* Install entry in hardware. */
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, em_key_attributes_mem,
                    MEM_BLOCK_ALL, 0, em_key_attr_entry));

        /* Initialize buffer to 0 */
        sal_memcpy(em_key_attr_entry, soc_mem_entry_null(unit, mem),
                soc_mem_entry_words(unit, mem) * sizeof(uint32));
        /* Mode 160: entry 1 */
        /* Read entry into SW buffer. */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, em_key_attributes_mem,
                    MEM_BLOCK_ANY, 1, em_key_attr_entry));

        /* Mode 160: Set BUCKET_MODE. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, BUCKET_MODEf, 0);
        /* Mode 160: Set KEY_BASE_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, KEY_BASE_WIDTHf, 1);
        /* Mode 160: Set KEY_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, KEY_WIDTHf, 11);
        /* Mode 160: Set DATA_BASE_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, DATA_BASE_WIDTHf, 0);
        /* Install entry in hardware. */
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, em_key_attributes_mem,
                    MEM_BLOCK_ALL, 1, em_key_attr_entry));

        /* Initialize buffer to 0 */
        sal_memcpy(em_key_attr_entry, soc_mem_entry_null(unit, mem),
                soc_mem_entry_words(unit, mem) * sizeof(uint32));
        /* Mode 320: entry 2 */
        /* Read entry into SW buffer. */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, em_key_attributes_mem,
                    MEM_BLOCK_ANY, 2, em_key_attr_entry));

        /* Mode 320: Set BUCKET_MODE. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, BUCKET_MODEf, 0);
        /* Mode 320: Set KEY_BASE_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, KEY_BASE_WIDTHf, 2);
        /* Mode 320: Set KEY_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, KEY_WIDTHf, 21);
        /* Mode 320: Set DATA_BASE_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, DATA_BASE_WIDTHf, 1);

        /* Install entry in hardware. */
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, em_key_attributes_mem,
                    MEM_BLOCK_ALL, 2, em_key_attr_entry));
    }
    return BCM_E_NONE;
}

int
_bcm_field_th3_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    bcmi_keygen_qual_flags_bmp_t qual_flags;
    bcmi_keygen_qual_cfg_info_db_t *db = NULL;
    BCMI_KEYGEN_QUAL_CFG_DECL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    /* L0 BUS extractor sections offset:
     *    - 32bit extractor = 0
     *    - 16bit extractor = 608
     *    - 8bit extractor  = 1120
     *    - 4bit extractor  = 1280
     *    - 2bit extractor  = 1400
     */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    _FP_XGS3_ALLOC(stage_fc->qual_cfg_info_db,
         sizeof(bcmi_keygen_qual_cfg_info_db_t),
         "IFP qualifiers");
    if (NULL == stage_fc->qual_cfg_info_db) {
        return (BCM_E_MEMORY);
    }
    db = stage_fc->qual_cfg_info_db;
    sal_memset(&qual_flags, 0, sizeof(bcmi_keygen_qual_flags_bmp_t));

    /* Pseudo qualifiers */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyStage, qual_flags);

    if (soc_feature(unit, soc_feature_field_exact_match_support)) {
        SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_IFP);
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyStageIngressExactMatch, qual_flags);
        SHR_BITCLR(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_IFP);
    }

    SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyStageIngress, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyExactMatchHitStatus, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyExactMatchActionClassId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyExactMatchGroupClassId, qual_flags);
    SHR_BITCLR(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 2, 64, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 19, 304, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcMac, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 3, 96, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 20, 320, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstMac, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 4, 128, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIp, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 8, 256, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIp, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 4, 128, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32, 5, 160, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E32, 6, 192, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(3, BCMI_KEYGEN_EXT_SECTION_L1E32, 7, 224, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIp6, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 4, 128, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32, 5, 160, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIp6Low, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 6, 192, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32, 7, 224, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIp6High, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32,  8, 256, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32,  9, 288, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E32, 10, 320, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(3, BCMI_KEYGEN_EXT_SECTION_L1E32, 11, 352, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIp6, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 8, 256, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32, 9, 288, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIp6Low, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 10, 320, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32, 11, 352, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIp6High, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 11, 352, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32, 10, 320, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E32,  9, 288, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(3, BCMI_KEYGEN_EXT_SECTION_L1E32,  8, 256, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(4, BCMI_KEYGEN_EXT_SECTION_L1E32,  7, 224, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(5, BCMI_KEYGEN_EXT_SECTION_L1E32,  6, 192, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(6, BCMI_KEYGEN_EXT_SECTION_L1E32,  5, 160, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(7, BCMI_KEYGEN_EXT_SECTION_L1E32,  4, 128, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL2PayLoad, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 11, 352, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32, 10, 320, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db,
                        bcmFieldQualifyL2PayloadFirstEightBytes, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelTtl, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 40, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelBos, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 41, 3);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelExp, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 44, 20);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelTtl, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 8, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelBos, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 9, 3);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelExp, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 12, 20);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsControlWord, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsControlWord, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 16, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7AHashUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7AHashUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7AHashLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7AHashLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 16, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7BHashUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7BHashUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7BHashLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7BHashLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 20);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIp6FlowLabel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 20);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIp6FlowLabel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyBfdYourDiscriminator, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyBfdYourDiscriminator, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyPacketLength, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyPacketLength, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOpaqueTagLow, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOpaqueTagLow, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 13);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOpaqueTagHigh, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 13);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOpaqueTagHigh, qual_flags);

    /* UDF1 Qualifiers. */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 0, 0, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_UDF, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData7, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 1, 16, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_UDF, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData6, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 32, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_UDF, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData5, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_UDF, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData4, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_UDF, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_UDF, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 6, 96, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_UDF, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData1, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 7, 112, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_UDF, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData0, qual_flags);

    /* UDF2 Qualifiers. */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 0, 0, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_UDF, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData15, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 1, 16, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_UDF, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData14, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 32, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_UDF, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData13, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_UDF, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData12, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_UDF, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData11, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_UDF, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData10, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 6, 96, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_UDF, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData9, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 7, 112, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_UDF, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData8, qual_flags);

    
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 8, 128, 9);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 8, 128, 9);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcTrunkMemberGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 8, 128, 9);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcModPortGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 8, 128, 9);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcModPortGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 9, 144, 9);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 9, 144, 9);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcTrunkMemberGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 9, 144, 9);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcModPortGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 9, 144, 9);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcModPortGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 8, 128, 9);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcTrunk, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 9, 144, 9);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcTrunk, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 168, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 10);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 10);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceIngressKeySelectClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 168, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceLookupClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyUdfClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 184, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 10);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 10);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceIngressKeySelectClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 184, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceLookupClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyUdfClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyForwardingVlanId, qual_flags);

    
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVrf, qual_flags);

    
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 14);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVpn, qual_flags);

    
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 13);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL3Ingress, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterVlan, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterVlanId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 236, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterVlanCfi, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 237, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterVlanPri, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 16, 256, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL4DstPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 17, 272, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL4SrcPort, qual_flags);

    /* Overlay on same region as L4SrcPort */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 17, 272, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIcmpTypeCode, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 18, 288, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyEtherType, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 21, 336, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 22, 352, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E16, 23, 368, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(3, BCMI_KEYGEN_EXT_SECTION_L1E16, 24, 384, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRangeCheck, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 0, 0, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 1, 8, 8);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E4, 13, 52, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVrf, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 2, 16, 8);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E4, 3, 12, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterVlanId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 3, 24, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 3, 24, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 3, 24, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 3, 24, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 3, 24, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 3, 24, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 3, 24, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceIngressKeySelectClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 3, 24, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceLookupClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 3, 24, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyUdfClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 4, 32, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 4, 32, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 4, 32, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 4, 32, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 4, 32, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 4, 32, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 4, 32, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceIngressKeySelectClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 4, 32, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceLookupClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 4, 32, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyUdfClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyForwardingVlanId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TOS_FN, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTos, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TOS_FN, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTosClassZero, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TOS_FN, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTosClassOne, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TTL_FN, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTtl, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TTL_FN, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTtlClassZero, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TTL_FN, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTtlClassOne, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpProtocol, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyExtensionHeader2Type, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 10, 80, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyExtensionHeaderType, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 11, 88, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyExtensionHeaderSubCode, qual_flags);

    /* ltid is assigned fixed offset to avoid issues
     * with matching on ltid with value 0
     */

    if (soc_feature(unit, soc_feature_fp_ltid_match_all_parts) &&
        (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS)) {
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 12, 96, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_LTID, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyPreLogicalTableId1, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 12, 96, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_LTID, 2);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyPreLogicalTableId2, qual_flags);
    }

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 12, 101, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyGroupClass, qual_flags);

    /* EM_LOOKUP_CONTROL */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 13, 104, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyExactMatchGroupClassIdLookup0, qual_flags);

    

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 15, 120, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TCP_FN, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTcpControl, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 15, 120, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TCP_FN, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTcpClassZero, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 15, 120, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TCP_FN, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTcpClassOne, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 16, 128, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ALT_TTL_FN, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpTunnelTtl, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 16, 128, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ALT_TTL_FN, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpTunnelTtlClassZero, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 17, 136, 8);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E8, 18, 144, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRangeCheckGroup, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 0, 0, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpType, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 4, 16, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIngressStpState, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 4, 16, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyStpState, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 4, 18, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDosAttack, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 4, 19, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyForwardingVlanValid, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 5, 23, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpTunnelHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 5, 22, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsTerminated, qual_flags);

    
    SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 5, 20, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsLabel1Hit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 5, 21, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsLabel2Hit, qual_flags);
    SHR_BITCLR(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyForwardingType, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyEtherTypeClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpProtocolClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 11, 44, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelAction, qual_flags);

    

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 16, 64, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpFlags, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 16, 66, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpFrag, qual_flags);

    SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 17, 68, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyColor, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 17, 70, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIntCongestionNotification, qual_flags);
    SHR_BITCLR(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 18, 72, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpFragNonOrFirst, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 18, 73, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL4Ports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 18, 74, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpInfo, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 18, 75, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIcmpError, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 19, 76, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL2Format, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 20, 81, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL3Routable, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 20, 80, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMyStationHit, qual_flags);

    
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 21, 84, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerTpid, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 21, 86, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterTpid, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 22, 88, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTunnelType, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 22, 88, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyLoopbackType, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 22, 91, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyLoopback, qual_flags);

    

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 24, 96, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVlanFormat, qual_flags);

    /********************** Post Mux Qualifiers. **********************/
    /******************************************************************/
    SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_PMUX);

    /* ltid is assigned fixed offset to avoid
     * issues with matching on ltid with value 0
     */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_LTID,
        0, 124, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db,
        _bcmFieldQualifyPreLogicalTableId, qual_flags);

    /*
     * Exact-Match doesn't support Post-Mux qualifiers.
     * Set the flag NOT_IN_EM to avoid initilization.
     */
    SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_DROP, 0, 159, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDrop, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_HIT, 1, 158, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, _bcmFieldQualifyExactMatchHitStatusLookup0, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_0_3, 2, 84, 4);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_4_7, 3, 88, 4);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2,
        BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_8_11, 4, 91, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, _bcmFieldQualifyExactMatchActionClassIdLookup0, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_INT_PRI, 13, 116, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyIntPriority, qual_flags);

    /* Class Id C */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_CLASS_ID_C, 10, 52, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifySrcClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_CLASS_ID_C, 10, 52, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_CLASS_ID_C, 10, 52, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifySrcClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_CLASS_ID_C, 10, 52, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_CLASS_ID_C, 10, 60, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifySrcClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_CLASS_ID_C, 10, 52, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_CLASS_ID_C, 10, 60, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifySrcClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_CLASS_ID_C, 10, 52, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstClassL3, qual_flags);

    /* Class Id D */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_CLASS_ID_D, 11, 68, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifySrcClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_CLASS_ID_D, 11, 68, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_CLASS_ID_D, 11, 68, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifySrcClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_CLASS_ID_D, 11, 68, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_CLASS_ID_D, 11, 76, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifySrcClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_CLASS_ID_D, 11, 68, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_CLASS_ID_D, 11, 76, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifySrcClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_CLASS_ID_D, 11, 68, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstClassL3, qual_flags);

    /* Destination Container 0 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_A, 7, 20, 9);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_DST_A, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_A, 7, 20, 9);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_DST_A, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstTrunk, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_A, 7, 20, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_A, 7, 110, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_DST_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMultipath, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_A, 7, 20, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_A, 7, 110, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_DST_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3EgressNextHops, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_A, 7, 20, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_A, 7, 110, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_DST_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3Egress, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_A, 7, 20, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_A, 7, 110, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_DST_A, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3MulticastGroup, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_A, 7, 20, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_A, 7, 110, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_DST_A, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL2MulticastGroup, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_B, 8, 36, 9);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_DST_B, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_B, 8, 36, 9);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_DST_B, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstTrunk, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_B, 8, 36, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_B, 8, 111, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_DST_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMultipath, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_B, 8, 36, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_B, 8, 111, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_DST_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3EgressNextHops, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_B, 8, 36, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_B, 8, 111, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_DST_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3Egress, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_B, 8, 36, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_B, 8, 111, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_DST_B, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3MulticastGroup, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_B, 8, 36, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_B, 8, 111, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_DST_B, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL2MulticastGroup, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_MH_OPCODE, 14, 120, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMHOpcode, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_PKT_RESOLUTION, 9, 104, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyPacketRes, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_0_3, 5, 98, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL2SrcHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_0_3, 5, 99, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL2DestHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_4_7, 6, 102, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyL3SrcHostHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_4_7, 6, 102, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyIpmcStarGroupHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_4_7, 6, 101, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyL3DestHostHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_4_7, 6, 103, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyL3DestRouteHit, qual_flags);

#if 0
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_4_7, 6, 100, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyL2StationMove, qual_flags);
#endif

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_0_3, 5, 97, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyL2CacheHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_0_3, 5, 96, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyL2CacheHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_IPBM, 0, 0, 20);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInPorts, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_IPBM, 0, 0, 20);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifySystemPortBitmap, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_IPBM, 0, 0, 20);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDevicePortBitmap, qual_flags);

    SHR_BITCLR(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);
    /********************** END Of Post Mux Qualifiers. **********************/

    

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
_bcm_field_th3_keygen_profile1_entry_pack(int unit, _field_stage_t *stage_fc,
                               _field_group_t *fg, int part, soc_mem_t mem,
                               ifp_key_gen_program_profile_entry_t *prof1_entry)
{
    int idx; /* Iterator index. */
    static const soc_field_t l1_32_sel[] = /* 32 bit extractors. */
        {
            L1_C_E32_SEL_0f,
            L1_C_E32_SEL_1f,
            L1_B_E32_SEL_0f,
            L1_B_E32_SEL_1f
        };
    static const soc_field_t l1_16_sel[] = /* 16 bit extractors. */
        {
            L1_C_E16_SEL_0f,
            L1_C_E16_SEL_1f,
            L1_C_E16_SEL_2f,
            L1_C_E16_SEL_3f,
            L1_C_E16_SEL_4f,
            L1_B_E16_SEL_0f,
            L1_B_E16_SEL_1f,
            L1_B_E16_SEL_2f,
            L1_B_E16_SEL_3f,
            L1_B_E16_SEL_4f,
        };
    static const soc_field_t l1_8_sel[] = /* 8 bit extractors. */
        {
            L1_A_E8_SEL_0f,
            L1_A_E8_SEL_1f,
            L1_A_E8_SEL_2f,
            L1_A_E8_SEL_3f,
            L1_A_E8_SEL_4f,
            L1_A_E8_SEL_5f,
        };
    static const soc_field_t l1_4_sel[] = /* 4 bit extractors. */
        {
            L1_A_E4_SEL_0f,
            L1_A_E4_SEL_1f,
            L1_A_E4_SEL_2f,
            L1_A_E4_SEL_3f,
            L1_A_E4_SEL_4f,
            L1_A_E4_SEL_5f,
            L1_A_E4_SEL_6f,
            L1_A_E4_SEL_7f,
            L1_A_E4_SEL_8f,
            L1_A_E4_SEL_9f
        };

    static const soc_field_t l2_16_sel[] = /* 16 bit extractors. */
        {
            L2_E16_SEL_0f,
            L2_E16_SEL_1f,
            L2_E16_SEL_2f,
            L2_E16_SEL_3f,
            L2_E16_SEL_4f,
        };
    static const soc_field_t l2_4_sel[] = /* 4 bit extractors. */
        {
            L2_E4_SEL_0f,
            L2_E4_SEL_1f,
            L2_E4_SEL_2f,
            L2_E4_SEL_3f,
            L2_E4_SEL_4f,
            L2_E4_SEL_5f,
            L2_E4_SEL_6f,
            L2_E4_SEL_7f,
            L2_E4_SEL_8f,
            L2_E4_SEL_9f,
            L2_E4_SEL_10f,
            L2_E4_SEL_11f,
            L2_E4_SEL_12f,
            L2_E4_SEL_13f,
            L2_E4_SEL_14f,
            L2_E4_SEL_15f,
        };
    static const soc_field_t l2_2_sel[] = /* 2 bit extractors. */
        {
            L2_E2_SEL_0f,
            L2_E2_SEL_1f,
            L2_E2_SEL_2f,
            L2_E2_SEL_3f,
            L2_E2_SEL_4f,
            L2_E2_SEL_5f,
            L2_E2_SEL_6f
        };
    static const soc_field_t l2_1_sel[] = /* 1 bit extractors. */
        {
            L2_E1_SEL_0f,
            L2_E1_SEL_1f,
        };

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == prof1_entry)) {
        return (BCM_E_PARAM);
    }

    /* Set 32bit extractor selcode values. */
    for (idx = 0; idx < 4; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e32_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l1_32_sel[idx],
                fg->ext_codes[part].l1_e32_sel[idx]);
        }
    }

    /* Set 16bit extractor selcode values. */
    for (idx = 0; idx < 10; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e16_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l1_16_sel[idx],
                fg->ext_codes[part].l1_e16_sel[idx]);
        }
    }

    /* Set 8bit extractor selcode values. */
    for (idx = 0; idx < 6; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e8_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l1_8_sel[idx],
                fg->ext_codes[part].l1_e8_sel[idx]);
        }
    }

    /* Set 4bit extractor selcode values. */
    for (idx = 0; idx < 10; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e4_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l1_4_sel[idx],
                fg->ext_codes[part].l1_e4_sel[idx]);
        }
    }

    /* Set 16bit extractor selcode values. */
    for (idx = 0; idx < 5; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l2_e16_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l2_16_sel[idx],
                fg->ext_codes[part].l2_e16_sel[idx]);
        }
    }

    /* Set 4bit extractor selcode values. */
    for (idx = 0; idx < 16; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l2_e4_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l2_4_sel[idx],
                fg->ext_codes[part].l2_e4_sel[idx]);
        }
    }

    /* Set 2bit extractor selcode values. */
    for (idx = 0; idx < 7; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l2_e2_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l2_2_sel[idx],
                fg->ext_codes[part].l2_e2_sel[idx]);
        }
    }

    /* Set 1bit extractor selcode values. */
    for (idx = 0; idx < 2; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l2_e1_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l2_1_sel[idx],
                fg->ext_codes[part].l2_e1_sel[idx]);
        }
    }

    return (BCM_E_NONE);
}

int
_bcm_field_th3_keygen_em_profile_entry_pack(int unit, _field_stage_t *stage_fc,
                                            _field_group_t *fg, int part, soc_mem_t mem,
                                            exact_match_key_gen_program_profile_entry_t *prof_entry)
{
    int idx; /* Iterator index. */
    static const soc_field_t l1_32_sel[] = /* 32 bit extractors. */
    {
        L1_C_E32_SEL_0f,
        L1_C_E32_SEL_1f,
        L1_B_E32_SEL_0f,
        L1_B_E32_SEL_1f
    };
    static const soc_field_t l1_16_sel[] = /* 16 bit extractors. */
    {
        L1_C_E16_SEL_0f,
        L1_C_E16_SEL_1f,
        L1_C_E16_SEL_2f,
        L1_C_E16_SEL_3f,
        L1_C_E16_SEL_4f,
        L1_B_E16_SEL_0f,
        L1_B_E16_SEL_1f,
        L1_B_E16_SEL_2f,
        L1_B_E16_SEL_3f,
        L1_B_E16_SEL_4f,
    };
    static const soc_field_t l1_8_sel[] = /* 8 bit extractors. */
    {
        L1_A_E8_SEL_0f,
        L1_A_E8_SEL_1f,
        L1_A_E8_SEL_2f,
        L1_A_E8_SEL_3f,
        L1_A_E8_SEL_4f,
        L1_A_E8_SEL_5f,
    };
    static const soc_field_t l1_4_sel[] = /* 4 bit extractors. */
    {
        L1_A_E4_SEL_0f,
        L1_A_E4_SEL_1f,
        L1_A_E4_SEL_2f,
        L1_A_E4_SEL_3f,
        L1_A_E4_SEL_4f,
        L1_A_E4_SEL_5f,
        L1_A_E4_SEL_6f,
        L1_A_E4_SEL_7f,
        L1_A_E4_SEL_8f,
        L1_A_E4_SEL_9f
    };

    static const soc_field_t l2_16_sel[] = /* 16 bit extractors. */
    {
        L2_E16_SEL_0f,
        L2_E16_SEL_1f,
        L2_E16_SEL_2f,
        L2_E16_SEL_3f,
        L2_E16_SEL_4f,
    };
    static const soc_field_t l2_4_sel[] = /* 4 bit extractors. */
    {
        L2_E4_SEL_0f,
        L2_E4_SEL_1f,
        L2_E4_SEL_2f,
        L2_E4_SEL_3f,
        L2_E4_SEL_4f,
        L2_E4_SEL_5f,
        L2_E4_SEL_6f,
        L2_E4_SEL_7f,
        L2_E4_SEL_8f,
        L2_E4_SEL_9f,
        L2_E4_SEL_10f,
        L2_E4_SEL_11f,
        L2_E4_SEL_12f,
        L2_E4_SEL_13f,
        L2_E4_SEL_14f,
        L2_E4_SEL_15f,
    };
    static const soc_field_t l2_2_sel[] = /* 2 bit extractors. */
    {
        L2_E2_SEL_0f,
        L2_E2_SEL_1f,
        L2_E2_SEL_2f,
        L2_E2_SEL_3f,
        L2_E2_SEL_4f,
        L2_E2_SEL_5f,
        L2_E2_SEL_6f
    };

    static const soc_field_t l2_1_sel[] = /* 1 bit extractors. */
    {
        L2_E1_SEL_0f,
        L2_E1_SEL_1f,
    };

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == prof_entry)) {
        return (BCM_E_PARAM);
    }

    /* Set 32bit extractor selcode values. */
    for (idx = 0; idx < 4; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e32_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof_entry, l1_32_sel[idx],
                    fg->ext_codes[part].l1_e32_sel[idx]);
        }
    }

    /* Set 16bit extractor selcode values. */
    for (idx = 0; idx < 10; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e16_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof_entry, l1_16_sel[idx],
                    fg->ext_codes[part].l1_e16_sel[idx]);
        }
    }

    /* Set 8bit extractor selcode values. */
    for (idx = 0; idx < 6; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e8_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof_entry, l1_8_sel[idx],
                    fg->ext_codes[part].l1_e8_sel[idx]);
        }
    }

    /* Set 4bit extractor selcode values. */
    for (idx = 0; idx < 10; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e4_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof_entry, l1_4_sel[idx],
                    fg->ext_codes[part].l1_e4_sel[idx]);
        }
    }

    /* Set 16bit extractor selcode values. */
    for (idx = 0; idx < 5; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l2_e16_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof_entry, l2_16_sel[idx],
                    fg->ext_codes[part].l2_e16_sel[idx]);
        }
    }

    /* Set 4bit extractor selcode values. */
    for (idx = 0; idx < 16; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l2_e4_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof_entry, l2_4_sel[idx],
                    fg->ext_codes[part].l2_e4_sel[idx]);
        }
    }

    /* Set 2bit extractor selcode values. */
    for (idx = 0; idx < 5; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l2_e2_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof_entry, l2_2_sel[idx],
                    fg->ext_codes[part].l2_e2_sel[idx]);
        }
    }

    /* Set 1bit extractor selcode values. */
    for (idx = 0; idx < 2; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l2_e1_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof_entry, l2_1_sel[idx],
                fg->ext_codes[part].l2_e1_sel[idx]);
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_field_th_udf_chunks_to_int_qset
 * Purpose:
 *      Sets internal data qualifiers corrspeonding to the
 *      offset chunks used by the UDF.
 * Parameters:
 *      unit                - (IN)  Unit number.
 *      udf_chunks_bmap     - (IN)  UDF offsets chunks map
 *      qset                - (OUT) Qualifier set
 * Returns:
 *      BCM_E_NONE
 * Notes:
 */
int
_bcm_field_th3_udf_chunks_to_int_qset(int unit, uint32 udf_chunks_bmap,
                                      bcm_field_qset_t *qset)
{
    int idx, max_chunks = 0;

    max_chunks = BCMI_XGS4_UDF_CTRL_MAX_UDF_CHUNKS(unit);


    for (idx = 0; idx < max_chunks; idx++) {
        if (udf_chunks_bmap & (1 << idx)) {
            BCM_FIELD_QSET_ADD_INTERNAL(*qset, _bcm_th3_udf_chunk_to_qual_mapping[idx]);
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_th3_qualify_udf
 * Purpose:
 *      Add UDF data that the packet must match to trigger qualifier.
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      eid             - (IN)  Field Entry.
 *      udf_id          - (IN)  UDF ID.
 *      length          - (IN)  Number of bytes to match in field entry.
 *      data            - (IN)  Input data to qualify the entry.
 *      mask            - (IN)  Input mask to qualify the entry.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
_bcm_field_th3_qualify_udf(
    int unit,
    bcm_field_entry_t eid,
    bcm_udf_id_t udf_id,
    int length,
    uint8 *data,
    uint8 *mask)
{
    int rv;
    int gran;
    int chunk;
    int bytes;
    int len = 0;
    int bits = 8;
    uint32 q_data, q_mask;
    uint32 chunk_bmap = 0;
    _bcm_field_internal_qualify_t qual;
    bcmi_xgs4_udf_offset_info_t *offset_info = NULL;
    int byte_offset;        /* Byte offset in the word.   */

    /* Get UDF info. */
    rv = bcmi_xgs4_udf_offset_node_get(unit, udf_id, &offset_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    /* If provided entry data length is greater than UDF width */
    if ((length > offset_info->width) || (length <= 0)) {
        return (BCM_E_PARAM);
    }

    /* Chunk Granularity */
    gran = BCMI_XGS4_UDF_CTRL_OFFSET_GRAN(unit);

    chunk_bmap = offset_info->hw_bmap;
    byte_offset = offset_info->byte_offset;

    /* Update udf_chuks bmap with qset */
    for (chunk = 0; chunk < BCMI_XGS4_UDF_CTRL_MAX_UDF_CHUNKS(unit); chunk++) {
        if(chunk_bmap & (1 << chunk)) {
            qual = _bcm_th3_udf_chunk_to_qual_mapping[chunk];
            q_data = q_mask = 0;
            for (bytes = byte_offset; bytes < gran; bytes++) {
                if (len >= length) {
                   break;
                }
                q_data |= data[len] << ((gran - 1 - bytes) * bits);
                q_mask |= mask[len] << ((gran - 1 - bytes) * bits);
                len++;
            }
            byte_offset = 0;
            rv = _field_qualify32(unit, eid, qual, q_data, q_mask);
            BCM_IF_ERROR_RETURN(rv);
        }
    }
    return BCM_E_NONE;
}

int
_bcm_field_th3_qualify_udf_get(
    int unit,
    bcm_field_entry_t eid,
    bcm_udf_id_t udf_id,
    int max_length,
    uint8 *data,
    uint8 *mask,
    int *actual_length)
{
    int rv;
    int gran;
    int chunk;
    int bytes;
    int len = 0;
    int bits = 8;
    uint32 q_data, q_mask;
    uint32 chunk_bmap = 0;
    _bcm_field_internal_qualify_t qual;
    bcmi_xgs4_udf_offset_info_t *offset_info = NULL;
    int byte_offset;        /* Byte offset in the word.   */

    /* Input parameters check. */
    if ((NULL == data) || (NULL == mask) ||
        (NULL == actual_length) || (max_length == 0)) {
        return (BCM_E_PARAM);
    }

    /* Get UDF info. */
    rv = bcmi_xgs4_udf_offset_node_get(unit, udf_id, &offset_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /*
     * If provided entry data length is less than data length used
     * to create qualifier then return error
     */
    if (max_length < offset_info->width) {
        return (BCM_E_PARAM);
    }


    /* Chunk Granularity */
    gran = BCMI_XGS4_UDF_CTRL_OFFSET_GRAN(unit);

    chunk_bmap = offset_info->hw_bmap;
    byte_offset = offset_info->byte_offset;

    /* Update udf_chuks bmap with qset */
    for (chunk = 0; chunk < BCMI_XGS4_UDF_CTRL_MAX_UDF_CHUNKS(unit); chunk++) {
        if(chunk_bmap & (1 << chunk)) {
            qual = _bcm_th3_udf_chunk_to_qual_mapping[chunk];
            q_data = q_mask = 0;
            rv = _bcm_field_entry_qualifier_uint32_get(unit, eid, qual,
                                                       &q_data, &q_mask);
            BCM_IF_ERROR_RETURN(rv);

            for (bytes = byte_offset; bytes < gran; bytes++) {
                if (len >= max_length) {
                   break;
                }

                data[len] = q_data >> ((gran - 1 - bytes) * bits);
                mask[len] = q_mask >> ((gran - 1 - bytes) * bits);
                len++;
            }
            byte_offset = 0;
        }
    }
    *actual_length = len;
    return BCM_E_NONE;
}

/*
 * Function: _bcm_field_th3_ForwardingType_set
 *
 * Purpose:
 *     Add qualification data to an entry for IP Type.
 *
 * Parameters:
 *     unit     - (IN) BCM unit number.
 *     stage_id - (IN) Stage ID.
 *     entry    - (IN) Entry ID.
 *     type     - (IN) Forwarding type.
 *     data     - (OUT) Data.
 *     mask     - (OUT) Mask.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th3_forwardingType_set(int unit, _field_stage_id_t stage_id,
                                 bcm_field_entry_t entry,
                                 bcm_field_ForwardingType_t type,
                                 uint32 *data, uint32 *mask)
{

    if (NULL == data || NULL == mask) {
       return BCM_E_PARAM;
    }

    switch (stage_id) {
       case _BCM_FIELD_STAGE_INGRESS:
       case _BCM_FIELD_STAGE_EXACTMATCH:
           switch (type) {
            case bcmFieldForwardingTypeAny:
                *data = 0;
                *mask = 0;
                break;
            case bcmFieldForwardingTypeL2:
                *data = 0;
                *mask = 0xE;
                break;
            case bcmFieldForwardingTypeL2Independent:
                *data = 0;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL2Shared:
                *data = 1;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL3Direct:
                *data = 4;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL3:
                *data = 5;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeMpls:
                *data = 7;
                *mask = 0xF;
                break;
            default:
               return (BCM_E_PARAM);
          }
          break;
      default:
          return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_th3_forwardingType_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyForwardingType
 *                     qualifier from the field entry.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stage_id - (IN) Stage ID.
 *      entry    - (IN) BCM field entry id.
 *      type     - (OUT) Qualifier match forwarding type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_field_th3_forwardingType_get(int unit, _field_stage_id_t stage_id,
                                 bcm_field_entry_t entry,
                                 uint32 data,  uint32 mask,
                                 bcm_field_ForwardingType_t *type)
{
    if (NULL == type) {
       return BCM_E_PARAM;
    }

    switch (stage_id) {
       case _BCM_FIELD_STAGE_INGRESS:
       case _BCM_FIELD_STAGE_EXACTMATCH:
          if ((0 == data) && (mask == 0)) {
              *type = bcmFieldForwardingTypeAny;
          } else if ((0 == data) && (mask == 0xE)) {
              *type = bcmFieldForwardingTypeL2;
          } else if ((0 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL2Independent;
          } else if ((1 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL2Shared;
          } else if ((4 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL3Direct;
          } else if ((5 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL3;
          } else if ((7 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeMpls;
          } else {
              return (BCM_E_INTERNAL);
          }
           break;
       default:
           return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_field_th3_qualify_ip_type
 * Purpose:
 *     Install ip type qualifier into TCAM
 * Parameters:
 *     unit  - (IN) BCM device number
 *     entry - (IN) Field entry index
 *     type  - (IN) Ip Type.
 *     qual  - (IN) Qualifier(IpType/InnerIpType)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */


int
_bcm_field_th3_qualify_ip_type(int unit, bcm_field_entry_t entry,
                               bcm_field_IpType_t type,
                               bcm_field_qualify_t qual)
{

    uint32              data = 0,
                        mask = 0;

    switch (type) {
      case bcmFieldIpTypeAny:
          data = 0x0;
          mask = 0x0;
          break;
      case bcmFieldIpTypeNonIp:
          data = 0xf;
          mask = 0xf;
          break;
      case bcmFieldIpTypeIp:
          data = 0x0;
          mask = 0x8;
          break;
          break;
      case bcmFieldIpTypeIpv4NoOpts:
          data = 0x0;
          mask = 0xf;
          break;
      case bcmFieldIpTypeIpv4WithOpts:
          data = 0x1;
          mask = 0xf;
          break;
      case bcmFieldIpTypeIpv4Any:
          data = 0x0;
          mask = 0xe;
          break;
      case bcmFieldIpTypeIpv6NoExtHdr:
          data = 0x4;
          mask = 0xf;
          break;
      case bcmFieldIpTypeIpv6OneExtHdr:
          data = 0x5;
          mask = 0xf;
          break;
      case bcmFieldIpTypeIpv6TwoExtHdr:
          data = 0x6;
          mask = 0xf;
          break;
      case bcmFieldIpTypeIpv6:
          data = 0x4;
          mask = 0xc;
          break;
      case bcmFieldIpTypeArp:
          data = 0x8;
          mask = 0xe;
          break;
      case bcmFieldIpTypeArpRequest:
          data = 0x8;
          mask = 0xf;
          break;
      case bcmFieldIpTypeArpReply:
          data = 0x9;
          mask = 0xf;
          break;
      case bcmFieldIpTypeTrill:
          data = 0xa;
          mask = 0xf;
          break;
      case bcmFieldIpTypeMplsUnicast:
          data = 0xc;
          mask = 0xf;
          break;
      case bcmFieldIpTypeMplsMulticast:
          data = 0xd;
          mask = 0xf;
          break;
      case bcmFieldIpTypeMim:
          data = 0xe;
          mask = 0xf;
          break;
      default:
          LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,
                     "FP(unit %d) vverb: entry=%d qualifying"
                     " on Invalid Iptype=%#x\n))"),
                     unit, entry, type));
          return (BCM_E_UNAVAIL);
    }
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "FP(unit %d) vverb: entry=%d qualifying on Iptype, "
                          "data=%#x, mask=%#x\n))"),
               unit, entry, data, mask));

    return _field_qualify32(unit, entry, qual,
                            data, mask);
}


/*
 * Function:
 *     _bcm_field_th3_qualify_ip_type_encode_get
 * Purpose:
 *     Get ip type qualifier encode value for given data and mask.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     data     - (IN) qualifier data.
 *     mask     - (IN) qualifier mask.
 *     type     - (OUT) Ip Type.
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
_bcm_field_th3_qualify_ip_type_encode_get(int unit,
                                          uint32 hw_data,
                                          uint32 hw_mask,
                                          bcm_field_IpType_t *type)
{

    if ((0 == hw_data) && (0 == hw_mask)) {
        *type = bcmFieldIpTypeAny;
    } else if ((0x0 == hw_data) && (0x8 == hw_mask)) {
        *type = bcmFieldIpTypeIp;
    } else if ((0xf == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeNonIp;
    } else if ((0x0 == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeIpv4NoOpts;
    } else if ((0x1 == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeIpv4WithOpts;
    } else if ((0x0 == hw_data) && (0xe == hw_mask)) {
        *type = bcmFieldIpTypeIpv4Any;
    } else if ((0x4 == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeIpv6NoExtHdr;
    } else if ((0x5 == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeIpv6OneExtHdr;
    } else if ((0x6 == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeIpv6TwoExtHdr;
    } else if ((0x4 == hw_data) && (0xc == hw_mask)) {
        *type = bcmFieldIpTypeIpv6;
    } else if ((0x8 == hw_data) && (0xe == hw_mask)) {
        *type = bcmFieldIpTypeArp;
    } else if ((0x8 == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeArpRequest;
    } else if ((0x9 == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeArpReply;
    } else if ((0xa == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeTrill;
    } else if ((0xc == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeMplsUnicast;
    } else if ((0xd == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeMplsMulticast;
    } else if ((0xe == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeMim;
    } else
    {
        return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th3_stage_action_support_check
 * Purpose:
 *     Check if action is supported by tomahawk for the given stage.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage    - (IN) Field processor stage.
 *     action   - (IN) Action to check(bcmFieldActionXXX)
 *     result   - (OUT)
 *               TRUE   - action is supported by device
 *               FALSE  - action is NOT supported by device
 * Returns:
 *     BCM_E_XXX
 */

int
_field_th3_stage_action_support_check(int unit,
                                     unsigned stage,
                                     bcm_field_action_t action,
                                     int *result)
{
    if (NULL == result) {
        return BCM_E_PARAM;
    }

    switch (stage) {
        case _BCM_FIELD_STAGE_INGRESS:
             switch (action) {
                  case bcmFieldActionIntEncapEnable:
                  case bcmFieldActionIntEncapDisable:
                  case bcmFieldActionElephantColorEnable:
                  case bcmFieldActionElephantColorDisable:
                  case bcmFieldActionElephantQueueEnable:
                  case bcmFieldActionElephantQueueDisable:
                  case bcmFieldActionElephantLookupEnable:
                  case bcmFieldActionElephantLookupDisable:
                  case bcmFieldActionMatchPbmpDrop:
                  case bcmFieldActionMatchPbmpRedirect:
                  case bcmFieldActionDlbEcmpMonitorEnable:
                  case bcmFieldActionMirrorZeroingEnable:
                       *result = TRUE;
                       return BCM_E_NONE;
                  default:
                       break;
             }
             break;
        case _BCM_FIELD_STAGE_LOOKUP:
             switch (action) {
                  case bcmFieldActionOuterVlanCfiCopyInner:
                  case bcmFieldActionOuterVlanPrioCopyInner:
                  case bcmFieldActionOuterVlanCopyInner:
                  case bcmFieldActionInnerVlanAdd:
                  case bcmFieldActionInnerVlanNew:
                  case bcmFieldActionInnerVlanDelete:
                  case bcmFieldActionInnerVlanCopyOuter:
                  case bcmFieldActionInnerVlanPrioNew:
                  case bcmFieldActionInnerVlanPrioCopyOuter:
                  case bcmFieldActionInnerVlanCfiNew:
                  case bcmFieldActionInnerVlanCfiCopyOuter:
                  case bcmFieldActionIngressGportSet:
                  case bcmFieldActionIncomingMplsPortSet:
                       *result = FALSE;
                       return BCM_E_NONE;
                  case bcmFieldActionMplsLookupEnable:
                  case bcmFieldActionTerminationAllowed:
                  case bcmFieldActionVisibilityEnable:
                  case bcmFieldActionL3IngressStrengthSet:
                       *result = TRUE;
                       return BCM_E_NONE;
                  default:
                       break;
             }
             break;
       case _BCM_FIELD_STAGE_EGRESS:
             switch (action) {
                  case bcmFieldActionLoopbackSrcModuleGportNew:
                  case bcmFieldActionLoopbackSrcGportNew:
                  case bcmFieldActionHiGigDstModuleGportNew:
                  case bcmFieldActionHiGigDstPortGportNew:
                  case bcmFieldActionHiGigDstGportNew:
                  case bcmFieldActionGpHiGigDropPrecedenceNew:
                  case bcmFieldActionYpHiGigDropPrecedenceNew:
                  case bcmFieldActionRpHiGigDropPrecedenceNew:
                  case bcmFieldActionHiGigDropPrecedenceNew:
                  case bcmFieldActionGpHiGigIntPriNew:
                  case bcmFieldActionYpHiGigIntPriNew:
                  case bcmFieldActionRpHiGigIntPriNew:
                  case bcmFieldActionHiGigIntPriNew:
                  case bcmFieldActionGpHGCongestionClassNew:
                  case bcmFieldActionYpHGCongestionClassNew:
                  case bcmFieldActionRpHGCongestionClassNew:
                  case bcmFieldActionVxlanHeaderFlags:
                  case bcmFieldActionVxlanHeaderBits8_31_Set:
                  case bcmFieldActionVxlanHeaderBits56_63_Set:
                  case bcmFieldActionIpFix:
                  case bcmFieldActionIpFixCancel:
                  case bcmFieldActionInnerVlanNew:
                  case bcmFieldActionYpEcnNew:
                  case bcmFieldActionGpEcnNew:
                  case bcmFieldActionRpEcnNew:
                  case bcmFieldActionRpDscpNew:
                  case bcmFieldActionYpDscpNew:
                  case bcmFieldActionGpDscpNew:
                  case bcmFieldActionInnerVlanPrioNew:
                  case bcmFieldActionRpInnerVlanPrioNew:
                  case bcmFieldActionGpInnerVlanPrioNew:
                  case bcmFieldActionYpInnerVlanPrioNew:
                  case bcmFieldActionYpDrop:
                  case bcmFieldActionRpDrop:
                  case bcmFieldActionGpDrop:
                  case bcmFieldActionYpOuterVlanPrioNew:
                  case bcmFieldActionRpOuterVlanPrioNew:
                  case bcmFieldActionGpOuterVlanPrioNew:
                  case bcmFieldActionInnerVlanCfiNew:
                  case bcmFieldActionGpInnerVlanCfiNew:
                  case bcmFieldActionRpInnerVlanCfiNew:
                  case bcmFieldActionYpInnerVlanCfiNew:
                  case bcmFieldActionYpOuterVlanCfiNew:
                  case bcmFieldActionRpOuterVlanCfiNew:
                  case bcmFieldActionGpOuterVlanCfiNew:
                       *result = FALSE;
                       return BCM_E_NONE;
                  case bcmFieldActionLatencyMonitorEnable:
                  case bcmFieldActionLatencyMonitorDisable:
                       *result = TRUE;
                       return BCM_E_NONE;
                  default:
                       break;
             }
        default:
             break;
    }

    return (_field_th_stage_action_support_check(unit, stage,
                                                      action, result));
}


/*
 * Function:
 *      _bcm_field_th3_qualify_LoopbackType
 * Purpose:
 *      Encode the loopback type qualifier
 * Parameters:
 *      loopback_type - (IN) Loopback Type
 *      tcam_data     - (OUT) Qualifier data
 *      tcam_mask     - (OUT) Qualifier mask
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_field_th3_qualify_LoopbackType(
        bcm_field_LoopbackType_t loopback_type,
        uint32                   *tcam_data,
        uint32                   *tcam_mask)
{
    switch (loopback_type) {
        case bcmFieldLoopbackTypeAny:
            *tcam_data = 0xf;
            *tcam_mask = 0xf;
            break;
        default:
            return (BCM_E_UNAVAIL);
    }

    return (BCM_E_NONE);

}

/*
 * Function:
 *      _bcm_field_th3_qualify_LoopbackType_get
 * Purpose:
 *      Decode the loopback type qualifier
 * Parameters:
 *      unit  - (IN) BCM device number
 *      tcam_data     - (IN) Qualifier data
 *      tcam_mask     - (IN) Qualifier mask
 *      loopback_type - (OUT) Loopback Type
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_th3_qualify_LoopbackType_get(
            uint8                    tcam_data,
            uint8                    tcam_mask,
            bcm_field_LoopbackType_t *loopback_type)
{
    switch (tcam_data & tcam_mask) {
        case 0xf:
            *loopback_type = bcmFieldLoopbackTypeAny;
            break;
        default:
            return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}


int
_bcm_field_th3_qualify_TunnelType(bcm_field_TunnelType_t tunnel_type,
                                  uint32                 *tcam_data,
                                  uint32                 *tcam_mask
                                  )
{
    switch (tunnel_type) {
        case bcmFieldTunnelTypeAny:
            *tcam_data = 0x0;
            *tcam_mask = 0x0;
            break;
        case bcmFieldTunnelTypeNone:
            *tcam_data = 0x0;
            *tcam_mask = 0xf;
            break;
        case bcmFieldTunnelTypeIp:
            *tcam_data = 0x1;
            *tcam_mask = 0xf;
            break;
        case bcmFieldTunnelTypeMpls:
            *tcam_data = 0x2;
            *tcam_mask = 0xf;
            break;
         default:
            return (BCM_E_UNAVAIL);
    }
    return (BCM_E_NONE);
}

int
_bcm_field_th3_qualify_TunnelType_get(uint8                  tcam_data,
                                      uint8                  tcam_mask,
                                      bcm_field_TunnelType_t *tunnel_type
                                      )
{
    switch (tcam_data & tcam_mask) {
        case 0x1:
            *tunnel_type = bcmFieldTunnelTypeIp;
            break;
        case 0x2:
            *tunnel_type = bcmFieldTunnelTypeMpls;
            break;
        case 0x0:
            if(tcam_mask) {
                *tunnel_type = bcmFieldTunnelTypeNone;
                break;
            }
        default:
            return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}


/* Function    : _field_th3_qualify_IpFrag
 * Description : qualify on Ip Frag Info.
 * Parameters  : (IN) unit       BCM device number
 *               (IN) entry      Field entry to qualify
 *               (IN) qual_id    Field qualifier id.
 *               (IN) frag_info  bcm_field_IpFrag_t to qualify
 * Returns     : BCM_E_XXX
 */
int
_field_th3_qualify_IpFrag(int unit, bcm_field_entry_t entry,
                      bcm_field_qualify_t qual_id,
                      bcm_field_IpFrag_t frag_info)
{
    int             rv;
    uint32          data;
    uint32          mask;

     /*
     * 01 : Whole packet
     * 10 : First fragment
     */
    switch(frag_info) {
      case bcmFieldIpFragNonOrFirst:
          /* Non-fragmented packet */
          data = 0x02;
          mask = 0x02;
          break;
      case bcmFieldIpFragFirst:
          /* First fragment of fragmented packet*/
          data = 0x02;
          mask = 0x03;
          break;
      case bcmFieldIpFragNon:
          /* Non-fragmented packet */
          data = 0x01;
          mask = 0x01;
          break;
      case bcmFieldIpFragAny:
          /* Any fragment of fragmented packet */
          data = 0x00;
          mask = 0x01;
          break;
      case bcmFieldIpFragNotFirst:
          /* Not the first fragment */
          data = 0x00;
          mask = 0x03;
          break;
      default:
          return (BCM_E_PARAM);
    }

    rv = _field_qualify32(unit, entry, qual_id, data, mask);
    return (rv);
}


/* Function    : _field_th3_qualify_IpFrag_get
 * Description : qualify on Ip Frag Info.
 * Parameters  : (IN) unit       BCM device number
 *               (IN) entry      Field entry to qualify
 *               (IN) qual_id    Field qualifier id.
 *               (OUT) frag_info  bcm_field_IpFrag_t to qualify
 * Returns     : BCM_E_XXX
 */
int
_field_th3_qualify_IpFrag_get(int unit, bcm_field_entry_t entry,
                          bcm_field_qualify_t qual_id,
                          bcm_field_IpFrag_t *frag_info)
{
    uint32    hw_data = 0;       /* HW buffer data.             */
    uint32    hw_mask = 0;       /* HW buffer mask.             */
    int       rv;                /* Operation return stauts.    */

    /* Input parameters check. */
    if (NULL == frag_info) {
        return (BCM_E_PARAM);
    }

    /* Read qualifier match value and mask. */
    rv = _bcm_field_entry_qualifier_uint32_get(unit, entry,
                                               qual_id,
                                               &hw_data, &hw_mask);
    BCM_IF_ERROR_RETURN(rv);

    if ((hw_data == 0x2) && (hw_mask == 0x2)) {
        *frag_info = bcmFieldIpFragNonOrFirst;
    } else  if ((hw_data == 0x2) && (hw_mask == 0x3)) {
        *frag_info = bcmFieldIpFragFirst;
    } else  if ((hw_data == 0x1) && (hw_mask == 0x1)) {
        *frag_info = bcmFieldIpFragNon;
    } else  if ((hw_data == 0x0) && (hw_mask == 0x3)) {
        *frag_info = bcmFieldIpFragNotFirst;
    } else  if ((hw_data == 0x0) && (hw_mask == 0x1)) {
        *frag_info = bcmFieldIpFragAny;
    } else {
        return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th3_redirect_profile_get
 * Purpose:
 *     Get the redirect profile for the unit
 * Parameters:
 *     unit             - BCM device number
 *     redirect_profile - (OUT) redirect profile
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     For triumph, External TCAM and IFP refer to same IFP_REDIRECTION_PROFILEm
 */
int
_bcm_field_th3_redirect_profile_get(int unit,
                                soc_profile_mem_t **redirect_profile)
{
    _field_control_t *fc = NULL;
 
    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
 
    *redirect_profile = &fc->redirect_profile1;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th3_redirect_profile_ref_count_get
 * Purpose:
 *     Get redirect profile entry use count.
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     index     - (IN) Profile entry index.
 *     ref_count - (OUT) redirect profile use count.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th3_redirect_profile_ref_count_get(int unit,
                                              int index,
                                              int *ref_count)
{
    soc_profile_mem_t *redirect_profile;

    if (NULL == ref_count) {
        return (BCM_E_PARAM);
    }

    /* Get the redirect profile */
    BCM_IF_ERROR_RETURN
        (_bcm_field_th3_redirect_profile_get(unit, &redirect_profile));

    BCM_IF_ERROR_RETURN(soc_profile_mem_ref_count_get(unit, redirect_profile,
                                                      index, ref_count));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th3_redirect_profile_delete
 * Purpose:
 *     Delete redirect profile entry.
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     index     - (IN) Profile entry index.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th3_redirect_profile_delete(int unit, int index)
{
    soc_profile_mem_t *redirect_profile;

    /* Get the redirect profile */
    BCM_IF_ERROR_RETURN
        (_bcm_field_th3_redirect_profile_get(unit, &redirect_profile));

    BCM_IF_ERROR_RETURN(soc_profile_mem_delete(unit, redirect_profile, index));
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th3_class_ctype_supported
 * Purpose:
 *    Check if the class type is supported for the device.
 * Parameters:
 *     unit        - (IN) BCM Device number.
 *     ctype       - (IN) Field Class Type.
 * Returns:
 *     BCM_E_XXX
 */
int
_field_th3_class_ctype_supported(int unit,
                                 _field_class_type_t ctype)
{
    int rv = BCM_E_NONE;

    switch(ctype) {
        case _FieldClassEtherType:
        case _FieldClassTtl:
        case _FieldClassToS:
        case _FieldClassIpProto:
        case _FieldClassTcp:
        case _FieldClassIpTunnelTtl:
              rv = BCM_E_NONE;
              break;
        default:
              rv = BCM_E_UNAVAIL;
              break;
    }
    return rv;
}

/*
 * Function:
 *  _bcm_field_th3_qual_class_size_get
 * Purpose:
 *  Get Field Qualifier Class Size
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  qual        - (IN) Field Qualifier.
 *  class_size  - (OUT) Field Qualifier Class Size.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  This api returns class size for provided qualifier
 *  Size provided is in bits.
 */
int
_bcm_field_th3_qual_class_size_get(int unit, bcm_field_qualify_t qual,
                                                   uint16 *class_size)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */

    switch (qual) {
        case bcmFieldQualifyEtherType:
            *class_size = _FP_QUAL_CLASS_SIZE_ETHERTYPE;
            break;
        case bcmFieldQualifyTtl:
            *class_size = _FP_QUAL_CLASS_SIZE_TTL;
            break;
        case bcmFieldQualifyTos:
            *class_size = _FP_QUAL_CLASS_SIZE_TOS;
            break;
        case bcmFieldQualifyIpProtocol:
            *class_size = _FP_QUAL_CLASS_SIZE_IP_PROTO;
            break;
        case bcmFieldQualifyTcpControl:
            *class_size = _FP_QUAL_CLASS_SIZE_TCP;
            break;
        case bcmFieldQualifyVrf:
            *class_size = _FP_QUAL_CLASS_SIZE_SRC_COMPRESSION;
            break;
        case bcmFieldQualifyIpTunnelTtl:
            *class_size = _FP_QUAL_CLASS_SIZE_IPTUNNELTTL;
            break;
        default:
            rv = BCM_E_PARAM;
    }

    return (rv);
}

/*
 * Function:
 *     _bcm_th3_field_ddrop_profile_alloc
 * Purpose:
 *     Allocate redirect profile index
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     f_ent    - (IN) Field entry structure to get policy info from.
 *     fa       - (IN) Field action.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th3_ddrop_profile_alloc(int unit, _field_entry_t *f_ent,
                                   _field_action_t *fa)
{
    ifp_egress_port_check_for_drop_entry_t entry_arr[1];
    uint32            *entry_ptr[1];
    soc_profile_mem_t *ddrop_profile;
    int               rv;
    void              *entries[1];
    soc_mem_t         profile_mem = IFP_EGRESS_PORT_CHECK_FOR_DROPm;

    entry_ptr[0] = (uint32 *)(&entry_arr[0]);
    entries[0] = (void *)&entry_arr[0];

    if ((NULL == f_ent) || (NULL == fa)) {
        return (BCM_E_PARAM);
    }

    /* Reset redirection profile entry. */
    sal_memset(entry_arr, 0, sizeof(ifp_egress_port_check_for_drop_entry_t));

    /* Get the redirect profile */
    rv = _bcm_field_ddrop_profile_get(unit, &ddrop_profile);
    BCM_IF_ERROR_RETURN(rv);

    soc_mem_pbmp_field_set(unit, profile_mem, entry_ptr[0],
                           PORT_BITMAPf, (fa->egress_pbmp));

    soc_mem_field32_set(unit, profile_mem, entry_ptr[0],
                        ELEPHANT_PKT_ONLYf, (fa->elephant_pkts_only));

    rv = soc_profile_mem_add(unit, ddrop_profile, entries,
                             1, (uint32*)&fa->hw_index);
    return (rv);
}

/*
 * Function:
 *     _bcm_th3_field_dredirect_profile_alloc
 * Purpose:
 *     Allocate redirect profile index
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     f_ent    - (IN) Field entry structure to get policy info from.
 *     fa       - (IN) Field action.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th3_dredirect_profile_alloc(int unit, _field_entry_t *f_ent,
                                   _field_action_t *fa)
{
    ifp_egress_port_check_for_redirect_entry_t entry_arr[1];
    uint32            *entry_ptr[1];
    soc_profile_mem_t *dredirect_profile;
    int               rv;
    void              *entries[1];
    soc_mem_t         profile_mem = IFP_EGRESS_PORT_CHECK_FOR_REDIRECTm;

    entry_ptr[0] = (uint32 *)(&entry_arr[0]);
    entries[0] = (void *)&entry_arr[0];

    if ((NULL == f_ent) || (NULL == fa)) {
        return (BCM_E_PARAM);
    }

    /* Reset redirection profile entry. */
    sal_memset(entry_arr, 0, sizeof(ifp_egress_port_check_for_redirect_entry_t));

    /* Get the redirect profile */
    rv = _bcm_field_dredirect_profile_get(unit, &dredirect_profile);
    BCM_IF_ERROR_RETURN(rv);

    soc_mem_pbmp_field_set(unit, profile_mem, entry_ptr[0],
                           PORT_BITMAPf, (fa->egress_pbmp));

    soc_mem_field32_set(unit, profile_mem, entry_ptr[0],
                        ELEPHANT_PKT_ONLYf, (fa->elephant_pkts_only));
    rv = soc_profile_mem_add(unit, dredirect_profile, entries,
                             1, (uint32*)&fa->hw_index);
    return (rv);
}

int
_bcm_field_th3_internal_em_group_create(int unit,
                                    _field_control_t *fc) {

    int id = _FP_PRESEL_INTERNAL_RESERVED_ID;
    uint32 data32, mask32;
    _field_presel_entry_t     *f_presel = NULL; /* Internal Presel descriptor */
    _field_presel_info_t      *presel_info;     /* Presel information structure */
    _bcm_field_presel_flags_t flags = _bcmFieldPreselCreateWithId;
    int rv = 0;
    int classifier_id = 0;
    bcm_field_group_config_t group_config;
    bcm_port_t        port;
    bcm_pbmp_t        redirect_pbm;
    uint32  param[_FP_ACTION_PARAM_SZ];
    int i;

    /* Create Presel ID */

    /* Get the preselector information */
    presel_info = fc->presel_info;
    if (presel_info == NULL) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
             "ERROR: Field control for Presel Information returns NULL.\n\r")));
       return BCM_E_INTERNAL;
    }
    /* Check if presel ID is already created */
    if (BCM_FIELD_PRESEL_TEST(presel_info->presel_set, id)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "ERROR: Preselector ID[%d] is already created.\n\r"), id));
       return BCM_E_EXISTS;
    }
    /* Allocate presel descriptor. */
    _FP_XGS3_ALLOC(f_presel, sizeof (_field_presel_entry_t), "Field Presel entity");
    if (NULL == f_presel) {
       return BCM_E_MEMORY;
    }
    /* Initialize the newly created presel id */
    f_presel->presel_id   = id;
    /* Initialize the hw index to No HW resources allocated */
    f_presel->hw_index    = _FP_INVALID_INDEX;
    /* Initialize internal flags and stage information */
    f_presel->flags       = flags;
    /* Insert Presel into Preselector database */
    if (fc->presel_db == NULL) {
        sal_free(f_presel);
        return BCM_E_INTERNAL;
    }
    fc->presel_db[id] = f_presel;
    /* Update preselector global information */
    /* coverity[negative_shift : FALSE] */
    BCM_FIELD_PRESEL_ADD(presel_info->presel_set, id);

    rv = bcm_esw_field_qualify_Stage(unit, id | BCM_FIELD_QUALIFY_PRESEL,
                                     bcmFieldStageIngressExactMatch);

    if (BCM_FAILURE(rv)) {
        sal_free(f_presel);
        fc->presel_db[id] = NULL;
        BCM_FIELD_PRESEL_REMOVE(presel_info->presel_set, id);
        return rv;
    }

    /* Key: CPU_TX_CTRL
     * Upper 2 bits should be 2, which indicates  the CPU TX flow
     * Lower 4 bits should be a reserved encoding, say 15
     * ((0x2 << 4) | (0xf))
     */
    data32 = 0x2f;
    mask32 = 0x3f;
    /* coverity[address_of : FALSE] */
    /* coverity[callee_ptr_arith : FALSE] */
    rv = _bcm_field_presel_qualify_set(unit, id | BCM_FIELD_QUALIFY_PRESEL,
                                       _bcmFieldQualifyCpuTxCtrl,
                                       &data32, &mask32);

    if (BCM_FAILURE(rv)) {
        sal_free(f_presel);
        fc->presel_db[id] = NULL;
        BCM_FIELD_PRESEL_REMOVE(presel_info->presel_set, id);
        return rv;
    }

    rv = bcm_esw_field_action_add(unit, id | BCM_FIELD_QUALIFY_PRESEL,
                                  bcmFieldActionGroupClassSet, 0x3, 0);

    if (BCM_FAILURE(rv)) {
        sal_free(f_presel);
        fc->presel_db[id] = NULL;
        BCM_FIELD_PRESEL_REMOVE(presel_info->presel_set, id);
        return rv;
    }

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_ASET_INIT(group_config.aset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyGroupClass);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngressExactMatch);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCosMapNew);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionStatGroup);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionEgressPortsAdd);

    BCM_FIELD_PRESEL_ADD(group_config.preselset, id);
    group_config.flags = BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    group_config.group =  _FP_INTERNAL_RESERVED_EM_ID;
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_ID;
    rv = _bcm_field_th_group_add(unit, 0, &group_config);

    if (BCM_FAILURE(rv)) {
        sal_free(f_presel);
        fc->presel_db[id] = NULL;
        BCM_FIELD_PRESEL_REMOVE(presel_info->presel_set, id);
        return rv;
    }

    rv = _bcm_field_entry_create_id(unit,group_config.group,
                                    _FP_INTERNAL_RESERVED_EM_ID);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_esw_field_qualify_GroupClass(unit, _FP_INTERNAL_RESERVED_EM_ID,
                                          0x3, 0x3);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_th3_cosq_field_reserved_classifier_get(unit, &classifier_id,
                                                     &port);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_esw_field_action_add(unit, _FP_INTERNAL_RESERVED_EM_ID,
                                  bcmFieldActionCosMapNew, classifier_id, 0);
    BCM_IF_ERROR_RETURN(rv);

    BCM_PBMP_CLEAR(redirect_pbm);
    BCM_PBMP_PORT_ADD(redirect_pbm, port);

    for (i = 0; (i < _FP_ACTION_PARAM_SZ) && (i < SOC_PBMP_WORD_MAX) ; i++) {
        param[i] = SOC_PBMP_WORD_GET(redirect_pbm, i);
    }
    rv = _bcm_field_action_ports_add(unit, _FP_INTERNAL_RESERVED_EM_ID,
                                     bcmFieldActionEgressPortsAdd,
                                     param[0], param[1], param[2],
                                     param[3], param[4], param[5]);

    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_esw_field_entry_install(unit, _FP_INTERNAL_RESERVED_EM_ID);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;

}

/*
 * Function:
 *     _bcm_field_th3_em_udf_keygen_mask_get
 * Purpose:
 *  Get the mask to be applied for data qualifiers.
 * Parameters:
 *  unit         - (IN)     BCM device number.
 *  fg           - (IN)     Field group structure.
 *  qid          - (IN)     Qualifier number.
 *  mask         - (OUT)    Mask to be applied.
 * Returns:
 *  BCM_E_XXX
 */
int _bcm_field_th3_em_udf_keygen_mask_get(int unit,
                                         _field_group_t *fg,
                                         uint16 qid, uint32 *mask)
{
    int rv = BCM_E_NONE; /* Operational status. */
    uint8 idx = 0;       /* Index Variable. */
    _field_stage_t *stage_fc = NULL; /* Stage operational Structure. */

    /* Parameters Check. */
    if (NULL == fg) {
        return BCM_E_PARAM;
    }

    /* Exact Match and IFP shares UDF's */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    if (stage_fc->data_ctrl == NULL) {
       return BCM_E_INTERNAL;
    }

    /* Derive the offset of byte bit map in QSET's udf_map
     * for a data qualifier. */
    *mask = 0;
    switch (qid) {
         case _bcmFieldQualifyData0:
              idx = stage_fc->data_ctrl->num_elems * 2;
              break;
         case _bcmFieldQualifyData1:
              idx = (stage_fc->data_ctrl->num_elems * 2) + 2;
              break;
         case _bcmFieldQualifyData2:
              idx = (stage_fc->data_ctrl->num_elems * 2) + 4;
              break;
         case _bcmFieldQualifyData3:
              idx = (stage_fc->data_ctrl->num_elems * 2) + 6;
              break;
         case _bcmFieldQualifyData4:
              idx = (stage_fc->data_ctrl->num_elems * 2) + 8;
              break;
         case _bcmFieldQualifyData5:
              idx = (stage_fc->data_ctrl->num_elems * 2) + 10;
              break;
         case _bcmFieldQualifyData6:
              idx = (stage_fc->data_ctrl->num_elems * 2) + 12;
              break;
         case _bcmFieldQualifyData7:
              idx = (stage_fc->data_ctrl->num_elems * 2) + 14;
              break;
         case _bcmFieldQualifyData8:
              idx = (stage_fc->data_ctrl->num_elems * 2) + 16;
              break;
         case _bcmFieldQualifyData9:
              idx = (stage_fc->data_ctrl->num_elems * 2) + 18;
              break;
         case _bcmFieldQualifyData10:
              idx = (stage_fc->data_ctrl->num_elems * 2) + 20;
              break;
         case _bcmFieldQualifyData11:
              idx = (stage_fc->data_ctrl->num_elems * 2) + 22;
              break;
         case _bcmFieldQualifyData12:
              idx = (stage_fc->data_ctrl->num_elems * 2) + 24;
              break;
         case _bcmFieldQualifyData13:
              idx = (stage_fc->data_ctrl->num_elems * 2) + 26;
              break;
         case _bcmFieldQualifyData14:
              idx = (stage_fc->data_ctrl->num_elems * 2) + 28;
              break;
         case _bcmFieldQualifyData15:
              idx = (stage_fc->data_ctrl->num_elems * 2) + 30;
              break;
         default:
              *mask = 0xFFFFFFFF;
              break;
    }

    if (idx) {
        if (SHR_BITGET(fg->qset.udf_map, idx)) {
            *mask |= 0xFF;
        }

        if (SHR_BITGET(fg->qset.udf_map, idx + 1)) {
            *mask |= 0xFF00;
        }
    }

    return BCM_E_NONE;
}
#endif
