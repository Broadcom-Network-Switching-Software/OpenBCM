/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field.c
 * Purpose:     BCM56850 Field Processor installation functions.
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#if defined(BCM_TRIDENT2_SUPPORT) && defined(BCM_FIELD_SUPPORT)

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/trident2.h>
#include <soc/triumph3.h>

#include <bcm/error.h>
#include <bcm/l3.h>
#include <bcm/field.h>
#include <bcm/tunnel.h>

#include <bcm_int/common/multicast.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/policer.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/nat.h>
#include <bcm_int/esw/oam.h>
#include <soc/scache.h>

#define ALIGN32(x)      (((x) + 31) & ~31)

/*
 * Function:
 *     _field_sc960_ingress_qualifiers_init
 * Purpose:
 *     Initialize device stage ingress qualifiers
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.
 *
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_sc960_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    const unsigned
        f2_offset = 10 - 2,
        fixed_offset = 138 - 2,
        fp_global_mask_tcam_ofs = ALIGN32(soc_mem_field_length(unit, 
                                                        FP_TCAMm, KEYf)),
        f3_offset = 2 - 2,
        f1_offset = 51 - 2;

    _FP_QUAL_DECL;

    /* Handled outside normal qualifier set/get scheme */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStage,
                 _bcmFieldSliceSelDisable, 0,
                 0, 0
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageIngress,
                 _bcmFieldSliceSelDisable, 0,
                 0, 0
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelDisable, 0,
                 0, 0
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPorts,
                 _bcmFieldSliceSelDisable, 0,
                 0, 0
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyNormalizeMacAddrs,
                 _bcmFieldSliceSelDisable, 0,
                 0, 0
                 );

/* FPF1 single wide */
/* Present in Global Mask Field */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_global_mask_tcam_ofs + f1_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_global_mask_tcam_ofs + f1_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset + 10, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_global_mask_tcam_ofs + f1_offset + 10, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_global_mask_tcam_ofs + f1_offset + 10, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan,
                               fp_global_mask_tcam_ofs + f1_offset + 20, 13);


    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunkMemberGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f1_offset + 28, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f1_offset + 28, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 1,
                 fp_global_mask_tcam_ofs + f1_offset + 37, 3);



    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 2,
                 fp_global_mask_tcam_ofs + f1_offset + 21, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset + 24, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_global_mask_tcam_ofs + f1_offset + 24, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_global_mask_tcam_ofs + f1_offset + 24, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset + 34, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_global_mask_tcam_ofs + f1_offset + 34, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_global_mask_tcam_ofs + f1_offset + 34, 10);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset + 16, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset + 28, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset + 29, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset + 32, 2);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_global_mask_tcam_ofs + f1_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_global_mask_tcam_ofs + f1_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_global_mask_tcam_ofs + f1_offset + 16, 16);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 0, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 3, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 8, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 9, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 13, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 14, 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 20, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 26, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 26, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 38, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 39, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 42, 2);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset + 16, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset + 18, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset + 20, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset + 22, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset + 25, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset + 33, 16);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_global_mask_tcam_ofs + f1_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_global_mask_tcam_ofs + f1_offset + 13, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 0,
                                fp_global_mask_tcam_ofs + f1_offset + 16,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan,
                               fp_global_mask_tcam_ofs + f1_offset + 26,
                               13);


    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan,
                               fp_global_mask_tcam_ofs + f1_offset + 26,
                               13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_global_mask_tcam_ofs + f1_offset + 40, 1
                 );

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData2,
                 _bcmFieldSliceSelFpf1, 9,
                 fp_global_mask_tcam_ofs + f1_offset, 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
                 _bcmFieldSliceSelFpf1, 9,
                 fp_global_mask_tcam_ofs + f1_offset + 36 , 13);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf1, 10,
                 fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf1, 10,
                 fp_global_mask_tcam_ofs + f1_offset + 21, 16);



    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_global_mask_tcam_ofs + f1_offset, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_global_mask_tcam_ofs + f1_offset + 7, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                       _bcmFieldSliceSelFpf1, 11,
                       fp_global_mask_tcam_ofs + f1_offset + 32, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                       _bcmFieldSliceSelFpf1, 11,
                       fp_global_mask_tcam_ofs + f1_offset + 36, 2);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 12,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset + 12,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_global_mask_tcam_ofs + f1_offset + 12,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_global_mask_tcam_ofs + f1_offset + 12,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset + 38,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_global_mask_tcam_ofs + f1_offset + 38,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_global_mask_tcam_ofs + f1_offset + 38,
                               10);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf1, 13,
                 fp_global_mask_tcam_ofs + f1_offset + 37, 4);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf1, 13,
                 fp_global_mask_tcam_ofs + f1_offset + 41, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyFabricQueueTag,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagFabricQueue,
                               fp_global_mask_tcam_ofs + f1_offset,
                               33);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashUpper,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagRtag7A,
                               fp_global_mask_tcam_ofs + f1_offset + 16, 17);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashUpper,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagRtag7B,
                               fp_global_mask_tcam_ofs + f1_offset + 16, 17);
                                          
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashLower,
                     _bcmFieldDevSelDisable, 0, 
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagRtag7A, 
                     0,
                     fp_global_mask_tcam_ofs + f1_offset, 16,  
                     fp_global_mask_tcam_ofs + f1_offset + 32, 1,
                     0, 0, 
                     0); 

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagRtag7B,
                     0,
                     fp_global_mask_tcam_ofs + f1_offset, 16,
                     fp_global_mask_tcam_ofs + f1_offset + 32, 1,
                     0, 0,
                     0); 
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
                 _bcmFieldSliceSelFpf1, 14,
                 fp_global_mask_tcam_ofs + f1_offset + 48, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf1, 14, 
                 fp_global_mask_tcam_ofs + f1_offset, 48
                 );


    /* FPF2  single wide */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 64, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 96, 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 1, f2_offset + 64, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 1, f2_offset + 96, 32);

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
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 32, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 80, 48);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 8, f2_offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                 _bcmFieldSliceSelFpf2, 9, f2_offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 11, f2_offset + 32, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 11, f2_offset + 80, 48);


/* FPF 3 */
/* Present in Global Mask Field */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan,
                               fp_global_mask_tcam_ofs + f3_offset + 20, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 1,
                 fp_global_mask_tcam_ofs + f3_offset + 23, 12);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_global_mask_tcam_ofs + f3_offset + 35, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_global_mask_tcam_ofs + f3_offset + 35, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_global_mask_tcam_ofs + f3_offset + 35, 10);


    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunkMemberGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 28, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 28, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGport,
                               _bcmFieldSliceSelFpf3, 2,
                                _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 2,
                 fp_global_mask_tcam_ofs + f3_offset + 37, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 18, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 20, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 22, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 25, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 33, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 28, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 29, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 32, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 36, 2);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 0, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 3, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 8, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 9, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 13, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 14, 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 20, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 26, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 26, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 38, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 39, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf3, 7,
                 fp_global_mask_tcam_ofs + f3_offset, 24);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, _bcmFieldQualifyRangeCheckBits24_31,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 3,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 0,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 1,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 2,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 12);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_global_mask_tcam_ofs + f3_offset + 20, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_global_mask_tcam_ofs + f3_offset + 22, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_global_mask_tcam_ofs + f3_offset + 24, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_global_mask_tcam_ofs + f3_offset + 26, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_global_mask_tcam_ofs + f3_offset + 28, 2);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData3,
                 _bcmFieldSliceSelFpf3, 9,
                 fp_global_mask_tcam_ofs + f3_offset, 32);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf3, 10,
                 fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf3, 10,
                 fp_global_mask_tcam_ofs + f3_offset + 21, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_global_mask_tcam_ofs + f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_global_mask_tcam_ofs + f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_global_mask_tcam_ofs + f3_offset, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_global_mask_tcam_ofs + f3_offset + 7, 8);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                                 _bcmFieldSliceSelFpf3, 12,
                                 fp_global_mask_tcam_ofs + f3_offset + 41, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                                 _bcmFieldSliceSelFpf3, 12,
                                 fp_global_mask_tcam_ofs + f3_offset + 37, 4);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyFabricQueueTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagFabricQueue,
                               fp_global_mask_tcam_ofs + f3_offset, 33);
                                                                                
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashUpper,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagRtag7A,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 17);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashUpper,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagRtag7B,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 17);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagRtag7A,
                     0,
                     fp_global_mask_tcam_ofs + f3_offset, 16,
                     fp_global_mask_tcam_ofs + f3_offset + 32, 1,
                     0, 0,
                     0);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagRtag7B,
                     0,
                     fp_global_mask_tcam_ofs + f3_offset, 16,
                     fp_global_mask_tcam_ofs + f3_offset + 32, 1,
                     0, 0,
                     0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_global_mask_tcam_ofs + f3_offset + 34, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
                 _bcmFieldSliceSelFpf3, 14,
                 fp_global_mask_tcam_ofs + f3_offset + 48, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf3, 14, 
                 fp_global_mask_tcam_ofs + f3_offset, 48
                 );


    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 15,
                 fp_global_mask_tcam_ofs + f3_offset + 37, 12);



    /* FIXED single wide */
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
                 _bcmFieldSliceSelDisable, 0, fixed_offset, 1);
        _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 1, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 2, 3);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, _bcmFieldQualifySvpValid,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 5, 1);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyLoopback,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 0,
                               fixed_offset + 17, 1);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyLoopbackType,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 0,
                               fixed_offset + 13, 5);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 18, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 21, 1);

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_sc960_lookup_qualifiers_init
 * Purpose:
 *     Initialize device stage lookup qaualifiers
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.

 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_sc960_lookup_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    const unsigned f1_offset = 164, f2_offset = 36, f3_offset = 0;
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

    /* FPF1 */
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
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                     0,
                     f1_offset + 36, 7, /* Port value in SGLP */
                     f1_offset + 43, 8, /* Module value in SGLP */
                     f1_offset + 51, 1, /* Trunk bit in SGLP (should be 0) */
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                     0,
                     f1_offset + 36, 15, /* trunk id field of SGLP */
                     f1_offset + 51, 1,  /* trunk bit of SGLP */
                     0, 0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                     0,
                     f1_offset + 36, 7, /* Port value in SGLP */
                     f1_offset + 43, 8, /* Module value in SGLP */
                     f1_offset + 51, 1, /* Trunk bit in SGLP (should be 0) */
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                     0,
                     f1_offset + 36, 15, /* mod + port field of unresolved SGLP */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                     0,
                     f1_offset + 36 + 7, 8, /* mod field of unresolved SGLP */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                     0,
                     f1_offset + 36 + 7, 8, /* mod field of unresolved SGLP */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityPortGroupNum,
                     0,
                     f1_offset + 36, 7, /* ingress port field */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityPortGroupNum,
                     0,
                     f1_offset + 36 + 7, 8, /* ingress port group field */
                     0, 0,
                     0, 0,
                     0
                     );


    /* FPF2 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 40,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 40,
                               16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 64,32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 96,32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 3, f2_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 3, f2_offset + 32, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 3, f2_offset + 80, 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 16, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 48, 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 80, 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 5, f2_offset + 16, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 5, f2_offset + 48, 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5, f2_offset + 80, 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLlc,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 104, 24);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 8, f2_offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                 _bcmFieldSliceSelFpf2, 9, f2_offset, 128);

    /* FPF3 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 0, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 16, 16);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 16,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 16,
                               16);

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

    /* DWF2 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 40,
                            16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 40,
                            16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                            _bcmFieldSliceSelFpf2, 2,
                            _bcmFieldSliceSelDisable, 0,
                            f2_offset, 128);

    /* DWF1 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_offset + 24,
                            16);

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_sc960_egress_qualifiers_init
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
_field_sc960_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_DECL;
    _key_fld_ = KEYf;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageEgress,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

    /* EG_NEXT_HOP_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 18, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 22, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 24, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 26, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 42, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 90, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 138, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 139, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 140, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 141, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 141, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 142, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 145, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 157, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 165, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 166, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 166, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 178, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 179, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 182, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 184, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 185, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 1,
                               193, 7);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                     _bcmFieldSliceSelEgrClassF4, 2,
                     0,
                     193, 6, /* CpuCos - 6 bits */
                     16, 2,  /* Original - Data and Mask bits[5:4] */
                     0, 0,
                     0
                     );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    /* MDL */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOamMdl,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrMdlKey4, 1,
                               206, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 211, 12);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 224, 2);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 226, 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 1, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 135, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 136, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 137, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 149, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 157, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 158, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 158, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 170, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 171, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 174, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 176, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 177, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 185, 7);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
   _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 210, 12);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 223, 2);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 225, 1);

    /* KEY1 without v4 specific fields. */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 1, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 135, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 136, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 137, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 149, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 157, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 158, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 158, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 170, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 171, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 174, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 176, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 177, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 185, 7);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
   _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 210, 12);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 145, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 146, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 147, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 159, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 167, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 168, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 168, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 180, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 181, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 184, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 186, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 187, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 3,
                               195, 7
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
   _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 213, 12);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 226, 2);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 228, 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 1, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 216, 1);

#if 0
/* FCOE EFP_KEY5 support is not enabled in SDK.
Uncomment this block when support is added */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 126, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 127, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 128, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 140, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 148, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 149, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 149, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 161, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 162, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 165, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 167, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 168, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 176, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 183, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
   _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 198, 2);
#endif
 
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_sc960_qualifiers_init
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
_field_sc960_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_qual_arr,
                   (_bcmFieldQualifyCount * sizeof(_bcm_field_qual_info_t *)),
                   "Field qualifiers");
    if (stage_fc->f_qual_arr == 0) {
        return (BCM_E_MEMORY);
    }

    switch (stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_LOOKUP:
            return (_field_sc960_lookup_qualifiers_init(unit, stage_fc));
        case _BCM_FIELD_STAGE_INGRESS:
            return (_field_sc960_ingress_qualifiers_init(unit, stage_fc));
        case _BCM_FIELD_STAGE_EGRESS:
            return (_field_sc960_egress_qualifiers_init(unit, stage_fc));
        default:
            ;
    }

    sal_free(stage_fc->f_qual_arr);
    return (BCM_E_INTERNAL);
}

/*
 * Function:
 *     _field_td2_ingress_qualifiers_init
 * Purpose:
 *     Initialize device stage ingress qualifiers
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.
 *
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_td2_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    const unsigned
        f2_offset = 10 - 2,
        fixed_offset = 138 - 2,
        fixed_pairing_overlay_offset = fixed_offset,
        fp_global_mask_tcam_ofs = ALIGN32(soc_mem_field_length(unit, FP_TCAMm, KEYf)),
        f3_offset = 2 - 2,
        f1_offset = 51 - 2,
        ipbm_pairing_f0_offset = 100 - 2;

    _FP_QUAL_DECL;

    /* Handled outside normal qualifier set/get scheme */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStage,
                 _bcmFieldSliceSelDisable, 0,
                 0, 0
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageIngress,
                 _bcmFieldSliceSelDisable, 0,
                 0, 0
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelDisable, 0,
                 0, 0
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelDisable, 0,
                 0, 0
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelDisable, 0,
                 0, 0
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPorts,
                 _bcmFieldSliceSelDisable, 0,
                 0, 0
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyNormalizeIpAddrs,
                 _bcmFieldSliceSelDisable, 0,
                 0, 0
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyNormalizeMacAddrs,
                 _bcmFieldSliceSelDisable, 0,
                 0, 0
                 );

/* FPF1 single wide */
/* Present in Global Mask Field */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 1,
                               fp_global_mask_tcam_ofs + f1_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_global_mask_tcam_ofs + f1_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_global_mask_tcam_ofs + f1_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset + 10, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 1,
                               fp_global_mask_tcam_ofs + f1_offset + 10, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_global_mask_tcam_ofs + f1_offset + 10, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_global_mask_tcam_ofs + f1_offset + 10, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVrf,
                               fp_global_mask_tcam_ofs + f1_offset + 20, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVpn,
                               fp_global_mask_tcam_ofs + f1_offset + 20, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan,
                               fp_global_mask_tcam_ofs + f1_offset + 20, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGports,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcNivGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcNivGports,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGports,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGports,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGports,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroups,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMultipath,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcNivGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcNivGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunkMemberGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f1_offset + 28, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f1_offset + 28, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 1,
                 fp_global_mask_tcam_ofs + f1_offset + 37, 3);



    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGports,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGports,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGports,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGports,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGports,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMulticastGroups,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMultipath,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 2,
                 fp_global_mask_tcam_ofs + f1_offset + 21, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset + 24, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 1,
                               fp_global_mask_tcam_ofs + f1_offset + 24, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_global_mask_tcam_ofs + f1_offset + 24, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_global_mask_tcam_ofs + f1_offset + 24, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset + 34, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 1,
                               fp_global_mask_tcam_ofs + f1_offset + 34, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_global_mask_tcam_ofs + f1_offset + 34, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_global_mask_tcam_ofs + f1_offset + 34, 10);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset + 16, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset + 28, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset + 29, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset + 32, 2);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_global_mask_tcam_ofs + f1_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_global_mask_tcam_ofs + f1_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_global_mask_tcam_ofs + f1_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_global_mask_tcam_ofs + f1_offset + 32, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_global_mask_tcam_ofs + f1_offset + 35, 3);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 0, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 3, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 8, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 9, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 10, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 10, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 11, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 13, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 14, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 15, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillEgressRbridgeHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 17, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillIngressRbridgeHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 17, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 17, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 17, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 18, 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 20, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 26, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 26, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 38, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 39, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 42, 2);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset + 16, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset + 18, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset + 20, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset + 22, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset + 25, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset + 31, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset + 33, 16);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_global_mask_tcam_ofs + f1_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_global_mask_tcam_ofs + f1_offset + 13, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 0,
                                fp_global_mask_tcam_ofs + f1_offset + 16,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 1,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVrf,
                               fp_global_mask_tcam_ofs + f1_offset + 26,
                               13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVpn,
                               fp_global_mask_tcam_ofs + f1_offset + 26,
                               13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan,
                               fp_global_mask_tcam_ofs + f1_offset + 26,
                               13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyGenericAssociatedChannelLabelValid,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_global_mask_tcam_ofs + f1_offset + 39, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRouterAlertLabelValid,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_global_mask_tcam_ofs + f1_offset + 40, 1);

#if 0
/* TBD.. NO MPLS HERE. But RAL,GAL */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsOuterLabelPop,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_global_mask_tcam_ofs + f1_offset + 39, 2);
    _FP_QUAL_ADD(unit, stage_fc,
                 bcmFieldQualifyMplsStationHitTunnelUnterminated,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_global_mask_tcam_ofs + f1_offset + 39, 2);
#endif


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_global_mask_tcam_ofs + f1_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceTosClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceTosClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceTosClassSelect, 1,
                 fp_global_mask_tcam_ofs + f1_offset + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 1,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVrf,
                               fp_global_mask_tcam_ofs + f1_offset + 26,
                               13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVpn,
                               fp_global_mask_tcam_ofs + f1_offset + 26,
                               13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan,
                               fp_global_mask_tcam_ofs + f1_offset + 26,
                               13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpAddrsNormalized,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_global_mask_tcam_ofs + f1_offset + 39, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_global_mask_tcam_ofs + f1_offset + 40, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyNatSrcRealmId,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_global_mask_tcam_ofs + f1_offset + 41, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyNatDstRealmId,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_global_mask_tcam_ofs + f1_offset + 43, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyNatNeeded,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_global_mask_tcam_ofs + f1_offset + 45, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpError,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_global_mask_tcam_ofs + f1_offset + 46, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIpLocal,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_global_mask_tcam_ofs + f1_offset + 47, 1);


    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData2,
                 _bcmFieldSliceSelFpf1, 9,
                 fp_global_mask_tcam_ofs + f1_offset, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyGenericAssociatedChannelLabelValid,
                 _bcmFieldSliceSelFpf1, 9,
                 fp_global_mask_tcam_ofs + f1_offset + 32, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRouterAlertLabelValid,
                 _bcmFieldSliceSelFpf1, 9,
                 fp_global_mask_tcam_ofs + f1_offset + 33, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
                 _bcmFieldSliceSelFpf1, 9,
                 fp_global_mask_tcam_ofs + f1_offset + 36 , 13);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMplsGports,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstNivGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstNivGports,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMimGports,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstVxlanGports,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstVlanGports,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);


    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMulticastGroups,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMultipath,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf1, 10,
                 fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf1, 10,
                 fp_global_mask_tcam_ofs + f1_offset + 21, 16);



    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_global_mask_tcam_ofs + f1_offset, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGports,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_global_mask_tcam_ofs + f1_offset, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_global_mask_tcam_ofs + f1_offset + 7, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf1, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcMplsGports,
                               _bcmFieldSliceSelFpf1, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcNivGport,
                               _bcmFieldSliceSelFpf1, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcNivGports,
                               _bcmFieldSliceSelFpf1, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGports,
                               _bcmFieldSliceSelFpf1, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGport,
                               _bcmFieldSliceSelFpf1, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGports,
                               _bcmFieldSliceSelFpf1, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf1, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGports,
                               _bcmFieldSliceSelFpf1, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset + 16,
                               16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                       _bcmFieldSliceSelFpf1, 11,
                       fp_global_mask_tcam_ofs + f1_offset + 32, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                       _bcmFieldSliceSelFpf1, 11,
                       fp_global_mask_tcam_ofs + f1_offset + 36, 2);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 12,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset + 12,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 1,
                               fp_global_mask_tcam_ofs + f1_offset + 12,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_global_mask_tcam_ofs + f1_offset + 12,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_global_mask_tcam_ofs + f1_offset + 12,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset + 22,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcMplsGports,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset + 22,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcNivGport,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset + 22,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcNivGports,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset + 22,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset + 22,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGports,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset + 22,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGport,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset + 22,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGports,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset + 22,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset + 22,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGports,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset + 22,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset + 38,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 1,
                               fp_global_mask_tcam_ofs + f1_offset + 38,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_global_mask_tcam_ofs + f1_offset + 38,
                               10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_global_mask_tcam_ofs + f1_offset + 38,
                               10);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWordValid,
                 _bcmFieldSliceSelFpf1, 13,
                 fp_global_mask_tcam_ofs + f1_offset + 36, 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf1, 13,
                 fp_global_mask_tcam_ofs + f1_offset + 37, 4);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf1, 13,
                 fp_global_mask_tcam_ofs + f1_offset + 41, 2);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagVn,
                               fp_global_mask_tcam_ofs + f1_offset, 33);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCnTag,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagCn,
                               fp_global_mask_tcam_ofs + f1_offset, 33);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyFabricQueueTag,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagFabricQueue,
                               fp_global_mask_tcam_ofs + f1_offset,
                               33);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabel,
                               _bcmFieldSliceSelFpf1, 13,                       
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                               fp_global_mask_tcam_ofs + f1_offset, 33          
                               );                                               
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelTtl,     
                     _bcmFieldDevSelDisable, 0,                                 
                     _bcmFieldSliceSelFpf1, 13,                                 
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,                                                         
                     fp_global_mask_tcam_ofs + f1_offset,      8,               
                     fp_global_mask_tcam_ofs + f1_offset + 32, 1,               
                     0,                         0,                              
                     0                                                          
                     );                                                         
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelBos,     
                     _bcmFieldDevSelDisable, 0,                                 
                     _bcmFieldSliceSelFpf1, 13,                                 
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,                                                         
                     fp_global_mask_tcam_ofs + f1_offset + 8,  1,               
                     fp_global_mask_tcam_ofs + f1_offset + 32, 1,               
                     0,                         0,                              
                     0                                                          
                     );                                                         
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelExp,     
                     _bcmFieldDevSelDisable, 0,                                 
                     _bcmFieldSliceSelFpf1, 13,                                 
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,                                                         
                     fp_global_mask_tcam_ofs + f1_offset + 9,  3,               
                     fp_global_mask_tcam_ofs + f1_offset + 32, 1,               
                     0,                         0,                              
                     0                                                          
                     );                                                         
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelId,
                               _bcmFieldSliceSelFpf1, 13,                       
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                               fp_global_mask_tcam_ofs + f1_offset + 12, 21     
                               );                                               
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWord,  
                               _bcmFieldSliceSelFpf1, 13,                       
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsCntlWord,
                               fp_global_mask_tcam_ofs + f1_offset, 33          
                               );                                               
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanHeaderBits8_31,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagVxlanReserved,
                               fp_global_mask_tcam_ofs + f1_offset, 24);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanHeaderBits56_63,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagVxlanReserved,
                               fp_global_mask_tcam_ofs + f1_offset + 24, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanNetworkId,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagVxlanOrLLTag,
                               fp_global_mask_tcam_ofs + f1_offset, 24);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanFlags,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagVxlanOrLLTag,
                               fp_global_mask_tcam_ofs + f1_offset + 24, 8);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashUpper,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagRtag7A,
                               fp_global_mask_tcam_ofs + f1_offset + 16, 17);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashUpper,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagRtag7B,
                               fp_global_mask_tcam_ofs + f1_offset + 16, 17);
                                          
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashLower,
                     _bcmFieldDevSelDisable, 0, 
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagRtag7A, 
                     0,
                     fp_global_mask_tcam_ofs + f1_offset, 16,  
                     fp_global_mask_tcam_ofs + f1_offset + 32, 1,
                     0, 0, 
                     0); 

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagRtag7B,
                     0,
                     fp_global_mask_tcam_ofs + f1_offset, 16,
                     fp_global_mask_tcam_ofs + f1_offset + 32, 1,
                     0, 0,
                     0); 
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelAction,      
                 _bcmFieldSliceSelFpf1, 13,                                     
                 fp_global_mask_tcam_ofs + f1_offset + 33, 3                    
                 );    
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
                 _bcmFieldSliceSelFpf1, 14,
                 fp_global_mask_tcam_ofs + f1_offset + 48, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf1, 14, 
                 fp_global_mask_tcam_ofs + f1_offset, 48
                 );

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf1, 15,
                 fp_global_mask_tcam_ofs + f1_offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf1, 15,
                 fp_global_mask_tcam_ofs + f1_offset + 8, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf1, 15,
                 fp_global_mask_tcam_ofs + f1_offset + 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf1, 15,
                 fp_global_mask_tcam_ofs + f1_offset + 24, 16);
    /* L4_NORMALIZED */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf1, 15,
                               _bcmFieldSliceTcpClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset + 41, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                               _bcmFieldSliceSelFpf1, 15,
                               _bcmFieldSliceTcpClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset + 41, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                               _bcmFieldSliceSelFpf1, 15,
                               _bcmFieldSliceTcpClassSelect, 1,
                 fp_global_mask_tcam_ofs + f1_offset + 41, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf1, 15,
                 fp_global_mask_tcam_ofs + f1_offset + 47, 2);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeVersionIsZero,
                 _bcmFieldSliceSelFpf1, 16,
                 fp_global_mask_tcam_ofs + f1_offset, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeSOF,
                 _bcmFieldSliceSelFpf1, 16,
                 fp_global_mask_tcam_ofs + f1_offset + 1, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanRCtl,
                 _bcmFieldSliceSelFpf1, 16,
                 fp_global_mask_tcam_ofs + f1_offset + 15, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVersion,
                 _bcmFieldSliceSelFpf1, 16,
                 fp_global_mask_tcam_ofs + f1_offset + 23, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTHopCount,
                 _bcmFieldSliceSelFpf1, 16,
                 fp_global_mask_tcam_ofs + f1_offset + 25, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVsanId,
                 _bcmFieldSliceSelFpf1, 16,
                 fp_global_mask_tcam_ofs + f1_offset + 30, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVsanPri,
                 _bcmFieldSliceSelFpf1, 16,
                 fp_global_mask_tcam_ofs + f1_offset + 42, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcFpmaCheck,
                 _bcmFieldSliceSelFpf1, 16,
                 fp_global_mask_tcam_ofs + f1_offset + 45, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcBindCheck,
                 _bcmFieldSliceSelFpf1, 16,
                 fp_global_mask_tcam_ofs + f1_offset + 46, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanZoneCheck,
                 _bcmFieldSliceSelFpf1, 16,
                 fp_global_mask_tcam_ofs + f1_offset + 47, 2);
    /* FPF2  single wide */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceTtlClassSelect, 0,
                                f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceTtlClassSelect, 0,
                                f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceTtlClassSelect, 1,
                                f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceTcpClassSelect, 0,
                                f2_offset + 8, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceTcpClassSelect, 0,
                                f2_offset + 8, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceTcpClassSelect, 1,
                                f2_offset + 8, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 14, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceTosClassSelect, 0,
                                f2_offset + 16, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceTosClassSelect, 0,
                                f2_offset + 16, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceTosClassSelect, 1,
                                f2_offset + 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 56, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 64, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 96, 32);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTtlClassSelect, 0,
                                f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTtlClassSelect, 0,
                                f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTtlClassSelect, 1,
                                f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTcpClassSelect, 0,
                                f2_offset + 8, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTcpClassSelect, 0,
                                f2_offset + 8, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTcpClassSelect, 1,
                                f2_offset + 8, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf2, 1, f2_offset + 14, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTosClassSelect, 0,
                                f2_offset + 16, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTosClassSelect, 0,
                                f2_offset + 16, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTosClassSelect, 1,
                                f2_offset + 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 1, f2_offset + 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 1, f2_offset + 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf2, 1, f2_offset + 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 1, f2_offset + 56, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 1, f2_offset + 64, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 1, f2_offset + 96, 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelFpf2, 2, f2_offset, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf2, 3, f2_offset, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceTtlClassSelect, 0,
                                f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceTtlClassSelect, 0,
                                f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceTtlClassSelect, 1,
                                f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceTcpClassSelect, 0,
                                f2_offset + 8, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceTcpClassSelect, 0,
                                f2_offset + 8, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceTcpClassSelect, 1,
                                f2_offset + 8, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 14, 20);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceTosClassSelect, 0,
                                f2_offset + 34, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceTosClassSelect, 0,
                                f2_offset + 34, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceTosClassSelect, 1,
                                f2_offset + 34, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 42, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 50, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 64, 64);

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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                               _bcmFieldSliceSelFpf2, 7,
                               _bcmFieldSliceTtlClassSelect, 0,
                                f2_offset + 32, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                               _bcmFieldSliceSelFpf2, 7,
                               _bcmFieldSliceTtlClassSelect, 0,
                                f2_offset + 32, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                               _bcmFieldSliceSelFpf2, 7,
                               _bcmFieldSliceTtlClassSelect, 1,
                                f2_offset + 32, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 40, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 48, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 80, 48);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 8, f2_offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                 _bcmFieldSliceSelFpf2, 9, f2_offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                 _bcmFieldSliceSelFpf2, 10, f2_offset, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 64, 64);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 11, f2_offset + 32, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 11, f2_offset + 80, 48);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDFCtl,
                 _bcmFieldSliceSelFpf2, 12, f2_offset + 32, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanCSCtl,
                 _bcmFieldSliceSelFpf2, 12, f2_offset + 40, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanFCtl,
                 _bcmFieldSliceSelFpf2, 12, f2_offset + 48, 24);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanType,
                 _bcmFieldSliceSelFpf2, 12, f2_offset + 72, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstId,
                 _bcmFieldSliceSelFpf2, 12, f2_offset + 80, 24);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcId,
                 _bcmFieldSliceSelFpf2, 12, f2_offset + 104, 24);
/* FPF 3 */
/* Present in Global Mask Field */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 1,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 1,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVrf,
                               fp_global_mask_tcam_ofs + f3_offset + 20, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVpn,
                               fp_global_mask_tcam_ofs + f3_offset + 20, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan,
                               fp_global_mask_tcam_ofs + f3_offset + 20, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcMplsGports,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcNivGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcNivGports,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGports,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGports,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGports,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 1,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVrf,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVpn,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 1,
                 fp_global_mask_tcam_ofs + f3_offset + 23, 12);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_global_mask_tcam_ofs + f3_offset + 35, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 1,
                               fp_global_mask_tcam_ofs + f3_offset + 35, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_global_mask_tcam_ofs + f3_offset + 35, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_global_mask_tcam_ofs + f3_offset + 35, 10);
