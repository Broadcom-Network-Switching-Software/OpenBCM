/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/defs.h>
#if defined(BCM_GREYHOUND2_SUPPORT) && defined(BCM_FIELD_SUPPORT)

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
#include <bcm_int/esw/greyhound.h>



/* local/static function prototypes */
STATIC void _field_gh2_functions_init(_field_funct_t *functions) ;

/*
 * Function:
 *     _field_gh2_ingress_qualifiers_init
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
_field_gh2_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    /* Singal wide DATA_KEY format for GH2:
        F1(39b), F2(128b), F3(39b), FIXED(21b), F4(8b)
    */
    const unsigned int fixed_offset = 8;
    const unsigned int f3_offset = fixed_offset + 21;  /* = 29 */
    const unsigned int f2_offset = f3_offset + 39;     /* = 68 */
    const unsigned int f1_offset = f2_offset + 128;    /* = 196 */

    /* Double wide DATA_KEY format for GH2:
        RSV(1b), WF1(66b), DWF2(128b), DWF3(32b), DWF4(8b)
    */
    const unsigned int dwf4_offset = 0;
    const unsigned int dwf3_offset = 8;
    const unsigned int dwf2_offset = dwf3_offset + 32;     /* = 40 */
    const unsigned int dwf1_offset = dwf2_offset + 128;    /* = 168 */

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

    /* FP_FIXED */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelDisable, 0, fixed_offset, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 1, 3);
    /* SVP_OR_L3IIF (fixed_offset + 4, 1) no support */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 5, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 9, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 10, 1);
    /* TUNNEL_TYPE_LOOPBACK_TYPE (fixed_offset + 11, 4) no support */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 15, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 16, 1);
    if (soc_feature(unit, soc_feature_miml)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimlPkt,
                     _bcmFieldSliceSelDisable, 0, fixed_offset + 17, 1);
    }
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCustomHeaderPkt,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 18, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPreemptablePacket,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 19, 1);
#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_pdelay_req)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPdelayReq,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 20, 1);
    }
