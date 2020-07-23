/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field.c
 * Purpose:     Field Processor installation functions.
 */
#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/scache.h>

#include <bcm/error.h>
#include <bcm/field.h>
#include <bcm/mirror.h>

#if defined(BCM_RAPTOR_SUPPORT) && defined(BCM_FIELD_SUPPORT)
 
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/raptor.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/mbcm.h>

STATIC void _field_raptor_functions_init(_field_funct_t *functions);
STATIC int _field_raptor_detach(int unit, _field_control_t *fc);
STATIC int _field_raptor_hw_clear(int unit, _field_stage_t *stage_fc);
STATIC int _field_raptor_entry_remove(int unit, _field_entry_t *f_ent,
                                      int tcam_idx);
STATIC int _field_raptor_tables_entry_clear(int unit,
                                            _field_entry_t *f_ent,
                                            _field_stage_id_t stage_id,
                                            int tcam_idx);
STATIC int _field_raptor_group_install(int unit, _field_group_t *fg);
STATIC int _field_raptor_entry_install(int unit, _field_entry_t *f_ent, 
                                       int tcam_idx);
STATIC int _field_raptor_entry_reinstall(int unit, _field_entry_t *f_ent, 
                                       int tcam_idx);
STATIC int _field_raptor_selcodes_install(int unit, _field_group_t *fg,
                                          uint8 slice_numb, 
                                          bcm_pbmp_t pbmp, int selcode_index);
STATIC int _field_raptor_policy_install(int unit, _field_entry_t *f_ent, 
                                        int tcam_idx);
STATIC int _field_raptor_ipbm_install(int unit, _field_entry_t *f_ent);
STATIC int _field_raptor_tcam_install(int unit, _field_entry_t *f_ent, 
                                      int tcam_idx);
STATIC int _field_raptor_tcam_get(int unit, _field_entry_t *f_ent, 
                                  fp_tcam_entry_t *t_entry);
STATIC int _field_raptor_action_get(int unit, _field_entry_t *f_ent, 
                                    int tcam_idx, _field_action_t *fa, 
                                    fp_policy_table_entry_t *p_entry);
STATIC int _field_raptor_meter_action_set(int unit, _field_entry_t *f_ent,
                                          fp_policy_table_entry_t *p_entry);
STATIC int _field_raptor_entry_move(int unit, _field_entry_t *f_ent, 
                                    int parts_count, int *tcam_idx_old, 
                                    int *tcam_idx_new);
STATIC int _field_raptor_entry_rule_copy(int unit, int old_index, 
                                         int new_index);
STATIC int _field_raptor_entry_rule_del(int unit, int index);
STATIC int _field_raptor_entry_ipbm_copy(int unit, _field_entry_t *f_ent, 
                                         int part, int new_tcam_idx);
STATIC int _field_raptor_ipbm_entry_set(int unit, int row, 
                                        int col, uint32 enable);
STATIC int _field_raptor_ipbm_entry_get(int unit, int row, 
                                        int col, uint32 *enable);
STATIC int _field_raptor_action_params_check(int unit,_field_entry_t *f_ent, 
                                             _field_action_t *fa);

/*
 * Function:
 *     _field_rp_ingress_qualifiers_init
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
_field_rp_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    soc_mem_t     tcam_mem;        /* TCAM memory id.           */
    soc_mem_t     policy_mem;      /* Policy table memory id .  */
    int           offset;          /* Tcam field offset.        */
    int           rv;              /* Operation return status.  */
    _FP_QUAL_DECL;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    rv = _field_fb_tcam_policy_mem_get(unit, stage_fc->stage_id, 
                                       &tcam_mem, &policy_mem);
    BCM_IF_ERROR_RETURN(rv);

    _key_fld_ = F4f;

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf4, 1, 2, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf4, 1, 0, 2);

    _key_fld_ = KEYf;
    offset = 0;

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStage,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPorts,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageIngress,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 0, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 0, 2, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 0, 6, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 0, 9, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf3, 0, 13, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf3, 1, 0, 11);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf3, 1, 0, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 1, 11, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf3, 2, 0, 11);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf3, 2, 0, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 2, 11, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelFpf3, 3, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, 3, 1, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf3, 3, 2, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf3, 3, 3, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf3, 3, 5, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf3, 3, 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf3, 3, 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, 3, 8, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf3, 3, 9, 1);
#if defined (BCM_RAVEN_SUPPORT)
    if (SOC_IS_RAVEN(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                _bcmFieldSliceSelFpf3, 3, 10, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                _bcmFieldSliceSelFpf3, 3, 11, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                _bcmFieldSliceSelFpf3, 3, 12, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                _bcmFieldSliceSelFpf3, 3, 13, 1);
    }
#endif /* BCM_RAVEN_SUPPORT */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf3, 3, 14, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf3, 3, 15, 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, 4, 0, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf3, 4, 6, 8);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 5, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 5, 0, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 5, 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 5, 13, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf3, 6, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, 6, 8, 8);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf3, 7, 0, 16);

    /* FPF2 */
    offset += _FIELD_MEM_FIELD_LENGTH(unit, tcam_mem, F3f);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 0, offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0, (offset + 8), 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
                 _bcmFieldSliceSelFpf2, 0, (offset + 14), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 0, (offset + 16), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 0, (offset + 24), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 0, (offset + 40), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 0, (offset + 56), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 0, (offset + 64), 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 0, (offset + 96), 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 1, offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 1, (offset + 8), 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
                 _bcmFieldSliceSelFpf2, 1, (offset + 14), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 1, (offset + 16), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 1, (offset + 24), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf2, 1, (offset + 40), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 1, (offset + 56), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 1, (offset + 64), 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 1, (offset + 96), 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 2, offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 2, (offset + 8), 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
                 _bcmFieldSliceSelFpf2, 2, (offset + 14), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 2, (offset + 16), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 2, (offset + 24), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf2, 2, (offset + 40), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 2, (offset + 56), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 2, (offset + 64), 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 2, (offset + 96), 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelFpf2, 3, offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf2, 4, offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 5, offset, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 5, (offset + 6), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf2, 5, (offset + 14), 20);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 5, (offset + 34), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 5, (offset + 42), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 5, (offset + 50), 64);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 6, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 6, offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 6, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 6, (offset + 13), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 6, (offset + 16), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 6, (offset + 32), 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 6, (offset + 80), 48);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 7, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 7, offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 7, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 7, (offset + 13), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 7, (offset + 16), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 7, (offset + 32), 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 7, (offset + 64), 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 8, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 8, offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 8, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 8, (offset + 13), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 8, (offset + 16), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 8, (offset + 32), 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 8, (offset + 64), 48);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 9, offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                 _bcmFieldSliceSelFpf2, 0xa, offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                 _bcmFieldSliceSelFpf2, 0xb, offset, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 0xb, (offset + 64), 64);


#if defined(BCM_RAVEN_SUPPORT) 
    if (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                     _bcmFieldSliceSelFpf2, 0xc, offset, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 8), 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 14), 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 16), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 24), 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 40), 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 56), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 64), 32);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 96), 32);
    }
#endif /* BCM_RAVEN_SUPPORT */

    /* FPF1 */
    offset += _FIELD_MEM_FIELD_LENGTH(unit, tcam_mem, F2f);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelFpf1, 0, offset, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf1, 0, (offset + 1), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf1, 0, (offset + 2), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf1, 0, (offset + 3), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf1, 0, (offset + 5), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf1, 0, (offset + 6), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf1, 0, (offset + 7), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf1, 0, (offset + 8), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf1, 0, (offset + 9), 1);
#if defined (BCM_RAVEN_SUPPORT)
    if (SOC_IS_RAVEN(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                _bcmFieldSliceSelFpf1, 0, (offset + 10), 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                _bcmFieldSliceSelFpf1, 0, (offset + 11), 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                _bcmFieldSliceSelFpf1, 0, (offset + 12), 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                _bcmFieldSliceSelFpf1, 0, (offset + 13), 1);
    }
#endif /* BCM_RAVEN_SUPPORT */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf1, 0, (offset + 14), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf1, 0, (offset + 15), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                 _bcmFieldSliceSelFpf1, 0, (offset + 16), 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 0, (offset + 23), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 0, (offset + 25), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 0, (offset + 27), 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf1, 0, (offset + 31), 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf1, 1, offset, 11);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf1, 1, offset, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf1, 1, (offset + 11), 11);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf1, 1, (offset + 11), 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 1, (offset + 22), 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf1, 2, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf1, 2, (offset + 16), 16);

#if defined(BCM_RAVEN_SUPPORT) 
    if (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         offset, 16, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         offset, 12, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 12), 1, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 13), 3, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                         _bcmFieldDevSelInnerVlanOverlay, 1,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         offset, 6, 0, 0, 0, 0, 0);
    } else 
#endif /* BCM_RAVEN_SUPPORT */
        { 
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                         _bcmFieldSliceSelFpf1, 3, offset, 16);
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                         _bcmFieldSliceSelFpf1, 3, offset, 12);
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                         _bcmFieldSliceSelFpf1, 3, (offset + 12), 1);
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                         _bcmFieldSliceSelFpf1, 3, (offset + 13), 3);
        }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 3, (offset + 16), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 3, (offset + 16), 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 3, (offset + 28), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 3, (offset + 29), 3);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 4, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 4, offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 4, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 4, (offset + 13), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 4, (offset + 16), 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf1, 5, offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 5, (offset + 8), 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelFpf1, 6, offset, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 1), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf1, 6, (offset + 2), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf1, 6, (offset + 3), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 5), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf1, 6, (offset + 6), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 7), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf1, 6, (offset + 8), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 9), 1);
#if defined(BCM_RAVEN_SUPPORT) 
    if (SOC_IS_RAVEN(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                _bcmFieldSliceSelFpf1, 6, (offset + 10), 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                _bcmFieldSliceSelFpf1, 6, (offset + 11), 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                _bcmFieldSliceSelFpf1, 6, (offset + 12), 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                _bcmFieldSliceSelFpf1, 6, (offset + 13), 1);
    }
#endif /* BCM_RAVEN_SUPPORT  */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf1, 6, (offset + 14), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf1, 6, (offset + 15), 1);
#if defined(BCM_RAVEN_SUPPORT) 
    if (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 16), 16, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 16), 12, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 28), 1, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 29), 3, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                         _bcmFieldDevSelInnerVlanOverlay, 1,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 16), 6, 0, 0, 0, 0, 0);
    } else 
#endif /* BCM_RAVEN_SUPPORT */
    {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                     _bcmFieldSliceSelFpf1, 6, (offset + 16), 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                     _bcmFieldSliceSelFpf1, 6, (offset + 16), 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                     _bcmFieldSliceSelFpf1, 6, (offset + 28), 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                     _bcmFieldSliceSelFpf1, 6, (offset + 29), 3);
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 7, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 7, offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 7, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 7, (offset + 13), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 7, (offset + 16), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 7, (offset + 18), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 7, (offset + 22), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 7, (offset + 25), 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf1, 7, (offset + 29), 3);

#if defined(BCM_RAVEN_SUPPORT) 
    if (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {

        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                     _bcmFieldSliceSelFpf1, 0xc, offset, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                     _bcmFieldSliceSelFpf1, 0xc, offset, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                     _bcmFieldSliceSelFpf1, 0xc, (offset + 12), 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                     _bcmFieldSliceSelFpf1, 0xc, (offset + 13), 3);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldSliceSelFpf1, 0xc, (offset + 16), 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelFpf1, 0xc, (offset + 31), 1);

        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelFpf1, 0xd, offset , 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldSliceSelFpf1, 0xd, (offset + 8) , 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelFpf1, 0xd, (offset + 23) , 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldSliceSelFpf1, 0xd, (offset + 24) , 8);
    }
#endif /* BCM_FIREBOLT2_SUPPORT */
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_rp_qualifiers_init
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
_field_rp_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
          rv = _field_rp_ingress_qualifiers_init(unit, stage_fc);
          break;
      default: 
          sal_free(stage_fc->f_qual_arr);
          return (BCM_E_PARAM);
    }
    return (rv);
}

/*
 * Function:
 *    _bcm_field_raptor_init
 * Purpose:
 *     Perform initializations that are specific to Raptor. This
 *     includes initializing the FP field select bit offset tables 
 *     for FPF[1-4]
 * Parameters:
 *     unit - (IN) BCM device number
 *     fc   - (IN) FP control struct
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */
int
_bcm_field_raptor_init(int unit, _field_control_t *fc) 
{
    _field_stage_t *stage_p; /* Stages iteration pointer */

    /* Input parameters check. */
    if (NULL == fc) {
        return (BCM_E_PARAM);
    }
    
    stage_p = fc->stages;

    if (stage_p == NULL ||
        _BCM_FIELD_STAGE_INGRESS != stage_p->stage_id) {
        return (BCM_E_PARAM);
    }

    /* Clear the hardware tables */ 
    BCM_IF_ERROR_RETURN(_field_raptor_hw_clear(unit, stage_p));

    /* Initialize Qsets, bit offsets and width tables. */
    BCM_IF_ERROR_RETURN(_field_rp_qualifiers_init(unit, stage_p));

    if (!SOC_WARM_BOOT(unit)) {
        /* Set the Filter Enable flags in the port table */
        BCM_IF_ERROR_RETURN(_field_port_filter_enable_set(unit, fc, TRUE));

        /* Set meter refreshing enable */
        BCM_IF_ERROR_RETURN(_field_meter_refresh_enable_set(unit, fc, TRUE));
    }
 
    /* Initialize the function pointers */
    _field_raptor_functions_init(&fc->functions);

    return (BCM_E_NONE);
}

/*
 *     _field_raptor_hw_clear
 * Purpose:
 *     Clear hardware memory.
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.
 *
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */
STATIC int
_field_raptor_hw_clear(int unit, _field_stage_t *stage_fc) 
{
    if (SOC_WARM_BOOT(unit)) {
        return (BCM_E_NONE);
    }

    COMPILER_REFERENCE(stage_fc);

    /* Clear udf match criteria registers. */
    BCM_IF_ERROR_RETURN(_bcm_field_fb_udf_ipprotocol_delete_all(unit));
    BCM_IF_ERROR_RETURN(_bcm_field_fb_udf_ethertype_delete_all(unit));

    SOC_IF_ERROR_RETURN                                                
        (soc_mem_clear((unit), FP_UDF_OFFSETm, COPYNO_ALL, TRUE));     
    SOC_IF_ERROR_RETURN                                               
        (soc_mem_clear((unit), FP_PORT_FIELD_SELm, COPYNO_ALL, TRUE));
    SOC_IF_ERROR_RETURN                                          
        (soc_mem_clear((unit), FP_RANGE_CHECKm, COPYNO_ALL, TRUE));
    SOC_IF_ERROR_RETURN                                          
        (soc_mem_clear((unit), FP_TCAMm, COPYNO_ALL, TRUE));          
    SOC_IF_ERROR_RETURN                                        
        (soc_mem_clear((unit), FP_POLICY_TABLEm, COPYNO_ALL, TRUE));
    SOC_IF_ERROR_RETURN                                       
        (soc_mem_clear((unit), FP_SLICE_ENTRY_PORT_SELm, COPYNO_ALL, TRUE));
    SOC_IF_ERROR_RETURN
        (soc_mem_clear((unit), FP_SLICE_MAPm, COPYNO_ALL, TRUE));
    SOC_IF_ERROR_RETURN
        (soc_mem_clear((unit), FP_METER_TABLEm, COPYNO_ALL, TRUE));  
    return (soc_mem_clear((unit), FP_COUNTER_TABLEm, COPYNO_ALL, TRUE));
}

#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_RAVEN_SUPPORT)
typedef enum
{
    _fieldRavenSliceModeSingle = 0,
    _fieldRavenSliceModeDouble = 1,
    _fieldRavenSliceModeTriple = 2,
    _fieldRavenSliceModeCount
} _field_raven_slice_mode_t;

typedef struct
{
    int8 fpf1;
    int8 fpf2;
    int8 fpf3;
    int8 fpf4;
    int8 inner_vlan_overlay;
} _field_raven_sel_codes_t;

STATIC int _field_raven_group_construct(int unit, _field_control_t *fc,
    uint32 slice_mode, _field_stage_id_t stage_id, int slice_index,
    bcm_port_t port, _field_raven_sel_codes_t *hw_sel_codes)
{
    int rv;
    _field_stage_t *stage_fc;
    _field_group_t *fg_p;
    int part_index;
    bcm_field_group_t group_id;
    int               priority = -1;
    int               group_flags = 0;
    uint16 qualifier_id;
    _bcm_field_qual_info_t *stage_qualifier_p;
    int config_index;
    _bcm_field_qual_conf_t *config_p = NULL;
    _bcm_field_selector_t selector;
    int qualifier_id_index;
    _bcm_field_group_qual_t *part_qualifier_p;
    int old_qualifier_count;
    int new_qualifier_count;
    uint16 *qualifier_ids_p;
    _bcm_field_qual_offset_t *qualifier_offsets_p;
    _bcm_field_qual_offset_t *offset_p;
    bcm_field_qset_t qset;
    int action_res_id = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
    bcm_pbmp_t group_pbmp;
    int idx;

    bcm_field_qset_t_init(&qset);

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, stage_id, &stage_fc));

    for (fg_p = fc->groups; fg_p != NULL; fg_p = fg_p->next)
    {
        if (((slice_mode == _fieldRavenSliceModeSingle) &&
                ((fg_p->flags & _FP_GROUP_SPAN_SINGLE_SLICE) == 0)) ||
            ((slice_mode == _fieldRavenSliceModeDouble) &&
                ((fg_p->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) == 0)) ||
            ((slice_mode == _fieldRavenSliceModeTriple) &&
                ((fg_p->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) == 0)))
        {
            /* This group isn't the same width as the one
               we're reconstructing */

            continue;
        }

        /* Check for primary slice numbers - there may be groups with the same
           QSET but residing in different slices */
        if (slice_index != fg_p->slices[0].slice_number) {
            continue;
        }

        /* See if selector codes match */

        for (part_index = 0; part_index <= slice_mode; ++part_index)
        {
            if ((fg_p->sel_codes[part_index].fpf1 !=
                    hw_sel_codes[part_index].fpf1) ||
                (fg_p->sel_codes[part_index].fpf2 !=
                    hw_sel_codes[part_index].fpf2) ||
                (fg_p->sel_codes[part_index].fpf3 !=
                    hw_sel_codes[part_index].fpf3) ||
                (fg_p->sel_codes[part_index].fpf4 !=
                    hw_sel_codes[part_index].fpf4))
            {
                /* Selectors don't match */

                break;
            }
        }

        if (part_index <= slice_mode)
        {
            /* Found a mismatch */

            continue;
        }

        /* If we've gotten this far, everything matches, so add
           the port to the group's PBMP */

        BCM_PBMP_PORT_ADD(fg_p->pbmp, port);
        BCM_PBMP_OR(fg_p->slices[0].pbmp, fg_p->pbmp);

        group_flags = 0;
        if (fc->l2warm)
        {
            /* Get stored group ID and qset for level 2 */

            BCM_IF_ERROR_RETURN(_field_group_info_retrieve(unit,
                                                           port, _FP_DEF_INST,
                                                           &group_id,
                                                           &priority,
                                                           &action_res_id,
                                                           &group_pbmp,
                                                           NULL,
                                                           &group_flags,
                                                           &qset,
                                                           fc
                                                           )
                                );

            if (group_id != -1)
            {
                sal_memcpy(&fg_p->qset, &qset, sizeof(bcm_field_qset_t));

                fg_p->gid = group_id;
                fg_p->action_res_id = action_res_id;

                if (group_flags & _FP_GROUP_SELECT_AUTO_EXPANSION) {
                    fg_p->flags = fg_p->flags | 
                                        _FP_GROUP_SELECT_AUTO_EXPANSION;
                }

            }
        }

        return BCM_E_NONE;
    }

    /* No existing groups match; create a new one */

    _FP_XGS3_ALLOC(fg_p, sizeof(_field_group_t), "field group");

    if (fg_p == NULL)
    {
        return BCM_E_MEMORY;
    }

        /* Initialize action res id and VMAP group to default */
        action_res_id = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
        for (idx = 0; idx < _FP_PAIR_MAX; idx++) {
            fg_p->vmap_group[idx] = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
        }


        group_flags = 0;
        if (fc->l2warm)
        {
                /* Get stored group ID */

                rv = _field_group_info_retrieve(unit,
                                                port, _FP_DEF_INST,
                                                &group_id,
                                                &priority,
                                                &action_res_id,
                                                &group_pbmp,
                                                NULL,
                                                &group_flags,
                                                &qset,
                                                fc
                                                );

        if (group_id != -1)
        {
            sal_memcpy(&fg_p->qset, &qset, sizeof(bcm_field_qset_t));
        } else {
            sal_free(fg_p);
            return rv;
        }
        }
        else
            {
                if ((rv = _bcm_field_group_id_generate(unit, &group_id))
                     == BCM_E_NONE) {
                unsigned vslice;
            
                for (vslice = 0; ; ++vslice) {
                    if (vslice >= COUNTOF(stage_fc->vmap[_FP_DEF_INST][0])) {
                        rv = BCM_E_INTERNAL;
                        
                        break;
                    }
                    
                    if (stage_fc->vmap[_FP_DEF_INST][0][vslice].vmap_key == slice_index) {
                        priority = stage_fc->vmap[_FP_DEF_INST][0][vslice].priority;
                        
                        break;
                    }
                }

                if (priority == -1) {
                    rv = BCM_E_INTERNAL;
                }
            }
        }

    if (BCM_FAILURE(rv))
    {
        sal_free(fg_p);

        return rv;
    }

    fg_p->gid      = group_id;
    fg_p->priority = priority;
    fg_p->action_res_id = action_res_id;
    if (group_flags & _FP_GROUP_SELECT_AUTO_EXPANSION) {
        fg_p->flags = fg_p->flags | _FP_GROUP_SELECT_AUTO_EXPANSION;
    }
    fg_p->stage_id = stage_id;

    BCM_PBMP_PORT_ADD(fg_p->pbmp, port);

    for (part_index = 0; part_index <= slice_mode; ++part_index)
    {
        fg_p->sel_codes[part_index].fpf1 = hw_sel_codes[part_index].fpf1;
        fg_p->sel_codes[part_index].fpf2 = hw_sel_codes[part_index].fpf2;
        fg_p->sel_codes[part_index].fpf3 = hw_sel_codes[part_index].fpf3;
        fg_p->sel_codes[part_index].fpf4 = hw_sel_codes[part_index].fpf4;
        fg_p->sel_codes[part_index].inner_vlan_overlay = 
            hw_sel_codes[part_index].inner_vlan_overlay;
    }

    switch (slice_mode)
    {
    case _fieldRavenSliceModeSingle:
        fg_p->flags |= _FP_GROUP_SPAN_SINGLE_SLICE;
        break;

    case _fieldRavenSliceModeDouble:
        fg_p->flags |= _FP_GROUP_SPAN_DOUBLE_SLICE;
        break;

    case _fieldRavenSliceModeTriple:
        fg_p->flags |= _FP_GROUP_SPAN_TRIPLE_SLICE;
        break;

    }

    /* Build qset from selectors */

    for (part_index = 0; part_index <= slice_mode; ++part_index)
    {
        for (qualifier_id = 0; qualifier_id < _bcmFieldQualifyCount;
            ++qualifier_id)
        {
            stage_qualifier_p = stage_fc->f_qual_arr[qualifier_id];

            if (stage_qualifier_p == NULL)
            {
                /* Qualifier does not exist in this stage */

                continue;
            }

            if (fc->l2warm && !BCM_FIELD_QSET_TEST(fg_p->qset, qualifier_id)) {
                continue; /* Qualifier not present in the group */
            }

            for (config_index = 0; config_index < stage_qualifier_p->conf_sz;
                ++config_index)
            {
                config_p = stage_qualifier_p->conf_arr + config_index;

                selector = config_p->selector;

                /* Check for F1 match */

                if ((selector.pri_sel == _bcmFieldSliceSelFpf1) &&
                    (selector.pri_sel_val ==
                        fg_p->sel_codes[part_index].fpf1))
                {
                    break;
                }

                /* Check for F2 match */

                if ((selector.pri_sel == _bcmFieldSliceSelFpf2) &&
                    (selector.pri_sel_val ==
                        fg_p->sel_codes[part_index].fpf2))
                {
                    break;
                }

                /* Check for F3 match */

                if ((selector.pri_sel == _bcmFieldSliceSelFpf3) &&
                    (selector.pri_sel_val ==
                        fg_p->sel_codes[part_index].fpf3))
                {
                    break;
                }

                /* Check for F4 match */

                if ((selector.pri_sel == _bcmFieldSliceSelFpf4) &&
                    (selector.pri_sel_val ==
                        fg_p->sel_codes[part_index].fpf4))
                {
                    break;
                }

                /* Check for Disabled match */
                if ((qualifier_id == bcmFieldQualifyStage)
                        || (qualifier_id == bcmFieldQualifyIpType)
                        || (qualifier_id == bcmFieldQualifyStageIngress)
                        || (qualifier_id == bcmFieldQualifyIp4)
                        || (qualifier_id == bcmFieldQualifyIp6)
                        || (qualifier_id == bcmFieldQualifyHiGig)) {
                    if ((selector.pri_sel == 0) &&
                            (selector.pri_sel_val == 0))
                    {
                        break;
                    }
                }
            }

            if (config_index < stage_qualifier_p->conf_sz)
            {
                /* Found a match; add the qualifier to the qset */

                BCM_FIELD_QSET_ADD(fg_p->qset, qualifier_id);

                part_qualifier_p = &(fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part_index]);

                for (qualifier_id_index = 0;
                    qualifier_id_index < part_qualifier_p->size;
                    ++qualifier_id_index)
                {
                    if (part_qualifier_p->qid_arr[qualifier_id_index] ==
                        qualifier_id)
                    {
                        /* Qualifier already in the qset */

                        break;
                    }
                }

                if (qualifier_id_index == part_qualifier_p->size)
                {
                    /* Qualifier not already in the qset; add it */

                    old_qualifier_count = part_qualifier_p->size;
                    new_qualifier_count = old_qualifier_count + 1;

                    qualifier_ids_p = NULL;
                    qualifier_offsets_p = NULL;

                    _FP_XGS3_ALLOC(qualifier_ids_p, new_qualifier_count *
                        sizeof (uint16), "Group qualifier ids");

                    if (qualifier_ids_p == NULL)
                    {
                        sal_free(fg_p);
                        return BCM_E_MEMORY;
                    }

                    _FP_XGS3_ALLOC(qualifier_offsets_p, new_qualifier_count *
                        sizeof (_bcm_field_qual_offset_t),
                        "Group qual offsets");

                    if (qualifier_offsets_p == NULL)
                    {
                        sal_free(fg_p);
                        sal_free(qualifier_ids_p);

                        return BCM_E_MEMORY;
                    }

                    if (old_qualifier_count > 0)
                    {
                        /* Copy existing data */

                        sal_memcpy(qualifier_ids_p, part_qualifier_p->qid_arr,
                            old_qualifier_count * sizeof (uint16));

                        sal_memcpy(qualifier_offsets_p,
                            part_qualifier_p->offset_arr,
                            old_qualifier_count *
                                sizeof (_bcm_field_qual_offset_t));

                        sal_free(part_qualifier_p->qid_arr);
                        sal_free(part_qualifier_p->offset_arr);
                    }

                    part_qualifier_p->qid_arr = qualifier_ids_p;
                    part_qualifier_p->offset_arr = qualifier_offsets_p;

                    part_qualifier_p->qid_arr[old_qualifier_count] =
                        qualifier_id;

                    offset_p = part_qualifier_p->offset_arr +
                        old_qualifier_count;

                    sal_memcpy(offset_p, &config_p->offset,
                               sizeof(config_p->offset));

                    part_qualifier_p->size = new_qualifier_count;

                    _field_qset_udf_bmap_reinit(unit,
                                                stage_fc,
                                                &fg_p->qset,
                                                qualifier_id
                                                );
                }
            }
        }
    }

    /* Associate slice with group */

    if (fg_p->slices == NULL)
    {
        fg_p->slices = stage_fc->slices[_FP_DEF_INST] + slice_index;
    }

    BCM_PBMP_OR(fg_p->slices[0].pbmp, fg_p->pbmp);

    /* Initialize group default ASET list. */
    rv = _field_group_default_aset_set(unit, fg_p);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    if (fc->l2warm
            && ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15)) {
        if (group_flags & _FP_GROUP_LOOKUP_ENABLED) {
            fg_p->flags = fg_p->flags | _FP_GROUP_LOOKUP_ENABLED;
        }
        if (group_flags & _FP_GROUP_WLAN) {
            fg_p->flags = fg_p->flags | _FP_GROUP_WLAN;
        }
    } else {
        fg_p->flags |= _FP_GROUP_LOOKUP_ENABLED;
    }

    fg_p->next = fc->groups;
    fc->groups = fg_p;

    return BCM_E_NONE;
}

