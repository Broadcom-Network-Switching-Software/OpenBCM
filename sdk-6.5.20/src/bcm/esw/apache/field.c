/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field.c
 * Purpose:     BCM56560 Field Processor installation functions.
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#if defined(BCM_APACHE_SUPPORT) && defined(BCM_FIELD_SUPPORT)

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
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/nat.h>

#define ALIGN32(x)      (((x) + 31) & ~31)
#define BCM_MAX_SAT_MAC_SA_ENTRIES 15


/*
 * Condition Qualifier add for Monterey A0/B0 F3Bit0 WAR
 */
#define _FP_QUAL_COND_EXT_ADD(_unit_, _stage_fc_, _qual_id_,                 \
                         _dev_sel_, _dev_sel_val_,                           \
                         _pri_sel_, _pri_sel_val_,                           \
                         _sec_sel_, _sec_sel_val_,                           \
                         _intraslice_,                                       \
                         _offset_0_, _width_0_,                              \
                         _offset_1_, _width_1_,                              \
                         _offset_2_, _width_2_,                              \
                         _secondary_, _primary_)                             \
    if (soc_feature(unit, soc_feature_monterey_b0_fp_sw_war) &&              \
        !(_secondary_) && !(_intraslice_)) {                  \
        __FP_QUAL_EXT_ADD((_unit_), (_stage_fc_), (_qual_id_),               \
                          (_dev_sel_), (_dev_sel_val_),                      \
                          (_pri_sel_), (_pri_sel_val_),                      \
                          (_sec_sel_), (_sec_sel_val_),                      \
                          _bcmFieldDevSelDisable, 0,                         \
                          (_intraslice_),                                    \
                          (fp_global_mask_tcam_ofs + f3_rsvd_bit_offset), 1, \
                          (_offset_0_) + 1, (_width_0_) - 1,                 \
                          _offset_1_, _width_1_,                             \
                          (_secondary_),                                     \
                          (1));                                              \
    }                                                                        \
    __FP_QUAL_EXT_ADD((_unit_), (_stage_fc_), (_qual_id_),                   \
                      (_dev_sel_), (_dev_sel_val_),                          \
                      (_pri_sel_), (_pri_sel_val_),                          \
                      (_sec_sel_), (_sec_sel_val_),                          \
                      _bcmFieldDevSelDisable, 0,                             \
                      (_intraslice_),                                        \
                      _offset_0_, _width_0_,                                 \
                      _offset_1_, _width_1_,                                 \
                      _offset_2_, _width_2_,                                 \
                      ((soc_feature(unit, soc_feature_monterey_a0_fp_sw_war) \
                        || (_secondary_)) ? (1) : (_secondary_)),            \
                      (_primary_)                                            \
                      )

/*
 * Function:
 *     _field_apache_functions_init
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
void _field_apache_functions_init(_field_funct_t *functions)
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
    functions->fp_action_depends_check = _bcm_field_trx_action_depends_check;
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
    functions->fp_qualify_packet_res = _bcm_field_td2_qualify_PacketRes;
    functions->fp_qualify_packet_res_get = _bcm_field_td2_qualify_PacketRes_get;
    return;
}

/*
 * Function:
 *     _field_apache_ingress_qualifiers_init
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
_field_apache_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    const unsigned
        f2_offset = 10 - 2,
        fixed_offset = 138 - 2,
        fixed_pairing_overlay_offset = fixed_offset,
        fp_global_mask_tcam_ofs = ALIGN32(soc_mem_field_length(unit, FP_TCAMm, KEYf)),
        f3_offset = 2 - 2,
        f1_offset = 51 - 2,
        ipbm_pairing_f0_offset = 101 - 2,
        f3_rsvd_bit_offset = 168 - 2;

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
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGports,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f1_offset + 21, 16);
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
#if defined(BCM_MONTEREY_SUPPORT) && defined(BCM_PREEMPTION_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPreemptablePacket,
                     _bcmFieldSliceSelFpf1, 2,
                     fp_global_mask_tcam_ofs + f1_offset + 44, 1);
    }
#endif

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
#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
#if defined(BCM_PREEMPTION_SUPPORT)
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPreemptablePacket,
                     _bcmFieldSliceSelFpf1, 3,
                     fp_global_mask_tcam_ofs + f1_offset + 34, 1);
#endif
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRoeFrameType,
                     _bcmFieldSliceSelFpf1, 3,
                     fp_global_mask_tcam_ofs + f1_offset + 35, 2);
    }
#endif


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
    if (soc_feature(unit, soc_feature_fcoe)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                     _bcmFieldSliceSelFpf1, 4,
                     fp_global_mask_tcam_ofs + f1_offset + 32, 3);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                     _bcmFieldSliceSelFpf1, 4,
                     fp_global_mask_tcam_ofs + f1_offset + 35, 3);
    }
#if defined(BCM_MONTEREY_SUPPORT) && defined(BCM_PREEMPTION_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPreemptablePacket,
                     _bcmFieldSliceSelFpf1, 4,
                     fp_global_mask_tcam_ofs + f1_offset + 38, 1);
    }
#endif


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
#if defined(BCM_MONTEREY_SUPPORT) && defined(BCM_PREEMPTION_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPreemptablePacket,
                     _bcmFieldSliceSelFpf1, 5,
                     fp_global_mask_tcam_ofs + f1_offset + 44, 1);
    }
#endif


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
                 fp_global_mask_tcam_ofs + f1_offset + 36 , 14);

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
                 fp_global_mask_tcam_ofs + f1_offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGports,
                 _bcmFieldSliceSelFpf1, 11,
                 fp_global_mask_tcam_ofs + f1_offset, 16);
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

    if (soc_feature(unit, soc_feature_fp_based_oam)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOamType,
                _bcmFieldSliceSelFpf1, 13,
                fp_global_mask_tcam_ofs + f1_offset + 43, 1);
    }

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


    if (soc_feature(unit, soc_feature_fcoe)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeVersionIsZero,
                     _bcmFieldSliceSelFpf1, 16,
                     fp_global_mask_tcam_ofs + f1_offset, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeSOF,
                     _bcmFieldSliceSelFpf1, 16,
                     fp_global_mask_tcam_ofs + f1_offset + 1, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                     _bcmFieldSliceSelFpf1, 16,
                     fp_global_mask_tcam_ofs + f1_offset + 9, 3);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                     _bcmFieldSliceSelFpf1, 16,
                     fp_global_mask_tcam_ofs + f1_offset + 12, 3);
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
    }
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
                bcmFieldQualifyIngressDropEthernetOamControl,
                _bcmFieldSliceSelFpf2, 3,
                _bcmFieldSliceOamOverLayEnable,
                _bcmFieldOamOverlayEnable,
                f2_offset + 57, 7);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                bcmFieldQualifyIngressDropEthernetOamData,
                _bcmFieldSliceSelFpf2, 3,
                _bcmFieldSliceOamOverLayEnable,
                _bcmFieldOamOverlayEnable,
                f2_offset + 57, 7);
#if 0 /* Not supported in Apache A0.*/
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                bcmFieldQualifyIngressDropMplsOamData,
                _bcmFieldSliceSelFpf2, 3,
                _bcmFieldSliceOamOverLayEnable,
                _bcmFieldOamOverlayEnable,
                f2_offset + 57, 7);