#if 0
/* TOS_FN_LOW */
#endif


    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMplsGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstNivGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstNivGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMimGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstVxlanGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstVlanGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMulticastGroups,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMultipath,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcMplsGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcNivGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcNivGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunkMemberGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 28, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGport,
                               _bcmFieldSliceSelFpf3, 2,
                                _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGports,
                               _bcmFieldSliceSelFpf3, 2,
                                _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 28, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 2,
                 fp_global_mask_tcam_ofs + f3_offset + 37, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 18, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 20, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 22, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 25, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 31, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 33, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 28, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 29, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 32, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 36, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIpLocal,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 38, 1);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset + 32, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset + 35, 3);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 0, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 3, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 8, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 9, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 10, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 10, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 11, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 13, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 14, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 15, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillEgressRbridgeHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 17, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillIngressRbridgeHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 17, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 17, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 17, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 18, 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 20, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 26, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 26, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 38, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 39, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf3, 7,
                 fp_global_mask_tcam_ofs + f3_offset, 24);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, _bcmFieldQualifyRangeCheckBits24_31,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 3,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 0,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 1,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 2,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 4,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 12);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_global_mask_tcam_ofs + f3_offset, 20);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_global_mask_tcam_ofs + f3_offset + 20, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_global_mask_tcam_ofs + f3_offset + 22, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_global_mask_tcam_ofs + f3_offset + 24, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_global_mask_tcam_ofs + f3_offset + 26, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_global_mask_tcam_ofs + f3_offset + 28, 2);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData3,
                 _bcmFieldSliceSelFpf3, 9,
                 fp_global_mask_tcam_ofs + f3_offset, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyGenericAssociatedChannelLabelValid,
                 _bcmFieldSliceSelFpf3, 9,
                 fp_global_mask_tcam_ofs + f3_offset + 32, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRouterAlertLabelValid,
                 _bcmFieldSliceSelFpf3, 9,
                 fp_global_mask_tcam_ofs + f3_offset + 33, 1);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGports,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGports,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGports,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGports,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGports,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroups,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMultipath,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf3, 10,
                 fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf3, 10,
                 fp_global_mask_tcam_ofs + f3_offset + 21, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_global_mask_tcam_ofs + f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_global_mask_tcam_ofs + f3_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_global_mask_tcam_ofs + f3_offset, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGports,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_global_mask_tcam_ofs + f3_offset, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_global_mask_tcam_ofs + f3_offset + 7, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGports,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcNivGport,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcNivGports,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGports,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGport,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGports,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGports,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWordValid,
                                 _bcmFieldSliceSelFpf3, 12,
                                 fp_global_mask_tcam_ofs + f3_offset + 36, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                                 _bcmFieldSliceSelFpf3, 12,
                                 fp_global_mask_tcam_ofs + f3_offset + 41, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                                 _bcmFieldSliceSelFpf3, 12,
                                 fp_global_mask_tcam_ofs + f3_offset + 37, 4);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagVn,
                               fp_global_mask_tcam_ofs + f3_offset, 33);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCnTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagCn,
                               fp_global_mask_tcam_ofs + f3_offset, 33);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyFabricQueueTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagFabricQueue,
                               fp_global_mask_tcam_ofs + f3_offset, 33);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabel,
                               _bcmFieldSliceSelFpf3, 12,                           
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                               fp_global_mask_tcam_ofs + f3_offset, 33               
                               );                                                   
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelTtl,         
                     _bcmFieldDevSelDisable, 0,                                     
                     _bcmFieldSliceSelFpf3, 12,                                     
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,                                                             
                     fp_global_mask_tcam_ofs + f3_offset,      8,                    
                     fp_global_mask_tcam_ofs + f3_offset + 32, 1,                    
                     0,                         0,                                  
                     0                                                              
                     );                                                             
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelBos,         
                     _bcmFieldDevSelDisable, 0,                                     
                     _bcmFieldSliceSelFpf3, 12,                                     
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,                                                             
                     fp_global_mask_tcam_ofs + f3_offset + 8,  1,                    
                     fp_global_mask_tcam_ofs + f3_offset + 32, 1,                    
                     0,                         0,                                  
                     0                                                              
                     );                                                             
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelExp,         
                     _bcmFieldDevSelDisable, 0,                                     
                     _bcmFieldSliceSelFpf3, 12,                                     
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,                                                             
                     fp_global_mask_tcam_ofs + f3_offset + 9,  3,               
                     fp_global_mask_tcam_ofs + f3_offset + 32, 1,               
                     0,                         0,                              
                     0                                                          
                     );                                                         
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelId,
                               _bcmFieldSliceSelFpf3, 12,                       
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                               fp_global_mask_tcam_ofs + f3_offset + 12, 21     
                               );                                               
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWord,  
                               _bcmFieldSliceSelFpf3, 12,                       
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsCntlWord,
                               fp_global_mask_tcam_ofs + f3_offset, 33          
                               );                                               
                                                                                
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashUpper,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagRtag7A,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 17);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashUpper,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagRtag7B,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 17);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagRtag7A,
                     0,
                     fp_global_mask_tcam_ofs + f3_offset, 16,
                     fp_global_mask_tcam_ofs + f3_offset + 32, 1,
                     0, 0,
                     0);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagRtag7B,
                     0,
                     fp_global_mask_tcam_ofs + f3_offset, 16,
                     fp_global_mask_tcam_ofs + f3_offset + 32, 1,
                     0, 0,
                     0);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanNetworkId,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagVxlanOrLLTag,
                               fp_global_mask_tcam_ofs + f3_offset, 24);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanFlags,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagVxlanOrLLTag,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 8);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanHeaderBits8_31,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagVxlanReserved,
                               fp_global_mask_tcam_ofs + f3_offset, 24);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanHeaderBits56_63,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagVxlanReserved,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 8);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelAction,      
                 _bcmFieldSliceSelFpf3, 12,                                     
                 fp_global_mask_tcam_ofs + f3_offset + 33, 3                    
                 );                                                             
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, 13,
                               _bcmFieldSliceTosClassSelect, 0,
                               fp_global_mask_tcam_ofs + f3_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                               _bcmFieldSliceSelFpf3, 13,
                               _bcmFieldSliceTosClassSelect, 0,
                               fp_global_mask_tcam_ofs + f3_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                               _bcmFieldSliceSelFpf3, 13,
                               _bcmFieldSliceTosClassSelect, 1,
                 fp_global_mask_tcam_ofs + f3_offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeader2Type,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_global_mask_tcam_ofs + f3_offset + 8, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_global_mask_tcam_ofs + f3_offset + 24, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIpLocal,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_global_mask_tcam_ofs + f3_offset + 32, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpAddrsNormalized,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_global_mask_tcam_ofs + f3_offset + 33, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_global_mask_tcam_ofs + f3_offset + 34, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_global_mask_tcam_ofs + f3_offset + 35, 13
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
                 _bcmFieldSliceSelFpf3, 14,
                 fp_global_mask_tcam_ofs + f3_offset + 48, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf3, 14, 
                 fp_global_mask_tcam_ofs + f3_offset, 48
                 );


    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroups,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMultipath,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcNivGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcNivGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 28, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 28, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 15,
                 fp_global_mask_tcam_ofs + f3_offset + 37, 12);



    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeVersionIsZero,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeSOF,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 1, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanRCtl,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 15, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVersion,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 23, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTHopCount,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 25, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVsanId,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 30, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVsanPri,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 42, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcFpmaCheck,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 45, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcBindCheck,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 46, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanZoneCheck,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 47, 2);
 


    /* FIXED single wide */
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
                 _bcmFieldSliceSelDisable, 0, fixed_offset, 1);
        _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 1, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 2, 3);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, _bcmFieldQualifySvpValid,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 5, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 5, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 6, 5);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 11, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 12, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyLoopback,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 17, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyLoopbackType,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 13, 5);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyTunnelType,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 13, 5);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 18, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyRepCopy,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 19, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 20, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelDisable, 0, fixed_offset + 21, 1);

    /* Overlay of FIXED key in pairing mode */
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     0,
                     0, 0,
                     fixed_pairing_overlay_offset, 2,
                     0, 0,
                     1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     0,
                     0, 0,
                     fixed_pairing_overlay_offset + 2, 8,
                     0, 0,
                     1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceTtlClassSelect, 0,
                     0,
                     0, 0,
                     fixed_pairing_overlay_offset + 10, 8,
                     0, 0,
                     1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceTtlClassSelect, 0,
                     0,
                     0, 0,
                     fixed_pairing_overlay_offset + 10, 8,
                     0, 0,
                     1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceTtlClassSelect, 1,
                     0,
                     0, 0,
                     fixed_pairing_overlay_offset + 10, 8,
                     0, 0,
                     1);

    /* Overlay of IPBM in pairing mode */
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceTcpClassSelect, 0,
                     0,
                     0, 0, 
                     fp_global_mask_tcam_ofs + ipbm_pairing_f0_offset, 6, 
                     0, 0,
                     1);  
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceTcpClassSelect, 0,
                     0,
                     0, 0, 
                     fp_global_mask_tcam_ofs + ipbm_pairing_f0_offset, 6, 
                     0, 0,
                     1);  
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceTcpClassSelect, 1,
                     0,
                     0, 0, 
                     fp_global_mask_tcam_ofs + ipbm_pairing_f0_offset, 6, 
                     0, 0,
                     1);  
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     0,
                     0, 0, 
                     fp_global_mask_tcam_ofs + ipbm_pairing_f0_offset + 6, 16,
                     0, 0, 
                     1);   
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     0,
                     0, 0,
                     fp_global_mask_tcam_ofs + ipbm_pairing_f0_offset + 22, 16,
                     0, 0,
                     1); 
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     0,
                     0, 0,
                     fp_global_mask_tcam_ofs + ipbm_pairing_f0_offset + 22, 16,
                     0, 0,
                     1); 

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_td2_lookup_qualifiers_init
 * Purpose:
 *     Initialize device stage lookup qaualifiers
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.

 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_td2_lookup_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    const unsigned f1_offset = 164, f2_offset = 36, f3_offset = 0;
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

    /* FPF1 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocolCommon,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f1_offset, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocolCommon,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f1_offset, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 3, 1);
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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpType,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f1_offset + 31,
                               5);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f1_offset + 31,
                               5);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                     0,
                     f1_offset + 36, 7, /* Port value in SGLP */
                     f1_offset + 43, 8, /* Module value in SGLP */
                     f1_offset + 51, 1, /* Trunk bit in SGLP (should be 0) */
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                     0,
                     f1_offset + 36, 15, /* trunk id field of SGLP */
                     f1_offset + 51, 1,  /* trunk bit of SGLP */
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                     0,
                     f1_offset + 36, 7, /* Port value in SGLP */
                     f1_offset + 43, 8, /* Module value in SGLP */
                     f1_offset + 51, 1, /* Trunk bit in SGLP (should be 0) */
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMplsGport,
                     0,
                     f1_offset + 36, 20, /* SVP_VALID + S_TYPE_SEL + S_FIELD */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGports,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMplsGport,
                     0,
                     f1_offset + 36, 20, /* SVP_VALID + S_TYPE_SEL + S_FIELD */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcNivGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityNivGport,
                     0,
                     f1_offset + 36, 20, /* SVP_VALID + S_TYPE_SEL + S_FIELD */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcNivGports,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityNivGport,
                     0,
                     f1_offset + 36, 20, /* SVP_VALID + S_TYPE_SEL + S_FIELD */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMimGport,
                     0,
                     f1_offset + 36, 20, /* SVP_VALID + S_TYPE_SEL + S_FIELD */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMimGports,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMimGport,
                     0,
                     f1_offset + 36, 20, /* SVP_VALID + S_TYPE_SEL + S_FIELD */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityVxlanGport,
                     0,
                     f1_offset + 36, 20, /* SVP_VALID + S_TYPE_SEL + S_FIELD */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGports,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityVxlanGport,
                     0,
                     f1_offset + 36, 20, /* SVP_VALID + S_TYPE_SEL + S_FIELD */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityVlanGport,
                     0,
                     f1_offset + 36, 20, /* SVP_VALID + S_TYPE_SEL + S_FIELD */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGports,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityVlanGport,
                     0,
                     f1_offset + 36, 20, /* SVP_VALID + S_TYPE_SEL + S_FIELD */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                     0,
                     f1_offset + 36, 15, /* mod + port field of unresolved SGLP */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGports,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                     0,
                     f1_offset + 36, 15, /* mod + port field of unresolved SGLP */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                     0,
                     f1_offset + 36 + 7, 8, /* mod field of unresolved SGLP */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityPortGroupNum,
                     0,
                     f1_offset + 36, 7, /* ingress port field */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityPortGroupNum,
                     0,
                     f1_offset + 36 + 7, 8, /* ingress port group field */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                     0,
                     f1_offset + 36 + 7, 8, /* mod field of unresolved SGLP */
                     0, 0,
                     0, 0,
                     0
                     );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 56, 1);


    /* FPF2 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTtl,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 8, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpFrag,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 14,
                               2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 14,
                               2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 16,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 16,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 24,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 24,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 40,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 40,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 40,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 56,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 56,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 64,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 64,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 96,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 96,
                               32);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp6,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 128);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp6,
                               _bcmFieldSliceSelFpf2, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                               _bcmFieldSliceSelFpf2, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 3, f2_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 3, f2_offset + 32, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 3, f2_offset + 80, 48);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 8,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 8,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 16,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 16,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 48,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 48,
                               32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 80, 48);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 8,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 8,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 16,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 16,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 48,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 48,
                               32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5, f2_offset + 80, 48);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 64,
                               64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 64,
                               64);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                 _bcmFieldSliceSelFpf2, 7, f2_offset, 33);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySnap,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 64, 40);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLlc,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 104, 24);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 8, f2_offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                 _bcmFieldSliceSelFpf2, 9, f2_offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeSOF,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 6, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDFCtl,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 14, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanCSCtl,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 22, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanFCtl,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 30, 24);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanType,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 54, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstId,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 62, 24);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcId,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 86, 24);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanRCtl,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 110, 8);

    /* FPF3 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 0, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 16, 16);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 0,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 0,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 16,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 16,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 16,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 0,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 0,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 8,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 8,
                               8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, 2, f3_offset + 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, 2, f3_offset + 24, 8);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 0, 8);
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

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTHopCount,
                 _bcmFieldSliceSelFpf3, 5, f3_offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVsanId,
                 _bcmFieldSliceSelFpf3, 5, f3_offset + 8, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVsanPri,
                 _bcmFieldSliceSelFpf3, 5, f3_offset + 20, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVersion,
                 _bcmFieldSliceSelFpf3, 5, f3_offset + 23, 2);

    /* DWF3 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 0, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 0, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTtl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 8, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 8, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 16,
                            6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpFrag,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 22,
                            2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 22,
                            2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 24,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 24,
                            8);

    /* DWF2 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTtl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 0,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 0,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 8,
                            6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpFrag,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 14,
                            2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 14,
                            2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 16,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 16,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 24,
                            16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 24,
                            16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 40,
                            16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 40,
                            16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 40,
                            16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 56,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 56,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 64,
                            32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 64,
                            32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 96,
                            32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 96,
                            32);

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset,
                            128);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset,
                            128);

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                            _bcmFieldSliceSelFpf2, 2,
                            _bcmFieldSliceSelDisable, 0,
                            f2_offset, 128);

    /* DWF1 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyExtensionHeaderSubCode,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_offset + 0,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_offset + 8,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_offset + 16,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_offset + 24,
                            16);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_td2_egress_qualifiers_init
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
_field_td2_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
    /* EG_DVP_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);
    /* DVP_VALID + DVP + SPARE_DVP */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGports,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGports,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGports,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGports,
                              _bcmFieldDevSelDisable, 0,
                              _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                              _bcmFieldSliceSelEgrDvpKey4, 0,
                              0,
                              1, 17,
                              227, 2, /* DVP TYPE */
                              0, 0,
                              0
                              );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGports,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                bcmFieldQualifyDestVirtualPortValid,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 18, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 22, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 24, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 26, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 42, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 90, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 138, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 139, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 140, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 141, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 141, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 142, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 145, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 157, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 164, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 165, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 166, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 166, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 178, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 179, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 182, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 184, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 185, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 1,
                               193, 7);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                     _bcmFieldSliceSelEgrClassF4, 2,
                     0,
                     193, 6, /* CpuCos - 6 bits */
                     16, 2,  /* Original - Data and Mask bits[5:4] */
                     0, 0,
                     0
                     );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               193, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               2, 12
                               );

    /* L3_PKT_TYPE */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrMdlKey4, 0,
                               206, 5);
    /* MDL */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOamMdl,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrMdlKey4, 1,
                               206, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 211, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 211, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 211, 13);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 224, 2);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 226, 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 1, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 7, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 9, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 15, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 31, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 31, 16);    
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 47, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 55, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 63, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 95, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 127, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 135, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 136, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 137, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 149, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 156, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 157, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 158, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 158, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 170, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 171, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 174, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 176, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 177, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 185, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 192, 5);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 12
                               );

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 210, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 210, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 210, 13);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 223, 2);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 225, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 0);


    /* KEY1 without v4 specific fields. */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 1, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 9, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 15, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 31, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 31, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 47, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 55, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 127, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 135, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 136, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 137, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 149, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 156, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 157, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 158, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 158, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 170, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 171, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 174, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 176, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 177, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 185, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 192, 5);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 210, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 210, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 210, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 223, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 225, 1);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 1, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP6,
                               9, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_DIP6,
                               9, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64,
                               9, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64,
                               73, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 137, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 145, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 146, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 147, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 159, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 166, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 167, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 168, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 168, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 180, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 181, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 184, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 186, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 187, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 3,
                               195, 7
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 12
                               );

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 208, 5);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 213, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 213, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 213, 13);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 226, 2);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 228, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 1, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 7, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 9, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 15, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 31, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 31, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 47, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 55, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 183, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 191, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 199, 5);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 2,
                               204, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 2,
                               204, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 2,
                               204, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 216, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 0);
#if 0
/* FCOE EFP_KEY5 support is not enabled in SDK.
Uncomment this block when support is added */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 126, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 127, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 128, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 140, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 147, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 148, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 149, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 149, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 161, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 162, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 165, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 167, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 168, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 176, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 183, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 3,
                               200, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 3,
                               200, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
   _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               200, 13
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 198, 2);
#endif
 
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_td2_ingress_selcodes_install
 *
 * Purpose:
 *     Writes the field select codes (ie. FPFx).
 *     for IFP (_BCM_FIELD_STAGE_INGRESS) lookup stage.
 *
 * Parameters:
 *     unit  - BCM device number
 *     fs    - slice that needs its select codes written
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_bcm_field_td2_ingress_selcodes_install(int            unit,
                                       _field_group_t *fg,
                                       uint8          slice_num,
                                       bcm_pbmp_t     *pbmp,
                                       int            selcode_idx
                                       )
{


    _field_sel_t * const          sel = &fg->sel_codes[selcode_idx];
    const unsigned                pfs_idx_count = soc_mem_index_count(unit, FP_PORT_FIELD_SELm);
    int                           errcode = BCM_E_NONE;
    bcm_port_t                    idx;
    uint32                        buf[SOC_MAX_MEM_FIELD_WORDS];
    SHR_BITDCL                    *pfs_bmp = 0;

    BCM_IF_ERROR_RETURN(_bcm_field_trx_ingress_selcodes_install(unit,
                fg,
                slice_num,
                pbmp,
                selcode_idx
                )
            );

    /* Get port field select table size and allocated bitmap of indexes
     * applicable to the group.
     */
    _FP_XGS3_ALLOC(pfs_bmp,
            MAX(SHR_BITALLOCSIZE(pfs_idx_count),
                sizeof(bcm_pbmp_t)
               ),
            "PFS bmp"
            );

    if (pfs_bmp == 0) {
        return (BCM_E_MEMORY);
    }

    /* Populate pfs indexes applicable for the group. */
    errcode = _bcm_field_trx_ingress_pfs_bmap_get(unit,
            fg,
            pbmp,
            selcode_idx,
            pfs_bmp
            );
    if (BCM_FAILURE(errcode)) {
        goto cleanup;
    }

    for (idx = 0; idx < pfs_idx_count; idx++) {
        static const soc_field_t fldtbl[][2] = {
            { SLICE0_NORMALIZE_IP_ADDRf,  SLICE0_NORMALIZE_MAC_ADDRf },
            { SLICE1_NORMALIZE_IP_ADDRf,  SLICE1_NORMALIZE_MAC_ADDRf },
            { SLICE2_NORMALIZE_IP_ADDRf,  SLICE2_NORMALIZE_MAC_ADDRf },
            { SLICE3_NORMALIZE_IP_ADDRf,  SLICE3_NORMALIZE_MAC_ADDRf },
            { SLICE4_NORMALIZE_IP_ADDRf,  SLICE4_NORMALIZE_MAC_ADDRf },
            { SLICE5_NORMALIZE_IP_ADDRf,  SLICE5_NORMALIZE_MAC_ADDRf },
            { SLICE6_NORMALIZE_IP_ADDRf,  SLICE6_NORMALIZE_MAC_ADDRf },
            { SLICE7_NORMALIZE_IP_ADDRf,  SLICE7_NORMALIZE_MAC_ADDRf },
            { SLICE8_NORMALIZE_IP_ADDRf,  SLICE8_NORMALIZE_MAC_ADDRf },
            { SLICE9_NORMALIZE_IP_ADDRf,  SLICE9_NORMALIZE_MAC_ADDRf },
            { SLICE10_NORMALIZE_IP_ADDRf, SLICE10_NORMALIZE_MAC_ADDRf },
            { SLICE11_NORMALIZE_IP_ADDRf, SLICE11_NORMALIZE_MAC_ADDRf }
        };

        if (0 == SHR_BITGET(pfs_bmp, idx)) {
            continue;
        }
        sal_memset(buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
        /* Read Port's current entry in FP_PORT_FIELD_SEL table */
        errcode = soc_mem_read(unit,
                FP_PORT_FIELD_SELm,
                MEM_BLOCK_ANY,
                idx,
                buf
                );
        if (BCM_FAILURE(errcode)) {
            goto cleanup;
        }

        if (sel->normalize_ip_sel != _FP_SELCODE_DONT_CARE) {
            soc_mem_field32_set(unit,
                    FP_PORT_FIELD_SELm,
                    buf,
                    fldtbl[slice_num][0],
                    sel->normalize_ip_sel
                    );
        }
        if (sel->normalize_mac_sel != _FP_SELCODE_DONT_CARE) {
            soc_mem_field32_set(unit,
                    FP_PORT_FIELD_SELm,
                    buf,
                    fldtbl[slice_num][1],
                    sel->normalize_mac_sel
                    );
        }

        /* Write each port's new entry */
        errcode = soc_mem_write(unit,
                FP_PORT_FIELD_SELm,
                MEM_BLOCK_ALL,
                idx,
                buf
                );
        if (BCM_FAILURE(errcode)) {
            goto cleanup;
        }
    }
cleanup:
    if (pfs_bmp) {
        sal_free(pfs_bmp);
    }
    return (errcode);


}

/*
 * Function:
 *     _bcm_field_td2_lookup_selcodes_install
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

int
_bcm_field_td2_lookup_selcodes_install(int            unit,
                                       _field_group_t *fg,
                                       uint8          slice_num,
                                       int            selcode_idx
                                       )
{
    static const soc_field_t s_type_fld_tbl[] = {
        SLICE_0_S_TYPE_SELf,
        SLICE_1_S_TYPE_SELf,
        SLICE_2_S_TYPE_SELf,
        SLICE_3_S_TYPE_SELf
    };

    _field_sel_t * const sel = &fg->sel_codes[selcode_idx];
    int           errcode = BCM_E_NONE;
    uint64        regval;
    uint64        val;

    BCM_IF_ERROR_RETURN(soc_reg64_get(unit,
                                      VFP_KEY_CONTROL_1r,
                                      REG_PORT_ANY,
                                      0,
                                      &regval
                                      )
                        );


    if ((fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)
        && (selcode_idx & 1)
        ) {
        if (sel->fpf2 != _FP_SELCODE_DONT_CARE) {
            COMPILER_64_SET(val, 0, sel->fpf2);
            soc_reg64_field_set(unit,
                                VFP_KEY_CONTROL_1r,
                                &regval,
                                _bcm_field_trx_vfp_double_wide_sel[slice_num],
                                val
                                );
        }
    } else {
        if (sel->fpf2 != _FP_SELCODE_DONT_CARE) {
            COMPILER_64_SET(val, 0, sel->fpf2);
            soc_reg64_field_set(unit,
                                VFP_KEY_CONTROL_1r,
                                &regval,
                                _bcm_field_trx_vfp_field_sel[slice_num][0],
                                val
                                );
        }
        if (sel->fpf3 != _FP_SELCODE_DONT_CARE) {
            COMPILER_64_SET(val, 0, sel->fpf3);
            soc_reg64_field_set(unit,
                                VFP_KEY_CONTROL_1r,
                                &regval,
                                _bcm_field_trx_vfp_field_sel[slice_num][1],
                                val
                                );
        }
    }


    if (sel->src_entity_sel != _FP_SELCODE_DONT_CARE) {
        uint32 value;


        switch (sel->src_entity_sel) {
            case _bcmFieldFwdEntityPortGroupNum:
                value = 4;
                break;
            case _bcmFieldFwdEntityMplsGport:
            case _bcmFieldFwdEntityNivGport:
            case _bcmFieldFwdEntityMimGport:
            case _bcmFieldFwdEntityWlanGport:
            case _bcmFieldFwdEntityVxlanGport:
            case _bcmFieldFwdEntityVlanGport:
                value = 0;
                break;
            case _bcmFieldFwdEntityModPortGport:
                value = 2;
                break;
            case _bcmFieldFwdEntityGlp:
                value = 1;
                break;
            default:
                return (BCM_E_INTERNAL);
        }
        COMPILER_64_SET(val, 0, value);
        soc_reg64_field_set(unit,
                            VFP_KEY_CONTROL_1r,
                            &regval,
                            s_type_fld_tbl[slice_num],
                            val
                            );
    }

    BCM_IF_ERROR_RETURN(soc_reg64_set(unit,
                                      VFP_KEY_CONTROL_1r,
                                      REG_PORT_ANY,
                                      0,
                                      regval
                                      )
                        );

    /* Set inner/outer ip header selection. */
    if (sel->ip_header_sel != _FP_SELCODE_DONT_CARE) {
        errcode = soc_reg_field32_modify(
                      unit,
                      VFP_KEY_CONTROL_2r,
                      REG_PORT_ANY,
                      _bcm_field_trx_vfp_ip_header_sel[slice_num],
                      sel->ip_header_sel
                      );
    }

    return (errcode);
}

STATIC int
_field_td2_egress_selcodes_install(int            unit,
                                   _field_group_t *fg,
                                   uint8          slice_num,
                                   bcm_pbmp_t     *pbmp,
                                   int            selcode_idx
                                   )
{
    static const soc_field_t fldtbl[][4] = {
        { SLICE_0_F1f, SLICE_1_F1f, SLICE_2_F1f, SLICE_3_F1f },
        { SLICE_0_F2f, SLICE_1_F2f, SLICE_2_F2f, SLICE_3_F2f },
        { SLICE_0_F4f, SLICE_1_F4f, SLICE_2_F4f, SLICE_3_F4f }
    };

    static const soc_field_t dvpfldtbl[4] = {
        SLICE_0f, SLICE_1f, SLICE_2f, SLICE_3f };
    
    static const soc_field_t mdlfldtbl[4] = {
        SLICE_0f, SLICE_1f, SLICE_2f, SLICE_3f };

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    static const soc_field_t oamoverlaytbl[4] = {
        SLICE_0_OAM_OVERLAY_ENABLEf,SLICE_1_OAM_OVERLAY_ENABLEf,
        SLICE_2_OAM_OVERLAY_ENABLEf, SLICE_3_OAM_OVERLAY_ENABLEf };

    static const soc_field_t efp_dest_port_selector_flds[][2] = {
             {SLICE_0_F1_DEST_SELf, SLICE_0_F5_DEST_SELf},
             {SLICE_1_F1_DEST_SELf, SLICE_1_F5_DEST_SELf},
             {SLICE_2_F1_DEST_SELf, SLICE_2_F5_DEST_SELf},
             {SLICE_3_F1_DEST_SELf, SLICE_3_F5_DEST_SELf},
        };
#endif

    _field_sel_t * const sel = &fg->sel_codes[selcode_idx];

    if (sel->egr_class_f1_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_CLASSID_SELECTORr,
                                                   REG_PORT_ANY,
                                                   fldtbl[0][slice_num],
                                                   sel->egr_class_f1_sel
                                                   )
                            );
    }
    if (sel->egr_class_f2_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_CLASSID_SELECTORr,
                                                   REG_PORT_ANY,
                                                   fldtbl[1][slice_num],
                                                   sel->egr_class_f2_sel
                                                   )
                            );
    }
    if (sel->egr_class_f4_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_CLASSID_SELECTORr,
                                                   REG_PORT_ANY,
                                                   fldtbl[2][slice_num],
                                                   sel->egr_class_f4_sel
                                                   )
                            );
    }
    /* SELECTOR CODES for EFP_KEY4_DVP_SELECTOR register */
    if (sel->egr_key4_dvp_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_KEY4_DVP_SELECTORr,
                                                   REG_PORT_ANY,
                                                   dvpfldtbl[slice_num],
                                                   sel->egr_key4_dvp_sel
                                                  )
                            );
    }
    /* SELECTOR CODES for EFP_KEY4_MDL_SELECTOR register */
    if (sel->egr_key4_mdl_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_KEY4_MDL_SELECTORr,
                                                   REG_PORT_ANY,
                                                   mdlfldtbl[slice_num],
                                                   sel->egr_key4_mdl_sel
                                                  )
                            );
    }

    /* SELECTOR CODES for EFP_KEY4_MDL_SELECTOR register
     * for field  OAM_OVERLAY_ENABLE
     */
#if defined BCM_TRIDENT2PLUS_SUPPORT
    if (soc_feature(unit, soc_feature_fp_based_oam)) {
        if (sel->egr_oam_overlay_sel != _FP_SELCODE_DONT_CARE) {
            BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_KEY4_MDL_SELECTORr,
                                                   REG_PORT_ANY,
                                                   oamoverlaytbl[slice_num],
                                                   sel->egr_oam_overlay_sel
                                                )
                            );

        }
    }

    if (SOC_REG_IS_VALID(unit, EFP_DEST_PORT_SELECTORr)) {
        if (sel->egr_dest_port_f1_sel != _FP_SELCODE_DONT_CARE) {
            BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                        EFP_DEST_PORT_SELECTORr,
                        REG_PORT_ANY,
                        efp_dest_port_selector_flds[slice_num][0],
                        sel->egr_dest_port_f1_sel
                        )
                    );
        }
        if (sel->egr_dest_port_f5_sel != _FP_SELCODE_DONT_CARE) {
            BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                        EFP_DEST_PORT_SELECTORr,
                        REG_PORT_ANY,
                        efp_dest_port_selector_flds[slice_num][1],
                        sel->egr_dest_port_f5_sel
                        )
                    );
        }
    }
#endif
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_td2_lookup_mode_set
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
_bcm_field_td2_lookup_mode_set(int            unit,
                               uint8          slice_num,
                               _field_group_t *fg,
                               uint8          flags
                               )
{
    static const soc_field_t wide_mode_flds[] = {
        SLICE_0_DOUBLE_WIDE_MODEf,
        SLICE_1_DOUBLE_WIDE_MODEf,
        SLICE_2_DOUBLE_WIDE_MODEf,
        SLICE_3_DOUBLE_WIDE_MODEf
    }, pairing_flds[] = {
        SLICE1_0_PAIRINGf,
        SLICE3_2_PAIRINGf
    };

    uint64 vfp_key_control_1_buf;

    BCM_IF_ERROR_RETURN(soc_reg64_get(unit,
                                      VFP_KEY_CONTROL_1r,
                                      REG_PORT_ANY,
                                      0,
                                      &vfp_key_control_1_buf
                                      )
                        );

    soc_reg64_field32_set(unit,
                          VFP_KEY_CONTROL_1r,
                          &vfp_key_control_1_buf,
                          pairing_flds[slice_num >> 1],
                          flags & _FP_GROUP_SPAN_DOUBLE_SLICE ? 1 : 0
                          );
    soc_reg64_field32_set(unit,
                          VFP_KEY_CONTROL_1r,
                          &vfp_key_control_1_buf,
                          wide_mode_flds[slice_num],
                          flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE ? 1 : 0
                          );


    return (soc_reg64_set(unit,
                          VFP_KEY_CONTROL_1r,
                          REG_PORT_ANY,
                          0,
                          vfp_key_control_1_buf
                          )
            );
}