static int _field_raven_counter_recover(int unit, _field_entry_t *entry_p,
    uint32 mode, uint32 counter_index, int part_index, bcm_field_stat_t sid)
{
    _field_control_t *fc;
    _field_stage_t *stage_fc;
    _field_group_t *fg_p;
    _field_slice_t *fs_p;
    _field_stage_id_t stage_id;
    uint32 submode;
    uint8 stat_count = 1;
    uint8 hw_entry_count = 1;
    _field_entry_stat_t *entry_stat_p;
    int hash_index;
    _field_stat_t *stat_p = NULL;
    int stat_id;
    bcm_field_stat_t stat_arr[2];
    int rv;

    fg_p = entry_p->group;
    fs_p = entry_p[part_index].fs;

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    stage_id = fg_p->stage_id;

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, stage_id, &stage_fc));

    submode = mode & ~BCM_FIELD_COUNTER_MODE_BYTES;

    switch (submode)
    {
    case BCM_FIELD_COUNTER_MODE_RED:
    case BCM_FIELD_COUNTER_MODE_YELLOW:
    case BCM_FIELD_COUNTER_MODE_GREEN:
    case BCM_FIELD_COUNTER_MODE_YES_NO:
    case BCM_FIELD_COUNTER_MODE_DEFAULT:
        hw_entry_count = 1;
        break;

    default:
        hw_entry_count = 2;
        break;
    }

    entry_stat_p = &entry_p->statistic;

    if (((hw_entry_count == 1) && (mode % 2 == 0) && 
            (_FP_COUNTER_BMP_TEST(fs_p->counter_bmp, counter_index * 2))) ||
        ((hw_entry_count == 1) && (mode % 2) && 
            (_FP_COUNTER_BMP_TEST(fs_p->counter_bmp, counter_index * 2 + 1))) ||
        ((hw_entry_count == 2) &&
            (_FP_COUNTER_BMP_TEST(fs_p->counter_bmp, counter_index * 2)) &&
            (_FP_COUNTER_BMP_TEST(fs_p->counter_bmp, counter_index * 2 + 1))))
    {
        /* Counter previously recovered; increment reference count */

        for (hash_index = 0; hash_index < _FP_HASH_SZ(fc); ++hash_index)
        {
            stat_p = fc->stat_hash[hash_index];

            while (stat_p != NULL)
            {
                if ((stat_p->hw_index == counter_index) &&
                    (stat_p->pool_index == fs_p->slice_number) &&
                    (stat_p->hw_mode == mode) &&
                    (stat_p->stage_id == stage_id))
                {
                    /* Found the counter */

                    break;
                }

                stat_p = stat_p->next;
            }

            if (stat_p != NULL)
            {
                /* Found the counter */

                break;
            }
        }

        if (stat_p == NULL)
        {
            /* Counter's bit set in bitmap but reference not found;
               this shouldn't happen */

            return BCM_E_INTERNAL;
        }

        ++stat_p->hw_ref_count;
        ++stat_p->sw_ref_count;
    }
    else
    {
        /* Counter not previously recovered; create new stat object */

        if (fc->l2warm)
        {
            stat_id = sid;
        }
        else
        {
            BCM_IF_ERROR_RETURN(_bcm_field_stat_id_alloc(unit, &stat_id));
        }

        _FP_XGS3_ALLOC(stat_p, sizeof (_field_stat_t), "Field stat object");

        if (stat_p == NULL)
        {
            return BCM_E_MEMORY;
        }

        entry_stat_p->flags |= _FP_ENTRY_STAT_INSTALLED;

        stat_p->sw_ref_count = 2;
        stat_p->hw_ref_count = 1;
        stat_p->pool_index = fs_p->slice_number;
        stat_p->hw_index = counter_index;
        stat_p->sid = stat_id;
        stat_p->stage_id = fg_p->stage_id;
        stat_p->gid = fg_p->gid;
        stat_p->hw_mode = mode;

        if (fg_p->stage_id == _BCM_FIELD_STAGE_INGRESS)
        {
            switch (submode)
            {
                case (0x1):
                case (0x2):
                    stat_arr[0] = bcmFieldStatPackets;
                    stat_count = 1;
                    break;
                case (0x9):
                case (0xa):
                    stat_arr[0] = bcmFieldStatBytes;
                    stat_count = 1;
                    break;
                case (0x3):
                    stat_arr[0] = bcmFieldStatRedPackets;
                    stat_arr[1] = bcmFieldStatNotRedPackets;
                    stat_count = 2;
                    break;
                case (0xb):
                    stat_arr[0] = bcmFieldStatRedBytes;
                    stat_arr[1] = bcmFieldStatNotRedBytes;
                    stat_count = 2;
                    break;
                case (0x4):
                    stat_arr[0] = bcmFieldStatGreenPackets;                            
                    stat_arr[1] = bcmFieldStatNotGreenPackets;
                    stat_count = 2;
                    break;
                case (0xc):
                    stat_arr[0] = bcmFieldStatGreenBytes;
                    stat_arr[1] = bcmFieldStatNotGreenBytes;
                    stat_count = 2;
                    break;
                case (0x5):
                    stat_arr[0] = bcmFieldStatGreenPackets;
                    stat_arr[1] = bcmFieldStatRedPackets;
                    stat_count = 2;
                    break;
                case (0xd):
                    stat_arr[0] = bcmFieldStatGreenBytes;
                    stat_arr[1] = bcmFieldStatRedBytes;
                    stat_count = 2;
                    break;
                case (0x6):
                    stat_arr[0] = bcmFieldStatGreenPackets;
                    stat_arr[1] = bcmFieldStatYellowPackets;
                    stat_count = 2;
                    break;
                case (0xe):
                    stat_arr[0] = bcmFieldStatGreenBytes;
                    stat_arr[1] = bcmFieldStatYellowBytes;
                    stat_count = 2;
                    break;
                case (0x7):
                    stat_arr[0] = bcmFieldStatRedPackets;
                    stat_arr[1] = bcmFieldStatYellowPackets;
                    stat_count = 2;
                    break;
                case (0xf):
                    stat_arr[0] = bcmFieldStatRedBytes;
                    stat_arr[1] = bcmFieldStatYellowBytes;
                    stat_count = 2;
                    break;
                default:
                    sal_free(stat_p);
                    return BCM_E_INTERNAL;
            }
        }
        else if (fg_p->stage_id == _BCM_FIELD_STAGE_EGRESS)
        {
            stat_arr[0] = bcmFieldStatPackets;
        }

        rv = _bcm_field_stat_array_init(unit, fc, stat_p, stat_count, stat_arr);

        if (BCM_FAILURE(rv))
        {
            sal_free(stat_p);

            return rv;
        }

        if (part_index > 0)
        {
            entry_p->flags |= _FP_ENTRY_STAT_IN_SECONDARY_SLICE;
        }

        _FP_HASH_INSERT(fc->stat_hash, stat_p,
            stat_id & _FP_HASH_INDEX_MASK(fc));

        ++fc->stat_count;

        if (stat_p->hw_entry_count == 1)
        {
            if (mode % 2 == 0) {
                _FP_COUNTER_BMP_ADD(fs_p->counter_bmp, 2 * counter_index);
            } else {
                _FP_COUNTER_BMP_ADD(fs_p->counter_bmp, 2 * counter_index + 1);
            }
        }
        else
        {
            _FP_COUNTER_BMP_ADD(fs_p->counter_bmp, 2 * counter_index);
            _FP_COUNTER_BMP_ADD(fs_p->counter_bmp, 2 * counter_index + 1);
        }

        ++fg_p->group_status.counter_count;
    }

    entry_stat_p->flags |= _FP_ENTRY_STAT_VALID;
    entry_stat_p->sid = stat_p->sid;

    return BCM_E_NONE;
}

#define POLICY_FIELD_GET(_field_) \
    PolicyGet(unit, FP_POLICY_TABLEm, policy_entry_p, (_field_))

#define SET_PARAM0 \
    if (action_map_item_p->param0_field != INVALIDf) \
        param0 = POLICY_FIELD_GET(action_map_item_p->param0_field)

#define SET_ACTION_FROM_MAP(_map_p_) \
    action_map_item_p = (_map_p_) + field_value; \
    action = action_map_item_p->action; \
    SET_PARAM0;

typedef struct
{
    bcm_field_action_t action;
    soc_field_t param0_field;
    soc_field_t param1_field;
} _action_map_t;

/*
 * Function:
 *    _field_raven_meter_index_in_use
 * Purpose:
 *    Returns success if meter index/meter pair is in use.
 * Parameters:
 *     unit             - (IN) BCM device number.
 *     fs               - (IN) Slice where meter resides.
 *     meter_pair_mode  - (IN) Meter mode.
 *     meter_offset     - (IN) Odd/Even meter offset value.
 *     idx              - (IN) Meter pair index.
 * Returns:
 *     BCM_E_XXX
 */
STATIC
int _field_raven_meter_index_in_use(int unit,
                                 _field_slice_t *fs,
                                 uint32 meter_pair_mode,
                                 uint32 meter_offset,
                                 int idx)
{
    /* Input parameter check. */
    if (NULL == fs) {
        return (BCM_E_INTERNAL);
    }

    /* Meters are per-slice resource. */
    if (meter_pair_mode == BCM_FIELD_METER_MODE_FLOW
        && _FP_METER_BMP_TEST(fs->meter_bmp, ((idx * 2) + meter_offset))) {
        /* Flow mode meter index in use. */
        return (BCM_E_NONE);
    } else if (_FP_METER_BMP_TEST(fs->meter_bmp, (idx * 2))
               && _FP_METER_BMP_TEST(fs->meter_bmp, ((idx * 2) + 1))) {
        /* Non-Flow mode meter pair index in use. */
        return (BCM_E_NONE);
    }
    return (BCM_E_NOT_FOUND);
}

int _field_raven_meter_recover(int unit, _field_entry_t *entry_p,
    fp_policy_table_entry_t *policy_entry_p, int part_index,
    bcm_policer_t pid)
{
    uint8 mode;
    uint8 meter_index;
    int rv;
    _field_group_t *fg_p;
    _field_slice_t *fs_p;
    _field_control_t *fc;
    _field_stage_t *stage_fc;
    _field_stage_id_t stage_id;
    int hash_index;
    _field_policer_t *policer_p = NULL;
    bcm_policer_config_t *policer_config_p;
    int meter_offset = 0;
    int meter_table_index;
    uint32 ent[SOC_MAX_MEM_FIELD_WORDS];
    uint32 refresh_count;
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    uint32 bucket_size;
#endif
    int policer_id;
    _field_entry_policer_t *entry_policer_p;
    uint32 bucket_count;

    meter_index = POLICY_FIELD_GET(METER_INDEX_EVENf);
    mode = POLICY_FIELD_GET(METER_PAIR_MODEf);

    if ((mode == 0) && (meter_index == 0))
    {
        return BCM_E_NONE;
    }

    fg_p = entry_p->group;
    fs_p = entry_p[part_index].fs;
    stage_id = fg_p->stage_id;

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, stage_id, &stage_fc));

    if (BCM_FIELD_METER_MODE_FLOW == mode) {
        if (POLICY_FIELD_GET(METER_UPDATE_EVENf)
            && POLICY_FIELD_GET(METER_TEST_EVENf)) {
            meter_offset = 0;
        } else if (POLICY_FIELD_GET(METER_UPDATE_ODDf)
                    && POLICY_FIELD_GET(METER_TEST_ODDf)) {
            meter_offset = 1;
        } else {
            return (BCM_E_INTERNAL);
        }
    }

    if (BCM_SUCCESS(_field_raven_meter_index_in_use(unit, fs_p,
            mode, meter_offset, meter_index)))
    {
        /* Policer previously found; increment reference count */

        for (hash_index = 0; hash_index < _FP_HASH_SZ(fc); ++hash_index)
        {
            policer_p = fc->policer_hash[hash_index];

            while (policer_p != NULL)
            {
                if ((policer_p->hw_index == meter_index) &&
                    (policer_p->pool_index = fs_p->slice_number) &&
                    (policer_p->stage_id == stage_id))
                {
                    /* Found it */

                    break;
                }
            }

            if (policer_p != NULL)
            {
                /* Found it */

                break;
            }
        }

        if (policer_p == NULL)
        {
            /* Bit set in bitmap but reference not found; This should
               never happen */

            return BCM_E_INTERNAL;
        }

        ++policer_p->hw_ref_count;
        ++policer_p->sw_ref_count;
    }
    else
    {
        /* Policer not previously recovered; create new object */

        if (fc->l2warm)
        {
            policer_id = pid;
        }
        else
        {
            BCM_IF_ERROR_RETURN(_field_policer_id_alloc(unit, &policer_id));
        }

        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "Creating policer id %d.\n"),
                   policer_id));
        
        _FP_XGS3_ALLOC(policer_p, sizeof (_field_policer_t),
            "Field policer object");

        if (policer_p == NULL)
        {
            return BCM_E_MEMORY;
        }

        policer_p->sw_ref_count = 2;
        policer_p->hw_ref_count = 1;
        policer_p->pool_index = fs_p->slice_number;
        policer_p->hw_index = meter_index;
        policer_p->pid = policer_id;

        policer_config_p = &policer_p->cfg;
        
        switch (mode) {
            case BCM_FIELD_METER_MODE_DEFAULT:
                policer_config_p->mode = bcmPolicerModeGreen;
                break;
            case BCM_FIELD_METER_MODE_FLOW:
                policer_config_p->mode = bcmPolicerModeCommitted;
                if (meter_offset) {
                    /* Flow mode using committed hardware meter. */
                    _FP_POLICER_EXCESS_HW_METER_CLEAR(policer_p);
                } else {
                    /* Flow mode using excess hardware meter. */
                    _FP_POLICER_EXCESS_HW_METER_SET(policer_p);
                }
                meter_table_index =
                    stage_fc->slices[_FP_DEF_INST][policer_p->pool_index]
                    .start_tcam_idx + 2 * policer_p->hw_index + meter_offset;
                rv = soc_mem_read(unit, FP_METER_TABLEm, MEM_BLOCK_ANY,
                                  meter_table_index, ent);
                
                if (BCM_FAILURE(rv)) {
                    sal_free(policer_p);
                    return rv;
                }
                
                refresh_count = soc_mem_field32_get(unit, FP_METER_TABLEm,
                                                    ent, REFRESHCOUNTf);
                bucket_count = soc_mem_field32_get(unit, FP_METER_TABLEm,
                                                     ent, BUCKETCOUNTf);
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
                bucket_size = soc_mem_field32_get(unit, FP_METER_TABLEm,
                                                  ent, BUCKETSIZEf);
#endif
                policer_config_p->ckbits_sec = refresh_count
                                                * _FP_POLICER_REFRESH_RATE;
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
                if (SOC_IS_FIREBOLT2(unit) || SOC_IS_SC_CQ(unit)) {
                    policer_config_p->ckbits_burst
                        = _bcm_fb2_bucket_encoding_to_kbits(bucket_size, TRUE);
                } else
#endif
                {
                    policer_config_p->ckbits_burst = bucket_count / 2 / 1024;
                    /* Refer to _field_fb_bucket_calc */
                }

                break;

            case BCM_FIELD_METER_MODE_trTCM_COLOR_BLIND:
                policer_config_p->flags |= BCM_POLICER_COLOR_BLIND;
                /* Fall through */
            case BCM_FIELD_METER_MODE_trTCM_COLOR_AWARE:
                policer_config_p->mode = bcmPolicerModeTrTcm;
                /* Get the peak attributes */
                meter_table_index =
                    stage_fc->slices[_FP_DEF_INST][policer_p->pool_index]
                    .start_tcam_idx + (2 * policer_p->hw_index);
                rv = soc_mem_read(unit, FP_METER_TABLEm, MEM_BLOCK_ANY,
                                  meter_table_index, ent);
                if (BCM_FAILURE(rv)) {
                    sal_free(policer_p);
                    return rv;
                }
                refresh_count = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                    REFRESHCOUNTf);
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
                bucket_size = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                  BUCKETSIZEf);
#endif
                bucket_count = soc_mem_field32_get(unit, FP_METER_TABLEm,
                                                     ent, BUCKETCOUNTf);
                policer_config_p->pkbits_sec
                    = refresh_count * _FP_POLICER_REFRESH_RATE;

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
                if (SOC_IS_FIREBOLT2(unit) || SOC_IS_SC_CQ(unit)) {
                    policer_config_p->pkbits_burst
                        = _bcm_fb2_bucket_encoding_to_kbits(bucket_size, TRUE);
                } else
#endif
                {
                    policer_config_p->pkbits_burst = bucket_count / 2 / 1024;
                    /* Refer to _field_fb_bucket_calc */
                }
                
                /* Get the committed attributes */
                meter_table_index =
                    stage_fc->slices[_FP_DEF_INST][policer_p->pool_index]
                    .start_tcam_idx + (2 * policer_p->hw_index) + 1;
                rv = soc_mem_read(unit, FP_METER_TABLEm, MEM_BLOCK_ANY,
                                  meter_table_index, ent);
                if (BCM_FAILURE(rv)) {
                    sal_free(policer_p);
                    return rv;
                }

                refresh_count = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                    REFRESHCOUNTf);
                bucket_count = soc_mem_field32_get(unit, FP_METER_TABLEm,
                                                     ent, BUCKETCOUNTf);
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
                bucket_size = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                  BUCKETSIZEf);
#endif
                policer_config_p->ckbits_sec = refresh_count * _FP_POLICER_REFRESH_RATE;

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
                if (SOC_IS_FIREBOLT2(unit) || SOC_IS_SC_CQ(unit)) {
                    policer_config_p->ckbits_burst
                        = _bcm_fb2_bucket_encoding_to_kbits(bucket_size, TRUE);
                } else
#endif
                {
                    policer_config_p->ckbits_burst = bucket_count / 2 / 1024;
                    /* Refer to _field_fb_bucket_calc */
                }
                break;
            case 4:
                if (stage_id == _BCM_FIELD_STAGE_EGRESS) {
                    policer_config_p->mode = bcmPolicerModePassThrough;
                }
                break;
            case 5:
                break;
            case BCM_FIELD_METER_MODE_srTCM_COLOR_BLIND:
                policer_config_p->flags |= BCM_POLICER_COLOR_BLIND;
                /* Fall through */
            case BCM_FIELD_METER_MODE_srTCM_COLOR_AWARE:
                policer_config_p->mode = bcmPolicerModeSrTcm;
                /* Get the peak attributes */
                meter_table_index =
                    stage_fc->slices[_FP_DEF_INST][policer_p->pool_index]
                    .start_tcam_idx + (2 * policer_p->hw_index);
                rv = soc_mem_read(unit, FP_METER_TABLEm, MEM_BLOCK_ANY,
                                  meter_table_index, ent);
                if (BCM_FAILURE(rv)) {
                    sal_free(policer_p);
                    return rv;
                }
                refresh_count = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                    REFRESHCOUNTf);
                bucket_count = soc_mem_field32_get(unit, FP_METER_TABLEm,
                                                     ent, BUCKETCOUNTf);
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
                bucket_size = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                  BUCKETSIZEf);
#endif
                policer_config_p->pkbits_sec
                    = refresh_count * _FP_POLICER_REFRESH_RATE;
                
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
                if (SOC_IS_FIREBOLT2(unit) || SOC_IS_SC_CQ(unit)) {
                    policer_config_p->pkbits_burst
                        = _bcm_fb2_bucket_encoding_to_kbits(bucket_size, TRUE);
                } else
#endif
                {
                    policer_config_p->pkbits_burst = bucket_count / 2 / 1024;
                    /* Refer to _field_fb_bucket_calc */
                }
                
                /* Get the committed attributes */
                meter_table_index =
                    stage_fc->slices[_FP_DEF_INST][policer_p->pool_index]
                    .start_tcam_idx + (2 * policer_p->hw_index) + 1;
                rv = soc_mem_read(unit, FP_METER_TABLEm, MEM_BLOCK_ANY,
                                  meter_table_index, ent);
                if (BCM_FAILURE(rv)) {
                    sal_free(policer_p);
                    return rv;
                }

                refresh_count = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                    REFRESHCOUNTf);
                bucket_count = soc_mem_field32_get(unit, FP_METER_TABLEm,
                                                     ent, BUCKETCOUNTf);
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
                bucket_size = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                  BUCKETSIZEf);
#endif
                policer_config_p->ckbits_sec
                    = refresh_count * _FP_POLICER_REFRESH_RATE;
                
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
                if (SOC_IS_FIREBOLT2(unit) || SOC_IS_SC_CQ(unit)) {
                    policer_config_p->ckbits_burst
                        = _bcm_fb2_bucket_encoding_to_kbits(bucket_size, TRUE);
                } else