#endif
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
#if 0 /* Not supported in Apache A0.*/
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                bcmFieldQualifyIngressDropMplsOamControl,
                _bcmFieldSliceSelFpf2, 3,
                _bcmFieldSliceOamOverLayEnable,
                _bcmFieldOamOverlayEnable,
                f2_offset + 54, 8);
#endif
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


    if (soc_feature(unit, soc_feature_fcoe)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeRxID,
                     _bcmFieldSliceSelFpf2, 12, f2_offset + 0, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeOxID,
                     _bcmFieldSliceSelFpf2, 12, f2_offset + 16, 16);
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
    }
/* FPF 3 */
/* Present in Global Mask Field */
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 0,
                          _bcmFieldSliceDstClassSelect, 0,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 10,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 0,
                          _bcmFieldSliceDstClassSelect, 1,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 10,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 0,
                          _bcmFieldSliceDstClassSelect, 2,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 10,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 0,
                          _bcmFieldSliceDstClassSelect, 3,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 10,
                          0, 0, 0, 0, 0, 0);

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



    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 1,
                          _bcmFieldSliceSrcClassSelect, 0,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 10,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 1,
                          _bcmFieldSliceSrcClassSelect, 1,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 10,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 1,
                          _bcmFieldSliceSrcClassSelect, 2,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 10,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 1,
                          _bcmFieldSliceSrcClassSelect, 3,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 10,
                          0, 0, 0, 0, 0, 0);



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
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTosLower4Bits,
                 _bcmFieldSliceSelFpf3, 1,
                 fp_global_mask_tcam_ofs + f3_offset + 45, 4);


    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);

    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstNivGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityNivGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstNivGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityNivGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMimGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVxlanGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVxlanGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroups,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 2,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMultipath,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);


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
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGports,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcGports,
                               _bcmFieldSliceSelFpf3, 2,
                                _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityCommonGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 2,
                 fp_global_mask_tcam_ofs + f3_offset + 37, 3);

    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 3,
                          _bcmFieldSliceSelDisable, 0,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 16,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 3,
                          _bcmFieldSliceSelDisable, 0,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 12,
                          0, 0, 0, 0, 0, 0);
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

    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 4,
                          _bcmFieldSliceSelDisable, 0,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 16,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 4,
                          _bcmFieldSliceSelDisable, 0,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 12,
                          0, 0, 0, 0, 0, 0);
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


    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 5,
                          _bcmFieldSliceSelDisable, 0,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 16,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 5,
                          _bcmFieldSliceSelDisable, 0,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 12,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 5,
                 fp_global_mask_tcam_ofs + f3_offset + 16, 16);
    if (soc_feature(unit, soc_feature_fcoe)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                     _bcmFieldSliceSelFpf3, 5,
                     fp_global_mask_tcam_ofs + f3_offset + 32, 3);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                     _bcmFieldSliceSelFpf3, 5,
                     fp_global_mask_tcam_ofs + f3_offset + 35, 3);
    }

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

    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 7,
                          _bcmFieldSliceSelDisable, 0,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 24,
                          0, 0, 0, 0, 0, 0);
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

    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 8,
                          _bcmFieldSliceSelDisable, 0,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 20,
                          0, 0, 0, 0, 0, 0);
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
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, _bcmFieldQualifyData3,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 9,
                          _bcmFieldSliceSelDisable, 0,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 32,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyGenericAssociatedChannelLabelValid,
                 _bcmFieldSliceSelFpf3, 9,
                 fp_global_mask_tcam_ofs + f3_offset + 32, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRouterAlertLabelValid,
                 _bcmFieldSliceSelFpf3, 9,
                 fp_global_mask_tcam_ofs + f3_offset + 33, 1);

    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);

    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstNivGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityNivGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstNivGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityNivGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMimGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVxlanGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVxlanGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroups,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 10,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMultipath,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf3, 10,
                 fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf3, 10,
                 fp_global_mask_tcam_ofs + f3_offset + 21, 16);
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_XFLOW_MACSEC)
    if (SOC_IS_MONTEREY(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacSecTagPresent,
                     _bcmFieldSliceSelFpf3, 10,
                     fp_global_mask_tcam_ofs + f3_offset + 37, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacSecTag,
                     _bcmFieldSliceSelFpf3, 10,
                     fp_global_mask_tcam_ofs + f3_offset + 38, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacSecDstMacRangeHit,
                     _bcmFieldSliceSelFpf3, 10,
                     fp_global_mask_tcam_ofs + f3_offset + 46, 1);
    }
#endif

    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 11,
                          _bcmFieldSliceSelDisable, 0,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 16,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 11,
                          _bcmFieldSliceSelDisable, 0,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 16,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 11,
                          _bcmFieldSliceSelDisable, 0,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 16,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 11,
                          _bcmFieldSliceSelDisable, 0,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 16,
                          0, 0, 0, 0, 0, 0);

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
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_XFLOW_MACSEC)
    if (SOC_IS_MONTEREY(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacSecTagPresent,
                     _bcmFieldSliceSelFpf3, 11,
                     fp_global_mask_tcam_ofs + f3_offset + 32, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacSecTag,
                     _bcmFieldSliceSelFpf3, 11,
                     fp_global_mask_tcam_ofs + f3_offset + 33, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacSecDstMacRangeHit,
                     _bcmFieldSliceSelFpf3, 11,
                     fp_global_mask_tcam_ofs + f3_offset + 41, 1);
    }