/*
 * Function:
 *     _field_td2_mode_set
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

STATIC int
_field_td2_mode_set(int unit, uint8 slice_num, _field_group_t *fg, uint8 flags)
{
    int (*func)(int unit, uint8 slice_num, _field_group_t *fg, uint8 flags);

    switch (fg->stage_id) {
        case _BCM_FIELD_STAGE_LOOKUP:
            func = _bcm_field_td2_lookup_mode_set;
            break;
        case _BCM_FIELD_STAGE_INGRESS:
            return (BCM_E_NONE);
        case _BCM_FIELD_STAGE_EGRESS:
            func = _bcm_field_trx_egress_mode_set;
            break;
        default:
            return (BCM_E_INTERNAL);
    }

    return ((*func)(unit, slice_num, fg, flags));
}


STATIC int
_field_td2_ingress_selcode_get(int              unit,
                              _field_stage_t   *stage_fc,
                              bcm_field_qset_t *qset_req,
                              _field_group_t   *fg)
{
    if (BCM_FIELD_QSET_TEST(*qset_req, bcmFieldQualifySrcMac)
            || BCM_FIELD_QSET_TEST(*qset_req, bcmFieldQualifyDstMac)
       ) {
        /* Qset includes MAC address qualifier
           => Set selector code for MAC address normalization
         */
        fg->sel_codes[0].normalize_mac_sel
            = BCM_FIELD_QSET_TEST(*qset_req, bcmFieldQualifyNormalizeMacAddrs)
            ? 1 : 0;
    }

    if (BCM_FIELD_QSET_TEST(*qset_req, bcmFieldQualifySrcIp)
            || BCM_FIELD_QSET_TEST(*qset_req, bcmFieldQualifyDstIp)
            || BCM_FIELD_QSET_TEST(*qset_req, bcmFieldQualifySrcIp6)
            || BCM_FIELD_QSET_TEST(*qset_req, bcmFieldQualifyDstIp6)
            || BCM_FIELD_QSET_TEST(*qset_req, bcmFieldQualifyL4SrcPort)
            || BCM_FIELD_QSET_TEST(*qset_req, bcmFieldQualifyL4DstPort)
       ) {
        /* Qset includes L3 or L4 address qualifier
           => Set selector code for IP (and L4) address normalization
         */
        fg->sel_codes[0].normalize_ip_sel
            = BCM_FIELD_QSET_TEST(*qset_req, bcmFieldQualifyNormalizeIpAddrs)
            ? 1 : 0;
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_td2_selcodes_get
 *
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
int
_field_td2_selcode_get(int unit, _field_stage_t *stage_fc,
                      bcm_field_qset_t *qset_req,
                      _field_group_t *fg)
{

    BCM_IF_ERROR_RETURN(_bcm_field_tr_selcode_get(unit, stage_fc, qset_req, fg));

    switch (fg->stage_id) {
        case _BCM_FIELD_STAGE_INGRESS:
            return (_field_td2_ingress_selcode_get(unit, stage_fc, qset_req, fg));
        default:
            ;
    }

    return (BCM_E_NONE);
}

/*  
 * Function:
 *     _field_td2_selcodes_install
 *
 * Purpose:
 *    Routine used to install selector codes.
 * Parameters: 
 *     unit        - (IN) BCM device number.
 *     fg          - (IN) Installed group structure.
 *     slice_numb  - (IN) Slice number to set mode for.
 *     pbmp        - (IN) Group active port bit map.
 *     selcode_idx - (IN) Index into select codes array.
 *  
 * Returns:
 *     BCM_E_XXX
 */


int
_field_td2_selcodes_install(int            unit,
                            _field_group_t *fg,
                            uint8          slice_num,
                            bcm_pbmp_t     pbmp,
                            int            selcode_idx
                            )
{
    int rv;

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_field_td2_mode_set(unit, slice_num, fg, fg->flags));

    switch (fg->stage_id) {
        case _BCM_FIELD_STAGE_LOOKUP:
            rv = _bcm_field_td2_lookup_selcodes_install(unit, fg, slice_num,
                                                        selcode_idx);
            break;
        case _BCM_FIELD_STAGE_INGRESS:
            rv =  _bcm_field_td2_ingress_selcodes_install(unit, fg, slice_num,
                                                          &pbmp, selcode_idx);
            break;
        case _BCM_FIELD_STAGE_EGRESS:
            rv = _field_td2_egress_selcodes_install(unit, fg, slice_num,
                                                    &pbmp, selcode_idx);;
            break;
        default:
            return (BCM_E_INTERNAL);
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_field_td2_selcodes_install
 *
 * Purpose:
 *    Routine used to install selector codes.
 * Parameters:
 *     unit        - (IN) BCM device number.
 *     fg          - (IN) Installed group structure.
 *     slice_numb  - (IN) Slice number to set mode for.
 *     pbmp        - (IN) Group active port bit map.
 *     selcode_idx - (IN) Index into select codes array.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_td2_selcodes_install(int            unit,
                                _field_group_t *fg,
                                uint8          slice_num,
                                bcm_pbmp_t     pbmp,
                                int            selcode_idx
                                )
{
    return _field_td2_selcodes_install(unit, fg, slice_num, pbmp, selcode_idx);
}
/*
 * Function:
 *    _field_td2_group_install
 *
 * Purpose:
 *    Auxiliary routine used to install field group.
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     fg         - (IN) Installed group structure.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_td2_group_install(int unit, _field_group_t *fg)
{
    _field_slice_t *fs;        /* Slice pointer.           */
    uint8  slice_number;       /* Slices iterator.         */
    int    parts_count;        /* Number of entry parts.   */
    int    idx;                /* Iteration index.         */

    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Get number of entry parts. */
    BCM_IF_ERROR_RETURN(_bcm_field_entry_tcam_parts_count(unit, 
                                                          fg->stage_id,
                                                          fg->flags, 
                                                          &parts_count));
    for (idx = 0; idx < parts_count; ++idx) {
        BCM_IF_ERROR_RETURN(_bcm_field_tcam_part_to_slice_number(unit, idx,
                                                                 fg,
                                                                 &slice_number
                                                                 )
                            );
        fs = fg->slices + slice_number;

        BCM_IF_ERROR_RETURN(_field_td2_selcodes_install(unit,
                                                        fg,
                                                        fs->slice_number,
                                                        fg->pbmp,
                                                        idx
                                                        )
                            );
    }

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_field_td2_group_install
 *
 * Purpose:
 *    Auxiliary routine used to install field group.
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     fg         - (IN) Installed group structure.
 *
 * Returns:
 *     BCM_E_XXX
 */

int 
_bcm_field_td2_group_install(int unit, _field_group_t *fg)
{
  return _field_td2_group_install(unit, fg);
}
/*
 * Function:
 *     _field_td2_lookup_slice_clear
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
_field_td2_lookup_slice_clear(int unit, unsigned slice_num)
{
    uint64 reg_val;

    BCM_IF_ERROR_RETURN(soc_reg64_get(unit,
                                      VFP_KEY_CONTROL_1r,
                                      REG_PORT_ANY,
                                      0,
                                      &reg_val
                                      )
                        );
    soc_reg64_field32_set(unit,
                          VFP_KEY_CONTROL_1r,
                          &reg_val,
                          _bcm_field_trx_vfp_double_wide_sel[slice_num],
                          0
                          );
    soc_reg64_field32_set(unit,
                          VFP_KEY_CONTROL_1r,
                          &reg_val,
                          _bcm_field_trx_vfp_field_sel[slice_num][0],
                          0
                          );
    soc_reg64_field32_set(unit,
                          VFP_KEY_CONTROL_1r,
                          &reg_val,
                          _bcm_field_trx_vfp_field_sel[slice_num][1],
                          0
                          );
    soc_reg64_field32_set(unit,
                          VFP_KEY_CONTROL_1r,
                          &reg_val,
                          _bcm_field_trx_slice_pairing_field[slice_num >> 1],
                          0
                          );
    BCM_IF_ERROR_RETURN(soc_reg64_set(unit,
                                      VFP_KEY_CONTROL_1r,
                                      REG_PORT_ANY,
                                      0,
                                      reg_val
                                      )
                        );


    return (soc_reg_field32_modify(unit,
                                   VFP_KEY_CONTROL_2r,
                                   REG_PORT_ANY,
                                   _bcm_field_trx_vfp_ip_header_sel[slice_num],
                                   0
                                   )
            );
}

/*
 * Function:
 *     _bcm_field_td2_lookup_slice_clear
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
int
_bcm_field_td2_lookup_slice_clear(int unit, unsigned slice_num)
{
    return _field_td2_lookup_slice_clear(unit, slice_num);
}
/*
 * Function:
 *     _field_td2_slice_clear
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
_field_td2_slice_clear(int unit, _field_group_t *fg, _field_slice_t *fs)
{
    int rv;

    switch (fs->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          rv = _bcm_field_trx_ingress_slice_clear(unit, fs->slice_number);
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _field_td2_lookup_slice_clear(unit, fs->slice_number);
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
 *     _bcm_field_td2_slice_clear
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
_bcm_field_td2_slice_clear(int unit, _field_group_t *fg, _field_slice_t *fs)
{
    return _field_td2_slice_clear(unit, fg, fs);
}

/*
 * Function:
 *     _field_td2_entry_move
 * Purpose:
 *     Copy an entry from one TCAM index to another. It copies the values in
 *     hardware from the old index to the new index. 
 *     IT IS ASSUMED THAT THE NEW INDEX IS EMPTY (VALIDf=00) IN HARDWARE.
 *     The old Hardware index is cleared at the end.
 * Parameters:
 *     unit           - (IN) BCM device number. 
 *     f_ent          - (IN) Entry to move
 *     parts_count    - (IN) Field entry parts count.
 *     tcam_idx_old   - (IN) Source entry tcam index.
 *     tcam_idx_new   - (IN) Destination entry tcam index.
 *                          
 * Returns:
 *     BCM_E_XXX
 */

int
_field_td2_entry_move(int unit, _field_entry_t *f_ent, int parts_count,
                      int *tcam_idx_old, int *tcam_idx_new)
{
    static uint32 e_buf[BCM_MAX_NUM_UNITS][25];
                                        /* For TCAM .                         */
    static uint32 p_buf[BCM_MAX_NUM_UNITS][_FP_MAX_ENTRY_WIDTH][25];
                                        /* For policies                       */
    int new_slice_numb = 0;             /* Entry new slice number.            */
    int new_slice_idx = 0;              /* Entry new offset in the slice      */
    soc_mem_t tcam_mem;                 /* TCAM memory id.                    */
    soc_mem_t policy_mem;               /* Policy table memory id .           */
    int tcam_idx_max;                   /* TCAM memory max index.             */
    int tcam_idx_min;                   /* TCAM memory min index.             */
    _field_stage_t *stage_fc;           /* Stage field control structure.     */
    _field_stage_id_t stage_id;         /* Field pipeline stage id.           */
    int idx;                            /* Iteration index.                   */
    _field_policer_t *f_pl = NULL;      /* Field policer descriptor.          */
    _field_stat_t    *f_st = NULL;      /* Field statistics descriptor.       */
    _field_group_t   *fg;               /* Field group structure.             */
    int              rv = BCM_E_NONE;   /* Operation return status.           */
    int              is_backup_entry = 0;  /* Flag ,checks the entry is
                                              backup entry or not*/
    int meter_moved = 0;               /* meter moved as part of entry move*/
    fp_global_mask_tcam_x_entry_t fp_global_mask_x[_FP_MAX_ENTRY_WIDTH];
    fp_global_mask_tcam_y_entry_t fp_global_mask_y[_FP_MAX_ENTRY_WIDTH];
    fp_global_mask_tcam_entry_t fp_global_mask[_FP_MAX_ENTRY_WIDTH];
    fp_gm_fields_entry_t fp_gm_fields[_FP_MAX_ENTRY_WIDTH];
    bcm_pbmp_t pbmp_x, pbmp_y;
#if defined (BCM_TOMAHAWK_SUPPORT)
    uint8  enabled = 0;
    int rv_temp;
#endif

    /* Input parameters check. */
    if ((NULL == f_ent) || (NULL == tcam_idx_old) || (NULL == tcam_idx_new)) {
        return (BCM_E_PARAM);
    }

    fg = f_ent->group;

    /* Get field stage control . */
    stage_id = f_ent->group->stage_id;
    rv = _field_stage_control_get(unit, stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_field_multi_pipe_support)) {
       /* Get entry tcam and policy tables. */
       rv = _bcm_field_th_tcam_policy_mem_get(unit, f_ent,
                                             &tcam_mem, &policy_mem);
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
    {
       /* Get entry tcam and actions. */
       rv = _field_fb_tcam_policy_mem_get(unit, stage_id,
                                          &tcam_mem, &policy_mem);
    }
    BCM_IF_ERROR_RETURN(rv);

    tcam_idx_max = soc_mem_index_max(unit, tcam_mem);
    tcam_idx_min = soc_mem_index_min(unit, tcam_mem);

        /* Update policy entry if moving across the slices. */
        if (((NULL != f_ent->ent_copy) && 
                    (f_ent->ent_copy->eid ==  _FP_INTERNAL_RESERVED_ID))) {
            is_backup_entry = 1;
        }

    for (idx = 0; idx < parts_count; idx++) {
        /* Index sanity check. */
        if ((tcam_idx_old[idx] < tcam_idx_min) || (tcam_idx_old[idx] > tcam_idx_max) ||
            (tcam_idx_new[idx] < tcam_idx_min) || (tcam_idx_new[idx] > tcam_idx_max)) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "FP(unit %d) vverb: Invalid index range for _field_td2_entry_move\n"
                                  "from %d to %d"), unit, tcam_idx_old[idx], tcam_idx_new[idx]));
            rv = BCM_E_PARAM;
            /* coverity[dead_error_line:FALSE] */
            BCM_IF_ERROR_RETURN(rv);

        }

        /* Read policy entry from current tcam index. */
        rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY, tcam_idx_old[idx],
                          p_buf[unit][idx]);
        BCM_IF_ERROR_RETURN(rv);

        /*
         * On Trident2, VALIDf of FP_GLOBAL_MASK_TCAMm must be set
         * for all parts of an wide/paired mode entry.
         * This bit would have been set during entry install.
         * Read this value from all parts of an entry while moving.
         * Otherwise, actions related to this entry would not work.
         */
        if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2X(unit))
             && (stage_id == _BCM_FIELD_STAGE_INGRESS)) {
            if ((fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE)
                && (0x1 & idx)) {
                rv = READ_FP_GM_FIELDSm(unit, MEM_BLOCK_ANY,
                        tcam_idx_old[idx], &fp_gm_fields[idx]);
                BCM_IF_ERROR_RETURN(rv);
            } else {
                rv = READ_FP_GLOBAL_MASK_TCAMm(unit, MEM_BLOCK_ANY,
                        tcam_idx_old[idx], &fp_global_mask[idx]);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
    }

    /* Calculate primary entry new slice & offset in the slice. */
    rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc, fg->instance,
                                             tcam_idx_new[0],
                                             &new_slice_numb, &new_slice_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Update policy entry if moving across the slices. */
    if ((1 != is_backup_entry) && (f_ent->fs->slice_number != new_slice_numb)) {
        /* Get policer associated with the entry. */
        if ((0 == (stage_fc->flags & _FP_STAGE_GLOBAL_METER_POOLS)) &&
            (f_ent->policer[0].flags & _FP_POLICER_INSTALLED)) {
            rv = (_bcm_field_policer_get(unit,
                                         f_ent->policer[0].pid,
                                         &f_pl));
            BCM_IF_ERROR_RETURN(rv);
        }
        /* Get statistics entity associated with the entry. */
        if ((0 == (stage_fc->flags & _FP_STAGE_GLOBAL_COUNTERS)) &&
            (f_ent->statistic.flags & _FP_ENTRY_STAT_INSTALLED)) {
            rv = (_bcm_field_stat_get(unit,
                                      f_ent->statistic.sid,
                                      &f_st));
            BCM_IF_ERROR_RETURN(rv);
        }
        if (fg->flags & (_FP_GROUP_SPAN_SINGLE_SLICE |
                         _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
            /*
             * For _FP_GROUP_INTRASLICE_DOUBLEWIDE, *even* if it is
             *     _FP_GROUP_SPAN_DOUBLE_SLICE, we do this.
             *     This is because in intraslice double-wide, the PRI
             *     slice has tcam_slice_sz/2 entries, and same number
             *     of counter/meter pairs.
             *         Thus, counter/meter will always be allocated in the
             *         PRI slice.
             */
            if (NULL != f_st) {
                /*
                 * Set the index of the counter for entry in new slice
                 * The new index has already been calculated in
                 * _field_entry_move
                 */
                if (stage_id == _BCM_FIELD_STAGE_EGRESS ) {
                    soc_mem_field32_set(unit, policy_mem, p_buf[unit][0],
                                        PID_COUNTER_INDEXf, f_st->hw_index);
                    soc_mem_field32_set(unit, policy_mem, p_buf[unit][0],
                                        PID_COUNTER_MODEf, f_st->hw_mode);
                } else if (stage_id == _BCM_FIELD_STAGE_LOOKUP ) {
                    soc_mem_field32_set(unit, policy_mem, p_buf[unit][0],
                                        VINTF_CTR_IDXf, f_st->hw_index);
                    soc_mem_field32_set(unit, policy_mem, p_buf[unit][0],
                                        USE_VINTF_CTR_IDXf, f_st->hw_mode);
                } else {
                    soc_mem_field32_set(unit, policy_mem, p_buf[unit][0],
                                        COUNTER_INDEXf, f_st->hw_index);
                    soc_mem_field32_set(unit, policy_mem, p_buf[unit][0],
                                        COUNTER_MODEf, f_st->hw_mode);
                }
            }
            if (NULL != f_pl) {
                meter_moved = 1;

                /*
                 * Set the index of the meter for entry in new slice
                 * The new index has already been calculated in
                 * _field_entry_move
                 */
                soc_mem_field32_set(unit, policy_mem, p_buf[unit][0],
                                    METER_INDEX_EVENf, f_pl->hw_index);
                soc_mem_field32_set(unit, policy_mem, p_buf[unit][0],
                                    METER_INDEX_ODDf, f_pl->hw_index);
                /* in case of meter move, the even or odd meter should be
                 * set based on meter index of new slice
                 */
                if ((f_pl->stage_id == _BCM_FIELD_STAGE_EGRESS) &&
                    (SOC_IS_TRIDENT2PLUS(unit))) {
                    /*
                     * Flow mode is the only one that cares about the test and
                     * update bits.
                     */
                    if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
                       /* Excess meter - even index. */
                        soc_mem_field32_set(unit, policy_mem,
                                     p_buf[unit][0], METER_TEST_ODDf, 0);
                        if (SOC_IS_KATANA2(unit) &&
                            (f_pl->cfg.flags & BCM_POLICER_AS_PACKET_COUNTER)) {
                            soc_mem_field32_set(unit, policy_mem,
                                         p_buf[unit][0],
                                         METER_TEST_EVENf, 0);
                        } else {
                            soc_mem_field32_set(unit, policy_mem,
                                         p_buf[unit][0],
                                         METER_TEST_EVENf, 1);
                        }
                        soc_mem_field32_set(unit, policy_mem,
                                     p_buf[unit][0],
                                     METER_UPDATE_ODDf, 0);
                        soc_mem_field32_set(unit, policy_mem,
                                     p_buf[unit][0],
                                     METER_UPDATE_EVENf, 1);
                    } else if (_FP_POLICER_COMMITTED_HW_METER(f_pl)) {
                    /* Committed meter - odd index. */
                        if (SOC_IS_KATANA2(unit) &&
                            (f_pl->cfg.flags & BCM_POLICER_AS_PACKET_COUNTER)) {
                            soc_mem_field32_set(unit, policy_mem, p_buf[unit][0], METER_TEST_ODDf, 0);
                        } else {
                            soc_mem_field32_set(unit, policy_mem, p_buf[unit][0], METER_TEST_ODDf, 1);
                        }
                        soc_mem_field32_set(unit, policy_mem,
                                     p_buf[unit][0],
                                     METER_TEST_EVENf, 0);
                        soc_mem_field32_set(unit, policy_mem,
                                     p_buf[unit][0],
                                     METER_UPDATE_ODDf, 1);
                        soc_mem_field32_set(unit, policy_mem,
                                     p_buf[unit][0],
                                     METER_UPDATE_EVENf, 0);
                    } else if (f_pl->cfg.mode == bcmPolicerModePassThrough) {
                        soc_mem_field32_set(unit, policy_mem,
                                     p_buf[unit][0],
                                     METER_TEST_EVENf, 1);
                    }
#if defined(BCM_TRIDENT2_SUPPORT)
                    else if (f_pl->cfg.mode == bcmPolicerModeSrTcmModified
                        && (SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit))) {
                        soc_mem_field32_set(unit, policy_mem,
                                     p_buf[unit][0],
                                    METER_MODE_MODIFIERf, 1);
                    }
#endif
                }
            }
        } else {
            if (NULL != f_st) {
                _bcm_field_fb_counter_adjust_wide_mode(unit, policy_mem,
                                                       f_st, f_ent,
                                                       f_ent + 1,
                                                       new_slice_numb,
                                                       p_buf[unit][0],
                                                       p_buf[unit][1]);
            }
            if (NULL != f_pl) {
                meter_moved = 1;
                _bcm_field_fb_meter_adjust_wide_mode(unit, policy_mem,
                                                     f_pl, f_ent, f_ent + 1,
                                                     p_buf[unit][0],
                                                     p_buf[unit][1]);
            }
        }
    }

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_field_multi_pipe_support)) {
        _field_slice_t *fs;
        for (idx = 0; idx < parts_count; idx++) {
           /* Calculate entry new slice & offset in the slice. */
           rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc,
                                                    fg->instance,
                                                    tcam_idx_new[idx],
                                                    &new_slice_numb,
                                                    &new_slice_idx);
           if (BCM_SUCCESS(rv)) {

               fs = &stage_fc->slices[f_ent->group->instance][new_slice_numb];
               /* Enable the slice if the entry moves to new slice. */
               if (!(fs->slice_flags & _BCM_FIELD_SLICE_HW_ENABLE)) {
                   if (BCM_FAILURE(_bcm_field_th_slice_enable_set(unit,
                                                      f_ent->group, fs, 1))) {
                       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                           "Failed to Enable Field Slice[%d] for Entry[%d].\n\r"),
                           fs->slice_number, f_ent->eid));
                   }
                   fs->slice_flags |= _BCM_FIELD_SLICE_HW_ENABLE;
                   enabled = 1;
               }
           }
        }
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
    /*
     * Write entry to the destination
     * ORDER is important
     */
    for (idx = parts_count - 1; idx >= 0; idx--) {

        /* Write duplicate  policy entry to new tcam index. */
        rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, tcam_idx_new[idx],
                           p_buf[unit][idx]);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        if (((SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2X(unit)))
             && (stage_id == _BCM_FIELD_STAGE_INGRESS)) {
            if ((fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE)
                && (0x1 & idx)) {
                rv = WRITE_FP_GM_FIELDSm(unit, MEM_BLOCK_ALL,
                        tcam_idx_new[idx], &fp_gm_fields[idx]);
                BCM_IF_ERROR_RETURN(rv);
            } else {
                if (SOC_MEM_IS_VALID(unit, FP_GLOBAL_MASK_TCAM_Xm) &&
                    SOC_MEM_IS_VALID(unit, FP_GLOBAL_MASK_TCAM_Ym)) {

                    rv = READ_FP_GLOBAL_MASK_TCAM_Xm(unit, MEM_BLOCK_ANY,
                                                     tcam_idx_old[idx],
                                                     &fp_global_mask_x[idx]);
                    BCM_IF_ERROR_RETURN(rv);
                    rv = READ_FP_GLOBAL_MASK_TCAM_Ym(unit, MEM_BLOCK_ANY,
                                                     tcam_idx_old[idx],
                                                     &fp_global_mask_y[idx]);
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_pbmp_field_get(unit, FP_GLOBAL_MASK_TCAM_Xm,
                                           &fp_global_mask_x[idx],
                                           IPBMf, &pbmp_x);
                    soc_mem_pbmp_field_get(unit, FP_GLOBAL_MASK_TCAM_Ym,
                                           &fp_global_mask_y[idx],
                                           IPBMf, &pbmp_y);
                    BCM_PBMP_OR(pbmp_x, pbmp_y);

                    soc_mem_pbmp_field_set(unit,
                                           FP_GLOBAL_MASK_TCAMm,
                                           &fp_global_mask[idx],
                                           IPBMf,
                                           &pbmp_x
                                          );
                    soc_mem_pbmp_field_get(unit, FP_GLOBAL_MASK_TCAM_Xm,
                                           &fp_global_mask_x[idx],
                                           IPBM_MASKf, &pbmp_x);
                    soc_mem_pbmp_field_get(unit, FP_GLOBAL_MASK_TCAM_Ym,
                                           &fp_global_mask_y[idx],
                                           IPBM_MASKf, &pbmp_y);
                    BCM_PBMP_OR(pbmp_x, pbmp_y);

                    soc_mem_pbmp_field_set(unit,
                                           FP_GLOBAL_MASK_TCAMm,
                                           &fp_global_mask[idx],
                                           IPBM_MASKf,
                                           &pbmp_x
                                          );
                }
                rv = WRITE_FP_GLOBAL_MASK_TCAMm(unit, MEM_BLOCK_ALL,
                        tcam_idx_new[idx], &fp_global_mask[idx]);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
        if (meter_moved == 0) {
            /* Read tcam entry from current tcam index. */
            rv = soc_mem_read(unit, tcam_mem, MEM_BLOCK_ANY, tcam_idx_old[idx],
                          e_buf[unit]);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            /* Write duplicate  tcam entry to new tcam index. */
            rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx_new[idx],
                           e_buf[unit]);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }

    /*    
     * fp_entry_move is invoked even for backup entry create 
     * to copy TCAM and FP_POLICY_TABLE tables. In this case
     * TCAM and FP_POLICY_TABLEs should not be erased for
     * original entry.
     */
    if (1 != is_backup_entry) {
        /*
         * Clear old location
         * ORDER is important
         */
        for (idx = 0; idx < parts_count; idx++) {
#if defined (BCM_TOMAHAWK_SUPPORT)
            if (soc_feature(unit, soc_feature_field_multi_pipe_support)) {
                rv = _bcm_field_th_tcam_policy_clear(unit, f_ent, tcam_idx_old[idx]);
            } else
#endif /* BCM_TOMAHAWK_SUPPORT */
            {
                rv = _field_fb_tcam_policy_clear(unit, NULL, stage_id, tcam_idx_old[idx]);
            }
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }
    return (BCM_E_NONE);

cleanup:
#if defined (BCM_TOMAHAWK_SUPPORT)
    if (enabled == 1) {
        _field_slice_t *fs;
        for (idx = 0; idx < parts_count; idx++) {
            /* Calculate entry new slice & offset in the slice. */
            rv_temp = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc,
                                                     fg->instance,
                                                     tcam_idx_new[idx],
                                                     &new_slice_numb,
                                                     &new_slice_idx);
            if (BCM_SUCCESS(rv_temp)) {
                fs = &stage_fc->slices[f_ent->group->instance][new_slice_numb];
                if (fs->slice_flags & _BCM_FIELD_SLICE_HW_ENABLE) {
                    if (BCM_FAILURE(_bcm_field_th_slice_enable_set(unit, f_ent->group,
                                                                   fs, 0))) {
                        LOG_WARN(BSL_LS_BCM_FP, (BSL_META("Failed to Disable Field"
                                                  " Slice[%d] for Entry[%d].\n\r"),
                                                    fs->slice_number, f_ent->eid));
                    }
                    fs->slice_flags &= ~_BCM_FIELD_SLICE_HW_ENABLE;
                }
            }
        }
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    return rv;

}


/*
 * Function:
 *     _bcm_field_td2_entry_move
 * Purpose:
 *     Copy an entry from one TCAM index to another. It copies the values in
 *     hardware from the old index to the new index. 
 *     IT IS ASSUMED THAT THE NEW INDEX IS EMPTY (VALIDf=00) IN HARDWARE.
 *     The old Hardware index is cleared at the end.
 * Parameters:
 *     unit           - (IN) BCM device number. 
 *     f_ent          - (IN) Entry to move
 *     parts_count    - (IN) Field entry parts count.
 *     tcam_idx_old   - (IN) Source entry tcam index.
 *     tcam_idx_new   - (IN) Destination entry tcam index.
 *                          
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_td2_entry_move(int unit, _field_entry_t *f_ent, int parts_count,
                      int *tcam_idx_old, int *tcam_idx_new)
{
    return _field_td2_entry_move(unit, f_ent, parts_count, 
                                 tcam_idx_old, tcam_idx_new);
}

int
_bcm_field_td2_qualify_LoopbackType(int                      unit,
                                    bcm_field_LoopbackType_t loopback_type,
                                    uint32                   *tcam_data,
                                    uint32                   *tcam_mask)
{
    switch (loopback_type) {
        case bcmFieldLoopbackTypeAny:
            *tcam_data = 0x10;
            *tcam_mask = 0x10;
            break;
        case bcmFieldLoopbackTypeMim:
            *tcam_data = 0x10;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldLoopbackTypeTrillNetwork:
            *tcam_data = 0x11;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldLoopbackTypeTrillAccess:
            *tcam_data = 0x12;
            *tcam_mask = 0x1f;
            break;
#if defined(BCM_APACHE_SUPPORT)
        case bcmFieldLoopbackTypeMirror:
            if (SOC_IS_APACHE(unit)) {
                *tcam_data = 0x13;
                *tcam_mask = 0x1f;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmFieldLoopbackTypeRedirect:
            if (SOC_IS_APACHE(unit)) {
                *tcam_data = 0x14;
                *tcam_mask = 0x1f;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
#endif /* BCM_APACHE_SUPPORT */
        case bcmFieldLoopbackTypeQcn:
#if defined(BCM_TOMAHAWK_SUPPORT)
            if (soc_feature(unit, soc_feature_field_multi_pipe_support)) {
               return BCM_E_UNAVAIL;
            }
#endif /* BCM_TOMAHAWK_SUPPORT */
            *tcam_data = 0x17;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldLoopbackTypeVxlan:
            *tcam_data = 0x1b;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldLoopbackTypeL2Gre:
            *tcam_data = 0x1e;
            *tcam_mask = 0x1f;
            break;
        default:
            return (BCM_E_PARAM);
    }

    return (BCM_E_NONE);
}

int
_bcm_field_td2_qualify_LoopbackType_get(uint8                    tcam_data,
                                        uint8                    tcam_mask,
                                        bcm_field_LoopbackType_t *loopback_type
                                        )
{
    switch (tcam_data & tcam_mask) {
        case 0x10:
            if (tcam_mask == 0x10) {
                *loopback_type = bcmFieldLoopbackTypeAny;
            } else {
                *loopback_type = bcmFieldLoopbackTypeMim;
            }
            break;
        case 0x11:
            *loopback_type = bcmFieldLoopbackTypeTrillNetwork;
            break;
        case 0x12:
            *loopback_type = bcmFieldLoopbackTypeTrillAccess;
            break;
#if defined(BCM_APACHE_SUPPORT)
        case 0x13:
            *loopback_type = bcmFieldLoopbackTypeMirror;
            break;
        case 0x14:
            *loopback_type = bcmFieldLoopbackTypeRedirect;
            break;
#endif /* BCM_APACHE_SUPPORT */
        case 0x17:
            *loopback_type = bcmFieldLoopbackTypeQcn;
            break;
        case 0x1b:
            *loopback_type = bcmFieldLoopbackTypeVxlan;
            break;
        case 0x1e:
            *loopback_type = bcmFieldLoopbackTypeL2Gre;
            break;
        default:
            return (BCM_E_PARAM);
    }

    return (BCM_E_NONE);
}

int
_bcm_field_td2_qualify_TunnelType(bcm_field_TunnelType_t tunnel_type,
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
            *tcam_mask = 0x1f;
            break;
        case bcmFieldTunnelTypeIp:
            *tcam_data = 0x1;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldTunnelTypeMpls:
            *tcam_data = 0x2;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldTunnelTypeMim:
            *tcam_data = 0x3;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldTunnelTypeAutoMulticast:
            *tcam_data = 0x6;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldTunnelTypeTrill:
            *tcam_data = 0x7;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldTunnelTypeL2Gre:
            *tcam_data = 0x8;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldTunnelTypeVxlan:
            *tcam_data = 0x9;
            *tcam_mask = 0x1f;
            break;
        default:
            return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}

