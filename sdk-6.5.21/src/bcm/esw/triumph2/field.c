/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field.c
 * Purpose:     BCM56624 Field Processor installation functions.
 */

#include <soc/defs.h>
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(BCM_FIELD_SUPPORT)

#include <soc/mem.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/l3.h>
#include <bcm/field.h>
#include <bcm/tunnel.h>

#include <bcm_int/esw_dispatch.h>

#include <bcm_int/esw/field.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>

extern int _field_trx_redirect_profile_get(int unit, soc_profile_mem_t
                                           **redirect_profile);

/* local/static function prototypes */
STATIC void _field_tr2_functions_init(_field_funct_t *functions) ;

/*
 * Function:
 *     _field_tr2_ingress_qualifiers_init
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
_field_tr2_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    int ipbm_pairing_overlay_offset = 0;
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

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelDisable, 0, 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelDisable, 0, 8, 3);
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifySvpValid,
                 _bcmFieldSliceSelDisable, 0, 11, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelDisable, 0, 12, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelDisable, 0, 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelDisable, 0, 17, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyLoopback,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 0, 21, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyLoopbackType,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 0, 18, 4);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTunnelType,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 1, 18, 4);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 2, 18, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelDisable, 0, 22, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelDisable, 0, 23, 1);

    /* FPF3 primary slice single wide. */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 0, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 1, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 2, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 2, 30, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 0, 30, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 1, 30, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 3, 30, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 3, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVrf, 36, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVpn, 36, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan, 36, 12);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 48, 13);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport, 48, 13);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport, 48, 13);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityL3Egress, 48, 13);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 0, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 1, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 2, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 2, 30, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 0, 30, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 1, 30, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 3, 30, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 3, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVrf, 36, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVpn, 36, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan, 36, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 1, 48, 12);


    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 24, 17);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstWlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityWlanGport, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityGlp, 41, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityGlp, 41, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 41, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityVlanGport, 41, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityMimGport, 41, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityWlanGport, 41, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport, 41, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport, 47, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport, 48, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityCommonGport, 41, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 2, 56, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 3, 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 3, 24, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 3, 36, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 3, 37, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 3, 40, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 3, 42, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 3, 44, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 3, 46, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 3, 49, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf3, 3, 54, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 4, 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 4, 24, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 4, 36, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 4, 37, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 4, 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 4, 40, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 4, 52, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 4, 53, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 5, 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 5, 24, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 5, 36, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 5, 37, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 5, 40, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelFpf3, 6, 24, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, 6, 25, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf3, 6, 26, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf3, 6, 27, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf3, 6, 29, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf3, 6, 30, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf3, 6, 31, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf3, 6, 33, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf3, 6, 34, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf3, 6, 35, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf3, 6, 36, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf3, 6, 37, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf3, 6, 38, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf3, 6, 39, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 6, 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 6, 40, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 6, 52, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 6, 53, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf3, 7, 24, 24);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, _bcmFieldQualifyRangeCheckBits24_31,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 3, 48, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 0, 48, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 1, 48, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 1, 48, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 2, 48, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf3, 8, 24, 20);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 8, 44, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 8, 46, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 8, 48, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 8, 50, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 8, 52, 2);
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData3, 
                 _bcmFieldSliceSelFpf3, 9, 24, 32); 

    /* FPF2 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTtlClassSelect, 0,
                 61, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTtlClassSelect, 0,
                 61, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTtlClassSelect, 1,
                 61, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTcpClassSelect, 0,
                 69, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTcpClassSelect, 0,
                 69, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTcpClassSelect, 1,
                 69, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
                 _bcmFieldSliceSelFpf2, 0, 75, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTosClassSelect, 0,
                 77, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTosClassSelect, 0,
                 77, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTosClassSelect, 1,
                 77, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 0, 85, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 0, 101, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf2, 0, 101, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 0, 117, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 0, 125, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 0, 157, 32);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTtlClassSelect, 0,
                 61, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTtlClassSelect, 0,
                 61, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTtlClassSelect, 1,
                 61, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTcpClassSelect, 0,
                 69, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTcpClassSelect, 0,
                 69, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTcpClassSelect, 1,
                 69, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf2, 1, 75, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTosClassSelect, 0,
                 77, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTosClassSelect, 0,
                 77, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTosClassSelect, 1,
                 77, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 1, 85, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 1, 101, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf2, 1, 101, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 1, 117, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 1, 125, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 1, 157, 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelFpf2, 2, 61, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf2, 3, 61, 128);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTtlClassSelect, 0,
                 61, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTtlClassSelect, 0,
                 61, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTtlClassSelect, 1,
                 61, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTcpClassSelect, 0,
                 69, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTcpClassSelect, 0,
                 69, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTcpClassSelect, 1,
                 69, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf2, 4, 75, 20);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTosClassSelect, 0,
                 95, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTosClassSelect, 0,
                 95, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTosClassSelect, 1,
                 95, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 4, 103, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 4, 125, 64);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 5, 61, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 5, 61, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 5, 73, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 5, 74, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 5, 77, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 5, 93, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5, 141, 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 6, 61, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 6, 61, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 6, 73, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 6, 74, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 6, 77, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 6, 93, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 6, 141, 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 7, 61, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 7, 61, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 7, 73, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 7, 74, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 7, 77, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 7,
                 _bcmFieldSliceTtlClassSelect, 0,
                 93, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                 _bcmFieldSliceSelFpf2, 7,
                 _bcmFieldSliceTtlClassSelect, 0,
                 93, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                 _bcmFieldSliceSelFpf2, 7,
                 _bcmFieldSliceTtlClassSelect, 1,
                 93, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 7, 101, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 7, 109, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 7, 141, 48);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 8, 61, 128);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                 _bcmFieldSliceSelFpf2, 9, 61, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                 _bcmFieldSliceSelFpf2, 10, 61, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 10, 125, 64);


    /* FPF1 - 0 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 0, 189, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 1, 189, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 2, 189, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 2, 195, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 0, 195, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 1, 195, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 3, 195, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 3, 189, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVrf, 201, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVpn, 201, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan, 201, 12);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 213, 13);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport, 213, 13);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport, 213, 13);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityL3Egress, 213, 13);

    /* FPF1 - 1 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstWlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityWlanGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityGlp, 206, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityGlp, 206, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 206, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityMimGport, 206, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityWlanGport, 206, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityVlanGport, 206, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport, 206, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport, 212, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport, 206 + 7, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityCommonGport, 206, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 1, 221, 3);


    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstWlanGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityWlanGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup, 189, 17);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 2, 206, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 0, 209, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 1, 209, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 2, 209, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 2, 215, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 0, 215, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 1, 215, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 3, 215, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 3, 209, 6);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 3, 189, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 3, 189, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 3, 201, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 3, 202, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 3, 205, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 3, 205, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 3, 217, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 3, 218, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf1, 3, 221, 2);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 4, 189, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 4, 189, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 4, 201, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 4, 202, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 4, 205, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelFpf1, 5, 189, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf1, 5, 190, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf1, 5, 191, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf1, 5, 192, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf1, 5, 194, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf1, 5, 195, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf1, 5, 196, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf1, 5, 198, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf1, 5, 199, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf1, 5, 200, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf1, 5, 201, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf1, 5, 202, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf1, 5, 203, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf1, 5, 204, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 5, 205, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 5, 205, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 5, 217, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 5, 218, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf1, 5, 221, 2);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 6, 189, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 6, 189, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 6, 201, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 6, 202, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 6, 205, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 6, 207, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf1, 6, 209, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 6, 211, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 6, 214, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf1, 6, 219, 3);
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData2, 
                 _bcmFieldSliceSelFpf1, 9, 189, 32);  

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 7, 189, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 7, 189, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 7, 201, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 7, 202, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 0, 205, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 1, 205, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 3, 205, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 2, 205, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVrf, 211, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVpn, 211, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan, 211, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsOuterLabelPop,
                 _bcmFieldSliceSelFpf1, 7, 223, 1);
    _FP_QUAL_ADD(unit, stage_fc,
                 bcmFieldQualifyMplsStationHitTunnelUnterminated,
                 _bcmFieldSliceSelFpf1, 7, 224, 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf1, 8, 189, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf1, 8,
                 _bcmFieldSliceTosClassSelect, 0,
                 197, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                 _bcmFieldSliceSelFpf1, 8,
                 _bcmFieldSliceTosClassSelect, 0,
                 197, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                 _bcmFieldSliceSelFpf1, 8,
                 _bcmFieldSliceTosClassSelect, 1,
                 197, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 0, 205, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 1, 205, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 3, 205, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 2, 205, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVrf, 211, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVpn, 211, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan, 211, 12);

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
                            _bcmFieldSliceTosClassSelect, 0, 7, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceTosClassSelect, 0, 7, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceTosClassSelect, 1, 7, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 15, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyExtensionHeaderSubCode,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 23, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 31, 8);

    /* DWF2 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 55, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 56, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 57, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 58, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 60, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 61, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 62, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 64, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 65, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 66, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 67, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 68, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 69, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 70, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 71, 24);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyRangeCheckBits24_31,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 3, 95, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 0, 95, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 1, 95, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 2, 95, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 103, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 135, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceSelDisable, 0, 39, 128);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                            _bcmFieldSliceSelFpf2, 2,
                            _bcmFieldSliceSelDisable, 0, 39, 128);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                            _bcmFieldSliceSelFpf2, 3,
                            _bcmFieldSliceSelDisable, 0, 39, 128);
    /* DWF1 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceTtlClassSelect, 0, 167, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceTtlClassSelect, 0, 167, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceTtlClassSelect, 1, 167, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 175, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 177, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 193, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 193, 16);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityGlp, 0x1,
                     209, 17, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityCommonGport, 0x1,
                     209, 17, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityGlp, 0x1,
                     209, 17, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityMplsGport, 0x1,
                     209, 17, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityMimGport, 0x1,
                     209, 17, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstWlanGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityWlanGport, 0x1,
                     209, 17, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityVlanGport, 0x1,
                     209, 17, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityL3Egress, 0x1,
                     209, 17, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityL3Egress, 0x1,
                     209, 17, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityMulticastGroup, 0x1,
                     209, 17, 0, 0, 0, 0, 0);

    /* IPBM OVERLAY qualifiers for Triumph 2 device */
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit)) {
        ipbm_pairing_overlay_offset = 0;
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                        _bcmFieldDevSelDisable, 0,
                        _bcmFieldSliceSelDisable, 0,
                        _bcmFieldSliceSelDisable, 0,
                        1,
                        ipbm_pairing_overlay_offset + 20, 14,
                        0, 0,
                        0, 0,
                        1);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                        _bcmFieldDevSelDisable, 0,
                        _bcmFieldSliceSelDisable, 0,
                        _bcmFieldSliceSelDisable, 0,
                        1,
                        ipbm_pairing_overlay_offset + 20, 14,
                        0, 0,
                        0, 0,
                        1);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                        _bcmFieldDevSelDisable, 0,
                        _bcmFieldSliceSelDisable, 0,
                        _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                        1,
                        ipbm_pairing_overlay_offset + 34, 14,
                        0, 0,
                        0, 0,
                        1);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                        _bcmFieldDevSelDisable, 0,
                        _bcmFieldSliceSelDisable, 0,
                        _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                        1,
                        ipbm_pairing_overlay_offset + 34, 14,
                        0, 0,
                        0, 0,
                        1);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                        _bcmFieldDevSelDisable, 0,
                        _bcmFieldSliceSelDisable, 0,
                        _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                        1,
                        ipbm_pairing_overlay_offset + 34, 14,
                        0, 0,
                        0, 0,
                        1);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                        _bcmFieldDevSelDisable, 0,
                        _bcmFieldSliceSelDisable, 0,
                        _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                        1,
                        ipbm_pairing_overlay_offset + 40, 7,
                        0, 0,
                        0, 0,
                        1);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                        _bcmFieldDevSelDisable, 0,
                        _bcmFieldSliceSelDisable, 0,
                        _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                        1,
                        ipbm_pairing_overlay_offset + 40, 7,
                        0, 0,
                        0, 0,
                        1);
    }
    /* DONE DWF */
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_lookup_qualifiers_init
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
_field_tr2_lookup_qualifiers_init(int unit, _field_stage_t *stage_fc)
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

