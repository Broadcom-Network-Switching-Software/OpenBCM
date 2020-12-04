/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field.c
 * Purpose:     BCM56624 Field Processor installation functions.
 */
#include <shared/bsl.h>

#include <soc/defs.h>
#if defined(BCM_TRX_SUPPORT) && defined(BCM_FIELD_SUPPORT)

#include <soc/drv.h>
#include <soc/scache.h>

#include <bcm/error.h>
#include <bcm/l3.h>
#include <bcm/field.h>
#include <bcm/tunnel.h>

#include <bcm_int/esw_dispatch.h>

#include <bcm_int/esw/field.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/katana.h>
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/trident2.h>
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_GREYHOUND2_SUPPORT)
#include <bcm_int/esw/policer.h>
#include <bcm_int/esw/flex_ctr.h>
#endif /* BCM_KATANA_SUPPORT || BCM_TRIUMPH3_SUPPORT ||
          BCM_GREYHOUND2_SUPPORT */

/* local/static function prototypes */
STATIC void _field_tr_functions_init(_field_funct_t *functions) ;

/*
 * Function:
 *     _bcm_field_tr_hw_clear
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
int
_bcm_field_tr_hw_clear(int unit, _field_stage_t *stage_fc)
{
    bcm_pbmp_t valid_pbm;         /* Holds valid pbm supported */
    bcm_port_t port;              /* Port iterator.           */
    int idx;                      /* Stages/slices iterator.  */
    int rv;                       /* Operation return status. */

    if (SOC_WARM_BOOT(unit)) {
        return (BCM_E_NONE);
    }

    switch (stage_fc->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
#if defined(BCM_TRIUMPH2_SUPPORT)
          if (SOC_MEM_IS_VALID(unit, FP_UDF_TCAMm)) {
              rv = soc_mem_clear(unit, FP_UDF_TCAMm, COPYNO_ALL, TRUE);
              BCM_IF_ERROR_RETURN(rv);
          }
#endif
          rv = soc_mem_clear(unit, FP_UDF_OFFSETm, COPYNO_ALL, TRUE);
          BCM_IF_ERROR_RETURN(rv);
          rv = soc_mem_clear(unit, FP_RANGE_CHECKm, COPYNO_ALL, TRUE);
          BCM_IF_ERROR_RETURN(rv);
          rv = soc_mem_clear(unit, FP_TCAMm, COPYNO_ALL, TRUE);
          BCM_IF_ERROR_RETURN(rv);
          rv = soc_mem_clear(unit, FP_POLICY_TABLEm, COPYNO_ALL, TRUE);
          BCM_IF_ERROR_RETURN(rv);
          rv = soc_mem_clear(unit, FP_METER_TABLEm, COPYNO_ALL, TRUE);
          BCM_IF_ERROR_RETURN(rv);
          rv = soc_mem_clear(unit, FP_COUNTER_TABLEm, COPYNO_ALL, TRUE);
          BCM_IF_ERROR_RETURN(rv);
          rv = soc_mem_clear(unit, FP_PORT_FIELD_SELm, COPYNO_ALL, TRUE);
          BCM_IF_ERROR_RETURN(rv);
          rv = soc_mem_clear(unit, FP_SLICE_MAPm, COPYNO_ALL, TRUE);
          BCM_IF_ERROR_RETURN(rv);
          rv = soc_mem_clear(unit, FP_SLICE_KEY_CONTROLm, COPYNO_ALL, TRUE);
          BCM_IF_ERROR_RETURN(rv);
          break;

      case _BCM_FIELD_STAGE_LOOKUP:
          rv = soc_mem_clear((unit), VFP_TCAMm, COPYNO_ALL, TRUE);
          BCM_IF_ERROR_RETURN(rv);
          rv = soc_mem_clear((unit), VFP_POLICY_TABLEm, COPYNO_ALL, TRUE);
          BCM_IF_ERROR_RETURN(rv);
          break;

      case _BCM_FIELD_STAGE_EGRESS:
          rv = soc_mem_clear(unit, EFP_TCAMm, COPYNO_ALL, TRUE);
          BCM_IF_ERROR_RETURN(rv);
          rv = soc_mem_clear(unit, EFP_POLICY_TABLEm, COPYNO_ALL, TRUE);
          BCM_IF_ERROR_RETURN(rv);
          rv = soc_mem_clear(unit, EFP_METER_TABLEm, COPYNO_ALL, TRUE);
          BCM_IF_ERROR_RETURN(rv);
          rv = soc_mem_clear(unit, EFP_COUNTER_TABLEm, COPYNO_ALL, TRUE);
          BCM_IF_ERROR_RETURN(rv);
          break;

      case _BCM_FIELD_STAGE_EXTERNAL:
          /* Read device port configuration. */
          BCM_PBMP_CLEAR(valid_pbm);
          BCM_IF_ERROR_RETURN(_bcm_field_valid_pbmp_get(unit, &valid_pbm));

          /* Reset the ACL related fields in ESM_MODE_PER_PORTr register */
          BCM_PBMP_ITER(valid_pbm, port) {
              rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                          L2_ACL_ENf, 0);
              BCM_IF_ERROR_RETURN(rv);
              rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                          L2_ACL_144_ENf, 0);
              BCM_IF_ERROR_RETURN(rv);
              rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                          IPV4_ACL_TYPEf, 0);
              BCM_IF_ERROR_RETURN(rv);
              rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                          IPV4_ACL_MODEf, 0);
              BCM_IF_ERROR_RETURN(rv);
              rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                          IPV4_ACL_144_ENf, 0);
              BCM_IF_ERROR_RETURN(rv);
              rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                          IPV6_ACL_TYPEf, 0);
              BCM_IF_ERROR_RETURN(rv);
              rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                          IPV6_ACL_MODEf, 0);
              BCM_IF_ERROR_RETURN(rv);
              rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                          IPV6_ACL_144_ENf, 0);
              BCM_IF_ERROR_RETURN(rv);
              rv =  soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                           IPV6_FULL_ACLf, 0);
              BCM_IF_ERROR_RETURN(rv);
          }

          /* Clear the TCAM, Policy mems */
          for (idx = 0; idx < stage_fc->tcam_slices; idx++) {
              if (_bcm_field_ext_data_mask_mems[idx] != 0) {
                  rv = soc_mem_clear((unit), _bcm_field_ext_data_mask_mems[idx], 
                                     COPYNO_ALL, TRUE);
                  BCM_IF_ERROR_RETURN(rv);
              }
              if (_bcm_field_ext_data_mems[idx] != 0) {
                  rv = soc_mem_clear((unit), _bcm_field_ext_data_mems[idx], 
                                     COPYNO_ALL, TRUE);
                  BCM_IF_ERROR_RETURN(rv);
              }
              if (_bcm_field_ext_mask_mems[idx] != 0) {
                  rv = soc_mem_clear((unit), _bcm_field_ext_mask_mems[idx], COPYNO_ALL, TRUE);
                  BCM_IF_ERROR_RETURN(rv);
              }
              rv = soc_mem_clear((unit), _bcm_field_ext_policy_mems[idx], COPYNO_ALL, TRUE);
              BCM_IF_ERROR_RETURN(rv);
          }

          /* Clear other mems */
          rv = soc_mem_clear((unit), EXT_FP_CNTRm, COPYNO_ALL, TRUE);
          BCM_IF_ERROR_RETURN(rv);
          rv = soc_mem_clear((unit), ESM_RANGE_CHECKm, COPYNO_ALL, TRUE);
          BCM_IF_ERROR_RETURN(rv);
          break;
    } /* end switch */

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr_ingress_qualifiers_init
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
_field_tr_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
                 _bcmFieldSliceSelDisable, 0, 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelDisable, 0, 7, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelDisable, 0, 11, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelDisable, 0, 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelDisable, 0, 13, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelDisable, 0, 14, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelDisable, 0, 15, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelDisable, 0, 16, 1);

    /* FPF3 primary slice single wide. */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 0, 19, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 1, 19, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 2, 19, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 2, 25, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 0, 25, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 1, 25, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 3, 25, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 3, 19, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
                 _bcmFieldSliceSelFpf3, 0, 39, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, 0, 31, 8);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 0, 19, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 1, 19, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 2, 19, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 2, 25, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 0, 25, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 1, 25, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 3, 25, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 3, 19, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, 1, 31, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 1, 39, 12);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf3, 2, 19, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf3, 2, 19, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf3, 2, 33, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf3, 2, 33, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 2, 47, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 3, 19, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 3, 19, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 3, 31, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 3, 32, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 3, 35, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 3, 35, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 3, 37, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 3, 39, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 3, 42, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf3, 3, 46, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 4, 19, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 4, 19, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 4, 31, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 4, 32, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 4, 35, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 4, 35, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 4, 47, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 4, 48, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 5, 19, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 5, 19, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 5, 31, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 5, 32, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 5, 35, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, 6, 20, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf3, 6, 21, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf3, 6, 22, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf3, 6, 24, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf3, 6, 25, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf3, 6, 26, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf3, 6, 28, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf3, 6, 29, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf3, 6, 30, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf3, 6, 31, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf3, 6, 32, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf3, 6, 33, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf3, 6, 34, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 6, 35, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 6, 35, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 6, 47, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 6, 48, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf3, 7, 19, 24);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, _bcmFieldQualifyRangeCheckBits24_31,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 3, 43, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 0, 43, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 1, 43, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 2, 43, 8);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf3, 8, 19, 20);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 8, 39, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 8, 39, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 8, 41, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 8, 43, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 8, 45, 2);

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
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
                 _bcmFieldSliceSelFpf1, 0, 201, 12);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf1, 1, 179, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf1, 1, 179, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf1, 1, 193, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf1, 1, 193, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 1, 207, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf1, 2, 179, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf1, 2, 179, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 2, 193, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 0, 196, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 1, 196, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 2, 196, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 2, 202, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 0, 202, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 1, 202, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 3, 202, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 3, 196, 6);
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
 *     _field_tr_lookup_qualifiers_init
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
_field_tr_lookup_qualifiers_init(int unit, _field_stage_t *stage_fc)
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

/*
 * Function:
 *     _field_tr_external_qualifiers_init
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
_field_tr_external_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
        
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 12, 48);
        
        
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
        
        
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 206, 6);
        
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
    
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 300, 48);
    
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
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6Low,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 0, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6Low,
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
 *     _field_tr_egress_qualifiers_init
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
_field_tr_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
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

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6Low,
        _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
        _bcmFieldSliceIp6AddrSelect,
        _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64, 14, 64);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6Low,
        _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
        _bcmFieldSliceIp6AddrSelect,
        _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64, 78, 64);
    
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
 *     _field_tr_qualifiers_init
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
_field_tr_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
          rv = _field_tr_ingress_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _field_tr_lookup_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _field_tr_egress_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_EXTERNAL:  
          rv = _field_tr_external_qualifiers_init(unit, stage_fc);
          break;
      default: 
          sal_free(stage_fc->f_qual_arr);
          return (BCM_E_PARAM);
    }
    return (rv);
}


/*
 * Function:
 *     _bcm_field_tr_external_init
 * Purpose:
 *     Initialize the External TCAM
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) (External) stage
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     IMPORTANT FPF_SZ_MAX should be >= stage_fc->tcam_slices
 *     Note the difference between size and size_with_pad. The latter is 
 *         used mainly for DMAing in counter collection
 *     The fields for all keys are from the perforce database:
 *         //depot/arch/include/triumph_regs/regsfile/arch/esm_formats.regs
 */