int
_bcm_field_td2_qualify_TunnelType_get(uint8                  tcam_data,
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
        case 0x3:
            *tunnel_type = bcmFieldTunnelTypeMim;
            break;
        case 0x6:
            *tunnel_type = bcmFieldTunnelTypeAutoMulticast;
            break;
        case 0x7:
            *tunnel_type = bcmFieldTunnelTypeTrill;
            break;
        case 0x8:
            *tunnel_type = bcmFieldTunnelTypeL2Gre;
            break;
        case 0x9:
            *tunnel_type = bcmFieldTunnelTypeVxlan;
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

static const struct {
    uint8 api, hw_data, hw_mask;
} pkt_res_xlate_tbl[] = {
    {  BCM_FIELD_PKT_RES_UNKNOWN,         0x00, 0x3f },
    {  BCM_FIELD_PKT_RES_CONTROL,         0x01, 0x3f },
    {  BCM_FIELD_PKT_RES_BPDU,            0x04, 0x3f },
    {  BCM_FIELD_PKT_RES_L2BC,            0x0c, 0x3f },
    {  BCM_FIELD_PKT_RES_L2UC,            0x08, 0x3f },
    {  BCM_FIELD_PKT_RES_L2UNKNOWN,       0x09, 0x3f },
    {  BCM_FIELD_PKT_RES_L3MCUNKNOWN,     0x13, 0x3f },
    {  BCM_FIELD_PKT_RES_L3MCKNOWN,       0x12, 0x3f },
    {  BCM_FIELD_PKT_RES_L2MCKNOWN,       0x0a, 0x3f },
    {  BCM_FIELD_PKT_RES_L2MCUNKNOWN,     0x0b, 0x3f },
    {  BCM_FIELD_PKT_RES_L3UCKNOWN,       0x10, 0x3f },
    {  BCM_FIELD_PKT_RES_L3UCUNKNOWN,     0x11, 0x3f },
    {  BCM_FIELD_PKT_RES_MPLSKNOWN,       0x1c, 0x3f },
    {  BCM_FIELD_PKT_RES_MPLSMCKNOWN,     0x1d, 0x3f },
    {  BCM_FIELD_PKT_RES_MPLSL3KNOWN,     0x1a, 0x3f },
    {  BCM_FIELD_PKT_RES_MPLSL2KNOWN,     0x18, 0x3f },
    {  BCM_FIELD_PKT_RES_MPLSUNKNOWN,     0x19, 0x3f },
    {  BCM_FIELD_PKT_RES_MIMKNOWN,        0x20, 0x3f },
    {  BCM_FIELD_PKT_RES_MIMUNKNOWN,      0x21, 0x3f },
    {  BCM_FIELD_PKT_RES_TRILLKNOWN,      0x28, 0x3f },
    {  BCM_FIELD_PKT_RES_TRILLUNKNOWN,    0x29, 0x3f },
    {  BCM_FIELD_PKT_RES_NIVKNOWN,        0x30, 0x3f },
    {  BCM_FIELD_PKT_RES_NIVUNKNOWN,      0x31, 0x3f },
    {  BCM_FIELD_PKT_RES_OAM,             0x02, 0x3f },
    {  BCM_FIELD_PKT_RES_BFD,             0x03, 0x3f },
    {  BCM_FIELD_PKT_RES_ICNM,            0x05, 0x3f },
    {  BCM_FIELD_PKT_RES_IEEE1588,        0x06, 0x3f },
    {  BCM_FIELD_PKT_RES_L2GREKNOWN,      0x32, 0x3f },
    {  BCM_FIELD_PKT_RES_VXLANKNOWN,      0x33, 0x3f },
    {  BCM_FIELD_PKT_RES_FCOEKNOWN,       0x34, 0x3f },
    {  BCM_FIELD_PKT_RES_FCOEUNKNOWN,     0x35, 0x3f },
    {  BCM_FIELD_PKT_RES_PIMBIDIRUNKNOWN, 0x1d, 0x3f },
    {  BCM_FIELD_PKT_RES_L2_ANY,          0x08, 0x38 },
    {  BCM_FIELD_PKT_RES_L2MC_ANY,        0x0a, 0x3e },
    {  BCM_FIELD_PKT_RES_TRILL_ANY,       0x28, 0x3e },
    {  BCM_FIELD_PKT_RES_NIV_ANY,         0x30, 0x3e },
    {  BCM_FIELD_PKT_RES_MPLS_ANY,        0x18, 0x38 },
    {  BCM_FIELD_PKT_RES_MIM_ANY,         0x20, 0x3e },
    {  BCM_FIELD_PKT_RES_FCOE_ANY,        0x34, 0x3e },
    {  BCM_FIELD_PKT_RES_L3_ANY,          0x10, 0x3e }
};

int
_bcm_field_td2_qualify_PacketRes(int               unit,
                                 bcm_field_entry_t entry,
                                 uint32            *data,
                                 uint32            *mask
                                 )
{
    uint32 i;
    uint8 mark = 0;

    /* Translate data #defines to hardware encodings */
    for (i = 0; i < COUNTOF(pkt_res_xlate_tbl); ++i) {
        if (*data == pkt_res_xlate_tbl[i].api) {
            *data = pkt_res_xlate_tbl[i].hw_data;
            *mask = pkt_res_xlate_tbl[i].hw_mask;
            mark = 1;
            break;
        }
    }
    if (mark == 1) {
        return (BCM_E_NONE);
    } else {   
        return (BCM_E_INTERNAL);
    }
}

int
_bcm_field_td2_qualify_PacketRes_get(int               unit,
                                     bcm_field_entry_t entry,
                                     uint32            *data,
                                     uint32            *mask
                                     )
{
    uint32 i;
    uint8 mark = 0;
    /* Translate data #defines from hardware encodings */

    for (i = 0; i < COUNTOF(pkt_res_xlate_tbl); ++i) {
        if (*data == pkt_res_xlate_tbl[i].hw_data ) {
            if ( pkt_res_xlate_tbl[i].hw_mask == 0x3f) {
                if ( (*mask == pkt_res_xlate_tbl[i].hw_data) ||
                     (*mask == 0x3f) ) {
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (soc_feature(unit,
                        soc_feature_field_multi_pipe_enhanced) &&
                        *data == 0x1d) {
        *data = BCM_FIELD_PKT_RES_PIMBIDIRUNKNOWN;
        if (*mask == 0x1d) {
            *mask = BCM_FIELD_PKT_RES_PIMBIDIRUNKNOWN;
                        } else {
                            *mask = BCM_FIELD_EXACT_MATCH_MASK;
        }
            mark = 1;
            break;
        }
#endif
                    *data = pkt_res_xlate_tbl[i].api;
                    if (*mask == pkt_res_xlate_tbl[i].hw_data) {
            *mask = pkt_res_xlate_tbl[i].api;
                    } else {
                        *mask = BCM_FIELD_EXACT_MATCH_MASK;
                    }
                    mark = 1;
            break;
        }
            } else {
                if ( (*mask != pkt_res_xlate_tbl[i].hw_data) &&
                     (*mask != 0x3f) ) {
                    *data = pkt_res_xlate_tbl[i].api;
                    *mask = BCM_FIELD_EXACT_MATCH_MASK;
                    mark = 1;
                    break;
                }
            }
        }
    }
    if (mark == 1) {
        return (BCM_E_NONE);
    } else {
        return (BCM_E_INTERNAL);
    }
}

/*
 * Function:
 *      _bcm_field_td2_qualify_VlanTranslationHit
 * Purpose:
 *      Set match criteria for bcmFieildQualifyVlanTranslationHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data  - (OUT) Qualifier match data.
 *      mask  - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_field_td2_qualify_VlanTranslationHit(int               unit,
                                          bcm_field_entry_t entry,
                                          uint8             *data,
                                          uint8             *mask)
{
    uint8 data1 = 0;
    uint8 mask1 = 0;
    uint8 valid_range;

    if (data == NULL || mask == NULL) {
       return BCM_E_PARAM;
    }

    valid_range = BCM_FIELD_VXLT_LOOKUP_STATUS_FIRST_HIT |
                  BCM_FIELD_VXLT_LOOKUP_STATUS_SECOND_HIT |
                  BCM_FIELD_VXLT_LOOKUP_STATUS_HIT;
    /* exact match mask. */
    if (*mask == 0xff) {
      *mask = valid_range;
    }

    if ((*data != BCM_FIELD_VXLT_LOOKUP_STATUS_NO_HIT ||
         *mask != BCM_FIELD_VXLT_LOOKUP_STATUS_NO_HIT) &&
        ((*data & ~(valid_range)) || (*mask & ~(valid_range)))) {
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                  "Error: Invalid input paramaeters *data=0x%x *mask=0x%x\n"),
                   *data, *mask));
        return BCM_E_PARAM;
    }
       
    if (*data == BCM_FIELD_VXLT_LOOKUP_STATUS_NO_HIT) {
        data1 = 0;
    } else {
        if (*data & BCM_FIELD_VXLT_LOOKUP_STATUS_FIRST_HIT) {
           data1 |= 0x1;
        }

        if (*data & BCM_FIELD_VXLT_LOOKUP_STATUS_SECOND_HIT) {
           data1 |= 0x2;
        }

        if (*data & BCM_FIELD_VXLT_LOOKUP_STATUS_HIT) {
           data1 |= 0x4;
        }
    }

    if (*mask == BCM_FIELD_VXLT_LOOKUP_STATUS_NO_HIT) {
        mask1 = 0x0;
    } else {
        if (*mask & BCM_FIELD_VXLT_LOOKUP_STATUS_FIRST_HIT) {
           mask1 |= 0x1;
        }

        if (*mask & BCM_FIELD_VXLT_LOOKUP_STATUS_SECOND_HIT) {
           mask1 |= 0x2;
        }

        if (*mask & BCM_FIELD_VXLT_LOOKUP_STATUS_HIT) {
           mask1 |= 0x4;
        }
    }

    *data = data1;
    *mask = mask1;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_field_td2_qualify_VlanTranslationHit_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyVlanTranslationHit
 *                     qualifier from the field entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      data  - (OUT) Qualifier match data.
 *      mask  - (OUT) Qualifier match mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_field_td2_qualify_VlanTranslationHit_get(int               unit,
                                              bcm_field_entry_t entry,
                                              uint8             *data,
                                              uint8             *mask)
{
    uint8 data1 = 0;
    uint8 mask1 = 0;

    if (data == NULL || mask == NULL) {
       return BCM_E_PARAM;
    }

    /* Translate data #defines to hardware encodings */
    if (*data == 0) {
        data1 = BCM_FIELD_VXLT_LOOKUP_STATUS_NO_HIT;
    } else {
        if (*data & 0x1) {
           data1 |= BCM_FIELD_VXLT_LOOKUP_STATUS_FIRST_HIT;
        }

        if (*data & 0x2) {
           data1 |= BCM_FIELD_VXLT_LOOKUP_STATUS_SECOND_HIT;
        }

        if (*data & 0x4) {
           data1 |= BCM_FIELD_VXLT_LOOKUP_STATUS_HIT;
        }
    }

    if (*mask == 0) {
        mask1 = BCM_FIELD_VXLT_LOOKUP_STATUS_NO_HIT;
    } else {
        if (*mask & 0x1) {
           mask1 |= BCM_FIELD_VXLT_LOOKUP_STATUS_FIRST_HIT;
        }

        if (*mask & 0x2) {
           mask1 |= BCM_FIELD_VXLT_LOOKUP_STATUS_SECOND_HIT;
        }

        if (*mask & 0x4) {
           mask1 |= BCM_FIELD_VXLT_LOOKUP_STATUS_HIT;
        }
    }

    *data = data1;
    *mask = mask1;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_td2_hash_select_profile_get
 * Purpose:
 *     Get the redirect profile for the unit
 * Parameters:
 *     unit                - (IN) BCM device number
 *     profile_mem         - (IN) HASH selection table mem
 *     hash_select_profile - (OUT) hash selection profile
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
_bcm_field_td2_hash_select_profile_get(int unit, 
                                       soc_mem_t profile_mem,
                                       soc_profile_mem_t **hash_select_profile)
{
    _field_stage_t *stage_fc;

    
    if ((profile_mem != VFP_HASH_FIELD_BMAP_TABLE_Am) &&
        (profile_mem != VFP_HASH_FIELD_BMAP_TABLE_Bm)) {
        return BCM_E_PARAM; 
    } 
         
    /* Get stage control structure. */
    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, _BCM_FIELD_STAGE_LOOKUP, &stage_fc));

    if (profile_mem == VFP_HASH_FIELD_BMAP_TABLE_Am) {
        *hash_select_profile = &stage_fc->hash_select[0];
    } else {
        *hash_select_profile = &stage_fc->hash_select[1];
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_td2_hash_select_profile_ref_count_get
 * Purpose:
 *     Get hash selection profile entry use count.
 * Parameters:
 *     unit        - (IN) BCM device number.
 *     profile_mem - (IN) HASH selection table mem
 *     index       - (IN) Profile entry index.
 *     ref_count   - (OUT) redirect profile use count.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_td2_hash_select_profile_ref_count_get(int unit,
                                                 soc_mem_t profile_mem, 
                                                 int index, 
                                                 int *ref_count)
{
    soc_profile_mem_t *hash_select_profile;

    if (NULL == ref_count) {
        return (BCM_E_PARAM);
    }

    /* Get the redirect profile */
    BCM_IF_ERROR_RETURN
        (_bcm_field_td2_hash_select_profile_get(unit,  profile_mem, 
                                                &hash_select_profile));

    BCM_IF_ERROR_RETURN(soc_profile_mem_ref_count_get(unit,
                                                      hash_select_profile,
                                                      index, ref_count));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_td2_hash_select_profile_delete
 * Purpose:
 *     Delete hash select profile entry.
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     index     - (IN) Profile entry index.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_td2_hash_select_profile_delete(int unit,
                                          soc_mem_t profile_mem,
                                          int index) 
{
    soc_profile_mem_t *hash_select_profile;

    /* Get the redirect profile */
    BCM_IF_ERROR_RETURN
        (_bcm_field_td2_hash_select_profile_get(unit,  profile_mem, 
                                                &hash_select_profile));

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_delete(unit, hash_select_profile, index));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_td2_hash_select_profile_alloc
 * Purpose:
 *     Allocate hash selection profile index
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     f_ent    - (IN) Field entry structure to get policy info from.
 *     fa       - (IN) Field action.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_td2_hash_select_profile_alloc(int unit, _field_entry_t *f_ent,
                                             _field_action_t *fa)
   
{
    vfp_hash_field_bmap_table_a_entry_t entry_arr[2];
    uint32            *entry_ptr[2];
    soc_profile_mem_t *hash_select_profile;
    int               rv;
    void              *entries[1];
    soc_mem_t         profile_mem;

    if ((NULL == f_ent) || (NULL == fa)) {
        return (BCM_E_PARAM);
    }

    profile_mem = (fa->action == bcmFieldActionHashSelect0)? 
                                    VFP_HASH_FIELD_BMAP_TABLE_Am:
                                    VFP_HASH_FIELD_BMAP_TABLE_Bm ;

    entry_ptr[0] = (uint32 *)entry_arr;
    entry_ptr[1] =  entry_ptr[0] + soc_mem_entry_words(unit, profile_mem);
    entries[0] = (void *)&entry_arr;

    /* Reset hash select profile entry. */
    sal_memcpy(entry_ptr[0], soc_mem_entry_null(unit, profile_mem),
               soc_mem_entry_words(unit, profile_mem) * sizeof(uint32));
    sal_memcpy(entry_ptr[1], soc_mem_entry_null(unit, profile_mem),
               soc_mem_entry_words(unit, profile_mem) * sizeof(uint32));
 
    /* Get the hash_select profile */
    rv = _bcm_field_td2_hash_select_profile_get(unit,  profile_mem, 
                                                &hash_select_profile);
    BCM_IF_ERROR_RETURN(rv);

    switch (fa->action) {
      case bcmFieldActionHashSelect0:
      case bcmFieldActionHashSelect1:
          soc_mem_field_set(unit, profile_mem, entry_ptr[0], 
                            BITMAPf, &fa->param[0]);
          rv = soc_profile_mem_add(unit, hash_select_profile, entries,
                                   1, (uint32*) &fa->hw_index);
          if (BCM_SUCCESS(rv) && (fa->hw_index == 0)) {
            /*
             * Index 0 is invalid as the hardware will consider hash bitmap
             * selection from VFP only if HASH_FIELD_BITMAP_PTR_A and
             * HASH_FIELD_BITMAP_PTR_B are non-zero.
             * If the index is 0, return BCM_E_INTERNAL.
             */ 
             return BCM_E_INTERNAL;
          }
          BCM_IF_ERROR_RETURN(rv);
          break;
      default:
          return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_field_td2_hash_select_profile_hw_free
 *
 * Purpose:
 *     Free hash selection profile indexes required for entry installation.
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     f_ent     - (IN) Field entry descriptor.
 *     flags     - (IN) Free flags (old/new/both). 
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
_bcm_field_td2_hash_select_profile_hw_free(int unit, 
                                           _field_entry_t *f_ent, 
                                           uint32 flags)
{
    _field_action_t *fa;  /* Field action descriptor. */
    int rv = BCM_E_NONE;  /* Operation return status. */
    soc_mem_t mem; /* hash selection memory */

    /* Applicable to stage lookup on TRIDENT2 devices. */
    if ((0 == (SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit)||
               soc_feature(unit, soc_feature_field_multi_pipe_support))) ||
        (_BCM_FIELD_STAGE_LOOKUP != f_ent->group->stage_id)) {
        return (BCM_E_NONE);
    }

    /* Extract the policy info from the entry structure. */
    for (fa = f_ent->actions; fa != NULL; fa = fa->next) {
        switch (fa->action) {
          case bcmFieldActionHashSelect0:
          case bcmFieldActionHashSelect1:
              mem = ((fa->action == bcmFieldActionHashSelect0)?   
                     VFP_HASH_FIELD_BMAP_TABLE_Am: 
                     VFP_HASH_FIELD_BMAP_TABLE_Bm);

              if ((flags & _FP_ACTION_RESOURCE_FREE) && 
                  (_FP_INVALID_INDEX != fa->hw_index)) {
                  rv = _bcm_field_td2_hash_select_profile_delete(unit, 
                                                                 mem,
                                                                 fa->hw_index);
                  BCM_IF_ERROR_RETURN(rv);
                  fa->hw_index = _FP_INVALID_INDEX;
              } 

              if ((flags & _FP_ACTION_OLD_RESOURCE_FREE) && 
                  (_FP_INVALID_INDEX != fa->old_index)) {
                  rv = _bcm_field_td2_hash_select_profile_delete(unit,
                                                                 mem,
                                                                 fa->old_index);
                  BCM_IF_ERROR_RETURN(rv);
                  fa->old_index = _FP_INVALID_INDEX;
              }
              break;
          default:
              break;
        }
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_field_td2_hash_select_profile_hw_alloc
 *
 * Purpose:
 *     Allocate redirect profile index required for entry installation.
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     f_ent     - (IN) Field entry descriptor.
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
_bcm_field_td2_hash_select_profile_hw_alloc (int unit, _field_entry_t *f_ent)
{
    int rv = BCM_E_NONE;  /* Operation return status.     */
    _field_action_t *fa;  /* Field action descriptor.     */
    int ref_count;        /* Profile use reference count. */
    soc_mem_t mem;        /* Profile mem. */

    /* Applicable to stage lookup on TRIDENT2 devices only. */
    if ((0 == (SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit) ||
               soc_feature(unit, soc_feature_field_multi_pipe_support))) ||
        (_BCM_FIELD_STAGE_LOOKUP != f_ent->group->stage_id)) { 
        return (BCM_E_NONE);
    }

    /* Extract the policy info from the entry structure. */
    for (fa = f_ent->actions; 
        ((fa != NULL) && (_FP_ACTION_VALID & fa->flags)); 
        fa = fa->next) {
        switch (fa->action) {
          case bcmFieldActionHashSelect0:
          case bcmFieldActionHashSelect1:
              mem = ((fa->action == bcmFieldActionHashSelect0)?   
                     VFP_HASH_FIELD_BMAP_TABLE_Am: 
                     VFP_HASH_FIELD_BMAP_TABLE_Bm);

              /*
               * Store previous hardware index value in old_index.
               */ 
              fa->old_index = fa->hw_index;

              rv = _bcm_field_td2_hash_select_profile_alloc(unit, f_ent, fa);
              if ((BCM_E_RESOURCE == rv) && 
                  (_FP_INVALID_INDEX != fa->old_index)) {
                  /* Destroy old profile ONLY 
                   * if it is not used by other entries.
                   */
                  rv = _bcm_field_td2_hash_select_profile_ref_count_get(unit,
                                                                 mem,  
                                                                 fa->old_index,
                                                                 &ref_count);
                  BCM_IF_ERROR_RETURN(rv);
                  if (ref_count > 1) {
                      return (BCM_E_RESOURCE);
                  }
                  rv = _bcm_field_td2_hash_select_profile_delete(unit,
                                                                 mem, 
                                                                 fa->old_index);
                  BCM_IF_ERROR_RETURN(rv);

                  /* Destroy old profile is no longer required. */
                  fa->old_index = _FP_INVALID_INDEX;

                  /* Reallocate profile for new action. */
                  rv = _bcm_field_td2_hash_select_profile_alloc(unit, 
                                                                f_ent, 
                                                                fa);
              } 
              break;
          default:
              break;
        }
        if (BCM_FAILURE(rv)) {
            _bcm_field_td2_hash_select_profile_hw_free(unit, f_ent, _FP_ACTION_HW_FREE);
            break;
        }
    }

    return (rv);
}

/*
 * Function:
 *     _field_td2_action_hash_select
 * Purpose:
 *     Install hash selection action in policy table.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     mem      - (IN) Policy table memory.
 *     f_ent    - (IN) Field entry structure to get policy info from.
 *     fa       - (IN) Field action.
 *     buf      - (OUT) Field Policy table entry.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_td2_action_hash_select(int unit, soc_mem_t mem, _field_entry_t *f_ent,
                                  _field_action_t *fa, uint32 *buf)
{
    soc_field_t   hash_select_field = INVALIDf;

    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    switch (fa->action) {
    case bcmFieldActionHashSelect0:
        hash_select_field = HASH_FIELD_BITMAP_PTR_Af; 
        break; 
         
    case bcmFieldActionHashSelect1:
        hash_select_field = HASH_FIELD_BITMAP_PTR_Bf; 
        break;
    default:
        return (BCM_E_PARAM);
    }

    PolicySet(unit, mem, buf, hash_select_field, fa->hw_index);
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_td2_stage_action_support_check
 * Purpose:
 *     Check if action is supported by device for the given stage.
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
STATIC int
_field_td2_stage_action_support_check(int                unit,
                                      unsigned           stage,
                                      bcm_field_action_t action,
                                      int                *result
                                      )
{
    switch (action) {
        case bcmFieldActionFibreChanSrcBindEnable:
        case bcmFieldActionFibreChanFpmaPrefixCheckEnable:
        case bcmFieldActionFibreChanZoneCheckEnable:
            *result = (soc_feature(unit, soc_feature_fcoe) && 
                (stage == _BCM_FIELD_STAGE_LOOKUP));
            return (BCM_E_NONE);
            break;
        case bcmFieldActionHashSelect0:
        case bcmFieldActionHashSelect1:
            *result = (stage == _BCM_FIELD_STAGE_LOOKUP);
            return (BCM_E_NONE);
            break;
        case bcmFieldActionFibreChanVsanId:
            *result = (soc_feature(unit, soc_feature_fcoe) && 
                (stage == _BCM_FIELD_STAGE_INGRESS || _BCM_FIELD_STAGE_LOOKUP));
            return (BCM_E_NONE);
            break;
        case bcmFieldActionFibreChanZoneCheckActionCancel:
        case bcmFieldActionFibreChanIntVsanPri:
            *result = (soc_feature(unit, soc_feature_fcoe) &&
                (stage == _BCM_FIELD_STAGE_INGRESS));
            return (BCM_E_NONE);
            break;
        case bcmFieldActionNewClassId:
        case bcmFieldActionEgressClassSelect:
        case bcmFieldActionHiGigClassSelect:
        case bcmFieldActionIngSampleEnable: 
        case bcmFieldActionEgrSampleEnable:
        case bcmFieldActionTrunkLoadBalanceCancel:
        case bcmFieldActionDoNotCutThrough:
            *result = (stage == _BCM_FIELD_STAGE_INGRESS);
            return (BCM_E_NONE);
            break;
        case bcmFieldActionTrunkResilientHashCancel:
            *result = (soc_feature(unit, soc_feature_lag_resilient_hash) && 
                (stage == _BCM_FIELD_STAGE_INGRESS));
            return (BCM_E_NONE);
            break;
        case bcmFieldActionHgTrunkResilientHashCancel:
            *result = (soc_feature(unit, soc_feature_hg_resilient_hash) && 
                (stage == _BCM_FIELD_STAGE_INGRESS));
            return (BCM_E_NONE);
            break;
        case bcmFieldActionEcmpResilientHashCancel:
            *result = (soc_feature(unit, soc_feature_ecmp_resilient_hash) && 
                (stage == _BCM_FIELD_STAGE_INGRESS));
            return (BCM_E_NONE);
            break;
        case bcmFieldActionVxlanHeaderBits8_31_Set:
        case bcmFieldActionVxlanHeaderBits56_63_Set:
        case bcmFieldActionVxlanHeaderFlags:
            *result = (stage == _BCM_FIELD_STAGE_EGRESS);
            return (BCM_E_NONE);
        case bcmFieldActionOamOlpHeaderAdd:
        case bcmFieldActionOamDmTimeFormat:
            /* In trident 2 based chipsets actions are applicable
             * only for OAM based on FP */
            if (soc_feature(unit, soc_feature_fp_based_oam)) {
                if ((stage == _BCM_FIELD_STAGE_INGRESS)
#if defined(BCM_APACHE_SUPPORT)
                        || (SOC_IS_APACHE(unit) &&
                            (stage == _BCM_FIELD_STAGE_EGRESS))
#endif
                   ) {
                    *result = TRUE;
                } else {
                    *result = FALSE;
                }
            } else {
                *result = FALSE;
            }
            return BCM_E_NONE;
            break;
        case bcmFieldActionEgressPbmpNullCopyToCpu:
#if defined (BCM_TRIDENT2PLUS_SUPPORT)
            if (SOC_IS_TD2P_TT2P(unit) || SOC_IS_APACHE(unit)) {
                *result = (stage == _BCM_FIELD_STAGE_INGRESS);
            } else
#endif
            {
                *result = FALSE;
            }
            return BCM_E_NONE;
            break;
        case bcmFieldActionPimBidirFwd:
            *result = (stage == _BCM_FIELD_STAGE_LOOKUP);
            return (BCM_E_NONE);
            break;

        case bcmFieldActionUcastQueueNew:
        case bcmFieldActionRpUcastQueueNew:
        case bcmFieldActionYpUcastQueueNew:
        case bcmFieldActionGpUcastQueueNew:
            *result = ( stage == _BCM_FIELD_STAGE_INGRESS);
            return (BCM_E_NONE);
            break;
        case bcmFieldActionIntCosUcastQueueNew:
        case bcmFieldActionRpIntCosUcastQueueNew:
        case bcmFieldActionYpIntCosUcastQueueNew:
        case bcmFieldActionGpIntCosUcastQueueNew:
            *result = ( stage == _BCM_FIELD_STAGE_INGRESS);
            return (BCM_E_NONE);
            break;
#if defined(BCM_APACHE_SUPPORT)
        case bcmFieldActionStrength:
            if (SOC_IS_APACHE(unit) && !SOC_IS_MONTEREY(unit)) {
                *result = (stage == _BCM_FIELD_STAGE_EGRESS);
            } else {
                *result = FALSE;
            }
            return BCM_E_NONE;
            break;
        case bcmFieldActionRedirectBufferPriority:
            if (SOC_IS_APACHE(unit) && !SOC_IS_MONTEREY(unit)) {
                *result = (stage == _BCM_FIELD_STAGE_EGRESS);
            } else {
                *result = FALSE;
            }
            return BCM_E_NONE;
            break;
        case bcmFieldActionOamStatObjectSessionId:
#ifdef BCM_MONTEREY_SUPPORT
            if (SOC_IS_MONTEREY(unit)) {
                *result = FALSE;
            } else
#endif
            if (SOC_IS_APACHE(unit)) {
                *result = ((stage == _BCM_FIELD_STAGE_EGRESS) || (stage == _BCM_FIELD_STAGE_INGRESS));
            } else {
                *result = FALSE;
            }
            return BCM_E_NONE;
            break;
        case bcmFieldActionLoopbackOamResponse:
        case bcmFieldActionLoopbackSatResponse:
            if (SOC_IS_APACHE(unit)) {
                *result = ((stage == _BCM_FIELD_STAGE_EGRESS) || (stage == _BCM_FIELD_STAGE_INGRESS));
            } else {
                *result = FALSE;
            }
            return BCM_E_NONE;
            break;
#endif /* BCM_APACHE_SUPPORT */
        default:
        ;
    }

    return (_bcm_field_trx_stage_action_support_check(unit, stage, action, result));
}
/*
 * Function:
 *     _bcm_field_td2_stage_action_support_check
 * Purpose:
 *     Check if action is supported by device for the given stage.
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
_bcm_field_td2_stage_action_support_check(int            unit,
                                      unsigned           stage,
                                      bcm_field_action_t action,
                                      int                *result
                                      )
{
    return _field_td2_stage_action_support_check(unit, stage, action, result);
}
/*
 * Function:
 *     _field_td2_action_support_check
 * Purpose:
 *     Check if action is supported by device.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     f_ent    - (IN) Field entry structure to get policy info from.
 *     action   - (IN) Action to check(bcmFieldActionXXX)
 *     result -(OUT)              
 *               TRUE   - action is supported by device
 *               FALSE  - action is NOT supported by device
 * Returns:
 *     BCM_E_XXX
 */
int
_field_td2_action_support_check(int                unit,
                                _field_entry_t     *f_ent,
                                bcm_field_action_t action,
                                int                *result
                                )
{
    return (_field_td2_stage_action_support_check(unit,
                                                  f_ent->group->stage_id,
                                                  action,
                                                  result
                                                  )
            );
}
/*
 * Function:
 *     _bcm_field_td2_action_support_check
 * Purpose:
 *     Check if action is supported by device.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     f_ent    - (IN) Field entry structure to get policy info from.
 *     action   - (IN) Action to check(bcmFieldActionXXX)
 *     result -(OUT)              
 *               TRUE   - action is supported by device
 *               FALSE  - action is NOT supported by device
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_td2_action_support_check(int                unit,
                                    _field_entry_t     *f_ent,
                                    bcm_field_action_t action,
                                    int                *result
                                    )
{
    return _field_td2_action_support_check(unit, f_ent, action, result);  
}



/*
 * Function:
 *    _bcm_field_action_stat_conflict_check
 *
 * Purpose:
 *     Check if actions conflict stat actions.
 *
 * Parameters:
 *     unit    -(IN) BCM device number
 *     f_ent   -(IN) Field entry structure.
 *     fa      -(IN) Field Action Structure.
 *
 * Returns:
 *     BCM_E_PARAM  - if input arguments are NULL.
 *     BCM_E_CONFIG - if actions do conflict.
 *     BCM_E_NONE   - if there is no conflict.
 */

int _bcm_field_action_stat_conflict_check (int unit, _field_entry_t *f_ent, _field_action_t *fa)
{
    _field_entry_stat_t *f_ent_st = NULL;

    if ((fa == NULL) || (f_ent == NULL)) {
        return BCM_E_PARAM;
    }

    f_ent_st = &f_ent->statistic;

    switch (fa->action) {
        case bcmFieldActionAddClassTag:
        case bcmFieldActionL3ChangeVlan:
        case bcmFieldActionL3ChangeVlanCancel:
        case bcmFieldActionL3ChangeMacDa:
        case bcmFieldActionL3ChangeMacDaCancel:
        case bcmFieldActionL3Switch:
        case bcmFieldActionL3SwitchCancel:
        case bcmFieldActionSrcMacNew:
        case bcmFieldActionDstMacNew:
        case bcmFieldActionOuterVlanNew:
        case bcmFieldActionFabricQueue:
        case bcmFieldActionVnTagNew:
        case bcmFieldActionVnTagDelete:
        case bcmFieldActionEtagNew:
        case bcmFieldActionEtagDelete:
        case bcmFieldActionRedirectEgrNextHop:
        case bcmFieldActionNewClassId:
        case bcmFieldActionEgressClassSelect:
        case bcmFieldActionHiGigClassSelect:
            if (f_ent_st->flags & _FP_ENTRY_EXTENDED_STAT_VALID) {
#ifdef BROADCOM_DEBUG
                LOG_ERROR(BSL_LS_BCM_FP,
                        (BSL_META_U(unit,
                                    "FP(unit %d) Error: action=%s conflicts with"
                                    " statistics action in entry=%d\n"), unit,
                         _field_action_name(fa->action), f_ent->eid));
#endif /* BROADCOM_DEBUG */
                return (BCM_E_CONFIG);
            }
            break;
        case bcmFieldActionVxlanHeaderBits8_31_Set:
        case bcmFieldActionVxlanHeaderBits56_63_Set:
        case bcmFieldActionVxlanHeaderFlags:
            if (f_ent_st->flags & _FP_ENTRY_STAT_VALID) {
#ifdef BROADCOM_DEBUG
                LOG_ERROR(BSL_LS_BCM_FP,
                        (BSL_META_U(unit,
                                    "FP(unit %d) Error: action=%s conflicts with"
                                    " statistics action in entry=%d\n"), unit,
                         _field_action_name(fa->action), f_ent->eid));
#endif /* BROADCOM_DEBUG */
                return (BCM_E_CONFIG);
            }
            break;
        case bcmFieldActionDscpNew:
        case bcmFieldActionGpDscpNew:
        case bcmFieldActionYpDscpNew:
        case bcmFieldActionRpDscpNew:
        case bcmFieldActionDscpCancel:
        case bcmFieldActionGpDscpCancel:
        case bcmFieldActionYpDscpCancel:
        case bcmFieldActionRpDscpCancel:
            if (f_ent->group->stage_id == _BCM_FIELD_STAGE_EGRESS) {
                if (f_ent_st->flags & _FP_ENTRY_EXTENDED_STAT_VALID) {
#ifdef BROADCOM_DEBUG
                    LOG_ERROR(BSL_LS_BCM_FP,
                            (BSL_META_U(unit,
                                        "FP(unit %d) Error: action=%s conflicts with"
                                        " statistics action in entry=%d\n"), unit,
                             _field_action_name(fa->action), f_ent->eid));
#endif /* BROADCOM_DEBUG */
                    return (BCM_E_CONFIG);
                }
            }
            break;
        default:
            break;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   _bcm_field_action_stat_flag_update
 *
 * Purpose:
 *     Update Stat_allowed flag for Stats based on actions added.
 *
 * Parameters:
 *     unit            - (IN) BCM device number
 *     f_ent           - (IN) Field entry structure.
 *     fa              - (IN) Field Action Structure.
 *     is_action_added - (IN) Action added or removed.
 *
 * Returns:
 *     BCM_E_PARAM  - if input arguments are NULL.
 *     BCM_E_CONFIG - if actions do conflict.
 *     BCM_E_NONE   - if there is no conflict.
 */
int _bcm_field_action_stat_flag_update (int unit,
        _field_entry_t *f_ent,
        _field_action_t *fa,
        uint8 is_action_added)
{
    _field_entry_stat_t *f_ent_st = NULL;
    _field_action_t     *fa_cnt = NULL;
    int stat_conflict_count = 0;
    int extended_stat_conflict_count = 0;

    if ((fa == NULL) || (f_ent == NULL)) {
        return BCM_E_PARAM;
    }

    f_ent_st = &f_ent->statistic;

    if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit))
        || soc_feature(unit, soc_feature_field_multi_pipe_support)) {
        switch (fa->action) {
            case bcmFieldActionAddClassTag:
            case bcmFieldActionL3ChangeVlan:
            case bcmFieldActionL3ChangeVlanCancel:
            case bcmFieldActionL3ChangeMacDa:
            case bcmFieldActionL3ChangeMacDaCancel:
            case bcmFieldActionL3Switch:
            case bcmFieldActionL3SwitchCancel:
            case bcmFieldActionSrcMacNew:
            case bcmFieldActionDstMacNew:
            case bcmFieldActionOuterVlanNew:
            case bcmFieldActionFabricQueue:
            case bcmFieldActionVnTagNew:
            case bcmFieldActionVnTagDelete:
            case bcmFieldActionEtagNew:
            case bcmFieldActionEtagDelete:
            case bcmFieldActionRedirectEgrNextHop:
            case bcmFieldActionNewClassId:
            case bcmFieldActionEgressClassSelect:
            case bcmFieldActionHiGigClassSelect:
                if (is_action_added) {
                    f_ent_st->flags |=  _FP_ENTRY_EXTENDED_STAT_NOT_ALLOWED;
                } else {
                    fa_cnt = f_ent->actions;
                    while (fa_cnt != NULL) {
                        if ((fa_cnt->action == bcmFieldActionAddClassTag)
                                ||(fa_cnt->action == bcmFieldActionL3ChangeVlan)
                                ||(fa_cnt->action == bcmFieldActionL3ChangeVlanCancel)
                                ||(fa_cnt->action == bcmFieldActionL3ChangeMacDa)
                                ||(fa_cnt->action == bcmFieldActionL3ChangeMacDaCancel)
                                ||(fa_cnt->action == bcmFieldActionL3Switch)
                                ||(fa_cnt->action == bcmFieldActionL3SwitchCancel)
                                ||(fa_cnt->action == bcmFieldActionSrcMacNew)
                                ||(fa_cnt->action == bcmFieldActionDstMacNew)
                                ||(fa_cnt->action == bcmFieldActionOuterVlanNew)
                                ||(fa_cnt->action == bcmFieldActionFabricQueue)
                                ||(fa_cnt->action == bcmFieldActionVnTagNew)
                                ||(fa_cnt->action == bcmFieldActionVnTagDelete)
                                ||(fa_cnt->action == bcmFieldActionEtagNew)
                                ||(fa_cnt->action == bcmFieldActionEtagDelete)
                                ||(fa_cnt->action == bcmFieldActionRedirectEgrNextHop)
                                ||(fa_cnt->action == bcmFieldActionNewClassId)
                                ||(fa_cnt->action == bcmFieldActionEgressClassSelect)
                                ||(fa_cnt->action == bcmFieldActionHiGigClassSelect)
                           ) {
                            extended_stat_conflict_count++;
                        }
                        fa_cnt = fa_cnt->next;
                    }
                    if (extended_stat_conflict_count == 1) {
                        f_ent_st->flags &=
                            ~_FP_ENTRY_EXTENDED_STAT_NOT_ALLOWED;
                    }
                }
                break;
            case bcmFieldActionVxlanHeaderBits8_31_Set:
            case bcmFieldActionVxlanHeaderBits56_63_Set:
            case bcmFieldActionVxlanHeaderFlags:
                if (is_action_added) {
                    f_ent_st->flags |=  _FP_ENTRY_STAT_NOT_ALLOWED;
                } else {
                    fa_cnt = f_ent->actions;
                    while (fa_cnt != NULL) {
                        if ((fa_cnt->action
                                == bcmFieldActionVxlanHeaderBits8_31_Set) ||
                            (fa_cnt->action ==
                                    bcmFieldActionVxlanHeaderBits56_63_Set) ||
                            (fa_cnt->action ==
                                    bcmFieldActionVxlanHeaderFlags)) {
                            if (fa_cnt->flags  & _FP_ACTION_VALID) {
                                stat_conflict_count++;
                            }
                        }
                        fa_cnt = fa_cnt->next;
                    }
                    if (stat_conflict_count == 1) {
                        f_ent_st->flags &=  ~_FP_ENTRY_STAT_NOT_ALLOWED;
                    }
                }
                break;
            case bcmFieldActionDscpNew:
            case bcmFieldActionGpDscpNew:
            case bcmFieldActionYpDscpNew:
            case bcmFieldActionRpDscpNew:
            case bcmFieldActionDscpCancel:
            case bcmFieldActionGpDscpCancel:
            case bcmFieldActionYpDscpCancel:
            case bcmFieldActionRpDscpCancel:
                if (f_ent->group->stage_id == _BCM_FIELD_STAGE_EGRESS) {
                    if (is_action_added) {
                        f_ent_st->flags |=  _FP_ENTRY_EXTENDED_STAT_NOT_ALLOWED;
                    } else {
                        fa_cnt = f_ent->actions;
                        while (fa_cnt != NULL) {
                            if ((fa_cnt->action == bcmFieldActionDscpNew)
                                    ||(fa_cnt->action == bcmFieldActionGpDscpNew)
                                    ||(fa_cnt->action == bcmFieldActionYpDscpNew)
                                    ||(fa_cnt->action == bcmFieldActionRpDscpNew)
                                    ||(fa_cnt->action == bcmFieldActionDscpCancel)
                                    ||(fa_cnt->action == bcmFieldActionGpDscpCancel)
                                    ||(fa_cnt->action == bcmFieldActionYpDscpCancel)
                                    ||(fa_cnt->action == bcmFieldActionRpDscpCancel)
                               ) {
                                extended_stat_conflict_count++;
                            }
                            fa_cnt = fa_cnt->next;
                        }
                        if (extended_stat_conflict_count == 1) {
                            f_ent_st->flags &=
                                ~_FP_ENTRY_EXTENDED_STAT_NOT_ALLOWED;
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
    return BCM_E_NONE;
}

int
_bcm_field_td2_action_conflict_check ( int                unit,
                                       _field_entry_t     *f_ent,
                                       bcm_field_action_t action1,
                                       bcm_field_action_t action
                                     )
{
    int rv = BCM_E_NONE;

    /* Input parameters check */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }
    if (NULL == f_ent->group) {
        return (BCM_E_PARAM);
    }

    if (f_ent->group->stage_id == _BCM_FIELD_STAGE_LOOKUP) {
        switch (action1) {
#if defined (BCM_TRIDENT3_SUPPORT)
            case bcmFieldActionAssignOpaqueObject4:
            case bcmFieldActionAssignOpaqueObject3:
            case bcmFieldActionAssignOpaqueObject2:
            case bcmFieldActionAssignOpaqueObject1:
                _FP_ACTIONS_CONFLICT(bcmFieldActionAssignOpaqueObject4);
                _FP_ACTIONS_CONFLICT(bcmFieldActionAssignOpaqueObject3);
                _FP_ACTIONS_CONFLICT(bcmFieldActionAssignOpaqueObject2);
                _FP_ACTIONS_CONFLICT(bcmFieldActionAssignOpaqueObject1);
                rv = BCM_E_NONE;
                break;
#endif
            case bcmFieldActionIncomingMplsPortSet:
                _FP_ACTIONS_CONFLICT(bcmFieldActionIngressGportSet);
                _FP_ACTIONS_CONFLICT(bcmFieldActionDoNotCheckVlan);
                _FP_ACTIONS_CONFLICT(bcmFieldActionIncomingMplsPortSet);
                rv = BCM_E_NONE;
                break;
            case bcmFieldActionIngressGportSet:
                _FP_ACTIONS_CONFLICT(bcmFieldActionDoNotCheckVlan);
                _FP_ACTIONS_CONFLICT(bcmFieldActionIncomingMplsPortSet);
                _FP_ACTIONS_CONFLICT(bcmFieldActionIngressGportSet);
                rv = BCM_E_NONE;
                break;
            case bcmFieldActionL3IngressSet:
                _FP_ACTIONS_CONFLICT(bcmFieldActionDoNotCheckVlan);
                _FP_ACTIONS_CONFLICT(bcmFieldActionL3IngressSet);
                rv = BCM_E_NONE;
                break;
            default:
                rv = _bcm_field_trx_action_conflict_check (unit, f_ent, action1, action); 
                break;
        }
    } else if (f_ent->group->stage_id == _BCM_FIELD_STAGE_EGRESS) {
        switch (action1) {
            case bcmFieldActionVxlanHeaderBits8_31_Set:
            case bcmFieldActionVxlanHeaderBits56_63_Set:
            case bcmFieldActionVxlanHeaderFlags:
                _FP_ACTIONS_CONFLICT(action1);
                _FP_ACTIONS_CONFLICT(bcmFieldActionInnerVlanNew);
                _FP_ACTIONS_CONFLICT(bcmFieldActionInnerVlanPrioNew);
                _FP_ACTIONS_CONFLICT(bcmFieldActionOuterVlanNew);
                _FP_ACTIONS_CONFLICT(bcmFieldActionOuterTpidNew);
                break;
            case bcmFieldActionInnerVlanNew:
            case bcmFieldActionInnerVlanPrioNew:
            case bcmFieldActionOuterVlanNew:
            case bcmFieldActionOuterTpidNew:
                _FP_ACTIONS_CONFLICT(bcmFieldActionVxlanHeaderBits8_31_Set);
                _FP_ACTIONS_CONFLICT(bcmFieldActionVxlanHeaderBits56_63_Set);
                _FP_ACTIONS_CONFLICT(bcmFieldActionVxlanHeaderFlags);
                break;
            case bcmFieldActionLoopbackOamResponse:
            case bcmFieldActionLoopbackSatResponse:
                _FP_ACTIONS_CONFLICT(bcmFieldActionLoopbackOamResponse);
                _FP_ACTIONS_CONFLICT(bcmFieldActionLoopbackSatResponse);
                break;
            case bcmFieldActionMacSecEncryptPktType:
                _FP_ACTIONS_CONFLICT(bcmFieldActionMacSecDecryptPktType);
                _FP_ACTIONS_CONFLICT(bcmFieldActionMacSecEncryptPktType);
                break;
            case bcmFieldActionMacSecDecryptPktType:
                _FP_ACTIONS_CONFLICT(bcmFieldActionMacSecDecryptPktType);
                _FP_ACTIONS_CONFLICT(bcmFieldActionMacSecEncryptPktType);
                _FP_ACTIONS_CONFLICT(bcmFieldActionMacSecSubPortNumAdd);
                _FP_ACTIONS_CONFLICT(bcmFieldActionMacSecUseSubportNumberFromEvxlt);
                break;
            case bcmFieldActionMacSecSubPortNumAdd:
            case bcmFieldActionMacSecUseSubportNumberFromEvxlt:
                _FP_ACTIONS_CONFLICT(bcmFieldActionMacSecSubPortNumAdd);
                _FP_ACTIONS_CONFLICT(bcmFieldActionMacSecUseSubportNumberFromEvxlt);
                _FP_ACTIONS_CONFLICT(bcmFieldActionMacSecDecryptPktType);
                break;

            default:
                rv = _bcm_field_trx_action_conflict_check (unit, f_ent, action1, action);
                break;
        }
    } else { /* For all other stages */
        switch (action1) {
            case bcmFieldActionNat:
            case bcmFieldActionNatCancel:
                _FP_ACTIONS_CONFLICT(bcmFieldActionNat);
                _FP_ACTIONS_CONFLICT(bcmFieldActionNatCancel);
                if (soc_feature(unit, soc_feature_fp_based_oam)) {
                    _FP_ACTIONS_CONFLICT(bcmFieldActionOamDmTimeFormat);
                    _FP_ACTIONS_CONFLICT(bcmFieldActionOamOlpHeaderAdd);
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_XFLOW_MACSEC)
                    _FP_ACTIONS_CONFLICT(bcmFieldActionMacSecEncrypt);
                    _FP_ACTIONS_CONFLICT(bcmFieldActionMacSecDecrypt);
#endif
                }
                break;
            case bcmFieldActionNatEgressOverride:
                _FP_ACTIONS_CONFLICT(bcmFieldActionNatEgressOverride);
                if (soc_feature(unit, soc_feature_fp_based_oam)) {
                    _FP_ACTIONS_CONFLICT(bcmFieldActionOamDmTimeFormat);
                    _FP_ACTIONS_CONFLICT(bcmFieldActionOamOlpHeaderAdd);
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_XFLOW_MACSEC)
                    _FP_ACTIONS_CONFLICT(bcmFieldActionMacSecEncrypt);
                    _FP_ACTIONS_CONFLICT(bcmFieldActionMacSecDecrypt);
#endif
                }
                break;
            case bcmFieldActionOamDmTimeFormat:
                _FP_ACTIONS_CONFLICT(bcmFieldActionOamDmTimeFormat);
                if (soc_feature(unit, soc_feature_fp_based_oam)) {
                    _FP_ACTIONS_CONFLICT(bcmFieldActionNat);
                    _FP_ACTIONS_CONFLICT(bcmFieldActionNatCancel);
                    _FP_ACTIONS_CONFLICT(bcmFieldActionNatEgressOverride);
                }
                break;
            case bcmFieldActionOamOlpHeaderAdd:
                _FP_ACTIONS_CONFLICT(bcmFieldActionOamOlpHeaderAdd);
                if (soc_feature(unit, soc_feature_fp_based_oam)) {
                    _FP_ACTIONS_CONFLICT(bcmFieldActionNat);
                    _FP_ACTIONS_CONFLICT(bcmFieldActionNatCancel);
                    _FP_ACTIONS_CONFLICT(bcmFieldActionNatEgressOverride);
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_XFLOW_MACSEC)
                    _FP_ACTIONS_CONFLICT(bcmFieldActionMacSecEncrypt);
                    _FP_ACTIONS_CONFLICT(bcmFieldActionMacSecDecrypt);
#endif
                }
                break;
            case bcmFieldActionLoopbackOamResponse:
            case bcmFieldActionLoopbackSatResponse:
                _FP_ACTIONS_CONFLICT(bcmFieldActionLoopbackOamResponse);
                _FP_ACTIONS_CONFLICT(bcmFieldActionLoopbackSatResponse);
                break;
            default:
                rv = _bcm_field_trx_action_conflict_check (unit, f_ent, action1, action);
                break;

        }
    }

    return rv;
}

#ifdef BCM_APACHE_SUPPORT
STATIC int
_bcm_field_copytocpu_params_check(int     unit,
                              soc_mem_t       mem,
                              _field_entry_t  *f_ent,
                              _field_action_t *fa)
{
    uint32       flags;
    uint32       val;
    uint8        idx;
    uint32       green_flags  = BCM_FIELD_COPYTOCPU_GREEN_PACKET | \
                                BCM_FIELD_COPYTOCPU_GREEN_DROPPED_PACKET | \
                                BCM_FIELD_COPYTOCPU_GREEN_NOT_DROPPED_PACKET;
    uint32       red_flags    = BCM_FIELD_COPYTOCPU_RED_PACKET | \
                                BCM_FIELD_COPYTOCPU_RED_DROPPED_PACKET | \
                                BCM_FIELD_COPYTOCPU_RED_NOT_DROPPED_PACKET;
    uint32       yellow_flags = BCM_FIELD_COPYTOCPU_YELLOW_PACKET | \
                                BCM_FIELD_COPYTOCPU_YELLOW_DROPPED_PACKET | \
                                BCM_FIELD_COPYTOCPU_YELLOW_NOT_DROPPED_PACKET;

    if ((NULL == f_ent) || (NULL == fa)) {
        return (BCM_E_PARAM);
    }

    if (_BCM_FIELD_STAGE_EGRESS != f_ent->group->stage_id) {
        return BCM_E_PARAM;
    }

    flags = fa->param[2];

    /* Only one of the Green flag can be configured */
    val = flags & green_flags;
    if (val & (val -1)) {
        return BCM_E_PARAM;
    }

    /* Only one of the Red flag can be configured */
    val = flags & red_flags;
    if (val & (val -1)) {
        return BCM_E_PARAM;
    }

    /* Only one of the Yellow flag can be configured */
    val = flags & yellow_flags;
    if (val & (val -1)) {
        return BCM_E_PARAM;
    }

    for (idx = 0 ; idx < 32 ; idx++) {
        val = flags & (1 << idx);

        switch(val) {
            case BCM_FIELD_COPYTOCPU_GREEN_NOT_DROPPED_PACKET:
                PolicyCheck(unit, mem, G_COPY_TO_CPUf, 0x1);
                break;
            case BCM_FIELD_COPYTOCPU_GREEN_DROPPED_PACKET:
                PolicyCheck(unit, mem, G_COPY_TO_CPUf, 0x2);
                break;
            case BCM_FIELD_COPYTOCPU_GREEN_PACKET:
                PolicyCheck(unit, mem, G_COPY_TO_CPUf, 0x3);
                break;
            case BCM_FIELD_COPYTOCPU_YELLOW_NOT_DROPPED_PACKET:
                PolicyCheck(unit, mem, Y_COPY_TO_CPUf, 0x1);
                break;
            case BCM_FIELD_COPYTOCPU_YELLOW_DROPPED_PACKET:
                PolicyCheck(unit, mem, Y_COPY_TO_CPUf, 0x2);
                break;
            case BCM_FIELD_COPYTOCPU_YELLOW_PACKET:
                PolicyCheck(unit, mem, Y_COPY_TO_CPUf, 0x3);
                break;
            case BCM_FIELD_COPYTOCPU_RED_NOT_DROPPED_PACKET:
                PolicyCheck(unit, mem, R_COPY_TO_CPUf, 0x1);
                break;
            case BCM_FIELD_COPYTOCPU_RED_DROPPED_PACKET:
                PolicyCheck(unit, mem, R_COPY_TO_CPUf, 0x2);
                break;
            case BCM_FIELD_COPYTOCPU_RED_PACKET:
                PolicyCheck(unit, mem, R_COPY_TO_CPUf, 0x3);
                break;
            case BCM_FIELD_COPYTOCPU_TRUNCATE:
                PolicyCheck(unit, mem, COPY_TO_CPU_FIRST_CELL_ONLYf, 0x1);
                break;
            case 0:
                break;
            default:
                return BCM_E_PARAM;
                break;
        }
    }

    if (fa->param[3]) {
        PolicyCheck(unit, mem, MATCHED_RULEf, fa->param[3]);
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_field_copytocpu_action_get(int       unit,
                          soc_mem_t       mem,
                          _field_entry_t  *f_ent,
                          _field_action_t *fa,
                          uint32          *buf)
{
    uint8        idx;
    uint32       val;
    uint32       flags;

    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    if (_BCM_FIELD_STAGE_EGRESS != f_ent->group->stage_id) {
        return BCM_E_PARAM;
    }

    flags = fa->param[2];

    for (idx = 0 ; idx < 32 ; idx++) {
        val = flags & (1 << idx);

        switch(val) {
            case BCM_FIELD_COPYTOCPU_GREEN_NOT_DROPPED_PACKET:
                PolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x1);
                break;
            case BCM_FIELD_COPYTOCPU_GREEN_DROPPED_PACKET:
                PolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x2);
                break;
            case BCM_FIELD_COPYTOCPU_GREEN_PACKET:
                PolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x3);
                break;
            case BCM_FIELD_COPYTOCPU_YELLOW_NOT_DROPPED_PACKET:
                PolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x1);
                break;
            case BCM_FIELD_COPYTOCPU_YELLOW_DROPPED_PACKET:
                PolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x2);
                break;
            case BCM_FIELD_COPYTOCPU_YELLOW_PACKET:
                PolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x3);
                break;
            case BCM_FIELD_COPYTOCPU_RED_NOT_DROPPED_PACKET:
                PolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x1);
                break;
            case BCM_FIELD_COPYTOCPU_RED_DROPPED_PACKET:
                PolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x2);
                break;
            case BCM_FIELD_COPYTOCPU_RED_PACKET:
                PolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x3);
                break;
            case BCM_FIELD_COPYTOCPU_TRUNCATE:
                PolicySet(unit, mem, buf, COPY_TO_CPU_FIRST_CELL_ONLYf, 0x1);
                break;
            case 0:
                break;
            default:
                return BCM_E_PARAM;
                break;
        }
    }

    if (fa->param[3]) {
        PolicySet(unit, mem, buf, MATCHED_RULEf, fa->param[3]);
    }

    return (BCM_E_NONE);
}