#endif
                {
                    policer_config_p->ckbits_burst = bucket_count / 2 / 1024;
                    /* Refer to _field_fb_bucket_calc */
                }
                break;
            default:
                break;
        }

        if (part_index > 0)
        {
            entry_p->flags |= _FP_ENTRY_POLICER_IN_SECONDARY_SLICE;
        }

        _FP_HASH_INSERT(fc->policer_hash, policer_p,
                        (policer_id & _FP_HASH_INDEX_MASK(fc)));
        ++fc->policer_count;

        /* Mark as used in the slice */
        if (!_FP_POLICER_IS_FLOW_MODE(policer_p)) {
            _FP_METER_BMP_ADD(fs_p->meter_bmp, (meter_index * 2));
            _FP_METER_BMP_ADD(fs_p->meter_bmp, ((meter_index * 2) + 1));
        } else {
            _FP_METER_BMP_ADD(fs_p->meter_bmp, ((meter_index * 2) + meter_offset));
        }
        ++fg_p->group_status.meter_count;
    }

    /* Associate policer with entry */
    entry_policer_p = entry_p->policer;
    entry_policer_p->flags |= _FP_POLICER_VALID;
    entry_policer_p->flags |= _FP_POLICER_INSTALLED;
    entry_policer_p->pid = policer_p->pid;

    return BCM_E_NONE;
}

int _field_raven_actions_recover(int unit,
    fp_policy_table_entry_t *policy_entry_p, _field_entry_t *entry_p,
        int part_index, _field_entry_wb_info_t *f_ent_wb_info)
{
    _action_map_t green_action_map[2][16] =
    {
        {
            {bcmFieldActionCount, INVALIDf, INVALIDf},
            {bcmFieldActionCosQNew, NEWPRIf, INVALIDf},
            {bcmFieldActionCosQCpuNew, NEWPRIf, INVALIDf},
            {bcmFieldActionCount, INVALIDf, INVALIDf},
            {bcmFieldActionPrioPktAndIntCopy, INVALIDf, INVALIDf},
            {bcmFieldActionPrioPktAndIntNew, NEWPRIf, INVALIDf},
            {bcmFieldActionPrioPktAndIntTos, INVALIDf, INVALIDf},
            {bcmFieldActionPrioPktAndIntCancel, INVALIDf, INVALIDf},
            {bcmFieldActionPrioPktCopy, INVALIDf, INVALIDf},
            {bcmFieldActionPrioPktNew, NEWPRIf, INVALIDf},
            {bcmFieldActionPrioPktTos, INVALIDf, INVALIDf},
            {bcmFieldActionPrioPktCancel, INVALIDf, INVALIDf},
            {bcmFieldActionPrioIntCopy, INVALIDf, INVALIDf},
            {bcmFieldActionPrioIntNew, NEWPRIf, INVALIDf},
            {bcmFieldActionPrioIntTos, INVALIDf, INVALIDf},
            {bcmFieldActionPrioIntCancel, INVALIDf, INVALIDf}
        },
        {
            {bcmFieldActionCount, INVALIDf, INVALIDf},
            {bcmFieldActionGpCosQNew, NEWPRIf, INVALIDf},
            {bcmFieldActionCount, INVALIDf, INVALIDf},
            {bcmFieldActionCount, INVALIDf, INVALIDf},
            {bcmFieldActionGpPrioPktAndIntCopy, INVALIDf, INVALIDf},
            {bcmFieldActionGpPrioPktAndIntNew, NEWPRIf, INVALIDf},
            {bcmFieldActionGpPrioPktAndIntTos, INVALIDf, INVALIDf},
            {bcmFieldActionGpPrioPktAndIntCancel, INVALIDf, INVALIDf},
            {bcmFieldActionGpPrioPktCopy, INVALIDf, INVALIDf},
            {bcmFieldActionGpPrioPktNew, NEWPRIf, INVALIDf},
            {bcmFieldActionGpPrioPktTos, INVALIDf, INVALIDf},
            {bcmFieldActionGpPrioPktCancel, INVALIDf, INVALIDf},
            {bcmFieldActionGpPrioIntCopy, INVALIDf, INVALIDf},
            {bcmFieldActionGpPrioIntNew, NEWPRIf, INVALIDf},
            {bcmFieldActionGpPrioIntTos, INVALIDf, INVALIDf},
            {bcmFieldActionGpPrioIntCancel, INVALIDf, INVALIDf}
        }
    };

    _action_map_t yellow_action_map[16] =
    {
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionYpCosQNew, NEWPRIf, INVALIDf},
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionYpPrioPktAndIntCopy, INVALIDf, INVALIDf},
        {bcmFieldActionYpPrioPktAndIntNew, NEWPRIf, INVALIDf},
        {bcmFieldActionYpPrioPktAndIntTos, INVALIDf, INVALIDf},
        {bcmFieldActionYpPrioPktAndIntCancel, INVALIDf, INVALIDf},
        {bcmFieldActionYpPrioPktCopy, INVALIDf, INVALIDf},
        {bcmFieldActionYpPrioPktNew, NEWPRIf, INVALIDf},
        {bcmFieldActionYpPrioPktTos, INVALIDf, INVALIDf},
        {bcmFieldActionYpPrioPktCancel, INVALIDf, INVALIDf},
        {bcmFieldActionYpPrioIntCopy, INVALIDf, INVALIDf},
        {bcmFieldActionYpPrioIntNew, NEWPRIf, INVALIDf},
        {bcmFieldActionYpPrioIntTos, INVALIDf, INVALIDf},
        {bcmFieldActionYpPrioIntCancel, INVALIDf, INVALIDf}
    };

    _action_map_t red_action_map[16] =
    {
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionRpCosQNew, NEWPRIf, INVALIDf},
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionRpPrioPktAndIntCopy, INVALIDf, INVALIDf},
        {bcmFieldActionRpPrioPktAndIntNew, NEWPRIf, INVALIDf},
        {bcmFieldActionRpPrioPktAndIntTos, INVALIDf, INVALIDf},
        {bcmFieldActionRpPrioPktAndIntCancel, INVALIDf, INVALIDf},
        {bcmFieldActionRpPrioPktCopy, INVALIDf, INVALIDf},
        {bcmFieldActionRpPrioPktNew, NEWPRIf, INVALIDf},
        {bcmFieldActionRpPrioPktTos, INVALIDf, INVALIDf},
        {bcmFieldActionRpPrioPktCancel, INVALIDf, INVALIDf},
        {bcmFieldActionRpPrioIntCopy, INVALIDf, INVALIDf},
        {bcmFieldActionRpPrioIntNew, NEWPRIf, INVALIDf},
        {bcmFieldActionRpPrioIntTos, INVALIDf, INVALIDf},
        {bcmFieldActionRpPrioIntCancel, INVALIDf, INVALIDf}
    };

    _action_map_t change_dscp_tos_action_map[8] =
    {
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionTosNew, NEWDSCP_TOSf, INVALIDf},
        {bcmFieldActionTosCopy, INVALIDf, INVALIDf},
        {bcmFieldActionDscpNew, NEWDSCP_TOSf, INVALIDf},
        {bcmFieldActionDscpCancel, INVALIDf, INVALIDf},
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionCount, INVALIDf, INVALIDf}
    };

    _action_map_t copy_to_cpu_action_map[4] =
    {
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionCopyToCpu, INVALIDf, MATCHED_RULEf},
        {bcmFieldActionCopyToCpuCancel, INVALIDf, INVALIDf},
        {bcmFieldActionSwitchToCpuCancel, INVALIDf, INVALIDf}
    };

    _action_map_t gp_copy_to_cpu_action_map[4] =
    {
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionGpCopyToCpu, INVALIDf, MATCHED_RULEf},
        {bcmFieldActionGpCopyToCpuCancel, INVALIDf, INVALIDf},
        {bcmFieldActionCount, INVALIDf, INVALIDf}
    };

    _action_map_t yp_copy_to_cpu_action_map[4] =
    {
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionYpCopyToCpu, INVALIDf, MATCHED_RULEf},
        {bcmFieldActionYpCopyToCpuCancel, INVALIDf, INVALIDf},
        {bcmFieldActionCount, INVALIDf, INVALIDf}
    };

    _action_map_t rp_copy_to_cpu_action_map[4] =
    {
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionRpCopyToCpu, INVALIDf, MATCHED_RULEf},
        {bcmFieldActionRpCopyToCpuCancel, INVALIDf, INVALIDf},
        {bcmFieldActionCount, INVALIDf, INVALIDf}
    };

    _action_map_t packet_redirection_action_map[8] =
    {
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionCount, INVALIDf, INVALIDf}, /* Handled in code */
        {bcmFieldActionRedirectCancel, INVALIDf, INVALIDf},
        {bcmFieldActionRedirectPbmp, REDIRECTIONf, INVALIDf},
        {bcmFieldActionEgressMask, REDIRECTIONf, INVALIDf},
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionCount, INVALIDf, INVALIDf}
    };

    _action_map_t drop_action_map[4] =
    {
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionDrop, INVALIDf, INVALIDf},
        {bcmFieldActionDropCancel, INVALIDf, INVALIDf},
        {bcmFieldActionCount, INVALIDf, INVALIDf}
    };

    _action_map_t yp_drop_action_map[4] =
    {
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionYpDrop, INVALIDf, INVALIDf},
        {bcmFieldActionYpDropCancel, INVALIDf, INVALIDf},
        {bcmFieldActionCount, INVALIDf, INVALIDf}
    };

    _action_map_t rp_drop_action_map[4] =
    {
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionRpDrop, INVALIDf, INVALIDf},
        {bcmFieldActionRpDropCancel, INVALIDf, INVALIDf},
        {bcmFieldActionCount, INVALIDf, INVALIDf}
    };

    _action_map_t l3sw_change_macda_or_vlan_action_map[8] =
    {
        {bcmFieldActionCount, INVALIDf, INVALIDf},
        {bcmFieldActionL3ChangeVlan, INVALIDf, INVALIDf},
        {bcmFieldActionL3ChangeVlanCancel, INVALIDf, INVALIDf},
        {bcmFieldActionAddClassTag, CLASSIFICATION_TAGf, INVALIDf},
        {bcmFieldActionL3ChangeMacDa, INVALIDf, INVALIDf},
        {bcmFieldActionL3ChangeMacDaCancel, INVALIDf, INVALIDf},
        {bcmFieldActionL3Switch, INVALIDf, INVALIDf},
        {bcmFieldActionL3SwitchCancel, INVALIDf, INVALIDf}
    };

    int field_index;
    bcm_field_action_t action;
    uint32 param0;
    uint32 param1;
    soc_field_t field;
    uint32 field_value;
    uint32 priority_mode;
    uint32 mtp_index;
    im_mtp_index_entry_t im_mtp_index_entry;
    em_mtp_index_entry_t em_mtp_index_entry;
    _field_action_t *fa_p;
    int ecmp_count;
    int nh_ecmp;
    _action_map_t *action_map_item_p;
    uint32 redirection_value;

    if (f_ent_wb_info == NULL) {
        return BCM_E_INTERNAL;
    }

    for (field_index = 0;
        field_index < SOC_MEM_INFO(unit, FP_POLICY_TABLEm).nFields;
        ++field_index)
    {
        action = bcmFieldActionCount;
        param0 = 0;
        param1 = 0;
        fa_p = NULL;

        field =
            SOC_MEM_INFO(unit, FP_POLICY_TABLEm).fields[field_index].field;

        field_value = POLICY_FIELD_GET(field);

        if (field_value != 0)
        {
            switch (field)
            {
            case CHANGE_PRIORITYf:
                priority_mode = POLICY_FIELD_GET(PRI_MODEf);

                SET_ACTION_FROM_MAP(green_action_map[priority_mode]);

                break;

            case YP_CHANGE_PRIORITYf:
                SET_ACTION_FROM_MAP(yellow_action_map);
                break;

            case RP_CHANGE_PRIORITYf:
                SET_ACTION_FROM_MAP(red_action_map);
                break;

            case ECN_CNGf:
                action = bcmFieldActionEcnNew;

                break;

            case DROP_PRECEDENCEf:
                action = bcmFieldActionDropPrecedence;
                param0 = field_value;

                break;

            case YP_DROP_PRECEDENCEf:
                /* Only set this action if all three aren't the same */

                if (field_value != POLICY_FIELD_GET(DROP_PRECEDENCEf) ||
                    field_value != POLICY_FIELD_GET(RP_DROP_PRECEDENCEf))
                {
                    action = bcmFieldActionYpDropPrecedence;
                    param0 = field_value;
                }

                break;

            case RP_DROP_PRECEDENCEf:
                /* Only set this action if all three aren't the same */

                if (field_value != POLICY_FIELD_GET(DROP_PRECEDENCEf) ||
                    field_value != POLICY_FIELD_GET(YP_DROP_PRECEDENCEf))
                {
                    action = bcmFieldActionRpDropPrecedence;
                    param0 = field_value;
                }

                break;

            case CHANGE_DSCP_TOSf:
                SET_ACTION_FROM_MAP(change_dscp_tos_action_map);
                break;

            case YP_CHANGE_DSCPf:
                action = bcmFieldActionYpDscpNew;
                param0 = POLICY_FIELD_GET(YP_DSCPf);
                break;

            case RP_CHANGE_DSCPf:
                action = bcmFieldActionRpDscpNew;
                param0 = POLICY_FIELD_GET(RP_DSCPf);
                break;

            case COPY_TO_CPUf:
                if (field_value == 1 && POLICY_FIELD_GET(COPY_TO_CPUf) == 1 &&
                    POLICY_FIELD_GET(YP_COPY_TO_CPUf) != 1 &&
                    POLICY_FIELD_GET(RP_COPY_TO_CPUf) != 1) {
                    /* Part of bcmFieldActionGpCopyToCpu */
                    SET_ACTION_FROM_MAP(gp_copy_to_cpu_action_map);
                } else if (field_value == 2 && POLICY_FIELD_GET(COPY_TO_CPUf)
                            == 2 && POLICY_FIELD_GET(YP_COPY_TO_CPUf) != 2
                            && POLICY_FIELD_GET(RP_COPY_TO_CPUf) != 2) {
                    /* Part of bcmFieldActionGpCopyToCpuCancel */
                    SET_ACTION_FROM_MAP(gp_copy_to_cpu_action_map);
                    break;
                } else {
                    SET_ACTION_FROM_MAP(copy_to_cpu_action_map);
                }
                param1 = POLICY_FIELD_GET(MATCHED_RULEf);
                if (param1 != 0)
                {
                    param0 = 1;
                }

                break;

            case YP_COPY_TO_CPUf:
                if (field_value == 1 && POLICY_FIELD_GET(COPY_TO_CPUf) == 1)
                {
                    /* Part of bcmFieldActionCopyToCpu */

                    break;
                }

                if (field_value == 2 && POLICY_FIELD_GET(COPY_TO_CPUf) == 2 &&
                    POLICY_FIELD_GET(RP_COPY_TO_CPUf) == 2)
                {
                    /* Part of bcmFieldActionCopyToCpuCancel */

                    break;
                }

                if (field_value == 3 && POLICY_FIELD_GET(COPY_TO_CPUf) == 3 &&
                     POLICY_FIELD_GET(RP_COPY_TO_CPUf) == 3)
                 {
                     /* Part of bcmFieldActionSwitchToCpuCancel */

                     break;
                 }

                SET_ACTION_FROM_MAP(yp_copy_to_cpu_action_map);
                param1 = POLICY_FIELD_GET(MATCHED_RULEf);
                if (param1 != 0)
                {
                    param0 = 1;
                }
                break;

            case RP_COPY_TO_CPUf:
                if (field_value == 1 && POLICY_FIELD_GET(COPY_TO_CPUf) == 1)
                {
                    /* Part of bcmFieldActionCopyToCpu */

                    break;
                }

                if (field_value == 2 && POLICY_FIELD_GET(COPY_TO_CPUf) == 2 &&
                    POLICY_FIELD_GET(YP_COPY_TO_CPUf) == 2)
                {
                    /* Part of bcmFieldActionCopyToCpuCancel */

                    break;
                }

                if (field_value == 3 && POLICY_FIELD_GET(COPY_TO_CPUf) == 3 &&
                     POLICY_FIELD_GET(YP_COPY_TO_CPUf) == 3)
                {
                    /* Part of bcmFieldActionSwitchToCpuCancel */

                    break;
                }

                SET_ACTION_FROM_MAP(rp_copy_to_cpu_action_map);
                param1 = POLICY_FIELD_GET(MATCHED_RULEf);
                if (param1 != 0)
                {
                    param0 = 1;
                }
                break;

            case PACKET_REDIRECTIONf:
                if (field_value == 1)
                {
                    redirection_value = POLICY_FIELD_GET(REDIRECTIONf);

                    if ((redirection_value & 0x40) != 0)
                    {
                        /* Trunk bit is set */

                        action = bcmFieldActionRedirectTrunk;
                        param0 = redirection_value & 0x3F;
                    }
                    else
                    {
                        /* Mod+port */

                        action = bcmFieldActionRedirect;
                        param0 = (redirection_value >> 7) & 0x0F;
                        param1 = redirection_value & 0x7F;
                    }
                }
                else
                {
                    SET_ACTION_FROM_MAP(packet_redirection_action_map);
                }

                break;

            case DROPf:
                SET_ACTION_FROM_MAP(drop_action_map);
                break;

            case YP_DROPf:
                /* Only set this action if not all three are the same */

                if (field_value != POLICY_FIELD_GET(DROPf) ||
                    field_value != POLICY_FIELD_GET(RP_DROPf))
                {
                    SET_ACTION_FROM_MAP(yp_drop_action_map);
                }

                break;

            case RP_DROPf:
                /* Only set this action if not all three are the same */

                if (field_value != POLICY_FIELD_GET(DROPf) ||
                    field_value != POLICY_FIELD_GET(YP_DROPf))
                {
                    SET_ACTION_FROM_MAP(rp_drop_action_map);
                }

                break;

            case MIRROR_OVERRIDEf:
                action = bcmFieldActionMirrorOverride;
                break;

            case MIRRORf:
                /* Because this field corresponds to two independent
                   actions simultaneously, actions are added directly
                   here rather than setting the action variable */

                if (field_value & 0x01)
                {
                    /* Ingress */

                    mtp_index = POLICY_FIELD_GET(IM_MTP_INDEXf);

                    SOC_IF_ERROR_RETURN(soc_mem_read(unit, IM_MTP_INDEXm,
                        MEM_BLOCK_ANY, mtp_index, &im_mtp_index_entry));

                    param0 = soc_IM_MTP_INDEXm_field32_get(unit,
                        &im_mtp_index_entry, MODULE_IDf);

                    param1 = soc_IM_MTP_INDEXm_field32_get(unit,
                        &im_mtp_index_entry, PORT_TGIDf);

                    BCM_IF_ERROR_RETURN(_field_action_alloc(unit,
                        bcmFieldActionMirrorIngress,
                        param0, param1, 0, 0, 0, 0, &fa_p));

                    /* Mark action installed */

                    fa_p->flags &= ~_FP_ACTION_DIRTY;

                    /* Add to entry's action list */

                    fa_p->next = entry_p->actions;
                    entry_p->actions = fa_p;
                }

                if (field_value & 0x02)
                {
                    /* Egress */

                    mtp_index = POLICY_FIELD_GET(EM_MTP_INDEXf);

                    SOC_IF_ERROR_RETURN(soc_mem_read(unit, EM_MTP_INDEXm,
                        MEM_BLOCK_ANY, mtp_index, &em_mtp_index_entry));

                    param0 = soc_EM_MTP_INDEXm_field32_get(unit,
                        &em_mtp_index_entry, MODULE_IDf);

                    param1 = soc_EM_MTP_INDEXm_field32_get(unit,
                        &em_mtp_index_entry, PORT_TGIDf);

                    BCM_IF_ERROR_RETURN(_field_action_alloc(unit,
                        bcmFieldActionMirrorEgress,
                        param0, param1, 0, 0, 0, 0, &fa_p));

                    /* Mark action installed */

                    fa_p->flags &= ~_FP_ACTION_DIRTY;

                    /* Add to entry's action list */

                    fa_p->next = entry_p->actions;
                    entry_p->actions = fa_p;
                }

                break;

            case L3SW_CHANGE_MACDA_OR_VLANf:
                SET_ACTION_FROM_MAP(l3sw_change_macda_or_vlan_action_map);

                if (field_value == 1 || field_value == 4 || field_value == 6)
                {
                    if (POLICY_FIELD_GET(ECMPf) != 0)
                    {
                        ecmp_count = POLICY_FIELD_GET(ECMP_COUNTf) + 1;
                        nh_ecmp = POLICY_FIELD_GET(ECMP_PTRf);

                        param0 = _FP_L3_ACTION_PACK_ECMP(nh_ecmp, ecmp_count);
                    }
                    else
                    {
                        nh_ecmp = POLICY_FIELD_GET(NEXT_HOP_INDEXf);

                        param0 = _FP_L3_ACTION_PACK_NEXT_HOP(nh_ecmp);
                    }
                }

                break;

            case COUNTER_MODEf:
                BCM_IF_ERROR_RETURN(_field_raven_counter_recover(unit,
                    entry_p, field_value, POLICY_FIELD_GET(COUNTER_INDEXf),
                    part_index, f_ent_wb_info->sid));

                break;

            case METER_PAIR_MODEf:
                BCM_IF_ERROR_RETURN(_field_raven_meter_recover(unit,
                    entry_p, policy_entry_p, part_index, f_ent_wb_info->pid));

                break;

            default:
                break;
            }

            if (action != bcmFieldActionCount)
            {
                /* Attach to entry's action list */

                BCM_IF_ERROR_RETURN(_field_action_alloc(unit, action,
                    param0, param1, 0, 0, 0, 0, &fa_p));

                /* Mark action installed */

                fa_p->flags &= ~_FP_ACTION_DIRTY;

                fa_p->next = entry_p->actions;
                entry_p->actions = fa_p;
            }
        }
    }

    return BCM_E_NONE;
}

#define _FP_RAVEN_IF_FAILURE_CLEANUP(op) \
    rv = (op); if (BCM_FAILURE(rv)) goto cleanup;