int
_bcm_field_tr_external_init(int unit, _field_stage_t *stage_fc)
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
        size = soc_mem_index_count(unit, _bcm_field_ext_policy_mems[i]);
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

    /* Initialize the profiles */
    /* 
     * redirect_profile will be used from IFP;
     *     both IFP and External TCAM share the same profile
     */
    mem = EXT_IFP_ACTION_PROFILEm;
    entry_words = sizeof(ext_ifp_action_profile_entry_t) / sizeof(uint32);
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

/*
 * Function:
 *     _bcm_field_tr_init
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
_bcm_field_tr_init(int unit, _field_control_t *fc) 
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
        BCM_IF_ERROR_RETURN(_field_tr_qualifiers_init(unit, stage_fc));

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
    _field_tr_functions_init(&fc->functions);;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_tr_detach
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
int
_bcm_field_tr_detach(int unit, _field_control_t *fc)
{
    _field_stage_t   *stage_p;  /* Stage iteration pointer. */

    stage_p = fc->stages;

    while (stage_p) { 
       /* Clear the hardware tables */
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
            BCM_IF_ERROR_RETURN(_bcm_field_tr3_hw_clear(unit, stage_p));
        } else
#endif
            {
                BCM_IF_ERROR_RETURN(_bcm_field_tr_hw_clear(unit, stage_p));
            }
      
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
 *     _field_tr_write_slice_map_ingress
 *
 * Purpose:
 *     Write the FP_SLICE_MAP (INGRESS)
 *
 * Parameters:
 *     unit
 *     stage_fc - pointer to stage control blocki
 *
 * Returns:
 *     nothing
 *     
 * Notes:
 */
STATIC int
_field_tr_write_slice_map_ingress(int unit, _field_stage_t *stage_fc)
{
    fp_slice_map_entry_t map_entry;  /* HW entry buffer.         */
    soc_field_t field;               /* HW entry fields.         */
    int vmap_size;                   /* Virtual map index count. */
    uint32 value;                    /* Field entry value.       */
    int map_id;                      /* L2/IP4/IP6 map id.       */
    int idx;                         /* Map fields iterator.     */     
    int rv;                          /* Operation return status. */
    uint32 virtual_to_physical_map_tr[3][17] = {
        {VIRTUAL_SLICE_0_PHYSICAL_SLICE_NUMBER_ENTRY_0f,
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
            VIRTUAL_SLICE_16_PHYSICAL_SLICE_NUMBER_ENTRY_0f},
        {VIRTUAL_SLICE_0_PHYSICAL_SLICE_NUMBER_ENTRY_1f,
            VIRTUAL_SLICE_1_PHYSICAL_SLICE_NUMBER_ENTRY_1f,
            VIRTUAL_SLICE_2_PHYSICAL_SLICE_NUMBER_ENTRY_1f,
            VIRTUAL_SLICE_3_PHYSICAL_SLICE_NUMBER_ENTRY_1f,
            VIRTUAL_SLICE_4_PHYSICAL_SLICE_NUMBER_ENTRY_1f,
            VIRTUAL_SLICE_5_PHYSICAL_SLICE_NUMBER_ENTRY_1f,
            VIRTUAL_SLICE_6_PHYSICAL_SLICE_NUMBER_ENTRY_1f,
            VIRTUAL_SLICE_7_PHYSICAL_SLICE_NUMBER_ENTRY_1f,
            VIRTUAL_SLICE_8_PHYSICAL_SLICE_NUMBER_ENTRY_1f,
            VIRTUAL_SLICE_9_PHYSICAL_SLICE_NUMBER_ENTRY_1f,
            VIRTUAL_SLICE_10_PHYSICAL_SLICE_NUMBER_ENTRY_1f,
            VIRTUAL_SLICE_11_PHYSICAL_SLICE_NUMBER_ENTRY_1f,
            VIRTUAL_SLICE_12_PHYSICAL_SLICE_NUMBER_ENTRY_1f,
            VIRTUAL_SLICE_13_PHYSICAL_SLICE_NUMBER_ENTRY_1f,
            VIRTUAL_SLICE_14_PHYSICAL_SLICE_NUMBER_ENTRY_1f,
            VIRTUAL_SLICE_15_PHYSICAL_SLICE_NUMBER_ENTRY_1f,
            VIRTUAL_SLICE_16_PHYSICAL_SLICE_NUMBER_ENTRY_1f},
        {VIRTUAL_SLICE_0_PHYSICAL_SLICE_NUMBER_ENTRY_2f,
            VIRTUAL_SLICE_1_PHYSICAL_SLICE_NUMBER_ENTRY_2f,
            VIRTUAL_SLICE_2_PHYSICAL_SLICE_NUMBER_ENTRY_2f,
            VIRTUAL_SLICE_3_PHYSICAL_SLICE_NUMBER_ENTRY_2f,
            VIRTUAL_SLICE_4_PHYSICAL_SLICE_NUMBER_ENTRY_2f,
            VIRTUAL_SLICE_5_PHYSICAL_SLICE_NUMBER_ENTRY_2f,
            VIRTUAL_SLICE_6_PHYSICAL_SLICE_NUMBER_ENTRY_2f,
            VIRTUAL_SLICE_7_PHYSICAL_SLICE_NUMBER_ENTRY_2f,
            VIRTUAL_SLICE_8_PHYSICAL_SLICE_NUMBER_ENTRY_2f,
            VIRTUAL_SLICE_9_PHYSICAL_SLICE_NUMBER_ENTRY_2f,
            VIRTUAL_SLICE_10_PHYSICAL_SLICE_NUMBER_ENTRY_2f,
            VIRTUAL_SLICE_11_PHYSICAL_SLICE_NUMBER_ENTRY_2f,
            VIRTUAL_SLICE_12_PHYSICAL_SLICE_NUMBER_ENTRY_2f,
            VIRTUAL_SLICE_13_PHYSICAL_SLICE_NUMBER_ENTRY_2f,
            VIRTUAL_SLICE_14_PHYSICAL_SLICE_NUMBER_ENTRY_2f,
            VIRTUAL_SLICE_15_PHYSICAL_SLICE_NUMBER_ENTRY_2f,
            VIRTUAL_SLICE_16_PHYSICAL_SLICE_NUMBER_ENTRY_2f}
    };
    uint32 virtual_to_group_map_tr[3][17] = {
        {VIRTUAL_SLICE_0_VIRTUAL_SLICE_GROUP_ENTRY_0f,
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
            VIRTUAL_SLICE_16_VIRTUAL_SLICE_GROUP_ENTRY_0f},
        {VIRTUAL_SLICE_0_VIRTUAL_SLICE_GROUP_ENTRY_1f,
            VIRTUAL_SLICE_1_VIRTUAL_SLICE_GROUP_ENTRY_1f,
            VIRTUAL_SLICE_2_VIRTUAL_SLICE_GROUP_ENTRY_1f,
            VIRTUAL_SLICE_3_VIRTUAL_SLICE_GROUP_ENTRY_1f,
            VIRTUAL_SLICE_4_VIRTUAL_SLICE_GROUP_ENTRY_1f,
            VIRTUAL_SLICE_5_VIRTUAL_SLICE_GROUP_ENTRY_1f,
            VIRTUAL_SLICE_6_VIRTUAL_SLICE_GROUP_ENTRY_1f,
            VIRTUAL_SLICE_7_VIRTUAL_SLICE_GROUP_ENTRY_1f,
            VIRTUAL_SLICE_8_VIRTUAL_SLICE_GROUP_ENTRY_1f,
            VIRTUAL_SLICE_9_VIRTUAL_SLICE_GROUP_ENTRY_1f,
            VIRTUAL_SLICE_10_VIRTUAL_SLICE_GROUP_ENTRY_1f,
            VIRTUAL_SLICE_11_VIRTUAL_SLICE_GROUP_ENTRY_1f,
            VIRTUAL_SLICE_12_VIRTUAL_SLICE_GROUP_ENTRY_1f,
            VIRTUAL_SLICE_13_VIRTUAL_SLICE_GROUP_ENTRY_1f,
            VIRTUAL_SLICE_14_VIRTUAL_SLICE_GROUP_ENTRY_1f,
            VIRTUAL_SLICE_15_VIRTUAL_SLICE_GROUP_ENTRY_1f,
            VIRTUAL_SLICE_16_VIRTUAL_SLICE_GROUP_ENTRY_1f},
        {VIRTUAL_SLICE_0_VIRTUAL_SLICE_GROUP_ENTRY_2f,
            VIRTUAL_SLICE_1_VIRTUAL_SLICE_GROUP_ENTRY_2f,
            VIRTUAL_SLICE_2_VIRTUAL_SLICE_GROUP_ENTRY_2f,
            VIRTUAL_SLICE_3_VIRTUAL_SLICE_GROUP_ENTRY_2f,
            VIRTUAL_SLICE_4_VIRTUAL_SLICE_GROUP_ENTRY_2f,
            VIRTUAL_SLICE_5_VIRTUAL_SLICE_GROUP_ENTRY_2f,
            VIRTUAL_SLICE_6_VIRTUAL_SLICE_GROUP_ENTRY_2f,
            VIRTUAL_SLICE_7_VIRTUAL_SLICE_GROUP_ENTRY_2f,
            VIRTUAL_SLICE_8_VIRTUAL_SLICE_GROUP_ENTRY_2f,
            VIRTUAL_SLICE_9_VIRTUAL_SLICE_GROUP_ENTRY_2f,
            VIRTUAL_SLICE_10_VIRTUAL_SLICE_GROUP_ENTRY_2f,
            VIRTUAL_SLICE_11_VIRTUAL_SLICE_GROUP_ENTRY_2f,
            VIRTUAL_SLICE_12_VIRTUAL_SLICE_GROUP_ENTRY_2f,
            VIRTUAL_SLICE_13_VIRTUAL_SLICE_GROUP_ENTRY_2f,
            VIRTUAL_SLICE_14_VIRTUAL_SLICE_GROUP_ENTRY_2f,
            VIRTUAL_SLICE_15_VIRTUAL_SLICE_GROUP_ENTRY_2f,
            VIRTUAL_SLICE_16_VIRTUAL_SLICE_GROUP_ENTRY_2f}
    };

    /* Calculate virtual map size. */
    rv = _bcm_field_virtual_map_size_get(unit, stage_fc, &vmap_size); 
    BCM_IF_ERROR_RETURN(rv);

    rv = READ_FP_SLICE_MAPm(unit, MEM_BLOCK_ANY, 0, &map_entry);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < vmap_size; idx++) {
        for (map_id = 0; map_id < _FP_VMAP_CNT; map_id++) {
            value = stage_fc->vmap[_FP_DEF_INST][map_id][idx].vmap_key;
            field = virtual_to_physical_map_tr[map_id][idx];
            soc_FP_SLICE_MAPm_field32_set(unit, &map_entry, field, value);

            value = stage_fc->vmap[_FP_DEF_INST][map_id][idx].virtual_group;
            field = virtual_to_group_map_tr[map_id][idx];
            soc_FP_SLICE_MAPm_field32_set(unit, &map_entry, field, value);
        }
    }

    rv = WRITE_FP_SLICE_MAPm(unit, MEM_BLOCK_ALL, 0, &map_entry);
    return (rv);
}