STATIC int
_bcm_field_redirect_params_check(int     unit,
                              soc_mem_t       mem,
                              _field_entry_t  *f_ent,
                              _field_action_t *fa)
{
    uint32       flags;
    uint32       val;
    uint8        idx;
    uint32       green_flags     = BCM_FIELD_REDIRECT_GREEN_PACKET | \
                                   BCM_FIELD_REDIRECT_GREEN_DROPPED_PACKET | \
                                   BCM_FIELD_REDIRECT_GREEN_NOT_DROPPED_PACKET;
    uint32       red_flags       = BCM_FIELD_REDIRECT_RED_PACKET | \
                                   BCM_FIELD_REDIRECT_RED_DROPPED_PACKET | \
                                   BCM_FIELD_REDIRECT_RED_NOT_DROPPED_PACKET;
    uint32       yellow_flags    = BCM_FIELD_REDIRECT_YELLOW_PACKET | \
                                   BCM_FIELD_REDIRECT_YELLOW_DROPPED_PACKET | \
                                   BCM_FIELD_REDIRECT_YELLOW_NOT_DROPPED_PACKET;
    uint32       redir_src_flags = BCM_FIELD_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE | \
                                   BCM_FIELD_REDIRECT_SOURCE_USE_ORIGINAL_DESTINATION | \
                                   BCM_FIELD_REDIRECT_SOURCE_USE_CONFIGURED;
    uint32       redir_field     = 0;
    uint32       redir_type      = 0;

    if ((NULL == f_ent) || (NULL == fa)) {
        return (BCM_E_PARAM);
    }

    if (_BCM_FIELD_STAGE_EGRESS != f_ent->group->stage_id) {
        return BCM_E_PARAM;
    }

    flags = fa->param[2];

    /* Only one of the Green flag can be configured */
    val = flags & green_flags;
    if (val & (val -1)) {
        return BCM_E_PARAM;
    }

    /* Only one of the Red flag can be configured */
    val = flags & red_flags;
    if (val & (val -1)) {
        return BCM_E_PARAM;
    }

    /* Only one of the Yellow flag can be configured */
    val = flags & yellow_flags;
    if (val & (val -1)) {
        return BCM_E_PARAM;
    }

    /* Only one of the Source port flag can be configured */
    val = flags & redir_src_flags;
    if(val & (val -1)) {
        return BCM_E_PARAM;
    }

    for (idx = 0 ; idx < 32 ; idx++) {
        val = flags & (1 << idx);

        switch(val) {
            case BCM_FIELD_REDIRECT_GREEN_NOT_DROPPED_PACKET:
                PolicyCheck(unit, mem, G_REDIRECTf, 0x1);
                break;
            case BCM_FIELD_REDIRECT_GREEN_DROPPED_PACKET:
                PolicyCheck(unit, mem, G_REDIRECTf, 0x2);
                break;
            case BCM_FIELD_REDIRECT_GREEN_PACKET:
                PolicyCheck(unit, mem, G_REDIRECTf, 0x3);
                break;
            case BCM_FIELD_REDIRECT_YELLOW_NOT_DROPPED_PACKET:
                PolicyCheck(unit, mem, Y_REDIRECTf, 0x1);
                break;
            case BCM_FIELD_REDIRECT_YELLOW_DROPPED_PACKET:
                PolicyCheck(unit, mem, Y_REDIRECTf, 0x2);
                break;
            case BCM_FIELD_REDIRECT_YELLOW_PACKET:
                PolicyCheck(unit, mem, Y_REDIRECTf, 0x3);
                break;
            case BCM_FIELD_REDIRECT_RED_NOT_DROPPED_PACKET:
                PolicyCheck(unit, mem, R_REDIRECTf, 0x1);
                break;
            case BCM_FIELD_REDIRECT_RED_DROPPED_PACKET:
                PolicyCheck(unit, mem, R_REDIRECTf, 0x2);
                break;
            case BCM_FIELD_REDIRECT_RED_PACKET:
                PolicyCheck(unit, mem, R_REDIRECTf, 0x3);
                break;
            case BCM_FIELD_REDIRECT_SOURCE_USE_ORIGINAL_DESTINATION:
                PolicyCheck(unit, mem, REDIR_SOURCEf, 0x1);
                break;
            case BCM_FIELD_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE:
                PolicyCheck(unit, mem, REDIR_SOURCEf, 0x2);
                break;
            case BCM_FIELD_REDIRECT_SOURCE_USE_CONFIGURED:
                PolicyCheck(unit, mem, REDIR_SOURCEf, 0x3);
                break;
            case BCM_FIELD_REDIRECT_TRUNCATE:
                PolicyCheck(unit, mem, REDIR_FIRST_CELL_ONLYf, 0x1);
                break;
            case 0:
                break;
            default:
                return BCM_E_PARAM;
                break;
        }
    }

    /* Verify destination type and destination value */
    if (fa->param[4] == BCM_GPORT_INVALID) {
        return BCM_E_PARAM;
    }
    switch(fa->param[3]) {
        case bcmFieldRedirectDestinationPort:
            redir_type   = 0x5;
            if (BCM_GPORT_IS_MODPORT(fa->param[4])) {
                redir_field  = BCM_GPORT_MODPORT_PORT_GET((int)fa->param[4]) & 0xff;
                redir_field |= ((BCM_GPORT_MODPORT_MODID_GET((int)fa->param[4]) & 0xff) << 8);
            } else {
                return BCM_E_PARAM;
            }
            break;
        case bcmFieldRedirectDestinationTrunk:
            redir_type   = 0x6;
            if (BCM_GPORT_IS_TRUNK(fa->param[4])) {
                redir_field  = BCM_GPORT_TRUNK_GET((int)fa->param[4]) & 0x1ff;
            } else if (BCM_GPORT_IS_SET(fa->param[4])) {
                return BCM_E_PARAM;
            } else {
                redir_field = fa->param[4];
            }
            break;
        case bcmFieldRedirectDestinationMCast:
            redir_type   = 0x7;
            if (_BCM_MULTICAST_IS_SET(fa->param[4])) {
                if (0 == _BCM_MULTICAST_IS_L2(fa->param[4])) {
                    return (BCM_E_PARAM);
                }
                redir_field  = _BCM_MULTICAST_ID_GET(fa->param[4]);
            } else {
                return BCM_E_PARAM;
            }
            break;
        default :
            return BCM_E_PARAM;
            break;
    }
    PolicyCheck(unit, mem, REDIRECTION_DEST_TYPEf, redir_type);
    PolicyCheck(unit, mem, REDIRECTION_DESTINATIONf, redir_field);

    /* Verify source type */
    if (flags & BCM_FIELD_REDIRECT_SOURCE_USE_CONFIGURED) {
        redir_type   = 0x1;
        if (BCM_GPORT_IS_MODPORT(fa->param[5])) {
            redir_type   = 0x0;
            redir_field  = BCM_GPORT_MODPORT_PORT_GET((int)fa->param[5]) & 0xff;
            redir_field |= ((BCM_GPORT_MODPORT_MODID_GET((int)fa->param[5]) & 0xff) << 8);
        } else if (BCM_GPORT_IS_MPLS_PORT(fa->param[5])) {
            redir_field  = BCM_GPORT_MPLS_PORT_ID_GET((int)fa->param[5]) & 0xffff;
        } else if (BCM_GPORT_IS_NIV_PORT(fa->param[5])) {
            redir_field  = BCM_GPORT_NIV_PORT_ID_GET((int)fa->param[5]) & 0xffff;
        } else if (BCM_GPORT_IS_MIM_PORT(fa->param[5])) {
            redir_field  = BCM_GPORT_MIM_PORT_ID_GET((int)fa->param[5]) & 0xffff;
        } else if (BCM_GPORT_IS_TRILL_PORT(fa->param[5])) {
            redir_field  = BCM_GPORT_TRILL_PORT_ID_GET((int)fa->param[5]) & 0xffff;
        } else if (BCM_GPORT_IS_L2GRE_PORT(fa->param[5])) {
            redir_field  = BCM_GPORT_L2GRE_PORT_ID_GET((int)fa->param[5]) & 0xffff;
        } else if (BCM_GPORT_IS_VXLAN_PORT(fa->param[5])) {
            redir_field  = BCM_GPORT_VXLAN_PORT_ID_GET((int)fa->param[5]) & 0xffff;
        } else {
            return BCM_E_PARAM;
        }
        PolicyCheck(unit, mem, REDIR_SOURCE_TYPEf, redir_type);
        PolicyCheck(unit, mem, REDIR_SOURCE_VALUEf, redir_field);
    }

    /* Set DROP orignial to all EP redirection */
    PolicyCheck(unit, mem, DROP_ORIGINAL_PACKETf, 0x1);

    return BCM_E_NONE;
}

STATIC int
_bcm_field_redirect_action_get(int       unit,
                          soc_mem_t       mem,
                          _field_entry_t  *f_ent,
                          _field_action_t *fa,
                          uint32          *buf)
{
    uint8        idx;
    uint32       val;
    uint32       flags;
    uint32       redir_field     = 0;
    uint32       redir_type      = 0;

    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    if (_BCM_FIELD_STAGE_EGRESS != f_ent->group->stage_id) {
        return BCM_E_PARAM;
    }

    flags = fa->param[2];

    for (idx = 0 ; idx < 32 ; idx++) {
        val = flags & (1 << idx);

        switch(val) {
            case BCM_FIELD_REDIRECT_GREEN_NOT_DROPPED_PACKET:
                PolicySet(unit, mem, buf, G_REDIRECTf, 0x1);
                break;
            case BCM_FIELD_REDIRECT_GREEN_DROPPED_PACKET:
                PolicySet(unit, mem, buf, G_REDIRECTf, 0x2);
                break;
            case BCM_FIELD_REDIRECT_GREEN_PACKET:
                PolicySet(unit, mem, buf, G_REDIRECTf, 0x3);
                break;
            case BCM_FIELD_REDIRECT_YELLOW_NOT_DROPPED_PACKET:
                PolicySet(unit, mem, buf, Y_REDIRECTf, 0x1);
                break;
            case BCM_FIELD_REDIRECT_YELLOW_DROPPED_PACKET:
                PolicySet(unit, mem, buf, Y_REDIRECTf, 0x2);
                break;
            case BCM_FIELD_REDIRECT_YELLOW_PACKET:
                PolicySet(unit, mem, buf, Y_REDIRECTf, 0x3);
                break;
            case BCM_FIELD_REDIRECT_RED_NOT_DROPPED_PACKET:
                PolicySet(unit, mem, buf, R_REDIRECTf, 0x1);
                break;
            case BCM_FIELD_REDIRECT_RED_DROPPED_PACKET:
                PolicySet(unit, mem, buf, R_REDIRECTf, 0x2);
                break;
            case BCM_FIELD_REDIRECT_RED_PACKET:
                PolicySet(unit, mem, buf, R_REDIRECTf, 0x3);
                break;
            case BCM_FIELD_REDIRECT_SOURCE_USE_ORIGINAL_DESTINATION:
                PolicySet(unit, mem, buf, REDIR_SOURCEf, 0x1);
                break;
            case BCM_FIELD_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE:
                PolicySet(unit, mem, buf, REDIR_SOURCEf, 0x2);
                break;
            case BCM_FIELD_REDIRECT_SOURCE_USE_CONFIGURED:
                PolicySet(unit, mem, buf, REDIR_SOURCEf, 0x3);
                break;
            case BCM_FIELD_REDIRECT_TRUNCATE:
                PolicySet(unit, mem, buf, REDIR_FIRST_CELL_ONLYf, 0x1);
                break;
            case 0:
                break;
            default:
                return BCM_E_PARAM;
                break;
        }
    }

    /* Configure destination type and destination value */
    switch(fa->param[3]) {
        case bcmFieldRedirectDestinationPort:
            redir_type   = 0x5;
            if (BCM_GPORT_IS_MODPORT(fa->param[4])) {
                redir_field  = BCM_GPORT_MODPORT_PORT_GET((int)fa->param[4]) & 0xff;
                redir_field |= ((BCM_GPORT_MODPORT_MODID_GET((int)fa->param[4]) & 0xff) << 8);
            } else {
                return BCM_E_PARAM;
            }
            break;
        case bcmFieldRedirectDestinationTrunk:
            redir_type   = 0x6;
            if (BCM_GPORT_IS_TRUNK(fa->param[4])) {
                redir_field  = BCM_GPORT_TRUNK_GET((int)fa->param[4]) & 0x1ff;
            } else if (BCM_GPORT_IS_SET(fa->param[4])) {
                return BCM_E_PARAM;
            } else {
                redir_field = fa->param[4];
            }
            break;
        case bcmFieldRedirectDestinationMCast:
            redir_type   = 0x7;
            if (_BCM_MULTICAST_IS_SET(fa->param[4])) {
                if (0 == _BCM_MULTICAST_IS_L2(fa->param[4])) {
                    return (BCM_E_PARAM);
                }
                redir_field  = _BCM_MULTICAST_ID_GET(fa->param[4]);
            } else {
                return BCM_E_PARAM;
            }
            break;
        default :
            return BCM_E_PARAM;
            break;
    }
    PolicySet(unit, mem, buf, REDIRECTION_DEST_TYPEf, redir_type);
    PolicySet(unit, mem, buf, REDIRECTION_DESTINATIONf, redir_field);

    /* Configure source type and source value */
    if (flags & BCM_FIELD_REDIRECT_SOURCE_USE_CONFIGURED) {
        redir_type   = 0x1;
        if (BCM_GPORT_IS_MODPORT(fa->param[5])) {
            redir_type   = 0x0;
            redir_field  = BCM_GPORT_MODPORT_PORT_GET((int)fa->param[5]) & 0xff;
            redir_field |= ((BCM_GPORT_MODPORT_MODID_GET((int)fa->param[5]) & 0xff) << 8);
        } else if (BCM_GPORT_IS_MPLS_PORT(fa->param[5])) {
            redir_field  = BCM_GPORT_MPLS_PORT_ID_GET((int)fa->param[5]) & 0xffff;
        } else if (BCM_GPORT_IS_NIV_PORT(fa->param[5])) {
            redir_field  = BCM_GPORT_NIV_PORT_ID_GET((int)fa->param[5]) & 0xffff;
        } else if (BCM_GPORT_IS_MIM_PORT(fa->param[5])) {
            redir_field  = BCM_GPORT_MIM_PORT_ID_GET((int)fa->param[5]) & 0xffff;
        } else if (BCM_GPORT_IS_TRILL_PORT(fa->param[5])) {
            redir_field  = BCM_GPORT_TRILL_PORT_ID_GET((int)fa->param[5]) & 0xffff;
        } else if (BCM_GPORT_IS_L2GRE_PORT(fa->param[5])) {
            redir_field  = BCM_GPORT_L2GRE_PORT_ID_GET((int)fa->param[5]) & 0xffff;
        } else if (BCM_GPORT_IS_VXLAN_PORT(fa->param[5])) {
            redir_field  = BCM_GPORT_VXLAN_PORT_ID_GET((int)fa->param[5]) & 0xffff;
        } else {
            return BCM_E_PARAM;
        }
        PolicySet(unit, mem, buf, REDIR_SOURCE_TYPEf, redir_type);
        PolicySet(unit, mem, buf, REDIR_SOURCE_VALUEf, redir_field);
    }

    /* Set DROP original to all EP redirection */
    PolicySet(unit, mem, buf, DROP_ORIGINAL_PACKETf, 0x1);

    return (BCM_E_NONE);
}

#endif

int
_field_td2_action_params_check(int             unit,
                               _field_entry_t  *f_ent,
                               _field_action_t *fa
                               )
{
    soc_mem_t mem;              /* Policy table memory id. */
    soc_mem_t tcam_mem;         /* Tcam memory id.         */
    int hw_index = 0;
    int rv = 0;
    int cosq_new;
#if defined(BCM_APACHE_SUPPORT)
    int8 flex_stat_pool_id = 0;
#endif

    if (_BCM_FIELD_STAGE_EXTERNAL != f_ent->group->stage_id) {

        BCM_IF_ERROR_RETURN(_field_fb_tcam_policy_mem_get(unit,
            f_ent->group->stage_id, &tcam_mem, &mem));

        switch (fa->action) {
            case bcmFieldActionFibreChanSrcBindEnable:
                PolicyCheck(unit, mem, FCOE_SRC_BIND_CHECK_ENABLEf, fa->param[0]);
                return (BCM_E_NONE);
            case bcmFieldActionFibreChanFpmaPrefixCheckEnable:
                PolicyCheck(unit, mem, FCOE_SRC_FPMA_PREFIX_CHECK_ENABLEf,
                            fa->param[0]);
                return (BCM_E_NONE);
            case bcmFieldActionFibreChanZoneCheckEnable:
                PolicyCheck(unit, mem, FCOE_ZONE_CHECK_ENABLEf, fa->param[0]);
                return (BCM_E_NONE);
            case bcmFieldActionFibreChanVsanId:
                PolicyCheck(unit, mem, FCOE_VSAN_IDf, fa->param[0]);
                return (BCM_E_NONE);
            case bcmFieldActionFibreChanZoneCheckActionCancel:
                PolicyCheck(unit, mem, FCOE_ZONE_CHECK_ACTIONf, fa->param[0]);
                return (BCM_E_NONE);
            case bcmFieldActionNewClassId:
                PolicyCheck(unit, mem, I2E_CLASSIDf, fa->param[0]);
                return (BCM_E_NONE);
            case bcmFieldActionEgressClassSelect:
                return (fa->param[0] <= BCM_FIELD_EGRESS_CLASS_SELECT_NEW
                    ? BCM_E_NONE : BCM_E_PARAM
                    );
            case bcmFieldActionHiGigClassSelect:
                return (fa->param[0] <= BCM_FIELD_HIGIG_CLASS_SELECT_PORT
                    ? BCM_E_NONE : BCM_E_PARAM
                    );
            case bcmFieldActionFibreChanIntVsanPri:
                PolicyCheck(unit, mem, FCOE_VSAN_PRIf, fa->param[0]);
                return (BCM_E_NONE);
            case bcmFieldActionHashSelect0:
            /* Intentional fall through case */
            /* coverity [unterminated_case] */
            case bcmFieldActionHashSelect1:
                if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
                    if (fa->param[0] == 0x0) {
                       /*
                        *  BITMAP 0x0 is assigned to reserved entry in the
                        *  VFP_HAS_FIELD_BMAP_TABLE_A/B to skip the 0th entry.
                        */
                       return BCM_E_PARAM;
                    }
                    mem = (fa->action == bcmFieldActionHashSelect0)? 
                                          VFP_HASH_FIELD_BMAP_TABLE_Am:
                                          VFP_HASH_FIELD_BMAP_TABLE_Bm;

                    PolicyCheck(unit, mem, BITMAPf, fa->param[0]);
                    return (BCM_E_NONE);
                }
                break;
            case bcmFieldActionEtagNew:
                    PolicyCheck(unit, EGR_L3_NEXT_HOPm, IFP_ACTIONS__VNTAGf,
                                fa->param[0]);              
                    return (BCM_E_NONE);                                        
                break;
            case bcmFieldActionVxlanHeaderBits8_31_Set:
                PolicyCheck(unit, mem, VXLAN_RESERVED_1f, fa->param[0]);
                return (BCM_E_NONE);
                break;
            case bcmFieldActionVxlanHeaderBits56_63_Set:
                PolicyCheck(unit, mem, VXLAN_RESERVED_2f, fa->param[0]);
                return (BCM_E_NONE);
                break;
            case bcmFieldActionVxlanHeaderFlags:
                PolicyCheck(unit, mem, VXLAN_FLAGSf, fa->param[0]);
                return (BCM_E_NONE);
                break;
            case bcmFieldActionOamDmTimeFormat:
                if (soc_feature(unit, soc_feature_fp_based_oam)) {
                    PolicyCheck(unit, mem, SAMPLE_TIMESTAMPf, fa->param[0]);
                    return BCM_E_NONE;
                }
                break;
            case bcmFieldActionOamOlpHeaderAdd:
                if (soc_feature(unit, soc_feature_fp_based_oam)) {
                    if ((fa->param[0] >> 1) == 0xB) {
                        return BCM_E_PARAM;
                    }
                    rv = _bcm_oam_olp_fp_hw_index_get (unit,fa->param[0],
                            &hw_index);
                    if (BCM_FAILURE(rv)) {
                        return BCM_E_PARAM;
                    }
                    PolicyCheck(unit, mem, OLP_HDR_TYPE_COMPRESSEDf, hw_index);
                    return BCM_E_NONE;
                }
                break;

            case bcmFieldActionUcastQueueNew:
                rv = _bcm_td2_cosq_index_resolve(unit, fa->param[0], 0,
                        _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                        &cosq_new, NULL);
                if (BCM_FAILURE(rv)) {
                    return BCM_E_PARAM;
                }

                PolicyCheck(unit, mem, R_COS_INT_PRIf, cosq_new);
                PolicyCheck(unit, mem, Y_COS_INT_PRIf, cosq_new);
                PolicyCheck(unit, mem, G_COS_INT_PRIf, cosq_new);
                return (BCM_E_NONE);
                break;

            case bcmFieldActionRpUcastQueueNew:
                rv = _bcm_td2_cosq_index_resolve(unit, fa->param[0], 0,
                        _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                        &cosq_new, NULL);
                if (BCM_FAILURE(rv)) {
                    return BCM_E_PARAM;
                }

                PolicyCheck(unit, mem, R_COS_INT_PRIf, cosq_new);
                return (BCM_E_NONE);
                break;
            case bcmFieldActionYpUcastQueueNew:
                rv = _bcm_td2_cosq_index_resolve(unit, fa->param[0], 0,
                        _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                        &cosq_new, NULL);
                if (BCM_FAILURE(rv)) {
                    return BCM_E_PARAM;
                }

                PolicyCheck(unit, mem, Y_COS_INT_PRIf, cosq_new);
                return (BCM_E_NONE);
                break;
            case bcmFieldActionGpUcastQueueNew:
                rv = _bcm_td2_cosq_index_resolve(unit, fa->param[0], 0,
                        _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                        &cosq_new, NULL);
                if (BCM_FAILURE(rv)) {
                    return BCM_E_PARAM;
                }

                PolicyCheck(unit, mem, G_COS_INT_PRIf, cosq_new);
                return (BCM_E_NONE);
                break;
            case bcmFieldActionIntCosUcastQueueNew:
                rv = _bcm_td2_cosq_index_resolve(unit, fa->param[0], 0,
                        _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                        &cosq_new, NULL);
                if (BCM_FAILURE(rv)) {
                    return BCM_E_PARAM;
                }

                PolicyCheck(unit, mem, R_COS_INT_PRIf,
                        (_BCM_COSQ_CLASSIFIER_FIELD_GET(fa->param[1]) << 12 |
                         cosq_new));

                PolicyCheck(unit, mem, Y_COS_INT_PRIf,
                        (_BCM_COSQ_CLASSIFIER_FIELD_GET(fa->param[1]) << 12 |
                         cosq_new));
                PolicyCheck(unit, mem, G_COS_INT_PRIf,
                        (_BCM_COSQ_CLASSIFIER_FIELD_GET(fa->param[1]) << 12 |
                         cosq_new));
                return (BCM_E_NONE);
                break;

            case bcmFieldActionRpIntCosUcastQueueNew:
                rv = _bcm_td2_cosq_index_resolve(unit, fa->param[0], 0,
                        _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                        &cosq_new, NULL);
                if (BCM_FAILURE(rv)) {
                    return BCM_E_PARAM;
                }

                PolicyCheck(unit, mem, R_COS_INT_PRIf,
                        (_BCM_COSQ_CLASSIFIER_FIELD_GET(fa->param[1]) << 12 |
                         cosq_new));
                return (BCM_E_NONE);
                break;
            case bcmFieldActionYpIntCosUcastQueueNew:
                rv = _bcm_td2_cosq_index_resolve(unit, fa->param[0], 0,
                        _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                        &cosq_new, NULL);
                if (BCM_FAILURE(rv)) {
                    return BCM_E_PARAM;
                }

                PolicyCheck(unit, mem, Y_COS_INT_PRIf,
                        (_BCM_COSQ_CLASSIFIER_FIELD_GET(fa->param[1]) << 12 |
                         cosq_new));
                return (BCM_E_NONE);
                break;

            case bcmFieldActionGpIntCosUcastQueueNew:
                rv = _bcm_td2_cosq_index_resolve(unit, fa->param[0], 0,
                        _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                        &cosq_new, NULL);
                if (BCM_FAILURE(rv)) {
                    return BCM_E_PARAM;
                }

                PolicyCheck(unit, mem, G_COS_INT_PRIf,
                        (_BCM_COSQ_CLASSIFIER_FIELD_GET(fa->param[1]) << 12 |
                         cosq_new));
                return (BCM_E_NONE);
                break;

            case bcmFieldActionDoNotCutThrough:
                return BCM_E_NONE;
                break;

#if defined(BCM_APACHE_SUPPORT)
            case bcmFieldActionStrength:
                PolicyCheck(unit, mem, REDIR_STRENGTHf, fa->param[0]);
                return BCM_E_NONE;
                break;
            case bcmFieldActionRedirectBufferPriority:
                PolicyCheck(unit, mem, REDIR_RDB_PRIORITYf, fa->param[0]);
                return BCM_E_NONE;
                break;
            case bcmFieldActionOamStatObjectSessionId:
                BCM_IF_ERROR_RETURN(_field_oam_lm_stat_pool_id_get(unit, fa->param[0],
                            0, &flex_stat_pool_id));
                PolicyCheck(unit, mem, SESSION_ID_POOL_SELECTORf, flex_stat_pool_id);
                return BCM_E_NONE;
                break;
            case bcmFieldActionCopyToCpu:
                if (SOC_IS_APACHE(unit) &&
                    (f_ent->group->stage_id == _BCM_FIELD_STAGE_EGRESS) &&
                    (fa->param[2])) {
                    return (_bcm_field_copytocpu_params_check(unit, mem, f_ent, fa));
                }
                break;
            case bcmFieldActionRedirect:
                if (SOC_IS_APACHE(unit) &&
                    (f_ent->group->stage_id == _BCM_FIELD_STAGE_EGRESS) &&
                    (fa->param[2])) {
                    return (_bcm_field_redirect_params_check(unit, mem, f_ent, fa));
                }
                break;
#endif /* BCM_APACHE_SUPPORT */

            default:
                ;
        }
    }

    return (_bcm_field_trx_action_params_check(unit, f_ent, fa));
}