#endif

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWordValid,
                                 _bcmFieldSliceSelFpf3, 12,
                                 fp_global_mask_tcam_ofs + f3_offset + 36, 1);
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

    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 12,
                          _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagVn,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 33,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyCnTag,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 12,
                          _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagCn,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 33,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyFabricQueueTag,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 12,
                          _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagFabricQueue,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 33,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabel, 
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 12,
                          _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 33,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelTtl,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 12,
                          _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsFwdingLabel,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset,      8,
                          fp_global_mask_tcam_ofs + f3_offset + 32, 1,
                          0, 0, 0, 0);
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
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWord,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 12,
                          _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagMplsCntlWord,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 33,
                          0, 0, 0, 0, 0, 0);

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

    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashLower,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 12,
                          _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagRtag7A,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 16,
                          fp_global_mask_tcam_ofs + f3_offset + 32, 1,
                          0, 0,
                          0, 0);
 
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashLower,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 12,
                          _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagRtag7B,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 16,
                          fp_global_mask_tcam_ofs + f3_offset + 32, 1,
                          0, 0,
                          0, 0);

    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyVxlanNetworkId,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 12,
                          _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagVxlanOrLLTag,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 24,
                          0, 0, 0, 0, 0, 0);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanFlags,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagVxlanOrLLTag,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 8);

    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyVxlanHeaderBits8_31,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 12,
                          _bcmFieldSliceAuxTag2Select, _bcmFieldAuxTagVxlanReserved,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 24,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVxlanHeaderBits56_63,
                               _bcmFieldSliceSelFpf3, 12,
                               _bcmFieldSliceAuxTag2Select,
                               _bcmFieldAuxTagVxlanReserved,
                               fp_global_mask_tcam_ofs + f3_offset + 24, 8);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelAction,
                 _bcmFieldSliceSelFpf3, 12,
                 fp_global_mask_tcam_ofs + f3_offset + 33, 3
                 );
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyTos,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 13,
                          0,
                          _bcmFieldSliceTosClassSelect, 0,
                          fp_global_mask_tcam_ofs + f3_offset, 8,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 13,
                          0,
                          _bcmFieldSliceTosClassSelect, 0,
                          fp_global_mask_tcam_ofs + f3_offset, 8,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 13,
                          0,
                          _bcmFieldSliceTosClassSelect, 1,
                          fp_global_mask_tcam_ofs + f3_offset, 8,
                          0, 0, 0, 0, 0, 0);

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
    
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 14,
                          0,
                          _bcmFieldSliceSelDisable, 0,
                          fp_global_mask_tcam_ofs + f3_offset, 48,
                          0, 0, 0, 0, 0, 0);

    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityGlp,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMplsGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);

    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstNivGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityNivGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstNivGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityNivGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMimGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMimGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVxlanGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVxlanGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityVlanGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityCommonGport,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityL3Egress,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroups,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMulticastGroup,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
    _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                          _bcmFieldDevSelDisable, 0,
                          _bcmFieldSliceSelFpf3, 15,
                          _bcmFieldSliceDstFwdEntitySelect, _bcmFieldFwdEntityMultipath,
                          0,
                          fp_global_mask_tcam_ofs + f3_offset, 21,
                          0, 0, 0, 0, 0, 0);
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
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGports,
                               _bcmFieldSliceSelFpf3, 15,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport,
                               fp_global_mask_tcam_ofs + f3_offset + 21, 16);
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
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 15,
                 fp_global_mask_tcam_ofs + f3_offset + 37, 12);



    if (soc_feature(unit, soc_feature_fcoe)) {
        _FP_QUAL_COND_EXT_ADD(unit, stage_fc, bcmFieldQualifyFcoeVersionIsZero,
                              _bcmFieldDevSelDisable, 0,
                              _bcmFieldSliceSelFpf3, 16,
                              _bcmFieldSliceSelDisable, 0,
                              0,
                              fp_global_mask_tcam_ofs + f3_offset, 1,
                              0, 0, 0, 0, 0, 0);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeSOF,
                     _bcmFieldSliceSelFpf3, 16,
                     fp_global_mask_tcam_ofs + f3_offset + 1, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                     _bcmFieldSliceSelFpf3, 16,
                     fp_global_mask_tcam_ofs + f3_offset + 9, 3);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                     _bcmFieldSliceSelFpf3, 16,
                     fp_global_mask_tcam_ofs + f3_offset + 12, 3);
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
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_XFLOW_MACSEC)
    if (SOC_IS_MONTEREY(unit)) {
        _FP_QUAL_PRIMARY_ADD(unit, stage_fc, bcmFieldQualifyMacSecTagPresent,
                             _bcmFieldSliceSelDisable, 0, fixed_offset + 24, 1);
    }
#endif
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
 *     _field_apache_lookup_qualifiers_init
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
_field_apache_lookup_qualifiers_init(int unit, _field_stage_t *stage_fc)
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

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 56, 1);
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_XFLOW_MACSEC)
    if (SOC_IS_MONTEREY(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFromMacSecPort,
                     _bcmFieldSliceSelFpf1, 0, f1_offset + 57, 1);
    }
#endif


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

    if (soc_feature(unit, soc_feature_fcoe)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                     _bcmFieldSliceSelFpf2, 10, f2_offset, 3);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                     _bcmFieldSliceSelFpf2, 10, f2_offset + 3, 6);
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
    }

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
    if (soc_feature(unit, soc_feature_fcoe)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTHopCount,
                     _bcmFieldSliceSelFpf3, 5, f3_offset, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVsanId,
                     _bcmFieldSliceSelFpf3, 5, f3_offset + 8, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVsanPri,
                     _bcmFieldSliceSelFpf3, 5, f3_offset + 20, 3);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVersion,
                     _bcmFieldSliceSelFpf3, 5, f3_offset + 23, 2);

        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTHopCount,
                     _bcmFieldSliceSelFpf3, 6, f3_offset, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVsanId,
                     _bcmFieldSliceSelFpf3, 6, f3_offset + 8, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVsanPri,
                     _bcmFieldSliceSelFpf3, 6, f3_offset + 20, 3);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVersion,
                 _bcmFieldSliceSelFpf3, 6, f3_offset + 23, 2);
    }

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
 *     _field_apache_egress_qualifiers_init
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
_field_apache_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_DECL;
    _key_fld_ = KEYf;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    if (soc_feature(unit, soc_feature_fp_based_oam)) {
    /* Initialze ETH_OAM type  and SAT FLPDU type to corresponding class id's */
    BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                EGR_OAM_SAT_MISC_ETHERTYPE_0r, REG_PORT_ANY,
                ETHERTYPEf, _FP_ETHOAM_ETHER_TYPE));
    BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                EGR_OAM_SAT_MISC_ETHERTYPE_0r, REG_PORT_ANY,
                CLASS_IDf, _FP_EGRESS_OAM_CLASS_ID));
    BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                EGR_OAM_SAT_MISC_ETHERTYPE_1r, REG_PORT_ANY,
                ETHERTYPEf, _FP_SAT_ETHER_TYPE));
    BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                EGR_OAM_SAT_MISC_ETHERTYPE_1r, REG_PORT_ANY,
                CLASS_IDf, _FP_EGRESS_SAT_CLASS_ID));
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
                               1, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGports,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGports,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGports,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 15);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGports,
                              _bcmFieldDevSelDisable, 0,
                              _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                              _bcmFieldSliceSelEgrDvpKey4, 0,
                              0,
                              1, 15,
                              227, 2, /* DVP TYPE */
                              0, 0,
                              0
                              );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGports,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 15);
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_XFLOW_MACSEC)
    if (SOC_IS_MONTEREY(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacSecFlow,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 16, 2);
    }
