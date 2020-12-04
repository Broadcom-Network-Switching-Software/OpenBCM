/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/defs.h>
#if defined(BCM_HURRICANE2_SUPPORT) && defined(BCM_FIELD_SUPPORT)

#include <soc/mem.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/l3.h>
#include <bcm/field.h>
#include <bcm/tunnel.h>

#include <bcm_int/esw_dispatch.h>

#include <bcm_int/esw/field.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/enduro.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/hurricane2.h>


static soc_field_t _vfp_hu2_slice_pairing_field[2] = {
    SLICE1_0_PAIRINGf,   SLICE3_2_PAIRINGf};

const soc_field_t _bcm_field_hu2_vfp_field_sel[][2] = {
    { SLICE_0_F2f, SLICE_0_F3f },
    { SLICE_1_F2f, SLICE_1_F3f },
    { SLICE_2_F2f, SLICE_2_F3f },
    { SLICE_3_F2f, SLICE_3_F3f }
};


const soc_field_t _bcm_field_hu2_slice_pairing_field[] = {
    SLICE1_0_PAIRINGf,
    SLICE3_2_PAIRINGf,
};


#define VFP_SLICE_COUNT    4
/* local/static function prototypes */
STATIC void _field_hu2_functions_init(_field_funct_t *functions) ;

/*
 * Function:
 *     _field_hu2_ingress_qualifiers_init
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
_field_hu2_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport, 48, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport, 48, 13);
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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport, 24, 17);
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
                               _bcmFieldFwdEntityModPortGport, 47, 8);
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
    /* bcmFieldQualifyInterfaceClassL3 is not supported */
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


    /* FPF1 */ 
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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport, 213, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport, 213, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityL3Egress, 213, 13);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport, 189, 17);
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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityVlanGport, 206, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityMimGport, 206, 15);
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
                               _bcmFieldFwdEntityModPortGport, 212, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 1, 221, 3);


    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport, 189, 17);
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
                               _bcmFieldSliceDstClassSelect, 0, 205, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceDstClassSelect, 1, 205, 6);
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
                               _bcmFieldSliceDstClassSelect, 0, 205, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceDstClassSelect, 1, 205, 6);
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
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData2,
                               _bcmFieldSliceSelFpf1, 9, 189, 32);

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
    /* bcmFieldQualifyInterfaceClassL3 is not supported */
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

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0, 
                     _bcmFieldSliceDstFwdEntitySelect, 
                     _bcmFieldFwdEntityGlp, 0x1,
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
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                     _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityVlanGport, 0x1,
                     209, 17, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                     _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityMimGport, 0x1,
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
    /* DONE DWF */
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_hu2_lookup_qualifiers_init
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
_field_hu2_lookup_qualifiers_init(int unit, _field_stage_t *stage_fc)
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

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                               _bcmFieldSliceSelFpf3, 1, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf3, 1, 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf3, 2, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf3, 2, 8, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, 2, 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, 2, 24, 8);
    /* bcmFieldQualifyInterfaceClassL3 is not supported */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 3, 8, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 3, 8, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 3, 20, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 3, 21, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf3, 3, 24, 7);

    /* FPF2 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                               _bcmFieldSliceSelFpf2, 0, 32, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0, 40, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                               _bcmFieldSliceSelFpf2, 0, 46, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 0, 48, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                               _bcmFieldSliceSelFpf2, 0, 56, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf2, 0, 72, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 0, 88, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 0, 96, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 0, 128, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                               _bcmFieldSliceSelFpf2, 1, 32, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                               _bcmFieldSliceSelFpf2, 2, 32, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 3, 48, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 3, 64, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 3, 112, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 4,  32, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 4, 40, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 4, 48, 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 4, 80, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 4, 112, 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 5, 32, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 5, 40, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 5, 48, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 5, 80, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5, 112, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                               _bcmFieldSliceSelFpf2, 6, 32, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                               _bcmFieldSliceSelFpf2, 6, 96, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySnap,
                 _bcmFieldSliceSelFpf2, 7, 96, 40);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLlc,
                 _bcmFieldSliceSelFpf2, 7, 136, 24);

    /* FPF1 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocolCommon,
                               _bcmFieldSliceSelFpf1, 0, 160, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelFpf1, 0, 163, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf1, 0, 164, 1);
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
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                               _bcmFieldSliceSelFpf1, 0, 205, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf1, 0, 209, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf1, 0, 4, 1);

    /* DWF3 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceSelDisable, 0, 0, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceSelDisable, 0, 8, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceSelDisable, 0, 16, 6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceSelDisable, 0, 22, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceSelDisable, 0, 24, 8);
    /* DWF2 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 32, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 40, 6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 46, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 48, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 56, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 72, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 88, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 96, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 128, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceSelDisable, 0, 32, 128);
    /* FPF1 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0, 160, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0, 168, 8);
    /* bcmFieldQualifyInterfaceClassL3 is not supported */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0, 184, 16);

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_hu2_egress_qualifiers_init
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
_field_hu2_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
    /* L3_PKT_TYPE */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrMdlKey4, 0, 206, 4);
    /* MDL */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOamMdl,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrMdlKey4, 1, 206, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 210, 1);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 1);
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
 *     _field_hu2_qualifiers_init
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
_field_hu2_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
          rv = _field_hu2_ingress_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _field_hu2_lookup_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _field_hu2_egress_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_EXTERNAL:  
      default: 
          sal_free(stage_fc->f_qual_arr);
          return (BCM_E_PARAM);
    }
    return (rv);
}



