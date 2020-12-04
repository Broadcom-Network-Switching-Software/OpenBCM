/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field.c
 * Purpose:     BCM56624 Field Processor installation functions.
 */
   
#include <soc/defs.h>
#if defined(BCM_SCORPION_SUPPORT) && defined(BCM_FIELD_SUPPORT)

#include <soc/mem.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/l3.h>
#include <bcm/field.h>
#include <bcm/tunnel.h>

#include <bcm_int/esw/field.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/scorpion.h>



/* local/static function prototypes */
STATIC void _field_sc_functions_init(_field_funct_t *functions) ;
STATIC int _field_sc_detach(int unit, _field_control_t *fc);
STATIC int _field_sc_hw_clear(int unit, _field_stage_t *stage_fc);
/*
 * Function:
 *     _field_sc_ingress_qualifiers_init
 * Purpose:
 *     Initialize device stage ingress qaualifiers 
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_sc_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_DECL;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStage,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageIngress,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPorts,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPorts,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelDisable, 0, 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelDisable, 0, 7, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelDisable, 0, 11, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelDisable, 0, 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelDisable, 0, 13, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelDisable, 0, 14, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelDisable, 0, 15, 1);

    /* FPF3 primary slice single wide. */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 0, 18, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 1, 18, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 2, 18, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 2, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 0, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 1, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 3, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 3, 18, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, 0, 30, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 0, 18, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 1, 18, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 2, 18, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 2, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 0, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 1, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 3, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 3, 18, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, 1, 30, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 1, 38, 12);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf3, 2, 18, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf3, 2, 18, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf3, 2, 33, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf3, 2, 33, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 2, 48, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 3, 18, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 3, 18, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 3, 30, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 3, 31, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 3, 34, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 3, 34, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 3, 36, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 3, 38, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 3, 41, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf3, 3, 45, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 4, 18, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 4, 18, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 4, 30, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 4, 31, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 4, 34, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 4, 34, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 4, 46, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 4, 47, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 5, 18, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 5, 18, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 5, 30, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 5, 31, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 5, 34, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, 6, 19, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf3, 6, 20, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf3, 6, 21, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf3, 6, 23, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf3, 6, 24, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf3, 6, 25, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf3, 6, 27, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf3, 6, 28, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf3, 6, 29, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf3, 6, 30, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf3, 6, 31, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf3, 6, 32, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf3, 6, 33, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 6, 34, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 6, 34, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 6, 46, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 6, 47, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf3, 7, 18, 24);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, _bcmFieldQualifyRangeCheckBits24_31,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 3, 42, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 0, 42, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 1, 42, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 2, 42, 8);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf3, 8, 18, 20);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 8, 38, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 8, 38, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 8, 40, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 8, 42, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 8, 44, 2);

    /* FPF2 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTtlClassSelect, 0,
                 51, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTtlClassSelect, 0,
                 51, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTtlClassSelect, 1,
                 51, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTcpClassSelect, 0,
                 59, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTcpClassSelect, 0,
                 59, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTcpClassSelect, 1,
                 59, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
                 _bcmFieldSliceSelFpf2, 0, 65, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTosClassSelect, 0,
                 67, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTosClassSelect, 0,
                 67, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTosClassSelect, 1,
                 67, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 0, 75, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 0, 91, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 0, 107, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 0, 115, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 0, 147, 32);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTtlClassSelect, 0,
                 51, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTtlClassSelect, 0,
                 51, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTtlClassSelect, 1,
                 51, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTcpClassSelect, 0,
                 59, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTcpClassSelect, 0,
                 59, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTcpClassSelect, 1,
                 59, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf2, 1, 65, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTosClassSelect, 0,
                 67, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTosClassSelect, 0,
                 67, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTosClassSelect, 1,
                 67, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 1, 75, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 1, 91, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 1, 107, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 1, 115, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 1, 147, 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelFpf2, 2, 51, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf2, 3, 51, 128);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTtlClassSelect, 0,
                 51, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTtlClassSelect, 0,
                 51, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTtlClassSelect, 1,
                 51, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTcpClassSelect, 0,
                 59, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTcpClassSelect, 0,
                 59, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTcpClassSelect, 1,
                 59, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf2, 4, 65, 20);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTosClassSelect, 0,
                 85, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTosClassSelect, 0,
                 85, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTosClassSelect, 1,
                 85, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 4, 93, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 4, 101, 64);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 5, 51, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 5, 51, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 5, 63, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 5, 64, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 5, 67, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 5, 83, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5, 131, 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 6, 51, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 6, 51, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 6, 63, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 6, 64, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 6, 67, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 6, 83, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 6, 115, 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 7, 51, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 7, 51, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 7, 63, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 7, 64, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 7, 67, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 7, 83, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 7, 115, 48);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 8, 51, 128);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                 _bcmFieldSliceSelFpf2, 9, 51, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                 _bcmFieldSliceSelFpf2, 10, 51, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 10, 115, 64);


    /* FPF1 */ 
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 0, 179, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 1, 179, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 2, 179, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 2, 185, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 0, 185, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 1, 185, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 3, 185, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 3, 179, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf1, 0, 191, 10);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf1, 1, 179, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf1, 1, 179, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf1, 1, 194, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf1, 1, 194, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 1, 209, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf1, 2, 179, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf1, 2, 179, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 2, 194, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 0, 197, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 1, 197, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 2, 197, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 2, 203, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 0, 203, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 1, 203, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 3, 203, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 3, 197, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 3, 179, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 3, 179, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 3, 191, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 3, 192, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 3, 195, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 3, 195, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 3, 207, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 3, 208, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf1, 3, 211, 2);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 4, 179, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 4, 179, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 4, 191, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 4, 192, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 4, 195, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf1, 5, 180, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf1, 5, 181, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf1, 5, 182, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf1, 5, 184, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf1, 5, 185, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf1, 5, 186, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf1, 5, 188, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf1, 5, 189, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf1, 5, 190, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf1, 5, 191, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf1, 5, 192, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf1, 5, 193, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf1, 5, 194, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 5, 195, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 5, 195, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 5, 207, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 5, 208, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf1, 5, 211, 2);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 6, 179, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 6, 179, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 6, 191, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 6, 192, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 6, 195, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf1, 6, 195, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 6, 197, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 6, 201, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 6, 204, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf1, 6, 208, 3);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 7, 179, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 7, 179, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 7, 191, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 7, 192, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 0, 195, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 1, 195, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 3, 195, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 2, 195, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf1, 7, 201, 11);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf1, 8, 179, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf1, 8,
                 _bcmFieldSliceTosClassSelect, 0,
                 187, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                 _bcmFieldSliceSelFpf1, 8,
                 _bcmFieldSliceTosClassSelect, 0,
                 187, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                 _bcmFieldSliceSelFpf1, 8,
                 _bcmFieldSliceTosClassSelect, 1,
                 187, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 0, 195, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 1, 195, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 3, 195, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 2, 195, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf1, 8, 201, 11);


    /* DWF4 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf4, 1,
                            _bcmFieldSliceTcpClassSelect, 0, 0, 6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                            _bcmFieldSliceSelFpf4, 1,
                            _bcmFieldSliceTcpClassSelect, 0, 0, 6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                            _bcmFieldSliceSelFpf4, 1,
                            _bcmFieldSliceTcpClassSelect, 1, 0, 6);

    /* DWF3 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceTosClassSelect, 0, 6, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceTosClassSelect, 0, 6, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceTosClassSelect, 1, 6, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 14, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyExtensionHeaderSubCode,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 22, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 30, 8);

    /* DWF2 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 54, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 55, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 56, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 57, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 59, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 60, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 61, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 63, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 64, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 65, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 66, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 67, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 68, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 69, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 70, 24);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyRangeCheckBits24_31,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 3, 94, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 0, 94, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 1, 94, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 2, 94, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 102, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 134, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceSelDisable, 0, 38, 128);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                            _bcmFieldSliceSelFpf2, 2,
                            _bcmFieldSliceSelDisable, 0, 38, 128);
    /* DWF1 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceTtlClassSelect, 0, 166, 7);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceTtlClassSelect, 0, 166, 7);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceTtlClassSelect, 1, 166, 7);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 173, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 175, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 191, 16);
    /* DONE DWF */
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_sc_lookup_qualifiers_init
 * Purpose:
 *     Initialize device stage lookup qaualifiers 
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_sc_lookup_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_DECL;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Enable the overlay of Sender Ethernet Address onto MACSA
     * on ARP/RARP packets.
     */
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ING_CONFIG_64r, REG_PORT_ANY,
                                ARP_VALIDATION_ENf, 1));

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageLookup,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

    /* FPF3 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 0, 0, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 0, 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 0, 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 0, 16, 16);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, 0, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, 0, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, 16, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, 16, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, 0, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, 0, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIp6NextHeader,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, 8, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, 2, 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, 2, 24, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                 _bcmFieldSliceSelFpf3, 3, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 3, 8, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 3, 8, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 3, 20, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 3, 21, 3);

    /* FPF2 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerTtl,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 32, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 32, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0, 40, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpFrag,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 46, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 46, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 48, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 48, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 56, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 56, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 72, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 72, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 88, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 88, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 96, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 96, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 128, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 128, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerSrcIp6,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, 32, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, 32, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerDstIp6,
                               _bcmFieldSliceSelFpf2, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, 32, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                               _bcmFieldSliceSelFpf2, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, 32, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 3, 48, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 3, 64, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 3, 112, 48);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, 32, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, 32, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, 40, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, 40, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, 48, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, 48, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, 80, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, 80, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 4, 112, 48);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, 32, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, 32, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, 40, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, 40, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, 48, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, 48, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, 80, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, 80, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5, 112, 48);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerSrcIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 1, 32, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 0, 32, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerDstIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 1, 96, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 0, 96, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySnap,
                 _bcmFieldSliceSelFpf2, 7, 96, 40);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLlc,
                 _bcmFieldSliceSelFpf2, 7, 136, 24);

    /* FPF1 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocolCommon,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 160, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocolCommon,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 160, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelFpf1, 0, 163, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf1, 0, 165, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf1, 0, 167, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 0, 169, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf1, 0, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf1, 0, 172, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 0, 173, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 0, 173, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 0, 185, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 0, 186, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 0, 189, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort, 
                 _bcmFieldSliceSelFpf1, 0, 191, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort, 
                 _bcmFieldSliceSelFpf1, 0, 197, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpType,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 205, 4);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 205, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports, 
                 _bcmFieldSliceSelFpf1, 0, 209, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo, 
                 _bcmFieldSliceSelFpf1, 0, 4, 1);

    /* DWF3 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 0, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 0, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTtl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 8, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 8, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 16, 6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpFrag,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 22, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 22, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 24, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 24, 8);
    /* DWF2 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTtl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 32, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 32, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 40, 6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpFrag,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 46, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 46, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 48, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 48, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 56, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 56, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 72, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 72, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 88, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 88, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 96, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 96, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 128, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 128, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceIpHeaderSelect, 1, 32, 128);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceIpHeaderSelect, 0, 32, 128);

    /* FPF1 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 160, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 168, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 176, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 184, 16);
    return (BCM_E_NONE);
}


STATIC int
_field_sc_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_DECL;
    _key_fld_ = KEYf;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageEgress,
                 _bcmFieldSliceSelDisable, 0, 0, 0);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 21, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 23, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 39, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 87, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 141, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 142, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 143, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 144, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 145, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 148, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 161, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 167, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 168, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 169, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 169, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 181, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 182, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 185, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 187, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 188, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 196, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 202, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 206, 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 10, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 12, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 18, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 34, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 50, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 58, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 66, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 98, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 130, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 138, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 139, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 148, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 161, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 167, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 168, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 169, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 169, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 181, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 182, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 185, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 187, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 188, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 196, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 202, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 206, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 12, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 18, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 34, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 50, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 58, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 130, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 138, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 139, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 148, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 161, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 167, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 168, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 169, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 169, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 181, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 182, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 185, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 187, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 188, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 196, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 202, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 206, 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 2, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP6,
                               10, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_DIP6, 
                               10, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 138, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 146, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 147, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 148, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 161, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 167, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 168, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 169, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 169, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 181, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 182, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 185, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 187, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 188, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 196, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 202, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 206, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 10, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 12, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 18, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 34, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 50, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 58, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 186, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 194, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 202, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 206, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 0);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_sc_qualifiers_init
 * Purpose:
 *     Initialize device qaualifiers select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */
STATIC int
_field_sc_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    int rv;           /* Operation return status. */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_qual_arr, 
                   (_bcmFieldQualifyCount * sizeof(_bcm_field_qual_info_t *)), 
                   "Field qualifiers");
    if (NULL == stage_fc->f_qual_arr) {
        return (BCM_E_MEMORY);
    }
    
    switch (stage_fc->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          rv = _field_sc_ingress_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _field_sc_lookup_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _field_sc_egress_qualifiers_init(unit, stage_fc);
          break;
      default: 
          sal_free(stage_fc->f_qual_arr);
          return (BCM_E_PARAM);
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_field_sc_init
 * Purpose:
 *     Perform initializations that are specific to BCM5682x. This
 *     includes initializing the FP field select bit offset tables for FPF[1-3]
 *     for every stage. 
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     fc         - (IN) Field Processor control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */
int
_bcm_field_sc_init(int unit, _field_control_t *fc) 
{
    _field_stage_t *stage_p; /* Stages iteration pointer */

    /* Input parameters check. */
    if (NULL == fc) {
        return (BCM_E_PARAM);
    }

    stage_p = fc->stages;
    while (stage_p) {
        /* Clear hardware table */
        BCM_IF_ERROR_RETURN(_field_sc_hw_clear(unit, stage_p));

        /* Initialize qualifiers info. */
        BCM_IF_ERROR_RETURN(_field_sc_qualifiers_init(unit, stage_p));

        /* Goto next stage */
        stage_p = stage_p->next;
    }

    /* Initialize the TOS_FN, TTL_FN, TCP_FN tables */
    BCM_IF_ERROR_RETURN(_bcm_field_trx_tcp_ttl_tos_init(unit));
    
    if (!SOC_WARM_BOOT(unit)) {
        /* Enable filter processor */
        BCM_IF_ERROR_RETURN(_field_port_filter_enable_set(unit, fc, TRUE));

        /* Enable meter refresh */
        BCM_IF_ERROR_RETURN(_field_meter_refresh_enable_set(unit, fc, TRUE));
    }

    /* Initialize the function pointers */
    _field_sc_functions_init(&fc->functions);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_sc_hw_clear
 * Purpose:
 *     Clear hardware memory of requested stage. 
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */
STATIC int
_field_sc_hw_clear(int unit, _field_stage_t *stage_fc)
{
    if (SOC_WARM_BOOT(unit)) {
        return (BCM_E_NONE);
    }

    switch (stage_fc->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          BCM_IF_ERROR_RETURN
              (soc_mem_clear(unit, FP_TCAM_Xm, COPYNO_ALL, TRUE));
          BCM_IF_ERROR_RETURN
              (soc_mem_clear(unit, FP_TCAM_Ym, COPYNO_ALL, TRUE));
          BCM_IF_ERROR_RETURN
              (soc_mem_clear(unit, FP_RANGE_CHECKm, COPYNO_ALL, TRUE));
          BCM_IF_ERROR_RETURN
              (soc_mem_clear(unit, FP_PORT_FIELD_SELm, COPYNO_ALL, TRUE));
          BCM_IF_ERROR_RETURN
              (soc_mem_clear(unit, FP_POLICY_TABLEm, COPYNO_ALL, TRUE));
          BCM_IF_ERROR_RETURN
              (soc_mem_clear(unit, FP_SLICE_MAPm, COPYNO_ALL, TRUE));
          BCM_IF_ERROR_RETURN
              (soc_mem_clear(unit, FP_METER_TABLEm, COPYNO_ALL, TRUE));
          BCM_IF_ERROR_RETURN
              (soc_mem_clear(unit, FP_COUNTER_TABLE_Xm, COPYNO_ALL, TRUE));
          BCM_IF_ERROR_RETURN
              (soc_mem_clear(unit, FP_COUNTER_TABLE_Ym, COPYNO_ALL, TRUE));
          BCM_IF_ERROR_RETURN
              (soc_mem_clear(unit, FP_UDF_OFFSETm, COPYNO_ALL, TRUE));
          BCM_IF_ERROR_RETURN
              (soc_mem_clear(unit, FP_SLICE_KEY_CONTROLm, COPYNO_ALL, TRUE));
          break;

      case _BCM_FIELD_STAGE_LOOKUP:
          BCM_IF_ERROR_RETURN
              (soc_mem_clear((unit), VFP_TCAMm, COPYNO_ALL, TRUE));
          BCM_IF_ERROR_RETURN
              (soc_mem_clear((unit), VFP_POLICY_TABLEm, COPYNO_ALL, TRUE));
          break;

      case _BCM_FIELD_STAGE_EGRESS:
          BCM_IF_ERROR_RETURN
              (soc_mem_clear((unit), EFP_TCAMm, COPYNO_ALL, TRUE));
          BCM_IF_ERROR_RETURN
              (soc_mem_clear((unit), EFP_POLICY_TABLEm, COPYNO_ALL, TRUE));
          BCM_IF_ERROR_RETURN
              (soc_mem_clear((unit), EFP_METER_TABLEm, COPYNO_ALL, TRUE));
          BCM_IF_ERROR_RETURN
              (soc_mem_clear((unit), EFP_COUNTER_TABLEm, COPYNO_ALL, TRUE));
          break;
      default: 
           return (BCM_E_PARAM);

    } /* end switch */

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_sc_detach
 *
 * Purpose:
 *     Deallocates BCM56624 field tables.
 *
 * Parameters:
 *     unit - BCM device number
 *
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_sc_detach(int unit, _field_control_t *fc)
{
    _field_stage_t   *stage_p;  /* Stage iteration pointer. */

    stage_p = fc->stages;

    while (stage_p) { 
       /* Clear the hardware tables */
        BCM_IF_ERROR_RETURN(_field_sc_hw_clear(unit, stage_p));
      
        /* Goto next stage */
        stage_p = stage_p->next; 
    }

    /* Clear udf match criteria registers. */
    BCM_IF_ERROR_RETURN(_bcm_field_fb_udf_ipprotocol_delete_all(unit));
    BCM_IF_ERROR_RETURN(_bcm_field_fb_udf_ethertype_delete_all(unit));

    /* Don't disable port controls during Warmboot or Reloading state */
    if (0 == SOC_HW_ACCESS_DISABLE(unit)) {
        /* Clear the Filter Enable flags in the port table */
        BCM_IF_ERROR_RETURN (_field_port_filter_enable_set(unit, fc, FALSE));
    }

    BCM_IF_ERROR_RETURN (_field_meter_refresh_enable_set(unit, fc, FALSE));
 
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_sc_write_slice_map_ingress
 *
 * Purpose:
 *     Write the FP_SLICE_MAP (INGRESS)
 *
 * Parameters:
 *     unit
 *     stage_fc - pointer to stage control block
 *
 * Returns:
 *     nothing
 *     
 * Notes:
 */
STATIC int 
_field_sc_write_slice_map_ingress(int unit, _field_stage_t *stage_fc)
{
    fp_slice_map_entry_t map_entry;  /* HW entry buffer.         */
    soc_field_t field;               /* HW entry fields.         */
    int vmap_size;                   /* Virtual map index count. */
    uint32 value;                    /* Field entry value.       */
    int idx;                         /* Map fields iterator.     */     
    int rv;                          /* Operation return status. */

    uint32 virtual_to_physical_map_sc[] = {
        VIRTUAL_SLICE_0_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_1_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_2_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_3_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_4_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_5_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_6_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_7_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_8_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_9_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_10_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_11_PHYSICAL_SLICE_NUMBERf
    };
    uint32 virtual_to_group_map_sc[] = {
        VIRTUAL_SLICE_0_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_1_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_2_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_3_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_4_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_5_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_6_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_7_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_8_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_9_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_10_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_11_VIRTUAL_SLICE_GROUPf
    };

    /* Calculate virtual map size. */
    rv = _bcm_field_virtual_map_size_get(unit, stage_fc, &vmap_size); 
    BCM_IF_ERROR_RETURN(rv);

    rv = READ_FP_SLICE_MAPm(unit, MEM_BLOCK_ANY, 0, &map_entry);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < vmap_size; idx++) {
        value = stage_fc->vmap[_FP_DEF_INST][_FP_VMAP_DEFAULT][idx].vmap_key;
        field = virtual_to_physical_map_sc[idx];
        soc_FP_SLICE_MAPm_field32_set(unit, &map_entry, field, value);

        value = stage_fc->vmap[_FP_DEF_INST][_FP_VMAP_DEFAULT][idx].virtual_group;
        field = virtual_to_group_map_sc[idx];
        soc_FP_SLICE_MAPm_field32_set(unit, &map_entry, field, value);
    }
    rv = WRITE_FP_SLICE_MAPm(unit, MEM_BLOCK_ALL, 0, &map_entry);
    return (rv);
}


/*
 * Function:
 *     _field_sc_write_slice_map
 *
 * Purpose:
 *     Write the FP_SLICE_MAP
 *
 * Parameters:
 *     unit
 *     stage_fc - pointer to stage control block
 *     fg - reference to field group structure.
 *
 * Returns:
 *     nothing
 *     
 * Notes:
 */
STATIC int
_field_sc_write_slice_map(int unit, _field_stage_t *stage_fc,
                          _field_group_t *fg)
{
    int rv = BCM_E_PARAM;    /* Operation return status. */

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        rv = _field_sc_write_slice_map_ingress(unit, stage_fc);
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EGRESS) {
        rv = _bcm_field_trx_write_slice_map_egress(unit, stage_fc);
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_LOOKUP) {
        rv = _bcm_field_trx_write_slice_map_vfp(unit, stage_fc);
    }
    return (rv);
}

/*
 * Function:
 *     _field_sc_default_policer_set
 *
 * Purpose:
 *     Set metering portion of Policy Table.
 *
 * Parameters:
 *     unit      - (IN)BCM device number. 
 *     stage_fc  - (IN)Stage control structure. 
 *     level     - (IN)Policer level.
 *     mem       - (IN)Policy table memory. 
 *     buf       - (IN/OUT)Hardware policy entry
 *
 * Returns:
 *     BCM_E_NONE  - Success
 */
STATIC int
_field_sc_default_policer_set(int unit, _field_stage_t *stage_fc,
                               soc_mem_t mem, uint32 *buf)
{
    /* Input parameter check. */
    if ((NULL == stage_fc) || (NULL == buf))  {
        return (BCM_E_PARAM);
    }

    if (_BCM_FIELD_STAGE_INGRESS == stage_fc->stage_id) {
        /* Set the meter mode to default. */
        soc_mem_field32_set(unit, mem, buf, METER_PAIR_MODEf, 0);
        /* Preserve packet color. */
        soc_mem_field32_set(unit, mem, buf, METER_PAIR_MODE_MODIFIERf, 1);
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EGRESS) {
        /* Set the meter mode to default. */
        soc_mem_field32_set(unit, mem, buf, METER_PAIR_MODEf, 0);
        /* Preserve packet color. */
        soc_mem_field32_set(unit, mem, buf, METER_TEST_EVENf, 1);
    } else {
        return BCM_E_PARAM;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_sc_policer_action_set
 *
 * Purpose:
 *     Get metering portion of Policy Table.
 *
 * Parameters:
 *     unit      - (IN)BCM device number. 
 *     f_ent     - (IN)Software entry structure to get tcam info from.
 *     mem       - (IN)Policy table memory. 
 *     buf       - (IN/OUT)Hardware policy entry
 *
 * Returns:
 *     BCM_E_NONE  - Success
 */
STATIC int
_field_sc_policer_action_set(int unit, _field_entry_t *f_ent,
                                  soc_mem_t mem, uint32 *buf)
{
    _field_entry_policer_t  *f_ent_pl; /* Field entry policer descriptor.*/
    _field_stage_t          *stage_fc; /* Stage field control structure. */
    bcm_field_stage_t       stage_id;  /* Stage that contains meter.     */
    _field_policer_t        *f_pl;     /* Field policer descriptor.      */
    uint32                  meter_pair_mode;/* Hw meter usage bits.      */
    int                     meter_pair_idx; /* Meter pair index.         */
    int                     rv;             /* Operation return status.  */ 


    /* Input parameter check. */
    if ((NULL == f_ent) || (NULL == buf))  {
        return (BCM_E_PARAM);
    }
    if (NULL == f_ent->group) {
        return (BCM_E_PARAM);
    }

    stage_id = f_ent->group->stage_id;
    meter_pair_idx = 0;

    /* No policers at lookup stage. */
    if (_BCM_FIELD_STAGE_LOOKUP == stage_id) { 
        return (BCM_E_NONE);
    }

    /* Get stage control structure. */
    rv = _field_stage_control_get(unit, stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    f_ent_pl = f_ent->policer;

    if (0 == (f_ent_pl->flags & _FP_POLICER_INSTALLED)) {
        /* Install preserve the color policer. */
        rv = _field_sc_default_policer_set(unit, stage_fc, mem, buf);
        return (rv);
    }

    /* Get policer config. */
    rv = _bcm_field_policer_get(unit, f_ent_pl->pid, &f_pl);
    BCM_IF_ERROR_RETURN(rv);

    /* Get hw encoding for meter mode. */
    rv = _bcm_field_meter_pair_mode_get(unit, f_pl, &meter_pair_mode);
    BCM_IF_ERROR_RETURN(rv);

    /* Get meter hw table index. */
    if (_BCM_FIELD_STAGE_INGRESS == stage_fc->stage_id) {
        /* Pair index is (Pool number * Pairs in Pool + Pair number) */
        meter_pair_idx = (f_pl->pool_index *
        stage_fc->meter_pool[_FP_DEF_INST][f_pl->pool_index]->num_meter_pairs)
                            + f_pl->hw_index;
    } else if (_BCM_FIELD_STAGE_EGRESS == stage_fc->stage_id) {
        meter_pair_idx = f_pl->hw_index;
    }

    /* Get the even and odd indexes from the entry. The even and odd
     * meter indices are the same.
     */
    soc_mem_field32_set(unit, mem, buf, METER_INDEX_EVENf, meter_pair_idx);
    soc_mem_field32_set(unit, mem, buf, METER_INDEX_ODDf, meter_pair_idx);

    /* Get the meter mode */
    soc_mem_field32_set(unit, mem, buf, METER_PAIR_MODEf, meter_pair_mode);

    /* 
     * Flow mode is the only one that cares about the test and update bits.
     * Even = BCM_FIELD_METER_PEAK
     * Odd = BCM_FIELD_METER_COMMITTED
     */
    if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
        soc_mem_field32_set(unit, mem, buf, METER_TEST_ODDf, 0);
        soc_mem_field32_set(unit, mem, buf, METER_TEST_EVENf, 1);
        soc_mem_field32_set(unit, mem, buf, METER_UPDATE_ODDf, 0);
        soc_mem_field32_set(unit, mem, buf, METER_UPDATE_EVENf, 1);
    } else if (_FP_POLICER_COMMITTED_HW_METER(f_pl)) {
        soc_mem_field32_set(unit, mem, buf, METER_TEST_ODDf, 1);
        soc_mem_field32_set(unit, mem, buf, METER_TEST_EVENf, 0);
        soc_mem_field32_set(unit, mem, buf, METER_UPDATE_ODDf, 1);
        soc_mem_field32_set(unit, mem, buf, METER_UPDATE_EVENf, 0);
    } 

    if (_BCM_FIELD_STAGE_INGRESS == stage_fc->stage_id) {
        if (_FP_POLICER_COMMITTED_HW_METER(f_pl)
            || (f_pl->cfg.mode == bcmPolicerModePassThrough)) {
            soc_mem_field32_set(unit, mem, buf, METER_PAIR_MODE_MODIFIERf, 1);
        } else if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
            soc_mem_field32_set(unit, mem, buf, METER_PAIR_MODE_MODIFIERf, 0);
        }
    } else if ((_BCM_FIELD_STAGE_EGRESS == stage_fc->stage_id)
                && (f_pl->cfg.mode == bcmPolicerModePassThrough)) {
        soc_mem_field32_set(unit, mem, buf, METER_TEST_EVENf, 1);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_sc_entry_reinstall
 * Purpose:
 *     Write policy table entry information into the chip's memory. 
 * Parameters:
 *     unit     -   BCM Unit
 *     f_ent    -   Physical entry structure to be installed 
 *     tcam_idx - common index of various tables
 * Returns:
 *     BCM_E_XXX        On TCAM read/write errors
 *     BCM_E_NONE
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_sc_entry_reinstall(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    uint32  e[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to fill Policy & TCAM entry.*/
    soc_mem_t        tcam_mem;          /* TCAM memory id.                    */
    soc_mem_t        policy_mem;        /* Policy table memory id .           */
    _field_action_t *fa = NULL;

    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    if (NULL == f_ent->fs) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN
        (_field_fb_tcam_policy_mem_get(unit, f_ent->fs->stage_id, 
                                       &tcam_mem, &policy_mem));

    if ((tcam_idx < soc_mem_index_min(unit, tcam_mem)) ||
        (tcam_idx > soc_mem_index_max(unit, tcam_mem))) {
        return (BCM_E_PARAM);
    }

    /* Handle the actions (policy) */
    /* Start with an empty POLICY entry */
    sal_memset(e, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    /* Extract the policy info from the entry structure. */
    for (fa = f_ent->actions; ((fa != NULL) && (_FP_ACTION_VALID & fa->flags)); fa = fa->next) {
        BCM_IF_ERROR_RETURN(
            _bcm_field_trx_action_get(unit, policy_mem, f_ent,
                                      tcam_idx, fa, e));
    }

    /* Handle color dependence/independence */
    if (soc_mem_field_valid(unit, policy_mem, GREEN_TO_PIDf)) {
        soc_mem_field32_set(unit, policy_mem, e, GREEN_TO_PIDf, 
                   (f_ent->flags & _FP_ENTRY_COLOR_INDEPENDENT) ? 1 : 0);
    }

    /* Extract meter related policy fields */
    BCM_IF_ERROR_RETURN(_field_sc_policer_action_set(unit, f_ent, 
                                                          policy_mem, e));

    /* Extract counter related policy fields */
    BCM_IF_ERROR_RETURN(_bcm_field_trx_stat_action_set(unit, f_ent, 
                                                       policy_mem,
                                                       tcam_idx, e));

    /* Write the POLICY Table */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, policy_mem, 
                                      MEM_BLOCK_ALL, tcam_idx, e));
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_sc_entry_install
 * Purpose:
 *     Write entry into the chip's memory. 
 * Parameters:
 *     unit     -   BCM Unit
 *     f_ent    -   Physical entry structure to be installed 
 *     tcam_idx - common index of various tables
 * Returns:
 *     BCM_E_XXX        On TCAM read/write errors
 *     BCM_E_NONE
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_sc_entry_install(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    uint32  e[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to fill Policy & TCAM entry.*/
    soc_mem_t        tcam_mem;          /* TCAM memory id.                    */
    soc_mem_t        policy_mem;        /* Policy table memory id .           */
    _field_action_t *fa = NULL;

    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    if (NULL == f_ent->fs) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN
        (_field_fb_tcam_policy_mem_get(unit, f_ent->fs->stage_id, 
                                       &tcam_mem, &policy_mem));

    if ((tcam_idx < soc_mem_index_min(unit, tcam_mem)) ||
        (tcam_idx > soc_mem_index_max(unit, tcam_mem))) {
        return (BCM_E_PARAM);
    }

    /* First Erase the TCAM: Valid bits = b00 */
    sal_memset(e, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx, e));

    /* Handle the actions (policy) */
    /* Start with an empty POLICY entry */
    sal_memset(e, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    /* Extract the policy info from the entry structure. */
    for (fa = f_ent->actions; ((fa != NULL) && (_FP_ACTION_VALID & fa->flags)); fa = fa->next) {
        BCM_IF_ERROR_RETURN(
            _bcm_field_trx_action_get(unit, policy_mem, f_ent,
                                      tcam_idx, fa, e));
    }

    /* Handle color dependence/independence */
    if (soc_mem_field_valid(unit, policy_mem, GREEN_TO_PIDf)) {
        soc_mem_field32_set(unit, policy_mem, e, GREEN_TO_PIDf, 
                   (f_ent->flags & _FP_ENTRY_COLOR_INDEPENDENT) ? 1 : 0);
    }

    /* Extract meter related policy fields */
    BCM_IF_ERROR_RETURN(_field_sc_policer_action_set(unit, f_ent, 
                                                          policy_mem, e));

    /* Extract counter related policy fields */
    BCM_IF_ERROR_RETURN(_bcm_field_trx_stat_action_set(unit, f_ent, 
                                                       policy_mem,
                                                       tcam_idx, e));

    /* Write the POLICY Table */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, policy_mem, 
                                      MEM_BLOCK_ALL, tcam_idx, e));

    /* Handle the rules (TCAM) */
    /* Start with an empty TCAM entry */
    sal_memset(e, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    /* Extract the qualifier info from the entry structure. */
    BCM_IF_ERROR_RETURN(_bcm_field_trx_tcam_get(unit, tcam_mem, f_ent, e));

    /* 
     * Write the IPBM table if necessary 
     *     For wide/paired mode entries, f_ent->pbmp will be valid ONLY
     * for one of the entries. For others it will be zero. f_ent->pbmp.mask
     * will take care of this.
     */
    if ((f_ent->fs->stage_id == _BCM_FIELD_STAGE_INGRESS) &&
        ((BCM_FIELD_QSET_TEST(f_ent->group->qset, bcmFieldQualifyInPort)) ||
         (BCM_FIELD_QSET_TEST(f_ent->group->qset, bcmFieldQualifyInPorts)))) {
        soc_mem_field_set(unit, FP_TCAMm, e, IPBMf, 
                          (uint32 *)&(f_ent->pbmp.data));
        soc_mem_field_width_fit_set(unit, FP_TCAMm, e, 
                          IPBM_MASKf, (uint32 *)&(f_ent->pbmp.mask));
    }
    /* Write the TCAM Table */
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx, e));

    if (f_ent->fs->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, FP_TCAM_Ym, MEM_BLOCK_ALL, tcam_idx, e));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_sc_counter_write
 * Purpose:
 *     Write accumulated sw & hw counters 
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   stage_fc      - (IN)  Stage field control. 
 *   counter_x_mem - (IN)  HW memory name.
 *   mem_x_buf     - (IN)  Memory table entry.
 *   counter_y_mem - (IN)  HW memory name.
 *   mem_y_buf     - (IN)  Memory table entry.
 *   idx           - (IN)  Counter index.
 *   packet_count  - (OUT) Packet Counter value
 *   byte_count    - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
int
_field_sc_counter_write(int unit, soc_mem_t mem, int idx, uint32 *buf,
                        _field_counter32_collect_t *cntrs32_buf,
                        _field_counter64_collect_t *cntrs64_buf,
                        uint64 *packet_count, uint64 *byte_count)
{
    uint32  hw_val[2];     /* Parsed field counter value.*/

    /* Set byte count in the memory */
    if (NULL != byte_count) {
        COMPILER_64_ZERO(cntrs64_buf->accumulated_counter);
        COMPILER_64_OR(cntrs64_buf->accumulated_counter, (*byte_count));
        COMPILER_64_SET(cntrs64_buf->last_hw_value,
                        COMPILER_64_HI(*byte_count) & 0x7,
                        COMPILER_64_LO(*byte_count));
        hw_val[0] = COMPILER_64_LO(cntrs64_buf->last_hw_value);
        hw_val[1] = COMPILER_64_HI(cntrs64_buf->last_hw_value);
        soc_mem_field_set(unit, mem, buf, BYTE_COUNTERf, hw_val);
    }

    /* Set packet count in the memory */
    if (NULL != packet_count) {
        COMPILER_64_ZERO(cntrs32_buf->accumulated_counter);
        COMPILER_64_OR(cntrs32_buf->accumulated_counter,
                       (*packet_count));
        cntrs32_buf->last_hw_value = 
            (COMPILER_64_LO(*packet_count) & 0x1fffffff);
        hw_val[0] = cntrs32_buf->last_hw_value;
        hw_val[1] = 0;
        soc_mem_field_set(unit, mem, buf, PACKET_COUNTERf, hw_val);
    }
    return soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, buf);
}

/*
 * Function:
 *     _field_sc_counter_set
 * Purpose:
 *     Write accumulated sw and hw counters
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   stage_fc      - (IN)  Stage field control. 
 *   counter_x_mem - (IN)  HW memory name.
 *   mem_x_buf     - (IN)  Memory table entry.
 *   counter_y_mem - (IN)  HW memory name.
 *   mem_y_buf     - (IN)  Memory table entry.
 *   idx           - (IN)  Counter index.
 *   packet_count  - (OUT) Packet Counter value
 *   byte_count    - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_sc_counter_set(int unit, _field_stage_t *stage_fc,
                      soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                      soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                      int idx, uint64 *packet_count, uint64 *byte_count)
{
    _field_counter32_collect_t *cntrs32_buf;  /* Sw packet counter value    */
    _field_counter64_collect_t *cntrs64_buf;  /* Sw byte counter value      */
    int                        rv;            /* Operation return status.   */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Write X pipeline counter value. */
    if (NULL != mem_x_buf) {
        cntrs64_buf = &stage_fc->_field_x64_counters[idx];
        cntrs32_buf = &stage_fc->_field_x32_counters[idx];
        rv = _field_sc_counter_write(unit, counter_x_mem, idx, mem_x_buf,
                                     cntrs32_buf, cntrs64_buf, packet_count,
                                     byte_count);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Write 0 to Y pipeline counter value. */
    if (NULL != packet_count) {
        COMPILER_64_ZERO(*packet_count);
    }

    if (NULL != byte_count) {
        COMPILER_64_ZERO(*byte_count);
    }

    if (NULL != mem_y_buf) {
        cntrs64_buf = &stage_fc->_field_y64_counters[idx];
        cntrs32_buf = &stage_fc->_field_y32_counters[idx];
        rv = _field_sc_counter_write(unit, counter_y_mem, idx, mem_y_buf,
                                     cntrs32_buf, cntrs64_buf, 
                                     packet_count, byte_count);
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_sc_counter_read
 * Purpose:
 *     Update accumulated sw counter and return current counter value.
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   stage_fc      - (IN)  Stage field control. 
 *   counter_x_mem - (IN)  HW memory name.
 *   mem_x_buf     - (IN)  Memory table entry.
 *   counter_y_mem - (IN)  HW memory name.
 *   mem_y_buf     - (IN)  Memory table entry.
 *   idx           - (IN)  Counter index.
 *   packet_count  - (OUT) Packet Counter value
 *   byte_count    - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
int
_field_sc_counter_read(int unit, soc_memacc_t *memacc_byte,
                       soc_memacc_t *memacc_pkt, uint32 *buf,
                        _field_counter32_collect_t *cntrs32_buf,
                        _field_counter64_collect_t *cntrs64_buf,
                        uint64 *packet_count, uint64 *byte_count)
{
    uint32  hw_val[2];     /* Parsed field counter value.*/

    /* Byte counter. */
    hw_val[0] = hw_val[1] = 0;
    soc_memacc_field_get(memacc_byte, buf, hw_val);
    _bcm_field_35bit_counter_update(unit, hw_val, cntrs64_buf);
    if (NULL != byte_count) {
        COMPILER_64_ADD_64(*byte_count, cntrs64_buf->accumulated_counter);
    }

    /* Packet counter. */
    hw_val[0] = hw_val[1] = 0;
    soc_memacc_field_get(memacc_pkt, buf, hw_val);
    _bcm_field_29bit_counter_update(unit, hw_val, cntrs32_buf);
    if (NULL != packet_count) {
        COMPILER_64_ADD_64(*packet_count, cntrs32_buf->accumulated_counter);
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_sc_counter_get
 * Purpose:
 *     Update accumulated sw counter and return current counter value.
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   stage_fc      - (IN)  Stage field control. 
 *   counter_x_mem - (IN)  HW memory name.
 *   mem_x_buf     - (IN)  Memory table entry.
 *   counter_y_mem - (IN)  HW memory name.
 *   mem_y_buf     - (IN)  Memory table entry.
 *   idx           - (IN)  Counter index.
 *   packet_count  - (OUT) Packet Counter value
 *   byte_count    - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_sc_counter_get(int unit, _field_stage_t *stage_fc,
                      soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                      soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                      int idx, uint64 *packet_count, uint64 *byte_count)
{
    _field_counter32_collect_t *cntrs32_buf;  /* Sw packet counter value    */
    _field_counter64_collect_t *cntrs64_buf;  /* Sw byte counter value      */
    soc_memacc_t *memacc_pkt_x, *memacc_byte_x; /* Memory access cache.   */
    soc_memacc_t *memacc_pkt_y, *memacc_byte_y; /* Memory access cache.   */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Extract X pipeline counter value. */
    if (NULL != mem_x_buf) {
        cntrs64_buf = &stage_fc->_field_x64_counters[idx];
        cntrs32_buf = &stage_fc->_field_x32_counters[idx];
        memacc_byte_x =
            &(stage_fc->_field_memacc_counters[_FIELD_COUNTER_MEMACC_BYTE]);
        memacc_pkt_x =
            &(stage_fc->_field_memacc_counters[_FIELD_COUNTER_MEMACC_PACKET]);
        SOC_IF_ERROR_RETURN
            (_field_sc_counter_read(unit, memacc_byte_x, memacc_pkt_x,
                                    mem_x_buf,
                                    cntrs32_buf, cntrs64_buf, 
                                    packet_count, byte_count));
    }

    /* Extract Y pipeline counter value. */
    if (NULL != mem_y_buf) {
        cntrs64_buf = &stage_fc->_field_y64_counters[idx];
        cntrs32_buf = &stage_fc->_field_y32_counters[idx];
        memacc_byte_y =
            &(stage_fc->_field_memacc_counters[_FIELD_COUNTER_MEMACC_BYTE_Y]);
        memacc_pkt_y =
            &(stage_fc->_field_memacc_counters[_FIELD_COUNTER_MEMACC_PACKET_Y]);
        SOC_IF_ERROR_RETURN
            (_field_sc_counter_read(unit, memacc_byte_y, memacc_pkt_y,
                                    mem_y_buf,
                                    cntrs32_buf, cntrs64_buf, 
                                    packet_count, byte_count));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_sc_selcode_get
 * Purpose:
 *     Finds a 4-tuple of select encodings that will satisfy the
 *     requested qualifier set (Qset).
 * Parameters:
 *     unit      - (IN) BCM unit number.
 *     stage_fc  - (IN) Stage Field control structure.
 *     qset_req  - (IN) Client qualifier set.
 *     fg        - (IN/OUT)Select code information filled into the group.  
 * 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_sc_selcode_get(int unit, _field_stage_t *stage_fc, 
                      bcm_field_qset_t *qset_req,
                      _field_group_t *fg)
{
    int  rv;                     /* Operation return status. */

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == qset_req) || (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    /* Egress qualifiers are selected based on Key. */
    switch (stage_fc->stage_id) {
      case _BCM_FIELD_STAGE_EGRESS:
          rv =  _bcm_field_trx_egress_selcode_get(unit, stage_fc, qset_req, fg);
          break;
      default:
          rv =  _bcm_field_selcode_get(unit, stage_fc, qset_req, fg);
    }
    return (rv);
}



/*
 * Function:
 *     _field_sc_functions_init
 *
 * Purpose:
 *     Set up functions pointers 
 *
 * Parameters:
 *     stage_fc - (IN/OUT) pointers to stage control block whe the device 
 *                         and stage specific functions will be registered.
 *
 * Returns:
 *     nothing
 * Notes:
 */
STATIC void
_field_sc_functions_init(_field_funct_t *functions)
{
    functions->fp_detach               = _field_sc_detach;
    functions->fp_group_install        = _bcm_field_fb_group_install;
    functions->fp_selcodes_install     = _bcm_field_trx_selcodes_install;
    functions->fp_slice_clear          = _bcm_field_trx_slice_clear;
    functions->fp_entry_remove         = _bcm_field_fb_entry_remove;
    functions->fp_entry_move           = _bcm_field_fb_entry_move;
    functions->fp_selcode_get          = _field_sc_selcode_get;
    functions->fp_selcode_to_qset      = _bcm_field_selcode_to_qset;
    functions->fp_qual_list_get        = _bcm_field_qual_lists_get;
    functions->fp_tcam_policy_install  = _field_sc_entry_install;
    functions->fp_tcam_policy_reinstall  = _field_sc_entry_reinstall;
    functions->fp_policer_install      = _bcm_field_trx_policer_install;
    functions->fp_qualify_ip_type      = _bcm_field_trx_qualify_ip_type;
    functions->fp_qualify_ip_type_get  = _bcm_field_trx_qualify_ip_type_get;
    functions->fp_counter_get          = _field_sc_counter_get;
    functions->fp_counter_set          = _field_sc_counter_set;
    functions->fp_stat_index_get       = _bcm_field_trx_stat_index_get;
    functions->fp_egress_key_match_type_set =
        _bcm_field_trx_egress_key_match_type_set;
    functions->fp_tcam_policy_clear       = NULL;
    functions->fp_action_support_check    = _bcm_field_trx_action_support_check;
    functions->fp_action_conflict_check   = _bcm_field_trx_action_conflict_check;
    functions->fp_write_slice_map         = _field_sc_write_slice_map;
    functions->fp_external_entry_install  = NULL;
    functions->fp_external_entry_reinstall  = NULL;
    functions->fp_external_entry_remove   = NULL;
    functions->fp_external_entry_prio_set = NULL;
    functions->fp_action_params_check  = _bcm_field_trx_action_params_check;
    functions->fp_data_qualifier_ethertype_add = 
                       _bcm_field_fb_data_qualifier_ethertype_add;
    functions->fp_data_qualifier_ethertype_delete= 
                       _bcm_field_fb_data_qualifier_ethertype_delete;
    functions->fp_data_qualifier_ip_protocol_add = 
                       _bcm_field_fb_data_qualifier_ip_protocol_add;
    functions->fp_data_qualifier_ip_protocol_delete= 
                       _bcm_field_fb_data_qualifier_ip_protocol_delete;
    functions->fp_data_qualifier_packet_format_add= 
                       _bcm_field_fb_data_qualifier_packet_format_add;
    functions->fp_data_qualifier_packet_format_delete=
                       _bcm_field_fb_data_qualifier_packet_format_delete;
    functions->fp_stat_value_get = NULL;
    functions->fp_stat_value_set = NULL;
    functions->fp_control_set = _bcm_field_control_set;
    functions->fp_control_get = _bcm_field_control_get;
    functions->fp_stat_hw_mode_get = _bcm_field_stat_hw_mode_get;
    functions->fp_stat_hw_alloc = _bcm_field_stat_hw_alloc;
    functions->fp_stat_hw_free = _bcm_field_stat_hw_free;
    functions->fp_group_add = NULL;
    functions->fp_qualify_trunk = _bcm_field_qualify_trunk;
    functions->fp_qualify_trunk_get = _bcm_field_qualify_trunk_get;
    functions->fp_qualify_inports = _bcm_field_qualify_InPorts;
    functions->fp_entry_stat_extended_attach = NULL;
    functions->fp_entry_stat_extended_get = NULL;
    functions->fp_entry_stat_detach = _bcm_field_entry_stat_detach;
    functions->fp_class_size_get = NULL;
    functions->fp_qualify_packet_res = _field_qualify_PacketRes;
    functions->fp_qualify_packet_res_get = _field_qualify_PacketRes_get;
}
#else /* BCM_SCORPION_SUPPORT && BCM_FIELD_SUPPORT */
typedef int _scorpion_field_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_SCORPION_SUPPORT && BCM_FIELD_SUPPORT */