int _field_raven_stage_reinit(int unit, _field_control_t *fc, 
    _field_stage_t *stage_fc)
{
    int temp, idx1;
    int rv, i, k, qual_idx, size;
    int *slice_is_expanded_flags = NULL;
    int *slice_master_idx = NULL;
    char *fp_tcam_buffer = NULL;
    char *fp_port_field_sel_buffer = NULL;
    int slice_index;
    int vslice_index;
    int slice_mode;
    int max;
    int master_slice;
    _field_slice_t *fs_p;
    int entry_index;
    bcm_port_t port;
    _field_raven_sel_codes_t sel_codes[_fieldRavenSliceModeCount];
    _field_raven_sel_codes_t *sel_codes_p;
    int port_sel_entry_index;
    char *fp_policy_table_buffer = NULL;
    fp_tcam_entry_t *fp_tcam_entry_p;
    fp_port_field_sel_entry_t *port_field_sel_entry_p;
    uint32 port_enabled_for_entry;
    bcm_pbmp_t entry_pbmp;
    _field_group_t *fg_p;
    _field_slice_t *group_slice_p;
    int part_count;
    _field_entry_t *entries_p;
    int primary_tcam_index;
    int part_slice_index;
    int part_tcam_index;
    int part_index;
    _field_entry_t *entry_p = NULL;
    _bcm_field_group_qual_t *part_qualifier_p;
    int group_qualifier_index;
    int qualifier_id;
    _bcm_field_qual_info_t *stage_qualifier_p;
    int config_index;
    _bcm_field_qual_offset_t *config_offset_p;
    _bcm_field_qual_offset_t *group_qualifier_offset_p;
    fp_policy_table_entry_t *policy_entry_p;
    _bcm_field_selector_t config_selector;
    _bcm_field_slice_sel_t selector;
    uint8 selector_value;
    int multigroup;
    int prev_prio;
    int slice_is_primary;
    int extra_qual; 
    char pbmp_string[SOC_PBMP_FMT_LEN];
    uint32 hw_data, hw_mask;
    _bcm_field_qual_info_t *f_qual_arr = NULL;
    _bcm_field_qual_offset_t *q_offset_arr = NULL;
    uint16 *qid_arr = NULL;
    uint8 *buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    uint8 *buf1 = fc->scache_ptr[_FIELD_SCACHE_PART_1];
#if defined(BCM_RAVEN_SUPPORT)
    uint32 inner_vlan_overlay = _FP_SELCODE_DONT_CARE, rval;
#endif
    _field_entry_wb_info_t f_ent_wb_info;
    bcm_pbmp_t temp_pbmp;
    _field_slice_t *fs_temp = NULL;
    uint8 old_physical_slice, slice_num;
    uint32 entry_flags;
    soc_field_t slice_f1_fields[16] =
    {
        SLICE0_F1f, SLICE1_F1f, SLICE2_F1f, SLICE3_F1f,
        SLICE4_F1f, SLICE5_F1f, SLICE6_F1f, SLICE7_F1f,
        SLICE8_F1f, SLICE9_F1f, SLICE10_F1f, SLICE11_F1f,
        SLICE12_F1f, SLICE13_F1f, SLICE14_F1f, SLICE15_F1f,
    };

    soc_field_t slice_f2_fields[16] =
    {
        SLICE0_F2f, SLICE1_F2f, SLICE2_F2f, SLICE3_F2f,
        SLICE4_F2f, SLICE5_F2f, SLICE6_F2f, SLICE7_F2f,
        SLICE8_F2f, SLICE9_F2f, SLICE10_F2f, SLICE11_F2f,
        SLICE12_F2f, SLICE13_F2f, SLICE14_F2f, SLICE15_F2f,
    };

    soc_field_t slice_f3_fields[16] =
    {
        SLICE0_F3f, SLICE1_F3f, SLICE2_F3f, SLICE3_F3f,
        SLICE4_F3f, SLICE5_F3f, SLICE6_F3f, SLICE7_F3f,
        SLICE8_F3f, SLICE9_F3f, SLICE10_F3f, SLICE11_F3f,
        SLICE12_F3f, SLICE13_F3f, SLICE14_F3f, SLICE15_F3f,
    };

    soc_field_t slice_f4_fields[16] =
    {
        SLICE0_F4f, SLICE1_F4f, SLICE2_F4f, SLICE3_F4f,
        SLICE4_F4f, SLICE5_F4f, SLICE6_F4f, SLICE7_F4f,
        SLICE8_F4f, SLICE9_F4f, SLICE10_F4f, SLICE11_F4f,
        SLICE12_F4f, SLICE13_F4f, SLICE14_F4f, SLICE15_F4f,
    };

    _field_action_bmp_t action_bmp;
    
    /* Reset Action bitmap to NULL. */
    action_bmp.w = NULL;

    sal_memset(sel_codes, _FP_SELCODE_DONT_CARE, sizeof(sel_codes));

    fc->scache_pos = 0;
    fc->scache_pos1 = 0;

    if (fc->l2warm) {
        if ((fc->wb_recovered_version) > (fc->wb_current_version)) {
            /* Notify the application with an event */
            /* The application will then need to reconcile the 
               version differences using the documented behavioral
               differences on per module (handle) basis */
            SOC_IF_ERROR_RETURN
                (soc_event_generate(unit, SOC_SWITCH_EVENT_WARM_BOOT_DOWNGRADE, 
                                    BCM_MODULE_FIELD, (fc->wb_recovered_version), 
                                    (fc->wb_current_version)));        
        } 

        fc->scache_pos += SOC_WB_SCACHE_CONTROL_SIZE;

        if(NULL != fc->scache_ptr[_FIELD_SCACHE_PART_1]) {
            if ((fc->wb_recovered_version) > (fc->wb_current_version)) {
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

    if (stage_fc->stage_id != _BCM_FIELD_STAGE_INGRESS)
    {
        return BCM_E_PARAM;
    }

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "Beginning ingress stage recovery.\n")));

    if (fc->l2warm) {
        rv = _field_scache_stage_hdr_chk(fc, _FIELD_IFP_DATA_START);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
    }

    /* Read TCAM into memory */
    _FP_RAVEN_IF_FAILURE_CLEANUP(_field_table_read(unit, FP_TCAMm,
        &fp_tcam_buffer, "FP_TCAM buffer"));

    /* Read FP_PORT_FIELD_SEL into memory */
    _FP_RAVEN_IF_FAILURE_CLEANUP(_field_table_read(unit,
        FP_PORT_FIELD_SELm, &fp_port_field_sel_buffer,
        "FP_PORT_FIELD_SEL buffer"));

    /* Read FP_POLICY_TABLE table into memory */
    _FP_RAVEN_IF_FAILURE_CLEANUP(_field_table_read(unit,
        FP_POLICY_TABLEm, &fp_policy_table_buffer,
        "FP_POLICY_TABLE"));

    /* Recover range checkers */
    rv = _field_range_check_reinit(unit, stage_fc, fc);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Recover data qualifiers */
    rv = _field_data_qual_recover(unit, fc, stage_fc);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

#if defined(BCM_RAVEN_SUPPORT)
    /* Get per-device selectors */
    if ((rv = READ_ING_MISC_CONFIG2r(unit, &rval)) < 0) {
        goto cleanup;
    }
    inner_vlan_overlay = soc_reg_field_get(unit, ING_MISC_CONFIG2r, rval,
                                           FP_INNER_VLAN_OVERLAY_ENABLEf);         
#endif
    
    slice_is_expanded_flags =
        sal_alloc(stage_fc->tcam_slices * sizeof(int),
            "slice_is_expanded_flags");

    if (slice_is_expanded_flags == NULL)
    {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(slice_is_expanded_flags, 0, stage_fc->tcam_slices * sizeof(int));

    slice_master_idx =
        sal_alloc(stage_fc->tcam_slices * sizeof(int),
            "slice_is_expanded_flags");

    if (slice_master_idx == NULL)
    {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(slice_master_idx, 0, stage_fc->tcam_slices * sizeof(int));

    _FP_RAVEN_IF_FAILURE_CLEANUP(_field_slice_expanded_status_get(unit,
        fc, stage_fc, slice_is_expanded_flags, slice_master_idx));

    for (slice_index = 0; slice_index < stage_fc->tcam_slices; ++slice_index)
    {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "  Checking slice %d...\n"),
                   slice_index));

        _FP_RAVEN_IF_FAILURE_CLEANUP(_field_fb_slice_is_primary(unit,
            slice_index, &slice_is_primary, &slice_mode));

        if (!slice_is_primary)
        {
            /* Don't need selectors for second slice of double or for
               second and third slices of triple */

            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "    Not a primary slice.\n")));

            continue;
        }

        if (slice_is_expanded_flags[slice_index] != 0)
        {
            /* Don't need selectors for an expanded slice */

            continue;
        }

        /* Check if the slice has any valid entries */

        fs_p = stage_fc->slices[_FP_DEF_INST] + slice_index;

        for (entry_index = 0; entry_index < fs_p->entry_count; ++entry_index)
        {
            fp_tcam_entry_p = soc_mem_table_idx_to_pointer(unit, FP_TCAMm,
                fp_tcam_entry_t *, fp_tcam_buffer,
                entry_index + fs_p->entry_count * slice_index);

            if (soc_FP_TCAMm_field32_get(unit, fp_tcam_entry_p, VALIDf) != 0)
            {
                /* There's at least one valid entry in the slice */

                break;
            }
        }

        if ((entry_index == fs_p->entry_count) && !fc->l2warm)
        {
            /* No valid groups and entries in the slice */

            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "    No valid groups and entries in slice.\n")));

            continue;
        }

        if (fc->l2warm)
        {
            /* Retrieve the group IDs in this slice */

            rv = _field_scache_slice_group_recover(unit, fc,
                slice_index, &multigroup);

            if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                fc->l2warm = 0;
                goto cleanup;
            }
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_NONE;
                continue;
            }
        }

        PBMP_ALL_ITER(unit, port)
        {
            port_field_sel_entry_p = soc_mem_table_idx_to_pointer(
                unit, FP_PORT_FIELD_SELm, fp_port_field_sel_entry_t *,
                fp_port_field_sel_buffer, port);

            for (part_index = 0; part_index <= slice_mode; ++part_index)
            {
                sel_codes_p = sel_codes + part_index;

                sel_codes_p->fpf1 = soc_FP_PORT_FIELD_SELm_field32_get(
                    unit, port_field_sel_entry_p,
                        slice_f1_fields[slice_index + part_index]);

                sel_codes_p->fpf2 = soc_FP_PORT_FIELD_SELm_field32_get(
                    unit, port_field_sel_entry_p,
                        slice_f2_fields[slice_index + part_index]);

                sel_codes_p->fpf3 = soc_FP_PORT_FIELD_SELm_field32_get(
                    unit, port_field_sel_entry_p,
                        slice_f3_fields[slice_index + part_index]);

                sel_codes_p->fpf4 = soc_FP_PORT_FIELD_SELm_field32_get(
                    unit, port_field_sel_entry_p,
                        slice_f4_fields[slice_index + part_index]);

#if defined(BCM_RAVEN_SUPPORT)
                if (inner_vlan_overlay) {
                    sel_codes_p->inner_vlan_overlay = inner_vlan_overlay;
                } else 
#endif
                {
                    sel_codes_p->inner_vlan_overlay = _FP_SELCODE_DONT_CARE;
                }
            }

            _FP_RAVEN_IF_FAILURE_CLEANUP(_field_raven_group_construct(unit,
                fc, slice_mode, stage_fc->stage_id, slice_index, port,
                    sel_codes));
        }

        /* Adjust group's QSET to potentially add InPort/InPorts */

        for (fg_p = fc->groups; fg_p != NULL; fg_p = fg_p->next) {
            for (extra_qual = 0; extra_qual < 2; extra_qual++) {
                if (extra_qual == 1) {
                    qual_idx = bcmFieldQualifyInPort;
                } else {
                    qual_idx = bcmFieldQualifyInPorts;
                }
                f_qual_arr = stage_fc->f_qual_arr[qual_idx];
                BCM_FIELD_QSET_ADD(fg_p->qset, qual_idx);
                /* Allocate memory for this qualfier and its
                   associated offset information if not already added 
                   to fg->qual_arr[i] */
                _FP_RAVEN_IF_FAILURE_CLEANUP(
                    _bcm_field_entry_tcam_parts_count(unit, fg_p->stage_id,
                                                      fg_p->flags, &part_count));
                for (i = 0; i < part_count; i++) {
                    for (k = 0; k < fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size; k++) {
                        if (fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr[k] == qual_idx) {
                            break; /* Qualifier already added */
                        }
                    }
                    if (k == fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size) {
                        size = fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size + 1;
                        qid_arr = NULL;
                        _FP_XGS3_ALLOC(qid_arr, size * sizeof (uint16), 
                                       "Group qual id");
                        if (NULL == qid_arr) {
                            rv = BCM_E_MEMORY;
                            goto cleanup;
                        }              
                        q_offset_arr = NULL;              
                        _FP_XGS3_ALLOC(q_offset_arr, size * 
                                       sizeof (_bcm_field_qual_offset_t), 
                                       "Group qual offset");
                        if (NULL == q_offset_arr) {
                            sal_free (qid_arr);
                            rv = BCM_E_MEMORY;
                            goto cleanup;
                        }
                        if (size) {
                            sal_memcpy(qid_arr, fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr,
                                       fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size * 
                                       sizeof (uint16));
                            sal_memcpy(q_offset_arr, 
                                       fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr, 
                                       fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size * 
                                       sizeof (_bcm_field_qual_offset_t));
                            if (fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size) {
                                sal_free(fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr);
                                sal_free(fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr);
                            }
                        }
                        fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr = qid_arr;
                        fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr = q_offset_arr;
                        fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr[size - 1] = qual_idx;
                        sal_memcpy(&fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr[size - 1],
                                   &f_qual_arr->conf_arr[0].offset,
                                   sizeof(f_qual_arr->conf_arr[0].offset));
                        fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size = size;
                        break; /* Just added the qualifier */
                    }
                }
            }
        }

        /* Recover entries in this slice */

        fs_p = stage_fc->slices[_FP_DEF_INST] + slice_index;
        prev_prio = -1;

        for (entry_index = 0; entry_index < fs_p->entry_count; ++entry_index)
        {
            fp_tcam_entry_p = soc_mem_table_idx_to_pointer(unit, FP_TCAMm,
                fp_tcam_entry_t *, fp_tcam_buffer, entry_index +
                    slice_index * fs_p->entry_count);

            if (soc_FP_TCAMm_field32_get(unit, fp_tcam_entry_p, VALIDf) == 0)
            {
                /* Skip empty entry */

                continue;
            }

            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "      Checking entry %d...\n"),
                       entry_index));

            /* Determine pbmp for this entry */

            BCM_PBMP_CLEAR(entry_pbmp);
            PBMP_ALL_ITER(unit, port)
            {
                port_sel_entry_index = _BCM_RP_FIELD_IPBM_INDEX(unit,
                    port, slice_index);

                _FP_RAVEN_IF_FAILURE_CLEANUP(
                    _field_raptor_ipbm_entry_get(unit, port_sel_entry_index,
                        entry_index, &port_enabled_for_entry));

                if (port_enabled_for_entry)
                {
                    BCM_PBMP_PORT_ADD(entry_pbmp, port);
                }
            }

            SOC_PBMP_FMT(entry_pbmp, pbmp_string);
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "Entry PBMP: %s\n"),
                       pbmp_string));

            /* Find a matching group */

            for (fg_p = fc->groups; fg_p != NULL; fg_p = fg_p->next)
            {
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "        Checking group %d...\n"),
                           fg_p->gid));

                group_slice_p = fg_p->slices;

                if (group_slice_p->slice_number != slice_index)
                {
                    /* Not this slice */

                    continue;
                }

                SOC_PBMP_FMT(fg_p->pbmp, pbmp_string);
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "Group PBMP: %s\n"),
                           pbmp_string));

                SOC_PBMP_CLEAR(temp_pbmp);
                SOC_PBMP_ASSIGN(temp_pbmp, fg_p->pbmp);
                SOC_PBMP_OR(temp_pbmp, PBMP_LB(unit));
                SOC_PBMP_AND(temp_pbmp, entry_pbmp);
                if (SOC_PBMP_EQ(temp_pbmp, entry_pbmp))
                {
                    /* Found a match */

                    break;
                }

                /* Check if entry pbmp is a subset of the group pbmp */

                SOC_PBMP_ITER(entry_pbmp, port)
                {
                    if (SOC_PBMP_MEMBER(fg_p->pbmp, port) == 0)
                    {
                        break;
                    }
                }

                if (port == SOC_PBMP_PORT_MAX)
                {
                    /* Got all the way through with no mismatches */

                    break;
                }
            }

            if (fg_p == NULL)
            {
                /* This should never happen */
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "Could not find a matching group for entry %d.\n"),
                           entry_index));
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }

            /* Allocate the entry */

            _FP_RAVEN_IF_FAILURE_CLEANUP(
                _bcm_field_entry_tcam_parts_count(unit, fg_p->stage_id, 
                                                  fg_p->flags, &part_count));

            entries_p = NULL;

            _FP_XGS3_ALLOC(entries_p, part_count * sizeof (_field_entry_t),
                "field entry");

            if (entries_p == NULL)
            {
                rv = BCM_E_MEMORY;

                goto cleanup;
            }

            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "        Entry matches group %d.\n"),
                       fg_p->gid));

            sal_memset(&f_ent_wb_info, 0, sizeof(_field_entry_wb_info_t));
            f_ent_wb_info.sid = -1;
            f_ent_wb_info.pid = -1;

            if (fc->l2warm)
            {
                if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
                   action_bmp.w = NULL;
                   _FP_XGS3_ALLOC(action_bmp.w,
                             SHR_BITALLOCSIZE(_bcmFieldActionNoParamCount),
                             "Action No Param Bitmap");
                   if (action_bmp.w == NULL) {
                      rv = BCM_E_MEMORY;
                      sal_free(entries_p);
                      goto cleanup;
                   }
                }
                f_ent_wb_info.action_bmp = &action_bmp;
                   
                rv = _field_entry_info_retrieve(unit, fc, stage_fc,
                    multigroup, &prev_prio, &f_ent_wb_info);

                if (BCM_FAILURE(rv))
                {
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "Failed to retrieve entry info.")));
                    sal_free(entries_p);
                    goto cleanup;
                }
            }
            else
            {
                _bcm_field_last_alloc_eid_incr(unit);

                f_ent_wb_info.eid = _bcm_field_last_alloc_eid_get(unit);
            }

            vslice_index = _field_physical_to_virtual(unit, _FP_DEF_INST,
                    slice_index, stage_fc);
            if (vslice_index == -1)
            {
                rv = BCM_E_INTERNAL;
                sal_free(entries_p);
                goto cleanup;
            }

            primary_tcam_index =
                slice_index * fs_p->entry_count + entry_index;

            for (part_index = 0; part_index < part_count; ++part_index)
            {
                entry_p = entries_p + part_index;

                entry_p->eid = f_ent_wb_info.eid;
                entry_p->group = fg_p;
                /* Set retrieved dvp_type and svp_type */
                entry_p->dvp_type = f_ent_wb_info.dvp_type[part_index];
                entry_p->svp_type = f_ent_wb_info.svp_type[part_index];

                if (part_index == 0) {
                    BCM_PBMP_ASSIGN(entry_p->pbmp.data, entry_pbmp);
                    BCM_PBMP_ASSIGN(entry_p->pbmp.mask, PBMP_ALL(unit));
                }

                /* Set entry flags */

                rv = _bcm_field_tcam_part_to_entry_flags(unit, part_index,
                        fg_p, &entry_p->flags);

                if (BCM_FAILURE(rv)) {
                    sal_free(entries_p);
                    goto cleanup;
                }

                if (f_ent_wb_info.color_independent)
                {
                    entry_p->flags |= _FP_ENTRY_COLOR_INDEPENDENT;
                }

                /* Get index of entry in tcam */

                rv = _bcm_field_entry_part_tcam_idx_get(unit, entries_p,
                        primary_tcam_index, part_index, &part_tcam_index);

                if (BCM_FAILURE(rv)) {
                    sal_free(entries_p);
                    goto cleanup;
                }

                /* Get slice and index within the slice */

                rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc,
                        _FP_DEF_INST, part_tcam_index, &part_slice_index,
                        (int *) (&entry_p->slice_idx));

                if (BCM_FAILURE(rv)) {
                    sal_free(entries_p);
                    goto cleanup;
                }

                /* Point the entry to its slice */

                entry_p->fs = stage_fc->slices[_FP_DEF_INST] + part_slice_index;
                --entry_p->fs->free_count;

                /* Set the slice pbmp */

                BCM_PBMP_OR(entry_p->fs->pbmp, fg_p->pbmp);

                /* Assign entry to slice */

                entry_p->fs->entries[entry_p->slice_idx] = entry_p;
                entry_p->flags |= _FP_ENTRY_INSTALLED;

                if (soc_FP_TCAMm_field32_get(unit, fp_tcam_entry_p, VALIDf) == 3) {
                    entry_p->flags |= _FP_ENTRY_ENABLED;
                }

                fp_tcam_entry_p =
                    (fp_tcam_entry_t *) soc_mem_table_idx_to_pointer(unit,
                    FP_TCAMm, fp_tcam_entry_t *, fp_tcam_buffer,
                    part_tcam_index);

                part_qualifier_p = &(fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part_index]);

                for (group_qualifier_index = 0;
                    group_qualifier_index < part_qualifier_p->size;
                    ++group_qualifier_index)
                {
                    qualifier_id =
                        part_qualifier_p->qid_arr[group_qualifier_index];

                    stage_qualifier_p = stage_fc->f_qual_arr[qualifier_id];

                    for (config_index = 0;
                        config_index < stage_qualifier_p->conf_sz;
                        ++config_index)
                    {
                        config_offset_p = &((stage_qualifier_p->conf_arr +
                            config_index)->offset);

                        group_qualifier_offset_p =
                            part_qualifier_p->offset_arr +
                                group_qualifier_index;

                        if (_mask_is_set(unit, config_offset_p,
                            (uint32 *) fp_tcam_entry_p,
                                stage_fc->stage_id) != 0)
                        {
                            config_selector = stage_qualifier_p->
                                conf_arr[config_index].selector;

                            selector = config_selector.pri_sel;
                            selector_value = config_selector.pri_sel_val;

                            if ((selector == _bcmFieldSliceSelFpf1 &&
                                selector_value == sel_codes[part_index].fpf1) ||
                                (selector == _bcmFieldSliceSelFpf2 &&
                                selector_value == sel_codes[part_index].fpf2) ||
                                (selector == _bcmFieldSliceSelFpf3 &&
                                selector_value == sel_codes[part_index].fpf3) ||
                                (selector == _bcmFieldSliceSelFpf4 &&
                                selector_value == sel_codes[part_index].fpf4))
                            {
                                /* This qualifier exists in this entry */

                                *group_qualifier_offset_p = *config_offset_p;

                                /* No need to check more configs */

                                break;
                            }
                        }
                    }
                }

                /* Get IP type and mask values. */
                hw_data = soc_FP_TCAMm_field32_get(unit, 
                                                   fp_tcam_entry_p, IP_TYPEf); 
                hw_mask = soc_FP_TCAMm_field32_get(unit,
                                                   fp_tcam_entry_p,
                                                   IP_TYPE_MASKf); 

                if ((hw_data == 0) && (hw_mask == 0)) {
                    entry_p->tcam.ip_type = bcmFieldIpTypeAny;
                } else if ((hw_data == 0) && (hw_mask == 3)) {
                    entry_p->tcam.ip_type = bcmFieldIpTypeNonIp;
                } else if ((hw_data == 0) && (hw_mask == 1)) {
                    entry_p->tcam.ip_type = bcmFieldIpTypeIpv4Not;
                } else if ((hw_data == 1) && (hw_mask == 3)) {
                    entry_p->tcam.ip_type = bcmFieldIpTypeIpv4NoOpts;
                } else if ((hw_data == 3) && (hw_mask == 3)) {
                    entry_p->tcam.ip_type = bcmFieldIpTypeIpv4WithOpts;
                } else if ((hw_data == 1) && (hw_mask == 1)) {
                    entry_p->tcam.ip_type  = bcmFieldIpTypeIpv4Any;
                } else if ((hw_data == 2) && (hw_mask == 3)) {
                    entry_p->tcam.ip_type = bcmFieldIpTypeIpv6;
                }

                /* Get Higig data and mask values. */
                hw_data = soc_FP_TCAMm_field32_get(unit,
                                                   fp_tcam_entry_p, HIGIG_PKTf);
                hw_mask = soc_FP_TCAMm_field32_get(unit,
                                                   fp_tcam_entry_p,
                                                   HIGIG_PKT_MASKf);
                entry_p->tcam.higig      = hw_data;
                entry_p->tcam.higig_mask = hw_mask;

                /* Get the actions for this part of the entry */

                policy_entry_p = soc_mem_table_idx_to_pointer(unit,
                    FP_POLICY_TABLEm, fp_policy_table_entry_t *,
                    fp_policy_table_buffer, part_tcam_index);

                rv = _field_raven_actions_recover(unit, policy_entry_p,
                        entries_p, part_index, &f_ent_wb_info);

                if (BCM_FAILURE(rv)) {
                    sal_free(entries_p);
                    goto cleanup;
                }

                if (action_bmp.w != NULL) {
                   _FP_ACTION_BMP_FREE(action_bmp);
                   action_bmp.w = NULL;
                }
                entry_p->prio = fc->l2warm ? f_ent_wb_info.prio :
                    (vslice_index << 10) | (fs_p->entry_count - entry_index);

            }
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "        Entry id = %d, priority = %d.\n"),
                       entries_p->eid,
                       entries_p->prio));

            /* Add the entry to the group */

            rv = _field_group_entry_add(unit, fg_p, entries_p);
            if (BCM_FAILURE(rv)) {
                sal_free(entries_p);
                goto cleanup;
            }
        }

        if (fc->l2warm)
        {
            _field_scache_slice_group_free(unit, fc, slice_index);
        }
    }

    /* Now go over the expanded slices */
    for (slice_index = 0; slice_index < stage_fc->tcam_slices; ++slice_index)
    {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "  Checking slice %d...\n"),
                   slice_index));

        if (slice_is_expanded_flags[slice_index] == 0)
        {
            continue;
        }

        _FP_RAVEN_IF_FAILURE_CLEANUP(_field_fb_slice_is_primary(unit,
            slice_index, &slice_is_primary, &slice_mode));

        if (!slice_is_primary)
        {
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "    Not a primary slice.\n")));
            continue;
        }

        /* Check if the slice has any valid entries */

        fs_p = stage_fc->slices[_FP_DEF_INST] + slice_index;

        for (entry_index = 0; entry_index < fs_p->entry_count; ++entry_index)
        {
            fp_tcam_entry_p = soc_mem_table_idx_to_pointer(unit, FP_TCAMm,
                fp_tcam_entry_t *, fp_tcam_buffer,
                entry_index + fs_p->entry_count * slice_index);

            if (soc_FP_TCAMm_field32_get(unit, fp_tcam_entry_p, VALIDf) != 0)
            {
                /* There's at least one valid entry in the slice */

                break;
            }
        }

        if (entry_index == fs_p->entry_count)
        {
            /* No valid entries in the slice */

            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "    No valid entries in slice.\n")));

            continue;
        }

        if (fc->l2warm)
        {
            /* Retrieve the group IDs in this slice */

            rv = _field_scache_slice_group_recover(unit, fc,
                slice_index, &multigroup);

            if (BCM_FAILURE(rv))
            {
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "Failed to retrieve group IDs in slice %d."),
                           slice_index));

                goto cleanup;
            }
        }

        /* Now find the master slice for this virtual group */
        vslice_index = _field_physical_to_virtual(unit, _FP_DEF_INST,
                                                  slice_index, stage_fc);
        if (fc->l2warm
                && ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_8)) {
            master_slice = slice_master_idx[slice_index];
        } else {
        max = -1;
        for (i = 0; i < stage_fc->tcam_slices; i++) {
            if ((stage_fc->vmap[_FP_DEF_INST][0][vslice_index].virtual_group == 
                stage_fc->vmap[_FP_DEF_INST][0][i].virtual_group) && (i != vslice_index)) {
                if (i > max) {
                    max = i;
                }
            } 
        }

        /* max - Must be a valid hardware slice index value */
        if (-1 == max) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }

        master_slice = stage_fc->vmap[_FP_DEF_INST][0][max].vmap_key;
        }

        /* See which group is in this slice - can be only one */
        fg_p = fc->groups;
        while (fg_p != NULL) {
            /* Check if group is in this slice */
            fs_p = &fg_p->slices[0];
            if ((fg_p->stage_id == stage_fc->stage_id)
                    && (fs_p->slice_number == master_slice)) {
                break;
            }
            fg_p = fg_p->next;
        }

        if (fg_p == NULL)
        {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }

        /* Get number of entry parts. */
        _FP_RAVEN_IF_FAILURE_CLEANUP(
                _bcm_field_entry_tcam_parts_count (unit, fg_p->stage_id,
                    fg_p->flags, &part_count));

        old_physical_slice = fs_p->slice_number;

        /* Set up the new physical slice parameters in Software */
        for(part_index = part_count - 1; part_index >= 0; part_index--) {
            /* Get entry flags. */
            _FP_RAVEN_IF_FAILURE_CLEANUP(
                    _bcm_field_tcam_part_to_entry_flags(unit,
                        part_index, fg_p, &entry_flags));

            /* Get slice id for entry part */
            _FP_RAVEN_IF_FAILURE_CLEANUP(
                    _bcm_field_tcam_part_to_slice_number(unit, part_index,
                        fg_p, &slice_num));

            /* Get slice pointer. */
            fs_p = stage_fc->slices[_FP_DEF_INST] + slice_index + slice_num;

            if (0 == (entry_flags & _FP_ENTRY_SECOND_HALF)) {
                /* Set per slice configuration &  number of free entries in the slice.*/
                fs_p->free_count = fs_p->entry_count;
                if (fg_p->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
                    fs_p->free_count >>= 1;
                }
                /* Set group flags in in slice.*/
                fs_p->group_flags = fg_p->flags & _FP_GROUP_STATUS_MASK;

                /* Add slice to slices linked list . */
                fs_temp = stage_fc->slices[_FP_DEF_INST] + old_physical_slice + slice_num;
                /* To handle more than one auto expanded slice in a group */
                while (fs_temp->next != NULL) {
                    fs_temp = fs_temp->next;
                }
                fs_temp->next = fs_p;
                fs_p->prev = fs_temp;
            }
        }

        fs_p = stage_fc->slices[_FP_DEF_INST] + slice_index;
        /* Recover entries in this slice */

        prev_prio = -1;

        for (entry_index = 0; entry_index < fs_p->entry_count; ++entry_index)
        {
            fp_tcam_entry_p = soc_mem_table_idx_to_pointer(unit, FP_TCAMm,
                fp_tcam_entry_t *, fp_tcam_buffer, entry_index +
                    slice_index * fs_p->entry_count);

            if (soc_FP_TCAMm_field32_get(unit, fp_tcam_entry_p, VALIDf) == 0)
            {
                /* Skip empty entry */

                continue;
            }

            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "      Checking entry %d...\n"),
                       entry_index));

            /* Determine pbmp for this entry */

            BCM_PBMP_CLEAR(entry_pbmp);
            PBMP_ALL_ITER(unit, port)
            {
                port_sel_entry_index = _BCM_RP_FIELD_IPBM_INDEX(unit,
                    port, slice_index);

                _FP_RAVEN_IF_FAILURE_CLEANUP(
                    _field_raptor_ipbm_entry_get(unit, port_sel_entry_index,
                        entry_index, &port_enabled_for_entry));

                if (port_enabled_for_entry)
                {
                    BCM_PBMP_PORT_ADD(entry_pbmp, port);
                }
            }

            SOC_PBMP_FMT(entry_pbmp, pbmp_string);
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "Entry PBMP: %s\n"),
                       pbmp_string));

            /* Find a matching group */

            for (fg_p = fc->groups; fg_p != NULL; fg_p = fg_p->next)
            {
                LOG_DEBUG(BSL_LS_BCM_FP,
                        (BSL_META_U(unit,
                                    "  Checking group %d...\n"),
                         fg_p->gid));

                group_slice_p = fg_p->slices;

                if ((fg_p->stage_id != stage_fc->stage_id)
                        || (group_slice_p->slice_number != master_slice))
                {
                    /* Not this slice */

                    continue;
                }

                SOC_PBMP_FMT(fg_p->pbmp, pbmp_string);
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "Group PBMP: %s\n"),
                           pbmp_string));

                SOC_PBMP_CLEAR(temp_pbmp);
                SOC_PBMP_ASSIGN(temp_pbmp, fg_p->pbmp);
                SOC_PBMP_OR(temp_pbmp, PBMP_LB(unit));
                SOC_PBMP_AND(temp_pbmp, entry_pbmp);
                if (SOC_PBMP_EQ(temp_pbmp, entry_pbmp))
                {
                    /* Found a match */

                    break;
                }

                /* Check if entry pbmp is a subset of the group pbmp */

                SOC_PBMP_ITER(entry_pbmp, port)
                {
                    if (SOC_PBMP_MEMBER(fg_p->pbmp, port) == 0)
                    {
                        break;
                    }
                }

                if (port == SOC_PBMP_PORT_MAX)
                {
                    /* Got all the way through with no mismatches */

                    break;
                }
            }

            if (fg_p == NULL)
            {
                /* This should never happen */
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "Could not find a matching group for entry %d.\n"),
                           entry_index));
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }

            /* Allocate the entry */

            _FP_RAVEN_IF_FAILURE_CLEANUP(
                _bcm_field_entry_tcam_parts_count(unit, fg_p->stage_id,
                                                  fg_p->flags, &part_count));

            entries_p = NULL;

            _FP_XGS3_ALLOC(entries_p, part_count * sizeof (_field_entry_t),
                "field entry");

            for (idx1 = 0; idx1 < _FP_POLICER_LEVEL_COUNT; idx1++) {
                 entries_p->policer[idx1].pid = _FP_INVALID_INDEX;
            }

            if (entries_p == NULL)
            {
                rv = BCM_E_MEMORY;

                goto cleanup;
            }

            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "        Entry matches group %d.\n"),
                       fg_p->gid));

            sal_memset(&f_ent_wb_info, 0, sizeof(_field_entry_wb_info_t));
            f_ent_wb_info.sid = -1;
            f_ent_wb_info.pid = -1;

            if (fc->l2warm)
            {
                if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
                   action_bmp.w = NULL;
                   _FP_XGS3_ALLOC(action_bmp.w,
                             SHR_BITALLOCSIZE(_bcmFieldActionNoParamCount),
                             "Action No Param Bitmap");
                   if (action_bmp.w == NULL) {
                      rv = BCM_E_MEMORY;
                      sal_free(entries_p);
                      goto cleanup;
                   }
                }
                f_ent_wb_info.action_bmp = &action_bmp;

                rv = _field_entry_info_retrieve(unit, fc, stage_fc,
                    multigroup, &prev_prio, &f_ent_wb_info);

                if (BCM_FAILURE(rv))
                {
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "Failed to retrieve entry info.")));
                    sal_free(entries_p);
                    goto cleanup;
                }
            }
            else
            {
                _bcm_field_last_alloc_eid_incr(unit);

                f_ent_wb_info.eid = _bcm_field_last_alloc_eid_get(unit);
            }

            primary_tcam_index =
                slice_index * fs_p->entry_count + entry_index;

            for (part_index = 0; part_index < part_count; ++part_index)
            {
                entry_p = entries_p + part_index;

                entry_p->eid = f_ent_wb_info.eid;
                entry_p->group = fg_p;
                /* Set retrieved dvp_type and svp_type */
                entry_p->dvp_type = f_ent_wb_info.dvp_type[part_index];
                entry_p->svp_type = f_ent_wb_info.svp_type[part_index];

                if (part_index == 0) {
                    BCM_PBMP_ASSIGN(entry_p->pbmp.data, entry_pbmp);
                    BCM_PBMP_ASSIGN(entry_p->pbmp.mask, PBMP_ALL(unit));
                }

                /* Set entry flags */

                rv = _bcm_field_tcam_part_to_entry_flags(unit, part_index,
                        fg_p, &entry_p->flags);

                if (BCM_FAILURE(rv)) {
                    sal_free(entries_p);
                    goto cleanup;
                }

                if (f_ent_wb_info.color_independent)
                {
                    entry_p->flags |= _FP_ENTRY_COLOR_INDEPENDENT;
                }

                /* Get index of entry in tcam */

                rv = _bcm_field_entry_part_tcam_idx_get(unit, entries_p,
                        primary_tcam_index, part_index, &part_tcam_index);

                if (BCM_FAILURE(rv)) {
                    sal_free(entries_p);
                    goto cleanup;
                }

                /* Get slice and index within the slice */

                rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc,
                        _FP_DEF_INST, part_tcam_index, &part_slice_index,
                        (int *) (&entry_p->slice_idx));

                if (BCM_FAILURE(rv)) {
                    sal_free(entries_p);
                    goto cleanup;
                }

                /* Point the entry to its slice */

                entry_p->fs = stage_fc->slices[_FP_DEF_INST]
                                + part_slice_index;
                --entry_p->fs->free_count;

                /* Set the slice pbmp */

                BCM_PBMP_OR(entry_p->fs->pbmp, fg_p->pbmp);

                /* Assign entry to slice */

                entry_p->fs->entries[entry_p->slice_idx] = entry_p;
                entry_p->flags |= _FP_ENTRY_INSTALLED;

                fp_tcam_entry_p =
                    (fp_tcam_entry_t *) soc_mem_table_idx_to_pointer(unit,
                    FP_TCAMm, fp_tcam_entry_t *, fp_tcam_buffer,
                    part_tcam_index);

                part_qualifier_p = &(fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part_index]);

                for (group_qualifier_index = 0;
                    group_qualifier_index < part_qualifier_p->size;
                    ++group_qualifier_index)
                {
                    qualifier_id =
                        part_qualifier_p->qid_arr[group_qualifier_index];

                    stage_qualifier_p = stage_fc->f_qual_arr[qualifier_id];

                    for (config_index = 0;
                        config_index < stage_qualifier_p->conf_sz;
                        ++config_index)
                    {
                        config_offset_p = &((stage_qualifier_p->conf_arr +
                            config_index)->offset);

                        group_qualifier_offset_p =
                            part_qualifier_p->offset_arr +
                                group_qualifier_index;

                        if (_mask_is_set(unit, config_offset_p,
                            (uint32 *) fp_tcam_entry_p,
                                stage_fc->stage_id) != 0)
                        {
                            config_selector = stage_qualifier_p->
                                conf_arr[config_index].selector;

                            selector = config_selector.pri_sel;
                            selector_value = config_selector.pri_sel_val;

                            if ((selector == _bcmFieldSliceSelFpf1 &&
                                selector_value == sel_codes[part_index].fpf1) ||
                                (selector == _bcmFieldSliceSelFpf2 &&
                                selector_value == sel_codes[part_index].fpf2) ||
                                (selector == _bcmFieldSliceSelFpf3 &&
                                selector_value == sel_codes[part_index].fpf3) ||
                                (selector == _bcmFieldSliceSelFpf4 &&
                                selector_value == sel_codes[part_index].fpf4))
                            {
                                /* This qualifier exists in this entry */

                                *group_qualifier_offset_p = *config_offset_p;

                                /* No need to check more configs */

                                break;
                            }
                        }
                    }
                }

                /* Get IP type and mask values. */
                hw_data = soc_FP_TCAMm_field32_get(unit, 
                                                   fp_tcam_entry_p, IP_TYPEf); 
                hw_mask = soc_FP_TCAMm_field32_get(unit,
                                                   fp_tcam_entry_p,
                                                   IP_TYPE_MASKf); 

                if ((hw_data == 0) && (hw_mask == 0)) {
                    entry_p->tcam.ip_type = bcmFieldIpTypeAny;
                } else if ((hw_data == 0) && (hw_mask == 3)) {
                    entry_p->tcam.ip_type = bcmFieldIpTypeNonIp;
                } else if ((hw_data == 0) && (hw_mask == 1)) {
                    entry_p->tcam.ip_type = bcmFieldIpTypeIpv4Not;
                } else if ((hw_data == 1) && (hw_mask == 3)) {
                    entry_p->tcam.ip_type = bcmFieldIpTypeIpv4NoOpts;
                } else if ((hw_data == 3) && (hw_mask == 3)) {
                    entry_p->tcam.ip_type = bcmFieldIpTypeIpv4WithOpts;
                } else if ((hw_data == 1) && (hw_mask == 1)) {
                    entry_p->tcam.ip_type  = bcmFieldIpTypeIpv4Any;
                } else if ((hw_data == 2) && (hw_mask == 3)) {
                    entry_p->tcam.ip_type = bcmFieldIpTypeIpv6;
                }

                /* Get the actions for this part of the entry */

                policy_entry_p = soc_mem_table_idx_to_pointer(unit,
                    FP_POLICY_TABLEm, fp_policy_table_entry_t *,
                    fp_policy_table_buffer, part_tcam_index);

                rv = _field_raven_actions_recover(unit,
                        policy_entry_p, entries_p, part_index,
                        &f_ent_wb_info);

                if (BCM_FAILURE(rv)) {
                    sal_free(entries_p);
                    goto cleanup;
                }

                if (action_bmp.w != NULL) {
                   _FP_ACTION_BMP_FREE(action_bmp);
                   action_bmp.w = NULL;
                }
                entry_p->prio = fc->l2warm ? f_ent_wb_info.prio :
                    (vslice_index << 10) | (fs_p->entry_count - entry_index);
            }

            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "        Entry id = %d, priority = %d.\n"),
                       entries_p->eid,
                       entries_p->prio));

            /* Add the entry to the group */

            rv = _field_group_entry_add(unit, fg_p, entries_p);
            if (BCM_FAILURE(rv)) {
                sal_free(entries_p);
                goto cleanup;
            }
        }

        if (fc->l2warm)
        {
            _field_scache_slice_group_free(unit, fc, slice_index);
        }

    }

    if (fc->l2warm) {

        /* Mark end of Slice Info */
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
            fc->scache_pos++;
        }

        /* Group Slice Selectors */
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
            rv = _field_group_slice_selectors_recover(unit,
                        &buf[fc->scache_pos], stage_fc->stage_id);
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

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "Ingress stage recovery complete.\n")));