/*
 * Function:
 *     _field_hu2_write_slice_map_ingress
 *
 * Purpose:
 *     Write the FP_SLICE_MAP (INGRESS)
 *
 * Parameters:
 *     unit
 *     stage_fc - pointer to stage control block
 *     new_fp_virtual_map - virtual map to be written
 *
 * Returns:
 *     nothing
 *     
 * Notes:
 */
STATIC int
_field_hu2_write_slice_map_ingress(int unit, _field_stage_t *stage_fc)
{
    fp_slice_map_entry_t map_entry;  /* HW entry buffer.         */
    soc_field_t field;               /* HW entry fields.         */
    int vmap_size;                   /* Virtual map index count. */
    uint32 value;                    /* Field entry value.       */
    int idx;                         /* Map fields iterator.     */     
    int rv;                          /* Operation return status. */
    uint32 virtual_to_physical_map_hu2[8] = {
            VIRTUAL_SLICE_0_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_1_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_2_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_3_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_4_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_5_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_6_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_7_PHYSICAL_SLICE_NUMBER_ENTRY_0f  
    };

    uint32 virtual_to_group_map_hu2[8] = {
            VIRTUAL_SLICE_0_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_1_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_2_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_3_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_4_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_5_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_6_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_7_VIRTUAL_SLICE_GROUP_ENTRY_0f
    };

    /* Calculate virtual map size. */
    rv = _bcm_field_virtual_map_size_get(unit, stage_fc, &vmap_size); 
    BCM_IF_ERROR_RETURN(rv);

    rv = READ_FP_SLICE_MAPm(unit, MEM_BLOCK_ANY, 0, &map_entry);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < vmap_size; idx++) {
        value = (stage_fc->vmap[_FP_DEF_INST][0][idx]).vmap_key;
        field = virtual_to_physical_map_hu2[idx];
        soc_FP_SLICE_MAPm_field32_set(unit, &map_entry, field, value);

        value = (stage_fc->vmap[_FP_DEF_INST][0][idx]).virtual_group;
        field = virtual_to_group_map_hu2[idx];
        soc_FP_SLICE_MAPm_field32_set(unit, &map_entry, field, value);
    }

    rv = WRITE_FP_SLICE_MAPm(unit, MEM_BLOCK_ALL, 0, &map_entry);
    return (rv);
}