/*
 * Function:
 *     _bcm_field_tr_write_slice_map
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
int
_bcm_field_tr_write_slice_map(int unit, _field_stage_t *stage_fc,
                              _field_group_t *fg)
{
    int rv = BCM_E_PARAM;   /* Operation return status. */

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        rv = _field_tr_write_slice_map_ingress(unit, stage_fc);
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EGRESS) {
        rv = _bcm_field_trx_write_slice_map_egress(unit, stage_fc);
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_LOOKUP) {
        rv = _bcm_field_trx_write_slice_map_vfp(unit, stage_fc);
    }
    return (rv);
}

int
_bcm_field_trx_global_meter_set(int unit, _field_entry_t *f_ent,
                                soc_mem_t policy_mem, uint32 *buf)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_GREYHOUND2_SUPPORT)
    int offset_mode = 0;
    int index = 0;
#endif /* BCM_KATANA_SUPPORT || BCM_TRIUMPH3_SUPPORT ||
          BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
    /* Extract global meter policer fields */
        if (f_ent->global_meter_policer.pid > 0) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_policer_offset_mode_get(
                    unit, f_ent->global_meter_policer.pid, &offset_mode));
            if (soc_mem_field_valid(unit, policy_mem, SVC_METER_OFFSET_MODEf)) {
                soc_mem_field32_set(
                    unit, policy_mem, buf, SVC_METER_OFFSET_MODEf, offset_mode);
            }
            _bcm_esw_get_policer_table_index(
                unit, f_ent->global_meter_policer.pid, &index);
            if (soc_mem_field_valid(unit, policy_mem, SVC_METER_INDEXf)) {
                soc_mem_field32_set(
                    unit, policy_mem, buf, SVC_METER_INDEXf, index);
            }
        }
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_KATANA2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit)
            || SOC_IS_APACHE(unit)) {
    /* Extract global meter policer fields */
        if (f_ent->global_meter_policer.pid > 0) {
            offset_mode = ((f_ent->global_meter_policer.pid &
                                  BCM_POLICER_GLOBAL_METER_MODE_MASK) >>
                                  BCM_POLICER_GLOBAL_METER_MODE_SHIFT);
            if (offset_mode >= 1) {
                offset_mode = offset_mode - 1;
            }
            if (soc_mem_field_valid(unit, policy_mem, SVC_METER_OFFSET_MODEf)) {
                soc_mem_field32_set(unit, policy_mem, buf, SVC_METER_OFFSET_MODEf,
                                                                offset_mode);
            }
            _bcm_esw_get_policer_table_index(unit,
                                      f_ent->global_meter_policer.pid, &index);
            if (soc_mem_field_valid(unit, policy_mem, SVC_METER_INDEXf)) {
                soc_mem_field32_set(unit, policy_mem, buf, SVC_METER_INDEXf, index);
            }
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT) || \
    defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANA2(unit)
                    || SOC_IS_APACHE(unit)) {
                if (soc_mem_field32_get(unit,
                                   policy_mem, buf, ACTION_PRI_MODIFIERf) == 0 ) {
                    soc_mem_field32_set(unit, policy_mem, buf,
                                   SVC_METER_INDEX_PRIORITYf,
                                  (f_ent->global_meter_policer.flags) & 0x1);
                }
            } else
#endif
            {
                soc_mem_field32_set(unit, policy_mem, buf,
                                 SVC_METER_INDEX_PRIORITYf,
                                   (f_ent->global_meter_policer.flags) & 0x1);
            }
        }
    }
#endif /* BCM_KATANA_SUPPORT  or BCM_TRIUMPH3_SUPPORT*/

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_tr_entry_reinstall
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
int
_bcm_field_tr_entry_reinstall(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    uint32  e[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to fill Policy & TCAM entry.*/
    soc_mem_t        tcam_mem;          /* TCAM memory id.                    */
    soc_mem_t        policy_mem;        /* Policy table memory id .           */
    int              rv;                /* Operation return status.           */
    _field_action_t *fa = NULL;

    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    if (NULL == f_ent->fs) {
        return (BCM_E_PARAM);
    }

    rv = _field_fb_tcam_policy_mem_get(unit, f_ent->group->stage_id, 
                                       &tcam_mem, &policy_mem);
    BCM_IF_ERROR_RETURN(rv);

    if ((tcam_idx < soc_mem_index_min(unit, tcam_mem)) ||
        (tcam_idx > soc_mem_index_max(unit, tcam_mem))) {
        return (BCM_E_PARAM);
    }

    /* Handle the actions (policy) */
    /* Start with an empty POLICY entry */
    sal_memset(e, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Extract the policy info from the entry structure. */
    for (fa = f_ent->actions; fa != NULL;fa = fa->next) {
        /* Skip the invalid entries */
        if (_FP_ACTION_VALID & fa->flags) {
            rv = _bcm_field_trx_action_get(unit, policy_mem, f_ent, 
                                           tcam_idx, fa, e);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    /* Handle color dependence/independence */
    if (soc_mem_field_valid(unit, policy_mem, GREEN_TO_PIDf)) {
        soc_mem_field32_set(unit, policy_mem, e, GREEN_TO_PIDf, 
                            (f_ent->flags & _FP_ENTRY_COLOR_INDEPENDENT) ? 1 : 0);
    }

    /* Extract meter related policy fields */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        rv = _bcm_field_tr3_policer_action_set(unit, f_ent, policy_mem, e);
    } else
#endif
        {
            rv = _bcm_field_trx_policer_action_set(unit, f_ent, policy_mem, e);
        }
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_field_trx_global_meter_set(unit, f_ent, policy_mem, e);
    BCM_IF_ERROR_RETURN(rv);

    /* Extract counter related policy fields */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        rv = _bcm_field_tr3_stat_action_set(unit, f_ent, policy_mem,
                                            tcam_idx, e);
    } else
#endif
    {
        rv = _bcm_field_trx_stat_action_set(unit, f_ent, policy_mem,
                                            tcam_idx, e);
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        if (SOC_IS_TD2P_TT2P(unit) || SOC_IS_APACHE(unit)) {
            rv = _bcm_field_td2plus_extended_stat_action_set (unit, f_ent, policy_mem,
                                                                tcam_idx, e);
        }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    }
    BCM_IF_ERROR_RETURN(rv);

#if defined BCM_TRIDENT2PLUS_SUPPORT
    if (soc_feature(unit, soc_feature_fp_based_oam)) {
        rv = _bcm_field_oam_stat_action_set (unit, f_ent, policy_mem, e);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

    /* Write the POLICY Table */
    rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, tcam_idx, e);
    BCM_IF_ERROR_RETURN(rv);
   
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_tr_entry_install
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
int
_bcm_field_tr_entry_install(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    uint32  e[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to fill Policy & TCAM entry.*/
    soc_mem_t        tcam_mem;          /* TCAM memory id.                    */
    soc_mem_t        policy_mem;        /* Policy table memory id .           */
    int              rv;                /* Operation return status.           */
    _field_action_t *fa = NULL;
    fp_global_mask_tcam_entry_t ipbm_entry;
#ifdef BCM_TRIDENT_SUPPORT
    bcm_pbmp_t pbmp_x;
    bcm_pbmp_t pbmp_y;
    fp_global_mask_tcam_x_entry_t ipbm_entry_x;
    fp_global_mask_tcam_y_entry_t ipbm_entry_y;
#endif

    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    if (NULL == f_ent->fs) {
        return (BCM_E_PARAM);
    }

    sal_memset(&ipbm_entry, 0, sizeof(fp_global_mask_tcam_entry_t));

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_field_multi_pipe_support)) {
       rv = _bcm_field_th_tcam_policy_mem_get(unit, f_ent,
                                       &tcam_mem, &policy_mem);
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
    {
       rv = _field_fb_tcam_policy_mem_get(unit, f_ent->group->stage_id, 
                                       &tcam_mem, &policy_mem);
    }
    BCM_IF_ERROR_RETURN(rv);

    if ((tcam_idx < soc_mem_index_min(unit, tcam_mem)) ||
        (tcam_idx > soc_mem_index_max(unit, tcam_mem))) {
        return (BCM_E_PARAM);
    }

    /* First Erase the TCAM: Valid bits = b00 */
    sal_memset(e, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx, e);
    BCM_IF_ERROR_RETURN(rv);

    /* Handle the actions (policy) */
    /* Start with an empty POLICY entry */
    sal_memset(e, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    /* Extract the policy info from the entry structure. */
    for (fa = f_ent->actions; fa != NULL; fa = fa->next) {
        /* Skip the invalid entries */
        if (_FP_ACTION_VALID & fa->flags) {
            rv = _bcm_field_trx_action_get(unit, policy_mem, f_ent, 
                                           tcam_idx, fa, e);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    /* Handle color dependence/independence */
    if (soc_mem_field_valid(unit, policy_mem, GREEN_TO_PIDf)) {
        soc_mem_field32_set(unit, policy_mem, e, GREEN_TO_PIDf, 
                            (f_ent->flags & _FP_ENTRY_COLOR_INDEPENDENT) ? 1 : 0);
    }

    /* Extract meter related policy fields */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        rv = _bcm_field_tr3_policer_action_set(unit, f_ent, policy_mem, e);
    } else
#endif
    {
        rv = _bcm_field_trx_policer_action_set(unit, f_ent, policy_mem, e);
    }
    BCM_IF_ERROR_RETURN(rv);

    /* Set global service meter */
    rv = _bcm_field_trx_global_meter_set(unit, f_ent, policy_mem, e);
    BCM_IF_ERROR_RETURN(rv);

    /* Extract counter related policy fields */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        rv = _bcm_field_tr3_stat_action_set(unit,
                                            f_ent,
                                            policy_mem,
                                            tcam_idx,
                                            e);
    } else
#endif
    {
        rv = _bcm_field_trx_stat_action_set(unit,
                                            f_ent,
                                            policy_mem,
                                            tcam_idx,
                                            e);
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        if (SOC_IS_TD2P_TT2P(unit) || SOC_IS_APACHE(unit)) {
            rv = _bcm_field_td2plus_extended_stat_action_set (unit, f_ent, policy_mem,
                    tcam_idx, e);
        }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    }
    BCM_IF_ERROR_RETURN(rv);

#if defined BCM_TRIDENT2PLUS_SUPPORT
    if (soc_feature(unit, soc_feature_fp_based_oam)) {
        rv = _bcm_field_oam_stat_action_set (unit, f_ent, policy_mem, e);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

    /* Write the POLICY Table */
    rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, tcam_idx, e);
    BCM_IF_ERROR_RETURN(rv);

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        /* Write qualifiers to TCAM */

        return (_bcm_field_td2_qual_tcam_key_mask_set(unit, f_ent, TRUE));
    } else
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        /* Write qualifiers to TCAM */

        return (_bcm_field_tr3_qual_tcam_key_mask_set(unit, f_ent, TRUE));
    }
#endif

#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            /* Write qualifiers to TCAM */
            return (_bcm_field_kt2_qual_tcam_key_mask_set(unit, f_ent, TRUE));
        }
#endif


    /* 
     * Write the FP_GLOBAL_MASK_TCAM table if necessary 
     *     For wide/paired mode entries, f_ent->pbmp will be valid ONLY
     * for one of the entries. For others it will be zero. f_ent->pbmp.mask
     * will take care of this.
     * (But for Trident, this may contain qualifiers in the secondary entry)
     */
    if (f_ent->fs->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        if (BCM_FIELD_QSET_TEST(f_ent->group->qset, bcmFieldQualifyInPort) ||
            BCM_FIELD_QSET_TEST(f_ent->group->qset, bcmFieldQualifyInPorts)) {
            soc_mem_field_set(unit, FP_GLOBAL_MASK_TCAMm, (uint32 *)&ipbm_entry,
                          IPBMf, (uint32 *)&(f_ent->pbmp.data));
            soc_mem_field_width_fit_set(unit, FP_GLOBAL_MASK_TCAMm, 
                                    (uint32 *)&ipbm_entry, IPBM_MASKf, 
                                    (uint32 *)&(f_ent->pbmp.mask));
            rv = WRITE_FP_GLOBAL_MASK_TCAMm(unit,
                    MEM_BLOCK_ALL, tcam_idx, &ipbm_entry);
            BCM_IF_ERROR_RETURN(rv);
        }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        /* For Katana, the VALID field of FP_GLOBAL_MASK_TCAM must be set,
         * even if IPBM and IPBM_MASK are all zeroes.
         */
        if (SOC_IS_KATANAX(unit) || SOC_IS_HURRICANE2(unit) ||
            SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
            SOC_IS_GREYHOUND2(unit)) {
            if (soc_mem_field_valid(unit, FP_GLOBAL_MASK_TCAMm, VALIDf)) {
                soc_mem_field32_set(unit, FP_GLOBAL_MASK_TCAMm,
                    (uint32 *)&ipbm_entry, VALIDf, SOC_IS_KATANA2(unit) ? 3: 1);
                rv = WRITE_FP_GLOBAL_MASK_TCAMm(unit,
                        MEM_BLOCK_ALL, tcam_idx, &ipbm_entry);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
#endif
    } 

    /* For Trident, the VALID field of FP_GLOBAL_MASK_TCAM must be set,
     * even if IPBM and IPBM_MASK are all zeroes.
     */
#if defined( BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TD_TT (unit)) {
        if (soc_mem_field_valid(unit, FP_GLOBAL_MASK_TCAMm, VALIDf)
            && (f_ent->fs->stage_id == _BCM_FIELD_STAGE_INGRESS)) {

            BCM_PBMP_CLEAR(pbmp_x);
            BCM_PBMP_CLEAR(pbmp_y);
            sal_memset(&ipbm_entry_x, 0, sizeof(fp_global_mask_tcam_x_entry_t));
            sal_memset(&ipbm_entry_y, 0, sizeof(fp_global_mask_tcam_y_entry_t));

            rv = READ_FP_GLOBAL_MASK_TCAM_Xm(unit, MEM_BLOCK_ANY, tcam_idx,
                    &ipbm_entry_x);
            BCM_IF_ERROR_RETURN(rv);
            rv = READ_FP_GLOBAL_MASK_TCAM_Ym(unit, MEM_BLOCK_ANY, tcam_idx,
                    &ipbm_entry_y);
            BCM_IF_ERROR_RETURN(rv);

        soc_mem_pbmp_field_get(unit, FP_GLOBAL_MASK_TCAM_Xm, &ipbm_entry_x, IPBMf,
            &pbmp_x);
        soc_mem_pbmp_field_get(unit, FP_GLOBAL_MASK_TCAM_Ym, &ipbm_entry_y, IPBMf,
            &pbmp_y);

        BCM_PBMP_OR(pbmp_x, pbmp_y);
        soc_mem_pbmp_field_set(unit, FP_GLOBAL_MASK_TCAMm, &ipbm_entry, IPBMf,
            &pbmp_x);

        BCM_PBMP_CLEAR(pbmp_x);
        BCM_PBMP_CLEAR(pbmp_y);

        soc_mem_pbmp_field_get(unit, FP_GLOBAL_MASK_TCAM_Xm, &ipbm_entry_x, IPBM_MASKf,
            &pbmp_x);
        soc_mem_pbmp_field_get(unit, FP_GLOBAL_MASK_TCAM_Ym, &ipbm_entry_y, IPBM_MASKf,
            &pbmp_y);

        BCM_PBMP_OR(pbmp_x, pbmp_y);
        soc_mem_pbmp_field_set(unit, FP_GLOBAL_MASK_TCAMm, &ipbm_entry, 
            IPBM_MASKf, &pbmp_x); 
        soc_mem_field32_set(unit, FP_GLOBAL_MASK_TCAMm,
                            (uint32 *)&ipbm_entry, VALIDf, 1);

            rv = WRITE_FP_GLOBAL_MASK_TCAMm(unit, MEM_BLOCK_ALL, 
                    tcam_idx, &ipbm_entry);
            BCM_IF_ERROR_RETURN(rv);
        }
    }
#endif

    /* Handle the rules (TCAM) */
    /* Start with an empty TCAM entry */
    sal_memset(e, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    /* Extract the qualifier info from the entry structure. */
    rv = _bcm_field_trx_tcam_get(unit, tcam_mem, f_ent, e);
    BCM_IF_ERROR_RETURN(rv);
    /* Write the TCAM Table */
    rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx, e);
    BCM_IF_ERROR_RETURN(rv);

#ifdef BCM_TRIDENT_SUPPORT
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "Checking for IPBM overlay\n")));

    if ((SOC_IS_TD_TT(unit)) && (f_ent->flags & _FP_ENTRY_USES_IPBM_OVERLAY))
    {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "Overlay in use\n")));

        sal_memset(e, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

        BCM_IF_ERROR_RETURN(_bcm_field_trx_tcam_get(unit, FP_GM_FIELDSm,
            f_ent, e));

        BCM_IF_ERROR_RETURN(WRITE_FP_GM_FIELDSm(unit, MEM_BLOCK_ALL, tcam_idx,
            e));
    } else