#endif
#if 0 /* Apache HW doesn't support DvpKey4 1 and 2 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 1,
                               2, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 2,
                               2, 12);
#endif

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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
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
                               193, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 1,
                               185, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 1,
                               193, 7);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 2,
                               185, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 2,
                               193, 6);
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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 4,
                               185, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 4,
                               193, 8);

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
#if defined(BCM_MONTEREY_SUPPORT) && defined(BCM_PREEMPTION_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPreemptablePacket,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 229, 1);
    }
#endif

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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 3,
                               197, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
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
#if defined(BCM_MONTEREY_SUPPORT) && defined(BCM_PREEMPTION_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPreemptablePacket,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 226, 1);
    }
#endif
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
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4,
                               _bcmFieldSliceSelEgrDestPortF1, 1,
                               0,
                               185, 7, /* DST_PORT */
                               177, 8, /* DST_MODID */
                               0, 0,
                               0
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
#if defined(BCM_MONTEREY_SUPPORT) && defined(BCM_PREEMPTION_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPreemptablePacket,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1_NO_V4, 226, 1);
    }
#endif

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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               187, 8);
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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 3,
                               187, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 3,
                               195, 7
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 4,
                               187, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 4,
                               195, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 4,
                               195, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 4,
                               195, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 4,
                               195, 12
                               );
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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 6,
                               187, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 6,
                               195, 8);
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
#if defined(BCM_MONTEREY_SUPPORT) && defined(BCM_PREEMPTION_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPreemptablePacket,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 229, 1);
    }
#endif
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
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 204, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 204, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 204, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 216, 1);
#if defined(BCM_MONTEREY_SUPPORT) && defined(BCM_PREEMPTION_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPreemptablePacket,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 217, 1);
    }
#endif
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 0);


    /* FCOE EFP_KEY5 */
    if (soc_feature(unit, soc_feature_fcoe)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTFabricId,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 1, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTHopCount,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 13, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTValid,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 21, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanRCtl,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 22, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDFCtl,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 30, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanFCtl,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 38, 24);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanType,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 62, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstId,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 70, 24);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcId,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 94, 24);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanCSCtl,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 118, 8);
    }
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
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrDestPortF5, 0,
                               168, 8
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrDestPortF5, 0,
                               176, 7
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrDestPortF5, 1,
                               0,
                               176, 7, /* DST_PORT */
                               168, 8, /* DST_MODID */
                               0, 0,
                               0
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 183, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 184, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 184, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 184, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 198, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 200, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 200, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 200, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 200, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 200, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 200, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 200, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 200, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 200, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 200, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressInterfaceClassVPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 200, 13);
    if (soc_feature(unit, soc_feature_fcoe)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTPri,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 213, 3);
    }
#if defined(BCM_MONTEREY_SUPPORT) && defined(BCM_PREEMPTION_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPreemptablePacket,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 216, 1);
    }
#endif

    if (soc_feature(unit, soc_feature_fp_based_oam)) {
    /* OAM_SAT EFP_KEY6 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOamSat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 0, 0);
    /* EG_DVP_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               1, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               1, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               1, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               1, 12);
    /* DVP_VALID + DVP + SPARE_DVP */
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               0,
                               0, 1,   /* DVP_VALID */
                               13, 14, /* DVP + SPARE_DVP */
                               0, 0,
                               0
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGports,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               0,
                               0, 1,   /* DVP_VALID */
                               13, 14, /* DVP + SPARE_DVP */
                               0, 0,
                               0
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstNivGport,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               0,
                               0, 1,   /* DVP_VALID */
                               13, 14, /* DVP + SPARE_DVP */
                               0, 0,
                               0
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstNivGports,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               0,
                               0, 1,   /* DVP_VALID */
                               13, 14, /* DVP + SPARE_DVP */
                               0, 0,
                               0
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               0,
                               0, 1,   /* DVP_VALID */
                               13, 14, /* DVP + SPARE_DVP */
                               0, 0,
                               0
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMimGports,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               0,
                               0, 1,   /* DVP_VALID */
                               13, 14, /* DVP + SPARE_DVP */
                               0, 0,
                               0
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               0,
                               0, 1,   /* DVP_VALID */
                               13, 14, /* DVP + SPARE_DVP */
                               0, 0,
                               0
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGports,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               0,
                               0, 1,   /* DVP_VALID */
                               13, 14, /* DVP + SPARE_DVP */
                               0, 0,
                               0
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               0,
                               0, 1,   /* DVP_VALID */
                               13, 14, /* DVP + SPARE_DVP */
                               0, 0,
                               0
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGports,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               0,
                               0, 1,   /* DVP_VALID */
                               13, 14, /* DVP + SPARE_DVP */
                               0, 0,
                               0
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               0,
                               0, 1,   /* DVP_VALID */
                               13, 14, /* DVP + SPARE_DVP */
                               0, 0,
                               0
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               0,
                               0, 1,   /* DVP_VALID */
                               13, 14, /* DVP + SPARE_DVP */
                               0, 0,
                               0
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOamEgressPortUnicastDstMacHit,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               28, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               29, 4);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               33, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyOamHeaderBits32_63,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               35, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyOamHeaderBits0_31,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               67, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOamEgressClass2Vxlt,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               99, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOamEgressClassVxlt,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               111, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyOamEgressClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               123, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               135, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOamEgressVxltSecondHit,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               143, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOamEgressVxltFirstHit,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               144, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOamEgressMulticastMacHit,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               145, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               147, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               147, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               159, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               160, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               163, 7);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               170, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               170, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               182, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               183, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               186, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOamDownMEPLoopbackPacket,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               188, 1);
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               189, 8);
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               197, 13
                               );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               197, 13
                               );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               197, 13
                               );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               197, 13
                               );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               197, 13
                               );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               197, 13
                               );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               197, 13
                               );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               197, 13
                               );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               197, 13
                               );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               197, 13
                               );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               197, 13
                               );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               197, 12
                               );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 1,
                               189, 8);
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 1,
                               197, 7);
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 2,
                               189, 8);
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 2,
                               197, 6);
    __FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 3,
                               0,
                               197, 7, /* DST_PORT */
                               189, 8, /* DST_MODID */
                               0, 0,
                               0, 0
                               );
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 4,
                               189, 8);
    _FP_QUAL_THREE_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               _bcmFieldSliceSelEgrClassF6, 4,
                               197, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOamEgressEtherType,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               224, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOamEgressClassSrcMac,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrOamOverlayKey4, 2,
                               226, 4);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_apache_qualifiers_init
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
_field_apache_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
            return (_field_apache_lookup_qualifiers_init(unit, stage_fc));
        case _BCM_FIELD_STAGE_INGRESS:
            return (_field_apache_ingress_qualifiers_init(unit, stage_fc));
        case _BCM_FIELD_STAGE_EGRESS:
            return (_field_apache_egress_qualifiers_init(unit, stage_fc));
        default:
            ;
    }

    sal_free(stage_fc->f_qual_arr);
    return (BCM_E_INTERNAL);
}