/*
 * Function:
 *     _bcm_field_hu2_write_slice_map
 *
 * Purpose:
 *     Write the FP_SLICE_MAP
 *
 * Parameters:
 *     unit
 *     stage_fc - pointer to stage control block
 *     fg - Reference to Field group structure.
 *
 * Returns:
 *     nothing
 *     
 * Notes:
 */
int
_bcm_field_hu2_write_slice_map(int unit, _field_stage_t *stage_fc,
                               _field_group_t *fg)
{
    int rv = BCM_E_PARAM;   /* Operation return status. */

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        rv = _field_hu2_write_slice_map_ingress(unit, stage_fc);
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EGRESS) {
        rv =  _bcm_field_trx_write_slice_map_egress(unit, stage_fc);
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_LOOKUP) {
        rv =  _bcm_field_trx_write_slice_map_vfp(unit, stage_fc);
    }

    return (rv);
}

/*
 * Function:
 *     _bcm_field_hu2_init
 * Purpose:
 *     Perform initializations that are specific to BCM56142. This
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
_bcm_field_hu2_init(int unit, _field_control_t *fc) 
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
        BCM_IF_ERROR_RETURN(_field_hu2_qualifiers_init(unit, stage_fc));

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
    _field_hu2_functions_init(&fc->functions);;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_hu2_lookup_mode_set
 *
 * Purpose:
 *     Helper function to _bcm_field_fb_mode_install that sets the mode of a
 *     slice in a register value that is to be used for VFP_KEY_CONTROLr.
 *
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     slice_numb - (IN) Slice number to set mode for.
 *     fg         - (IN) Filed group structure.
 *     flags      - (IN) New group/slice mode.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_hu2_lookup_mode_set(int unit, uint8 slice_numb,
                              _field_group_t *fg, uint8 flags)

{
    int paired;
    uint32  reg_val;
    if (slice_numb >= VFP_SLICE_COUNT) {
        return (BCM_E_PARAM);
    }
    SOC_IF_ERROR_RETURN(READ_VFP_KEY_CONTROLr(unit, &reg_val));

    paired = (flags & _FP_GROUP_SPAN_DOUBLE_SLICE) ? 1 : 0;
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      _vfp_hu2_slice_pairing_field[slice_numb / 2], paired);

    SOC_IF_ERROR_RETURN(WRITE_VFP_KEY_CONTROLr(unit, reg_val));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_hu2_mode_set
 *
 * Purpose:
 *    Auxiliary routine used to set group pairing mode.
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     slice_numb - (IN) Slice number to set mode for.
 *     fg         - (IN) Installed group structure.
 *     flags      - (IN) New group/slice mode.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_hu2_mode_set(int unit, uint8 slice_numb, _field_group_t *fg, uint8 flags)
{
    int rv;     /* Operation return status. */

    /* Input parameter check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    switch (fg->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          rv = BCM_E_NONE; /* Mode and select codes programmed together. */
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv  = _bcm_field_hu2_lookup_mode_set(unit, slice_numb, fg, flags);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _bcm_field_trx_egress_mode_set(unit, slice_numb, fg, flags);
          break;
      default:
          rv = BCM_E_PARAM;
    }
    return (rv);
}