#endif
    if ((SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_KATANAX(unit))
        && (f_ent->flags & _FP_ENTRY_USES_IPBM_OVERLAY)) {

        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "Overlay in use\n")));

        sal_memset(e, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

        BCM_IF_ERROR_RETURN(_bcm_field_trx_tcam_get(unit, FP_GLOBAL_MASK_TCAMm,
            f_ent, e));

        BCM_IF_ERROR_RETURN(WRITE_FP_GLOBAL_MASK_TCAMm(unit, MEM_BLOCK_ALL, tcam_idx,
            e));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_tr_external_counter_idx_get
 * Purpose:
 *     Calculate external tcam counter index.
 * Parameters:
 *     unit     - (IN) unit
 *     stage_fc - (IN) Field stage structure
 *     fg       - (IN) Field group structure
 * Returns: 
 *     BCM_E_RESOURCE - No select code will satisfy qualifier set
 *     BCM_E_NONE     - Success
 */
int
_bcm_field_tr_external_counter_idx_get(int unit, uint8  slice_number, 
                                        bcm_field_entry_t entry_index, 
                                        int  *idx)
{
    if (NULL == idx) {
        return (BCM_E_PARAM);
    }

    return soc_tcam_part_index_to_mem_index(unit,
                                            _bcm_field_fp_tcam_partitions[slice_number],
                                            entry_index, EXT_FP_CNTRm, idx);

}

/*
 * Function:
 *     _bcm_field_tr_external_slice_clear
 * Purpose:
 *     Remove an external ACL group
 * Parameters:
 *     unit     - (IN) unit
 *     fg       - (IN) Field group structure
 *     fs       - (IN) Field slice structure
 * Returns: 
 *     BCM_E_XXX
 */
int
_bcm_field_tr_external_slice_clear(int unit, _field_group_t *fg)
{
    bcm_port_t port;
    int        rv;

    if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp6)) {
        /* V6 group */
        BCM_PBMP_ITER(fg->pbmp, port) {
            rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port, 
                                        IPV6_ACL_MODEf, 0);
            BCM_IF_ERROR_RETURN(rv);
            
            rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                        IPV6_ACL_144_ENf, 0);
            BCM_IF_ERROR_RETURN(rv);

            rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                        IPV6_FULL_ACLf, 0);
            BCM_IF_ERROR_RETURN(rv);

        }
    } else if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4)) {
        /* V4 group */
        BCM_PBMP_ITER(fg->pbmp, port) {
            rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port, 
                                        IPV4_ACL_MODEf, 0);
            BCM_IF_ERROR_RETURN(rv);
            rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                        IPV4_ACL_144_ENf, 0);
            BCM_IF_ERROR_RETURN(rv);

            rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                        IPV4_ACL_TYPEf, 0);
            BCM_IF_ERROR_RETURN(rv);
        }
    } else {
        /* L2 group */
        BCM_PBMP_ITER(fg->pbmp, port) {
            rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port, 
                                        L2_ACL_ENf, 0);
            BCM_IF_ERROR_RETURN(rv);
            rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                        L2_ACL_144_ENf, 0);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_tr_external_tcam_write
 * Purpose:
 *     Writes an ACL h/w entry
 * Parameters:
 *     unit     - (IN) unit
 *     f_ent    - (IN) entry to be writted
 *     index    - (IN) index at which to be written
 * Returns: 
 *     BCM_E_NONE     - Success
 *     BCM_E_XXX
 * Notes:
 *     It is not necessary that f_ent->slice_idx == index
 *         e.g. if called from _bcm_field_tr_external_entry_move.
 *     The 1 bit field mentioned in various (9) ACL inits are not set
 *         (i.e. left as Don't Care),
 *         as we DONOT allow sharing of key types in same block.
 *     Also we have ESM_MODE_PER_PORTr.IPV4_ACL_TYPE and
 *                  ESM_MODE_PER_PORTr.IPV6_ACL_TYPE as 0 (port based ACL)
 */
int
_field_tr_external_tcam_write(int unit, _field_entry_t *f_ent, int index)
{
    uint32 buf[SOC_MAX_MEM_FIELD_WORDS];
    int slice;
    _field_tcam_t *tcam;
    int rv;

    tcam = &f_ent->tcam;

    slice = f_ent->fs->slice_number;
    switch (slice) {
      case _FP_EXT_ACL_144_L2:
      case _FP_EXT_ACL_L2:
      case _FP_EXT_ACL_144_IPV4:
      case _FP_EXT_ACL_IPV4:
      case _FP_EXT_ACL_144_IPV6:
          sal_memset(buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
          soc_mem_field_set(unit, _bcm_field_ext_data_mask_mems[slice], 
                            buf, DATAf, tcam->key_hw);
          /* 
           * External TCAM: mask polarity is reversed.
           *     Use this instead of soc_mem_field_set
           */
          soc_mem_mask_field_set(unit, _bcm_field_ext_data_mask_mems[slice], 
                                 buf, MASKf, tcam->mask_hw);

          rv = soc_mem_write(unit, _bcm_field_ext_data_mask_mems[slice], 
                             MEM_BLOCK_ALL, index, buf);
          BCM_IF_ERROR_RETURN(rv);
          break;
      case _FP_EXT_ACL_L2_IPV4:
      case _FP_EXT_ACL_IPV6_SHORT:
      case _FP_EXT_ACL_IPV6_FULL:
      case _FP_EXT_ACL_L2_IPV6:
          /* Write the mask */
          sal_memset(buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
          /* 
           * External TCAM: mask polarity is reversed.
           *     Use this instead of soc_mem_field_set
           */
          soc_mem_mask_field_set(unit, _bcm_field_ext_mask_mems[slice], 
                                 buf, MASKf, tcam->mask_hw);
          rv = soc_mem_write(unit, _bcm_field_ext_mask_mems[slice], 
                             MEM_BLOCK_ALL, 0, buf);
          BCM_IF_ERROR_RETURN(rv);

          /* Write the data */
          sal_memset(buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
          soc_mem_field_set(unit, _bcm_field_ext_data_mems[slice], 
                            buf, DATAf, tcam->key_hw);

          rv =  soc_mem_write(unit, _bcm_field_ext_data_mems[slice], 
                              MEM_BLOCK_ALL, index, buf);
          BCM_IF_ERROR_RETURN(rv);
          break;
      default:
          /* should not come here */
          return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_tr_external_policy_mem_get
 * Purpose:
 *     Get external policy table name.
 * Parameters:
 *     unit     - (IN) unit
 *     stage_fc - (IN) stage (External)
 *     f_ent    - (IN) Field entry to be installed
 * Returns: 
 *     BCM_E_XXX
 */
int
_bcm_field_tr_external_policy_mem_get(int unit, _field_action_t *fa, soc_mem_t *mem)
{
    /* Input parameters check. */
    if ((NULL == mem) || (NULL == fa)) {
        return (BCM_E_PARAM); 
    }

    switch (fa->action) {
        /* These actions are in a separate table */
        case bcmFieldActionPrioPktCopy:
        case bcmFieldActionRpPrioPktCopy:
        case bcmFieldActionYpPrioPktCopy:
        case bcmFieldActionGpPrioPktCopy:
        case bcmFieldActionPrioPktNew:
        case bcmFieldActionRpPrioPktNew:
        case bcmFieldActionYpPrioPktNew:
        case bcmFieldActionGpPrioPktNew:
        case bcmFieldActionPrioPktTos:
        case bcmFieldActionRpPrioPktTos:
        case bcmFieldActionYpPrioPktTos:
        case bcmFieldActionGpPrioPktTos:
        case bcmFieldActionPrioPktCancel:
        case bcmFieldActionRpPrioPktCancel:
        case bcmFieldActionYpPrioPktCancel:
        case bcmFieldActionGpPrioPktCancel:
        case bcmFieldActionPrioIntCopy:
        case bcmFieldActionRpPrioIntCopy:
        case bcmFieldActionYpPrioIntCopy:
        case bcmFieldActionGpPrioIntCopy:
        case bcmFieldActionPrioIntNew:
        case bcmFieldActionRpPrioIntNew:
        case bcmFieldActionYpPrioIntNew:
        case bcmFieldActionGpPrioIntNew:
        case bcmFieldActionPrioIntTos:
        case bcmFieldActionRpPrioIntTos:
        case bcmFieldActionYpPrioIntTos:
        case bcmFieldActionGpPrioIntTos:
        case bcmFieldActionPrioIntCancel:
        case bcmFieldActionRpPrioIntCancel:
        case bcmFieldActionYpPrioIntCancel:
        case bcmFieldActionGpPrioIntCancel:
        case bcmFieldActionPrioPktAndIntNew:
        case bcmFieldActionRpPrioPktAndIntNew:
        case bcmFieldActionYpPrioPktAndIntNew:
        case bcmFieldActionGpPrioPktAndIntNew:
        case bcmFieldActionPrioPktAndIntCopy:
        case bcmFieldActionRpPrioPktAndIntCopy:
        case bcmFieldActionYpPrioPktAndIntCopy:
        case bcmFieldActionGpPrioPktAndIntCopy:
        case bcmFieldActionPrioPktAndIntTos:
        case bcmFieldActionRpPrioPktAndIntTos:
        case bcmFieldActionYpPrioPktAndIntTos:
        case bcmFieldActionGpPrioPktAndIntTos:
        case bcmFieldActionPrioPktAndIntCancel:
        case bcmFieldActionRpPrioPktAndIntCancel:
        case bcmFieldActionYpPrioPktAndIntCancel:
        case bcmFieldActionGpPrioPktAndIntCancel:
        case bcmFieldActionDscpNew:
        case bcmFieldActionRpDscpNew:
        case bcmFieldActionYpDscpNew:
        case bcmFieldActionGpDscpNew:
        case bcmFieldActionDscpCancel:
        case bcmFieldActionRpDscpCancel:
        case bcmFieldActionYpDscpCancel:
        case bcmFieldActionGpDscpCancel:
        case bcmFieldActionCosQNew:
        case bcmFieldActionRpCosQNew:
        case bcmFieldActionYpCosQNew:
        case bcmFieldActionGpCosQNew:
        case bcmFieldActionVlanCosQNew:
        case bcmFieldActionRpVlanCosQNew:
        case bcmFieldActionYpVlanCosQNew:
        case bcmFieldActionGpVlanCosQNew:
        case bcmFieldActionGpTosPrecedenceCopy:
        case bcmFieldActionGpTosPrecedenceNew:
            *mem = EXT_IFP_ACTION_PROFILEm;
            break;
        default:
            *mem = EXT_FP_POLICYm;
    }
    return (BCM_E_NONE);
}
 
/*
 * Function:
 *     _field_tr_external_policy_install
 * Purpose:
 *     Installs an (external) policy entry into hardware.
 * Parameters:
 *     unit     - (IN) unit
 *     stage_fc - (IN) stage (External)
 *     f_ent    - (IN) Field entry to be installed
 * Returns: 
 *     BCM_E_NONE     - Success
 *     BCM_E_XXX
 */

int
_field_tr_external_policy_install(int unit, _field_stage_t *stage_fc, 
                                       _field_entry_t *f_ent)
{
    soc_mem_t policy_mem;
    uint32 policy[SOC_MAX_MEM_FIELD_WORDS];
    uint32 ext_act_profile_index;
    uint32 ext_act_profile[SOC_MAX_MEM_FIELD_WORDS];
    _field_action_t *fa;
    void *entries[1];
    int rv;


    /* Start with an empty POLICY entry */
    sal_memset(policy, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    sal_memset(ext_act_profile, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Extract the policy info from the entry structure. */
    for (fa = f_ent->actions; fa != NULL; fa = fa->next) {

       /* skip invalid actions. */
       if (!(_FP_ACTION_VALID & fa->flags)) {
           continue;
        }

        rv = _bcm_field_tr_external_policy_mem_get(unit, fa, &policy_mem);
        BCM_IF_ERROR_RETURN(rv);
        if (EXT_IFP_ACTION_PROFILEm == policy_mem) {
            rv = _bcm_field_trx_action_get(unit, policy_mem, f_ent, 
                                           f_ent->slice_idx, fa, 
                                           ext_act_profile);
            BCM_IF_ERROR_RETURN(rv);
        } else {
            rv = _bcm_field_trx_action_get(unit, policy_mem, f_ent, 
                                           f_ent->slice_idx, fa, policy);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    /* Setup the PROFILE_PTR for EXT_IFP_ACTION_PROFILEm */
    entries[0] = ext_act_profile;
    rv = soc_profile_mem_add(unit, &stage_fc->ext_act_profile, 
                             entries, 1, &ext_act_profile_index);
    BCM_IF_ERROR_RETURN(rv);

    f_ent->ext_act_profile_idx = ext_act_profile_index;

    /* Set slice dependent policy memory name. */
    policy_mem = _bcm_field_ext_policy_mems[f_ent->fs->slice_number];

    soc_mem_field32_set(unit, policy_mem, policy, PROFILE_PTRf, 
                        ext_act_profile_index);

    /* Handle color dependence/independence */
    soc_mem_field32_set(unit, policy_mem, policy, GREEN_TO_PIDf, 
                   (f_ent->flags & _FP_ENTRY_COLOR_INDEPENDENT) ? 1 : 0);

    /* Extract meter related policy fields */
    rv = _bcm_field_trx_policer_action_set(unit, f_ent, policy_mem, policy);
    BCM_IF_ERROR_RETURN(rv);

    /* Extract counter related policy fields */
    rv = _bcm_field_trx_stat_action_set(unit, f_ent, policy_mem,
                                        f_ent->slice_idx, policy);
    BCM_IF_ERROR_RETURN(rv);

    /* Set policy entry valid bit. */
    soc_mem_field32_set(unit, policy_mem, policy, VALIDf, 1 );

    /* Write into the Table */
    rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, 
                       f_ent->slice_idx, policy);
    BCM_IF_ERROR_RETURN(rv);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr_external_counter_reset
 * Purpose:
 *     Initializes the external-FP counter to 0
 * Parameters:
 *     unit     - (IN) unit
 *     stage_fc - (IN) Stage structure
 *     f_ent    - (IN) Field entry to be installed
 * Returns: 
 *     BCM_E_NONE     - Success
 *     BCM_E_XXX
 */
int
_field_tr_external_counter_reset(int unit, _field_stage_t *stage_fc, 
                                      _field_entry_t *f_ent)
{
    int index;
    ext_fp_cntr_entry_t ext_fp_cntr;
    _field_stat_t  *f_st; 
    int rv; 

    if (f_ent->statistic.flags & _FP_ENTRY_STAT_INSTALLED) {
        rv = _bcm_field_stat_get(unit, f_ent->statistic.sid, &f_st); 
        BCM_IF_ERROR_RETURN(rv);
        f_st->hw_index = f_ent->slice_idx;
    }

    /* Get absolute index */
    BCM_IF_ERROR_RETURN(soc_tcam_part_index_to_mem_index(unit, 
                            _bcm_field_fp_tcam_partitions[f_ent->fs->slice_number], 
                            f_ent->slice_idx, EXT_FP_CNTRm, &index));

    sal_memcpy(&ext_fp_cntr, soc_mem_entry_null(unit, EXT_FP_CNTRm), 
               soc_mem_entry_words(unit, EXT_FP_CNTRm) * sizeof(uint32)); 
    SOC_IF_ERROR_RETURN(
        soc_mem_write(unit, EXT_FP_CNTRm, MEM_BLOCK_ALL, index, &ext_fp_cntr));

    /* Clear the counter collection S/W */
    sal_memset(&stage_fc->_field_x32_counters[f_ent->slice_idx], 0, 
                   sizeof(_field_counter32_collect_t));
    sal_memset(&stage_fc->_field_x64_counters[f_ent->slice_idx], 0, 
                   sizeof(_field_counter64_collect_t));
    
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_tr_external_entry_clear
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
_field_tr_external_entry_clear(int unit, int slice, int index)
{
    int rv; 
    uint32 buf[SOC_MAX_MEM_FIELD_WORDS];

    sal_memset(buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    switch (slice) {
      case _FP_EXT_ACL_144_L2:
      case _FP_EXT_ACL_L2:
      case _FP_EXT_ACL_144_IPV4:
      case _FP_EXT_ACL_IPV4: 
      case _FP_EXT_ACL_144_IPV6:
          rv = soc_mem_write(unit, _bcm_field_ext_data_mask_mems[slice], 
                             MEM_BLOCK_ALL, index, buf);
          BCM_IF_ERROR_RETURN(rv);
          break;
      case _FP_EXT_ACL_L2_IPV4:
      case _FP_EXT_ACL_IPV6_SHORT:
      case _FP_EXT_ACL_IPV6_FULL:
      case _FP_EXT_ACL_L2_IPV6:
          /* Write the mask */
          rv =  soc_mem_write(unit, _bcm_field_ext_mask_mems[slice], 
                              MEM_BLOCK_ALL, 0, buf);
          BCM_IF_ERROR_RETURN(rv);
          /* Write the data */
          rv = soc_mem_write(unit, _bcm_field_ext_data_mems[slice], 
                             MEM_BLOCK_ALL, index, buf);
          BCM_IF_ERROR_RETURN(rv);
          break;
      default:
          /* should not come here */
          return (BCM_E_INTERNAL);
    }
    /* Clear policy entry */
    rv =  soc_mem_write(unit, _bcm_field_ext_policy_mems[slice],
                        MEM_BLOCK_ALL, index, buf);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_tr_external_entry_reinstall
 * Purpose:
 *     Re-Installs an (external) ACL entry into hardware.
 * Parameters:
 *     unit     - (IN) unit
 *     f_ent    - (IN) Field entry to be installed
 * Returns: 
 *     BCM_E_NONE     - Success
 *     BCM_E_XXX
 */
int
_bcm_field_tr_external_entry_reinstall(int unit, _field_entry_t *f_ent)
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

    /* Write EXT_FP_CNTR to 0 */
    BCM_IF_ERROR_RETURN(_field_tr_external_counter_reset(unit, 
                stage_fc, f_ent));

    /* Write EXT_FP_POLICY */
    BCM_IF_ERROR_RETURN
        (_field_tr_external_policy_install(unit, stage_fc, f_ent));

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_delete(unit, &ext_act_profile, ext_act_profile_index));


    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_tr_external_entry_install
 * Purpose:
 *     Installs an (external) ACL entry into hardware.
 * Parameters:
 *     unit     - (IN) unit
 *     f_ent    - (IN) Field entry to be installed
 * Returns: 
 *     BCM_E_NONE     - Success
 *     BCM_E_XXX
 */
int
_bcm_field_tr_external_entry_install(int unit, _field_entry_t *f_ent)
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

    /* Write EXT_FP_CNTR to 0 */
    BCM_IF_ERROR_RETURN(_field_tr_external_counter_reset(unit, 
                stage_fc, f_ent));

    /* Write EXT_FP_POLICY */
    _field_tr_external_policy_install(unit, stage_fc, f_ent);

    /* Write TCAM */
    _field_tr_external_tcam_write(unit, f_ent, f_ent->slice_idx);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_tr_external_entry_remove
 * Purpose:
 *     Removes an (external) ACL entry from hardware.
 * Parameters:
 *     unit     - (IN) unit
 *     f_ent    - (IN) Field entry to be removed
 * Returns: 
 *     BCM_E_NONE     - Success
 * Notes:
 *      This also zeros out the H/W replica,
 *          and removes the entry from the slice's entry list.
 *      Now the entry only exists in the hash list.
 */
int
_bcm_field_tr_external_entry_remove(int unit, _field_entry_t *f_ent)
{
    uint32 policy[SOC_MAX_MEM_FIELD_WORDS];
    _field_stage_t *stage_fc;
    soc_mem_t policy_mem;
    uint32 profile_idx; 
    int rv;
    
    /* Get stage field control. */
    rv = _field_stage_control_get(unit, f_ent->fs->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    policy_mem = _bcm_field_ext_policy_mems[f_ent->fs->slice_number];

    /* Read EXT_FP_POLICY from index1 */
    rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY, 
                      f_ent->slice_idx, policy);
    BCM_IF_ERROR_RETURN(rv);
   
    profile_idx = soc_mem_field32_get(unit, policy_mem, policy, PROFILE_PTRf);
    /* Release the PROFILE_PTR for EXT_IFP_ACTION_PROFILEm */
    rv = soc_profile_mem_delete(unit, &stage_fc->ext_act_profile, profile_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear the H/W entry */
    _field_tr_external_entry_clear(unit, f_ent->fs->slice_number, 
                                              f_ent->slice_idx);
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
 *     _bcm_field_tr_external_entry_move
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
_bcm_field_tr_external_entry_move(int unit, _field_entry_t *f_ent,
                                       int index1, int index2)
{
    _field_stage_t *stage_fc;           /* Stage information */
    _field_stat_t *f_st;                /* Stats information */
    _field_stat_t f_st_old;             /* Old Stats information */
    uint32  policy[SOC_MAX_MEM_FIELD_WORDS]; /* policy info */
    soc_mem_t policy_mem;               /* policy table memory */
    int counter_index1, counter_index2; /* New and old Hw Cntr index */
    int rv;                             /* Operation return status */ 

    /* Check if entry move is required. */
    if (index1 == index2) {
        return (BCM_E_NONE);
    }

    rv = _field_stage_control_get(unit, f_ent->fs->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear TCAM at index2 */
    _field_tr_external_entry_clear(unit, f_ent->fs->slice_number, index2);

    policy_mem = _bcm_field_ext_policy_mems[f_ent->fs->slice_number];
    /* Read EXT_FP_POLICY from index1 */
    rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY, index1, policy);
    BCM_IF_ERROR_RETURN(rv);

    /* Write EXT_FP_POLICY to index2 */
    rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, index2, policy);
    BCM_IF_ERROR_RETURN(rv);

    /* Write TCAM at index2 */
    _field_tr_external_tcam_write(unit, f_ent, index2);

    /* Clear TCAM at index1 */
    _field_tr_external_entry_clear(unit, f_ent->fs->slice_number, index1);

    /* Move the counters */
    rv = soc_tcam_part_index_to_mem_index(unit, 
                                          _bcm_field_fp_tcam_partitions[f_ent->fs->slice_number],
                                          index1, EXT_FP_CNTRm, &counter_index1);
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_tcam_part_index_to_mem_index(unit, 
                                          _bcm_field_fp_tcam_partitions[f_ent->fs->slice_number], 
                                          index2, EXT_FP_CNTRm, &counter_index2);


    /* Update the software */
    f_ent->fs->entries[index1] = NULL;
    f_ent->slice_idx = index2;
    if (f_ent->statistic.flags & _FP_ENTRY_STAT_INSTALLED) {
        rv = _bcm_field_stat_get(unit, f_ent->statistic.sid, &f_st);
        BCM_IF_ERROR_RETURN(rv);

        /* Save old Stat info */
        f_st_old = *f_st;

        f_st->hw_index = counter_index2;

        /* Perform counter move operation */
        rv = _bcm_field_entry_counter_move(unit, stage_fc, 
                                           f_ent->fs->slice_number,
                                           counter_index1, &f_st_old, f_st);
        BCM_IF_ERROR_RETURN(rv);

        f_st->hw_index = index2;
    }
    f_ent->fs->entries[index2] = f_ent;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_tr_external_entry_prio_set
 * Purpose:
 *     Sets the priority of an external entry
 * Parameters:
 *     unit     - (IN) unit
 *     f_ent    - (IN) Field entry to be removed
 *     prio     - (IN) new priority for the entry
 * Returns: 
 *     BCM_E_NONE     - Success
 *     BCM_E_XXX
 * Notes:
 *     1. If not installed, just update the priority
 *     2. Find the target location (this may move the entry)
 *     3. Move the entry to target location
 *     4. Update the prio_mgmt structure
 */
int
_bcm_field_tr_external_entry_prio_set(int unit, _field_entry_t *f_ent, 
                                       int prio)
{
    _field_stage_t *stage_fc;
    uint32 new_location;
    uint32 old_location;

    /* If entry is not installed, just update the priority */
    if (!(f_ent->flags & _FP_ENTRY_INSTALLED)) {
        f_ent->prio = prio;
        return (BCM_E_NONE);
    }
    
    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, f_ent->fs->stage_id, &stage_fc));

    /* This may change f_ent->slice_idx, as it involves moves */
    BCM_IF_ERROR_RETURN(_bcm_field_entry_target_location(unit, 
                            stage_fc, f_ent, prio, &new_location));

    old_location = f_ent->slice_idx;

#if defined (BCM_TRIUMPH3_SUPPORT)
if (SOC_IS_TRIUMPH3(unit)) {
        _bcm_field_tr3_external_entry_move(unit, f_ent, f_ent->slice_idx, new_location);
} else 
#endif /* BCM_TRIUMPH3_SUPPORT */
{
    BCM_IF_ERROR_RETURN(_bcm_field_tr_external_entry_move(unit, 
                            f_ent, f_ent->slice_idx, new_location));
}
    
    /* 
     * Update the prio_mgmt structure 
     *     to reflect number of entries of each prio 
     */
    BCM_IF_ERROR_RETURN
        (_bcm_field_entry_prio_mgmt_update(unit, f_ent, -1, old_location));
    f_ent->prio = prio;
    BCM_IF_ERROR_RETURN(_bcm_field_entry_prio_mgmt_update(unit, f_ent, +1, 0));

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_field_tr_external_tcam_key_mask_get
 * Purpose:
 *     Allocate & read from hw tcam key/mask image. 
 * Parameters:
 *     unit  - (IN)     BCM device number. 
 *     f_ent - (IN/OUT) Field entry.
 * Returns:
 *     BCM_E_XXX 
 * Notes:
 *     - 1st time: allocate both key and key_hw
 *     - at install time: copy key to key_hw and free key 
 *                         (_field_tcam_policy_install function)
 *     - other times: allocate key, and copy key_hw to key
 * 
 *     Also, for Triumph, we are using the key_width according to
 *         regs file, not the actual width. e.g. _FP_EXT_ACL_L2 = 274. 
 */
int
_bcm_field_tr_external_tcam_key_mask_get(int unit, _field_entry_t *f_ent)
{
    int key_size;  /* in bytes */

    switch (f_ent->fs->slice_number) {
      case _FP_EXT_ACL_144_L2:
      case _FP_EXT_ACL_144_IPV4:
      case _FP_EXT_ACL_144_IPV6:
          key_size = 18;
          break;
      case _FP_EXT_ACL_L2:
      case _FP_EXT_ACL_IPV4:
          key_size = 36;
          break;
      case _FP_EXT_ACL_IPV6_SHORT:
          key_size = 45;
          break;
      case _FP_EXT_ACL_L2_IPV4:
      case _FP_EXT_ACL_IPV6_FULL:
      case _FP_EXT_ACL_L2_IPV6:
          key_size = 54;
          break;
      default:
          /* should not come here */
          return (BCM_E_INTERNAL);
    }
    /* Bytes: Full number of words alligned */
    key_size = (((key_size >> 2) + 1) << 2);
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

/*
 * Function:
 *     _bcm_field_tr_external_counters_collect
 * Purpose:
 *     Counter collection thread
 * Parameters:
 *     unit  - 
 *     qi    - qualifier info
 *     f_ent - Field entry
 *     data  - 128 bit address
 *     mask  - 128 bit mask
 * Returns:
 *     BCM_E_XXX
 * Notes: 
 *     Each time we donot do all tables,  but part of one of them.
 *     The stage variables counter_collect_table and 
 *         counter_collect_index keeps track of this.
 *     The variable magic_number says how many DMAs to be done each time.
 *         We choose 16, which leads to 8K (size of Internal FP_COUNTER_TABLE)
 */
int
_bcm_field_tr_external_counters_collect(int unit, _field_stage_t *stage_fc)
{
    soc_mem_t mem;
    int alloc_size;            /* DMA buffer size.             */
    char *buf;                 /* Buffer to read the table.    */
    int idx, max_idx, min_idx, buf_end_idx;
    int magic_number = 16;     /* Maximum number of dma ops.   */
    int retval = BCM_E_NONE;   /* Operation return value.      */
    int table_id;              /* Extern ACL memory id.        */
    int unified_start_index;   /* Index min EXT_FP_CNTR table. */
    int unified_end_index;     /* Index max EXT_FP_CNTR table. */
    uint32 prev_index;

    table_id = stage_fc->counter_collect_table;
    prev_index = stage_fc->counter_collect_index;
    while (soc_mem_index_max(unit, _bcm_field_ext_counter_mems[table_id]) == -1) {
        /* Skip Empty tables */
        table_id = (table_id + 1) % stage_fc->tcam_slices;
        prev_index = 0;
        if (table_id == stage_fc->counter_collect_table) {
            /* 
             * Should not come here, 
             *     as no counters => stage_fc->_field_counters will be NULL
             */
            return (BCM_E_EMPTY);
        }
    }
    mem = _bcm_field_ext_counter_mems[table_id];
    
    /* Allocate memory buffer. */
    alloc_size = (_FP_64_COUNTER_BUCKET * sizeof(ext_fp_cntr_entry_t));
    buf = soc_cm_salloc(unit, alloc_size, "fp_64_bit_counter");
    if (buf == NULL) {
        return (BCM_E_MEMORY);
    }

    min_idx = prev_index;
    max_idx = min_idx + magic_number * _FP_64_COUNTER_BUCKET - 1;
    if (max_idx > soc_mem_index_max(unit, mem)) {
        max_idx = soc_mem_index_max(unit, mem);
    }

    /* Dma part of the table & update software counters. */
    soc_mem_lock(unit, mem);
    for (idx = min_idx; idx <= max_idx; idx += _FP_64_COUNTER_BUCKET) {
        buf_end_idx = MIN(max_idx, (idx + _FP_64_COUNTER_BUCKET - 1));
        /* Dma range of entries in counter table. */
        retval = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, idx,
                                    buf_end_idx, buf);
        if (BCM_FAILURE(retval)) {
            break;
        }

        /* Get start index in the unified S/W view table */
        soc_tcam_part_index_to_mem_index(unit, _bcm_field_fp_tcam_partitions[table_id], 
                                         idx, EXT_FP_CNTRm, 
                                         &unified_start_index);
        /* Get end index in the unified S/W view table */
        soc_tcam_part_index_to_mem_index(unit, _bcm_field_fp_tcam_partitions[table_id], 
                                         buf_end_idx, EXT_FP_CNTRm, 
                                         &unified_end_index);


        /* Update software counters. */
        retval = _bcm_field_sw_counter_update(unit, stage_fc, EXT_FP_CNTRm, 
                                              unified_start_index,
                                              unified_end_index, buf, 
                                              _FIELD_FIRST_MEMORY_COUNTERS);

        if (BCM_FAILURE(retval)) {
            break;
        }
    }
    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, buf);

    if (retval == BCM_E_NONE) {
        if (max_idx >= (soc_mem_index_max(unit, _bcm_field_ext_counter_mems[table_id]))) {
            stage_fc->counter_collect_table = (table_id + 1) % 
                                                   stage_fc->tcam_slices;
            stage_fc->counter_collect_index = 0;
        } else {
            stage_fc->counter_collect_table = table_id;
            stage_fc->counter_collect_index = max_idx + 1;
        }
    }

    return (retval);

}

/*
 * Function:
 *     _bcm_field_tr_counter_set
 * Purpose:
 *     Set accumulated sw counter and return current counter value.
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
_bcm_field_tr_counter_set(int unit, _field_stage_t *stage_fc,
                      soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                      soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                      int idx, uint64 *packet_count, uint64 *byte_count)
{
    _field_counter32_collect_t *cntrs32_buf;  /* Sw packet counter value    */
    _field_counter64_collect_t *cntrs64_buf;  /* Sw byte counter value      */
    uint32                     hw_val[2];     /* Parsed field counter value.*/

    /* Input parameters check. */
    if ((NULL == mem_x_buf) || (INVALIDm == counter_x_mem) || \
        (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    /* Byte counter. */
    if (NULL != byte_count) {
        cntrs64_buf = &stage_fc->_field_x64_counters[idx];
        COMPILER_64_ZERO(cntrs64_buf->accumulated_counter);
        COMPILER_64_OR(cntrs64_buf->accumulated_counter, (*byte_count));
        COMPILER_64_SET(cntrs64_buf->last_hw_value,
                        COMPILER_64_HI(*byte_count) & 0x7,
                        COMPILER_64_LO(*byte_count));
        hw_val[0] = COMPILER_64_LO(cntrs64_buf->last_hw_value);
        hw_val[1] = COMPILER_64_HI(cntrs64_buf->last_hw_value);
        soc_mem_field_set(unit, counter_x_mem, mem_x_buf, 
                          BYTE_COUNTERf, hw_val);
    }

    /* Packet counter. */
    if (NULL != packet_count) {
        cntrs32_buf = &stage_fc->_field_x32_counters[idx];
        COMPILER_64_ZERO(cntrs32_buf->accumulated_counter);
        COMPILER_64_OR(cntrs32_buf->accumulated_counter, (*packet_count));
        COMPILER_64_TO_32_LO(cntrs32_buf->last_hw_value, (*packet_count));
        cntrs32_buf->last_hw_value &= 0x1fffffff; /* 29 bits */
        hw_val[0] = cntrs32_buf->last_hw_value;
        hw_val[1] = 0;
        soc_mem_field_set(unit, counter_x_mem, mem_x_buf, 
                          PACKET_COUNTERf, hw_val);
    }
    return  soc_mem_write(unit, counter_x_mem, MEM_BLOCK_ALL, idx, mem_x_buf);
}

/*
 * Function:
 *     _bcm_field_tr_counter_get
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
_bcm_field_tr_counter_get(int unit, _field_stage_t *stage_fc,
                      soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                      soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                      int idx, uint64 *packet_count, uint64 *byte_count)
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
    _bcm_field_35bit_counter_update(unit, hw_val, cntrs64_buf);
    /* Return counter value to caller if (out) pointer was provided. */ 
    if (NULL != byte_count) {
        COMPILER_64_OR((*byte_count), cntrs64_buf->accumulated_counter);
    }

    /* Packet counter. */
    cntrs32_buf = &stage_fc->_field_x32_counters[idx];
    memacc_pkt =
        &(stage_fc->_field_memacc_counters[_FIELD_COUNTER_MEMACC_PACKET]);
    soc_memacc_field_get(memacc_pkt, mem_x_buf, hw_val);
    _bcm_field_29bit_counter_update(unit, hw_val, cntrs32_buf);
    /* Return counter value to caller if (out) pointer was provided. */ 
    if (NULL != packet_count) {
        COMPILER_64_OR((*packet_count), cntrs32_buf->accumulated_counter);
    }
    return (BCM_E_NONE);
}

/*      
 * Function:
 *      _bcm_field_tr_multipath_hashcontrol_set
 * Description:
 *      Set the Hash Control for FP L3 Switch to ecmp group action.
 * Parameters:
 *      unit - Bcm device number
 *      arg  - BCM_HASH_XXX
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_field_tr_multipath_hashcontrol_set(int unit, int arg)
{
    _field_control_t *fc;        /* Field control structure. */
    int rv;                      /* Operation return status. */
    soc_field_t  fields[] = {ENABLEf, USE_CRCf, USE_UPPERf};
    uint32 values[] = {FALSE, FALSE, FALSE};
    soc_reg_t reg;

#if defined BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_field_multi_pipe_support)) {
       reg = IFP_ECMP_HASH_CONTROLr;
    } else
#endif
    {
       reg = FP_ECMP_HASH_CONTROLr;
    }

    if (0 == SOC_REG_IS_VALID(unit, reg)) {
        return (BCM_E_UNAVAIL);
    }

    FP_LOCK(unit);
    /* Get unit FP control structure. */
    rv = _field_control_get(unit, &fc);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return rv;
    }

    switch (arg) {
      case BCM_HASH_ZERO:
          break;
      case BCM_HASH_XORL:
          values[0] = TRUE;
          break;
      case BCM_HASH_CRC32L:
          values[0] = TRUE;
          values[1] = TRUE;
          break;
      case BCM_HASH_XORU:
          values[0] = TRUE;
          values[2] = TRUE;
          break;
      case BCM_HASH_CRC32U:
          values[0] = TRUE;
          values[1] = TRUE;
          values[2] = TRUE;
          break;
      default:
          FP_UNLOCK(unit);
          return (BCM_E_UNAVAIL);
    }

    rv = soc_reg_fields32_modify(unit, reg, REG_PORT_ANY, 
                                 COUNTOF(fields), fields, values);
    FP_UNLOCK(unit);
    return (rv);
}

/*      
 * Function:
 *      _bcm_field_tr_multipath_hashcontrol_get
 * Description:
 *      Get the Hash Control for FP L3 Switch to ecmp group action.
 * Parameters:
 *      unit - (IN) Bcm device number
 *      arg  - (OUT) BCM_HASH_XXX
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_field_tr_multipath_hashcontrol_get(int unit, int *arg)
{
    uint32 hash_control;         /* Hash control value.      */
    int rv;                      /* Operation return status. */
    soc_reg_t reg;

#if defined BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_field_multi_pipe_support)) {
       reg = IFP_ECMP_HASH_CONTROLr;
    } else
#endif
    {
       reg = FP_ECMP_HASH_CONTROLr;
    }

    if (0 == SOC_REG_IS_VALID(unit, reg)) {
        return (BCM_E_UNAVAIL);
    }

    rv = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &hash_control);
    BCM_IF_ERROR_RETURN(rv);

    if (0 == soc_reg_field_get(unit, reg, 
                               hash_control, ENABLEf)) {
        /* Hash overwrite disable.*/
        *arg = BCM_HASH_ZERO;
    } else if (soc_reg_field_get(unit, reg, 
                                 hash_control, USE_CRCf)) {
        /* CRC32 hash algorithm enabled. .*/
        if (soc_reg_field_get(unit, reg, 
                              hash_control, USE_UPPERf)) {
            *arg = BCM_HASH_CRC32U;
        } else {
            *arg = BCM_HASH_CRC32L;
        }
    } else {
        /* Byte XOR hash algorithm enabled .*/
        if (soc_reg_field_get(unit, reg, 
                              hash_control, USE_UPPERf)) {
            *arg = BCM_HASH_XORU;
        } else {
            *arg = BCM_HASH_XORL;
        }
    }
    return (rv);
}


/*
 * Function:
 *     _field_tr_external_port_conflict_check
 * Purpose:
 *     Verify if specific key can satisfy user requested qset. 
 *     
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
_field_tr_external_port_conflict_check(int unit, _field_stage_t *stage_fc,
                                       bcm_field_qset_t *qset_req,
                                       _field_group_t *fg)
{
    _field_slice_t *fs;          /* Field slice structure.   */ 
    bcm_pbmp_t pbmp;             /* Aggregated IPv6 pbmp.    */

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == qset_req) || (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    BCM_PBMP_CLEAR(pbmp);

    if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp6)) {
        fs = stage_fc->slices[_FP_DEF_INST] + _FP_EXT_ACL_144_IPV6;
        BCM_PBMP_OR(pbmp, fs->ext_pbmp[_FP_EXT_IP6]);
        fs = stage_fc->slices[_FP_DEF_INST] + _FP_EXT_ACL_IPV6_SHORT;
        BCM_PBMP_OR(pbmp, fs->ext_pbmp[_FP_EXT_IP6]);
        fs = stage_fc->slices[_FP_DEF_INST] + _FP_EXT_ACL_IPV6_FULL;
        BCM_PBMP_OR(pbmp, fs->ext_pbmp[_FP_EXT_IP6]);
        fs = stage_fc->slices[_FP_DEF_INST] + _FP_EXT_ACL_L2_IPV6;
        BCM_PBMP_OR(pbmp, fs->ext_pbmp[_FP_EXT_IP6]);
    } else if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4)) {
        fs = stage_fc->slices[_FP_DEF_INST] + _FP_EXT_ACL_144_IPV4;
        BCM_PBMP_OR(pbmp, fs->ext_pbmp[_FP_EXT_IP4]);
        fs = stage_fc->slices[_FP_DEF_INST] + _FP_EXT_ACL_IPV4;
        BCM_PBMP_OR(pbmp, fs->ext_pbmp[_FP_EXT_IP4]);
        fs = stage_fc->slices[_FP_DEF_INST] + _FP_EXT_ACL_L2_IPV4;
        BCM_PBMP_OR(pbmp, fs->ext_pbmp[_FP_EXT_IP4]);
    } else {
        fs = stage_fc->slices[_FP_DEF_INST] + _FP_EXT_ACL_144_L2;
        BCM_PBMP_OR(pbmp, fs->ext_pbmp[_FP_EXT_L2]);
        fs = stage_fc->slices[_FP_DEF_INST] + _FP_EXT_ACL_L2;
        BCM_PBMP_OR(pbmp, fs->ext_pbmp[_FP_EXT_L2]);
    }
    /* Check if the pbmps are disjoint for IPV6 */
    BCM_PBMP_AND(pbmp, fg->pbmp);
    if (BCM_PBMP_NOT_NULL(pbmp)) {
        return (BCM_E_RESOURCE);
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_tr_external_key_attempt
 * Purpose:
 *     Verify if specific key can satisfy user requested qset. 
 *     
 * Parameters:
 *     unit      - (IN) BCM unit number.
 *     stage_fc  - (IN) Stage Field control structure.
 *     qset_req  - (IN) Client qualifier set.
 *     key_id    - (IN) External key id.
 *     pkt_type  - (IN) Packet type (V6/V4/L2).
 *     fg        - (IN/OUT)Select code information filled into the group.  
 * 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr_external_key_attempt(int unit, _field_stage_t *stage_fc, 
                               bcm_field_qset_t *qset_req,
                               uint8 key_id, uint8 pkt_type,
                               _field_group_t *fg)
{
    _field_slice_t *fs;          /* Field slice structure.   */ 

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == qset_req) || (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    /* Get specific key slice pointer. */
    fs = stage_fc->slices[_FP_DEF_INST] + key_id;
    /* If slice present and not used -> attempt to satisfy requested qset.*/
    if ((fs->entry_count) && (BCM_PBMP_IS_NULL(fs->ext_pbmp[pkt_type])) &&
        (0 != fs->pkt_type[pkt_type])) {
        fg->sel_codes[0].extn = key_id; 
        /* Verify that each requested qualifier present in the key. */
        return _bcm_field_selcode_get(unit, stage_fc, &fg->qset, fg);
    }

    return (BCM_E_RESOURCE);
}

/*
 * Function:
 *     _field_tr_external_selcode_get
 * Purpose:
 *     Finds a select encodings that will satisfy the
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
_field_tr_external_selcode_get(int unit, _field_stage_t *stage_fc, 
                               bcm_field_qset_t *qset_req,
                               _field_group_t *fg)
{
    int             rv;          /* Operation return status. */

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == qset_req) || (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    /* Only single wide groups are supported. */
    if ((fg->flags & _FP_GROUP_STATUS_MASK) != _FP_GROUP_SPAN_SINGLE_SLICE) {
        return (BCM_E_RESOURCE);
    }

    /* Check if ports are already committed for a specific packet type. */
    rv = _field_tr_external_port_conflict_check(unit, stage_fc, qset_req, fg);
    BCM_IF_ERROR_RETURN(rv);

    /* selcode_get depending on ACL type: L2/V4/V6 */
    if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp6)) {

        /* Attempt _FP_EXT_ACL_144_IPV6  key. */
        rv = _field_tr_external_key_attempt(unit, stage_fc, qset_req, 
                                            _FP_EXT_ACL_144_IPV6, 
                                            _FP_EXT_IP6, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }

        /* Attempt _FP_EXT_ACL_IPV6_SHORT key. */
        rv = _field_tr_external_key_attempt(unit, stage_fc, qset_req, 
                                            _FP_EXT_ACL_IPV6_SHORT, 
                                            _FP_EXT_IP6, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }

        /* Attempt _FP_EXT_ACL_IPV6_FULL key. */
        rv = _field_tr_external_key_attempt(unit, stage_fc, qset_req, 
                                            _FP_EXT_ACL_IPV6_FULL, 
                                            _FP_EXT_IP6, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }

        /* Attempt _FP_EXT_ACL_L2_IPV6 key. */
        rv = _field_tr_external_key_attempt(unit, stage_fc, qset_req, 
                                            _FP_EXT_ACL_L2_IPV6, 
                                            _FP_EXT_IP6, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }
    } else if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4)) {

        /* Attempt _FP_EXT_ACL_144_IPV4  key. */
        rv = _field_tr_external_key_attempt(unit, stage_fc, qset_req, 
                                            _FP_EXT_ACL_144_IPV4, 
                                            _FP_EXT_IP4, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }

        /* Attempt _FP_EXT_ACL_IPV4 key. */
        rv = _field_tr_external_key_attempt(unit, stage_fc, qset_req, 
                                            _FP_EXT_ACL_IPV4, 
                                            _FP_EXT_IP4, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }

        /* Attempt _FP_EXT_ACL_L2_IPV4 key. */
        rv = _field_tr_external_key_attempt(unit, stage_fc, qset_req, 
                                            _FP_EXT_ACL_L2_IPV4, 
                                            _FP_EXT_IP4, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }
    } else {
        /* Attempt _FP_EXT_ACL_144_IPV4  key. */
        rv = _field_tr_external_key_attempt(unit, stage_fc, qset_req, 
                                            _FP_EXT_ACL_144_L2, 
                                            _FP_EXT_L2, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }

        /* Attempt _FP_EXT_ACL_IPV4 key. */
        rv = _field_tr_external_key_attempt(unit, stage_fc, qset_req, 
                                            _FP_EXT_ACL_L2, 
                                            _FP_EXT_L2, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }
    }
    return (BCM_E_RESOURCE);
}

