/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field.c
 * Purpose:     BCM56860 Field Processor installation functions.
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#if (defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_APACHE_SUPPORT)) && \
defined(BCM_FIELD_SUPPORT)

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
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/policer.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/nat.h>

#define ALIGN32(x)      (((x) + 31) & ~31)


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
void _field_td2plus_functions_init(_field_funct_t *functions)
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
    functions->fp_stat_hw_alloc = _bcm_field_td2plus_stat_hw_alloc;
    functions->fp_stat_hw_free = _bcm_field_td2plus_stat_hw_free;
    functions->fp_group_add = NULL;
    functions->fp_qualify_trunk = _bcm_field_qualify_trunk;
    functions->fp_qualify_trunk_get = _bcm_field_qualify_trunk_get;
    functions->fp_qualify_inports = _bcm_field_qualify_InPorts;
    functions->fp_entry_stat_extended_attach =
        _bcm_field_td2plus_entry_stat_extended_attach;
    functions->fp_entry_stat_extended_get =
        _bcm_field_td2plus_entry_stat_extended_get;
    functions->fp_entry_stat_detach = _bcm_field_td2plus_entry_stat_detach;
    functions->fp_class_size_get = NULL;
    functions->fp_qualify_packet_res = _bcm_field_td2_qualify_PacketRes;
    functions->fp_qualify_packet_res_get = _bcm_field_td2_qualify_PacketRes_get;

    return;
}

/*
 * Function:
 *     _field_td2plus_ingress_qualifiers_init
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
_field_td2plus_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    const unsigned
        f2_offset = 10 - 2,
        fixed_offset = 138 - 2,
        fixed_pairing_overlay_offset = fixed_offset,
        fp_global_mask_tcam_ofs = ALIGN32(soc_mem_field_length(unit, FP_TCAMm, KEYf)),
        f3_offset = 2 - 2,
        f1_offset = 51 - 2,
        ipbm_pairing_f0_offset = 101 - 2;

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
    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
        /* SW WAR  for Td2+ A0 to ignore F1_0[3:0] for second part of double wide mode */
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_global_mask_tcam_ofs + f1_offset, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 1,
                               fp_global_mask_tcam_ofs + f1_offset, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_global_mask_tcam_ofs + f1_offset, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_global_mask_tcam_ofs + f1_offset, 10);
    } else {
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
    }
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
    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroups,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMultipath,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    } else {
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
    }
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


    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGports,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGports,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGports,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGports,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGports,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMulticastGroups,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMultipath,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    } else {
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
    }
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

    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    } else {
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 3,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    }
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


    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    } else {
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 4,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    }
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


    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 0, 3);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 3, 1);
    } else {
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 0, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf1, 5,
                 fp_global_mask_tcam_ofs + f1_offset + 3, 1);
    }
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


    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    } else {
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 6,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    }
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


    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    } else {
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 7,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    }
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


    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_global_mask_tcam_ofs + f1_offset, 8);
    } else {
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf1, 8,
                 fp_global_mask_tcam_ofs + f1_offset, 8);
    }
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


    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, _bcmFieldQualifyData2,
                 _bcmFieldSliceSelFpf1, 9,
                 fp_global_mask_tcam_ofs + f1_offset, 32);
    } else {
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData2,
                 _bcmFieldSliceSelFpf1, 9,
                 fp_global_mask_tcam_ofs + f1_offset, 32);
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyGenericAssociatedChannelLabelValid,
                 _bcmFieldSliceSelFpf1, 9,
                 fp_global_mask_tcam_ofs + f1_offset + 32, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRouterAlertLabelValid,
                 _bcmFieldSliceSelFpf1, 9,
                 fp_global_mask_tcam_ofs + f1_offset + 33, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
                 _bcmFieldSliceSelFpf1, 9,
                 fp_global_mask_tcam_ofs + f1_offset + 36 , 14);

    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMplsGports,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstNivGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstNivGports,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMimGports,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstVxlanGports,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstVlanGports,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f1_offset, 21);


    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMulticastGroups,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf1, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMultipath,
                               fp_global_mask_tcam_ofs + f1_offset, 21);
    } else {
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
    }
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf1, 10,
                 fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf1, 10,
                 fp_global_mask_tcam_ofs + f1_offset + 21, 16);



    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_global_mask_tcam_ofs + f1_offset, 15);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGports,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_global_mask_tcam_ofs + f1_offset, 15);
    } else {
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
    }
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


    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 12,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    } else {
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 12,
                 fp_global_mask_tcam_ofs + f1_offset, 12);
    }
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


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf1, 13,
                 fp_global_mask_tcam_ofs + f1_offset + 37, 4);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf1, 13,
                 fp_global_mask_tcam_ofs + f1_offset + 41, 2);

    if (soc_feature(unit, soc_feature_fp_based_oam)) {
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOamType,
                _bcmFieldSliceSelFpf1, 13,
                fp_global_mask_tcam_ofs + f1_offset + 43, 1);
    }
    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
        _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                _bcmFieldSliceSelFpf1, 13,
                _bcmFieldSliceAuxTag1Select,
                _bcmFieldAuxTagVn,
                fp_global_mask_tcam_ofs + f1_offset, 33);
        _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCnTag,
                _bcmFieldSliceSelFpf1, 13,
                _bcmFieldSliceAuxTag1Select,
                _bcmFieldAuxTagCn,
                fp_global_mask_tcam_ofs + f1_offset, 33);
        _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                bcmFieldQualifyFabricQueueTag,
                _bcmFieldSliceSelFpf1, 13,
                _bcmFieldSliceAuxTag1Select,
                _bcmFieldAuxTagFabricQueue,
                fp_global_mask_tcam_ofs + f1_offset,
                33);
        _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                bcmFieldQualifyMplsForwardingLabel,
                _bcmFieldSliceSelFpf1, 13,
                _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                fp_global_mask_tcam_ofs + f1_offset, 33
                );
        _FP_QUAL_PRIMARY_EXT_ADD(unit, stage_fc,
                bcmFieldQualifyMplsForwardingLabelTtl,
                _bcmFieldDevSelDisable, 0,
                _bcmFieldSliceSelFpf1, 13,
                _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                0,
                fp_global_mask_tcam_ofs + f1_offset,      8,
                fp_global_mask_tcam_ofs + f1_offset + 32, 1,
                0,                         0,
                0, 1
                );
        _FP_QUAL_PRIMARY_EXT_ADD(unit, stage_fc,
                bcmFieldQualifyMplsForwardingLabelBos,
                _bcmFieldDevSelDisable, 0,
                _bcmFieldSliceSelFpf1, 13,
                _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                0,
                fp_global_mask_tcam_ofs + f1_offset + 8,  1,
                fp_global_mask_tcam_ofs + f1_offset + 32, 1,
                0,                         0,
                0, 1
                );
        _FP_QUAL_PRIMARY_EXT_ADD(unit, stage_fc,
                bcmFieldQualifyMplsForwardingLabelExp,
                _bcmFieldDevSelDisable, 0,
                _bcmFieldSliceSelFpf1, 13,
                _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                0,
                fp_global_mask_tcam_ofs + f1_offset + 9,  3,
                fp_global_mask_tcam_ofs + f1_offset + 32, 1,
                0,                         0,
                0, 1
                );
    } else {
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
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                bcmFieldQualifyMplsForwardingLabel,
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
    }
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelId,
                               _bcmFieldSliceSelFpf1, 13,                       
                               _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsFwdingLabel,
                               fp_global_mask_tcam_ofs + f1_offset + 12, 21     
                               );                                               
    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
        _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWord,
                _bcmFieldSliceSelFpf1, 13,
                _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagMplsCntlWord,
                fp_global_mask_tcam_ofs + f1_offset, 33
                );
        _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanHeaderBits8_31,
                _bcmFieldSliceSelFpf1, 13,
                _bcmFieldSliceAuxTag1Select,
                _bcmFieldAuxTagVxlanReserved,
                fp_global_mask_tcam_ofs + f1_offset, 24);
        _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanNetworkId,
                _bcmFieldSliceSelFpf1, 13,
                _bcmFieldSliceAuxTag1Select,
                _bcmFieldAuxTagVxlanOrLLTag,
                fp_global_mask_tcam_ofs + f1_offset, 24);
        _FP_QUAL_PRIMARY_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashLower,
                _bcmFieldDevSelDisable, 0,
                _bcmFieldSliceSelFpf1, 13,
                _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagRtag7A,
                0,
                fp_global_mask_tcam_ofs + f1_offset, 16,
                fp_global_mask_tcam_ofs + f1_offset + 32, 1,
                0, 0,
                0, 1);
        _FP_QUAL_PRIMARY_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashLower,
                _bcmFieldDevSelDisable, 0,
                _bcmFieldSliceSelFpf1, 13,
                _bcmFieldSliceAuxTag1Select, _bcmFieldAuxTagRtag7B,
                0,
                fp_global_mask_tcam_ofs + f1_offset, 16,
                fp_global_mask_tcam_ofs + f1_offset + 32, 1,
                0, 0,
                0, 1);

    } else {
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
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanNetworkId,
                _bcmFieldSliceSelFpf1, 13,
                _bcmFieldSliceAuxTag1Select,
                _bcmFieldAuxTagVxlanOrLLTag,
                fp_global_mask_tcam_ofs + f1_offset, 24);
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
    }
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanHeaderBits56_63,
                               _bcmFieldSliceSelFpf1, 13,
                               _bcmFieldSliceAuxTag1Select,
                               _bcmFieldAuxTagVxlanReserved,
                               fp_global_mask_tcam_ofs + f1_offset + 24, 8);
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
                                          
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelAction,      
                 _bcmFieldSliceSelFpf1, 13,                                     
                 fp_global_mask_tcam_ofs + f1_offset + 33, 3                    
                 );    
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
                 _bcmFieldSliceSelFpf1, 14,
                 fp_global_mask_tcam_ofs + f1_offset + 48, 1
                 );

    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
        _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                _bcmFieldSliceSelFpf1, 14,
                fp_global_mask_tcam_ofs + f1_offset, 48
                );
    } else {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                _bcmFieldSliceSelFpf1, 14,
                fp_global_mask_tcam_ofs + f1_offset, 48
                );
    }

    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf1, 15,
                 fp_global_mask_tcam_ofs + f1_offset, 8);
    } else {
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf1, 15,
                 fp_global_mask_tcam_ofs + f1_offset, 8);
    }
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


    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyFcoeVersionIsZero,
                 _bcmFieldSliceSelFpf1, 16,
                 fp_global_mask_tcam_ofs + f1_offset, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyFcoeSOF,
                 _bcmFieldSliceSelFpf1, 16,
                 fp_global_mask_tcam_ofs + f1_offset + 1, 8);
    } else {
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeVersionIsZero,
                 _bcmFieldSliceSelFpf1, 16,
                 fp_global_mask_tcam_ofs + f1_offset, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeSOF,
                 _bcmFieldSliceSelFpf1, 16,
                 fp_global_mask_tcam_ofs + f1_offset + 1, 8);
    }
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

    if (!soc_feature(unit, soc_feature_fp_based_oam)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                _bcmFieldSliceSelFpf2, 3, f2_offset, 128);
    } else {
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable,
                 _bcmFieldOamOverlayDisable,
                 f2_offset, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyEthernetOamHeaderBits0_31,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable,
                 _bcmFieldOamOverlayEnable,
                 f2_offset + 96, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyEthernetOamHeaderBits32_63,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable,
                 _bcmFieldOamOverlayEnable,
                 f2_offset + 64, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyEthernetOamDstClassL2,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable,
                 _bcmFieldOamOverlayEnable,
                 f2_offset + 57, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyEthernetOamTxPktUPMEP,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable,
                 _bcmFieldOamOverlayEnable,
                 f2_offset + 56, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyEthernetOamInterfaceClassPort,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable,
                 _bcmFieldOamOverlayEnable,
                 f2_offset + 32, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyEthernetOamClassVlanTranslateKeyFirst,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable,
                 _bcmFieldOamOverlayEnable,
                 f2_offset + 24, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyEthernetOamClassVlanTranslateKeySecond,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable,
                 _bcmFieldOamOverlayEnable,
                 f2_offset + 16, 8);
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyEthernetOamInterfaceClassNiv,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable, _bcmFieldOamOverlayEnable,
                 _bcmFieldSliceOamInterfaceClassSelect,
                 _bcmFieldOamInterfaceClassSelectNiv,
                 f2_offset + 8, 8);
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyEthernetOamInterfaceClassMim,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable, _bcmFieldOamOverlayEnable,
                 _bcmFieldSliceOamInterfaceClassSelect,
                 _bcmFieldOamInterfaceClassSelectMim,
                 f2_offset + 8, 8);
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyEthernetOamInterfaceClassVxlan,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable, _bcmFieldOamOverlayEnable,
                 _bcmFieldSliceOamInterfaceClassSelect,
                 _bcmFieldOamInterfaceClassSelectVxlan,
                 f2_offset + 8, 8);
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyEthernetOamInterfaceClassVlan,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable, _bcmFieldOamOverlayEnable,
                 _bcmFieldSliceOamInterfaceClassSelect,
                 _bcmFieldOamInterfaceClassSelectVlan,
                 f2_offset + 8, 8);
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyEthernetOamInterfaceClassMpls,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable, _bcmFieldOamOverlayEnable,
                 _bcmFieldSliceOamInterfaceClassSelect,
                 _bcmFieldOamInterfaceClassSelectMpls,
                 f2_offset + 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyEthernetOamClassVpn,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable,
                 _bcmFieldOamOverlayEnable,
                 f2_offset + 0, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyMplsOamHeaderBits0_31,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable,
                 _bcmFieldOamOverlayEnable,
                 f2_offset + 96, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyMplsOamHeaderBits32_63,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable,
                 _bcmFieldOamOverlayEnable,
                 f2_offset + 64, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyMplsOamUpperLabelDataDrop,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable,
                 _bcmFieldOamOverlayEnable,
                 f2_offset + 53, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyMplsOamGALLabelOnly,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable,
                 _bcmFieldOamOverlayEnable,
                 f2_offset + 52, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyMplsOamGALExposed,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable,
                 _bcmFieldOamOverlayEnable,
                 f2_offset + 47, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyMplsOamACH,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable,
                 _bcmFieldOamOverlayEnable,
                 f2_offset + 15, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyMplsOamControlPktType,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable,
                 _bcmFieldOamOverlayEnable,
                 f2_offset + 12, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyMplsOamClassMplsSwitchLabel,
                 _bcmFieldSliceSelFpf2, 3,
                 _bcmFieldSliceOamOverLayEnable,
                 _bcmFieldOamOverlayEnable,
                 f2_offset + 0, 12);
    }

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
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 50, 14);
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
    if (soc_feature(unit, soc_feature_td2p_fp_sw_war)) {
        /* SW WAR for Td2+ A0 to ignore F3 for second part of double wide mode */
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 1,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 1,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVrf,
                               fp_global_mask_tcam_ofs + f3_offset + 20, 13);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVpn,
                               fp_global_mask_tcam_ofs + f3_offset + 20, 13);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan,
                               fp_global_mask_tcam_ofs + f3_offset + 20, 13);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcMplsGports,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcNivGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcNivGports,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGports,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGports,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGports,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 33, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 0,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 1,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 2,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 3,
                               fp_global_mask_tcam_ofs + f3_offset, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVrf,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 13);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVpn,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 13);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVlan,
                               fp_global_mask_tcam_ofs + f3_offset + 10, 13);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 1,
                 fp_global_mask_tcam_ofs + f3_offset + 23, 12);

    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 0,
                               fp_global_mask_tcam_ofs + f3_offset + 35, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 1,
                               fp_global_mask_tcam_ofs + f3_offset + 35, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 2,
                               fp_global_mask_tcam_ofs + f3_offset + 35, 10);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 3,
                               fp_global_mask_tcam_ofs + f3_offset + 35, 10);
