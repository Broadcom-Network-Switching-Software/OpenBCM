/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field.c
 * Purpose:     BCM56450 Field Processor installation functions.
 */
   
#include <soc/defs.h>
#if defined(BCM_KATANA2_SUPPORT) && defined(BCM_FIELD_SUPPORT)

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
#include <bcm_int/esw/katana.h>
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/flex_ctr.h>

#include <soc/scache.h>
#if defined(BCM_WARM_BOOT_SUPPORT)
#include <bcm_int/esw/policer.h>

STATIC soc_field_t _kt2_vfp_slice_wide_mode_field[4] = {
    SLICE_0_DOUBLE_WIDE_MODEf,
    SLICE_1_DOUBLE_WIDE_MODEf,
    SLICE_2_DOUBLE_WIDE_MODEf,
    SLICE_3_DOUBLE_WIDE_MODEf};

const soc_field_t _kt2_vfp_src_type_sel[]= {
    SLICE_0_SOURCE_TYPE_SELf, SLICE_1_SOURCE_TYPE_SELf,
    SLICE_2_SOURCE_TYPE_SELf, SLICE_3_SOURCE_TYPE_SELf};
#endif /* BCM_WARM_BOOT_SUPPORT */

#define ALIGN32(x)      (((x) + 31) & ~31)

/* local/static function prototypes */
STATIC void _field_kt2_functions_init(_field_funct_t *functions) ;

/*
 * Function:
 *     _field_kt2_ingress_qualifiers_init
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
_field_kt2_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    const unsigned
        f1_ofs                  = 194 - 2,
        f2_ofs                  = 66 - 2,
        f3_ofs                  = 26 - 2,
        dw_f1_ofs               = 171 - 3 + 1,
        dw_f2_ofs               = 43 - 3 + 1,
        dw_f3_ofs               = 11 - 3 + 1,
        dw_f4_ofs               = 3 - 3 + 1,
        fp_global_mask_tcam_ofs =
                    ALIGN32(soc_mem_field_length(unit, FP_TCAMm, DATA_KEYf));

        unsigned int fixed_ofs = 0;

    _FP_QUAL_DECL;
#if defined(BCM_METROLITE_SUPPORT)
    if (SOC_IS_METROLITE(unit)) {
        fixed_ofs              = 33 - 1;
    } else
#endif
#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        fixed_ofs               = 97 - 1;
    } else
#endif 
    {
        fixed_ofs               = 172 - 2;
    }

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

    /* IFP single wide Fixed */
    /* N.B.  Fixed field is in FP_GLOBAL_MASK_TCAM */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 1, 3);
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifySvpValid,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 4, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 5, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 9, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 10, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTunnelType,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 1,
                               fp_global_mask_tcam_ofs + fixed_ofs + 11, 4);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyLoopbackType,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 0,
                               fp_global_mask_tcam_ofs + fixed_ofs + 11, 4);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyLoopback,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 0,
                               fp_global_mask_tcam_ofs + fixed_ofs + 14, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 2,
                               fp_global_mask_tcam_ofs + fixed_ofs + 11, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 15, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRecoverableDrop,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 17, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
                 _bcmFieldSliceSelDisable, 0,
                 fp_global_mask_tcam_ofs + fixed_ofs + 18, 1);
#if defined (BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDownMepSatTerminated,
                     _bcmFieldSliceSelDisable, 0,
                     fp_global_mask_tcam_ofs + fixed_ofs + 19, 1);
    }