int
_bcm_field_td2_action_params_check(int        unit,
                               _field_entry_t  *f_ent,
                               _field_action_t *fa
                               )
{
    return _field_td2_action_params_check(unit, f_ent, fa);
} 
STATIC int
_field_td2_ingress_qual_tcam_key_mask_get(int unit,
                                          _field_entry_t *f_ent,
                                          _field_tcam_t *tcam
                                          )
{
    int errcode = BCM_E_INTERNAL;
    soc_mem_t mem;
    const unsigned fp_tcam_words
        = BITS2WORDS(soc_mem_field_length(unit,
                                          FP_TCAMm,
                                          KEYf
                                          )
                     );

    tcam->key_size
        = WORDS2BYTES(fp_tcam_words
                      + BITS2WORDS(soc_mem_field_length(
                                       unit,
                                       FP_GLOBAL_MASK_TCAMm,
                                       KEYf
                                                        )
                                   )
                      );
    tcam->key  = sal_alloc(tcam->key_size, " ");
    tcam->mask = sal_alloc(tcam->key_size, " ");
    if (tcam->key == 0 || tcam->mask == 0) {
        errcode = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(tcam->key,  0, tcam->key_size);
    sal_memset(tcam->mask, 0, tcam->key_size);

    if (f_ent->flags & _FP_ENTRY_INSTALLED) {
        int    tcam_idx;
        uint32 tcam_entry[SOC_MAX_MEM_FIELD_WORDS];

        sal_memset(tcam_entry, 0, sizeof(tcam_entry));

        errcode = _bcm_field_entry_tcam_idx_get(unit, f_ent, &tcam_idx);
        if (BCM_FAILURE(errcode)) {
            goto error;
        }

        errcode = soc_mem_read(unit,
                               FP_TCAMm,
                               MEM_BLOCK_ANY,
                               tcam_idx,
                               tcam_entry
                               );
        if (BCM_FAILURE(errcode)) {
            goto error;
        }
        soc_mem_field_get(unit, FP_TCAMm, tcam_entry, KEYf, tcam->key);
        soc_mem_field_get(unit, FP_TCAMm, tcam_entry, MASKf, tcam->mask);
        
        if (f_ent->flags & _FP_ENTRY_SECONDARY) {
            mem = FP_GM_FIELDSm;
        } else {
            mem = FP_GLOBAL_MASK_TCAMm;
        }

        errcode = soc_mem_read(unit,
                               mem,
                               MEM_BLOCK_ANY,
                               tcam_idx,
                               tcam_entry
                               );
        if (BCM_FAILURE(errcode)) {
            goto error;
        }

        soc_mem_field_get(unit,
                          mem,
                          tcam_entry,
                          KEYf,
                          tcam->key + fp_tcam_words
                          );
        soc_mem_field_get(unit,
                          mem,
                          tcam_entry,
                          MASKf,
                          tcam->mask + fp_tcam_words
                          );
    }

    return (BCM_E_NONE);

 error:
    if (tcam->key) {
        sal_free(tcam->key);
        tcam->key = 0;
    }
    if (tcam->mask) {
        sal_free(tcam->mask);
        tcam->mask = 0;
    }

    return (errcode);
}

int
_bcm_field_td2_qual_tcam_key_mask_get(int unit,
                                      _field_entry_t *f_ent,
                                      _field_tcam_t *tcam
                                      )
{
    switch (f_ent->group->stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
        return (_field_td2_ingress_qual_tcam_key_mask_get(unit, f_ent, tcam));
    default:
        ;
    }

    return (_field_qual_tcam_key_mask_get(unit, f_ent, tcam, 0));
}

STATIC int
_field_td2_ingress_qual_tcam_key_mask_set(int            unit,
                                          _field_entry_t *f_ent,
                                          unsigned       tcam_idx,
                                          unsigned       validf
                                          )
{
    uint32  e[SOC_MAX_MEM_FIELD_WORDS] = {0}; /* Buffer to fill Policy & TCAM entry.*/
    _field_tcam_t  * const tcam = &f_ent->tcam;
    _field_group_t * const fg = f_ent->group;
    const unsigned fp_tcam_words
        = BITS2WORDS(soc_mem_field_length(unit,
                                          FP_TCAMm,
                                          KEYf
                                          )
                     );
    fp_global_mask_tcam_entry_t tcam_entry;
    fp_global_mask_tcam_x_entry_t tcam_entry_x;
    fp_global_mask_tcam_y_entry_t tcam_entry_y;
    bcm_pbmp_t pbmp_x;
    bcm_pbmp_t pbmp_y;

    sal_memset(&tcam_entry, 0, sizeof(fp_global_mask_tcam_entry_t));

    BCM_IF_ERROR_RETURN(soc_mem_read(unit,
                                     FP_TCAMm,
                                     MEM_BLOCK_ANY,
                                     tcam_idx,
                                     e
                                     )
                        );
    soc_mem_field_set(unit, FP_TCAMm, e, KEYf, tcam->key);
    soc_mem_field_set(unit, FP_TCAMm, e, MASKf, tcam->mask);
    soc_mem_field32_set(unit,
                        FP_TCAMm,
                        e,
                        VALIDf,
                        validf
                        ? ((fg->flags & _FP_GROUP_LOOKUP_ENABLED)
                           ? 3 : 2
                           )
                        : 0
                        );
    BCM_IF_ERROR_RETURN(soc_mem_write(unit,
                                      FP_TCAMm,
                                      MEM_BLOCK_ALL,
                                      tcam_idx,
                                      e
                                      )
                        );

    if (f_ent->flags & _FP_ENTRY_SECONDARY) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "Overlay in use\n")));

        sal_memset(e, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
        BCM_IF_ERROR_RETURN(soc_mem_read(unit,
                                         FP_GM_FIELDSm,
                                         MEM_BLOCK_ANY,
                                         tcam_idx,
                                         e
                                         )
                            );

        soc_mem_field32_set(unit, FP_GM_FIELDSm, e, VALIDf, validf ? 3 : 2);
        soc_mem_field_width_fit_set(unit, FP_GM_FIELDSm, e, KEYf, tcam->key + fp_tcam_words);
        soc_mem_field_width_fit_set(unit, FP_GM_FIELDSm, e, MASKf, tcam->mask + fp_tcam_words);

        BCM_IF_ERROR_RETURN(WRITE_FP_GM_FIELDSm(unit, MEM_BLOCK_ALL, tcam_idx,
            e));
    } else {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit,
                                         FP_GLOBAL_MASK_TCAMm,
                                         MEM_BLOCK_ANY,
                                         tcam_idx,
                                         &tcam_entry
                                         )
                            );

        soc_mem_field_set(unit,
                          FP_GLOBAL_MASK_TCAMm,
                          (uint32 *)&tcam_entry,
                          KEYf,
                          tcam->key + fp_tcam_words
                          );
        soc_mem_field_set(unit,
                          FP_GLOBAL_MASK_TCAMm,
                          (uint32 *)&tcam_entry,
                          MASKf,
                          tcam->mask + fp_tcam_words
                          );

        if (BCM_FIELD_QSET_TEST(f_ent->group->qset, bcmFieldQualifyInPort) ||
            BCM_FIELD_QSET_TEST(f_ent->group->qset, bcmFieldQualifyInPorts)) {
            soc_mem_field_set(unit, FP_GLOBAL_MASK_TCAMm, (uint32 *)&tcam_entry,
                              IPBMf, (uint32 *)&(f_ent->pbmp.data));
            soc_mem_field_width_fit_set(unit, FP_GLOBAL_MASK_TCAMm,
                                        (uint32 *)&tcam_entry, IPBM_MASKf,
                                        (uint32 *)&(f_ent->pbmp.mask));

        }
        else if (SOC_MEM_IS_VALID(unit, FP_GLOBAL_MASK_TCAM_Xm) &&
                SOC_MEM_IS_VALID(unit, FP_GLOBAL_MASK_TCAM_Ym)) {
            BCM_PBMP_CLEAR(pbmp_x);
            BCM_PBMP_CLEAR(pbmp_y);
            sal_memset(&tcam_entry_x, 0, sizeof(fp_global_mask_tcam_x_entry_t));
            sal_memset(&tcam_entry_y, 0, sizeof(fp_global_mask_tcam_y_entry_t));

            BCM_IF_ERROR_RETURN(soc_mem_read(unit,
                                            FP_GLOBAL_MASK_TCAM_Xm,
                                            MEM_BLOCK_ANY,
                                            tcam_idx,
                                            &tcam_entry_x
                                            )
                                );
            BCM_IF_ERROR_RETURN(soc_mem_read(unit,
                                            FP_GLOBAL_MASK_TCAM_Ym,
                                            MEM_BLOCK_ANY,
                                            tcam_idx,
                                            &tcam_entry_y
                                            )
                                );

            soc_mem_pbmp_field_get(unit, FP_GLOBAL_MASK_TCAM_Xm, &tcam_entry_x,
                                   IPBMf, &pbmp_x);
            soc_mem_pbmp_field_get(unit, FP_GLOBAL_MASK_TCAM_Ym, &tcam_entry_y,
                                   IPBMf, &pbmp_y);

            BCM_PBMP_OR(pbmp_x, pbmp_y);

            soc_mem_pbmp_field_set(unit,
                                   FP_GLOBAL_MASK_TCAMm,
                                   &tcam_entry,
                                   IPBMf,
                                   &pbmp_x 
                                   );
        }

        if (!(f_ent->flags & _FP_ENTRY_SECOND_HALF)) {
            soc_mem_field_set(unit,
                              FP_GLOBAL_MASK_TCAMm,
                              (uint32 *)&tcam_entry,
                              IPBMf,
                              f_ent->pbmp.data.pbits
                              );
            soc_mem_field_set(unit,
                              FP_GLOBAL_MASK_TCAMm,
                              (uint32 *)&tcam_entry,
                              IPBM_MASKf,
                              f_ent->pbmp.mask.pbits
                              );
        }
        soc_mem_field32_set(unit,
                            FP_GLOBAL_MASK_TCAMm,
                            (uint32 *)&tcam_entry,
                            VALIDf,
                            validf ? 3 : 2
                            );
        BCM_IF_ERROR_RETURN(soc_mem_write(unit,
                                          FP_GLOBAL_MASK_TCAMm,
                                          MEM_BLOCK_ALL,
                                          tcam_idx,
                                          &tcam_entry
                                          )
                            );

    }
    return (BCM_E_NONE);
}

int
_bcm_field_td2_qual_tcam_key_mask_set(int            unit,
                                      _field_entry_t *f_ent,
                                      unsigned       validf
                                      )
{
    int       tcam_idx;
    soc_mem_t tcam_mem;

    BCM_IF_ERROR_RETURN(_bcm_field_entry_tcam_idx_get(unit,
                                                      f_ent,
                                                      &tcam_idx
                                                      )
                        );

    switch (f_ent->group->stage_id) {
        case _BCM_FIELD_STAGE_LOOKUP:
            tcam_mem = VFP_TCAMm;
            break;
        case _BCM_FIELD_STAGE_INGRESS:
            return (_field_td2_ingress_qual_tcam_key_mask_set(unit,
                                                              f_ent,
                                                              tcam_idx,
                                                              validf));
        case _BCM_FIELD_STAGE_EGRESS:
            tcam_mem = EFP_TCAMm;
           break;
        default:
            return (BCM_E_INTERNAL);
    }
    {
        uint32 tcam_entry[SOC_MAX_MEM_FIELD_WORDS] = {0};

        BCM_IF_ERROR_RETURN(_bcm_field_trx_tcam_get(unit, tcam_mem,
                                                    f_ent, tcam_entry));
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL,
                                          tcam_idx, tcam_entry));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_td2_action_get
 * Purpose:
 *     Get the actions to be written
 * Parameters:
 *     unit     - BCM device number
 *     mem      - Policy table memory
 *     f_ent    - entry structure to get policy info from
 *     tcam_idx - index into TCAM
 *     fa       - field action
 *     buf      - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     FP unit lock should be held by calling function.
 */
int
_bcm_field_td2_action_get(int             unit,
                          soc_mem_t       mem,
                          _field_entry_t  *f_ent,
                          int             tcam_idx,
                          _field_action_t *fa,
                          uint32          *buf
                          )
{
#ifdef INCLUDE_L3
    int32     hw_idx=0, hw_half=0;
#endif /* INCLUDE_L3 */
    int hw_index = 0;
    int rv = 0;
    int cosq_new;

    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    switch (fa->action) {
        case bcmFieldActionFibreChanSrcBindEnable:
            PolicySet(unit, mem, buf, FCOE_SRC_BIND_CHECK_ENABLEf, fa->param[0]);
            break;
        case bcmFieldActionFibreChanFpmaPrefixCheckEnable:
            PolicySet(unit, mem, buf, FCOE_SRC_FPMA_PREFIX_CHECK_ENABLEf, fa->param[0]);
            break;
        case bcmFieldActionFibreChanZoneCheckEnable:
            PolicySet(unit, mem, buf, FCOE_ZONE_CHECK_ENABLEf, fa->param[0]);
            break;
        case bcmFieldActionFibreChanVsanId:
            PolicySet(unit, mem, buf, FCOE_VSAN_IDf, fa->param[0]);
            break;
        case bcmFieldActionFibreChanZoneCheckActionCancel:
            PolicySet(unit, mem, buf, FCOE_ZONE_CHECK_ACTIONf, fa->param[0]);
            break;
        case bcmFieldActionFibreChanIntVsanPri:
            PolicySet(unit, mem, buf, FCOE_VSAN_PRIf, fa->param[0]);
            PolicySet(unit, mem, buf, FCOE_VSAN_PRI_VALIDf, 1);
            break;
        case bcmFieldActionNewClassId:
            PolicySet(unit, mem, buf, I2E_CLASSIDf, fa->param[0]);
            PolicySet(unit, mem, buf, G_L3SW_CHANGE_L2_FIELDSf, 0x8);
            break;

        case bcmFieldActionEgressClassSelect:
            {
                unsigned i2e_cl_sel;

                switch (fa->param[0]) {
                    case BCM_FIELD_EGRESS_CLASS_SELECT_PORT:
                        i2e_cl_sel = 0x1;
                        break;
                    case BCM_FIELD_EGRESS_CLASS_SELECT_SVP:
                        i2e_cl_sel = 0x2;
                        break;
                    case BCM_FIELD_EGRESS_CLASS_SELECT_L3_IIF:
                        i2e_cl_sel = 0x3;
                        break;
                    case BCM_FIELD_EGRESS_CLASS_SELECT_FIELD_SRC:
                        i2e_cl_sel = 0x4; /* VFP hi */
                        break;
                    case BCM_FIELD_EGRESS_CLASS_SELECT_FIELD_DST:
                        i2e_cl_sel = 0x5; /* VFP lo */
                        break;
                    case BCM_FIELD_EGRESS_CLASS_SELECT_L2_SRC:
                        i2e_cl_sel = 0x6;
                        break;
                    case BCM_FIELD_EGRESS_CLASS_SELECT_L2_DST:
                        i2e_cl_sel = 0x7;
                        break;
                    case BCM_FIELD_EGRESS_CLASS_SELECT_L3_SRC:
                        i2e_cl_sel = 0x8;
                        break;
                    case BCM_FIELD_EGRESS_CLASS_SELECT_L3_DST:
                        i2e_cl_sel = 0x9;
                        break;
                    case BCM_FIELD_EGRESS_CLASS_SELECT_VLAN:
                        i2e_cl_sel = 0xa;
                        break;
                    case BCM_FIELD_EGRESS_CLASS_SELECT_VRF:
                        i2e_cl_sel = 0xb;
                        break;
                    case BCM_FIELD_EGRESS_CLASS_SELECT_NEW:
                        i2e_cl_sel = 0xf;
                        break;
                    default:
                        /* Invalid parameter should have been caught earlier */
                        return (BCM_E_INTERNAL);
                }
                PolicySet(unit, mem, buf, I2E_CLASSID_SELf, i2e_cl_sel);
                PolicySet(unit, mem, buf, G_L3SW_CHANGE_L2_FIELDSf, 0x8);
            }
            break;

        case bcmFieldActionHiGigClassSelect:
            {
                unsigned hg_cl_sel;

                switch (fa->param[0]) {
                    case BCM_FIELD_HIGIG_CLASS_SELECT_EGRESS:
                        hg_cl_sel = 1;
                        break;
                    case BCM_FIELD_HIGIG_CLASS_SELECT_PORT:
                        hg_cl_sel = 4;
                        break;
                    default:
                        /* Invalid parameter should have been caught earlier */
                        return (BCM_E_INTERNAL);
                }

                PolicySet(unit, mem, buf, HG_CLASSID_SELf, hg_cl_sel);
            }
            break;

        case bcmFieldActionNatCancel:
            PolicySet(unit, mem, buf, DO_NOT_NATf, 0x1);
            if (soc_feature(unit, soc_feature_fp_based_oam)) {
                PolicySet(unit, mem, buf, NAT_OAM_ACTION_SELECTf, _bcmFieldNatOamValidNat);
            }
            break;

        case bcmFieldActionNat:
            PolicySet(unit, mem, buf, NAT_ENABLEf, 0x1);
            if (soc_feature(unit, soc_feature_fp_based_oam)) {
                PolicySet(unit, mem, buf, NAT_OAM_ACTION_SELECTf, _bcmFieldNatOamValidNat);
            }
            break;

        case bcmFieldActionNatEgressOverride:
#ifdef INCLUDE_L3
            BCM_L3_NAT_EGRESS_HW_IDX_GET(fa->param[0], hw_idx, hw_half);
            if ((hw_idx < 0) || (hw_idx > soc_mem_index_max(unit,
                                          EGR_NAT_PACKET_EDIT_INFOm))) {
               return BCM_E_PARAM;
            }

            PolicySet(unit, mem, buf, NAT_PACKET_EDIT_IDXf, hw_idx);
            PolicySet(unit, mem, buf, NAT_PACKET_EDIT_ENTRY_SELf, hw_half);
#else
            return (BCM_E_UNAVAIL);   
#endif /* INCLUDE_L3 */ 
            break;

        case bcmFieldActionIngSampleEnable: 
             PolicySet(unit, mem, buf, SFLOW_ING_SAMPLEf, 0x1);
             break;

        case bcmFieldActionEgrSampleEnable: 
             PolicySet(unit, mem, buf, SFLOW_EGR_SAMPLEf, 0x1);
             break;

        case bcmFieldActionHashSelect0:
        case bcmFieldActionHashSelect1:
            /* hash selection available only in lookup stage */
            if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
                _field_td2_action_hash_select(unit, mem, f_ent, fa, buf);
            }
            break;

        case bcmFieldActionTrunkLoadBalanceCancel:
             PolicySet(unit, mem, buf, LAG_RH_DISABLEf, 1);
             PolicySet(unit, mem, buf, HGT_RH_DISABLEf, 1);
             PolicySet(unit, mem, buf, HGT_DLB_DISABLEf, 1);
             break;

        case bcmFieldActionEcmpResilientHashCancel:
             PolicySet(unit, mem, buf, ECMP_RH_DISABLEf, 1);
             break;

        case bcmFieldActionHgTrunkResilientHashCancel:
             PolicySet(unit, mem, buf, HGT_RH_DISABLEf, 1);
             break;

        case bcmFieldActionTrunkResilientHashCancel:
             PolicySet(unit, mem, buf, LAG_RH_DISABLEf, 1);
             break;
        case bcmFieldActionVxlanHeaderBits8_31_Set:
            PolicySet(unit, mem, buf, VXLAN_RESERVED_1f, fa->param[0]);
            PolicySet(unit, mem, buf, VXLAN_ACTIONf, 1);
            break;
        case bcmFieldActionVxlanHeaderBits56_63_Set:
            PolicySet(unit, mem, buf, VXLAN_RESERVED_2f, fa->param[0]);
            PolicySet(unit, mem, buf, VXLAN_ACTIONf, 1);
            break;
        case bcmFieldActionVxlanHeaderFlags:
            PolicySet(unit, mem, buf, VXLAN_FLAGSf, fa->param[0]);
            PolicySet(unit, mem, buf, VXLAN_ACTIONf, 1);
            break;
        case bcmFieldActionOamOlpHeaderAdd:
            if (soc_feature(unit, soc_feature_fp_based_oam)) {
                if ((_BCM_FIELD_STAGE_INGRESS == f_ent->group->stage_id)
#if defined(BCM_APACHE_SUPPORT)
                    || (SOC_IS_APACHE(unit) &&
                         (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id))
#endif
                    ) {
                    if (fa->param[0] >= bcmFieldOlpHeaderTypeCount) {
                        return (BCM_E_PARAM);
                    }
                    hw_index = 0;
                    rv = _bcm_oam_olp_fp_hw_index_get (unit,fa->param[0],
                            &hw_index);
                    if (BCM_FAILURE(rv)) {
                        return BCM_E_PARAM;
                    }

                    PolicySet(unit, mem, buf, ADD_OLP_HEADERf, 1);
                    PolicySet(unit, mem, buf, OLP_HDR_TYPE_COMPRESSEDf,
                              hw_index);
                    /* Egress stage doesn't have NAT OAM ACTIONS.
                       In IFP, if MACSEC based OLP header types
                       bcmFieldOlpHeaderTypeMacSecEncrypt and bcmFieldOlpHeaderTypeMacSecDecrypt
                       used then they doesn't require to program NAT_OAM_ACTION_SELECT to add OLP header.
                       So avoid programming NAT_OAM_ACTION_SELECTf for MacSecEncrypt/MacSecDecrypt header types.
                       bcmFieldOlpHeaderTypeMacSecEncrypt and bcmFieldOlpHeaderTypeMacSecDecrypt have
                       enum values 22 and 23 respectively. Computation done to check whether the param is
                       anyone of the 2 MACSEC header types.
                       22 = 0001 0110 (0x16) ==> After Right shift ==> 0000 1011 (0x0B)
                       23 = 0001 0111 (0x17) ==> After Right shift ==> 0000 1011 (0x0B)
                       So right shift and compare with 0xB instead of multiple comparisions.
                       */
                    if ((_BCM_FIELD_STAGE_INGRESS == f_ent->group->stage_id)
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_XFLOW_MACSEC)
                         && (0xB != (fa->param[0] >> 1))
#endif /* BCM_MONTEREY_SUPPORT & INCLUDE_XFLOW_MACSEC */
                        ) {
                        PolicySet(unit, mem, buf, NAT_OAM_ACTION_SELECTf,
                                _bcmFieldNatOamValidOam);
                    }
                }
            }
            break;
        case bcmFieldActionOamDmTimeFormat:
            if (soc_feature(unit, soc_feature_fp_based_oam)) {
                if ((_BCM_FIELD_STAGE_INGRESS == f_ent->group->stage_id)
#if defined(BCM_APACHE_SUPPORT)
                    || (SOC_IS_APACHE(unit) &&
                         (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id))
#endif
                    ) {
                    unsigned oam_dm_type;
                    switch (fa->param[0]) {
                        case BCM_FIELD_OAM_DM_TIME_FORMAT_IEEE1588:
                            oam_dm_type = 1;
                            break;
                        case BCM_FIELD_OAM_DM_TIME_FORMAT_NTP:
                            oam_dm_type = 2;
                            break;
                        default:
                            /* Invalid parameter should have been caught earlier */
                            return (BCM_E_INTERNAL);
                    }

                    PolicySet(unit, mem, buf, SAMPLE_TIMESTAMPf, oam_dm_type);
                    /* Egress stage doesn't have NAT OAM ACTIONS */
                    if (_BCM_FIELD_STAGE_INGRESS == f_ent->group->stage_id) {
                        PolicySet(unit, mem, buf, NAT_OAM_ACTION_SELECTf, _bcmFieldNatOamValidOam);
                    }
                }
            }
            break;
        case bcmFieldActionEgressPbmpNullCopyToCpu:
            if (SOC_MEM_FIELD_VALID(unit, mem, INSTRUMENTATION_TRIGGERS_ENABLEf)) {
                PolicySet(unit, mem, buf, INSTRUMENTATION_TRIGGERS_ENABLEf, TRUE);
            }
            break;
        case bcmFieldActionPimBidirFwd:
            PolicySet(unit, mem, buf, PIM_BIDIR_FORWARDf, 1);
            break;
        case bcmFieldActionDscpMapNew:
            hw_index = 0;
#if defined BCM_TRIUMPH2_SUPPORT
            rv = _bcm_tr2_qos_id2idx(unit, fa->param[0], &hw_index);
                if (BCM_FAILURE(rv)) {
                    return BCM_E_PARAM;
                }
#endif
            PolicySet(unit, mem, buf, TRUST_DSCP_PTRf, hw_index);
            PolicySet(unit, mem, buf, TRUST_DSCPf, 1);
            break;

        case bcmFieldActionUcastQueueNew:
            PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 2);
            PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 2);
            PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 2);
            rv = _bcm_td2_cosq_index_resolve(unit, fa->param[0], 0,
                       _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                       &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }
            PolicySet(unit, mem, buf, R_COS_INT_PRIf, cosq_new);
            PolicySet(unit, mem, buf, Y_COS_INT_PRIf, cosq_new);
            PolicySet(unit, mem, buf, G_COS_INT_PRIf, cosq_new);
            break;
        case bcmFieldActionRpUcastQueueNew:
            PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 2);
            rv = _bcm_td2_cosq_index_resolve(unit, fa->param[0], 0,
                       _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                       &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }
            PolicySet(unit, mem, buf, R_COS_INT_PRIf, cosq_new);
            break;
        case bcmFieldActionYpUcastQueueNew:
            PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 2);
            rv = _bcm_td2_cosq_index_resolve(unit, fa->param[0], 0,
                       _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                       &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }
            PolicySet(unit, mem, buf, Y_COS_INT_PRIf, cosq_new);
            break;
        case bcmFieldActionGpUcastQueueNew:
            PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 2);
            rv = _bcm_td2_cosq_index_resolve(unit, fa->param[0], 0,
                       _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                       &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }
            PolicySet(unit, mem, buf, G_COS_INT_PRIf, cosq_new);
            break;

        case bcmFieldActionIntCosUcastQueueNew:
            PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 3);
            PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 3);
            PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 3);
            rv = _bcm_td2_cosq_index_resolve(unit, fa->param[0], 0,
                       _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                       &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }
            PolicySet(unit, mem, buf, R_COS_INT_PRIf,
                      (_BCM_COSQ_CLASSIFIER_FIELD_GET(fa->param[1]) << 12 |
                       cosq_new));
            PolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                      (_BCM_COSQ_CLASSIFIER_FIELD_GET(fa->param[1]) << 12 |
                       cosq_new));
            PolicySet(unit, mem, buf, G_COS_INT_PRIf,
                      (_BCM_COSQ_CLASSIFIER_FIELD_GET(fa->param[1]) << 12 |
                       cosq_new));
            break;
        case bcmFieldActionRpIntCosUcastQueueNew:
            PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 3);
            rv = _bcm_td2_cosq_index_resolve(unit, fa->param[0], 0,
                       _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                       &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }
            PolicySet(unit, mem, buf, R_COS_INT_PRIf,
                      (_BCM_COSQ_CLASSIFIER_FIELD_GET(fa->param[1]) << 12 |
                       cosq_new));
            break;
        case bcmFieldActionYpIntCosUcastQueueNew:
            PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 3);
            rv = _bcm_td2_cosq_index_resolve(unit, fa->param[0], 0,
                       _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                       &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }
            PolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                     (_BCM_COSQ_CLASSIFIER_FIELD_GET(fa->param[1]) << 12 |
                       cosq_new));
            break;
        case bcmFieldActionGpIntCosUcastQueueNew:
            PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 3);
            rv = _bcm_td2_cosq_index_resolve(unit, fa->param[0], 0,
                       _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                       &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }
            PolicySet(unit, mem, buf, G_COS_INT_PRIf,
                      (_BCM_COSQ_CLASSIFIER_FIELD_GET(fa->param[1]) << 12 |
                       cosq_new));
            break;
        case bcmFieldActionDoNotCutThrough:
            PolicySet(unit, mem, buf, DO_NOT_CUT_THROUGHf, 0x1);
            break;
#if defined(BCM_APACHE_SUPPORT)
        case bcmFieldActionCopyToCpu:
            if (SOC_IS_APACHE(unit) &&
                    (f_ent->group->stage_id == _BCM_FIELD_STAGE_EGRESS) &&
                    (fa->param[2])) {
                return (_bcm_field_copytocpu_action_get(unit, mem, f_ent, fa, buf));
            } else {
                return (BCM_E_UNAVAIL);
            }
            break;
        case bcmFieldActionRedirect:
            if (SOC_IS_APACHE(unit) &&
                    (f_ent->group->stage_id == _BCM_FIELD_STAGE_EGRESS) &&
                    (fa->param[2])) {
                return (_bcm_field_redirect_action_get(unit, mem, f_ent, fa, buf));
            } else {
                return (BCM_E_UNAVAIL);
            }
            break;
#endif
#if defined(BCM_TOMAHAWK2_SUPPORT)
        case bcmFieldActionMplsLabel1ExpNew:
            if (SOC_IS_TOMAHAWK2(unit) &&
                (f_ent->group->stage_id == _BCM_FIELD_STAGE_EGRESS)) {
                PolicySet(unit, mem, buf, CHANGE_EXPf, 0x1);
                PolicySet(unit, mem, buf, EXPf, fa->param[0]);
            } else {
                return (BCM_E_UNAVAIL);
            }
            break;