cleanup:
    if (slice_is_expanded_flags != NULL)
    {
        sal_free(slice_is_expanded_flags);
    }
    if (slice_master_idx != NULL)
    {
        sal_free(slice_master_idx);
    }
    if (fp_tcam_buffer != NULL)
    {
        soc_cm_sfree(unit, fp_tcam_buffer);
    }
    if (fp_port_field_sel_buffer != NULL) 
    {
        soc_cm_sfree(unit, fp_port_field_sel_buffer);
    }
    if (fp_policy_table_buffer != NULL) 
    {
        soc_cm_sfree(unit, fp_policy_table_buffer);
    }
    if (action_bmp.w != NULL) {
        _FP_ACTION_BMP_FREE(action_bmp);
        action_bmp.w = NULL;
    }
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "Leaving ingress stage recovery.\n")));

    return rv;
}
#else
#define _field_raven_stage_reinit (NULL)
#endif /* BCM_WARM_BOOT_SUPPORT && BCM_RAVEN_SUPPORT */

/*
 * Function:
 *     _field_raptor_qualify_ip_type
 * Purpose:
 *     Install ip type qualifier into TCAM
 * Parameters:
 *     unit  - (IN) BCM device number
 *     entry - (IN) Field entry index
 *     type  - (IN) Ip Type. 
 *     qual  - (IN) Qualifier(IpType)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
#define BCM_FIELD_IPTYPE_BAD 0xff
STATIC int
_field_raptor_qualify_ip_type(int unit, bcm_field_entry_t entry,
                              bcm_field_IpType_t type,
                              bcm_field_qualify_t qual) 
{
    _field_group_t   *fg;
    _field_entry_t   *f_ent;
    uint32  data = BCM_FIELD_IPTYPE_BAD,
            mask = BCM_FIELD_IPTYPE_BAD;

    BCM_IF_ERROR_RETURN
         (_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent));

    /* Confirm that IpType is in group's Qset. */
    fg = f_ent->group;
    if (!BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIpType)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "FP(unit %d) Error: IpType not in entry=%d Qset.\n"),
                   unit, f_ent->eid));
        return (BCM_E_PARAM);
    }

    switch (type) {
      case bcmFieldIpTypeAny:
          data = 0x0;
          mask = 0x0;
          break;
      case bcmFieldIpTypeNonIp:
          data = 0x0;
          mask = 0x3;
          break;
      case bcmFieldIpTypeIpv4Not:
          data = 0x0;
          mask = 0x1;
          break;
      case bcmFieldIpTypeIpv4NoOpts:
          data = 0x1;
          mask = 0x3;
          break;
      case bcmFieldIpTypeIpv4WithOpts:
          data = 0x3;
          mask = 0x3;
          break;
      case bcmFieldIpTypeIpv4Any:
          data = 0x1;
          mask = 0x1;
          break;
      case bcmFieldIpTypeIpv6:
          data = 0x2;
          mask = 0x3;
          break;
      default:
          break;
    }

    if ((data == BCM_FIELD_IPTYPE_BAD) ||
        (mask == BCM_FIELD_IPTYPE_BAD)) {
        return (BCM_E_UNAVAIL);
    }

    BCM_IF_ERROR_RETURN
        (_bcm_field_entry_qual_get(unit, f_ent->eid, 
                                   bcmFieldQualifyIpType, &f_ent));

    /* Add data & mask to entry. */
    f_ent->tcam.ip_type      = type;
    f_ent->flags |= _FP_ENTRY_DIRTY;

    return (BCM_E_NONE);
}
#undef BCM_FIELD_IPTYPE_BAD 
/*
 * Function:
 *     _bcm_field_raptor_qualify_ip_type_get
 * Purpose:
 *     Read ip type qualifier match criteria from the HW.
 * Parameters:
 *     unit  - (IN) BCM device number
 *     entry - (IN) Field entry index
 *     type  - (OUT) Ip Type. 
 *     qual  - (IN) Qualifier(IpType)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
_bcm_field_raptor_qualify_ip_type_get(int unit, bcm_field_entry_t entry, 
                                  bcm_field_IpType_t *type,
                                  bcm_field_qualify_t qual) 
{
    _field_entry_t  *f_ent;

    /* Input parameters checks. */
    if ((NULL == type)) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN
         (_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent));

    *type = f_ent->tcam.ip_type;
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_raptor_action_support_check
 *
 * Purpose:
 *     Check if action is supported by device.
 *
 * Parameters:
 *     unit   -(IN)BCM device number
 *     f_ent  -(IN)Field entry structure.  
 *     action -(IN) Action to check(bcmFieldActionXXX)
 *     result -(OUT)
 *               TRUE   - action is supported by device
 *               FALSE  - action is NOT supported by device
 *
 * Returns:
 *     BCM_E_XXX   
 */