/*
 * Function:
 *     _field_hu2_lookup_selcodes_install
 *
 * Purpose:
 *     Writes the field select codes (ie. FPFx).
 *     for VFP (_BCM_FIELD_STAGE_LOOKUP) lookup stage.
 *
 * Parameters:
 *     unit  - BCM device number
 *     fs    - slice that needs its select codes written
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_hu2_lookup_selcodes_install(int unit, _field_group_t *fg,
                                   uint8 slice_numb, int selcode_index)
{
    uint32        reg_val;
    uint32        old_reg_val;
    _field_sel_t  *sel;
    int           rv;
    sel = &fg->sel_codes[selcode_index];

    rv = READ_VFP_KEY_CONTROLr(unit, &reg_val);
    BCM_IF_ERROR_RETURN(rv);
    old_reg_val = reg_val;

    if (((selcode_index % 2)  == 0) ||
        ((fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) &&
            (selcode_index & 1))){
        if (sel->fpf2 != _FP_SELCODE_DONT_CARE) {
            soc_reg_field_set(unit,
                              VFP_KEY_CONTROLr,
                              &reg_val,
                              _bcm_field_hu2_vfp_field_sel[slice_numb][0],
                              sel->fpf2
                             );
        }
        if (sel->fpf3 != _FP_SELCODE_DONT_CARE) {
            soc_reg_field_set(unit,
                              VFP_KEY_CONTROLr,
                              &reg_val,
                              _bcm_field_hu2_vfp_field_sel[slice_numb][1],
                              sel->fpf3
                             );
        }
    }
    
    if (old_reg_val != reg_val) {
        rv = WRITE_VFP_KEY_CONTROLr(unit, reg_val);
        BCM_IF_ERROR_RETURN(rv);
    }

    return (rv);
}

STATIC int
_field_hu2_egress_selcodes_install(int            unit,
                                   _field_group_t *fg,
                                   uint8          slice_num,
                                   int            selcode_idx)
{
    static const soc_field_t fldtbl[][4] = {
        { SLICE_0_F1f, SLICE_1_F1f, SLICE_2_F1f, SLICE_3_F1f },
        { SLICE_0_F2f, SLICE_1_F2f, SLICE_2_F2f, SLICE_3_F2f },
        { SLICE_0_F3f, SLICE_1_F3f, SLICE_2_F3f, SLICE_3_F3f },
        { SLICE_0_F4f, SLICE_1_F4f, SLICE_2_F4f, SLICE_3_F4f }
    };

    static const soc_field_t mdlfldtbl[4] = {
        SLICE_0f, SLICE_1f, SLICE_2f, SLICE_3f };

    _field_sel_t * const sel = &fg->sel_codes[selcode_idx];

    if (sel->egr_class_f1_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(
            soc_reg_field32_modify(unit, EFP_CLASSID_SELECTORr, REG_PORT_ANY,
                               fldtbl[0][slice_num], sel->egr_class_f1_sel));
    }
    if (sel->egr_class_f2_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(
           soc_reg_field32_modify(unit, EFP_CLASSID_SELECTORr, REG_PORT_ANY,
                               fldtbl[1][slice_num], sel->egr_class_f2_sel));
    }
    if (sel->egr_class_f3_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(
            soc_reg_field32_modify(unit, EFP_CLASSID_SELECTORr, REG_PORT_ANY,
                                fldtbl[2][slice_num], sel->egr_class_f3_sel));
    }
    if (sel->egr_class_f4_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(
            soc_reg_field32_modify(unit, EFP_CLASSID_SELECTORr, REG_PORT_ANY,
                                fldtbl[3][slice_num], sel->egr_class_f4_sel));
    }

    /* SELECTOR CODES for EFP_KEY4_MDL_SELECTOR register */
    if (sel->egr_key4_mdl_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(
            soc_reg_field32_modify(unit, EFP_KEY4_MDL_SELECTORr, REG_PORT_ANY,
                                mdlfldtbl[slice_num], sel->egr_key4_mdl_sel));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_hu2_selcodes_install
 *
 * Purpose:
 *     Writes the field select codes (ie. FPFx).
 *
 * Parameters:
 *     unit  - BCM device number
 *     fs    - slice that needs its select codes written
 *
 * Returns:
 *     BCM_E_INTERNAL - On read/write errors
 *     BCM_E_NONE     - Success
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
int
_bcm_field_hu2_selcodes_install(int unit, _field_group_t *fg,
                                uint8 slice_numb, bcm_pbmp_t pbmp,
                                int selcode_index)
{
    int rv;    /* Operation return status. */

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Set slice mode. Single/Double/Triple, Intraslice */
    rv = _bcm_field_hu2_mode_set(unit, slice_numb, fg, fg->flags);
    BCM_IF_ERROR_RETURN(rv);

    switch (fg->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          rv = _bcm_field_trx_ingress_selcodes_install(unit, fg, slice_numb,
                                                       &pbmp, selcode_index);
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _field_hu2_lookup_selcodes_install(unit, fg, slice_numb,
                                                  selcode_index);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _field_hu2_egress_selcodes_install(unit, fg, slice_numb,
                                                  selcode_index);
          break;
      default:
          return BCM_E_INTERNAL;
    }
    return (rv);
}

/*
 * Function:
 *     _field_hu2_lookup_slice_clear
 *
 * Purpose:
 *     Reset slice configuraton on group deletion event.
 *
 * Parameters:
 *     unit  - BCM device number
 *     fs    - slice that needs its select codes written
 *
 * Returns:
 *     BCM_E_NONE     - Success
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_hu2_lookup_slice_clear(int unit, uint8 slice_numb)
{
    uint32 reg_val;
    SOC_IF_ERROR_RETURN(READ_VFP_KEY_CONTROLr(unit, &reg_val));
    soc_reg_field_set(unit,
                      VFP_KEY_CONTROLr,
                      &reg_val,
                      _bcm_field_hu2_vfp_field_sel[slice_numb][0],
                      0
                      );
    soc_reg_field_set(unit,
                      VFP_KEY_CONTROLr,
                      &reg_val,
                      _bcm_field_hu2_vfp_field_sel[slice_numb][1],
                      0
                      );
    soc_reg_field_set(unit,
                      VFP_KEY_CONTROLr,
                      &reg_val,
                      _bcm_field_hu2_slice_pairing_field[slice_numb / 2],
                      0
                      );
    SOC_IF_ERROR_RETURN(WRITE_VFP_KEY_CONTROLr(unit, reg_val));


    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_hu2_slice_clear
 *
 * Purpose:
 *     Clear slice configuration on group removal
 *
 * Parameters:
 *     unit  - BCM device number
 *     fg    - Field group slice belongs to
 *     fs    - Field slice structure.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_hu2_slice_clear(int unit, _field_group_t *fg, _field_slice_t *fs)
{
    int rv;

    switch (fs->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          rv = _bcm_field_trx_ingress_slice_clear(unit, fs->slice_number);
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _field_hu2_lookup_slice_clear(unit, fs->slice_number);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _bcm_field_trx_egress_slice_clear(unit, fs->slice_number);
          break;
      default:
          rv = BCM_E_INTERNAL;
    }
    return (rv);
}



/*
 * Function:
 *     _field_hu2_functions_init
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
_field_hu2_functions_init(_field_funct_t *functions)
{
    functions->fp_detach               = _bcm_field_tr_detach;
    functions->fp_group_install        = _bcm_field_fb_group_install;
    functions->fp_selcodes_install     = _bcm_field_hu2_selcodes_install;
    functions->fp_slice_clear          = _bcm_field_hu2_slice_clear;
    functions->fp_entry_remove         = _bcm_field_fb_entry_remove;
    functions->fp_entry_move           = _bcm_field_fb_entry_move;
    functions->fp_selcode_get          = _bcm_field_tr_selcode_get;
    functions->fp_selcode_to_qset      = _bcm_field_selcode_to_qset;
    functions->fp_qual_list_get        = _bcm_field_qual_lists_get;
    functions->fp_tcam_policy_clear    = NULL;
    functions->fp_tcam_policy_install  = _bcm_field_tr_entry_install;
    functions->fp_tcam_policy_reinstall  = _bcm_field_tr_entry_reinstall;
    functions->fp_policer_install      = _bcm_field_trx_policer_install;
    functions->fp_write_slice_map      = _bcm_field_hu2_write_slice_map;
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
#else /* BCM_HURRICANE2_SUPPORT && BCM_FIELD_SUPPORT */
typedef int _hurricane2_field_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_HURRICANE2_SUPPORT && BCM_FIELD_SUPPORT */