#endif
        default:
            return (BCM_E_UNAVAIL);
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_td2_ingress_write_slice_map
 * Purpose:
 *     Write FP_SLICE_MAP
 * Parameters:
 *     unit       - (IN) BCM device number
 *     stage_fc   - (IN) stage control structure
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_td2_ingress_write_slice_map(int unit, _field_stage_t *stage_fc)
{
    fp_slice_map_entry_t entry;
    soc_field_t field;
    uint32 value;
    int vmap_size;
    int index;
    static const soc_field_t physical_slice[] = {
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
    static const soc_field_t slice_group[] = {
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
    BCM_IF_ERROR_RETURN
        (_bcm_field_virtual_map_size_get(unit, stage_fc, &vmap_size));

    SOC_IF_ERROR_RETURN(READ_FP_SLICE_MAPm(unit, MEM_BLOCK_ANY, 0, &entry));

    for (index = 0; index < vmap_size; index++) {
        value = stage_fc->vmap[_FP_DEF_INST][_FP_VMAP_DEFAULT][index].vmap_key;
        field = physical_slice[index];
        soc_FP_SLICE_MAPm_field32_set(unit, &entry, field, value);

        value = stage_fc->vmap[_FP_DEF_INST][_FP_VMAP_DEFAULT][index].virtual_group;
        field = slice_group[index];
        soc_FP_SLICE_MAPm_field32_set(unit, &entry, field, value);
    }

    return WRITE_FP_SLICE_MAPm(unit, MEM_BLOCK_ALL, 0, &entry);
}

/*
 * Function:
 *     _bcm_field_td2_write_slice_map
 * Purpose:
 *     Write FP_SLICE_MAP, EFP_SLICE_MAP, VFP_SLICE_MAP
 * Parameters:
 *     unit       - (IN) BCM device number
 *     stage_fc   - (IN) stage control structure
 *     fg         - (IN) reference to field group structure.
 * Returns:
 *     BCM_E_XXX
 */

int
_bcm_field_td2_write_slice_map(int unit, _field_stage_t *stage_fc,
                               _field_group_t *fg)
{
    switch (stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_LOOKUP:
            return (_bcm_field_trx_write_slice_map_vfp(unit, stage_fc));

        case _BCM_FIELD_STAGE_INGRESS:
            return (_field_td2_ingress_write_slice_map(unit, stage_fc));

        case _BCM_FIELD_STAGE_EGRESS:
            return (_bcm_field_trx_write_slice_map_egress(unit, stage_fc));

        default:
            ;
    }

    return (BCM_E_INTERNAL);
}

/*
 * Function:
 *     _bcm_field_td2_qualify_ip_type
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
_bcm_field_td2_qualify_ip_type(int unit, bcm_field_entry_t entry,
                               bcm_field_IpType_t type,
                               bcm_field_qualify_t qual)
{
    uint32              data = 0,
                        mask = 0;
#if defined BCM_TRIDENT3_SUPPORT
    _field_stage_t           *stage_fc = NULL;
    _field_stage_id_t        stage_id = -1;
    bcm_error_t              rv = BCM_E_INTERNAL;
    _field_entry_t           *f_ent = NULL;
#endif

#if defined BCM_TOMAHAWK3_SUPPORT
   if (SOC_IS_TOMAHAWK3(unit)) {
      return _bcm_field_th3_qualify_ip_type(unit, entry,
                                            type, qual);
   }
#endif

#if defined BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_td3_style_fp)
            && (FALSE == _BCM_FIELD_IS_PRESEL_ENTRY(entry))) {
        rv = _bcm_field_entry_get_by_id(unit, entry, &f_ent);
        BCM_IF_ERROR_RETURN(rv);
        stage_id = f_ent->group->stage_id;
        rv = _field_stage_control_get(unit, stage_id, &stage_fc);
        BCM_IF_ERROR_RETURN(rv);
        if ((_BCM_FIELD_STAGE_EGRESS == stage_fc->stage_id)
                && ((bcmFieldIpTypeArpReply == type) ||
                    (bcmFieldIpTypeArpRequest == type))) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,
                                "FP(unit %d) vverb: entry=%d qualifying"
                                " on Invalid Iptype=%#x\n))"),
                     unit, entry, type));
            return (BCM_E_UNAVAIL);
        }
    }
#endif
    switch (type) {
      case bcmFieldIpTypeAny:
          data = 0x0;
          mask = 0x0;
          break;
      case bcmFieldIpTypeNonIp:
          data = 0x1f;
          mask = 0x1f;
          break;
      case bcmFieldIpTypeIp:
          data = 0x0;
          mask = 0x8;
          break;
          break;
      case bcmFieldIpTypeIpv4NoOpts:
          data = 0x0;
          mask = 0x1f;
          break;
      case bcmFieldIpTypeIpv4WithOpts:
          data = 0x1;
          mask = 0x1f;
          break;
      case bcmFieldIpTypeIpv4Any:
          data = 0x0;
          mask = 0xe;
          break;
      case bcmFieldIpTypeIpv6NoExtHdr:
          data = 0x4;
          mask = 0x1f;
          break;
      case bcmFieldIpTypeIpv6OneExtHdr:
          data = 0x5;
          mask = 0x1f;
          break;
      case bcmFieldIpTypeIpv6TwoExtHdr:
          data = 0x6;
          mask = 0x1f;
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
          mask = 0x1f;
          break;
      case bcmFieldIpTypeArpReply:
          data = 0x9;
          mask = 0x1f;
          break;
      case bcmFieldIpTypeTrill:
          data = 0xa;
          mask = 0x1f;
          break;
      case bcmFieldIpTypeFCoE:
          data = 0xb;
          mask = 0x1f;
          break;
      case bcmFieldIpTypeMplsUnicast:
          data = 0xc;
          mask = 0x1f;
          break;
      case bcmFieldIpTypeMplsMulticast:
          data = 0xd;
          mask = 0x1f;
          break;
      case bcmFieldIpTypeMpls:
          data = 0xc;
          mask = 0x1e;
          break;
      case bcmFieldIpTypeMim:
          data = 0xe;
          mask = 0x1f;
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
 *     _bcm_field_td2_qualify_ip_type_get
 * Purpose:
 *     Read ip type qualifier match criteria from the HW.
 * Parameters:
 *     unit  - (IN) BCM device number
 *     entry - (IN) Field entry index
 *     type  - (OUT) Ip Type.
 *     qual  - (IN) Qualifier(IpType/InnerIpType)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */

int
_bcm_field_td2_qualify_ip_type_get(int unit, bcm_field_entry_t entry,
                                   bcm_field_IpType_t *type, 
                                   bcm_field_qualify_t qual)
{
    uint32 hw_data;          /* HW encoded qualifier data.  */
    uint32 hw_mask;          /* HW encoding qualifier mask. */
    int rv;                  /* Operation return status.    */

    /* Input parameters checks. */
    if (NULL == type) {
        return (BCM_E_PARAM);
    }


    /* Read qualifier match value and mask. */
    rv = _bcm_field_entry_qualifier_uint32_get(unit, entry,
                                               qual,
                                               &hw_data, &hw_mask);
    BCM_IF_ERROR_RETURN(rv);
	
    /* Read IpType encoding for qualifier data and mask */
    rv = _bcm_field_td2_qualify_ip_type_encode_get(unit, hw_data, 
                                                   hw_mask, type);
    return rv;

}

/*
 * Function:
 *     _bcm_field_td2_qualify_ip_type_encode_get
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
_bcm_field_td2_qualify_ip_type_encode_get(int unit,
                                          uint32 hw_data,
                                          uint32 hw_mask,
                                          bcm_field_IpType_t *type)
{
#if defined BCM_TOMAHAWK3_SUPPORT
   if (SOC_IS_TOMAHAWK3(unit)) {
      return _bcm_field_th3_qualify_ip_type_encode_get(unit, 
                                            hw_data, hw_mask,
                                            type);
   }
#endif


    if ((0 == hw_data) && (0 == hw_mask)) {
        *type = bcmFieldIpTypeAny;
    } else if ((0x1f == hw_data) && (0x1f == hw_mask)) {
        *type = bcmFieldIpTypeNonIp;
    } else if ((0x0 == hw_data) && (0x8 == hw_mask)) {
        *type = bcmFieldIpTypeIp;
    } else if ((0x0 == hw_data) && (0x1f == hw_mask)) {
        *type = bcmFieldIpTypeIpv4NoOpts;
    } else if ((0x1 == hw_data) && (0x1f == hw_mask)) {
        *type = bcmFieldIpTypeIpv4WithOpts;
    } else if ((0x0 == hw_data) && (0xe == hw_mask)) {
        *type = bcmFieldIpTypeIpv4Any;
    } else if ((0x4 == hw_data) && (0x1f == hw_mask)) {
        *type = bcmFieldIpTypeIpv6NoExtHdr;
    } else if ((0x5 == hw_data) && (0x1f == hw_mask)) {
        *type = bcmFieldIpTypeIpv6OneExtHdr;
    } else if ((0x6 == hw_data) && (0x1f == hw_mask)) {
        *type = bcmFieldIpTypeIpv6TwoExtHdr;
    } else if ((0x4 == hw_data) && (0xc == hw_mask)) {
        *type = bcmFieldIpTypeIpv6;
    } else if ((0x8 == hw_data) && (0xe == hw_mask)) {
        *type = bcmFieldIpTypeArp;
    } else if ((0x8 == hw_data) && (0x1f == hw_mask)) {
        *type = bcmFieldIpTypeArpRequest;
    } else if ((0x9 == hw_data) && (0x1f == hw_mask)) {
        *type = bcmFieldIpTypeArpReply;
    } else if ((0xa == hw_data) && (0x1f == hw_mask)) {
        *type = bcmFieldIpTypeTrill;
    } else if ((0xb == hw_data) && (0x1f == hw_mask)) {
        *type = bcmFieldIpTypeFCoE;
    } else if ((0xc == hw_data) && (0x1f == hw_mask)) {
        *type = bcmFieldIpTypeMplsUnicast;
    } else if ((0xd == hw_data) && (0x1f == hw_mask)) {
        *type = bcmFieldIpTypeMplsMulticast;
    } else if ((0xc == hw_data) && (0x1e == hw_mask)) {
        *type = bcmFieldIpTypeMpls;
    } else if ((0xe == hw_data) && (0x1f == hw_mask)) {
        *type = bcmFieldIpTypeMim;
    } else
    {
        return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_td2_functions_init
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
_field_td2_functions_init(_field_funct_t *functions)
{
    functions->fp_detach               = _bcm_field_tr_detach;
    functions->fp_group_install        = _field_td2_group_install;
    functions->fp_selcodes_install     = _field_td2_selcodes_install;
    functions->fp_slice_clear          = _field_td2_slice_clear;
    functions->fp_entry_remove         = _bcm_field_fb_entry_remove;
    functions->fp_entry_move           = _field_td2_entry_move;
    functions->fp_selcode_get          = _field_td2_selcode_get;
    functions->fp_selcode_to_qset      = _bcm_field_selcode_to_qset;
    functions->fp_qual_list_get        = _bcm_field_qual_lists_get;
    functions->fp_tcam_policy_clear    = NULL;
    functions->fp_tcam_policy_install  = _bcm_field_tr_entry_install;
    functions->fp_tcam_policy_reinstall = _bcm_field_tr_entry_reinstall;
    functions->fp_policer_install      = _bcm_field_trx_policer_install;
    functions->fp_write_slice_map      = _bcm_field_td2_write_slice_map;
    functions->fp_qualify_ip_type      = _bcm_field_td2_qualify_ip_type;
    functions->fp_qualify_ip_type_get  = _bcm_field_td2_qualify_ip_type_get;
    functions->fp_action_support_check = _field_td2_action_support_check;
    functions->fp_action_conflict_check = _bcm_field_td2_action_conflict_check;
    functions->fp_counter_get          = _bcm_field_td_counter_get;
    functions->fp_counter_set          = _bcm_field_td_counter_set;
    functions->fp_stat_index_get       = _bcm_field_trx_stat_index_get;
    functions->fp_action_params_check  = _field_td2_action_params_check;
    functions->fp_action_depends_check = NULL;
    functions->fp_egress_key_match_type_set
        = _bcm_field_trx_egress_key_match_type_set;
    functions->fp_external_entry_install  = NULL;
    functions->fp_external_entry_reinstall  = NULL;
    functions->fp_external_entry_remove   = NULL;
    functions->fp_external_entry_prio_set = NULL;
    functions->fp_data_qualifier_ethertype_add
        = _bcm_field_trx2_data_qualifier_ethertype_add;
    functions->fp_data_qualifier_ethertype_delete
        = _bcm_field_trx2_data_qualifier_ethertype_delete;
    functions->fp_data_qualifier_ip_protocol_add
        = _bcm_field_trx2_data_qualifier_ip_protocol_add;
    functions->fp_data_qualifier_ip_protocol_delete
        = _bcm_field_trx2_data_qualifier_ip_protocol_delete;
    functions->fp_data_qualifier_packet_format_add
        = _bcm_field_trx2_data_qualifier_packet_format_add;
    functions->fp_data_qualifier_packet_format_delete
        = _bcm_field_trx2_data_qualifier_packet_format_delete;
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
    functions->fp_qualify_packet_res = _bcm_field_td2_qualify_PacketRes;
    functions->fp_qualify_packet_res_get = _bcm_field_td2_qualify_PacketRes_get;
}

/*
 * Function:
 *     _field_td2_qualifiers_init
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
_field_td2_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    if (!soc_feature(unit, soc_feature_l3) 
        && !soc_feature(unit, soc_feature_virtual_switching)) {
        return _field_sc960_qualifiers_init(unit, stage_fc);
    }

    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_qual_arr,
                   (_bcmFieldQualifyCount * sizeof(_bcm_field_qual_info_t *)),
                   "Field qualifiers");
    if (stage_fc->f_qual_arr == 0) {
        return (BCM_E_MEMORY);
    }

    switch (stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_LOOKUP:
            return (_field_td2_lookup_qualifiers_init(unit, stage_fc));
        case _BCM_FIELD_STAGE_INGRESS:
            return (_field_td2_ingress_qualifiers_init(unit, stage_fc));
        case _BCM_FIELD_STAGE_EGRESS:
            return (_field_td2_egress_qualifiers_init(unit, stage_fc));
        default:
            ;
    }

    sal_free(stage_fc->f_qual_arr);
    return (BCM_E_INTERNAL);
}

int
_bcm_field_td2_stage_init(int unit,
                          _field_stage_t *stage_fc)
{
    /* Input prameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    switch (stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_LOOKUP:
            /* Flags */
            stage_fc->flags |= (_FP_STAGE_SLICE_ENABLE
                                | _FP_STAGE_AUTO_EXPANSION
                                | _FP_STAGE_GLOBAL_COUNTERS);
            /* Slice geometry */
            stage_fc->tcam_sz     = soc_mem_index_count(unit, VFP_TCAMm);
            stage_fc->tcam_slices = 4;
            if (soc_feature(unit, 
                            soc_feature_field_stage_lookup_256_half_slice)) {
                stage_fc->flags |= _FP_STAGE_HALF_SLICE;
            }
            if (soc_feature(unit,
                        soc_feature_field_stage_lookup_512_half_slice)) {
                stage_fc->flags |= _FP_STAGE_HALF_SLICE;
            }
            break;
        case _BCM_FIELD_STAGE_INGRESS:
            /* Flags */
            stage_fc->flags
                |= _FP_STAGE_SLICE_ENABLE
                | _FP_STAGE_GLOBAL_METER_POOLS
                | _FP_STAGE_SEPARATE_PACKET_BYTE_COUNTERS
                | _FP_STAGE_AUTO_EXPANSION;
            /* Slice geometry */
            stage_fc->tcam_sz     = soc_mem_index_count(unit, FP_TCAMm);
            if (soc_feature(unit, soc_feature_field_slices8)) {
                stage_fc->tcam_slices = 8;
            } else {
                stage_fc->tcam_slices = 12;
            }
            if (soc_feature(unit, soc_feature_field_stage_quarter_slice)) {
                stage_fc->flags |= _FP_STAGE_QUARTER_SLICE;
            } else if (soc_feature(unit, soc_feature_field_stage_half_slice)) {
                stage_fc->flags |= _FP_STAGE_HALF_SLICE;
            }
            break;
        case _BCM_FIELD_STAGE_EGRESS:
            /* Flags */
            stage_fc->flags |= _FP_STAGE_SLICE_ENABLE
                | _FP_STAGE_GLOBAL_COUNTERS
                | _FP_STAGE_SEPARATE_PACKET_BYTE_COUNTERS
                | _FP_STAGE_AUTO_EXPANSION;
            /* Slice geometry */
            stage_fc->tcam_sz     = soc_mem_index_count(unit, EFP_TCAMm);
            stage_fc->tcam_slices = 4;
            if (soc_feature(unit, 
                            soc_feature_field_stage_egress_256_half_slice)) {
                stage_fc->flags |= _FP_STAGE_HALF_SLICE;
            }
            if (soc_feature(unit,
                        soc_feature_field_stage_egress_512_half_slice)) {
                stage_fc->flags |= _FP_STAGE_HALF_SLICE;
            }
            break;
        default:
            return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_td2_init
 * Purpose:
 *     Perform initializations that are specific to BCM56850. This
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
_bcm_field_td2_init(int unit, _field_control_t *fc)
{
    _field_stage_t *stage_fc;

    /* Input parameters check. */
    if (NULL == fc) {
        return (BCM_E_PARAM);
    }

    stage_fc = fc->stages;
    while (stage_fc) {

        if (!SAL_BOOT_BCMSIM && !SAL_BOOT_QUICKTURN && !SAL_BOOT_XGSSIM) {
            /* Clear hardware table */
            BCM_IF_ERROR_RETURN(_bcm_field_tr_hw_clear(unit, stage_fc));
        }

#if defined (BCM_APACHE_SUPPORT)
        if (SOC_IS_APACHE(unit)) {
            /* Initialize qualifiers info. */
            BCM_IF_ERROR_RETURN(_field_apache_qualifiers_init(unit, stage_fc));
        } else
#endif /*BCM_APACHE_SUPPORT*/
    /* Initialize the function pointers */
#if defined (BCM_TRIDENT2PLUS_SUPPORT)
        if (SOC_IS_TD2P_TT2P(unit)) {
            BCM_IF_ERROR_RETURN(_field_td2plus_qualifiers_init(unit, stage_fc));
        } else
#endif /*BCM_TRIDENT2PLUS_SUPPORT*/
        {
            BCM_IF_ERROR_RETURN(_field_td2_qualifiers_init(unit, stage_fc));
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
#if defined (BCM_TRIDENT2PLUS_SUPPORT)
    if (SOC_IS_TD2P_TT2P(unit)) {
        _field_td2plus_functions_init(&fc->functions);
        return BCM_E_NONE;
    }
#endif /*BCM_TRIDENT2PLUS_SUPPORT*/

#if defined (BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
        _field_apache_functions_init(&fc->functions);
        return BCM_E_NONE;
    }
#endif /*BCM_APACHE_SUPPORT*/

    _field_td2_functions_init(&fc->functions);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_td2_qualify_class
 * Purpose:
 *     Qualifies field entry classifier id data and mask value
 * Parameters:
 *     unit  - (IN) BCM device number
 *     entry - (IN) Field entry identifier
 *     data  - (IN) Classifier ID lookup key value
 *     mask  - (IN) Classifier ID lookup mask value
 * Returns:
 *     BCM_E_XXXX
 */
int
_bcm_field_td2_qualify_class(int unit,
                             bcm_field_entry_t entry,
                             bcm_field_qualify_t qual,
                             uint32 *data,
                             uint32 *mask
                             )
{
    _field_entry_t *f_ent;
    uint32 cl_width, cl_max;
    uint32 ifp_cl_type;

    /* Get field entry part that contains the qualifier. */
    BCM_IF_ERROR_RETURN(_bcm_field_entry_qual_get(unit, entry, qual, &f_ent));

    switch (f_ent->group->stage_id) {
        case _BCM_FIELD_STAGE_INGRESS:
            switch (qual) {
                case bcmFieldQualifySrcClassL2:
                case bcmFieldQualifySrcClassL3:
                case bcmFieldQualifySrcClassField:
                case bcmFieldQualifyDstClassL2:
                case bcmFieldQualifyDstClassL3:
                case bcmFieldQualifyDstClassField:
                    cl_width = 10;
                    break;
                case bcmFieldQualifyInterfaceClassL2:
                case bcmFieldQualifyInterfaceClassL3:
                case bcmFieldQualifyInterfaceClassPort:
                case bcmFieldQualifyInterfaceClassVPort:  
                    cl_width = 12;
                    break;
                default:
                    return (BCM_E_INTERNAL);
            }

            cl_max = 1 << cl_width;

            if (*data >= cl_max ||
               (*mask != BCM_FIELD_EXACT_MATCH_MASK && *mask >= cl_max)) {
                return (BCM_E_PARAM);
            }
            break;

        case _BCM_FIELD_STAGE_LOOKUP:
            switch (qual) {
                case bcmFieldQualifyInterfaceClassPort:
                    cl_width = 8;
                    break;
                case bcmFieldQualifyInterfaceClassL3:
                    cl_width = 8;
                    break;
                default:
                    return (BCM_E_INTERNAL);
            }

            cl_max   = 1 << cl_width;
            if (*data >= cl_max || (*mask != BCM_FIELD_EXACT_MATCH_MASK && *mask >= cl_max)) {
                return (BCM_E_PARAM);
            }
            break;

        case _BCM_FIELD_STAGE_EGRESS:
            cl_width = 9;
            cl_max   = 1 << cl_width;

            if (*data >= cl_max || (*mask != BCM_FIELD_EXACT_MATCH_MASK && *mask >= cl_max)) {
                return (BCM_E_PARAM);
            }

            /* Need to set IFP_CLASS_TYPE in TCAM (upper 4 bits) */
            switch (qual) {
                case bcmFieldQualifySrcClassL2:
                    ifp_cl_type = 6;
                    break;
                case bcmFieldQualifySrcClassL3:
                    ifp_cl_type = 8;
                    break;
                case bcmFieldQualifySrcClassField:
                    ifp_cl_type = 4;
                    break;
                case bcmFieldQualifyDstClassL2:
                    ifp_cl_type = 7;
                    break;
                case bcmFieldQualifyDstClassL3:
                    ifp_cl_type = 9;
                    break;
                case bcmFieldQualifyDstClassField:
                    ifp_cl_type = 5;
                    break;
                case bcmFieldQualifyInterfaceClassL2:
                    ifp_cl_type = 10;
                    break;
                case bcmFieldQualifyInterfaceClassL3:
                    ifp_cl_type = 3;
                    break;
                case bcmFieldQualifyIngressClassField:
                    ifp_cl_type = 15;
                    break;
                case bcmFieldQualifyIngressInterfaceClassPort:
                    ifp_cl_type = 1;
                    break;
                case bcmFieldQualifyIngressInterfaceClassVPort:
                    ifp_cl_type = 2;
                    break;
                default:
                    return (BCM_E_INTERNAL);
            }

            *data |= ifp_cl_type << cl_width;
            *mask |= 0xf << cl_width;
            break;

        default:
            return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_td2_qualify_class_get
 * Purpose:
 *     Retrieve field entry classifier id data and mask values
 * Parameters:
 *     unit  - (IN) BCM device number
 *     entry - (IN) Field entry identifier
 *     qual  - (IN) Field entry qualifier enumeration
 *     data  - (IN/OUT) Classifier ID lookup key value
 *     mask  - (IN/OUT) Classifier ID lookup mask value
 * Returns:
 *     BCM_E_XXXX
 */
int
_bcm_field_td2_qualify_class_get(int unit,
                                 bcm_field_entry_t entry,
                                 bcm_field_qualify_t qual,
                                 uint32 *data,
                                 uint32 *mask
                                 )
{
    _field_entry_t *f_ent;
    const uint32 m = (1 << 9) - 1;

    /* Get field entry part that contains the qualifier. */
    BCM_IF_ERROR_RETURN(_bcm_field_entry_qual_get(unit, entry, qual, &f_ent));

    if (f_ent->group->stage_id == _BCM_FIELD_STAGE_EGRESS) {
        /* Mask off IFP_CLASSID_TYPE */
        *data &= m;
        *mask &= m;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_td2_hash_select_profile_alloc_reserve_entry
 * Purpose:
 *     Index 0 of vfp_hash_field_bmap_table_a/b is invalid as the
 *     hardware will consider hash bitmap selection from VFP only
 *     if HASH_FIELD_BITMAP_PTR_A and HASH_FIELD_BITMAP_PTR_B are non-zero.
 *     Making the entry to NULL.
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage structure.
 *
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */
int _bcm_field_td2_hash_select_profile_alloc_reserve_entry(int unit,
                                            _field_stage_t *stage_fc)
{
    vfp_hash_field_bmap_table_a_entry_t entry_arr[2];
    uint32            *entry_ptr[2];
    int               rv, ct;
    void              *entries[1];
    uint32            null = 0x0;
    soc_mem_t         profile_mem[2] = { VFP_HASH_FIELD_BMAP_TABLE_Am,
                                         VFP_HASH_FIELD_BMAP_TABLE_Bm };
    uint32            hw_index = 0;

    if ((stage_fc == NULL) ||
        (stage_fc->stage_id != _BCM_FIELD_STAGE_LOOKUP)) {
        return BCM_E_PARAM;
    }

    for (ct = 0; ct < 2; ct++) {
        entry_ptr[0] = (uint32 *)entry_arr;
        entry_ptr[1] = entry_ptr[0] +
                           soc_mem_entry_words(unit, profile_mem[ct]);
        entries[0] = (void *)&entry_arr;

        /* Reset hash select profile entry. */
        sal_memcpy(entry_ptr[0], soc_mem_entry_null(unit, profile_mem[ct]),
                   soc_mem_entry_words(unit, profile_mem[ct]) * sizeof(uint32));
        sal_memcpy(entry_ptr[1], soc_mem_entry_null(unit, profile_mem[ct]),
                   soc_mem_entry_words(unit, profile_mem[ct]) * sizeof(uint32));
 
        soc_mem_field_set(unit, profile_mem[ct], entry_ptr[0], 
                            BITMAPf, &null);

        rv = soc_profile_mem_add(unit, &stage_fc->hash_select[ct], entries,
                                 1, &hw_index);
        BCM_IF_ERROR_RETURN(rv);
        if (hw_index != 0) {
           return BCM_E_INTERNAL;
        }
    }

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC soc_field_t _td2_vfp_slice_wide_mode_field[4] = {
    SLICE_0_DOUBLE_WIDE_MODEf,
    SLICE_1_DOUBLE_WIDE_MODEf,
    SLICE_2_DOUBLE_WIDE_MODEf,
    SLICE_3_DOUBLE_WIDE_MODEf};

/*
 * Function:
 *     _bcm_field_td2_scache_sync
 *
 * Purpose:
 *     Save field module software state to external cache.
 *
 * Parameters:
 *     unit             - (IN) BCM device number
 *     fc               - (IN) Pointer to device field control structure
 *     stage_fc         - (IN) FP stage control info.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_td2_scache_sync(int              unit,
                           _field_control_t *fc,
                           _field_stage_t   *stage_fc
                           )
{
    uint8 *buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    uint8 *buf1 = fc->scache_ptr[_FIELD_SCACHE_PART_1];
    uint32 start_char, end_char;
    uint32 val;
    uint64 rval;
    int slice_idx, range_count = 0;
    int rv = BCM_E_NONE;
    int efp_slice_mode, paired = 0;
    _field_slice_t *fs;
    _field_group_t *fg;
    _field_data_control_t *data_ctrl;
    _field_range_t *fr;
    fp_port_field_sel_entry_t pfs;
    soc_field_t fld;
    int ratio = 0;
    int idx;
    _field_entry_t *f_ent;

    soc_field_t _fb2_slice_pairing_field[8] = {
        SLICE1_0_PAIRINGf,   SLICE3_2_PAIRINGf,
        SLICE5_4_PAIRINGf,   SLICE7_6_PAIRINGf,
        SLICE9_8_PAIRINGf,   SLICE11_10_PAIRINGf,
        SLICE13_12_PAIRINGf, SLICE15_14_PAIRINGf};
    soc_field_t _efp_slice_mode[] = {SLICE_0_MODEf, SLICE_1_MODEf,
                                     SLICE_2_MODEf, SLICE_3_MODEf};

    switch (stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_INGRESS:
            start_char = _FIELD_IFP_DATA_START;
            end_char = _FIELD_IFP_DATA_END;
            break;
        case _BCM_FIELD_STAGE_EGRESS:
            start_char = _FIELD_EFP_DATA_START;
            end_char = _FIELD_EFP_DATA_END;
            break;
        case _BCM_FIELD_STAGE_LOOKUP:
            start_char = _FIELD_VFP_DATA_START;
            end_char = _FIELD_VFP_DATA_END;
            break;
        case _BCM_FIELD_STAGE_EXTERNAL:
            /*
            if (_field_tr2_ext_scache_sync_chk(unit, fc, stage_fc)) {
                return (_field_tr2_ext_scache_sync(unit, fc, stage_fc));
            } */
            start_char = _FIELD_EXTFP_DATA_START;
            end_char   = _FIELD_EXTFP_DATA_END;
            break;
        default:
            return BCM_E_PARAM;
    }

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "FP(unit %d) vverb: _bcm_field_td2_scache_sync() - Synching scache"
                           " for FP stage %d...\n"), unit, stage_fc->stage_id));

    _field_scache_stage_hdr_save(fc, start_char);

    /* Save the range check IDs */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        fr = stage_fc->ranges;
        while (fr) {
            fr = fr->next;
            range_count++;
        }
        buf[fc->scache_pos] = (uint8)range_count;
        fc->scache_pos++;
        if (range_count) {
            fr = stage_fc->ranges;
            while (fr) {
                buf[fc->scache_pos] = fr->rid & 0xFF;
                fc->scache_pos++;
                buf[fc->scache_pos] = (fr->rid >> 8) & 0xFF;
                fc->scache_pos++;
                buf[fc->scache_pos] = (fr->rid >> 16) & 0xFF;
                fc->scache_pos++;
                buf[fc->scache_pos] = (fr->rid >> 24) & 0xFF;
                fc->scache_pos++;
                fr = fr->next;
            }
        }

        /* Sync Hints information */
        if (soc_feature(unit, soc_feature_field_ingress_two_slice_types)) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,"Syncing hints @pos= %d\r\n"),
                                                     fc->scache_pos));
            BCM_IF_ERROR_RETURN (_bcm_field_hints_scache_sync(unit,

                        fc, stage_fc, &buf[fc->scache_pos]));
        }

       /* Write data qual count */
        buf[fc->scache_pos] = (uint8)(bcmFieldQualifyCount >> 8);
        fc->scache_pos++;
        buf[fc->scache_pos] = (uint8)bcmFieldQualifyCount;
        fc->scache_pos++;

    }

    /* Save data qualifiers */
    if ((data_ctrl = stage_fc->data_ctrl) != 0) {
        if (soc_mem_is_valid(unit, FP_UDF_TCAMm)) {
            /* Device has UDF TCAM =>
               Save internal information regarding TCAM entry usage
            */

            _field_data_tcam_entry_t *p;
            unsigned                 n;

            for (p = data_ctrl->tcam_entry_arr,
                     n = soc_mem_index_count(unit, FP_UDF_TCAMm);
                 n;
                 --n, ++p
                 ) {
                buf[fc->scache_pos] = p->ref_count;
                fc->scache_pos++;
            }
        }

        _field_scache_sync_data_quals_write(fc, data_ctrl);
    }

    _field_slice_map_write(unit, fc, stage_fc);

    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "FP(unit %d) vverb: _bcm_field_td2_scache_sync() - "
                               "Checking slice %d...\n"), unit, slice_idx));
        /* Skip slices without groups */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        fg = fc->groups;
        while (fg != NULL) {
            if (fg->stage_id != stage_fc->stage_id) {
                fg = fg->next;
                continue; /* Not in this stage */
            }
            if (fg->slices[0].slice_number == slice_idx) {
                break;
            }
            fg = fg->next;
        }

        if (fg == NULL) {
            continue; /* No group found */
        }

        /* Also skip expanded slices */
        if ((stage_fc->slices[_FP_DEF_INST] + slice_idx)->prev != NULL) {
            continue;
        }

        paired = 0;

        /* Ignore secondary slice in paired mode */
        switch (stage_fc->stage_id) {
            case _BCM_FIELD_STAGE_INGRESS:
                BCM_IF_ERROR_RETURN(soc_mem_read(unit,
                                                 FP_PORT_FIELD_SELm,
                                                 MEM_BLOCK_ANY,
                                                 0,
                                                 &pfs));
                fld = _fb2_slice_pairing_field[slice_idx / 2];
                paired = soc_FP_PORT_FIELD_SELm_field32_get(unit,
                                                            &pfs,
                                                            fld);
                break;
            case _BCM_FIELD_STAGE_EGRESS:
                BCM_IF_ERROR_RETURN(READ_EFP_SLICE_CONTROLr(unit, &val));
                efp_slice_mode = soc_reg_field_get(unit,
                                     EFP_SLICE_CONTROLr,
                                     val,
                                     _efp_slice_mode[slice_idx]);
                if ((efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE)
                    || (efp_slice_mode
                    == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY)
                    || (efp_slice_mode
                    == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6)) {
                    paired = 1;
                }
                break;
            case _BCM_FIELD_STAGE_LOOKUP:
                BCM_IF_ERROR_RETURN(READ_VFP_KEY_CONTROL_1r(unit, &rval));
                fld = _bcm_field_trx_slice_pairing_field[slice_idx / 2];
                paired = soc_reg64_field32_get(unit,
                            VFP_KEY_CONTROL_1r,
                            rval,
                            fld);
                break;
            case _BCM_FIELD_STAGE_EXTERNAL:
                paired = 0;
                break;
            default:
                return BCM_E_PARAM;
                break;
        }
        if (paired && (slice_idx % 2)) {
            continue;
        }
        BCM_IF_ERROR_RETURN
            (_field_tr2_group_entry_write(unit, _FP_DEF_INST,
                               slice_idx, fs, fc, stage_fc));
    }

    /* Now sync the expanded slices */
    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        /* Skip empty slices */
        if (fs->entry_count == fs->free_count) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "FP(unit %d) vverb: _bcm_field_td2_scache_sync() -"
                                   " Slice is empty.\n"), unit));
            continue;
        }

        /* Skip master slices */
        if ((stage_fc->slices[_FP_DEF_INST] + slice_idx)->prev == NULL) {
            continue;
        }

        /* 
         * Skip expanded slices with no entries installed in Hw
         * to match recovery logic.
         */
        if (fs->group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
            ratio = 2;
        } else {
            ratio = 1;
        }

        for (idx = 0; idx < fs->entry_count / ratio; idx++) {
            /* Find EID that matches this HW index */
            f_ent = fs->entries[idx];
            if (f_ent == NULL) {
                continue;
            }
            if (!(f_ent->flags & _FP_ENTRY_INSTALLED)) {
                continue;
            }
            break;
        }

        if (idx == (fs->entry_count / ratio)) {
            continue;
        }

        paired = 0;

        /* Ignore secondary slice in paired mode */
        switch (stage_fc->stage_id) {
            case _BCM_FIELD_STAGE_INGRESS:
                BCM_IF_ERROR_RETURN
                    (soc_mem_read(unit,
                        FP_PORT_FIELD_SELm,
                        MEM_BLOCK_ANY,
                        0,
                        &pfs));
                fld = _fb2_slice_pairing_field[slice_idx / 2];
                paired = soc_FP_PORT_FIELD_SELm_field32_get(unit,
                            &pfs,
                            fld);
                break;
            case _BCM_FIELD_STAGE_EGRESS:
                BCM_IF_ERROR_RETURN(READ_EFP_SLICE_CONTROLr(unit, &val));
                efp_slice_mode = soc_reg_field_get(unit,
                                    EFP_SLICE_CONTROLr,
                                    val,
                                    _efp_slice_mode[slice_idx]);
                if ((efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE)
                    || (efp_slice_mode
                    == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY)
                    || (efp_slice_mode
                    == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6)) {
                    paired = 1;
                }
                break;
            case _BCM_FIELD_STAGE_LOOKUP:
                BCM_IF_ERROR_RETURN(READ_VFP_KEY_CONTROL_1r(unit, &rval));
                fld = _bcm_field_trx_slice_pairing_field[slice_idx / 2];
                paired = soc_reg64_field32_get(unit,
                            VFP_KEY_CONTROL_1r,
                            rval,
                            fld);
                break;
            case _BCM_FIELD_STAGE_EXTERNAL:
                paired = 0;
                break;
            default:
                return BCM_E_PARAM;
                break;
        }
        if (paired && (slice_idx % 2)) {
            continue;
        }
        BCM_IF_ERROR_RETURN
            (_field_tr2_group_entry_write(unit, _FP_DEF_INST,
                               slice_idx, fs, fc, stage_fc));
    }

    if ((fc->wb_current_version) >= BCM_FIELD_WB_VERSION_1_15) {
       /* Mark end of Slice Info */
       buf[fc->scache_pos] = _FP_WB_END_OF_SLICES;
       fc->scache_pos++;
    }

    if (soc_feature(unit, soc_feature_field_ingress_two_slice_types)) {
        if ( stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS ||
             stage_fc->stage_id == _BCM_FIELD_STAGE_EGRESS ||
             stage_fc->stage_id == _BCM_FIELD_STAGE_LOOKUP
           ) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,"FP(unit %d) vverb:"
                                "_bcm_field_td2_scache_sync() -section "
                                "Writing  HintIds information @ byte %d.\n"),
                                                        unit, fc->scache_pos));
            BCM_IF_ERROR_RETURN (_bcm_hints_scache_hintid_sync (unit, stage_fc,
                        &buf[fc->scache_pos]));
        }
    }

    if ((fc->wb_current_version) >= BCM_FIELD_WB_VERSION_1_15) {
        /* Group Slice Selectors */
        BCM_IF_ERROR_RETURN(_field_group_slice_selectors_sync(unit,
                            &buf[fc->scache_pos], stage_fc->stage_id));
    }

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "FP(unit %d) vverb: _bcm_field_td2_scache_sync() -"
                           " Writing end of section @ byte %d.\n"), unit, fc->scache_pos));

    /* Mark the end of the IFP section */
    buf[fc->scache_pos] = end_char & 0xFF;
    fc->scache_pos++;
    buf[fc->scache_pos] = (end_char >> 8) & 0xFF;
    fc->scache_pos++;
    buf[fc->scache_pos] = (end_char >> 16) & 0xFF;
    fc->scache_pos++;
    buf[fc->scache_pos] = (end_char >> 24) & 0xFF;
    fc->scache_pos++;
    fc->scache_usage = fc->scache_pos; /* Usage in bytes */

    if(NULL != buf1) {
        /* Mark the end of the IFP section */
        buf1[fc->scache_pos1] = end_char & 0xFF;
        fc->scache_pos1++;
        buf1[fc->scache_pos1] = (end_char >> 8) & 0xFF;
        fc->scache_pos1++;
        buf1[fc->scache_pos1] = (end_char >> 16) & 0xFF;
        fc->scache_pos1++;
        buf1[fc->scache_pos1] = (end_char >> 24) & 0xFF;
        fc->scache_pos1++;
    }

    /* Check for overflow */
    if (fc->scache_pos > fc->scache_size[_FIELD_SCACHE_PART_0]) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "FP(unit %d) Error: Current Scache Position[%u] exceeds Scache size[%u]\n"),
                 unit, fc->scache_pos, fc->scache_size[_FIELD_SCACHE_PART_0]));
        return BCM_E_INTERNAL;
    }

    if (buf1 != NULL) {
        if (fc->scache_pos1 > fc->scache_size[_FIELD_SCACHE_PART_1]) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,
                                "FP(unit %d) Error: Current Scache Position1[%u] exceeds Scache size[%u]\n"),
                     unit, fc->scache_pos1, fc->scache_size[_FIELD_SCACHE_PART_1]));
            return BCM_E_INTERNAL;
        }
    }

    return rv;
}

/*
 * Function:
 *     _bcm_field_td2_stage_lookup_reinit
 *
 * Purpose:
 *     Reconstruct's VFP/Lookup stage groups and entries.
 *
 * Parameters:
 *     unit             - (IN) BCM device number.
 *     fc               - (IN) Device field control structure pointer.
 *     stage_fc         - (IN) FP stage control info.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_td2_stage_lookup_reinit(int unit,
                               _field_control_t *fc,
                               _field_stage_t *stage_fc)
{
    int vslice_idx, max, master_slice;
    int idx, idx1, slice_idx, index_min, index_max, ratio, rv = BCM_E_NONE;
    int group_found, mem_sz, parts_count, slice_ent_cnt, expanded[4], slice_master_idx[4];
    int i, pri_tcam_idx, part_index, slice_number, prev_prio, multigroup;
    int paired, intraslice;
    char *vfp_policy_buf = NULL; /* Buffer to read the VFP_POLICY table */
    uint8 *buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    uint8 *buf1 = fc->scache_ptr[_FIELD_SCACHE_PART_1];
    uint32 rval, dbl_wide_key, dbl_wide_key_sec;
    uint32 *vfp_tcam_buf = NULL; /* Buffer to read the VFP_TCAM table */
    uint32 vfp_key2, temp;
    uint64 vfp_key_1;
    soc_field_t fld;
    vfp_tcam_entry_t *vfp_tcam_entry;
    vfp_policy_table_entry_t *vfp_policy_entry;
    _field_hw_qual_info_t hw_sels;
    _field_slice_t *fs;
    _field_group_t *fg;
    _field_entry_t *f_ent = NULL;
    bcm_pbmp_t entry_pbmp, temp_pbmp;
    uint8 old_physical_slice, slice_num;
    uint32 entry_flags;
    _field_slice_t *fs_temp = NULL;
#if defined(BCM_APACHE_SUPPORT)
    int offset_mode = 0, policer_index = 0;