STATIC int
_field_raptor_action_support_check(int unit, _field_entry_t *f_ent,
                               bcm_field_action_t action, int *result)
{
    /* Input parameters check */
    if ((NULL == f_ent) || (NULL == result)) {
        return (BCM_E_PARAM);
    }

    switch (action) {
          case bcmFieldActionCosQNew:
          case bcmFieldActionCosQCpuNew:
          case bcmFieldActionPrioPktAndIntCopy:
          case bcmFieldActionPrioPktAndIntNew:
          case bcmFieldActionPrioPktAndIntTos:
          case bcmFieldActionPrioPktAndIntCancel:
          case bcmFieldActionPrioPktCopy:
          case bcmFieldActionPrioPktNew:
          case bcmFieldActionPrioPktTos:
          case bcmFieldActionPrioPktCancel:
          case bcmFieldActionPrioIntCopy:
          case bcmFieldActionPrioIntNew:
          case bcmFieldActionPrioIntTos:
          case bcmFieldActionPrioIntCancel:
          case bcmFieldActionTosNew:
          case bcmFieldActionTosCopy:
          case bcmFieldActionTosCancel:
          case bcmFieldActionDscpNew:
          case bcmFieldActionDscpCancel:
          case bcmFieldActionCopyToCpu:
          case bcmFieldActionCopyToCpuCancel:
          case bcmFieldActionSwitchToCpuCancel:
          case bcmFieldActionRedirect:
          case bcmFieldActionRedirectTrunk:
          case bcmFieldActionRedirectCancel:
          case bcmFieldActionRedirectPbmp:
          case bcmFieldActionEgressMask:
          case bcmFieldActionDrop:
          case bcmFieldActionDropCancel:
          case bcmFieldActionMirrorOverride:
          case bcmFieldActionMirrorIngress:
          case bcmFieldActionMirrorEgress:
          case bcmFieldActionL3ChangeVlan:
          case bcmFieldActionL3ChangeVlanCancel:
          case bcmFieldActionL3ChangeMacDa:
          case bcmFieldActionL3ChangeMacDaCancel:
          case bcmFieldActionL3Switch:
          case bcmFieldActionL3SwitchCancel:
          case bcmFieldActionAddClassTag:
          case bcmFieldActionEcnNew:
          case bcmFieldActionDropPrecedence:
          case bcmFieldActionRpDrop:
          case bcmFieldActionRpDropCancel:
          case bcmFieldActionRpDropPrecedence:
          case bcmFieldActionRpCopyToCpu:
          case bcmFieldActionRpCopyToCpuCancel:
          case bcmFieldActionRpDscpNew:
          case bcmFieldActionYpDrop:
          case bcmFieldActionYpDropCancel:
          case bcmFieldActionYpDropPrecedence:
          case bcmFieldActionYpCopyToCpu:
          case bcmFieldActionYpCopyToCpuCancel:
          case bcmFieldActionYpDscpNew:
          case bcmFieldActionGpDrop:
          case bcmFieldActionGpDropCancel:
          case bcmFieldActionGpDropPrecedence:
          case bcmFieldActionGpCopyToCpu:
          case bcmFieldActionGpCopyToCpuCancel:
          case bcmFieldActionGpDscpNew:
          case bcmFieldActionColorIndependent:
              *result = TRUE;
              break;
          case bcmFieldActionGpCosQNew:
          case bcmFieldActionGpPrioPktAndIntCopy:
          case bcmFieldActionGpPrioPktAndIntNew:
          case bcmFieldActionGpPrioPktAndIntTos:
          case bcmFieldActionGpPrioPktAndIntCancel:
          case bcmFieldActionGpPrioPktCopy:
          case bcmFieldActionGpPrioPktNew:
          case bcmFieldActionGpPrioPktTos:
          case bcmFieldActionGpPrioPktCancel:
          case bcmFieldActionGpPrioIntCopy:
          case bcmFieldActionGpPrioIntNew:
          case bcmFieldActionGpPrioIntTos:
          case bcmFieldActionGpPrioIntCancel:
          case bcmFieldActionRpCosQNew:
          case bcmFieldActionRpPrioPktAndIntCopy:
          case bcmFieldActionRpPrioPktAndIntNew:
          case bcmFieldActionRpPrioPktAndIntTos:
          case bcmFieldActionRpPrioPktAndIntCancel:
          case bcmFieldActionRpPrioPktCopy:
          case bcmFieldActionRpPrioPktNew:
          case bcmFieldActionRpPrioPktTos:
          case bcmFieldActionRpPrioPktCancel:
          case bcmFieldActionRpPrioIntCopy:
          case bcmFieldActionRpPrioIntNew:
          case bcmFieldActionRpPrioIntTos:
          case bcmFieldActionRpPrioIntCancel:
          case bcmFieldActionYpCosQNew:
          case bcmFieldActionYpPrioPktAndIntCopy:
          case bcmFieldActionYpPrioPktAndIntNew:
          case bcmFieldActionYpPrioPktAndIntTos:
          case bcmFieldActionYpPrioPktAndIntCancel:
          case bcmFieldActionYpPrioPktCopy:
          case bcmFieldActionYpPrioPktNew:
          case bcmFieldActionYpPrioPktTos:
          case bcmFieldActionYpPrioPktCancel:
          case bcmFieldActionYpPrioIntCopy:
          case bcmFieldActionYpPrioIntNew:
          case bcmFieldActionYpPrioIntTos:
          case bcmFieldActionYpPrioIntCancel:
              *result = (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) ? TRUE : FALSE;
              break;
          default:
              *result = FALSE;
        }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_raptor_action_conflict_check
 *
 * Purpose:
 *     Check if two action conflict (occupy the same field in FP policy table)
 *
 * Parameters:
 *     unit    -(IN)BCM device number
 *     f_ent   -(IN)Field entry structure.  
 *     action -(IN) Action to check(bcmFieldActionXXX)
 *     action1 -(IN) Action to check(bcmFieldActionXXX)
 *
 * Returns:
 *     BCM_E_CONFIG - if actions do conflict
 *     BCM_E_NONE   - if there is no conflict
 */
STATIC int
_field_raptor_action_conflict_check(int unit, _field_entry_t *f_ent,
                                 bcm_field_action_t action1, 
                                 bcm_field_action_t action)
{
    /* Input parameters check */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    /* Two identical actions are forbidden. */
    _FP_ACTIONS_CONFLICT(action1);

    switch (action1) {
      case bcmFieldActionCosQNew:
      case bcmFieldActionCosQCpuNew:
      case bcmFieldActionPrioPktAndIntCopy:
      case bcmFieldActionPrioPktAndIntNew:
      case bcmFieldActionPrioPktAndIntTos:
      case bcmFieldActionPrioPktAndIntCancel:
      case bcmFieldActionPrioPktCopy:
      case bcmFieldActionPrioPktNew:
      case bcmFieldActionPrioPktTos:
      case bcmFieldActionPrioPktCancel:
      case bcmFieldActionPrioIntCopy:
      case bcmFieldActionPrioIntNew:
      case bcmFieldActionPrioIntTos:
      case bcmFieldActionPrioIntCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCancel);
          break;
      case bcmFieldActionRpCosQNew:
      case bcmFieldActionRpPrioPktAndIntCopy:
      case bcmFieldActionRpPrioPktAndIntNew:
      case bcmFieldActionRpPrioPktAndIntTos:
      case bcmFieldActionRpPrioPktAndIntCancel:
      case bcmFieldActionRpPrioPktCopy:
      case bcmFieldActionRpPrioPktNew:
      case bcmFieldActionRpPrioPktTos:
      case bcmFieldActionRpPrioPktCancel:
      case bcmFieldActionRpPrioIntCopy:
      case bcmFieldActionRpPrioIntNew:
      case bcmFieldActionRpPrioIntTos:
      case bcmFieldActionRpPrioIntCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCancel);
          break;
      case bcmFieldActionYpCosQNew:
      case bcmFieldActionYpPrioPktAndIntCopy:
      case bcmFieldActionYpPrioPktAndIntNew:
      case bcmFieldActionYpPrioPktAndIntTos:
      case bcmFieldActionYpPrioPktAndIntCancel:
      case bcmFieldActionYpPrioPktCopy:
      case bcmFieldActionYpPrioPktNew:
      case bcmFieldActionYpPrioPktTos:
      case bcmFieldActionYpPrioPktCancel:
      case bcmFieldActionYpPrioIntCopy:
      case bcmFieldActionYpPrioIntNew:
      case bcmFieldActionYpPrioIntTos:
      case bcmFieldActionYpPrioIntCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCancel);
          break;
      case bcmFieldActionGpCosQNew:
      case bcmFieldActionGpPrioPktAndIntCopy:
      case bcmFieldActionGpPrioPktAndIntNew:
      case bcmFieldActionGpPrioPktAndIntTos:
      case bcmFieldActionGpPrioPktAndIntCancel:
      case bcmFieldActionGpPrioPktCopy:
      case bcmFieldActionGpPrioPktNew:
      case bcmFieldActionGpPrioPktTos:
      case bcmFieldActionGpPrioPktCancel:
      case bcmFieldActionGpPrioIntCopy:
      case bcmFieldActionGpPrioIntNew:
      case bcmFieldActionGpPrioIntTos:
      case bcmFieldActionGpPrioIntCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
          break;
      case bcmFieldActionTosNew:
      case bcmFieldActionTosCopy:
      case bcmFieldActionTosCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionEcnNew);
          break;
      case bcmFieldActionDscpNew:
      case bcmFieldActionDscpCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
          break;
      case bcmFieldActionRpDscpNew:
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
          break;
      case bcmFieldActionYpDscpNew:
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
          break;
      case bcmFieldActionGpDscpNew:
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
          break;
      case bcmFieldActionEcnNew:
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          break;

      case bcmFieldActionCopyToCpu:
      case bcmFieldActionCopyToCpuCancel:
      case bcmFieldActionSwitchToCpuCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpuCancel);
          break;
      case bcmFieldActionRpCopyToCpu:
      case bcmFieldActionRpCopyToCpuCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpuCancel);
          break;
      case bcmFieldActionYpCopyToCpu:
      case bcmFieldActionYpCopyToCpuCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpuCancel);
          break;
      case bcmFieldActionGpCopyToCpu:
      case bcmFieldActionGpCopyToCpuCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpuCancel);
          break;
      case bcmFieldActionDropPrecedence:
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropPrecedence);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropPrecedence);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropPrecedence);
          break;
      case bcmFieldActionRpDropPrecedence:
          _FP_ACTIONS_CONFLICT(bcmFieldActionDropPrecedence);
          break;
      case bcmFieldActionYpDropPrecedence:
          _FP_ACTIONS_CONFLICT(bcmFieldActionDropPrecedence);
          break;
      case bcmFieldActionGpDropPrecedence:
          _FP_ACTIONS_CONFLICT(bcmFieldActionDropPrecedence);
          break;
      case bcmFieldActionRedirect:
      case bcmFieldActionRedirectTrunk:
      case bcmFieldActionRedirectCancel:
      case bcmFieldActionRedirectPbmp:
      case bcmFieldActionEgressMask:
          _FP_ACTIONS_CONFLICT(bcmFieldActionRedirect);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRedirectTrunk);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRedirectCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRedirectPbmp);
          _FP_ACTIONS_CONFLICT(bcmFieldActionEgressMask);
          break;
      case bcmFieldActionDrop:
      case bcmFieldActionDropCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropCancel);
          break;
      case bcmFieldActionRpDrop:
      case bcmFieldActionRpDropCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropCancel);
          break;
      case bcmFieldActionYpDrop:
      case bcmFieldActionYpDropCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropCancel);
          break;
      case bcmFieldActionGpDrop:
      case bcmFieldActionGpDropCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropCancel);
          break;
      case bcmFieldActionMirrorIngress:
          _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorOverride);
          break;
      case bcmFieldActionMirrorEgress:
          _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorOverride);
          break;
      case bcmFieldActionMirrorOverride:
          _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorIngress);
          _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorEgress);
          break;
      case bcmFieldActionL3ChangeVlan:
      case bcmFieldActionL3ChangeVlanCancel:
      case bcmFieldActionL3ChangeMacDa:
      case bcmFieldActionL3ChangeMacDaCancel:
      case bcmFieldActionL3Switch:
      case bcmFieldActionL3SwitchCancel:
      case bcmFieldActionAddClassTag:
          _FP_ACTIONS_CONFLICT(bcmFieldActionAddClassTag);
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlan);
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlanCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDa);
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDaCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3Switch);
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3SwitchCancel);
          break;
      case bcmFieldActionColorIndependent:
          break;
      default:
          break;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_raptor_functions_init
 * Purpose:
 *     Set up functions pointers 
 * Parameters:
 *     functions - pointers to device specific functions
 * Returns:
 *     nothing
 * Notes:
 *     Some function pointers point to functions in ../firebolt/field.c
 */
STATIC void
_field_raptor_functions_init(_field_funct_t *functions) 
{
    functions->fp_detach               = _field_raptor_detach;
    functions->fp_group_install        = _field_raptor_group_install;
    functions->fp_selcodes_install     = _field_raptor_selcodes_install;
    functions->fp_slice_clear          = _bcm_field_fb_slice_clear;
    functions->fp_entry_remove         = _field_raptor_entry_remove;
    functions->fp_entry_move           = _field_raptor_entry_move;
    functions->fp_selcode_get          = _bcm_field_selcode_get;
    functions->fp_selcode_to_qset      = _bcm_field_selcode_to_qset;
    functions->fp_qual_list_get        = _bcm_field_qual_lists_get;
    functions->fp_tcam_policy_clear    = _field_raptor_tables_entry_clear;
    functions->fp_tcam_policy_install  = _field_raptor_entry_install;
    functions->fp_tcam_policy_reinstall  = _field_raptor_entry_reinstall;
    functions->fp_action_params_check  = _field_raptor_action_params_check;
    functions->fp_policer_install      = _bcm_field_fb_policer_install;
    functions->fp_write_slice_map      = _bcm_field_fb_write_slice_map;
    functions->fp_qualify_ip_type      = _field_raptor_qualify_ip_type;
    functions->fp_qualify_ip_type_get  = _bcm_field_raptor_qualify_ip_type_get;
    functions->fp_action_support_check = _field_raptor_action_support_check;
    functions->fp_action_conflict_check = _field_raptor_action_conflict_check;
    functions->fp_counter_get          = _bcm_field_fb_counter_get;
    functions->fp_counter_set          = _bcm_field_fb_counter_set;
    functions->fp_stat_index_get       = _bcm_field_fb_stat_index_get;
    functions->fp_egress_key_match_type_set = NULL;
    functions->fp_external_entry_install = NULL;
    functions->fp_external_entry_reinstall = NULL;
    functions->fp_external_entry_remove = NULL;
    functions->fp_external_entry_prio_set = NULL;
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

/*
 * Function:
 *     _field_raptor_detach
 * Purpose:
 *     Deallocates field tables.
 * Parameters:
 *     unit - (IN) BCM device number
 *     fc   - (IN) Control Structure
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */
STATIC int
_field_raptor_detach(int unit, _field_control_t *fc)
{
    _field_stage_t   *stage_p;  /* Stage iteration pointer. */

    if (NULL == fc) {
        return BCM_E_PARAM;
    }
    stage_p = fc->stages;

    if (NULL == stage_p ||
        _BCM_FIELD_STAGE_INGRESS != stage_p->stage_id) {
        return BCM_E_PARAM;
    } 

    /* Clear the hardware tables */
    BCM_IF_ERROR_RETURN(_field_raptor_hw_clear(unit, stage_p));

    /* Don't disable port controls during Warmboot or Reloading state */
    if (0 == SOC_HW_ACCESS_DISABLE(unit)) {
        /* Clear the Filter Enable flags in the port table */
        BCM_IF_ERROR_RETURN
            (_field_port_filter_enable_set(unit, fc, FALSE));
    }

    /* Disable field meter refreshing */
    return (_field_meter_refresh_enable_set(unit, fc, FALSE));
}

/*
 * Function:
 *     _field_raptor_group_install
 * Purpose:
 *     Writes the Group's mode and field select codes into hardware. This
 *     should be called both at group creation time and any time the select
 *     codes change (i.e. bcm_field_group_set calls).
 * Parameters:
 *     unit  - BCM device number
 *     fg    - group to install
 * Returns:
 *     BCM_E_INTERNAL - On read/write errors
 *     BCM_E_NONE     - Success
 * Note:
 *     Unit lock should be held by calling function.
 *     Uses function common to firebolt: _bcm_field_fb_mode_install
 */
STATIC int
_field_raptor_group_install(int unit, _field_group_t *fg) 
{
    assert(fg != NULL);

    if (fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
        return (BCM_E_INTERNAL);
    } 

    /* Write the field select codes*/
    if (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
        BCM_IF_ERROR_RETURN
            (_field_raptor_selcodes_install(unit, fg, 
                                            fg->slices[2].slice_number,
                                            fg->pbmp, 2));
    }

    if ((fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) || 
        (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE)) {
        BCM_IF_ERROR_RETURN
            (_field_raptor_selcodes_install(unit, fg, 
                                            fg->slices[1].slice_number,
                                            fg->pbmp, 1));
    }
    BCM_IF_ERROR_RETURN 
        (_field_raptor_selcodes_install(unit, fg, 
                                        fg->slices[0].slice_number,
                                        fg->pbmp, 0));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_selcodes_install
 * Purpose:
 *     Writes the field select codes (ie. FPFx).
 * Parameters:
 *     unit  - BCM device number
 *     fs    - slice that needs its select codes written
 * Returns:
 *     BCM_E_INTERNAL - On read/write errors
 *     BCM_E_NONE     - Success
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_raptor_selcodes_install(int unit, _field_group_t *fg,
                               uint8 slice_numb, bcm_pbmp_t pbmp,
                               int selcode_index)
{
    bcm_port_t                  port;
    fp_port_field_sel_entry_t   pfs_entry;
    bcm_pbmp_t                  all_pbmp;
#if defined(BCM_RAVEN_SUPPORT)     
    static int                  field_tbl[16][4] = {
        {SLICE0_F1f, SLICE0_F2f, SLICE0_F3f, SLICE0_F4f},
        {SLICE1_F1f, SLICE1_F2f, SLICE1_F3f, SLICE1_F4f},
        {SLICE2_F1f, SLICE2_F2f, SLICE2_F3f, SLICE2_F4f},
        {SLICE3_F1f, SLICE3_F2f, SLICE3_F3f, SLICE3_F4f},
        {SLICE4_F1f, SLICE4_F2f, SLICE4_F3f, SLICE4_F4f},
        {SLICE5_F1f, SLICE5_F2f, SLICE5_F3f, SLICE5_F4f},
        {SLICE6_F1f, SLICE6_F2f, SLICE6_F3f, SLICE6_F4f},
        {SLICE7_F1f, SLICE7_F2f, SLICE7_F3f, SLICE7_F4f},
        {SLICE8_F1f, SLICE8_F2f, SLICE8_F3f, SLICE8_F4f},
        {SLICE9_F1f, SLICE9_F2f, SLICE9_F3f, SLICE9_F4f},
        {SLICE10_F1f, SLICE10_F2f, SLICE10_F3f, SLICE10_F4f},
        {SLICE11_F1f, SLICE11_F2f, SLICE11_F3f, SLICE11_F4f},
        {SLICE12_F1f, SLICE12_F2f, SLICE12_F3f, SLICE12_F4f},
        {SLICE13_F1f, SLICE13_F2f, SLICE13_F3f, SLICE13_F4f},
        {SLICE14_F1f, SLICE14_F2f, SLICE14_F3f, SLICE14_F4f},
        {SLICE15_F1f, SLICE15_F2f, SLICE15_F3f, SLICE15_F4f}};
#else  /* BCM_RAVEN_SUPPORT */
    static int                  field_tbl[8][4] = {
        {SLICE0_F1f, SLICE0_F2f, SLICE0_F3f, SLICE0_F4f},
        {SLICE1_F1f, SLICE1_F2f, SLICE1_F3f, SLICE1_F4f},
        {SLICE2_F1f, SLICE2_F2f, SLICE2_F3f, SLICE2_F4f},
        {SLICE3_F1f, SLICE3_F2f, SLICE3_F3f, SLICE3_F4f},
        {SLICE4_F1f, SLICE4_F2f, SLICE4_F3f, SLICE4_F4f},
        {SLICE5_F1f, SLICE5_F2f, SLICE5_F3f, SLICE5_F4f},
        {SLICE6_F1f, SLICE6_F2f, SLICE6_F3f, SLICE6_F4f},
        {SLICE7_F1f, SLICE7_F2f, SLICE7_F3f, SLICE7_F4f}};
#endif /* BCM_RAVEN_SUPPORT */
    soc_field_t                 f1_field, f2_field, f3_field, f4_field;
    _field_sel_t               *sel = NULL;

    if (_BCM_FIELD_STAGE_INGRESS == fg->stage_id) {
        /* Write the group mode into appropriate slices for 
         * wide-mode devices. 
         */
        if (soc_feature(unit, soc_feature_field_wide)) {
            BCM_IF_ERROR_RETURN(_bcm_field_fb_mode_set(unit, slice_numb,
                                                       fg, fg->flags));
        }  

        sel = &fg->sel_codes[selcode_index];
 
        sal_memcpy(&pfs_entry, soc_mem_entry_null(unit, FP_PORT_FIELD_SELm), 
                   sizeof(pfs_entry));

        /* Determine which 4 fields will be modified */
        f1_field = field_tbl[slice_numb][0];
        f2_field = field_tbl[slice_numb][1];
        f3_field = field_tbl[slice_numb][2];
        f4_field = field_tbl[slice_numb][3];

        /* Iterate over all ports */
        PBMP_ITER(pbmp, port) {
            /* Read Port's current entry in FP_PORT_FIELD_SEL table */
            SOC_IF_ERROR_RETURN(READ_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ANY,
                                                        port, &pfs_entry));

            /* modify 0-4 fields depending on state of SELCODE_DONT_CARE */
            if (sel->fpf1 != _FP_SELCODE_DONT_CARE) {
                soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, f1_field,
                                                   sel->fpf1);
            }
            if (sel->fpf2 != _FP_SELCODE_DONT_CARE) {
                soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, f2_field,
                                                   sel->fpf2);
            }
            if (sel->fpf3 != _FP_SELCODE_DONT_CARE) {
                soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, f3_field,
                                                   sel->fpf3);
            }
            if (sel->fpf4 != _FP_SELCODE_DONT_CARE) {
                soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, f4_field,
                                                   sel->fpf4);
            }

            /* Write each port's new entry */
            SOC_IF_ERROR_RETURN(WRITE_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL,
                                                         port, &pfs_entry));
#if defined(BCM_RAVEN_SUPPORT)
            /*  IFP_PORT_FIELD_SEL table will be used to generate IFP_TCAM key
                when a port is configured in HIGIG mode (PORT_TAB.HIGIG_PACKET).
                This table will be automatically updated for other HG ports by
                the hardware when a value is configured in the FP_PORT_FIELD_SEL
                table except for cpu port */
            if (SOC_IS_RAVEN(unit) && IS_CPU_PORT(unit, port)) {
                SOC_IF_ERROR_RETURN(WRITE_IFP_PORT_FIELD_SELm(unit,
                                             MEM_BLOCK_ALL, port, &pfs_entry));
            }
#endif
        }
#if defined(BCM_RAVEN_SUPPORT)     
        /* Set inner vlan overlay config. */
        if (_FP_SELCODE_DONT_CARE != fg->sel_codes[0].inner_vlan_overlay) {
            if (SOC_REG_FIELD_VALID(unit, ING_MISC_CONFIG2r,
                    FP_INNER_VLAN_OVERLAY_ENABLEf)) {
                BCM_IF_ERROR_RETURN(
                    soc_reg_field32_modify(unit,
                        ING_MISC_CONFIG2r,
                        REG_PORT_ANY, FP_INNER_VLAN_OVERLAY_ENABLEf,
                        fg->sel_codes[0].inner_vlan_overlay));
            } 
        }