/*
 * Function:
 *     _bcm_field_apache_oam_drop_hw_encode
 * Purpose:
 *     Encode data and mask based on the OAM MEP type
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_id   - (IN) Field Processor stage id
 *     qual_id    - (IN) Field Processor qualifier id
 *     mep_type   - (IN) OAM Drop MEP type
 *     data       - (OUT) Encoded data
 *     mask       - (OUT) Encoded mask
 *
 * Returns:
 *     BCM_E_XXXX
 * Notes:
 */
int
_bcm_field_apache_oam_drop_hw_encode(int unit,
        _field_stage_id_t stage_id,
        int qual_id,
        bcm_field_oam_drop_mep_type_t mep_type,
        uint8 *data, uint8 *mask)
{
    uint8     mask_bits;
    int       rv;

    if ((data == NULL) || (mask == NULL)) {
        return BCM_E_PARAM;
    }

    if (stage_id == _BCM_FIELD_STAGE_INGRESS) {
        if (qual_id == bcmFieldQualifyIngressDropEthernetOamControl) {
            switch (mep_type) {
                case bcmFieldOamDropPortDownMepControl:
                case bcmFieldOamDropInnerVlanDownMepControl:
                case bcmFieldOamDropOuterVlanDownMepControl:
                case bcmFieldOamDropInnerPlusOuterVlanDownMepControl:
                    mask_bits = 7;
                    rv = _bcm_oam_drop_control_fp_encode(unit, mep_type, data, mask);
                    *mask = *mask & ((1 << mask_bits) - 1);
                    return rv;
                    break;
                default :
                    break;
            }
        } else if (qual_id == bcmFieldQualifyIngressDropEthernetOamData) {
            switch (mep_type) {
                case bcmFieldOamDropPortDownMepData:
                case bcmFieldOamDropInnerVlanDownMepData:
                case bcmFieldOamDropOuterVlanDownMepData:
                case bcmFieldOamDropInnerPlusOuterVlanDownMepData:
                case bcmFieldOamDropInnerVlanUpMepData:
                case bcmFieldOamDropOuterVlanUpMepData:
                case bcmFieldOamDropInnerPlusOuterVlanUpMepData:
                    mask_bits = 7;
                    rv = _bcm_oam_drop_control_fp_encode(unit, mep_type, data, mask);
                    *mask = *mask & ((1 << mask_bits) - 1);
                    return rv;
                    break;
                default :
                    break;
            }
        }else if (qual_id == bcmFieldQualifyIngressDropMplsOamControl) {
            switch (mep_type) {
                case bcmFieldOamDropSectionPortMepControl:
                case bcmFieldOamDropSectionInnerVlanMepControl:
                case bcmFieldOamDropSectionOuterVlanMepControl:
                case bcmFieldOamDropSectionInnerPlusOuterVlanMepControl:
                case bcmFieldOamDropLSPMepControl:
                case bcmFieldOamDropPwMepControl:
                    mask_bits = 8;
                    rv = _bcm_oam_drop_control_fp_encode(unit, mep_type, data, mask);
                    *mask = *mask & ((1 << mask_bits) - 1);
                    return rv;
                    break;
                default :
                    break;
            }
        } else if (qual_id == bcmFieldQualifyIngressDropMplsOamData) {
            switch (mep_type) {
                case bcmFieldOamDropSectionPortMepData:
                case bcmFieldOamDropSectionInnerVlanMepData:
                case bcmFieldOamDropSectionOuterVlanMepData:
                case bcmFieldOamDropSectionInnerPlusOuterVlanMepData:
                case bcmFieldOamDropLSPMepData:
                case bcmFieldOamDropPwMepData:
                    mask_bits = 7;
                    rv = _bcm_oam_drop_control_fp_encode(unit, mep_type, data, mask);
                    *mask = *mask & ((1 << mask_bits) - 1);
                    return rv;
                    break;
                default :
                    break;
            }
        }
    }

    return BCM_E_PARAM;
}