#endif /* BCM_SABER2_SUPPORT */
    /* FPF3 primary slice single wide.
     * IFP single wide F3_0  (f3_ofs = 26 - 2)*/
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 0, f3_ofs, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 1, f3_ofs, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 2, f3_ofs, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 2, f3_ofs + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 0, f3_ofs + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 1, f3_ofs + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 3, f3_ofs+ 6 , 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 3, f3_ofs , 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVrf, f3_ofs + 12, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVpn, f3_ofs + 12, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, 
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVlan, f3_ofs + 12, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport, f3_ofs + 24, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport, f3_ofs + 24, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport, f3_ofs + 24, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f3_ofs + 24, 13);
    /* IFP single wide F3_1  (f3_ofs = 26 - 2) */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 0, f3_ofs, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 1, f3_ofs, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 2, f3_ofs, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 3, f3_ofs, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 0, f3_ofs + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 1, f3_ofs + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 2, f3_ofs + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 3, f3_ofs + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVrf, f3_ofs + 12, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVpn, f3_ofs + 12, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVlan, f3_ofs + 12, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 1, f3_ofs + 24, 12);

    /* IFP single wide F3_2  (f3_ofs = 26 - 2) */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMultipath, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityGlp, f3_ofs + 19, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityGlp, f3_ofs + 19, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 2,
                                _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityMplsGport, f3_ofs + 19, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                                _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityVlanGport, f3_ofs + 19, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityMimGport, f3_ofs + 19, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport, f3_ofs + 19, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcTrunkMemberGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport, f3_ofs + 19, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport, f3_ofs + 26, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport, f3_ofs + 26, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityCommonGport, f3_ofs + 19, 17);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
                 _bcmFieldSliceSelFpf3, 2, f3_ofs + 35, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 2, f3_ofs + 36, 3);

    /* IFP single wide F3_3  (f3_ofs = 26 - 2) */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs + 16, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs + 18, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs + 20, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs + 22, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs + 25, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs + 31, 3);

    /* IFP single wide F3_4  (f3_ofs = 26 - 2) */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 4, f3_ofs, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 4, f3_ofs, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 4, f3_ofs + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 4, f3_ofs + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 4, f3_ofs + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 4, f3_ofs + 16, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 4, f3_ofs + 28, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 4, f3_ofs + 29, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, 4, f3_ofs + 32, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, 4, f3_ofs + 34, 4);

    /* IFP single wide F3_5  (f3_ofs = 26 - 2) */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 5, f3_ofs, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 5, f3_ofs, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 5, f3_ofs + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 5, f3_ofs + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 5, f3_ofs + 16, 16);

    /* IFP single wide F3_6  (f3_ofs = 26 - 2) */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 1, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 3, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 8, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 9, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 10, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 10, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 11, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 13, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 14, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 15, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillEgressRbridgeHit,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 17, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillIngressRbridgeHit,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 17, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 17, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 17, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 18, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 18, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 18 + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 6, f3_ofs + 18 + 13, 3);

    /* IFP single wide F3_7  (f3_ofs = 26 - 2) */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf3, 7, f3_ofs, 24);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               _bcmFieldQualifyRangeCheckBits24_31,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 3,
                               f3_ofs + 24, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 0,
                               f3_ofs + 24, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 1,
                               f3_ofs + 24, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 1,
                               f3_ofs + 24, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 2,
                               f3_ofs + 24, 8);

    /* IFP single wide F3_8  (f3_ofs = 26 - 2) */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf3, 8, f3_ofs, 20);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 8, f3_ofs + 20, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 8, f3_ofs + 22, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 8, f3_ofs + 24, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 8, f3_ofs + 26, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 8, f3_ofs + 28, 2);

    /* IFP single wide F3_9  (f3_ofs = 26 - 2) */
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData3,
                 _bcmFieldSliceSelFpf3, 9, f3_ofs, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsOuterLabelPop,
                 _bcmFieldSliceSelFpf3, 9, f3_ofs + 32, 1);
    _FP_QUAL_ADD(unit, stage_fc,
                 bcmFieldQualifyMplsStationHitTunnelUnterminated,
                 _bcmFieldSliceSelFpf3, 9, f3_ofs + 33, 1);
    _FP_QUAL_ADD(unit, stage_fc,
                 bcmFieldQualifyGenericAssociatedChannelLabelValid,
                 _bcmFieldSliceSelFpf3, 9, f3_ofs + 36, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRouterAlertLabelValid,
                 _bcmFieldSliceSelFpf3, 9, f3_ofs + 37, 1);
    
    /* IFP single wide F3_10  (f3_ofs = 26 - 2) */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup, f3_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMultipath, f3_ofs, 19);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf3, 10, f3_ofs + 19, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf3, 10, f3_ofs + 19, 16);

    /* IFP single wide F3_11  (f3_ofs = 26 - 2) */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf3, 11, f3_ofs, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf3, 11, f3_ofs, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                 _bcmFieldSliceSelFpf3, 11, f3_ofs, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf3, 11, f3_ofs + 7, 9);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                 _bcmFieldSliceSelFpf3, 11, f3_ofs + 16, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                 _bcmFieldSliceSelFpf3, 11, f3_ofs + 16, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                 _bcmFieldSliceSelFpf3, 11, f3_ofs + 16, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
                 _bcmFieldSliceSelFpf3, 11, f3_ofs + 29, 1);

    /* IFP single wide F3_12  (f3_ofs = 26 - 2) */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagVn, f3_ofs, 33);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySubportPktTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagVxlanOrLLTag, f3_ofs, 33);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCnTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagCn, f3_ofs, 33);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyFabricQueueTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagFabricQueue, f3_ofs, 33);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyMplsForwardingLabel,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagMplsFwdingLabel, f3_ofs, 33);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWord,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagMplsCntlWord, f3_ofs, 33);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelTtl,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select,
                     _bcmFieldAuxTagMplsFwdingLabel, 0,
                     f3_ofs, 8, f3_ofs + 32, 1, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelBos,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select,
                     _bcmFieldAuxTagMplsFwdingLabel, 0,
                     f3_ofs + 8, 1, f3_ofs + 32, 1, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelExp,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select,
                     _bcmFieldAuxTagMplsFwdingLabel, 0,
                     f3_ofs + 9, 3, f3_ofs + 32, 1, 0, 0, 0);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyMplsForwardingLabelId,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagMplsFwdingLabel, f3_ofs + 12, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashUpper,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagRtag7A, f3_ofs + 16, 17);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select,
                     _bcmFieldAuxTagRtag7A, 0,
                     f3_ofs, 16, f3_ofs + 32, 1, 0, 0, 0);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashUpper,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagRtag7B, f3_ofs + 16, 17);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select,
                     _bcmFieldAuxTagRtag7B, 0,
                     f3_ofs, 16, f3_ofs + 32, 1, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyLlidValue,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select,
                     _bcmFieldAuxTagVxlanOrLLTag, 0,
                     f3_ofs, 16, f3_ofs + 32, 1, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWordValid,
                 _bcmFieldSliceSelFpf3, 12, f3_ofs + 33, 1);

    /* IFP single wide F3_13  (f3_ofs = 26 - 2) */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf3, 13,
                               _bcmFieldSliceTosClassSelect, 0,
                               f3_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                               _bcmFieldSliceSelFpf3, 13,
                               _bcmFieldSliceTosClassSelect, 0,
                               f3_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                               _bcmFieldSliceSelFpf3, 13,
                               _bcmFieldSliceTosClassSelect, 1,
                               f3_ofs, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeader2Type,
                 _bcmFieldSliceSelFpf3, 13, f3_ofs + 8, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, 13, f3_ofs + 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, 13, f3_ofs + 24, 8);

    /* IFP single wide F2_0  (f2_ofs = 66 - 2)*/
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceTtlClassSelect, 0,
                                f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceTtlClassSelect, 0,
                                f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceTtlClassSelect, 1,
                                f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceTcpClassSelect, 0,
                                f2_ofs+ 8, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceTcpClassSelect, 0,
                                f2_ofs+ 8, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceTcpClassSelect, 1,
                                f2_ofs+ 8, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
                 _bcmFieldSliceSelFpf2, 0, f2_ofs + 14, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceTosClassSelect, 0,
                                f2_ofs+ 16, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceTosClassSelect, 0,
                                f2_ofs+ 16, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceTosClassSelect, 1,
                                f2_ofs+ 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 0, f2_ofs + 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 0, f2_ofs + 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf2, 0, f2_ofs + 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 0, f2_ofs + 56, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 0, f2_ofs + 64, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 0, f2_ofs + 96, 32);

    /* IFP single wide F2_1 (f2_ofs = 66 - 2)*/
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTtlClassSelect, 0,
                               f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTtlClassSelect, 0,
                               f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTtlClassSelect, 1,
                               f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTcpClassSelect, 0,
                               f2_ofs + 8, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTcpClassSelect, 0,
                               f2_ofs + 8, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTcpClassSelect, 1,
                               f2_ofs + 8, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf2, 1, f2_ofs + 14, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTosClassSelect, 0,
                               f2_ofs + 16, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTosClassSelect, 0,
                               f2_ofs + 16, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceTosClassSelect, 1,
                               f2_ofs + 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 1, f2_ofs + 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 1, f2_ofs + 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf2, 1, f2_ofs + 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 1, f2_ofs + 56, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 1, f2_ofs + 64, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 1, f2_ofs + 96, 32);

    /* IFP single wide F2_2 (f2_ofs = 66 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelFpf2, 2, f2_ofs, 128);

    /* IFP single wide F2_3 (f2_ofs = 66 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf2, 3, f2_ofs, 128);

    /* IFP single wide F2_4 (f2_ofs = 66 - 2)*/
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceTtlClassSelect, 0,
                               f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceTtlClassSelect, 0,
                               f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceTtlClassSelect, 1,
                               f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceTcpClassSelect, 0,
                               f2_ofs + 8, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceTcpClassSelect, 0,
                               f2_ofs + 8, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                              _bcmFieldSliceSelFpf2, 4,
                              _bcmFieldSliceTcpClassSelect, 1,
                              f2_ofs + 8, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
            _bcmFieldSliceSelFpf2, 4, f2_ofs + 14, 20);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                              _bcmFieldSliceSelFpf2, 4,
                              _bcmFieldSliceTosClassSelect, 0,
                              f2_ofs + 34, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceTosClassSelect, 0,
                               f2_ofs + 34, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceTosClassSelect, 1,
                               f2_ofs + 34, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 4, f2_ofs + 42, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 4, f2_ofs + 64, 64);

    /* IFP single wide F2_5 (f2_ofs = 66 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 5, f2_ofs, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 5, f2_ofs, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 5, f2_ofs + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 5, f2_ofs + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 5, f2_ofs + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 5, f2_ofs + 32, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5, f2_ofs + 80, 48);

    /* IFP single wide F2_6 (f2_ofs = 66 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 6, f2_ofs, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 6, f2_ofs, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 6, f2_ofs + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 6, f2_ofs + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 6, f2_ofs + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 6, f2_ofs + 32, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 6, f2_ofs + 80, 32);

    /* IFP single wide F2_7 (f2_ofs = 66 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 7, f2_ofs, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 7, f2_ofs, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 7, f2_ofs + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 7, f2_ofs + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 7, f2_ofs + 16, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                               _bcmFieldSliceSelFpf2, 7,
                               _bcmFieldSliceTtlClassSelect, 0,
                               f2_ofs + 32, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                               _bcmFieldSliceSelFpf2, 7,
                               _bcmFieldSliceTtlClassSelect, 0,
                               f2_ofs + 32, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                               _bcmFieldSliceSelFpf2, 7,
                               _bcmFieldSliceTtlClassSelect, 1,
                               f2_ofs + 32, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 7, f2_ofs + 40, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 7, f2_ofs + 48, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 7, f2_ofs + 80, 48);

    /* IFP single wide F2_8 (f2_ofs = 66 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 8, f2_ofs, 128);

    /* IFP single wide F2_9 (f2_ofs = 66 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                     _bcmFieldSliceSelFpf2, 9, f2_ofs, 128);

    /* IFP single wide F2_10 (f2_ofs = 66 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                     _bcmFieldSliceSelFpf2, 10, f2_ofs, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                     _bcmFieldSliceSelFpf2, 10, f2_ofs + 64, 64);

    /* IFP single wide F2_11 (f2_ofs = 66 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 11, f2_ofs + 32, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 11, f2_ofs + 80, 48);
    /* DIP_127_96 omitted -- no qualifier?  Omitted in Trident, too.*/

    /* IFP single wide F1_0 (f1_ofs = 194 - 2)*/
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 0, f1_ofs, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 1, f1_ofs, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 2, f1_ofs, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 3, f1_ofs, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 0, f1_ofs + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 1, f1_ofs + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 2, f1_ofs + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 3, f1_ofs + 6, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVrf, f1_ofs + 12, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVpn, f1_ofs + 12, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVlan, f1_ofs + 12, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport, f1_ofs + 24, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport, f1_ofs + 24, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport, f1_ofs + 24, 13);

    /* IFP single wide F1_1  (f1_ofs = 194 - 2)*/
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMultipath, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityGlp, f1_ofs + 19, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityGlp, f1_ofs + 19, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityMplsGport, f1_ofs + 19, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityVlanGport, f1_ofs + 19, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityMimGport, f1_ofs + 19, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport, f1_ofs + 19, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcTrunkMemberGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport, f1_ofs + 19, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport, f1_ofs + 26, 10);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityModPortGport, f1_ofs + 26, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceSrcEntitySelect,
                               _bcmFieldFwdEntityCommonGport, f1_ofs + 19, 17);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
                 _bcmFieldSliceSelFpf1, 1, f1_ofs + 35, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 1, f1_ofs + 36, 3);

    /* IFP single wide F1_2  (f1_ofs = 194 - 2)*/
    
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMultipath, f1_ofs, 19);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 2, f1_ofs + 19, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 0, f1_ofs + 22, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 1, f1_ofs + 22, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 2, f1_ofs + 22, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 3, f1_ofs + 22, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 0, f1_ofs + 28, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 1, f1_ofs + 28, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 2, f1_ofs + 28, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 3, f1_ofs + 28, 6);

    /* IFP single wide F1_3  (f1_ofs = 194 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 3, f1_ofs, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 3, f1_ofs, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 3, f1_ofs + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 3, f1_ofs + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 3, f1_ofs + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 3, f1_ofs + 16, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 3, f1_ofs + 28, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 3, f1_ofs + 29, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf1, 3, f1_ofs + 32, 2);

    /* IFP single wide F1_4  (f1_ofs = 194 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 4, f1_ofs, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 4, f1_ofs, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 4, f1_ofs + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 4, f1_ofs + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 4, f1_ofs + 16, 16);

    /* IFP single wide F1_5  (f1_ofs = 194 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 1, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 3, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 8, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 9, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 10, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 10, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 11, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 13, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 14, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 15, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillEgressRbridgeHit,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 17, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillIngressRbridgeHit,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 17, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 17, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 17, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 18, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 18, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 30, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 31, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf1, 5, f1_ofs + 34, 2);

    /* IFP single wide F1_6  (f1_ofs = 194 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 6, f1_ofs, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 6, f1_ofs, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 6, f1_ofs + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 6, f1_ofs + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 6, f1_ofs + 16, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 6, f1_ofs + 18, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf1, 6, f1_ofs + 20, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 6, f1_ofs + 22, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 6, f1_ofs + 25, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf1, 6, f1_ofs + 31, 3);

    /* IFP single wide F1_7  (f1_ofs = 194 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 7, f1_ofs, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 7, f1_ofs, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 7, f1_ofs + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 7, f1_ofs + 13, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 0, f1_ofs + 16, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 1, f1_ofs + 16, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 2, f1_ofs + 16, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 3, f1_ofs + 16, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVrf, f1_ofs + 22, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVpn, f1_ofs + 22, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, 
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVlan, f1_ofs + 22, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsOuterLabelPop,
                 _bcmFieldSliceSelFpf1, 7, f1_ofs + 34 , 1);
    _FP_QUAL_ADD(unit, stage_fc, 
                 bcmFieldQualifyMplsStationHitTunnelUnterminated,
                 _bcmFieldSliceSelFpf1, 7, f1_ofs + 35, 1);
    _FP_QUAL_ADD(unit, stage_fc,
                 bcmFieldQualifyGenericAssociatedChannelLabelValid,
                 _bcmFieldSliceSelFpf1, 7, f1_ofs + 36, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRouterAlertLabelValid,
                 _bcmFieldSliceSelFpf1, 7, f1_ofs + 37, 1);

    /* IFP single wide F1_8  (f1_ofs = 194 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf1, 8, f1_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceTosClassSelect, 0,
                               f1_ofs + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceTosClassSelect, 0,
                               f1_ofs + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceTosClassSelect, 1,
                               f1_ofs + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 0, f1_ofs + 16, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 1, f1_ofs + 16, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 2, f1_ofs + 16, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 3, f1_ofs + 16, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVrf, f1_ofs + 22, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVpn, f1_ofs + 22, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, 
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVlan, f1_ofs + 22, 12);

    /* IFP single wide F1_9  (f1_ofs = 194 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData2,
                 _bcmFieldSliceSelFpf1, 9, f1_ofs, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsOuterLabelPop,
                 _bcmFieldSliceSelFpf1, 9, f1_ofs + 32 , 1);
    _FP_QUAL_ADD(unit, stage_fc, 
                 bcmFieldQualifyMplsStationHitTunnelUnterminated,
                 _bcmFieldSliceSelFpf1, 9, f1_ofs + 33, 1);
    _FP_QUAL_ADD(unit, stage_fc,
                 bcmFieldQualifyGenericAssociatedChannelLabelValid,
                 _bcmFieldSliceSelFpf1, 9, f1_ofs + 36, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRouterAlertLabelValid,
                 _bcmFieldSliceSelFpf1, 9, f1_ofs + 37, 1);

    /* IFP single wide F1_10  (f1_ofs = 194 - 2)*/
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup, f1_ofs, 19);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMultipath, f1_ofs, 19);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf1, 10, f1_ofs + 19, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf1, 10, f1_ofs + 19, 16);

    /* IFP single wide F1_11  (f1_ofs = 194 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf1, 11, f1_ofs, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf1, 11, f1_ofs, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                 _bcmFieldSliceSelFpf1, 11, f1_ofs, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf1, 11, f1_ofs + 7, 9);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                 _bcmFieldSliceSelFpf1, 11, f1_ofs + 16, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                 _bcmFieldSliceSelFpf1, 11, f1_ofs + 16, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                 _bcmFieldSliceSelFpf1, 11, f1_ofs + 16, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
                 _bcmFieldSliceSelFpf1, 11, f1_ofs + 29, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf1, 11, f1_ofs + 30, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf1, 11, f1_ofs + 32, 4);

    /* IFP single wide F1_12  (f1_ofs = 194 - 2)*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 12, f1_ofs, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 0, f1_ofs + 12, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 1, f1_ofs + 12, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 2, f1_ofs + 12, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceDstClassSelect, 3, f1_ofs + 12, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 0, f1_ofs + 18, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 1, f1_ofs + 18, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 2, f1_ofs + 18, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceSrcClassSelect, 3, f1_ofs + 18, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport, f1_ofs + 24, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport, f1_ofs + 24, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 12,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport, f1_ofs + 24, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelAction,
                 _bcmFieldSliceSelFpf1, 12, f1_ofs + 37, 3);

    /* IFP single wide F1_13  (f1_ofs = 194 - 2)*/
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagVn,
                               f1_ofs, 33);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySubportPktTag,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagVxlanOrLLTag,
                               f1_ofs, 33);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCnTag,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagCn,
                               f1_ofs, 33);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyFabricQueueTag,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagFabricQueue,
                               f1_ofs, 33);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyMplsForwardingLabel,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagMplsFwdingLabel, f1_ofs, 33);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWord,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagMplsCntlWord, f1_ofs, 33);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelTtl,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select,
                     _bcmFieldAuxTagMplsFwdingLabel, 0,
                     f1_ofs, 8, f1_ofs + 32, 1, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelBos,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select,
                     _bcmFieldAuxTagMplsFwdingLabel, 0,
                     f1_ofs + 8, 1, f1_ofs + 32, 1, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelExp,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select,
                     _bcmFieldAuxTagMplsFwdingLabel, 0,
                     f1_ofs + 9, 3, f1_ofs + 32, 1, 0, 0, 0);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyMplsForwardingLabelId,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagMplsFwdingLabel, f1_ofs + 12, 21);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashUpper,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagRtag7A, f1_ofs + 16, 17);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select,
                     _bcmFieldAuxTagRtag7A, 0,
                     f1_ofs, 16, f1_ofs + 32, 1, 0, 0, 0);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashUpper,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagRtag7B, f1_ofs + 16, 17);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select,
                     _bcmFieldAuxTagRtag7B, 0,
                     f1_ofs, 16, f1_ofs + 32, 1, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyLlidValue,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 13,
                     _bcmFieldSliceAuxTag1Select,
                     _bcmFieldAuxTagVxlanOrLLTag, 0,
                     f1_ofs, 16, f1_ofs + 32, 1, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWordValid,
                 _bcmFieldSliceSelFpf1, 13, f1_ofs + 33, 1);
   _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelAction,
                 _bcmFieldSliceSelFpf1, 13, f1_ofs + 34, 3);

    /* IFP DWF4_1 (dw_f4_ofs = 3 - 3 + 1)*/
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf4, 1,
                            _bcmFieldSliceTcpClassSelect, 0, dw_f4_ofs, 6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
                            _bcmFieldSliceSelFpf4, 1,
                            _bcmFieldSliceTcpClassSelect, 0, dw_f4_ofs, 6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
                            _bcmFieldSliceSelFpf4, 1,
                            _bcmFieldSliceTcpClassSelect, 1, dw_f4_ofs, 6);

    /* IFP DWF3 (dw_f3_ofs = 11 - 3 + 1,)*/
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceTosClassSelect, 0, dw_f3_ofs, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceTosClassSelect, 0, dw_f3_ofs, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceTosClassSelect, 1, dw_f3_ofs, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f3_ofs + 8, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyExtensionHeaderSubCode,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f3_ofs + 16, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f3_ofs + 24, 8);

    /* IFP DWF2_0 (dw_f2_ofs = = 43 - 3 + 1)*/
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 + 1, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 + 3, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 + 4, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 + 6, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 + 7, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 + 8, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 + 9, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 +10, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 +10, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 +11, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 +12, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 +13, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 +14, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 +15, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 +16, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyTrillEgressRbridgeHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 +16, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 +16, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 +16, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 +17, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyTrillIngressRbridgeHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 +17, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 +17, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 14 +17, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs + 32, 24);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyRangeCheckBits24_31,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 3,
                            dw_f2_ofs + 56, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 0,
                            dw_f2_ofs + 56, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassVPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 1,
                            dw_f2_ofs + 56, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 1,
                            dw_f2_ofs + 56, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 2,
                            dw_f2_ofs + 56, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            dw_f2_ofs + 64, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0,
                            dw_f2_ofs + 96, 32);

    /* IFP DWF2_1 (dw_f2_ofs = = 43 - 3 + 1)*/
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs, 128);

    /* IFP DWF2_2 (dw_f2_ofs = = 43 - 3 + 1)*/
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                            _bcmFieldSliceSelFpf2, 2,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs, 128);

    /* IFP DWF2_3 (dw_f2_ofs = = 43 - 3 + 1)*/
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                            _bcmFieldSliceSelFpf2, 3,
                            _bcmFieldSliceSelDisable, 0, dw_f2_ofs, 128);
    /* IFP DWF1_0  (dw_f1_ofs = 171 - 3 + 1)*/
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceTtlClassSelect, 0, dw_f1_ofs, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceTtlClassSelect, 0, dw_f1_ofs, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceTtlClassSelect, 1, dw_f1_ofs, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f1_ofs + 8, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f1_ofs + 10, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f1_ofs + 26, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceSelDisable, 0, dw_f1_ofs + 26, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect,
                            _bcmFieldFwdEntityGlp, dw_f1_ofs + 42, 19);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect,
                            _bcmFieldFwdEntityCommonGport, dw_f1_ofs + 42, 19);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect,
                            _bcmFieldFwdEntityGlp, dw_f1_ofs + 42, 19);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect,
                            _bcmFieldFwdEntityMplsGport, dw_f1_ofs + 42, 19);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect,
                            _bcmFieldFwdEntityVlanGport, dw_f1_ofs + 42, 19);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect,
                            _bcmFieldFwdEntityMimGport, dw_f1_ofs + 42, 19);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect,
                            _bcmFieldFwdEntityL3Egress, dw_f1_ofs + 42, 19);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect,
                            _bcmFieldFwdEntityL3Egress, dw_f1_ofs + 42, 19);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect,
                            _bcmFieldFwdEntityMulticastGroup,
                            dw_f1_ofs + 42, 19);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceDstFwdEntitySelect,
                            _bcmFieldFwdEntityMultipath, dw_f1_ofs + 42, 19);

    /* IFP DWF1_1  (dw_f1_ofs = 171 - 3 + 1)*/
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyMplsForwardingLabelId,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0, dw_f1_ofs, 20);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyMplsForwardingLabelAction,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0, dw_f1_ofs + 20, 3);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWord,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0, dw_f1_ofs + 23, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWordValid,
                            _bcmFieldSliceSelFpf1, 1,
                            _bcmFieldSliceSelDisable, 0, dw_f1_ofs + 55, 1);


    /* IPBM OVERLAY qualifiers for Katana2 */
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     1,
                     0, 16,
                     0, 0,
                     0, 0,
                     1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     1,
                     0, 16,
                     0, 0,
                     0, 0,
                     1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                     1,
                     16, 16,
                     0, 0,
                     0, 0,
                     1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                     1,
                     16, 16,
                     0, 0,
                     0, 0,
                     1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                     1,
                     16, 16,
                     0, 0,
                     0, 0,
                     1);
     _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                     1,
                     23, 8,
                     0, 0,
                     0, 0,
                     1);
     _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                     1,
                     23, 8,
                     0, 0,
                     0, 0,
                     1);


    /* DONE DWF */
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_kt2_lookup_qualifiers_init
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
_field_kt2_lookup_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    const unsigned f1_ofs = 160,
                   f2_ofs = 32,
                   f3_ofs = 0;
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

    /* VFP_F3_0 */
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

    /* VFP_F3_1 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_ofs, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_ofs, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 0,f3_ofs + 16, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 1,f3_ofs + 16, 16);

    /* VFP_F3_2 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_ofs + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_ofs + 8, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, 2, f3_ofs + 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, 2, f3_ofs + 24, 8);

    /* VFP_F3_3 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs + 8, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs + 8, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs + 20, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 3, f3_ofs + 21, 3);
#if defined (BCM_METROLITE_SUPPORT)
    if (SOC_IS_METROLITE(unit)) {
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf3, 3,
                               _bcmFieldSliceSrcTypeSelect, 0, f3_ofs + 24, 6);
   } else 
#endif
   {
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf3, 3,
                               _bcmFieldSliceSrcTypeSelect, 0, f3_ofs + 24, 8);
   }

    /* VFP_F2_0 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerTtl,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0, f2_ofs + 8, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpFrag,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 14, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 14, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 16, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 16, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1,f2_ofs + 24, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0,f2_ofs + 24, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1,f2_ofs + 40, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0,f2_ofs + 40, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 56, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 56, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1,f2_ofs + 64, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0,f2_ofs + 64, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1,f2_ofs + 96, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0,f2_ofs + 96, 32);
    /* VFP_F2_1 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerSrcIp6,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs, 128);
    /* VFP_F2_2 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerDstIp6,
                               _bcmFieldSliceSelFpf2, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                               _bcmFieldSliceSelFpf2, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs, 128);
    /* VFP_F2_3 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 3, f2_ofs + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 3, f2_ofs + 32, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 3, f2_ofs + 80, 48);
    /* VFP_F2_4 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1,f2_ofs + 16, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0,f2_ofs + 16, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1,f2_ofs + 48, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0,f2_ofs + 48, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 4, 112, 48);

    /* VFP_F2_5 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1,f2_ofs + 16, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0,f2_ofs + 16, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1,f2_ofs + 48, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0,f2_ofs + 48, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5, f2_ofs + 80, 48);

    /* VFP_F2_6 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerSrcIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_ofs, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_ofs, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerDstIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 1,f2_ofs + 64, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 0,f2_ofs + 64, 64);

    /* VFP_F2_7 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                 _bcmFieldSliceSelFpf2, 7, f2_ofs, 33);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySubportPktTag,
                 _bcmFieldSliceSelFpf2, 7, f2_ofs, 34);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySnap,
                 _bcmFieldSliceSelFpf2, 7, f2_ofs + 64, 40);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLlc,
                 _bcmFieldSliceSelFpf2, 7, f2_ofs + 104, 24);

    /* VFP_F2_8 */
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 8, f2_ofs, 128);

    /* VFP_F2_9 */
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                 _bcmFieldSliceSelFpf2, 9, f2_ofs, 128);

    /* VFP_F1_0 */
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
                 _bcmFieldSliceSelFpf1, 0, f1_ofs + 25, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 0, f1_ofs + 26, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 0, f1_ofs + 29, 2);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf1, 0,
                 _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                 0,
                 f1_ofs + 31, 7, /* Port value in SGLP */
                 f1_ofs + 38, 8, /* Module value in SGLP */
                 f1_ofs + 46, 1, /* Trunk bit in SGLP (should be 0) */
                 0
                 );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf1, 0,
                 _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                 0,
                 f1_ofs + 31, 15, /* trunk id field of SGLP */
                 f1_ofs + 46, 1,  /* trunk bit of SGLP */
                 0, 0,
                 0
                 );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf1, 0,
                 _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMplsGport,
                 0,
                 f1_ofs + 31, 16, /* S_FIELD */
                 f1_ofs + 56, 1,  /* SVP_VALID */
                 0, 0,
                 0
                 );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf1, 0,
                 _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityVlanGport,
                 0,
                 f1_ofs + 31, 16, /* S_FIELD */
                 f1_ofs + 56, 1,  /* SVP_VALID */
                 0, 0,
                 0
                 );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf1, 0,
                 _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMimGport,
                 0,
                 f1_ofs + 31, 16, /* S_FIELD */
                 f1_ofs + 56, 1,  /* SVP_VALID */
                 0, 0,
                 0
                 );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf1, 0,
                 _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityWlanGport,
                 0,
                 f1_ofs + 31, 16, /* S_FIELD */
                 f1_ofs + 56, 1,  /* SVP_VALID */
                 0, 0,
                 0
                 );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf1, 0,
                 _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                 0,
                 f1_ofs + 31, 15, /* mod + port field of unresolved SGLP */
                 0, 0,
                 0, 0,
                 0
                 );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf1, 0,
                 _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                 0,
                 f1_ofs + 31 + 7, 8, /* mod field of unresolved SGLP */
                 0, 0,
                 0, 0,
                 0
                 );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf1, 0,
                 _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityModPortGport,
                 0,
                 f1_ofs + 31 + 7, 8, /* mod field of unresolved SGLP */
                 0, 0,
                 0, 0,
                 0
                 );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf1, 0,
                 _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityPortGroupNum,
                 0,
                 f1_ofs + 31, 8, /* ingress port field */
                 0, 0,
                 0, 0,
                 0
                 );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf1, 0,
                 _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityPortGroupNum,
                 0,
                 f1_ofs + 31 + 8, 8, /* ingress port group field */
                 0, 0,
                 0, 0,
                 0
                 );

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpType,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f1_ofs + 48, 4);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f1_ofs + 48, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports, 
                 _bcmFieldSliceSelFpf1, 0, f1_ofs + 52, 1);


    /* VFP_DOUBLE_WIDE_F3 */
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
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_ofs + 24, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f3_ofs + 24, 8);

    /* VFP_DOUBLE_WIDE_F2_0 */
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

    /* VFP_DOUBLE_WIDE_F2_1 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_ofs, 128);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_ofs, 128);

    /* VFP_DOUBLE_WIDE_F2_2 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                            _bcmFieldSliceSelFpf2, 2,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_ofs, 128);

    /* VFP_DOUBLE_WIDE_F1 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyExtensionHeaderSubCode,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_ofs, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_ofs + 8, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_ofs + 16, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_ofs + 24, 16);
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_kt2_egress_qualifiers_init
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
_field_kt2_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_DECL;
    _key_fld_ = KEYf;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageEgress,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

   /*EFP_KEY4 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 0, 1);
    /* EG_NEXT_HOP_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 1,
                               2, 12);
    /* EG_L3_INTF_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 2,
                               2, 12);

    /* DVP_VALID + DVP */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 14);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 14);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 14);

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
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 141, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 142, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 145, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 157, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 165, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 166, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 167, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 167, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 179, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 180, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 183, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 185, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 186, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0, 194, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0, 194, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0, 194, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0, 194, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0, 194, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0, 194, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0, 194, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0, 194, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 1, 194, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0, 194, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0, 194, 13);
    /* CPUCOS */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 2, 194, 6);
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
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 213, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 213, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 213, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 227, 2);

    /*EFP_KEY1 */
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
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 149, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 157, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 158, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 159, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 159, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 172, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 175, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 177, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 178, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 186, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 194, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 198, 1);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 1, 199, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 2, 199, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 212, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 212, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 212, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 226, 2);
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
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 149, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 157, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 158, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 159, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 159, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 172, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 175, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 177, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 178, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 186, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 194, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 198, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 1, 199, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 2, 199, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 0, 199, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 212, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 212, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 212, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 226, 2);

    /*EFP_KEY2 */
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
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 159, 8);
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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0, 196, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0, 196, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0, 196, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0, 196, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0, 196, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0, 196, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF2, 0, 196, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF2, 0, 196, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 1, 196, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 2, 196, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 3, 196, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0, 196, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0, 196, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 209, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 213, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 214, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 214, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 214, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 228, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 0);

    /*EFP_KEY3 */
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
                               _bcmFieldSliceSelEgrClassF3, 0, 204, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 1, 204, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 0);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_kt2_qualifiers_init
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
_field_kt2_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
          rv = _field_kt2_ingress_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _field_kt2_lookup_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _field_kt2_egress_qualifiers_init(unit, stage_fc);
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
 *     _bcm_field_kt2_init

 * Purpose:
 *     Perform initializations that are specific to BCM56230. This
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
_bcm_field_kt2_init(int unit, _field_control_t *fc)
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
        BCM_IF_ERROR_RETURN(_field_kt2_qualifiers_init(unit, stage_fc));

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
    _field_kt2_functions_init(&fc->functions);;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_kt2_lookup_selcodes_install
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
_field_kt2_lookup_selcodes_install(int            unit,
                                   _field_group_t *fg,
                                   uint8          slice_num,
                                   int            selcode_idx)
{
    static const soc_field_t s_type_fld_tbl[] = {
        SLICE_0_SOURCE_TYPE_SELf,
        SLICE_1_SOURCE_TYPE_SELf,
        SLICE_2_SOURCE_TYPE_SELf,
        SLICE_3_SOURCE_TYPE_SELf
    };

    _field_sel_t * const sel = &fg->sel_codes[selcode_idx];
    int           errcode = BCM_E_NONE;
    uint64        regval;
    uint64        val;

    BCM_IF_ERROR_RETURN(soc_reg64_get(unit, VFP_KEY_CONTROL_1r,
                                      REG_PORT_ANY, 0, &regval));

    if ((fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)
        && (selcode_idx & 1)) {
        if (sel->fpf2 != _FP_SELCODE_DONT_CARE) {
            COMPILER_64_SET(val, 0, sel->fpf2);
            soc_reg64_field_set(unit, VFP_KEY_CONTROL_1r, &regval,
                            _bcm_field_trx_vfp_double_wide_sel[slice_num], val);
        }
    } else {
        if (sel->fpf2 != _FP_SELCODE_DONT_CARE) {
            COMPILER_64_SET(val, 0, sel->fpf2);
            soc_reg64_field_set(unit, VFP_KEY_CONTROL_1r, &regval,
                            _bcm_field_trx_vfp_field_sel[slice_num][0], val);
        }
        if (sel->fpf3 != _FP_SELCODE_DONT_CARE) {
            COMPILER_64_SET(val, 0, sel->fpf3);
            soc_reg64_field_set(unit, VFP_KEY_CONTROL_1r, &regval,
                            _bcm_field_trx_vfp_field_sel[slice_num][1], val);
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
        soc_reg64_field_set(unit, VFP_KEY_CONTROL_1r, &regval,
                            s_type_fld_tbl[slice_num], val);
    }

    BCM_IF_ERROR_RETURN(
        soc_reg64_set(unit, VFP_KEY_CONTROL_1r, REG_PORT_ANY, 0, regval));

    /* Set inner/outer ip header selection. */
    if (sel->ip_header_sel != _FP_SELCODE_DONT_CARE) {
        errcode = soc_reg_field32_modify(
                      unit, VFP_KEY_CONTROL_2r, REG_PORT_ANY,
                      _bcm_field_trx_vfp_ip_header_sel[slice_num],
                      sel->ip_header_sel);
    }
    return (errcode);
}

STATIC int
_field_kt2_egress_selcodes_install(int            unit,
                                   _field_group_t *fg,
                                   uint8          slice_num,
                                   bcm_pbmp_t     *pbmp,
                                   int            selcode_idx)
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
    /* SELECTOR CODES for EFP_KEY4_DVP_SELECTOR register */
    if (sel->egr_key4_dvp_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(
            soc_reg_field32_modify(unit, EFP_KEY4_DVP_SELECTORr, REG_PORT_ANY,
                                dvpfldtbl[slice_num], sel->egr_key4_dvp_sel));
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
 *     _field_kt2_lookup_mode_set
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
_field_kt2_lookup_mode_set(int            unit,
                           uint8          slice_num,
                           _field_group_t *fg,
                           uint8          flags)
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

    BCM_IF_ERROR_RETURN(
        soc_reg64_get(unit, VFP_KEY_CONTROL_1r, REG_PORT_ANY, 0,
                      &vfp_key_control_1_buf));

    soc_reg64_field32_set(unit, VFP_KEY_CONTROL_1r,
                          &vfp_key_control_1_buf, pairing_flds[slice_num >> 1],
                          flags & _FP_GROUP_SPAN_DOUBLE_SLICE ? 1 : 0);
    soc_reg64_field32_set(unit, VFP_KEY_CONTROL_1r,
                          &vfp_key_control_1_buf, wide_mode_flds[slice_num],
                          flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE ? 1 : 0);

    return (soc_reg64_set(unit, VFP_KEY_CONTROL_1r, REG_PORT_ANY, 0,
                          vfp_key_control_1_buf));
}

/*
 * Function:
 *     _field_kt2_mode_set
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
_field_kt2_mode_set(int unit, uint8 slice_num,
                         _field_group_t *fg, uint8 flags)
{
    int (*func)(int unit, uint8 slice_num, _field_group_t *fg, uint8 flags);

    switch (fg->stage_id) {
        case _BCM_FIELD_STAGE_LOOKUP:
            func = _field_kt2_lookup_mode_set;
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
_field_kt2_egress_key_match_type_set (int unit, _field_entry_t *f_ent)
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

    /*
     * COVERITY
     * This flow takes care of the Out-of-bounds access issue for data and mask.
     */
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
_field_kt2_selcodes_install(int            unit,
                            _field_group_t *fg,
                            uint8          slice_num,
                            bcm_pbmp_t     pbmp,
                            int            selcode_idx)
{
    int rv;

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_field_kt2_mode_set(unit, slice_num, fg, fg->flags));

    switch (fg->stage_id) {
        case _BCM_FIELD_STAGE_LOOKUP:
            rv = _field_kt2_lookup_selcodes_install(unit, fg, slice_num,
                                                    selcode_idx);
            break;
        case _BCM_FIELD_STAGE_INGRESS:
            rv =  _bcm_field_trx_ingress_selcodes_install(unit, fg, slice_num,
                                                          &pbmp, selcode_idx);
            break;
        case _BCM_FIELD_STAGE_EGRESS:
            rv = _field_kt2_egress_selcodes_install(unit, fg, slice_num,
                                                    &pbmp, selcode_idx);;
            break;
        default:
            return (BCM_E_INTERNAL);
    }
    return (rv);
}