#endif
        BCM_IF_ERROR_RETURN(_bcm_field_valid_pbmp_get(unit, &all_pbmp));
        if (BCM_PBMP_NEQ(all_pbmp, pbmp)) {
            /* Update Group Flags to denote Port based Group */
            fg->flags |= _FP_GROUP_PER_PORT_OR_PBMP;
        }
    } else {
        return (BCM_E_UNAVAIL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_raptor_entry_reinstall
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
_field_raptor_entry_reinstall(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    assert(f_ent != NULL);
    assert(tcam_idx >= soc_mem_index_min(unit, FP_TCAMm));
    assert(tcam_idx <= soc_mem_index_max(unit, FP_TCAMm));
    
    /* Next Write the Actions */
    BCM_IF_ERROR_RETURN(
        _field_raptor_policy_install(unit, f_ent, tcam_idx));
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_entry_install
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
_field_raptor_entry_install(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    assert(f_ent != NULL);
    assert(tcam_idx >= soc_mem_index_min(unit, FP_TCAMm));
    assert(tcam_idx <= soc_mem_index_max(unit, FP_TCAMm));
    
    /* Next Write the Actions */
    BCM_IF_ERROR_RETURN(
        _field_raptor_policy_install(unit, f_ent, tcam_idx));

    /* Next write the IPBM */
    BCM_IF_ERROR_RETURN(
        _field_raptor_ipbm_install(unit, f_ent));

    /* Finally Write the TCAM, Valid bits = b11 */
    BCM_IF_ERROR_RETURN(
        _field_raptor_tcam_install(unit, f_ent, tcam_idx));
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_ipbm_entry_set
 * Purpose:
 *     Set a bit in IPBM in the chip's memory.
 *     Helper function for ipbm related functions
 * Parameters:
 *     unit     -   BCM Unit
 *     row      -   row in IPBM: The port/slice #
 *     col      -   col. in IPBM: The entry #
 *     enable   -   value (bit) to be written
 * Returns:
 *     BCM_E_PARAM - incorrect col.
 *     BEM_E_NONE
 * Note:
 */
STATIC int
_field_raptor_ipbm_entry_set(int unit, int row, 
                             int col, uint32 enable)
{
    fp_slice_entry_port_sel_entry_t ipbm_entry;
    soc_field_t all_fields[4] = {ENTRIES_PER_PORT_31_0f,
                                 ENTRIES_PER_PORT_63_32f,
                                 ENTRIES_PER_PORT_95_64f,
                                 ENTRIES_PER_PORT_127_96f};
    soc_field_t field;
    uint32 field_offsets[4] = {0, 32, 64, 96};
    uint32 index;
    uint32 curr_value, new_value;
    
    SOC_IF_ERROR_RETURN(READ_FP_SLICE_ENTRY_PORT_SELm
                        (unit, MEM_BLOCK_ANY, row, &ipbm_entry));
  
    field = all_fields[col/32];
    index = col - field_offsets[col/32];

    curr_value = soc_FP_SLICE_ENTRY_PORT_SELm_field32_get
                 (unit, &ipbm_entry, field);
    
    new_value = enable ?
                (curr_value | (1 << index)) :
                (curr_value & ~(1 << index));

    /* This is an optimization: writing only if the bit is different */
    if (new_value != curr_value) {
        soc_FP_SLICE_ENTRY_PORT_SELm_field32_set
                  (unit, &ipbm_entry, field, new_value);

        SOC_IF_ERROR_RETURN(WRITE_FP_SLICE_ENTRY_PORT_SELm
                            (unit, MEM_BLOCK_ALL, row, &ipbm_entry));
    }

    return BCM_E_NONE;
}
    
/*
 * Function:
 *     _field_raptor_ipbm_entry_get
 * Purpose:
 *     Get a bit in IPBM in the chip's memory.
 *     Helper function for ipbm related functions
 * Parameters:
 *     unit     -   BCM Unit
 *     row      -   row in IPBM: The port/slice #
 *     col      -   col. in IPBM: The entry #
 *     enable   -   (OUT) value (bit) read
 * Returns:
 *     BCM_E_PARAM - incorrect col.
 *     BEM_E_NONE
 * Note:
 */
STATIC int
_field_raptor_ipbm_entry_get(int unit, int row, 
                             int col, uint32 *enable)
{
    fp_slice_entry_port_sel_entry_t ipbm_entry;
    soc_field_t all_fields[4] = {ENTRIES_PER_PORT_31_0f,
                                 ENTRIES_PER_PORT_63_32f,
                                 ENTRIES_PER_PORT_95_64f,
                                 ENTRIES_PER_PORT_127_96f};
    soc_field_t field;
    uint32 field_offsets[4] = {0, 32, 64, 96};
    uint32 index, result;
    
    SOC_IF_ERROR_RETURN(READ_FP_SLICE_ENTRY_PORT_SELm
                        (unit, MEM_BLOCK_ANY, row, &ipbm_entry));
  
    field = all_fields[col/32];
    index = col - field_offsets[col/32];

    result = soc_FP_SLICE_ENTRY_PORT_SELm_field32_get
             (unit, &ipbm_entry, field);

    *enable = (result >> index) & 0x1; /* Return the bit at col. */
    
    return BCM_E_NONE;
}
    
/*
 * Function:
 *     _field_raptor_ipbm_install
 * Purpose:
 *     Write IPBM into the chip's memory. 
 * Parameters:
 *     unit     -   BCM Unit
 *     f_ent    -   Physical entry structure to be installed 
 *     tcam_idx - location
 * Returns:
 *     BCM_E_NONE
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_raptor_ipbm_install(int unit, _field_entry_t *f_ent)
{
    bcm_port_t p;
    bcm_pbmp_t pbmp, pbmp_mask;
    uint32 result;
    int    ipbm_index;
    _field_stage_t *stage_fc;
    bcm_port_config_t  port_config;
    
    pbmp = f_ent->pbmp.data;
    pbmp_mask = f_ent->pbmp.mask;

    /* Read device port configuration. */ 
    BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &port_config));

    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc));
    
    PBMP_ITER(port_config.all, p) {
        if (BCM_PBMP_MEMBER(pbmp_mask, p)) {
            if (BCM_PBMP_MEMBER(pbmp, p)) {
                result = 1;
            } else {
                result = 0;
            }
        } else {
            result = 1;
        }

        /* set result at the appropriate location */
        ipbm_index = _BCM_RP_FIELD_IPBM_INDEX(unit, p, f_ent->fs->slice_number);
        BCM_IF_ERROR_RETURN (_field_raptor_ipbm_entry_set(unit, ipbm_index, 
                                                          f_ent->slice_idx, 
                                                          result));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_tcam_install
 * Purpose:
 *     Write rules into the chip's memory. 
 * Parameters:
 *     unit     -   BCM Unit
 *     f_ent    -   Physical entry structure to be installed 
 *     tcam_idx - location in TCAM
 * Returns:
 *     BCM_E_XXX        On read/write errors
 *     BCM_E_NONE
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_raptor_tcam_install(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    fp_tcam_entry_t t_entry;

    assert(f_ent != NULL);
    assert(tcam_idx >= soc_mem_index_min(unit, FP_TCAMm));
    assert(tcam_idx <= soc_mem_index_max(unit, FP_TCAMm));
    
    /* Start with an empty TCAM entry */
    sal_memcpy(&t_entry, soc_mem_entry_null(unit, FP_TCAMm), 
               sizeof(t_entry));

    /* Extract the qualifier info from the entry structure. */
    BCM_IF_ERROR_RETURN(
        _field_raptor_tcam_get(unit, f_ent, &t_entry));

    /* Write the TCAM Table */
    SOC_IF_ERROR_RETURN
        (WRITE_FP_TCAMm(unit, MEM_BLOCK_ALL, tcam_idx, &t_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_tcam_get
 * Purpose:
 *    Get the rules to be written into tcam.
 * Parameters:
 *     unit      -  BCM Unit
 *     f_ent     -  entry structure to get tcam info from.
 *     t_entry - (OUT) TCAM entry
 * Returns:
 *     BCM_E_NONE  - Success
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_raptor_tcam_get(int unit, _field_entry_t *f_ent, fp_tcam_entry_t *t_entry)
{
    _field_tcam_t       *tcam = NULL;

    assert(f_ent     != NULL);
    assert(t_entry != NULL);

    tcam = &f_ent->tcam;

    soc_mem_field_set(unit, FP_TCAMm, (uint32 *)t_entry, KEYf,  tcam->key);
    soc_mem_field_set(unit, FP_TCAMm, (uint32 *)t_entry, MASKf, tcam->mask);

    soc_FP_TCAMm_field32_set(unit, t_entry, F4f, tcam->f4);
    soc_FP_TCAMm_field32_set(unit, t_entry, F4_MASKf, tcam->f4_mask);

    /* Qualify on IP_TYPE */
    if (BCM_FIELD_QSET_TEST(f_ent->group->qset,
                            bcmFieldQualifyIpType)) {
        int data, mask;
        switch (tcam->ip_type) {
            case bcmFieldIpTypeAny:
                data = 0x0;
                mask = 0x0;
                break;
            case bcmFieldIpTypeNonIp:
                data = 0x0;
                mask = 0x3;
                break;
            case bcmFieldIpTypeIpv4Not:
                data = 0x0;
                mask = 0x1;
                break;
            case bcmFieldIpTypeIpv4NoOpts:
                data = 0x1;
                mask = 0x3;
                break;
            case bcmFieldIpTypeIpv4WithOpts:
                data = 0x3;
                mask = 0x3;
                break;
            case bcmFieldIpTypeIpv4Any:
                data = 0x1;
                mask = 0x1;
                break;
            case bcmFieldIpTypeIpv6:
                data = 0x2;
                mask = 0x3;
                break;
            case bcmFieldIpTypeIpv6Not:
            default:
                /* Should not come here */
                data = 0x0;
                mask = 0x0;
                break;
        }
        soc_FP_TCAMm_field32_set(unit, t_entry, IP_TYPEf, data);
        soc_FP_TCAMm_field32_set(unit, t_entry, IP_TYPE_MASKf, mask);
    } else {
       soc_FP_TCAMm_field32_set(unit, t_entry, IP_TYPEf, 0);
       soc_FP_TCAMm_field32_set(unit, t_entry, IP_TYPE_MASKf, 0);
    }

    /* Qualify on HiGig packets. */
    if (BCM_FIELD_QSET_TEST(f_ent->group->qset,
                            bcmFieldQualifyHiGig)) {
        soc_FP_TCAMm_field32_set(unit, t_entry, HIGIG_PKTf, tcam->higig ? 1 : 0);
        soc_FP_TCAMm_field32_set(unit, t_entry, HIGIG_PKT_MASKf, tcam->higig_mask ? 1 : 0);
    } else {
        soc_FP_TCAMm_field32_set(unit, t_entry, HIGIG_PKTf, 0);
        soc_FP_TCAMm_field32_set(unit, t_entry, HIGIG_PKT_MASKf, 0);
    }
#if defined(BCM_RAVEN_SUPPORT)
    if (soc_mem_field_valid(unit, FP_TCAMm, RESERVED_MASKf)) {
        soc_mem_field32_set(unit, FP_TCAMm, t_entry, RESERVED_MASKf, 0);
    }
    if (soc_mem_field_valid(unit, FP_TCAMm, RESERVED_KEYf)) {
        soc_mem_field32_set(unit, FP_TCAMm, t_entry, RESERVED_KEYf, 0);
    }
#endif /* BCM_RAVEN_SUPPORT */
    /* Set the Valid bits */
    soc_FP_TCAMm_field32_set(unit, t_entry, VALIDf, 3);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_stat_action_set
 *
 * Purpose:
 *     Get counter portion of Policy Table.
 *
 * Parameters:
 *     unit      - (IN)BCM device number. 
 *     f_ent     - (IN)Software entry structure to get tcam info from.
 *     mem       - (IN)Policy table memory. 
 *     buf       - (IN/OUT)Hardware policy entry
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_raptor_stat_action_set(int unit, _field_entry_t *f_ent,
                              fp_policy_table_entry_t *p_entry)
{
    _field_stat_t  *f_st;  /* Field policer descriptor. */  
    int mode;              /* Counter hw mode.          */                    
    int idx;               /* Counter index.            */

    /* Input parameter check. */
    if ((NULL == f_ent) || (NULL == p_entry))  {
        return (BCM_E_PARAM);
    }

    /* Disable counting if counter was not attached to the entry. */
    if ((0 == (f_ent->statistic.flags & _FP_ENTRY_STAT_INSTALLED)) ||
        ((f_ent->flags & _FP_ENTRY_PRIMARY) && 
         (f_ent->flags & _FP_ENTRY_STAT_IN_SECONDARY_SLICE))) {
        mode = 0;
        idx = 0;
    } else {
        /* Get statistics entity description structure. */
        BCM_IF_ERROR_RETURN(_bcm_field_stat_get(unit, f_ent->statistic.sid, &f_st));
        idx = f_st->hw_index;
        mode = f_st->hw_mode;
        /* Adjust counter hw mode for COUNTER_MODE_YES_NO/NO_YES */
        if (f_ent->statistic.flags & _FP_ENTRY_STAT_USE_ODD) {
            mode++;
        }
    }

    soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, COUNTER_INDEXf, idx);
    soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, COUNTER_MODEf, mode);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_raptor_policy_install
 * Purpose:
 *     Write policy into the chip's memory. 
 * Parameters:
 *     unit     -   BCM Unit
 *     f_ent    -   Physical entry structure to be installed 
 *     tcam_idx - location in TCAM
 * Returns:
 *     BCM_E_XXX        On read/write errors
 *     BCM_E_NONE
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_raptor_policy_install(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    fp_policy_table_entry_t p_entry;
    _field_action_t              *fa = NULL;

    assert(f_ent != NULL);
    assert(tcam_idx >= soc_mem_index_min(unit, FP_POLICY_TABLEm));
    assert(tcam_idx <= soc_mem_index_max(unit, FP_POLICY_TABLEm));
    
    /* Start with an empty POLICY entry */
    sal_memcpy(&p_entry, soc_mem_entry_null(unit, FP_POLICY_TABLEm), 
               sizeof(p_entry));

    /* Extract the policy info from the entry structure. */
    for (fa = f_ent->actions;
            ((fa != NULL) && (_FP_ACTION_VALID & fa->flags)); fa = fa->next) {
        if ((f_ent->flags & _FP_ENTRY_POLICER_IN_SECONDARY_SLICE) && 
            (_FP_ACTION_IS_COLOR_BASED(fa->action))) {
            continue;
        }
        BCM_IF_ERROR_RETURN(
            _field_raptor_action_get(unit, f_ent, tcam_idx, fa, &p_entry));
    }

    /* Handle color dependence/independence */
    if (soc_feature(unit, soc_feature_field_color_indep)) {
        if (f_ent->flags & _FP_ENTRY_COLOR_INDEPENDENT) {
            soc_mem_field32_set(unit, FP_POLICY_TABLEm, &p_entry,
                                GREEN_TO_PIDf, 1);
        } else {
            soc_mem_field32_set(unit, FP_POLICY_TABLEm, &p_entry,
                                GREEN_TO_PIDf, 0);
        }
    }

    /* If the entry has a meter, get its index. */
    if (f_ent->policer[0].flags & _FP_POLICER_INSTALLED) {
        BCM_IF_ERROR_RETURN(_field_raptor_meter_action_set(unit, f_ent, 
                                                           &p_entry));
    }
   
    /* Install statistics entity. */
    BCM_IF_ERROR_RETURN(_field_raptor_stat_action_set(unit, f_ent, &p_entry));

    /* Write the POLICY Table */
    SOC_IF_ERROR_RETURN
        (WRITE_FP_POLICY_TABLEm(unit, MEM_BLOCK_ALL, tcam_idx, &p_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_action_get
 * Purpose:
 *     Get the actions to be written
 * Parameters:
 *     unit     - BCM device number
 *     tcam_idx - index into TCAM
 *     fa       - field action 
 *     p_entry  - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     FP unit lock should be held by calling function.
 */
STATIC int
_field_raptor_action_get(int unit, _field_entry_t *f_ent, int tcam_idx, 
                         _field_action_t *fa, fp_policy_table_entry_t *p_entry)
{
    uint32    reg_val;
    uint32    redir_field;
    uint8     raptor_actions_enable = TRUE;    

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "FP(unit %d) vverb: BEGIN _field_raptor_action_get(eid=%d, tcam_idx=0x%x, "),
               unit, f_ent->eid, tcam_idx));

#if defined(BCM_RAVEN_SUPPORT) 
    /* First deal with Raven-only actions */
    if (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        raptor_actions_enable = FALSE;
        switch (fa->action) {
          case bcmFieldActionGpCosQNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionGpPrioPktAndIntCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 4);
              break;
          case bcmFieldActionGpPrioPktAndIntNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 5);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionGpPrioPktAndIntTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 6);
              break;
          case bcmFieldActionGpPrioPktAndIntCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 7);
              break;
          case bcmFieldActionGpPrioPktCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1); 
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 8);
              break;
          case bcmFieldActionGpPrioPktNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 9);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionGpPrioPktTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 10);
              break;
          case bcmFieldActionGpPrioPktCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 11);
              break;
          case bcmFieldActionGpPrioIntCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 12);
              break;
          case bcmFieldActionGpPrioIntNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 13);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionGpPrioIntTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 14);
              break;
          case bcmFieldActionGpPrioIntCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 15);
              break;
          case bcmFieldActionYpCosQNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionYpPrioPktAndIntCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 4);
              break;
          case bcmFieldActionYpPrioPktAndIntNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 5);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionYpPrioPktAndIntTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 6);
              break;
          case bcmFieldActionYpPrioPktAndIntCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 7);
              break;
          case bcmFieldActionYpPrioPktCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1); 
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 8);
              break;
          case bcmFieldActionYpPrioPktNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 9);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionYpPrioPktTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 10);
              break;
          case bcmFieldActionYpPrioPktCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 11);
              break;
          case bcmFieldActionYpPrioIntCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 12);
              break;
          case bcmFieldActionYpPrioIntNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 13);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionYpPrioIntTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 14);
              break;
          case bcmFieldActionYpPrioIntCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 15);
              break;
          case bcmFieldActionRpCosQNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionRpPrioPktAndIntCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 4);
              break;
          case bcmFieldActionRpPrioPktAndIntNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 5);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionRpPrioPktAndIntTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 6);
              break;
          case bcmFieldActionRpPrioPktAndIntCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 7);
              break;
          case bcmFieldActionRpPrioPktCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1); 
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 8);
              break;
          case bcmFieldActionRpPrioPktNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 9);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionRpPrioPktTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 10);
              break;
          case bcmFieldActionRpPrioPktCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 11);
              break;
          case bcmFieldActionRpPrioIntCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 12);
              break;
          case bcmFieldActionRpPrioIntNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 13);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionRpPrioIntTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 14);
              break;
          case bcmFieldActionRpPrioIntCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 15);
              break;
          default:
              raptor_actions_enable = TRUE; /* Check for Raptor  / 
                                               Raven common actions. */
              break;
        }
    }
#endif  /* BCM_RAVEN_SUPPORT */
    if (raptor_actions_enable) {
        switch (fa->action) {
          case bcmFieldActionCosQNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionCosQCpuNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 2);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionPrioPktAndIntCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 4);
              break;
          case bcmFieldActionPrioPktAndIntNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 5);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionPrioPktAndIntTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 6);
              break;
          case bcmFieldActionPrioPktAndIntCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 7);
              break;
          case bcmFieldActionPrioPktCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 8);
              break;
          case bcmFieldActionPrioPktNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 9);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionPrioPktTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 10);
              break;
          case bcmFieldActionPrioPktCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 11);
              break;
          case bcmFieldActionPrioIntCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 12);
              break;
          case bcmFieldActionPrioIntNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 13);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionPrioIntTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 14);
              break;
          case bcmFieldActionPrioIntCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 15);
              break;
          case bcmFieldActionTosNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_DSCP_TOSf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWDSCP_TOSf, fa->param[0]);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_DSCPf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_DSCPf, fa->param[0]);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_DSCPf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_DSCPf, fa->param[0]);
              break;
          case bcmFieldActionTosCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_DSCP_TOSf, 2);
              break;
          case bcmFieldActionDscpNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_DSCP_TOSf, 3);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWDSCP_TOSf, fa->param[0]);
              break;
          case bcmFieldActionTosCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_DSCP_TOSf, 4);
              break;
          case bcmFieldActionDscpCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_DSCP_TOSf, 4);
              break;
          case bcmFieldActionCopyToCpu:
              /* if the action is CopyToCpu, and param0 is nonzero, which means the user
               * wants to install the value of param1 as the MATCHED_RULEf, we want to
               * make sure that param1 can fit into 8 bits
               */
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_COPY_TO_CPUf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_COPY_TO_CPUf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, COPY_TO_CPUf, 1);
              if(fa->param[0] != 0) {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, MATCHED_RULEf, fa->param[1]);
              }
              break;
          case bcmFieldActionCopyToCpuCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_COPY_TO_CPUf, 2);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_COPY_TO_CPUf, 2);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, COPY_TO_CPUf, 2);
              break;
          case bcmFieldActionSwitchToCpuCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_COPY_TO_CPUf, 3);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_COPY_TO_CPUf, 3);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, COPY_TO_CPUf, 3);
              break;
          case bcmFieldActionRedirect: /* param0 = modid, param1 = port/tgid */
              /* modid = 4 bits, port/tgid = 7 bits */
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PACKET_REDIRECTIONf, 1);
              if (soc_feature(unit, soc_feature_table_hi)) {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTION_LOf, (fa->param[0] << 7) | fa->param[1]);
              } else {
                  redir_field = PolicyGet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTIONf);
                  redir_field &= (0xfffff800);  /* Zero mod/port[11:0] bits */
                  redir_field |= (fa->param[0] << 7) | fa->param[1];
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTIONf, redir_field);       
              }
              break;
          case bcmFieldActionRedirectTrunk:    /* param0 = trunk ID */
              /* Trunk indicator is bit 6 (7th from right) */
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PACKET_REDIRECTIONf, 1);
              if (soc_feature(unit, soc_feature_table_hi)) {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTION_LOf, (fa->param[0] & 0x3f) | 0x40);
              } else {
                  redir_field = PolicyGet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTIONf);
                  redir_field &= (0xffffff80);  /* Zero trunk [6:0] bits */
                  redir_field |= (fa->param[0] & 0x3f) | 0x40;
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTIONf, redir_field);       
              }
              break;
          case bcmFieldActionRedirectCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PACKET_REDIRECTIONf, 2);
              break;
          case bcmFieldActionRedirectPbmp:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PACKET_REDIRECTIONf, 3);
              if (soc_feature(unit, soc_feature_table_hi)) {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTION_LOf, fa->param[0]);
#if defined(BCM_RAPTOR1_SUPPORT)
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTION_HIf, fa->param[1]);
#endif
              } else {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTIONf, fa->param[0]);
              }
              break;
          case bcmFieldActionEgressMask:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PACKET_REDIRECTIONf, 4);
              if (soc_feature(unit, soc_feature_table_hi)) {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTION_LOf, fa->param[0]);
#if defined(BCM_RAPTOR1_SUPPORT)
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTION_HIf, fa->param[1]);
#endif
              } else {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTIONf, fa->param[0]);
              }
              break;
          case bcmFieldActionDrop:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_DROPf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_DROPf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, DROPf, 1);
              break;
          case bcmFieldActionDropCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_DROPf, 2);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_DROPf, 2);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, DROPf, 2);
              break;
          case bcmFieldActionMirrorOverride:
              if (soc_feature(unit, soc_feature_field_mirror_ovr)) {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, MIRROR_OVERRIDEf, 1);
              } else {
                  return BCM_E_PARAM;
              }
              break;
          case bcmFieldActionMirrorIngress:    /* param0=modid, param1=port/tgid */
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, MIRRORf, 
                        PolicyGet(unit, FP_POLICY_TABLEm, p_entry, MIRRORf) | 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, IM_MTP_INDEXf, fa->hw_index);
              if (soc_feature(unit, soc_feature_field_mirror_pkts_ctl)) {
                  SOC_IF_ERROR_RETURN(READ_ING_CONFIGr(unit, &reg_val));
                  soc_reg_field_set(unit, ING_CONFIGr, &reg_val,
                                    ENABLE_FP_FOR_MIRROR_PKTSf, 1 );
                  SOC_IF_ERROR_RETURN(WRITE_ING_CONFIGr(unit, reg_val));
              }
              break;
          case bcmFieldActionMirrorEgress:     /* param0=modid, param1=port/tgid */
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, MIRRORf, 
                        PolicyGet(unit, FP_POLICY_TABLEm, p_entry, MIRRORf) | 2);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, EM_MTP_INDEXf, fa->hw_index);
              if (soc_feature(unit, soc_feature_field_mirror_pkts_ctl)) {
                  SOC_IF_ERROR_RETURN(READ_ING_CONFIGr(unit, &reg_val));
                  soc_reg_field_set(unit, ING_CONFIGr, &reg_val,
                                    ENABLE_FP_FOR_MIRROR_PKTSf, 1 );
                  SOC_IF_ERROR_RETURN(WRITE_ING_CONFIGr(unit, reg_val));
              }
              break;
#ifdef INCLUDE_L3
          case bcmFieldActionL3ChangeVlan:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, L3SW_CHANGE_MACDA_OR_VLANf, 1);
              BCM_IF_ERROR_RETURN
                  (_bcm_field_policy_set_l3_info(unit, FP_POLICY_TABLEm, 
                                                 fa->param[0], (uint32 *)p_entry)); 
              break;
          case bcmFieldActionL3ChangeVlanCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, L3SW_CHANGE_MACDA_OR_VLANf, 2);
              break;
          case bcmFieldActionL3ChangeMacDa:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, L3SW_CHANGE_MACDA_OR_VLANf, 4);
              BCM_IF_ERROR_RETURN
                  (_bcm_field_policy_set_l3_info(unit, FP_POLICY_TABLEm, 
                                                 fa->param[0], (uint32 *)p_entry)); 
              break;
          case bcmFieldActionL3ChangeMacDaCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, L3SW_CHANGE_MACDA_OR_VLANf, 5);
              break;
          case bcmFieldActionL3Switch:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, L3SW_CHANGE_MACDA_OR_VLANf, 6);
              BCM_IF_ERROR_RETURN
                  (_bcm_field_policy_set_l3_info(unit, FP_POLICY_TABLEm, 
                                                 fa->param[0], (uint32 *)p_entry)); 
              break;
          case bcmFieldActionL3SwitchCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, L3SW_CHANGE_MACDA_OR_VLANf, 7);
              break;
#endif /* INCLUDE_L3 */
          case bcmFieldActionAddClassTag:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, L3SW_CHANGE_MACDA_OR_VLANf, 3);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CLASSIFICATION_TAGf, fa->param[0]);
              break;
          case bcmFieldActionEcnNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, ECN_CNGf, 0x1);
              /* coverity[MISSING_BREAK : FALSE] */
              /* passthru */
          case bcmFieldActionDropPrecedence:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_DROP_PRECEDENCEf, fa->param[0]);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_DROP_PRECEDENCEf, fa->param[0]);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, DROP_PRECEDENCEf, fa->param[0]);
              break;
          case bcmFieldActionRpDrop:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_DROPf, 1);
              break;
          case bcmFieldActionRpDropCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_DROPf, 2);
              break;
          case bcmFieldActionRpDropPrecedence:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_DROP_PRECEDENCEf, fa->param[0]);
              break;
          case bcmFieldActionRpCopyToCpu:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_COPY_TO_CPUf, 1);
              if(fa->param[0] != 0) {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, MATCHED_RULEf, fa->param[1]);
              }
              break;
          case bcmFieldActionRpCopyToCpuCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_COPY_TO_CPUf, 2);
              break;
          case bcmFieldActionRpDscpNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_DSCPf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_DSCPf, fa->param[0]);
              break;
          case bcmFieldActionYpDrop:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_DROPf, 1);
              break;
          case bcmFieldActionYpDropCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_DROPf, 2);
              break;
          case bcmFieldActionYpDropPrecedence:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_DROP_PRECEDENCEf, fa->param[0]);
              break;
          case bcmFieldActionYpCopyToCpu:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_COPY_TO_CPUf, 1);
              if(fa->param[0] != 0) {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, MATCHED_RULEf, fa->param[1]);
              }
              break;
          case bcmFieldActionYpCopyToCpuCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_COPY_TO_CPUf, 2);
              break;
          case bcmFieldActionYpDscpNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_DSCPf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_DSCPf, fa->param[0]);
              break;
          case bcmFieldActionGpDrop:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, DROPf, 1);
              break;
          case bcmFieldActionGpDropCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, DROPf, 2);
              break;
          case bcmFieldActionGpDropPrecedence:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, DROP_PRECEDENCEf, fa->param[0]);
              break;
          case bcmFieldActionGpCopyToCpu:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, COPY_TO_CPUf, 1);
              if(fa->param[0] != 0) {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, MATCHED_RULEf, fa->param[1]);
              }
              break;
          case bcmFieldActionGpCopyToCpuCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, COPY_TO_CPUf, 2);
              break;
          case bcmFieldActionGpDscpNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_DSCP_TOSf, 3);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWDSCP_TOSf, fa->param[0]);
              break;
          default:
              LOG_ERROR(BSL_LS_BCM_FP,
                        (BSL_META_U(unit,
                                    "FP(unit %d) Error: Unknown action (%d)\n"),
                         unit, (uint32)fa->action));
              return BCM_E_PARAM;
        }
    }

    fa->flags &= ~_FP_ACTION_DIRTY; /* Mark action as installed. */

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "FP(unit %d) vverb: END _field_raptor_action_get()\n"),
               unit));
    return BCM_E_NONE;

}

/*
 * Function:
 *     _field_raptor_action_params_check
 * Purpose:
 *     Check field action parameters.
 * Parameters:
 *     unit     - BCM device number
 *     f_ent    - Field entry structure.
 *     fa       - field action 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_raptor_action_params_check(int unit,_field_entry_t *f_ent, 
                                  _field_action_t *fa)
{
#if defined(INCLUDE_L3)
    uint32 flags;            /* L3 forwarding flags.    */ 
    int nh_ecmp_id;          /* Next hop/Ecmp group id. */
#endif /* INCLUDE_L3 */
    int rv;                  /* Operation return value. */ 
    int raptor_actions_enable = TRUE;

#if defined(BCM_RAVEN_SUPPORT) 
    /* First deal with Raven-only actions */
    if (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        raptor_actions_enable = FALSE;
        switch (fa->action) {
          case bcmFieldActionGpCosQNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionGpPrioPktAndIntNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionGpPrioPktNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionGpPrioIntNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionYpCosQNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, YP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionYpPrioPktAndIntNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, YP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionYpPrioPktNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, YP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionYpPrioIntNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, YP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionRpCosQNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, RP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionRpPrioPktAndIntNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, RP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionRpPrioPktNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, RP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionRpPrioIntNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, RP_NEWPRIf, fa->param[0]);
              break;
          default:
              raptor_actions_enable = TRUE; /* Check for Raptor  / 
                                               Raven common actions. */
              break;
        }
    }