#endif


    /* FPF3 */
    /* F3_0 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 0, f3_offset, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 1, f3_offset, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 2, f3_offset, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 3, f3_offset, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 0,
                               f3_offset + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 1,
                               f3_offset + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 2,
                               f3_offset + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 3,
                               f3_offset + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVrf, f3_offset + 12, 12);
    /* VFI (bcmFieldQualifyVpn) are not supported in ForwardingField */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan, f3_offset + 12, 12);
    /* SVP_L3IIF_OVERLAY (f3_offset + 24, 13) no support */

    /* F3_1 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 0, f3_offset, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 1, f3_offset, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 2, f3_offset, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 3, f3_offset, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 0,
                               f3_offset + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 1,
                               f3_offset + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 2,
                               f3_offset + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 3,
                               f3_offset + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVrf, f3_offset + 12, 12);
    /* VFI (bcmFieldQualifyVpn) are not supported in ForwardingField */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan, f3_offset + 12, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 1, f3_offset + 24, 12);

    /* F3_2 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f3_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f3_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f3_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f3_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup, f3_offset, 19);
    /* GLP: T(1b), MODID(8b) ,PORT(7b) */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityGlp, f3_offset + 19, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityGlp, f3_offset + 19, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               f3_offset + 19, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               f3_offset + 26, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyTrunkMemberSourceModuleId,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               f3_offset + 26, 8);
    /* SVP_VALID (f3_offset + 35, 1) no support */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 2, f3_offset + 36, 3);

    /* F3_3 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 3, f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 3, f3_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 13, 3);
    /* PACKET_FORMAT: pkt_fmt(2b), outer_tagged(1b), inner_tagged(1b) */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 16, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 18, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 20, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 22, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 25, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 30, 3);

    /* F3_4 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 4, f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 4, f3_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 4, f3_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 4, f3_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 4, f3_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 4, f3_offset + 16, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 4, f3_offset + 28, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 4, f3_offset + 29, 3);

    /* F3_5 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 5, f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 5, f3_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 5, f3_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 5, f3_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 5, f3_offset + 16, 16);

    /* F3_6 */
    /* MPLS Terminated (f3_offset, 1) is not supported */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 1, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 2, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 3, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 5, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 7, 1);
    /* OUTER_LABEL_POP (f3_offset + 8, 1) */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 9, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 10, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 11, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 13, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 14, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 15, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 16, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 28, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 29, 3);

    /* F3_7 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf3, 7, f3_offset, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 0,
                               f3_offset + 24, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 1,
                               f3_offset + 24, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 2,
                               f3_offset + 24, 8);
   _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                              _bcmFieldQualifyRangeCheckBits24_31,
                              _bcmFieldSliceSelFpf3, 7,
                              _bcmFieldSliceIntfClassSelect, 3,
                              f3_offset + 24, 8);

    /* F3_8 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf3, 8, f3_offset, 20);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 8, f3_offset + 20, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 8, f3_offset + 22, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 8, f3_offset + 24, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 8, f3_offset + 26, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 8, f3_offset + 28, 2);

    /* F3_9 */
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData3,
                 _bcmFieldSliceSelFpf3, 9, f3_offset, 32);
    /* MPLS_INFO (f3_offset + 32, 2) no support */

    /* F3_10 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f3_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f3_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f3_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f3_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup, f3_offset, 19);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf3, 10, f3_offset + 19, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf3, 10, f3_offset + 19, 16);

    /* F3_11 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf3, 11, f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf3, 11, f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                 _bcmFieldSliceSelFpf3, 11, f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf3, 11, f3_offset + 7, 8);
    /* SVP (f1_offset + 16, 15) no support */
    /* SVP_VALID (f1_offset + 31, 1) no support */

    /* F3_12 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagVn, f3_offset, 33);
    if (soc_feature(unit, soc_feature_miml)) {
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMimlInnerTag,
                                   _bcmFieldSliceSelFpf3, 12,
                                   _bcmFieldSliceAuxTag2Select,
                                   _bcmFieldAuxTagMimlHeader, f3_offset, 33);
    }
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCustomHeaderData,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagCustomHeader, f3_offset, 32);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadL4HdrParseable,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f3_offset, 1,
                     f3_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadL3HdrParseable,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f3_offset + 1, 1,
                     f3_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadInnerVlan,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f3_offset + 2, 16,
                     f3_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadInnerVlanId,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f3_offset + 2, 12,
                     f3_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadInnerVlanCfi,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f3_offset + 14, 1,
                     f3_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadInnerVlanPri,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f3_offset + 15, 3,
                     f3_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadVlanFormat,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f3_offset + 18, 2,
                     f3_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadSOF,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f3_offset + 20, 1,
                     f3_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapHdrType,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f3_offset + 21, 1,
                     f3_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapHdrRid,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f3_offset + 22, 5,
                     f3_offset + 32, 1,
                     0,  0,
                     0
                     );

    /* F3_13 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRoceVer1Pkt,
                 _bcmFieldSliceSelFpf3, 13, f3_offset, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRoceVer2Pkt,
                 _bcmFieldSliceSelFpf3, 13, f3_offset + 1, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRoceBthFlags,
                 _bcmFieldSliceSelFpf3, 13, f3_offset + 2, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRoceBthDstQueuePair,
                 _bcmFieldSliceSelFpf3, 13, f3_offset + 10, 24);

    /* FPF2 */
    /* F2_0 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 0, f2_offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 8, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 14, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 56, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 64, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 96, 32);

    /* F2_1 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 1, f2_offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 1, f2_offset + 8, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf2, 1, f2_offset + 14, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 1, f2_offset + 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 1, f2_offset + 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 1, f2_offset + 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 1, f2_offset + 56, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 1, f2_offset + 64, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 1, f2_offset + 96, 32);

    /* F2_2 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelFpf2, 2, f2_offset, 128);

    /* F2_3 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf2, 3, f2_offset, 128);

    /* F2_4 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 4, f2_offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 8, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 14, 20);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 34, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 42, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 50, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 64, 64);

    /* F2_5 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 5, f2_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 5, f2_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 5, f2_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 5, f2_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 5, f2_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 5, f2_offset + 32, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5, f2_offset + 80, 48);

    /* F2_6 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 6, f2_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 6, f2_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 6, f2_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 6, f2_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 6, f2_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 6, f2_offset + 32, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 6, f2_offset + 80, 32);

    /* F2_7 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 7, f2_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 7, f2_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 32, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 40, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 48, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 80, 48);

    /* F2_8 */
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 8, f2_offset, 128);

    /* F2_9 */
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                 _bcmFieldSliceSelFpf2, 9, f2_offset, 128);

    /* F2_10 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                 _bcmFieldSliceSelFpf2, 10, f2_offset, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 64, 64);

    /* F2_11 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 11, f2_offset + 32, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 11, f2_offset + 80, 48);

    /* F2_12 */
    if (soc_feature(unit, soc_feature_miml)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimlSrcMac,
                     _bcmFieldSliceSelFpf2, 12, f2_offset, 48);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimlDstMac,
                     _bcmFieldSliceSelFpf2, 12, f2_offset + 48, 48);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimlVlan,
                     _bcmFieldSliceSelFpf2, 12, f2_offset + 96, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimlVlanId,
                     _bcmFieldSliceSelFpf2, 12, f2_offset + 96, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimlVlanCfi,
                     _bcmFieldSliceSelFpf2, 12, f2_offset + 108, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimlVlanPri,
                     _bcmFieldSliceSelFpf2, 12, f2_offset + 109, 3);
    }

    /* F2_13 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadOuterVlan,
                 _bcmFieldSliceSelFpf2, 13, f2_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadOuterVlanId,
                 _bcmFieldSliceSelFpf2, 13, f2_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 13, f2_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 13, f2_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadEtherType,
                 _bcmFieldSliceSelFpf2, 13, f2_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadSrcMac,
                 _bcmFieldSliceSelFpf2, 13, f2_offset + 32, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadDstMac,
                 _bcmFieldSliceSelFpf2, 13, f2_offset + 80, 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadOuterVlan,
                 _bcmFieldSliceSelFpf2, 13, f2_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadOuterVlanId,
                 _bcmFieldSliceSelFpf2, 13, f2_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 13, f2_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 13, f2_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadEtherType,
                 _bcmFieldSliceSelFpf2, 13, f2_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadSrcMac,
                 _bcmFieldSliceSelFpf2, 13, f2_offset + 32, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadDstMac,
                 _bcmFieldSliceSelFpf2, 13, f2_offset + 80, 48);

    /* F2_14 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadL4DstPort,
                 _bcmFieldSliceSelFpf2, 14, f2_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadL4SrcPort,
                 _bcmFieldSliceSelFpf2, 14, f2_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadTos,
                 _bcmFieldSliceSelFpf2, 14, f2_offset + 32, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadIpProtocol,
                 _bcmFieldSliceSelFpf2, 14, f2_offset + 40, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadDip,
                 _bcmFieldSliceSelFpf2, 14, f2_offset + 48, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadSip,
                 _bcmFieldSliceSelFpf2, 14, f2_offset + 80, 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadL4DstPort,
                 _bcmFieldSliceSelFpf2, 14, f2_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadL4SrcPort,
                 _bcmFieldSliceSelFpf2, 14, f2_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                 _bcmFieldSliceSelFpf2, 14, f2_offset + 32, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadIpProtocol,
                 _bcmFieldSliceSelFpf2, 14, f2_offset + 40, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadDip,
                 _bcmFieldSliceSelFpf2, 14, f2_offset + 48, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadSip,
                 _bcmFieldSliceSelFpf2, 14, f2_offset + 80, 32);

    /* F2_15 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadSip6,
                 _bcmFieldSliceSelFpf2, 15, f2_offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadSip6,
                 _bcmFieldSliceSelFpf2, 15, f2_offset, 128);

    /* F2_16 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadDip6,
                 _bcmFieldSliceSelFpf2, 16, f2_offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadDip6,
                 _bcmFieldSliceSelFpf2, 16, f2_offset, 128);

    /* SINGLE F2 17 */
    if (soc_feature(unit, soc_feature_tsn_taf)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTafEnable,
                     _bcmFieldSliceSelFpf2, 17,
                     f2_offset + 113, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTafGateId,
                     _bcmFieldSliceSelFpf2, 17,
                     f2_offset + 106, 7);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTafGateState,
                     _bcmFieldSliceSelFpf2, 17,
                     f2_offset + 105, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTafCosProfile,
                     _bcmFieldSliceSelFpf2, 17,
                     f2_offset + 101, 4);
    }
    /* not support: PACKET_IS_1588 */
    /* not support: PTP_DOMAIN_ID_MISMATCH */
    /* not support: PTP_SOURCE_PORT_IDENTITY */
    /* not support: PTP_MESSAGE_TYPE */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySRSrcNodeIsSan,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 76, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcMulticastHit,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 73, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DstMulticastHit,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 71, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestSRNodeType,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 69, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySRSupervisionType,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 67, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySRTagType,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 64, 3);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySRLanId,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf2, 17,
                     0, 0,
                     0,
                     f2_offset + 63, 1,
                     f2_offset + 31, 1,
                     0, 0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySRNetId,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf2, 17,
                     0, 0,
                     0,
                     f2_offset + 60, 3,
                     f2_offset + 30, 1,
                     0, 0,
                     0
                     );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPortSRRoleInterlink,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 58, 1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcPortSRType,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf2, 17,
                     0, 0,
                     0,
                     f2_offset + 55, 3,
                     f2_offset + 59, 1,
                     0, 0,
                     0
                     );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPortSRLanId,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 54, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPortSRNetId,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 51, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPortSRMode,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 48, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPortSRRoleInterlink,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 46, 1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPortSRType,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf2, 17,
                     0, 0,
                     0,
                     f2_offset + 43, 3,
                     f2_offset + 47, 1,
                     0, 0,
                     0
                     );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPortSRLanId,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 42, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPortSRNetId,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 39, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPortSRMode,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 36, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanSREnable,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 35, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanSRLanId,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 34, 1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySRError,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf2, 17,
                     0, 0,
                     0,
                     f2_offset + 75, 1,
                     f2_offset + 32, 2,
                     0, 0,
                     0
                     );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySRFlowId,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 17, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTsnFlowId,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 5, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset + 1, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySRNetIdMatched,
                 _bcmFieldSliceSelFpf2, 17,
                 f2_offset, 1);

    /* F2_18 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 18, f2_offset, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 18, f2_offset + 32, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadDstMac,
                 _bcmFieldSliceSelFpf2, 18, f2_offset + 64, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                 _bcmFieldSliceSelFpf2, 18, f2_offset + 112, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf2, 18, f2_offset + 120, 2);

    /* FPF1 */
    /* F1_0 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 0, f1_offset, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 1, f1_offset, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 2, f1_offset, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 3, f1_offset, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 0,
                               f1_offset + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 1,
                               f1_offset + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 2,
                               f1_offset + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 3,
                               f1_offset + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVrf, f1_offset + 12, 12);
    /* VFI (bcmFieldQualifyVpn) are not supported in ForwardingField */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan, f1_offset + 12, 12);
    /* SVP_L3_IIF (f1_offset + 24, 13) no support */

    /* F1_1 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f1_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f1_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f1_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f1_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup, f1_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityGlp, f1_offset + 19, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityGlp, f1_offset + 19, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               f1_offset + 19, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               f1_offset + 26, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyTrunkMemberSourceModuleId,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               f1_offset + 26, 8);
    /* SVP_VALID (f1_offset + 35, 1) no support */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 1, f1_offset + 36, 3);

    /* F1_2 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f1_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f1_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f1_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f1_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup, f1_offset, 19);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 2, f1_offset + 19, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 0,
                               f1_offset + 22, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 1,
                               f1_offset + 22, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 2,
                               f1_offset + 22, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 3,
                               f1_offset + 22, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 0,
                               f1_offset + 28, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 1,
                               f1_offset + 28, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 2,
                               f1_offset + 28, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 3,
                               f1_offset + 28, 6);

    /* F1_3 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 3, f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 3, f1_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 3, f1_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 3, f1_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 3, f1_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 3, f1_offset + 16, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 3, f1_offset + 28, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 3, f1_offset + 29, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf1, 3, f1_offset + 32, 2);

    /* F1_4 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 4, f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 4, f1_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 4, f1_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 4, f1_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 4, f1_offset + 16, 16);

    /* F1_5 */
    /* bcmFieldQualifyMplsTerminated is not supported */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 1, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 2, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 3, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 5, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 7, 1);
    /* OUTER_LABEL_POP */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 9, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 10, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 11, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 13, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 14, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 15, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 16, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 28, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 29, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf1, 5, f1_offset + 32, 2);

    /* F1_6 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 6, f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 6, f1_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 6, f1_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 6, f1_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 6, f1_offset + 16, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 6, f1_offset + 18, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf1, 6, f1_offset + 20, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 6, f1_offset + 22, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 6, f1_offset + 25, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf1, 6, f1_offset + 30, 3);

    /* F1_7 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 7, f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 7, f1_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 7, f1_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 7, f1_offset + 13, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceDstClassSelect, 0,
                               f1_offset + 16, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceDstClassSelect, 1,
                               f1_offset + 16, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 2,
                               f1_offset + 16, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 3,
                               f1_offset + 16, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVrf, f1_offset + 22, 12);
    /* VFI (bcmFieldQualifyVpn) are not supported in ForwardingField */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan, f1_offset + 22, 12);

    /* F1_8 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf1, 8, f1_offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf1, 8, f1_offset + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceDstClassSelect, 0,
                               f1_offset + 16, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceDstClassSelect, 1,
                               f1_offset + 16, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 2,
                               f1_offset + 16, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 3,
                               f1_offset + 16, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVrf, f1_offset + 22, 12);
    /* VFI (bcmFieldQualifyVpn) are not supported in ForwardingField */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan, f1_offset + 22, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                               _bcmFieldSliceSelFpf1, 8, f1_offset + 34, 2);

    /* F1_9 */
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData2,
                               _bcmFieldSliceSelFpf1, 9, f1_offset, 32);

    /* F1_10 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f1_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f1_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f1_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f1_offset, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup, f1_offset, 19);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf1, 10, f1_offset + 19, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf1, 10, f1_offset + 19, 16);

    /* F1_11 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf1, 11, f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf1, 11, f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                 _bcmFieldSliceSelFpf1, 11, f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf1, 11, f1_offset + 7, 8);
    /* SVP (f1_offset + 16, 15) no support */
    /* SVP_VALID (f1_offset + 31, 1) no support */

    /* F1_12 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 12, f1_offset, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 0,
                               f1_offset + 12, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 1,
                               f1_offset + 12, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 2,
                               f1_offset + 12, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 3,
                               f1_offset + 12, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 0,
                               f1_offset + 18, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 1,
                               f1_offset + 18, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 2,
                               f1_offset + 18, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 3,
                               f1_offset + 18, 6);
    /* SVP (f1_offset + 24, 15) no support */

    /* F1_13 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagVn, f1_offset, 33);
    if (soc_feature(unit, soc_feature_miml)) {
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMimlInnerTag,
                                   _bcmFieldSliceSelFpf1, 13,
                                   _bcmFieldSliceAuxTag1Select,
                                   _bcmFieldAuxTagMimlHeader, f1_offset, 33);
    }
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCustomHeaderData,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagCustomHeader, f1_offset, 32);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadL4HdrParseable,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f1_offset, 1,
                     f1_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadL3HdrParseable,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f1_offset + 1, 1,
                     f1_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadInnerVlan,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f1_offset + 2, 16,
                     f1_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadInnerVlanId,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f1_offset + 2, 12,
                     f1_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadInnerVlanCfi,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f1_offset + 14, 1,
                     f1_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadInnerVlanPri,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f1_offset + 15, 3,
                     f1_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadVlanFormat,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f1_offset + 18, 2,
                     f1_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadSOF,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f1_offset + 20, 1,
                     f1_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapHdrType,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f1_offset + 21, 1,
                     f1_offset + 32, 1,
                     0,  0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCapwapHdrRid,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagCapwapInfo,
                     0,
                     f1_offset + 22, 5,
                     f1_offset + 32, 1,
                     0,  0,
                     0
                     );

    /* F1_14 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
                 _bcmFieldSliceSelFpf1, 14, f1_offset, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVxlanClassValid,
                 _bcmFieldSliceSelFpf1, 14, f1_offset + 1, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVxlanNetworkId,
                 _bcmFieldSliceSelFpf1, 14, f1_offset + 2, 24);
    /* Not required for VXLAN-LITE: VXLAN_CLASS_ID (f1_offset + 26, 6) */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVxlanPacket,
                 _bcmFieldSliceSelFpf1, 14, f1_offset + 32, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelFpf1, 14, f1_offset + 33, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVxlanPayloadVlanFormat,
                 _bcmFieldSliceSelFpf1, 14, f1_offset + 34, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVxlanVnidVlanTranslateHit,
                 _bcmFieldSliceSelFpf1, 14, f1_offset + 36, 1);

    /* F1_15 */
    
    
    
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRoceVer1Pkt,
                 _bcmFieldSliceSelFpf1, 15, f1_offset + 13, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRoceVer2Pkt,
                 _bcmFieldSliceSelFpf1, 15, f1_offset + 14, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRoceBthOpcode,
                 _bcmFieldSliceSelFpf1, 15, f1_offset + 15, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRoceBthPartitionKey,
                 _bcmFieldSliceSelFpf1, 15, f1_offset + 23, 16);


    /* DWF4 */
    /* DWF4_1 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf4, 1,
                            _bcmFieldSliceSelDisable, 0, dwf4_offset, 6);

    /* DWF3 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceSelDisable, 0, dwf3_offset, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceSelDisable, 0, dwf3_offset + 8, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyExtensionHeaderSubCode,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceSelDisable, 0, dwf3_offset + 16, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceSelDisable, 0, dwf3_offset + 24, 8);

    /* DWF2 */
    /* DWF2_0 */
    /* bcmFieldQualifyMplsTerminated is not supported */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 17, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 18, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 19, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 21, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 22, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 23, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 25, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 26, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 27, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 28, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 29, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 30, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 31, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 32, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 0,
                            dwf2_offset + 56, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 1,
                            dwf2_offset + 56, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 2,
                            dwf2_offset + 56, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyRangeCheckBits24_31,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 3,
                            dwf2_offset + 56, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 64, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 96, 32);

    /* DWF2_1 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset, 128);

    /* DWF2_2 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                            _bcmFieldSliceSelFpf2, 2,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset, 128);

    /* DWF2_3 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                            _bcmFieldSliceSelFpf2, 3,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset, 128);

    /* DWF2_4 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyCapwapPayloadOuterVlan,
                            _bcmFieldSliceSelFpf2, 4,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyCapwapPayloadOuterVlanId,
                            _bcmFieldSliceSelFpf2, 4,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset, 12);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyCapwapPayloadOuterVlanCfi,
                            _bcmFieldSliceSelFpf2, 4,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 12, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyCapwapPayloadOuterVlanPri,
                            _bcmFieldSliceSelFpf2, 4,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 13, 3);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyCapwapPayloadEtherType,
                            _bcmFieldSliceSelFpf2, 4,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 16, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadSrcMac,
                            _bcmFieldSliceSelFpf2, 4,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 32, 48);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadDstMac,
                            _bcmFieldSliceSelFpf2, 4,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 80, 48);

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyTunnelPayloadOuterVlan,
                            _bcmFieldSliceSelFpf2, 4,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyTunnelPayloadOuterVlanId,
                            _bcmFieldSliceSelFpf2, 4,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset, 12);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyTunnelPayloadOuterVlanCfi,
                            _bcmFieldSliceSelFpf2, 4,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 12, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyTunnelPayloadOuterVlanPri,
                            _bcmFieldSliceSelFpf2, 4,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 13, 3);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyTunnelPayloadEtherType,
                            _bcmFieldSliceSelFpf2, 4,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 16, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadSrcMac,
                            _bcmFieldSliceSelFpf2, 4,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 32, 48);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadDstMac,
                            _bcmFieldSliceSelFpf2, 4,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 80, 48);

    /* DWF2_5 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyCapwapPayloadL4DstPort,
                            _bcmFieldSliceSelFpf2, 5,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyCapwapPayloadL4SrcPort,
                            _bcmFieldSliceSelFpf2, 5,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 16, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadTos,
                            _bcmFieldSliceSelFpf2, 5,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 32, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyCapwapPayloadIpProtocol,
                            _bcmFieldSliceSelFpf2, 5,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 40, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadDip,
                            _bcmFieldSliceSelFpf2, 5,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 48, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadSip,
                            _bcmFieldSliceSelFpf2, 5,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 80, 32);

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyTunnelPayloadL4DstPort,
                            _bcmFieldSliceSelFpf2, 5,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyTunnelPayloadL4SrcPort,
                            _bcmFieldSliceSelFpf2, 5,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 16, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                            _bcmFieldSliceSelFpf2, 5,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 32, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyTunnelPayloadIpProtocol,
                            _bcmFieldSliceSelFpf2, 5,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 40, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadDip,
                            _bcmFieldSliceSelFpf2, 5,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 48, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadSip,
                            _bcmFieldSliceSelFpf2, 5,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset + 80, 32);

    /* DWF2_6 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadSip6,
                            _bcmFieldSliceSelFpf2, 6,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset, 128);

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadSip6,
                            _bcmFieldSliceSelFpf2, 6,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset, 128);

    /* DWF2_7 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadDip6,
                            _bcmFieldSliceSelFpf2, 7,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset, 128);

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTunnelPayloadDip6,
                            _bcmFieldSliceSelFpf2, 7,
                            _bcmFieldSliceSelDisable, 0, dwf2_offset, 128);

    /* DWF1 */
    /* DWF1_0 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0, dwf1_offset, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0, dwf1_offset + 8, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0, dwf1_offset + 10, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0, dwf1_offset + 26, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect,
                            _bcmFieldFwdEntityGlp,
                            dwf1_offset + 42, 19);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect,
                            _bcmFieldFwdEntityGlp,
                            dwf1_offset + 42, 19);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect,
                            _bcmFieldFwdEntityL3Egress,
                            dwf1_offset + 42, 19);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect,
                            _bcmFieldFwdEntityL3Egress,
                            dwf1_offset + 42, 19);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect,
                            _bcmFieldFwdEntityMulticastGroup,
                            dwf1_offset + 42, 19);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyIntCongestionNotification,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0,
                            dwf1_offset + 61, 2);

    /* DWF1_1 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyCapwapPayloadL4DstPort,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0, dwf1_offset, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyCapwapPayloadL4SrcPort,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0, dwf1_offset + 16, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyCapwapPayloadTos,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0, dwf1_offset + 32, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyCapwapPayloadIpProtocol,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0, dwf1_offset + 40, 8);

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyTunnelPayloadL4DstPort,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0, dwf1_offset, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyTunnelPayloadL4SrcPort,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0, dwf1_offset + 16, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0, dwf1_offset + 32, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyTunnelPayloadIpProtocol,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0, dwf1_offset + 40, 8);

    /* DWF1_2 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyRoceVer1Pkt,
                 _bcmFieldSliceSelFpf1, 2,
                 _bcmFieldSliceSelDisable, 0, dwf1_offset, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyRoceVer2Pkt,
                 _bcmFieldSliceSelFpf1, 2,
                 _bcmFieldSliceSelDisable, 0, dwf1_offset + 1, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyRoceBthFlags,
                 _bcmFieldSliceSelFpf1, 2,
                 _bcmFieldSliceSelDisable, 0, dwf1_offset + 2, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyRoceBthDstQueuePair,
                 _bcmFieldSliceSelFpf1, 2,
                 _bcmFieldSliceSelDisable, 0, dwf1_offset + 10, 24);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyRoceBthOpcode,
                 _bcmFieldSliceSelFpf1, 2,
                 _bcmFieldSliceSelDisable, 0, dwf1_offset + 34, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyRoceBthPartitionKey,
                 _bcmFieldSliceSelFpf1, 2,
                 _bcmFieldSliceSelDisable, 0, dwf1_offset + 42, 16);


    /* DONE DWF */
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_gh2_lookup_qualifiers_init
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
_field_gh2_lookup_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    /* Key Format:
        F1(53b), F2(128b), F3(32b)
     */
    const unsigned int f3_offset = 0;
    const unsigned int f2_offset = 32;
    const unsigned int f1_offset = f2_offset + 128;    /* = 160 */

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


    /* VFP_F3 */
    /* VFP_F3_0 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 0, f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 0, f3_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 16, 16);

    /* VFP_F3_1 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, 1, f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, 1, f3_offset + 16, 16);

    /* VFP_F3_2 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, 2, f3_offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, 2, f3_offset + 8, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, 2, f3_offset + 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, 2, f3_offset + 24, 8);

    /* VFP_F3_3 */
    /* TUNNEL_CLASSID is not supported */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 8, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 8, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 20, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 21, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 24, 8);

    /* VFP_F3_4 */
    if (soc_feature(unit, soc_feature_vxlan_lite_riot)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVxlanNetworkId,
                     _bcmFieldSliceSelFpf3, 3, f3_offset, 24);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVxlanPacket,
                     _bcmFieldSliceSelFpf3, 3, f3_offset + 24, 1);
    }

    /* VFP_F2 */
    /* VFP_F2_0 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 0, f2_offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 8, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 14, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 56, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 64, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 96, 32);

    /* VFP_F2_1 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelFpf2, 1, f2_offset, 128);

    /* VFP_F2_2 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf2, 2, f2_offset, 128);

    /* VFP_F2_3 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 3, f2_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 3, f2_offset + 32, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 3, f2_offset + 80, 48);

    /* VFP_F2_4 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 4, f2_offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 8, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 16, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 48, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 80, 48);

    /* VFP_F2_5 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 5, f2_offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 5, f2_offset + 8, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 5, f2_offset + 16, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 5, f2_offset + 48, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5, f2_offset + 80, 48);

    /* VFP_F2_6 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                 _bcmFieldSliceSelFpf2, 6, f2_offset, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 6, f2_offset + 64, 64);

    /* VFP_F2_7 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                 _bcmFieldSliceSelFpf2, 7, f2_offset, 33);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySnap,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 64, 40);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLlc,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 104, 24);


    /* VFP_F1 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocolCommon,
                 _bcmFieldSliceSelFpf1, 0, f1_offset, 3);
    /* bcmFieldQualifyTunnelTerminated is not supported */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 4, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 7, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 9, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 11, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 13, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 13, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 25, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 26, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 29, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 31, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 38, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 46, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 50, 1);

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_gh2_egress_qualifiers_init
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
_field_gh2_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_DECL;
    _key_fld_ = KEYf;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageEgress,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

    /* EFP_KEY1 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 1, 2);
    if (soc_feature(unit, soc_feature_vxlan_lite_riot)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDecap,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 3, 1);
    }
#ifdef BCM_FIRELIGHT_SUPPORT
    if (SOC_IS_FIRELIGHT(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHPriority,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 4, 4);
    }
#endif
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
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 165, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 172, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 173, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 174, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 174, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 186, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 187, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 190, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 192, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 193, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 201, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 208, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 212, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPreemptablePacket,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 213, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 0);


    /* EFP_KEY2 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 1, 2);
    if (soc_feature(unit, soc_feature_vxlan_lite_riot)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDecap,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 3, 1);
    }
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
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 165, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 172, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 173, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 174, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 174, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 186, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 187, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 190, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 192, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 193, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 201, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 208, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 212, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPreemptablePacket,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 213, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 0);


    /* EFP_KEY3 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 1, 2);
    if (soc_feature(unit, soc_feature_vxlan_lite_riot)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDecap,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 3, 1);
    }
#ifdef BCM_FIRELIGHT_SUPPORT
    if (SOC_IS_FIRELIGHT(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHPriority,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 4, 4);
    }
#endif
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
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPreemptablePacket,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 213, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 0);


    /* EFP_KEY4 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 1, 2);
    if (soc_feature(unit, soc_feature_vxlan_lite_riot)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDecap,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 3, 1);
    }
#ifdef BCM_FIRELIGHT_SUPPORT
    if (SOC_IS_FIRELIGHT(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHPriority,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 4, 4);
    }
#endif
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
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 165, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 172, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 173, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 174, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 174, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 186, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 187, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 190, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 192, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 193, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 201, 7);
    /* L3_PKT_TYPE */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrMdlKey4, 0, 208, 4);
    /* MDL */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOamMdl,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrMdlKey4, 1, 208, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 212, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPreemptablePacket,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 213, 1);

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
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 165, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 172, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 173, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 174, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 174, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 186, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 187, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 190, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 192, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 193, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 201, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 208, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 212, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPreemptablePacket,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 213, 1);


    /* EFP_KEY5 */
    /* not support: KEY_MATCH_TYPE */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPreemptablePacket,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 213, 1);
    /* not support: PTP_DOMAIN_ID_MISMATCH */
    /* not support: SOURCE_PORT_IDENTITY */
    /* not support: PTP_MESSAGE_TYPE */
    /* not support: EGR_PORT_PTP_DOMAIN_ID */
    /* not support: PTP_DOMAIN_ID */
    /* not support: PACKET_IS_1588 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestSRNodeType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                 89, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySRTagType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                 50, 3);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySRLanId,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                     0, 0,
                     0,
                     49, 1,
                     45, 1,
                     0, 0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySRNetId,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                     0, 0,
                     0,
                     46, 3,
                     44, 1,
                     0, 0,
                     0
                     );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPortSRRoleInterlink,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                 42, 1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcPortSRType,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                     0, 0,
                     0,
                     39, 3,
                     43, 1,
                     0, 0,
                     0
                     );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPortSRLanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                 38, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPortSRNetId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                 35, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPortSRMode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                 32, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPortSRRoleInterlink,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                 30, 1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPortSRType,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                     0, 0,
                     0,
                     27, 3,
                     31, 1,
                     0, 0,
                     0
                     );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPortSRLanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                 26, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPortSRNetId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                 23, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPortSRMode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                 20, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanSREnable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                 19, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanSRLanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                 18, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySRFlowId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                 5, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                 1, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySRDuplicate,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                 0, 1);

    return (BCM_E_NONE);
}



/*
 * Function:
 *     _field_gh2_qualifiers_init
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
_field_gh2_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
          rv = _field_gh2_ingress_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _field_gh2_lookup_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _field_gh2_egress_qualifiers_init(unit, stage_fc);
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
 *     _field_gh2_write_slice_map_ingress
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
_field_gh2_write_slice_map_ingress(int unit, _field_stage_t *stage_fc)
{
    fp_slice_map_entry_t map_entry;  /* HW entry buffer.         */
    soc_field_t field;               /* HW entry fields.         */
    int vmap_size;                   /* Virtual map index count. */
    uint32 value;                    /* Field entry value.       */
    int idx;                         /* Map fields iterator.     */
    int rv;                          /* Operation return status. */
    uint32 virtual_to_physical_map_gh2[12] = {
            VIRTUAL_SLICE_0_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_1_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_2_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_3_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_4_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_5_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_6_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_7_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_8_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_9_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_10_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
            VIRTUAL_SLICE_11_PHYSICAL_SLICE_NUMBER_ENTRY_0f
    };

    uint32 virtual_to_group_map_gh2[12] = {
            VIRTUAL_SLICE_0_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_1_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_2_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_3_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_4_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_5_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_6_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_7_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_8_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_9_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_10_VIRTUAL_SLICE_GROUP_ENTRY_0f,
            VIRTUAL_SLICE_11_VIRTUAL_SLICE_GROUP_ENTRY_0f
    };

    /* Calculate virtual map size. */
    rv = _bcm_field_virtual_map_size_get(unit, stage_fc, &vmap_size);
    BCM_IF_ERROR_RETURN(rv);

    rv = READ_FP_SLICE_MAPm(unit, MEM_BLOCK_ANY, 0, &map_entry);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < vmap_size; idx++) {
        value = (stage_fc->vmap[_FP_DEF_INST][0][idx]).vmap_key;
        field = virtual_to_physical_map_gh2[idx];
        soc_FP_SLICE_MAPm_field32_set(unit, &map_entry, field, value);

        value = (stage_fc->vmap[_FP_DEF_INST][0][idx]).virtual_group;
        field = virtual_to_group_map_gh2[idx];
        soc_FP_SLICE_MAPm_field32_set(unit, &map_entry, field, value);
    }

    rv = WRITE_FP_SLICE_MAPm(unit, MEM_BLOCK_ALL, 0, &map_entry);
    return (rv);
}