STATIC int
_field_kt2_group_install(int unit, _field_group_t *fg)
{
    _field_slice_t *fs;        /* Slice pointer.           */
    uint8  slice_number;       /* Slices iterator.         */
    int    parts_count;        /* Number of entry parts.   */
    int    idx;                /* Iteration index.         */

    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Get number of entry parts. */
    BCM_IF_ERROR_RETURN(
        _bcm_field_entry_tcam_parts_count(unit, fg->stage_id, 
                                          fg->flags, &parts_count));

    for (idx = 0; idx < parts_count; ++idx) {
        BCM_IF_ERROR_RETURN(
           _bcm_field_tcam_part_to_slice_number(unit, idx, fg,
                                                &slice_number));
        fs = fg->slices + slice_number;

        BCM_IF_ERROR_RETURN(
            _field_kt2_selcodes_install(unit, fg, fs->slice_number,
                                        fg->pbmp, idx));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_kt2_lookup_slice_clear
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
_field_kt2_lookup_slice_clear(int unit, unsigned slice_num)
{
    uint64 reg_val;

    BCM_IF_ERROR_RETURN(
        soc_reg64_get(unit, VFP_KEY_CONTROL_1r, REG_PORT_ANY, 0, &reg_val));
    soc_reg64_field32_set(unit, VFP_KEY_CONTROL_1r, &reg_val,
                          _bcm_field_trx_vfp_double_wide_sel[slice_num], 0);
    soc_reg64_field32_set(unit, VFP_KEY_CONTROL_1r, &reg_val,
                          _bcm_field_trx_vfp_field_sel[slice_num][0], 0);
    soc_reg64_field32_set(unit, VFP_KEY_CONTROL_1r, &reg_val,
                          _bcm_field_trx_vfp_field_sel[slice_num][1], 0);
    soc_reg64_field32_set(unit, VFP_KEY_CONTROL_1r, &reg_val,
                         _bcm_field_trx_slice_pairing_field[slice_num >> 1], 0);
    BCM_IF_ERROR_RETURN(
        soc_reg64_set(unit, VFP_KEY_CONTROL_1r, REG_PORT_ANY, 0, reg_val));

    return (soc_reg_field32_modify(unit, VFP_KEY_CONTROL_2r, REG_PORT_ANY,
                           _bcm_field_trx_vfp_ip_header_sel[slice_num],0));
}

/*
 * Function:
 *     _field_kt2_slice_clear
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
STATIC int
_field_kt2_slice_clear(int unit, _field_group_t *fg, _field_slice_t *fs)
{
    int rv;

    switch (fs->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          rv = _bcm_field_trx_ingress_slice_clear(unit, fs->slice_number);
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _field_kt2_lookup_slice_clear(unit, fs->slice_number);
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
 *     _field_kt2_write_slice_map_ingress
 * Purpose:
 *     Write FP_SLICE_MAP
 * Parameters:
 *     unit       - (IN) BCM device number
 *     stage_fc   - (IN) stage control structure
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_kt2_write_slice_map_ingress(int unit, _field_stage_t *stage_fc)
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
        VIRTUAL_SLICE_11_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
        VIRTUAL_SLICE_12_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
        VIRTUAL_SLICE_13_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
        VIRTUAL_SLICE_14_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
        VIRTUAL_SLICE_15_PHYSICAL_SLICE_NUMBER_ENTRY_0f
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
        VIRTUAL_SLICE_11_VIRTUAL_SLICE_GROUP_ENTRY_0f,
        VIRTUAL_SLICE_12_VIRTUAL_SLICE_GROUP_ENTRY_0f,
        VIRTUAL_SLICE_13_VIRTUAL_SLICE_GROUP_ENTRY_0f,
        VIRTUAL_SLICE_14_VIRTUAL_SLICE_GROUP_ENTRY_0f,
        VIRTUAL_SLICE_15_VIRTUAL_SLICE_GROUP_ENTRY_0f
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

#ifdef BCM_SABER2_SUPPORT
    /* Dagger_2 uses only 8 slices instead of 12
     * FP_SLICE_MAP entries for the upper four virtual slices
     * needs to updated with values > 7.
     * If not configured, those virtual slice will map to
     * physical slice 0 and slice 0 will be considered as
     * highest priority irrespective of its group priority
     * This fix is required for Beagle (Metrolite variant) */
    if (SOC_IS_SABER2(unit) && (vmap_size < 12)) {
        for (index = vmap_size; index < 12; index++) {
            field = physical_slice[index];
            if (!SOC_MEM_FIELD_VALID(unit,
                        FP_SLICE_MAPm, field)) {
                break;
            }
            value = index;
            soc_FP_SLICE_MAPm_field32_set(unit, &entry, field, value);

            value = index;
            field = slice_group[index];
            soc_FP_SLICE_MAPm_field32_set(unit, &entry, field, value);
        }
    }
#endif

    return WRITE_FP_SLICE_MAPm(unit, MEM_BLOCK_ALL, 0, &entry);
}

/*
 * Function:
 *     _field_kt2_write_slice_map
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
_field_kt2_write_slice_map(int unit, _field_stage_t *stage_fc,
                           _field_group_t *fg)
{
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        return _field_kt2_write_slice_map_ingress(unit, stage_fc);
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EGRESS) {
        return _bcm_field_trx_write_slice_map_egress(unit, stage_fc);
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_LOOKUP) {
        return _bcm_field_trx_write_slice_map_vfp(unit, stage_fc);
    }

    return BCM_E_INTERNAL;
}

/*
 * Function:
 *     _field_kt2_counter_write
 * Purpose:
 *     Write accumulated sw & hw counters
 * Parameters:
 *   unit          - (IN) BCM device number
 *   stage_fc      - (IN) Stage field control
 *   counter_x_mem - (IN) Memory name for X pipe FP counter
 *   mem_x_buf     - (IN) Memory table entry for X pipe FP counter
 *   counter_y_mem - (IN) Memory name for Y pipe FP counter
 *   mem_y_buf     - (IN) Memory table entry for Y pipe FP counter
 *   idx           - (IN) Counter index
 *   packet_count  - (IN) Packet counter value
 *   byte_count    - (IN) Byte counter value
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_kt2_counter_write(int unit, soc_mem_t mem, int idx, uint32 *buf,
                            _field_counter32_collect_t *cntrs32_buf,
                            _field_counter64_collect_t *cntrs64_buf,
                            uint64 *packet_count, uint64 *byte_count)
{
    uint32  hw_val[2];     /* Parsed field counter value.*/
    int     len = 0;       /* Field length */
    /* Set byte count in the memory */
    if (NULL != byte_count) {
        COMPILER_64_ZERO(cntrs64_buf->accumulated_counter);
        COMPILER_64_OR(cntrs64_buf->accumulated_counter, (*byte_count));
        len = soc_mem_field_length(unit, mem, BYTE_COUNTERf);
        if (len <= 32) {
           return BCM_E_INTERNAL;
        }
        COMPILER_64_SET(cntrs64_buf->last_hw_value,
                        COMPILER_64_HI(*byte_count) & ((1 << (len - 32)) - 1),
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
 *     _field_kt2_counter_set
 * Purpose:
 *     Write accumulated sw and hw counters
 * Parameters:
 *   unit          - (IN) BCM device number
 *   stage_fc      - (IN) Stage field control
 *   counter_x_mem - (IN) Memory name for X pipe FP counter
 *   mem_x_buf     - (IN) Memory table entry for X pipe FP counter
 *   counter_y_mem - (IN) Memory name for Y pipe FP counter
 *   mem_y_buf     - (IN) Memory table entry for Y pipe FP counter
 *   idx           - (IN) Counter index
 *   packet_count  - (IN) Packet counter value
 *   byte_count    - (IN) Byte counter value
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_kt2_counter_set(int unit, _field_stage_t *stage_fc,
                          soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                          soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                          int idx, uint64 *packet_count, uint64 *byte_count)
{
    _field_counter32_collect_t *cntrs32_buf;  /* Sw packet counter value    */
    _field_counter64_collect_t *cntrs64_buf;  /* Sw byte counter value      */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return BCM_E_PARAM;
    }

    /* Write counter value. */
    if (NULL != mem_x_buf) {
        cntrs64_buf = &stage_fc->_field_x64_counters[idx];
        cntrs32_buf = &stage_fc->_field_x32_counters[idx];
        BCM_IF_ERROR_RETURN(_field_kt2_counter_write(
                                unit, counter_x_mem, idx, mem_x_buf,
                                cntrs32_buf, cntrs64_buf,
                                packet_count, byte_count));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_kt2_counter_get
 * Purpose:
 *     Update accumulated sw counter and return current counter value.
 * Parameters:
 *   unit          - (IN)  BCM device number
 *   stage_fc      - (IN)  Stage field control
 *   counter_x_mem - (IN)  Memory name for X pipe FP counter
 *   mem_x_buf     - (IN)  Memory table entry for X pipe FP counter
 *   counter_y_mem - (IN)  Memory name for Y pipe FP counter
 *   mem_y_buf     - (IN)  Memory table entry for Y pipe FP counter
 *   idx           - (IN)  Counter index
 *   packet_count  - (OUT) Packet counter value
 *   byte_count    - (OUT) Byte counter value
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_kt2_counter_get(int unit, _field_stage_t *stage_fc,
                          soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                          soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                          int idx, uint64 *packet_count, uint64 *byte_count)
{
    _field_counter32_collect_t *cntrs32_buf = NULL;  /* Sw packet counter value    */
    _field_counter64_collect_t *cntrs64_buf = NULL;  /* Sw byte counter value      */
    soc_memacc_t *memacc_pkt = NULL, *memacc_byte = NULL; /*Memory access cache*/
    uint32  hw_val[2];     /* Parsed field counter value.*/

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

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_EGRESS) {
        /* EFP supports 36 bits Byte Counter. */
        _bcm_field_36bit_counter_update(unit, hw_val, cntrs64_buf);
    }
    else {
        _bcm_field_35bit_counter_update(unit, hw_val, cntrs64_buf);
    }

    /* Return counter value to caller if (out) pointer was provided. */
    if (NULL != byte_count) {
        COMPILER_64_ADD_64(*byte_count, cntrs64_buf->accumulated_counter);
    }

    /* Packet counter. */
    cntrs32_buf = &stage_fc->_field_x32_counters[idx];
    memacc_pkt =
        &(stage_fc->_field_memacc_counters[_FIELD_COUNTER_MEMACC_PACKET]);
    soc_memacc_field_get(memacc_pkt, mem_x_buf, hw_val);
    _bcm_field_29bit_counter_update(unit, hw_val, cntrs32_buf);
    /* Return counter value to caller if (out) pointer was provided. */
    if (NULL != packet_count) {
        COMPILER_64_ADD_64(*packet_count, cntrs32_buf->accumulated_counter);
    }
    return BCM_E_NONE;
}

/* FP_RESOLUTION packet field values for Katana2 */
static const struct {
    uint8 api, hw_data, hw_mask;
} pkt_res_xlate_tbl[] = {
    {  BCM_FIELD_PKT_RES_UNKNOWN,      0x00, 0x3f },
    {  BCM_FIELD_PKT_RES_CONTROL,      0x01, 0x3f },
    {  BCM_FIELD_PKT_RES_OAM,          0x02, 0x3f },
    {  BCM_FIELD_PKT_RES_BFD,          0x03, 0x3f },
    {  BCM_FIELD_PKT_RES_BPDU,         0x04, 0x3f },
    {  BCM_FIELD_PKT_RES_ICNM,         0x05, 0x3f },
    {  BCM_FIELD_PKT_RES_IEEE1588,     0x06, 0x3f },
    {  BCM_FIELD_PKT_RES_L2UC,         0x08, 0x3f },
    {  BCM_FIELD_PKT_RES_L2UNKNOWN,    0x09, 0x3f },
    {  BCM_FIELD_PKT_RES_L2BC,         0x0c, 0x3f },
    {  BCM_FIELD_PKT_RES_L2MCKNOWN,    0x0a, 0x3f },
    {  BCM_FIELD_PKT_RES_L2MCUNKNOWN,  0x0b, 0x3f },
    {  BCM_FIELD_PKT_RES_L3UCKNOWN,    0x10, 0x3f }, /* 16 */
    {  BCM_FIELD_PKT_RES_L3UCUNKNOWN,  0x11, 0x3f }, /* 17 */
    {  BCM_FIELD_PKT_RES_L3MCKNOWN,    0x12, 0x3f }, /* 18 */
    {  BCM_FIELD_PKT_RES_L3MCUNKNOWN,  0x13, 0x3f }, /* 19 */
    {  BCM_FIELD_PKT_RES_MPLSL2KNOWN,  0x18, 0x3f }, /* 24 */
    {  BCM_FIELD_PKT_RES_MPLSL3KNOWN,  0x1a, 0x3f }, /* 26 */
    {  BCM_FIELD_PKT_RES_MPLSKNOWN,    0x1c, 0x3f }, /* 28 */
    {  BCM_FIELD_PKT_RES_MPLSUNKNOWN,  0x19, 0x3f }, /* 25 */
    {  BCM_FIELD_PKT_RES_MPLSMCKNOWN,  0x1d, 0x3f }, /* 29 */
    {  BCM_FIELD_PKT_RES_MIMKNOWN,     0x20, 0x3f }, /* 32 */
    {  BCM_FIELD_PKT_RES_MIMUNKNOWN,   0x21, 0x3f }, /* 33 */
    {  BCM_FIELD_PKT_RES_TRILLKNOWN,   0x28, 0x3f }, /* 40 */
    {  BCM_FIELD_PKT_RES_TRILLUNKNOWN, 0x29, 0x3f }, /* 41 */
    {  BCM_FIELD_PKT_RES_NIVKNOWN,     0x30, 0x3f }, /* 48 */
    {  BCM_FIELD_PKT_RES_NIVUNKNOWN,   0x31, 0x3f }, /* 49 */
    {  BCM_FIELD_PKT_RES_L2_ANY,       0x08, 0x38 },
    {  BCM_FIELD_PKT_RES_L2MC_ANY,     0x0a, 0x3e },
    {  BCM_FIELD_PKT_RES_TRILL_ANY,    0x28, 0x3e },
    {  BCM_FIELD_PKT_RES_NIV_ANY,      0x30, 0x3e },
    {  BCM_FIELD_PKT_RES_MPLS_ANY,     0x18, 0x38 },
    {  BCM_FIELD_PKT_RES_MIM_ANY,      0x20, 0x3e },
    {  BCM_FIELD_PKT_RES_L3_ANY,       0x10, 0x3e }
};

int
_bcm_field_kt2_qualify_PacketRes(int               unit,
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
_bcm_field_kt2_qualify_PacketRes_get(int               unit,
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

STATIC int
_field_kt2_ingress_qual_tcam_key_mask_get(int unit,
                                          _field_entry_t *f_ent,
                                          _field_tcam_t *tcam)
{
    int errcode = BCM_E_INTERNAL;
    const unsigned fp_tcam_words
        = BITS2WORDS(soc_mem_field_length(unit,
                    FP_TCAMm,
                    DATAf
                    )
                );

    tcam->key_size
        = WORDS2BYTES(fp_tcam_words
                + BITS2WORDS(soc_mem_field_length(
                        unit,
                        FP_GLOBAL_MASK_TCAMm,
                        FULL_KEYf
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

        errcode = soc_mem_read(unit, FP_TCAMm, MEM_BLOCK_ANY,
                               tcam_idx, tcam_entry);
        if (BCM_FAILURE(errcode)) {
            goto error;
        }
        soc_mem_field_get(unit, FP_TCAMm, tcam_entry, DATAf, tcam->key);
        soc_mem_field_get(unit, FP_TCAMm, tcam_entry, DATA_MASKf, tcam->mask);

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
                FULL_KEYf,
                tcam->key + fp_tcam_words
                );
        soc_mem_field_get(unit,
                FP_GLOBAL_MASK_TCAMm,
                tcam_entry,
                FULL_MASKf,
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
_bcm_field_kt2_qual_tcam_key_mask_get(int unit,
                                      _field_entry_t *f_ent,
                                      _field_tcam_t *tcam
                                      )
{
    switch (f_ent->group->stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
        return (_field_kt2_ingress_qual_tcam_key_mask_get(unit, f_ent, tcam));
    default:
        ;
    }

    return (_field_qual_tcam_key_mask_get(unit, f_ent, tcam, 0));
}

STATIC int
_field_kt2_ingress_qual_tcam_key_mask_set(int unit,
                                          _field_entry_t *f_ent,
                                          unsigned       tcam_idx,
                                          unsigned       validf)
{
    _field_tcam_t  * const tcam = &f_ent->tcam;
    _field_tcam_t  * const extra_tcam = &f_ent->extra_tcam;
    _field_group_t * const fg = f_ent->group;
    uint32         tcam_entry[SOC_MAX_MEM_FIELD_WORDS];
    const unsigned fp_tcam_words
        = BITS2WORDS(soc_mem_field_length(unit,
                    FP_TCAMm,
                    DATAf
                    )
                );

    int valid_bits = 0;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, FP_TCAMm, MEM_BLOCK_ANY,
                                     tcam_idx, tcam_entry));
    soc_mem_field_set(unit, FP_TCAMm, tcam_entry, DATAf, tcam->key);
    soc_mem_field_set(unit, FP_TCAMm, tcam_entry, DATA_MASKf, tcam->mask);
    soc_mem_field32_set(unit, FP_TCAMm, tcam_entry, VALIDf, validf ? 
                       ((fg->flags & _FP_GROUP_LOOKUP_ENABLED) ? 3 : 2) : 0);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, FP_TCAMm, MEM_BLOCK_ALL,
                                      tcam_idx, tcam_entry));

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, FP_GLOBAL_MASK_TCAMm, MEM_BLOCK_ANY,
                                     tcam_idx, tcam_entry));
   if (!(f_ent->flags & _FP_ENTRY_USES_IPBM_OVERLAY)) {
       /* In TCAM A fixed and IPBM are valid so setting accordingly */
       soc_mem_field_set(unit,
               FP_GLOBAL_MASK_TCAMm,
               tcam_entry,
               FULL_KEYf,
               tcam->key + fp_tcam_words
               );
       soc_mem_field_set(unit,
               FP_GLOBAL_MASK_TCAMm,
               tcam_entry,
               FULL_MASKf,
               tcam->mask + fp_tcam_words
               );
       soc_mem_field_set(unit, FP_GLOBAL_MASK_TCAMm, tcam_entry, IPBMf,
               f_ent->pbmp.data.pbits);
       soc_mem_field_set(unit, FP_GLOBAL_MASK_TCAMm, tcam_entry, IPBM_MASKf,
               f_ent->pbmp.mask.pbits);
   } else {
       /* In TCAM B IPBM overlay bits are valid so setting accordingly */
       soc_mem_field_set(unit,
               FP_GLOBAL_MASK_TCAMm,
               tcam_entry,
               IPBMf,
               extra_tcam->key 
               );
       soc_mem_field_set(unit,
               FP_GLOBAL_MASK_TCAMm,
               tcam_entry,
               IPBM_MASKf,
               extra_tcam->mask 
               );
    }

    valid_bits = soc_mem_field_length(unit, FP_GLOBAL_MASK_TCAMm, VALIDf);
    soc_mem_field32_set(unit, FP_GLOBAL_MASK_TCAMm, tcam_entry, VALIDf, 
                        validf ? ((0x1 << valid_bits) -1):0);

    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, FP_GLOBAL_MASK_TCAMm, MEM_BLOCK_ALL,
                      tcam_idx, tcam_entry));

    return (BCM_E_NONE);
}


int
_bcm_field_kt2_qual_tcam_key_mask_set(int            unit,
                                      _field_entry_t *f_ent,
                                      unsigned       validf
                                      )
{
    int       tcam_idx;
    soc_mem_t tcam_mem;

    BCM_IF_ERROR_RETURN(
        _bcm_field_entry_tcam_idx_get(unit, f_ent, &tcam_idx));

    switch (f_ent->group->stage_id) {
    case _BCM_FIELD_STAGE_INGRESS:
        return (_field_kt2_ingress_qual_tcam_key_mask_set(unit,
                                                     f_ent, tcam_idx, validf));
    case _BCM_FIELD_STAGE_EGRESS:
        tcam_mem = EFP_TCAMm;
        break;
    case _BCM_FIELD_STAGE_LOOKUP:
        tcam_mem = VFP_TCAMm;
        break;
    default:
        return (BCM_E_INTERNAL);
    }
    {
        uint32 tcam_entry[SOC_MAX_MEM_FIELD_WORDS];
        sal_memset(tcam_entry, 0, sizeof(tcam_entry));

        BCM_IF_ERROR_RETURN(
            _bcm_field_trx_tcam_get(unit, tcam_mem, f_ent, tcam_entry));
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx, tcam_entry));
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_kt2_functions_init
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
_field_kt2_functions_init(_field_funct_t *functions)
{
    functions->fp_detach               = _bcm_field_tr_detach;
    functions->fp_group_install        = _field_kt2_group_install;
    functions->fp_selcodes_install     = _field_kt2_selcodes_install;
    functions->fp_slice_clear          = _field_kt2_slice_clear;
    functions->fp_entry_remove         = _bcm_field_fb_entry_remove;
    functions->fp_entry_move           = _bcm_field_fb_entry_move;
    functions->fp_selcode_get          = _bcm_field_tr_selcode_get;
    functions->fp_selcode_to_qset      = _bcm_field_selcode_to_qset;
    functions->fp_qual_list_get        = _bcm_field_qual_lists_get;
    functions->fp_tcam_policy_clear    = NULL;
    functions->fp_tcam_policy_install  = _bcm_field_tr_entry_install;
    functions->fp_tcam_policy_reinstall = _bcm_field_tr_entry_reinstall;
    functions->fp_policer_install      = _bcm_field_trx_policer_install;
    functions->fp_write_slice_map      = _field_kt2_write_slice_map;
    functions->fp_qualify_ip_type      = _bcm_field_trx_qualify_ip_type;
    functions->fp_qualify_ip_type_get  = _bcm_field_trx_qualify_ip_type_get;
    functions->fp_action_support_check = _bcm_field_trx_action_support_check;
    functions->fp_action_conflict_check = _bcm_field_trx_action_conflict_check;
    functions->fp_counter_get          = _field_kt2_counter_get;
    functions->fp_counter_set          = _field_kt2_counter_set;
    functions->fp_stat_index_get       = _bcm_field_trx_stat_index_get;
    functions->fp_action_params_check  = _bcm_field_trx_action_params_check;
    functions->fp_action_depends_check = NULL;
    functions->fp_external_entry_install  = NULL;
    functions->fp_external_entry_reinstall = NULL;
    functions->fp_external_entry_remove   = NULL;
    functions->fp_external_entry_prio_set = NULL;
    functions->fp_egress_key_match_type_set =
                      _field_kt2_egress_key_match_type_set;
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
    functions->fp_entry_stat_extended_attach =
        _bcm_field_kt2_entry_stat_extended_attach;
    functions->fp_entry_stat_extended_get =
        _bcm_field_kt2_entry_stat_extended_get;
    functions->fp_entry_stat_detach = _bcm_field_entry_stat_detach;
    functions->fp_class_size_get = NULL;
    functions->fp_policer_packet_counter_get32 =
                      _bcm_field_trx_policer_packet_counter_get;
    functions->fp_policer_packet_reset_counter_get32 =
                      _bcm_field_trx_policer_packet_reset_counter_get;
    functions->fp_qualify_packet_res = _bcm_field_kt2_qualify_PacketRes;
    functions->fp_qualify_packet_res_get = _bcm_field_kt2_qualify_PacketRes_get;
}

int
_bcm_field_kt2_qualify_class(int                 unit,
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
        break;


    case _BCM_FIELD_STAGE_LOOKUP:
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
            case bcmFieldQualifyIngressClassField:
                ifp_cl_type = 15;
                break;
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
_bcm_field_kt2_qualify_class_get(int                 unit,
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


/*
 * Function:
 *    _bcm_field_kt2_entry_stat_extended_attach
 *
 * Description:
 *       Attach statistics with action entity to Field Processor entry.
 * Parameters:
 *      unit        - (IN) BCM device number.
 *      entry       - (IN) Field entry id.
 *      stat_id     - (IN) Statistics entity id.
 *      stat_action - (IN) Stat Action.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_kt2_entry_stat_extended_attach(int unit,
                                          _field_entry_t *f_ent,
                                          int stat_id,
                                          bcm_field_stat_action_t stat_action)
{
    _field_entry_stat_t    *f_ent_st; /* Field entry statistics structure.*/
    _field_stat_t          *f_st;     /* Internal statisics descriptor.   */
    _field_stage_id_t      stage_id;  /* Pipeline stage id.               */
    int                    rv;        /* Operation return status.         */

    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    /* Get entry pipeline stage id. */
    stage_id = f_ent->group->stage_id;

    /* Valid only for Ingress & Egress Stage */
    if (stage_id != _BCM_FIELD_STAGE_LOOKUP) {
        return BCM_E_UNAVAIL;
    }

    /* Check params for stat action */
    if ((stat_action < bcmFieldStatActionIncrement)
            || (stat_action >= bcmFieldStatActionCount)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                (BSL_META("FP(unit %d) Error: Not a valid Stat Action value\r\n"),
                 unit));
        return BCM_E_PARAM;
    }

    /* Check if another statistics entity is already attached. */
    f_ent_st = &f_ent->statistic;

    if (f_ent_st->flags & _FP_ENTRY_EXTENDED_STAT_VALID) {
        return (BCM_E_EXISTS);
    }

    /* Check if statistics action conflicts with other actions */
    if (f_ent_st->flags & _FP_ENTRY_EXTENDED_STAT_NOT_ALLOWED) {
        return (BCM_E_CONFIG);
    }

    /* Check if statid is not normal stat id */
    if (f_ent_st->sid == stat_id) {
        return BCM_E_PARAM;
    }

    /* Get statistics entity  description structure. */
    rv = _bcm_field_stat_get(unit, stat_id, &f_st);
    BCM_IF_ERROR_RETURN(rv);

    if ((f_st->stage_id != stage_id) ||
            (f_st->gid !=  f_ent->group->gid)) {
        return (BCM_E_PARAM);
    }

    /* Increment statistics entity reference counter. */
    f_st->sw_ref_count++;

    /* Attach statistics entity to an entry. */
    f_ent_st->flags |= _FP_ENTRY_EXTENDED_STAT_VALID;
    f_ent_st->flags |= _FP_ENTRY_EXTENDED_STAT_RESERVED;
    f_ent_st->extended_sid    = stat_id;
    f_ent_st->stat_action = stat_action;

    /*
     * If qualifiers have not been modified for this entry,
     * set Action only dirty flag.
     */
    if (0 == (f_ent->flags & _FP_ENTRY_DIRTY)) {
        f_ent->flags |= _FP_ENTRY_POLICY_TABLE_ONLY_DIRTY;
    }

    /* Entry must be reinstalled for statistics to take effect. */
    f_ent->flags  |= _FP_ENTRY_DIRTY;

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _bcm_field_kt2_entry_stat_extended_get
 *
 * Description:
 *       Get extended statid and stat_action.
 * Parameters:
 *      unit        - (IN)  BCM device number.
 *      entry       - (IN)  Field entry id.
 *      stat_id     - (OUT) Statistics entity id.
 *      stat_action - (OUT) Stat Action.
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_field_kt2_entry_stat_extended_get (int unit,
                                         _field_entry_t *f_ent,
                                         int *stat_id,
                                         bcm_field_stat_action_t *stat_action)
{
    _field_stage_id_t      stage_id;  /* Pipeline stage id. */

    if ((f_ent == NULL) || (stat_id == NULL) || (stat_action == NULL)) {
        return BCM_E_PARAM;
    }

    /* Get entry pipeline stage id. */
    stage_id = f_ent->group->stage_id;

    /* Valid only for Ingress & Egress Stage */
    if (stage_id != _BCM_FIELD_STAGE_LOOKUP) {
        return BCM_E_UNAVAIL;
    }

    if (f_ent->statistic.flags & _FP_ENTRY_EXTENDED_STAT_VALID) {
        *stat_id = f_ent->statistic.extended_sid;
        *stat_action = f_ent->statistic.stat_action;
    } else {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_kt2_extended_stat_hw_free
 *
 * Purpose:
 *     Deallocate hw counter from an entry.
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     f_ent     - (IN) Entry statistics belongs to.
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_field_kt2_extended_stat_hw_free (int unit, _field_entry_t *f_ent)
{
    _field_entry_stat_t *f_ent_st;     /* Field entry statistics structure.*/
    _field_stat_t     *f_st;           /* Statistics entity descriptor.    */
    _field_control_t  *fc;             /* Field control.                   */
    uint64            value;           /* 64 bit zero to reset hw value.   */
    int               idx;             /* Statistics iteration index.      */
    int               rv;              /* Operation return status.         */
    _field_stage_t   *stage_fc = NULL; /* pipeline stage                   */
    int               entry_idx = 0;   /* Entry index at policy table      */
    _field_stage_id_t stage_id;        /* Pipeline stage id.               */
    soc_mem_t         tcam_mem;        /* TCAM memory id.         */
    soc_mem_t         policy_mem;      /* Policy table memory id. */

    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    /* Get entry pipeline stage id. */
    stage_id = f_ent->group->stage_id;

    /* Valid only for vfp lookup Stage */
    if (stage_id != _BCM_FIELD_STAGE_LOOKUP) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN (_field_control_get(unit, &fc));

    f_ent_st = &f_ent->statistic;

    /* Skip uninstalled statistics entity. */
    if (0 == (f_ent_st->flags & _FP_ENTRY_EXTENDED_STAT_INSTALLED)) {
        return (BCM_E_NONE);
    }

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                f_ent->group->stage_id, &stage_fc));

    rv = _field_fb_tcam_policy_mem_get(unit, f_ent->group->stage_id,
            &tcam_mem, &policy_mem);
    BCM_IF_ERROR_RETURN(rv);

    /* Read stat entity configuration. */
    BCM_IF_ERROR_RETURN(_bcm_field_stat_get(unit,
                f_ent_st->extended_sid, &f_st));

    /* Decrement hw reference count. */
    if (f_st->hw_ref_count > 0) {
        f_st->hw_ref_count--;
        if (f_st->flex_mode != 0 && f_st->hw_ref_count > 0) {
            /*
             * Get entry index at which flex STAT information
             * has been programmed in policy table.
             */
            BCM_IF_ERROR_RETURN (_bcm_field_slice_offset_to_tcam_idx(unit,
                        stage_fc, f_ent->group->instance,
                        f_ent->fs->slice_number,
                        f_ent->slice_idx, &entry_idx));

            rv = _bcm_esw_stat_flex_detach_ingress_table_counters (unit,
                    policy_mem, entry_idx);

            /*
             * Flex STAT module detached before field module, ignore
             * error during bcm_field_detach().
             */
            if (BCM_FAILURE(rv) && (TRUE == fc->init)) {
                return (rv);
            }
        }
    }

    /* Statistics is not used by any other entry. */
    if (f_st->hw_ref_count == 0) {
        COMPILER_64_ZERO(value);
        /* Read & Reset  individual statistics. */
        for (idx = 0; idx < f_st->nstat; idx++) {
            rv = _field_stat_value_get(unit, 0, f_st, f_st->stat_arr[idx],
                    f_st->stat_values + idx);
            if (BCM_FAILURE(rv)) {
                /* During system restart flex stat module
                 * might be reinitialized before fp reinit. Hence
                 * calls to the flex stat module will result in
                 * BCM_E_NOT_FOUND * error code.
                 */
                if (BCM_E_NOT_FOUND != rv) {
                    return rv;
                }
            } else {
                rv = _field_stat_value_set(unit, f_st,
                        f_st->stat_arr[idx], value);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
        /* Mark hw resource as unused in sw. */
        BCM_IF_ERROR_RETURN(_field_flex_counter_free(unit, f_ent, f_st));
    }
    /*
     * If qualifiers have not been modified for this entry,
     * set Action only dirty flag.
     */
    if (0 == (f_ent->flags & _FP_ENTRY_DIRTY)) {
        f_ent->flags |= _FP_ENTRY_POLICY_TABLE_ONLY_DIRTY;
    }

    f_ent_st->flags &= ~_FP_ENTRY_EXTENDED_STAT_INSTALLED;
    f_ent_st->flags &= ~_FP_ENTRY_EXTENDED_STAT_RESERVED;

    /* Mark entry for reinstall. */
    f_ent->flags |= _FP_ENTRY_DIRTY;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_kt2_entry_stat_detach
 *
 * Description:
 *       Detach statistics entity from Field processor entry.
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      f_ent     - (IN) Field entry structure.
 *      stat_id   - (IN) Statistics entity id.
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_field_kt2_entry_stat_detach (int unit,
                                          _field_entry_t *f_ent,
                                          int stat_id)
{

    _field_stat_t       *f_st;              /* Internal statistics entity. */
    _field_entry_stat_t *f_ent_st;          /* Field entry stat structure. */
    int                 rv;                 /* Operation return status.    */
    uint8               isExtendedStat = 0; /* Check if extended stat id.  */
    _field_stage_id_t      stage_id;        /* Pipeline stage id.          */

    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    /* Get entry pipeline stage id. */
    stage_id = f_ent->group->stage_id;

    /* Valid only for Vfp Lookup Stage */
    if (stage_id != _BCM_FIELD_STAGE_LOOKUP) {
        return _bcm_field_entry_stat_detach(unit, f_ent, stat_id);
    }

    f_ent_st = &f_ent->statistic;

    if (stat_id  == f_ent_st->sid) {
        isExtendedStat = 0;
    } else if (stat_id  == f_ent_st->extended_sid) {
        isExtendedStat = 1;
    } else {
        if ((0 == (f_ent_st->flags & _FP_ENTRY_STAT_VALID)) &&
                (0 == (f_ent_st->flags & _FP_ENTRY_EXTENDED_STAT_VALID))) {
            return (BCM_E_EMPTY);
        }
        return BCM_E_PARAM;
    }

    /* Make sure statistics entity attached to the entry. */
    if ((0 == (f_ent_st->flags & _FP_ENTRY_STAT_VALID)) && !isExtendedStat) {
        return (BCM_E_EMPTY);
    }

    if ((0 == (f_ent_st->flags & _FP_ENTRY_EXTENDED_STAT_VALID))
            && isExtendedStat) {
        return (BCM_E_EMPTY);
    }

    /* Get statics entity descriptor structure. */
    if (isExtendedStat) {
        BCM_IF_ERROR_RETURN(_bcm_field_stat_get(unit,
                    f_ent_st->extended_sid, &f_st));
        BCM_IF_ERROR_RETURN (
                _bcm_field_kt2_extended_stat_hw_free(unit, f_ent));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_stat_get(unit, f_ent_st->sid, &f_st));
        BCM_IF_ERROR_RETURN (_bcm_field_stat_hw_free(unit, f_ent));
    }

    /* Decrement statistics entity reference counter. */
    f_st->sw_ref_count--;

    /* If no one else is using the entity and it is internal destroy it. */
    if ((1 == f_st->sw_ref_count) && (f_st->hw_flags & _FP_STAT_INTERNAL)) {
        if (isExtendedStat) {
            rv = _field_stat_destroy(unit, f_ent_st->extended_sid);
        } else {
            rv = _field_stat_destroy(unit, f_ent_st->sid);
        }
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Detach policer from an entry. */
    if (isExtendedStat) {
        f_ent_st->extended_sid   = _FP_INVALID_INDEX;
        f_ent_st->flags &= ~(_FP_ENTRY_EXTENDED_STAT_VALID);
        f_ent_st->flags &= ~(_FP_ENTRY_EXTENDED_STAT_INSTALLED);
        f_ent_st->flags &= ~(_FP_ENTRY_STAT_INSTALLED);
    } else {
        f_ent_st->sid   = _FP_INVALID_INDEX;
        f_ent_st->flags &= ~(_FP_ENTRY_STAT_VALID);
        f_ent_st->flags &= ~(_FP_ENTRY_STAT_INSTALLED);
        f_ent_st->flags &= ~(_FP_ENTRY_STAT_USE_EVEN);
        f_ent_st->flags &= ~(_FP_ENTRY_STAT_USE_ODD);
    }

    /* Mark entry for reinstall. */
    f_ent->flags |= _FP_ENTRY_DIRTY;

    return BCM_E_NONE;
}

#if defined(BCM_WARM_BOOT_SUPPORT)
int
_field_kt2_stage_lookup_reinit(int unit, _field_control_t *fc,
                               _field_stage_t *stage_fc)
{
    int vslice_idx, max, master_slice;
    int idx, idx1, slice_idx, index_min, index_max, ratio, rv = BCM_E_NONE;
    int group_found, mem_sz, parts_count, slice_ent_cnt, expanded[4], slice_master_idx[4];
    int i, pri_tcam_idx, part_index, slice_number, prev_prio;
    uint32 *vfp_tcam_buf = NULL; /* Buffer to read the VFP_TCAM table */
    char *vfp_policy_buf = NULL; /* Buffer to read the VFP_POLICY table */
    uint32 rval, paired, intraslice, dbl_wide_key, dbl_wide_key_sec;
    uint32 vfp_key2, temp;
    uint64 vfp_key1;
    soc_field_t fld;
    vfp_tcam_entry_t *vfp_tcam_entry;
    vfp_policy_table_entry_t *vfp_policy_entry;
    _field_hw_qual_info_t hw_sels;
    _field_slice_t *fs;
    _field_group_t *fg;
    _field_entry_t *f_ent = NULL;
    bcm_pbmp_t entry_pbmp, temp_pbmp;
    uint8 *buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    uint8 *buf1 = fc->scache_ptr[_FIELD_SCACHE_PART_1];
    int offset_mode = 0, policer_index = 0;
    uint8 old_physical_slice, slice_num;
    uint32 entry_flags;
    _field_slice_t *fs_temp = NULL;
    _field_action_bmp_t action_bmp;
    _field_entry_wb_info_t f_ent_wb_info;
    soc_field_t vfp_en_flds[4] = {SLICE_ENABLE_SLICE_0f, SLICE_ENABLE_SLICE_1f,
                                  SLICE_ENABLE_SLICE_2f, SLICE_ENABLE_SLICE_3f};

    soc_field_t vfp_lk_en_flds[4] =
                     {LOOKUP_ENABLE_SLICE_0f, LOOKUP_ENABLE_SLICE_1f,
                      LOOKUP_ENABLE_SLICE_2f, LOOKUP_ENABLE_SLICE_3f};


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
    vfp_tcam_buf = soc_cm_salloc(unit, sizeof(vfp_tcam_entry_t) *
                                soc_mem_index_count(unit, VFP_TCAMm),
                                "VFP TCAM buffer");
    if (NULL == vfp_tcam_buf) {
        return BCM_E_MEMORY;
    }
    sal_memset(vfp_tcam_buf, 0, sizeof(vfp_tcam_entry_t) *
               soc_mem_index_count(unit, VFP_TCAMm));
    index_min = soc_mem_index_min(unit, VFP_TCAMm);
    index_max = soc_mem_index_max(unit, VFP_TCAMm);
    fs = stage_fc->slices[_FP_DEF_INST];
    if (stage_fc->flags & _FP_STAGE_HALF_SLICE) {
        slice_ent_cnt = fs->entry_count * 2;
        /* DMA in chunks */
        for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
            fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
            if ((rv = soc_mem_read_range(unit, VFP_TCAMm, MEM_BLOCK_ALL,
                                         slice_idx * slice_ent_cnt,
                                         slice_idx * slice_ent_cnt +
                                             fs->entry_count / 2 - 1,
                                         vfp_tcam_buf + slice_idx *
                                             slice_ent_cnt *
                                  soc_mem_entry_words(unit, VFP_TCAMm))) < 0 ) {
                goto cleanup;
            }
            if ((rv = soc_mem_read_range(unit, VFP_TCAMm, MEM_BLOCK_ALL,
                                         slice_idx * slice_ent_cnt +
                                         fs->entry_count,
                                         slice_idx * slice_ent_cnt +
                                         fs->entry_count +
                                             fs->entry_count / 2 - 1,
                                         vfp_tcam_buf + (slice_idx *
                                         slice_ent_cnt + fs->entry_count) *
                                   soc_mem_entry_words(unit, VFP_TCAMm))) < 0 ) {
                goto cleanup;
            }
        }
    } else {
        slice_ent_cnt = fs->entry_count;
        if ((rv = soc_mem_read_range(unit, VFP_TCAMm, MEM_BLOCK_ALL,
                                     index_min, index_max,
                                     vfp_tcam_buf)) < 0 ) {
            goto cleanup;
        }
    }
    vfp_policy_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES
                                  (unit, VFP_POLICY_TABLEm),
                                  "VFP POLICY TABLE buffer");
    if (NULL == vfp_policy_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, VFP_POLICY_TABLEm);
    index_max = soc_mem_index_max(unit, VFP_POLICY_TABLEm);
    if ((rv = soc_mem_read_range(unit, VFP_POLICY_TABLEm, MEM_BLOCK_ALL,
                                 index_min, index_max,
                                 vfp_policy_buf)) < 0 ) {
        goto cleanup;
    }

    /* Get slice expansion status and virtual map */
    if ((rv = _field_slice_expanded_status_get(unit, fc, stage_fc, expanded, slice_master_idx)) < 0) {
        goto cleanup;
    }

    /* Iterate over the slices */
    if ((rv = READ_VFP_SLICE_CONTROLr(unit, &rval)) < 0) {
        goto cleanup;
    }
    if ((rv = READ_VFP_KEY_CONTROL_1r(unit, &vfp_key1)) < 0) {
        goto cleanup;
    }
    if (SOC_REG_IS_VALID(unit, VFP_KEY_CONTROL_2r)) {
        if ((rv = READ_VFP_KEY_CONTROL_2r(unit, &vfp_key2)) < 0) {
            goto cleanup;
        }
    }
    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        /* Ignore disabled slice */
        if ((soc_reg_field_get(unit, VFP_SLICE_CONTROLr, rval,
                               vfp_en_flds[slice_idx]) == 0) ||
            (soc_reg_field_get(unit, VFP_SLICE_CONTROLr, rval,
                               vfp_lk_en_flds[slice_idx]) == 0)) {
            continue;
        }
        /* Ignore secondary slice in paired mode */
        fld = _bcm_field_trx_slice_pairing_field[slice_idx / 2];
        paired = soc_reg64_field32_get(unit, VFP_KEY_CONTROL_1r, vfp_key1, fld);
        fld = _kt2_vfp_slice_wide_mode_field[slice_idx];
        if (soc_reg_field_valid(unit, VFP_KEY_CONTROL_1r, fld)) {
            intraslice = soc_reg64_field32_get(unit, VFP_KEY_CONTROL_1r, vfp_key1, fld);
        } else {
            intraslice = 0;
        }
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
                                 (unit, VFP_TCAMm, vfp_tcam_entry_t *,
                                  vfp_tcam_buf, idx +
                                  slice_ent_cnt * slice_idx);
            if (soc_VFP_TCAMm_field32_get(unit, vfp_tcam_entry,
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
            = soc_reg64_field32_get(unit,
                                VFP_KEY_CONTROL_1r,
                                vfp_key1,
                                _bcm_field_trx_vfp_field_sel[slice_idx][0]
                                );
        hw_sels.pri_slice[0].fpf3
            = soc_reg64_field32_get(unit,
                                VFP_KEY_CONTROL_1r,
                                vfp_key1,
                                _bcm_field_trx_vfp_field_sel[slice_idx][1]
                                );
        if (SOC_REG_IS_VALID(unit, VFP_KEY_CONTROL_2r)) {
            hw_sels.pri_slice[0].ip_header_sel
                = soc_reg_field_get(unit,
                                VFP_KEY_CONTROL_2r,
                                vfp_key2,
                                _bcm_field_trx_vfp_ip_header_sel[slice_idx]
                                );
        }

        hw_sels.pri_slice[0].src_entity_sel
                = soc_reg64_field32_get(unit,
                                    VFP_KEY_CONTROL_1r,
                                    vfp_key1,
                                    _kt2_vfp_src_type_sel[slice_idx]
                                    );           

        /* If intraslice, get double-wide key - only 2 options */
        if (intraslice) {
            dbl_wide_key = soc_reg64_field32_get(
                               unit,
                               VFP_KEY_CONTROL_1r,
                               vfp_key1,
                               _bcm_field_trx_vfp_double_wide_sel[slice_idx]
                                             );
            hw_sels.pri_slice[1].intraslice = TRUE;
            hw_sels.pri_slice[1].fpf2 = dbl_wide_key;
            if (SOC_REG_IS_VALID(unit, VFP_KEY_CONTROL_2r)) {
                hw_sels.pri_slice[1].ip_header_sel
                    = soc_reg_field_get(unit,
                                    VFP_KEY_CONTROL_2r, vfp_key2,
                                    _bcm_field_trx_vfp_ip_header_sel[slice_idx]
                                    );
            }
            hw_sels.pri_slice[1].src_entity_sel
                   = soc_reg64_field32_get(unit,
                                       VFP_KEY_CONTROL_1r,
                                       vfp_key1,
                                       _kt2_vfp_src_type_sel[slice_idx]
                                       );
        }
        /* If paired, get secondary slice's selectors */
        if (paired) {
            hw_sels.sec_slice[0].fpf1 = 0;
            hw_sels.sec_slice[0].fpf2
                = soc_reg64_field32_get(
                      unit,
                      VFP_KEY_CONTROL_1r,
                      vfp_key1,
                      _bcm_field_trx_vfp_field_sel[slice_idx + 1][0]
                                    );
            hw_sels.sec_slice[0].fpf3
                = soc_reg64_field32_get(
                      unit,
                      VFP_KEY_CONTROL_1r,
                      vfp_key1,
                      _bcm_field_trx_vfp_field_sel[slice_idx + 1][1]
                                    );
            if (SOC_REG_IS_VALID(unit, VFP_KEY_CONTROL_2r)) {
                hw_sels.sec_slice[0].ip_header_sel
                    = soc_reg_field_get(unit,
                                    VFP_KEY_CONTROL_2r, vfp_key2,
                                    _bcm_field_trx_vfp_ip_header_sel[slice_idx]
                                    );
            }
            hw_sels.sec_slice[0].src_entity_sel
                   = soc_reg64_field32_get(unit,
                                       VFP_KEY_CONTROL_1r,
                                       vfp_key1,
                                       _kt2_vfp_src_type_sel[slice_idx]
                                       );
            if (intraslice) {
                dbl_wide_key_sec
                    = soc_reg64_field32_get(
                          unit,
                          VFP_KEY_CONTROL_1r,
                          vfp_key1,
                          _bcm_field_trx_vfp_double_wide_sel[slice_idx]
                                        );
                hw_sels.sec_slice[1].intraslice = TRUE;
                hw_sels.sec_slice[1].fpf2 = dbl_wide_key_sec;
                if (SOC_REG_IS_VALID(unit, VFP_KEY_CONTROL_2r)) {
                    hw_sels.sec_slice[1].ip_header_sel
                        = soc_reg_field_get(
                          unit,
                          VFP_KEY_CONTROL_2r,
                          vfp_key2,
                          _bcm_field_trx_vfp_ip_header_sel[slice_idx]
                                        );
                }
                hw_sels.sec_slice[1].src_entity_sel
                      = soc_reg_field_get(unit,
                                          VFP_KEY_CONTROL_1r,
                                          vfp_key1,
                                          _kt2_vfp_src_type_sel[slice_idx]
                                          );
            }
        }
        /* Create a group based on HW qualifiers (or find existing) */
        rv = _field_tr2_group_construct(unit, _FP_DEF_INST,
                                        &hw_sels, intraslice,
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
                                 (unit, VFP_TCAMm, vfp_tcam_entry_t *,
                                  vfp_tcam_buf, idx +
                                  slice_ent_cnt * slice_idx);
            if (soc_VFP_TCAMm_field32_get(unit, vfp_tcam_entry,
                                          VALIDf) == 0) {
                continue;
            }
            /* All ports are applicable to this entry */
            SOC_PBMP_ASSIGN(entry_pbmp, PBMP_PORT_ALL(unit));
            SOC_PBMP_OR(entry_pbmp, PBMP_CMIC(unit));
            if (soc_feature(unit, soc_feature_linkphy_coe) ||
                soc_feature(unit, soc_feature_subtag_coe)) {
                _bcm_kt2_subport_pbmp_update(unit, &entry_pbmp);
            }
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
                if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
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
                rv = _bcm_field_tcam_part_to_entry_flags(unit, i, fg,
                                                         &f_ent[i].flags);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                if (f_ent_wb_info.color_independent) {
                    f_ent[i].flags |= _FP_ENTRY_COLOR_INDEPENDENT;
                }
                rv = _bcm_field_entry_part_tcam_idx_get(unit, f_ent,
                                                        pri_tcam_idx,
                                                        i, &part_index);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc,
                                                         _FP_DEF_INST,
                                                         part_index,
                                                         &slice_number,
                                                (int *)&f_ent[i].slice_idx);
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
                                                (unit, VFP_POLICY_TABLEm,
                                                 vfp_policy_table_entry_t *,
                                                 vfp_policy_buf, part_index);
                rv = _field_tr2_actions_recover(unit,
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
        paired = soc_reg64_field32_get(unit, VFP_KEY_CONTROL_1r, vfp_key1, fld);
        if (paired && (slice_idx % 2)) {
            continue;
        }
        /* Skip if slice has no valid groups and entries */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        for (idx = 0; idx < slice_ent_cnt; idx++) {
            vfp_tcam_entry = soc_mem_table_idx_to_pointer
                                 (unit, VFP_TCAMm, vfp_tcam_entry_t *,
                                  vfp_tcam_buf, idx +
                                  slice_ent_cnt * slice_idx);
            if (soc_VFP_TCAMm_field32_get(unit, vfp_tcam_entry,
                                          VALIDf) != 0) {
                break;
            }
        }

        if (idx == slice_ent_cnt) {
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

        /* Now find the master slice for this virtual group */
        vslice_idx = _field_physical_to_virtual(unit, _FP_DEF_INST,
                                              slice_idx, stage_fc);
        if (vslice_idx < 0) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }

        if (fc->l2warm
                && ((fc->wb_recovered_version) >=BCM_FIELD_WB_VERSION_1_8)) {
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
            group_found = 0;
            vfp_tcam_entry = soc_mem_table_idx_to_pointer
                                 (unit, VFP_TCAMm, vfp_tcam_entry_t *,
                                  vfp_tcam_buf, idx +
                                  slice_ent_cnt * slice_idx);
            if (soc_VFP_TCAMm_field32_get(unit, vfp_tcam_entry,
                                          VALIDf) == 0) {
                continue;
            }

            /* All ports are applicable to this entry */
            SOC_PBMP_ASSIGN(entry_pbmp, PBMP_PORT_ALL(unit));
            SOC_PBMP_OR(entry_pbmp, PBMP_CMIC(unit));
            if (soc_feature(unit, soc_feature_linkphy_coe) ||
                soc_feature(unit, soc_feature_subtag_coe)) {
                _bcm_kt2_subport_pbmp_update(unit, &entry_pbmp);
            }
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
                if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
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
                rv = _bcm_field_tcam_part_to_entry_flags(unit, i, fg,
                                                         &f_ent[i].flags);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                if (f_ent_wb_info.color_independent) {
                    f_ent[i].flags |= _FP_ENTRY_COLOR_INDEPENDENT;
                }
                rv = _bcm_field_entry_part_tcam_idx_get(unit, f_ent,
                                                        pri_tcam_idx,
                                                        i, &part_index);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc,
                                                         _FP_DEF_INST,
                                                         part_index,
                                                         &slice_number,
                                                (int *)&f_ent[i].slice_idx);
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
                                                (unit, VFP_POLICY_TABLEm,
                                                 vfp_policy_table_entry_t *,
                                                 vfp_policy_buf, part_index);
                rv = _field_tr2_actions_recover(unit,
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

    _field_tr2_stage_reinit_all_groups_cleanup(unit, fc,
                                               _BCM_FIELD_STAGE_LOOKUP,
                                               NULL);

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
 *     _bcm_field_kt2_loopback_type_sel_recover
 *
 * Purpose:
 *     Retrieve slice TunnelType/LoopbackType settings from installed
 *     field entry.
 *
 * Parameters:
 *     unit              - (IN) BCM device number
 *     slice_idx         - (IN) Slice number to enable
 *     fp_tcam_buf       - (IN) TCAM entry
 *     stage_fc          - (IN) FP stage control info.
 *     intraslice        - (IN) Slice in Intra-Slice Double Wide mode.
 *     loopback_type_sel - (OUT) Tunnel/Loopback Type selector value
 *
 * Returns:
 *     Nothing
 */
int
_bcm_field_kt2_loopback_type_sel_recover(int unit,
                                     int slice_idx,
                                      char *fp_global_mask_tcam_buf,
                                     _field_stage_t *stage_fc,
                                     int intraslice,
                                     int8 *loopback_type_sel)
{
    fp_global_mask_tcam_entry_t *global_tcam_entry;                /* Installed TCAM entry.     */
    uint32 tunnel_loopback_type = 0;            /* Hw Tunnel/Loopback value. */
    fp_slice_map_entry_t fp_slice_map_buf;      /* Slice map table entry.    */
    _field_slice_t       *fs;                   /* Pointer to Slice info.    */
    _field_control_t     *fc;                   /* Pointer to field control  */
    int slice_ent_cnt;                          /* No. of entries in slice.  */
    int idx;                                    /* Slice index.              */
    int vmap_size, index;                       /* Virtual Map entry size.   */
    int virtual_group = -1;                     /* Virtual Group ID.         */
    int phy_slice;                              /* Physical slice value.     */
    int offset;                                 /* offset of loopback_type   */

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
#if defined(BCM_METROLITE_SUPPORT)
    if (SOC_IS_METROLITE(unit)) {
        offset = 44;
    } else
#endif
#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        offset = 108;
    } else
#endif
    {
        offset = 183;
    }

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
            global_tcam_entry = soc_mem_table_idx_to_pointer
                            (unit,
                             FP_GLOBAL_MASK_TCAMm,
                             fp_global_mask_tcam_entry_t *,
                             fp_global_mask_tcam_buf,
                             idx + slice_ent_cnt * phy_slice
                             );
            if (soc_FP_GLOBAL_MASK_TCAMm_field32_get(unit, 
                                                     global_tcam_entry, 
                                                     VALIDf) != 0) {
                /* Extract Tunnel Info from global_tcam_entry  */
                _field_extract((uint32 *)global_tcam_entry,
                               offset,
                               4,
                               &tunnel_loopback_type
                               );
                switch (tunnel_loopback_type) {
                    case 0x1: /* IP_TUNNEL       */
                    case 0x2: /* MPLS_TUNNEL     */
                    case 0x3: /* MIM_TUNNEL      */
                    case 0x4: /* WLAN_WTP TUNNEL */
                    case 0x5: /* WLAN_AC TUNNEL  */
                    case 0x6: /* AMT_TUNNEL      */
                    case 0x7: /* TRILL_TUNNEL    */
                        *loopback_type_sel = 1;
                        break;
                    case 0x8: /* NONE                                    */
                    case 0x9: /* EGRESS MIRROR LOOPBACK                  */
                    case 0xa: /* EP REDIRECTION LOOPBACK                 */
                    case 0xb: /* MiM LOOPBACK                            */
                    case 0xc: /* WLAN LOOPBACK || Trill Network Loopback */
                    case 0xd: /* Trill Access Loopback                   */
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

#endif /* BCM_WARM_BOOT_SUPPORT */
#else /* BCM_KATANA2_SUPPORT && BCM_FIELD_SUPPORT */
typedef int _kt2_field_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_KATANA2_SUPPORT && BCM_FIELD_SUPPORT */