#if 0
/* TOS_FN_LOW */
#endif


    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMplsGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstNivGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstNivGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMimGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstVxlanGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstVlanGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMulticastGroups,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMultipath,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcMplsGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcNivGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifySrcNivGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 15);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 15);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunkMemberGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 28, 8);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGport,
                               _bcmFieldSliceSelFpf3, 2,
                                _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGports,
                               _bcmFieldSliceSelFpf3, 2,
                                _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 28, 8);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 2,
                 fp_global_mask_tcam_ofs + f3_offset + 37, 3);

    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset, 16);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset, 12);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 12, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 13, 3);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 2);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 18, 2);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 20, 2);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 22, 3);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 25, 6);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 31, 2);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 3,
                 fp_global_mask_tcam_ofs + f3_offset + 33, 16);

    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset, 16);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset, 12);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 12, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 13, 3);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 12);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 28, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 29, 3);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 32, 4);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 36, 2);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyDstIpLocal,
                 _bcmFieldSliceSelFpf3, 4,
                 fp_global_mask_tcam_ofs + f3_offset + 38, 1);


    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset, 16);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset, 12);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset + 12, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset + 13, 3);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset + 32, 3);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset + 35, 3);


    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 0, 3);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 3, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 4, 2);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 6, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 7, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 8, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 9, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 10, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 10, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 11, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 12, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 13, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 14, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 15, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyTrillEgressRbridgeHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 17, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyTrillIngressRbridgeHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 17, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 17, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 17, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 18, 1);

    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 20, 6);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 26, 16);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 26, 12);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 38, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 6,
                 fp_global_mask_tcam_ofs + f3_offset + 39, 3);

    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf3, 7,
                 fp_global_mask_tcam_ofs + f3_offset, 24);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, _bcmFieldQualifyRangeCheckBits24_31,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 3,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 8);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 0,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 12);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 1,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 12);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 2,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 12);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 4,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 12);

    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_global_mask_tcam_ofs + f3_offset, 20);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_global_mask_tcam_ofs + f3_offset + 20, 2);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_global_mask_tcam_ofs + f3_offset + 22, 2);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_global_mask_tcam_ofs + f3_offset + 24, 2);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_global_mask_tcam_ofs + f3_offset + 26, 2);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 8,
                 fp_global_mask_tcam_ofs + f3_offset + 28, 2);

    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, _bcmFieldQualifyData3,
                 _bcmFieldSliceSelFpf3, 9,
                 fp_global_mask_tcam_ofs + f3_offset, 32);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyGenericAssociatedChannelLabelValid,
                 _bcmFieldSliceSelFpf3, 9,
                 fp_global_mask_tcam_ofs + f3_offset + 32, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyRouterAlertLabelValid,
                 _bcmFieldSliceSelFpf3, 9,
                 fp_global_mask_tcam_ofs + f3_offset + 33, 1);

    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGports,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGports,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGports,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGports,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGports,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroups,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf3, 10,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMultipath,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf3, 10,
                 fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf3, 10,
                 fp_global_mask_tcam_ofs + f3_offset + 21, 16);

    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_global_mask_tcam_ofs + f3_offset, 16);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_global_mask_tcam_ofs + f3_offset, 16);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_global_mask_tcam_ofs + f3_offset, 15);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGports,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_global_mask_tcam_ofs + f3_offset, 15);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf3, 11,
                 fp_global_mask_tcam_ofs + f3_offset + 7, 8);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGports,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcNivGport,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcNivGports,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGports,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGport,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGports,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGports,
                               _bcmFieldSliceSelFpf3, 11,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 16);

    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyColor,
                               _bcmFieldSliceSelFpf3, 12,
                               fp_global_mask_tcam_ofs + f3_offset + 41, 2);
    if (soc_feature(unit, soc_feature_fp_based_oam)) {
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOamType,
                               _bcmFieldSliceSelFpf3, 12,
                               fp_global_mask_tcam_ofs + f3_offset + 43, 1);
    }
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                               _bcmFieldSliceSelFpf3, 12,
                               fp_global_mask_tcam_ofs + f3_offset + 37, 4);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagVn,
                               fp_global_mask_tcam_ofs + f3_offset, 33);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCnTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagCn,
                               fp_global_mask_tcam_ofs + f3_offset, 33);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyFabricQueueTag,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagFabricQueue,
                               fp_global_mask_tcam_ofs + f3_offset, 33);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabel,
                               _bcmFieldSliceSelFpf3, 12,                           
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                               fp_global_mask_tcam_ofs + f3_offset, 33               
                               );                                                   
    _FP_QUAL_PRIMARY_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelTtl,         
                     _bcmFieldDevSelDisable, 0,                                     
                     _bcmFieldSliceSelFpf3, 12,                                     
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,                                                             
                     fp_global_mask_tcam_ofs + f3_offset,      8,                    
                     fp_global_mask_tcam_ofs + f3_offset + 32, 1,                    
                     0,                         0,                                  
                     0, 1                                                              
                     );
    _FP_QUAL_PRIMARY_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelBos,         
                     _bcmFieldDevSelDisable, 0,                                     
                     _bcmFieldSliceSelFpf3, 12,                                     
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,                                                             
                     fp_global_mask_tcam_ofs + f3_offset + 8,  1,                    
                     fp_global_mask_tcam_ofs + f3_offset + 32, 1,                    
                     0,                         0,                                  
                     0, 1                                                              
                     );                                                             
    _FP_QUAL_PRIMARY_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelExp,         
                     _bcmFieldDevSelDisable, 0,                                     
                     _bcmFieldSliceSelFpf3, 12,                                     
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                     0,                                                             
                     fp_global_mask_tcam_ofs + f3_offset + 9,  3,               
                     fp_global_mask_tcam_ofs + f3_offset + 32, 1,               
                     0,                         0,                              
                     0, 1                                                          
                     );                                                         
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelId,
                               _bcmFieldSliceSelFpf3, 12,                       
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                               fp_global_mask_tcam_ofs + f3_offset + 12, 21     
                               );                                               
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWord,  
                               _bcmFieldSliceSelFpf3, 12,                       
                               _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsCntlWord,
                               fp_global_mask_tcam_ofs + f3_offset, 33          
                               );                                               
                                                                                
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashUpper,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagRtag7A,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 17);

    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashUpper,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagRtag7B,
                               fp_global_mask_tcam_ofs + f3_offset + 16, 17);

    _FP_QUAL_PRIMARY_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagRtag7A,
                     0,
                     fp_global_mask_tcam_ofs + f3_offset, 16,
                     fp_global_mask_tcam_ofs + f3_offset + 32, 1,
                     0, 0,
                     0, 1);

    _FP_QUAL_PRIMARY_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashLower,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, 12,
                     _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagRtag7B,
                     0,
                     fp_global_mask_tcam_ofs + f3_offset, 16,
                     fp_global_mask_tcam_ofs + f3_offset + 32, 1,
                     0, 0,
                     0, 1);

    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanNetworkId,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagVxlanOrLLTag,
                               fp_global_mask_tcam_ofs + f3_offset, 24);

    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanFlags,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagVxlanOrLLTag,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 8);

    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanHeaderBits8_31,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagVxlanReserved,
                               fp_global_mask_tcam_ofs + f3_offset, 24);

    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanHeaderBits56_63,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagVxlanReserved,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 8);

    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelAction,      
                 _bcmFieldSliceSelFpf3, 12,                                     
                 fp_global_mask_tcam_ofs + f3_offset + 33, 3                    
                 );                                                             
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                                       _bcmFieldSliceSelFpf3, 13,
                                       _bcmFieldSliceTosClassSelect, 0,
                                       fp_global_mask_tcam_ofs + f3_offset, 8);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                                       _bcmFieldSliceSelFpf3, 13,
                                       _bcmFieldSliceTosClassSelect, 0,
                                       fp_global_mask_tcam_ofs + f3_offset, 8);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                                       _bcmFieldSliceSelFpf3, 13,
                                       _bcmFieldSliceTosClassSelect, 1,
                                       fp_global_mask_tcam_ofs + f3_offset, 8);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeader2Type,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_global_mask_tcam_ofs + f3_offset + 8, 8);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 8);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_global_mask_tcam_ofs + f3_offset + 24, 8);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyDstIpLocal,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_global_mask_tcam_ofs + f3_offset + 32, 1
                 );
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyIpAddrsNormalized,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_global_mask_tcam_ofs + f3_offset + 33, 1
                 );
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_global_mask_tcam_ofs + f3_offset + 34, 1
                 );
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
                 _bcmFieldSliceSelFpf3, 13,
                 fp_global_mask_tcam_ofs + f3_offset + 35, 14
                 );
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
                 _bcmFieldSliceSelFpf3, 14,
                 fp_global_mask_tcam_ofs + f3_offset + 48, 1
                 );
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf3, 14, 
                 fp_global_mask_tcam_ofs + f3_offset, 48
                 );


    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroups,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMultipath,
                               fp_global_mask_tcam_ofs + f3_offset, 21);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcNivGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcNivGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityNivGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVxlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityVlanGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 15);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 15);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 28, 8);
    _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTrunkMemberSourceModuleId,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 28, 8);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 15,
                 fp_global_mask_tcam_ofs + f3_offset + 37, 12);



    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyFcoeVersionIsZero,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyFcoeSOF,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 1, 8);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyFibreChanRCtl,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 15, 8);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVersion,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 23, 2);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTHopCount,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 25, 5);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVsanId,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 30, 12);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVsanPri,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 42, 3);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcFpmaCheck,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 45, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcBindCheck,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 46, 1);
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyFibreChanZoneCheck,
                 _bcmFieldSliceSelFpf3, 16,
                 fp_global_mask_tcam_ofs + f3_offset + 47, 2);
    } else {
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
                 fp_global_mask_tcam_ofs + f3_offset + 7, 9);
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

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                               _bcmFieldSliceSelFpf3, 12,
                               fp_global_mask_tcam_ofs + f3_offset + 41, 2);
    if (soc_feature(unit, soc_feature_fp_based_oam)) {
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOamType,
                               _bcmFieldSliceSelFpf3, 12,
                               fp_global_mask_tcam_ofs + f3_offset + 43, 1);
    }
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
                 fp_global_mask_tcam_ofs + f3_offset + 35, 14
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
    }

    /* FIXED single wide */
    if ((soc_property_get(unit, spn_RIOT_ENABLE, 0)) ||
        (soc_feature(unit, soc_feature_l2_entry_used_as_my_station) &&
         soc_property_get(unit,spn_L2_ENTRY_USED_AS_MY_STATION, 0))) {
        /* MY_STATION TCAM_2 hit maps to MY_STATION_HIT in FP */
        _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyMyStation2Hit,
                _bcmFieldSliceSelDisable, 0, fixed_offset, 1);
    }
    /* MY_STATION_TCAM hit maps to MY_STATION_1_HIT in FP */
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
            _bcmFieldSliceSelDisable, 0, fixed_offset + 23, 1);
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
    _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyRecoverableDrop,
            _bcmFieldSliceSelDisable, 0, fixed_offset + 22, 1);

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
                     _bcmFieldDevSelDisable, 0,
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
 *     _field_td2plus_lookup_qualifiers_init
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
_field_td2plus_lookup_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
                     f1_offset + 36 + 7, 8,
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
 *     _field_td2plus_egress_qualifiers_init
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
_field_td2plus_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
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
    if (!soc_feature(unit, soc_feature_fp_based_oam)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 42, 48);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 90, 48);
    } else {
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                 _bcmFieldSliceSelEgrOamOverlayKey4, 0,
                 42, 48);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                 _bcmFieldSliceSelEgrOamOverlayKey4, 0,
                 90, 48);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyOamHeaderBits32_63,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                 _bcmFieldSliceSelEgrOamOverlayKey4, 1,
                 42, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyOamHeaderBits0_31,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                 _bcmFieldSliceSelEgrOamOverlayKey4, 1,
                 74, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyOamEgressClassVxlt,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                 _bcmFieldSliceSelEgrOamOverlayKey4, 1,
                 106, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                 bcmFieldQualifyOamEgressClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                 _bcmFieldSliceSelEgrOamOverlayKey4, 1,
                 118, 12);
    }
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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               185, 8);
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
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 3,
                               0,
                               193, 7, /* DST_PORT */
                               185, 8, /* DST_MODID */
                               0, 0,
                               0
                               );
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

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrDestPortF1, 1,
                               0,
                               185, 7, /* DST_PORT */
                               177, 8, /* DST_MODID */
                               0, 0,
                               0
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrDestPortF1, 0,
                               177, 8
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrDestPortF1, 0,
                               185, 7
                               );

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
                               _bcmFieldSliceSelEgrClassF1, 3,
                               197, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 3,
                               197, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 3,
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

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrDestPortF1, 1,
                               0,
                               176, 7, /* DST_PORT */
                               168, 8, /* DST_MODID */
                               0, 0,
                               0
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrDestPortF1, 0,
                               177, 8
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrDestPortF1, 0,
                               185, 7
                               );
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
                               _bcmFieldSliceSelEgrClassF1, 3,
                               197, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 3,
                               197, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrClassF1, 3,
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

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 5,
                               0,
                               195, 7, /* DST_PORT */
                               187, 8, /* DST_MODID */
                               0, 0,
                               0
                               );

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
 *     _field_td2plus_qualifiers_init
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
int
_field_td2plus_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
            return (_field_td2plus_lookup_qualifiers_init(unit, stage_fc));
        case _BCM_FIELD_STAGE_INGRESS:
            return (_field_td2plus_ingress_qualifiers_init(unit, stage_fc));
        case _BCM_FIELD_STAGE_EGRESS:
            return (_field_td2plus_egress_qualifiers_init(unit, stage_fc));
        default:
            ;
    }

    sal_free(stage_fc->f_qual_arr);
    return (BCM_E_INTERNAL);
}