#endif
    soc_field_t vfp_en_flds[4] = {SLICE_ENABLE_SLICE_0f,
                                  SLICE_ENABLE_SLICE_1f,
                                  SLICE_ENABLE_SLICE_2f,
                                  SLICE_ENABLE_SLICE_3f};

    soc_field_t vfp_lk_en_flds[4] = {LOOKUP_ENABLE_SLICE_0f,
                                     LOOKUP_ENABLE_SLICE_1f,
                                     LOOKUP_ENABLE_SLICE_2f,
                                     LOOKUP_ENABLE_SLICE_3f};
    static const soc_field_t s_type_fld_tbl[] = {
        SLICE_0_S_TYPE_SELf,
        SLICE_1_S_TYPE_SELf,
        SLICE_2_S_TYPE_SELf,
        SLICE_3_S_TYPE_SELf
    };

    _field_action_bmp_t action_bmp;
    _field_entry_wb_info_t f_ent_wb_info;

    /* Reset Action bitmap to NULL. */
    action_bmp.w = NULL;

    SOC_PBMP_CLEAR(entry_pbmp);
    sal_memset(expanded, 0, 4 * sizeof(int));
    sal_memset(slice_master_idx, 0, 4 * sizeof(int));

    if (fc->l2warm) {
        rv = _field_scache_stage_hdr_chk(fc, _FIELD_VFP_DATA_START);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
    }

    /* DMA various tables */
    vfp_tcam_buf = soc_cm_salloc(unit,
                    sizeof(vfp_tcam_entry_t) *
                    soc_mem_index_count(unit, VFP_TCAMm),
                    "VFP TCAM buffer");
    if (NULL == vfp_tcam_buf) {
        return BCM_E_MEMORY;
    }

    sal_memset(vfp_tcam_buf,
               0,
               sizeof(vfp_tcam_entry_t) *
               soc_mem_index_count(unit, VFP_TCAMm)
               );
    index_min = soc_mem_index_min(unit, VFP_TCAMm);
    index_max = soc_mem_index_max(unit, VFP_TCAMm);

    fs = stage_fc->slices[_FP_DEF_INST];

    if (stage_fc->flags & _FP_STAGE_HALF_SLICE) {
        slice_ent_cnt = fs->entry_count * 2;
        /* DMA in chunks */
        for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
            fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
            if ((rv = soc_mem_read_range
                        (unit,
                         VFP_TCAMm, MEM_BLOCK_ALL,
                         slice_idx * slice_ent_cnt,
                         slice_idx * slice_ent_cnt + fs->entry_count / 2 - 1,
                         vfp_tcam_buf + slice_idx * slice_ent_cnt
                         * soc_mem_entry_words(unit, VFP_TCAMm))
                         ) < 0) {
                goto cleanup;
            }

            if ((rv = soc_mem_read_range(unit,
                        VFP_TCAMm, MEM_BLOCK_ALL,
                        slice_idx * slice_ent_cnt + fs->entry_count,
                        slice_idx * slice_ent_cnt
                        + fs->entry_count + fs->entry_count / 2 - 1,
                        vfp_tcam_buf + (slice_idx
                        * slice_ent_cnt + fs->entry_count)
                        * soc_mem_entry_words(unit, VFP_TCAMm))
                        ) < 0 ) {
                goto cleanup;
            }
        }
    } else {
        slice_ent_cnt = fs->entry_count;
        if ((rv = soc_mem_read_range(unit,
                    VFP_TCAMm,
                    MEM_BLOCK_ALL,
                    index_min,
                    index_max,
                    vfp_tcam_buf)
                    ) < 0 ) {
            goto cleanup;
        }
    }

    vfp_policy_buf = soc_cm_salloc(unit,
                        SOC_MEM_TABLE_BYTES(unit, VFP_POLICY_TABLEm),
                        "VFP POLICY TABLE buffer"
                        );
    if (NULL == vfp_policy_buf) {
        return BCM_E_MEMORY;
    }
    index_min = soc_mem_index_min(unit, VFP_POLICY_TABLEm);
    index_max = soc_mem_index_max(unit, VFP_POLICY_TABLEm);
    if ((rv = soc_mem_read_range(unit,
                VFP_POLICY_TABLEm,
                MEM_BLOCK_ALL,
                index_min,
                index_max,
                vfp_policy_buf)
                ) < 0 ) {
        goto cleanup;
    }

    /* Get slice expansion status and virtual map */
    if ((rv = _field_slice_expanded_status_get
                (unit,
                 fc,
                 stage_fc,
                 expanded,
                 slice_master_idx)
                ) < 0) {
        goto cleanup;
    }

    /* Iterate over the slices */
    if ((rv = READ_VFP_SLICE_CONTROLr(unit, &rval)) < 0) {
        goto cleanup;
    }

    if ((rv = READ_VFP_KEY_CONTROL_1r(unit, &vfp_key_1)) < 0) {
        goto cleanup;
    }

    if ((rv = READ_VFP_KEY_CONTROL_2r(unit, &vfp_key2)) < 0) {
        goto cleanup;
    }

    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {

        /* Ignore disabled slice */
        if ((soc_reg_field_get
                (unit,
                 VFP_SLICE_CONTROLr,
                 rval,
                 vfp_en_flds[slice_idx]
                 ) == 0)
            || (soc_reg_field_get
                    (unit,
                     VFP_SLICE_CONTROLr,
                     rval, vfp_lk_en_flds[slice_idx]
                     ) == 0)) {
            continue;
        }

        /* Ignore secondary slice in paired mode */
        fld = _bcm_field_trx_slice_pairing_field[slice_idx / 2];
        paired = soc_reg64_field32_get
                    (unit,
                     VFP_KEY_CONTROL_1r,
                     vfp_key_1,
                     fld
                     );

        fld = _td2_vfp_slice_wide_mode_field[slice_idx];
        intraslice = soc_reg64_field32_get
                        (unit,
                         VFP_KEY_CONTROL_1r,
                         vfp_key_1,
                         fld
                         );
        if (paired && (slice_idx % 2)) {
            continue;
        }

        /* Don't need to read selectors for expanded slice */
        if (expanded[slice_idx]) {
            continue;
        }

        /* Skip if slice has no valid groups and entries */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        for (idx = 0; idx < slice_ent_cnt; idx++) {
            vfp_tcam_entry = soc_mem_table_idx_to_pointer
                                (unit,
                                 VFP_TCAMm,
                                 vfp_tcam_entry_t *,
                                 vfp_tcam_buf, idx + slice_ent_cnt
                                 * slice_idx
                                 );
            if (soc_VFP_TCAMm_field32_get(unit,
                                          vfp_tcam_entry,
                                          VALIDf) != 0) {
                break;
            }
        }
        if (idx == slice_ent_cnt && !fc->l2warm) {
            continue;
        }
        /* If Level 2, retrieve the GIDs in this slice */
        if (fc->l2warm) {
            rv = _field_trx_scache_slice_group_recover(unit,
                                                       fc, _FP_DEF_INST,
                                                       slice_idx,
                                                       &multigroup,
                                                       stage_fc,
                                                       0);
            if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                fc->l2warm = 0;
                goto cleanup;
            }
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_NONE;
                continue;
            }
        }

        /* Construct the group based on HW selector values */
        _FIELD_SELCODE_CLEAR(hw_sels.pri_slice[0]);
        hw_sels.pri_slice[0].intraslice = _FP_SELCODE_DONT_USE;
        _FIELD_SELCODE_CLEAR(hw_sels.pri_slice[1]);
        hw_sels.pri_slice[1].intraslice = _FP_SELCODE_DONT_USE;
        _FIELD_SELCODE_CLEAR(hw_sels.sec_slice[0]);
        hw_sels.sec_slice[0].intraslice = _FP_SELCODE_DONT_USE;
        _FIELD_SELCODE_CLEAR(hw_sels.sec_slice[1]);
        hw_sels.sec_slice[1].intraslice = _FP_SELCODE_DONT_USE;

        /* Get primary slice's selectors */
        hw_sels.pri_slice[0].fpf1 = 0;
        hw_sels.pri_slice[0].fpf2
            = soc_reg64_field32_get
                (unit,
                 VFP_KEY_CONTROL_1r,
                 vfp_key_1,
                 _bcm_field_trx_vfp_field_sel[slice_idx][0]
                 );
        hw_sels.pri_slice[0].fpf3
            = soc_reg64_field32_get
                (unit,
                 VFP_KEY_CONTROL_1r,
                 vfp_key_1,
                 _bcm_field_trx_vfp_field_sel[slice_idx][1]
                 );

        hw_sels.pri_slice[0].ip_header_sel
            = soc_reg_field_get
                (unit,
                 VFP_KEY_CONTROL_2r,
                 vfp_key2,
                 _bcm_field_trx_vfp_ip_header_sel[slice_idx]
                 );

        hw_sels.pri_slice[0].src_entity_sel
            = soc_reg64_field32_get(unit,
                    VFP_KEY_CONTROL_1r,
                    vfp_key_1,
                    s_type_fld_tbl[slice_idx]
                    );

        /* If intraslice, get double-wide key - only 2 options */
        if (intraslice) {
            dbl_wide_key = soc_reg64_field32_get
                            (unit,
                             VFP_KEY_CONTROL_1r,
                             vfp_key_1,
                             _bcm_field_trx_vfp_double_wide_sel[slice_idx]
                             );
            hw_sels.pri_slice[1].intraslice = TRUE;
            hw_sels.pri_slice[1].fpf2 = dbl_wide_key;
            hw_sels.pri_slice[1].fpf3 = 0;
            hw_sels.pri_slice[1].ip_header_sel
                = soc_reg_field_get
                    (unit,
                     VFP_KEY_CONTROL_2r,
                     vfp_key2,
                    _bcm_field_trx_vfp_ip_header_sel[slice_idx]
                    );

            hw_sels.pri_slice[1].src_entity_sel
                = soc_reg64_field32_get(unit,
                        VFP_KEY_CONTROL_1r,
                        vfp_key_1,
                        s_type_fld_tbl[slice_idx]
                        );
        }

        /* If paired, get secondary slice's selectors */
        if (paired) {
            hw_sels.sec_slice[0].fpf1 = 0;
            hw_sels.sec_slice[0].fpf2
                = soc_reg64_field32_get
                    (unit,
                     VFP_KEY_CONTROL_1r,
                     vfp_key_1,
                     _bcm_field_trx_vfp_field_sel[slice_idx + 1][0]
                     );
            hw_sels.sec_slice[0].fpf3
                = soc_reg64_field32_get
                    (unit,
                     VFP_KEY_CONTROL_1r,
                     vfp_key_1,
                     _bcm_field_trx_vfp_field_sel[slice_idx + 1][1]
                     );
            hw_sels.sec_slice[0].ip_header_sel
                = soc_reg_field_get
                    (unit,
                     VFP_KEY_CONTROL_2r, vfp_key2,
                     _bcm_field_trx_vfp_ip_header_sel[slice_idx + 1]
                     );

            hw_sels.sec_slice[0].src_entity_sel
                = soc_reg64_field32_get(unit,
                        VFP_KEY_CONTROL_1r,
                        vfp_key_1,
                        s_type_fld_tbl[slice_idx+1]
                        );

            /* If in intraslie double wide mode, get DW keysel value. */
            if (intraslice) {
                dbl_wide_key_sec
                    = soc_reg64_field32_get
                        (unit,
                         VFP_KEY_CONTROL_1r,
                         vfp_key_1,
                         _bcm_field_trx_vfp_double_wide_sel[slice_idx + 1]
                         );
                hw_sels.sec_slice[1].intraslice = TRUE;
                hw_sels.sec_slice[1].fpf2 = dbl_wide_key_sec;
                hw_sels.sec_slice[1].fpf3 = 0;
                hw_sels.sec_slice[1].ip_header_sel
                    = soc_reg_field_get
                        (unit,
                         VFP_KEY_CONTROL_2r,
                         vfp_key2,
                         _bcm_field_trx_vfp_ip_header_sel[slice_idx + 1]
                         );

                hw_sels.sec_slice[1].src_entity_sel
                    = soc_reg64_field32_get(unit,
                            VFP_KEY_CONTROL_1r,
                            vfp_key_1,
                            s_type_fld_tbl[slice_idx+1]
                            );
            }
        }

        /* Create a group based on HW qualifiers (or find existing) */
        rv = _field_tr2_group_construct
                (unit, _FP_DEF_INST, &hw_sels, intraslice,
                 paired, fc, -1,
                 _BCM_FIELD_STAGE_LOOKUP,
                 slice_idx
                 );
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        /* Now go over the entries */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        if (fs->group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
            fs->free_count >>= 1;
            ratio = 2;
        } else {
            ratio = 1;
        }
        prev_prio = -1;
        for (idx = 0; idx < slice_ent_cnt / ratio; idx++) {
            group_found = 0;
            vfp_tcam_entry = soc_mem_table_idx_to_pointer
                                 (unit,
                                  VFP_TCAMm,
                                  vfp_tcam_entry_t *, vfp_tcam_buf,
                                  idx + slice_ent_cnt * slice_idx
                                  );
            if (soc_VFP_TCAMm_field32_get
                    (unit,
                     vfp_tcam_entry,
                     VALIDf
                     ) == 0) {
                continue;
            }

            /* All ports are applicable to this entry */
            SOC_PBMP_ASSIGN(entry_pbmp, PBMP_PORT_ALL(unit));
            SOC_PBMP_OR(entry_pbmp, PBMP_CMIC(unit));

            /* Search groups to find match */
            fg = fc->groups;
            while (fg != NULL) {

                /* Check if group is in this slice */
                fs = &fg->slices[0];
                if (fs->slice_number != slice_idx) {
                    fg = fg->next;
                    continue;
                }

                /* Check if entry_pbmp is a subset of group pbmp */
                SOC_PBMP_ASSIGN(temp_pbmp, fg->pbmp);
                SOC_PBMP_AND(temp_pbmp, entry_pbmp);
                if (SOC_PBMP_EQ(temp_pbmp, entry_pbmp)) {
                    group_found = 1;
                    break;
                }
                fg = fg->next;
            }

            if (!group_found) {
                return BCM_E_INTERNAL; /* Should never happen */
            }

            /* Allocate memory for the entry */
            rv = _bcm_field_entry_tcam_parts_count(unit, fg->stage_id,
                                                   fg->flags, &parts_count);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            mem_sz = parts_count * sizeof (_field_entry_t);
            _FP_XGS3_ALLOC(f_ent, mem_sz, "field entry");
            if (f_ent == NULL) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }
            for (idx1 = 0; idx1 < _FP_POLICER_LEVEL_COUNT; idx1++) {
                 f_ent->policer[idx1].pid = _FP_INVALID_INDEX;
            }


            sal_memset(&f_ent_wb_info, 0, sizeof(_field_entry_wb_info_t));
            f_ent_wb_info.action_bmp = &action_bmp;
            f_ent_wb_info.sid = f_ent_wb_info.pid = f_ent_wb_info.pid1 = -1;
            if (fc->l2warm) {
                if ((fc->wb_recovered_version) > BCM_FIELD_WB_VERSION_1_14) {
                   action_bmp.w = NULL;
                   _FP_XGS3_ALLOC(action_bmp.w,
                            SHR_BITALLOCSIZE(_bcmFieldActionNoParamCount),
                            "Action No Param Bitmap");
                   if (action_bmp.w == NULL) {
                       rv = BCM_E_MEMORY;
                       sal_free(f_ent);
                       goto cleanup;
                   }
                }

                rv = _field_trx_entry_info_retrieve(unit,
                                                    fc,
                                                    stage_fc,
                                                    multigroup,
                                                    &prev_prio,
                                                    &f_ent_wb_info
                                                    );
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
            } else {
                _bcm_field_last_alloc_eid_incr(unit);
            }

            pri_tcam_idx = idx + slice_ent_cnt * slice_idx;
            for (i = 0; i < parts_count; i++) {
                if (fc->l2warm) {
                    /* Use retrieved EID */
                    f_ent[i].eid = f_ent_wb_info.eid;
                    /* Set retrieved dvp_type and svp_type */
                    f_ent[i].dvp_type = f_ent_wb_info.dvp_type[i];
                    f_ent[i].svp_type = f_ent_wb_info.svp_type[i];
                    f_ent[i].prio = f_ent_wb_info.prio;
                } else {
                    f_ent[i].eid = _bcm_field_last_alloc_eid_get(unit);
                    f_ent[i].prio = (slice_idx << 10) | (slice_ent_cnt - idx);
                }
                f_ent[i].group = fg;
                rv = _bcm_field_tcam_part_to_entry_flags
                        (unit, i,
                         fg,
                         &f_ent[i].flags
                         );
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                if (f_ent_wb_info.color_independent) {
                    f_ent[i].flags |= _FP_ENTRY_COLOR_INDEPENDENT;
                }
                if (f_ent_wb_info.vfp_gport_type) {
                       f_ent[i].flags |= _FP_ENTRY_QUAL_PORT_TYPE_TRUNK;
                }
                rv = _bcm_field_entry_part_tcam_idx_get
                        (unit,
                         f_ent,
                         pri_tcam_idx,
                         i,
                         &part_index
                         );
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc,
                         _FP_DEF_INST, part_index,       
                         &slice_number,
                         (int *)&f_ent[i].slice_idx
                         );
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                f_ent[i].fs = stage_fc->slices[_FP_DEF_INST] + slice_number;
                if (0 == (f_ent[i].flags & _FP_ENTRY_SECOND_HALF)) {
                    /* Decrement slice free entry count for primary
                       entries. */
                    f_ent[i].fs->free_count--;
                }
                /* Assign entry to a slice */
                f_ent[i].fs->entries[f_ent[i].slice_idx] = f_ent + i;
                BCM_PBMP_OR(f_ent[i].fs->pbmp, fg->pbmp);
                f_ent[i].flags |= _FP_ENTRY_INSTALLED;

                if (soc_VFP_TCAMm_field32_get(unit, vfp_tcam_entry, VALIDf) == 3) {
                    f_ent[i].flags |= _FP_ENTRY_ENABLED;
                }

                /* Get the actions associated with this part of the entry */
                vfp_policy_entry = soc_mem_table_idx_to_pointer
                                    (unit,
                                     VFP_POLICY_TABLEm,
                                     vfp_policy_table_entry_t *,
                                     vfp_policy_buf,
                                     part_index
                                     );
                rv = _field_tr2_actions_recover
                        (unit,
                         VFP_POLICY_TABLEm,
                         (uint32 *) vfp_policy_entry,
                         f_ent,
                         i,
                         &f_ent_wb_info);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
   
                if (action_bmp.w != NULL) {
                   _FP_ACTION_BMP_FREE(action_bmp);
                   action_bmp.w = NULL;
                }

                if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
                    _field_adv_flex_stat_info_retrieve(unit, f_ent->statistic.sid);
                }
#if defined(BCM_APACHE_SUPPORT)
                if (SOC_IS_APACHE(unit) && !SOC_IS_MONTEREY(unit)) {
                    policer_index = PolicyGet(unit,
                                    VFP_POLICY_TABLEm,
                                    vfp_policy_entry,
                                    SVC_METER_INDEXf);

                    offset_mode  = PolicyGet(unit,
                                   VFP_POLICY_TABLEm,
                                   vfp_policy_entry,
                                   SVC_METER_OFFSET_MODEf);

                    rv = _bcm_esw_get_policer_id_from_index_offset(unit,
                                   policer_index, offset_mode,
                                   &(f_ent->global_meter_policer.pid));

                    if (BCM_FAILURE(rv)) {
                        sal_free(f_ent);
                        goto cleanup;
                    }

                    f_ent->global_meter_policer.flags = PolicyGet(unit,
                                                 VFP_POLICY_TABLEm,
                                                 vfp_policy_entry,
                                                 SVC_METER_INDEX_PRIORITYf);
                }
#endif /* BCM_APACHE_SUPPORT */
            }
            /* Add to the group */
            rv = _field_group_entry_add(unit, fg, f_ent);
            if (BCM_FAILURE(rv)) {
                sal_free(f_ent);
                goto cleanup;
            }
            f_ent = NULL;
        }
        /* Free up the temporary slice group info */
        if (fc->l2warm) {
            if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_18) {
                /* update selcode and construct group qual offset */
                rv = _field_tr2_group_construct_quals_with_sel_update(unit, fc, stage_fc);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
            }
            _field_scache_slice_group_free(unit,
                                           fc,
                                           slice_idx
                                           );
        }
    }

    /* Now go over the expanded slices */
    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        if (!expanded[slice_idx]) {
            continue;
        }
        /* Ignore secondary slice in paired mode */
        fld = _bcm_field_trx_slice_pairing_field[slice_idx / 2];
        paired = soc_reg64_field32_get
                    (unit,
                     VFP_KEY_CONTROL_1r,
                     vfp_key_1,
                     fld
                     );
        if (paired && (slice_idx % 2)) {
            continue;
        }
        /* Skip if slice has no valid groups and entries */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        for (idx = 0; idx < slice_ent_cnt; idx++) {
            vfp_tcam_entry = soc_mem_table_idx_to_pointer
                                 (unit,
                                  VFP_TCAMm,
                                  vfp_tcam_entry_t *,
                                  vfp_tcam_buf,
                                  idx + slice_ent_cnt * slice_idx
                                  );
            if (soc_VFP_TCAMm_field32_get
                    (unit,
                     vfp_tcam_entry,
                     VALIDf) != 0) {
                break;
            }
        }

        if (idx == slice_ent_cnt) {
            continue;
        }

        /* If Level 2, retrieve the GIDs in this slice */
        if (fc->l2warm) {
            rv = _field_trx_scache_slice_group_recover
                    (unit,
                     fc, _FP_DEF_INST,
                     slice_idx,
                     &multigroup,
                     stage_fc,
                     0);
            if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                fc->l2warm = 0;
                goto cleanup;
            }
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_NONE;
                continue;
            }
        }

        /* Now find the master slice for this virtual group */
        vslice_idx = _field_physical_to_virtual(unit, _FP_DEF_INST,
                                                slice_idx, stage_fc);
        if (vslice_idx < 0) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }

        if (fc->l2warm
                && ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_8)) {
            master_slice = slice_master_idx[slice_idx];
        } else {
        max = -1;
        for (i = 0; i < stage_fc->tcam_slices; i++) {
            if ((stage_fc->vmap[_FP_DEF_INST][0][vslice_idx].virtual_group ==
                stage_fc->vmap[_FP_DEF_INST][0][i].virtual_group) && (i != vslice_idx)) {
                if (i > max) {
                    max = i;
                }
            }
        }
        if (max < 0) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }

        master_slice = stage_fc->vmap[_FP_DEF_INST][0][max].vmap_key;
        }
        /* See which group is in this slice - can be only one */
        fg = fc->groups;
        while (fg != NULL) {
            /* Check if group is in this slice */
            fs = &fg->slices[0];
            if ((fg->stage_id == stage_fc->stage_id)
                    && (fs->slice_number == master_slice)) {
                break;
            }
            fg = fg->next;
        }
        if (fg == NULL) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }

        /* Get number of entry parts for the group. */
        rv = _bcm_field_entry_tcam_parts_count (unit, fg->stage_id,
                                                fg->flags, &parts_count);
        BCM_IF_ERROR_RETURN(rv);

        old_physical_slice = fs->slice_number;

        /* Set up the new physical slice parameters in Software */
        for(part_index = parts_count - 1; part_index >= 0; part_index--) {
            /* Get entry flags. */
            rv = _bcm_field_tcam_part_to_entry_flags(unit, part_index, fg, &entry_flags);
            BCM_IF_ERROR_RETURN(rv);

            /* Get slice id for entry part */
            rv = _bcm_field_tcam_part_to_slice_number(unit, part_index,
                                                      fg, &slice_num);
            BCM_IF_ERROR_RETURN(rv);

            /* Get slice pointer. */
            fs = stage_fc->slices[_FP_DEF_INST] + slice_idx
                    + slice_num;

            if (0 == (entry_flags & _FP_ENTRY_SECOND_HALF)) {
                /* Set per slice configuration &  number of free entries in the slice.*/
                fs->free_count = fs->entry_count;
                if (fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
                    fs->free_count >>= 1;
                }
                /* Set group flags in in slice.*/
                fs->group_flags = fg->flags & _FP_GROUP_STATUS_MASK;

                /* Add slice to slices linked list . */
                fs_temp = stage_fc->slices[_FP_DEF_INST] + old_physical_slice + slice_num;
                /* To handle more than one auto expanded slice in a group */
                while (fs_temp->next != NULL) {
                    fs_temp = fs_temp->next;
                }
                fs_temp->next = fs;
                fs->prev = fs_temp;
            }
        }

        /* Now go over the entries */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        fs->group_flags = fg->flags & _FP_GROUP_STATUS_MASK;
        if (fs->group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
            ratio = 2;
        } else {
            ratio = 1;
        }

        prev_prio = -1;
        for (idx = 0; idx < slice_ent_cnt / ratio; idx++) {
            vfp_tcam_entry = soc_mem_table_idx_to_pointer
                                 (unit,
                                  VFP_TCAMm,
                                  vfp_tcam_entry_t *,
                                  vfp_tcam_buf,
                                  idx + slice_ent_cnt * slice_idx
                                  );
            if (soc_VFP_TCAMm_field32_get
                    (unit,
                     vfp_tcam_entry,
                     VALIDf
                     ) == 0) {
                continue;
            }

            /* All ports are applicable to this entry */
            SOC_PBMP_ASSIGN(entry_pbmp, PBMP_PORT_ALL(unit));
            SOC_PBMP_OR(entry_pbmp, PBMP_CMIC(unit));

            /* Search groups to find match */
            fg = fc->groups;
            while (fg != NULL) {

                /* Check if group is in this slice */
                fs = &fg->slices[0];
                if ((fg->stage_id != stage_fc->stage_id)
                        || (fs->slice_number != master_slice)) {
                    fg = fg->next;
                    continue;
                }

                /* Check if entry_pbmp is a subset of group pbmp */
                SOC_PBMP_ASSIGN(temp_pbmp, fg->pbmp);
                SOC_PBMP_AND(temp_pbmp, entry_pbmp);
                if (SOC_PBMP_EQ(temp_pbmp, entry_pbmp)) {
                    break;
                }
                fg = fg->next;
            }

            if (fg == NULL) {
                goto cleanup;
            }

            /* Allocate memory for the entry */
            rv = _bcm_field_entry_tcam_parts_count(unit,fg->stage_id,
                                                   fg->flags, &parts_count);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            mem_sz = parts_count * sizeof (_field_entry_t);
            _FP_XGS3_ALLOC(f_ent, mem_sz, "field entry");
            if (f_ent == NULL) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }

            for (idx1 = 0; idx1 < _FP_POLICER_LEVEL_COUNT; idx1++) {
                 f_ent->policer[idx1].pid = _FP_INVALID_INDEX;
            }

            sal_memset(&f_ent_wb_info, 0, sizeof(_field_entry_wb_info_t));
            f_ent_wb_info.action_bmp = &action_bmp;
            f_ent_wb_info.sid = f_ent_wb_info.pid = f_ent_wb_info.pid1 = -1;
            if (fc->l2warm) {
                if ((fc->wb_recovered_version) > BCM_FIELD_WB_VERSION_1_14) {
                   action_bmp.w = NULL;
                   _FP_XGS3_ALLOC(action_bmp.w,
                            SHR_BITALLOCSIZE(_bcmFieldActionNoParamCount),
                            "Action No Param Bitmap");
                   if (action_bmp.w == NULL) {
                       rv = BCM_E_MEMORY;
                       sal_free(f_ent);
                       goto cleanup;
                   }
                }

                rv = _field_trx_entry_info_retrieve
                        (unit,
                         fc,
                         stage_fc,
                         multigroup,
                         &prev_prio,
                         &f_ent_wb_info);

                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
            } else {
                _bcm_field_last_alloc_eid_incr(unit);
            }
            pri_tcam_idx = idx + slice_ent_cnt * slice_idx;
            for (i = 0; i < parts_count; i++) {
                if (fc->l2warm) {
                    /* Use retrieved EID */
                    f_ent[i].eid = f_ent_wb_info.eid;
                    /* Set retrieved dvp_type and svp_type */
                    f_ent[i].dvp_type = f_ent_wb_info.dvp_type[i];
                    f_ent[i].svp_type = f_ent_wb_info.svp_type[i];
                    f_ent[i].prio = f_ent_wb_info.prio;
                } else {
                    f_ent[i].eid = _bcm_field_last_alloc_eid_get(unit);
                    f_ent[i].prio = (slice_idx << 10) | (slice_ent_cnt - idx);
                }
                f_ent[i].group = fg;
                rv = _bcm_field_tcam_part_to_entry_flags
                        (unit, i,
                         fg,
                         &f_ent[i].flags
                         );
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                if (f_ent_wb_info.color_independent) {
                    f_ent[i].flags |= _FP_ENTRY_COLOR_INDEPENDENT;
                }
                if (f_ent_wb_info.vfp_gport_type) {
                       f_ent[i].flags |= _FP_ENTRY_QUAL_PORT_TYPE_TRUNK;
                }
                rv = _bcm_field_entry_part_tcam_idx_get
                        (unit,
                         f_ent,
                         pri_tcam_idx,
                         i,
                         &part_index
                         );
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                rv = _bcm_field_tcam_idx_to_slice_offset
                        (unit,
                         stage_fc,
                         _FP_DEF_INST,
                         part_index,
                        &slice_number,
                        (int *)&f_ent[i].slice_idx
                        );
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                f_ent[i].fs = stage_fc->slices[_FP_DEF_INST] + slice_number;
                if (0 == (f_ent[i].flags & _FP_ENTRY_SECOND_HALF)) {
                    /* Decrement slice free entry count for primary
                       entries. */
                    f_ent[i].fs->free_count--;
                }
                /* Assign entry to a slice */
                f_ent[i].fs->entries[f_ent[i].slice_idx] = f_ent + i;
                BCM_PBMP_OR(f_ent[i].fs->pbmp, fg->pbmp);
                f_ent[i].flags |= _FP_ENTRY_INSTALLED;

                if (soc_VFP_TCAMm_field32_get(unit, vfp_tcam_entry, VALIDf) == 3) {
                    f_ent[i].flags |= _FP_ENTRY_ENABLED;
                }

                /* Get the actions associated with this part of the entry */
                vfp_policy_entry = soc_mem_table_idx_to_pointer
                                    (unit,
                                     VFP_POLICY_TABLEm,
                                     vfp_policy_table_entry_t *,
                                     vfp_policy_buf,
                                     part_index
                                     );
                rv = _field_tr2_actions_recover
                        (unit,
                         VFP_POLICY_TABLEm,
                         (uint32 *) vfp_policy_entry,
                         f_ent,
                         i,
                         &f_ent_wb_info);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                if (action_bmp.w != NULL) {
                   _FP_ACTION_BMP_FREE(action_bmp);
                   action_bmp.w = NULL;
                }
                if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
                    _field_adv_flex_stat_info_retrieve(unit, f_ent->statistic.sid);
                }
#if defined(BCM_APACHE_SUPPORT)
                if (SOC_IS_APACHE(unit) && !SOC_IS_MONTEREY(unit)) {
                    policer_index = PolicyGet(unit,
                                    VFP_POLICY_TABLEm,
                                    vfp_policy_entry,
                                    SVC_METER_INDEXf);

                    offset_mode  = PolicyGet(unit,
                                   VFP_POLICY_TABLEm,
                                   vfp_policy_entry,
                                   SVC_METER_OFFSET_MODEf);

                    rv = _bcm_esw_get_policer_id_from_index_offset(unit,
                                   policer_index, offset_mode,
                                   &(f_ent->global_meter_policer.pid));

                    if (BCM_FAILURE(rv)) {
                        sal_free(f_ent);
                        goto cleanup;
                    }

                    f_ent->global_meter_policer.flags = PolicyGet(unit,
                                                 VFP_POLICY_TABLEm,
                                                 vfp_policy_entry,
                                                 SVC_METER_INDEX_PRIORITYf);
                }
#endif /* BCM_APACHE_SUPPORT */
            }
            /* Add to the group */
            rv = _field_group_entry_add(unit, fg, f_ent);
            if (BCM_FAILURE(rv)) {
                sal_free(f_ent);
                goto cleanup;
            }
            f_ent = NULL;
        }
        /* Free up the temporary slice group info */
        if (fc->l2warm) {
            _field_scache_slice_group_free(unit,
                                           fc,
                                           slice_idx
                                           );
        }
    }

    if (fc->l2warm) {

        /* Mark end of Slice Info */
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
            fc->scache_pos++;
        }

        if (soc_feature(unit, soc_feature_field_ingress_two_slice_types)) {
            if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
                if (stage_fc->stage_id == _BCM_FIELD_STAGE_LOOKUP) {
                    LOG_DEBUG(BSL_LS_BCM_FP,
                            (BSL_META_U(unit,"Recovering hint Ids "
                                        "from pos = %d\r\n"), fc->scache_pos));
                    BCM_IF_ERROR_RETURN (_bcm_hints_scache_hintid_recover(
                                            unit, stage_fc,
                                            fc, &buf[fc->scache_pos]));
                }
            }
        }

        /* Group Slice Selectors */
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
            rv = (_field_group_slice_selectors_recover(unit,
                        &buf[fc->scache_pos], stage_fc->stage_id));
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }

        temp = 0;
        temp |= buf[fc->scache_pos];
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 8;
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 16;
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 24;
        fc->scache_pos++;
        if (temp != _FIELD_VFP_DATA_END) {
            fc->l2warm = 0;
            rv = BCM_E_INTERNAL;
        }

        if (NULL != buf1) {
            temp = 0;
            temp |= buf1[fc->scache_pos1];
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 8;
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 16;
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 24;
            fc->scache_pos1++;
            if (temp != _FIELD_VFP_DATA_END) {
                fc->l2warm = 0;
                rv = BCM_E_INTERNAL;
            }
        }

    }

    if (BCM_SUCCESS(rv)) {
        _field_group_slice_vmap_recovery(unit, fc, stage_fc);
    }

    _field_tr2_stage_reinit_all_groups_cleanup
        (unit,
         fc,
         _BCM_FIELD_STAGE_LOOKUP,
         NULL
         );

cleanup:
    if (vfp_tcam_buf) {
        soc_cm_sfree(unit, vfp_tcam_buf);
    }
    if (vfp_policy_buf) {
        soc_cm_sfree(unit, vfp_policy_buf);
    }
    if (action_bmp.w != NULL) {
       _FP_ACTION_BMP_FREE(action_bmp);
       action_bmp.w = NULL;
    }
    return rv;
}

/*
 * Function:
 *     _bcm_field_td2_egress_selcode_set
 *
 * Purpose:
 *     Sets the select code based on the EFP slice mode and key match type
 *
 * Parameters:
 *     unit             - (IN) BCM device number
 *     fg               - (IN/OUT) Select code information filled into the group
 *     efp_slice_mode   - (IN) EFP slice mode from EFP_SLICE_CONTROl register
 *     efp_key_mode     - (IN) EFP key mode from EFP_SLICE_CONTROl register
 *     key_match_type   - (IN) EFP key match type
 *     parts_count      - (OUT) Number of parts in entry
 *
 * Returns:
 *     BCM_E_NONE
 */
int
_bcm_field_td2_egress_selcode_set(int unit, _field_group_t *fg,
                                  uint32 efp_slice_mode, uint32 efp_key_mode,
                                  uint32 key_match_type, int *parts_count)
{
    if (!fg) {
        return (BCM_E_INTERNAL);
    }    

    switch (efp_slice_mode) {
        case _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L2:
            fg->flags |= _FP_GROUP_SPAN_SINGLE_SLICE;
            fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY4;
            *parts_count = 1;
            break;
        case _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3:
            fg->flags |= _FP_GROUP_SPAN_SINGLE_SLICE;
            if (key_match_type == ~0 /* No entries */
                    || key_match_type == _BCM_FIELD_EFP_KEY1_MATCH_TYPE
            ) {
                /* IPv4 single wide key */
                fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                *parts_count = 1;
                fg->sel_codes[0].ip6_addr_sel = _FP_SELCODE_DONT_CARE;
            } else {
                /* key_match_type == _BCM_FIELD_EFP_KEY2_MATCH_TYPE */
                /* IPv6 single wide key */
                fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY4;
                *parts_count = 1;
                fg->sel_codes[0].ip6_addr_sel = efp_key_mode;
            }
            break;
        case _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3_ANY:
            fg->flags |= _FP_GROUP_SPAN_SINGLE_SLICE;
            if ((key_match_type == _BCM_FIELD_EFP_KEY1_MATCH_TYPE)) {
                /* IPv4 single wide key */
                fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                *parts_count = 1;
                fg->sel_codes[0].ip6_addr_sel = _FP_SELCODE_DONT_CARE;
            } else {
                fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY4;
                *parts_count = 1;
            }
            break;
        case _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE:
            fg->flags |= _FP_GROUP_SPAN_DOUBLE_SLICE;
            if (key_match_type == ~0 /* No entries */
                        || key_match_type == _BCM_FIELD_EFP_KEY2_KEY3_MATCH_TYPE
                        ) {
                fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY3;
                fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY2;
            } else { /* _BCM_FIELD_EFP_KEY1_KEY4_MATCH_TYPE */
                fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;
            }
            *parts_count = 2;
            break;
        case _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY:
            fg->flags |= _FP_GROUP_SPAN_DOUBLE_SLICE;
            if ((key_match_type == _BCM_FIELD_EFP_KEY1_KEY4_MATCH_TYPE))  {
                /* IPv4 double wide key */
                fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;
            } else {
                fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1_NO_V4;
                fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;
            }
            *parts_count = 2;
            break;
        case _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6:
            fg->flags |= _FP_GROUP_SPAN_DOUBLE_SLICE;
            fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY2;
            fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;
            fg->sel_codes[0].ip6_addr_sel = efp_key_mode;
            *parts_count = 2;
            break;
        default:
            return (BCM_E_INTERNAL);
        }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_td2_group_construct_quals_add
 *
 * Purpose:
 *     Recover all qualifiers for the given group, based on its selector codes
 *
 * Parameters:
 *     unit             - (IN) BCM device number
 *     fc               - (IN) Device field control structure pointer
 *     stage_fc         - (IN) FP stage control info
 *     entry_type       - (IN) EFP entry type 
 *     fg               - (IN/OUT) Select code information filled into the group
 *
 * Returns:
 *     BCM_E_NONE
 */
int
_bcm_field_td2_group_construct_quals_add(int unit,
                                         _field_control_t *fc,
                                         _field_stage_t *stage_fc,
                                         uint8 entry_type,
                                         _field_group_t *fg,
                                         int8 pri_key1,
                                         int8 pri_key2
                                        )
{
    int                rv;
    int                parts_cnt, part_idx;
    bcm_field_qset_t   qset;
    int8               fpf3 = -1;

    if ((!fc) || (!stage_fc) || (!fg)) {
        return(BCM_E_INTERNAL);
    }


    BCM_FIELD_QSET_INIT(qset);

    if ((stage_fc->stage_id == _BCM_FIELD_STAGE_EGRESS) &&
            (_FP_ENTRY_TYPE_1 == entry_type)) {
        if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
            fpf3 = fg->sel_codes[1].fpf3;
        }
        rv = _bcm_field_trx_qset_get(unit, &(fg->qset), stage_fc,
                fg->sel_codes[0].fpf3, fpf3,
                &qset, fg);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        qset = fg->qset;
    }

    if (fc->l2warm) {
        rv = _field_trx_group_construct_quals_add(unit, fc, fg, entry_type, &qset,
                                                  pri_key1, pri_key2);
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    rv = _bcm_field_entry_tcam_parts_count(unit, fg->stage_id,
                                           fg->flags, &parts_cnt);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    for (part_idx = 0; part_idx < parts_cnt; ++part_idx) {
        _bcm_field_group_qual_t *grp_qual = &fg->qual_arr[entry_type][part_idx];
        unsigned qid;

        for (qid = 0; qid < _bcmFieldQualifyCount; ++qid) {
            _bcm_field_qual_info_t *f_qual_arr = stage_fc->f_qual_arr[qid];
            unsigned               j;
            uint8                  diff_cnt;

            if (f_qual_arr == NULL) {
                continue; /* Qualifier does not exist in this stage */
            }

            if (fc->l2warm && !BCM_FIELD_QSET_TEST(qset, qid)) {
                continue; /* Qualifier not present in the group */
            }

            /* Add all of the stage's qualifiers that match the recovered
               selector codes.  Qualifiers that appear more than once
               (because more than one configuration of a qualifier matches
               the recovered selector codes) will be cleaned up later.
            */

            for (j = 0; j < f_qual_arr->conf_sz; j++) {
                if (_field_selector_diff(unit,
                                         fg->sel_codes,
                                         part_idx,
                                         &f_qual_arr->conf_arr[j].selector,
                                         &diff_cnt
                                         )
                    != BCM_E_NONE
                    || diff_cnt != 0
                    ) {
                    continue;
                }

                if ((!fc->l2warm) && (entry_type != _FP_ENTRY_TYPE_1)) {
                    BCM_FIELD_QSET_ADD(fg->qset, qid);
                }
                _field_trx_group_qual_add(grp_qual,
                                          qid,
                                          &f_qual_arr->conf_arr[j].offset
                                          );

                _field_qset_udf_bmap_reinit(unit,
                                            stage_fc,
                                            &fg->qset,
                                            qid
                                            );
            }
        }
    }

    return (BCM_E_NONE);
}

#endif
#else /* BCM_TRIDENT2_SUPPORT && BCM_FIELD_SUPPORT */
typedef int _td2_field_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_TRIDENT2_SUPPORT && BCM_FIELD_SUPPORT */