#endif  /* BCM_RAVEN_SUPPORT */
    if (raptor_actions_enable) {
        switch (fa->action) {
          case bcmFieldActionCosQNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionCosQCpuNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionPrioPktAndIntNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionPrioPktNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionPrioIntNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionTosNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWDSCP_TOSf, fa->param[0]);
              PolicyCheck(unit, FP_POLICY_TABLEm, YP_DSCPf, fa->param[0]);
              PolicyCheck(unit, FP_POLICY_TABLEm, RP_DSCPf, fa->param[0]);
              break;
          case bcmFieldActionDscpNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWDSCP_TOSf, fa->param[0]);
              break;
          case bcmFieldActionCopyToCpu:
          case bcmFieldActionRpCopyToCpu:
          case bcmFieldActionYpCopyToCpu:
          case bcmFieldActionGpCopyToCpu:
              /* if the action is Gp/Yp/Rp CopyToCpu, and param0 is nonzero,
               * which means the user wants to install the value of param1 as
               * the MATCHED_RULEf, we want to make sure that param1 can fit
               * into 8 bits.
               */
              if ((fa->param[0] != 0) && (fa->param[1] >= (1 << 8))) {
                  return BCM_E_PARAM;
              }

              if(fa->param[0] != 0) {
                  PolicyCheck(unit, FP_POLICY_TABLEm, MATCHED_RULEf, fa->param[1]);
              }
              break;
          case bcmFieldActionRedirect: 
              /* param0 = modid, param1 = port/tgid */
              /* modid = 4 bits, port/tgid = 7 bits */
              rv = _bcm_field_action_dest_check(unit, fa);
              BCM_IF_ERROR_RETURN(rv);
              if (fa->param[0] > 0xf || fa->param[1] > 0x7f) {
                  return BCM_E_PARAM;
              }
              break;
          case bcmFieldActionRedirectTrunk:    /* param0 = trunk ID */
              if (fa->param[0] > 0x3f) {
                  return BCM_E_PARAM;
              }
              break;
          case bcmFieldActionRedirectPbmp:
              if (soc_feature(unit, soc_feature_table_hi)) {
                  PolicyCheck(unit, FP_POLICY_TABLEm, REDIRECTION_LOf, fa->param[0]);
#if defined(BCM_RAPTOR1_SUPPORT)
                  PolicyCheck(unit, FP_POLICY_TABLEm, REDIRECTION_HIf, fa->param[1]);
#endif
              } else {
                  PolicyCheck(unit, FP_POLICY_TABLEm, REDIRECTIONf, fa->param[0]);
              }
              break;
          case bcmFieldActionEgressMask:
              if (soc_feature(unit, soc_feature_table_hi)) {
                  PolicyCheck(unit, FP_POLICY_TABLEm, REDIRECTION_LOf, fa->param[0]);
#if defined(BCM_RAPTOR1_SUPPORT)
                  PolicyCheck(unit, FP_POLICY_TABLEm, REDIRECTION_HIf, fa->param[1]);
#endif
              } else {
                  PolicyCheck(unit, FP_POLICY_TABLEm, REDIRECTIONf, fa->param[0]);
              }
              break;
          case bcmFieldActionMirrorIngress: 
          case bcmFieldActionMirrorEgress:
              rv = _bcm_field_action_dest_check(unit, fa);
              BCM_IF_ERROR_RETURN(rv);
              break;
#ifdef INCLUDE_L3 
          case bcmFieldActionL3ChangeVlan:
          case bcmFieldActionL3ChangeMacDa:
          case bcmFieldActionL3Switch:
              rv = _bcm_field_policy_set_l3_nh_resolve(unit,  fa->param[0],
                                                       &flags, &nh_ecmp_id);

              BCM_IF_ERROR_RETURN(rv);
              if (flags & BCM_L3_MULTIPATH) {
                  PolicyCheck(unit, FP_POLICY_TABLEm, ECMP_PTRf, nh_ecmp_id);
              } else {
                  PolicyCheck(unit, FP_POLICY_TABLEm, NEXT_HOP_INDEXf, nh_ecmp_id);
              }
              break;
#endif /* INCLUDE_L3 */
          case bcmFieldActionAddClassTag:
              PolicyCheck(unit, FP_POLICY_TABLEm, CLASSIFICATION_TAGf, fa->param[0]);
              break;
          case bcmFieldActionEcnNew:
          case bcmFieldActionDropPrecedence:
              PolicyCheck(unit, FP_POLICY_TABLEm, RP_DROP_PRECEDENCEf, fa->param[0]);
              PolicyCheck(unit, FP_POLICY_TABLEm, YP_DROP_PRECEDENCEf, fa->param[0]);
              PolicyCheck(unit, FP_POLICY_TABLEm, DROP_PRECEDENCEf, fa->param[0]);
              break;
          case bcmFieldActionRpDropPrecedence:
              PolicyCheck(unit, FP_POLICY_TABLEm, RP_DROP_PRECEDENCEf, fa->param[0]);
              break;
          case bcmFieldActionRpDscpNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, RP_DSCPf, fa->param[0]);
              break;
          case bcmFieldActionYpDropPrecedence:
              PolicyCheck(unit, FP_POLICY_TABLEm, YP_DROP_PRECEDENCEf, fa->param[0]);
              break;
          case bcmFieldActionYpDscpNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, YP_DSCPf, fa->param[0]);
              break;
          case bcmFieldActionGpDropPrecedence:
              PolicyCheck(unit, FP_POLICY_TABLEm, DROP_PRECEDENCEf, fa->param[0]);
              break;
          case bcmFieldActionGpDscpNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWDSCP_TOSf, fa->param[0]);
              break;
          default:
              return (BCM_E_NONE);
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_meter_action_set
 * Purpose:
 *     Get metering portion of Policy Table.
 * Parameters:
 *     unit      -  BCM Unit
 *     f_ent     -  entry structure to get tcam info from.
 *     p_entry - (OUT) Policy hardware entry
 * Returns:
 *     BCM_E_NONE  - Success
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_raptor_meter_action_set(int unit, _field_entry_t *f_ent,
                        fp_policy_table_entry_t *p_entry)
{
    _field_policer_t *f_pl;
    uint32 meter_pair_mode; 

    if ((NULL == f_ent) || (NULL == p_entry)) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN
        (_bcm_field_policer_get(unit, f_ent->policer[0].pid, &f_pl));

    if (((f_ent->flags & _FP_ENTRY_PRIMARY) && 
         (f_ent->flags & _FP_ENTRY_POLICER_IN_SECONDARY_SLICE))) {
        /* If no meter, install default meter pair mode.
         * Default meter pair mode doesn't need to burn a real meter.
         */
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, METER_PAIR_MODEf, 0);
        return (BCM_E_NONE);
    }

    /* Get hw encoding for meter mode. */
    BCM_IF_ERROR_RETURN
        (_bcm_field_meter_pair_mode_get(unit, f_pl, &meter_pair_mode));


    /* Set the even and odd indexes from the entry */
    soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, METER_INDEX_EVENf,
                                     f_pl->hw_index);
    soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, METER_INDEX_ODDf, 
                                     f_pl->hw_index);

    /* 
     * Flow mode is the only one that cares about the test and update bits.
     * Even = BCM_FIELD_METER_PEAK
     * Odd = BCM_FIELD_METER_COMMITTED
     */
    if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry,
                                         METER_TEST_EVENf,    1);
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry,
                                         METER_UPDATE_EVENf,  1);
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry,
                                         METER_TEST_ODDf,   0);
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry,
                                         METER_UPDATE_ODDf, 0);
    } else if (_FP_POLICER_COMMITTED_HW_METER(f_pl)) {
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry,
                                         METER_TEST_EVENf,    0);
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry,
                                         METER_UPDATE_EVENf,  0);
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry,
                                         METER_TEST_ODDf,   1);
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry,
                                         METER_UPDATE_ODDf, 1);
    }

    soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, METER_PAIR_MODEf,
                                     meter_pair_mode);
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_entry_remove
 * Purpose:
 *     Remove a previously installed physical entry.
 * Parameters:
 *     unit   - BCM device number
 *     f_ent  - Physical entry data
 *     tcam_idx - TCAM index to remove entry from.
 * Returns:
 *     BCM_E_XXX
 *     BCM_E_NONE
 * Notes:
 *     FP unit lock should be held by calling function.
 *     Not clearing the policy, ipbm tables; only TCAM.
 */
STATIC int
_field_raptor_entry_remove(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    fp_tcam_entry_t          t_entry;

    sal_memcpy(&t_entry, soc_mem_entry_null(unit, FP_TCAMm), 
               sizeof(t_entry));
    soc_FP_TCAMm_field32_set(unit, &t_entry, VALIDf, 0);

    SOC_IF_ERROR_RETURN(WRITE_FP_TCAMm(unit, MEM_BLOCK_ALL,
                                       tcam_idx, &t_entry));

    f_ent->flags |= _FP_ENTRY_DIRTY; /* Mark entry as not installed */

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_tables_entry_clear
 * Purpose:
 *     Writes a null entry into the the tables.
 * Parameters:
 *     unit      -   (IN)BCM device number.
 *     stage_id  -   (IN)FP stage pipeline id. 
 *     tcam_idx  -   (IN)Entry tcam index.
 * Returns:
 *     BCM_E_XXX
 * Note:
 *     Unit lock should be held by calling function.
 *     Not clearing the policy, ipbm tables, only TCAM.
 */
STATIC int
_field_raptor_tables_entry_clear(int unit, _field_entry_t *f_ent,
                                 _field_stage_id_t stage_id, int tcam_idx)
{
    fp_tcam_entry_t t_entry;

    if (_BCM_FIELD_STAGE_INGRESS != stage_id) {
        return (BCM_E_PARAM);
    }

    /* Memory index sanity check. */
    if (tcam_idx > soc_mem_index_max(unit, FP_TCAMm)) {
        return (BCM_E_PARAM);
    }  

    sal_memcpy(&t_entry, soc_mem_entry_null(unit, FP_TCAMm), 
               sizeof(t_entry));

    soc_FP_TCAMm_field32_set(unit, &t_entry, VALIDf, 0);

    SOC_IF_ERROR_RETURN(WRITE_FP_TCAMm(unit, MEM_BLOCK_ALL,
                                       tcam_idx, &t_entry));

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_raptor_entry_rule_copy
 * Purpose:
 *     Helper function for _field_raptor_entry_move_single/double/triple
 * Parameters:
 *     unit           - BCM unit
 *     old_index      - old index
 *     new_index      - new_index
 * Returns:
 *     BCM_E_XXX      - Hardware R/W errors
 *     BCM_E_NONE     - Success
 * Note:
 */
STATIC int
_field_raptor_entry_rule_copy(int unit, int old_index, int new_index)
{
    fp_tcam_entry_t t_entry;

    /* Read the entry rules from current tcam index. */
    SOC_IF_ERROR_RETURN(READ_FP_TCAMm(unit, MEM_BLOCK_ANY, 
                                      old_index, &t_entry));

    /* Write these at the new index */
    SOC_IF_ERROR_RETURN(WRITE_FP_TCAMm(unit, MEM_BLOCK_ALL, 
                                       new_index, &t_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_entry_rule_del
 * Purpose:
 *     Helper function for _field_raptor_entry_move_single/double/triple
 * Parameters:
 *     unit       - BCM unit
 *     index      - old index
 * Returns:
 *     BCM_E_XXX      - Hardware R/W errors
 *     BCM_E_NONE     - Success
 * Note:
 */
STATIC int
_field_raptor_entry_rule_del(int unit, int index)
{
    fp_tcam_entry_t t_entry;

    /* Invalidate  index entry. */
    sal_memcpy(&t_entry, soc_mem_entry_null(unit, FP_TCAMm), 
               sizeof(t_entry));
    soc_FP_TCAMm_field32_set(unit, &t_entry, VALIDf, 0);

    SOC_IF_ERROR_RETURN(WRITE_FP_TCAMm(unit, MEM_BLOCK_ALL, 
                                       index, &t_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_entry_ipbm_copy
 * Purpose:
 *     Helper function for _field_raptor_entry_move_single/double/triple
 * Parameters:
 *     unit         - BCM device number.
 *     f_ent        - Field entry structure.
 *     part         - Entry part.
 *     new_tcam_idx - New tcam index.
 * Returns:
 *     
 * Note:
 */
STATIC int
_field_raptor_entry_ipbm_copy(int unit, _field_entry_t *f_ent, 
                              int part, int tcam_idx_new)
{
    bcm_port_config_t  port_config;/* Device port config structure. */
    int new_slice_numb = 0;        /* Entry new slice number.       */
    int new_slice_idx = 0;         /* Entry new offset in a slice   */ 
    _field_stage_t *stage_fc;      /* Stage field control structure.*/    
    int old_slice_num;             /* Entry's old slice number.     */
    bcm_port_t p;                  /* Port number iterator.         */
    uint32 value = 0;              /* IPBM value.                   */
    int    ipbm_index;             /* IPBM index.                   */
    int    rv;                     /* Opeartion return status.      */

    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    /* Read device port configuration. */ 
    BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &port_config));

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    old_slice_num = f_ent->fs->slice_number;

    /* Calculate primary entry new slice & offset in the slice. */
    rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc, _FP_DEF_INST,
                                             tcam_idx_new, &new_slice_numb,
                                             &new_slice_idx);
    BCM_IF_ERROR_RETURN(rv);

    PBMP_ITER(port_config.all, p) {
        ipbm_index = _BCM_RP_FIELD_IPBM_INDEX(unit, p, old_slice_num);
        BCM_IF_ERROR_RETURN(_field_raptor_ipbm_entry_get(unit, ipbm_index, 
                                                         f_ent->slice_idx, 
                                                         &value));
        ipbm_index = _BCM_RP_FIELD_IPBM_INDEX(unit, p, new_slice_numb);
        BCM_IF_ERROR_RETURN(_field_raptor_ipbm_entry_set(unit, ipbm_index, 
                                                         new_slice_idx, value));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_entry_move
 * Purpose:
 *     Moves an entry (rules + policies) from one index to another. 
 *     It copies the values in hardware from the old index 
 *     to the new index. The old index is then cleared and set invalid.
 * Parameters:
 *     unit           - BCM device number.
 *     f_ent          - Field entry pointer. 
 *     parts_count    - (IN) Field entry parts count.
 *     tcam_idx_old   - (IN) Source entry tcam index.
 *     tcam_idx_new   - (IN) Destination entry tcam index.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_raptor_entry_move(int unit, _field_entry_t *f_ent, int parts_count,
                         int *tcam_idx_old, int *tcam_idx_new)
{
    fp_policy_table_entry_t     p_entry[_FP_MAX_ENTRY_WIDTH];
    _field_policer_t            *f_pl = NULL; /* Field policer descriptor.   */
    _field_stat_t               *f_st = NULL; /* Field statistic descriptor. */
    _field_group_t              *fg;          /* Field group structure.      */
    int new_slice_numb = 0;                   /* Entry new slice number.     */
    int new_slice_idx = 0;                    /* Entry new offset in a slice */ 
    _field_stage_t *stage_fc;                 /* Stage control structure.    */
    int idx;                                  /* Entry parts iterator.       */
    int rv;                                   /* Operation return status.    */
    int      is_backup_entry = 0; /* Flag ,checks the entry is backup entry or not*/
    int const  tcam_idx_max = soc_mem_index_max(unit, FP_TCAMm);

    if ((NULL == f_ent) || (NULL == tcam_idx_old) || (NULL == tcam_idx_new)) {
        return (BCM_E_PARAM);
    }

    fg = f_ent->group;

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Get number of entry parts. */
    rv = _bcm_field_entry_tcam_parts_count (unit, fg->stage_id, 
                                            fg->flags, &parts_count);
    BCM_IF_ERROR_RETURN(rv);

    /* Update policy entry if moving across the slices. */
    if (((NULL != f_ent->ent_copy) && 
                (f_ent->ent_copy->eid ==  _FP_INTERNAL_RESERVED_ID))) {
        is_backup_entry = 1;
    }

    /* Entry read & destination clear loop. */
    for (idx = 0; idx < parts_count; idx++) {
        /* Index sanity check. */
        if ((tcam_idx_old[idx] < 0) || (tcam_idx_old[idx] > tcam_idx_max) ||
            (tcam_idx_new[idx] < 0) || (tcam_idx_new[idx] > tcam_idx_max)) {
            return (BCM_E_PARAM);
        }

        /* Invalidate the entry at new index
         * To avoid intermediate state as there are 3 tables in raptor:
         * ipbm, rules, policy */
        rv = _field_raptor_entry_rule_del(unit, tcam_idx_new[idx]);
        BCM_IF_ERROR_RETURN(rv);

        /* Copy the IPBM */
        rv = _field_raptor_entry_ipbm_copy(unit, f_ent + idx, idx, tcam_idx_new[idx]);
        BCM_IF_ERROR_RETURN(rv);

        /* Copy the policies */
        /* Read the entry policies from current index. */
        rv = READ_FP_POLICY_TABLEm(unit, MEM_BLOCK_ANY, tcam_idx_old[idx], 
                                   p_entry + idx);
        BCM_IF_ERROR_RETURN(rv);

    }

    /* Calculate primary entry new slice & offset in the slice. */
    rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc, _FP_DEF_INST,
                                             tcam_idx_new[0],
                                             &new_slice_numb, &new_slice_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Update policy entry if moving across the slices. */
    if ((1 != is_backup_entry) && (f_ent->slice_idx != new_slice_numb)) {
        if ( (f_ent->policer[0].flags & _FP_POLICER_INSTALLED)) {
            rv = _bcm_field_policer_get(unit, f_ent->policer[0].pid, &f_pl);
            BCM_IF_ERROR_RETURN(rv);
        }
        if ( (f_ent->statistic.flags & _FP_ENTRY_STAT_INSTALLED)) {
            rv = _bcm_field_stat_get(unit, f_ent->statistic.sid, &f_st);
            BCM_IF_ERROR_RETURN(rv);
        }

        if (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
            if (NULL != f_st) {
                /* 
                 * Set the index of the counter for entry in new slice
                 * The new index has already been calculated in 
                 * _field_entry_move
                 */
                soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, COUNTER_INDEXf,
                                                 f_st->hw_index);
            }
            if (NULL != f_pl) {
                /* 
                 * Set the index of the meter for entry in new slice
                 * The new index has already been calculated in 
                 * _field_entry_move
                 */
                soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, METER_INDEX_EVENf,
                                                 f_pl->hw_index);
                soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, METER_INDEX_ODDf,
                                                 f_pl->hw_index);
            }
        } else {
            if (NULL != f_st) {
                _bcm_field_fb_counter_adjust_wide_mode(unit, FP_POLICY_TABLEm,
                                                       f_st, f_ent, f_ent + 1,
                                                       new_slice_numb,
                                                       (uint32*)&p_entry[0],
                                                       (uint32*)&p_entry[1]);
            }
            if (NULL != f_pl) {
                _bcm_field_fb_meter_adjust_wide_mode(unit, FP_POLICY_TABLEm,
                                                     f_pl, f_ent, f_ent + 1,
                                                     (uint32*)&p_entry[0],
                                                     (uint32*)&p_entry[1]);
            }
        }
    }

    /* 
     * Write entry to the destination
     * ORDER is important
     */ 
    for (idx = parts_count - 1; idx >= 0; idx--) {

        /* Write these at the new index */
        rv = WRITE_FP_POLICY_TABLEm(unit, MEM_BLOCK_ALL, tcam_idx_new[idx], 
                                    p_entry + idx);
        BCM_IF_ERROR_RETURN(rv);

        /* Copy rules */
        rv = _field_raptor_entry_rule_copy(unit, tcam_idx_old[idx],
                                           tcam_idx_new[idx]);
        BCM_IF_ERROR_RETURN(rv);
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
            /* Invalidate the old entries */
            rv = _field_raptor_entry_rule_del(unit, tcam_idx_old[idx]);
        }
    }
    return BCM_E_NONE;
}

/*
 * Function: _bcm_field_raven_qualify_ttl_get
 *
 * Purpose:
 *     Add a field qualification to a filter entry.
 *
 * Parameters:
 *     unit  - (IN)BCM device number
 *     entry - (IN)Field entry array.
 *     data - (OUT)Data to qualify with 
 *     mask - (OUT)Mask to qualify with
 *
 * Returns:
 *     BCM_E_XXX        Other errors
 */
int
_bcm_field_raven_qualify_ttl_get(int unit, bcm_field_entry_t entry,
                                 uint8 *data, uint8 *mask)
{
    _field_entry_t *entry_arr[_FP_MAX_ENTRY_WIDTH];
    _field_group_t *fg = NULL;
    uint8 result;
    int entry_idx = -1;
    int rv;   

    rv = _bcm_field_entry_get_by_id(unit, entry, entry_arr);
    BCM_IF_ERROR_RETURN(rv);

    fg = entry_arr[0]->group;
    /* Check if ttl is part of group qset. */
    rv = _bcm_field_qset_test(bcmFieldQualifyTtl, &fg->qset, &result);
    BCM_IF_ERROR_RETURN(rv);

    if (TRUE == result) {
        return _bcm_field_entry_qualifier_uint8_get(unit, entry, 
                                                    bcmFieldQualifyTtl, 
                                                    data, mask);
    }

    /* Find the entry for which f4 == 0 and write TTL_ENCODE */
    if (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
        if (fg->sel_codes[0].fpf4 != 1) {
            entry_idx = 0;
        } 
    } else if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        if (fg->sel_codes[0].fpf4 != 1) {
            entry_idx = 0;
        } else if (fg->sel_codes[1].fpf4 != 1) {
            entry_idx = 1;
        }
    } else if (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
        if (fg->sel_codes[0].fpf4 != 1) {
            entry_idx = 0;
        } else if (fg->sel_codes[1].fpf4 != 1) {
            entry_idx = 1;
        } else if (fg->sel_codes[2].fpf4 != 1) {
            entry_idx = 2;
        }
    }

    if (entry_idx == -1) {
        return BCM_E_PARAM;
    } 

    /* bcmFieldQualifyTtl not present, use TTL_ENCODE (if present) */
    switch (entry_arr[entry_idx]->tcam.f4 & 0x3) {
      case 0:
          *data = 0x0;
          *mask = 0xff;
          break;
      case 1:
          *data = 0x1;
          *mask = 0x3;
          break;
      case 3:
          *data = 0xff;
          *mask = 0xff;
          break;
      default:
          return (BCM_E_UNAVAIL);
    }
    return BCM_E_NONE;                    
}

/*
 * Function: _bcm_field_raven_qualify_ttl
 *
 * Purpose:
 *     Add a field qualification to a filter entry.
 *
 * Parameters:
 *     unit  - BCM device number
 *     entry - Field entry array.
 *     data - Data to qualify with 
 *     mask - Mask to qualify with
 *
 * Returns:
 *     BCM_E_XXX        Other errors
 */
int
_bcm_field_raven_qualify_ttl(int unit, bcm_field_entry_t entry,
                           uint8 data, uint8 mask)
{
    _field_entry_t *entry_arr[_FP_MAX_ENTRY_WIDTH];
    _field_group_t *fg = NULL;
    uint8 result;
    int entry_idx = -1;
    int rv;   

    rv = _bcm_field_entry_get_by_id(unit, entry, entry_arr);
    BCM_IF_ERROR_RETURN(rv);

    fg = entry_arr[0]->group;
    /* Check if ttl is part of group qset. */
    rv = _bcm_field_qset_test(bcmFieldQualifyTtl, &fg->qset, &result);
    BCM_IF_ERROR_RETURN(rv);

    if (TRUE == result) {
        return _field_qualify32(unit, entry, bcmFieldQualifyTtl, data, mask);
    }

    /* bcmFieldQualifyTtl not present, use TTL_ENCODE (if present) */
    switch (data & mask) {
      case 0:
          data = 0x0;
          mask &= 0x3;
          break;
      case 1:
          data = 0x1;
          mask &= 0x3;
          break;
      case 255:
          data = 0x3;
          mask &= 0x3;
          break;
      default:
          data = 0x2;
          mask &= 0x3;    
    }

    /* Find the entry for which f4 == 0 and write TTL_ENCODE */
    if (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
        if (fg->sel_codes[0].fpf4 != 1) {
            entry_idx = 0;
        } 
    } else if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        if (fg->sel_codes[0].fpf4 != 1) {
            entry_idx = 0;
        } else if (fg->sel_codes[1].fpf4 != 1) {
            entry_idx = 1;
        }
    } else if (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
        if (fg->sel_codes[0].fpf4 != 1) {
            entry_idx = 0;
        } else if (fg->sel_codes[1].fpf4 != 1) {
            entry_idx = 1;
        } else if (fg->sel_codes[2].fpf4 != 1) {
            entry_idx = 2;
        }
    }

    if (entry_idx == -1) {
        return BCM_E_PARAM;
    } 

    entry_arr[entry_idx]->tcam.f4 &= ~(mask);
    entry_arr[entry_idx]->tcam.f4 |= data;

    entry_arr[entry_idx]->tcam.f4_mask &= ~(mask);
    entry_arr[entry_idx]->tcam.f4_mask |= mask;

    entry_arr[entry_idx]->flags |= _FP_ENTRY_DIRTY;
    return BCM_E_NONE;                    
}
#else
typedef int _raptor_field_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_RAPTOR_SUPPORT) && BCM_FIELD_SUPPORT */