/*
 * Function:
 *     _field_oam_stat_action_node_get 
 * Description:
 *      Get Oam Stat action node in Field Processor entry list.
 * Parameters:
 *      unit                 - (IN) BCM device number.
 *      f_ent                - (IN) Field entry pointer.
 *      oam_stat_action      - (IN) Oam Stat Action Structure.
 *      oam_stat_action_node - (IN) Oam Stat Action Node.
 * Returns:
 *      BCM_E_XXX
 */
int _field_oam_stat_action_node_get (int unit, _field_entry_t *f_ent,
                            bcm_field_oam_stat_action_t *oam_stat_action,
                            _field_oam_stat_action_t **oam_stat_get_node)
{
    int rv = BCM_E_NOT_FOUND; /* Return status */
    _field_oam_stat_action_t *oam_stat_node; /* Oam stat Node */
    bcm_field_oam_stat_action_t *oam_action; /* Oam stat action */

    /* Input Check */
    if ((f_ent == NULL) || (oam_stat_action == NULL)
                        || (oam_stat_get_node == NULL)) {
        return BCM_E_PARAM;
    }

    oam_stat_node = f_ent->field_oam_stat_action;

    /* If node is found update the get_node and return */
    while (oam_stat_node != NULL) {
        oam_action = oam_stat_node->oam_stat_action;
        if (oam_action != NULL) {
            if (oam_action->stat_object == oam_stat_action->stat_object) {
                *oam_stat_get_node = oam_stat_node;
                return BCM_E_NONE;
            }
        }
        oam_stat_node = oam_stat_node->next;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_field_oam_stat_action_search
 * Description:
 *      Search Oam Stat action in Field Processor entry list.
 * Parameters:
 *      unit            - (IN) BCM device number.
 *      f_ent           - (IN) Field entry pointer.
 *      oam_stat_action - (IN) Oam Stat Action Structure.
 * Returns:
 *      BCM_E_XXX
 */
int _field_oam_stat_action_search (int unit, _field_entry_t *f_ent,
                            bcm_field_oam_stat_action_t *oam_stat_action)
{
    int rv = BCM_E_NOT_FOUND; /* Return status */
    _field_oam_stat_action_t *oam_stat_node; /* Oam stat Node */
    bcm_field_oam_stat_action_t *oam_action; /* Oam stat action */

    /* Input Check */
    if ((f_ent == NULL) || (oam_stat_action == NULL)) {
        return BCM_E_PARAM;
    }

    oam_stat_node = f_ent->field_oam_stat_action;

    /* Return stat nodes that are valid, if valid flag is not set
     * It means it is marked for removal */
    while (oam_stat_node != NULL) {
        oam_action = oam_stat_node->oam_stat_action;
        if (oam_action != NULL) {
            if ((oam_action->stat_object == oam_stat_action->stat_object)
                    && (oam_stat_node->flags &  _FP_OAM_STAT_ACTION_VALID)) {
                return BCM_E_EXISTS;
            }
        }
        oam_stat_node = oam_stat_node->next;
    }

    return rv;
}

/*
 * Function:
 *      _field_oam_lm_stat_pool_id_get 
 * Description:
 *      Return Flex Stat Pool Id from Stat Object and Stat Mode Id.
 * Parameters:
 *      unit              - (IN) BCM device number.
 *      stat_object       - (IN) Flex Stat Object.
 *      stat_mode_id      - (IN) Flex Stat Mode Id.
 *      flex_stat_pool_id - (OUT) Flex Stat Pool Id.
 * Returns:
 *      BCM_E_XXX
 */
int _field_oam_lm_stat_pool_id_get (int unit, bcm_stat_object_t stat_object,
        uint32 stat_mode_id, int8 *flex_stat_pool_id)
{
    int rv = BCM_E_NONE;
    uint32 pool_id;
    int8 flex_pool_id;

    if (flex_stat_pool_id == NULL) {
        return BCM_E_PARAM;
    }
    rv = _bcm_esw_stat_object_pool_num_get(unit, stat_object, &pool_id);
    if (BCM_SUCCESS(rv)) {
        flex_pool_id = pool_id;
        sal_memcpy (flex_stat_pool_id, &flex_pool_id, sizeof (int8));
    }
    return rv;
}

/*
 * Function:
 *      _field_oam_stat_action_pool_instance_reserve 
 * Description:
 *      Reserve Oam Stat Pool Instance.
 * Parameters:
 *      unit            - (IN) BCM device number.
 *      pool_instance   - (IN) Oam Stat Action Pool Instance.
 * Returns:
 *      BCM_E_XXX
 */
int _field_oam_stat_action_pool_instance_reserve (int unit,
                                      _field_entry_t *f_ent, int8 *instance,
                                      _field_oam_stat_pool_config_t config)
{
    int idx = 0;
    uint8 *local_pool_instance = NULL;

    if ((config < 0) || (config > _bcmFieldOamStatPoolInstanceCount)) {
        return BCM_E_PARAM;
    }

    if ((instance == NULL) && (config != _bcmFieldOamStatPoolInstanceFreeAll)){
        return BCM_E_PARAM;
    }

    if (f_ent == NULL) {
        return BCM_E_PARAM;
    }

    _FP_XGS3_ALLOC(local_pool_instance, sizeof (uint8),
                                        "local_pool_instance");
    if (local_pool_instance == NULL) {
        return BCM_E_PARAM;
    }

    sal_memcpy (local_pool_instance,
             &(f_ent->oam_pool_instance), sizeof(uint8));

    switch (config) {
        case _bcmFieldOamStatPoolInstanceReserve:
            /* Reserve from one of the 3 instances */
            for (idx = 0; idx <= 2; idx++) {
                if (SHR_BITGET(local_pool_instance, idx) == 0) {
                    *instance = idx;
                    SHR_BITSET(local_pool_instance, idx);
                    f_ent->oam_pool_instance = *local_pool_instance;
                    sal_free (local_pool_instance);
                    return BCM_E_NONE;
                }
            }
            break;
        case _bcmFieldOamStatPoolInstanceFree:
            /* Free the pool instance */
            if ((*instance < 0) || (*instance > 2)) {
                sal_free (local_pool_instance);
                return BCM_E_PARAM;
            }

            SHR_BITCLR(local_pool_instance, (*instance));
            f_ent->oam_pool_instance = *local_pool_instance;
            sal_free (local_pool_instance);
            return BCM_E_NONE;
            break;

        case _bcmFieldOamStatPoolInstanceFreeAll:
            f_ent->oam_pool_instance = 0;
            sal_free (local_pool_instance);
            return BCM_E_NONE;
            break;

        case _bcmFieldOamStatPoolInstanceGet:
            /* Return Pool Instance Bitmap */
            *instance = f_ent->oam_pool_instance;
            sal_free (local_pool_instance);
            return BCM_E_NONE;
            break;

        case _bcmFieldOamStatPoolInstanceCount:
            sal_free (local_pool_instance);
            return BCM_E_PARAM;

            /* Will never hit default case */
    }

    sal_free (local_pool_instance);
    return BCM_E_FULL;
}

/*
 * Function:
 *      _bcm_field_oam_stat_action_params_check
 * Description:
 *      Check Oam Stat action Params 
 * Parameters:
 *      unit            - (IN) BCM device number.
 *      oam_stat_action - (IN) Oam Stat Action Structure.
 * Returns:
 *      BCM_E_XXX
 */

int _bcm_field_oam_stat_action_params_check (int unit,
        bcm_field_oam_stat_action_t *oam_stat_action) 
{
    bcm_stat_flex_direction_t direction;

    if (oam_stat_action == NULL) {
        return BCM_E_PARAM;
    }

    /* Check Stat Object */
    if ((oam_stat_action->stat_object < 0)
            || (oam_stat_action->stat_object >= bcmStatObjectMaxValue)) {
        return BCM_E_PARAM;
    }
    /* Validate object */
    BCM_IF_ERROR_RETURN(
                   _bcm_esw_stat_validate_object(unit,
                                                 oam_stat_action->stat_object,
                                                 &direction));
    /* Check Stat Mode */
    if (direction == bcmStatFlexDirectionIngress) {
        if ((oam_stat_action->stat_mode_id & BCM_STAT_GROUP_MODE_ID_OAM_MASK)
            != BCM_STAT_GROUP_MODE_ID_OAM_SUFFIX) {
            return BCM_E_PARAM;
        }
    }

    /* Check Stat Action */
    if  ((oam_stat_action->action < 0)
            || (oam_stat_action->action >= bcmFieldStatActionCount)
            || (oam_stat_action->action == bcmFieldStatActionIncrement)) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_oam_stat_action_add
 * Description:
 *      Add Oam Stat action to Field Processor entry.
 * Parameters:
 *      unit            - (IN) BCM device number.
 *      entry           - (IN) Field entry id.
 *      oam_stat_action - (IN) Oam Stat Action Structure.
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_field_oam_stat_action_add (int unit, bcm_field_entry_t entry,
                                 bcm_field_oam_stat_action_t *oam_stat_action)
{
    int rv = BCM_E_NONE;                            /* Return Value           */
    _field_oam_stat_action_t *oam_stat_node = NULL; /* Oam Stat Action Node   */
    int8 oam_lm_stat_pool_id = -1;                  /* Oam Lm Stat Pool Id    */
    int8 pool_instance = -1;                        /* Oam Stat Pool Instance */
    _field_entry_t *f_ent;                          /* Field entry pointer    */

    if (oam_stat_action == NULL) {
        return BCM_E_PARAM;
    }

    /* Input Params Check */
    BCM_IF_ERROR_RETURN((_bcm_field_oam_stat_action_params_check(unit,
                                                      oam_stat_action)));
    /* Get entry info. */
    BCM_IF_ERROR_RETURN
        (_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent));

    /* Check for conflicting actions */
    if (f_ent->flags & _FP_ENTRY_OAM_STAT_NOT_ALLOWED) {
#ifdef BROADCOM_DEBUG
        LOG_ERROR(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "FP(unit %d) Error: Oam Stat Action conflicts with"
                            " Nat Actions in  entry=%d\n"), unit, entry ));
#endif
        return (BCM_E_CONFIG);
    }

    /* Check for duplicate stat action already existing */
    rv = _field_oam_stat_action_search (unit, f_ent, oam_stat_action);
    if (rv == BCM_E_EXISTS) {
        return rv ;
    }

    /* Get Flex Stat PoolId from the Stat Object and Stat Mode Id */
    BCM_IF_ERROR_RETURN (_field_oam_lm_stat_pool_id_get (unit,
                oam_stat_action->stat_object, oam_stat_action->stat_mode_id,
                &oam_lm_stat_pool_id));

    /* Reserve the free pool instance from one of 3 instances */
    rv = _field_oam_stat_action_pool_instance_reserve (unit, f_ent,
            &pool_instance, _bcmFieldOamStatPoolInstanceReserve);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Allocate memory and copy the input params */
    _FP_XGS3_ALLOC(oam_stat_node, sizeof (_field_oam_stat_action_t),
                                                "field oam stat action node");
    if (oam_stat_node == NULL) {
        LOG_ERROR(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "FP(unit %d) Error: allocation failure for "
                            "oam_stat_action_node\n"), unit));
        rv = _field_oam_stat_action_pool_instance_reserve (unit, f_ent,
                &pool_instance, _bcmFieldOamStatPoolInstanceFree);
        return (BCM_E_MEMORY);
    }

    /* Allocate memory and copy the input params */
    _FP_XGS3_ALLOC(oam_stat_node->oam_stat_action,
                 sizeof (bcm_field_oam_stat_action_t),
            "field oam stat action params");
    if (oam_stat_node->oam_stat_action == NULL) {
        LOG_ERROR(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "FP(unit %d) Error: allocation failure for "
                            "oam_stat_action_node\n"), unit));
        rv = _field_oam_stat_action_pool_instance_reserve (unit, f_ent,
                &pool_instance, _bcmFieldOamStatPoolInstanceFree);
        sal_free (oam_stat_node);
        return (BCM_E_MEMORY);
    }


    /* Copy the input parameters and insert to linked list */
    sal_memcpy (oam_stat_node->oam_stat_action, oam_stat_action,
                                sizeof (bcm_field_oam_stat_action_t));
    oam_stat_node->flex_pool_id = oam_lm_stat_pool_id;
    oam_stat_node->pool_instance = pool_instance;
    oam_stat_node->next = NULL;
    oam_stat_node->flags |= _FP_OAM_STAT_ACTION_VALID;
    oam_stat_node->flags |= _FP_OAM_STAT_ACTION_DIRTY;

    /* Add node to link list */
    _FP_LINKLIST_INSERT(&(f_ent->field_oam_stat_action), oam_stat_node, &rv);
    if (BCM_FAILURE(rv)) {
        sal_free (oam_stat_node->oam_stat_action);
        sal_free (oam_stat_node);
        rv = _field_oam_stat_action_pool_instance_reserve (unit, f_ent,
                &pool_instance, _bcmFieldOamStatPoolInstanceFree);
        return rv;
    }

    /* Update entry flags */
    if (0 == (f_ent->flags & _FP_ENTRY_DIRTY)) {
        f_ent->flags |= _FP_ENTRY_POLICY_TABLE_ONLY_DIRTY;
    }

    /* If oam action is present update entry flag 
     * to say oam stat is valid. Used to check conflicts
     * with Nat Actions */
    f_ent->flags  |= _FP_ENTRY_DIRTY;
    rv = _field_oam_stat_action_pool_instance_reserve (unit, f_ent,
            &pool_instance, _bcmFieldOamStatPoolInstanceGet);
    if (BCM_FAILURE (rv)) {
        sal_free (oam_stat_node->oam_stat_action);
        sal_free (oam_stat_node);
        rv = _field_oam_stat_action_pool_instance_reserve (unit, f_ent,
                &pool_instance, _bcmFieldOamStatPoolInstanceFree);
        return rv;
    }

    if (pool_instance != 0) {
        f_ent->flags  |= _FP_ENTRY_OAM_STAT_VALID;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_oam_stat_action_delete
 * Description:
 *      Delete Oam Stat action from Field Processor entry.
 * Parameters:
 *      unit            - (IN) BCM device number.
 *      entry           - (IN) Field entry id.
 *      oam_stat_action - (IN) Oam Stat Action Structure.
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_field_oam_stat_action_delete (int unit, bcm_field_entry_t entry,
                    bcm_field_oam_stat_action_t *oam_stat_action)
{
    _field_entry_t *f_ent;                          /* Field entry pointer.  */
    _field_oam_stat_action_t *oam_stat_node = NULL; /* Oam Stat Action Node. */
    int rv = BCM_E_NONE;                            /* Return Value.         */
    int8 pool_instance = -1;                        /* Pool Instance         */

    /* Check input parameters */
    if (oam_stat_action == NULL) {
        return BCM_E_PARAM;
    }

    /* Get entry info. */
    BCM_IF_ERROR_RETURN
        (_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent));

    /* Check for stat action already existing */
    rv = _field_oam_stat_action_search (unit, f_ent, oam_stat_action);
    if (rv == BCM_E_NOT_FOUND) {
        return rv;
    }

    /* Get stat action node and update flag as dirty */
    rv = _field_oam_stat_action_node_get (unit, f_ent,
            oam_stat_action, &oam_stat_node);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Free the reserved flex stat pool index */
    pool_instance = oam_stat_node->pool_instance;
    rv = _field_oam_stat_action_pool_instance_reserve (unit, f_ent,
            &pool_instance, _bcmFieldOamStatPoolInstanceFree);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Update the flags */
    oam_stat_node->flags &= ~(_FP_OAM_STAT_ACTION_VALID);
    oam_stat_node->flags |= _FP_OAM_STAT_ACTION_DIRTY;

    if (0 == (f_ent->flags & _FP_ENTRY_DIRTY)) {
        f_ent->flags |= _FP_ENTRY_POLICY_TABLE_ONLY_DIRTY;
    }

    f_ent->flags  |= _FP_ENTRY_DIRTY;

    /* Free the Pool Instance reserved by the oam stat action */
    rv = _field_oam_stat_action_pool_instance_reserve (unit, f_ent,
            &pool_instance, _bcmFieldOamStatPoolInstanceGet);
    if (BCM_FAILURE (rv)) {
        return rv;
    }

    /* If there no pools reserved, reset the oam stat valid flag 
     * so that nat actions can be configured */
    if (pool_instance == 0) {
        f_ent->flags  &= ~_FP_ENTRY_OAM_STAT_VALID;
    }


    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_oam_stat_action_delete_all
 * Description:
 *     Remove all Oam Stat action from Field Processor entry.
 * Parameters:
 *      unit            - (IN) BCM device number.
 *      entry           - (IN) Field entry id.
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_field_oam_stat_action_delete_all (int unit, bcm_field_entry_t entry)
{
    _field_entry_t *f_ent;                          /* Field entry pointer.  */
    _field_oam_stat_action_t *oam_stat_node;        /* Oam stat Node         */
    int8 pool_instance = -1;                        /* Pool Instance         */
    int rv = BCM_E_NONE;                            /* Return Value          */

    /* Get entry info. */
    BCM_IF_ERROR_RETURN
        (_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent));

    oam_stat_node = f_ent->field_oam_stat_action;

    while (oam_stat_node != NULL) {
        /* Free the reserved flex stat pool index */
        pool_instance = oam_stat_node->pool_instance;
        rv = _field_oam_stat_action_pool_instance_reserve (unit, f_ent,
                &pool_instance, _bcmFieldOamStatPoolInstanceFree);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        /* Update the flags */
        oam_stat_node->flags &= ~(_FP_OAM_STAT_ACTION_VALID);
        oam_stat_node->flags |= _FP_OAM_STAT_ACTION_DIRTY;

        if (0 == (f_ent->flags & _FP_ENTRY_DIRTY)) {
            f_ent->flags |= _FP_ENTRY_POLICY_TABLE_ONLY_DIRTY;
        }

        f_ent->flags  |= _FP_ENTRY_DIRTY;
        oam_stat_node = oam_stat_node->next;
    }

    rv = _field_oam_stat_action_pool_instance_reserve (unit, f_ent,
            &pool_instance, _bcmFieldOamStatPoolInstanceGet);
    if (BCM_FAILURE (rv)) {
        return rv;
    }

    /* If there no pools reserved, reset the oam stat valid flag 
     * so that nat actions can be configured */
    if (pool_instance == 0) {
        f_ent->flags  &= ~_FP_ENTRY_OAM_STAT_VALID;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_oam_stat_action_get
 * Description:
 *     Get Oam Stat action from Field Processor entry.
 * Parameters:
 *      unit            - (IN) BCM device number.
 *      entry           - (IN) Field entry id.
 *      oam_stat_action - (INOUT) Oam Stat Action Structure.
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_field_oam_stat_action_get (int unit, bcm_field_entry_t entry,
                            bcm_field_oam_stat_action_t *oam_stat_action)
{
    _field_entry_t *f_ent;                          /* Field entry pointer.  */
    _field_oam_stat_action_t *oam_stat_node;        /* Oam stat Node.        */
    bcm_field_oam_stat_action_t *oam_action;        /* Oam stat action.      */
    int rv = BCM_E_NONE;

    /* Check input parameters */
    if (oam_stat_action == NULL) {
        return BCM_E_PARAM;
    }

    /* Get entry info. */
    BCM_IF_ERROR_RETURN
        (_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent));


    /* Get stat action node and fill the get node */
    rv = _field_oam_stat_action_node_get (unit, f_ent,
            oam_stat_action, &oam_stat_node);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    /* Search and fill the structure and return */
    if (oam_stat_node != NULL) {
        oam_action = oam_stat_node->oam_stat_action;
        if (oam_action != NULL) {
            oam_stat_action->stat_object = oam_action->stat_object;
            oam_stat_action->stat_mode_id = oam_action->stat_mode_id;
            oam_stat_action->action = oam_action->action;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_field_oam_stat_action_get_all
 * Description:
 *     Get all Oam Stat action from Field Processor entry.
 * Parameters:
 *      unit            - (IN) BCM device number.
 *      entry           - (IN) Field entry id.
 *      oam_stat_max    - (IN) Max number of Oam Stat actions.
 *      oam_stat_action - (IN) Oam Stat Action Structure.
 *      oam_stat_count  - (INOUT) Number of oam stats configured.
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_field_oam_stat_action_get_all(int unit,
                         bcm_field_entry_t entry, int oam_stat_max,
                         bcm_field_oam_stat_action_t *oam_stat_action,
                         int *oam_stat_count)
{
    _field_entry_t *f_ent;                          /* Field entry pointer.  */
    _field_oam_stat_action_t *oam_stat_node;        /* Oam stat Node.        */
    bcm_field_oam_stat_action_t *oam_action;        /* Oam stat action.      */

    if (oam_stat_count == NULL) {
        return BCM_E_PARAM;
    }

    if ((oam_stat_max != 0) && (oam_stat_action == NULL)) {
        return BCM_E_PARAM;
    }

    *oam_stat_count = 0;

    /* Get entry info. */
    BCM_IF_ERROR_RETURN
        (_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent));

    oam_stat_node = f_ent->field_oam_stat_action;

    /* Search and fill the structure and return */
    /* If oam_stat_max equal zero, oam_stat_count being non-NULL is
     * expected to return the number of stat actions present in the entry
     * so that sufficient memory for the oam_action can be alloted by the user
     * and get_all api can be called again with non-zero oam_stat_max value 
     */
    while (oam_stat_node != NULL) {
        oam_action = oam_stat_node->oam_stat_action;
        if (oam_action != NULL) {
            if (oam_stat_max == 0) {
                (*oam_stat_count)++;
            } else {
                if (oam_stat_max >= (*oam_stat_count)) {
                    sal_memcpy (&oam_stat_action[*oam_stat_count],
                            oam_action, sizeof (bcm_field_oam_stat_action_t));
                }
                (*oam_stat_count)++;
            }
        }
        oam_stat_node = oam_stat_node->next;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_oam_stat_actions_destroy
 * Description:
 *      Destroy all the stat actions
 * Parameters:
 *      f_ent           - (IN) Field entry Structure.
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_field_oam_stat_actions_destroy (int unit, _field_entry_t *f_ent)
{
    _field_oam_stat_action_t *oam_stat_node;     /* Oam stat Node.    */
    int rv = 0;

    if (f_ent == NULL) {
        return BCM_E_PARAM;
    }

    /* If there are no oam actions return E_NONE */
    if (f_ent->field_oam_stat_action == NULL) {
        return BCM_E_NONE;
    }

    while (f_ent->field_oam_stat_action != NULL) {
        oam_stat_node = f_ent->field_oam_stat_action;
        f_ent->field_oam_stat_action =
            (f_ent->field_oam_stat_action)->next;

        sal_free (oam_stat_node->oam_stat_action);
        oam_stat_node->oam_stat_action = NULL;
        sal_free (oam_stat_node);
        oam_stat_node = NULL;
    }

    /* Update flags and free the pool instances */
    f_ent->flags &= ~_FP_ENTRY_OAM_STAT_VALID;
    rv = _field_oam_stat_action_pool_instance_reserve (unit, f_ent,
            NULL, _bcmFieldOamStatPoolInstanceFreeAll);
    if (BCM_FAILURE (rv)) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_oam_stat_invalid_actions_remove
 * Description:
 *      Remove invalid Oam Stat Actions in H/w.
 * Parameters:
 *      unit            - (IN) BCM device number.
 *      f_ent           - (IN) Field entry Structure.
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_field_oam_stat_invalid_actions_remove (int unit,
                                     _field_entry_t *f_ent)
{
    _field_oam_stat_action_t *oam_stat_node;     /* Oam stat Node.    */
    _field_oam_stat_action_t *oam_prev_node;     /* Oam stat Node.    */
    _field_oam_stat_action_t *oam_temp_node;     /* Oam stat Node.    */
    int rv = 0;

    if (f_ent == NULL) {
        return BCM_E_PARAM;
    }

    if (f_ent->field_oam_stat_action == NULL) {
        return BCM_E_NONE;
    }

    oam_stat_node = f_ent->field_oam_stat_action;
    oam_prev_node = oam_stat_node;

    while (oam_stat_node != NULL) {
        if ((oam_stat_node->flags & _FP_OAM_STAT_ACTION_VALID) == 0) {
            if (oam_stat_node == f_ent->field_oam_stat_action) {
                oam_temp_node = oam_stat_node;
                f_ent->field_oam_stat_action = oam_stat_node->next;
                oam_prev_node = f_ent->field_oam_stat_action;
            } else {
                oam_prev_node->next = oam_stat_node->next;
                oam_temp_node = oam_stat_node;
            }

            oam_stat_node = oam_stat_node->next;
            sal_free (oam_temp_node->oam_stat_action);
            oam_temp_node->oam_stat_action = NULL;
            sal_free (oam_temp_node);
            oam_temp_node = NULL;
        } else {
            oam_prev_node = oam_stat_node;
            oam_stat_node = oam_stat_node->next;
        }
    }

    /* Reset the flag if all the oam stat actions are removed */
    if (f_ent->field_oam_stat_action == NULL) {
        f_ent->flags &= ~_FP_ENTRY_OAM_STAT_VALID;
        rv = _field_oam_stat_action_pool_instance_reserve (unit, f_ent,
                NULL, _bcmFieldOamStatPoolInstanceFreeAll);
        if (BCM_FAILURE (rv)) {
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_oam_stat_action_set 
 * Description:
 *     Set Oam Stat Actions in H/w.
 * Parameters:
 *      unit            - (IN) BCM device number.
 *      f_ent           - (IN) Field entry Structure.
 *      policy_mem      - (IN) Policy table memory
 *      policy_buf      - (INOUT) Memory buf.
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_field_oam_stat_action_set (int unit, _field_entry_t *f_ent,
                             soc_mem_t mem, uint32 *buf)
{
    _field_oam_stat_action_t *oam_stat_node;     /* Oam stat Node.    */
    bcm_field_oam_stat_action_t *oam_action;     /* Oam stat action.  */
    int action = 0;                              /* Stat Action Value */

    if ((f_ent == NULL) || (buf == NULL)) {
        return BCM_E_PARAM;
    }

    oam_stat_node = f_ent->field_oam_stat_action;

    while (oam_stat_node != NULL) {
        if (oam_stat_node->flags & _FP_OAM_STAT_ACTION_VALID) {
            oam_action = oam_stat_node->oam_stat_action;
            if (oam_action == NULL) {
                return BCM_E_PARAM;
            }

            action = oam_action->action;

            switch (oam_stat_node->pool_instance) {
                case 0:
                    PolicySet (unit, mem, buf, LM_COUNTER_POOL_ID_Af,
                                           oam_stat_node->flex_pool_id);
                    PolicySet (unit, mem, buf, LM_COUNTER_POOL_ACTION_Af,
                                           action);

                    /* Egress stage doesn't have NAT OAM ACTIONS */
                    if (_BCM_FIELD_STAGE_INGRESS == f_ent->group->stage_id) {
                        PolicySet (unit, mem, buf, NAT_OAM_ACTION_SELECTf,
                                              _bcmFieldNatOamValidOam);
                    }
                    break;
                case 1:
                    PolicySet (unit, mem, buf, LM_COUNTER_POOL_ID_Bf,
                                           oam_stat_node->flex_pool_id);
                    PolicySet (unit, mem, buf, LM_COUNTER_POOL_ACTION_Bf,
                                           action);

                    /* Egress stage doesn't have NAT OAM ACTIONS */
                    if (_BCM_FIELD_STAGE_INGRESS == f_ent->group->stage_id) {
                        PolicySet (unit, mem, buf, NAT_OAM_ACTION_SELECTf,
                                              _bcmFieldNatOamValidOam);
                    }
                    break;
                case 2:
                    PolicySet (unit, mem, buf, LM_COUNTER_POOL_ID_Cf,
                                           oam_stat_node->flex_pool_id);
                    PolicySet (unit, mem, buf, LM_COUNTER_POOL_ACTION_Cf,
                                           action);

                    /* Egress stage doesn't have NAT OAM ACTIONS */
                    if (_BCM_FIELD_STAGE_INGRESS == f_ent->group->stage_id) {
                        PolicySet (unit, mem, buf, NAT_OAM_ACTION_SELECTf,
                                              _bcmFieldNatOamValidOam);
                    }
                    break;
                default:
                    return BCM_E_PARAM;
            }
        }
        oam_stat_node->flags &= ~_FP_OAM_STAT_ACTION_DIRTY;
        oam_stat_node = oam_stat_node->next;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_oam_stat_action_dump 
 * Description:
 *     Dump Oam Stat Actions.
 * Parameters:
 *      unit            - (IN) BCM device number.
 *      f_ent           - (IN) Field entry Structure.
 * Returns:
 *      BCM_E_XXX
 */
int _field_oam_stat_action_dump (int unit, _field_entry_t *f_ent) 
{
    _field_oam_stat_action_t *oam_stat_node;     /* Oam stat Node.    */
    bcm_field_oam_stat_action_t *oam_action;     /* Oam stat action.  */
    int rv = BCM_E_NONE;
    char                 *sname[] = BCM_FIELD_STAT_ACTION;
    char                 *oname[] = BCM_STAT_OBJECT;
    char                 stat_mode[16];

    /* Input parameters check. */
    if (f_ent == NULL) {
        return (BCM_E_PARAM);
    }

    oam_stat_node = f_ent->field_oam_stat_action;
    /* Display entry's oam Stat Actions. */
    if (oam_stat_node == NULL) {
        LOG_CLI((BSL_META_U(unit,
                        "         Oam stat Action=NULL\r\n")));
    } else {
        LOG_CLI((BSL_META_U(unit,
                        "         Oam stat Action=\r\n")));
        while (oam_stat_node != NULL) {
            oam_action = oam_stat_node->oam_stat_action;
            if (oam_action != NULL) {
                memset (stat_mode,0, sizeof(stat_mode));
               if ((oam_action->stat_mode_id & BCM_STAT_GROUP_MODE_ID_OAM_MASK)
                        == BCM_STAT_GROUP_MODE_ID_OAM_SUFFIX) {
                    sal_strlcpy (stat_mode,
                                "OamCapable", sizeof(stat_mode));
                } else {
                    sal_strlcpy (stat_mode,
                                "OamInCapable", sizeof (stat_mode));
                }
                LOG_CLI((BSL_META_U(unit,
                                "                        {Object: %s; Mode:"
                                " %s; Action: %s;}\r\n"),
                            oname[oam_action->stat_object],
                            stat_mode, sname[oam_action->action]));
            }
            oam_stat_node = oam_stat_node->next;
        }
    }

    return rv;
}

/*
 * Function:
 *    _field_oam_stat_action_display 
 * Description:
 *     Dump Oam Stat Actions of all entries and groups.
 * Parameters:
 *      unit            - (IN) BCM device number.
 * Returns:
 *      BCM_E_XXX
 */
int _field_oam_stat_action_display (int unit) 
{
    _field_group_t *fg = NULL;                 /* Field Group Structure */
    _field_control_t *fc = NULL;               /* Field Control Structure */
    uint32 eid = 0;                            /* Entry Id */
    int idx = 0;                               /* Entry Id */
    _field_entry_t      *f_ent;                /* Field entry info */
    _field_oam_stat_action_t *oam_stat_node;   /* Oam stat Node    */
    bcm_field_oam_stat_action_t *oam_action;   /* Oam stat action  */
    char *sname[] = BCM_FIELD_STAT_ACTION;     /* Stat action String */
    char *oname[] = BCM_STAT_OBJECT;           /* Stat Object String */
    int8 pool_bitmap = 0;
    int rv = 0;

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    fg = fc->groups;

    while (fg != NULL) {
        if (fg->stage_id == _BCM_FIELD_STAGE_INGRESS) {

            LOG_CLI((BSL_META_U(unit,
                            "Oam stat Action for Group %d\r\n"),fg->gid));
            for (idx = 0; idx < fg->group_status.entry_count; idx++) {
                eid = (fg->entry_arr[idx])->eid;

                /* Get entry description structure. */
                BCM_IF_ERROR_RETURN((_field_entry_get(unit,
                                eid, _FP_ENTRY_PRIMARY, &f_ent)));

                oam_stat_node = f_ent->field_oam_stat_action;
                rv = _field_oam_stat_action_pool_instance_reserve (unit, f_ent,
                        &pool_bitmap, _bcmFieldOamStatPoolInstanceGet);
                if (BCM_FAILURE (rv)) {
                    return rv;
                }

                /* Display entry's oam Stat Actions. */
                LOG_CLI((BSL_META_U(unit, "Oam stat Action: Entry %d"
                                " for Pool Bitmap %d\r\n"),eid,pool_bitmap));
                while (oam_stat_node != NULL) {
                    LOG_CLI((BSL_META_U(unit, " Pool Instance: %d"
                                    " Flex_Pool: %d\r\n"),
                                oam_stat_node->pool_instance,
                                oam_stat_node->flex_pool_id));

                    oam_action = oam_stat_node->oam_stat_action;
                    if (oam_action != NULL) {
                        LOG_CLI((BSL_META_U(unit,
                                        "Node: %p Node_action:%p {Object: "
                                        "%s;Mode Id: %d;Action: %s;}\r\n"),
                                    oam_stat_node, oam_action,
                                    oname[oam_action->stat_object],
                                    oam_action->stat_mode_id,
                                    sname[oam_action->action]));
                    } else {
                        return BCM_E_INTERNAL;
                    }
                    oam_stat_node = oam_stat_node->next;
                }
            }
            fg = fg->next;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_td2plus_entry_stat_extended_attach
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
_bcm_field_td2plus_entry_stat_extended_attach(int unit,
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
    if ((stage_id != _BCM_FIELD_STAGE_INGRESS) &&
        (stage_id != _BCM_FIELD_STAGE_EGRESS)) {
        return BCM_E_UNAVAIL;
    }

    /* TD3 supports extended stats only for stage EFP*/
    if (soc_feature(unit, soc_feature_td3_style_fp) &&
        (stage_id != _BCM_FIELD_STAGE_EGRESS)) {
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

    if ((SOC_IS_TRIDENT2PLUS(unit)
        || soc_feature(unit, soc_feature_td3_style_fp)) &&
        (stat_action != bcmFieldStatActionIncrement) &&
        (stage_id == _BCM_FIELD_STAGE_EGRESS)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                (BSL_META("FP(unit %d) Error: Egress Stage supports "
                          "only Incremenent stat action\r\n"), unit));
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
 *    _bcm_field_td2plus_entry_stat_extended_get
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
int _bcm_field_td2plus_entry_stat_extended_get (int unit,
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
    if ((stage_id != _BCM_FIELD_STAGE_INGRESS) &&
        (stage_id != _BCM_FIELD_STAGE_EGRESS)) {
        return BCM_E_UNAVAIL;
    }

    /* TD3 supports extended stats only for EFP */
    if (soc_feature(unit, soc_feature_td3_style_fp) &&
        (stage_id != _BCM_FIELD_STAGE_EGRESS)) {
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
 *    _bcm_field_td2plus_entry_stat_detach
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
int _bcm_field_td2plus_entry_stat_detach (int unit,
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

    /* Valid only for Ingress & Egress Stage */
    if ((stage_id != _BCM_FIELD_STAGE_INGRESS)
            && (stage_id != _BCM_FIELD_STAGE_EGRESS)) {
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
                _bcm_field_td2plus_extended_stat_hw_free(unit, f_ent));
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

/*
 * Function:
 *     _bcm_field_td2plus_extended_stat_hw_alloc
 *
 * Purpose:
 *     Allocate hw counter from an entry.
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     f_ent     - (IN) Entry statistics belongs to.
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_field_td2plus_extended_stat_hw_alloc (int unit, _field_entry_t *f_ent)
{
    _field_entry_stat_t    *f_ent_st;  /* Field entry stat structure.   */
    _field_stat_t          *f_st;      /* Field statistics descriptor.  */
    int                    rv;         /* Operation return status.      */
    bcm_stat_flex_mode_t    offset_mode;  /* Counter mode.                  */
    bcm_stat_group_mode_t   stat_mode;    /* Stat type bcmStatGroupModeXXX. */
    bcm_stat_object_t       stat_obj;     /* Stat object type.              */
    uint32                  pool_num;     /* Flex Stat Hw Pool No.          */
    uint32                  base_index;   /* Flex Stat counter base index.  */
    uint32                  flex_sid;     /* Flex Stat ID.                  */
    uint32                  num_hw_cntrs; /* Number of counters allocated.  */
    _field_stage_id_t      stage_id;  /* Pipeline stage id. */


    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    /* Get entry pipeline stage id. */
    stage_id = f_ent->group->stage_id;

    /* Valid only for Ingress Stage */
    if ((stage_id != _BCM_FIELD_STAGE_INGRESS)
            && (stage_id != _BCM_FIELD_STAGE_EGRESS)) {
        return BCM_E_UNAVAIL;
    }

    f_ent_st = &f_ent->statistic;

    /* Skip entries without countres attached. */
    if ((0 == (f_ent_st->flags & _FP_ENTRY_EXTENDED_STAT_VALID)) ||
            (f_ent_st->flags & _FP_ENTRY_STAT_EMPTY)) {
        return (BCM_E_NONE);
    }

    /* Read statistics entity configuration. */
    rv = _bcm_field_stat_get(unit, f_ent_st->extended_sid, &f_st);
    BCM_IF_ERROR_RETURN (rv);

    if (soc_feature(unit, soc_feature_advanced_flex_counter)) {
        /* Allocate flex STAT entity hw index. */
        /* If counter is disabled skip allocation. */
        if (0 == f_st->hw_stat) {
            return (BCM_E_NONE);
        }

        if (_FP_INVALID_INDEX == f_st->hw_index) {
            switch (f_st->stage_id) {
                case _BCM_FIELD_STAGE_INGRESS:
                    stat_obj = bcmStatObjectIngFieldStageIngress;
                    break;
                case _BCM_FIELD_STAGE_EGRESS:
                    stat_obj = bcmStatObjectEgrFieldStageEgress;
                    break;
                default:
                    /* IFP and EFP stages does not use Flex STAT. */
                    return (BCM_E_INTERNAL);
            }

            flex_sid = f_st->flex_mode;
            num_hw_cntrs = f_st->hw_entry_count;
            /* Create a flex stat group. */
            if ((f_ent_st->flags & _FP_ENTRY_EXTENDED_STAT_RESERVED) == 0) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_stat_group_create(unit,
                                               stat_obj,
                                               (bcm_stat_group_mode_t)
                                               f_st->hw_mode,
                                               &flex_sid,
                                               &num_hw_cntrs
                                              )
                    );
            }

            /* Get flex counter hardware details stored in flex stat ID.*/
            _bcm_esw_stat_get_counter_id_info(unit, flex_sid,
                    &stat_mode,
                    &stat_obj,
                    &offset_mode,
                    &pool_num,
                    &base_index
                    );
            /* Store flex stat hardware details in stat data structure. */
            f_st->flex_mode = flex_sid;
            f_st->pool_index = pool_num;
            f_st->hw_index = base_index;
            f_st->hw_mode = offset_mode;
            f_st->hw_entry_count = num_hw_cntrs;
        }
        /* Increment the group counters count equal to
           the hw entries count .*/
           f_ent->group->group_status.counter_count =
                 f_ent->group->group_status.counter_count + f_st->hw_entry_count;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_td2plus_extended_stat_hw_free
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
int _bcm_field_td2plus_extended_stat_hw_free (int unit, _field_entry_t *f_ent)
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

    /* Valid only for Ingress & Egress Stage */
    if ((stage_id != _BCM_FIELD_STAGE_INGRESS)
            && (stage_id != _BCM_FIELD_STAGE_EGRESS)) {
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

            if (stage_id == _BCM_FIELD_STAGE_INGRESS) {
                rv = _bcm_esw_stat_flex_detach_ingress_table_counters (unit,
                        policy_mem, entry_idx);
            } else if (stage_id == _BCM_FIELD_STAGE_EGRESS) {
                rv = _bcm_esw_stat_flex_detach_egress_table_counters (unit, 0,
                        policy_mem, entry_idx);
            }

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
 *     _bcm_field_td2plus_stat_hw_alloc
 *
 * Purpose:
 *    Allocate hw counter from an entry.
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     f_ent     - (IN) Entry statistics belongs to.
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_field_td2plus_stat_hw_alloc (int unit, _field_entry_t *f_ent)
{
    _field_stage_id_t      stage_id;  /* Pipeline stage id. */

    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    /* Get entry pipeline stage id. */
    stage_id = f_ent->group->stage_id;

    BCM_IF_ERROR_RETURN (_bcm_field_stat_hw_alloc(unit, f_ent));

    /* Valid only for Ingress Stage */
    if ((stage_id == _BCM_FIELD_STAGE_INGRESS)
            || (stage_id == _BCM_FIELD_STAGE_EGRESS)) {
        BCM_IF_ERROR_RETURN (_bcm_field_td2plus_extended_stat_hw_alloc
                (unit, f_ent));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_td2plus_stat_hw_free
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
int _bcm_field_td2plus_stat_hw_free (int unit, _field_entry_t *f_ent)
{
    _field_stage_id_t      stage_id;  /* Pipeline stage id. */

    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    /* Get entry pipeline stage id. */
    stage_id = f_ent->group->stage_id;

    BCM_IF_ERROR_RETURN (_bcm_field_stat_hw_free(unit, f_ent));

    /* Valid only for Ingress,Egress Stage */
    if ((stage_id == _BCM_FIELD_STAGE_INGRESS)
            || (stage_id == _BCM_FIELD_STAGE_EGRESS)) {
        BCM_IF_ERROR_RETURN (
                _bcm_field_td2plus_extended_stat_hw_free(unit, f_ent));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_td2plus_extended_stat_action_set
 * Purpose:
 *     Install counter update action into policy table.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     f_ent        - (IN) entry structure to get policy info from
 *     mem          - (IN) Policy table memory
 *     tcam_idx     - (IN) Common index of various tables
 *     buf          - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_field_td2plus_extended_stat_action_set (int unit,
                                                 _field_entry_t *f_ent,
                                                 soc_mem_t mem, int tcam_idx,
                                                 uint32 *buf)
{
    _field_stat_t  *f_st;  /* Field statistics descriptor. */
    int             idx;   /* Stat array index.            */
    _field_stage_id_t      stage_id;  /* Pipeline stage id. */


    /* Check input params */
    if ((f_ent == NULL) || (buf == NULL)) {
        return BCM_E_PARAM;
    }

    /* Get entry pipeline stage id. */
    stage_id = f_ent->group->stage_id;
    if ((stage_id != _BCM_FIELD_STAGE_INGRESS)
            && (stage_id != _BCM_FIELD_STAGE_EGRESS)) {
        return BCM_E_NONE;
    }

    /* Increment statistics hw reference count. */
    if ((f_ent->statistic.flags & _FP_ENTRY_EXTENDED_STAT_VALID)
            && !(f_ent->statistic.flags & _FP_ENTRY_EXTENDED_STAT_INSTALLED)) {
        /* Get statistics entity description structure. */
        BCM_IF_ERROR_RETURN
            (_bcm_field_stat_get(unit, f_ent->statistic.extended_sid, &f_st));
        if (_FP_INVALID_INDEX != f_st->hw_index) {
            /*
             * For STATs that are shared by entries, hardware counters
             * are not allocated again. But reference count is incremented
             * for these counters.
             */
            f_st->hw_ref_count++;

            if (stage_id == _BCM_FIELD_STAGE_INGRESS) {
                PolicySet(unit, mem, buf, G_L3SW_CHANGE_L2_FIELDSf, 0x9);
                PolicySet(unit, mem, buf, FLEX_COUNTER_ACTIONf,
                        f_ent->statistic.stat_action);
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_stat_flex_attach_ingress_table_counters1
                     (unit, mem, tcam_idx, f_st->hw_mode, f_st->hw_index,
                      f_st->pool_index, buf));
            } else if (stage_id == _BCM_FIELD_STAGE_EGRESS) {
                PolicySet(unit, mem, buf, FLEX_COUNTER_DSCP_SELf, 0x1);
                if (SOC_IS_APACHE(unit)) {
                    PolicySet(unit, mem, buf, FLEX_COUNTER_ACTIONf,
                            f_ent->statistic.stat_action);
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_stat_flex_attach_egress_table_counters1
                     (unit, mem, tcam_idx, f_st->hw_mode, f_st->hw_index,
                      f_st->pool_index, buf));
            }

            /* Mark entry as installed. */
            f_ent->statistic.flags |=  _FP_ENTRY_EXTENDED_STAT_INSTALLED;

            /*
             * Write individual statistics previous value, first time
             * entry is installed in hardware.
             */
            if (1 == f_st->hw_ref_count) {
                for (idx = 0; idx < f_st->nstat; idx++) {
                    BCM_IF_ERROR_RETURN
                        (_field_stat_value_set(unit, f_st, f_st->stat_arr[idx],
                                               f_st->stat_values[idx]));
                }
            }
        }
    }
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

/* Function: _field_oam_stat_action_count_get
 *
 * Purpose:
 *        Sync  Oam Stat Action information into scache
 * Parameters:
 *                 unit - (IN)  BCM device number.
 *    oam_pool_instance - (IN)  Oam Stat Pool Instance Bitmap.
 *       oam_stat_count - (OUT) Number of Oam Stat configured.
 * Returns:
 *      BCM_E_XXXX.
 */
int _field_oam_stat_action_count_get (int unit,
        uint8 oam_pool_instance, int8 *oam_stat_count)
{
    int idx = 0;
    int count = 0;

    if (oam_stat_count == NULL) {
        return BCM_E_PARAM;
    }

    for (idx = 0; idx < 3; idx++) {
        if (oam_pool_instance & (1 << idx)) {
            count++;
        }
    }

    *oam_stat_count = count;

    return BCM_E_NONE;
}

/* Function: _field_oam_stat_action_scache_sync
 *
 * Purpose:
 *        Sync  Oam Stat Action information into scache
 * Parameters:
 *         unit      - (IN)    BCM device number.
 *           fc      - (IN)    Field Control.
 *     stage_fc      - (IN)    Stage Control.
 *     scache_ptr    - (INOUT) Scache Pointer.
 * Returns:
 *      BCM_E_XXXX.
 */
int _field_oam_stat_action_scache_sync (int unit,
                                     _field_stage_t   *stage_fc,
                                     _field_entry_t   *f_ent,
                                     _field_entry_oam_wb_t *f_ent_oam_wb)
{
    int8 oam_stat_count = 0;
    int idx = 0;
    _field_oam_stat_action_t *oam_stat_node;        /* Oam stat Node.        */
    bcm_field_oam_stat_action_t *oam_action;        /* Oam stat action.      */
    int oam_stat_size = 0;

    if ((stage_fc == NULL) || (f_ent_oam_wb == NULL) || (f_ent == NULL)) {
        return BCM_E_PARAM;
    }

    if (stage_fc->stage_id != _BCM_FIELD_STAGE_INGRESS) {
        return BCM_E_NONE;
    }

    /* If oam_stat_node is NULL return*/
    oam_stat_node = f_ent->field_oam_stat_action;
    if (oam_stat_node == NULL) {
        return BCM_E_NONE;
    }

    /* Get number of stat actions configured */
    BCM_IF_ERROR_RETURN(_field_oam_stat_action_count_get (unit,
                       f_ent->oam_pool_instance, &oam_stat_count));

    if (oam_stat_count != 0) {
        /* Set Bitmap for syncing oam stats */
        f_ent_oam_wb->is_present_bmp |= _FP_OAM_STAT_ACTION_WB_SYNC;


        /* Alloc Structure for oam_stat */
         oam_stat_size = sizeof (_field_oam_stat_action_wb_t)
                             * oam_stat_count;
        _FP_XGS3_ALLOC(f_ent_oam_wb->oam_stat_action_wb,
                oam_stat_size, "Oam Stat Action WB");
        if (f_ent_oam_wb->oam_stat_action_wb == NULL) {
            return BCM_E_MEMORY;
        }

        /* Add Oam Stat information */
        while (oam_stat_node != NULL) {
            if ((oam_stat_node->flags & _FP_OAM_STAT_ACTION_VALID) &&
                   ((oam_stat_node->flags & _FP_OAM_STAT_ACTION_DIRTY) == 0)) {
                oam_action = oam_stat_node->oam_stat_action;
                if (oam_action != NULL) {
                    f_ent_oam_wb->oam_stat_action_wb[idx].stat_object =
                        oam_action->stat_object;
                    f_ent_oam_wb->oam_stat_action_wb[idx].stat_mode_id =
                        oam_action->stat_mode_id;
                }
                idx++;
            }
            oam_stat_node = oam_stat_node->next;
        }

        /* Add oam stat count */
        f_ent_oam_wb->oam_stat_action_count = idx;
    }

    return BCM_E_NONE;
}

/* Function: _field_oam_stat_action_scache_recover
 *
 * Purpose:
 *        Recover Oam Stat action information from scache
 * Parameters:
 *         unit      - (IN)    BCM device number.
 *           fc      - (IN)    Field Control.
 *     stage_fc      - (IN)    Stage Control.
 *     scache_ptr    - (INOUT) Scache Pointer.
 * Returns:
 *      BCM_E_XXXX.
 */
int _field_oam_stat_action_scache_recover (int unit,
                                           _field_stage_t   *stage_fc,
                                           _field_entry_t *f_ent,
                                           _field_entry_oam_wb_t *f_ent_oam_wb)
{
    bcm_field_oam_stat_action_t oam_stat_action;
    int idx = 0;
    _field_oam_stat_action_t *oam_stat_node = NULL; /* Oam Stat Action Node   */
    int8 oam_lm_stat_pool_id = -1;                  /* Oam Lm Stat Pool Id    */
    int rv = BCM_E_NONE;


    if ((f_ent == NULL) || (f_ent_oam_wb == NULL) || (stage_fc == NULL)) {
        return BCM_E_PARAM;
    }

    if (stage_fc->stage_id != _BCM_FIELD_STAGE_INGRESS) {
        return BCM_E_NONE;
    }

    if ((f_ent_oam_wb->is_present_bmp & _FP_OAM_STAT_ACTION_WB_SYNC) == 0) {
        return BCM_E_NONE;
    }

    if (f_ent_oam_wb->oam_stat_action_count == 0) {
        return BCM_E_NONE;
    }

    for (idx = 0 ; idx < f_ent_oam_wb->oam_stat_action_count; idx++) {
        sal_memset (&oam_stat_action, 0, sizeof(bcm_field_oam_stat_action_t));
        oam_stat_action.stat_object  = f_ent_oam_wb->oam_stat_action_wb[idx].stat_object;
        oam_stat_action.stat_mode_id = f_ent_oam_wb->oam_stat_action_wb[idx].stat_mode_id;
        oam_stat_action.action = bcmFieldStatActionNone;

        oam_stat_node = NULL;

        /* Allocate memory and copy the input params */
        _FP_XGS3_ALLOC(oam_stat_node, sizeof (_field_oam_stat_action_t),
                "field oam stat action node");
        if (oam_stat_node == NULL) {
            LOG_ERROR(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,
                                "FP(unit %d) Error: allocation failure for "
                                "oam_stat_action_node\n"), unit));
            return (BCM_E_MEMORY);
        }

        /* Allocate memory and copy the input params */
        _FP_XGS3_ALLOC(oam_stat_node->oam_stat_action,
                sizeof (bcm_field_oam_stat_action_t),
                "field oam stat action params");
        if (oam_stat_node->oam_stat_action == NULL) {
            LOG_ERROR(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,
                                "FP(unit %d) Error: allocation failure for "
                                "oam_stat_action_node\n"), unit));
            sal_free (oam_stat_node);
            return (BCM_E_MEMORY);
        }

        /* Get Flex Stat PoolId from the Stat Object and Stat Mode Id */
        rv = _field_oam_lm_stat_pool_id_get (unit,
                    oam_stat_action.stat_object, oam_stat_action.stat_mode_id,
                    &oam_lm_stat_pool_id);
        if (BCM_FAILURE(rv)) {
            sal_free (oam_stat_node->oam_stat_action);
            sal_free (oam_stat_node);
            return rv;
        }

        /* Copy the input parameters and insert to linked list */
        sal_memcpy (oam_stat_node->oam_stat_action, &oam_stat_action,
                sizeof (bcm_field_oam_stat_action_t));
        oam_stat_node->flex_pool_id = oam_lm_stat_pool_id;
        oam_stat_node->next = NULL;
        oam_stat_node->flags |= _FP_OAM_STAT_ACTION_VALID;

        /* Add node to link list */
        _FP_LINKLIST_INSERT(&(f_ent->field_oam_stat_action), oam_stat_node, &rv);
        if (BCM_FAILURE(rv)) {
            sal_free (oam_stat_node->oam_stat_action);
            sal_free (oam_stat_node);
            return rv;
        }

        f_ent->flags  |= _FP_ENTRY_OAM_STAT_VALID;
    }

    return BCM_E_NONE;
}

/* Function: _field_oam_stat_action_wb_add
 *
 * Purpose:
 *        Recover Oam Stat action information from H/W
 *        and add it to the structure
 * Parameters:
 *         unit      - (IN)    BCM device number.
 *           fc      - (IN)    Field Control.
 *     stage_fc      - (IN)    Stage Control.
 *     scache_ptr    - (INOUT) Scache Pointer.
 * Returns:
 *      BCM_E_XXXX.
 */
int _field_oam_stat_action_wb_add (int unit, _field_entry_t *f_ent,
                                   _field_oam_action_recover_t *oam_action_hw)
{
    _field_oam_stat_action_t *oam_stat_node = NULL; /* Oam Stat Action Node   */
    bcm_field_oam_stat_action_t *oam_action;
    int idx = 0;
    uint8 *local_pool_instance = NULL;

    if ((f_ent == NULL) || (oam_action_hw == NULL)) {
        return BCM_E_PARAM;
    }

    _FP_XGS3_ALLOC(local_pool_instance, sizeof (uint8),
                                        "local_pool_instance");
    if (local_pool_instance == NULL) {
        return BCM_E_PARAM;
    }

    sal_memcpy (local_pool_instance,
             &(f_ent->oam_pool_instance), sizeof(uint8));

    oam_stat_node = f_ent->field_oam_stat_action;

    while (oam_stat_node != NULL) {
        for (idx = 0; idx < 3; idx++) {
            if ((oam_stat_node->flex_pool_id
                        == oam_action_hw[idx].flex_pool_id)) {
                /* Set Pool instance */
                oam_stat_node->pool_instance = idx;
                /* Reserve the pool instance in f_ent*/
                SHR_BITSET(local_pool_instance, idx);
                /* Set the oam action */
                oam_action = oam_stat_node->oam_stat_action;
                if (oam_action != NULL) {
                    oam_action->action =
                        (oam_action_hw[oam_stat_node->pool_instance].action);
                }
                break;
            }
        }
        oam_stat_node = oam_stat_node->next;
    }

    f_ent->oam_pool_instance = *local_pool_instance;
    sal_free (local_pool_instance);
    local_pool_instance = NULL;

    return BCM_E_NONE;
}


/* Function: _field_extended_stat_scache_sync
 *
 * Purpose:
 *        Sync  Oam Stat Action information into scache
 * Parameters:
 *         unit      - (IN)    BCM device number.
 *           fc      - (IN)    Field Control.
 *     stage_fc      - (IN)    Stage Control.
 *     scache_ptr    - (INOUT) Scache Pointer.
 * Returns:
 *      BCM_E_XXXX.
 */
int _field_extended_stat_scache_sync (int unit, _field_stage_t   *stage_fc,
        _field_entry_t   *f_ent,  _field_entry_oam_wb_t *f_ent_oam_wb)
{
    _field_entry_stat_t    *f_ent_st; /* Field entry statistics structure.*/
    _field_stat_t          *f_st;     /* Internal statisics descriptor.   */
    int rv = BCM_E_NONE;
    _field_extended_stat_wb_t *extended_stat_wb = NULL;


    if ((stage_fc == NULL) || (f_ent_oam_wb == NULL) || (f_ent == NULL)) {
        return BCM_E_PARAM;
    }

    if ((stage_fc->stage_id != _BCM_FIELD_STAGE_INGRESS) &&
            (stage_fc->stage_id != _BCM_FIELD_STAGE_EGRESS)) {
        return BCM_E_NONE;
    }

    f_ent_st = &f_ent->statistic;

    if ((f_ent_st->flags & _FP_ENTRY_EXTENDED_STAT_VALID)
            && (f_ent_st->flags & _FP_ENTRY_EXTENDED_STAT_INSTALLED)) {
        /* Set Bitmap for syncing extended stats */
        f_ent_oam_wb->is_present_bmp |= _FP_EXTENDED_STATS_WB_SYNC;

        _FP_XGS3_ALLOC(f_ent_oam_wb->extended_stat_wb,
                (sizeof (_field_extended_stat_wb_t)),
                "Extended Stat WB");
        if (f_ent_oam_wb->extended_stat_wb == NULL) {
            return BCM_E_MEMORY;
        }
        extended_stat_wb = f_ent_oam_wb->extended_stat_wb;

        extended_stat_wb->stat_id = f_ent_st->extended_sid;

        /* Get statistics entity  description structure. */
        rv = _bcm_field_stat_get(unit, f_ent_st->extended_sid , &f_st);
        if (BCM_FAILURE(rv)) {
            sal_free (f_ent_oam_wb->extended_stat_wb);
            f_ent_oam_wb->extended_stat_wb = NULL;
            return (rv);
        }

        extended_stat_wb->flex_mode = f_st->flex_mode;
        extended_stat_wb->hw_flags =  f_st->hw_flags;

        switch(f_st->nstat) {
            case 1:if(f_st->stat_arr[0] == bcmFieldStatBytes) {
                      extended_stat_wb->flex_stat_map = 1;
                   } else {
                      extended_stat_wb->flex_stat_map = 2;
                   }
                   break;
            case 2:
            default: /* Although 0-and-default cases are invalid */
                   extended_stat_wb->flex_stat_map = 3;
                   break;
        }
    }

    return BCM_E_NONE;
}

/* Function: _field_extended_stat_scache_recover
 *
 * Purpose:
 *        Sync  Oam Stat Action information into scache
 * Parameters:
 *         unit      - (IN)    BCM device number.
 *           fc      - (IN)    Field Control.
 *     stage_fc      - (IN)    Stage Control.
 *     scache_ptr    - (INOUT) Scache Pointer.
 * Returns:
 *      BCM_E_XXXX.
 */
int _field_extended_flex_counter_recover(int unit, _field_entry_t *f_ent,
                               int part, bcm_field_stat_t sid,
                               _field_entry_oam_wb_t *f_ent_oam_wb,
                               bcm_field_stat_action_t action)
{
    _field_group_t      *fg=NULL;
    _field_stage_t      *stage_fc=NULL;
    _field_stage_id_t   stage_id;
    _field_control_t    *fc=NULL;
    _field_stat_t       *f_st = NULL;
    _field_entry_stat_t *f_ent_st = NULL;
    int                 rv=0;
    int                 stat_id=0;
    bcm_field_stat_t    stat_arr[2]={bcmFieldStatBytes, bcmFieldStatPackets};
    uint8               nstat = 2;
    _field_extended_stat_wb_t *extended_stats;
    bcm_stat_group_mode_t   stat_mode;    /* Stat type bcmStatGroupModeXXX. */
    bcm_stat_object_t       stat_obj;     /* Stat object type.              */
    uint32                  pool_num;     /* Flex Stat Hw Pool No.          */
    uint32                  base_index;   /* Flex Stat counter base index.  */
    int                     offset_mode = 0;

    /* Input Param Check */
    if (f_ent_oam_wb == NULL) {
        return BCM_E_NONE;
    }

    extended_stats = f_ent_oam_wb->extended_stat_wb;
    if (extended_stats == NULL) {
        return BCM_E_NONE;
    }

    stage_id = f_ent->group->stage_id;

    /* Valid only for Ingress and Egress Stage */
    if ((stage_id != _BCM_FIELD_STAGE_INGRESS)
            && (stage_id != _BCM_FIELD_STAGE_EGRESS)) {
        return BCM_E_NONE;
    }

    /* Get field control and stage control */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, stage_id, &stage_fc));

    /* if not level 2 warmboot return */
    if (!fc->l2warm) {
        return BCM_E_NONE;
    }

    rv  = _bcm_field_stat_get(unit, extended_stats->stat_id, &f_st);
    if (!((rv == BCM_E_NOT_FOUND) || (rv == BCM_E_NONE))) {
        return rv;
    }

    if (rv == BCM_E_NOT_FOUND) {
        stat_id = extended_stats->stat_id;

        _FP_XGS3_ALLOC(f_st, sizeof (_field_stat_t), "Field stat entity");
        if (NULL == f_st) {
            return (BCM_E_MEMORY);
        }

        fg = f_ent->group;

        f_st->hw_ref_count = 0;
        f_st->sw_ref_count = 1;
        f_st->pool_index = _FP_INVALID_INDEX;
        f_st->hw_index = _FP_INVALID_INDEX;
        f_st->sid = stat_id;
        f_st->hw_flags = 0; /* INTERNAL */
        f_st->stage_id = fg->stage_id;
        f_st->gid      = fg->gid;
        /* Allocate counters array. */
        switch(extended_stats->flex_stat_map) {
            case 1:
                nstat = 1;
                stat_arr[0] = bcmFieldStatBytes;
                LOG_VERBOSE(BSL_LS_BCM_FP,
                        (BSL_META_U(unit,
                                    "_field_flex_counter_recover:"
                                    " StatBytes \n")));
                break;
            case 2:
                nstat = 1;
                stat_arr[0] = bcmFieldStatPackets;
                LOG_VERBOSE(BSL_LS_BCM_FP,
                        (BSL_META_U(unit,
                                    "_field_flex_counter_recover:"
                                    " StatPackets \n")));
                break;
            case 3:
                LOG_VERBOSE(BSL_LS_BCM_FP,
                        (BSL_META_U(unit,
                                    "_field_flex_counter_recover: "
                                    " StatBytes & Packets\n")));
                break;
            default:
                LOG_WARN(BSL_LS_BCM_FP,
                        (BSL_META_U(unit,
                                    "_field_flex_counter_recover: "
                                    " Default Bytes&Pkts.\n")));
                break;
        }

        /* Get field control and stage control */
        rv = _field_control_get(unit, &fc);
        if (BCM_FAILURE(rv)) {
            sal_free(f_st);
            return (rv);
        }

        rv = _bcm_field_stat_array_init(unit, fc, f_st, nstat, stat_arr);
        if (BCM_FAILURE(rv)) {
            sal_free(f_st);
            return (rv);
        }
        f_st->hw_entry_count = 1;
        _FP_HASH_INSERT(fc->stat_hash, f_st,
                (stat_id & _FP_HASH_INDEX_MASK(fc)));
        fc->stat_count++;
    }

    f_ent_st = &f_ent->statistic;
    f_st->hw_ref_count++;
    f_st->sw_ref_count++;
    /* Associate the stat object with the entry */
    f_ent_st->flags |= _FP_ENTRY_EXTENDED_STAT_VALID;
    f_ent_st->flags |= _FP_ENTRY_EXTENDED_STAT_INSTALLED;
    f_ent_st->extended_sid = f_st->sid;
    f_ent_st->stat_action = action;

    if (!soc_feature(unit, soc_feature_advanced_flex_counter)) {
        return BCM_E_INTERNAL;
    }

    rv = _bcm_field_stat_get(unit, f_ent_st->extended_sid , &f_st);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (extended_stats->flex_mode != 0) {

        f_st->flex_mode = extended_stats->flex_mode;
        f_st->hw_flags = extended_stats->hw_flags;
        _bcm_esw_stat_get_counter_id_info(unit, f_st->flex_mode,
                &stat_mode,
                &stat_obj,
                (uint32 *)&offset_mode,
                &pool_num,
                &base_index
                );
        f_st->hw_index = base_index;
        f_st->pool_index = pool_num;
        f_st->hw_mode = offset_mode; /* offset_mode from counter info */
        f_st->hw_entry_count = 1; /* PlsNote:For SingleMode=1 */
        /* Currently OnlySingleMode is supportes so above OK */
        /* else decision will be based on stat_mode          */
    }

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* BCM_TRIDENT2PLUS_SUPPORT && BCM_FIELD_SUPPORT */
