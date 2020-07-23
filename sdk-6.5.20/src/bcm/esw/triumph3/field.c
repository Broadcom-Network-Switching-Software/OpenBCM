/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field.c
 * Purpose:     BCM56640 Field Processor installation functions.
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#if defined(BCM_TRIUMPH3_SUPPORT) && defined(BCM_FIELD_SUPPORT)

#include <soc/drv.h>
#include <soc/scache.h>
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
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/mirror.h>

#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph3.h>

#ifndef BIT
#define BIT(n)          (1U << (n))
#endif
#define ALIGN32(x)      (((x) + 31) & ~31)


/* Device info */

#define NUM_CNTR_POOLS_IFP      16
#define NUM_CNTRS_PER_POOL_IFP  512

#define NUM_METER_POOLS_IFP      8
#define NUM_METER_PAIRS_PER_POOL_IFP  512

#define NUM_LOGICAL_METER_POOLS_IFP      16
#define NUM_LOGICAL_METER_PAIRS_PER_POOL_IFP  256

/* IP Protocol */
#define IP_PROTO_ICMP       0x1
#define IP_PROTO_IGMP       0x2
#define IP_PROTO_IPv4       0x4
#define IP_PROTO_TCP        0x6
#define IP_PROTO_UDP        0x11
#define IP_PROTO_IPv6       0x29
#define IP_PROTO_MPLS       0x89

/*
 * IFP - FP_DOUBLE_WIDE_SELECT fields.
 */
const soc_field_t _tr3_dw_f1_sel[] =  {
    SLICE_0_F1f, SLICE_1_F1f,
    SLICE_2_F1f, SLICE_3_F1f,
    SLICE_4_F1f, SLICE_5_F1f,
    SLICE_6_F1f, SLICE_7_F1f,
    SLICE_8_F1f, SLICE_9_F1f,
    SLICE_10_F1f, SLICE_11_F1f,
    SLICE_12_F1f, SLICE_13_F1f,
    SLICE_14_F1f, SLICE_15_F1f
    };

static const soc_field_t s_type_fld_tbl[] = {
    SLICE_0_S_TYPE_SELf,
    SLICE_1_S_TYPE_SELf,
    SLICE_2_S_TYPE_SELf,
    SLICE_3_S_TYPE_SELf
    };

/*
 * - Check all "structured" qualifiers
 * (S_TYPE, S_FIELD), (D_TYPE, DFIELD), etc.
 * Qualifier table definition
 * Set/get functions
 */

/* We extend the buffer used by the entry for the TCAM word to be a
   concatentation of device memory words.  For TR3, the entry's buffer looks
   like this:

   +----------
   |
   | Buffer for FP_TCAM (key/mask)
   |
   +----------
   | Padding (if any), to bring next buffer to 32-bit boundary
   +----------
   |
   | Buffer for FP_GLOBAL_MASK_TCAM (key/mask)
   |
   +----------

   It will be the responsibility of the entry hardware write and read
   functions to know which part of the logical buffer goes where.
*/

/*
 * Function:
 *     _field_tr3_ingress_qualifiers_init
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
_field_tr3_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    

    const unsigned
        fp_tcam_ofs             = 0,
        f1_ofs                  = 187 - 2,
        f2_ofs                  = 59 - 2,
        f3_ofs                  = 10 - 2,
        dw_f1_ofs               = 170 - 3 + 1,
        dw_f2_ofs               = 42 - 3 + 1,
        dw_f3_ofs               = 10 - 3 + 1,
        dw_f4_ofs               = 3 - 3 + 1,
        fp_global_mask_tcam_ofs = ALIGN32(soc_mem_field_length(unit, FP_TCAMm, KEYf)),
        dw_f0_ofs               = 1 - 1,
        fixed_ofs               = 65 - 1;

    _FP_QUAL_DECL;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /***** Special qualifiers *****/

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

    /***** FIXED single-wide *****/

    /* N.B.  Fixed field is in FP_GLOBAL_MASK_TCAM */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 1, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifySvpValid,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 4, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 4, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 5, 4
                 );
    /* (fp_global_mask_tcam_ofs + fixed_ofs + 9, 1) is reserved */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 10, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 11, 1
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyLoopbackType,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 0,
                               fp_global_mask_tcam_ofs + fixed_ofs + 12, 5
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTunnelType,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 1,
                               fp_global_mask_tcam_ofs + fixed_ofs + 12, 5
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyLoopback,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 0,
                               fp_global_mask_tcam_ofs + fixed_ofs + 16, 1
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRecoverableDrop,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 18, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 19, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 20, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRepCopy,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 21, 1
                 );

    /***** FPF1 single-wide *****/

    /* FPF1 = 0 */

    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                                 _bcmFieldSliceSelFpf1, 0,
                                 _bcmFieldSliceSrcClassSelect, 0,
                                 _bcmFieldSliceFpf1SrcDstClassSelect, 0,
                                 fp_tcam_ofs + f1_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                                 _bcmFieldSliceSelFpf1, 0,
                                 _bcmFieldSliceSrcClassSelect, 1,
                                 _bcmFieldSliceFpf1SrcDstClassSelect, 0,
                                 fp_tcam_ofs + f1_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                 _bcmFieldSliceSelFpf1, 0,
                                 _bcmFieldSliceSrcClassSelect, 2,
                                 _bcmFieldSliceFpf1SrcDstClassSelect, 0,
                                 fp_tcam_ofs + f1_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                 _bcmFieldSliceSelFpf1, 0,
                                 _bcmFieldSliceSrcClassSelect, 3,
                                 _bcmFieldSliceFpf1SrcDstClassSelect, 0,
                                 fp_tcam_ofs + f1_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                                 _bcmFieldSliceSelFpf1, 0,
                                 _bcmFieldSliceDstClassSelect, 0,
                                 _bcmFieldSliceFpf1SrcDstClassSelect, 1,
                                 fp_tcam_ofs + f1_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                                 _bcmFieldSliceSelFpf1, 0,
                                 _bcmFieldSliceDstClassSelect, 1,
                                 _bcmFieldSliceFpf1SrcDstClassSelect, 1,
                                 fp_tcam_ofs + f1_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                 _bcmFieldSliceSelFpf1, 0,
                                 _bcmFieldSliceDstClassSelect, 2,
                                 _bcmFieldSliceFpf1SrcDstClassSelect, 1,
                                 fp_tcam_ofs + f1_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                 _bcmFieldSliceSelFpf1, 0,
                                 _bcmFieldSliceDstClassSelect, 3,
                                 _bcmFieldSliceFpf1SrcDstClassSelect, 1,
                                 fp_tcam_ofs + f1_ofs, 10
                                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVrf,
                               fp_tcam_ofs + f1_ofs + 10, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVpn,
                               fp_tcam_ofs + f1_ofs + 10, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVlan,
                               fp_tcam_ofs + f1_ofs + 10, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f1_ofs + 24, 16
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f1_ofs + 24, 16
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f1_ofs + 24, 16
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityWlanGport,
                               fp_tcam_ofs + f1_ofs + 24, 16
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f1_ofs + 24, 16
                               );

    /* FPF1 = 1 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMultipath,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f1_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f1_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f1_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f1_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f1_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityWlanGport,
                               fp_tcam_ofs + f1_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                               fp_tcam_ofs + f1_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunkMemberGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                               fp_tcam_ofs + f1_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                               fp_tcam_ofs + f1_ofs + 21 + 7, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                               fp_tcam_ofs + f1_ofs + 21 + 7, 8
                               );
    
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
                 _bcmFieldSliceSelFpf1, 1,
                 fp_tcam_ofs + f1_ofs + 37, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 1,
                 fp_tcam_ofs + f1_ofs + 38, 3
                 );

    /* FPF1 = 2 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMultipath,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 2,
                 fp_tcam_ofs + f1_ofs + 21, 3
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_tcam_ofs + f1_ofs + 24, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 1,
                               fp_tcam_ofs + f1_ofs + 24, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_tcam_ofs + f1_ofs + 24, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_tcam_ofs + f1_ofs + 24, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_tcam_ofs + f1_ofs + 34, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 1,
                               fp_tcam_ofs + f1_ofs + 34, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_tcam_ofs + f1_ofs + 34, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_tcam_ofs + f1_ofs + 34, 10
                               );

    /* FPF1 = 3 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs + 16, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs + 16, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs + 16 + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs + 16 + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs + 32, 2
                 );

    /* FPF1 = 4 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_tcam_ofs + f1_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_tcam_ofs + f1_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_tcam_ofs + f1_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_tcam_ofs + f1_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_tcam_ofs + f1_ofs + 16, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_tcam_ofs + f1_ofs + 32, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_tcam_ofs + f1_ofs + 35, 3
                 );

    /* FPF1 = 5 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 1, 1
                 );
    /* Bit 2 is VXLT0/VXLT1 hit indicator; not used */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 3, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 4, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 6, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 7, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 8, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 9, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 10, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 10, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 11, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 13, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 14, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 15, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 16, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillEgressRbridgeHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 16, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 16, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 16, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillIngressRbridgeHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 20, 6
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 26, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 26, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 26 + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 26 + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 42, 2
                 );

    /* FPF1 = 6 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 16, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 16 + 2, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 20, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 22, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 25, 6
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 31, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 34, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 37, 3
                 );

    /* FPF1 = 7 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_tcam_ofs + f1_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_tcam_ofs + f1_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_tcam_ofs + f1_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_tcam_ofs + f1_ofs + 13, 3
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_tcam_ofs + f1_ofs + 16, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 1,
                               fp_tcam_ofs + f1_ofs + 16, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_tcam_ofs + f1_ofs + 16, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_tcam_ofs + f1_ofs + 16, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVrf,
                               fp_tcam_ofs + f1_ofs + 26, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVpn,
                               fp_tcam_ofs + f1_ofs + 26, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVlan,
                               fp_tcam_ofs + f1_ofs + 26, 14
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyGenericAssociatedChannelLabelValid,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_tcam_ofs + f1_ofs + 40, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRouterAlertLabelValid,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_tcam_ofs + f1_ofs + 41, 1
                 );

    /* FPF1 = 8 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_tcam_ofs + f1_ofs, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf1, 8,
                 _bcmFieldSliceTosClassSelect, 0,
                 fp_tcam_ofs + f1_ofs + 8, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                 _bcmFieldSliceSelFpf1, 8,
                 _bcmFieldSliceTosClassSelect, 0,
                 fp_tcam_ofs + f1_ofs + 8, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                 _bcmFieldSliceSelFpf1, 8,
                 _bcmFieldSliceTosClassSelect, 1,
                 fp_tcam_ofs + f1_ofs + 8, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_tcam_ofs + f1_ofs + 16, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 1,
                               fp_tcam_ofs + f1_ofs + 16, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_tcam_ofs + f1_ofs + 16, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_tcam_ofs + f1_ofs + 16, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVrf,
                               fp_tcam_ofs + f1_ofs + 26, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVpn,
                               fp_tcam_ofs + f1_ofs + 26, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVlan,
                               fp_tcam_ofs + f1_ofs + 26, 14
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIpLocal,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_tcam_ofs + f1_ofs + 40, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpAddrsNormalized,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_tcam_ofs + f1_ofs + 41, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_tcam_ofs + f1_ofs + 42, 1
                 );

    /* FPF1 = 9 */
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData2,
                 _bcmFieldSliceSelFpf1, 9,
                 fp_tcam_ofs + f1_ofs, 32
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyGenericAssociatedChannelLabelValid,
                 _bcmFieldSliceSelFpf1, 9,
                 fp_tcam_ofs + f1_ofs + 32, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRouterAlertLabelValid,
                 _bcmFieldSliceSelFpf1, 9,
                 fp_tcam_ofs + f1_ofs + 33, 1
                 );

    /*
     * FPF1=9 - UDF1_31_0
     */

    /* FPF1 = 10 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMultipath,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf1, 10,
                 fp_tcam_ofs + f1_ofs + 21, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf1, 10,
                 fp_tcam_ofs + f1_ofs + 21, 16
                 );

    /* FPF1 = 11 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs + 7, 9
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs + 16, 17
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs + 16, 17
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs + 16, 17
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs + 16, 17
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs + 32, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs + 33, 4
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs + 37, 2
                 );

    /* FPF1 = 12 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 12,
                 fp_tcam_ofs + f1_ofs, 12
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_tcam_ofs + f1_ofs + 12, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 1,
                               fp_tcam_ofs + f1_ofs + 12, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_tcam_ofs + f1_ofs + 12, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_tcam_ofs + f1_ofs + 12, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f1_ofs + 22, 16
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f1_ofs + 22, 16
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f1_ofs + 22, 16
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityWlanGport,
                               fp_tcam_ofs + f1_ofs + 22, 16
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_tcam_ofs + f1_ofs + 38, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 1,
                               fp_tcam_ofs + f1_ofs + 38, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_tcam_ofs + f1_ofs + 38, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_tcam_ofs + f1_ofs + 38, 10
                               );

    /* FPF1 = 13 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagVn,
                               fp_tcam_ofs + f1_ofs, 33
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCnTag,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagCn,
                               fp_tcam_ofs + f1_ofs, 33
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyFabricQueueTag,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagFabricQueue,
                               fp_tcam_ofs + f1_ofs, 33
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabel,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                               fp_tcam_ofs + f1_ofs, 33
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelTtl,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,
                     fp_tcam_ofs + f1_ofs,      8,
                     fp_tcam_ofs + f1_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelBos,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,
                     fp_tcam_ofs + f1_ofs + 8,  1,
                     fp_tcam_ofs + f1_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelExp,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,
                     fp_tcam_ofs + f1_ofs + 9,  3,
                     fp_tcam_ofs + f1_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelId,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                               fp_tcam_ofs + f1_ofs + 12, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWord,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsCntlWord,
                               fp_tcam_ofs + f1_ofs, 33
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashUpper,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagRtag7A,
                               fp_tcam_ofs + f1_ofs + 16, 17
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagRtag7A,
                     0,
                     fp_tcam_ofs + f1_ofs,      16,
                     fp_tcam_ofs + f1_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashUpper,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagRtag7B,
                               fp_tcam_ofs + f1_ofs + 16, 17
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagRtag7B,
                     0,
                     fp_tcam_ofs + f1_ofs,      16,
                     fp_tcam_ofs + f1_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelAction,
                 _bcmFieldSliceSelFpf1, 13,
                 fp_tcam_ofs + f1_ofs + 33, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWordValid,
                 _bcmFieldSliceSelFpf1, 13,
                 fp_tcam_ofs + f1_ofs + 36, 1
                 );

    /***** FPF2 single-wide *****/

    /* FPF2 = 0 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTtlClassSelect, 0,
                 fp_tcam_ofs + f2_ofs, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTtlClassSelect, 0,
                 fp_tcam_ofs + f2_ofs, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTtlClassSelect, 1,
                 fp_tcam_ofs + f2_ofs, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTcpClassSelect, 0,
                 fp_tcam_ofs + f2_ofs + 8, 6
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTcpClassSelect, 0,
                 fp_tcam_ofs + f2_ofs + 8, 6
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTcpClassSelect, 1,
                 fp_tcam_ofs + f2_ofs + 8, 6
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
                 _bcmFieldSliceSelFpf2, 0,
                 fp_tcam_ofs + f2_ofs + 14, 2
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTosClassSelect, 0,
                 fp_tcam_ofs + f2_ofs + 16, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTosClassSelect, 0,
                 fp_tcam_ofs + f2_ofs + 16, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                 _bcmFieldSliceSelFpf2, 0,
                 _bcmFieldSliceTosClassSelect, 1,
                 fp_tcam_ofs + f2_ofs + 16, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 0,
                 fp_tcam_ofs + f2_ofs + 24, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 0,
                 fp_tcam_ofs + f2_ofs + 40, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf2, 0,
                 fp_tcam_ofs + f2_ofs + 40, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 0,
                 fp_tcam_ofs + f2_ofs + 56, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 0,
                 fp_tcam_ofs + f2_ofs + 64, 32
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 0,
                 fp_tcam_ofs + f2_ofs + 96, 32
                 );

    /* FPF2 = 1 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTtlClassSelect, 0,
                 fp_tcam_ofs + f2_ofs, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTtlClassSelect, 0,
                 fp_tcam_ofs + f2_ofs, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTtlClassSelect, 1,
                 fp_tcam_ofs + f2_ofs, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTcpClassSelect, 0,
                 fp_tcam_ofs + f2_ofs + 8, 6
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTcpClassSelect, 0,
                 fp_tcam_ofs + f2_ofs + 8, 6
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTcpClassSelect, 1,
                 fp_tcam_ofs + f2_ofs + 8, 6
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf2, 1,
                 fp_tcam_ofs + f2_ofs + 14, 2
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTosClassSelect, 0,
                 fp_tcam_ofs + f2_ofs + 16, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTosClassSelect, 0,
                 fp_tcam_ofs + f2_ofs + 16, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                 _bcmFieldSliceSelFpf2, 1,
                 _bcmFieldSliceTosClassSelect, 1,
                 fp_tcam_ofs + f2_ofs + 16, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 1,
                 fp_tcam_ofs + f2_ofs + 24, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 1,
                 fp_tcam_ofs + f2_ofs + 40, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf2, 1,
                 fp_tcam_ofs + f2_ofs + 40, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 1,
                 fp_tcam_ofs + f2_ofs + 56, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 1,
                 fp_tcam_ofs + f2_ofs + 64, 32
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 1,
                 fp_tcam_ofs + f2_ofs + 96, 32
                 );

    /* FPF2 = 2 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelFpf2, 2,
                 fp_tcam_ofs + f2_ofs, 128
                 );

    /* FPF2 = 3 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf2, 3,
                 fp_tcam_ofs + f2_ofs, 128
                 );

    /* FPF2 = 4 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTtlClassSelect, 0,
                 fp_tcam_ofs + f2_ofs, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTtlClassSelect, 0,
                 fp_tcam_ofs + f2_ofs, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTtlClassSelect, 1,
                 fp_tcam_ofs + f2_ofs, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTcpClassSelect, 0,
                 fp_tcam_ofs + f2_ofs + 8, 6
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTcpClassSelect, 0,
                 fp_tcam_ofs + f2_ofs + 8, 6
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTcpClassSelect, 1,
                 fp_tcam_ofs + f2_ofs + 8, 6
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf2, 4,
                 fp_tcam_ofs + f2_ofs + 14, 20
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTosClassSelect, 0,
                 fp_tcam_ofs + f2_ofs + 34, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTosClassSelect, 0,
                 fp_tcam_ofs + f2_ofs + 34, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                 _bcmFieldSliceSelFpf2, 4,
                 _bcmFieldSliceTosClassSelect, 1,
                 fp_tcam_ofs + f2_ofs + 34, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 4,
                 fp_tcam_ofs + f2_ofs + 42, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 4,
                 fp_tcam_ofs + f2_ofs + 64, 64
                 );

    /* FPF2 = 5 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 5,
                 fp_tcam_ofs + f2_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 5,
                 fp_tcam_ofs + f2_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 5,
                 fp_tcam_ofs + f2_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 5,
                 fp_tcam_ofs + f2_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 5,
                 fp_tcam_ofs + f2_ofs + 16, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 5,
                 fp_tcam_ofs + f2_ofs + 32, 48
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5,
                 fp_tcam_ofs + f2_ofs + 80, 48
                 );

    /* FPF2 = 6 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 6,
                 fp_tcam_ofs + f2_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 6,
                 fp_tcam_ofs + f2_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 6,
                 fp_tcam_ofs + f2_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 6,
                 fp_tcam_ofs + f2_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 6,
                 fp_tcam_ofs + f2_ofs + 16, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 6,
                 fp_tcam_ofs + f2_ofs + 32, 48
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 6,
                 fp_tcam_ofs + f2_ofs + 80, 32
                 );

    /* FPF2 = 7 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 7,
                 fp_tcam_ofs + f2_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 7,
                 fp_tcam_ofs + f2_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 7,
                 fp_tcam_ofs + f2_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 7,
                 fp_tcam_ofs + f2_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 7,
                 fp_tcam_ofs + f2_ofs + 16, 16
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 7,
                 _bcmFieldSliceTtlClassSelect, 0,
                 fp_tcam_ofs + f2_ofs + 32, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                 _bcmFieldSliceSelFpf2, 7,
                 _bcmFieldSliceTtlClassSelect, 0,
                 fp_tcam_ofs + f2_ofs + 32, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                 _bcmFieldSliceSelFpf2, 7,
                 _bcmFieldSliceTtlClassSelect, 1,
                 fp_tcam_ofs + f2_ofs + 32, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 7,
                 fp_tcam_ofs + f2_ofs + 40, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 7,
                 fp_tcam_ofs + f2_ofs + 48, 32
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 7,
                 fp_tcam_ofs + f2_ofs + 80, 48
                 );

    /* FPF2 = 8 */

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 8,
                 fp_tcam_ofs + f2_ofs, 128
                 );

    /* FPF2 = 9 */

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                 _bcmFieldSliceSelFpf2, 9,
                 fp_tcam_ofs + f2_ofs, 128
                 );

    /* FPF2 = 10 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                 _bcmFieldSliceSelFpf2, 10,
                 fp_tcam_ofs + f2_ofs, 64
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 10,
                 fp_tcam_ofs + f2_ofs + 64, 64
                 );

    /* FPF2 = 11 */

    /*
     * DIP_127_96 omitted -- no qualifier?  Omitted in Trident, too.
     */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 11,
                 fp_tcam_ofs + f2_ofs + 32, 48
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 11,
                 fp_tcam_ofs + f2_ofs + 80, 48
                 );

    /***** FPF3 single-wide *****/

    /* FPF3 = 0 */

    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                                 _bcmFieldSliceSelFpf3, 0,
                                 _bcmFieldSliceSrcClassSelect, 0,
                                 _bcmFieldSliceFpf3SrcDstClassSelect, 0,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                                 _bcmFieldSliceSelFpf3, 0,
                                 _bcmFieldSliceSrcClassSelect, 1,
                                 _bcmFieldSliceFpf3SrcDstClassSelect, 0,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                 _bcmFieldSliceSelFpf3, 0,
                                 _bcmFieldSliceSrcClassSelect, 2,
                                 _bcmFieldSliceFpf3SrcDstClassSelect, 0,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                 _bcmFieldSliceSelFpf3, 0,
                                 _bcmFieldSliceSrcClassSelect, 3,
                                 _bcmFieldSliceFpf3SrcDstClassSelect, 0,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                                 _bcmFieldSliceSelFpf3, 0,
                                 _bcmFieldSliceDstClassSelect, 0,
                                 _bcmFieldSliceFpf3SrcDstClassSelect, 1,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                                 _bcmFieldSliceSelFpf3, 0,
                                 _bcmFieldSliceDstClassSelect, 1,
                                 _bcmFieldSliceFpf3SrcDstClassSelect, 1,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                 _bcmFieldSliceSelFpf3, 0,
                                 _bcmFieldSliceDstClassSelect, 2,
                                 _bcmFieldSliceFpf3SrcDstClassSelect, 1,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                 _bcmFieldSliceSelFpf3, 0,
                                 _bcmFieldSliceDstClassSelect, 3,
                                 _bcmFieldSliceFpf3SrcDstClassSelect, 1,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVrf,
                               fp_tcam_ofs + f3_ofs + 10, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVpn,
                               fp_tcam_ofs + f3_ofs + 10, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVlan,
                               fp_tcam_ofs + f3_ofs + 10, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f3_ofs + 24, 16
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f3_ofs + 24, 16
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f3_ofs + 24, 16
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f3_ofs + 24, 16
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityWlanGport,
                               fp_tcam_ofs + f3_ofs + 24, 16
                               );

    /* FPF3 = 1 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                                 _bcmFieldSliceSelFpf3, 1,
                                 _bcmFieldSliceSrcClassSelect, 0,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                                 _bcmFieldSliceSelFpf3, 1,
                                 _bcmFieldSliceSrcClassSelect, 1,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                 _bcmFieldSliceSelFpf3, 1,
                                 _bcmFieldSliceSrcClassSelect, 2,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                 _bcmFieldSliceSelFpf3, 1,
                                 _bcmFieldSliceSrcClassSelect, 3,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVrf,
                               fp_tcam_ofs + f3_ofs + 10, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVpn,
                               fp_tcam_ofs + f3_ofs + 10, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVlan,
                               fp_tcam_ofs + f3_ofs + 10, 14
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 1,
                 fp_tcam_ofs + f3_ofs + 24, 12
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_tcam_ofs + f3_ofs + 36, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 1,
                               fp_tcam_ofs + f3_ofs + 36, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_tcam_ofs + f3_ofs + 36, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_tcam_ofs + f3_ofs + 36, 10
                               );

    /* FPF3 = 2 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f3_ofs, 21
                               );
   _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport, 
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMultipath,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f3_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f3_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f3_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f3_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f3_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityWlanGport,
                               fp_tcam_ofs + f3_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                               fp_tcam_ofs + f3_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunkMemberGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                               fp_tcam_ofs + f3_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                               fp_tcam_ofs + f3_ofs + 21 + 7, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                               fp_tcam_ofs + f3_ofs + 21 + 7, 8
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
                 _bcmFieldSliceSelFpf3, 2,
                 fp_tcam_ofs + f3_ofs + 37, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 2,
                 fp_tcam_ofs + f3_ofs + 38, 3
                 );

    /* FPF3 = 3 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 16, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 16 + 2, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 20, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 22, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 25, 6
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 31, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 34, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 37, 3
                 );

    /* FPF3 = 4 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 16, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 16, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 16 + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 16 + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 32, 4
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 36, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIpLocal,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 38, 1
                 );

    /* FPF3 = 5 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_tcam_ofs + f3_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_tcam_ofs + f3_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_tcam_ofs + f3_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_tcam_ofs + f3_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_tcam_ofs + f3_ofs + 16, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_tcam_ofs + f3_ofs + 32, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_tcam_ofs + f3_ofs + 35, 3
                 );

    /* FPF3 = 6 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 1, 1
                 );
    /* Bit 2 is VXLT0/VXLT1 hit indicator; not used */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 3, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 4, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 6, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 7, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 8, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 9, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 10, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 10, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 11, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 13, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 14, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 15, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 16, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillEgressRbridgeHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 16, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 16, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 16, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillIngressRbridgeHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 20, 6
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 26, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 26, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 26 + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 26 + 13, 3
                 );

    /* FPF3 = 7 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf3, 7,
                 fp_tcam_ofs + f3_ofs, 24
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, _bcmFieldQualifyRangeCheckBits24_31,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 3,
                               fp_tcam_ofs + f3_ofs + 24, 8
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 0,
                               fp_tcam_ofs + f3_ofs + 24, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 1,
                               fp_tcam_ofs + f3_ofs + 24, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 1,
                               fp_tcam_ofs + f3_ofs + 24, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 2,
                               fp_tcam_ofs + f3_ofs + 24, 12
                               );

    /* FPF3 = 8 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_tcam_ofs + f3_ofs, 20
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_tcam_ofs + f3_ofs + 20, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_tcam_ofs + f3_ofs + 20 + 2, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_tcam_ofs + f3_ofs + 24, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_tcam_ofs + f3_ofs + 26, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_tcam_ofs + f3_ofs + 28, 2
                 );

    /* FPF3 = 9 */
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData3,
                 _bcmFieldSliceSelFpf3, 9,
                 fp_tcam_ofs + f3_ofs, 32
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyGenericAssociatedChannelLabelValid,
                 _bcmFieldSliceSelFpf3, 9,
                 fp_tcam_ofs + f3_ofs + 32, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRouterAlertLabelValid,
                 _bcmFieldSliceSelFpf3, 9,
                 fp_tcam_ofs + f3_ofs + 33, 1
                 );

    /*
     * FPF3=9 - UDF1_95_64
     */

    /* FPF3 = 10 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMultipath,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf3, 10,
                 fp_tcam_ofs + f3_ofs + 21, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf3, 10,
                 fp_tcam_ofs + f3_ofs + 21, 16
                 );

    /* FPF3 = 11 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs + 7, 9
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs + 16, 17
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs + 16, 17
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs + 16, 17
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs + 16, 17
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs + 32, 1
                 );

    /* FPF3 = 12 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagVn,
                               fp_tcam_ofs + f3_ofs, 33
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCnTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagCn,
                               fp_tcam_ofs + f3_ofs, 33
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyFabricQueueTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagFabricQueue,
                               fp_tcam_ofs + f3_ofs, 33
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabel,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                               fp_tcam_ofs + f3_ofs, 33
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelTtl,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,
                     fp_tcam_ofs + f3_ofs,      8,
                     fp_tcam_ofs + f3_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelBos,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,
                     fp_tcam_ofs + f3_ofs + 8,  1,
                     fp_tcam_ofs + f3_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelExp,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,
                     fp_tcam_ofs + f3_ofs + 9,  3,
                     fp_tcam_ofs + f3_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelId,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                               fp_tcam_ofs + f3_ofs + 12, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWord,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsCntlWord,
                               fp_tcam_ofs + f3_ofs, 33
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashUpper,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagRtag7A,
                               fp_tcam_ofs + f3_ofs + 16, 17
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagRtag7A,
                     0,
                     fp_tcam_ofs + f3_ofs,      16,
                     fp_tcam_ofs + f3_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashUpper,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagRtag7B,
                               fp_tcam_ofs + f3_ofs + 16, 17
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagRtag7B,
                     0,
                     fp_tcam_ofs + f3_ofs,      16,
                     fp_tcam_ofs + f3_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelAction,
                 _bcmFieldSliceSelFpf3, 12,
                 fp_tcam_ofs + f3_ofs + 33, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWordValid,
                 _bcmFieldSliceSelFpf3, 12,
                 fp_tcam_ofs + f3_ofs + 36, 1
                 );

    /* FPF3 = 13 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, 13,
                 _bcmFieldSliceTosClassSelect, 0,
                 fp_tcam_ofs + f3_ofs, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                 _bcmFieldSliceSelFpf3, 13,
                 _bcmFieldSliceTosClassSelect, 0,
                 fp_tcam_ofs + f3_ofs, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                 _bcmFieldSliceSelFpf3, 13,
                 _bcmFieldSliceTosClassSelect, 1,
                 fp_tcam_ofs + f3_ofs, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeader2Type,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_tcam_ofs + f3_ofs + 8, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_tcam_ofs + f3_ofs + 16, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_tcam_ofs + f3_ofs + 24, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIpLocal,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_tcam_ofs + f3_ofs + 32, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpAddrsNormalized,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_tcam_ofs + f3_ofs + 33, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_tcam_ofs + f3_ofs + 34, 1
                 );

    /***** FPF0 double-wide *****/

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs, 16
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs, 16
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs + 16, 17
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs + 16, 17
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs + 16, 17
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs + 16, 17
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs + 32, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyColor,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs + 33, 2
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs + 35, 2
                            );

    /***** FPF1 double-wide *****/

    /* FPF1 = 0 */

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceTtlClassSelect, 0,
                            fp_tcam_ofs + dw_f1_ofs, 8
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceTtlClassSelect, 0,
                            fp_tcam_ofs + dw_f1_ofs, 8
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceTtlClassSelect, 1,
                            fp_tcam_ofs + dw_f1_ofs, 8
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs + 8, 16
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs + 24, 16
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs + 24, 16
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMultipath,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs + 61, 4
                            );

    /* FPF1 = 1 */

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelId,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs, 20
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelAction,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs + 20, 3
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWord,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs + 23, 32
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWordValid,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs + 55, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs + 56, 4
                            );

    /***** FPF2 double-wide *****/

    /* FPF2 = 0 */

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 1, 1
                            );
    /* Bit 2 is VXLT0/VXLT1 hit indicator; not used */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 3, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 4, 2
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 6, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 7, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 8, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 9, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 10, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 10, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 11, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 12, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 13, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 14, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 15, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 16, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTrillEgressRbridgeHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 16, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 16, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 16, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 17, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTrillIngressRbridgeHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 17, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 17, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 17, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 22, 6
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 28, 24
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyRangeCheckBits24_31,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 3,
                            fp_tcam_ofs + dw_f2_ofs + 52, 8
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 0,
                            fp_tcam_ofs + dw_f2_ofs + 52, 12
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassVPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 1,
                            fp_tcam_ofs + dw_f2_ofs + 52, 12
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 1,
                            fp_tcam_ofs + dw_f2_ofs + 52, 12
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 2,
                            fp_tcam_ofs + dw_f2_ofs + 52, 12
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 64, 32
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 96, 32
                            );

    /* FPF2 = 1 */

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs, 128
                            );

    /* FPF2 = 2 */

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                            _bcmFieldSliceSelFpf2, 2,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs, 128
                            );

    /* FPF2 = 3 */

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                            _bcmFieldSliceSelFpf2, 3,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs, 128
                            );

    /***** FPF3 double-wide  *****/

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceTosClassSelect, 0,
                            fp_tcam_ofs + dw_f3_ofs, 8
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceTosClassSelect, 0,
                            fp_tcam_ofs + dw_f3_ofs, 8
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceTosClassSelect, 1,
                            fp_tcam_ofs + dw_f3_ofs, 8
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f3_ofs + 8, 8
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f3_ofs + 16, 8
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f3_ofs + 24, 8
                            );

    /***** FPF4 double-wide *****/

    /* FPF4 = 0 */

    

    /* FPF4 = 1 */

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf4, 1,
                            _bcmFieldSliceTcpClassSelect, 0,
                            fp_tcam_ofs + dw_f4_ofs, 6
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                            _bcmFieldSliceSelFpf4, 1,
                            _bcmFieldSliceTcpClassSelect, 0,
                            fp_tcam_ofs + dw_f4_ofs, 6
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                            _bcmFieldSliceSelFpf4, 1,
                            _bcmFieldSliceTcpClassSelect, 1,
                            fp_tcam_ofs + dw_f4_ofs, 6
                            );
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr3_lookup_qualifiers_init
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
_field_tr3_lookup_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    const unsigned f1_ofs = 164, f2_ofs = 36, f3_ofs = 0;

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
                 _bcmFieldSliceSelFpf3, 0, f3_ofs, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 0, f3_ofs, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 0, f3_ofs + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 0, f3_ofs + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 0, f3_ofs + 16, 16);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_ofs, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_ofs, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_ofs + 16, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_ofs + 16, 16);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_ofs + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_ofs + 8, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, 2, f3_ofs + 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, 2, f3_ofs + 24, 8);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs + 12, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs + 12, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs + 24, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs + 25, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs + 28, 8);

    

    /* FPF2 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTtl,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0, f2_ofs + 8, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpFrag,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 14, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 14, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 16, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 16, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 24, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 24, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 40, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 40, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 56, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 56, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 64, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 64, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 96, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 96, 32);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp6,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs, 128);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp6,
                               _bcmFieldSliceSelFpf2, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                               _bcmFieldSliceSelFpf2, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 3, f2_ofs + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 3, f2_ofs + 32, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 3, f2_ofs + 80, 48);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 16, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 16, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 48, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 48, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 4, f2_ofs + 80, 48);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 16, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 16, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 48, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 48, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5, f2_ofs + 80, 48);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 64, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 64, 64);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                 _bcmFieldSliceSelFpf2, 7, f2_ofs, 33);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySnap,
                 _bcmFieldSliceSelFpf2, 7, f2_ofs + 64, 40);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLlc,
                 _bcmFieldSliceSelFpf2, 7, f2_ofs + 104, 24);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 8, f2_ofs, 128);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                 _bcmFieldSliceSelFpf2, 9, f2_ofs, 128);

    /* FPF1 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocolCommon,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f1_ofs, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocolCommon,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f1_ofs, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelFpf1, 0, f1_ofs + 3, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf1, 0, f1_ofs + 4, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf1, 0, f1_ofs + 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf1, 0, f1_ofs + 7, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 0, f1_ofs + 9, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf1, 0, f1_ofs + 11, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf1, 0, f1_ofs + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 0, f1_ofs + 13, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 0, f1_ofs + 13, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 0, f1_ofs + 13 + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 0, f1_ofs + 13 + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 0, f1_ofs + 29, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f1_ofs + 31, 4);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpType,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f1_ofs + 31, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf1, 0, f1_ofs + 35, 1);

    

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                     0,
                     f1_ofs + 36, 7, /* Port value in SGLP */
                     f1_ofs + 43, 8, /* Module value in SGLP */
                     f1_ofs + 51, 1, /* Trunk bit in SGLP (should be 0) */
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                     0,
                     f1_ofs + 36, 15, /* trunk id field of SGLP */
                     f1_ofs + 51, 1,  /* trunk bit of SGLP */
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMplsGport,
                     0,
                     f1_ofs + 36, 20, /* SVP_VALID + S_TYPE_SEL + S_FIELD */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityVlanGport,
                     0,
                     f1_ofs + 36, 20, /* SVP_VALID + S_TYPE_SEL + S_FIELD */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMimGport,
                     0,
                     f1_ofs + 36, 20, /* SVP_VALID + S_TYPE_SEL + S_FIELD */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityWlanGport,
                     0,
                     f1_ofs + 36, 20, /* SVP_VALID + S_TYPE_SEL + S_FIELD */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                     0,
                     f1_ofs + 36, 15, /* mod + port field of unresolved SGLP */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                     0,
                     f1_ofs + 36 + 7, 8, /* mod field of unresolved SGLP */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                     0,
                     f1_ofs + 36 + 7, 8, /* mod field of unresolved SGLP */
                     0, 0,
                     0, 0,
                     0
                     );


    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityPortGroupNum,
                     0,
                     f1_ofs + 36, 7, /* ingress port field */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityPortGroupNum,
                     0,
                     f1_ofs + 36 + 7, 8, /* ingress port group field */
                     0, 0,
                     0, 0,
                     0
                     );

    /* DWF3 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f3_ofs, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_ofs, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTtl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f3_ofs + 8, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_ofs + 8, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_ofs + 16, 6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpFrag,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f3_ofs + 22, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_ofs + 22, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f3_ofs + 24, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_ofs + 24, 8);

    /* DWF2 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTtl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_ofs, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_ofs, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 8, 6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpFrag,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 14, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 14, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 16, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 16, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 24, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 24, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 40, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 40, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 56, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 56, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 64, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 64, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 96, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 96, 32);

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_ofs, 128);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_ofs, 128);

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                            _bcmFieldSliceSelFpf2, 2,
                            _bcmFieldSliceSelDisable, 0,
                            f2_ofs, 128);

    /* DWF1 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyExtensionHeaderSubCode,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_ofs, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_ofs + 8, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_ofs + 16, 12);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_ofs + 28, 16);
    

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_tr3_egress_qualifiers_init
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
_field_tr3_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_DECL;
    _key_fld_ = KEYf;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 0, 1);

    /* EG_NEXT_HOP_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 1,
                               2, 12);
    /* EG_L3_INTF_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 2,
                               2, 12);
    /* EG_DVP_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);
    /* DVP */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
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
    /* CPUCOS */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 2,
                               193, 6);
    /* L3_PKT_TYPE */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrMdlKey4, 0,
                               206, 4);
    /* MDL */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOamMdl,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrMdlKey4, 1,
                               206, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 210, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                 211, 14
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                 211, 14
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                 211, 14
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                 225, 2
                 );

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageEgress,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

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
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 192, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 196, 1);

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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 1,
                               197, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 2,
                               197, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 3,
                               197, 12);
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
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                 210, 14
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                 210, 14
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                 210, 14
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                 224, 2
                 );
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
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 192, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 196, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 1,
                               197, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 2,
                               197, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 3,
                               197, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               197, 13
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                 210, 14
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                 210, 14
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                 210, 14
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                 224, 2
                 );

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
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               195, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 1,
                               195, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 2,
                               195, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 3,
                               195, 7);
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
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 208, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 212, 1);
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
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 47, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 55, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 183, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 191, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 199, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 203, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 0,
                               204, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 1,
                               204, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 2,
                               204, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 0);

    return (BCM_E_NONE);
}


STATIC int
_field_tr3_external_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_DECL;

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStage,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageExternal,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

    /* _FP_EXT_ACL_144_L2 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 24, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 72, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 72, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 84, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 85, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 88, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 136, 16);

    /* _FP_EXT_ACL_L2 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 0, 1); 
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 1, 2); 
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 3, 1); 
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 5, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 7, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 9, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 10, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 14, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 24, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 36, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 37, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 56, 10);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 56, 10);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 56, 10);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 66, 10);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 66, 10);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 66, 10);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 76, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 76, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 76, 12);

    
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                     _bcmFieldSliceSelDisable, 0,
                     0,
                     88, 7, /* Port value in SGLP */
                     95, 8, /* Module value in SGLP */
                     103, 1, /* Trunk bit in SGLP */
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                     _bcmFieldSliceSelDisable, 0,
                     0,
                     88, 15, /* trunk id field of SGLP */
                     103, 1, /* trunk bit of SGLP (should be 1) */
                     0, 0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                     _bcmFieldSliceSelDisable, 0,
                     0,
                     88, 16, /* SVP */
                     4, 1,   /* SVP valid (should be 1) */
                     0, 0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                     _bcmFieldSliceSelDisable, 0,
                     0,
                     88, 16, /* SVP */
                     4, 1,   /* SVP valid (should be 1) */
                     0, 0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                     _bcmFieldSliceSelDisable, 0,
                     0,
                     88, 16, /* SVP */
                     4, 1,   /* SVP valid (should be 1) */
                     0, 0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                     _bcmFieldSliceSelDisable, 0,
                     0,
                     88, 16, /* SVP */
                     4, 1,   /* SVP valid (should be 1) */
                     0, 0,
                     0
                     );

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 104, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 152, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 152, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 164, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 165, 3);

    /* Emulate the behavior of the ext_tcam_mode SOC propert */
    if (soc_property_get(unit, spn_EXT_TCAM_MODE, 0) == 1) {
        /* Was non-7K in TR2 */

        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 40, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 232, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 248, 48);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 296, 16);
    } else {
        /* Was 7K in TR2 */

        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 40, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 232, 48);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 280, 16);
    }

    /* _FP_EXT_ACL_144_IPV4 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 0, 1); 
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 1, 2); 
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 3, 1); 
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 4, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 8, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 22, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 24, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 32, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 56, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 72, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 104, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 136, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 136, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 148, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 149, 3);

    /* _FP_EXT_ACL_IPV4 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 0, 1); 
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 1, 2); 
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 3, 1); 
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 4, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 8, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 9, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 10, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 11, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 15, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 24, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 32, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 46, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 48, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 64, 10);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 64, 10);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 64, 10);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 74, 10);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 74, 10);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 74, 10);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 84, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 84, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 84, 12);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 96, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 112, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 128, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 160, 32);

    
    if (soc_property_get(unit, spn_EXT_TCAM_MODE, 0) == 0) {
        /* Was non-7K in TR2 */
        ;
    } else {
        /* Was 7K in TR2 */
        ;
    }

    /* _FP_EXT_ACL_L2_IPV4 */
    if (soc_property_get(unit, spn_EXT_L2IP4_ACL_TABLE_MASTER_KEY_TYPE, 0) == 1) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 0, 0);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 0, 1); 
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 1, 2); 
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 3, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 4, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 5, 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 7, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 8, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 9, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 10, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 11, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 11, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 12, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 13, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 14, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 15, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 16, 13);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 30, 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 32, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 40, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 48, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 48, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 64, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 64, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 80, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 96, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 112, 32);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 144, 32);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 176, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 192, 48);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 240, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 240, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 252, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 253, 3);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 256, 48);
    } else {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 0, 0);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 0, 1); 
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 1, 2); 
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 3, 1); 
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 4, 4);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 8, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 9, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 10, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 11, 4);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 15, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 16, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 24, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 32, 13);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 46, 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 48, 16);

        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 64, 10);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 64, 10);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 64, 10);

        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 74, 10);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 74, 10);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 74, 10);

        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 84, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 84, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 84, 12);

        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 96, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 112, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 128, 32);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 160, 32);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 192, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 192, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 192, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 208, 48);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 256, 48);
    }
    /* _FP_EXT_ACL_144_IPV6 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 0, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 16, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 80, 64);

    /* _FP_EXT_ACL_IPV6_SHORT */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 0, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 128, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 129, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 131, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 132, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 136, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 137, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 141, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 144, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 158, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 160, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6TrafficClass,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 168, 8);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 176, 10);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 176, 10);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 176, 10);

#if 0 /* For V6 SHORT, F5 is set to F5_CLASSID_SRC so interface class
       * qualifiers are not applicable */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 186, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 186, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 186, 6);
#endif

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 192, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 208, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 224, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 240, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 240, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 252, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 253, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 256, 128);

    /* _FP_EXT_ACL_IPV6_FULL */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 0, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 128, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 129, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 131, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 132, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 133, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 134, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 135, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 139, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 144, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 158, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 160, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6TrafficClass,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 168, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 176, 8);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 192, 10);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 192, 10);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 192, 10);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 202, 10);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 202, 10);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 202, 10);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 212, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 212, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 212, 12);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 224, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 240, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 256, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 256, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 268, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 269, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 272, 128);

    /* _FP_EXT_ACL_L2_IPV6 */
    if (soc_property_get(unit, spn_EXT_L2IP6_ACL_TABLE_MASTER_KEY_TYPE, 0) == 1) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 0, 0);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 0, 128);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 128, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 129, 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 131, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 132, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 133, 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 135, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 136, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 137, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 138, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 139, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 139, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 140, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 141, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 142, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 143, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 150, 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 152, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 152, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 164, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 165, 3);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6TrafficClass,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 168, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6NextHeader,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 176, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 184, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 200, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 216, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 216, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 232, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 232, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 249, 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 284, 20);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 304, 128);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 432, 48);
    } else {    
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 0, 0);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 0, 128);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 128, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 129, 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 131, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 132, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 133, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 136, 13);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 149, 3);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 152, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 152, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 164, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 165, 3);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6TrafficClass,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 168, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6NextHeader,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 176, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 184, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 200, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 216, 128);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 344, 48);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 392, 48);
    }
    return (BCM_E_NONE);
}

#ifdef BCM_HELIX4_SUPPORT
/*
 * Function:
 *     _field_hx4_ingress_qualifiers_init
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
_field_hx4_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    

    const unsigned
        fp_tcam_ofs             = 0,
        f1_ofs                  = 187 - 2,
        f2_ofs                  = 59 - 2,
        f3_ofs                  = 10 - 2,
        dw_f1_ofs               = 170 - 3 + 1,
        dw_f2_ofs               = 42 - 3 + 1,
        dw_f3_ofs               = 10 - 3 + 1,
        dw_f4_ofs               = 3 - 3 + 1,
        fp_global_mask_tcam_ofs = ALIGN32(soc_mem_field_length(unit, FP_TCAMm, KEYf)),
        dw_f0_ofs               = 1 - 1,
        fixed_ofs               = 65 - 1;

    _FP_QUAL_DECL;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /***** Special qualifiers *****/

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

    /***** FIXED single-wide *****/

    /* N.B.  Fixed field is in FP_GLOBAL_MASK_TCAM */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 1, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifySvpValid,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 4, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 4, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 5, 4
                 );
    /* (fp_global_mask_tcam_ofs + fixed_ofs + 9, 1) is reserved */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 10, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 11, 1
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyLoopbackType,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 0,
                               fp_global_mask_tcam_ofs + fixed_ofs + 12, 5
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTunnelType,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 1,
                               fp_global_mask_tcam_ofs + fixed_ofs + 12, 5
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyLoopback,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 0,
                               fp_global_mask_tcam_ofs + fixed_ofs + 16, 1
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRecoverableDrop,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 18, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 19, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 20, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRepCopy,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 21, 1
                 );

    /***** FPF1 single-wide *****/

    /* FPF1 = 0 */

    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                                 _bcmFieldSliceSelFpf1, 0,
                                 _bcmFieldSliceSrcClassSelect, 0,
                                 _bcmFieldSliceFpf1SrcDstClassSelect, 0,
                                 fp_tcam_ofs + f1_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                                 _bcmFieldSliceSelFpf1, 0,
                                 _bcmFieldSliceSrcClassSelect, 1,
                                 _bcmFieldSliceFpf1SrcDstClassSelect, 0,
                                 fp_tcam_ofs + f1_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                 _bcmFieldSliceSelFpf1, 0,
                                 _bcmFieldSliceSrcClassSelect, 2,
                                 _bcmFieldSliceFpf1SrcDstClassSelect, 0,
                                 fp_tcam_ofs + f1_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                 _bcmFieldSliceSelFpf1, 0,
                                 _bcmFieldSliceSrcClassSelect, 3,
                                 _bcmFieldSliceFpf1SrcDstClassSelect, 0,
                                 fp_tcam_ofs + f1_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                                 _bcmFieldSliceSelFpf1, 0,
                                 _bcmFieldSliceDstClassSelect, 0,
                                 _bcmFieldSliceFpf1SrcDstClassSelect, 1,
                                 fp_tcam_ofs + f1_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                                 _bcmFieldSliceSelFpf1, 0,
                                 _bcmFieldSliceDstClassSelect, 1,
                                 _bcmFieldSliceFpf1SrcDstClassSelect, 1,
                                 fp_tcam_ofs + f1_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                 _bcmFieldSliceSelFpf1, 0,
                                 _bcmFieldSliceDstClassSelect, 2,
                                 _bcmFieldSliceFpf1SrcDstClassSelect, 1,
                                 fp_tcam_ofs + f1_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                 _bcmFieldSliceSelFpf1, 0,
                                 _bcmFieldSliceDstClassSelect, 3,
                                 _bcmFieldSliceFpf1SrcDstClassSelect, 1,
                                 fp_tcam_ofs + f1_ofs, 10
                                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVrf,
                               fp_tcam_ofs + f1_ofs + 10, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVpn,
                               fp_tcam_ofs + f1_ofs + 10, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVlan,
                               fp_tcam_ofs + f1_ofs + 10, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f1_ofs + 24, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f1_ofs + 24, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f1_ofs + 24, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f1_ofs + 24, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityWlanGport,
                               fp_tcam_ofs + f1_ofs + 24, 13
                               );

    /* FPF1 = 1 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMultipath,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f1_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f1_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f1_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f1_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f1_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityWlanGport,
                               fp_tcam_ofs + f1_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                               fp_tcam_ofs + f1_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunkMemberGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                               fp_tcam_ofs + f1_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                               fp_tcam_ofs + f1_ofs + 21 + 7, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                               fp_tcam_ofs + f1_ofs + 21 + 7, 8
                               );
    
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
                 _bcmFieldSliceSelFpf1, 1,
                 fp_tcam_ofs + f1_ofs + 37, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 1,
                 fp_tcam_ofs + f1_ofs + 38, 3
                 );

    /* FPF1 = 2 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMultipath,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 2,
                 fp_tcam_ofs + f1_ofs + 21, 3
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_tcam_ofs + f1_ofs + 24, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 1,
                               fp_tcam_ofs + f1_ofs + 24, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_tcam_ofs + f1_ofs + 24, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_tcam_ofs + f1_ofs + 24, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_tcam_ofs + f1_ofs + 34, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 1,
                               fp_tcam_ofs + f1_ofs + 34, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_tcam_ofs + f1_ofs + 34, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_tcam_ofs + f1_ofs + 34, 10
                               );

    /* FPF1 = 3 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs + 16, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs + 16, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs + 16 + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs + 16 + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_tcam_ofs + f1_ofs + 32, 2
                 );

    /* FPF1 = 4 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_tcam_ofs + f1_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_tcam_ofs + f1_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_tcam_ofs + f1_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_tcam_ofs + f1_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_tcam_ofs + f1_ofs + 16, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_tcam_ofs + f1_ofs + 32, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_tcam_ofs + f1_ofs + 35, 3
                 );

    /* FPF1 = 5 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 1, 1
                 );
    /* Bit 2 is VXLT0/VXLT1 hit indicator; not used */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 3, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 4, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 6, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 7, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 8, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 9, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 10, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 10, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 11, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 13, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 14, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 15, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 16, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillEgressRbridgeHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 16, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 16, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 16, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillIngressRbridgeHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 20, 6
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 26, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 26, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 26 + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 26 + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_tcam_ofs + f1_ofs + 42, 2
                 );

    /* FPF1 = 6 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 16, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 16 + 2, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 20, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 22, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 25, 6
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 31, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 34, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_tcam_ofs + f1_ofs + 37, 3
                 );

    /* FPF1 = 7 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_tcam_ofs + f1_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_tcam_ofs + f1_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_tcam_ofs + f1_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_tcam_ofs + f1_ofs + 13, 3
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_tcam_ofs + f1_ofs + 16, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 1,
                               fp_tcam_ofs + f1_ofs + 16, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_tcam_ofs + f1_ofs + 16, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_tcam_ofs + f1_ofs + 16, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVrf,
                               fp_tcam_ofs + f1_ofs + 26, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVpn,
                               fp_tcam_ofs + f1_ofs + 26, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVlan,
                               fp_tcam_ofs + f1_ofs + 26, 14
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyGenericAssociatedChannelLabelValid,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_tcam_ofs + f1_ofs + 40, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRouterAlertLabelValid,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_tcam_ofs + f1_ofs + 41, 1
                 );

    /* FPF1 = 8 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_tcam_ofs + f1_ofs, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_tcam_ofs + f1_ofs + 8, 8
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_tcam_ofs + f1_ofs + 16, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 1,
                               fp_tcam_ofs + f1_ofs + 16, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_tcam_ofs + f1_ofs + 16, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_tcam_ofs + f1_ofs + 16, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVrf,
                               fp_tcam_ofs + f1_ofs + 26, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVpn,
                               fp_tcam_ofs + f1_ofs + 26, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVlan,
                               fp_tcam_ofs + f1_ofs + 26, 14
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIpLocal,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_tcam_ofs + f1_ofs + 40, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpAddrsNormalized,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_tcam_ofs + f1_ofs + 41, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_tcam_ofs + f1_ofs + 42, 1
                 );

    /* FPF1 = 9 */
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData2,
                 _bcmFieldSliceSelFpf1, 9,
                 fp_tcam_ofs + f1_ofs, 32
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyGenericAssociatedChannelLabelValid,
                 _bcmFieldSliceSelFpf1, 9,
                 fp_tcam_ofs + f1_ofs + 32, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRouterAlertLabelValid,
                 _bcmFieldSliceSelFpf1, 9,
                 fp_tcam_ofs + f1_ofs + 33, 1
                 );

    /*
     * FPF1=9 - UDF1_31_0
     */

    /* FPF1 = 10 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMultipath,
                               fp_tcam_ofs + f1_ofs, 21
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf1, 10,
                 fp_tcam_ofs + f1_ofs + 21, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf1, 10,
                 fp_tcam_ofs + f1_ofs + 21, 16
                 );

    /* FPF1 = 11 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs + 7, 9
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs + 16, 14
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs + 16, 14
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs + 16, 14
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs + 16, 14
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs + 29, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs + 30, 4
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_tcam_ofs + f1_ofs + 34, 2
                 );

    /* FPF1 = 12 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 12,
                 fp_tcam_ofs + f1_ofs, 12
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_tcam_ofs + f1_ofs + 12, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 1,
                               fp_tcam_ofs + f1_ofs + 12, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_tcam_ofs + f1_ofs + 12, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_tcam_ofs + f1_ofs + 12, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f1_ofs + 22, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f1_ofs + 22, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f1_ofs + 22, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityWlanGport,
                               fp_tcam_ofs + f1_ofs + 22, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_tcam_ofs + f1_ofs + 35, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 1,
                               fp_tcam_ofs + f1_ofs + 35, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_tcam_ofs + f1_ofs + 35, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_tcam_ofs + f1_ofs + 35, 10
                               );

    /* FPF1 = 13 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagVn,
                               fp_tcam_ofs + f1_ofs, 33
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCnTag,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagCn,
                               fp_tcam_ofs + f1_ofs, 33
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyFabricQueueTag,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagFabricQueue,
                               fp_tcam_ofs + f1_ofs, 33
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabel,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                               fp_tcam_ofs + f1_ofs, 33
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelTtl,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,
                     fp_tcam_ofs + f1_ofs,      8,
                     fp_tcam_ofs + f1_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelBos,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,
                     fp_tcam_ofs + f1_ofs + 8,  1,
                     fp_tcam_ofs + f1_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelExp,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,
                     fp_tcam_ofs + f1_ofs + 9,  3,
                     fp_tcam_ofs + f1_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelId,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                               fp_tcam_ofs + f1_ofs + 12, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWord,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsCntlWord,
                               fp_tcam_ofs + f1_ofs, 33
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashUpper,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagRtag7A,
                               fp_tcam_ofs + f1_ofs + 16, 17
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagRtag7A,
                     0,
                     fp_tcam_ofs + f1_ofs,      16,
                     fp_tcam_ofs + f1_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashUpper,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagRtag7B,
                               fp_tcam_ofs + f1_ofs + 16, 17
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagRtag7B,
                     0,
                     fp_tcam_ofs + f1_ofs,      16,
                     fp_tcam_ofs + f1_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelAction,
                 _bcmFieldSliceSelFpf1, 13,
                 fp_tcam_ofs + f1_ofs + 33, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWordValid,
                 _bcmFieldSliceSelFpf1, 13,
                 fp_tcam_ofs + f1_ofs + 36, 1
                 );

    /***** FPF2 single-wide *****/

    /* FPF2 = 0 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 0,
                 fp_tcam_ofs + f2_ofs, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0,
                 fp_tcam_ofs + f2_ofs + 8, 6
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
                 _bcmFieldSliceSelFpf2, 0,
                 fp_tcam_ofs + f2_ofs + 14, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 0,
                 fp_tcam_ofs + f2_ofs + 16, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 0,
                 fp_tcam_ofs + f2_ofs + 24, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 0,
                 fp_tcam_ofs + f2_ofs + 40, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf2, 0,
                 fp_tcam_ofs + f2_ofs + 40, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 0,
                 fp_tcam_ofs + f2_ofs + 56, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 0,
                 fp_tcam_ofs + f2_ofs + 64, 32
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 0,
                 fp_tcam_ofs + f2_ofs + 96, 32
                 );

    /* FPF2 = 1 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 1,
                 fp_tcam_ofs + f2_ofs, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 1,
                 fp_tcam_ofs + f2_ofs + 8, 6
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf2, 1,
                 fp_tcam_ofs + f2_ofs + 14, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 1,
                 fp_tcam_ofs + f2_ofs + 16, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 1,
                 fp_tcam_ofs + f2_ofs + 24, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 1,
                 fp_tcam_ofs + f2_ofs + 40, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf2, 1,
                 fp_tcam_ofs + f2_ofs + 40, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 1,
                 fp_tcam_ofs + f2_ofs + 56, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 1,
                 fp_tcam_ofs + f2_ofs + 64, 32
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 1,
                 fp_tcam_ofs + f2_ofs + 96, 32
                 );

    /* FPF2 = 2 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelFpf2, 2,
                 fp_tcam_ofs + f2_ofs, 128
                 );

    /* FPF2 = 3 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf2, 3,
                 fp_tcam_ofs + f2_ofs, 128
                 );

    /* FPF2 = 4 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 4,
                 fp_tcam_ofs + f2_ofs, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 4,
                 fp_tcam_ofs + f2_ofs + 8, 6
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf2, 4,
                 fp_tcam_ofs + f2_ofs + 14, 20
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 4,
                 fp_tcam_ofs + f2_ofs + 34, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 4,
                 fp_tcam_ofs + f2_ofs + 42, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 4,
                 fp_tcam_ofs + f2_ofs + 64, 64
                 );

    /* FPF2 = 5 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 5,
                 fp_tcam_ofs + f2_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 5,
                 fp_tcam_ofs + f2_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 5,
                 fp_tcam_ofs + f2_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 5,
                 fp_tcam_ofs + f2_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 5,
                 fp_tcam_ofs + f2_ofs + 16, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 5,
                 fp_tcam_ofs + f2_ofs + 32, 48
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5,
                 fp_tcam_ofs + f2_ofs + 80, 48
                 );

    /* FPF2 = 6 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 6,
                 fp_tcam_ofs + f2_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 6,
                 fp_tcam_ofs + f2_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 6,
                 fp_tcam_ofs + f2_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 6,
                 fp_tcam_ofs + f2_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 6,
                 fp_tcam_ofs + f2_ofs + 16, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 6,
                 fp_tcam_ofs + f2_ofs + 32, 48
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 6,
                 fp_tcam_ofs + f2_ofs + 80, 32
                 );

    /* FPF2 = 7 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 7,
                 fp_tcam_ofs + f2_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 7,
                 fp_tcam_ofs + f2_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 7,
                 fp_tcam_ofs + f2_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 7,
                 fp_tcam_ofs + f2_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 7,
                 fp_tcam_ofs + f2_ofs + 16, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 7,
                 fp_tcam_ofs + f2_ofs + 32, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 7,
                 fp_tcam_ofs + f2_ofs + 40, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 7,
                 fp_tcam_ofs + f2_ofs + 48, 32
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 7,
                 fp_tcam_ofs + f2_ofs + 80, 48
                 );

    /* FPF2 = 8 */

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 8,
                 fp_tcam_ofs + f2_ofs, 128
                 );

    /* FPF2 = 9 */

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                 _bcmFieldSliceSelFpf2, 9,
                 fp_tcam_ofs + f2_ofs, 128
                 );

    /* FPF2 = 10 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                 _bcmFieldSliceSelFpf2, 10,
                 fp_tcam_ofs + f2_ofs, 64
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 10,
                 fp_tcam_ofs + f2_ofs + 64, 64
                 );

    /* FPF2 = 11 */

    /*
     * DIP_127_96 omitted -- no qualifier?  Omitted in Trident, too.
     */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 11,
                 fp_tcam_ofs + f2_ofs + 32, 48
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 11,
                 fp_tcam_ofs + f2_ofs + 80, 48
                 );

    /***** FPF3 single-wide *****/

    /* FPF3 = 0 */

    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                                 _bcmFieldSliceSelFpf3, 0,
                                 _bcmFieldSliceSrcClassSelect, 0,
                                 _bcmFieldSliceFpf3SrcDstClassSelect, 0,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                                 _bcmFieldSliceSelFpf3, 0,
                                 _bcmFieldSliceSrcClassSelect, 1,
                                 _bcmFieldSliceFpf3SrcDstClassSelect, 0,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                 _bcmFieldSliceSelFpf3, 0,
                                 _bcmFieldSliceSrcClassSelect, 2,
                                 _bcmFieldSliceFpf3SrcDstClassSelect, 0,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                 _bcmFieldSliceSelFpf3, 0,
                                 _bcmFieldSliceSrcClassSelect, 3,
                                 _bcmFieldSliceFpf3SrcDstClassSelect, 0,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                                 _bcmFieldSliceSelFpf3, 0,
                                 _bcmFieldSliceDstClassSelect, 0,
                                 _bcmFieldSliceFpf3SrcDstClassSelect, 1,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                                 _bcmFieldSliceSelFpf3, 0,
                                 _bcmFieldSliceDstClassSelect, 1,
                                 _bcmFieldSliceFpf3SrcDstClassSelect, 1,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                 _bcmFieldSliceSelFpf3, 0,
                                 _bcmFieldSliceDstClassSelect, 2,
                                 _bcmFieldSliceFpf3SrcDstClassSelect, 1,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                 _bcmFieldSliceSelFpf3, 0,
                                 _bcmFieldSliceDstClassSelect, 3,
                                 _bcmFieldSliceFpf3SrcDstClassSelect, 1,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVrf,
                               fp_tcam_ofs + f3_ofs + 10, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVpn,
                               fp_tcam_ofs + f3_ofs + 10, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVlan,
                               fp_tcam_ofs + f3_ofs + 10, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f3_ofs + 24, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f3_ofs + 24, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f3_ofs + 24, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f3_ofs + 24, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect, _bcmFieldFwdEntityWlanGport,
                               fp_tcam_ofs + f3_ofs + 24, 13
                               );

    /* FPF3 = 1 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                                 _bcmFieldSliceSelFpf3, 1,
                                 _bcmFieldSliceSrcClassSelect, 0,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                                 _bcmFieldSliceSelFpf3, 1,
                                 _bcmFieldSliceSrcClassSelect, 1,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                 _bcmFieldSliceSelFpf3, 1,
                                 _bcmFieldSliceSrcClassSelect, 2,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                 _bcmFieldSliceSelFpf3, 1,
                                 _bcmFieldSliceSrcClassSelect, 3,
                                 fp_tcam_ofs + f3_ofs, 10
                                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVrf,
                               fp_tcam_ofs + f3_ofs + 10, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVpn,
                               fp_tcam_ofs + f3_ofs + 10, 14
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect, _bcmFieldFwdFieldVlan,
                               fp_tcam_ofs + f3_ofs + 10, 14
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 1,
                 fp_tcam_ofs + f3_ofs + 24, 12
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_tcam_ofs + f3_ofs + 36, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 1,
                               fp_tcam_ofs + f3_ofs + 36, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_tcam_ofs + f3_ofs + 36, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_tcam_ofs + f3_ofs + 36, 10
                               );

    /* FPF3 = 2 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMultipath,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f3_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f3_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f3_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f3_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f3_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityWlanGport,
                               fp_tcam_ofs + f3_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                               fp_tcam_ofs + f3_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunkMemberGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                               fp_tcam_ofs + f3_ofs + 21, 17
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                               fp_tcam_ofs + f3_ofs + 21 + 7, 10
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                               fp_tcam_ofs + f3_ofs + 21 + 7, 8
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
                 _bcmFieldSliceSelFpf3, 2,
                 fp_tcam_ofs + f3_ofs + 37, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 2,
                 fp_tcam_ofs + f3_ofs + 38, 3
                 );

    /* FPF3 = 3 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 16, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 16 + 2, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 20, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 22, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 25, 6
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 31, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 34, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_tcam_ofs + f3_ofs + 37, 3
                 );

    /* FPF3 = 4 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 16, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 16, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 16 + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 16 + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 32, 4
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 36, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIpLocal,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_tcam_ofs + f3_ofs + 38, 1
                 );

    /* FPF3 = 5 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_tcam_ofs + f3_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_tcam_ofs + f3_ofs, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_tcam_ofs + f3_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_tcam_ofs + f3_ofs + 13, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_tcam_ofs + f3_ofs + 16, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_tcam_ofs + f3_ofs + 32, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_tcam_ofs + f3_ofs + 35, 3
                 );

    /* FPF3 = 6 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 1, 1
                 );
    /* Bit 2 is VXLT0/VXLT1 hit indicator; not used */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 3, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 4, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 6, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 7, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 8, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 9, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 10, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 10, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 11, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 13, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 14, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 15, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 16, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillEgressRbridgeHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 16, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 16, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 16, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillIngressRbridgeHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 17, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 20, 6
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 26, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 26, 12
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 26 + 12, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_tcam_ofs + f3_ofs + 26 + 13, 3
                 );

    /* FPF3 = 7 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf3, 7,
                 fp_tcam_ofs + f3_ofs, 24
                 );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, _bcmFieldQualifyRangeCheckBits24_31,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 3,
                               fp_tcam_ofs + f3_ofs + 24, 8
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 0,
                               fp_tcam_ofs + f3_ofs + 24, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 1,
                               fp_tcam_ofs + f3_ofs + 24, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 2,
                               fp_tcam_ofs + f3_ofs + 24, 12
                               );

    /* FPF3 = 8 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_tcam_ofs + f3_ofs, 20
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_tcam_ofs + f3_ofs + 20, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_tcam_ofs + f3_ofs + 20 + 2, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_tcam_ofs + f3_ofs + 24, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_tcam_ofs + f3_ofs + 26, 2
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_tcam_ofs + f3_ofs + 28, 2
                 );

    /* FPF3 = 9 */
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData3,
                 _bcmFieldSliceSelFpf3, 9,
                 fp_tcam_ofs + f3_ofs, 32
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyGenericAssociatedChannelLabelValid,
                 _bcmFieldSliceSelFpf3, 9,
                 fp_tcam_ofs + f3_ofs + 32, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRouterAlertLabelValid,
                 _bcmFieldSliceSelFpf3, 9,
                 fp_tcam_ofs + f3_ofs + 33, 1
                 );

    /*
     * FPF3=9 - UDF1_95_64
     */

    /* FPF3 = 10 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMultipath,
                               fp_tcam_ofs + f3_ofs, 21
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf3, 10,
                 fp_tcam_ofs + f3_ofs + 21, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf3, 10,
                 fp_tcam_ofs + f3_ofs + 21, 16
                 );

    /* FPF3 = 11 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs, 16
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs + 7, 9
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs + 16, 14
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs + 16, 14
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs + 16, 14
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs + 16, 14
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_tcam_ofs + f3_ofs + 29, 1
                 );

    /* FPF3 = 12 */

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagVn,
                               fp_tcam_ofs + f3_ofs, 33
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCnTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagCn,
                               fp_tcam_ofs + f3_ofs, 33
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyFabricQueueTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagFabricQueue,
                               fp_tcam_ofs + f3_ofs, 33
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabel,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                               fp_tcam_ofs + f3_ofs, 33
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelTtl,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,
                     fp_tcam_ofs + f3_ofs,      8,
                     fp_tcam_ofs + f3_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelBos,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,
                     fp_tcam_ofs + f3_ofs + 8,  1,
                     fp_tcam_ofs + f3_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelExp,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,
                     fp_tcam_ofs + f3_ofs + 9,  3,
                     fp_tcam_ofs + f3_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelId,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                               fp_tcam_ofs + f3_ofs + 12, 21
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWord,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsCntlWord,
                               fp_tcam_ofs + f3_ofs, 33
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashUpper,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagRtag7A,
                               fp_tcam_ofs + f3_ofs + 16, 17
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagRtag7A,
                     0,
                     fp_tcam_ofs + f3_ofs,      16,
                     fp_tcam_ofs + f3_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashUpper,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagRtag7B,
                               fp_tcam_ofs + f3_ofs + 16, 17
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagRtag7B,
                     0,
                     fp_tcam_ofs + f3_ofs,      16,
                     fp_tcam_ofs + f3_ofs + 32, 1,
                     0,                         0,
                     0
                     );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelAction,
                 _bcmFieldSliceSelFpf3, 12,
                 fp_tcam_ofs + f3_ofs + 33, 3
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWordValid,
                 _bcmFieldSliceSelFpf3, 12,
                 fp_tcam_ofs + f3_ofs + 36, 1
                 );

    /* FPF3 = 13 */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_tcam_ofs + f3_ofs, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeader2Type,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_tcam_ofs + f3_ofs + 8, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_tcam_ofs + f3_ofs + 16, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_tcam_ofs + f3_ofs + 24, 8
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIpLocal,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_tcam_ofs + f3_ofs + 32, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpAddrsNormalized,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_tcam_ofs + f3_ofs + 33, 1
                 );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_tcam_ofs + f3_ofs + 34, 1
                 );

    /***** FPF0 double-wide *****/

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs, 16
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs, 16
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs + 16, 17
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs + 16, 17
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs + 16, 17
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs + 16, 17
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs + 32, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyColor,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs + 33, 2
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_global_mask_tcam_ofs + dw_f0_ofs + 35, 2
                            );

    /***** FPF1 double-wide *****/

    /* FPF1 = 0 */

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs, 8
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs + 8, 16
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs + 24, 16
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs + 24, 16
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport,
                               fp_tcam_ofs + dw_f1_ofs + 40, 21
                               );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMultipath,
                            fp_tcam_ofs + dw_f1_ofs + 40, 21
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs + 61, 4
                            );

    /* FPF1 = 1 */

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelId,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs, 20
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelAction,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs + 20, 3
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWord,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs + 23, 32
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWordValid,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs + 55, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f1_ofs + 56, 4
                            );

    /***** FPF2 double-wide *****/

    /* FPF2 = 0 */

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 1, 1
                            );
    /* Bit 2 is VXLT0/VXLT1 hit indicator; not used */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 3, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 4, 2
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 6, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 7, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 8, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 9, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 10, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 10, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 11, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 12, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 13, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 14, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 15, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 16, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTrillEgressRbridgeHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 16, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 16, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 16, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 17, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTrillIngressRbridgeHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 17, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 17, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 2 + 17, 1
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 22, 6
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 28, 24
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyRangeCheckBits24_31,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 3,
                            fp_tcam_ofs + dw_f2_ofs + 52, 8
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 0,
                            fp_tcam_ofs + dw_f2_ofs + 52, 12
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 1,
                            fp_tcam_ofs + dw_f2_ofs + 52, 12
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 2,
                            fp_tcam_ofs + dw_f2_ofs + 52, 12
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 64, 32
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs + 96, 32
                            );

    /* FPF2 = 1 */

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs, 128
                            );

    /* FPF2 = 2 */

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                            _bcmFieldSliceSelFpf2, 2,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs, 128
                            );

    /* FPF2 = 3 */

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                            _bcmFieldSliceSelFpf2, 3,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f2_ofs, 128
                            );

    /***** FPF3 double-wide  *****/

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f3_ofs, 8
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f3_ofs + 8, 8
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f3_ofs + 16, 8
                            );
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f3_ofs + 24, 8
                            );

    /***** FPF4 double-wide *****/

    /* FPF4 = 0 */

    

    /* FPF4 = 1 */

    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf4, 1,
                            _bcmFieldSliceSelDisable, 0,
                            fp_tcam_ofs + dw_f4_ofs, 6
                            );
    return (BCM_E_NONE);
}
#endif /* BCM_HELIX4_SUPPORT */

/*
 * Function:
 *     _field_tr3_qualifiers_init
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
_field_tr3_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
        return (_field_tr3_lookup_qualifiers_init(unit, stage_fc));
    case _BCM_FIELD_STAGE_INGRESS:
#ifdef BCM_HELIX4_SUPPORT
        if (SOC_IS_HELIX4(unit)) {
        return (_field_hx4_ingress_qualifiers_init(unit, stage_fc));
        } else
#endif /* BCM_HELIX4_SUPPORT */ 
        {
        return (_field_tr3_ingress_qualifiers_init(unit, stage_fc));
        }
    case _BCM_FIELD_STAGE_EXTERNAL:
        return (_field_tr3_external_qualifiers_init(unit, stage_fc));
    case _BCM_FIELD_STAGE_EGRESS:
        return (_field_tr3_egress_qualifiers_init(unit, stage_fc));
    default:
        ;
    }

    sal_free(stage_fc->f_qual_arr);
    return (BCM_E_INTERNAL);
}


STATIC int
_field_tr3_ingress_slice_enable_set(int            unit,
                                    _field_stage_t *stage_fc,
                                    _field_slice_t *fs,
                                    unsigned       endis
                                    )
{
    static const soc_field_t regfield[] = {
        FP_SLICE_ENABLE_SLICE_0f,
        FP_SLICE_ENABLE_SLICE_1f,
        FP_SLICE_ENABLE_SLICE_2f,
        FP_SLICE_ENABLE_SLICE_3f,
        FP_SLICE_ENABLE_SLICE_4f,
        FP_SLICE_ENABLE_SLICE_5f,
        FP_SLICE_ENABLE_SLICE_6f,
        FP_SLICE_ENABLE_SLICE_7f,
        FP_SLICE_ENABLE_SLICE_8f,
        FP_SLICE_ENABLE_SLICE_9f,
        FP_SLICE_ENABLE_SLICE_10f,
        FP_SLICE_ENABLE_SLICE_11f,
        FP_SLICE_ENABLE_SLICE_12f,
        FP_SLICE_ENABLE_SLICE_13f,
        FP_SLICE_ENABLE_SLICE_14f,
        FP_SLICE_ENABLE_SLICE_15f
    };

    uint32 regval[1];

    BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                      FP_SLICE_ENABLEr,
                                      REG_PORT_ANY,
                                      0,
                                      regval
                                      )
                        );

    soc_reg_field_set(unit,
                      FP_SLICE_ENABLEr,
                      regval,
                      regfield[fs->slice_number],
                      endis ? 1 : 0
                      );

    return (soc_reg32_set(unit, FP_SLICE_ENABLEr, REG_PORT_ANY, 0, regval[0]));
}



int
_bcm_field_tr3_slice_enable_set(int            unit,
                                _field_stage_t *stage_fc,
                                _field_group_t *fg,
                                _field_slice_t *fs,
                                unsigned       endis
                                )
{
    switch (stage_fc->stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
        return (_field_tr3_ingress_slice_enable_set(unit, stage_fc, fs, endis));
    default:
        ;
    }

    return (_bcm_field_fb_slice_enable_set(unit, fg, fs->slice_number, endis));

}


STATIC int
_field_tr3_lookup_slice_clear(int unit, unsigned slice_num)
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
 *     _field_tr3_ingress_slice_clear
 *
 * Purpose:
 *     Resets the IFP field slice configuration.
 *
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     slice_numb - (IN) Field slice number.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr3_ingress_slice_clear(int unit, uint8 slice_numb)
{
    uint32 rval;

    BCM_IF_ERROR_RETURN
        (_bcm_field_trx_ingress_slice_clear(unit, slice_numb));

    /* Get Src/Dest ClassID select for F1.0 & F3.0 keys. */
    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit,
                       IFP_KEY_CLASSID_SELECTr,
                       REG_PORT_ANY,
                       0,
                       &rval
                       )
        );

    /* Reset slice F1_0 Src/Dst classid selection. */
    soc_reg_field_set(unit,
                      IFP_KEY_CLASSID_SELECTr,
                      &rval,
                      F1_0_SLICEf,
                      (soc_reg_field_get(unit,
                                         IFP_KEY_CLASSID_SELECTr,
                                         rval,
                                         F1_0_SLICEf)
                      & ~(1 << slice_numb)
                      )
                    );

    /* Reset slice F3_0 Src/Dst classid selection. */
    soc_reg_field_set(unit,
                      IFP_KEY_CLASSID_SELECTr,
                      &rval,
                      F3_0_SLICEf,
                      (soc_reg_field_get(unit,
                                        IFP_KEY_CLASSID_SELECTr,
                                        rval,
                                        F3_0_SLICEf)
                      & ~(1 << slice_numb)
                      )
                    );

    /* Update hardware register. */
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit,
                       IFP_KEY_CLASSID_SELECTr,
                       REG_PORT_ANY,
                       0,
                       rval
                       )
        );

    /* Clear double wide key selector settings. */
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit,
                                FP_DOUBLE_WIDE_SELECTr,
                                REG_PORT_ANY,
                                _tr3_dw_f1_sel[slice_numb],
                                0
                                )
        );
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit,
                                FP_DOUBLE_WIDE_SELECTr,
                                REG_PORT_ANY,
                                _bcm_field_trx_dw_f4_sel[slice_numb],
                                0
                                )
        );

    return (BCM_E_NONE);
}

STATIC int
_field_tr3_slice_clear(int unit, _field_group_t *fg, _field_slice_t *fs)
{
    switch (fs->stage_id) {
    case _BCM_FIELD_STAGE_LOOKUP:
        return (_field_tr3_lookup_slice_clear(unit, fs->slice_number));
    case _BCM_FIELD_STAGE_INGRESS:
        return (_field_tr3_ingress_slice_clear(unit, fs->slice_number));
    case _BCM_FIELD_STAGE_EXTERNAL:
        return (BCM_E_NONE);
    case _BCM_FIELD_STAGE_EGRESS:
        return (_bcm_field_trx_egress_slice_clear(unit, fs->slice_number));
    default:
        ;
    }

    return (BCM_E_INTERNAL);
}


int
_bcm_field_tr3_group_qset_update(int unit, _field_group_t *fg)
{
    /* Add fields of MPLS forwarding label */

    if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyMplsForwardingLabel)) {
        BCM_FIELD_QSET_ADD(fg->qset, bcmFieldQualifyMplsForwardingLabelTtl);
        BCM_FIELD_QSET_ADD(fg->qset, bcmFieldQualifyMplsForwardingLabelBos);
        BCM_FIELD_QSET_ADD(fg->qset, bcmFieldQualifyMplsForwardingLabelExp);
        BCM_FIELD_QSET_ADD(fg->qset, bcmFieldQualifyMplsForwardingLabelId);
    }

    return (BCM_E_NONE);
}


STATIC int
_field_tr3_ingress_selcode_get(int              unit,
                               _field_stage_t   *stage_fc,
                               bcm_field_qset_t *qset_req,
                               _field_group_t   *fg
                               )
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


STATIC int
_field_tr3_selcode_get(int              unit,
                       _field_stage_t   *stage_fc,
                       bcm_field_qset_t *qset_req,
                       _field_group_t   *fg
                       )
{
    BCM_IF_ERROR_RETURN(_bcm_field_tr_selcode_get(unit, stage_fc, qset_req, fg));

    switch (fg->stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
        return (_field_tr3_ingress_selcode_get(unit, stage_fc, qset_req, fg));
    default:
        ;
    }

    return (BCM_E_NONE);
}


STATIC int
_field_tr3_lookup_selcodes_install(int            unit,
                                   _field_group_t *fg,
                                   uint8          slice_num,
                                   bcm_pbmp_t     *pbmp,
                                   int            selcode_idx
                                   )
{
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
_field_tr3_ingress_selcodes_install(int            unit,
                                    _field_group_t *fg,
                                    uint8          slice_num,
                                    bcm_pbmp_t     *pbmp,
                                    int            selcode_idx
                                    )
{
    _field_sel_t * const sel = &fg->sel_codes[selcode_idx];
    const unsigned pfs_idx_count = soc_mem_index_count(unit,
                                                       FP_PORT_FIELD_SELm
                                                       );
    int        errcode = BCM_E_NONE;
    bcm_port_t idx;
    uint32     buf[SOC_MAX_MEM_FIELD_WORDS];
    SHR_BITDCL *pfs_bmp = 0;
    uint32     regval;

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

    /* Selector code field for DW F2 got a new name, so program it */
    if ((fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)
        && (selcode_idx == _FP_INTRA_SLICE_PART_1
        || selcode_idx == _FP_INTRA_SLICE_PART_3)) {

        if (sel->fpf2 != _FP_SELCODE_DONT_CARE) {
            /* Do the same thing for each entry in FP_PORT_FIELD_SEL table */
            for (idx = 0; idx < pfs_idx_count; idx++) {
                static soc_field_t dw_f2_key_sel_fld[] = {
                    SLICE0_DOUBLE_WIDE_F2_KEY_SELECTf,
                    SLICE1_DOUBLE_WIDE_F2_KEY_SELECTf,
                    SLICE2_DOUBLE_WIDE_F2_KEY_SELECTf,
                    SLICE3_DOUBLE_WIDE_F2_KEY_SELECTf,
                    SLICE4_DOUBLE_WIDE_F2_KEY_SELECTf,
                    SLICE5_DOUBLE_WIDE_F2_KEY_SELECTf,
                    SLICE6_DOUBLE_WIDE_F2_KEY_SELECTf,
                    SLICE7_DOUBLE_WIDE_F2_KEY_SELECTf,
                    SLICE8_DOUBLE_WIDE_F2_KEY_SELECTf,
                    SLICE9_DOUBLE_WIDE_F2_KEY_SELECTf,
                    SLICE10_DOUBLE_WIDE_F2_KEY_SELECTf,
                    SLICE11_DOUBLE_WIDE_F2_KEY_SELECTf,
                    SLICE12_DOUBLE_WIDE_F2_KEY_SELECTf,
                    SLICE13_DOUBLE_WIDE_F2_KEY_SELECTf,
                    SLICE14_DOUBLE_WIDE_F2_KEY_SELECTf,
                    SLICE15_DOUBLE_WIDE_F2_KEY_SELECTf
                };

                if (SHR_BITGET(pfs_bmp, idx) == 0) {
                    continue;
                }
                errcode = soc_mem_read(unit, FP_PORT_FIELD_SELm, MEM_BLOCK_ANY,
                                       idx, buf);
                if (BCM_FAILURE(errcode)) {
                    goto cleanup;
                }
                soc_mem_field32_set(unit, FP_PORT_FIELD_SELm, buf,
                                    dw_f2_key_sel_fld[slice_num],
                                    sel->fpf2);
                errcode = soc_mem_write(unit, FP_PORT_FIELD_SELm, MEM_BLOCK_ALL,
                                        idx, buf);
                if (BCM_FAILURE(errcode)) {
                    goto cleanup;
                }
            }
        }

        /* Write appropriate values in FP_DOUBLE_WIDE_SELECTr */
        if (sel->fpf1 != _FP_SELCODE_DONT_CARE) {
            /*
             * F1.0 - L4
             * F1.1 - MPLS
             */
            errcode = soc_reg_field32_modify
                        (unit,
                         FP_DOUBLE_WIDE_SELECTr,
                         REG_PORT_ANY,
                         _tr3_dw_f1_sel[slice_num],
                         sel->fpf1
                         );
            if (BCM_FAILURE(errcode)) {
                goto cleanup;
            }
        }
        if (sel->fpf4 != _FP_SELCODE_DONT_CARE) {
            /*
             * F4.0 - PORT_FIELD_SEL_INDEX
             * F4.1 - TCP_FLAGS
             */
            errcode = soc_reg_field32_modify
                        (unit,
                         FP_DOUBLE_WIDE_SELECTr,
                         REG_PORT_ANY,
                         _bcm_field_trx_dw_f4_sel[slice_num],
                         sel->fpf4
                         );
            if (BCM_FAILURE(errcode)) {
                goto cleanup;
            }
        }
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
            { SLICE11_NORMALIZE_IP_ADDRf, SLICE11_NORMALIZE_MAC_ADDRf },
            { SLICE12_NORMALIZE_IP_ADDRf, SLICE12_NORMALIZE_MAC_ADDRf },
            { SLICE13_NORMALIZE_IP_ADDRf, SLICE13_NORMALIZE_MAC_ADDRf },
            { SLICE14_NORMALIZE_IP_ADDRf, SLICE14_NORMALIZE_MAC_ADDRf },
            { SLICE15_NORMALIZE_IP_ADDRf, SLICE15_NORMALIZE_MAC_ADDRf }
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

    errcode = soc_reg32_get(unit,
                            IFP_KEY_CLASSID_SELECTr,
                            REG_PORT_ANY,
                            0,
                            &regval
                            );
    if (BCM_FAILURE(errcode)) {
        goto cleanup;
    }

    if (sel->src_dest_class_f1_sel != _FP_SELCODE_DONT_CARE) {
        soc_reg_field_set(unit,
                          IFP_KEY_CLASSID_SELECTr,
                          &regval,
                          F1_0_SLICEf,
                          (soc_reg_field_get(unit,
                                             IFP_KEY_CLASSID_SELECTr,
                                             regval,
                                             F1_0_SLICEf
                                             )
                           & ~(1 << slice_num)
                           )
                          | (sel->src_dest_class_f1_sel << slice_num)
                          );
    }
    if (sel->src_dest_class_f3_sel != _FP_SELCODE_DONT_CARE) {
        soc_reg_field_set(unit,
                          IFP_KEY_CLASSID_SELECTr,
                          &regval,
                          F3_0_SLICEf,
                          (soc_reg_field_get(unit,
                                             IFP_KEY_CLASSID_SELECTr,
                                             regval,
                                             F3_0_SLICEf
                                             )
                           & ~(1 << slice_num)
                           )
                          | (sel->src_dest_class_f3_sel << slice_num)
                          );
    }

    errcode = soc_reg32_set(unit,
                            IFP_KEY_CLASSID_SELECTr,
                            REG_PORT_ANY,
                            0,
                            regval
                            );

 cleanup:
    if (pfs_bmp) {
        sal_free(pfs_bmp);
    }
    return (errcode);
}


STATIC int
_field_tr3_egress_selcodes_install(int            unit,
                                   _field_group_t *fg,
                                   uint8          slice_num,
                                   bcm_pbmp_t     *pbmp,
                                   int            selcode_idx
                                   )
{
    static const soc_field_t fldtbl[][4] = {
        { SLICE_0_F1f, SLICE_1_F1f, SLICE_2_F1f, SLICE_3_F1f },
        { SLICE_0_F2f, SLICE_1_F2f, SLICE_2_F2f, SLICE_3_F2f },
        { SLICE_0_F3f, SLICE_1_F3f, SLICE_2_F3f, SLICE_3_F3f },
        { SLICE_0_F4f, SLICE_1_F4f, SLICE_2_F4f, SLICE_3_F4f }
    };

    static const soc_field_t dvpfldtbl[4] = {
        SLICE_0f, SLICE_1f, SLICE_2f, SLICE_3f };

    static const soc_field_t mdlfldtbl[4] = {
        SLICE_0f, SLICE_1f, SLICE_2f, SLICE_3f };

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
    if (sel->egr_class_f3_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_CLASSID_SELECTORr,
                                                   REG_PORT_ANY,
                                                   fldtbl[2][slice_num],
                                                   sel->egr_class_f3_sel
                                                   )
                            );
    }
    if (sel->egr_class_f4_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_CLASSID_SELECTORr,
                                                   REG_PORT_ANY,
                                                   fldtbl[3][slice_num],
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

    return (BCM_E_NONE);
}



STATIC int
_field_tr3_lookup_mode_set(int            unit,
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


STATIC int
_field_tr3_external_mode_set(int            unit,
                             uint8          slice_num,
                             _field_group_t *fg,
                             uint8          flags
                             )
{
    unsigned                  pkt_type;
    soc_tcam_partition_type_t tcam_part;

    switch (slice_num) {
    case _FP_EXT_ACL_144_L2:
        pkt_type  = PKT_TYPE_L2;
        tcam_part = TCAM_PARTITION_ACL_L2C;
        break;
    case _FP_EXT_ACL_L2:
        pkt_type  = PKT_TYPE_L2;
        tcam_part = TCAM_PARTITION_ACL_L2;
        break;
    case _FP_EXT_ACL_144_IPV4:
        pkt_type  = PKT_TYPE_IPV4;
        tcam_part = TCAM_PARTITION_ACL_IP4C;
        break;
    case _FP_EXT_ACL_IPV4:
        pkt_type  = PKT_TYPE_IPV4;
        tcam_part = TCAM_PARTITION_ACL_IP4;
        break;
    case _FP_EXT_ACL_L2_IPV4:
        pkt_type  = PKT_TYPE_IPV4;
        tcam_part = TCAM_PARTITION_ACL_L2IP4;
        break;
    case _FP_EXT_ACL_144_IPV6:
        pkt_type  = PKT_TYPE_IPV6;
        tcam_part = TCAM_PARTITION_ACL_IP6C;
        break;
    case _FP_EXT_ACL_IPV6_SHORT:
        pkt_type  = PKT_TYPE_IPV6;
        tcam_part = TCAM_PARTITION_ACL_IP6S;
        break;
    case _FP_EXT_ACL_IPV6_FULL:
        pkt_type  = PKT_TYPE_IPV6;
        tcam_part = TCAM_PARTITION_ACL_IP6F;
        break;
    case _FP_EXT_ACL_L2_IPV6:
        pkt_type  = PKT_TYPE_IPV6;
        tcam_part = TCAM_PARTITION_ACL_L2IP6;
        break;
    default:
        return (BCM_E_INTERNAL);
    }

    return (soc_tr3_esm_pkt_type_to_slice_map_set(unit, pkt_type, tcam_part));
}


STATIC int
_field_tr3_mode_set(int unit, uint8 slice_num, _field_group_t *fg, uint8 flags)
{
    int (*func)(int unit, uint8 slice_num, _field_group_t *fg, uint8 flags);

    switch (fg->stage_id) {
    case _BCM_FIELD_STAGE_LOOKUP:
        func = _field_tr3_lookup_mode_set;
        break;
    case _BCM_FIELD_STAGE_INGRESS:
        return (BCM_E_NONE);
    case _BCM_FIELD_STAGE_EXTERNAL:
        func = _field_tr3_external_mode_set;
        break;
    case _BCM_FIELD_STAGE_EGRESS:
        func = _bcm_field_trx_egress_mode_set;
        break;
    default:
        return (BCM_E_INTERNAL);
    }

    return ((*func)(unit, slice_num, fg, flags));
}

STATIC int
_field_tr3_selcodes_install(int            unit,
                            _field_group_t *fg,
                            uint8          slice_num,
                            bcm_pbmp_t     pbmp,
                            int            selcode_idx
                            )
{
    int (*func)(int, _field_group_t *, uint8, bcm_pbmp_t *, int);

    BCM_IF_ERROR_RETURN(_field_tr3_mode_set(unit, slice_num, fg, fg->flags));

    switch (fg->stage_id) {
    case _BCM_FIELD_STAGE_LOOKUP:
        func = _field_tr3_lookup_selcodes_install;
        break;
    case _BCM_FIELD_STAGE_INGRESS:
        func = _field_tr3_ingress_selcodes_install;
        break;
    case _BCM_FIELD_STAGE_EXTERNAL:
        return (BCM_E_NONE);
    case _BCM_FIELD_STAGE_EGRESS:
        func = _field_tr3_egress_selcodes_install;
        break;
    default:
        return (BCM_E_INTERNAL);
    }

    return ((*func)(unit, fg, slice_num, &pbmp, selcode_idx));
}


STATIC int
_field_tr3_external_tcam_data_mem(int unit, unsigned slice_num, soc_mem_t *tcam_data_mem)
{
    static const soc_mem_t tcam_data_mems[] = {
        EXT_ACL144_TCAM_L2m,
        EXT_ACL288_TCAM_L2m,
        EXT_ACL144_TCAM_IPV4m,
        EXT_ACL288_TCAM_IPV4m,
        EXT_ACL432_TCAM_DATA_L2_IPV4m,
        EXT_ACL144_TCAM_IPV6m,
        EXT_ACL360_TCAM_DATA_IPV6_SHORTm,
        EXT_ACL432_TCAM_DATA_IPV6_LONGm,
        EXT_ACL432_TCAM_DATA_L2_IPV6m
    };

    if (slice_num >= COUNTOF(tcam_data_mems)) {
        *tcam_data_mem = INVALIDm;
        return (BCM_E_INTERNAL);
    }

    *tcam_data_mem = tcam_data_mems[slice_num];
    return (BCM_E_NONE);
}


STATIC int
_field_tr3_external_tcam_mask_mem(int unit, unsigned slice_num, soc_mem_t *tcam_mask_mem)
{
    static const soc_mem_t tcam_mask_mems[] = {
        INVALIDm,
        INVALIDm,
        INVALIDm,
        INVALIDm,
        INVALIDm,
        INVALIDm,
        EXT_ACL360_TCAM_MASK_IPV6_SHORTm,
        EXT_ACL432_TCAM_MASK_IPV6_LONGm,
        EXT_ACL432_TCAM_MASK_L2_IPV6m
    };

    if (slice_num >= COUNTOF(tcam_mask_mems)) {
        *tcam_mask_mem = INVALIDm;
        return (BCM_E_INTERNAL);
    }

    *tcam_mask_mem = tcam_mask_mems[slice_num];
    return (BCM_E_NONE);
}


STATIC int
_field_tr3_external_policy_mem(int unit, unsigned slice_num, soc_mem_t *policy_mem)
{
    static const soc_mem_t policy_mems[][_FP_EXT_NUM_PARTITIONS] = {
        { INVALIDm,
          INVALIDm,
          INVALIDm,
          INVALIDm,
          INVALIDm,
          INVALIDm,
          INVALIDm,
          INVALIDm,
          INVALIDm
        },
        { EXT_FP_POLICY_ACL144_L2_1Xm,
          EXT_FP_POLICY_ACL288_L2_1Xm,
          EXT_FP_POLICY_ACL144_IPV4_1Xm,
          EXT_FP_POLICY_ACL288_IPV4_1Xm,
          EXT_FP_POLICY_ACL432_L2_IPV4_1Xm,
          EXT_FP_POLICY_ACL144_IPV6_1Xm,
          EXT_FP_POLICY_ACL360_IPV6_SHORT_1Xm,
          EXT_FP_POLICY_ACL432_IPV6_LONG_1Xm,
          EXT_FP_POLICY_ACL432_L2_IPV6_1Xm
        },
        { EXT_FP_POLICY_ACL144_L2_2Xm,
          EXT_FP_POLICY_ACL288_L2_2Xm,
          EXT_FP_POLICY_ACL144_IPV4_2Xm,
          EXT_FP_POLICY_ACL288_IPV4_2Xm,
          EXT_FP_POLICY_ACL432_L2_IPV4_2Xm,
          EXT_FP_POLICY_ACL144_IPV6_2Xm,
          EXT_FP_POLICY_ACL360_IPV6_SHORT_2Xm,
          EXT_FP_POLICY_ACL432_IPV6_LONG_2Xm,
          EXT_FP_POLICY_ACL432_L2_IPV6_2Xm
        },
        { EXT_FP_POLICY_ACL144_L2_3Xm,
          EXT_FP_POLICY_ACL288_L2_3Xm,
          EXT_FP_POLICY_ACL144_IPV4_3Xm,
          EXT_FP_POLICY_ACL288_IPV4_3Xm,
          EXT_FP_POLICY_ACL432_L2_IPV4_3Xm,
          EXT_FP_POLICY_ACL144_IPV6_3Xm,
          EXT_FP_POLICY_ACL360_IPV6_SHORT_3Xm,
          EXT_FP_POLICY_ACL432_IPV6_LONG_3Xm,
          EXT_FP_POLICY_ACL432_L2_IPV6_3Xm
        },
        { EXT_FP_POLICY_ACL144_L2_4Xm,
          EXT_FP_POLICY_ACL288_L2_4Xm,
          EXT_FP_POLICY_ACL144_IPV4_4Xm,
          EXT_FP_POLICY_ACL288_IPV4_4Xm,
          EXT_FP_POLICY_ACL432_L2_IPV4_4Xm,
          EXT_FP_POLICY_ACL144_IPV6_4Xm,
          EXT_FP_POLICY_ACL360_IPV6_SHORT_4Xm,
          EXT_FP_POLICY_ACL432_IPV6_LONG_4Xm,
          EXT_FP_POLICY_ACL432_L2_IPV6_4Xm
        },
        { INVALIDm,
          INVALIDm,
          INVALIDm,
          INVALIDm,
          INVALIDm,
          INVALIDm,
          INVALIDm,
          INVALIDm,
          INVALIDm
        },
        { EXT_FP_POLICY_ACL144_L2_6Xm,
          EXT_FP_POLICY_ACL288_L2_6Xm,
          EXT_FP_POLICY_ACL144_IPV4_6Xm,
          EXT_FP_POLICY_ACL288_IPV4_6Xm,
          EXT_FP_POLICY_ACL432_L2_IPV4_6Xm,
          EXT_FP_POLICY_ACL144_IPV6_6Xm,
          EXT_FP_POLICY_ACL360_IPV6_SHORT_6Xm,
          EXT_FP_POLICY_ACL432_IPV6_LONG_6Xm,
          EXT_FP_POLICY_ACL432_L2_IPV6_6Xm
        }
    };

    static const char * const policy_width_props[] = {
        spn_EXT_L2C_ACL_TABLE_POLICY_WIDTH,
        spn_EXT_L2_ACL_TABLE_POLICY_WIDTH,
        spn_EXT_IP4C_ACL_TABLE_POLICY_WIDTH,
        spn_EXT_IP4_ACL_TABLE_POLICY_WIDTH,
        spn_EXT_L2IP4_ACL_TABLE_POLICY_WIDTH,
        spn_EXT_IP6C_ACL_TABLE_POLICY_WIDTH,
        spn_EXT_IP6S_ACL_TABLE_POLICY_WIDTH,
        spn_EXT_IP6F_ACL_TABLE_POLICY_WIDTH,
        spn_EXT_L2IP6_ACL_TABLE_POLICY_WIDTH
    };

    unsigned width;

    if ((width = soc_property_get(unit, policy_width_props[slice_num], 1)) >= COUNTOF(policy_mems)
        || slice_num >= COUNTOF(policy_mems[0])
        ) {
        *policy_mem = INVALIDm;
        return (BCM_E_INTERNAL);
    }

    *policy_mem = policy_mems[width][slice_num];
    return (BCM_E_NONE);
}


int
_bcm_field_tr3_external_present(int unit)
{
    /* Check that all EXTFP TCAMs are valid, and it is not the case that they
       are all zero size
    */

    unsigned  slice_num, n, z;

    for (slice_num = z = 0, n = _FP_EXT_NUM_PARTITIONS; n; --n, ++slice_num) {
        soc_mem_t ext_tcam_data_mem;

        if (BCM_FAILURE(_field_tr3_external_tcam_data_mem(unit,
                                                          slice_num,
                                                          &ext_tcam_data_mem
                                                          )
                        )
            || !SOC_MEM_IS_VALID(unit, ext_tcam_data_mem)
            ) {
            return (FALSE);
        }
        if (soc_mem_index_count(unit, ext_tcam_data_mem) == 0) {
            ++z;
        }
    }

    return (z != _FP_EXT_NUM_PARTITIONS);
}


STATIC int
_field_tr3_stage_action_support_check(int                unit,
                                      unsigned           stage,
                                      bcm_field_action_t action,
                                      int                *result
                                      )
{
    switch (action) {
        case bcmFieldActionNewClassId:
        case bcmFieldActionTrunkLoadBalanceCancel:
        case bcmFieldActionEcmpLoadBalanceCancel:
        case bcmFieldActionEgressClassSelect:
        case bcmFieldActionHiGigClassSelect:
        case bcmFieldActionOamDmTimeFormat:
        case bcmFieldActionOamLmDmSampleEnable:
        case bcmFieldActionOamTagStatusCheck:
        case bcmFieldActionOamTunnelControl:
        case bcmFieldActionReplaceInnerVlanPriority:
        case bcmFieldActionReplaceOuterVlanPriority:
        case bcmFieldActionReplaceInnerVlan:
        case bcmFieldActionReplaceOuterVlan:
        case bcmFieldActionReplaceSrcMac:
        case bcmFieldActionReplaceDstMac:
            *result = (stage == _BCM_FIELD_STAGE_INGRESS);
            return (BCM_E_NONE);

        case bcmFieldActionRegex:
        case bcmFieldActionNoRegex:
            *result = (stage == _BCM_FIELD_STAGE_LOOKUP);
            return (BCM_E_NONE);

        case bcmFieldActionPolicerLevel0:
        case bcmFieldActionPolicerLevel1:
            *result = ((stage == _BCM_FIELD_STAGE_INGRESS) || (stage == _BCM_FIELD_STAGE_EXTERNAL));
            return (BCM_E_NONE);

        case bcmFieldActionPolicerGroup:
        case bcmFieldActionStatGroup:
            *result = ((stage == _BCM_FIELD_STAGE_LOOKUP) || (stage == _BCM_FIELD_STAGE_EXTERNAL));
            return (BCM_E_NONE);

        case bcmFieldActionCopyToCpu:
        case bcmFieldActionRpCopyToCpu:
        case bcmFieldActionYpCopyToCpu:
        case bcmFieldActionGpCopyToCpu:
            *result = ((stage == _BCM_FIELD_STAGE_INGRESS) ||
                                (stage ==  _BCM_FIELD_STAGE_LOOKUP) ||
                                (stage == _BCM_FIELD_STAGE_EXTERNAL)) ? TRUE
                                : (soc_feature(unit, soc_feature_axp)
                                   && (_BCM_FIELD_STAGE_EGRESS == stage));
            return (BCM_E_NONE);
        default:
            ;
    }

    return (_bcm_field_trx_stage_action_support_check(unit, stage, action, result));
}


STATIC int
_field_tr3_action_support_check(int                unit,
                                _field_entry_t     *f_ent,
                                bcm_field_action_t action,
                                int                *result
                                )
{
    return (_field_tr3_stage_action_support_check(unit,
                                                  f_ent->group->stage_id,
                                                  action,
                                                  result
                                                  )
            );
}


int
_bcm_field_tr3_external_policy_mem_get(int             unit,
                                       _field_action_t *fa,
                                       soc_mem_t       *mem
                                       )
{
    *mem = FP_POLICY_TABLEm;

    return (BCM_E_NONE);
}


STATIC int
_field_tr3_action_params_check(int             unit,
                               _field_entry_t  *f_ent,
                               _field_action_t *fa
                               )
{
    uint32 arg = fa->param[0];  /* Action param value      */
    soc_mem_t mem;              /* Policy table memory id. */
    soc_mem_t tcam_mem;         /* Tcam memory id.         */
    bcm_port_t local_port;
    int index;
    int rv = 0;
    int cosq_new;

    if (_BCM_FIELD_STAGE_EXTERNAL != f_ent->group->stage_id) {

        BCM_IF_ERROR_RETURN(_field_fb_tcam_policy_mem_get(unit,
            f_ent->group->stage_id, &tcam_mem, &mem));

        switch (fa->action) {
        case bcmFieldActionNewClassId:
            PolicyCheck(unit, mem, I2E_CLASSIDf, arg);
            return (BCM_E_NONE);

        case bcmFieldActionEgressClassSelect:
            return (arg <= BCM_FIELD_EGRESS_CLASS_SELECT_NEW
                    ? BCM_E_NONE : BCM_E_PARAM
                    );

        case bcmFieldActionHiGigClassSelect:
            return (arg <= BCM_FIELD_HIGIG_CLASS_SELECT_PORT
                    ? BCM_E_NONE : BCM_E_PARAM
                    );

        case bcmFieldActionOamDmTimeFormat:
            return (arg <= BCM_FIELD_OAM_DM_TIME_FORMAT_NTP
                    ? BCM_E_NONE : BCM_E_PARAM
                );

        case bcmFieldActionOamTagStatusCheck:
            return (arg < (BCM_FIELD_OAM_TAG_STATUS_INNER_L2_INNER_MASK << 1)
                    ? BCM_E_NONE : BCM_E_PARAM
                    );

        case bcmFieldActionOamTunnelControl:
            return (arg <= BCM_FIELD_OAM_TUNNEL_CONTROL_NOT_TUNNELDED
                    ? BCM_E_NONE : BCM_E_PARAM
                    );

        case bcmFieldActionEcnNew:
        case bcmFieldActionRpEcnNew:
        case bcmFieldActionYpEcnNew:
        case bcmFieldActionGpEcnNew:
            return (arg <= 3 ? BCM_E_NONE : BCM_E_PARAM);

        case bcmFieldActionUcastCosQNew:
             if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(fa->param[0])) {
                 return BCM_E_PARAM;
             }
         /* passthru */
         /* coverity[MISSING_BREAK: FALSE] */
        case bcmFieldActionCosQNew:
             if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(fa->param[0])) {
                 BCM_IF_ERROR_RETURN
                     (_bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                                       _BCM_TR3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                       &local_port, &index, NULL));

                 PolicyCheck(unit, mem, R_COS_INT_PRIf, index);
                 PolicyCheck(unit, mem, Y_COS_INT_PRIf, index);
                 PolicyCheck(unit, mem, G_COS_INT_PRIf, index);
             } else {
                 PolicyCheck(unit, mem, R_COS_INT_PRIf, arg);
                 PolicyCheck(unit, mem, Y_COS_INT_PRIf, arg);
                 PolicyCheck(unit, mem, G_COS_INT_PRIf, arg);
             }
             return (BCM_E_NONE);
        case bcmFieldActionGpCosQNew:
        case bcmFieldActionGpUcastCosQNew:
             if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(fa->param[0])) {
                 BCM_IF_ERROR_RETURN
                     (_bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                                       _BCM_TR3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                       &local_port, &index, NULL));

                 PolicyCheck(unit, mem, G_COS_INT_PRIf, index);
             } else {
                 PolicyCheck(unit, mem, G_COS_INT_PRIf, arg);
             }
            return (BCM_E_NONE);

        case bcmFieldActionYpCosQNew:
        case bcmFieldActionYpUcastCosQNew:
             if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(fa->param[0])) {
                 BCM_IF_ERROR_RETURN
                     (_bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                                       _BCM_TR3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                       &local_port, &index, NULL));

                 PolicyCheck(unit, mem, Y_COS_INT_PRIf, index);
             } else {
                 PolicyCheck(unit, mem, Y_COS_INT_PRIf, arg);
             }
            return (BCM_E_NONE);

        case bcmFieldActionRpCosQNew:
        case bcmFieldActionRpUcastCosQNew:
             if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(fa->param[0])) {
                 BCM_IF_ERROR_RETURN
                     (_bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                                       _BCM_TR3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                       &local_port, &index, NULL));

                 PolicyCheck(unit, mem, R_COS_INT_PRIf, index);
             } else {
                 PolicyCheck(unit, mem, R_COS_INT_PRIf, arg);
             }
            return (BCM_E_NONE);

        case bcmFieldActionMcastCosQNew:
            PolicyCheck(unit, mem, R_COS_INT_PRIf, (arg << 4));
            PolicyCheck(unit, mem, Y_COS_INT_PRIf, (arg << 4));
            PolicyCheck(unit, mem, G_COS_INT_PRIf, (arg << 4));
            return (BCM_E_NONE);

        case bcmFieldActionGpMcastCosQNew:
            PolicyCheck(unit, mem, G_COS_INT_PRIf, (arg << 4));
            return (BCM_E_NONE);

        case bcmFieldActionYpMcastCosQNew:
            PolicyCheck(unit, mem, Y_COS_INT_PRIf, (arg << 4));
            return (BCM_E_NONE);

        case bcmFieldActionRpMcastCosQNew:
            PolicyCheck(unit, mem, R_COS_INT_PRIf, (arg << 4));
            return (BCM_E_NONE);

        case bcmFieldActionUcastQueueNew:
            rv = _bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                    _BCM_TR3_COSQ_INDEX_STYLE_BUCKET, NULL,
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
            rv = _bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                    _BCM_TR3_COSQ_INDEX_STYLE_BUCKET, NULL,
                    &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }

            PolicyCheck(unit, mem, R_COS_INT_PRIf, cosq_new);
            return (BCM_E_NONE);
            break;

        case bcmFieldActionYpUcastQueueNew:
            rv = _bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                    _BCM_TR3_COSQ_INDEX_STYLE_BUCKET, NULL,
                    &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }

            PolicyCheck(unit, mem, Y_COS_INT_PRIf, cosq_new);
            return (BCM_E_NONE);
            break;

        case bcmFieldActionGpUcastQueueNew:
            rv = _bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                    _BCM_TR3_COSQ_INDEX_STYLE_BUCKET, NULL,
                    &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }

            PolicyCheck(unit, mem, G_COS_INT_PRIf, cosq_new);
            return (BCM_E_NONE);
            break;

        case bcmFieldActionIntCosUcastQueueNew:
            rv = _bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                    _BCM_TR3_COSQ_INDEX_STYLE_BUCKET, NULL,
                    &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }

            PolicyCheck(unit, mem, R_COS_INT_PRIf, cosq_new);
            PolicyCheck(unit, mem, Y_COS_INT_PRIf, cosq_new);
            PolicyCheck(unit, mem, G_COS_INT_PRIf, cosq_new);
            return (BCM_E_NONE);
            break;

        case bcmFieldActionRpIntCosUcastQueueNew:
            rv = _bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                    _BCM_TR3_COSQ_INDEX_STYLE_BUCKET, NULL,
                    &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }

            PolicyCheck(unit, mem, R_COS_INT_PRIf, cosq_new);
            return (BCM_E_NONE);
            break;

        case bcmFieldActionYpIntCosUcastQueueNew:
            rv = _bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                    _BCM_TR3_COSQ_INDEX_STYLE_BUCKET, NULL,
                    &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }

            PolicyCheck(unit, mem, Y_COS_INT_PRIf, cosq_new);
            return (BCM_E_NONE);
            break;

        case bcmFieldActionGpIntCosUcastQueueNew:
            rv = _bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                    _BCM_TR3_COSQ_INDEX_STYLE_BUCKET, NULL,
                    &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }

            PolicyCheck(unit, mem, G_COS_INT_PRIf, cosq_new);
            return (BCM_E_NONE);
            break;

        case bcmFieldActionEtagNew:
            PolicyCheck(unit, EGR_L3_NEXT_HOPm, L2_ACTIONS__VNTAGf, fa->param[0]);
            return (BCM_E_NONE);
        default:
            ;
        }
    }

    return (_bcm_field_trx_action_params_check(unit, f_ent, fa));
}


int
_bcm_field_tr3_group_default_aset_set(int unit, _field_group_t *fg)
{
    switch (fg->stage_id) {
    case _BCM_FIELD_STAGE_EXTERNAL:
        return (BCM_E_NONE);    /* Default aset is {} */
    default:
        ;
    }

    /* All other stages get normal (all supported actions) aset */

    return (_bcm_field_group_default_aset_set(unit, fg));
}


/* Table to map API actions to bit positions in ESM_ACL_ACTION_CONTROL word */

static const struct _field_tr3_external_actions_to_esm_acl_action_control_mapping {
    bcm_field_action_t action;  /* API action */
    soc_field_t        field;   /* Field in ESM_ACL_ACTION_CONTROL word */
    uint8              bit_num; /* Bit number in above field */
    uint8              npaw;    /* Width (bits) in NON_PROFILED_ACTIONS field */
} _field_tr3_external_actions_to_esm_acl_action_control_mapping_tbl[] = {
    { bcmFieldActionCnmCancel,              MISC1_SET_ENABLEf,    0,  1 },
    { bcmFieldActionDoNotChangeTtl,         MISC1_SET_ENABLEf,    1,  1 },
    { bcmFieldActionDoNotCheckUrpf,         MISC1_SET_ENABLEf,    2,  1 },
    { bcmFieldActionDynamicHgTrunkCancel,   MISC1_SET_ENABLEf,    3,  1 },
    { bcmFieldActionTrunkLoadBalanceCancel, MISC1_SET_ENABLEf,    4,  1 },
    { bcmFieldActionEcmpLoadBalanceCancel,  MISC1_SET_ENABLEf,    5,  1 },
    { bcmFieldActionColorIndependent,       MISC1_SET_ENABLEf,    6,  1 },
    { bcmFieldActionMirrorOverride,         MISC1_SET_ENABLEf,    7,  1 },
    { bcmFieldActionIpFix,                  MISC2_SET_ENABLEf,    0,  2 },
    { bcmFieldActionIpFixCancel,            MISC2_SET_ENABLEf,    0,  2 },
    { bcmFieldActionRpDropPrecedence,       MISC2_SET_ENABLEf,    1,  2 },
    { bcmFieldActionYpDropPrecedence,       MISC2_SET_ENABLEf,    2,  2 },
    { bcmFieldActionGpDropPrecedence,       MISC2_SET_ENABLEf,    3,  2 },
    { bcmFieldActionDropPrecedence,         MISC2_SET_ENABLEf,    1,  2 },
    { bcmFieldActionDropPrecedence,         MISC2_SET_ENABLEf,    2,  2 },
    { bcmFieldActionDropPrecedence,         MISC2_SET_ENABLEf,    3,  2 },
    { bcmFieldActionRpDrop,                 MISC2_SET_ENABLEf,    4,  2 },
    { bcmFieldActionYpDrop,                 MISC2_SET_ENABLEf,    5,  2 },
    { bcmFieldActionGpDrop,                 MISC2_SET_ENABLEf,    6,  2 },
    { bcmFieldActionDrop,                   MISC2_SET_ENABLEf,    4,  2 },
    { bcmFieldActionDrop,                   MISC2_SET_ENABLEf,    5,  2 },
    { bcmFieldActionDrop,                   MISC2_SET_ENABLEf,    6,  2 },
    { bcmFieldActionOffloadRedirect,        GOA_SET_ENABLEf,      0,  7 },
    { bcmFieldActionOffloadRedirect,        REDIRECT_SET_ENABLEf, 0, 24 },
    { bcmFieldActionCosQCpuNew,             CPU_COS_SET_ENABLEf,  0,  8 },
    { bcmFieldActionMirrorEgress,           MIRROR_SET_ENABLEf,   0, 12 },
    { bcmFieldActionMirrorIngress,          MIRROR_SET_ENABLEf,   0, 12 },
    { bcmFieldActionCopyToCpu,              COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionRpCopyToCpu,            COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionYpCopyToCpu,            COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionGpCopyToCpu,            COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionCopyToCpuCancel,        COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionRpCopyToCpuCancel,      COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionYpCopyToCpuCancel,      COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionGpCopyToCpuCancel,      COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionSwitchToCpuCancel,      COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionRpSwitchToCpuCancel,    COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionYpSwitchToCpuCancel,    COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionGpSwitchToCpuCancel,    COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionSwitchToCpuReinstate,   COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionRpSwitchToCpuReinstate, COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionYpSwitchToCpuReinstate, COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionGpSwitchToCpuReinstate, COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionTimeStampToCpu,         COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionTimeStampToCpu,         MISC2_SET_ENABLEf,       4,  2 },
    { bcmFieldActionTimeStampToCpu,         MISC2_SET_ENABLEf,       5,  2 },
    { bcmFieldActionTimeStampToCpu,         MISC2_SET_ENABLEf,       6,  2 },
    { bcmFieldActionTimeStampToCpuCancel,   COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionTimeStampToCpuCancel,   MISC2_SET_ENABLEf,       4,  2 },
    { bcmFieldActionTimeStampToCpuCancel,   MISC2_SET_ENABLEf,       5,  2 },
    { bcmFieldActionTimeStampToCpuCancel,   MISC2_SET_ENABLEf,       6,  2 },
    { bcmFieldActionRpTimeStampToCpu,       COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionRpTimeStampToCpu,       MISC2_SET_ENABLEf,       4,  2 },
    { bcmFieldActionYpTimeStampToCpu,       COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionYpTimeStampToCpu,       MISC2_SET_ENABLEf,       5,  2 },
    { bcmFieldActionGpTimeStampToCpu,       COPY_TO_CPU_SET_ENABLEf, 0, 18 },
    { bcmFieldActionGpTimeStampToCpu,       MISC2_SET_ENABLEf,       6,  2 },
    /* Pseudo-actions (i.e. included in aset, but never added to entry as actual
       actions; to announce intention to use resources by group
    */
    { bcmFieldActionPolicerLevel0,          METER_SET_ENABLEf,        0, 19 },
    { bcmFieldActionPolicerLevel1,          SHARED_METER_SET_ENABLEf, 0, 18 },
    { bcmFieldActionPolicerGroup,           SVC_METER_SET_ENABLEf,    0, 19 },
    { bcmFieldActionStatGroup,              COUNTER_SET_ENABLEf,      0, 20 }
};


STATIC int
_field_tr3_external_aset_install(int unit, _field_group_t *fg)
{
    const unsigned slice_num = fg->slices->slice_number;

    const struct _field_tr3_external_actions_to_esm_acl_action_control_mapping *p;
    esm_acl_action_control_entry_t esm_acl_action_control_buf;
    unsigned                       non_profiled_actions_width = 0, n;
    soc_mem_t                      policy_mem;

    sal_memset(esm_acl_action_control_buf.entry_data, 0, sizeof(esm_acl_action_control_buf.entry_data));

#define ACTION_CONTROL_ADD(fld, b, npaw)                       \
    {                                                          \
        uint32 __temp[2];                                      \
                                                               \
        __temp[0] = soc_mem_field32_get(unit,                           \
                                        ESM_ACL_ACTION_CONTROLm,        \
                                        esm_acl_action_control_buf.entry_data, \
                                        (fld)                           \
                                        );                              \
        __temp[1] = __temp[0] | BIT(b);                                 \
        if (__temp[1] != __temp[0]) {                                   \
            soc_mem_field32_set(unit,                                   \
                                ESM_ACL_ACTION_CONTROLm,                \
                                esm_acl_action_control_buf.entry_data,  \
                                (fld),                                  \
                                __temp[1]                               \
                                );                                      \
            non_profiled_actions_width += (npaw);                       \
        }                                                               \
    }

    /* Map given actions to bits in ESM_ACL_ACTION_CONTROL */

    for (p = _field_tr3_external_actions_to_esm_acl_action_control_mapping_tbl,
             n = COUNTOF(_field_tr3_external_actions_to_esm_acl_action_control_mapping_tbl);
         n;
         --n, ++p
         ) {
        if (!SHR_BITGET(fg->aset.w, p->action)) {
            continue;
        }

        ACTION_CONTROL_ADD(p->field, p->bit_num, p->npaw);
    }

    /* Check that policy table is wide enough to contain all requested actions */

    BCM_IF_ERROR_RETURN(_field_tr3_external_policy_mem(unit,
                                                       slice_num,
                                                       &policy_mem
                                                       )
                        );

    if (non_profiled_actions_width
        > soc_mem_field_length(unit,
                               policy_mem,
                               NON_PROFILED_ACTIONSf
                               )
        ) {
        return (BCM_E_CONFIG);
    }

    return (soc_mem_write(unit,
                          ESM_ACL_ACTION_CONTROLm,
                          MEM_BLOCK_ALL,
                          slice_num,
                          esm_acl_action_control_buf.entry_data
                          )
            );

#undef ACTION_CONTROL_MOD
}


int
_bcm_field_tr3_aset_install(int unit, _field_group_t *fg)
{
    switch (fg->stage_id) {
    case _BCM_FIELD_STAGE_EXTERNAL:
        return (_field_tr3_external_aset_install(unit, fg));
    default:
        ;
    }

    return (BCM_E_NONE);
}


STATIC int
_field_tr3_egress_key_match_type_set (int unit, _field_entry_t *f_ent)
{
    _bcm_field_qual_offset_t q_offset;
    const _field_group_t *fg = f_ent->group;
    uint32 data, mask;

    sal_memset(&q_offset, 0, sizeof(q_offset));
    q_offset.field = KEYf;
    q_offset.num_offsets = 1;
    q_offset.offset[0] = 231;
    q_offset.width[0] = 3;

    if (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
        switch (fg->sel_codes[0].fpf3) {
          case _BCM_FIELD_EFP_KEY1:
              data = _BCM_FIELD_EFP_KEY1_MATCH_TYPE;
              break;
          case _BCM_FIELD_EFP_KEY2:
              data = _BCM_FIELD_EFP_KEY2_MATCH_TYPE;
              break;
          case _BCM_FIELD_EFP_KEY4:
              data = _BCM_FIELD_EFP_KEY4_MATCH_TYPE;
              break;
          case _BCM_FIELD_EFP_KEY1_NO_V4:
              data = _BCM_FIELD_EFP_KEY1_MATCH_TYPE;
              break;
          default:
              return (BCM_E_INTERNAL);
        }
    } else {
        switch (fg->sel_codes[1].fpf3) {
          case _BCM_FIELD_EFP_KEY2:
              data = _BCM_FIELD_EFP_KEY2_KEY3_MATCH_TYPE;
              break;
          case _BCM_FIELD_EFP_KEY4:
              if ((_BCM_FIELD_EFP_KEY1 == fg->sel_codes[0].fpf3) ||
                  (_BCM_FIELD_EFP_KEY1_NO_V4 == fg->sel_codes[0].fpf3)) {
                  data = _BCM_FIELD_EFP_KEY1_KEY4_MATCH_TYPE;
              } else {
                  data = _BCM_FIELD_EFP_KEY2_KEY4_MATCH_TYPE;
              }
              break;
          default:
              return (BCM_E_INTERNAL);
        }
    }

    mask = 0x7;

    /* coverity[callee_ptr_arith : FALSE] */
    return (_bcm_field_qual_value_set(unit,
                                      (_bcm_field_qual_offset_t *) &q_offset,
                                      f_ent,
                                      &data,
                                      &mask
                                      )
            );
}


STATIC int
_field_tr3_group_install(int unit, _field_group_t *fg)
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
                                                          &parts_count
                                                          )
                        );

    for (idx = 0; idx < parts_count; ++idx) {
        BCM_IF_ERROR_RETURN(_bcm_field_tcam_part_to_slice_number(unit, idx,
                                                                 fg,
                                                                 &slice_number
                                                                 )
                            );
        fs = fg->slices + slice_number;

        BCM_IF_ERROR_RETURN(_field_tr3_selcodes_install(unit,
                                                        fg,
                                                        fs->slice_number,
                                                        fg->pbmp,
                                                        idx
                                                        )
                            );
    }

    return (BCM_E_NONE);
}


int
_bcm_field_tr3_qualify_LoopbackType(bcm_field_LoopbackType_t loopback_type,
                                    uint32                   *tcam_data,
                                    uint32                   *tcam_mask
                                    )
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
        case bcmFieldLoopbackTypeMirror:
            *tcam_data = 0x13;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldLoopbackTypeRedirect:
            *tcam_data = 0x14;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldLoopbackTypeRegexMatch:
            *tcam_data = 0x15;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldLoopbackTypeMplsP2mp:
            *tcam_data = 0x16;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldLoopbackTypeQcn:
            *tcam_data = 0x17;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldLoopbackTypeWlanEncap:
            *tcam_data = 0x18;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldLoopbackTypeWlanEncapEncrypt:
            *tcam_data = 0x19;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldLoopbackTypeWlanDecap:
            *tcam_data = 0x1a;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldLoopbackTypePassThru:
            *tcam_data = 0x1c;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldLoopbackTypeMplsExtendedLookup:
            *tcam_data = 0x1d;
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
_bcm_field_tr3_qualify_LoopbackType_get(uint8                    tcam_data,
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
        case 0x13:
            *loopback_type = bcmFieldLoopbackTypeMirror;
            break;
        case 0x14:
            *loopback_type = bcmFieldLoopbackTypeRedirect;
            break;
        case 0x15:
            *loopback_type = bcmFieldLoopbackTypeRegexMatch;
            break;
        case 0x16:
            *loopback_type = bcmFieldLoopbackTypeMplsP2mp;
            break;
        case 0x17:
            *loopback_type = bcmFieldLoopbackTypeQcn;
            break;
        case 0x18:
            *loopback_type = bcmFieldLoopbackTypeWlanEncap;
            break;
        case 0x19:
            *loopback_type = bcmFieldLoopbackTypeWlanEncapEncrypt;
            break;
        case 0x1a:
            *loopback_type = bcmFieldLoopbackTypeWlanDecap;
            break;
        case 0x1c:
            *loopback_type = bcmFieldLoopbackTypePassThru;
            break;
        case 0x1d:
            *loopback_type = bcmFieldLoopbackTypeMplsExtendedLookup;
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
_bcm_field_tr3_qualify_TunnelType(bcm_field_TunnelType_t tunnel_type,
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
        case bcmFieldTunnelTypeWlanWtpToAc:
            *tcam_data = 0x4;
            *tcam_mask = 0x1f;
            break;
        case bcmFieldTunnelTypeWlanAcToAc:
            *tcam_data = 0x5;
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
        default:
            return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}


int
_bcm_field_tr3_qualify_TunnelType_get(uint8                  tcam_data,
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
        case 0x4:
            *tunnel_type = bcmFieldTunnelTypeWlanWtpToAc;
            break;
        case 0x5:
            *tunnel_type = bcmFieldTunnelTypeWlanAcToAc;
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
    {  BCM_FIELD_PKT_RES_UNKNOWN,      0x00, 0x3f },
    {  BCM_FIELD_PKT_RES_CONTROL,      0x01, 0x3f },
    {  BCM_FIELD_PKT_RES_BPDU,         0x04, 0x3f },
    {  BCM_FIELD_PKT_RES_L2BC,         0x0c, 0x3f },
    {  BCM_FIELD_PKT_RES_L2UC,         0x08, 0x3f },
    {  BCM_FIELD_PKT_RES_L2UNKNOWN,    0x09, 0x3f },
    {  BCM_FIELD_PKT_RES_L3MCUNKNOWN,  0x13, 0x3f },
    {  BCM_FIELD_PKT_RES_L3MCKNOWN,    0x12, 0x3f },
    {  BCM_FIELD_PKT_RES_L2MCKNOWN,    0x0a, 0x3f },
    {  BCM_FIELD_PKT_RES_L2MCUNKNOWN,  0x0b, 0x3f },
    {  BCM_FIELD_PKT_RES_L3UCKNOWN,    0x10, 0x3f },
    {  BCM_FIELD_PKT_RES_L3UCUNKNOWN,  0x11, 0x3f },
    {  BCM_FIELD_PKT_RES_MPLSKNOWN,    0x1c, 0x3f },
    {  BCM_FIELD_PKT_RES_MPLSMCKNOWN,  0x1d, 0x3f },
    {  BCM_FIELD_PKT_RES_MPLSL3KNOWN,  0x1a, 0x3f },
    {  BCM_FIELD_PKT_RES_MPLSL2KNOWN,  0x18, 0x3f },
    {  BCM_FIELD_PKT_RES_MPLSUNKNOWN,  0x19, 0x3f },
    {  BCM_FIELD_PKT_RES_MIMKNOWN,     0x20, 0x3f },
    {  BCM_FIELD_PKT_RES_MIMUNKNOWN,   0x21, 0x3f },
    {  BCM_FIELD_PKT_RES_TRILLKNOWN,   0x28, 0x3f },
    {  BCM_FIELD_PKT_RES_TRILLUNKNOWN, 0x29, 0x3f },
    {  BCM_FIELD_PKT_RES_NIVKNOWN,     0x30, 0x3f },
    {  BCM_FIELD_PKT_RES_NIVUNKNOWN,   0x31, 0x3f },
    {  BCM_FIELD_PKT_RES_OAM,          0x02, 0x3f },
    {  BCM_FIELD_PKT_RES_BFD,          0x03, 0x3f },
    {  BCM_FIELD_PKT_RES_ICNM,         0x05, 0x3f },
    {  BCM_FIELD_PKT_RES_IEEE1588,     0x06, 0x3f },
    {  BCM_FIELD_PKT_RES_L2_ANY,       0x08, 0x38 },
    {  BCM_FIELD_PKT_RES_L2MC_ANY,     0x0a, 0x3e },
    {  BCM_FIELD_PKT_RES_TRILL_ANY,    0x28, 0x3e },
    {  BCM_FIELD_PKT_RES_NIV_ANY,      0x30, 0x3e },
    {  BCM_FIELD_PKT_RES_MPLS_ANY,     0x18, 0x38 },
    {  BCM_FIELD_PKT_RES_MIM_ANY,      0x20, 0x3e },
    {  BCM_FIELD_PKT_RES_L3_ANY,       0x10, 0x3e }
};

int
_bcm_field_tr3_qualify_PacketRes(int               unit,
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
        return  (BCM_E_NONE);
    } else {
        return (BCM_E_INTERNAL);
    }
}

int
_bcm_field_tr3_qualify_PacketRes_get(int               unit,
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
            if ( pkt_res_xlate_tbl[i].hw_mask == 0x3f ) {
                if ( (*mask == pkt_res_xlate_tbl[i].hw_data) ||
                     (*mask == 0x3f) ) {
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
        return  (BCM_E_NONE);
    } else {
        return (BCM_E_INTERNAL);
    }
}

int
_bcm_field_tr3_qualify_class(int                 unit,
                             bcm_field_entry_t   entry,
                             bcm_field_qualify_t qual,
                             uint32              *data,
                             uint32              *mask
                             )
{
    _field_entry_t *f_ent;
    unsigned       cl_width, cl_max;

    /* Get field entry part that contains the qualifier. */
    BCM_IF_ERROR_RETURN(_bcm_field_entry_qual_get(unit, entry, qual, &f_ent));

    switch (f_ent->group->stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
    case _BCM_FIELD_STAGE_EXTERNAL:
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
            cl_width = 12;
            break;
        default:
            return (BCM_E_INTERNAL);
        }

        cl_max = 1 << cl_width;

        if (*data >= cl_max || (*mask != BCM_FIELD_EXACT_MATCH_MASK && *mask >= cl_max)) {
            return (BCM_E_PARAM);
        }

        break;

    case _BCM_FIELD_STAGE_LOOKUP:
        switch (qual) {
            case bcmFieldQualifyInterfaceClassPort:
                cl_width = 8;
                break;
            case bcmFieldQualifyInterfaceClassL3:
                cl_width = 12;
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
        {
            uint32         ifp_cl_type;

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
        }
        break;

        default:
        return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}


int
_bcm_field_tr3_qualify_class_get(int                 unit,
                                 bcm_field_entry_t   entry,
                                 bcm_field_qualify_t qual,
                                 uint32              *data,
                                 uint32              *mask
                                 )
{
    _field_entry_t *f_ent;

    /* Get field entry part that contains the qualifier. */
    BCM_IF_ERROR_RETURN(_bcm_field_entry_qual_get(unit, entry, qual, &f_ent));

    if (f_ent->group->stage_id == _BCM_FIELD_STAGE_EGRESS) {
        /* Mask off IFP_CLASSID_TYPE */

        const uint32 m = (1 << 9) - 1;

        *data &= m;
        *mask &= m;
    }

    return (BCM_E_NONE);
}


#define IS_FLOW_MODE(f_pl)                       \
    ((f_pl)->cfg.mode == bcmPolicerModeCommitted \
     || (f_pl)->cfg.mode == bcmPolicerModePeak   \
     )
#define IS_MOD_TRTCM_MODE(f_pl)                                 \
    ((f_pl)->cfg.mode == bcmPolicerModeTrTcmDs                  \
     || (f_pl)->cfg.mode == bcmPolicerModeCoupledTrTcmDs        \
     )

STATIC int
_field_tr3_ingress_policers_compat(int              unit,
                                   _field_entry_t   *f_ent,
                                   unsigned         lvl,
                                   _field_policer_t *f_pl
                                   )
{
    const unsigned olvl = 1 - lvl;
    _field_policer_t *_f_pl[2] = { 0, 0 };

    _f_pl[lvl] = f_pl;

    if (!(f_ent->policer[olvl].flags & _FP_POLICER_VALID)) {
        return (BCM_E_NONE);
    }

    BCM_IF_ERROR_RETURN(_bcm_field_policer_get(unit,
                                               f_ent->policer[olvl].pid,
                                               &_f_pl[olvl]
                                               )
                        );

    switch (_f_pl[1]->cfg.flags & BCM_POLICER_COLOR_MERGE_MASK) {
    case BCM_POLICER_COLOR_MERGE_AND:
    case BCM_POLICER_COLOR_MERGE_OR:
        /* Level 1 sharing mode is SINGLE_AND or SINGLE_OR
           => level 0 and level 1 must both be in flow or modified
           trTCM mode
        */

        if ((IS_FLOW_MODE(_f_pl[0]) && IS_FLOW_MODE(_f_pl[1]))
            || (IS_MOD_TRTCM_MODE(_f_pl[0]) && IS_MOD_TRTCM_MODE(_f_pl[1]))
            ) {
            break;
        }
        return (BCM_E_PARAM);

    case BCM_POLICER_COLOR_MERGE_DUAL:
        /* Level 1 sharing mode is DUAL
           => level 0 must be modified trTCM
        */

        if (IS_MOD_TRTCM_MODE(_f_pl[0])) {
            break;
        }
        return (BCM_E_PARAM);

    default:
        return (BCM_E_PARAM);
    }

    return (BCM_E_NONE);
}


STATIC int
_field_tr3_ingress_policer_mode_support(int              unit,
                                        _field_entry_t   *f_ent,
                                        int              lvl,
                                        _field_policer_t *f_pl
                                        )
{
    switch (f_pl->cfg.mode) {
    case bcmPolicerModeGreen:
    case bcmPolicerModePassThrough:
    case bcmPolicerModeCommitted:
    case bcmPolicerModeTrTcmDs:
    case bcmPolicerModeCoupledTrTcmDs:
        break;
    case bcmPolicerModeSrTcm:
    case bcmPolicerModeSrTcmModified:
    case bcmPolicerModeTrTcm:
        if (lvl == 1) {
            return (BCM_E_PARAM);
        }
        break;
    default:
        return (BCM_E_PARAM);
    }

    return (_field_tr3_ingress_policers_compat(unit, f_ent, lvl, f_pl));
}


int
_bcm_field_tr3_policer_mode_support(int            unit,
                                    _field_entry_t *f_ent,
                                    int            lvl,
                                    _field_policer_t *f_pl
                                    )
{
    int (*func)(int, _field_entry_t *, int, _field_policer_t *);

    switch (f_ent->group->stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
        func = _field_tr3_ingress_policer_mode_support;
        break;
    default:
        func = _bcm_field_policer_mode_support;
    }

    return ((*func)(unit, f_ent, lvl, f_pl));
}


STATIC int
_field_tr3_ingress_policer_action_set(int            unit,
                                      _field_entry_t *f_ent,
                                      uint32         *policy_buf
                                      )
{
    _field_entry_policer_t *f_ent_pl;
    unsigned               api_mode, api_flags, pool_idx, hw_idx;
    uint8                  hw_mode, modifier, sh_mode;
    _field_policer_t       *f_pl = NULL;

    /* Level 0 */

    if ((f_ent_pl = &f_ent->policer[0])->flags & _FP_POLICER_INSTALLED) {
        BCM_IF_ERROR_RETURN(_bcm_field_policer_get(unit, f_ent_pl->pid, &f_pl));
        api_mode  = f_pl->cfg.mode;
        api_flags = f_pl->cfg.flags;
        pool_idx  = f_pl->pool_index;
        hw_idx    = f_pl->hw_index;
    } else {
        api_mode = bcmPolicerModePassThrough;
        api_flags = pool_idx = hw_idx = 0;
    }

    switch (api_mode) {
    case bcmPolicerModeGreen:
    case bcmPolicerModePassThrough:
        hw_mode = 0;
        modifier = (api_mode == bcmPolicerModePassThrough) ? 1 : 0;
        break;
    case bcmPolicerModeCommitted:
        hw_mode  = 1;
        modifier = (_FP_POLICER_EXCESS_HW_METER(f_pl)) ? 0 : 1;
        break;
    case bcmPolicerModeSrTcm:
    case bcmPolicerModeSrTcmModified:
        hw_mode  = (api_flags & BCM_POLICER_COLOR_BLIND) ? 6 : 7;
        modifier = (api_mode == bcmPolicerModeSrTcmModified) ? 1 : 0;
        break;
    case bcmPolicerModeTrTcm:
        hw_mode = (api_flags & BCM_POLICER_COLOR_BLIND) ? 2 : 3;
        modifier = 0;
        break;
    case bcmPolicerModeTrTcmDs:
    case bcmPolicerModeCoupledTrTcmDs:
        hw_mode = (api_flags & BCM_POLICER_COLOR_BLIND) ? 4 : 5;
        modifier = 0;
        break;
    default:
        return (BCM_E_INTERNAL);
    }

    soc_mem_field32_set(unit,
                        FP_POLICY_TABLEm,
                        policy_buf,
                        METER_PAIR_POOL_NUMBERf,
                        pool_idx
                        );
    soc_mem_field32_set(unit,
                        FP_POLICY_TABLEm,
                        policy_buf,
                        METER_PAIR_POOL_INDEXf,
                        hw_idx
                        );
    soc_mem_field32_set(unit,
                        FP_POLICY_TABLEm,
                        policy_buf,
                        METER_PAIR_MODEf,
                        hw_mode
                        );
    soc_mem_field32_set(unit,
                        FP_POLICY_TABLEm,
                        policy_buf,
                        METER_PAIR_MODE_MODIFIERf,
                        modifier
                        );

    /* Level 1 */

    if ((f_ent_pl = &f_ent->policer[1])->flags & _FP_POLICER_INSTALLED) {
        BCM_IF_ERROR_RETURN(_bcm_field_policer_get(unit, f_ent_pl->pid, &f_pl));
        api_mode  = f_pl->cfg.mode;
        api_flags = f_pl->cfg.flags;
        pool_idx  = f_pl->pool_index;
        hw_idx    = f_pl->hw_index;
    } else {
        api_mode = bcmPolicerModePassThrough;
        api_flags = pool_idx = hw_idx = 0;
    }

    if (api_mode == bcmPolicerModePassThrough) {
        hw_mode = sh_mode = modifier = 0;
    } else {
        switch (api_mode) {
        case bcmPolicerModeCommitted:
            hw_mode = 0;
            modifier = (_FP_POLICER_EXCESS_HW_METER(f_pl)) ? 0 : 1;
            break;
        case bcmPolicerModeTrTcmDs:
        case bcmPolicerModeCoupledTrTcmDs:
            hw_mode = 1;
            modifier = (api_flags & BCM_POLICER_COLOR_BLIND) ? 0 : 1;
            break;
        default:
            return (BCM_E_INTERNAL);
        }

        switch (api_flags & BCM_POLICER_COLOR_MERGE_MASK) {
        case BCM_POLICER_COLOR_MERGE_AND:
            sh_mode = 2;
            break;
        case BCM_POLICER_COLOR_MERGE_OR:
            sh_mode = 1;
            break;
        case BCM_POLICER_COLOR_MERGE_DUAL:
            sh_mode = 3;
            break;
        default:
            return (BCM_E_INTERNAL);
        }
    }

    soc_mem_field32_set(unit,
                        FP_POLICY_TABLEm,
                        policy_buf,
                        SHARED_METER_PAIR_POOL_NUMBERf,
                        pool_idx
                        );
    soc_mem_field32_set(unit,
                        FP_POLICY_TABLEm,
                        policy_buf,
                        SHARED_METER_PAIR_POOL_INDEXf,
                        hw_idx
                        );
    soc_mem_field32_set(unit,
                        FP_POLICY_TABLEm,
                        policy_buf,
                        SHARED_METER_PAIR_MODEf,
                        hw_mode
                        );
    soc_mem_field32_set(unit,
                        FP_POLICY_TABLEm,
                        policy_buf,
                        METER_SHARING_MODEf,
                        sh_mode
                        );
    soc_mem_field32_set(unit,
                        FP_POLICY_TABLEm,
                        policy_buf,
                        METER_SHARING_MODE_MODIFIERf,
                        modifier
                        );

    return (BCM_E_NONE);
}


int
_bcm_field_tr3_policer_action_set(int            unit,
                                  _field_entry_t *f_ent,
                                  soc_mem_t      mem,
                                  uint32         *policy_buf
                                  )
{
    switch (f_ent->group->stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
        return (_field_tr3_ingress_policer_action_set(unit, f_ent, policy_buf));
    default:
        ;
    }

    return (_bcm_field_trx_policer_action_set(unit, f_ent, mem, policy_buf));
}


struct cntr_hw_mode {
    uint32 hw_bmap;
    uint8  hw_entry_count;
};

static const struct cntr_hw_mode ingress_cntr_hw_mode_tbl[] = {
    /* Hardware mode 0
       (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 0)
    */
    { 0, 0 },
    /* Hardware mode 1
       (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 1)
    */
    { BIT(bcmFieldStatGreenPackets) | BIT(bcmFieldStatGreenBytes), 1 },
    /* Hardware mode 2
       (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 2)
    */
    { 0, 0 },               /* Logically equivalent to mode 1 */
    /* Hardware mode 3
       (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 3)
    */
    { 0, 0 },               /* Logically equivalent to mode 1 */
    /* Hardware mode 4
       (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 0)
    */
    { BIT(bcmFieldStatYellowPackets) | BIT(bcmFieldStatYellowBytes), 1 },
    /* Hardware mode 5
       (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 1)
    */
    { BIT(bcmFieldStatNotRedPackets) | BIT(bcmFieldStatNotRedBytes), 1 },
    /* Hardware mode 6
       (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 2)
    */
    { BIT(bcmFieldStatYellowPackets) | BIT(bcmFieldStatYellowBytes)
      | BIT(bcmFieldStatGreenPackets) | BIT(bcmFieldStatGreenBytes)
      | BIT(bcmFieldStatNotRedPackets) | BIT(bcmFieldStatNotRedBytes),
      2
    },
    /* Hardware mode 7
       (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 3)
    */
    { 0, 0 },               /* Logically equivalent to mode 6 */
    /* Hardware mode 8
       (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 0)
    */
    { 0, 0 },               /* Logically equivalent to mode 4 */
    /* Hardware mode 9
       (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 1)
    */
    { 0, 0 },               /* Logically equivalent to mode 6 */
    /* Hardware mode 10
       (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 2)
    */
    { 0, 0 },               /* Logically equivalent to mode 5 */
    /* Hardware mode 11
       (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 3)
    */
    { 0, 0 },               /* Logically equivalent to mode 5 */
    /* Hardware mode 12
       (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 3, G_COUNTER_OFS = 0)
    */
    { 0, 0 },               /* Logically equivalent to mode 4 */
    /* Hardware mode 13
       (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 3, G_COUNTER_OFS = 1)
    */
    { 0, 0 },               /* Logically equivalent to mode 6 */
    /* Hardware mode 14
       (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 3, G_COUNTER_OFS = 2)
    */
    { 0, 0 },               /* Logically equivalent to mode 6 */
    /* Hardware mode 15
       (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 3, G_COUNTER_OFS = 3)
    */
    { 0, 0 },               /* Logically equivalent to mode 5 */
    /* Hardware mode 16
       (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 0)
    */
    { BIT(bcmFieldStatRedPackets) | BIT(bcmFieldStatRedBytes), 1 },
    /* Hardware mode 17
       (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 1)
    */
    { BIT(bcmFieldStatNotYellowPackets) | BIT(bcmFieldStatNotYellowBytes), 1 },
    /* Hardware mode 18
       (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 2)
    */
    { BIT(bcmFieldStatRedPackets) | BIT(bcmFieldStatRedBytes)
      | BIT(bcmFieldStatGreenPackets) | BIT(bcmFieldStatGreenBytes)
      | BIT(bcmFieldStatNotYellowPackets) | BIT(bcmFieldStatNotYellowBytes),
      2
    },
    /* Hardware mode 19
       (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 3)
    */
    { 0, 0 },               /* Logically equivalent to mode 18 */
    /* Hardware mode 20
       (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 0)
    */
    { BIT(bcmFieldStatNotGreenPackets) | BIT(bcmFieldStatNotGreenBytes), 1 },
    /* Hardware mode 21
       (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 1)
    */
    { BIT(bcmFieldStatPackets) | BIT(bcmFieldStatBytes), 1 },
    /* Hardware mode 22
       (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 2)
    */
    { BIT(bcmFieldStatNotGreenPackets) | BIT(bcmFieldStatNotGreenBytes)
      | BIT(bcmFieldStatGreenPackets) | BIT(bcmFieldStatGreenBytes)
      | BIT(bcmFieldStatPackets) | BIT(bcmFieldStatBytes),
      2
    },
    /* Hardware mode 23
       (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 3)
    */
    { 0, 0 },               /* Logically equivalent to mode 22 */
    /* Hardware mode 24
       (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 0)
    */
    { BIT(bcmFieldStatRedPackets) | BIT(bcmFieldStatRedBytes)
      | BIT(bcmFieldStatYellowPackets) | BIT(bcmFieldStatYellowBytes)
      | BIT(bcmFieldStatNotGreenPackets) | BIT(bcmFieldStatNotGreenBytes),
      2
    },
    /* Hardware mode 25
       (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 1)
    */
    { BIT(bcmFieldStatNotYellowPackets) | BIT(bcmFieldStatNotYellowBytes)
      | BIT(bcmFieldStatYellowPackets) | BIT(bcmFieldStatYellowBytes)
      | BIT(bcmFieldStatPackets) | BIT(bcmFieldStatBytes),
      2
    },
    /* Hardware mode 26
       (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 2)
    */
    { BIT(bcmFieldStatRedPackets) | BIT(bcmFieldStatRedBytes)
      | BIT(bcmFieldStatNotRedPackets) | BIT(bcmFieldStatNotRedBytes)
      | BIT(bcmFieldStatPackets) | BIT(bcmFieldStatBytes),
      2
    },
    /* Hardware mode 27
       (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 3)
    */

    { BIT(bcmFieldStatRedPackets) | BIT(bcmFieldStatRedBytes)
      | BIT(bcmFieldStatYellowPackets) | BIT(bcmFieldStatYellowBytes)
      | BIT(bcmFieldStatGreenPackets) | BIT(bcmFieldStatGreenBytes)
      | BIT(bcmFieldStatNotRedPackets) | BIT(bcmFieldStatNotRedBytes)
      | BIT(bcmFieldStatNotYellowPackets) | BIT(bcmFieldStatNotYellowBytes)
      | BIT(bcmFieldStatNotGreenPackets) | BIT(bcmFieldStatNotGreenBytes)
      | BIT(bcmFieldStatPackets) | BIT(bcmFieldStatBytes),
      3
    }

    /* Everything higher is logically equivalent to a lower-numbered mode.
       Here they are, for completeness:

       Hardware mode 28
       (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 3, G_COUNTER_OFS = 0)

       { 0, 0 },                  Logically equivalent to mode 24
       Hardware mode 29
       (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 3, G_COUNTER_OFS = 1)

       { 0, 0 },                  Logically equivalent to mode 25
       Hardware mode 30
       (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 3, G_COUNTER_OFS = 2)

       { 0, 0 },                  Logically equivalent to mode 27
       Hardware mode 31
       (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 3, G_COUNTER_OFS = 3)

       { 0, 0 },                  Logically equivalent to mode 26
       Hardware mode 32
       (R_COUNTER_OFS = 2, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 0)

       { 0, 0 },                  Logically equivalent to mode 16
       Hardware mode 33
       (R_COUNTER_OFS = 2, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 1)

       { 0, 0 },                  Logically equivalent to mode 18
       Hardware mode 34
       (R_COUNTER_OFS = 2, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 2)

       { 0, 0 },                  Logically equivalent to mode 17
       Hardware mode 35
       (R_COUNTER_OFS = 2, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 3)

       { 0, 0 },                  Logically equivalent to mode 18
       Hardware mode 36
       (R_COUNTER_OFS = 2, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 0)

       { 0, 0 },                  Logically equivalent to mode 24
       Hardware mode 37
       (R_COUNTER_OFS = 2, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 1)

       { 0, 0 },                  Logically equivalent to mode 26
       Hardware mode 38
       (R_COUNTER_OFS = 2, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 2)

       { 0, 0 },                  Logically equivalent to mode 25
       Hardware mode 39
       (R_COUNTER_OFS = 2, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 3)

       { 0, 0 },                  Logically equivalent to mode 27
       Hardware mode 40
       (R_COUNTER_OFS = 2, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 0)

       { 0, 0 },                  Logically equivalent to mode 20
       Hardware mode 41
       (R_COUNTER_OFS = 2, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 1)

       { 0, 0 },                  Logically equivalent to mode 22
       Hardware mode 42
       (R_COUNTER_OFS = 2, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 2)

       { 0, 0 },                  Logically equivalent to mode 21
       Hardware mode 43
       (R_COUNTER_OFS = 2, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 3)

       { 0, 0 },                  Logically equivalent to mode 22
       Hardware mode 44
       (R_COUNTER_OFS = 2, Y_COUNTER_OFS = 3, G_COUNTER_OFS = 0)

       { 0, 0 },                  Logically equivalent to mode 24
       Hardware mode 45
       (R_COUNTER_OFS = 2, Y_COUNTER_OFS = 3, G_COUNTER_OFS = 1)

       { 0, 0 },                  Logically equivalent to mode 27
       Hardware mode 46
       (R_COUNTER_OFS = 2, Y_COUNTER_OFS = 3, G_COUNTER_OFS = 2)

       { 0, 0 },                  Logically equivalent to mode 25
       Hardware mode 47
       (R_COUNTER_OFS = 2, Y_COUNTER_OFS = 3, G_COUNTER_OFS = 3)

       { 0, 0 },                  Logically equivalent to mode 26
       Hardware mode 48
       (R_COUNTER_OFS = 3, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 0)

       { 0, 0 },                  Logically equivalent to mode 16
       Hardware mode 49
       (R_COUNTER_OFS = 3, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 1)

       { 0, 0 },                  Logically equivalent to mode 18
       Hardware mode 50
       (R_COUNTER_OFS = 3, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 2)

       { 0, 0 },                  Logically equivalent to mode 18
       Hardware mode 51
       (R_COUNTER_OFS = 3, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 3)

       { 0, 0 },                  Logically equivalent to mode 17
       Hardware mode 52
       (R_COUNTER_OFS = 3, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 0)

       { 0, 0 },                  Logically equivalent to mode 24
       Hardware mode 53
       (R_COUNTER_OFS = 3, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 1)

       { 0, 0 },                  Logically equivalent to mode 26
       Hardware mode 54
       (R_COUNTER_OFS = 3, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 2)

       { 0, 0 },                  Logically equivalent to mode 27
       Hardware mode 55
       (R_COUNTER_OFS = 3, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 3)

       { 0, 0 },                  Logically equivalent to mode 25
       Hardware mode 56
       (R_COUNTER_OFS = 3, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 0)

       { 0, 0 },                  Logically equivalent to mode 24
       Hardware mode 57
       (R_COUNTER_OFS = 3, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 1)

       { 0, 0 },                  Logically equivalent to mode 27
       Hardware mode 58
       (R_COUNTER_OFS = 3, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 2)

       { 0, 0 },                  Logically equivalent to mode 26
       Hardware mode 59
       (R_COUNTER_OFS = 3, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 3)

       { 0, 0 },                  Logically equivalent to mode 25
       Hardware mode 60
       (R_COUNTER_OFS = 3, Y_COUNTER_OFS = 3, G_COUNTER_OFS = 0)

       { 0, 0 },                  Logically equivalent to mode 20
       Hardware mode 61
       (R_COUNTER_OFS = 3, Y_COUNTER_OFS = 3, G_COUNTER_OFS = 1)

       { 0, 0 },                  Logically equivalent to mode 22
       Hardware mode 62
       (R_COUNTER_OFS = 3, Y_COUNTER_OFS = 3, G_COUNTER_OFS = 2)

       { 0, 0 },                  Logically equivalent to mode 22
       Hardware mode 63
       (R_COUNTER_OFS = 3, Y_COUNTER_OFS = 3, G_COUNTER_OFS = 3)

       { 0, 0 }                   Logically equivalent to mode 21
    */
};


int
_bcm_field_tr3_stat_hw_mode_max(_field_stage_id_t stage_id,
                                int               *hw_mode_max
                                )
{
    switch (stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
        *hw_mode_max = COUNTOF(ingress_cntr_hw_mode_tbl);
        break;
    default:
        *hw_mode_max = _FP_STAT_HW_MODE_MAX;
    }

    return (BCM_E_NONE);
}


STATIC int
_field_tr3_ingress_stat_hw_mode_to_bmap(int               unit,
                                        uint16             mode,
                                        uint32            *hw_bmap,
                                        uint8             *hw_entry_count
                                        )
{

    const struct cntr_hw_mode *p;

    if (mode >= COUNTOF(ingress_cntr_hw_mode_tbl)) {
        return (BCM_E_INTERNAL);
    }

    p = &ingress_cntr_hw_mode_tbl[mode];
    *hw_bmap        = p->hw_bmap;
    *hw_entry_count = p->hw_entry_count;

    return (BCM_E_NONE);
}


int
_bcm_field_tr3_stat_hw_mode_to_bmap(int               unit,
                                    uint16             mode,
                                    _field_stage_id_t stage_id,
                                    uint32            *hw_bmap,
                                    uint8             *hw_entry_count
                                    )
{
    switch (stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
        return (_field_tr3_ingress_stat_hw_mode_to_bmap(unit,
                                                        mode,
                                                        hw_bmap,
                                                        hw_entry_count
                                                        )
                );
    default:
        ;
    }

    return (_bcm_field_trx_stat_hw_mode_to_bmap(unit,
                                                mode,
                                                stage_id,
                                                hw_bmap,
                                                hw_entry_count
                                                )
            );
}


STATIC int
_field_tr3_ingress_counter_hw_alloc(int            unit,
                                    _field_entry_t *f_ent,
                                    _field_stat_t  *f_st
                                    )
{
    _field_stage_t     *stage_fc;
    _field_cntr_pool_t *p = NULL;
    _field_group_t     *fg = NULL;
    unsigned           n, hw_idx, i;
    int                idx, free_pool_idx;
    uint8              pool_idx=0;
    int                temp_idx=0;
    int                found =0;
    int                temp_hw_idx = 0;

    if ((NULL == f_st) || (NULL == f_ent)) {
        return BCM_E_INTERNAL;
    }

    fg = f_ent->group;

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                                                 f_ent->group->stage_id,
                                                 &stage_fc
                                                 ));

    /* Find required number of consecutive free counters in a pool */
    free_pool_idx = _FP_INVALID_INDEX;
    n = stage_fc->num_cntr_pools;
    for (idx = 0; idx < n; ++idx) {
        p = stage_fc->cntr_pool[idx];
        if (p == NULL) {
            return (BCM_E_INTERNAL);
        }

        /* If we have a matching pool with free entries - just use it. */
        if ((p->slice_id == fg->slices->slice_number) &&
            (p->free_cntrs >= f_st->hw_entry_count)) {
            found = 0;
            temp_idx =0;
            for (hw_idx = 0, temp_idx = p->size + 1 - f_st->hw_entry_count;
                    temp_idx; --temp_idx, ++hw_idx) {
                for (i = 0; i < f_st->hw_entry_count &&
                      SHR_BITGET(p->cntr_bmp.w, hw_idx + i) == 0; ++i);
                if (i >= f_st->hw_entry_count) {
                    temp_hw_idx = hw_idx;
                    found = 1;
                    break;
                }
            }
            if (found) {
                pool_idx = idx;
                break;
            }
        }

        /* Preserve first free pool */
        if ((_FP_INVALID_INDEX  == free_pool_idx) &&
            (_FP_INVALID_INDEX  == p->slice_id)) {
            free_pool_idx = idx;
        }
    }

    if (idx < n) {
        p = stage_fc->cntr_pool[pool_idx];
    } else {
        if (_FP_INVALID_INDEX == free_pool_idx) {
            return (BCM_E_RESOURCE);
        }
        pool_idx = free_pool_idx;
        p = stage_fc->cntr_pool[pool_idx];
        p->slice_id = fg->slices->slice_number;
    }

    if (p->free_cntrs < f_st->hw_entry_count) {
        return (BCM_E_RESOURCE);
    }

    f_st->pool_index = pool_idx;
    f_st->hw_index   = temp_hw_idx;

    /* Mark counters as in use */
    for (i = 0; i < f_st->hw_entry_count; ++i, ++temp_hw_idx) {
        SHR_BITSET(p->cntr_bmp.w, temp_hw_idx);
    }
    p->free_cntrs -= f_st->hw_entry_count;

    /* Increment the group counters count equal to
       the hw entries count .*/
    f_ent->group->group_status.counter_count =
         f_ent->group->group_status.counter_count + f_st->hw_entry_count;

    return (BCM_E_NONE);
}


int
_bcm_field_tr3_counter_hw_alloc(int            unit,
                                _field_entry_t *f_ent
                                )
{
    _field_stat_t *f_st;

    BCM_IF_ERROR_RETURN(_bcm_field_stat_get(unit, f_ent->statistic.sid, &f_st));

    switch (f_ent->group->stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
        return (_field_tr3_ingress_counter_hw_alloc(unit, f_ent, f_st));
    default:
        ;
    }

    return (_bcm_field_counter_hw_alloc(unit, f_ent, f_ent->fs));
}


STATIC int
_field_tr3_ingress_stat_hw_free(int            unit,
                                _field_entry_t *f_ent,
                                _field_stat_t  *f_st
                                )
{
    _field_stage_t     *stage_fc;
    _field_cntr_pool_t *p;
    unsigned           i, n;
    uint64             value;      /* 64 bit zero to reset hw value.    */
    int                idx;        /* Statistics iteration index.       */
    int                rv;         /* Operation return status.          */

    /* Decrement hw reference count. */
    if (f_st->hw_ref_count > 0) {
        f_st->hw_ref_count--;
    }

    /* Statistics is not used by any other entry. */
    if (f_st->hw_ref_count == 0) {
        COMPILER_64_ZERO(value);
        /* Read & Reset  individual statistics. */
        for (idx = 0; idx < f_st->nstat; idx++) {
            rv = _field_stat_value_get(unit, 0, f_st, f_st->stat_arr[idx],
                                       f_st->stat_values + idx);
            BCM_IF_ERROR_RETURN(rv);

            rv = _field_stat_value_set(unit, f_st, f_st->stat_arr[idx], value);
            BCM_IF_ERROR_RETURN(rv);
        }

        BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                                                 f_ent->group->stage_id,
                                                 &stage_fc
                                                 ));
        p = stage_fc->cntr_pool[f_st->pool_index];

        for (i = f_st->hw_index, n = f_st->hw_entry_count; n; --n, ++i) {
            SHR_BITCLR(p->cntr_bmp.w, i);
        }

        if ((p->free_cntrs += f_st->hw_entry_count) >= p->size) {
            p->slice_id = -1;
        }

        /* decrement the group's counter count equal to
           the hw counters used for this stat */
        f_ent->group->group_status.counter_count =
             f_ent->group->group_status.counter_count - f_st->hw_entry_count;

        f_st->hw_index   = _FP_INVALID_INDEX;
        f_st->pool_index = _FP_INVALID_INDEX;
    }

    /*
     * If qualifiers have not been modified for this entry,
     * set Action only dirty flag.
     */
    if (0 == (f_ent->flags & _FP_ENTRY_DIRTY)) {
        f_ent->flags |= _FP_ENTRY_POLICY_TABLE_ONLY_DIRTY;
    }

    f_ent->statistic.flags &= ~_FP_ENTRY_STAT_INSTALLED;

    /* Mark entry for reinstall. */
    f_ent->flags |= _FP_ENTRY_DIRTY;

    return (BCM_E_NONE);
}


int
_bcm_field_tr3_stat_hw_free(int unit, _field_entry_t *f_ent)
{
    _field_stat_t *f_st;

    if (!(f_ent->statistic.flags & _FP_ENTRY_STAT_INSTALLED)) {
        return (BCM_E_NONE);
    }

    BCM_IF_ERROR_RETURN(_bcm_field_stat_get(unit, f_ent->statistic.sid, &f_st));

    switch (f_ent->group->stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
        return (_field_tr3_ingress_stat_hw_free(unit, f_ent, f_st));
    default:
        ;
    }

    return (_bcm_field_stat_hw_free(unit, f_ent));
}


STATIC int
_field_tr3_ingress_stat_action_set(int            unit,
                                   _field_entry_t *f_ent,
                                   uint32         *policy_buf
                                   )
{
    unsigned hw_mode, pool_idx, hw_idx;

    if (f_ent->statistic.flags & _FP_ENTRY_STAT_INSTALLED) {
        _field_stat_t *f_st;

        BCM_IF_ERROR_RETURN(_bcm_field_stat_get(unit, f_ent->statistic.sid, &f_st));
        hw_mode  = f_st->hw_mode;
        pool_idx = f_st->pool_index;
        hw_idx   = f_st->hw_index;
    } else {
        hw_mode = pool_idx = hw_idx = 0;
    }

    PolicySet(unit,
              FP_POLICY_TABLEm,
              policy_buf,
              COUNTER_POOL_NUMBERf,
              pool_idx
              );
    PolicySet(unit,
              FP_POLICY_TABLEm,
              policy_buf,
              COUNTER_POOL_INDEXf,
              hw_idx
              );
    PolicySet(unit,
              FP_POLICY_TABLEm,
              policy_buf,
              R_COUNTER_OFFSETf,
              hw_mode >> 4
              );
    PolicySet(unit,
              FP_POLICY_TABLEm,
              policy_buf,
              Y_COUNTER_OFFSETf,
              (hw_mode >> 2) & 3
              );
    PolicySet(unit,
              FP_POLICY_TABLEm,
              policy_buf,
              G_COUNTER_OFFSETf,
              hw_mode & 3
              );

    return (BCM_E_NONE);
}


int
_bcm_field_tr3_stat_action_set(int            unit,
                               _field_entry_t *f_ent,
                               soc_mem_t      mem,
                               int            tcam_idx,
                               uint32         *policy_buf
                               )
{
    switch (f_ent->group->stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
        return (_field_tr3_ingress_stat_action_set(unit, f_ent, policy_buf));
    default:
        ;
    }

    return (_bcm_field_trx_stat_action_set
                (unit, f_ent, mem, tcam_idx, policy_buf));
}


STATIC int
_field_tr3_ingress_stat_index_get(int              unit,
                                  _field_stat_t    *f_st,
                                  bcm_field_stat_t stat,
                                  int              *idx1,
                                  int              *idx2,
                                  int              *idx3,
                                  uint32           *out_flags
                                  )
{
    const unsigned r_cntr_ofs = f_st->hw_mode >> 4,
        y_cntr_ofs = (f_st->hw_mode >> 2) & 3,
        g_cntr_ofs = f_st->hw_mode & 3,
        r_not_used = (r_cntr_ofs == 0),
        r_ne_y = (r_cntr_ofs != y_cntr_ofs),
        r_ne_g = (r_cntr_ofs != g_cntr_ofs),
        y_not_used = (y_cntr_ofs == 0),
        y_ne_g = (y_cntr_ofs != g_cntr_ofs),
        g_not_used = (g_cntr_ofs == 0);

    int counters_per_pool = 0;

    *idx1 = *idx2 = *idx3 = _FP_INVALID_INDEX;
    *out_flags = 0;

    switch (stat) {
    case bcmFieldStatRedBytes:
        *out_flags |= _FP_STAT_BYTES;
        /* coverity[MISSING_BREAK : FALSE] */
    case bcmFieldStatRedPackets:
        if (r_not_used) {
            return (BCM_E_INTERNAL);
        }
        *idx1 = r_cntr_ofs;
        break;

    case bcmFieldStatYellowBytes:
        *out_flags |= _FP_STAT_BYTES;
        /* coverity[MISSING_BREAK : FALSE] */
    case bcmFieldStatYellowPackets:
        if (y_not_used) {
            return (BCM_E_INTERNAL);
        }
        *idx1 = y_cntr_ofs;
        break;

    case bcmFieldStatGreenBytes:
        *out_flags |= _FP_STAT_BYTES;
        /* coverity[MISSING_BREAK : FALSE] */
    case bcmFieldStatGreenPackets:
        if (g_not_used) {
            return (BCM_E_INTERNAL);
        }
        *idx1 = g_cntr_ofs;
        break;

    case bcmFieldStatNotRedBytes:
        *out_flags |= _FP_STAT_BYTES;
        /* coverity[MISSING_BREAK : FALSE] */
    case bcmFieldStatNotRedPackets:
        if (y_not_used || g_not_used) {
            return (BCM_E_INTERNAL);
        }
        *idx1 = y_cntr_ofs;
        if (y_ne_g) {
            *idx2 = g_cntr_ofs;
        }
        break;

    case bcmFieldStatNotYellowBytes:
        *out_flags |= _FP_STAT_BYTES;
        /* coverity[MISSING_BREAK : FALSE] */
    case bcmFieldStatNotYellowPackets:
        if (r_not_used || g_not_used) {
            return (BCM_E_INTERNAL);
        }
        *idx1 = r_cntr_ofs;
        if (r_ne_g) {
            *idx2 = g_cntr_ofs;
        }
        break;

    case bcmFieldStatNotGreenBytes:
        *out_flags |= _FP_STAT_BYTES;
        /* coverity[MISSING_BREAK : FALSE] */
    case bcmFieldStatNotGreenPackets:
        if (r_not_used || y_not_used) {
            return (BCM_E_INTERNAL);
        }
        *idx1 = r_cntr_ofs;
        if (r_ne_y) {
            *idx2 = y_cntr_ofs;
        }
        break;

    case bcmFieldStatBytes:
        *out_flags |= _FP_STAT_BYTES;
        /* coverity[MISSING_BREAK : FALSE] */
    case bcmFieldStatPackets:
        if (r_not_used || y_not_used || g_not_used) {
            return (BCM_E_INTERNAL);
        }
        *idx1 = r_cntr_ofs;
        if (r_ne_y) {
            *idx2 = y_cntr_ofs;

            if (y_ne_g && r_ne_g) {
                *idx3 = g_cntr_ofs;
            }
        } else {
            if (r_ne_g) {
                *idx2 = g_cntr_ofs;
            }
        }
        break;

    default:
        return (BCM_E_INTERNAL);
    }

    /* In Half Slice case, FP still retains the actual hardware index value. 
     * Hence the counters_per_pool should NOT be modified, else the accessing 
     * index will be different from the actual Hardware Index.
     */
    counters_per_pool = NUM_CNTRS_PER_POOL_IFP;

    if (SOC_IS_HELIX4(unit)) {
        counters_per_pool >>= 1;
    }

    if (*idx1 != _FP_INVALID_INDEX) {
        *idx1 = f_st->pool_index * counters_per_pool + f_st->hw_index + *idx1 - 1;
    }
    if (*idx2 != _FP_INVALID_INDEX) {
        *idx2 = f_st->pool_index * counters_per_pool + f_st->hw_index + *idx2 - 1;
        *out_flags |= _FP_STAT_ADD;
    }
    if (*idx3 != _FP_INVALID_INDEX) {
        *idx3 = f_st->pool_index * counters_per_pool + f_st->hw_index + *idx3 - 1;
        *out_flags |= _FP_STAT_ADD;
    }

    return (BCM_E_NONE);
}


STATIC int
_field_tr3_stat_index_get(int              unit,
                          _field_stat_t    *f_st,
                          bcm_field_stat_t stat,
                          int              *idx1,
                          int              *idx2,
                          int              *idx3,
                          uint32           *out_flags
                          )
{
    switch (f_st->stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
        return (_field_tr3_ingress_stat_index_get(unit,
                                                  f_st,
                                                  stat,
                                                  idx1,
                                                  idx2,
                                                  idx3,    
                                                  out_flags
                                                  )
                );
    default:
        ;
    }

    return (_bcm_field_trx_stat_index_get(unit,
                                          f_st,
                                          stat,
                                          idx1,
                                          idx2,
                                          idx3, 
                                          out_flags
                                          )
            );
}


STATIC int
_field_tr3_ingress_qual_tcam_key_mask_get(int unit,
                                          _field_entry_t *f_ent,
                                          _field_tcam_t *tcam
                                          )
{
    int errcode = BCM_E_INTERNAL;
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

        errcode = soc_mem_read(unit,
                               FP_GLOBAL_MASK_TCAMm,
                               MEM_BLOCK_ANY,
                               tcam_idx,
                               tcam_entry
                               );
        if (BCM_FAILURE(errcode)) {
            goto error;
        }
        soc_mem_field_get(unit,
                          FP_GLOBAL_MASK_TCAMm,
                          tcam_entry,
                          KEYf,
                          tcam->key + fp_tcam_words
                          );
        soc_mem_field_get(unit,
                          FP_GLOBAL_MASK_TCAMm,
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


STATIC int
_field_tr3_external_qual_tcam_key_mask_get(int unit,
                                           _field_entry_t *f_ent,
                                           _field_tcam_t *tcam
                                           )
{
    soc_mem_t tcam_data_mem;
    unsigned  key_size;         /* In bytes */

    BCM_IF_ERROR_RETURN(_field_tr3_external_tcam_data_mem(
                            unit,
                            f_ent->fs->slice_number,
                            &tcam_data_mem
                                                          )
                        );

    key_size = WORDS2BYTES(BITS2WORDS(soc_mem_field_length(unit,
                                                           tcam_data_mem,
                                                           DATAf
                                                           )
                                      )
                           );

    f_ent->tcam.key_size = key_size;

    /* Allocate S/W Tcam copy */
    if (NULL == f_ent->tcam.key) {
        f_ent->tcam.key  = sal_alloc(key_size, "field_entry_tcam_key");
        f_ent->tcam.mask = sal_alloc(key_size, "field_entry_tcam_mask");
        if ((NULL == f_ent->tcam.key) || (NULL == f_ent->tcam.mask)) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "FP(unit %d) Error: allocation failure for field_entry tcam\n"),
                       unit));
            if (f_ent->tcam.key) sal_free(f_ent->tcam.key);
            if (f_ent->tcam.mask) sal_free(f_ent->tcam.mask);
            return (BCM_E_MEMORY);
        }
    }
    sal_memset(f_ent->tcam.key, 0, key_size);
    sal_memset(f_ent->tcam.mask, 0, key_size);

    /*
     * Allocate H/W replica copy: only happens 1st time
     *     O/W copy H/W copy to S/W copy
     */
    if (f_ent->tcam.key_hw == NULL) {
        f_ent->tcam.key_hw  = sal_alloc(key_size, "field_entry_tcam_key");
        f_ent->tcam.mask_hw = sal_alloc(key_size, "field_entry_tcam_mask");
        if ((NULL == f_ent->tcam.key_hw) || (NULL == f_ent->tcam.mask_hw)) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "FP(unit %d) Error: allocation failure for field_entry tcam\n"),
                       unit));
            if (f_ent->tcam.key_hw) sal_free(f_ent->tcam.key_hw);
            if (f_ent->tcam.mask_hw) sal_free(f_ent->tcam.mask_hw);
            return (BCM_E_MEMORY);
        }
        sal_memset(f_ent->tcam.key_hw, 0, key_size);
        sal_memset(f_ent->tcam.mask_hw, 0, key_size);
    } else {
        sal_memcpy(f_ent->tcam.key, f_ent->tcam.key_hw, key_size);
        sal_memcpy(f_ent->tcam.mask, f_ent->tcam.mask_hw, key_size);
    }

    return (BCM_E_NONE);
}


int
_bcm_field_tr3_qual_tcam_key_mask_get(int unit,
                                      _field_entry_t *f_ent,
                                      _field_tcam_t *tcam
                                      )
{
    switch (f_ent->group->stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
        return (_field_tr3_ingress_qual_tcam_key_mask_get(unit, f_ent, tcam));
    case _BCM_FIELD_STAGE_EXTERNAL:
        return (_field_tr3_external_qual_tcam_key_mask_get(unit, f_ent, tcam));
    default:
        ;
    }

    return (_field_qual_tcam_key_mask_get(unit, f_ent, tcam, 0));
}


STATIC int
_field_tr3_ingress_qual_tcam_key_mask_set(int            unit,
                                          _field_entry_t *f_ent,
                                          unsigned       tcam_idx,
                                          unsigned       validf
                                          )
{
    _field_tcam_t  * const tcam = &f_ent->tcam;
    _field_group_t * const fg = f_ent->group;
    const unsigned fp_tcam_words
        = BITS2WORDS(soc_mem_field_length(unit,
                                          FP_TCAMm,
                                          KEYf
                                          )
                     );
    uint32         tcam_entry[SOC_MAX_MEM_FIELD_WORDS] = {0};

    BCM_IF_ERROR_RETURN(soc_mem_read(unit,
                                     FP_TCAMm,
                                     MEM_BLOCK_ANY,
                                     tcam_idx,
                                     tcam_entry
                                     )
                        );
    soc_mem_field_set(unit, FP_TCAMm, tcam_entry, KEYf, tcam->key);
    soc_mem_field_set(unit, FP_TCAMm, tcam_entry, MASKf, tcam->mask);
    soc_mem_field32_set(unit,
                        FP_TCAMm,
                        tcam_entry,
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
                                      tcam_entry
                                      )
                        );

    BCM_IF_ERROR_RETURN(soc_mem_read(unit,
                                     FP_GLOBAL_MASK_TCAMm,
                                     MEM_BLOCK_ANY,
                                     tcam_idx,
                                     tcam_entry
                                     )
                        );
    soc_mem_field_set(unit,
                      FP_GLOBAL_MASK_TCAMm,
                      tcam_entry,
                      KEYf,
                      tcam->key + fp_tcam_words
                      );
    soc_mem_field_set(unit,
                      FP_GLOBAL_MASK_TCAMm,
                      tcam_entry,
                      MASKf,
                      tcam->mask + fp_tcam_words
                      );
    if (!(f_ent->flags & _FP_ENTRY_SECOND_HALF)) {
        soc_mem_field_set(unit,
                          FP_GLOBAL_MASK_TCAMm,
                          tcam_entry,
                          IPBMf,
                          f_ent->pbmp.data.pbits
                          );
        soc_mem_field_set(unit,
                          FP_GLOBAL_MASK_TCAMm,
                          tcam_entry,
                          IPBM_MASKf,
                          f_ent->pbmp.mask.pbits
                          );
    }
    soc_mem_field32_set(unit,
                        FP_GLOBAL_MASK_TCAMm,
                        tcam_entry,
                        VALIDf,
                        validf ? 1 : 0
                        );
    BCM_IF_ERROR_RETURN(soc_mem_write(unit,
                                      FP_GLOBAL_MASK_TCAMm,
                                      MEM_BLOCK_ALL,
                                      tcam_idx,
                                      tcam_entry
                                      )
                        );

    return (BCM_E_NONE);
}


int
_bcm_field_tr3_qual_tcam_key_mask_set(int            unit,
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
        return (_field_tr3_ingress_qual_tcam_key_mask_set(unit, f_ent, tcam_idx, validf));
    case _BCM_FIELD_STAGE_EGRESS:
        tcam_mem = EFP_TCAMm;
        break;
    default:
        return (BCM_E_INTERNAL);
    }
    {
        uint32 tcam_entry[SOC_MAX_MEM_FIELD_WORDS] = {0};

        BCM_IF_ERROR_RETURN(_bcm_field_trx_tcam_get(unit, tcam_mem, f_ent, tcam_entry));
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx, tcam_entry));
    }

    return (BCM_E_NONE);
}


STATIC int
_field_tr3_ingress_write_slice_map(int unit, _field_stage_t *stage_fc)
{
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
        VIRTUAL_SLICE_11_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
        VIRTUAL_SLICE_12_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
        VIRTUAL_SLICE_13_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
        VIRTUAL_SLICE_14_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
        VIRTUAL_SLICE_15_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
        VIRTUAL_SLICE_16_PHYSICAL_SLICE_NUMBER_ENTRY_0f
    }, slice_group[] = {
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
        VIRTUAL_SLICE_11_VIRTUAL_SLICE_GROUP_ENTRY_0f,
        VIRTUAL_SLICE_12_VIRTUAL_SLICE_GROUP_ENTRY_0f,
        VIRTUAL_SLICE_13_VIRTUAL_SLICE_GROUP_ENTRY_0f,
        VIRTUAL_SLICE_14_VIRTUAL_SLICE_GROUP_ENTRY_0f,
        VIRTUAL_SLICE_15_VIRTUAL_SLICE_GROUP_ENTRY_0f,
        VIRTUAL_SLICE_16_VIRTUAL_SLICE_GROUP_ENTRY_0f
    };

    _field_control_t     *fc;
    fp_slice_map_entry_t fp_slice_map_buf;
    unsigned             vmap_size, index;

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    vmap_size = stage_fc->tcam_slices;
    if (fc->flags & _FP_EXTERNAL_PRESENT) {
        ++vmap_size;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit,
                                     FP_SLICE_MAPm,
                                     MEM_BLOCK_ANY,
                                     0,
                                     fp_slice_map_buf.entry_data
                                     )
                        );

    for (index = 0; index < vmap_size; index++) {
        soc_mem_field32_set(unit,
                            FP_SLICE_MAPm,
                            fp_slice_map_buf.entry_data,
                            physical_slice[index],
                            stage_fc->vmap[_FP_DEF_INST][_FP_VMAP_DEFAULT][index].vmap_key
                            );
        soc_mem_field32_set(unit,
                            FP_SLICE_MAPm,
                            fp_slice_map_buf.entry_data,
                            slice_group[index],
                            stage_fc->vmap[_FP_DEF_INST][_FP_VMAP_DEFAULT][index].virtual_group
                            );
    }

    return (soc_mem_write(unit,
                          FP_SLICE_MAPm,
                          MEM_BLOCK_ALL,
                          0,
                          fp_slice_map_buf.entry_data
                          )
            );
}


int
_bcm_field_tr3_write_slice_map(int unit, _field_stage_t *stage_fc,
                               _field_group_t *fg)
{
    switch (stage_fc->stage_id) {
    case _BCM_FIELD_STAGE_LOOKUP:
        return (_bcm_field_trx_write_slice_map_vfp(unit, stage_fc));

    case _BCM_FIELD_STAGE_INGRESS:
        return (_field_tr3_ingress_write_slice_map(unit, stage_fc));

    case _BCM_FIELD_STAGE_EGRESS:
        return (_bcm_field_trx_write_slice_map_egress(unit, stage_fc));

    default:
        ;
    }

    return (BCM_E_INTERNAL);
}

/*
 * Function:
 *     _bcm_field_tr3_cosq_action_param_get
 * Purpose:
 *     Returns CosQ action's param value if action is set for the
 *     entry.
 * Parameters:
 *     unit     - BCM device number
 *     f_ent    - pointer to field entry structure
 *     action   - CosQ action for which param value needs to be retrieved
 *     cosq     - (OUT) CoS value
 * Returns:
 *     BCM_E_XXX
 */
STATIC
int _bcm_field_tr3_cosq_action_param_get(int unit,
                                         _field_entry_t *f_ent,
                                         bcm_field_action_t action,
                                         int *cosq)
{
    _field_action_t *fa_iter = NULL;    /* Field entry action */

    if (NULL == cosq || NULL == f_ent) {
        return (BCM_E_INTERNAL);
    }

    /* Get the requested action param. */
    fa_iter = f_ent->actions;
    while (fa_iter != NULL) {
        if (action == fa_iter->action) {
            *cosq = fa_iter->param[0];
            return (BCM_E_NONE);
        }
        fa_iter = fa_iter->next;
    }

    /* Specified action not found in entry action list. */
    return (BCM_E_NOT_FOUND);
}

/*
 * Function:
 *     _bcm_field_tr3_ucast_mcast_action_set
 * Purpose:
 *     Set Unicast and Multicast CoS action in the entry buffer
 * Parameters:
 *     unit     - BCM device number
 *     mem      - Policy table memory
 *     f_ent    - entry structure to get policy info from
 *     tcam_idx - index into TCAM
 *     fa       - field action
 *     buf      - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_XXX
 */
STATIC
int _bcm_field_tr3_ucast_mcast_action_set(int unit,
                                             soc_mem_t mem,
                                             _field_entry_t *f_ent,
                                             int tcam_idx,
                                             _field_action_t *fa,
                                             uint32 *buf)
{
    int m_cos = BCM_COS_INVALID;
    int u_cos = BCM_COS_INVALID;
    int cos = BCM_COS_INVALID;
    int rv;


    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    cos = (int) fa->param[0];

    switch (fa->action) {
        case bcmFieldActionMcastCosQNew:
            m_cos = cos;
            rv = _bcm_field_tr3_cosq_action_param_get(unit,
                    f_ent, bcmFieldActionUcastCosQNew, &u_cos);
            if (BCM_SUCCESS(rv)) {
                PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);
                PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);
                PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

                PolicySet(unit, mem, buf, R_COS_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
                PolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
                PolicySet(unit, mem, buf, G_COS_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
            } else {
                PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_MCAST_QUEUE_NEW_MODE);
                PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_MCAST_QUEUE_NEW_MODE);
                PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_MCAST_QUEUE_NEW_MODE);

                PolicySet(unit, mem, buf, R_COS_INT_PRIf,
                    _FP_ACTION_MCAST_QUEUE_SET(m_cos));
                PolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                    _FP_ACTION_MCAST_QUEUE_SET(m_cos));
                PolicySet(unit, mem, buf, G_COS_INT_PRIf,
                    _FP_ACTION_MCAST_QUEUE_SET(m_cos));
            }
            break;

        case bcmFieldActionGpMcastCosQNew:
            m_cos = cos;
            rv = _bcm_field_tr3_cosq_action_param_get(unit,
                    f_ent, bcmFieldActionGpUcastCosQNew, &u_cos);
            if (BCM_SUCCESS(rv)) {
                PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

                PolicySet(unit, mem, buf, G_COS_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
            } else {
                PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_MCAST_QUEUE_NEW_MODE);

                PolicySet(unit, mem, buf, G_COS_INT_PRIf,
                    _FP_ACTION_MCAST_QUEUE_SET(m_cos));
            }
            break;

        case bcmFieldActionYpMcastCosQNew:
            m_cos = cos;
            rv = _bcm_field_tr3_cosq_action_param_get(unit,
                    f_ent, bcmFieldActionYpUcastCosQNew, &u_cos);
            if (BCM_SUCCESS(rv)) {
                PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

                PolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
            } else {
                PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_MCAST_QUEUE_NEW_MODE);

                PolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                    _FP_ACTION_MCAST_QUEUE_SET(m_cos));
            }
            break;

        case bcmFieldActionRpMcastCosQNew:
            m_cos = cos;
            rv = _bcm_field_tr3_cosq_action_param_get(unit,
                    f_ent, bcmFieldActionRpUcastCosQNew, &u_cos);
            if (BCM_SUCCESS(rv)) {
                PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

                PolicySet(unit, mem, buf, R_COS_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
            } else {
                PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_MCAST_QUEUE_NEW_MODE);

                PolicySet(unit, mem, buf, R_COS_INT_PRIf,
                    _FP_ACTION_MCAST_QUEUE_SET(m_cos));
            }
            break;

        case bcmFieldActionUcastCosQNew:
            u_cos = cos;
            rv = _bcm_field_tr3_cosq_action_param_get(unit,
                    f_ent, bcmFieldActionMcastCosQNew, &m_cos);
            if (BCM_SUCCESS(rv)) {
                PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);
                PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);
                PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

                PolicySet(unit, mem, buf, R_COS_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
                PolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
                PolicySet(unit, mem, buf, G_COS_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
            } else {
                PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_QUEUE_NEW_MODE);
                PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_QUEUE_NEW_MODE);
                PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_QUEUE_NEW_MODE);

                PolicySet(unit, mem, buf, R_COS_INT_PRIf,
                    _FP_ACTION_UCAST_QUEUE_SET(u_cos));
                PolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                    _FP_ACTION_UCAST_QUEUE_SET(u_cos));
                PolicySet(unit, mem, buf, G_COS_INT_PRIf,
                    _FP_ACTION_UCAST_QUEUE_SET(u_cos));
            }
            break;

        case bcmFieldActionGpUcastCosQNew:
            u_cos = cos;
            rv = _bcm_field_tr3_cosq_action_param_get(unit,
                    f_ent, bcmFieldActionGpMcastCosQNew, &m_cos);
            if (BCM_SUCCESS(rv)) {
                PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

                PolicySet(unit, mem, buf, G_COS_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
            } else {
                PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_QUEUE_NEW_MODE);

                PolicySet(unit, mem, buf, G_COS_INT_PRIf,
                    _FP_ACTION_UCAST_QUEUE_SET(u_cos));
            }
            break;

        case bcmFieldActionYpUcastCosQNew:
            u_cos = cos;
            rv = _bcm_field_tr3_cosq_action_param_get(unit,
                    f_ent, bcmFieldActionYpMcastCosQNew, &m_cos);
            if (BCM_SUCCESS(rv)) {
                PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

                PolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
            } else {
                PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_QUEUE_NEW_MODE);

                PolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                    _FP_ACTION_UCAST_QUEUE_SET(u_cos));
            }
            break;

        case bcmFieldActionRpUcastCosQNew:
            u_cos = cos;
            rv = _bcm_field_tr3_cosq_action_param_get(unit,
                    f_ent, bcmFieldActionRpMcastCosQNew, &m_cos);
            if (BCM_SUCCESS(rv)) {
                PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

                PolicySet(unit, mem, buf, R_COS_INT_PRIf,
                    _FP_ACTION_UCAST_MCAST_QUEUE_SET(u_cos, m_cos));
            } else {
                PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                    _FP_ACTION_UCAST_QUEUE_NEW_MODE);

                PolicySet(unit, mem, buf, R_COS_INT_PRIf,
                    _FP_ACTION_UCAST_QUEUE_SET(u_cos));
            }
            break;

        default:
            return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_tr3_action_get
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
_bcm_field_tr3_action_get(int             unit,
                          soc_mem_t       mem,
                          _field_entry_t  *f_ent,
                          int             tcam_idx,
                          _field_action_t *fa,
                          uint32          *buf
                          )
{
    uint32 arg = fa->param[0];
    bcm_port_t local_port;
    int index;
    int cosq_new;
    int rv = 0;

    switch (fa->action) {
        case bcmFieldActionNewClassId:
            PolicySet(unit, mem, buf, I2E_CLASSIDf, arg);
            PolicySet(unit, mem, buf, G_L3SW_CHANGE_L2_FIELDSf, 0x8);
            break;
        case bcmFieldActionTrunkLoadBalanceCancel:
            PolicySet(unit, mem, buf, LAG_DLB_DISABLEf, 1);
            PolicySet(unit, mem, buf, HGT_DLB_DISABLEf, 1);
            break;

        case bcmFieldActionEcmpLoadBalanceCancel:
            PolicySet(unit, mem, buf, ECMP_DLB_DISABLEf, 1);
            break;

        case bcmFieldActionEgressClassSelect:
            {
                unsigned i2e_cl_sel;

                switch (arg) {
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

                switch (arg) {
                    case BCM_FIELD_HIGIG_CLASS_SELECT_EGRESS:
                        hg_cl_sel = 1;
                        break;
                    case BCM_FIELD_HIGIG_CLASS_SELECT_EGR_DVP:
                        hg_cl_sel = 4;
                        break;
                    case BCM_FIELD_HIGIG_CLASS_SELECT_EGR_L3_INTERFACE:
                        hg_cl_sel = 3;
                        break;
                    case BCM_FIELD_HIGIG_CLASS_SELECT_EGR_NEXT_HOP:
                        hg_cl_sel = 2;
                        break;
                    case BCM_FIELD_HIGIG_CLASS_SELECT_PORT:
                        hg_cl_sel = 7;
                        break;
                    default:
                        /* Invalid parameter should have been caught earlier */
                        return (BCM_E_INTERNAL);
                }

                PolicySet(unit, mem, buf, HG_CLASSID_SELf, hg_cl_sel);
            }
            break;

        case bcmFieldActionOamDmTimeFormat:
            {
                unsigned oam_dm_type;

                switch (arg) {
                    case BCM_FIELD_OAM_DM_TIME_FORMAT_IEEE1588:
                        oam_dm_type = 0;
                        break;
                    case BCM_FIELD_OAM_DM_TIME_FORMAT_NTP:
                        oam_dm_type = 1;
                        break;
                    default:
                        /* Invalid parameter should have been caught earlier */
                        return (BCM_E_INTERNAL);
                }

                PolicySet(unit, mem, buf, OAM_DM_TYPEf, oam_dm_type);
            }
            break;

        case bcmFieldActionOamLmDmSampleEnable:
            PolicySet(unit, mem, buf, OAM_ENABLE_LM_DM_SAMPLEf, arg ? 1 : 0);
            break;

        case bcmFieldActionOamTagStatusCheck:
            PolicySet(unit, mem, buf, OAM_TAG_STATUS_CHECK_CONTROLf, arg);
            break;

        case bcmFieldActionOamTunnelControl:
            {
                unsigned oam_tnl_cntl;

                switch (arg) {
                    case BCM_FIELD_OAM_TUNNEL_CONTROL_ANY:
                        oam_tnl_cntl = 0;
                        break;
                    case BCM_FIELD_OAM_TUNNEL_CONTROL_NOT_TUNNELDED:
                        oam_tnl_cntl = 1;
                        break;
                    default:
                        /* Invalid parameter should have been caught earlier */
                        return (BCM_E_INTERNAL);
                }

                PolicySet(unit, mem, buf, OAM_TUNNEL_CONTROLf, oam_tnl_cntl);
            }
            break;

        case bcmFieldActionRegex:
            PolicySet(unit, mem, buf, DO_DPIf, 1);
            break;

        case bcmFieldActionNoRegex:
            PolicySet(unit, mem, buf, DO_NOT_DPIf, 1);
            break;

        case bcmFieldActionRegexActionCancel:
            PolicySet(unit, mem, buf, DPI_CNT_ONLYf, 1);
            break;

        case bcmFieldActionEcnNew:
            PolicySet(unit, mem, buf, R_CHANGE_ECNf, 1);
            PolicySet(unit, mem, buf, R_NEW_ECNf, arg);
            PolicySet(unit, mem, buf, Y_CHANGE_ECNf, 1);
            PolicySet(unit, mem, buf, Y_NEW_ECNf, arg);
            PolicySet(unit, mem, buf, G_CHANGE_ECNf, 1);
            PolicySet(unit, mem, buf, G_NEW_ECNf, arg);
            break;

        case bcmFieldActionRpEcnNew:
            PolicySet(unit, mem, buf, R_CHANGE_ECNf, 1);
            PolicySet(unit, mem, buf, R_NEW_ECNf, arg);
            break;

        case bcmFieldActionYpEcnNew:
            PolicySet(unit, mem, buf, Y_CHANGE_ECNf, 1);
            PolicySet(unit, mem, buf, Y_NEW_ECNf, arg);
            break;

        case bcmFieldActionGpEcnNew:
            PolicySet(unit, mem, buf, G_CHANGE_ECNf, 1);
            PolicySet(unit, mem, buf, G_NEW_ECNf, arg);
            break;

        case bcmFieldActionCosQNew:
            /* Set new CoSQ/hardware queue value. */
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(fa->param[0])) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                                      _BCM_TR3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                      &local_port, &index, NULL));
            } else {
                index = fa->param[0];
            }

            PolicySet(unit, mem, buf, R_COS_INT_PRIf,
                      _FP_ACTION_UCAST_MCAST_QUEUE_SET(index,index));
            PolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                      _FP_ACTION_UCAST_MCAST_QUEUE_SET(index,index));
            PolicySet(unit, mem, buf, G_COS_INT_PRIf,
                      _FP_ACTION_UCAST_MCAST_QUEUE_SET(index,index));

            PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                      _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);
            PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                      _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);
            PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                      _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);
            break;

        case bcmFieldActionGpCosQNew:
            /* Apply this action for Gp packets. */
            PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf,
                      _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

            /* Set new CoSQ/hardware queue value. */
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(fa->param[0])) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                                      _BCM_TR3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                      &local_port, &index, NULL));
            } else {
                index = fa->param[0];
            }

            PolicySet(unit, mem, buf, G_COS_INT_PRIf,
                      _FP_ACTION_UCAST_MCAST_QUEUE_SET(index,index));
            break;

        case bcmFieldActionYpCosQNew:
            /* Apply this action for Yp packets. */
            PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf,
                      _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

            /* Set new CoSQ/hardware queue value. */
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(fa->param[0])) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                                      _BCM_TR3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                      &local_port, &index, NULL));
            } else {
                index = fa->param[0];
            }

            PolicySet(unit, mem, buf, Y_COS_INT_PRIf,
                      _FP_ACTION_UCAST_MCAST_QUEUE_SET(index,index));
            break;

        case bcmFieldActionRpCosQNew:
            /* Apply this action for Rp packets. */
            PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf,
                      _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE);

            /* Set new CoSQ/hardware queue value. */
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(fa->param[0])) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                                      _BCM_TR3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                      &local_port, &index, NULL));
            } else {
                index = fa->param[0];
            }

            PolicySet(unit, mem, buf, R_COS_INT_PRIf,
                      _FP_ACTION_UCAST_MCAST_QUEUE_SET(index,index));
            break;

        case bcmFieldActionUcastCosQNew:
        case bcmFieldActionMcastCosQNew:
        case bcmFieldActionGpMcastCosQNew:
        case bcmFieldActionYpMcastCosQNew:
        case bcmFieldActionRpMcastCosQNew:
        case bcmFieldActionGpUcastCosQNew:
        case bcmFieldActionYpUcastCosQNew:
        case bcmFieldActionRpUcastCosQNew:
            BCM_IF_ERROR_RETURN
                (_bcm_field_tr3_ucast_mcast_action_set(unit, mem, f_ent,
                                                       tcam_idx, fa, buf));
            break;

        case bcmFieldActionUcastQueueNew:
            PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 2);
            PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 2);
            PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 2);
            rv = _bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                    _BCM_TR3_COSQ_INDEX_STYLE_BUCKET, NULL,
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
            rv = _bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                    _BCM_TR3_COSQ_INDEX_STYLE_BUCKET, NULL,
                    &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }
            PolicySet(unit, mem, buf, R_COS_INT_PRIf, cosq_new);
            break;

        case bcmFieldActionYpUcastQueueNew:
            PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 2);
            rv = _bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                    _BCM_TR3_COSQ_INDEX_STYLE_BUCKET, NULL,
                    &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }
            PolicySet(unit, mem, buf, Y_COS_INT_PRIf, cosq_new);
            break;

        case bcmFieldActionGpUcastQueueNew:
            PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 2);
            rv = _bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                    _BCM_TR3_COSQ_INDEX_STYLE_BUCKET, NULL,
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
            rv = _bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                    _BCM_TR3_COSQ_INDEX_STYLE_BUCKET, NULL,
                    &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }
            PolicySet(unit, mem, buf, R_COS_INT_PRIf, cosq_new);
            PolicySet(unit, mem, buf, Y_COS_INT_PRIf, cosq_new);
            PolicySet(unit, mem, buf, G_COS_INT_PRIf, cosq_new);
            break;

        case bcmFieldActionRpIntCosUcastQueueNew:
            PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 3);
            rv = _bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                    _BCM_TR3_COSQ_INDEX_STYLE_BUCKET, NULL,
                    &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }
            PolicySet(unit, mem, buf, R_COS_INT_PRIf, cosq_new);
            break;

        case bcmFieldActionYpIntCosUcastQueueNew:
            PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 3);
            rv = _bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                    _BCM_TR3_COSQ_INDEX_STYLE_BUCKET, NULL,
                    &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }
            PolicySet(unit, mem, buf, Y_COS_INT_PRIf, cosq_new);
            break;

        case bcmFieldActionGpIntCosUcastQueueNew:
            PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 3);
            rv = _bcm_tr3_cosq_index_resolve(unit, fa->param[0], 0,
                    _BCM_TR3_COSQ_INDEX_STYLE_BUCKET, NULL,
                    &cosq_new, NULL);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }
            PolicySet(unit, mem, buf, G_COS_INT_PRIf, cosq_new);
            break;

        case bcmFieldActionCopyToCpu:
        case bcmFieldActionRpCopyToCpu:
        case bcmFieldActionYpCopyToCpu:
        case bcmFieldActionGpCopyToCpu:
            BCM_IF_ERROR_RETURN(_field_trx_action_copy_to_cpu(unit, mem, f_ent,
                                                              fa, buf));
            break;
        default:
            return (BCM_E_UNAVAIL);
    }

    return (BCM_E_NONE);
}


int
_bcm_field_tr3_counter_mem_get(int            unit,
                               _field_stage_t *stage_fc,
                               soc_mem_t      *cntr_x_mem,
                               soc_mem_t      *cntr_y_mem
                               )
{
    switch (stage_fc->stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
        *cntr_x_mem = FP_COUNTER_TABLEm;
        *cntr_y_mem = INVALIDm;
        break;

    case _BCM_FIELD_STAGE_EXTERNAL:
        *cntr_x_mem = *cntr_y_mem = INVALIDm;
        break;

    default:
        return (_bcm_field_counter_mem_get(unit,
                                           stage_fc,
                                           cntr_x_mem,
                                           cntr_y_mem
                                           )
                );
    }

    return (BCM_E_NONE);
}


STATIC int
_field_tr3_counter_write(int                        unit,
                         soc_mem_t                  mem,
                         int                        idx,
                         uint32                     *buf,
                         _field_counter32_collect_t *cntrs32_buf,
                         _field_counter64_collect_t *cntrs64_buf,
                         uint64                     *packet_count,
                         uint64                     *byte_count
                         )
{
    uint32  hw_val[2];     /* Parsed field counter value.*/

    /* Set byte count in the memory */
    if (NULL != byte_count) {
        COMPILER_64_ZERO(cntrs64_buf->accumulated_counter);
        COMPILER_64_OR(cntrs64_buf->accumulated_counter, (*byte_count));
        COMPILER_64_SET(cntrs64_buf->last_hw_value,
                        COMPILER_64_HI(*byte_count) & 0xf,
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


STATIC int
_field_tr3_counter_set(int            unit,
                       _field_stage_t *stage_fc,
                       soc_mem_t      counter_x_mem,
                       uint32         *mem_x_buf,
                       soc_mem_t      counter_y_mem,
                       uint32         *mem_y_buf,
                       int            idx,
                       uint64         *packet_count,
                       uint64         *byte_count
                       )
{
    _field_counter32_collect_t *cntrs32_buf;  /* Sw packet counter value    */
    _field_counter64_collect_t *cntrs64_buf;  /* Sw byte counter value      */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return BCM_E_PARAM;
    }

    /* Write X pipeline counter value. */
    if (NULL != mem_x_buf) {
        cntrs64_buf = &stage_fc->_field_x64_counters[idx];
        cntrs32_buf = &stage_fc->_field_x32_counters[idx];
        BCM_IF_ERROR_RETURN(
            _field_tr3_counter_write(unit,
                                     counter_x_mem,
                                     idx,
                                     mem_x_buf,
                                     cntrs32_buf,
                                     cntrs64_buf,
                                     packet_count,
                                     byte_count
                                     )
                            );
    }

    return (BCM_E_NONE);
}


STATIC int
_field_tr3_counter_get(int            unit,
                       _field_stage_t *stage_fc,
                       soc_mem_t      counter_x_mem,
                       uint32         *mem_x_buf,
                       soc_mem_t      counter_y_mem,
                       uint32         *mem_y_buf,
                       int            idx,
                       uint64         *packet_count,
                       uint64         *byte_count
                       )
{
    _field_counter32_collect_t *cntrs32_buf;  /* Sw packet counter value    */
    _field_counter64_collect_t *cntrs64_buf;  /* Sw byte counter value      */
    uint32                     hw_val[2];     /* Parsed field counter value.*/
    soc_memacc_t *memacc_pkt, *memacc_byte;   /* Memory access cache.   */

    /* Input parameters check. */
    if ((NULL == mem_x_buf) || (INVALIDm == counter_x_mem) || \
        (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    /* Byte counter. */
    hw_val[0] = hw_val[1] = 0;
    cntrs64_buf = &stage_fc->_field_x64_counters[idx];
    memacc_byte =
        &(stage_fc->_field_memacc_counters[_FIELD_COUNTER_MEMACC_BYTE]);
    soc_memacc_field_get(memacc_byte, mem_x_buf, hw_val);
    _bcm_field_36bit_counter_update(unit, hw_val, cntrs64_buf);
    /* Return counter value to caller if (out) pointer was provided. */
    if (NULL != byte_count) {
        COMPILER_64_OR((*byte_count), cntrs64_buf->accumulated_counter);
    }

    /* Packet counter. */
    cntrs32_buf = &stage_fc->_field_x32_counters[idx];
    memacc_pkt =
        &(stage_fc->_field_memacc_counters[_FIELD_COUNTER_MEMACC_PACKET]);
    soc_memacc_field_get(memacc_pkt, mem_x_buf, hw_val);
    _bcm_field_30bit_counter_update(unit, hw_val, cntrs32_buf);
    /* Return counter value to caller if (out) pointer was provided. */
    if (NULL != packet_count) {
        COMPILER_64_OR((*packet_count), cntrs32_buf->accumulated_counter);
    }
    return (BCM_E_NONE);
}


STATIC int
_field_tr3_external_tcam_write(int unit, _field_entry_t *f_ent, int index)
{
    const unsigned        slice_num     = f_ent->fs->slice_number;
    _field_tcam_t * const tcam          = &f_ent->tcam;

    soc_mem_t tcam_data_mem, tcam_mask_mem;
    uint32    buf[SOC_MAX_MEM_FIELD_WORDS];

    BCM_IF_ERROR_RETURN(_field_tr3_external_tcam_data_mem(unit,
                                                          slice_num,
                                                          &tcam_data_mem
                                                          )
                        );
    BCM_IF_ERROR_RETURN(_field_tr3_external_tcam_mask_mem(unit,
                                                          slice_num,
                                                          &tcam_mask_mem
                                                          )
                        );

    if (tcam_mask_mem == INVALIDm) {
        sal_memset(buf, 0, sizeof(buf));
        soc_mem_field_set(unit, tcam_data_mem, buf, DATAf, tcam->key_hw);
        soc_mem_mask_field_set(unit, tcam_data_mem, buf, MASKf, tcam->mask_hw);
        return (soc_mem_write(unit, tcam_data_mem, MEM_BLOCK_ALL, index, buf));
    }

    sal_memset(buf, 0, sizeof(buf));
    soc_mem_mask_field_set(unit, tcam_mask_mem, buf, MASKf, tcam->mask_hw);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit,
                                      tcam_mask_mem,
                                      MEM_BLOCK_ALL,
                                      0,
                                      buf
                                      )
                        );

    sal_memset(buf, 0, sizeof(buf));
    soc_mem_field_set(unit, tcam_data_mem, buf, DATAf, tcam->key_hw);
    return (soc_mem_write(unit, tcam_data_mem, MEM_BLOCK_ALL, index, buf));
}


/* Translate an IFP policy table word into non-profiled fields of EXTFP policy.
   N.B. The order of table entries is important; it is the order of bits in
   ESM_ACL_ACTION_CONTROL word, from LSB to MSB.
 */

static const struct _field_tr3_external_non_profiled_policy_fld {
    soc_field_t esm_acl_action_control_fld;
    uint8       bit_num;
    soc_mem_t   policy_mem;
    soc_field_t policy_fld;
    uint8       skip_bits;
} _field_tr3_external_non_profiled_policy_fld_tbl[] = {
    { MISC1_SET_ENABLEf,          0, FP_POLICY_TABLEm,  DO_NOT_GENERATE_CNMf },
    { MISC1_SET_ENABLEf,          1, FP_POLICY_TABLEm,  DO_NOT_CHANGE_TTLf },
    { MISC1_SET_ENABLEf,          2, FP_POLICY_TABLEm,  DO_NOT_URPFf },
    { MISC1_SET_ENABLEf,          3, FP_POLICY_TABLEm,  HGT_DLB_DISABLEf },
    { MISC1_SET_ENABLEf,          4, FP_POLICY_TABLEm,  LAG_DLB_DISABLEf },
    { MISC1_SET_ENABLEf,          5, FP_POLICY_TABLEm,  ECMP_DLB_DISABLEf },
    { MISC1_SET_ENABLEf,          6, FP_POLICY_TABLEm,  GREEN_TO_PIDf },
    { MISC1_SET_ENABLEf,          7, FP_POLICY_TABLEm,  MIRROR_OVERRIDEf },
    { MISC2_SET_ENABLEf,          0, FP_POLICY_TABLEm,  IPFIX_CONTROLf },
    { MISC2_SET_ENABLEf,          1, FP_POLICY_TABLEm,  R_DROP_PRECEDENCEf },
    { MISC2_SET_ENABLEf,          2, FP_POLICY_TABLEm,  Y_DROP_PRECEDENCEf },
    { MISC2_SET_ENABLEf,          3, FP_POLICY_TABLEm,  G_DROP_PRECEDENCEf },
    { MISC2_SET_ENABLEf,          4, FP_POLICY_TABLEm,  R_DROPf },
    { MISC2_SET_ENABLEf,          5, FP_POLICY_TABLEm,  Y_DROPf },
    { MISC2_SET_ENABLEf,          6, FP_POLICY_TABLEm,  G_DROPf },
    { GOA_SET_ENABLEf,            0, FP_POLICY_TABLEm,  HI_PRI_SUPPRESS_VXLTf },
    { GOA_SET_ENABLEf,            0, FP_POLICY_TABLEm,  SUPPRESS_SW_ACTIONSf },
    { GOA_SET_ENABLEf,            0, FP_POLICY_TABLEm,  DEFER_QOS_MARKINGSf },
    { GOA_SET_ENABLEf,            0, FP_POLICY_TABLEm,  SUPPRESS_COLOR_SENSITIVE_ACTIONSf },
    { GOA_SET_ENABLEf,            0, FP_POLICY_TABLEm,  SPLIT_DROP_RESOLVEf },
    { GOA_SET_ENABLEf,            0, FP_POLICY_TABLEm,  HI_PRI_RESOLVEf },
    { GOA_SET_ENABLEf,            0, FP_POLICY_TABLEm,  HI_PRI_ACTION_CONTROLf },
    { CPU_COS_SET_ENABLEf,        0, FP_POLICY_TABLEm,  CPU_COSf },
    { CPU_COS_SET_ENABLEf,        0, FP_POLICY_TABLEm,  CHANGE_CPU_COSf },
    { MIRROR_SET_ENABLEf,         0, FP_POLICY_TABLEm,  MTP_INDEX0f },
    { MIRROR_SET_ENABLEf,         0, FP_POLICY_TABLEm,  MTP_INDEX1f },
    { MIRROR_SET_ENABLEf,         0, FP_POLICY_TABLEm,  MTP_INDEX2f },
    { MIRROR_SET_ENABLEf,         0, FP_POLICY_TABLEm,  MTP_INDEX3f },
    { MIRROR_SET_ENABLEf,         0, FP_POLICY_TABLEm,  MIRRORf },
    { COPY_TO_CPU_SET_ENABLEf,    0, FP_POLICY_TABLEm,  R_COPY_TO_CPUf },
    { COPY_TO_CPU_SET_ENABLEf,    0, FP_POLICY_TABLEm,  Y_COPY_TO_CPUf },
    { COPY_TO_CPU_SET_ENABLEf,    0, FP_POLICY_TABLEm,  G_COPY_TO_CPUf },
    { COPY_TO_CPU_SET_ENABLEf,    0, FP_POLICY_TABLEm,  MATCHED_RULEf },
    { COPY_TO_CPU_SET_ENABLEf,    0, FP_POLICY_TABLEm,  COPY_TO_PASSTHRU_NLFf },
    { SHARED_METER_SET_ENABLEf,   0, FP_POLICY_TABLEm,  METER_SHARING_MODE_MODIFIERf },
    { SHARED_METER_SET_ENABLEf,   0, FP_POLICY_TABLEm,  METER_SHARING_MODEf },
    { SHARED_METER_SET_ENABLEf,   0, FP_POLICY_TABLEm,  SHARED_METER_PAIR_MODEf },
    { SHARED_METER_SET_ENABLEf,   0, FP_POLICY_TABLEm,  SHARED_METER_PAIR_POOL_INDEXf,  1 },
    { SHARED_METER_SET_ENABLEf,   0, FP_POLICY_TABLEm,  SHARED_METER_PAIR_POOL_NUMBERf, 1 },
    { METER_SET_ENABLEf,          0, FP_POLICY_TABLEm,  METER_PAIR_MODE_MODIFIERf },
    { METER_SET_ENABLEf,          0, FP_POLICY_TABLEm,  METER_PAIR_MODEf },
    { METER_SET_ENABLEf,          0, FP_POLICY_TABLEm,  METER_PAIR_POOL_INDEXf,  1 },
    { METER_SET_ENABLEf,          0, FP_POLICY_TABLEm,  METER_PAIR_POOL_NUMBERf, 1 },
    { METER_SET_ENABLEf,          0, FP_POLICY_TABLEm,  USE_SVC_METER_COLORf },
    { SVC_METER_SET_ENABLEf,      0, VFP_POLICY_TABLEm, SVC_METER_INDEXf },
    { SVC_METER_SET_ENABLEf,      0, VFP_POLICY_TABLEm, SVC_METER_OFFSET_MODEf },
    { L3SW_L2_FIELDS_SET_ENABLEf, 0, FP_POLICY_TABLEm,  ECMP_NH_INFOf },
    { L3SW_L2_FIELDS_SET_ENABLEf, 0, FP_POLICY_TABLEm,  G_L3SW_CHANGE_L2_FIELDSf },
    { REDIRECT_SET_ENABLEf,       0, FP_POLICY_TABLEm,  G_PACKET_REDIRECTIONf },
    { REDIRECT_SET_ENABLEf,       0, FP_POLICY_TABLEm,  REDIRECT_NHIf },
    { REDIRECT_SET_ENABLEf,       0, FP_POLICY_TABLEm,  UNICAST_REDIRECT_CONTROLf },
    { COUNTER_SET_ENABLEf,        0, VFP_POLICY_TABLEm, FLEX_CTR_BASE_COUNTER_IDXf },
    { COUNTER_SET_ENABLEf,        0, VFP_POLICY_TABLEm, FLEX_CTR_POOL_NUMBERf },
    { COUNTER_SET_ENABLEf,        0, VFP_POLICY_TABLEm, FLEX_CTR_OFFSET_MODEf, 1 },
    { OAM_SET_ENABLEf,            0, FP_POLICY_TABLEm,  OAM_TUNNEL_CONTROLf },
    { OAM_SET_ENABLEf,            0, FP_POLICY_TABLEm,  OAM_TAG_STATUS_CHECK_CONTROLf },
    { OAM_SET_ENABLEf,            0, FP_POLICY_TABLEm,  OAM_ENABLE_LM_DM_SAMPLEf },
    { OAM_SET_ENABLEf,            0, FP_POLICY_TABLEm,  OAM_UP_MEPf },
    { OAM_SET_ENABLEf,            0, FP_POLICY_TABLEm,  OAM_TXf },
    { OAM_SET_ENABLEf,            0, FP_POLICY_TABLEm,  OAM_LMEP_MDLf },
    { OAM_SET_ENABLEf,            0, FP_POLICY_TABLEm,  OAM_SERVICE_PRI_MAPPING_PTRf },
    { OAM_SET_ENABLEf,            0, FP_POLICY_TABLEm,  OAM_LM_BASE_PTRf },
    { OAM_SET_ENABLEf,            0, FP_POLICY_TABLEm,  OAM_DM_TYPEf },
    { OAM_SET_ENABLEf,            0, FP_POLICY_TABLEm,  OAM_DM_ENf },
    { OAM_SET_ENABLEf,            0, FP_POLICY_TABLEm,  OAM_LM_ENf },
    { OAM_SET_ENABLEf,            0, FP_POLICY_TABLEm,  OAM_LMEP_ENf }
};

STATIC int
_field_tr3_exteral_non_profiled_policy(
    int                      unit,
    _field_entry_t           *f_ent,
    uint32                   *fp_policy_table_entry_data,
    uint32                   *vfp_policy_table_entry_data,
    ext_fp_policy_6x_entry_t *ext_fp_policy_buf,
    soc_mem_t                ext_policy_mem
                                       )
{
    const unsigned slice_num = f_ent->fs->slice_number;

    esm_acl_action_control_entry_t esm_acl_action_control_buf;
    uint32   non_profiled_actions_buf[COUNTOF(ext_fp_policy_buf->entry_data)];
    unsigned ofs, n;
    const struct _field_tr3_external_non_profiled_policy_fld *p;

    /* Check bits in ESM_ACL_ACTION_CONTROL word, and for each set enabled, copy
       the corresponding field(s) from the VFP or IFP policy word into the external
       non-profiled actions
    */

    sal_memset(non_profiled_actions_buf, 0, sizeof(non_profiled_actions_buf));
    ofs = 0;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit,
                                     ESM_ACL_ACTION_CONTROLm,
                                     MEM_BLOCK_ANY,
                                     slice_num,
                                     esm_acl_action_control_buf.entry_data
                                     )
                        );

    for (p = _field_tr3_external_non_profiled_policy_fld_tbl,
             n = COUNTOF(_field_tr3_external_non_profiled_policy_fld_tbl);
         n;
         --n, ++p
         ) {
        if (soc_mem_field32_get(unit,
                                ESM_ACL_ACTION_CONTROLm,
                                esm_acl_action_control_buf.entry_data,
                                p->esm_acl_action_control_fld
                                )
            & BIT(p->bit_num)
            ) {
            unsigned   w;
            SHR_BITDCL val;

            w   = soc_mem_field_length(unit,
                                       p->policy_mem,
                                       p->policy_fld
                                       );
            val = soc_mem_field32_get(unit,
                                      p->policy_mem,
                                      p->policy_mem == FP_POLICY_TABLEm
                                          ? fp_policy_table_entry_data
                                          : vfp_policy_table_entry_data,
                                      p->policy_fld
                                      );

            SHR_BITCOPY_RANGE(non_profiled_actions_buf, ofs, &val, 0, w);
            ofs += w + p->skip_bits;
        }
    }

    soc_mem_field_set(unit,
                      ext_policy_mem,
                      ext_fp_policy_buf->entry_data,
                      NON_PROFILED_ACTIONSf,
                      non_profiled_actions_buf
                      );

    return (BCM_E_NONE);
}


/* Translate an IFP policy table word into profiled fields of EXTFP policy.
   N.B. The order of fields is important -- it is the order of fields in the
   ESM_ACL_PROFILE word, from LSB to MSB.
 */

static const soc_field_t _field_tr3_external_profiled_policy_fld_tbl[] = {
    R_NEW_ECNf,
    Y_NEW_ECNf,
    G_NEW_ECNf,
    R_CHANGE_ECNf,
    Y_CHANGE_ECNf,
    G_CHANGE_ECNf,
    R_NEW_PKT_PRIf,
    Y_NEW_PKT_PRIf,
    G_NEW_PKT_PRIf,
    R_CHANGE_PKT_PRIf,
    Y_CHANGE_PKT_PRIf,
    G_CHANGE_PKT_PRIf,
    R_NEW_DSCPf,
    Y_NEW_DSCPf,
    G_NEW_DSCP_TOSf,
    R_CHANGE_DSCPf,
    Y_CHANGE_DSCPf,
    G_CHANGE_DSCP_TOSf,
    R_COS_INT_PRIf,
    Y_COS_INT_PRIf,
    G_COS_INT_PRIf,
    R_CHANGE_COS_OR_INT_PRIf,
    Y_CHANGE_COS_OR_INT_PRIf,
    G_CHANGE_COS_OR_INT_PRIf
};

STATIC int
_field_tr3_exteral_profiled_policy(
    int                      unit,
    _field_entry_t           *f_ent,
    fp_policy_table_entry_t  *fp_policy_table_buf,
    esm_acl_profile_entry_t  *esm_acl_profile_buf
                                   )
{
    uint32    profiled_actions_buf[COUNTOF(esm_acl_profile_buf->entry_data)];
    unsigned  ofs, n;
    const soc_field_t *p;

    /* Copy profiled actions from IFP policy word */

    sal_memset(profiled_actions_buf, 0, sizeof(profiled_actions_buf));
    ofs = 0;

    for (p = _field_tr3_external_profiled_policy_fld_tbl,
             n = COUNTOF(_field_tr3_external_profiled_policy_fld_tbl);
         n;
         --n, ++p
         ) {
        unsigned   w   = soc_mem_field_length(unit, FP_POLICY_TABLEm, *p);
        SHR_BITDCL val = soc_mem_field32_get(unit,
                                             FP_POLICY_TABLEm,
                                             fp_policy_table_buf->entry_data,
                                             *p
                                             );

        SHR_BITCOPY_RANGE(profiled_actions_buf, ofs, &val, 0, w);
        ofs += w;
    }

    soc_mem_field_set(unit,
                      ESM_ACL_PROFILEm,
                      esm_acl_profile_buf->entry_data,
                      ACTIONSf,
                      profiled_actions_buf
                      );

    return (BCM_E_NONE);
}

/* Set fields that control flex counters in fake VFP policy table entry.
   Copied from _field_tcam_policy_install().
*/

STATIC int
_field_tr3_external_policy_counters(int unit,
                                    _field_entry_t *f_ent,
                                    uint32 *vfp_policy_table_entry_data,
                                    bcm_stat_flex_mode_t *offset_mode,
                                    uint32 *pool_number,
                                    uint32 *base_index
                                    )
{
    bcm_stat_object_t     object          = bcmStatObjectIngPort;
    bcm_stat_group_mode_t group_mode      = bcmStatGroupModeSingle;
    _field_stat_t         *f_st           = NULL;
    _field_entry_stat_t   *f_ent_st       = NULL;

    f_ent_st = &f_ent->statistic;
    if (_bcm_field_stat_get(unit, f_ent_st->sid, &f_st) == BCM_E_NONE
        && f_st->flex_mode != 0
        ) {
        _bcm_esw_stat_get_counter_id_info(unit, f_st->flex_mode,
                                          &group_mode,
                                          &object,
                                          offset_mode,
                                          pool_number,
                                          base_index
                                          );
        BCM_IF_ERROR_RETURN(_bcm_esw_set_flex_counter_fields_values(
                                unit,0,
                                VFP_POLICY_TABLEm,
                                vfp_policy_table_entry_data,
                                0,
                                *offset_mode,
                                *pool_number,
                                *base_index
                                                                    )
                            );
    }

    return (BCM_E_NONE);
}

/* Set fields that control service meters in fake VFP policy table entry.
   Copied from _bcm_field_tr_entry_install().
*/

STATIC int
_field_tr3_external_policy_meters(int            unit,
                                  _field_entry_t *f_ent,
                                  uint32         *vfp_policy_table_entry_data
                                  )
{
    int offset_mode = 0;
    int index       = 0;

    if (f_ent->global_meter_policer.pid > 0) {
        offset_mode = ((f_ent->global_meter_policer.pid & BCM_POLICER_GLOBAL_METER_MODE_MASK)
                       >> BCM_POLICER_GLOBAL_METER_MODE_SHIFT
                       );
        if (offset_mode >= 1) {
            offset_mode = offset_mode - 1;
        }
        soc_mem_field32_set(unit,
                            VFP_POLICY_TABLEm,
                            vfp_policy_table_entry_data,
                            SVC_METER_OFFSET_MODEf,
                            offset_mode
                            );

        _bcm_esw_get_policer_table_index(unit,
                                         f_ent->global_meter_policer.pid,
                                         &index
                                         );
        soc_mem_field32_set(unit,
                            VFP_POLICY_TABLEm,
                            vfp_policy_table_entry_data,
                            SVC_METER_INDEXf,
                            index
                            );
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_esw_tr3_ext_stat_flex_detach_ingress_table_counters
 * Purpose:
 *     Detach i.e. Update Ingress accounting table's statistics
 * Parameters:
 *     unit     - (IN) unit
 *     f_ent    - (IN) Field entry 
 * Returns:
 *     BCM_E_NONE     - Success
 * Notes:
 */                                                        
int
_bcm_esw_tr3_ext_stat_flex_detach_ingress_table_counters(int unit, 
                                                         _field_entry_t *f_ent)
{
    bcm_stat_object_t object = bcmStatObjectIngPort;
    bcm_stat_group_mode_t group_mode      = bcmStatGroupModeSingle;
    bcm_stat_flex_mode_t offset_mode;
    uint32 pool_number;
    uint32 base_index;
    int rv = BCM_E_NONE;
    _field_stat_t         *f_st           = NULL;
    _field_entry_stat_t   *f_ent_st       = NULL;

    f_ent_st = &f_ent->statistic;
    if (_bcm_field_stat_get(unit, f_ent_st->sid, &f_st) == BCM_E_NONE
        && f_st->flex_mode != 0
        ) {

        _bcm_esw_stat_get_counter_id_info(unit, f_st->flex_mode,
                                          &group_mode,
                                          &object,
                                          &offset_mode,
                                          &pool_number,
                                          &base_index
                                          );
        rv = _bcm_esw_stat_flex_detach_ingress_table_counters_update(
                        unit, pool_number, base_index, offset_mode);
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}

/* Policy install for external stage
   The general idea is to compose a policy table entry as for another stage
   (lookup or ingress), and the translate that into the non-profiled and
   profiled portions for the external stage.
   (This can be done because the EXTFP policy table is a subset of the VFP [flex
   counters and service meters] and IFP policy tables [everything else], with
   policy table fields encoded the same way).
 */

STATIC int
_field_tr3_external_policy_install(int            unit,
                                   _field_stage_t *stage_fc,
                                   _field_entry_t *f_ent
                                   )
{
    const unsigned  slice_num  = f_ent->fs->slice_number;

    soc_mem_t                ext_policy_mem;
    fp_policy_table_entry_t  fp_policy_table_buf;
    vfp_policy_table_entry_t vfp_policy_table_buf;
    ext_fp_policy_6x_entry_t ext_fp_policy_buf;
    esm_acl_profile_entry_t  esm_acl_profile_buf;
    uint32                   ext_act_profile_index;
    _field_action_t *fa;
    void            *entries[1];
    _field_stat_t  *f_st;
    bcm_stat_flex_mode_t offset_mode;
    uint32 pool_number;
    uint32 base_index;

    BCM_IF_ERROR_RETURN(_field_tr3_external_policy_mem(unit,
                                                       slice_num,
                                                       &ext_policy_mem
                                                       )
                        );

    sal_memset(ext_fp_policy_buf.entry_data,
               0,
               sizeof(ext_fp_policy_buf.entry_data)
               );
    sal_memset(esm_acl_profile_buf.entry_data,
               0,
               sizeof(esm_acl_profile_buf.entry_data)
               );

    /* For all actions, compose the policy entry as for IFP */

    sal_memset(fp_policy_table_buf.entry_data,
               0,
               sizeof(fp_policy_table_buf.entry_data)
               );

    for (fa = f_ent->actions; fa != NULL; fa = fa->next) {

        /* skip invalid actions. */
        if (!(_FP_ACTION_VALID & fa->flags)) {
            continue;
        }

        BCM_IF_ERROR_RETURN(_bcm_field_trx_action_get(
                                unit,
                                FP_POLICY_TABLEm,
                                f_ent,
                                f_ent->slice_idx,
                                fa,
                                fp_policy_table_buf.entry_data
                                                      )
                            );
    }

    if (f_ent->flags & _FP_ENTRY_COLOR_INDEPENDENT) {
        soc_mem_field32_set(unit,
                            FP_POLICY_TABLEm,
                            fp_policy_table_buf.entry_data,
                            GREEN_TO_PIDf,
                            1
                            );
    }

    /* Compose a VFP policy table entry for flex counters and service meters */

    sal_memset(vfp_policy_table_buf.entry_data,
               0,
               sizeof(vfp_policy_table_buf.entry_data)
               );
    BCM_IF_ERROR_RETURN(_field_tr3_external_policy_counters(
                            unit,
                            f_ent,
                            vfp_policy_table_buf.entry_data,
                            &offset_mode,
                            &pool_number,
                            &base_index
                                                            )
                        );
    BCM_IF_ERROR_RETURN(_field_tr3_external_policy_meters(
                            unit,
                            f_ent,
                            vfp_policy_table_buf.entry_data
                                                            )
                        );

    /* Extract non-profiled external fields from IFP and VFP policy entries */

    BCM_IF_ERROR_RETURN(_field_tr3_exteral_non_profiled_policy(
                            unit,
                            f_ent,
                            fp_policy_table_buf.entry_data,
                            vfp_policy_table_buf.entry_data,
                            &ext_fp_policy_buf,
                            ext_policy_mem));

    if (f_ent->statistic.flags & _FP_ENTRY_STAT_INSTALLED) {
        BCM_IF_ERROR_RETURN(_bcm_field_stat_get(unit, f_ent->statistic.sid, &f_st));
        f_st->hw_index = f_ent->slice_idx;
        f_st->pool_index = f_ent->fs->slice_number;

        BCM_IF_ERROR_RETURN
            (_bcm_esw_stat_flex_attach_ingress_table_counters_update(
                            unit, pool_number, 0,
                            base_index, offset_mode));
    }
                                                            
    soc_mem_field32_set(unit,
                        ext_policy_mem,
                        ext_fp_policy_buf.entry_data,
                        POLICY_TABLE_IDf,
                        slice_num
                        );

    /* Extract profiled external field from IFP policy */

    BCM_IF_ERROR_RETURN(_field_tr3_exteral_profiled_policy(
                            unit,
                            f_ent,
                            &fp_policy_table_buf,
                            &esm_acl_profile_buf
                                                           )
                        );
    entries[0] = esm_acl_profile_buf.entry_data;
    BCM_IF_ERROR_RETURN (soc_profile_mem_add(unit,
                                             &stage_fc->ext_act_profile,
                                             entries,
                                             1,
                                             &ext_act_profile_index)
                         );
    f_ent->ext_act_profile_idx = ext_act_profile_index;

    soc_mem_field32_set(unit,
                        ext_policy_mem,
                        ext_fp_policy_buf.entry_data,
                        PROFILE_IDf,
                        ext_act_profile_index
                        );

    /* Write into the Table */
    return (soc_mem_write(unit,
                          ext_policy_mem,
                          MEM_BLOCK_ALL,
                          f_ent->slice_idx,
                          ext_fp_policy_buf.entry_data
                          )
            );
}


/*
 * Function:
 *     _bcm_field_tr3_external_hw_entry_clear
 * Purpose:
 *     Clears an ACL h/w entry
 * Parameters:
 *     unit     - (IN) unit
 *     slice    - (IN) slice having the index to be cleared
 *     index    - (IN) index to be cleared
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_XXX
 */
int
_bcm_field_tr3_external_hw_entry_clear(int unit, int slice, int index)
{
    soc_mem_t tcam_data_mem, tcam_mask_mem, policy_mem;
    uint32 buf[SOC_MAX_MEM_FIELD_WORDS];
    int rv = BCM_E_NONE;

    sal_memset(buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Get Policy mem, data mem and mask mem  */
    BCM_IF_ERROR_RETURN(_field_tr3_external_policy_mem(unit, slice,
                                                       &policy_mem));
    BCM_IF_ERROR_RETURN(_field_tr3_external_tcam_data_mem(unit, slice,
                                                       &tcam_data_mem));
    BCM_IF_ERROR_RETURN(_field_tr3_external_tcam_mask_mem(unit, slice,
                                                       &tcam_mask_mem));

    rv = soc_mem_write(unit, tcam_data_mem, MEM_BLOCK_ALL, index, buf);
    BCM_IF_ERROR_RETURN(rv);

    if (tcam_mask_mem != INVALIDm) {
        /* For all valid tcam mask mems, only index 0 is valid */
        rv = soc_mem_write(unit, tcam_mask_mem, MEM_BLOCK_ALL, 0, buf);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (policy_mem != INVALIDm) {
        rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, index, buf);
        BCM_IF_ERROR_RETURN(rv);
    }

    return (rv);
}


/*
 * Function:
 *     _bcm_field_tr3_external_entry_move
 * Purpose:
 *     Moves an external entry
 * Parameters:
 *     unit     - (IN) unit
 *     f_ent    - (IN) Field entry to be moved
 *     index1   - (IN) original index
 *     index2   - (IN) destination index
 * Returns:
 *     BCM_E_NONE     - Success
 * Notes:
 *     It is the calling function's responsibility to
 *         update the prio_mgmt structure.
 */                                                        
int
_bcm_field_tr3_external_entry_move(int unit, _field_entry_t *f_ent,
                                   int index1, int index2)
{
    _field_stage_t *stage_fc;           /* Stage information */
    uint32  policy[SOC_MAX_MEM_FIELD_WORDS]; /* policy info */
    soc_mem_t policy_mem;               /* policy table memory */
    int rv;                             /* Operation return status */

    /* Check if entry move is required. */
    if (index1 == index2) {
        return (BCM_E_NONE);
    }

    if (f_ent == NULL ) {
        return (BCM_E_NONE);
    }

    rv = _field_stage_control_get(unit, f_ent->fs->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear TCAM at index2 */
    _bcm_field_tr3_external_hw_entry_clear(unit, 
                              f_ent->fs->slice_number, index2);

    /* Get Policy mem */
    BCM_IF_ERROR_RETURN(_field_tr3_external_policy_mem(unit, 
                              f_ent->fs->slice_number, &policy_mem));

    /* Read EXT_FP_POLICY from index1 */
    rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY, index1, policy);
    BCM_IF_ERROR_RETURN(rv);

    /* Write EXT_FP_POLICY to index2 */
    rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, index2, policy);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_tr3_external_tcam_write(unit, f_ent, index2);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear TCAM at index1 */
    _bcm_field_tr3_external_hw_entry_clear(unit, 
                              f_ent->fs->slice_number, index1);

    /* Update the software */
    f_ent->fs->entries[index1] = NULL;
    f_ent->slice_idx = index2;

    f_ent->fs->entries[index2] = f_ent;

    return (rv);
}


STATIC int
_field_tr3_external_entry_clear(int unit, int slice, int index)
{
    soc_mem_t ext_tcam_mem;

    /* Clear TCAM entry valid bit */

    BCM_IF_ERROR_RETURN(_field_tr3_external_tcam_data_mem(unit, slice, &ext_tcam_mem));
    return (soc_tr3_set_vbit(unit, ext_tcam_mem, index, 0));
}

STATIC int
_field_tr3_external_entry_install(int unit, _field_entry_t *f_ent)
{
    _field_stage_t *stage_fc;
    uint32 new_location;

    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, f_ent->fs->stage_id, &stage_fc));

    BCM_IF_ERROR_RETURN(_bcm_field_entry_target_location(unit,
                stage_fc, f_ent, f_ent->prio, &new_location));

    f_ent->slice_idx = new_location;
    f_ent->fs->entries[new_location] = f_ent;

    BCM_IF_ERROR_RETURN
        (_bcm_field_entry_prio_mgmt_update(unit, f_ent, 1, 0));

    /* Write EXT_FP_POLICY */
    _field_tr3_external_policy_install(unit, stage_fc, f_ent);

    /* Write TCAM */
    _field_tr3_external_tcam_write(unit, f_ent, f_ent->slice_idx);

    return (BCM_E_NONE);
}


STATIC int
_field_tr3_external_entry_reinstall(int unit, _field_entry_t *f_ent)
{
    soc_profile_mem_t   ext_act_profile;
    _field_stage_t     *stage_fc;
    uint32              ext_act_profile_index = 0;

    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, f_ent->fs->stage_id, &stage_fc));

    /* Store the existing profile index so that it can be cleared later.
       When reinstall API is called, it is assumed that the priority is not
       changed. When priority is modified, then a separate API is used since
       change in priority will affect the ordering in TCAM. This API should
       modify only the policy table and not the TCAM table
     */

    ext_act_profile = stage_fc->ext_act_profile;
    ext_act_profile_index = f_ent->ext_act_profile_idx;

    /* Write EXT_FP_POLICY */
    BCM_IF_ERROR_RETURN
        (_field_tr3_external_policy_install(unit, stage_fc, f_ent));

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_delete(unit, &ext_act_profile, ext_act_profile_index));

    return (BCM_E_NONE);
}



STATIC int
_field_tr3_external_entry_remove(int unit, _field_entry_t *f_ent)
{
    _field_stage_t           *stage_fc;
    soc_mem_t                ext_policy_mem;
    ext_fp_policy_6x_entry_t ext_fp_policy_buf;

    BCM_IF_ERROR_RETURN(_field_tr3_external_policy_mem(unit,
                                                       f_ent->fs->slice_number,
                                                       &ext_policy_mem
                                                       )
                        );
    SOC_IF_ERROR_RETURN(soc_mem_read(unit,
                                     ext_policy_mem,
                                     MEM_BLOCK_ANY,
                                     f_ent->slice_idx,
                                     ext_fp_policy_buf.entry_data
                                     )
                        );

    /* Release the PROFILE_PTR for EXT_IFP_ACTION_PROFILEm */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                                                 f_ent->fs->stage_id,
                                                 &stage_fc
                                                 )
                        );
    BCM_IF_ERROR_RETURN(soc_profile_mem_delete(
                            unit,
                            &stage_fc->ext_act_profile,
                            soc_mem_field32_get(
                                 unit,
                                 ext_policy_mem,
                                 ext_fp_policy_buf.entry_data,
                                 PROFILE_IDf
                                                )
                                               )
                        );

    /* Clear the H/W entry */
    _field_tr3_external_entry_clear(unit,
                                    f_ent->fs->slice_number,
                                    f_ent->slice_idx
                                    );

    /* Clear the H/W replica */
    sal_memset(f_ent->tcam.key_hw, 0, f_ent->tcam.key_size);
    sal_memset(f_ent->tcam.mask_hw, 0, f_ent->tcam.key_size);

    f_ent->fs->entries[f_ent->slice_idx] = NULL;

    BCM_IF_ERROR_RETURN
        (_bcm_field_entry_prio_mgmt_update(unit, f_ent, -1, f_ent->slice_idx));

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_tr3_functions_init
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
_field_tr3_functions_init(_field_funct_t *functions)
{
    functions->fp_detach               = _bcm_field_tr_detach;
    functions->fp_group_install        = _field_tr3_group_install;
    functions->fp_selcodes_install     = _field_tr3_selcodes_install;
    functions->fp_slice_clear          = _field_tr3_slice_clear;
    functions->fp_entry_remove         = _bcm_field_fb_entry_remove;
    functions->fp_entry_move           = _bcm_field_fb_entry_move;
    functions->fp_selcode_get          = _field_tr3_selcode_get;
    functions->fp_selcode_to_qset      = _bcm_field_selcode_to_qset;
    functions->fp_qual_list_get        = _bcm_field_qual_lists_get;
    functions->fp_tcam_policy_clear    = NULL;
    functions->fp_tcam_policy_install  = _bcm_field_tr_entry_install;
    functions->fp_tcam_policy_reinstall = _bcm_field_tr_entry_reinstall;
    functions->fp_policer_install      = _bcm_field_trx_policer_install;
    functions->fp_write_slice_map      = _bcm_field_tr3_write_slice_map;
    functions->fp_qualify_ip_type      = _bcm_field_trx_qualify_ip_type;
    functions->fp_qualify_ip_type_get  = _bcm_field_trx_qualify_ip_type_get;
    functions->fp_action_support_check = _field_tr3_action_support_check;
    functions->fp_action_conflict_check = _bcm_field_trx_action_conflict_check;
    functions->fp_counter_get          = _field_tr3_counter_get;
    functions->fp_counter_set          = _field_tr3_counter_set;
    functions->fp_stat_index_get       = _field_tr3_stat_index_get;
    functions->fp_action_params_check  = _field_tr3_action_params_check;
    functions->fp_action_depends_check = NULL;
    functions->fp_egress_key_match_type_set =
        _field_tr3_egress_key_match_type_set;
    functions->fp_external_entry_install   = _field_tr3_external_entry_install;
    functions->fp_external_entry_reinstall = _field_tr3_external_entry_reinstall;
    functions->fp_external_entry_remove    = _field_tr3_external_entry_remove;
    functions->fp_external_entry_prio_set  = _bcm_field_tr_external_entry_prio_set;
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
    functions->fp_qualify_packet_res = _bcm_field_tr3_qualify_PacketRes;
    functions->fp_qualify_packet_res_get = _bcm_field_tr3_qualify_PacketRes_get;
}

int
_bcm_field_tr3_meter_pool_info(int            unit,
                               _field_stage_t *stage_fc,
                               int            *num_pools,
                               int            *pairs_per_pool,
                               uint16         *pool_size
                               )
{
    switch (stage_fc->stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
        *num_pools      = NUM_METER_POOLS_IFP;
        *pairs_per_pool = NUM_METER_PAIRS_PER_POOL_IFP;
        *pool_size      = NUM_METER_PAIRS_PER_POOL_IFP * 2;

        if (soc_feature(unit, soc_feature_field_stage_quarter_slice)) {
            *pairs_per_pool = NUM_METER_PAIRS_PER_POOL_IFP / 4;
        } else if (soc_feature(unit, soc_feature_field_stage_half_slice)) {
            *pairs_per_pool = NUM_METER_PAIRS_PER_POOL_IFP / 2;
        }

#if defined(INCLUDE_REGEX)
        /* If Regex supported, reserve the last meter pool for its use. */
        if (soc_feature(unit, soc_feature_regex)) {
            *num_pools = NUM_METER_POOLS_IFP - 1;
        }
#endif

#if defined(BCM_HELIX4_SUPPORT)
        if (SOC_IS_HELIX4(unit)) {
            *pool_size      =  (*pool_size) / 2;
            *pairs_per_pool =  (*pairs_per_pool) / 2;
        }
#endif
        break;
    default:
        *num_pools = *pairs_per_pool = 0;
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *    _bcm_field_tr3_logical_meter_pool_info
 * Purpose:
 *     Get Logical Meter Pool Info
 * Parameters:
 *     unit                   - (IN)  BCM device number
 *     stage_fc               - (IN)  Field Processor stage control structure.
 *     num_logical_pools      - (OUT) Number of logical pools.
 *     pairs_per_logical_pool - (OUT) Number of meter pairs per logical pool.
 *     logical_pool_size      - (OUT) Logical Pool Size.
 * Returns:
 *     BCM_E_NONE
 */
int
_bcm_field_tr3_logical_meter_pool_info(int            unit,
                                      _field_stage_t *stage_fc,
                                      int            *num_logical_pools,
                                      int            *pairs_per_logical_pool,
                                      uint16         *logical_pool_size
                                      )
{
    switch (stage_fc->stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
        *num_logical_pools      = NUM_LOGICAL_METER_POOLS_IFP;
        *pairs_per_logical_pool = NUM_LOGICAL_METER_PAIRS_PER_POOL_IFP;
        *logical_pool_size      = NUM_LOGICAL_METER_PAIRS_PER_POOL_IFP * 2;

        if (soc_feature(unit, soc_feature_field_stage_quarter_slice)) {
            *pairs_per_logical_pool = NUM_LOGICAL_METER_PAIRS_PER_POOL_IFP / 4;
        } else if (soc_feature(unit, soc_feature_field_stage_half_slice)) {
            *pairs_per_logical_pool = NUM_LOGICAL_METER_PAIRS_PER_POOL_IFP / 2;
        }

#if defined(INCLUDE_REGEX)
        /* If Regex supported, reserve the last meter pool for its use. */
        if (soc_feature(unit, soc_feature_regex)) {
            *num_logical_pools = NUM_LOGICAL_METER_POOLS_IFP - 2;
        }
#endif

#if defined(BCM_HELIX4_SUPPORT)
        if (SOC_IS_HELIX4(unit)) {
            *logical_pool_size      =  (*logical_pool_size) / 2;
            *pairs_per_logical_pool =  (*pairs_per_logical_pool) / 2;
        }
#endif
        break;
    default:
        *num_logical_pools = *pairs_per_logical_pool = 0;
    }

    return (BCM_E_NONE);
}

int
_bcm_field_tr3_cntr_pool_info(int            unit,
                              _field_stage_t *stage_fc,
                              unsigned       *num_pools,
                              unsigned       *pairs_per_pool
                              )
{
    switch (stage_fc->stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:

        *num_pools      = NUM_CNTR_POOLS_IFP;
        if (stage_fc->flags & _FP_STAGE_QUARTER_SLICE) {
            *pairs_per_pool =  (NUM_CNTRS_PER_POOL_IFP >> 2);
        } else if (stage_fc->flags & _FP_STAGE_HALF_SLICE) {
            *pairs_per_pool =  (NUM_CNTRS_PER_POOL_IFP >> 1);
        } else {
            *pairs_per_pool = NUM_CNTRS_PER_POOL_IFP;
        }
#if defined(BCM_HELIX4_SUPPORT)
        if (SOC_IS_HELIX4(unit)) {
            *pairs_per_pool =  (*pairs_per_pool) >> 1;
        }
#endif
        break;
    default:
        *num_pools = *pairs_per_pool = 0;
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_field_tr3_external_ip_proto_init
 * Purpose:
 *     Initialize the ESM_L3_PROTOCOL_FN table
 * Parameters:
 *     unit       - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_field_tr3_external_ip_proto_init(int unit)
{
    int index_min, index_max, i, rv = BCM_E_NONE;
    esm_l3_protocol_fn_entry_t l3_proto_fn_entry;
    enum ip_proto_map_e {
        _IpProtoTcp = 1,
        _IpProtoUdp,
        _IpProtoIcmp,
        _IpProtoIgmp,
        _IpProtoIpv4,
        _IpProtoIpv6,
        _IpProtoMpls
    };

    if (SOC_WARM_BOOT(unit)) {
        return (rv);
    }

    sal_memset(&l3_proto_fn_entry, 0, sizeof(esm_l3_protocol_fn_entry_t));

    /* IpProtocol used to index ESM_L3_PROTOCOL_FN table is mapped to 
       ip_proto_map_t values. */
    index_min = soc_mem_index_min(unit, ESM_L3_PROTOCOL_FNm);
    index_max = soc_mem_index_max(unit, ESM_L3_PROTOCOL_FNm);
    for (i = index_min; i <= index_max;  i++) {
        switch (i) {
            case IP_PROTO_TCP:
                soc_mem_field32_set(unit, ESM_L3_PROTOCOL_FNm, 
                                    &l3_proto_fn_entry, L3_PROTOCOL_FNf,
                                    _IpProtoTcp);
                BCM_IF_ERROR_RETURN(soc_mem_write(unit, ESM_L3_PROTOCOL_FNm, 
                                        MEM_BLOCK_ALL, i, &l3_proto_fn_entry));
                break;
            case IP_PROTO_UDP:
                soc_mem_field32_set(unit, ESM_L3_PROTOCOL_FNm, 
                                    &l3_proto_fn_entry, L3_PROTOCOL_FNf,
                                    _IpProtoUdp);
                BCM_IF_ERROR_RETURN(soc_mem_write(unit, ESM_L3_PROTOCOL_FNm, 
                                        MEM_BLOCK_ALL, i, &l3_proto_fn_entry));
                break;
            case IP_PROTO_ICMP:
                soc_mem_field32_set(unit, ESM_L3_PROTOCOL_FNm, 
                                    &l3_proto_fn_entry, L3_PROTOCOL_FNf,
                                    _IpProtoIcmp);
                BCM_IF_ERROR_RETURN(soc_mem_write(unit, ESM_L3_PROTOCOL_FNm, 
                                        MEM_BLOCK_ALL, i, &l3_proto_fn_entry));
                break;
            case IP_PROTO_IGMP:
                soc_mem_field32_set(unit, ESM_L3_PROTOCOL_FNm, 
                                    &l3_proto_fn_entry, L3_PROTOCOL_FNf,
                                    _IpProtoIgmp);
                BCM_IF_ERROR_RETURN(soc_mem_write(unit, ESM_L3_PROTOCOL_FNm, 
                                        MEM_BLOCK_ALL, i, &l3_proto_fn_entry));
                break;
            case IP_PROTO_IPv4:
                soc_mem_field32_set(unit, ESM_L3_PROTOCOL_FNm, 
                                    &l3_proto_fn_entry, L3_PROTOCOL_FNf,
                                    _IpProtoIpv4);
                BCM_IF_ERROR_RETURN(soc_mem_write(unit, ESM_L3_PROTOCOL_FNm, 
                                        MEM_BLOCK_ALL, i, &l3_proto_fn_entry));
                break;
            case IP_PROTO_IPv6:
                soc_mem_field32_set(unit, ESM_L3_PROTOCOL_FNm, 
                                    &l3_proto_fn_entry, L3_PROTOCOL_FNf,
                                    _IpProtoIpv6);
                BCM_IF_ERROR_RETURN(soc_mem_write(unit, ESM_L3_PROTOCOL_FNm, 
                                        MEM_BLOCK_ALL, i, &l3_proto_fn_entry));
                break;
            case IP_PROTO_MPLS:
                soc_mem_field32_set(unit, ESM_L3_PROTOCOL_FNm, 
                                    &l3_proto_fn_entry, L3_PROTOCOL_FNf,
                                    _IpProtoMpls);
                BCM_IF_ERROR_RETURN(soc_mem_write(unit, ESM_L3_PROTOCOL_FNm, 
                                        MEM_BLOCK_ALL, i, &l3_proto_fn_entry));
                break;
            default:
                soc_mem_field32_set(unit, ESM_L3_PROTOCOL_FNm, 
                                    &l3_proto_fn_entry, L3_PROTOCOL_FNf, 0xf);
                BCM_IF_ERROR_RETURN(soc_mem_write(unit, ESM_L3_PROTOCOL_FNm,
                                        MEM_BLOCK_ALL, i, &l3_proto_fn_entry));
                break;
        }
    }
    return (rv);
}


/*
 * Function:
 *     _bcm_field_tr3_external_ip_proto_deinit
 * Purpose:
 *     Reset the ESM_L3_PROTOCOL_FN table fields initilized by corresponding 
 *     init function
 * Parameters:
 *     unit       - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_field_tr3_external_ip_proto_deinit(int unit)
{
    int rv = BCM_E_NONE;

    if (SOC_WARM_BOOT(unit)) {
        return (rv);
    }

    rv = soc_mem_clear((unit), ESM_L3_PROTOCOL_FNm, COPYNO_ALL, TRUE);
    BCM_IF_ERROR_RETURN(rv);

    return (rv);
}


STATIC int
_field_tr3_external_init(int unit, _field_stage_t *stage_fc)
{
    int i, size, total_size = 0;
    soc_mem_t mem;
    int entry_words;
    _field_slice_t *fs;
    int alloc_size;
    int rv;

    /* Read the sizes for various external databases */
    for (i = 0; i < stage_fc->tcam_slices; i++) {
        /* slice i is valid for a particular pkt_type (L2, IPV4, IPV6) */
        fs = stage_fc->slices[_FP_DEF_INST] + i;
        switch (i) {
          case _FP_EXT_ACL_144_L2:
          case _FP_EXT_ACL_L2:
              fs->pkt_type[_FP_EXT_L2] = 1;
              fs->pkt_type[_FP_EXT_IP4] = 0;
              fs->pkt_type[_FP_EXT_IP6] = 0;
              break;
          case _FP_EXT_ACL_144_IPV4:
          case _FP_EXT_ACL_IPV4:
          case _FP_EXT_ACL_L2_IPV4:
              fs->pkt_type[_FP_EXT_L2] = 0;
              fs->pkt_type[_FP_EXT_IP4] = 1;
              fs->pkt_type[_FP_EXT_IP6] = 0;
              break;
          case _FP_EXT_ACL_144_IPV6:
          case _FP_EXT_ACL_IPV6_SHORT:
          case _FP_EXT_ACL_IPV6_FULL:
          case _FP_EXT_ACL_L2_IPV6:
              fs->pkt_type[_FP_EXT_L2] = 0;
              fs->pkt_type[_FP_EXT_IP4] = 0;
              fs->pkt_type[_FP_EXT_IP6] = 1;
              break;
          default:
              continue;
        }
        BCM_PBMP_CLEAR(fs->ext_pbmp[_FP_EXT_L2]);
        BCM_PBMP_CLEAR(fs->ext_pbmp[_FP_EXT_IP4]);
        BCM_PBMP_CLEAR(fs->ext_pbmp[_FP_EXT_IP6]);

        /* We use policy_mem as it is in one array as compared to data_mems */
        BCM_IF_ERROR_RETURN(_field_tr3_external_tcam_data_mem(unit,
                                                              i,
                                                              &mem
                                                              )
                           );
        size = soc_mem_index_count(unit, mem);
        fs->entry_count = size;
        fs->free_count  = size;

#ifdef BCM_WARM_BOOT_SUPPORT
        {
            int stable_size;

            SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

            if (stable_size > 0 && !SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
                /* Level-2 warm boot supported */

                /* The number of entries need to be adjusted, to allow for space for
                   storing scache information for FP ESM level-2 warm boot.
                   We carve off some portion of each paritition for this storage
                   area.
                */

                fs->entry_count -= _bcm_esw_field_tr2_ext_scache_size(unit, i);
                fs->free_count  =  fs->entry_count;
            }
        }
#endif /* BCM_WARM_BOOT_SUPPORT */

        if (size) {
            alloc_size = size * sizeof (_field_entry_t *);
            _FP_XGS3_ALLOC(fs->entries, alloc_size, "array of external entry pointers");
            if (NULL == fs->entries) {
                _bcm_field_stage_entries_free(unit, stage_fc);
                return (BCM_E_MEMORY);
            }
            total_size += size;
        }
    }
    stage_fc->tcam_sz = total_size;

    /* Initialize ESM_L3_PROTOCOL_FN table */
    BCM_IF_ERROR_RETURN(_bcm_field_tr3_external_ip_proto_init(unit));

    /* Initialize the profiles */
    /*
     * redirect_profile will be used from IFP;
     *     both IFP and External TCAM share the same profile
     */
    mem = ESM_ACL_PROFILEm;
    entry_words = sizeof(esm_acl_profile_entry_t) / sizeof(uint32);
    rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                &stage_fc->ext_act_profile);
    if (BCM_FAILURE(rv)) {
        _bcm_field_stage_entries_free(unit, stage_fc);
        return (rv);
    }

    /* Set the Seed-ID for range creation */
    stage_fc->range_id = 1;

    /* Initialize the prio_mgmt structure */
    rv = _bcm_field_prio_mgmt_init(unit, stage_fc);
    if (BCM_FAILURE(rv)) {
        _bcm_field_stage_entries_free(unit, stage_fc);
        return (rv);
    }

    return (BCM_E_NONE);
}

#ifdef BCM_HELIX4_SUPPORT
/*
 * Function:
 * _bcm_field_helix4_stage_init
 * Purpose: Initialize device stage wise flags
 * Parameters:
 * unit - (IN) BCM device number.
 * stage_fc - (IN) Field Processor stage control structure.
 * Returns: BCM_E_NONE
 */
int
_bcm_field_helix4_stage_init(int unit, _field_stage_t *stage_fc)
{

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    switch (stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_LOOKUP:
            /* Flags */
            stage_fc->flags |= (_FP_STAGE_SLICE_ENABLE | _FP_STAGE_AUTO_EXPANSION
                            | _FP_STAGE_GLOBAL_COUNTERS);
            /* Slice geometry */
            stage_fc->tcam_sz     = soc_mem_index_count(unit, VFP_TCAMm);
            stage_fc->tcam_slices = 4;

            if (soc_feature(unit, soc_feature_field_stage_half_slice)) {
                stage_fc->flags |= _FP_STAGE_HALF_SLICE;
            }

            break;

        case _BCM_FIELD_STAGE_INGRESS:
            /* Flags */
            stage_fc->flags |= _FP_STAGE_SLICE_ENABLE
                            | _FP_STAGE_GLOBAL_METER_POOLS
                            | _FP_STAGE_GLOBAL_CNTR_POOLS
                            | _FP_STAGE_SEPARATE_PACKET_BYTE_COUNTERS
                            | _FP_STAGE_AUTO_EXPANSION;
            /* Slice geometry */
            stage_fc->tcam_sz     = soc_mem_index_count(unit, FP_TCAMm);
            stage_fc->tcam_slices = 16;

            if (soc_feature(unit, soc_feature_field_stage_half_slice)) {
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

            if (soc_feature(unit, soc_feature_field_stage_half_slice)) {
                stage_fc->flags |= _FP_STAGE_HALF_SLICE;
            }

            break;

        default:
        return (BCM_E_INTERNAL);

    }

    return (BCM_E_NONE);
}
#endif /* BCM_HELIX4_SUPPORT */

int
_bcm_field_tr3_stage_init(int unit, _field_stage_t *stage_fc)
{
    /* Input prameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    switch (stage_fc->stage_id) {
    case _BCM_FIELD_STAGE_LOOKUP:
        /* Flags */
        stage_fc->flags |= (_FP_STAGE_SLICE_ENABLE | _FP_STAGE_AUTO_EXPANSION
                            | _FP_STAGE_GLOBAL_COUNTERS);
        /* Slice geometry */
        stage_fc->tcam_sz     = soc_mem_index_count(unit, VFP_TCAMm);
        stage_fc->tcam_slices = 4;

        if (soc_feature(unit, soc_feature_field_stage_quarter_slice)) {
            stage_fc->flags |= _FP_STAGE_QUARTER_SLICE;
        }

        break;
    case _BCM_FIELD_STAGE_INGRESS:
        /* Flags */
        stage_fc->flags
            |= _FP_STAGE_SLICE_ENABLE
            | _FP_STAGE_GLOBAL_METER_POOLS
            | _FP_STAGE_GLOBAL_CNTR_POOLS
            | _FP_STAGE_SEPARATE_PACKET_BYTE_COUNTERS
            | _FP_STAGE_AUTO_EXPANSION;
        /* Slice geometry */
        stage_fc->tcam_sz     = soc_mem_index_count(unit, FP_TCAMm);
        stage_fc->tcam_slices = 16;

        if (soc_feature(unit, soc_feature_field_stage_half_slice)) {
            stage_fc->flags |= _FP_STAGE_HALF_SLICE;
        }

        if (soc_feature(unit, soc_feature_field_stage_quarter_slice)) {
            stage_fc->flags &= ~(_FP_STAGE_HALF_SLICE);
            stage_fc->flags |= _FP_STAGE_QUARTER_SLICE;
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

        /* 
         * Among the the triumph3 variants,Only Ranger+ has holes in EFP.
         * Double check on half slice and quarter slice is to distinguish 
         * Ranger+ from other variants as Ranger+ is the only 
         * triumph3 variant having both quarter slice and half slice fatures 
         * enabled  
         */ 
        if (soc_feature(unit, soc_feature_field_stage_quarter_slice)) {
            if (soc_feature(unit, soc_feature_field_stage_half_slice)) {
                stage_fc->flags |= _FP_STAGE_HALF_SLICE;
            }
        }
  
        break;
    case _BCM_FIELD_STAGE_EXTERNAL:
        {
            unsigned  slice_num, n;
            soc_mem_t tcam_data_mem;

            stage_fc->tcam_slices = _FP_EXT_NUM_PARTITIONS;
            for (stage_fc->tcam_sz = slice_num = 0, n = stage_fc->tcam_slices;
                 n;
                 --n, ++slice_num
                 ) {
                BCM_IF_ERROR_RETURN(_field_tr3_external_tcam_data_mem(
                                        unit,
                                        slice_num,
                                        &tcam_data_mem
                                                                      )
                                    );
                stage_fc->tcam_sz += soc_mem_index_count(unit, tcam_data_mem);
            }
        }
        break;

    default:
        return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}


int
_bcm_field_tr3_hw_clear(int unit, _field_stage_t *stage_fc)
{
    int rv = BCM_E_NONE;

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXTERNAL) {
        /* Initialize ESM_L3_PROTOCOL_FN table */
        rv = _bcm_field_tr3_external_ip_proto_deinit(unit);
    } else {
        rv = _bcm_field_tr_hw_clear(unit, stage_fc);
    }

    return (rv);
}

/*
 * Function:
 *     _bcm_field_tr3_init
 * Purpose:
 *     Perform initializations that are specific to BCM56640. This
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
_bcm_field_tr3_init(int unit, _field_control_t *fc)
{
    _field_stage_t *stage_fc; /* Stages iteration pointer */

    /* Input parameters check. */
    if (NULL == fc) {
        return (BCM_E_PARAM);
    }
    /*Set default logical pool mode */
    fc->ingress_logical_policer_pools_mode =
        bcmFieldIngressLogicalPolicerPools8x1024;

    /*  Check external tcam presence for ingress (default stage) */
    /* Only for EasyRider; Triumph taken care of elsewhere */
    fc->tcam_ext_numb = FP_EXT_TCAM_NONE;

    stage_fc = fc->stages;
    while (stage_fc) {
        /* Clear hardware table */
        BCM_IF_ERROR_RETURN(_bcm_field_tr3_hw_clear(unit, stage_fc));

        /* Initialize qualifiers info. */
        BCM_IF_ERROR_RETURN(_field_tr3_qualifiers_init(unit, stage_fc));

        if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXTERNAL) {
            BCM_IF_ERROR_RETURN(_field_tr3_external_init(unit, stage_fc));
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

    /* Initialize TR3 function pointers */
    _field_tr3_functions_init(&fc->functions);

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_tr3_logical_meter_pools_init
 * Purpose:
 *     Initialize field meter pools usage bitmap.
 * Parameters:
 *     unit         - (IN) BCM device number.
 *     stage_fc     - (IN/OUT) Stage field control structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_field_tr3_logical_meter_pools_init(int unit, _field_stage_t *stage_fc)
{
    int      logical_meter_pair_num = -1;  /* Number of logical meter pairs.  */
    int      logical_pool_idx;             /* Virtual Meter pools iterator.   */
    int      logical_size;                 /* Virtual Allocation size.        */
    uint16   logical_pool_size = 0;        /* Total number of meters
                                                             in logical pool. */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    if (0 == (stage_fc->flags & _FP_STAGE_GLOBAL_METER_POOLS)) {
        return (BCM_E_PARAM);
    }

    /* Init Virtual Meter Pools
     * Total meters per logical pool = 2 * (No. of logical meter pairs)
    */

    BCM_IF_ERROR_RETURN(_bcm_field_tr3_logical_meter_pool_info(unit,
                                           stage_fc,
                                           &stage_fc->num_logical_meter_pools,
                                           &logical_meter_pair_num,
                                           &logical_pool_size
                                           )
                                       );

    logical_size = SHR_BITALLOCSIZE(logical_meter_pair_num << 1);

    for (logical_pool_idx = 0;
            logical_pool_idx < stage_fc->num_logical_meter_pools;
            logical_pool_idx++) {
        /* Allocate pool usage bitmap. */
        stage_fc->logical_meter_pool[logical_pool_idx] =
            sal_alloc(sizeof(_field_meter_pool_t),
                    "fp_logical_meter_pool");
        if (stage_fc->logical_meter_pool[logical_pool_idx] == NULL) {
            return (BCM_E_MEMORY);
        }

        /* Initalize stage meter pool info. */
        stage_fc->logical_meter_pool[logical_pool_idx]->slice_id    = -1;
                                           /* Virtual Pool is not in use. */
        stage_fc->logical_meter_pool[logical_pool_idx]->pool_size   =
                                                         logical_pool_size;
                               /* Total number of meters in logical pool. */
        stage_fc->logical_meter_pool[logical_pool_idx]->level = -1;
        stage_fc->logical_meter_pool[logical_pool_idx]->size        =
                                              (logical_meter_pair_num * 2);
                                /* Number of valid meters in logical pool.*/
        stage_fc->logical_meter_pool[logical_pool_idx]->free_meters =
                                              (logical_meter_pair_num * 2);
        stage_fc->logical_meter_pool[logical_pool_idx]->num_meter_pairs =
                                                    logical_meter_pair_num;
        stage_fc->logical_meter_pool[logical_pool_idx]->meter_bmp.w =
                                   sal_alloc(logical_size,
                                             "fp_logical_meters_map"
                                            );
        if (stage_fc->logical_meter_pool[logical_pool_idx]->meter_bmp.w
                == NULL) {
               return (BCM_E_MEMORY);
        }

        /* Reset pool usage bitmap. */
        sal_memset(stage_fc->logical_meter_pool[logical_pool_idx]->meter_bmp.w,
                   0, logical_size );
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_tr3_logical_meter_pool_free
 *
 * Purpose:
 *     Unreserve a logical meter pool.
 *
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Stage field control structure.
 *     pool_idx - (IN) Meter pool index.
 * Returns:
 *     BCM_E_XXX
 */
int
_field_tr3_logical_meter_pool_free(int unit,
                       _field_stage_t *stage_fc, uint8 logical_pool_idx)
{
    _field_meter_pool_t *f_logical_mp;          /* Meter pool descriptor. */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    f_logical_mp = stage_fc->logical_meter_pool[logical_pool_idx];
    if (NULL == f_logical_mp) {
        return (BCM_E_INTERNAL);
    }

    /* Make sure pool entries are not in use. */
    if (f_logical_mp->free_meters != f_logical_mp->size) {
        return (BCM_E_BUSY);
    }

    f_logical_mp->slice_id = _FP_INVALID_INDEX;
    f_logical_mp->level = -1;

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "FP(unit %d) vverb: "
                           "_field_tr3_logical_meter_pool_free()"
                           " Deallocated logical meter pool %d\n"),
               unit, logical_pool_idx));
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_tr3_logical_meter_pool_alloc
 *
 * Purpose:
 *     Allocate/Find meter pool with free meters
 *
 * Parameters:
 *     unit              - (IN) BCM device number.
 *     stage_fc          - (IN) Stage field control structure.
 *     f_pl              - (IN) Field policer descriptor
 *     slice_id          - (IN) Slice base index.
 *     pool_idx          - (OUT) Meter pool index.
 *     logical_pool_idx  - (OUT) Logical Meter pool index.
 *     updateHigherMeter - (OUT) update 0-511 or 512-1023 index.

 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr3_logical_meter_pool_alloc(int unit, _field_stage_t *stage_fc,
                        _field_policer_t *f_pl, uint8 slice_id, uint8 *pool_idx,
                        uint8 *logical_pool_idx,
                        uint8 *updateHigherMeter)
{
    _field_meter_pool_t         *f_mp;
                                                /* Meter pool descriptor. */
    _field_meter_pool_t         *f_logical_mp;
                                                /* Meter pool descriptor. */
    _field_control_t            *fc;          /* Field control structure. */
    int                          idx;           /* Pool iteration index.  */
    int                          free_pool;           /* Free meter pool. */
    int                          logical_free_pool;  /* Free Virtual Pool */
    uint8                        isLogicalPoolModeSplit;
                                                     /* Logical Pool Mode */


    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == pool_idx)) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    free_pool = _FP_INVALID_INDEX;
    logical_free_pool = _FP_INVALID_INDEX;

    for (idx = 0 ; idx < stage_fc->num_logical_meter_pools; idx++) {
        f_logical_mp = stage_fc->logical_meter_pool[idx];
        if (NULL == f_logical_mp) {
            return (BCM_E_INTERNAL);
        }

        /* In 16x512Split Mode:
         * Logical Pools (0,8)  (1,9)  (2,10) (3,11)
         *               (4,12) (5,13) (6,14) (7,15) shares same physical Pool
         * In 16x512Contiguous Mode:
         * Logical Pools (0,1)  (2,3)  (4,5) (6,7)
         *               (8,9) (10,11) (12,13) (14,15) share a physical Pool*/
        if (fc->ingress_logical_policer_pools_mode ==
                          bcmFieldIngressLogicalPolicerPools16x512Split) {
            f_mp = stage_fc->meter_pool[_FP_DEF_INST][idx % 8];
            isLogicalPoolModeSplit = TRUE;
        } else if (fc->ingress_logical_policer_pools_mode ==
                          bcmFieldIngressLogicalPolicerPools16x512Contiguous) {
            f_mp = stage_fc->meter_pool[_FP_DEF_INST][idx / 2];
            isLogicalPoolModeSplit = FALSE;
        } else {
            return (BCM_E_INTERNAL);
        }

        if (NULL == f_mp) {
            return (BCM_E_INTERNAL);
        }

        /* Allocate next index from an already allocated pool only if
         *   the following conditions are met:
         * 1) slice id of the already existing logical meter pool and
                                                       new entry must match.
         * 2) level of the already existing logical meter pool and
                                                       new entry must match.
         * 3) level of the physical meter pool corresponding to the
                                logical meter pool and new entry must match.
         *
         * When the above conditions are not met, allocate a new logical pool.
         * When new logical pool is allocated, verify if the physical pool
         * corresponding to the new logical pool is also newly allocated.
         * If the physical pool is already allocated to another logical pool,
         * ensure the level of physical meter pool corresponding to new
         * logical pool must match the entry's meter pool level or
         * Example :- In contiguos mode ,
         *  creating group 0,level 0 implies Meter Pool 0 (0-511) are allocated.
         *  creating group 1,level 1 shouldn't allocate Meter Pool 0 (512-1024).
         *  It has to allocate Meter Pool 1 (0-511);
         */
        if ((f_logical_mp->slice_id == slice_id)
                && (f_logical_mp->level == f_pl->level)
                && (f_mp->level == f_pl->level)
                && ((_FP_POLICER_IS_FLOW_MODE(f_pl) && f_logical_mp->free_meters > 0)
                    || (!_FP_POLICER_IS_FLOW_MODE(f_pl)
                        && f_logical_mp->free_meters >= 2))) {
            if (isLogicalPoolModeSplit) {
                *pool_idx = idx % 8;
                *logical_pool_idx = idx;
                if (idx > 7) {
                    *updateHigherMeter = TRUE;
                } else {
                    *updateHigherMeter = FALSE;
                }
            } else if (!isLogicalPoolModeSplit) {
                *pool_idx = idx/2;
                *logical_pool_idx = idx;
                if ( idx % 2 == 0 ) {
                    *updateHigherMeter = FALSE;
                } else {
                    *updateHigherMeter = TRUE;
                }
            }
            f_pl->cfg.pool_id = *logical_pool_idx;
            return (BCM_E_NONE);
        } else if ((f_mp->level != -1) && (f_mp->level != f_pl->level))  {
            continue;
        }

        /* Preserve first free pool */
        if ((_FP_INVALID_INDEX  == free_pool) &&
                (_FP_INVALID_INDEX  == f_logical_mp->slice_id)) {
            if (isLogicalPoolModeSplit) {
                free_pool = idx % 8;
                logical_free_pool = idx;
                if (idx > 7) {
                    *updateHigherMeter = TRUE;
                } else {
                    *updateHigherMeter = FALSE;
                }
            } else if (!isLogicalPoolModeSplit) {
                free_pool = idx / 2;
                logical_free_pool = idx;
                if ( idx % 2 == 0 ) {
                    *updateHigherMeter = FALSE;
                } else {
                    *updateHigherMeter = TRUE;
                }
            }
        }
    }
    /* We have to use free pool at this point. */
    if (_FP_INVALID_INDEX == free_pool) {
        return (BCM_E_RESOURCE);
    }

    f_mp = stage_fc->meter_pool[_FP_DEF_INST][free_pool];
    if (f_mp->slice_id == -1) {
        f_mp->slice_id = slice_id;
    }
    f_mp->level = f_pl->level;

    f_logical_mp = stage_fc->logical_meter_pool[logical_free_pool];
    f_logical_mp->slice_id = slice_id;
    f_logical_mp->level = f_pl->level;

    *pool_idx = free_pool;
    *logical_pool_idx = logical_free_pool;
    f_pl->cfg.pool_id = *logical_pool_idx;

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "Unit(%d): %s() : Alloted Physical Pool: %d at "
                           "level: %d with corresponding Logical Pool : %d\r\n"),
               unit, FUNCTION_NAME(), free_pool, f_mp->level,
               logical_free_pool));
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_tr3_logical_meter_pool_entry_alloc
 *
 * Purpose:
 *     Allocate meter pool entry.
 *
 * Parameters:
 *     unit             - (IN) BCM device number.
 *     stage_fc         - (IN) Stage field control structure.
 *     pool_slice_id    - (IN) Meter pool index.
 *     f_pl             - (IN) Field policer descriptor
 * Returns:
 *     BCM_E_XXX
 */

int
_field_tr3_logical_meter_pool_entry_alloc (int unit, _field_stage_t *stage_fc,
                              uint8 pool_slice_id, _field_policer_t *f_pl)
{
    int                     idx;                /* Pool iteration index.     */
    int                     startIdx;           /* Pool iteration index.     */
    int                     endIdx;             /* Pool iteration index.     */
    int                     logicalIdx;         /* Logical Pool   index.     */
    uint8                   pool_idx;           /* Meter pool index.         */
    uint8                   logical_pool_idx;   /* Logical Meter pool index. */
    uint8                   updateHigherMeter = 0;
                                                /* update 0-511 or 512-1023. */
    _field_meter_pool_t    *f_mp;               /* Meter pool descriptor     */
    _field_meter_pool_t    *f_logical_mp;   /* Logical Meter pool descriptor */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == f_pl)) {
        return (BCM_E_PARAM);
    }

    if (f_pl->level >= _FP_POLICER_LEVEL_COUNT) {
        return (BCM_E_PARAM);
    }

    /* Allocate meter pool with free meters. */
    BCM_IF_ERROR_RETURN(_field_tr3_logical_meter_pool_alloc(unit,
                                                            stage_fc, f_pl,
                                                            pool_slice_id,
                                                            &pool_idx,
                                                            &logical_pool_idx,
                                                            &updateHigherMeter
                                                            ));
    f_mp = stage_fc->meter_pool[_FP_DEF_INST][pool_idx];
    f_logical_mp = stage_fc->logical_meter_pool[logical_pool_idx];

    /* Update either (512-1023) index or (0-511) index
     * f_mp->size = 1024 for triumph3.
     */
    if (updateHigherMeter) {
        startIdx = f_mp->size/2;
          endIdx = f_mp->size;
    }else {
        startIdx = 0;
          endIdx = f_mp->size/2;
    }


    /* Reserve unused meter. */
    logicalIdx = 0;
    for (idx = startIdx; idx < endIdx; idx++) {

        /* Allocate meter indices for non-flow mode meters. */
        if (!_FP_POLICER_IS_FLOW_MODE(f_pl)) {

            if (0 != (idx % 2)) {
                /* skip odd meter indices */
                continue;
            }

            if (0 != (logicalIdx % 2)) {
                /* skip odd meter indices */
                logicalIdx++;
            }

            if ((0 == _FP_METER_BMP_TEST(f_mp->meter_bmp, idx))
                    && (0 == _FP_METER_BMP_TEST(f_mp->meter_bmp, idx + 1))) {
                _FP_METER_BMP_ADD(f_mp->meter_bmp, idx);
                _FP_METER_BMP_ADD(f_mp->meter_bmp, (idx + 1));
                _FP_METER_BMP_ADD(f_logical_mp->meter_bmp, logicalIdx);
                _FP_METER_BMP_ADD(f_logical_mp->meter_bmp, (logicalIdx + 1));
                /* Decrement free meters count in meter pool. */
                f_mp->free_meters -= 2;
                f_logical_mp->free_meters -= 2;
                f_pl->pool_index = pool_idx ;
                f_pl->hw_index   = (idx / 2);

                f_pl->logical_pool_index = logical_pool_idx;

                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                           "Unit(%d): Alloted Physical Pool(%d): MeterIndex: %d"
                             " at level: %d  corresponding to Logical Pool(%d):"
                                " MeterIndex: %d\r\n"), unit, pool_idx, (idx/2),
                           f_mp->level,logical_pool_idx, logicalIdx));
                return (BCM_E_NONE);
            }
        }

        /* Allocate hardware resources for flow mode meters. */
        if (_FP_POLICER_IS_FLOW_MODE(f_pl)
                && (0 == _FP_METER_BMP_TEST(f_mp->meter_bmp, idx))) {

            if (f_pl->hw_flags & _FP_POLICER_DIRTY) {
                f_pl->hw_flags &= (idx % 2)
                    ? ~_FP_POLICER_PEAK_DIRTY
                    : ~_FP_POLICER_COMMITTED_DIRTY;
            } else {
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                            "FP(unit %d) Error: Meter alloc called with invalid"
                            " hw_flags=%x\n"), unit, f_pl->hw_flags));
                return (BCM_E_INTERNAL);
            }

            if (f_pl->hw_flags & _FP_POLICER_PEAK_DIRTY) {
                _FP_POLICER_EXCESS_HW_METER_SET(f_pl);
            } else {
                _FP_POLICER_EXCESS_HW_METER_CLEAR(f_pl);
            }
            _FP_METER_BMP_ADD(f_mp->meter_bmp, idx);
            _FP_METER_BMP_ADD(f_logical_mp->meter_bmp, logicalIdx);


            /* Decrement free meter count in meter pool. */
            f_mp->free_meters--;
            f_logical_mp->free_meters--;
            f_pl->pool_index =  pool_idx;
            f_pl->hw_index   = (idx / 2);
            f_pl->logical_pool_index = logical_pool_idx;

            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                             "Unit(%d): Alloted Physical Pool(%d):Index: %d at "
                                 "level: %d  corresponding to Logical Pool(%d):"
                                   "Index: %d\r\n"), unit, pool_idx, (idx/2),
                       f_mp->level,logical_pool_idx, logicalIdx));
            return (BCM_E_NONE);
        }
        logicalIdx++;
    }

    /* Never reached - pool alloc makes sure at least one meter is available. */
    if (f_mp->free_meters == f_mp->size) {
        BCM_IF_ERROR_RETURN
            (_field_meter_pool_free(unit, stage_fc, _FP_DEF_INST, pool_idx));
    }

    /* Never reached - pool alloc makes sure at least one meter is available. */
    if (f_logical_mp->free_meters == f_logical_mp->size) {
        BCM_IF_ERROR_RETURN(_field_tr3_logical_meter_pool_free(unit,
                                                stage_fc, logical_pool_idx));
    }
    return (BCM_E_RESOURCE);
}

/* free the meter pool info of the allocated for the policer
 * prior to calling bcm_policer_set API.
 */
int
_field_tr3_logical_meter_pool_entry_free_old (int unit, _field_stage_t *stage_fc,
                              _field_policer_t *f_pl)
{
     _field_control_t   *fc;
    _field_meter_pool_t *f_mp;          /* Meter pool descriptor. */
    _field_meter_pool_t *f_logical_mp;  /* Logical Meter pool descriptor. */
    int idx;                            /* Meter index.           */
    int logical_idx;                    /* Logical Meter index.        */
    int logical_pool_idx = -1;               /* Logical Meter index.        */
    uint8 isLogicalPoolModeSplit = 0;  /*Specify split or contiguous
                                        logical pool mode */
    uint8 updateHigherMeter = 0;

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == f_pl)) {
        return (BCM_E_PARAM);
    }

    /* Meter pool index sanity check. */
    if (stage_fc->num_meter_pools <= f_pl->old_pool_index) {
        return (BCM_E_PARAM);
    }

    /* In 16x512Split Mode:
     * Logical Pools (0,8)  (1,9)  (2,10) (3,11)
     *               (4,12) (5,13) (6,14) (7,15) shares same physical Pool
     * In 16x512Contiguous Mode:
     * Logical Pools (0,1)  (2,3)  (4,5) (6,7)
     *               (8,9) (10,11) (12,13) (14,15) share a physical Pool
     */

    /* Get field control information. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    if (fc->ingress_logical_policer_pools_mode ==
            bcmFieldIngressLogicalPolicerPools16x512Split) {
        isLogicalPoolModeSplit = TRUE;
    } else if (fc->ingress_logical_policer_pools_mode ==
            bcmFieldIngressLogicalPolicerPools16x512Contiguous) {
        isLogicalPoolModeSplit = FALSE;
    } else
        return (BCM_E_INTERNAL);

    f_logical_mp = NULL;

    f_mp = stage_fc->meter_pool[_FP_DEF_INST][f_pl->old_pool_index];


    if (f_pl->old_logical_pool_index != _FP_INVALID_INDEX) {
        f_logical_mp = stage_fc->logical_meter_pool[f_pl->old_logical_pool_index];
        logical_pool_idx = f_pl->old_logical_pool_index;
    }


    if (f_logical_mp == NULL)
        return (BCM_E_INTERNAL);

    if (isLogicalPoolModeSplit) {
        if (logical_pool_idx > 7)
            updateHigherMeter = TRUE;
        else
            updateHigherMeter = FALSE;
    } else if (!isLogicalPoolModeSplit) {
        if ( logical_pool_idx % 2 == 0 )
            updateHigherMeter = FALSE;
        else
            updateHigherMeter = TRUE;
    }

    /* Mark meter as free in the slice. */
    idx = f_pl->old_hw_index * 2;

    if (updateHigherMeter)
    {
        logical_idx = idx - (f_mp->size/2);
    }else {
        logical_idx = idx;
    }

    /* Meter index in the pool sanity check. */
    if ((f_mp->size <= idx)
        || (f_mp->size <= (idx + 1))) {
        return (BCM_E_PARAM);
    }

    /* Logical Meter index in the pool sanity check. */
    if ((f_logical_mp->size <= logical_idx)
            || (f_logical_mp->size <= (logical_idx + 1))) {
        return (BCM_E_PARAM);
    }


    if (bcmPolicerModeCommitted != (f_pl)->old_mode) {
        _FP_METER_BMP_REMOVE(f_mp->meter_bmp, idx);
        _FP_METER_BMP_REMOVE(f_mp->meter_bmp, idx + 1);
        f_mp->free_meters += 2;

        _FP_METER_BMP_REMOVE(f_logical_mp->meter_bmp, logical_idx);
        _FP_METER_BMP_REMOVE(f_logical_mp->meter_bmp, logical_idx + 1);
        f_logical_mp->free_meters += 2;

        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "FP(unit %d) vverb: "
                               "_field_tr3_logical_meter_pool_entry_free()"
                               " Non Flow Mode: Deallocating physical meter"
                               "id %d,%d and logical meter %d,%d\n"), unit,
                   idx, (idx + 1), logical_idx, (logical_idx + 1)));
    } else {
        if ((bcmPolicerModeCommitted == (f_pl)->old_mode) &&
		    ((f_pl)->old_hw_flags & _FP_POLICER_EXCESS_METER)) {
            _FP_METER_BMP_REMOVE(f_mp->meter_bmp, idx);
            _FP_METER_BMP_REMOVE(f_logical_mp->meter_bmp, logical_idx);
        } else if ((bcmPolicerModeCommitted == (f_pl)->old_mode) &&
		          (!((f_pl)->old_hw_flags & _FP_POLICER_EXCESS_METER))) {
            _FP_METER_BMP_REMOVE(f_mp->meter_bmp, idx + 1);
            _FP_METER_BMP_REMOVE(f_logical_mp->meter_bmp, logical_idx + 1);
        }
        f_mp->free_meters++;
        f_logical_mp->free_meters++;

        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "FP(unit %d) vverb: "
                               "_field_tr3_logical_meter_pool_entry_free()"
                               " Flow Mode: Deallocating physical meter"
                               "id %d and logical meter %d\n"), unit,
                   idx, logical_idx));
    }

    /* Free logical empty pool. */
    if (f_logical_mp->free_meters == f_logical_mp->size) {
        BCM_IF_ERROR_RETURN
            (_field_tr3_logical_meter_pool_free(unit, stage_fc, logical_pool_idx));
    }

    /* Free empty pool. */
    if (f_mp->free_meters == f_mp->size) {
        BCM_IF_ERROR_RETURN
            (_field_meter_pool_free(unit, stage_fc, _FP_DEF_INST,
                                    f_pl->old_pool_index));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr3_logical_meter_pool_entry_free
 *
 * Purpose:
 *     Free meter pool entry.
 *     if necessary free the pool itself.
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     stage_fc  - (IN) Stage field control structure.
 *     f_pl      - (IN) Policer descriptor.
 * Returns:
 *     BCM_E_XXX
 */
int
_field_tr3_logical_meter_pool_entry_free (int unit, _field_stage_t *stage_fc,
                              _field_policer_t *f_pl)
{
     _field_control_t   *fc;
    _field_meter_pool_t *f_mp;          /* Meter pool descriptor. */
    _field_meter_pool_t *f_logical_mp;  /* Logical Meter pool descriptor. */
    int idx;                            /* Meter index.           */
    int logical_idx;                    /* Logical Meter index.        */
    int logical_pool_idx = -1;               /* Logical Meter index.        */
    uint8 isLogicalPoolModeSplit = 0;  /*Specify split or contiguous
                                        logical pool mode */
    uint8 updateHigherMeter = 0;

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == f_pl)) {
        return (BCM_E_PARAM);
    }

    /* Meter pool index sanity check. */
    if (stage_fc->num_meter_pools <= f_pl->pool_index) {
        return (BCM_E_PARAM);
    }

    /* In 16x512Split Mode:
     * Logical Pools (0,8)  (1,9)  (2,10) (3,11)
     *               (4,12) (5,13) (6,14) (7,15) shares same physical Pool
     * In 16x512Contiguous Mode:
     * Logical Pools (0,1)  (2,3)  (4,5) (6,7)
     *               (8,9) (10,11) (12,13) (14,15) share a physical Pool
     */

    /* Get field control information. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    if (fc->ingress_logical_policer_pools_mode ==
            bcmFieldIngressLogicalPolicerPools16x512Split) {
        isLogicalPoolModeSplit = TRUE;
    } else if (fc->ingress_logical_policer_pools_mode ==
            bcmFieldIngressLogicalPolicerPools16x512Contiguous) {
        isLogicalPoolModeSplit = FALSE;
    } else
        return (BCM_E_INTERNAL);

    f_logical_mp = NULL;

    f_mp = stage_fc->meter_pool[_FP_DEF_INST][f_pl->pool_index];


    if (f_pl->logical_pool_index != _FP_INVALID_INDEX) {
        f_logical_mp = stage_fc->logical_meter_pool[f_pl->logical_pool_index];
        logical_pool_idx = f_pl->logical_pool_index;
    }


    if (f_logical_mp == NULL)
        return (BCM_E_INTERNAL);

    if (isLogicalPoolModeSplit) {
        if (logical_pool_idx > 7)
            updateHigherMeter = TRUE;
        else
            updateHigherMeter = FALSE;
    } else if (!isLogicalPoolModeSplit) {
        if ( logical_pool_idx % 2 == 0 )
            updateHigherMeter = FALSE;
        else
            updateHigherMeter = TRUE;
    }

    /* Mark meter as free in the slice. */
    idx = f_pl->hw_index * 2;

    if (updateHigherMeter)
    {
        logical_idx = idx - (f_mp->size/2);
    }else {
        logical_idx = idx;
    }

    /* Meter index in the pool sanity check. */
    if ((f_mp->size <= idx)
        || (f_mp->size <= (idx + 1))) {
        return (BCM_E_PARAM);
    }

    /* Logical Meter index in the pool sanity check. */
    if ((f_logical_mp->size <= logical_idx)
            || (f_logical_mp->size <= (logical_idx + 1))) {
        return (BCM_E_PARAM);
    }


    if (!_FP_POLICER_IS_FLOW_MODE(f_pl)) {
        _FP_METER_BMP_REMOVE(f_mp->meter_bmp, idx);
        _FP_METER_BMP_REMOVE(f_mp->meter_bmp, idx + 1);
        f_mp->free_meters += 2;

        _FP_METER_BMP_REMOVE(f_logical_mp->meter_bmp, logical_idx);
        _FP_METER_BMP_REMOVE(f_logical_mp->meter_bmp, logical_idx + 1);
        f_logical_mp->free_meters += 2;

        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "FP(unit %d) vverb: "
                               "_field_tr3_logical_meter_pool_entry_free()"
                               " Non Flow Mode: Deallocating physical meter"
                               "id %d,%d and logical meter %d,%d\n"), unit,
                   idx, (idx + 1), logical_idx, (logical_idx + 1)));
    } else {

        if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
            _FP_METER_BMP_REMOVE(f_mp->meter_bmp, idx);
            _FP_METER_BMP_REMOVE(f_logical_mp->meter_bmp, logical_idx);

            /* Clear excess meter flag bit. */
            _FP_POLICER_EXCESS_HW_METER_CLEAR(f_pl);
        } else if (_FP_POLICER_COMMITTED_HW_METER(f_pl)) {
            _FP_METER_BMP_REMOVE(f_mp->meter_bmp, idx + 1);
            _FP_METER_BMP_REMOVE(f_logical_mp->meter_bmp, logical_idx + 1);
        }
        f_mp->free_meters++;
        f_logical_mp->free_meters++;

        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "FP(unit %d) vverb: "
                               "_field_tr3_logical_meter_pool_entry_free()"
                               " Flow Mode: Deallocating physical meter"
                               "id %d and logical meter %d\n"), unit,
                   idx, logical_idx));
    }

    /* Free logical empty pool. */
    if (f_logical_mp->free_meters == f_logical_mp->size) {
        BCM_IF_ERROR_RETURN
            (_field_tr3_logical_meter_pool_free(unit, stage_fc, logical_pool_idx));
    }

    /* Free empty pool. */
    if (f_mp->free_meters == f_mp->size) {
        BCM_IF_ERROR_RETURN
            (_field_meter_pool_free(unit, stage_fc, _FP_DEF_INST,
                                    f_pl->pool_index));
    }

    /* Reset pool and hw index information in policer structure. */
    f_pl->pool_index = _FP_INVALID_INDEX;
    f_pl->logical_pool_index = _FP_INVALID_INDEX;
    f_pl->hw_index = _FP_INVALID_INDEX;

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm__field_tr3_action_replaceL2Fields
 * Purpose:
 *    fill EGR_IFP_MOD_FIELDS memory to replace L2 fields
 * Parameters:
 *     unit             - BCM device number
 *     fa - (OUT) field action structure
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     For triumph3 and helix4 devices only.
 */

int _bcm__field_tr3_action_replaceL2Fields(int unit,_field_action_t *fa)
{
    uint32 tbl_entry[SOC_MAX_MEM_FIELD_WORDS];
    int rv = BCM_E_NONE;
    bcm_mac_t mac_addr;
    uint32 priorityVal = 0;
    _field_control_t *fc;               /* Unit field control information.  */

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    if (NULL == fa || fa->hw_index >= fc->_fp_egr_ifp_mod_fields_size) {
        return (BCM_E_PARAM);
    }
    rv = READ_EGR_IFP_MOD_FIELDSm(unit, MEM_BLOCK_ANY,
                                   fa->hw_index, &tbl_entry);
    if(BCM_FAILURE(rv)) {
        return BCM_E_UNAVAIL;
    }
    switch (fa->action) {
        case bcmFieldActionReplaceSrcMac:
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                 &tbl_entry, CHANGE_MAC_SAf,1);
            SAL_MAC_ADDR_FROM_UINT32(mac_addr,fa->param);
            soc_mem_mac_addr_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, MAC_SAf, mac_addr);
            break;
        case bcmFieldActionReplaceDstMac:
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                 &tbl_entry, CHANGE_MAC_DAf,1);
            SAL_MAC_ADDR_FROM_UINT32(mac_addr,fa->param);
            soc_mem_mac_addr_set(unit, EGR_IFP_MOD_FIELDSm,
                                   &tbl_entry, MAC_DAf, mac_addr);
            break;
        case bcmFieldActionReplaceInnerVlan:
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, CHANGE_INNER_VLANf,1);
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, INNER_VLANf, fa->param[0]);
            break;
        case bcmFieldActionReplaceOuterVlan:
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, CHANGE_OUTER_VLANf,1);
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, OUTER_VLANf, fa->param[0]);
            break;
        case bcmFieldActionReplaceInnerVlanPriority:
            if(fa->param[0] == BCM_FIELD_REPLACE_VLAN_PRIORITY) {
                priorityVal =  (fa->param[1]) << 1;
                soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, IPRI_ICFI_SELf,1);
            } else if (fa->param[0] ==
                          BCM_FIELD_REPLACE_VLAN_PRIORITY_USE_INTERNAL_CFI) {
                priorityVal =  (fa->param[1]) << 1;
                priorityVal =  priorityVal | 1;
                soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, IPRI_ICFI_SELf,1);

            } else {
                priorityVal =0;
                soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, IPRI_ICFI_SELf,2);
            }
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, NEW_IPRI_ICFIf, priorityVal);
            break;
        case bcmFieldActionReplaceOuterVlanPriority:
            if(fa->param[0] == BCM_FIELD_REPLACE_VLAN_PRIORITY) {
                priorityVal =  (fa->param[1]) << 1;
                soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, OPRI_OCFI_SELf,1);
            } else if (fa->param[0] ==
                         BCM_FIELD_REPLACE_VLAN_PRIORITY_USE_INTERNAL_CFI) {
                priorityVal =  (fa->param[1]) << 1;
                priorityVal =  priorityVal | 1;
                soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, OPRI_OCFI_SELf,1);
            } else {
                priorityVal =0;
                soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, OPRI_OCFI_SELf,2);
            }
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, NEW_OPRI_OCFIf, priorityVal);
            break;
        default:
            return (BCM_E_PARAM);
    } /*end of switch */
    rv = WRITE_EGR_IFP_MOD_FIELDSm(unit, MEM_BLOCK_ANY,
                                     fa->hw_index, &tbl_entry);
    if(BCM_FAILURE(rv)) {
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm__field_tr3_action_replaceL2Fields_remove_HWEntries
 * Purpose:
 *    remove entries from EGR_IFP_MOD_FIELDS table
 * Parameters:
 *     unit             - BCM device number
 *    f_ent             - Field entry structure
 *     fa - (OUT) field action structure
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     For triumph3 and helix4 devices only.
 */
int _bcm__field_tr3_action_replaceL2Fields_remove_HWEntries( int unit,
                                      _field_entry_t *f_ent,
                                      _field_action_t *fa) {
    int rv = BCM_E_NONE;
    bcm_mac_t mac_addr;
    uint32 mac_addr_words[2];
    uint32 tbl_entry[SOC_MAX_MEM_FIELD_WORDS];
    int removeIndex;
    _field_action_t     *fa_iter;    /* Field entry actions iterator.*/

    if (NULL == fa || f_ent == NULL) {
        return (BCM_E_PARAM);
    }
    mac_addr_words[0] = 0;
    mac_addr_words[1] = 0;
    SAL_MAC_ADDR_FROM_UINT32(mac_addr,mac_addr_words);
    rv = READ_EGR_IFP_MOD_FIELDSm(unit, MEM_BLOCK_ANY,
                                    fa->hw_index, &tbl_entry);
    if(BCM_FAILURE(rv)) {
        return BCM_E_UNAVAIL;
    }
    switch (fa->action) {
        case bcmFieldActionReplaceSrcMac:
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, CHANGE_MAC_SAf, 0);

            soc_mem_mac_addr_set(unit, EGR_IFP_MOD_FIELDSm,
                                   &tbl_entry, MAC_SAf, mac_addr);
            break;
        case bcmFieldActionReplaceDstMac:
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, CHANGE_MAC_DAf, 0);
            soc_mem_mac_addr_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, MAC_DAf, mac_addr);
            break;
        case bcmFieldActionReplaceInnerVlan:
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, CHANGE_INNER_VLANf, 0);
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, INNER_VLANf, 0);
            break;
        case bcmFieldActionReplaceOuterVlan:
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                   &tbl_entry, CHANGE_OUTER_VLANf,0);
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, OUTER_VLANf, 0);
            break;
        case bcmFieldActionReplaceInnerVlanPriority:
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, IPRI_ICFI_SELf, 0);
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, NEW_IPRI_ICFIf, 0);
            break;
        case bcmFieldActionReplaceOuterVlanPriority:
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, OPRI_OCFI_SELf, 0);
            soc_mem_field32_set(unit, EGR_IFP_MOD_FIELDSm,
                                  &tbl_entry, NEW_OPRI_OCFIf, 0);
            break;
        default:
            return (BCM_E_PARAM);
    } /*end of switch */
    rv = WRITE_EGR_IFP_MOD_FIELDSm(unit, MEM_BLOCK_ANY,
                                     fa->hw_index, &tbl_entry);
    if(BCM_FAILURE(rv)) {
        return BCM_E_UNAVAIL;
    }
    /* clear the bit representing the index of EGR_IFP_MOD_FIELDS */
    fa_iter = f_ent->actions;
    removeIndex = 0;
    while (fa_iter != NULL) {
        if( (fa_iter->flags & _FP_ACTION_VALID) &&
            (fa_iter->action == bcmFieldActionReplaceSrcMac ||
            fa_iter->action == bcmFieldActionReplaceDstMac ||
            fa_iter->action == bcmFieldActionReplaceInnerVlan ||
            fa_iter->action == bcmFieldActionReplaceOuterVlan ||
            fa_iter->action == bcmFieldActionReplaceInnerVlanPriority  ||
            fa_iter->action == bcmFieldActionReplaceOuterVlanPriority) ) {
            removeIndex++;
        }
        fa_iter = fa_iter->next;
    }

    if(removeIndex == 0) {
        _field_control_t *fc;               /* Unit field control information.  */
        BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
        SHR_BITCLR(fc->field_action_l2mod_index_bmp.w,fa->hw_index);
    }
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

static soc_field_t _tr3_vfp_slice_wide_mode_field[4] = {
    SLICE_0_DOUBLE_WIDE_MODEf,
    SLICE_1_DOUBLE_WIDE_MODEf,
    SLICE_2_DOUBLE_WIDE_MODEf,
    SLICE_3_DOUBLE_WIDE_MODEf};

/*
 * Function:
 *     _bcm_field_tr3_meter_recover
 *
 * Purpose:
 *     This routine recovers policer configuration from hardware
 *     and reconstructs field entry's policer software structure
 *     parameters
 *
 * Parameters:
 *     unit         - (IN) BCM device number
 *     f_ent        - (IN) Pointer to a field entry
 *     part         - (IN) Field entry's part number
 *     pid          - (IN) Policer ID
 *     level        - (IN) Policer Level(1-Macro/0-Micro)
 *     policy_mem   - (IN) Policy table memory name
 *     policy_buf   - (IN) Policy table entry buffer pointer
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_tr3_meter_recover(int             unit,
                             _field_entry_t  *f_ent,
                             int             part,
                             bcm_policer_t   pid,
                             uint32          level,
                             soc_mem_t       policy_mem,
                             uint32          *policy_buf,
                             uint8          *already_recovered
                             )
{
     _field_group_t *fg;                 /* Field group information.         */
    _field_stage_t *stage_fc;           /* Field stage information.         */
    _field_control_t *fc;               /* Unit field control information.  */
    _field_slice_t *fs;                 /* Field slice information.         */
    _field_policer_t *f_pl = NULL;      /* Policer descriptor pointer type. */
    _field_entry_policer_t *f_ent_pl;   /* Policer attached to entry.       */
    int rv, policer_id, meter_hw_idx;   /* Meter pair/flow meter hw index.  */
    int pool_index = _FP_INVALID_INDEX; /* Meter pool number.               */
    int hw_index = _FP_INVALID_INDEX;   /* Pool local meter pair index.     */
    soc_field_t mpair_idx_f;            /* Meter pair index field.          */
    soc_field_t modifier_f;             /* Meter modifier field.            */
    soc_field_t pool_num_f;             /* Meter modifier field.            */
    uint32 idx, found = 0;              /* Policer hash idx; Policer found. */
    uint32 flags = 0;                   /* Policer flags.                   */
    uint32 meter_pair_mode = 0;         /* Flow/SrTcm/TrTcm meter modes.    */
    uint32 modifier_mode = 0;           /* Committed - 1/Excess meter - 0.  */
    uint32 meter_offset = 0;            /* Even or Odd meter index.         */
    _field_meter_pool_t *f_mp = NULL;   /* Meter pool information.          */
    _meter_config_t meter_conf;         /* Meter configuration in hardware. */
    soc_mem_t meter_mem = INVALIDm;            /* Meter memory name.        */
    uint32 meter_sharing_mode = -1;     /* Meter BW sharing mode */
   _field_meter_pool_t *f_logical_mp = NULL;
                                        /* Logical Meter pool descriptor */
    int                     logicalIdx = 0;
    uint8                   logical_pool_idx = 0;
                                        /* Logical Meter pool index. */




    fg = f_ent->group;
    fs = f_ent[part].fs;

    sal_memset(&meter_conf, 0, sizeof(_meter_config_t));

    /* Get field control */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get stage control */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Retrieve meter config parameters from policy table and meter table. */
    switch (fg->stage_id) {
        case  _BCM_FIELD_STAGE_INGRESS:
            meter_mem = FP_METER_TABLEm;
            /* Get - Flow/SrTcm/TrTcm mode. */
            if (level == 1) {
                /* meter BW/Sharing mode 0=OFF, 1=MIN, 2=MAX, 3=MIN+MAX */
                if ((meter_sharing_mode = soc_mem_field32_get(
                                             unit, policy_mem,
                                             policy_buf,
                                             METER_SHARING_MODEf))
                    == 0) {
                    return (BCM_E_NONE);
                }
                if (SOC_MEM_FIELD_VALID(unit, policy_mem,
                                        SHARED_METER_PAIR_MODEf)){
                    meter_pair_mode = soc_mem_field32_get
                                      (unit, policy_mem,
                                      policy_buf, SHARED_METER_PAIR_MODEf);
                }

                /* Mode - Flow/TrTcmDs */
                switch (meter_pair_mode) {
                    case 0:
                        meter_pair_mode = BCM_FIELD_METER_MODE_FLOW;
                        break;
                    case 1:
                        meter_pair_mode = 4;
                                          /* no macro created yet */
                        break;
                    default:
                        return BCM_E_NONE;
                }
                mpair_idx_f = SHARED_METER_PAIR_POOL_INDEXf;
                modifier_f = METER_SHARING_MODE_MODIFIERf;
                pool_num_f = SHARED_METER_PAIR_POOL_NUMBERf;
            } else {
                if ((meter_pair_mode = soc_mem_field32_get
                                        (unit, policy_mem,
                                         policy_buf, METER_PAIR_MODEf))
                    == 0) {
                    return (BCM_E_NONE);
                }
                mpair_idx_f = METER_PAIR_POOL_INDEXf;
                modifier_f = METER_PAIR_MODE_MODIFIERf;
                pool_num_f = METER_PAIR_POOL_NUMBERf;
            }

            /* Get meter pair index value from policy table. */
            hw_index = soc_mem_field32_get(unit, policy_mem, policy_buf,
                                             mpair_idx_f);

            /* modifier_mode: 0 = Excess Meter, 1 = Committed Meter. */
            modifier_mode = soc_mem_field32_get(unit, policy_mem, policy_buf,
                                                modifier_f);

            /* Get meter pool number from policy table. */
            pool_index = soc_mem_field32_get(unit, policy_mem, policy_buf,
                                                pool_num_f);

            /* Determine meter offset value for flow meters. */
            if (BCM_FIELD_METER_MODE_FLOW == meter_pair_mode) {
                    meter_offset = (0 == modifier_mode) ? 0 : 1;
            }

            /* Get meter pool info pointer. */
            f_mp = stage_fc->meter_pool[_FP_DEF_INST][pool_index];

            /* Setup meter pool to entry slice mapping. */
            if (_FP_INVALID_INDEX == f_mp->slice_id) {
                f_mp->slice_id = fg->slices->slice_number;
                f_mp->level = level;
            }
            break;

        case _BCM_FIELD_STAGE_EGRESS:
            meter_mem = EFP_METER_TABLEm;

            meter_conf.meter_mode = soc_mem_field32_get(unit, policy_mem,
                                                        policy_buf,
                                                        METER_PAIR_MODEf);
            meter_conf.meter_idx = soc_mem_field32_get(unit, policy_mem,
                                                       policy_buf,
                                                       METER_INDEX_EVENf);
            meter_conf.meter_update_odd = soc_mem_field32_get(unit,
                                            policy_mem, policy_buf,
                                            METER_UPDATE_ODDf);
            meter_conf.meter_test_odd = soc_mem_field32_get(unit, policy_mem,
                                                            policy_buf,
                                                            METER_TEST_ODDf);
            meter_conf.meter_update_even = soc_mem_field32_get(unit,
                                            policy_mem, policy_buf,
                                            METER_UPDATE_EVENf);
            meter_conf.meter_test_even = soc_mem_field32_get(unit,
                                                             policy_mem,
                                                             policy_buf,
                                                             METER_TEST_EVENf);

            /* Get - Flow/SrTcm/TrTcm mode. */
            meter_pair_mode = meter_conf.meter_mode;

            /* modifier_mode: 0 = Excess Meter, 1 = Committed Meter. */
            if (BCM_FIELD_METER_MODE_FLOW == meter_pair_mode) {
                modifier_mode = (meter_conf.meter_update_even
                                && meter_conf.meter_test_even) ? 0 : 1;
                meter_offset = (0 == modifier_mode) ? 0 : 1;
            }

            /* Slice in which meter config resides. */
            pool_index = fs->slice_number;

            /* Slice meter pair index. */
            hw_index = meter_conf.meter_idx;

            if ((BCM_FIELD_METER_MODE_DEFAULT == meter_pair_mode)
                && (0 == hw_index)) {
                /* bcmPolicerModePassThrough */
                return BCM_E_NONE;
            }
            break;

        default:
            /* Must be a valid stage. */
            return (BCM_E_INTERNAL);
    }

    /* Check if meter index is already in use. */
    if (BCM_SUCCESS(_field_trx_meter_index_in_use(unit, stage_fc, fs,
                    f_mp, meter_pair_mode, meter_offset, hw_index))) {
        /* Policer has been detected - increment reference count */
        /* Happens when policer is shared by different entries */
        /* Search the hash to match against the HW index */

        found = 0;

        for (idx = 0; idx < _FP_HASH_SZ(fc); idx++) {
            f_pl = fc->policer_hash[idx];
            while (f_pl != NULL) {
                if ((f_pl->hw_index == hw_index) &&
                    (f_pl->pool_index == pool_index) &&
                    (f_pl->stage_id == fg->stage_id)) {
                    if (meter_pair_mode == BCM_FIELD_METER_MODE_FLOW) {
                        if (((modifier_mode == 1) &&
                             (_FP_POLICER_EXCESS_HW_METER(f_pl))) ||
                            ((modifier_mode == 0) &&
                             !(_FP_POLICER_EXCESS_HW_METER(f_pl)))) {
                            f_pl = f_pl->next;
                            continue;
                        } else {
                            found =1;
                            break;
                        }
                    } else {
                        found = 1;
                        break;
                    }
                }
                f_pl = f_pl->next;
            }
            if (found) {
                break;
            }
        }
        if (!found) {
            return BCM_E_INTERNAL;
        }
        f_pl->hw_ref_count++;
        f_pl->sw_ref_count++;
    } else {
        /* Policer does not exist => Allocate new policer object */
        if (fc->l2warm) {
            policer_id = pid;
        } else {
            BCM_IF_ERROR_RETURN(_field_policer_id_alloc(unit, &policer_id));
        }

        _FP_XGS3_ALLOC(f_pl, sizeof (_field_policer_t), "Field policer entity");
        if (f_pl == NULL) {
            return (BCM_E_MEMORY);
        }

        flags |= _FP_POLICER_INSTALLED;
        if ((f_pl->level = level) == 1 && meter_sharing_mode == 1) {
            f_pl->cfg.flags |= BCM_POLICER_COLOR_MERGE_OR;
        }
        f_pl->sw_ref_count = 2;
        f_pl->hw_ref_count = 1;
        f_pl->pid          = policer_id;
        f_pl->stage_id     = fg->stage_id;
        f_pl->pool_index   = pool_index;
        f_pl->cfg.pool_id  = f_pl->pool_index;
        f_pl->hw_index     = hw_index;
        f_pl->logical_pool_index = _FP_INVALID_INDEX;

        /* Calculate hardware meter index. */
        if (stage_fc->flags & _FP_STAGE_GLOBAL_METER_POOLS) {
            /*
             * Hw index is:
             * ((Pool number * pool_size) + (2 * pair number)
             */
            meter_hw_idx = (pool_index *
                stage_fc->meter_pool[_FP_DEF_INST][pool_index]->size)
                            + (2 * hw_index);
        } else {
            /*
             * Hw index is:
             * (slice index) + (2 * pair number)
             */
            meter_hw_idx =
                stage_fc->slices[_FP_DEF_INST][pool_index].start_tcam_idx
                            + (2 * hw_index);
        }

        switch (meter_pair_mode) {
            case BCM_FIELD_METER_MODE_DEFAULT: /* 0 */
                f_pl->cfg.mode = bcmPolicerModeGreen;
                break;

            case BCM_FIELD_METER_MODE_FLOW: /* 1 */
                switch (fg->stage_id) {
                    case _BCM_FIELD_STAGE_INGRESS:
                        f_pl->cfg.mode = bcmPolicerModeCommitted;
                        if (1 == modifier_mode) {
                            /* Flow mode using committed hardware meter. */
                            _FP_POLICER_EXCESS_HW_METER_CLEAR(f_pl);
                        } else if (0 == modifier_mode) {
                            /* Flow mode using excess hardware meter. */
                            _FP_POLICER_EXCESS_HW_METER_SET(f_pl);
                        } else {
                            sal_free(f_pl);
                            return BCM_E_INTERNAL;
                        }

                        meter_hw_idx =
                            (stage_fc->slices[_FP_DEF_INST][pool_index].start_tcam_idx * 2)
                                        + (2 * hw_index);

                        /* Update with meter_offset for flow mode meters. */
                        meter_hw_idx += meter_offset;

                        /* Get policer rates. */
                        _bcm_field_trx_meter_rate_burst_recover(unit,
                            FP_METER_TABLEm, meter_hw_idx,
                            &f_pl->cfg.ckbits_sec,
                            &f_pl->cfg.ckbits_burst,
                            &f_pl->cfg.flags);
                        break;

                    case _BCM_FIELD_STAGE_EGRESS:
                       if (meter_conf.meter_update_even) {
                            meter_offset = 0;
                            /* Flow mode using committed hardware meter. */
                            _FP_POLICER_EXCESS_HW_METER_CLEAR(f_pl);
                        } else if (meter_conf.meter_update_odd) {
                            meter_offset = 1;
                            /* Flow mode using excess hardware meter. */
                            _FP_POLICER_EXCESS_HW_METER_SET(f_pl);
                        } else {
                            sal_free(f_pl);
                            return BCM_E_INTERNAL;
                        }

                        f_pl->cfg.mode = bcmPolicerModeCommitted;

                         /* Update with meter_offset for flow mode meters. */
                        meter_hw_idx += meter_offset;

                        /* Get policer rates. */
                        _bcm_field_trx_meter_rate_burst_recover(unit,
                            EFP_METER_TABLEm, meter_hw_idx,
                            &f_pl->cfg.ckbits_sec,
                            &f_pl->cfg.ckbits_burst,
                            &f_pl->cfg.flags);
                        break;
                      /*
                       * COVERITY
                       *
                       * There are two more cases which needs to be handled.
                       * It is kept intentionally as a defensive
                       * default for future development.
                       */
                      /* coverity[dead_error_begin : FALSE] */
                    default:
                        /* Must be a valid pipeline stage. */
                        return (BCM_E_INTERNAL);
                }
                break;

            case BCM_FIELD_METER_MODE_trTCM_COLOR_BLIND: /* 2 */
                f_pl->cfg.flags |= BCM_POLICER_COLOR_BLIND;
                /* Fall through */
            case BCM_FIELD_METER_MODE_trTCM_COLOR_AWARE: /* 3 */
                f_pl->cfg.mode = bcmPolicerModeTrTcm;

                _bcm_field_trx_meter_rate_burst_recover(unit, meter_mem,
                    meter_hw_idx, &f_pl->cfg.pkbits_sec,
                    &f_pl->cfg.pkbits_burst,
                    &f_pl->cfg.flags);

                _bcm_field_trx_meter_rate_burst_recover(unit, meter_mem,
                    meter_hw_idx + 1, &f_pl->cfg.ckbits_sec,
                    &f_pl->cfg.ckbits_burst,
                    &f_pl->cfg.flags);
                break;

            case BCM_FIELD_METER_MODE_srTCM_COLOR_BLIND: /* 6 */
                f_pl->cfg.flags |= BCM_POLICER_COLOR_BLIND;
                /* Fall through */
            case BCM_FIELD_METER_MODE_srTCM_COLOR_AWARE: /* 7 */
                 if (fg->stage_id == _BCM_FIELD_STAGE_EGRESS) {
                      f_pl->cfg.mode = bcmPolicerModeSrTcm;
                  } else {
                     f_pl->cfg.mode = modifier_mode
                                          ? bcmPolicerModeSrTcmModified
                                          : bcmPolicerModeSrTcm;
                  }

                _bcm_field_trx_meter_rate_burst_recover(unit, meter_mem,
                    meter_hw_idx, &f_pl->cfg.pkbits_sec,
                    &f_pl->cfg.pkbits_burst,
                    &f_pl->cfg.flags);

                _bcm_field_trx_meter_rate_burst_recover(unit, meter_mem,
                    meter_hw_idx + 1, &f_pl->cfg.ckbits_sec,
                    &f_pl->cfg.ckbits_burst,
                    &f_pl->cfg.flags);
                break;
            case 4:
                f_pl->cfg.flags |= BCM_POLICER_COLOR_BLIND;
                /* Fall through */
            case 5:
                if (fg->stage_id != _BCM_FIELD_STAGE_EGRESS) {
                    f_pl->cfg.mode = modifier_mode
                                        ? bcmPolicerModeCoupledTrTcmDs
                                        : bcmPolicerModeTrTcmDs;
                } else {
                    f_pl->cfg.mode = bcmPolicerModeTrTcmDs;
                }
                _bcm_field_trx_meter_rate_burst_recover(unit, meter_mem,
                    meter_hw_idx, &f_pl->cfg.pkbits_sec,
                    &f_pl->cfg.pkbits_burst,
                    &f_pl->cfg.flags);

                _bcm_field_trx_meter_rate_burst_recover(unit, meter_mem,
                    meter_hw_idx + 1, &f_pl->cfg.ckbits_sec,
                    &f_pl->cfg.ckbits_burst,
                    &f_pl->cfg.flags);

                break;
            default:
                ;
        }

        /* Assume policer was created using the policer_create API */
        if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
            if (fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
                if ((part == 2) || (part == 3)) {
                    f_ent->flags |= _FP_ENTRY_POLICER_IN_SECONDARY_SLICE;
                }
            } else {
                if (part == 1) {
                    f_ent->flags |= _FP_ENTRY_POLICER_IN_SECONDARY_SLICE;
                }
            }
        }

        _FP_HASH_INSERT(fc->policer_hash, f_pl,
            (policer_id & _FP_HASH_INDEX_MASK(fc)));
        fc->policer_count++;

        /* Mark as used in the slice/pool */
        if (fg->stage_id == _BCM_FIELD_STAGE_EGRESS) {
            _FP_METER_BMP_ADD(fs->meter_bmp, meter_conf.meter_idx);
        } else {
            if (!_FP_POLICER_IS_FLOW_MODE(f_pl)) {
                _FP_METER_BMP_ADD(f_mp->meter_bmp, (f_pl->hw_index * 2));
                _FP_METER_BMP_ADD(f_mp->meter_bmp, ((f_pl->hw_index * 2) + 1));
                f_mp->free_meters -= 2;
                /* For Triumph3 devices f_mp->size = 1024*/
                if (fc->ingress_logical_policer_pools_mode ==
                                   bcmFieldIngressLogicalPolicerPools16x512Contiguous) {

                    if (((2 * f_pl->hw_index) < f_mp->size / 2) && (f_pl->hw_index >= 0)) {
                        /*index 0~511 in a same logical pool*/
                        logical_pool_idx = f_pl->pool_index * 2;
                        logicalIdx = f_pl->hw_index;
                        f_logical_mp = stage_fc->logical_meter_pool[logical_pool_idx];
                    } else if (((2 * f_pl->hw_index) >= f_mp->size / 2)
                                && (f_pl->hw_index < f_mp->size / 2)){
                        /*index 512~1023 in a same logical pool */
                        logical_pool_idx = f_pl->pool_index * 2 + 1;
                        logicalIdx = (2 * f_pl->hw_index) % (f_mp->size / 2);
                        f_logical_mp = stage_fc->logical_meter_pool[logical_pool_idx];
                    } else {
                        return BCM_E_INTERNAL;
                    }

                    _FP_METER_BMP_ADD(f_logical_mp->meter_bmp, (logicalIdx * 2));
                    _FP_METER_BMP_ADD(f_logical_mp->meter_bmp, ((logicalIdx * 2) + 1));
                    f_logical_mp->free_meters -= 2;
                    f_pl->logical_pool_index = logical_pool_idx;
                }
                if (fc->ingress_logical_policer_pools_mode ==
                                   bcmFieldIngressLogicalPolicerPools16x512Split) {
                    if (((2 * f_pl->hw_index) < f_mp->size / 2) && (f_pl->hw_index >= 0)) {
                        /*index 0~511 in a same logical pool*/
                        logical_pool_idx = f_pl->pool_index;
                        logicalIdx = f_pl->hw_index;
                        f_logical_mp = stage_fc->logical_meter_pool[logical_pool_idx];
                    } else if (((2 * f_pl->hw_index) >= f_mp->size / 2)
                                && (f_pl->hw_index < f_mp->size / 2)){
                        /*index 512~1023 in a same logical pool */
                        logical_pool_idx = f_pl->pool_index + 8;
                        logicalIdx = (2 * f_pl->hw_index) % (f_mp->size / 2);
                        f_logical_mp = stage_fc->logical_meter_pool[logical_pool_idx];
                    } else {
                        return BCM_E_INTERNAL;
                    }
                    _FP_METER_BMP_ADD(f_logical_mp->meter_bmp, (logicalIdx * 2));
                    _FP_METER_BMP_ADD(f_logical_mp->meter_bmp, ((logicalIdx * 2) + 1));
                    f_logical_mp->free_meters -= 2;
                    f_pl->logical_pool_index = logical_pool_idx;
                }

            } else {
                _FP_METER_BMP_ADD(f_mp->meter_bmp, ((f_pl->hw_index * 2) + meter_offset));
                f_mp->free_meters--;

                if (fc->ingress_logical_policer_pools_mode ==
                                   bcmFieldIngressLogicalPolicerPools16x512Contiguous) {
                    if (((2 * f_pl->hw_index) < f_mp->size / 2) && (f_pl->hw_index >= 0)) {
                        /*index 0~511 in a same logical pool*/
                        logical_pool_idx = f_pl->pool_index * 2;
                        logicalIdx = f_pl->hw_index;
                        f_logical_mp = stage_fc->logical_meter_pool[logical_pool_idx];
                    } else if (((2 * f_pl->hw_index) >= f_mp->size / 2)
                                && (f_pl->hw_index < f_mp->size / 2)){
                        /*index 512~1023 in a same logical pool */
                        logical_pool_idx = f_pl->pool_index * 2 + 1;
                        logicalIdx = (2 * f_pl->hw_index) % (f_mp->size / 2);
                        f_logical_mp = stage_fc->logical_meter_pool[logical_pool_idx];
                    } else {
                        return BCM_E_INTERNAL;
                    }
                    _FP_METER_BMP_ADD(f_logical_mp->meter_bmp, (logicalIdx * 2) + meter_offset);
                    f_logical_mp->free_meters--;
                    f_pl->logical_pool_index = logical_pool_idx;
                }
                if (fc->ingress_logical_policer_pools_mode ==
                                   bcmFieldIngressLogicalPolicerPools16x512Split) {
                    if (((2 * f_pl->hw_index) < f_mp->size / 2) && (f_pl->hw_index >= 0)) {
                        /*index 0~511 in a same logical pool*/
                        logical_pool_idx = f_pl->pool_index;
                        logicalIdx = f_pl->hw_index;
                        f_logical_mp = stage_fc->logical_meter_pool[logical_pool_idx];
                    } else if (((2 * f_pl->hw_index) >= f_mp->size / 2)
                                && (f_pl->hw_index < f_mp->size / 2)){
                        /*index 512~1023 in a same logical pool */
                        logical_pool_idx = f_pl->pool_index + 8;
                        logicalIdx = (2 * f_pl->hw_index) % (f_mp->size / 2);
                        f_logical_mp = stage_fc->logical_meter_pool[logical_pool_idx];
                    } else {
                        return BCM_E_INTERNAL;
                    }
                    _FP_METER_BMP_ADD(f_logical_mp->meter_bmp, ((logicalIdx * 2) + meter_offset));
                    f_logical_mp->free_meters--;
                    f_pl->logical_pool_index = logical_pool_idx;
                }

            }
        }

        fg->group_status.meter_count++;
    }

    /* Associate the policer object with the entry */
    f_ent_pl = &f_ent->policer[level];
    f_ent_pl->flags |= (_FP_POLICER_VALID | flags);
    f_ent_pl->pid = f_pl->pid;
    *already_recovered = 1;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_tr3_ext_part_mems
 * Purpose:
 *     Get policy mem, data mem and mask mem  
 * Parameters:
 *     unit             - (IN) BCM device number
 *     part_idx         - (IN) Slice id
 *     mems             - (OUT) mems
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr3_ext_part_mems(int unit, unsigned part_idx, soc_mem_t *mems)
{
    /* mems[0] = policy mem
       mems[1] = data mem or data-and-mask mem
       mems[2] = mask mem or INVALIDm
    */
    soc_mem_t *policy_mem=&mems[0], *tcam_data_mem=&mems[1], *tcam_mask_mem=&mems[2];

    /* ACL partition range check */
    if (part_idx >= _FP_EXT_NUM_PARTITIONS) {
        return (BCM_E_INTERNAL);
    }
    /* mem validity check */
    if (!mems) {
        return (BCM_E_INTERNAL);
    }

    BCM_IF_ERROR_RETURN(_field_tr3_external_policy_mem(unit, part_idx, policy_mem));
    BCM_IF_ERROR_RETURN(_field_tr3_external_tcam_data_mem(unit, part_idx, tcam_data_mem));
    BCM_IF_ERROR_RETURN(_field_tr3_external_tcam_mask_mem(unit, part_idx, tcam_mask_mem));

    return(BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_esw_field_tr3_ext_scache_size
 * Purpose:
 *     This function returns the configured internal scache size for a given external 
 *     Slice ID
 * Parameters:
 *     unit             - (IN) BCM device number
 *     part_idx         - (IN) Slice id
 * Returns:
 *     Configured scache size
 */
int
_bcm_esw_field_tr3_ext_scache_size(int       unit,
                                   unsigned  part_idx
                                   )
{
    soc_mem_t mems[3];

    BCM_IF_ERROR_RETURN(_field_tr3_ext_part_mems(unit, part_idx, mems));
    return (_field_trx_ext_scache_size(unit, part_idx, mems));
}

/*
 * Function:
 *     _field_tr3_ext_scache_sync_chk
 * Purpose:
 *     For each slice check the configured internal scache size, and return TRUE
 *     if configured and FALSE if not configured  
 * Parameters:
 *     unit             - (IN) BCM device number
 *     fc               - (IN) Pointer to device field control structure
 *     stage_fc         - (IN) FP stage control info.
 * Returns:
 *     TRUE / FALSE
 */
STATIC int
_field_tr3_ext_scache_sync_chk(int              unit,
                               _field_control_t *fc,
                               _field_stage_t   *stage_fc
                               )
{
    unsigned slice_idx;
    for (slice_idx = 0; slice_idx < _FP_EXT_NUM_PARTITIONS; ++slice_idx) {
        if (_bcm_esw_field_tr3_ext_scache_size(unit, slice_idx) != 0) {
            return (TRUE);
        }
    }
    return (FALSE);
}
  
/*
 * Function:
 *     _bcm_field_tr3_scache_sync
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
_bcm_field_tr3_scache_sync(int              unit,
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
    _field_meter_pool_t *f_logical_mp;

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
            if (_field_tr3_ext_scache_sync_chk(unit, fc, stage_fc)) {
                LOG_ERROR(BSL_LS_BCM_FP, \
                          (BSL_META_U(unit, \
                                      "Configuration of scache size not supported   \
                                      for external stage\n"))); 
                return(BCM_E_CONFIG);
            } 
            start_char = _FIELD_EXTFP_DATA_START;
            end_char   = _FIELD_EXTFP_DATA_END;
            break;
        default:
            return BCM_E_PARAM;
    }

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "FP(unit %d) vverb: _bcm_field_tr3_scache_sync() - Synching scache"
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
        /* ingress_logical_policer_pools_mode is synced twice.
         * It has to be synced at the beggining of the ingress
         * before slice recovery is done.
         * Hence syncing it here again.
         */
        if ((fc->wb_current_version) >= BCM_FIELD_WB_VERSION_1_23) {
            buf[fc->scache_pos] = fc->ingress_logical_policer_pools_mode;
            fc->scache_pos++;
        }
    }

    /* Save data qualifiers */
    if ((stage_fc->stage_id != _BCM_FIELD_STAGE_EXTERNAL) &&
       ((data_ctrl = stage_fc->data_ctrl) != 0)) {
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
                              "FP(unit %d) vverb: _bcm_field_tr3_scache_sync() - "
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
                                  "FP(unit %d) vverb: _bcm_field_tr3_scache_sync() -"
                                   " Slice is empty.\n"), unit));
            continue;
        }

        /* Skip master slices */
        if (stage_fc->slices[_FP_DEF_INST][slice_idx].prev == NULL) {
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

    /* Bumping up the ingress_logical_policer_pools_mode by
     * (stage_fc->tcam_slices * 2) during sync and must be
     * subracted during recovery.
     * This is done for the following reason :-
     * When no groups are installed,the last scache buf[fc->scache_pos]
     * value is 0xEF.
     * The _field_trx_scache_slice_group_recover function (last recovery
     * function) expects the buf[fc->scache_pos] value as
     * (slice_number * 2)  or 0xEF.
     * The valid ingress_logical_policer_pools_mode values are 0,1,2.
     * _field_trx_scache_slice_group_recover functions sees this as valid
     * group number and tries to recover the group and its entries. This leads
     * to undefined behaviour.Hence the values are bumped up by
     * (2 * maximum tcam) slices
     */

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        /* Saving the ingress logical policer pool mode*/
        if (((fc->wb_current_version) >= BCM_FIELD_WB_VERSION_1_4) ) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "FP(unit %d) vverb: _bcm_field_tr3_scache_sync() -"
                               " Writing Ingress Logical Policer Pool Info at scache"
                               " position %d\n"), unit, (fc->scache_pos)));
            buf[fc->scache_pos] = (stage_fc->tcam_slices * 2)
                                  + fc->ingress_logical_policer_pools_mode;
            fc->scache_pos++;
        }



        /* Saving logical meter pool info */

        for (idx = 0; idx < stage_fc->num_logical_meter_pools; idx++) {
            f_logical_mp = stage_fc->logical_meter_pool[idx];
            if (NULL == f_logical_mp) {
                return (BCM_E_INTERNAL);
            }

            if (f_logical_mp->slice_id == _FP_INVALID_INDEX) {
                fc->scache_pos++;
                continue;
            }

            buf[fc->scache_pos] = 0x80;
            /* Use 6th bit to specify the meter level */
            buf[fc->scache_pos] |= (f_logical_mp->level << 6);
            /* Use remaining to store the slice_id*/
            buf[fc->scache_pos]  += f_logical_mp->slice_id;

            fc->scache_pos++;
        }
    }

    if ((fc->wb_current_version) >= BCM_FIELD_WB_VERSION_1_15) {
       /* Mark end of Slice Info */
       buf[fc->scache_pos] = _FP_WB_END_OF_SLICES;
       fc->scache_pos++;
    }

    /* Sync EgressPortsAdd Workaround Information */
    if (soc_feature(unit, soc_feature_epc_linkflap_recover)) {
        BCM_IF_ERROR_RETURN (_field_egr_ports_recovery_scache_sync
                (unit, fc, stage_fc, &buf[fc->scache_pos]));
    }

    if ((fc->wb_current_version) >= BCM_FIELD_WB_VERSION_1_15) {
        /* Group Slice Selectors */
        BCM_IF_ERROR_RETURN(_field_group_slice_selectors_sync(unit,
                            &buf[fc->scache_pos], stage_fc->stage_id));
    }
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "FP(unit %d) vverb: _bcm_field_tr3_scache_sync() -"
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
 *     _field_tr3_ingress_entity_get
 *
 * Purpose:
 *     Reconstruct's slice ingress entity selectors.
 *
 * Parameters:
 *     unit                 - (IN) BCM device number
 *     slice_idx            - (IN) Slice number to enable
 *     fp_gm_tcam_buf       - (IN) TCAM entry
 *     slice_ent_cnt        - (IN) Number of entries in the slice
 *     stage_fc             - (IN) FP stage control info.
 *     ingress_entity_sel   - (OUT) Ingress entity selector value.
 *
 * Returns:
 *     Nothing
 */
STATIC void
_field_tr3_ingress_entity_get(int unit,
                              int slice_idx,
                              char *fp_gm_tcam_buf,
                              int slice_ent_cnt,
                              _field_stage_t *stage_fc,
                              int8 *ingress_entity_sel)
{
    fp_global_mask_tcam_entry_t *gm_tcam_entry; /* Global Mask TCAM entry. */
    uint32 svp_valid = 0;
    int idx;

    for (idx = 0; idx < slice_ent_cnt; idx++) {
        gm_tcam_entry = soc_mem_table_idx_to_pointer(unit,
                            FP_GLOBAL_MASK_TCAMm,
                            fp_global_mask_tcam_entry_t *,
                            fp_gm_tcam_buf,
                            idx + slice_ent_cnt * slice_idx
                            );
        if (soc_FP_GLOBAL_MASK_TCAMm_field32_get(unit,
                gm_tcam_entry,
                VALIDf
                )
            != 0) {
            /* Get field entry SVP_VALID bit value. */
            _field_extract((uint32 *)gm_tcam_entry,
                            69,
                            1,
                            &svp_valid
                            );
            if (svp_valid) {
                *ingress_entity_sel = _bcmFieldFwdEntityMimGport;
                break;
            }
        }
    }
    return;
}

/*
 * Function:
 *     _field_tr3_loopback_type_sel_recover
 *
 * Purpose:
 *     Retrieve slice TunnelType/LoopbackType settings from installed
 *     field entry.
 *
 * Parameters:
 *     unit             - (IN) BCM device number
 *     slice_idx        - (IN) Slice number to enable
 *     fp_gm_tcam_buf   - (IN) TCAM entry
 *     stage_fc         - (IN) FP stage control info.
 *     intraslice       - (IN) Slice in Intra-Slice Double Wide mode.
 *     tunnel_type_sel  - (OUT) Tunnel Type configured in the entry.
 *
 * Returns:
 *     Nothing
 */
STATIC int
_field_tr3_loopback_type_sel_recover(int unit,
                                     int slice_idx,
                                     char *fp_gm_tcam_buf,
                                     _field_stage_t *stage_fc,
                                     int intraslice,
                                     int8 *loopback_type_sel)
{
    fp_slice_map_entry_t fp_slice_map_buf;      /* Slice map table entry.   */
    _field_slice_t       *fs;                   /* Pointer to Slice info.   */
    _field_control_t     *fc;                   /* Pointer to field control */
    fp_global_mask_tcam_entry_t *gm_tcam_entry; /* Global Mask TCAM entry.  */
    uint32 tunnel_loopback_type = 0;            /* Selector Hw value.       */
    int slice_ent_cnt;                          /* No. of entries in slice. */
    int idx;                                    /* Slice index.             */
    int vmap_size, index;                       /* Virtual Map entry size.  */
    int virtual_group = -1;                     /* Virtual Group ID.        */
    int phy_slice;                              /* Physical slice value.    */
    
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
        VIRTUAL_SLICE_11_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
        VIRTUAL_SLICE_12_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
        VIRTUAL_SLICE_13_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
        VIRTUAL_SLICE_14_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
        VIRTUAL_SLICE_15_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
        VIRTUAL_SLICE_16_PHYSICAL_SLICE_NUMBER_ENTRY_0f
    }, slice_group[] = {
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
        VIRTUAL_SLICE_11_VIRTUAL_SLICE_GROUP_ENTRY_0f,
        VIRTUAL_SLICE_12_VIRTUAL_SLICE_GROUP_ENTRY_0f,
        VIRTUAL_SLICE_13_VIRTUAL_SLICE_GROUP_ENTRY_0f,
        VIRTUAL_SLICE_14_VIRTUAL_SLICE_GROUP_ENTRY_0f,
        VIRTUAL_SLICE_15_VIRTUAL_SLICE_GROUP_ENTRY_0f,
        VIRTUAL_SLICE_16_VIRTUAL_SLICE_GROUP_ENTRY_0f
    };
    
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    vmap_size = stage_fc->tcam_slices;
    if (fc->flags & _FP_EXTERNAL_PRESENT) {
        ++vmap_size;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit,
                                     FP_SLICE_MAPm,
                                     MEM_BLOCK_ANY,
                                     0,
                                     fp_slice_map_buf.entry_data
                                     )
                        );

    /* Get the virtual group id for given slice_idx. */
    for (index = 0; index < vmap_size; index++) {
        phy_slice = soc_mem_field32_get(unit,
                            FP_SLICE_MAPm,
                            fp_slice_map_buf.entry_data,
                            physical_slice[index]);
        if (slice_idx != phy_slice) {
            continue;
        }
        virtual_group = soc_mem_field32_get(unit,
                            FP_SLICE_MAPm,
                            fp_slice_map_buf.entry_data,
                            slice_group[index]);
        break;
    }

    /* Must be a valid virtual_group to proceed further. */
    if (-1 == virtual_group) {
        return (BCM_E_INTERNAL);
    }

    /*
     * For expanded group slices, check if any entry in
     * this slice has loopback_type_sel selector set.
     */ 
    for (index = 0; index < vmap_size; index++) {
        if (virtual_group
            != soc_mem_field32_get(unit,
                                   FP_SLICE_MAPm,
                                   fp_slice_map_buf.entry_data,
                                   slice_group[index])) {
            continue;
        }

        phy_slice = soc_mem_field32_get(unit,
                            FP_SLICE_MAPm,
                            fp_slice_map_buf.entry_data,
                            physical_slice[index]);
        fs = stage_fc->slices[_FP_DEF_INST] + phy_slice;
        slice_ent_cnt = fs->entry_count;
        
        if (intraslice) {
            slice_ent_cnt >>= 1;
        }

        for (idx = 0; idx < slice_ent_cnt; idx++) {
            gm_tcam_entry = soc_mem_table_idx_to_pointer
                                (unit,
                                 FP_GLOBAL_MASK_TCAMm,
                                 fp_global_mask_tcam_entry_t *,
                                 fp_gm_tcam_buf,
                                 idx + slice_ent_cnt * phy_slice
                                 );
            if (soc_FP_GLOBAL_MASK_TCAMm_field32_get(unit,
                                                     gm_tcam_entry,
                                                     VALIDf
                                                     ) != 0) {
                /* Extract Tunnel_Loopback_Type Info from gm_tcam_entry  */
                _field_extract((uint32 *)gm_tcam_entry,
                               77,
                               5,
                               &tunnel_loopback_type
                               );
                switch (tunnel_loopback_type) {
                    case 0x1: /* IP_TUNNEL */
                    case 0x2: /* MPLS_TUNNEL */
                    case 0x3: /* MIM_TUNNEL */
                    case 0x4: /* WLAN_CAPWAP_WTP2AC */
                    case 0x5: /* WLAN_CAPWAP_AC2AC */
                    case 0x6: /* AMT_TUNNEL */
                    case 0x7: /* TRILL_TUNNEL */
                    case 0x8: /* L2GRE_TUNNEL */
                        *loopback_type_sel = 1;
                        break;
                    case 0x10: /* MIM_LOOPBACK */
                    case 0x11: /* TRILL_NETWORK_PORT_LOOPBACK */
                    case 0x12: /* TRILL_ACCESS_PORT_LOOPBACK */
                    case 0x13: /* EP_MIRROR_LOOPBACK */
                    case 0x14: /* EP_REDIRECT_LOOPBACK */
                    case 0x15: /* DPI_SM_LOOPBACK */
                    case 0x16: /* MPLS_P2MP_LOOPBACK */
                    case 0x17: /* QCN_LOOPBACK */
                    case 0x18: /* WLAN_ENCAP_LOOPBACK */
                    case 0x19: /* WLAN_ENCAP_ENCRYPT_LOOPBACK */
                    case 0x1a: /* WLAN_DECAP_LOOPBACK */
                    case 0x1c: /* IFP_GENERIC */
                    case 0x1d: /* MPLS_EXTENDED_LOOKUP */
                    case 0x1e: /* L2GRE_LOOPBACK */
                        *loopback_type_sel = 0;
                        break;
                    default:
                        ;
                }

                if (*loopback_type_sel != _FP_SELCODE_DONT_CARE) {
                    goto done;
                }
            }
        }
    }

done:
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_field_tr3_counter_recover
 *
 * Purpose:
 *      Reconstruct's an entry's STAT configuration.
 * Parameters:
 *     unit             - (IN) BCM device number
 *     f_ent            - (IN) Pointer to a field entry
 *     policy_entry     - (IN) Policy table entry
 *     part             - (IN) Entry part count
 *     sid              - (IN) Field STAT ID
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_tr3_counter_recover(int              unit,
                           _field_entry_t   *f_ent,
                           uint32           *policy_entry,
                           int              part,
                           bcm_field_stat_t sid
                           )
{
    _field_group_t *fg;
    _field_stage_t *stage_fc;
    _field_stage_id_t stage_id;
    _field_control_t *fc;
    _field_stat_t *f_st = NULL;
    _field_entry_stat_t *f_ent_st = NULL;
    _field_cntr_pool_t *p = NULL;
    bcm_field_stat_t stat_arr[_bcmFieldStatCount];
    uint8 nstat = 0;
    uint8 hw_entry_count = 0;
    uint8 cntr_pool_num;
    uint16 cntr_hw_mode;
    uint8 in_use = 0;
    uint8 bp;
    uint8 i;
    uint32 hw_bmap = 0;
    uint32 hw_idx;
    uint32 cntr_pool_idx;
    int rv, idx, stat_id, found = 0;

    fg = f_ent->group;

    /* Get field control and stage control */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    stage_id = f_ent->group->stage_id;

    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit,
                                  stage_id,
                                  &stage_fc));

    sal_memset(stat_arr, 0, sizeof(stat_arr));

    cntr_pool_num = soc_mem_field32_get (unit,
                        FP_POLICY_TABLEm,
                        policy_entry,
                        COUNTER_POOL_NUMBERf);

    cntr_pool_idx =  soc_mem_field32_get(unit,
                            FP_POLICY_TABLEm,
                            policy_entry,
                            COUNTER_POOL_INDEXf);
    cntr_hw_mode = (soc_mem_field32_get(unit,
                        FP_POLICY_TABLEm,
                        policy_entry,
                        G_COUNTER_OFFSETf)
                    | (soc_mem_field32_get(unit,
                        FP_POLICY_TABLEm,
                        policy_entry,
                        Y_COUNTER_OFFSETf) << 2)
                    | (soc_mem_field32_get(unit,
                        FP_POLICY_TABLEm,
                        policy_entry,
                        R_COUNTER_OFFSETf) << 4));


    _field_tr3_ingress_stat_hw_mode_to_bmap(unit,
                                            cntr_hw_mode,
                                            &hw_bmap,
                                            &hw_entry_count);
    /* Get the counter pool pointer for cntr_pool_num. */
    p = stage_fc->cntr_pool[cntr_pool_num];
    if (NULL == p) {
        return (BCM_E_INTERNAL);
    }

    SHR_BITTEST_RANGE(p->cntr_bmp.w, cntr_pool_idx, hw_entry_count, in_use);

    if (in_use) {
        for (idx = 0; idx < _FP_HASH_SZ(fc); idx++) {
            f_st = fc->stat_hash[idx];
            while (f_st != NULL) {
                if ((f_st->hw_index == cntr_pool_idx)
                    && (f_st->pool_index == cntr_pool_num)
                    && (f_st->hw_mode == cntr_hw_mode)
                    && (f_st->stage_id == stage_id)) {
                    found = 1;
                    break;
                }
                f_st = f_st->next;
            }
            if (found) {
                break;
            }
        }

        if (!found) {
            return (BCM_E_INTERNAL);
        }

        f_st->hw_ref_count++;
        f_st->sw_ref_count++;
        f_ent_st = &f_ent->statistic;
    }

    if (!found) {
        /* Allocate new stat object */
        if (fc->l2warm) {
            stat_id = sid;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_field_stat_id_alloc(unit, &stat_id));
        }
        _FP_XGS3_ALLOC(f_st, sizeof (_field_stat_t), "Field stat entity");
        if (NULL == f_st) {
            return (BCM_E_MEMORY);
        }
        f_st->sw_ref_count = 2;
        f_st->hw_ref_count = 1;
        f_st->pool_index = cntr_pool_num;
        f_st->hw_index = cntr_pool_idx;
        f_st->sid = stat_id;
        f_st->stage_id = fg->stage_id;
        f_st->gid = fg->gid;
        f_st->hw_mode = cntr_hw_mode;
        f_ent_st = &f_ent->statistic;
        f_st->hw_entry_count = hw_entry_count;
        
        /*
         * COVERITY 
         *
         * _bcm_field_stat_e is internal counter type.
         *  It is kept intentionally for future development. 
         */

        for (bp = 0; bp < _bcmFieldStatCount; bp++) {
            if (hw_bmap & (1 << bp)) {
                /* coverity[mixed_enums] */
                stat_arr[nstat] = bp;
                nstat++;
            }
        }

        rv = _bcm_field_stat_array_init(unit, fc, f_st, nstat, stat_arr);
        if (BCM_FAILURE(rv)) {
            sal_free(f_st);
            return rv;
        }

        if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
            if (fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
                if ((part == 2) || (part == 3)) {
                    f_ent->flags |= _FP_ENTRY_STAT_IN_SECONDARY_SLICE;
                }
            } else {
                if (part == 1) {
                    f_ent->flags |= _FP_ENTRY_STAT_IN_SECONDARY_SLICE;
                }
            }
        }

        _FP_HASH_INSERT(fc->stat_hash,
                        f_st,
                        (stat_id & _FP_HASH_INDEX_MASK(fc)));
        fc->stat_count++;

        /* Mark counters as in use */
        hw_idx = f_st->hw_index;
        for (i = 0; i < f_st->hw_entry_count; ++i, ++hw_idx) {
            SHR_BITSET(p->cntr_bmp.w, hw_idx);
        }
        p->free_cntrs -= f_st->hw_entry_count;

        if (p->slice_id == _FP_INVALID_INDEX) {
            p->slice_id = fg->slices->slice_number;
        }

        fg->group_status.counter_count++;
    }

    /* Associate the stat object with the entry */
    f_ent_st->flags |= (_FP_ENTRY_STAT_VALID | _FP_ENTRY_STAT_INSTALLED);
    f_ent_st->sid = f_st->sid;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr3_slice_key_control_entry_recover
 *
 * Purpose:
 *     Retrieve slice key control selector values.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     slice_num    - (IN) slice number
 *     sel          - (OUT) retrieve slice key selectors
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr3_slice_key_control_entry_recover(int      unit,
                                           unsigned slice_num,
                                           _field_sel_t *sel
                                           )
{
    uint32 rval;

    /*
     * Recover Src/Dst/Intf slice class selectors
     * and Aux_tag_1 and Aux_tag_2 selectors.
     */
    BCM_IF_ERROR_RETURN
        (_field_tr2_slice_key_control_entry_recover(unit,
                                                    slice_num,
                                                    sel));
    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit,
                       IFP_KEY_CLASSID_SELECTr,
                       REG_PORT_ANY,
                       0,
                       &rval
                       )
        );

    /* Src/Dest ClassID select for F1.0 key. */
    if (0 == sel->fpf1) {
        sel->src_dest_class_f1_sel = (soc_reg_field_get(unit,
                                        IFP_KEY_CLASSID_SELECTr,
                                        rval,
                                        F1_0_SLICEf
                                        )
                                        & (1 << slice_num)
                                        );
    } else {
        sel->src_dest_class_f1_sel = _FP_SELCODE_DONT_CARE;
    }

    /* Src/Dest ClassID select for F3.0 key. */
    if (0 == sel->fpf3) {
        sel->src_dest_class_f3_sel = (soc_reg_field_get(unit,
                                        IFP_KEY_CLASSID_SELECTr,
                                        rval,
                                        F3_0_SLICEf
                                        )
                                        & (1 << slice_num)
                                        );
    } else {
        sel->src_dest_class_f3_sel = _FP_SELCODE_DONT_CARE;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_scache_logical_ingress_meter_pool_recover
 *
 * Purpose:
 *     Retrieve Ingress Logical Meter Pool information.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     fc           - (IN) Device field control structure pointer.
 *     stage_fc     - (IN) FP stage control info.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_scache_logical_ingress_meter_pool_recover (int  unit,
                                                  _field_control_t *fc,
                                                  _field_stage_t   *stage_fc)
{
    int8 isLogicalPoolModeSplit = -1;
    int8 updateHigherMeter = -1;
    int logical_pool_idx = 0;
    int pool_idx = 0;
    int entryIdx = 0;
    int startIdx = 0;
    int endIdx = 0;
    int logicalIdx = 0;
    int physicalSliceId = 0;
    _field_meter_pool_t *f_mp;
    _field_meter_pool_t *f_logical_mp;

    if (fc->ingress_logical_policer_pools_mode ==
            bcmFieldIngressLogicalPolicerPools16x512Split) {
        isLogicalPoolModeSplit = TRUE;
    } else if (fc->ingress_logical_policer_pools_mode ==
            bcmFieldIngressLogicalPolicerPools16x512Contiguous) {
        isLogicalPoolModeSplit = FALSE;
    } else {
        return (BCM_E_NONE);
    }

    for (logical_pool_idx = 0;
            logical_pool_idx < stage_fc->num_logical_meter_pools;
            logical_pool_idx++) {
        f_logical_mp = stage_fc->logical_meter_pool[logical_pool_idx];

        if (f_logical_mp->slice_id == -1)
            continue;

        if (isLogicalPoolModeSplit) {
            if (f_logical_mp->slice_id > 7) {
                updateHigherMeter = TRUE;
                physicalSliceId = f_logical_mp->slice_id  - 8;
            } else {
                physicalSliceId = f_logical_mp->slice_id;
                updateHigherMeter = FALSE;
            }
        } else if (!isLogicalPoolModeSplit) {
            if ( f_logical_mp->slice_id  % 2 == 0 ) {
                updateHigherMeter = FALSE;
                physicalSliceId = f_logical_mp->slice_id;
            } else {
                physicalSliceId = (f_logical_mp->slice_id) - 1 ;
                updateHigherMeter = TRUE;
            }
        }

        for (pool_idx = 0; pool_idx < stage_fc->num_meter_pools; pool_idx++) {
            f_mp = stage_fc->meter_pool[_FP_DEF_INST][pool_idx];

            if (f_mp->slice_id == -1) {
                continue;
            }

            if (f_mp->slice_id == physicalSliceId) {
                if (updateHigherMeter) {
                    startIdx = f_mp->size/2;
                    endIdx = f_mp->size;
                }else {
                    startIdx = 0;
                    endIdx = f_mp->size/2;
                }
                logicalIdx = 0;
                for (entryIdx = startIdx; entryIdx < endIdx; entryIdx++) {
                    if (_FP_METER_BMP_TEST(f_mp->meter_bmp, entryIdx)) {
                        _FP_METER_BMP_ADD(f_logical_mp->meter_bmp, logicalIdx);
                        f_logical_mp->free_meters -= 1;
                    }
                    logicalIdx++;
                }
            }
        }
    }
    return (BCM_E_NONE);
}

STATIC int
_bcm_field_stage_ingress_tr3_logical_meter_idx_update (int unit,
                                        _field_control_t *fc,
                                        _field_stage_t   *stage_fc)
{
    int                     entry            = 0;
    int                     level            = 0;
    int                     rv               = 0;
    uint8                   logical_pool_idx = 0;
    _field_meter_pool_t     *f_mp            = NULL;
    _field_policer_t       *f_pl             = NULL;
    _field_entry_policer_t *f_ent_pl         = NULL;
    _field_entry_t         *f_ent            = NULL;
    _field_group_t         *fg               = NULL;

    fg = fc->groups;
    /* Traverse all groups */
    while (fg != NULL) {
        if (_BCM_FIELD_STAGE_INGRESS != fg->stage_id) {
            fg = fg->next;
            continue;
        }
        for (entry = 0; entry < fg->group_status.entry_count; entry++){
            f_ent = fg->entry_arr[entry];
            for (level = 0; level < _FP_POLICER_LEVEL_COUNT; level++) {
                f_ent_pl = f_ent->policer + level;
                /* Read policer configuration.*/
                rv = _bcm_field_policer_get(unit, f_ent_pl->pid, &f_pl);
                if (BCM_E_NOT_FOUND == rv) {
                    continue;
                }
                f_mp = stage_fc->meter_pool[_FP_DEF_INST][f_pl->pool_index];
                if (!_FP_POLICER_IS_FLOW_MODE(f_pl)) {
                    /* For Triumph3 devices f_mp->size = 1024*/
                    if (fc->ingress_logical_policer_pools_mode ==
                            bcmFieldIngressLogicalPolicerPools16x512Contiguous) {

                        if (((2 * f_pl->hw_index) < f_mp->size / 2) && (f_pl->hw_index >= 0)) {
                            /*index 0~511 in a same logical pool*/
                            logical_pool_idx = f_pl->pool_index * 2;
                        } else if (((2 * f_pl->hw_index) >= f_mp->size / 2)
                                && (f_pl->hw_index < f_mp->size / 2)){
                            /*index 512~1023 in a same logical pool */
                            logical_pool_idx = f_pl->pool_index * 2 + 1;
                        } else {
                            return BCM_E_INTERNAL;
                        }

                    }
                    if (fc->ingress_logical_policer_pools_mode ==
                            bcmFieldIngressLogicalPolicerPools16x512Split) {
                        if (((2 * f_pl->hw_index) < f_mp->size / 2) && (f_pl->hw_index >= 0)) {
                            /*index 0~511 in a same logical pool*/
                            logical_pool_idx = f_pl->pool_index;
                        } else if (((2 * f_pl->hw_index) >= f_mp->size / 2)
                                && (f_pl->hw_index < f_mp->size / 2)){
                            /*index 512~1023 in a same logical pool */
                            logical_pool_idx = f_pl->pool_index + 8;
                        } else {
                            return BCM_E_INTERNAL;
                        }
                    }

                } else {
                    if (fc->ingress_logical_policer_pools_mode ==
                            bcmFieldIngressLogicalPolicerPools16x512Contiguous) {
                        if (((2 * f_pl->hw_index) < f_mp->size / 2) && (f_pl->hw_index >= 0)) {
                            /*index 0~511 in a same logical pool*/
                            logical_pool_idx = f_pl->pool_index * 2;
                        } else if (((2 * f_pl->hw_index) >= f_mp->size / 2)
                                && (f_pl->hw_index < f_mp->size / 2)){
                            /*index 512~1023 in a same logical pool */
                            logical_pool_idx = f_pl->pool_index * 2 + 1;
                        } else {
                            return BCM_E_INTERNAL;
                        }
                    }
                    if (fc->ingress_logical_policer_pools_mode ==
                            bcmFieldIngressLogicalPolicerPools16x512Split) {
                        if (((2 * f_pl->hw_index) < f_mp->size / 2) && (f_pl->hw_index >= 0)) {
                            /*index 0~511 in a same logical pool*/
                            logical_pool_idx = f_pl->pool_index;
                        } else if (((2 * f_pl->hw_index) >= f_mp->size / 2)
                                && (f_pl->hw_index < f_mp->size / 2)){
                            /*index 512~1023 in a same logical pool */
                            logical_pool_idx = f_pl->pool_index + 8;
                        } else {
                            return BCM_E_INTERNAL;
                        }
                    }
                }
                f_pl->logical_pool_index = logical_pool_idx;
                f_pl->cfg.pool_id = f_pl->logical_pool_index;
            }
        }
        fg = fg->next;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_tr3_stage_ingress_reinit
 *
 * Purpose:
 *     Reconstruct's IFP/Ingress stage groups and entries.
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
_bcm_field_tr3_stage_ingress_reinit(int              unit,
                                    _field_control_t *fc,
                                    _field_stage_t   *stage_fc
                                    )
{

    static const soc_field_t ifp_en_flds[] = {
        FP_SLICE_ENABLE_SLICE_0f,
        FP_SLICE_ENABLE_SLICE_1f,
        FP_SLICE_ENABLE_SLICE_2f,
        FP_SLICE_ENABLE_SLICE_3f,
        FP_SLICE_ENABLE_SLICE_4f,
        FP_SLICE_ENABLE_SLICE_5f,
        FP_SLICE_ENABLE_SLICE_6f,
        FP_SLICE_ENABLE_SLICE_7f,
        FP_SLICE_ENABLE_SLICE_8f,
        FP_SLICE_ENABLE_SLICE_9f,
        FP_SLICE_ENABLE_SLICE_10f,
        FP_SLICE_ENABLE_SLICE_11f,
        FP_SLICE_ENABLE_SLICE_12f,
        FP_SLICE_ENABLE_SLICE_13f,
        FP_SLICE_ENABLE_SLICE_14f,
        FP_SLICE_ENABLE_SLICE_15f
    };
    static const soc_field_t ifp_lk_en_flds[] = {
        FP_LOOKUP_ENABLE_SLICE_0f,
        FP_LOOKUP_ENABLE_SLICE_1f,
        FP_LOOKUP_ENABLE_SLICE_2f,
        FP_LOOKUP_ENABLE_SLICE_3f,
        FP_LOOKUP_ENABLE_SLICE_4f,
        FP_LOOKUP_ENABLE_SLICE_5f,
        FP_LOOKUP_ENABLE_SLICE_6f,
        FP_LOOKUP_ENABLE_SLICE_7f,
        FP_LOOKUP_ENABLE_SLICE_8f,
        FP_LOOKUP_ENABLE_SLICE_9f,
        FP_LOOKUP_ENABLE_SLICE_10f,
        FP_LOOKUP_ENABLE_SLICE_11f,
        FP_LOOKUP_ENABLE_SLICE_12f,
        FP_LOOKUP_ENABLE_SLICE_13f,
        FP_LOOKUP_ENABLE_SLICE_14f,
        FP_LOOKUP_ENABLE_SLICE_15f
    };
    static const soc_field_t ifp_double_wide_f2_key_sel[] = {
        SLICE0_DOUBLE_WIDE_F2_KEY_SELECTf,
        SLICE1_DOUBLE_WIDE_F2_KEY_SELECTf,
        SLICE2_DOUBLE_WIDE_F2_KEY_SELECTf,
        SLICE3_DOUBLE_WIDE_F2_KEY_SELECTf,
        SLICE4_DOUBLE_WIDE_F2_KEY_SELECTf,
        SLICE5_DOUBLE_WIDE_F2_KEY_SELECTf,
        SLICE6_DOUBLE_WIDE_F2_KEY_SELECTf,
        SLICE7_DOUBLE_WIDE_F2_KEY_SELECTf,
        SLICE8_DOUBLE_WIDE_F2_KEY_SELECTf,
        SLICE9_DOUBLE_WIDE_F2_KEY_SELECTf,
        SLICE10_DOUBLE_WIDE_F2_KEY_SELECTf,
        SLICE11_DOUBLE_WIDE_F2_KEY_SELECTf,
        SLICE12_DOUBLE_WIDE_F2_KEY_SELECTf,
        SLICE13_DOUBLE_WIDE_F2_KEY_SELECTf,
        SLICE14_DOUBLE_WIDE_F2_KEY_SELECTf,
        SLICE15_DOUBLE_WIDE_F2_KEY_SELECTf
    };
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    const soc_field_t _bcm_ifp_normalize_fldtbl[][2] = {
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
        { SLICE11_NORMALIZE_IP_ADDRf, SLICE11_NORMALIZE_MAC_ADDRf },
        { SLICE12_NORMALIZE_IP_ADDRf, SLICE12_NORMALIZE_MAC_ADDRf },
        { SLICE13_NORMALIZE_IP_ADDRf, SLICE13_NORMALIZE_MAC_ADDRf },
        { SLICE14_NORMALIZE_IP_ADDRf, SLICE14_NORMALIZE_MAC_ADDRf },
        { SLICE15_NORMALIZE_IP_ADDRf, SLICE15_NORMALIZE_MAC_ADDRf }
    };
#endif

    int phys_tcam_idx;                    /* Entry Hw index value.         */
    int idx, idx1, slice_idx, vslice_idx;       /* Physical/Virtual slice index. */
    int index_min, index_max, rv = BCM_E_NONE;
    int group_found;                      /* Existing FP Group status.      */
    int parts_count, slice_ent_cnt;       /* No. of FP entry parts.         */
    int i, pri_tcam_idx;                  /* Field entry TCAM index.        */
    int part_index;                       /* Field entry part number.       */
    int slice_number;                     /* Slice number.                  */
    int expanded[16];                     /* Slice exanded info.            */
    int slice_master_idx[16];                     /* Slice exanded info.            */
    int prev_prio, multigroup, max, master_slice;
    int8 s_field, d_field;                /* Src/Dst Field Sec selectors.   */
    uint8 *buf = fc->scache_ptr[_FIELD_SCACHE_PART_0]; /* Secondary storage 
                                                               base ptr.    */
    uint8 *buf1 = fc->scache_ptr[_FIELD_SCACHE_PART_1];
    uint16 recovered_ver_part0 = 0;       /* Recoved WB version             */
    char *fp_pfs_buf = NULL;              /* Buffer to read the
                                             FP_PORT_FIELD_SEL table.       */
    char *fp_policy_buf = NULL;           /* Buffer to read the FP_POLICY
                                             table.                         */
    char *fp_global_mask_tcam_buf = NULL; /* Buffer to read the
                                             FP_GLOBAL_MASK_TCAM table.     */
    uint32 *fp_tcam_buf = NULL;           /* Buffer to read the FP_TCAM
                                             table.                         */
    uint32 paired, intraslice;
    uint32 se_rval;                       /* Slice enable control value.    */
    uint32 le_rval;                       /* Slice lookup enable control
                                             register value.                */
    uint32 temp;                          /* Store IFP scache END marker.   */
    fp_port_field_sel_entry_t *pfs_entry; /* Port field selector entry.     */
    fp_tcam_entry_t *tcam_entry;          /* FP_TCAM table entry.           */
    fp_policy_table_entry_t *policy_entry; /* Policy table entry.           */
    fp_global_mask_tcam_entry_t *global_mask_tcam_entry; /* GM TCAM entry.  */
    _field_hw_qual_info_t hw_sels;        /* Hw Pri/Sec selectors.          */
    _field_slice_t *fs;                   /* Pointer to Slice info.         */
    _field_group_t *fg = NULL;            /* Pointer to group info.         */
    _field_entry_t *f_ent = NULL;         /* Field entry pointer.           */
    bcm_port_t port;                      /* Device port number.            */
    bcm_pbmp_t entry_pbmp, entry_mask_pbmp, temp_pbmp; /* Global Mask TCAM
                                            IPBM/IPBM_MASK values.          */
    soc_field_t fld;                      /* Store Memory/Register fields.  */
    _field_table_pointers_t *fp_table_pointers = NULL;
    _field_meter_pool_t *f_logical_mp;          /* Meter pool descriptor. */
    _field_entry_oam_wb_t *f_ent_oam_wb = NULL;
    _field_entry_policer_wb_t *f_ent_policer_wb = NULL;
    uint32 fwd_field_sel;
    uint8 old_physical_slice, slice_num;
    uint32 entry_flags;
    _field_slice_t *fs_temp = NULL;
    _field_action_bmp_t action_bmp;
    _field_entry_wb_info_t f_ent_wb_info;
    _field_group_t *group = NULL;            /* Pointer to group info.         */

    /* Reset Action bitmap to NULL. */
    action_bmp.w = NULL;
    fc->scache_pos = 0;
    fc->scache_pos1 = 0;

    if (fc->l2warm) {
        recovered_ver_part0 = (fc->wb_recovered_version);
        if ((fc->wb_recovered_version) > BCM_FIELD_WB_DEFAULT_VERSION) {
            /* Notify the application with an event */
            /* The application will then need to reconcile the
               version differences using the documented behavioral
               differences on per module (handle) basis */
            SOC_IF_ERROR_RETURN
                (soc_event_generate(unit,
                                    SOC_SWITCH_EVENT_WARM_BOOT_DOWNGRADE,
                                    BCM_MODULE_FIELD,
                                    (fc->wb_recovered_version),
                                    (fc->wb_current_version)
                                    )
                );
        }

        fc->scache_pos += SOC_WB_SCACHE_CONTROL_SIZE;

        if(NULL != fc->scache_ptr[_FIELD_SCACHE_PART_1]) {
            if ((fc->wb_recovered_version) > BCM_FIELD_WB_DEFAULT_VERSION) {
                /* Notify the application with an event */
                /* The application will then need to reconcile the
                   version differences using the documented behavioral
                   differences on per module (handle) basis */
                SOC_IF_ERROR_RETURN
                    (soc_event_generate(unit,
                                        SOC_SWITCH_EVENT_WARM_BOOT_DOWNGRADE,
                                        BCM_MODULE_FIELD, (fc->wb_recovered_version),
                                        (fc->wb_current_version)));
            }

            fc->scache_pos1 += SOC_WB_SCACHE_CONTROL_SIZE;
        }

    }

    /* Clear entry IPBM KEY and MASK values. */
    BCM_PBMP_CLEAR(entry_pbmp);
    BCM_PBMP_CLEAR(entry_mask_pbmp);

    /* Clear expanded slice status for all 16 Hw slices on this device. */
    sal_memset(expanded, 0, 16 * sizeof(int));
    sal_memset(slice_master_idx, 0, 16 * sizeof(int));

    /* Check IFP DATA START signature for Level2 recovery. */
    if (fc->l2warm) {
        rv = _field_scache_stage_hdr_chk(fc, _FIELD_IFP_DATA_START);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
    }

    fp_table_pointers = soc_cm_salloc(unit, sizeof(_field_table_pointers_t),
                                                "FP Table buffer Pointers");

    if (NULL == fp_table_pointers) {
        return BCM_E_MEMORY;
    }

    rv = _field_table_pointers_init(unit, fp_table_pointers);
    BCM_IF_ERROR_RETURN(rv);

    /* Allocate buffer to read out FP_TCAM table entries. */
    fp_tcam_buf = soc_cm_salloc
                    (unit,
                     sizeof(fp_tcam_entry_t)
                     * soc_mem_index_count(unit, FP_TCAMm),
                     "FP TCAM buffer"
                     );
    if (NULL == fp_tcam_buf) {
        return BCM_E_MEMORY;
    }

    sal_memset(fp_tcam_buf,
               0,
               sizeof(fp_tcam_entry_t) * soc_mem_index_count(unit, FP_TCAMm)
               );
    index_min = soc_mem_index_min(unit, FP_TCAMm);
    index_max = soc_mem_index_max(unit, FP_TCAMm);

    fs = stage_fc->slices[_FP_DEF_INST];
    if (stage_fc->flags & _FP_STAGE_HALF_SLICE) {
        slice_ent_cnt = fs->entry_count * 2;
        /* DMA in chunks */
        for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
            fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
            if ((rv = soc_mem_read_range(unit,
                        FP_TCAMm,
                        MEM_BLOCK_ALL,
                        slice_idx * slice_ent_cnt,
                        slice_idx * slice_ent_cnt + fs->entry_count / 2 - 1,
                        fp_tcam_buf + slice_idx * slice_ent_cnt
                        * soc_mem_entry_words(unit, FP_TCAMm)
                        )
                ) < 0 ) {
                goto cleanup;
            }

            if ((rv = soc_mem_read_range(unit,
                        FP_TCAMm,
                        MEM_BLOCK_ALL,
                        slice_idx * slice_ent_cnt + fs->entry_count,
                        slice_idx * slice_ent_cnt + fs->entry_count
                        + fs->entry_count / 2 - 1,
                        fp_tcam_buf
                        + (slice_idx * slice_ent_cnt + fs->entry_count)
                        * soc_mem_entry_words(unit, FP_TCAMm)
                        )
                ) < 0 ) {
                goto cleanup;
            }
        }
    } else {
        /* Readout table entries into allocated buffer. */
        if ((rv = soc_mem_read_range(unit,
                    FP_TCAMm,
                    MEM_BLOCK_ALL,
                    index_min,
                    index_max,
                    fp_tcam_buf
                    )
            ) < 0 ) {
            goto cleanup;
        }
    }

    /* FP_PORT_FIELD_SEL table buffer. */
    fp_pfs_buf = soc_cm_salloc(unit,
                    SOC_MEM_TABLE_BYTES(unit, FP_PORT_FIELD_SELm),
                    "FP PORT_FIELD_SEL buffer");
    if (NULL == fp_pfs_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, FP_PORT_FIELD_SELm);
    index_max = soc_mem_index_max(unit, FP_PORT_FIELD_SELm);

    /* Get table entries into allocated buffer. */
    if ((rv = soc_mem_read_range(unit,
                FP_PORT_FIELD_SELm,
                MEM_BLOCK_ALL,
                index_min,
                index_max,
                fp_pfs_buf
                )
        ) < 0 ) {
        goto cleanup;
    }

    /* FP_POLICY_TABLE buffer. */
    fp_policy_buf = soc_cm_salloc(unit,
                        SOC_MEM_TABLE_BYTES(unit, FP_POLICY_TABLEm),
                        "FP POLICY TABLE buffer");
    if (NULL == fp_policy_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    /* Readout table entries into allocated buffer. */
    index_min = soc_mem_index_min(unit, FP_POLICY_TABLEm);
    index_max = soc_mem_index_max(unit, FP_POLICY_TABLEm);
    if ((rv = soc_mem_read_range(unit,
                FP_POLICY_TABLEm,
                MEM_BLOCK_ALL,
                index_min,
                index_max,
                fp_policy_buf
                )
        ) < 0 ) {
        goto cleanup;
    }

    /*
     * FP_GLOBAL_MASK_TCAM table buffer.
     * Note:
     *     FIXED_KEY and DOUBLE_WIDE_F0 fields are in this table on TR3.
     *     TD/TR2 has FIXED_KEY field in FP_TCAM table.
     */
    fp_global_mask_tcam_buf = soc_cm_salloc
                                (unit,
                                 SOC_MEM_TABLE_BYTES
                                 (unit, FP_GLOBAL_MASK_TCAMm),
                                 "FP_GLOBAL_MASK_TCAM buffer"
                                 );
    if (fp_global_mask_tcam_buf == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    /* Read out FP_GLOBAL_MASK_TCAM table entries into allocated buffer. */
    index_min = soc_mem_index_min(unit, FP_GLOBAL_MASK_TCAMm);
    index_max = soc_mem_index_max(unit, FP_GLOBAL_MASK_TCAMm);
    if ((rv = soc_mem_read_range(unit,
                FP_GLOBAL_MASK_TCAMm,
                MEM_BLOCK_ALL,
                index_min,
                index_max,
                fp_global_mask_tcam_buf
                )
        ) < 0) {
        goto cleanup;
    }

    fp_table_pointers->fp_global_mask_tcam_buf = fp_global_mask_tcam_buf;

    /* Recover range checkers */
    rv = _field_range_check_reinit(unit, stage_fc, fc);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    if ((fc->l2warm) &&
        ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_23)) {
        fc->ingress_logical_policer_pools_mode = buf[fc->scache_pos];
        fc->scache_pos++;
    }

    /* Recover data qualifiers */
    rv = _field_data_qual_recover(unit, fc, stage_fc);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Get slice expansion status and virtual map */
    if ((rv = _field_tr2_ifp_slice_expanded_status_get(unit,
                                                       fc,
                                                       stage_fc,
                                                       expanded,
                                                       slice_master_idx)
        ) < 0) {
        goto cleanup;
    }

    /* Get slice enable and slice lookup enable control for IFP */
    if ((rv = READ_FP_SLICE_ENABLEr(unit, &se_rval)) < 0) {
        goto cleanup;
    }
    if ((rv = READ_FP_LOOKUP_ENABLEr(unit, &le_rval)) < 0) {
        goto cleanup;
    }

    /* Iterate over the slices */
    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "FP(unit %d) vverb: _bcm_field_tr3_stage_ingress_reinit()- "
                            "Checking slice %d...\n"), unit, slice_idx));
        /* Ignore disabled slice */
        if ((soc_reg_field_get(unit,
                FP_SLICE_ENABLEr,
                se_rval,
                ifp_en_flds[slice_idx]
                ) == 0)
            || soc_reg_field_get(unit,
                    FP_LOOKUP_ENABLEr,
                    le_rval,
                    ifp_lk_en_flds[slice_idx]
                    ) == 0) {
            continue;
        }

        /* Ignore secondary slice in paired mode */
        pfs_entry = soc_mem_table_idx_to_pointer(unit,
                        FP_PORT_FIELD_SELm,
                        fp_port_field_sel_entry_t *,
                        fp_pfs_buf,
                        0
                        );
        fld = _bcm_field_trx_slice_pairing_field[slice_idx / 2];

        /* Get slice paired mode status. */
        paired = soc_FP_PORT_FIELD_SELm_field32_get(unit,
                    pfs_entry,
                    fld);

        /* Get double wide mode slice status. */
        intraslice = soc_mem_field32_get_def(unit,
                        FP_PORT_FIELD_SELm,
                        pfs_entry,
                        _bcm_field_trx_slice_wide_mode_field[slice_idx],
                        FALSE
                        );

        /* Skip second slice (ODD slice) if paried mode is TRUE. */
        if (paired && (slice_idx % 2)) {
            continue;
        }

        /* Don't need to read selectors for expanded slice */
        if (expanded[slice_idx]) {
            continue;
        }

        /* Skip if slice has no valid groups and entries */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        slice_ent_cnt = fs->entry_count;
        for (idx = 0; idx < slice_ent_cnt; idx++) {
            /*
             * Get entry TCAM index, given
             * entry Stage, Slice no. and entry offset in slice.
             */
            if (_bcm_field_slice_offset_to_tcam_idx
                    (unit,
                     stage_fc,
                     _FP_DEF_INST,
                     slice_idx,
                     idx,
                     &phys_tcam_idx
                     ) != BCM_E_NONE) {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }

            /* Get field entry at phys_tcam_idx from the buffer. */
            tcam_entry = soc_mem_table_idx_to_pointer(unit,
                            FP_TCAMm,
                            fp_tcam_entry_t *,
                            fp_tcam_buf,
                            phys_tcam_idx
                            );
            if (soc_FP_TCAMm_field32_get(unit, tcam_entry, VALIDf) != 0) {
                break;
            }
        }

        /*
         * No VALID entry found in this slice.
         * When in Level1 recovery mode continue searching
         * for VALID entries in next slie.
         * In Level2 recovery mode, group qualifier information
         * is store in scache. So, continue with group recovery.
         */
        if ((idx == slice_ent_cnt) && !fc->l2warm) {
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

        /*
         * Iterate over FP_PORT_FIELD_SEL for all ports and this slice
         * to identify bins for selectors (also groups for level 1)
         */
        PBMP_ALL_ITER(unit, port) {
            if (IS_LB_PORT(unit, port)) {
                continue;
            }

            _FIELD_SELCODE_CLEAR(hw_sels.pri_slice[0]);
            hw_sels.pri_slice[0].intraslice
                = hw_sels.pri_slice[0].secondary = _FP_SELCODE_DONT_USE;
            _FIELD_SELCODE_CLEAR(hw_sels.pri_slice[1]);
            hw_sels.pri_slice[1].intraslice
                = hw_sels.pri_slice[1].secondary = _FP_SELCODE_DONT_USE;
            _FIELD_SELCODE_CLEAR(hw_sels.sec_slice[0]);
            hw_sels.sec_slice[0].intraslice
                = hw_sels.sec_slice[0].secondary = _FP_SELCODE_DONT_USE;
            _FIELD_SELCODE_CLEAR(hw_sels.sec_slice[1]);
            hw_sels.sec_slice[1].intraslice
                = hw_sels.sec_slice[1].secondary = _FP_SELCODE_DONT_USE;

            /* Get the FPF selectors for the port. */
            pfs_entry = soc_mem_table_idx_to_pointer
                            (unit,
                             FP_PORT_FIELD_SELm,
                             fp_port_field_sel_entry_t *,
                             fp_pfs_buf,
                             port
                             );

            /* Get primary slice's selectors */
            hw_sels.pri_slice[0].fpf1 = soc_FP_PORT_FIELD_SELm_field32_get
                                            (unit,
                                             pfs_entry,
                                             _bcm_field_trx_field_sel[slice_idx][0]
                                             );
            hw_sels.pri_slice[0].fpf2 = soc_FP_PORT_FIELD_SELm_field32_get
                                            (unit,
                                             pfs_entry,
                                             _bcm_field_trx_field_sel[slice_idx][1]
                                             );
            hw_sels.pri_slice[0].fpf3 = soc_FP_PORT_FIELD_SELm_field32_get
                                            (unit,
                                             pfs_entry,
                                             _bcm_field_trx_field_sel[slice_idx][2]
                                             );

            /*
             * Recover loopback_type_sel secondary selector
             * settings.
             */
            rv = _field_tr3_loopback_type_sel_recover
                    (unit,
                     slice_idx,
                     fp_global_mask_tcam_buf,
                     stage_fc,
                     intraslice,
                     &(hw_sels.pri_slice[0].loopback_type_sel)
                     );
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            if (SOC_REG_FIELD_VALID(unit, FP_FORCE_FORWARDING_FIELDr,
                        _bcm_trx2_vrf_force_forwarding_enable_field[slice_idx])) {
                READ_FP_FORCE_FORWARDING_FIELDr(unit, &fwd_field_sel);

                if (soc_reg_field_get(unit,
                            FP_FORCE_FORWARDING_FIELDr,
                            fwd_field_sel,
                            _bcm_trx2_vrf_force_forwarding_enable_field[slice_idx]
                            )) {
                    hw_sels.pri_slice[0].fwd_field_sel = 1;
                }
            }

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
            if (soc_mem_field_valid(unit, FP_PORT_FIELD_SELm,
                        _bcm_ifp_normalize_fldtbl[slice_idx][0])) {
                hw_sels.pri_slice[0].normalize_ip_sel = 0;
                if(soc_FP_PORT_FIELD_SELm_field32_get(
                              unit,
                              pfs_entry,
                              _bcm_ifp_normalize_fldtbl[slice_idx][0]
                            )) {
                    hw_sels.pri_slice[0].normalize_ip_sel = 1;
                }
            }

            if (soc_mem_field_valid(unit, FP_PORT_FIELD_SELm,
                        _bcm_ifp_normalize_fldtbl[slice_idx][1])) {
                hw_sels.pri_slice[0].normalize_mac_sel = 0;
                if(soc_FP_PORT_FIELD_SELm_field32_get(
                              unit,
                              pfs_entry,
                              _bcm_ifp_normalize_fldtbl[slice_idx][1]
                            )) {
                    hw_sels.pri_slice[0].normalize_mac_sel = 1;
                }
            }
#endif

            s_field = soc_FP_PORT_FIELD_SELm_field32_get
                        (unit,
                         pfs_entry,
                         _bcm_field_trx_s_type_sel[slice_idx]
                         );

            d_field = soc_FP_PORT_FIELD_SELm_field32_get
                        (unit,
                         pfs_entry,
                         _bcm_field_trx_d_type_sel[slice_idx]
                         );

            if ((hw_sels.pri_slice[0].fpf1 == 0) ||
                (hw_sels.pri_slice[0].fpf3 == 0)) {
                /* Check for L3_IIF or SVP */
                _field_tr3_ingress_entity_get
                    (unit,
                     slice_idx,
                     fp_global_mask_tcam_buf,
                     slice_ent_cnt,
                     stage_fc,
                     &(hw_sels.pri_slice[0].ingress_entity_sel)
                     );
            }

            switch (s_field) {
                case 1:
                    hw_sels.pri_slice[0].src_entity_sel
                        = _bcmFieldFwdEntityGlp;
                    break;
                case 2:
                    hw_sels.pri_slice[0].src_entity_sel
                        = _bcmFieldFwdEntityModPortGport;
                    break;
                case 3:
                    /* Need to adjust later based on actual VP */
                    hw_sels.pri_slice[0].src_entity_sel
                        = _bcmFieldFwdEntityMimGport;
                    break;
                default:
                    hw_sels.pri_slice[0].src_entity_sel
                        = _bcmFieldFwdEntityAny;
                    break;
            }

            switch (d_field) {
                case 0:
                    hw_sels.pri_slice[0].dst_fwd_entity_sel
                        = _bcmFieldFwdEntityGlp;
                    break;
                case 1:
                    hw_sels.pri_slice[0].dst_fwd_entity_sel
                        = _bcmFieldFwdEntityL3Egress;
                    break;
                case 2:
                    hw_sels.pri_slice[0].dst_fwd_entity_sel
                        = _bcmFieldFwdEntityMulticastGroup;
                    break;
                case 3:
                    /* Need to adjust later based on actual VP */
                    hw_sels.pri_slice[0].dst_fwd_entity_sel
                        = _bcmFieldFwdEntityMimGport;
                    break;
                default:
                    hw_sels.pri_slice[0].dst_fwd_entity_sel
                        = _bcmFieldFwdEntityAny;
                    break;
            }

            rv = _field_tr3_slice_key_control_entry_recover(unit,
                                                       slice_idx,
                                                       &hw_sels.pri_slice[0]
                                                       );
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            /* If intraslice, get double-wide key */
            if (intraslice) {
                uint32 dw_key_sel;

                hw_sels.pri_slice[1].intraslice = TRUE;
                hw_sels.pri_slice[1].fpf2
                    = soc_mem_field32_get
                        (unit,
                         FP_PORT_FIELD_SELm,
                         pfs_entry,
                         ifp_double_wide_f2_key_sel[slice_idx]
                         );

                rv = READ_FP_DOUBLE_WIDE_SELECTr(unit, &dw_key_sel);
                if (SOC_FAILURE(rv)) {
                   goto cleanup;
                }

                hw_sels.pri_slice[1].fpf4
                    = soc_reg_field_get
                        (unit,
                         FP_DOUBLE_WIDE_SELECTr,
                         dw_key_sel,
                         _bcm_field_trx_dw_f4_sel[slice_idx]
                         );

                hw_sels.pri_slice[1].fpf1
                    = soc_reg_field_get
                        (unit,
                         FP_DOUBLE_WIDE_SELECTr,
                         dw_key_sel,
                         _tr3_dw_f1_sel[slice_idx]
                         );

                if (soc_mem_field_valid
                        (unit,
                         FP_PORT_FIELD_SELm,
                         SLICE0_S_TYPE_SELf)) {
                    d_field = soc_mem_field32_get
                                (unit,
                                 FP_PORT_FIELD_SELm,
                                 pfs_entry,
                                 _bcm_field_trx_d_type_sel[slice_idx]
                                 );
                    switch (d_field) {
                        case 1:
                            hw_sels.pri_slice[1].dst_fwd_entity_sel
                                = _bcmFieldFwdEntityL3Egress;
                            break;
                        case 2:
                            hw_sels.pri_slice[1].dst_fwd_entity_sel
                                = _bcmFieldFwdEntityMulticastGroup;
                            break;
                        case 3:
                            /* Need to adjust later based on actual VP */
                            hw_sels.pri_slice[1].dst_fwd_entity_sel
                                = _bcmFieldFwdEntityMimGport;
                            break;
                        default:
                            hw_sels.pri_slice[1].dst_fwd_entity_sel
                                = _bcmFieldFwdEntityGlp;
                            break;
                    }
                }
            }

            /* If paired, get secondary slice's selectors */
            if (paired) {
                hw_sels.sec_slice[0].secondary = _FP_SELCODE_DONT_CARE;

                hw_sels.sec_slice[0].fpf1
                    = soc_FP_PORT_FIELD_SELm_field32_get
                        (unit,
                         pfs_entry,
                         _bcm_field_trx_field_sel[slice_idx + 1][0]
                         );
                hw_sels.sec_slice[0].fpf2
                    = soc_FP_PORT_FIELD_SELm_field32_get
                        (unit,
                         pfs_entry,
                         _bcm_field_trx_field_sel[slice_idx + 1][1]
                         );
                hw_sels.sec_slice[0].fpf3
                    = soc_FP_PORT_FIELD_SELm_field32_get
                        (unit,
                         pfs_entry,
                         _bcm_field_trx_field_sel[slice_idx + 1][2]
                         );

                if (SOC_REG_FIELD_VALID(unit, FP_FORCE_FORWARDING_FIELDr,
                            _bcm_trx2_vrf_force_forwarding_enable_field[slice_idx + 1])) {
                    READ_FP_FORCE_FORWARDING_FIELDr(unit, &fwd_field_sel);

                    if (soc_reg_field_get(unit,
                                FP_FORCE_FORWARDING_FIELDr,
                                fwd_field_sel,
                                _bcm_trx2_vrf_force_forwarding_enable_field[slice_idx + 1]
                                )) {
                        hw_sels.sec_slice[0].fwd_field_sel = 1;
                    }
                }

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
                if (soc_mem_field_valid(unit, FP_PORT_FIELD_SELm,
                            _bcm_ifp_normalize_fldtbl[slice_idx + 1][0])) {
                    hw_sels.sec_slice[0].normalize_ip_sel = 0;
                    if(soc_FP_PORT_FIELD_SELm_field32_get(
                                unit,
                                pfs_entry,
                                _bcm_ifp_normalize_fldtbl[slice_idx + 1][0]
                                )) {
                        hw_sels.sec_slice[0].normalize_ip_sel = 1;
                    }
                }

                if (soc_mem_field_valid(unit, FP_PORT_FIELD_SELm,
                            _bcm_ifp_normalize_fldtbl[slice_idx + 1][1])) {
                    hw_sels.sec_slice[0].normalize_mac_sel = 0;
                    if(soc_FP_PORT_FIELD_SELm_field32_get(
                                unit,
                                pfs_entry,
                                _bcm_ifp_normalize_fldtbl[slice_idx + 1][1]
                                )) {
                        hw_sels.sec_slice[0].normalize_mac_sel = 1;
                    }
                }
#endif

                if (soc_mem_field_valid
                        (unit,
                         FP_PORT_FIELD_SELm,
                         SLICE0_S_TYPE_SELf)) {
                    s_field = soc_FP_PORT_FIELD_SELm_field32_get
                                (unit,
                                 pfs_entry,
                                 _bcm_field_trx_s_type_sel[slice_idx + 1]
                                 );
                    d_field = soc_FP_PORT_FIELD_SELm_field32_get
                                (unit,
                                 pfs_entry,
                                 _bcm_field_trx_d_type_sel[slice_idx + 1]
                                 );

                    if ((hw_sels.sec_slice[0].fpf1 == 0)
                        || (hw_sels.sec_slice[0].fpf3 == 0)) {
                        /* Check for L3_IIF or SVP */
                        _field_tr3_ingress_entity_get
                            (unit,
                             slice_idx + 1,
                             fp_global_mask_tcam_buf,
                             slice_ent_cnt,
                             stage_fc,
                             &(hw_sels.sec_slice[0].ingress_entity_sel)
                             );
                    }
                }

                switch (s_field) {
                    case 1:
                        hw_sels.sec_slice[0].src_entity_sel
                            = _bcmFieldFwdEntityGlp;
                        break;
                    case 2:
                        hw_sels.sec_slice[0].src_entity_sel
                            = _bcmFieldFwdEntityModPortGport;
                        break;
                    case 3:
                        /* Need to adjust later based on actual VP */
                        hw_sels.sec_slice[0].src_entity_sel
                            = _bcmFieldFwdEntityMimGport;
                        break;
                    default:
                        hw_sels.sec_slice[0].src_entity_sel
                            = _bcmFieldFwdEntityAny;
                        break;
                }

                switch (d_field) {
                    case 1:
                        hw_sels.sec_slice[0].dst_fwd_entity_sel
                            = _bcmFieldFwdEntityL3Egress;
                        break;
                    case 2:
                        hw_sels.sec_slice[0].dst_fwd_entity_sel
                            = _bcmFieldFwdEntityMulticastGroup;
                        break;
                    case 3:
                        /* Need to adjust later based on actual VP */
                        hw_sels.sec_slice[0].dst_fwd_entity_sel
                            = _bcmFieldFwdEntityMimGport;
                        break;
                    default:
                        hw_sels.sec_slice[0].dst_fwd_entity_sel
                            = _bcmFieldFwdEntityAny;
                        break;
                }

                rv = _field_tr3_slice_key_control_entry_recover
                        (unit,
                         slice_idx + 1,
                         &hw_sels.sec_slice[0]
                         );
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }

                if (intraslice) {
                    uint32 dw_key_sel;

                    hw_sels.sec_slice[1].intraslice = TRUE;
                    hw_sels.sec_slice[1].fpf2
                        = soc_mem_field32_get
                            (unit,
                             FP_PORT_FIELD_SELm,
                             pfs_entry,
                             ifp_double_wide_f2_key_sel[slice_idx + 1]
                             );

                    rv = READ_FP_DOUBLE_WIDE_SELECTr(unit, &dw_key_sel);
                    if (SOC_FAILURE(rv)) {
                       goto cleanup;
                    }
                    hw_sels.sec_slice[1].fpf4
                        = soc_reg_field_get
                            (unit,
                             FP_DOUBLE_WIDE_SELECTr,
                             dw_key_sel,
                             _bcm_field_trx_dw_f4_sel[slice_idx + 1]
                             );
                    hw_sels.sec_slice[1].fpf1
                        = soc_reg_field_get
                            (unit,
                             FP_DOUBLE_WIDE_SELECTr,
                             dw_key_sel,
                             _tr3_dw_f1_sel[slice_idx + 1]
                             );

                    if (soc_mem_field_valid(unit,
                                            FP_PORT_FIELD_SELm,
                                            SLICE0_S_TYPE_SELf)) {
                        d_field = soc_mem_field32_get
                                    (unit,
                                     FP_PORT_FIELD_SELm,
                                     pfs_entry,
                                     _bcm_field_trx_d_type_sel[slice_idx + 1]
                                     );
                        switch (d_field) {
                            case 1:
                                hw_sels.sec_slice[1].dst_fwd_entity_sel
                                    = _bcmFieldFwdEntityL3Egress;
                                break;
                            case 2:
                                hw_sels.sec_slice[1].dst_fwd_entity_sel
                                    = _bcmFieldFwdEntityMulticastGroup;
                            break;
                            case 3:
                                /* Need to adjust later based on actual VP */
                                hw_sels.sec_slice[1].dst_fwd_entity_sel
                                    = _bcmFieldFwdEntityMimGport;
                                break;
                            default:
                                hw_sels.sec_slice[1].dst_fwd_entity_sel
                                    = _bcmFieldFwdEntityGlp;
                                break;
                        }
                    }
                }
            }

            /* Create a group based on HW qualifiers (or find existing) */
            rv = _field_tr2_group_construct
                    (unit, _FP_DEF_INST,
                     &hw_sels,
                     intraslice,
                     paired,
                     fc,
                     port,
                     _BCM_FIELD_STAGE_INGRESS,
                     slice_idx
                     );
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }

        /* Now go over the entries in this slice */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        slice_ent_cnt = fs->entry_count;
        if (fs->group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
            slice_ent_cnt >>= 1;
            fs->free_count >>= 1;
        }
        prev_prio = -1;
        for (idx = 0; idx < slice_ent_cnt; idx++) {
            group_found = 0;
            if (_bcm_field_slice_offset_to_tcam_idx(unit,
                    stage_fc, _FP_DEF_INST, slice_idx, idx,
                    &phys_tcam_idx) != BCM_E_NONE) {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }
            tcam_entry = soc_mem_table_idx_to_pointer
                            (unit,
                             FP_TCAMm,
                             fp_tcam_entry_t *,
                             fp_tcam_buf,
                             phys_tcam_idx
                             );
            if (soc_FP_TCAMm_field32_get(unit, tcam_entry, VALIDf) == 0) {
                continue;
            }

            /* Check which ports are applicable to this entry */
            global_mask_tcam_entry = soc_mem_table_idx_to_pointer
                                        (unit,
                                         FP_GLOBAL_MASK_TCAMm,
                                         fp_global_mask_tcam_entry_t *,
                                         fp_global_mask_tcam_buf,
                                         phys_tcam_idx
                                         );
            soc_mem_pbmp_field_get(unit,
                                   FP_GLOBAL_MASK_TCAMm,
                                   global_mask_tcam_entry,
                                   IPBMf,
                                   &entry_pbmp
                                   );
            soc_mem_pbmp_field_get(unit,
                                   FP_GLOBAL_MASK_TCAMm,
                                   global_mask_tcam_entry,
                                   IPBM_MASKf,
                                   &entry_mask_pbmp
                                   );

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

                SOC_PBMP_CLEAR(temp_pbmp);
                SOC_PBMP_ASSIGN(temp_pbmp, fg->pbmp);
                SOC_PBMP_OR(temp_pbmp, PBMP_LB(unit));
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
            rv = _bcm_field_entry_tcam_parts_count(unit,fg->stage_id,
                                                   fg->flags, &parts_count);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            _FP_XGS3_ALLOC(f_ent,
                           (parts_count * sizeof (_field_entry_t)),
                           "field entry"
                           );
            if (f_ent == NULL) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }

            for (idx1 = 0; idx1 < _FP_POLICER_LEVEL_COUNT; idx1++) {
                 f_ent->policer[idx1].pid = _FP_INVALID_INDEX;
            }

            sal_memset(&f_ent_wb_info, 0, sizeof(_field_entry_wb_info_t));
            f_ent_wb_info.sid = f_ent_wb_info.pid = f_ent_wb_info.pid1 = -1;
            if (fc->l2warm) {
                /* Recover f_ent_oam_wb structure from here */
                /* Allocate f_ent_oam_wb structure and pass it to
                 * corresponding function */
                if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_8) {
                    _FP_XGS3_ALLOC(f_ent_oam_wb, sizeof (_field_entry_oam_wb_t),
                            "Field Entry Warmboot");
                    if (f_ent_oam_wb == NULL) {
                        sal_free (f_ent);
                        return BCM_E_MEMORY;
                    }
                }
                f_ent_wb_info.f_ent_oam_wb = f_ent_oam_wb;

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
                f_ent_wb_info.action_bmp = &action_bmp;

                /* Recover f_ent_policer_wb structure from here */
                /* Allocate f_ent_policer_wb structure and pass it to
                 * corresponding function */
                if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_17) {
                    _FP_XGS3_ALLOC(f_ent_policer_wb, sizeof (_field_entry_policer_wb_t),
                            "Field Entry Warmboot");
                    if (f_ent_policer_wb == NULL) {
                        rv = BCM_E_MEMORY;
                        sal_free (f_ent);
                        goto cleanup;
                    }
                }
                f_ent_wb_info.f_ent_policer_wb = f_ent_policer_wb;

                rv = _field_trx_entry_info_retrieve(unit,
                                                    fc,
                                                    stage_fc,
                                                    multigroup,
                                                    &prev_prio,
                                                    &f_ent_wb_info
                                                    );
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    _field_entry_oam_wb_free (unit, &f_ent_oam_wb);
                    goto cleanup;
                }
            } else {
                _bcm_field_last_alloc_eid_incr(unit);
            }
            pri_tcam_idx = phys_tcam_idx;
            vslice_idx = _field_physical_to_virtual(unit, _FP_DEF_INST,
                                                    slice_idx, stage_fc);
            if (vslice_idx == -1) {
                rv = BCM_E_INTERNAL;
                sal_free(f_ent);
                _field_entry_oam_wb_free (unit, &f_ent_oam_wb);
                goto cleanup;
            }
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
                    f_ent[i].prio = (vslice_idx << 10) | (slice_ent_cnt - idx);
                }
                f_ent[i].group = fg;
                if (i == 0) {
                    BCM_PBMP_ASSIGN(f_ent[i].pbmp.data, entry_pbmp);
                    BCM_PBMP_ASSIGN(f_ent[i].pbmp.mask, entry_mask_pbmp);
                }

                rv = _bcm_field_tcam_part_to_entry_flags
                        (unit, i,
                         fg,
                         &f_ent[i].flags
                         );
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    _field_entry_oam_wb_free (unit, &f_ent_oam_wb);
                    goto cleanup;
                }
                if (f_ent_wb_info.color_independent) {
                    f_ent[i].flags |= _FP_ENTRY_COLOR_INDEPENDENT;
                }
                rv = _bcm_field_entry_part_tcam_idx_get
                        (unit,
                         f_ent,
                         pri_tcam_idx,
                         i, &part_index
                         );
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    _field_entry_oam_wb_free (unit, &f_ent_oam_wb);
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
                    _field_entry_oam_wb_free (unit, &f_ent_oam_wb);
                    goto cleanup;
                }
                f_ent[i].fs = stage_fc->slices[_FP_DEF_INST] + slice_number;
                BCM_PBMP_OR(f_ent[i].fs->pbmp, fg->pbmp);
                if (0 == (f_ent[i].flags & _FP_ENTRY_SECOND_HALF)) {
                    /* Decrement slice free entry count for primary
                       entries. */
                    f_ent[i].fs->free_count--;
                }
                /* Assign entry to a slice */
                f_ent[i].fs->entries[f_ent[i].slice_idx] = f_ent + i;
                f_ent[i].flags |= _FP_ENTRY_INSTALLED;

                if (soc_FP_TCAMm_field32_get(unit, tcam_entry, VALIDf) == 3) {
                    f_ent[i].flags |= _FP_ENTRY_ENABLED;
                }

                /* Get the actions associated with this part of the entry */
                policy_entry = soc_mem_table_idx_to_pointer
                                (unit,
                                 FP_POLICY_TABLEm,
                                 fp_policy_table_entry_t *,
                                 fp_policy_buf,
                                 part_index
                                 );
                rv = _field_tr2_actions_recover
                        (unit,
                         FP_POLICY_TABLEm,
                         (uint32 *) policy_entry,
                         f_ent,
                         i,
                         &f_ent_wb_info);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    _field_entry_oam_wb_free (unit, &f_ent_oam_wb);
                    goto cleanup;
                }
                if (action_bmp.w != NULL) {
                   _FP_ACTION_BMP_FREE(action_bmp);
                   action_bmp.w = NULL;
                }
            }

            /* Add to the group */
            rv = _field_group_entry_add(unit, fg, f_ent);
            if (BCM_FAILURE(rv)) {
                sal_free(f_ent);
                _field_entry_oam_wb_free (unit, &f_ent_oam_wb);
                goto cleanup;
            }
            f_ent = NULL;
            /* Entry has been added to group,
             * so free f_ent_oam_wb structure */
            _field_entry_oam_wb_free (unit, &f_ent_oam_wb);
            if (f_ent_policer_wb != NULL) {
                sal_free(f_ent_policer_wb);
                f_ent_policer_wb = NULL;
            }
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
        LOG_DEBUG(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "FP(unit %d) vverb: _bcm_field_tr3_stage_ingress_reinit()- "
                            "Checking slice %d...\n"), unit, slice_idx));
        if (!expanded[slice_idx]) {
            continue;
        }
        /* Ignore secondary slice in paired mode */
        pfs_entry = soc_mem_table_idx_to_pointer
                        (unit,
                         FP_PORT_FIELD_SELm,
                         fp_port_field_sel_entry_t *,
                         fp_pfs_buf,
                         0
                         );
        fld = _bcm_field_trx_slice_pairing_field[slice_idx / 2];
        paired = soc_FP_PORT_FIELD_SELm_field32_get
                    (unit,
                     pfs_entry,
                     fld
                     );
        if (paired && (slice_idx % 2)) {
            continue;
        }
        /* Skip if slice has no valid entries */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        slice_ent_cnt = fs->entry_count;
        for (idx = 0; idx < slice_ent_cnt; idx++) {
            if (_bcm_field_slice_offset_to_tcam_idx(unit,
                                                    stage_fc,
                                                    _FP_DEF_INST,
                                                    slice_idx,
                                                    idx,
                                                    &phys_tcam_idx
                                                    )
                != BCM_E_NONE
                ) {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }
            tcam_entry = soc_mem_table_idx_to_pointer
                            (unit,
                             FP_TCAMm,
                             fp_tcam_entry_t *,
                             fp_tcam_buf,
                             phys_tcam_idx
                             );
            if (soc_FP_TCAMm_field32_get(unit, tcam_entry, VALIDf) != 0) {
                break;
            }
        }
        if (idx == slice_ent_cnt) {
            continue;
        }
        /* If Level 2, retrieve the GIDs in this slice */
        if (fc->l2warm) {
            rv = _field_trx_scache_slice_group_recover (unit,
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


        /* Append slice to group's doubly-linked list of slices */
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

        /* Now go over the entries in this slice */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        fs->group_flags = fg->flags & _FP_GROUP_STATUS_MASK;

        slice_ent_cnt = fs->entry_count;
        if (fs->group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
            slice_ent_cnt >>= 1;
        }
        prev_prio = -1;
        for (idx = 0; idx < slice_ent_cnt; idx++) {
            group_found = 0;
            if (_bcm_field_slice_offset_to_tcam_idx(unit,
                                                    stage_fc,
                                                    _FP_DEF_INST,
                                                    slice_idx,
                                                    idx,
                                                    &phys_tcam_idx
                                                    )
                != BCM_E_NONE
                ) {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }
            tcam_entry = soc_mem_table_idx_to_pointer
                            (unit,
                             FP_TCAMm,
                             fp_tcam_entry_t *,
                             fp_tcam_buf,
                             phys_tcam_idx
                             );
            if (soc_FP_TCAMm_field32_get(unit, tcam_entry, VALIDf) == 0) {
                continue;
            }

            /* Check which ports are applicable to this entry */
            global_mask_tcam_entry = soc_mem_table_idx_to_pointer
                                        (unit,
                                         FP_GLOBAL_MASK_TCAMm,
                                         fp_global_mask_tcam_entry_t *,
                                         fp_global_mask_tcam_buf,
                                         phys_tcam_idx
                                         );
            soc_mem_pbmp_field_get(unit,
                                   FP_GLOBAL_MASK_TCAMm,
                                   global_mask_tcam_entry,
                                   IPBMf,
                                   &entry_pbmp
                                   );

            soc_mem_pbmp_field_get(unit,
                                   FP_GLOBAL_MASK_TCAMm,
                                   global_mask_tcam_entry,
                                   IPBM_MASKf,
                                   &entry_mask_pbmp
                                   );

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

                SOC_PBMP_CLEAR(temp_pbmp);
                SOC_PBMP_ASSIGN(temp_pbmp, fg->pbmp);
                SOC_PBMP_OR(temp_pbmp, PBMP_LB(unit));
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

            _FP_XGS3_ALLOC(f_ent,
                           (parts_count * sizeof (_field_entry_t)),
                           "field entry"
                           );
            if (f_ent == NULL) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }

            for (idx1 = 0; idx1 < _FP_POLICER_LEVEL_COUNT; idx1++) {
                 f_ent->policer[idx1].pid = _FP_INVALID_INDEX;
            }

            sal_memset(&f_ent_wb_info, 0, sizeof(_field_entry_wb_info_t));
            f_ent_wb_info.sid = f_ent_wb_info.pid = f_ent_wb_info.pid1 = -1;
            if (fc->l2warm) {
                /* Recover f_ent_oam_wb structure from here */
                /* Allocate f_ent_oam_wb structure and pass it to
                 * corresponding function */
                if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_8) {
                    _FP_XGS3_ALLOC(f_ent_oam_wb, sizeof (_field_entry_oam_wb_t),
                            "Field Entry Warmboot");
                    if (f_ent_oam_wb == NULL) {
                        sal_free (f_ent);
                        return BCM_E_MEMORY;
                    }
                }
                f_ent_wb_info.f_ent_oam_wb = f_ent_oam_wb;

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
                f_ent_wb_info.action_bmp = &action_bmp;

                /* Recover f_ent_policer_wb structure from here */
                /* Allocate f_ent_policer_wb structure and pass it to
                 * corresponding function */
                if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_17) {
                    _FP_XGS3_ALLOC(f_ent_policer_wb, sizeof (_field_entry_policer_wb_t),
                            "Field Entry Warmboot");
                    if (f_ent_policer_wb == NULL) {
                        rv = BCM_E_MEMORY;
                        sal_free (f_ent);
                        goto cleanup;
                    }
                }
                f_ent_wb_info.f_ent_policer_wb = f_ent_policer_wb;

                rv = _field_trx_entry_info_retrieve
                        (unit,
                         fc,
                         stage_fc,
                         multigroup,
                         &prev_prio,
                         &f_ent_wb_info
                         );
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    _field_entry_oam_wb_free (unit, &f_ent_oam_wb);
                    goto cleanup;
                }
            } else {
                _bcm_field_last_alloc_eid_incr(unit);
            }
            pri_tcam_idx = phys_tcam_idx;
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
                    f_ent[i].prio = (vslice_idx << 10) | (slice_ent_cnt - idx);
                }
                f_ent[i].group = fg;
                if (i == 0) {
                    BCM_PBMP_ASSIGN(f_ent[i].pbmp.data, entry_pbmp);
                    BCM_PBMP_ASSIGN(f_ent[i].pbmp.mask, entry_mask_pbmp);
                }

                rv = _bcm_field_tcam_part_to_entry_flags
                        (unit, i,
                         fg,
                         &f_ent[i].flags
                         );
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    _field_entry_oam_wb_free (unit, &f_ent_oam_wb);
                    goto cleanup;
                }
                if (f_ent_wb_info.color_independent) {
                    f_ent[i].flags |= _FP_ENTRY_COLOR_INDEPENDENT;
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
                    _field_entry_oam_wb_free (unit, &f_ent_oam_wb);
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
                    _field_entry_oam_wb_free (unit, &f_ent_oam_wb);
                    goto cleanup;
                }
                f_ent[i].fs = stage_fc->slices[_FP_DEF_INST] + slice_number;
                BCM_PBMP_OR(f_ent[i].fs->pbmp, fg->pbmp);
                if (0 == (f_ent[i].flags & _FP_ENTRY_SECOND_HALF)) {
                    /* Decrement slice free entry count for primary
                       entries. */
                    f_ent[i].fs->free_count--;
                }

                /* Assign entry to a slice */
                f_ent[i].fs->entries[f_ent[i].slice_idx] = f_ent + i;
                f_ent[i].flags |= _FP_ENTRY_INSTALLED;

                if (soc_FP_TCAMm_field32_get(unit, tcam_entry, VALIDf) == 3) {
                    f_ent[i].flags |= _FP_ENTRY_ENABLED;
                }

                /* Get the actions associated with this part of the entry */
                policy_entry = soc_mem_table_idx_to_pointer
                                (unit,
                                 FP_POLICY_TABLEm,
                                 fp_policy_table_entry_t *,
                                 fp_policy_buf,
                                 part_index
                                 );
                rv = _field_tr2_actions_recover
                        (unit,
                         FP_POLICY_TABLEm,
                         (uint32 *) policy_entry,
                         f_ent,
                         i,
                         &f_ent_wb_info);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    _field_entry_oam_wb_free (unit, &f_ent_oam_wb);
                    goto cleanup;
                }
                if (action_bmp.w != NULL) {
                   _FP_ACTION_BMP_FREE(action_bmp);
                   action_bmp.w = NULL;
                }
            }
            /* Add to the group */
            rv = _field_group_entry_add(unit, fg, f_ent);
            if (BCM_FAILURE(rv)) {
                sal_free(f_ent);
                _field_entry_oam_wb_free (unit, &f_ent_oam_wb);
                goto cleanup;
            }
            f_ent = NULL;
            /* Entry has been added to group,
             * so free f_ent_oam_wb structure */
            _field_entry_oam_wb_free (unit, &f_ent_oam_wb);
            if (f_ent_policer_wb != NULL) {
                sal_free(f_ent_policer_wb);
                f_ent_policer_wb = NULL;
            }
        }
        /* Free up the temporary slice group info */
        if (fc->l2warm) {
            _field_scache_slice_group_free(unit,
                                           fc,
                                           slice_idx
                                           );
        }
    }

    if (fc->l2warm)  {
        /* Check to skip the recovery when not sync'd in warmboot upgrade case*/
        if (recovered_ver_part0 >= BCM_FIELD_WB_VERSION_1_4) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "FP(unit %d) vverb: () Recovering Ingress Logical "
                               " Policer Pool Info from scache position %d \n"), unit,
                   fc->scache_pos));
            fc->ingress_logical_policer_pools_mode =
                buf[fc->scache_pos] - (stage_fc->tcam_slices * 2);
            fc->scache_pos++;
            if (recovered_ver_part0 < BCM_FIELD_WB_VERSION_1_23) {
                /* ingress_logical_policer_pools_mode is synced twice.
                  * WB support for it is enabled in version 1_4 and the location.
                 * of sync is changed in version 1_23. Hence, the below code is
                 * required to properly update meter indexes for the wb versions
                 * between 1_4 and 1_23.
                 */
                if (fc->ingress_logical_policer_pools_mode !=
                     bcmFieldIngressLogicalPolicerPools8x1024) {
                    LOG_DEBUG(BSL_LS_BCM_FP,
                            (BSL_META_U(unit,
                                        "FP(unit %d) Policer Pools is in non default mode."
                                        "Updating Logical Meter indixes \r\n"), unit));
                    rv =  _bcm_field_stage_ingress_tr3_logical_meter_idx_update(unit, fc, stage_fc);
                    if (BCM_FAILURE(rv)) {
                        goto cleanup;
                    }
                }
            }

            for (idx = 0; idx < stage_fc->num_logical_meter_pools; idx++) {
                f_logical_mp = stage_fc->logical_meter_pool[idx];
                if (NULL == f_logical_mp) {
                    return (BCM_E_INTERNAL);
                }

                /* Check if entry is valid using 7th bit */
                if ((buf[fc->scache_pos] & 0x80) == 0) {
                    fc->scache_pos++;
                    continue;
                }

                /* Retrieve meter level from  6th bit */
                if (buf[fc->scache_pos] & (1 << 6))
                    f_logical_mp->level = 1;
                else
                    f_logical_mp->level = 0;

                /* Use remaining to retrieve the slice_id*/
                f_logical_mp->slice_id = (buf[fc->scache_pos] & 0x0F);

                fc->scache_pos++;
            }

            rv = _field_scache_logical_ingress_meter_pool_recover
                                                 (unit, fc, stage_fc);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }

    if (fc->l2warm) {
        /* Mark end of Slice Info */
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
            fc->scache_pos++;
        }
    }


    /* To set per port flags */
    group = fc->groups;
    while (group != NULL) {
           if (group->stage_id == _BCM_FIELD_STAGE_INGRESS) {
               if (!BCM_PBMP_EQ(PBMP_ALL(unit), group->pbmp)) {
                   /* Update Group Flags to denote Port based Group */
                   group->flags |= _FP_GROUP_PER_PORT_OR_PBMP;
               }
           }
           group = group->next;
    }

    /* Recover EgressPortsAdd Workaround Information */
    if (soc_feature(unit, soc_feature_epc_linkflap_recover)) {
        if (recovered_ver_part0 >= BCM_FIELD_WB_VERSION_1_6) {
            BCM_IF_ERROR_RETURN (_field_egr_ports_recovery_scache_recover
                    (unit,fc, stage_fc, &buf[fc->scache_pos]));
        } else {
            BCM_IF_ERROR_RETURN (_field_egr_ports_recovery_wb_upgrade_add
                    (unit, fg));
        }
    }

    if (fc->l2warm) {
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
        if (temp != _FIELD_IFP_DATA_END) {
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
            if (temp != _FIELD_IFP_DATA_END) {
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
         _BCM_FIELD_STAGE_INGRESS,
         fp_table_pointers
         );

cleanup:
    if (fp_global_mask_tcam_buf) {
        soc_cm_sfree(unit, fp_global_mask_tcam_buf);
    }

    if (fp_tcam_buf) {
        soc_cm_sfree(unit, fp_tcam_buf);
    }
    if (fp_pfs_buf) {
        soc_cm_sfree(unit, fp_pfs_buf);
    }
    if (fp_policy_buf) {
        soc_cm_sfree(unit, fp_policy_buf);
    }
    if (fp_table_pointers) {
        soc_cm_sfree(unit, fp_table_pointers);
    }
    if (action_bmp.w != NULL) {
        _FP_ACTION_BMP_FREE(action_bmp);
        action_bmp.w = NULL;
    }
    if (f_ent_policer_wb != NULL) {
        sal_free(f_ent_policer_wb);
        f_ent_policer_wb = NULL;
    }
    return rv;
}

/*
 * Function:
 *     _bcm_field_tr3_stage_lookup_reinit
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
_bcm_field_tr3_stage_lookup_reinit(int unit,
                               _field_control_t *fc,
                               _field_stage_t *stage_fc)
{
    int vslice_idx, max, master_slice;
    int idx, idx1, slice_idx, index_min, index_max, ratio, rv = BCM_E_NONE;
    int group_found, mem_sz, parts_count, slice_ent_cnt, expanded[4], slice_master_idx[4];
    int i, pri_tcam_idx, part_index, slice_number, prev_prio;
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
    int offset_mode = 0, policer_index = 0;
    uint8 old_physical_slice, slice_num;
    uint32 entry_flags;
    _field_slice_t *fs_temp = NULL;
    soc_field_t vfp_en_flds[4] = {SLICE_ENABLE_SLICE_0f,
                                  SLICE_ENABLE_SLICE_1f,
                                  SLICE_ENABLE_SLICE_2f,
                                  SLICE_ENABLE_SLICE_3f};

    soc_field_t vfp_lk_en_flds[4] = {LOOKUP_ENABLE_SLICE_0f,
                                     LOOKUP_ENABLE_SLICE_1f,
                                     LOOKUP_ENABLE_SLICE_2f,
                                     LOOKUP_ENABLE_SLICE_3f};
    uint64        regval;
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

        fld = _tr3_vfp_slice_wide_mode_field[slice_idx];
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
                                                       NULL,
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

        BCM_IF_ERROR_RETURN(soc_reg64_get(unit,
                                          VFP_KEY_CONTROL_1r,
                                          REG_PORT_ANY,
                                          0,
                                          &regval
                                          )
                            );
        hw_sels.pri_slice[0].src_entity_sel = soc_reg64_field_get(unit,
                            VFP_KEY_CONTROL_1r,
                            regval,
                            s_type_fld_tbl[slice_idx]
                            );
        if (0 ==  hw_sels.pri_slice[0].src_entity_sel) {
            hw_sels.pri_slice[0].src_entity_sel = _FP_SELCODE_DONT_CARE;
        }

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
            rv = _bcm_field_entry_tcam_parts_count (unit, fg->stage_id,
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
                                                    0,
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

                if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
                    _field_adv_flex_stat_info_retrieve(unit, f_ent->statistic.sid);
                }
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
                     NULL,
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

        /* Append slice to group's doubly-linked list of slices */
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
            group_found = 0;
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

                rv = _field_trx_entry_info_retrieve
                        (unit,
                         fc,
                         stage_fc,
                         0,
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
                if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
                    _field_adv_flex_stat_info_retrieve(unit, f_ent->statistic.sid);
                }
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
 *     _bcm_field_tr3_stage_egress_reinit
 *
 * Purpose:
 *      Reconstruct EFP/Egress stage groups and entries.
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
_bcm_field_tr3_stage_egress_reinit(int unit,
                               _field_control_t *fc,
                               _field_stage_t *stage_fc)
{
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr3_profiled_actions_recover
 * Purpose:
 *     Recovering profiled actions for external stage 
 *     during level 2 warm boot
 * Parameters:
 *     unit             - (IN) BCM device number
 *     policy_mem       - (IN) Policy mem
 *     entry_idx        - (IN) Entry Id
 *     stage_fc         - (IN) Stage field control structure
 *     f_ent            - (IN/OUT) Field entry structure 
 * Returns:
 *     BCM_E_XXX
 */
int
_field_tr3_profiled_actions_recover(int             unit,
                                    soc_mem_t       policy_mem,
                                    unsigned        entry_idx,
                                    _field_stage_t  *stage_fc,
                                    _field_entry_t  *f_ent
                                    )
{
    int part_index, table_index = 0, rv = BCM_E_NONE;
    const soc_field_t *p;
    unsigned ofs = 0, n;

    bcm_field_action_t action = -1;

    /* fp policy entry with maximum width is used as it can also be used for 
       fp policy entries with lesser width (1x/2x/3x/4x) */ 
    ext_fp_policy_6x_entry_t ext_fp_policy_buf;

    esm_acl_profile_entry_t esm_acl_profile_buf;
    uint32 append = 0, action_val = 0, param0 = 0, param1 = 0, param2 = 0; 
    uint32    profiled_actions_buf[COUNTOF(esm_acl_profile_buf.entry_data)];

    soc_profile_mem_t *profile = &stage_fc->ext_act_profile;
    soc_profile_mem_table_t *table;

    /* Buffer initialization */
    sal_memset(ext_fp_policy_buf.entry_data,
                0,
                sizeof(ext_fp_policy_buf.entry_data)
                );
                
    sal_memset(esm_acl_profile_buf.entry_data,
                0,
                sizeof(esm_acl_profile_buf.entry_data)
                );

    sal_memset(profiled_actions_buf, 0, sizeof(profiled_actions_buf));

    rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY, entry_idx, 
                      ext_fp_policy_buf.entry_data);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "Error while reading policy entry\n")));
        return(rv);
    }

    part_index =  soc_mem_field32_get(unit,
                        policy_mem,
                        ext_fp_policy_buf.entry_data,
                        PROFILE_IDf
                        );  

    /* Update the reference count */
    profile = &stage_fc->ext_act_profile;
    table = &profile->tables[table_index];
    table->entries[part_index].ref_count++;
    table->entries[part_index].entries_per_set = 1;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit,
                                ESM_ACL_PROFILEm,
                                MEM_BLOCK_ANY,
                                part_index,
                                esm_acl_profile_buf.entry_data
                                )
                                );
    soc_mem_field_get(unit,
                      ESM_ACL_PROFILEm,
                      esm_acl_profile_buf.entry_data,
                      ACTIONSf,
                      profiled_actions_buf
                      );
                                
    /* Identify profiled actions from profiled_actions_buf using width 
       and offset */
    for (p = _field_tr3_external_profiled_policy_fld_tbl,
         n = COUNTOF(_field_tr3_external_profiled_policy_fld_tbl);
         n;
         --n, ++p
         ) {
        unsigned w = soc_mem_field_length(unit, FP_POLICY_TABLEm, *p);
        SHR_BITDCL val = 0;

        SHR_BITCOPY_RANGE(&val, 0, profiled_actions_buf, ofs, w);

        if (!val) {
            ofs += w;
            continue;
        }
    
        switch (*p) {
            case R_NEW_ECNf:
                param0 = val;
                break;
            case Y_NEW_ECNf:
                param1 = val;
                break;
            case G_NEW_ECNf:
                param2 = val;
                break;
            case R_CHANGE_ECNf:
                if ((param0) && !((param0 == param1) && (param1 == param2)))  {
                    action = bcmFieldActionRpEcnNew;
                    append = 1;
                } else {
                    action_val = val;
                }
                break;
            case Y_CHANGE_ECNf:
                if ((param1) && !((param0 == param1) && (param1 == param2)))  {
                    action = bcmFieldActionYpEcnNew;
                    param0 = param1; 
                    append = 1;
                } else {
                    action_val |= val << 1;
                }
                break;
            case G_CHANGE_ECNf:
                if ((param2) && !((param0 == param1) && (param1 == param2)))  {
                    action = bcmFieldActionGpEcnNew;
                    param0 = param2; 
                    append = 1;
                    break;
                } else {
                    action_val |= val << 1;
                }
                if (action_val) {
                    action = bcmFieldActionEcnNew;
                    append = 1;
                } 
                action_val = 0;
                break;
            case R_NEW_PKT_PRIf:
                param0 = val;
                break; 
            case Y_NEW_PKT_PRIf:
                param1 = val;
                break; 
            case G_NEW_PKT_PRIf:
                param2 = val;
                break; 
            case R_CHANGE_PKT_PRIf:
                switch (val) {
                    case 4:
                        action = bcmFieldActionRpPrioPktCopy;
                        append = 1;
                        break; 
                    case 5:
                        if (!((param0 == param1) && (param1 == param2)))  {
                            action = bcmFieldActionRpPrioPktNew;
                            append = 1;
                        } else {
                            action_val = val;
                        }
                        break;
                    case 6:
                        action = bcmFieldActionRpPrioPktTos;
                        append = 1;
                        break; 
                    case 7:
                        action = bcmFieldActionRpPrioPktCancel;
                        append = 1;
                        break; 
                    default:
                        break;
                }
                break;
            case Y_CHANGE_PKT_PRIf:
                switch (val) {
                    case 4:
                        action = bcmFieldActionYpPrioPktCopy;
                        append = 1;
                        break; 
                    case 5:
                        if (!((param0 == param1) && (param1 == param2)))  {
                            action = bcmFieldActionYpPrioPktNew;
                            param0 = param1;
                            append = 1;
                        } else {
                            action_val |= val << 3;
                        }
                        break; 
                    case 6:
                        action = bcmFieldActionYpPrioPktTos;
                        append = 1;
                        break; 
                    case 7:
                        action = bcmFieldActionYpPrioPktCancel;
                        append = 1;
                        break; 
                    default:
                        break;
                }
                break;
            case G_CHANGE_PKT_PRIf:
                switch (val) {
                    case 4:
                        action = bcmFieldActionGpPrioPktCopy;
                        append = 1;
                        break; 
                    case 5:
                        if (!((param0 == param1) && (param1 == param2)))  {
                            action = bcmFieldActionGpPrioPktNew;
                            param0 = param2;
                            append = 1;
                        } else {
                            action_val |= val << 6;
                        }
                        if (action_val & 0x16d) {
                            action = bcmFieldActionPrioPktNew;
                            append = 1;
                        } 
                        action_val = 0;
                        break; 
                    case 6:
                        action = bcmFieldActionGpPrioPktTos;
                        append = 1;
                        break; 
                    case 7:
                        action = bcmFieldActionGpPrioPktCancel;
                        append = 1;
                        break; 
                    default:
                        break;
                }
                break;
            case R_NEW_DSCPf:
                param0 = val;
                break;
            case Y_NEW_DSCPf:
                param1 = val;
                break;
            case G_NEW_DSCP_TOSf:
                param2 = val;
                break;
            case R_CHANGE_DSCPf:
                switch (val) {
                    case 1:
                        if (!((param0 == param1) && (param1 == param2))) {
                            action = bcmFieldActionRpDscpNew;
                            append = 1;
                        } else {
                            action_val = val;
                        }
                        break;
                    case 2:
                        action = bcmFieldActionRpDscpCancel;
                        append = 1;
                        break;
                }
                break;
            case Y_CHANGE_DSCPf:
                switch (val) {
                    case 1:
                        if (!((param0 == param1) && (param1 == param2))) {
                            action = bcmFieldActionYpDscpNew;
                            param0 = param1;
                            append = 1;
                        } else {
                            action_val |= val << 2;
                        }
                        break;
                    case 2:
                        action = bcmFieldActionYpDscpCancel;
                        append = 1;
                        break;
                }
                break;
            case G_CHANGE_DSCP_TOSf:
                switch (val) {
                    case 1:
                        action = bcmFieldActionGpTosPrecedenceNew;
                        param0 = param2; 
                        break;
                    case 2:
                        action = bcmFieldActionGpTosPrecedenceCopy;
                        append = 1;
                        break;
                    case 3:
                        if (!((param0 == param1) && (param1 == param2))) {
                            action = bcmFieldActionGpDscpNew;
                            append = 1;
                        } else {
                            action_val |= val << 4;
                        }
                        if (action_val == 0x35) {
                            action = bcmFieldActionDscpNew;
                            append = 1;
                        }
                        action_val = 0;
                        break;
                    case 4:
                        action = bcmFieldActionGpDscpCancel;
                        append = 1;
                        break;
                }
                break;
            case R_COS_INT_PRIf:
                param0 = val;
                break;
            case Y_COS_INT_PRIf:
                param1 = val;
                break;
            case G_COS_INT_PRIf:
                param2 = val;
                break;
            case R_CHANGE_COS_OR_INT_PRIf:
                switch (val) {    
                    case 2:
                        if (!((param0 == param1) && (param1 == param2)))  {
                            action = bcmFieldActionRpVlanCosQNew;
                            param0 -= 8;
                            append = 1;
                        } else {
                            action_val = val;
                        }
                        break;
                    case 4:
                        action = bcmFieldActionRpPrioIntCopy;
                        append = 1;
                        break;
                    case 5:
                        if (!((param0 == param1) && (param1 == param2)))  {
                            action = bcmFieldActionRpPrioIntNew;
                            append = 1;
                        } else {
                            action_val = val;
                        }
                        break;
                    case 6:
                        action = bcmFieldActionRpPrioIntTos;
                        append = 1;
                        break;
                    case 7:
                        action = bcmFieldActionRpPrioIntCancel;
                        append = 1;
                        break;
                    case 8:
                        if (!((param0 == param1) && (param1 == param2)))  {
                            action = bcmFieldActionRpCosQNew;
                            append = 1;
                        } else {
                            action_val = val;
                        }
                        break;
                    default:
                        break;  
                }
                break;
            case Y_CHANGE_COS_OR_INT_PRIf:
                switch (val) {    
                    case 2:
                        if (!((param0 == param1) && (param1 == param2)))  {
                            action = bcmFieldActionYpVlanCosQNew;
                            param0 = param1 - 8;
                            append = 1;
                        } else {
                            action_val |= val << 4;
                        }
                        break;
                    case 4:
                        action = bcmFieldActionYpPrioIntCopy;
                        append = 1;
                        break;
                    case 5:
                        if (!((param0 == param1) && (param1 == param2)))  {
                            action = bcmFieldActionYpPrioIntNew;
                            param0 = param1;
                            append = 1;
                        } else {
                            action_val |= val << 4;
                        }
                        break;
                    case 6:
                        action = bcmFieldActionYpPrioIntTos;
                        append = 1;
                        break;
                    case 7:
                        action = bcmFieldActionYpPrioIntCancel;
                        append = 1;
                        break;
                    case 8:
                        if (!((param0 == param1) && (param1 == param2)))  {
                            action = bcmFieldActionYpCosQNew;
                            param0 = param1;
                            append = 1;
                        } else {
                            action_val |= val << 4;
                        }
                        break;
                    default:
                        break;
                }
                break;
            case G_CHANGE_COS_OR_INT_PRIf:
                switch (val) {    
                    case 2:
                        if (!((param0 == param1) && (param1 == param2)))  {
                            action = bcmFieldActionGpVlanCosQNew;
                            param0 = param2 - 8;;
                            append = 1;
                        } else {
                            action_val |= val << 8;
                        }
                        if (action_val == 0x222) {
                            action = bcmFieldActionVlanCosQNew;
                            append = 1;
                        } 
                        action_val = 0;
                        break;
                    case 4:
                        action = bcmFieldActionGpPrioIntCopy;
                        append = 1;
                        break;
                    case 5:
                        if (!((param0 == param1) && (param1 == param2)))  {
                            action = bcmFieldActionGpPrioIntNew;
                            param0 = param2;
                            append = 1;
                        } else {
                            action_val |= val << 8;
                        }
                        if (action_val == 0x555) {
                            action = bcmFieldActionPrioIntNew;
                            append = 1;
                        } 
                        action_val = 0;
                        break;
                    case 6:
                        action = bcmFieldActionGpPrioIntTos;
                        append = 1;
                        break;
                    case 7:
                        action = bcmFieldActionYpPrioIntCancel;
                        append = 1;
                        break;
                    case 8:
                        if (!((param0 == param1) && (param1 == param2)))  {
                            action = bcmFieldActionGpCosQNew;
                            param0 = param2;
                            append = 1;
                        } else {
                            action_val |= val << 8;
                        }
                        if (action_val == 0x888) {
                            action = bcmFieldActionCosQNew;
                            append = 1;
                        } 
                        action_val = 0;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break; 
        }

        /* Update the field entry with action information */
        if (append) {
            rv = _field_trx_actions_recover_action_add(unit,
                                            f_ent,
                                            action,
                                            param0,
                                            0, 
                                            0, 
                                            0, 
                                            0,
                                            0,
                                            0 
                                            );
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            append = param0 = 0;
    	}
        ofs += w;
    }
    return(rv);        
}

/*
 * Function:
 *     _field_tr3_non_profiled_actions_recover
 * Purpose:
 *     Recovering non profiled actions for external stage 
 *     during level 1 and level 2 warm boot
 * Parameters:
 *     unit             - (IN) BCM device number
 *     policy_mem       - (IN) Policy mem
 *     entry_idx        - (IN) Entry Id
 *     sid              - (IN) Stat Id
 *     _field_entry_t   - (OUT) Field entry structure 
 * Returns:
 *     BCM_E_XXX
 */
int
_field_tr3_non_profiled_actions_recover(int             unit,
                                        soc_mem_t       policy_mem,
                                        unsigned        entry_idx,
                                        int             sid,
                                        _field_entry_t  *f_ent 
                                       )
{
    int part_index, drop_action_pending = 0, rv = BCM_E_NONE;
    unsigned ofs, n;
    ext_fp_policy_6x_entry_t ext_fp_policy_buf;

    esm_acl_action_control_entry_t esm_acl_action_control_buf;

    uint32   non_profiled_actions_buf[COUNTOF(ext_fp_policy_buf.entry_data)];
    uint32 append=0, action_val=0, param0=0, param1=0; 
    bcm_field_action_t action = -1;
    const struct _field_tr3_external_non_profiled_policy_fld *p;
    _field_control_t *fc=NULL;

    /* Buffer initialization */
    sal_memset(ext_fp_policy_buf.entry_data,
                0,
                sizeof(ext_fp_policy_buf.entry_data)
                );
    sal_memset(esm_acl_action_control_buf.entry_data,
                0,
                sizeof(esm_acl_action_control_buf.entry_data)
                );
    sal_memset(non_profiled_actions_buf, 0, sizeof(non_profiled_actions_buf));
    ofs = 0;
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    /* Read policy mem and get index to the ESM_ACL_ACTION_CONTROL */
    rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY, entry_idx, 
                      ext_fp_policy_buf.entry_data);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "Error while reading policy entry\n")));
        return(rv);
    }
    part_index =  soc_mem_field32_get(unit,
                        policy_mem,
                        ext_fp_policy_buf.entry_data,
                        POLICY_TABLE_IDf
                        );

    soc_mem_field_get(unit,
                        policy_mem,
                        ext_fp_policy_buf.entry_data,
                        NON_PROFILED_ACTIONSf,
                        non_profiled_actions_buf
                        );

    BCM_IF_ERROR_RETURN(soc_mem_read(unit,
                                ESM_ACL_ACTION_CONTROLm,
                                MEM_BLOCK_ANY,
                                part_index,
                                esm_acl_action_control_buf.entry_data
                                )
                                );

    /* Based on the action control bit set in ESM_ACL_ACTION_CONTROL table,
       extract the non profiled actions by getting width and offset */
    for (p = _field_tr3_external_non_profiled_policy_fld_tbl,
        n = COUNTOF(_field_tr3_external_non_profiled_policy_fld_tbl);
        n;
        --n, ++p) {
        if (soc_mem_field32_get(unit,
                            ESM_ACL_ACTION_CONTROLm,
                            esm_acl_action_control_buf.entry_data,
                            p->esm_acl_action_control_fld
                            )
            & BIT(p->bit_num)) {
            /* Action bit is set in ESM_ACL_ACTION_CONTROL */
            unsigned   w;
            SHR_BITDCL val=0;

            w   = soc_mem_field_length(unit,
                                    p->policy_mem,
                                    p->policy_fld
                                    );

            SHR_BITCOPY_RANGE(&val, 0, non_profiled_actions_buf, ofs, w);

            append = 0;
            /* Based on the non-profile action value, identify the action */
            switch(p->esm_acl_action_control_fld) {
                case MISC1_SET_ENABLEf:
                    switch(p->policy_fld) {
                        case DO_NOT_GENERATE_CNMf:
                            if (val) {
                                action = bcmFieldActionCnmCancel;
                                append = 1;
                            }
                            break;    
                        case DO_NOT_CHANGE_TTLf:
                            if (val) {
                                action = bcmFieldActionDoNotChangeTtl;
                                append = 1;
                            }
                            break;    
                        case DO_NOT_URPFf:
                            if (val) {
                                action = bcmFieldActionDoNotCheckUrpf;
                                append = 1;
                            }
                            break;    
                        case HGT_DLB_DISABLEf:
                            /* Not supported for external stage */
                            /* action = bcmFieldActionDynamicHgTrunkCancel */
                            break;    
                        case LAG_DLB_DISABLEf:
                            /* Not supported for external stage */
                            /* action = bcmFieldActionTrunkLoadBalanceCancel */
                            break;    
                        case ECMP_DLB_DISABLEf:
                            /* Not supported for external stage */
                            /* action = bcmFieldActionEcmpLoadBalanceCancel */
                            break;    
                        case GREEN_TO_PIDf:
                            if (val) {
                                f_ent->flags |= _FP_ENTRY_COLOR_INDEPENDENT;
                            }
                            break;    
                        case MIRROR_OVERRIDEf:
                            if (val) {
                                action = bcmFieldActionMirrorOverride;
                                append = 1;
                            }
                            break;    
                        default:
                            break;
                    }
                    break;   
            
                case MISC2_SET_ENABLEf:
                    switch(p->policy_fld) {
                        case IPFIX_CONTROLf:
                            if (val == 1) {
                                action = bcmFieldActionIpFix; 
                                append = 1;
                            } else if (val == 2) {
                                action = bcmFieldActionIpFixCancel;
                                append = 1;
                            }
                            break;
                        /* For R_DROP_PRECEDENCE, Y_DROP_PRECEDENCE, G_DROP_PRECEDENCE:
                           if param0=0 (preserve), the action is not recovered as 
                           NON_PROFILED_ACTIONS=0 in policy table */ 
                        case R_DROP_PRECEDENCEf:
                            if (val) {
                                action = bcmFieldActionRpDropPrecedence;
                                param0 = val;
                                append = 1;
                            }
                            break;
                        case Y_DROP_PRECEDENCEf:
                            if (val) {
                                action = bcmFieldActionYpDropPrecedence;
                                param0 = val;
                                append = 1;
                            }
                            break;
                        case G_DROP_PRECEDENCEf:
                            if (val) {
                                action = bcmFieldActionGpDropPrecedence;
                                param0 = val;
                                append = 1;
                            }
                            break;
                        /* Keep the drop actions pending until we process copy to cpu 
                           action */
                        case R_DROPf:
                            drop_action_pending = val;
                            break;
                        case Y_DROPf:
                            drop_action_pending |= val << 2;
                            break;
                        case G_DROPf:
                            drop_action_pending |= val << 4;
                            break;
                        default:
                            break;
                    }
                    break;
    
                case GOA_SET_ENABLEf:
                    switch(p->policy_fld) {
                        case HI_PRI_SUPPRESS_VXLTf:
                        case SUPPRESS_SW_ACTIONSf:
                        case DEFER_QOS_MARKINGSf:
                        case SUPPRESS_COLOR_SENSITIVE_ACTIONSf:
                        case SPLIT_DROP_RESOLVEf:
                        case HI_PRI_RESOLVEf:
                        case HI_PRI_ACTION_CONTROLf:
                            /* Not supported for external stage         */
                            /* action = bcmFieldActionOffloadRedirect   */
                            break;
                        default:
                            break;
                    } 
                    break;
    
                case CPU_COS_SET_ENABLEf:
                    switch(p->policy_fld) {
                        case CPU_COSf:
                            param0 = val; 
                            break;
                        case CHANGE_CPU_COSf:
                            if (val == 1) {
                                action = bcmFieldActionCosQCpuNew;
                                append = 1;
                            }
                        default:
                            break; 
                    }
                    break;
    
                case MIRROR_SET_ENABLEf:
                    switch(p->policy_fld) {
                        case MTP_INDEX0f:
                        case MTP_INDEX1f:
                        case MTP_INDEX2f:
                        case MTP_INDEX3f:
                            break;
                        case MIRRORf:
                            {
                                static const unsigned mtp_index_field[] = {
                                    MTP_INDEX0f, MTP_INDEX1f, MTP_INDEX2f, MTP_INDEX3f
                                };
        
                                uint32   ms_reg, mtp_type;
                                unsigned b, idx;
                                bcm_module_t modid;
                                bcm_gport_t  port;
        
                                /* Get the mirroring direction control setting */
                                BCM_IF_ERROR_RETURN(READ_MIRROR_SELECTr(unit, &ms_reg));
                                mtp_type = soc_reg_field_get(unit, MIRROR_SELECTr, 
                                                            ms_reg, MTP_TYPEf);
        
                                /* For each mirroring slot in the policy entry, ... */
                                for (b = 1, idx = 0; idx < COUNTOF(mtp_index_field); 
                                                                  ++idx, b <<= 1) {
                                    if (!(val & b)) {
                                        /* Slot not in use => Skip */
                                        continue;
                                    }
        
                                    _bcm_esw_mirror_mtp_to_modport(unit,
                                                idx,
                                                FALSE,
                                                mtp_type & b
                                                    ? BCM_MIRROR_PORT_EGRESS
                                                    : BCM_MIRROR_PORT_INGRESS,
                                                &modid,
                                                &port
                                                );
            
                                    rv = _field_trx_actions_recover_action_add(
                                                unit,
                                                f_ent,
                                                mtp_type & b
                                                    ? bcmFieldActionMirrorEgress
                                                    : bcmFieldActionMirrorIngress,
                                                modid,
                                                port,
                                                0,
                                                0,
                                                0,
                                                0,
                                                0
                                               );
                                    if (BCM_FAILURE(rv)) {
                                        return rv;
                                    }   
                                }
                            }
                            break;
                        default:
                            break;
                    }
                    break;
    
                case COPY_TO_CPU_SET_ENABLEf:
                    switch(p->policy_fld) {
                        case R_COPY_TO_CPUf:
                            if ((val != 5) && ((drop_action_pending & 3) == 1)) {
                                action = bcmFieldActionRpDrop;
                                append = 1;
                            } else if ((val != 2) &&
                                       ((drop_action_pending & 3) == 2)) {
                                action = bcmFieldActionRpDropCancel;
                                append = 1;
                            }
        
                            if (append) {
                                rv = _field_trx_actions_recover_action_add(unit,
                                                               f_ent,
                                                               action,
                                                               param0,
                                                               param1,
                                                               0,
                                                               0,
                                                               0,
                                                               0,
                                                               0
                                                               );
                                if (BCM_FAILURE(rv)) {
                                    return rv;
                                }
                                append = 0;
                            }
        
                            switch (val) {
                                case 1:
                                    action_val = val;
                                    break;
                                case 2:
                                    if ((drop_action_pending & 3) == 2) {
                                        action = bcmFieldActionRpTimeStampToCpuCancel;
                                    } else {
                                        action = bcmFieldActionRpCopyToCpuCancel;
                                    }
                                    append = 1;
                                    break;
                                case 3:
                                    action = bcmFieldActionRpSwitchToCpuCancel;
                                    append = 1;
                                    break;
                                case 4:
                                    action = bcmFieldActionRpSwitchToCpuReinstate;
                                    append = 1;
                                    break;
                                case 5:
                                    action_val = val;
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case Y_COPY_TO_CPUf:
                            if ((val != 5) && ((drop_action_pending >> 2) & 3) == 1) {
                                action = bcmFieldActionYpDrop;
                                append = 1;
                            } else  if ((val != 2) &&
                                       ((drop_action_pending >> 2) & 3) == 2) {
                                action = bcmFieldActionYpDropCancel;
                                append = 1;
                            }
                            if (append) {
                                rv = _field_trx_actions_recover_action_add(unit,
                                                               f_ent,
                                                               action,
                                                               param0,
                                                               param1,
                                                               0,
                                                               0,
                                                               0,
                                                               0,
                                                               0
                                                               );
                                if (BCM_FAILURE(rv)) {
                                    return rv;
                                }
                                append = 0;
                            }
        
                            switch (val) {
                                case 1:
                                    action_val |= val << 3;
                                    break;
                                case 2:
                                    if (((drop_action_pending >> 2) & 3) == 2) {
                                        action = bcmFieldActionYpTimeStampToCpuCancel;
                                    } else {
                                        action = bcmFieldActionYpCopyToCpuCancel;
                                    }
                                    append = 1;
                                    break;
                                case 3:
                                    action = bcmFieldActionYpSwitchToCpuCancel;
                                    append = 1;
                                    break;
                                case 4:
                                    action = bcmFieldActionYpSwitchToCpuReinstate;
                                    append = 1;
                                    break;
                                case 5:
                                    action_val |= val << 3;
                                    break;
                                default:
                                    break;
                            }
                            break;
        
                        case G_COPY_TO_CPUf:
                            if ((val != 5) && ((drop_action_pending >> 4) & 3) == 1) {
                                action = bcmFieldActionGpDrop;
                                append = 1;
                            } else  if ((val != 2) &&
                                       ((drop_action_pending >> 4) & 3) == 2) {
                                action = bcmFieldActionGpDropCancel;
                                append = 1;
                            }
                            if (append) {
                                rv = _field_trx_actions_recover_action_add(unit,
                                                               f_ent,
                                                               action,
                                                               param0,
                                                               param1,
                                                               0,
                                                               0,
                                                               0,
                                                               0,
                                                               0
                                                               );
                                if (BCM_FAILURE(rv)) {
                                    return rv;
                                }
                                append = 0;
                            }
        
                            switch (val) {
                                case 1:
                                    action_val |= val << 6;
                                    break;
                                case 2:
                                    if (((drop_action_pending >> 4) & 3) == 2) {
                                        action = bcmFieldActionGpTimeStampToCpuCancel;
                                    } else {
                                        action = bcmFieldActionGpCopyToCpuCancel;
                                    }
                                    append = 1;
                                    break;
                                case 3:
                                    action = bcmFieldActionGpSwitchToCpuCancel;
                                    append = 1;
                                    break;
                                case 4:
                                    action = bcmFieldActionGpSwitchToCpuReinstate;
                                    append = 1;
                                    break;
                                case 5:
                                    action_val |= val << 6; 
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case MATCHED_RULEf:
                            if (val) {
                                param0 = 1;
                                param1 = val;
                            }
        
                            /* Drop action is already set */
                            drop_action_pending = 0;
        
                            if (action_val == 0x49) {
                                action = bcmFieldActionCopyToCpu;
                                append = 1; 
                                break;    
                            } 
                            if (action_val == 0x16d) {
                                action = bcmFieldActionTimeStampToCpu;
                                append = 1; 
                                break;    
                            } 
                            /* CopyToCpu action */
                            if ((action_val & 7) == 1) {
                                action = bcmFieldActionRpCopyToCpu;
                                rv = _field_trx_actions_recover_action_add(unit,
                                                               f_ent,
                                                               action,
                                                               param0,
                                                               param1,
                                                               0,
                                                               0,
                                                               0,
                                                               0,
                                                               0
                                                               );
                                if (BCM_FAILURE(rv)) {
                                    return rv;
                                }
                            }
                            if (((action_val >> 3) & 7) == 1) {
                                action = bcmFieldActionYpCopyToCpu;
                                rv = _field_trx_actions_recover_action_add(unit,
                                                               f_ent,
                                                               action,
                                                               param0,
                                                               param1,
                                                               0,
                                                               0,
                                                               0,
                                                               0,
                                                               0
                                                               );
                                if (BCM_FAILURE(rv)) {
                                    return rv;
                                }
                            }
                            if (((action_val >> 6) & 7) == 1) {
                                action = bcmFieldActionGpCopyToCpu;
                                rv = _field_trx_actions_recover_action_add(unit,
                                                               f_ent,
                                                               action,
                                                               param0,
                                                               param1,
                                                               0,
                                                               0,
                                                               0,
                                                               0,
                                                               0
                                                               );
                                if (BCM_FAILURE(rv)) {
                                    return rv;
                                }
                            }
                            /* TimeStampToCpu action */
                            if ((action_val & 7) == 5) {
                                action = bcmFieldActionRpTimeStampToCpu;
                                rv = _field_trx_actions_recover_action_add(unit,
                                                               f_ent,
                                                               action,
                                                               param0,
                                                               param1,
                                                               0,
                                                               0,
                                                               0,
                                                               0,
                                                               0
                                                               );
                                if (BCM_FAILURE(rv)) {
                                    return rv;
                                }
                            }
                            if (((action_val >> 3) & 7) == 5) {
                                action = bcmFieldActionYpTimeStampToCpu;
                                rv = _field_trx_actions_recover_action_add(unit,
                                                               f_ent,
                                                               action,
                                                               param0,
                                                               param1,
                                                               0,
                                                               0,
                                                               0,
                                                               0,
                                                               0
                                                               );
                                if (BCM_FAILURE(rv)) {
                                    return rv;
                                }
                            }
                            if (((action_val >> 6) & 7) == 5) {
                                action = bcmFieldActionGpTimeStampToCpu;
                                rv = _field_trx_actions_recover_action_add(unit,
                                                               f_ent,
                                                               action,
                                                               param0,
                                                               param1,
                                                               0,
                                                               0,
                                                               0,
                                                               0,
                                                               0
                                                               );
                                if (BCM_FAILURE(rv)) {
                                    return rv;
                                }
                            }
                            action_val = 0;  
                            break;
                        case COPY_TO_PASSTHRU_NLFf:
                            /* TO DO  */
                            break;
                        default:
                            break;
                    }
                    break;
                case SHARED_METER_SET_ENABLEf:
                case METER_SET_ENABLEf:
                case SVC_METER_SET_ENABLEf:
                    /* TO DO: Service Meters */ 
                    break;
                case L3SW_L2_FIELDS_SET_ENABLEf:
                    /* Not supported for external stage
                       DstMacNew/ SrcMacNew/ OuterVlanNew/ VnTagNew/ VnTagDelete 
                       And for AddClassTag action, this bit is not set */   
                    break;
                case REDIRECT_SET_ENABLEf:
                    /* OffloadRedirect action is currently not supported for 
                       external stage */
                    break;
                case COUNTER_SET_ENABLEf:
                    if ((p->policy_fld == FLEX_CTR_BASE_COUNTER_IDXf) &&
                        ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_14)) {
                        if (val) {
                            BCM_IF_ERROR_RETURN(
                                _field_flex_counter_recover(unit, f_ent, 0, sid));
                        }
                    }
                    break;
                case OAM_SET_ENABLEf:
                    /* This action is not recovered as OAM_SET_ENABLE field 
                       is not set after field action add */
                    switch(p->policy_fld) {
                        case OAM_UP_MEPf:
                            action = bcmFieldActionOamUpMep;
                            append = 1;
                            break;
                        case OAM_TXf:
                            action = bcmFieldActionOamTx;
                            append = 1;
                            break;
                        case OAM_LMEP_MDLf:
                            action = bcmFieldActionOamLmepMdl;
                            append = 1;
                            break;
                        case OAM_SERVICE_PRI_MAPPING_PTRf:
                            action = bcmFieldActionOamServicePriMappingPtr;
                            append = 1;
                            break;
                        case OAM_LM_BASE_PTRf:
                            action = bcmFieldActionOamLmBasePtr;
                            append = 1;
                            break;
                        case OAM_DM_ENf:
                            action = bcmFieldActionOamDmEnable;
                            append = 1;
                            break;
                        case OAM_LM_ENf:
                            action = bcmFieldActionOamLmEnable;
                            append = 1;
                            break;
                        case OAM_LMEP_ENf:
                            action = bcmFieldActionOamLmepEnable;
                            append = 1;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    /* Do nothing */
                    break;
            }

            if (append) {
                rv = _field_trx_actions_recover_action_add(unit,
                                                       f_ent,
                                                       action,
                                                       param0,
                                                       param1,
                                                       0,
                                                       0,
                                                       0,
                                                       0,
                                                       0
                                                       );
                if (BCM_FAILURE(rv)) {
                    return rv;
                }
                append = param0 = param1 = 0;
            }

            ofs += w + p->skip_bits;
        }
    }
    /* Drop action could be pending if COPY_TO_CPU_ENABLE is not set */
    if (drop_action_pending) {
        if (drop_action_pending == 0x15) {
            action = bcmFieldActionDrop;
            append = 1;
        } else if ((drop_action_pending & 3) == 1) {
            action = bcmFieldActionRpDrop;
            append = 1;
        } else if ((drop_action_pending & 3) == 2) {
            action = bcmFieldActionRpDropCancel;
            append = 1;
        } else if (((drop_action_pending >> 2) & 3) == 1) {
            action = bcmFieldActionYpDrop;
            append = 1;
        } else if (((drop_action_pending >> 2) & 3) == 2) {
            action = bcmFieldActionYpDropCancel;
            append = 1;
        } else if (((drop_action_pending >> 4) & 3) == 1) {
            action = bcmFieldActionGpDrop;
            append = 1;
        } else if (((drop_action_pending >> 4) & 3) == 2) {
            action = bcmFieldActionGpDropCancel;
            append = 1;
        }

        if (append) {
            rv = _field_trx_actions_recover_action_add(unit,
                                                       f_ent,
                                                       action,
                                                       param0,
                                                       param1,
                                                       0,
                                                       0,
                                                       0,
                                                       0,
                                                       0
                                                       );
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }
    return(rv);
}


/*
 * Function:
 *     _field_tr3_ext_slice_to_pkt_type
 * Purpose:
 *     Mapping slice to packet type for external stage
 * Parameters:
 *     slice_idx    - (IN) slice id
 *     fs           - (OUT) Device field slice structure pointer 
 *     pkt_type     - (OUT) packet type
 */
STATIC int
_field_tr3_ext_slice_to_pkt_type(int unit,
                                 unsigned slice_idx, 
                                 _field_slice_t *fs, 
                                 unsigned *pkt_type)
{
    /* slice idx is valid for a particular pkt_type (L2, IPV4, IPV6) */
    switch (slice_idx) {
        case _FP_EXT_ACL_144_L2:
        case _FP_EXT_ACL_L2:
            fs->pkt_type[_FP_EXT_L2] = 1;
            fs->pkt_type[_FP_EXT_IP4] = 0;
            fs->pkt_type[_FP_EXT_IP6] = 0;
            *pkt_type = _FP_EXT_L2;
            break;
        case _FP_EXT_ACL_144_IPV4:
        case _FP_EXT_ACL_IPV4:
        case _FP_EXT_ACL_L2_IPV4:
            fs->pkt_type[_FP_EXT_L2] = 0;
            fs->pkt_type[_FP_EXT_IP4] = 1;
            fs->pkt_type[_FP_EXT_IP6] = 0;
            *pkt_type = _FP_EXT_IP4;
            break;
        case _FP_EXT_ACL_144_IPV6:
        case _FP_EXT_ACL_IPV6_SHORT:
        case _FP_EXT_ACL_IPV6_FULL:
        case _FP_EXT_ACL_L2_IPV6:
            fs->pkt_type[_FP_EXT_L2] = 0;
            fs->pkt_type[_FP_EXT_IP4] = 0;
            fs->pkt_type[_FP_EXT_IP6] = 1;
            *pkt_type = _FP_EXT_IP6;
            break;
        default:
            return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}   

STATIC uint32
_field_tr3_ext_acl_table_size_prop_get(int unit, unsigned part_idx) 
{
    static const char * const ext_acl_table_size_props[] = {
        spn_EXT_L2C_ACL_TABLE_SIZE,
        spn_EXT_L2_ACL_TABLE_SIZE,
        spn_EXT_IP4C_ACL_TABLE_SIZE,
        spn_EXT_IP4_ACL_TABLE_SIZE,
        spn_EXT_L2IP4_ACL_TABLE_SIZE,
        spn_EXT_IP6C_ACL_TABLE_SIZE,
        spn_EXT_IP6S_ACL_TABLE_SIZE,
        spn_EXT_IP6F_ACL_TABLE_SIZE,
        spn_EXT_L2IP6_ACL_TABLE_SIZE
    };
    return (soc_property_get(unit, ext_acl_table_size_props[part_idx], 
                             0xffffffff));
}

/*
 * Function:
 *     _field_tr3_stage_external_reinit
 * Purpose:
 *     Reconstruct ESM ACL/External stage groups and entries, reading
 *     data from hardware tables and external scache for warm boot 
 *     level 1 and level 2 recovery
 * Parameters:
 *      unit             - (IN) BCM device number
 *      fc               - (IN) Device field control structure pointer.
 *      stage_fc         - (IN) FP stage control info.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_tr3_stage_external_reinit(int              unit,
                                 _field_control_t *fc,
                                 _field_stage_t   *stage_fc
                                 )
{
    int      rv = BCM_E_NONE, wb_mode = 0, i, j;
    int      group_flags = 0;
    unsigned slice_idx;
    unsigned cum_slice_entry_cnt[TCAM_PARTITION_ACL_L2IP6 + 1 - 
                                            TCAM_PARTITION_ACL_L2];
    _field_group_t *fg    = 0;
    _field_entry_t *f_ent = 0;
    uint8 *buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    uint8 *buf1 = fc->scache_ptr[_FIELD_SCACHE_PART_1];
    uint8 val;   
    uint32 result = 0;
    uint32 db_y[24], db_x[24], sz;
    int action_res_id, idx, idx1;
    bcm_pbmp_t group_pbmp;
    _field_stat_t  *f_st;
    _field_entry_wb_info_t f_ent_wb_info;

    static int part_order[] = {
        TCAM_PARTITION_ACL_L2C,
        TCAM_PARTITION_ACL_L2,
        TCAM_PARTITION_ACL_IP4C,
        TCAM_PARTITION_ACL_IP4,
        TCAM_PARTITION_ACL_L2IP4,
        TCAM_PARTITION_ACL_IP6C,
        TCAM_PARTITION_ACL_IP6S,
        TCAM_PARTITION_ACL_IP6F,
        TCAM_PARTITION_ACL_L2IP6
    };

    if (fc->l2warm) {
        /* Recovering start sequence */
        for (i = 0; i < 4; i++) {
            val = buf[fc->scache_pos];
            ++fc->scache_pos;
            result |= val << (i * 8);
        }
        if (result != _FIELD_EXTFP_DATA_START) {
            /* Start sequence not found */
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "0:Start sequence NOT found in scache\n")));
            rv = BCM_E_INTERNAL;
            fc->l2warm = 0;
            goto cleanup;
        }
        result = 0;
        if(NULL != buf1) {
            for (i = 0; i < 4; i++) {
                val = buf1[fc->scache_pos1];
                ++fc->scache_pos1;
                result |= val << (i * 8);
            }
            if (result != _FIELD_EXTFP_DATA_START) {
                /* Start sequence not found */
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "1:Start sequence NOT found in scache\n")));
                rv = BCM_E_INTERNAL;
                fc->l2warm = 0;
                goto cleanup;
            }

        }   
        /* Recovering long-id setting */
        val = buf[fc->scache_pos];
        ++fc->scache_pos;
        fc->flags = val;

    }

    for (j = 0, i = stage_fc->tcam_slices - 1; i >= 0; --i) {
        cum_slice_entry_cnt[i] = j
            + stage_fc->slices[_FP_DEF_INST][i].entry_count;
        j = cum_slice_entry_cnt[i];
    }

    /* For each slice, find the valid entries and recover group and entry 
       information */
    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; ++slice_idx) {
        _field_slice_t *fs = &stage_fc->slices[_FP_DEF_INST][slice_idx];
        int            port; 
        unsigned       pkt_type = 0, entry_idx, entry_cnt;
        soc_mem_t      policy_mem, tcam_data_mem, tcam_mask_mem;

        /* Check for the configured external acl table size and if not 
           configured skip the slice */
        if ((sz = _field_tr3_ext_acl_table_size_prop_get(unit, slice_idx)) 
                                                         == 0xffffffff) {
            continue;
        }

        /* Get Policy mem, data mem and mask mem  */
        BCM_IF_ERROR_RETURN(_field_tr3_external_policy_mem(unit, slice_idx,
                                                           &policy_mem));
        BCM_IF_ERROR_RETURN(_field_tr3_external_tcam_data_mem(unit, slice_idx, 
                                                              &tcam_data_mem));
        BCM_IF_ERROR_RETURN(_field_tr3_external_tcam_mask_mem(unit, slice_idx, 
                                                              &tcam_mask_mem));

        /* Update the packet type based on the slice id */ 
        _field_tr3_ext_slice_to_pkt_type(unit, slice_idx, fs, &pkt_type);

        /* Find all valid entries in slice's TCAM */
        for (entry_cnt = entry_idx = 0;
             entry_idx < fs->entry_count;
             ++entry_idx
             ) {
            int                     valid_ent; 
            ext_fp_policy_entry_t   policy_buf;
            int                     multigroup=0;
            int                     prev_prio; 
            unsigned                qual_idx;
            _bcm_field_qual_info_t *f_qual_arr;
            ext_acl432_tcam_data_entry_t tcam_buf;
            ext_acl432_tcam_mask_entry_t tcam_mask;

            /* First level check for valid entries using policy table. If valid 
               entry found, go ahead with group and entry recovery process */
            valid_ent = 0;
            rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY, entry_idx, 
                                                       policy_buf.entry_data);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            if (soc_mem_field32_get(unit, policy_mem, policy_buf.entry_data, 
                                    NON_PROFILED_ACTIONSf) != 0) {
                valid_ent = 1;
            } 

            /* Second level check for valid entries using Vbit that reads the 
               entry directly from tcam. As it is raw access to tcam which 
               writes to registers, need to do warm boot done temporarily */
            if (!valid_ent) {
                if (!wb_mode) {
                    SOC_WARM_BOOT_DONE(unit);
                    wb_mode = 1;                
                }
                rv = soc_tr3_tcam_read_entry(unit, part_order[slice_idx], 
                                             entry_idx, db_y, db_x, &valid_ent);
                if (BCM_FAILURE(rv)) {
                    if (wb_mode) {                
                        SOC_WARM_BOOT_START(unit);
                        wb_mode = 0;
                    }
                    goto cleanup;
                }

                if (!valid_ent) {
                /* if the valid entry is not found, continue in non-warmboot state,
                   to check for the next valid entry */     
                    continue;
                }		
                /* If the valid entry is found, rest of the processing is done
                   in warm boot */   
                if (wb_mode) {                
                    SOC_WARM_BOOT_START(unit);
                    wb_mode = 0;
                }
            }

            /* TCAM entry in use */
            if (entry_cnt == 0) {
                /* First in-use entry found => Create group */
                int multigroup = FALSE; /* Will always be FALSE for external */
                bcm_field_group_t gid;
                int               priority;
                bcm_field_qset_t  qset;
                prev_prio = -1;

                /* If Level 2, retrieve the GIDs in this slice and for  
                   external stage it will be exactly 1 group           */
                if (fc->l2warm) {
                    /* Recover group info */
                    rv = _field_trx_scache_slice_group_recover(unit,
                                                               fc, _FP_DEF_INST,
                                                               slice_idx,
                                                               &multigroup,
                                                               stage_fc,
                                                               0);
                    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                        LOG_ERROR(BSL_LS_BCM_FP, \
                                  (BSL_META_U(unit, \
                                              "Group recovery failed during    \
                                              level 2 warmboot\n")));
                        fc->l2warm = 0;
                        goto cleanup;
                    }
                    if (rv == BCM_E_NOT_FOUND) {
                        rv = BCM_E_NONE;
                        continue;
                    }
                }

                _FP_XGS3_ALLOC(fg, sizeof(*fg), "field group");
                if (fg == NULL) {
                    LOG_ERROR(BSL_LS_BCM_FP, \
                              (BSL_META_U(unit, \
                                          "Error while allocating memory for   \
                                          field group\n")));
                    return BCM_E_MEMORY;
                }

                /* Initialize action res id and VMAP group to default */
                action_res_id = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
                for (idx = 0; idx < _FP_PAIR_MAX; idx++) {
                    fg->vmap_group[idx] = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
                }

                group_flags = 0;
                if (fc->l2warm) {
                    /* Get stored group ID and QSET for Level 2 */
                    rv = _field_group_info_retrieve(unit,
                                                    0 /* port */, _FP_DEF_INST,
                                                    &gid,
                                                    &priority,
                                                    &action_res_id,
                                                    &group_pbmp,
                                                    NULL,
                                                    &group_flags,
                                                    &qset,
                                                    fc
                                                    );
                    if (gid != -1) {
                        sal_memcpy(&fg->qset, &qset, sizeof(fg->qset));
                    } else {
                        rv = BCM_E_INTERNAL;
                    }
                } else { 
                    /* Generate group ID for Level 1 */
                    if ((rv = _bcm_field_group_id_generate(unit, &gid)) 
                                                              == BCM_E_NONE) {
                        unsigned vmap, vslice;

                        for (priority = -1, vmap = 0; 
                             priority == -1 && vmap < _FP_VMAP_CNT; 
                             ++vmap) {
                            for (vslice = 0; 
                                 vslice < COUNTOF(stage_fc->vmap[_FP_DEF_INST][0]); 
                                 ++vslice) {
                                if (stage_fc->vmap[_FP_DEF_INST][vmap][vslice].vmap_key 
                                                              == slice_idx) {
                                    priority = 
                                       stage_fc->vmap[_FP_DEF_INST][vmap][vslice].priority;

                                    break;
                                }
                            }
                        }

                        if (priority == -1) {
                            rv = BCM_E_INTERNAL;
                        }
                    }

                    /* Recover group's qset */
                    BCM_FIELD_QSET_INIT(fg->qset);
                    BCM_FIELD_QSET_ADD(fg->qset, bcmFieldQualifyStage);
                    BCM_FIELD_QSET_ADD(fg->qset, bcmFieldQualifyStageExternal);

                    for (qual_idx = 0; qual_idx < _bcmFieldQualifyCount; 
                                                                ++qual_idx) {
                        f_qual_arr = stage_fc->f_qual_arr[qual_idx];

                        if (f_qual_arr == NULL) {
                            /* Qualifier does not exist in this stage */
                            continue; 
                        }

                        for (i = 0; i < f_qual_arr->conf_sz; ++i) {
                            if (f_qual_arr->conf_arr[i].selector.pri_sel_val
                                                              == slice_idx) {
                                BCM_FIELD_QSET_ADD(fg->qset, qual_idx);

                                break;
                            }
                        }
                    }
                }

                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
                /* Recover group's qual_arr */
                for (fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].size = qual_idx = 0; 
                     qual_idx < _bcmFieldQualifyCount; ++qual_idx) {
                    if (BCM_FIELD_QSET_TEST(fg->qset, qual_idx)) {
                        ++fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].size;
                    }
                }

                _FP_XGS3_ALLOC(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].qid_arr, 
                   fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].size * sizeof(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].qid_arr[0]),
                   "Group qual id"
                   );
                _FP_XGS3_ALLOC(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].offset_arr, 
                   fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].size * sizeof(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].offset_arr[0]),
                   "Group qual offset"
                   );

                for (j = qual_idx = 0; qual_idx < _bcmFieldQualifyCount; 
                                       ++qual_idx) {
                    if (!BCM_FIELD_QSET_TEST(fg->qset, qual_idx)) {
                        continue;
                    }

                    fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].qid_arr[j] = qual_idx;
                    f_qual_arr = stage_fc->f_qual_arr[qual_idx];

                    for (i = 0; i < f_qual_arr->conf_sz; ++i) {
                        if (f_qual_arr->conf_arr[i].selector.pri_sel_val 
                                                             == slice_idx) {
                            fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].offset_arr[j] = 
                                                f_qual_arr->conf_arr[i].offset;
                            break;
                        }
                    }

                    ++j;
                }
                fg->gid               =  gid;
                fg->priority          =  priority;
                fg->action_res_id     =  action_res_id;
                fg->stage_id          =  stage_fc->stage_id;
                fg->sel_codes[0].extn =  slice_idx;
                fg->slices            =  fs;
                fg->flags            |= _FP_GROUP_SPAN_SINGLE_SLICE;
                if (fc->l2warm
                        && ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15)) {
                    if (group_flags & _FP_GROUP_LOOKUP_ENABLED) {
                        fg->flags = fg->flags | _FP_GROUP_LOOKUP_ENABLED;
                    }
                    if (group_flags & _FP_GROUP_WLAN) {
                        fg->flags = fg->flags | _FP_GROUP_WLAN;
                    }
                } else {
                    fg->flags |= _FP_GROUP_LOOKUP_ENABLED;
                }

                /* Recover ports for group */
                BCM_PBMP_CLEAR(fg->pbmp);
                PBMP_PORT_ITER(unit, port) {
                    BCM_PBMP_PORT_ADD(fg->pbmp, port);
                }
                BCM_PBMP_PORT_ADD(fg->pbmp, 0); 
                fs->pbmp = fg->pbmp;

                /* Hook new group into stage */
                fg->next = fc->groups;
                fc->groups = fg;

            }  /* entry_cnt == 0 */

            /* Add entry to group */
            _FP_XGS3_ALLOC(f_ent, sizeof(*f_ent), "field entry");
            if (f_ent == NULL) {
                LOG_ERROR(BSL_LS_BCM_FP, \
                          (BSL_META_U(unit, \
                                      "Error while allocating memory for  \
                                      field entry\n")));
                rv = BCM_E_MEMORY;
                goto cleanup;
            }

            for (idx1 = 0; idx1 < _FP_POLICER_LEVEL_COUNT; idx1++) {
                 f_ent->policer[idx1].pid = _FP_INVALID_INDEX;
            }

            sal_memset(&f_ent_wb_info, 0, sizeof(_field_entry_wb_info_t));
            f_ent_wb_info.sid = -1;
            f_ent_wb_info.pid = -1;
            f_ent_wb_info.pid1 = -1;

            if (fc->l2warm) {
                /* Retrieve entry info */
                rv = _field_trx_entry_info_retrieve(unit,
                                                    fc,
                                                    stage_fc,
                                                    multigroup,
                                                    &prev_prio,
                                                    &f_ent_wb_info
                                                    );
                if (BCM_FAILURE(rv)) {
                    fc->l2warm = 0;
                    goto cleanup;
                }
            } else {
                _bcm_field_last_alloc_eid_incr(unit);
            }

            if (fc->l2warm) {
                /* Use retrieved EID */
                f_ent->eid = f_ent_wb_info.eid;
                /* Set retrieved dvp_type and svp_type */
                f_ent->dvp_type = f_ent_wb_info.dvp_type[0];
                f_ent->svp_type = f_ent_wb_info.svp_type[0];
            } else {
                f_ent->eid = _bcm_field_last_alloc_eid_get(unit);
            }
            f_ent->group = fg;

            f_ent->flags     = _FP_ENTRY_PRIMARY;
            f_ent->slice_idx = entry_idx;
            f_ent->fs        = fs;

            if (f_ent_wb_info.color_independent) {
                f_ent->flags |= _FP_ENTRY_COLOR_INDEPENDENT;
            }

            f_ent->tcam.key_size = ((soc_mem_field_length(unit, 
                               tcam_data_mem, DATAf) + 31) & ~31) >> (5 - 2);

            f_ent->tcam.key_hw=sal_alloc(f_ent->tcam.key_size,"TCAM data buf");
            f_ent->tcam.mask_hw=sal_alloc(f_ent->tcam.key_size,"TCAM mask buf");

            rv = soc_mem_read(unit, tcam_data_mem, MEM_BLOCK_ANY, 
                              entry_idx, tcam_buf.entry_data);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "Error while reading tcam data entry\n")));
                goto cleanup;
            }
            soc_mem_field_get(unit, tcam_data_mem, tcam_buf.entry_data, 
                              DATAf, f_ent->tcam.key_hw);

            if (tcam_mask_mem != INVALIDm) {
                rv = soc_mem_read(unit, tcam_mask_mem, MEM_BLOCK_ANY, 0, 
                                  tcam_mask.entry_data);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_FP, \
                              (BSL_META_U(unit, \
                                          "Error while reading    \
                                          tcam mask entry\n")));
                    goto cleanup;
                }
                soc_mem_field_get(unit, tcam_mask_mem, tcam_mask.entry_data,
                                       MASKf, f_ent->tcam.mask_hw);
            } else {
                soc_mem_field_get(unit, tcam_data_mem, tcam_buf.entry_data, 
                                  MASKf, f_ent->tcam.mask_hw);
            }
 
            for (i=0; i < (f_ent->tcam.key_size/4); i++) {
                f_ent->tcam.mask_hw[i] = ~(f_ent->tcam.mask_hw[i]);
            }

            /* Assign entry to a slice */
            fs->entries[entry_idx] = f_ent;
            f_ent->flags |= _FP_ENTRY_INSTALLED;
            --fs->free_count; 

            /* Assign entry to group */
            f_ent->group = fg;

            /* Recover non-profiled actions */
            rv = _field_tr3_non_profiled_actions_recover(unit, policy_mem, 
                                                         entry_idx, f_ent_wb_info.sid, f_ent);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            if (soc_feature(unit, soc_feature_advanced_flex_counter) &&
                ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_14)) {
                _field_adv_flex_stat_info_retrieve(unit, f_ent->statistic.sid);
                rv = _bcm_field_stat_get(unit, f_ent->statistic.sid, &f_st);
                if (BCM_SUCCESS(rv)) {
                    f_st->hw_index = f_ent->slice_idx;
                    f_st->pool_index = f_ent->fs->slice_number;
                }
            }

            /* Recover profiled actions */
            rv = _field_tr3_profiled_actions_recover(unit, policy_mem, 
                                                     entry_idx, stage_fc, f_ent);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            /* Add to the group */
            if (fc->l2warm) {
                f_ent->prio = f_ent_wb_info.prio;
            } else {
                f_ent->prio = cum_slice_entry_cnt[slice_idx] - entry_idx;
            }

            rv = _field_group_entry_add(unit, fg, f_ent);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            f_ent = 0;

            ++entry_cnt;

        } /* For loop -- entry */

        if (wb_mode) {                
            SOC_WARM_BOOT_START(unit);
            wb_mode = 0;
        }

        /* Free up the temporary slice group info */
        if (fc->l2warm) {
            _field_scache_slice_group_free(unit,
                                           fc,
                                           slice_idx
                                           );
        }
        BCM_IF_ERROR_RETURN
            (_bcm_field_prio_mgmt_slice_reinit(unit, stage_fc, fs));

        fg = 0;
    }  /* For loop -- slice */

    if (fc->l2warm) {
        /* Mark end of Slice Info */
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
            fc->scache_pos++;
        }

        /* Group Slice Selectors */
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
            rv = (_field_group_slice_selectors_recover(unit,
                        &buf[fc->scache_pos], stage_fc->stage_id));
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }

        /* Recovering end sequence */
        result = 0;
        for (i = 0; i < 4; i++) {
            val = buf[fc->scache_pos];
            ++fc->scache_pos;
            result |= val << (i * 8);
        }
        if (result != _FIELD_EXTFP_DATA_END) {
            /* End sequence not found */
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "0:End sequence NOT found in scache\n")));
            rv = BCM_E_INTERNAL;
            fc->l2warm = 0;
            goto cleanup;
        }
        if(NULL != buf1) {
            result = 0;
            for (i = 0; i < 4; i++) {
                val = buf1[fc->scache_pos1];
                ++fc->scache_pos1;
                result |= val << (i * 8);
            }
            if (result != _FIELD_EXTFP_DATA_END) {
                /* End sequence not found */
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "1:End sequence NOT found in scache\n")));
                rv = BCM_E_INTERNAL;
                fc->l2warm = 0;
                goto cleanup;
            }

        }
    }

    if (BCM_SUCCESS(rv)) {
        _field_group_slice_vmap_recovery(unit, fc, stage_fc);
    }

 cleanup:
    if (wb_mode) {                
        SOC_WARM_BOOT_START(unit);
        wb_mode = 0;
    }
    if (f_ent) {
        sal_free(f_ent);
    }
    if (fg) {
        sal_free(fg);
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_field_tr3_stage_external_reinit
 * Purpose:
 *      Reconstruct ESM ACL/External stage groups and entries.
 * Parameters:
 *     unit             - (IN) BCM device number.
 *     fc               - (IN) Device field control structure pointer.
 *     stage_fc         - (IN) FP stage control info.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_tr3_stage_external_reinit(int unit,
                                 _field_control_t *fc,
                                 _field_stage_t *stage_fc)
{
    if ((!fc) || (!stage_fc)) {
        return(BCM_E_INTERNAL);     
    }
    if (!_field_tr3_ext_scache_sync_chk(unit, fc, stage_fc)) {
        return (_field_tr3_stage_external_reinit(unit, fc, stage_fc));
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_tr3_actions_recover_replaceL2Fields
 * Purpose:
 *    recover the fields actions depending on EGR_IFP_MOD_FIELDS table
 * Parameters:
 *     unit             - BCM device number
 *     index            - the hardware index assigned to the action
 *    f_ent             - Field entry structure
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     For triumph3 and helix4 devices only.
 */

int _field_tr3_actions_recover_replaceL2Fields(int unit,
                                      int                index,
                                      _field_entry_t     *f_ent
                                      )
{
    int isSet = 0;
    uint32 fieldVal;
    uint32 cfiVal;
    uint32 tbl_entry[SOC_MAX_MEM_FIELD_WORDS];
    int rv = BCM_E_NONE;
    _field_control_t *fc;               /* Unit field control information.  */
    sal_mac_addr_t mac_addr;             /* MAC address info */
    uint32 mac_addr_words[2];            /* MAC address info for Param[0],
                                          * Param[1] */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    if (index > fc->_fp_egr_ifp_mod_fields_size) {
        return (BCM_E_PARAM);
    }

    /*set the index in the bitmap*/
    SHR_BITSET(fc->field_action_l2mod_index_bmp.w, index);
    rv = READ_EGR_IFP_MOD_FIELDSm(unit, MEM_BLOCK_ANY, index, &tbl_entry);
    if(BCM_FAILURE(rv)) {
        return BCM_E_UNAVAIL;
    }
    /*if inner vlan is set */
    isSet = soc_mem_field32_get(unit, EGR_IFP_MOD_FIELDSm,
                           &tbl_entry, CHANGE_INNER_VLANf);
    if(isSet) {
        fieldVal = soc_mem_field32_get(unit, EGR_IFP_MOD_FIELDSm,
                           &tbl_entry, INNER_VLANf);

        rv = _field_trx_actions_recover_action_add(unit,
                                             f_ent,
                                             bcmFieldActionReplaceInnerVlan,
                                             fieldVal,
                                             0, 0, 0, 0, 0,
                                             index
                                             );
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
    /* if outer vlan is set*/
    isSet = soc_mem_field32_get(unit, EGR_IFP_MOD_FIELDSm,
                           &tbl_entry, CHANGE_OUTER_VLANf);
    if(isSet) {
        fieldVal = soc_mem_field32_get(unit, EGR_IFP_MOD_FIELDSm,
                           &tbl_entry, OUTER_VLANf);

        rv = _field_trx_actions_recover_action_add(unit,
                                             f_ent,
                                             bcmFieldActionReplaceOuterVlan,
                                             fieldVal,
                                             0, 0, 0, 0, 0,
                                             index
                                             );
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
    /* if srcMac is set */
    isSet = soc_mem_field32_get(unit, EGR_IFP_MOD_FIELDSm,
                           &tbl_entry, CHANGE_MAC_SAf);
    if(isSet) {
        soc_mem_mac_addr_get(unit, EGR_IFP_MOD_FIELDSm,
            &tbl_entry, MAC_SAf, mac_addr);
        SAL_MAC_ADDR_TO_UINT32(mac_addr, mac_addr_words);

        /* Re-Create bcmFieldActionReplaceSrcMac action for the
         * FP entry */
        rv = _field_trx_actions_recover_action_add(unit,
                                             f_ent,
                                             bcmFieldActionReplaceSrcMac,
                                             mac_addr_words[0],
                                             mac_addr_words[1], 0, 0, 0, 0,
                                             index
                                             );
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* if dstMac is set */
    isSet = soc_mem_field32_get(unit, EGR_IFP_MOD_FIELDSm,
                           &tbl_entry, CHANGE_MAC_DAf);
    if(isSet) {

        soc_mem_mac_addr_get(unit, EGR_IFP_MOD_FIELDSm,
            &tbl_entry, MAC_DAf, mac_addr);
        SAL_MAC_ADDR_TO_UINT32(mac_addr, mac_addr_words);

        /* Re-Create bcmFieldActionReplaceDstMac action for the
         * FP entry */
         rv = _field_trx_actions_recover_action_add(unit,
                                             f_ent,
                                             bcmFieldActionReplaceDstMac,
                                             mac_addr_words[0],
                                             mac_addr_words[1], 0, 0, 0, 0,
                                             index
                                             );
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* if innerpriority is set */
    isSet = soc_mem_field32_get(unit, EGR_IFP_MOD_FIELDSm,
                           &tbl_entry, IPRI_ICFI_SELf);
    if(isSet == 1) {
        fieldVal = soc_mem_field32_get(unit, EGR_IFP_MOD_FIELDSm,
                           &tbl_entry, NEW_IPRI_ICFIf);
        cfiVal = fieldVal & 1;
        fieldVal = fieldVal >> 1;
        if(cfiVal == 0) {
            rv = _field_trx_actions_recover_action_add(unit,
                                    f_ent,
                                    bcmFieldActionReplaceInnerVlanPriority,
                                    BCM_FIELD_REPLACE_VLAN_PRIORITY,
                                    fieldVal,
                                    0, 0, 0, 0,
                                    index);
        } else {
            rv = _field_trx_actions_recover_action_add(unit,
                        f_ent,
                        bcmFieldActionReplaceInnerVlanPriority,
                        BCM_FIELD_REPLACE_VLAN_PRIORITY_USE_INTERNAL_CFI,
                        fieldVal,
                        0, 0, 0, 0,
                        index);
        }

    }  else if(isSet == 2) {
        rv = _field_trx_actions_recover_action_add(unit,
                        f_ent,
                        bcmFieldActionReplaceInnerVlanPriority,
                        BCM_FIELD_REPLACE_VLAN_PRIORITY_FROM_INT_PRIO_AND_CNG,
                        0, 0, 0, 0, 0,
                        index);
    }
    if (isSet && BCM_FAILURE(rv)) {
        return rv;
    }

    /* if outerpriority set */
    isSet = soc_mem_field32_get(unit, EGR_IFP_MOD_FIELDSm,
                           &tbl_entry, OPRI_OCFI_SELf);
    if(isSet == 1) {
        fieldVal = soc_mem_field32_get(unit, EGR_IFP_MOD_FIELDSm,
                           &tbl_entry, NEW_OPRI_OCFIf);
        cfiVal = fieldVal & 1;
        fieldVal = fieldVal >> 1;
        if(cfiVal == 0) {
            rv = _field_trx_actions_recover_action_add(unit,
                        f_ent,
                        bcmFieldActionReplaceOuterVlanPriority,
                        BCM_FIELD_REPLACE_VLAN_PRIORITY,
                        fieldVal,
                        0, 0, 0, 0,
                        index);
        } else {
            rv = _field_trx_actions_recover_action_add(unit,
                        f_ent,
                        bcmFieldActionReplaceOuterVlanPriority,
                        BCM_FIELD_REPLACE_VLAN_PRIORITY_USE_INTERNAL_CFI,
                        fieldVal,
                        0, 0, 0, 0,
                        index);

        }
    }  else if(isSet == 2) {
        rv = _field_trx_actions_recover_action_add(unit,
                        f_ent,
                        bcmFieldActionReplaceOuterVlanPriority,
                        BCM_FIELD_REPLACE_VLAN_PRIORITY_FROM_INT_PRIO_AND_CNG,
                        0, 0, 0, 0, 0,
                        index);
    }
    if (isSet && BCM_FAILURE(rv)) {
        return rv;
    }
    return BCM_E_NONE;
}

#endif
#else /* BCM_TRIUMPH3_SUPPORT && BCM_FIELD_SUPPORT */
typedef int _tr3_field_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_TRIUMPH3_SUPPORT && BCM_FIELD_SUPPORT */