/*
 * Function:
 *     _bcm_field_tr_selcode_get
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
_bcm_field_tr_selcode_get(int unit, _field_stage_t *stage_fc, 
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
      case _BCM_FIELD_STAGE_EXTERNAL:
          rv =  _field_tr_external_selcode_get(unit, stage_fc, qset_req, fg);
          break;
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
 *     _field_v4_external_mode_set
 *
 * Purpose:
 *     Auxiliary routine used to program external key selector.
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
_field_v4_external_mode_set(int unit, uint8 slice_numb, 
                                _field_group_t *fg, uint8 flags)
{
    bcm_port_t port;
    uint32  mode = 0;
    uint32  acl144 = 0;
    int     rv;

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    switch (slice_numb) {
      case _FP_EXT_ACL_144_IPV4:
          acl144 = 1;
          mode = 2;
          break;
      case _FP_EXT_ACL_IPV4:
          mode = 2;
          break;
      case _FP_EXT_ACL_L2_IPV4:
          mode = 3;
          break;
      default:
          /* should not come here */
          return BCM_E_INTERNAL;
    }

    BCM_PBMP_ITER(fg->pbmp, port) {
        rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port, 
                                    IPV4_ACL_MODEf, mode);
        BCM_IF_ERROR_RETURN(rv);
        rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                    IPV4_ACL_144_ENf, acl144);
        BCM_IF_ERROR_RETURN(rv);

        /*
         * IPV4_ACL_TYPE = 1:
         * Means OuterVlan/Tunnel info are part of this key.
         */
        rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                    IPV4_ACL_TYPEf, 1);
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_l2_external_mode_set
 *
 * Purpose:
 *     Auxiliary routine used to program external key selector.
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
_field_l2_external_mode_set(int unit, uint8 slice_numb, 
                                _field_group_t *fg, uint8 flags)
{
    bcm_port_t port;
    int rv;

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    BCM_PBMP_ITER(fg->pbmp, port) {
        rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port, 
                                    L2_ACL_ENf, 1);
        BCM_IF_ERROR_RETURN(rv);

        rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port, 
                                    IPV4_ACL_MODEf, 1);
        BCM_IF_ERROR_RETURN(rv);

        rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port, 
                                    IPV6_ACL_MODEf, 1);
        BCM_IF_ERROR_RETURN(rv);

        rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                    L2_ACL_144_ENf, 
                                    (_FP_EXT_ACL_144_L2 == slice_numb) ? 1 : 0);
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_v6_external_mode_set
 *
 * Purpose:
 *     Auxiliary routine used to program external key selector.
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
_field_v6_external_mode_set(int unit, uint8 slice_numb, 
                                _field_group_t *fg, uint8 flags)
{
    bcm_port_t port;
    uint32  mode = 0;
    uint32  acl144 = 0;
    uint32  ipv6_full = 0;
    int     rv;

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    switch (slice_numb) {
      case _FP_EXT_ACL_144_IPV6:
          acl144 = 1;
          mode = 2;
          break;
      case _FP_EXT_ACL_IPV6_SHORT:
          mode = 2;
          break;
      case _FP_EXT_ACL_IPV6_FULL:
          ipv6_full = 1;
          mode = 2;
          break;
      case _FP_EXT_ACL_L2_IPV6:
          mode = 3;
          break;
      default:
          /* should not come here */
          return BCM_E_INTERNAL;
    }

    BCM_PBMP_ITER(fg->pbmp, port) {
        rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port, 
                                    IPV6_ACL_MODEf, mode);
        BCM_IF_ERROR_RETURN(rv);
        rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                    IPV6_ACL_144_ENf, acl144);
        BCM_IF_ERROR_RETURN(rv);
        rv = soc_reg_field32_modify(unit, ESM_MODE_PER_PORTr, port,
                                    IPV6_FULL_ACLf, ipv6_full);
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_tr_external_mode_set
 *
 * Purpose:
 *     Auxiliary routine used to program external key selector.
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
_bcm_field_tr_external_mode_set(int unit, uint8 slice_numb, 
                                _field_group_t *fg, uint8 flags)
{
    int rv; 

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    switch (slice_numb) {
      case _FP_EXT_ACL_144_IPV6:
      case _FP_EXT_ACL_IPV6_SHORT:
      case _FP_EXT_ACL_IPV6_FULL:
      case _FP_EXT_ACL_L2_IPV6:
          rv = _field_v6_external_mode_set(unit, slice_numb, fg, flags);
          break;
      case _FP_EXT_ACL_144_IPV4:
      case _FP_EXT_ACL_IPV4:
      case _FP_EXT_ACL_L2_IPV4:
          rv = _field_v4_external_mode_set(unit, slice_numb, fg, flags);
          break;
      case _FP_EXT_ACL_144_L2:
      case _FP_EXT_ACL_L2:
          rv = _field_l2_external_mode_set(unit, slice_numb, fg, flags);
          break;
      default:
          rv = BCM_E_INTERNAL;
    }
    return (rv);
}

/*
 * Function:
 *     _field_tr_functions_init
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
_field_tr_functions_init(_field_funct_t *functions)
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
    functions->fp_egress_key_match_type_set = 
        _bcm_field_trx_egress_key_match_type_set;
    functions->fp_external_entry_install  = _bcm_field_tr_external_entry_install;
    functions->fp_external_entry_reinstall  = _bcm_field_tr_external_entry_reinstall;
    functions->fp_external_entry_remove   = _bcm_field_tr_external_entry_remove;
    functions->fp_external_entry_prio_set = _bcm_field_tr_external_entry_prio_set;
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
#else /* BCM_TRIUMPH_SUPPORT && BCM_FIELD_SUPPORT */
typedef int _triumph_field_not_empty;
#endif  /* BCM_TRIUMPH_SUPPORT && BCM_FIELD_SUPPORT */