/*
 * Function:
 *      _bcm_field_apache_qualify_OamDropReason
 * Purpose:
 *      Set match criteria to qualify on Drop reason for
 *      a given entry.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      entry     - (IN) BCM field entry id.
 *      qual_id   - (IN) BCM field qualifier id.
 *      mep_type  - (IN) Qualifier Mep Type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_field_apache_qualify_OamDropReason(
    int unit,
    bcm_field_entry_t entry,
    int qual_id,
    bcm_field_oam_drop_mep_type_t mep_type)
{
    int                 rv;
    _field_entry_t      *f_ent = NULL;
    uint8               data;
    uint8               mask;

    /* Search the entry by eid. */
    BCM_IF_ERROR_RETURN(_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent));

    if (f_ent->group == NULL) {
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN(_bcm_field_apache_oam_drop_hw_encode(unit,
                f_ent->group->stage_id,
                qual_id, mep_type, &data, &mask));

    rv = _field_qualify32(unit, entry, qual_id,
                          data, mask);

    /* Update the Software Database*/
    if (BCM_SUCCESS(rv)) {
        switch(qual_id) {
         case bcmFieldQualifyIngressDropEthernetOamControl:
             f_ent->eth_oam_ctrl_mep_type = mep_type;
             break;
         case bcmFieldQualifyIngressDropEthernetOamData:
             f_ent->eth_oam_data_mep_type = mep_type;
             break;
         case bcmFieldQualifyIngressDropMplsOamControl:
             f_ent->mpls_oam_ctrl_mep_type = mep_type;
             break;
         case bcmFieldQualifyIngressDropMplsOamData:
             f_ent->mpls_oam_data_mep_type = mep_type;
             break;
         default :
             break;
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_field_apache_qualify_OamDropReason_get
 * Purpose:
 *      Get qualifier Drop reason for a given entry.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      entry     - (IN) BCM field entry id.
 *      qual_id   - (IN) BCM field qualifier id.
 *      mep_type  - (OUT) Qualifier Mep Type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_field_apache_qualify_OamDropReason_get(
    int unit,
    bcm_field_entry_t entry,
    int qual_id,
    bcm_field_oam_drop_mep_type_t *mep_type)
{
    int                 rv;
    _field_entry_t      *f_ent = NULL;
    uint8               data;
    uint8               mask;

    if (mep_type == NULL) {
        return BCM_E_PARAM;
    }

    /* Read qualifier match value and mask. */
    BCM_IF_ERROR_RETURN(_bcm_field_entry_qualifier_uint8_get(unit, entry,
                qual_id, &data, &mask));

    if (mask == 0) {
        return BCM_E_NOT_FOUND;
    }

    FP_LOCK(unit);

    /* Search the entry by eid. */
    rv = _field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent);

    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return rv;
    }

    switch(qual_id) {
        case bcmFieldQualifyIngressDropEthernetOamControl:
            *mep_type = f_ent->eth_oam_ctrl_mep_type;
            break;
        case bcmFieldQualifyIngressDropEthernetOamData:
            *mep_type = f_ent->eth_oam_data_mep_type;
            break;
        case bcmFieldQualifyIngressDropMplsOamControl:
            *mep_type = f_ent->mpls_oam_ctrl_mep_type;
            break;
        case bcmFieldQualifyIngressDropMplsOamData:
            *mep_type = f_ent->mpls_oam_data_mep_type;
            break;
        default :
            *mep_type = 0;
            FP_UNLOCK(unit);
            return BCM_E_INTERNAL;
            break;
    }

    FP_UNLOCK(unit);

    if (*mep_type == 0) {
        return BCM_E_NOT_FOUND;
    }

    return rv;
}

int
_bcm_field_apache_qualify_OamDropReason_delete(
    int unit,
    bcm_field_entry_t entry,
    int qual_id)
{
    _field_entry_t      *f_ent = NULL;

    if(!((qual_id == bcmFieldQualifyIngressDropEthernetOamControl) ||
            (qual_id == bcmFieldQualifyIngressDropEthernetOamData) ||
            (qual_id == bcmFieldQualifyIngressDropMplsOamControl) ||
            (qual_id == bcmFieldQualifyIngressDropMplsOamData))) {
        return BCM_E_NONE;
    }

    /* Search the entry by eid. */
    BCM_IF_ERROR_RETURN(_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent));

    switch(qual_id) {
        case bcmFieldQualifyIngressDropEthernetOamControl:
            f_ent->eth_oam_ctrl_mep_type = 0;
            break;
        case bcmFieldQualifyIngressDropEthernetOamData:
            f_ent->eth_oam_data_mep_type = 0;
            break;
        case bcmFieldQualifyIngressDropMplsOamControl:
            f_ent->mpls_oam_ctrl_mep_type = 0;
            break;
        case bcmFieldQualifyIngressDropMplsOamData:
            f_ent->mpls_oam_data_mep_type = 0;
            break;
            /* coverity[dead_error_begin] */
        default :
            break;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_apache_sat_macsa_add
 * Purpose:
 *      Add a mac sa and class id entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mac -  (IN) Src Mac.
 *      class_id - (IN) Class id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_field_apache_sat_macsa_add(
    int unit,
    bcm_mac_t mac,
    uint32 class_id)
{
   int index;
   uint64 reg64,temp_mac;
   uint32 class_id_get;
   int rv = BCM_E_NONE;

   COMPILER_64_ZERO(reg64);
   COMPILER_64_ZERO(temp_mac);

   if (class_id <= 0 || class_id > BCM_MAX_SAT_MAC_SA_ENTRIES) {
       return BCM_E_PARAM;
   }
   rv = _bcm_field_apache_sat_macsa_get(unit, mac, &class_id_get, &index);
   if (rv == BCM_E_NONE) {
       return BCM_E_EXISTS;
   }
   SAL_MAC_ADDR_TO_UINT64(mac, temp_mac);
   if (index < BCM_MAX_SAT_MAC_SA_ENTRIES) {
       soc_reg64_field_set(unit, EGR_OAM_SAT_MISC_MAC_SAr, &reg64,
                    MAC_SAf, temp_mac);
       soc_reg64_field32_set(unit, EGR_OAM_SAT_MISC_MAC_SAr, &reg64,
                    MAC_SA_CLASS_IDf, class_id);
       rv = WRITE_EGR_OAM_SAT_MISC_MAC_SAr(unit, index, reg64);
       if (BCM_FAILURE(rv)) {
           return rv;
       }
       return BCM_E_NONE;
   }
   return BCM_E_FULL;
}

/*
 * Function:
 *      _bcm_field_apache_sat_macsa_get
 * Purpose:
 *      Get class id corresponding to mac sa entry.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      maci     - (IN) Src Mac.
 *      class_id - (OUT)Class id.
 *      idx      - (OUT)index
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_field_apache_sat_macsa_get(
    int unit,
    bcm_mac_t mac,
    uint32 *class_id,
    int *idx)
{
   int index, empty = 0, first_void = 0;
   uint64 reg64, temp_mac, fetch_mac, zero64;
   int rv = BCM_E_NONE;

   *idx = -1;
   COMPILER_64_ZERO(reg64);
   COMPILER_64_ZERO(temp_mac);
   COMPILER_64_ZERO(fetch_mac);
   COMPILER_64_ZERO(zero64);

   SAL_MAC_ADDR_TO_UINT64(mac, temp_mac);
   for (index = 0; index < BCM_MAX_SAT_MAC_SA_ENTRIES; index++) {
        rv = READ_EGR_OAM_SAT_MISC_MAC_SAr(unit, index, &reg64);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        fetch_mac = soc_reg64_field_get(unit, EGR_OAM_SAT_MISC_MAC_SAr,
                       reg64, MAC_SAf);
        if (COMPILER_64_EQ(fetch_mac, zero64) && first_void == 0) {
            *idx = index;
            first_void = 1;
        } else if (COMPILER_64_NE(fetch_mac, zero64)) {
            if (COMPILER_64_EQ(fetch_mac, temp_mac)) {
                *class_id = soc_reg64_field32_get(unit, EGR_OAM_SAT_MISC_MAC_SAr,
                                reg64,  MAC_SA_CLASS_IDf);
                *idx = index;
                return BCM_E_NONE;
            } else {
                empty = 1;
            }
        }
   }
   if (*idx == -1) {
       *idx = index;
   }
   *class_id = 0;
   if (empty == 0) {
       return BCM_E_EMPTY;
   }
   return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_field_apache_sat_macsa_delete
 * Purpose:
 *      Delete mac sa and class id entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mac -  (IN) Src Mac.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_field_apache_sat_macsa_delete(
    int unit,
    bcm_mac_t mac)
{
   int index;
   uint64 reg64, zero64;
   uint32 class_id_get;
   int rv = BCM_E_NONE;

   COMPILER_64_ZERO(reg64);
   COMPILER_64_ZERO(zero64);

   rv = _bcm_field_apache_sat_macsa_get(unit, mac, &class_id_get, &index);
   if (rv == BCM_E_NONE) {
       soc_reg64_field_set(unit, EGR_OAM_SAT_MISC_MAC_SAr, &reg64,
                    MAC_SAf, zero64);
       soc_reg64_field32_set(unit, EGR_OAM_SAT_MISC_MAC_SAr, &reg64,
                    MAC_SA_CLASS_IDf, 0);
       rv = WRITE_EGR_OAM_SAT_MISC_MAC_SAr(unit, index, reg64);
   }
   return rv;
}

/*
 * Function:
 *      _bcm_field_apache_sat_macsa_delete_all
 * Purpose:
 *      Delete all mac sa and class id entries.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_field_apache_sat_macsa_delete_all(int unit)
{
   int index;
   uint64 reg64, zero64;
   int rv = BCM_E_NONE;

   COMPILER_64_ZERO(reg64);
   COMPILER_64_ZERO(zero64);

   for (index = 0; index < BCM_MAX_SAT_MAC_SA_ENTRIES; index++) {
        soc_reg64_field_set(unit, EGR_OAM_SAT_MISC_MAC_SAr, &reg64,
                     MAC_SAf, zero64);
        soc_reg64_field32_set(unit, EGR_OAM_SAT_MISC_MAC_SAr, &reg64,
                     MAC_SA_CLASS_IDf, 0);
        rv = WRITE_EGR_OAM_SAT_MISC_MAC_SAr(unit, index, reg64);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
   }
   return BCM_E_NONE;
}

#if defined BCM_WARM_BOOT_SUPPORT
int
_field_apache_entry_info_sync(int unit,
                               _field_control_t  *fc,
                               _field_stage_t    *stage_fc,
                               _field_entry_t    *f_ent
                               )
{
    _field_action_t     *redirect_action = NULL;
    _field_action_t     *fa = NULL;
    uint8               action_flags = 0;
    uint8               statobject = 0;
    uint8               *buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];

    if ( (stage_fc == NULL) || (f_ent == NULL)) {
        return BCM_E_INTERNAL;
    }

    /* Save Ingress OAM Drop MEP Types */
    if (_BCM_FIELD_STAGE_INGRESS == f_ent->group->stage_id) {
        buf[fc->scache_pos] = f_ent->eth_oam_ctrl_mep_type;
        fc->scache_pos++;
        buf[fc->scache_pos] = f_ent->eth_oam_data_mep_type;
        fc->scache_pos++;
        buf[fc->scache_pos] = f_ent->mpls_oam_ctrl_mep_type;
        fc->scache_pos++;
        buf[fc->scache_pos] = f_ent->mpls_oam_data_mep_type;
        fc->scache_pos++;
    }

    /* Save COPYTOCPU, Redirect & StatObjSessionId action values */
    if ((_BCM_FIELD_STAGE_INGRESS == f_ent->group->stage_id) ||
            (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id)) {

        fa = f_ent->actions;

        while (fa !=NULL) {
            if ((fa->action == bcmFieldActionRedirect) &&
                    (fa->param[2])){
                action_flags |= _FIELD_WB_ENTRY_ACTION_REDIRECT;
                if ((fa->param[3] == bcmFieldRedirectDestinationTrunk) &&
                        !BCM_GPORT_IS_TRUNK(fa->param[4])) {
                    action_flags |= _FIELD_WB_ENTRY_ACTION_REDIRECT_TRUNK;
                }
                if (fa->param[2] & BCM_FIELD_REDIRECT_SOURCE_USE_CONFIGURED) {
                    action_flags |= _FIELD_WB_ENTRY_ACTION_REDIRECT_SOURCE_GPORT;
                }
                redirect_action = fa;
            } else if ((fa->action == bcmFieldActionCopyToCpu) &&
                    (fa->param[2])){
                action_flags |= _FIELD_WB_ENTRY_ACTION_COPYTOCPU;
            } else if (fa->action == bcmFieldActionOamStatObjectSessionId) {
                statobject = fa->param[0];
                action_flags |= _FIELD_WB_ENTRY_ACTION_OAM_STATOBJECT_SESSION_ID;
            }
            fa = fa->next;
        }

        buf[fc->scache_pos] = action_flags;
        fc->scache_pos++;

        /* Write bcm_stat_object_t value */
        if (action_flags &
                _FIELD_WB_ENTRY_ACTION_OAM_STATOBJECT_SESSION_ID) {
            buf[fc->scache_pos] = statobject;
            fc->scache_pos++;
        }

        /* Write source gport type encoding values */
        if ((action_flags & _FIELD_WB_ENTRY_ACTION_REDIRECT_SOURCE_GPORT) &&
                redirect_action) {
            buf[fc->scache_pos] = 0;
            if (BCM_GPORT_IS_MPLS_PORT(redirect_action->param[5])) {
                buf[fc->scache_pos] = 1;
            } else if (BCM_GPORT_IS_NIV_PORT(redirect_action->param[5])) {
                buf[fc->scache_pos] = 2;
            } else if (BCM_GPORT_IS_MIM_PORT(redirect_action->param[5])) {
                buf[fc->scache_pos] = 3;
            } else if (BCM_GPORT_IS_TRILL_PORT(redirect_action->param[5])) {
                buf[fc->scache_pos] = 4;
            } else if (BCM_GPORT_IS_L2GRE_PORT(redirect_action->param[5])) {
                buf[fc->scache_pos] = 5;
            } else if (BCM_GPORT_IS_VXLAN_PORT(redirect_action->param[5])) {
                buf[fc->scache_pos] = 6;
            }
            fc->scache_pos++;
        }
    }
    return BCM_E_NONE;
}

int
_field_apache_entry_info_recovery(int                 unit,
                               _field_control_t       *fc,
                               _field_stage_t         *stage_fc,
                               _field_entry_wb_info_t *f_ent_wb_info
                               )
{
    uint8 *buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    uint8     action_flags;

    if ((f_ent_wb_info == NULL) || (stage_fc == NULL)) {
        return BCM_E_INTERNAL;
    }

    /* Retrieve OAM MEP type */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        f_ent_wb_info->ingress_oam_mep_type[0] = buf[fc->scache_pos];
        fc->scache_pos++;
        f_ent_wb_info->ingress_oam_mep_type[1] = buf[fc->scache_pos];
        fc->scache_pos++;
        f_ent_wb_info->ingress_oam_mep_type[2] = buf[fc->scache_pos];
        fc->scache_pos++;
        f_ent_wb_info->ingress_oam_mep_type[3] = buf[fc->scache_pos];
        fc->scache_pos++;
    }

    /* Retrieve COPYTOCPU, Redirect & StatObjectSessionId params */
    if ((_BCM_FIELD_STAGE_INGRESS == stage_fc->stage_id) ||
            (_BCM_FIELD_STAGE_EGRESS == stage_fc->stage_id)) {

        action_flags = buf[fc->scache_pos];
        fc->scache_pos++;

        /* Retrieve StatObjectSessionId params */
        if (action_flags &
                _FIELD_WB_ENTRY_ACTION_OAM_STATOBJECT_SESSION_ID) {
            f_ent_wb_info->action_StatObjSId_wb.valid = 1;
            f_ent_wb_info->action_StatObjSId_wb.stat_object = buf[fc->scache_pos];
            fc->scache_pos++;
        }

        /* Retrieve COPYTOCPU params */
        if (action_flags &
                _FIELD_WB_ENTRY_ACTION_COPYTOCPU) {
            f_ent_wb_info->action_copytocpu_wb.valid = 1;
        }

        /* Retrieve Redirect params */
        if (action_flags &
                _FIELD_WB_ENTRY_ACTION_REDIRECT) {
            f_ent_wb_info->action_redir_wb.valid = 1;

            if (action_flags &
                    _FIELD_WB_ENTRY_ACTION_REDIRECT_TRUNK) {
                f_ent_wb_info->action_redir_wb.valid = 3;
            }

            f_ent_wb_info->action_redir_wb.params.destination = BCM_GPORT_INVALID;
            f_ent_wb_info->action_redir_wb.params.source_port = BCM_GPORT_INVALID;
            /* Retrieve Redirect source port */
            if (action_flags &
                    _FIELD_WB_ENTRY_ACTION_REDIRECT_SOURCE_GPORT) {
                f_ent_wb_info->action_redir_wb.params.source_port = buf[fc->scache_pos];
                fc->scache_pos++;
            }
        }
    }
    return BCM_E_NONE;
}