/*
 * Function:
 *     _field_tr2_external_qualifiers_init
 * Purpose:
 *     Initialize device stage external qaualifiers
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.
 *
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_tr2_external_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    uint32 rval;
    uint32 keygen_type;
    _FP_QUAL_DECL;

    BCM_IF_ERROR_RETURN(READ_ESM_KEYGEN_CTLr(unit, &rval));
    keygen_type = soc_reg_field_get(unit, ESM_KEYGEN_CTLr, rval, TCAM_TYPEf);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStage,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageExternal,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

    /* _FP_EXT_ACL_144_L2 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 0, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 48, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 48, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 60, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 61, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 64, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 70, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 72, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 120, 16);

    if (keygen_type != 3) {
        /* _FP_EXT_ACL_L2 */

        _FP_QUAL_TWO_SLICE_SEL_ADD(unit,
                                   stage_fc,
                                   bcmFieldQualifyVpn,
                                   _bcmFieldSliceSelExternal,
                                   _FP_EXT_ACL_L2,
                                   _bcmFieldSliceFwdFieldSelect,
                                   _bcmFieldFwdFieldVpn,
                                   0,
                                   12
                                   );
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit,
                                   stage_fc,
                                   bcmFieldQualifyForwardingVlanId,
                                   _bcmFieldSliceSelExternal,
                                   _FP_EXT_ACL_L2,
                                   _bcmFieldSliceFwdFieldSelect,
                                   _bcmFieldFwdFieldVlan,
                                   0,
                                   12
                                   );
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 12, 48);
        _FP_QUAL_ADD(unit,
                     stage_fc,
                     bcmFieldQualifyForwardingType,
                     _bcmFieldSliceSelExternal,
                     _FP_EXT_ACL_L2,
                     60,
                     1
                     );

        
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 62, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 64, 4);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 68, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 69, 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 71, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 72, 48);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 120, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 136, 2);
        
        /* Unused: 2 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 143, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 144, 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2PayloadFirstEightBytes,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 150, 64);
        /* Unused: 1 bit */
        /* L2 or IPV4: 1 bit: the tcam_write will handle this */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 216, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 217, 1);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceDstClassSelect, 0, 218, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceDstClassSelect, 2, 218, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceSrcClassSelect, 2, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceSrcClassSelect, 0, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceSrcClassSelect, 3, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceDstClassSelect, 3, 218, 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 230, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassPort,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceIntfClassSelect, 0, 238, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassL3,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceIntfClassSelect, 1, 238, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceIntfClassSelect, 2, 238, 8);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                         _bcmFieldDevSelDisable, 0,
                         _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                         _bcmFieldSliceSelDisable, 0, 0,
                         246, 8, 138, 3, 0, 0, 0);
        /* Unused: 2 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 256, 13);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 256, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 256, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 268, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 269, 3);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 272, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 272, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 284, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 285, 3);
    } else {
        /* _FP_EXT_ACL_L2 (for 7K 350MHz) */
        /* not for 7K
           _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
           _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 272,16);
        */
        /* Unused: 12 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 12, 48);
        /* Unused: 2 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 62, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 64, 4);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 68, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 69, 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 71, 1);
        _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 88, 32);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 120, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 136, 2);
        
        /* Unused: 2 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 143, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 144, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 156, 48);
        _FP_QUAL_ADD(unit,
                     stage_fc,
                     bcmFieldQualifyForwardingType,
                     _bcmFieldSliceSelExternal,
                     _FP_EXT_ACL_L2,
                     204,
                     1
                     );
        
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 206, 6);

        /* COS_CFI_LOWER 212, 2 */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 212, 1);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                         _bcmFieldDevSelDisable, 0,
                         _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                         _bcmFieldSliceSelDisable, 0, 0,
                         213, 1, 254, 2, 0, 0, 0);

        /* Unused: 1 bit */
        /* L2 or IPV4: 1 bit: the tcam_write will handle this */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 216, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 217, 1);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceDstClassSelect, 0, 218, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceDstClassSelect, 2, 218, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceSrcClassSelect, 2, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceSrcClassSelect, 0, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceSrcClassSelect, 3, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceDstClassSelect, 3, 218, 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 230, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassPort,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceIntfClassSelect, 0, 238, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassL3,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceIntfClassSelect, 1, 238, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceIntfClassSelect, 2, 238, 8);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                         _bcmFieldDevSelDisable, 0,
                         _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                         _bcmFieldSliceSelDisable, 0, 0,
                         246, 8, 138, 3, 0, 0, 0);
        
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 256, 13);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 256, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 256, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 268, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 269, 3);
        _FP_QUAL_EXT_ADD(unit, stage_fc, _bcmFieldQualifyData2,
                         _bcmFieldDevSelDisable, 0,
                         _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                         _bcmFieldSliceSelDisable, 0, 0,
                         272, 16, 72, 16, 0, 0, 0);
    }

    /* _FP_EXT_ACL_144_IPV4 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 0, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 32, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 48, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 48, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 60, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 61, 3);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4,
                     _bcmFieldSliceSelDisable, 0, 0,
                     64, 8, 138, 3, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 72, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 73, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 105, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 121, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 129, 8);
    /* ESM_IP_PROTOCOL_OVERLAY_EN 137, 1: Useless:
       See ESM_KEYGEN_CTLr.IP_PROT_OVERLAY_EN */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 141, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 143, 1);

    if (keygen_type != 3) {
        /* _FP_EXT_ACL_IPV4 */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 0, 0);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 0, 32);
        /* Unused: 18 + 3 + 8 bits */
        
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 62, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 63, 4);
        /* Unused: 1 bit */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 68, 1);
        /* Unused: 2 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 71, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 72, 32);
        
        
        /* Unused: 29 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 143, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 144, 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 150, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 166, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 182, 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 188, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 196, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 204, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 212, 2);
        /* ACL_Type: 1 bit: tcam_write will handle this */
        /* L2 or IPV4: 1 bit: the tcam_write will handle this */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 216, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 217, 1);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceDstClassSelect, 0, 218, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceDstClassSelect, 1, 218, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceDstClassSelect, 2, 218, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceSrcClassSelect, 2, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceSrcClassSelect, 0, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceSrcClassSelect, 1, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceSrcClassSelect, 3, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceDstClassSelect, 3, 218, 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 230, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassPort,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceIntfClassSelect, 0, 238, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassL3,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceIntfClassSelect, 1, 238, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceIntfClassSelect, 2, 238, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 246, 11);
        /* Unused: 15 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 272, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 272, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 284, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 285, 3);
    } else {
        /* _FP_EXT_ACL_IPV4 (for 7K 350 MHz) */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 0, 32);
        /* Unused: 18 + 3 + 8 bits */
        
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 62, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 63, 4);
        /* Unused: 1 bit */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 68, 1);
        /* Unused: 2 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 71, 1);
        /* Unused: 17 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 89, 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 95, 6);
        
        
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 111, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 127, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 143, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 144, 0);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 144, 32);
        /* Unused: 12 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 188, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 196, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 204, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 212, 2);
        /* ACL_Type: 1 bit: tcam_write will handle this */
        /* L2 or IPV4: 1 bit: the tcam_write will handle this */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 216, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 217, 1);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceDstClassSelect, 0, 218, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceDstClassSelect, 1, 218, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceDstClassSelect, 2, 218, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceSrcClassSelect, 2, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceSrcClassSelect, 0, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceSrcClassSelect, 1, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceSrcClassSelect, 3, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceDstClassSelect, 3, 218, 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 230, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassPort,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceIntfClassSelect, 0, 238, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassL3,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceIntfClassSelect, 1, 238, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceIntfClassSelect, 2, 238, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 246, 11);
        /* Unused: 15 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 272, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 272, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 284, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 285, 3);
    }

    /* _FP_EXT_ACL_L2_IPV4 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 0, 32);
    /* Unused: 18 + 3 + 8 bits */
    
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 62, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 63, 4);
    /* Unused: 1 bit */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 68, 1);
    /* Unused: 2 bits */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 71, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 72, 32);
    
    
    /* Unused: 29 bits */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 143, 1);
    /* Unused: 12 bits */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 156, 48);
    /* Unused: 11 + 1 bits */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 216, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 222, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 238, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 254, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 260, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 268, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 276, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 284, 2);
    /* ACL_Type: 1 bit: tcam_write will handle this */
    /* L2 or IPV4: 1 bit: the tcam_write will handle this */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit,
                               stage_fc,
                               bcmFieldQualifyVpn,
                               _bcmFieldSliceSelExternal,
                               _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVpn,
                               288,
                               12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit,
                               stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelExternal,
                               _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan,
                               288,
                               12
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 300, 48);
    _FP_QUAL_ADD(unit,
                 stage_fc,
                 bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelExternal,
                 _FP_EXT_ACL_L2_IPV4,
                 348,
                 1
                 );
    /* Unused: 11 bits */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 360, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 361, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceDstClassSelect, 0, 362, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceDstClassSelect, 1, 362, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceDstClassSelect, 2, 362, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceSrcClassSelect, 2, 368, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceSrcClassSelect, 0, 368, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceSrcClassSelect, 1, 368, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceSrcClassSelect, 3, 368, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceDstClassSelect, 3, 362, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceDstClassSelect, 3, 374, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceIntfClassSelect, 0, 382, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceIntfClassSelect, 1, 382, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceIntfClassSelect, 2, 382, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 390, 11);
    /* Unused: 15 bits */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 416, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 416, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 428, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 429, 3);

    /* _FP_EXT_ACL_144_IPV6 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 0, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 72, 64);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6,
                     _bcmFieldSliceSelDisable, 0, 0,
                     136, 8, 64, 8, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6,
                     _bcmFieldSliceSelDisable, 0, 0,
                     136, 8, 64, 4, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 68, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 69, 3);

    /* _FP_EXT_ACL_IPV6_SHORT */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT,
                     _bcmFieldSliceSelDisable, 0, 0,
                     80, 64, 0, 64, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 64, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 72, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 73, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 77, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 78, 2);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT,
                     _bcmFieldSliceSelDisable, 0, 0,
                     216, 72, 144, 56, 0, 0, 0);
    
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 200, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 200, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 200, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 212, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 213, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 288, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 289, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT,
                               _bcmFieldSliceSrcClassSelect, 0, 290, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT,
                               _bcmFieldSliceSrcClassSelect, 1, 290, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT,
                               _bcmFieldSliceSrcClassSelect, 3, 290, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT,
                               _bcmFieldSliceDstClassSelect, 3, 290, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 296, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 312, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 328, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6HopLimit,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 334, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6NextHeader,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 342, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6TrafficClass,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 350, 8);


    /* _FP_EXT_ACL_IPV6_FULL */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                     _bcmFieldSliceSelDisable, 0, 0,
                     80, 64, 0, 64, 0, 0, 0);
    /* Unused: 8 bits */
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                     _bcmFieldSliceSelDisable, 0, 0,
                     213, 3, 72, 5, 0, 0, 0);
    /* Unused: 3 bits */
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyIp6NextHeader,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                     _bcmFieldSliceSelDisable, 0, 0,
                     284, 4, 144, 4, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6TrafficClass,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 148, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 156, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 156, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 168, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 169, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 182, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 188, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 190, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 191, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 195, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 196, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 204, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 205, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 216, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 217, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceDstClassSelect, 0, 218, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceDstClassSelect, 1, 218, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceDstClassSelect, 2, 218, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceSrcClassSelect, 2, 224, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceSrcClassSelect, 0, 224, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceSrcClassSelect, 1, 224, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceSrcClassSelect, 3, 224, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceDstClassSelect, 3, 218, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceIntfClassSelect, 0, 230, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceIntfClassSelect, 1, 230, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceIntfClassSelect, 2, 230, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 238, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 254, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 270, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6HopLimit,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 276, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6HopLimit,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 276, 8);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                     _bcmFieldSliceSelDisable, 0, 0,
                     368, 64, 288, 64, 0, 0, 0);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                     _bcmFieldSliceSelDisable, 0, 0,
                     352, 8, 365, 3, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 363, 8);

    /* _FP_EXT_ACL_L2_IPV6 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 0, 0);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6,
                     _bcmFieldSliceSelDisable, 0, 0,
                     423, 9, 288, 72, 216, 47, 0);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6,
                     _bcmFieldSliceSelDisable, 0, 0,
                     263, 25, 144, 72, 72, 31, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6,
                     _bcmFieldSliceSelDisable, 0, 0,
                     115, 29, 0, 19, 0, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 19, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 68, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 70, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 71, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 103, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 103, 11);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 360, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 361, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 372, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 388, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6NextHeader,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 404, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6TrafficClass,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 415, 8);

    
    /* ESM_IP_PROTOCOL_OVERLAY_EN 412, 1: Useless:
       See ESM_KEYGEN_CTLr.IP_PROT_OVERLAY_EN */
    /* bcmFieldQualifyTcpControl 404, 8: OVERLAY: not used:
       see ESM_KEYGEN_CTLr.IP_PROT_OVERLAY_EN */
    
    
    /* bcmFieldQualifyInPort 362, 6: OVERLAY: not used: see ESM_KEYGEN_CTL */
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_tr2_egress_qualifiers_init
 * Purpose:
 *     Initialize device stage egress qaualifiers
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.
 *
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_tr2_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 19, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 23, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 25, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 27, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 43, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 91, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 139, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 145, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 146, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 147, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 148, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 148, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 149, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 152, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 165, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 172, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 173, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 173, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 185, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 186, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 189, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 191, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 192, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 200, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 206, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 210, 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 8, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 12, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 14, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 16, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 22, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 38, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 54, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 62, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 70, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 102, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 134, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 142, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 143, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 152, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 165, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 172, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 173, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 173, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 185, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 186, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 189, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 191, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 192, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 200, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 206, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 210, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 0);

    /* KEY1 without v4 specific fields. */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 8, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 12, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 16, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 22, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 38, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 54, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 62, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 134, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 142, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 143, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 152, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 165, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 172, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 173, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 173, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 185, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 186, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 189, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 191, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 192, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 200, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 206, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 210, 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 6, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP6,
                               14, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_DIP6,
                               14, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64,
                               14, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64,
                               78, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 142, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 150, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 151, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 152, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 165, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 172, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 173, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 173, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 185, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 186, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 189, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 191, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 192, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 200, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 206, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 210, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 8, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 12, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 14, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 16, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 22, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 38, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 54, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 62, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 190, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 198, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 206, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 210, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 0);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_qualifiers_init
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
_field_tr2_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
          rv = _field_tr2_ingress_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _field_tr2_lookup_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _field_tr2_egress_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_EXTERNAL:
          rv = _field_tr2_external_qualifiers_init(unit, stage_fc);
          break;
      default:
          sal_free(stage_fc->f_qual_arr);
          return (BCM_E_PARAM);
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_field_tr2_init
 * Purpose:
 *     Perform initializations that are specific to BCM56624. This
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
_bcm_field_tr2_init(int unit, _field_control_t *fc)
{
    _field_stage_t *stage_fc; /* Stages iteration pointer */

    /* Input parameters check. */
    if (NULL == fc) {
        return (BCM_E_PARAM);
    }

    stage_fc = fc->stages;
    while (stage_fc) {
        /* Clear hardware table */
        BCM_IF_ERROR_RETURN(_bcm_field_tr_hw_clear(unit, stage_fc));

        /* Initialize qualifiers info. */
        BCM_IF_ERROR_RETURN(_field_tr2_qualifiers_init(unit, stage_fc));

        if (_BCM_FIELD_STAGE_EXTERNAL == stage_fc->stage_id) {
            _bcm_field_tr_external_init(unit, stage_fc);
            stage_fc = stage_fc->next;
            continue;
        }

        /* Goto next stage */
        stage_fc = stage_fc->next;
    }

    /* Initialize the TOS_FN, TTL_FN, TCP_FN tables */
    BCM_IF_ERROR_RETURN(_bcm_field_trx_tcp_ttl_tos_init(unit));

    if (0 == SOC_WARM_BOOT(unit)) {
        /* Enable filter processor */
        BCM_IF_ERROR_RETURN(_field_port_filter_enable_set(unit, fc, TRUE));

        /* Enable meter refresh */
        BCM_IF_ERROR_RETURN(_field_meter_refresh_enable_set(unit, fc, TRUE));
    }

    /* Initialize the function pointers */
    _field_tr2_functions_init(&fc->functions);

    return (BCM_E_NONE);
}