/*
 * Function:
 *     _bcm_field_gh2_write_slice_map
 *
 * Purpose:
 *     Write the FP_SLICE_MAP
 *
 * Parameters:
 *     unit
 *     stage_fc - pointer to stage control block
 *     fg - Reference to Field Group Structure.
 *
 * Returns:
 *     nothing
 *
 * Notes:
 */
STATIC int
_bcm_field_gh2_write_slice_map(int unit, _field_stage_t *stage_fc,
                              _field_group_t *fg)
{
    int rv = BCM_E_PARAM;   /* Operation return status. */

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        rv = _field_gh2_write_slice_map_ingress(unit, stage_fc);
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EGRESS) {
        rv =  _bcm_field_trx_write_slice_map_egress(unit, stage_fc);
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_LOOKUP) {
        rv =  _bcm_field_trx_write_slice_map_vfp(unit, stage_fc);
    }

    return (rv);
}


/*
 * Function:
 *     _bcm_field_gh2_init
 * Purpose:
 *     Perform initializations that are specific to BCM53570. This
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
_bcm_field_gh2_init(int unit, _field_control_t *fc)
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
        BCM_IF_ERROR_RETURN(_field_gh2_qualifiers_init(unit, stage_fc));

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
    _field_gh2_functions_init(&fc->functions);

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_gh2_functions_init
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
_field_gh2_functions_init(_field_funct_t *functions)
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
    /* functions->fp_slice_reinit      = NULL; */
    functions->fp_write_slice_map      = _bcm_field_gh2_write_slice_map;
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
    functions->fp_external_entry_install =
                        _bcm_field_tr_external_entry_install;
    functions->fp_external_entry_reinstall =
                        _bcm_field_tr_external_entry_reinstall;
    functions->fp_external_entry_remove =
                        _bcm_field_tr_external_entry_remove;
    functions->fp_external_entry_prio_set =
                        _bcm_field_tr_external_entry_prio_set;
    functions->fp_data_qualifier_ethertype_add =
                       _bcm_field_fb_data_qualifier_ethertype_add;
    functions->fp_data_qualifier_ethertype_delete =
                       _bcm_field_fb_data_qualifier_ethertype_delete;
    functions->fp_data_qualifier_ip_protocol_add =
                       _bcm_field_fb_data_qualifier_ip_protocol_add;
    functions->fp_data_qualifier_ip_protocol_delete =
                       _bcm_field_fb_data_qualifier_ip_protocol_delete;
    functions->fp_data_qualifier_packet_format_add =
                       _bcm_field_fb_data_qualifier_packet_format_add;
    functions->fp_data_qualifier_packet_format_delete =
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
#else /* BCM_GREYHOUND2_SUPPORT && BCM_FIELD_SUPPORT */
typedef int _greyhound2_field_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_GREYHOUND2_SUPPORT && BCM_FIELD_SUPPORT */