int
_field_apache_actions_recovery(int                    unit,
                               soc_mem_t              policy_mem,
                               uint32                 *policy_entry,
                               _field_entry_t         *f_ent,
                               _field_entry_wb_info_t *f_ent_wb_info
                               )
{
    uint32 param0, param1, param2 = 0, param3 = 0, param4 = 0, param5 = 0;
    uint32 hw_index, append;
    bcm_field_action_t action;          /* Action type */
    int rv = BCM_E_NONE;

    if ((f_ent == NULL) || (f_ent_wb_info == NULL) ||
        (policy_entry == NULL)) {
        return BCM_E_INTERNAL;
    }

    if (f_ent_wb_info->action_copytocpu_wb.valid) {
        f_ent_wb_info->action_copytocpu_wb.valid = 0;
        action = param0 = param1 = param2 = param3 = param4 = param5 = hw_index = append = 0;
        action = bcmFieldActionCopyToCpu;
        param2 = f_ent_wb_info->action_copytocpu_wb.params.flags;
        if (SOC_MEM_FIELD_VALID(unit, policy_mem, MATCHED_RULEf)) {
            param3 = PolicyGet(unit, policy_mem, policy_entry, MATCHED_RULEf);
        }
        rv = _field_trx_actions_recover_action_add(unit,
                f_ent,
                action,
                param0,
                param1,
                param2,
                param3,
                param4,
                param5,
                hw_index
                );
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    if (f_ent_wb_info->action_redir_wb.valid) {
        f_ent_wb_info->action_redir_wb.valid = 0;
        action = param0 = param1 = param2 = param3 = param4 = param5 = hw_index = append = 0;
        action = bcmFieldActionRedirect;
        param2 = f_ent_wb_info->action_redir_wb.params.flags;
        param3 = f_ent_wb_info->action_redir_wb.params.destination_type;
        param4 = f_ent_wb_info->action_redir_wb.params.destination;
        param5 = f_ent_wb_info->action_redir_wb.params.source_port;
        rv = _field_trx_actions_recover_action_add(unit,
                f_ent,
                action,
                param0,
                param1,
                param2,
                param3,
                param4,
                param5,
                hw_index
                );
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* BCM_APACHE_SUPPORT && BCM_FIELD_SUPPORT */