/*
 * Function:
 *     _field_tr2_functions_init
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
_field_tr2_functions_init(_field_funct_t *functions)
{
    functions->fp_detach               = _bcm_field_tr_detach;
    functions->fp_group_install        = _bcm_field_fb_group_install;
    functions->fp_selcodes_install     = _bcm_field_trx_selcodes_install;
    functions->fp_slice_clear          = _bcm_field_trx_slice_clear;
    functions->fp_entry_remove         = _bcm_field_fb_entry_remove;
    functions->fp_entry_move           = _bcm_field_fb_entry_move;
    functions->fp_selcode_get          = _bcm_field_tr_selcode_get;
    functions->fp_selcode_to_qset      = _bcm_field_selcode_to_qset;
    functions->fp_qual_list_get        = _bcm_field_qual_lists_get;
    functions->fp_tcam_policy_clear    = NULL;
    functions->fp_tcam_policy_install  = _bcm_field_tr_entry_install;
    functions->fp_tcam_policy_reinstall  = _bcm_field_tr_entry_reinstall;
    functions->fp_policer_install      = _bcm_field_trx_policer_install;
    functions->fp_write_slice_map      = _bcm_field_tr_write_slice_map;
    functions->fp_qualify_ip_type      = _bcm_field_trx_qualify_ip_type;
    functions->fp_qualify_ip_type_get  = _bcm_field_trx_qualify_ip_type_get;
    functions->fp_action_support_check = _bcm_field_trx_action_support_check;
    functions->fp_action_conflict_check = _bcm_field_trx_action_conflict_check;
    functions->fp_counter_get          = _bcm_field_tr_counter_get;
    functions->fp_counter_set          = _bcm_field_tr_counter_set;
    functions->fp_stat_index_get       = _bcm_field_trx_stat_index_get;
    functions->fp_action_params_check  = _bcm_field_trx_action_params_check;
    functions->fp_egress_key_match_type_set =
        _bcm_field_trx_egress_key_match_type_set;
    functions->fp_external_entry_install  = _bcm_field_tr_external_entry_install;
    functions->fp_external_entry_reinstall  = _bcm_field_tr_external_entry_reinstall;
    functions->fp_external_entry_remove   = _bcm_field_tr_external_entry_remove;
    functions->fp_external_entry_prio_set = _bcm_field_tr_external_entry_prio_set;
    functions->fp_data_qualifier_ethertype_add =
                       _bcm_field_trx2_data_qualifier_ethertype_add;
    functions->fp_data_qualifier_ethertype_delete=
                       _bcm_field_trx2_data_qualifier_ethertype_delete;
    functions->fp_data_qualifier_ip_protocol_add =
                       _bcm_field_trx2_data_qualifier_ip_protocol_add;
    functions->fp_data_qualifier_ip_protocol_delete=
                       _bcm_field_trx2_data_qualifier_ip_protocol_delete;
    functions->fp_data_qualifier_packet_format_add=
                       _bcm_field_trx2_data_qualifier_packet_format_add;
    functions->fp_data_qualifier_packet_format_delete=
                       _bcm_field_trx2_data_qualifier_packet_format_delete;
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

/*
 * Function:
 *   _bcm_field_tr2_stat_hw_mode_to_bmap
 *
 * Description:
 *      HW counter mode to statistics bitmap.
 * Parameters:
 *      unit           - (IN) BCM device number.
 *      mode           - (IN) HW counter mode.
 *      stage_id       - (IN) Stage id.
 *      hw_bmap        - (OUT) Statistics bitmap.
 *      hw_entry_count - (OUT) Number of counters required.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_tr2_stat_hw_mode_to_bmap(int unit, uint16 mode,
                                    _field_stage_id_t stage_id,
                                    uint32 *hw_bmap, uint8 *hw_entry_count)
{
    uint32 value = 0;
    /* Input parameters check. */
    if ((NULL == hw_bmap) || (NULL == hw_entry_count)) {
        return (BCM_E_PARAM);
    }
    switch (mode) {
        case 1:
            if (_BCM_FIELD_STAGE_EXTERNAL == stage_id) {
                value |= (1 << (int)bcmFieldStatGreenBytes);
                value |= (1 << (int)bcmFieldStatGreenPackets);
            } else {
                value |= (1 << (int)bcmFieldStatBytes);
                value |= (1 << (int)bcmFieldStatPackets);
            }
            *hw_entry_count = 1;
            break;
        case 2:
            if (_BCM_FIELD_STAGE_EXTERNAL == stage_id) {
                value |= (1 << (int)bcmFieldStatYellowBytes);
                value |= (1 << (int)bcmFieldStatYellowPackets);
            } else {
                value |= (1 << (int)bcmFieldStatPackets);
                value |= (1 << (int)bcmFieldStatBytes);
            }
            *hw_entry_count = 1;
            break;
        case 3:
            if (_BCM_FIELD_STAGE_EXTERNAL == stage_id) {
                value |= (1 << (int)bcmFieldStatNotRedBytes);
                value |= (1 << (int)bcmFieldStatNotRedPackets);
                *hw_entry_count = 1;
            } else {
                value |= (1 << (int)bcmFieldStatRedBytes);
                value |= (1 << (int)bcmFieldStatRedPackets);
                value |= (1 << (int)bcmFieldStatNotRedBytes);
                value |= (1 << (int)bcmFieldStatNotRedPackets);
                *hw_entry_count = 2;
            }
            break;
        case 4:
            if (_BCM_FIELD_STAGE_EXTERNAL == stage_id) {
                value |= (1 << (int)bcmFieldStatRedBytes);
                value |= (1 << (int)bcmFieldStatRedPackets);
                *hw_entry_count = 1;
            } else {
                value |= (1 << (int)bcmFieldStatGreenBytes);
                value |= (1 << (int)bcmFieldStatGreenPackets);
                value |= (1 << (int)bcmFieldStatNotGreenBytes);
                value |= (1 << (int)bcmFieldStatNotGreenPackets);
                *hw_entry_count = 2;
            }
            break;
        case 5:
            if (_BCM_FIELD_STAGE_EXTERNAL == stage_id) {
                value |= (1 << (int)bcmFieldStatNotYellowBytes);
                value |= (1 << (int)bcmFieldStatNotYellowPackets);
                *hw_entry_count = 1;
            } else {
                value |= (1 << (int)bcmFieldStatGreenBytes);
                value |= (1 << (int)bcmFieldStatGreenPackets);
                value |= (1 << (int)bcmFieldStatRedBytes);
                value |= (1 << (int)bcmFieldStatRedPackets);
                value |= (1 << (int)bcmFieldStatNotYellowBytes);
                value |= (1 << (int)bcmFieldStatNotYellowPackets);
                *hw_entry_count = 2;
            }
            break;
        case 6:
            if (_BCM_FIELD_STAGE_EXTERNAL == stage_id) {
                value |= (1 << (int)bcmFieldStatNotGreenBytes);
                value |= (1 << (int)bcmFieldStatNotGreenPackets);
                *hw_entry_count = 1;
            } else {                                                
                value |= (1 << (int)bcmFieldStatGreenBytes);
                value |= (1 << (int)bcmFieldStatGreenPackets);
                value |= (1 << (int)bcmFieldStatYellowBytes);
                value |= (1 << (int)bcmFieldStatYellowPackets);
                value |= (1 << (int)bcmFieldStatNotRedBytes);
                value |= (1 << (int)bcmFieldStatNotRedPackets);
                *hw_entry_count = 2;
            }
            break;
        case 7:
            if (_BCM_FIELD_STAGE_EXTERNAL == stage_id) {
                value |= (1 << (int)bcmFieldStatBytes);
                value |= (1 << (int)bcmFieldStatPackets);
                *hw_entry_count = 1;
            } else {
                value |= (1 << (int)bcmFieldStatRedBytes);
                value |= (1 << (int)bcmFieldStatRedPackets);
                value |= (1 << (int)bcmFieldStatYellowBytes);
                value |= (1 << (int)bcmFieldStatYellowPackets);
                value |= (1 << (int)bcmFieldStatNotGreenBytes);
                value |= (1 << (int)bcmFieldStatNotGreenPackets);
                *hw_entry_count = 2;
            }
            break;
        default:
            *hw_entry_count = 0;
    }
    *hw_bmap = value;                                               
    return (BCM_E_NONE);
}

#else /* BCM_TRIUMPH2_SUPPORT && BCM_FIELD_SUPPORT */
typedef int _triumph2_field_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_TRIUMPH2_SUPPORT && BCM_FIELD_SUPPORT */
