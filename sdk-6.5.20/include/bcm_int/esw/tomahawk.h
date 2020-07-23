/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk.h
 * Purpose:     Function declarations for Tomahawk Internal functions.
 */

#ifndef _BCM_INT_TOMAHAWK_H_
#define _BCM_INT_TOMAHAWK_H_

#include <soc/defs.h>

#if defined(BCM_TOMAHAWK_SUPPORT)

#include <bcm/types.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/ipmc.h>
#include <bcm_int/esw/keygen_api.h>

#define TH_AGG_ID_INVALID (-1)
#define TH_AGG_ID_HW_INVALID (0x3F)
/* ECMP mode encodings match hardware values */
#define TH_ECMP_MODE_SINGLE 0x1
#define TH_ECMP_MODE_SINGLE_LOW_LATENCY 0x0
#define TH_ECMP_MODE_HIERARCHICAL 0x3

#define RH_OPT_MAX_PATHS(u) (((SOC_IS_TOMAHAWK3(u)) || (SOC_IS_TOMAHAWK2(u))) \
                            ? 32768: 16384)
#define RH_OPT_MIN_PATHS(u) 64

/* ECMP LB mode encodings match hardware values */
#define BCM_TH_L3_ECMP_LB_MODE_RH 0x01
#define BCM_TH_L3_ECMP_LB_MODE_RANDOM 0x2
#define BCM_TH_L3_ECMP_LB_MODE_RR 0x3

#ifdef BCM_WARM_BOOT_SUPPORT
/* ECMP member table size */
#define RH_ECMP_MEMBER_TBL_MAX(u)    (SOC_IS_TOMAHAWK2(u) ? 32768: 16384)
#endif /* BCM_WARM_BOOT_SUPPORT */

#define L3_EXT_VIEW_INVALID_VNTAG_ETAG_PROFILE 0
#define VNTAG_ACTIONS_NIV   1
#define VNTAG_ACTIONS_ETAG  2

#define TH_MGMT_PORT0_NUM 129  /* Management Port 0 Physical Number */
#define TH_MGMT_PORT1_NUM 131  /* Management Port 1 Physical Number */

#if defined(BCM_FIELD_SUPPORT)
#define _FP_TCAM_IPBMP_SIZE(unit) _field_fp_tcam_ipbmp_size(unit)
extern int _field_fp_tcam_ipbmp_size(int unit);
extern int _bcm_field_th_group_lt_prio_update(int unit, _field_group_t *fg,
                                              int priority);
extern int _bcm_field_th_group_status_calc(int unit, _field_group_t *fg);
extern int _bcm_field_th_group_enable_set(int unit, bcm_field_group_t group, uint32 enable);
extern int _bcm_field_th_group_free_unused_slices(int unit,
                                                  _field_stage_t *stage_fc,
                                                  _field_group_t *fg);
extern int _bcm_field_th_group_add_slice_validate(int unit,
                                                  _field_stage_t *stage_fc,
                                                  _field_group_t *fg,
                                                  int slice_id);
extern int _bcm_field_th_entry_part_tcam_idx_get(int unit,
                                                 _field_entry_t *f_ent,
                                                 uint32 idx_pri,
                                                 uint8 entry_part,
                                                 int *idx_out);
extern int _bcm_field_th_slice_offset_to_tcam_idx(int unit,
                                       _field_stage_t *stage_fc,
                                       int instance, int slice, int slice_idx,
                                       int *tcam_idx);
extern int _bcm_field_th_tcam_idx_to_slice_offset(int unit, 
                                       _field_stage_t *stage_fc,
                                       _field_entry_t *f_ent, int tcam_idx,
                                       int *slice, int *slice_idx);
extern int _bcm_field_th_tcam_part_to_slice_number(int entry_part,
                                                   uint32 group_flags,
                                                   uint8 *slice_number);
extern int _bcm_field_th_tcam_part_to_entry_flags(int unit, int entry_part,
                                                  uint32 group_flags,
                                                  uint32 *entry_flags);
extern int _bcm_field_th_entry_flags_to_tcam_part (int unit, uint32 entry_flags,
                                                   uint32 group_flags, 
                                                   uint8 *entry_part);
extern int _bcm_field_th_lt_tcam_idx_to_slice_offset(int unit,
                                                     _field_stage_t *stage_fc,
                                                     int instance, int tcam_idx,
                                                     int *slice,
                                                     int *slice_idx);
extern int _bcm_field_th_lt_tcam_entry_get(int unit,
                                       _field_group_t *fg,
                                       _field_lt_entry_t *lt_f_ent);
extern int _bcm_field_th_lt_slice_offset_to_tcam_index(int unit,
                                                       _field_stage_t *stage_fc,
                                                       int instance, int slice,
                                                       int slice_idx,
                                                       int *tcam_idx);
extern int _bcm_field_th_entry_tcam_parts_count(int unit, uint32 group_flags,
                                                int *part_count);
extern int _bcm_field_th_lt_entry_tcam_parts_count(int unit,
                                                  _field_stage_id_t stage_id,
                                                  uint32 group_flags,
                                                  int *part_count);
extern int _bcm_field_th_group_deinit(int unit, _field_group_t *fg);
extern int _bcm_field_th_group_add(int unit, uint32 flags, bcm_field_group_config_t *gc);
extern int _bcm_field_th_stages_delete(int unit, _field_control_t *fc);
extern int _bcm_field_th_stages_add(int unit, _field_control_t *fc);
extern int _bcm_field_th_stage_init(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_th_init(int unit, _field_control_t *fc);
extern int _field_th_keygen_profiles_init(int unit, _field_stage_t *stage_fc,
                                          _field_control_t *fc);
extern int _bcm_field_th_multi_pipe_counter_mem_get(int unit,
                                         _field_stage_t *stage_fc,
                                         soc_mem_t *counter_mem);
extern int _bcm_field_th_counter_multi_pipe_collect_init(int unit,
                                                      _field_control_t *fc,
                                                      _field_stage_t *stage_fc);
extern int _bcm_field_th_val_set(uint32 *p_fn_data, uint32 *p_data,
                                       uint32 offset, uint32 width);
extern int _bcm_field_th_val_get(uint32 *p_fn_data, uint32 *p_data,
                                       uint32 offset, uint32 width);
extern int _bcm_field_th_profile1_action_set(int unit, _field_entry_t *f_ent,
                                         _field_action_t *fa, uint32 *entbuf);
extern int _bcm_field_th_profile2_action_set(int unit, _field_entry_t *f_ent,
                                         _field_action_t *fa, uint32 *entbuf);
extern int _bcm_field_th_misc_action_set(int unit, _field_entry_t *f_ent,
                                     _field_action_t *fa, uint32 *entbuf);
extern int _bcm_field_th_l3swl2change_action_set(int unit,
                                                 _field_entry_t *f_ent,
                                                 _field_action_t *fa,
                                                 uint32 *entbuf);
extern int _bcm_field_th_redirect_action_set(int unit, _field_entry_t *f_ent,
                                         _field_action_t *fa, uint32 *entbuf);
extern int _field_th_ingress_policer_action_set(int unit, 
                                                _field_entry_t *f_ent,
                                                          uint32 *buf);
extern int
_bcm_field_th_stat_attach(int unit,
                          bcm_field_group_t field_group,
                          uint32 flex_sid, uint32 *stat_id);
extern int _bcm_field_th_action_params_check(int unit, 
                                             _field_entry_t  *f_ent,
                                             _field_action_t *fa);
extern int _field_calc_group_aset_size(int unit, 
                                       _field_group_t *fg, uint16 *aset_size);
extern int _field_group_check_action_profile(int unit,
                                             _field_group_t *fg, uint16 *new_action_profile);
extern int _bcm_field_qset_update_with_internal_actions(int unit,
                                                        _field_group_t *fg);
extern int _bcm_field_th_action_support_check(int                unit,
                                              _field_entry_t     *f_ent,
                                              bcm_field_action_t action,
                                              int                *result);
extern int _bcm_field_th_action_set(int unit, soc_mem_t mem, _field_entry_t *f_ent,
                                    int tcam_idx, _field_action_t *fa, uint32 *entbuf);
extern int _bcm_field_th_selcodes_install(int unit,
                                          _field_group_t *fg,
                                          uint8 slice_numb,
                                          bcm_pbmp_t pbmp,
                                          int selcode_index);
extern int _bcm_field_th_scache_sync(int unit,
                                     _field_control_t *fc,
                                     _field_stage_t   *stage_fc);

extern int _bcm_field_th_policer_meter_hw_free(int unit, 
                                               _field_entry_t *f_ent,
                                              _field_entry_policer_t *f_ent_pl);
extern int _bcm_field_th_policer_meter_hw_free_old(int unit,
                                               _field_entry_t *f_ent,
                                              _field_entry_policer_t *f_ent_pl);
extern int _bcm_field_th_policer_mem_get(int unit, _field_stage_t *stage_fc,
        int instance, soc_mem_t *policer_mem);
extern int _bcm_field_th_group_oper_mode_set(int unit,
                                             bcm_field_qualify_t stage,
                                             bcm_field_group_oper_mode_t mode);

extern int _bcm_field_th_group_oper_mode_get(int unit,
                                             bcm_field_qualify_t stage,
                                             bcm_field_group_oper_mode_t *mode);

extern int _bcm_field_wb_group_oper_mode_set(int unit,
                                             bcm_field_qualify_t stage,
                                             _field_stage_t **stage_fc,
                                             bcm_field_group_oper_mode_t mode);

extern int _bcm_field_th_source_class_mode_set(int unit,
                                             bcm_field_stage_t stage,
                                             bcm_pbmp_t pbmp,
                                             bcm_field_src_class_mode_t mode);

extern int _bcm_field_th_source_class_mode_get(int unit,
                                             bcm_field_stage_t stage,
                                             bcm_pbmp_t pbmp,
                                             bcm_field_src_class_mode_t *mode);


extern int _bcm_field_th_qualify_MixedSrcClassId(int unit,
                                             bcm_field_entry_t entry,
                                             bcm_pbmp_t pbmp,
                                             bcm_field_src_class_t data,
                                             bcm_field_src_class_t mask);

extern int _bcm_field_th_qualify_MixedSrcClassId_get(int unit,
                                             bcm_field_entry_t entry,
                                             bcm_pbmp_t pbmp,
                                             uint32 hw_data,
                                             uint32 hw_mask,
                                             bcm_field_src_class_t *data,
                                             bcm_field_src_class_t *mask);

extern int _bcm_field_th_qualifier_MixedSrcClassId_delete(int unit,
                                             bcm_field_entry_t entry);

extern int _bcm_field_th_entry_install(int unit, _field_entry_t *f_ent,
                                       int tcam_idx);

extern int _bcm_field_th_entry_reinstall(int unit, _field_entry_t *f_ent,
                                         int tcam_idx);

extern int _bcm_field_th_qual_tcam_key_mask_get(int unit,
                                                _field_entry_t *f_ent,
                                                _field_tcam_t *tcam);

extern int
_bcm_field_th_entry_remove(int unit, _field_entry_t *f_ent, int tcam_idx);

extern int
_bcm_field_th_entry_move(int unit, _field_entry_t *f_ent, int parts_count,
                         int *tcam_idx_old, int *tcam_idx_new);
extern int
_bcm_field_th_entry_enable_set(int unit, _field_entry_t *f_ent,
                               int enable_flag);
extern int
_bcm_field_th_slice_enable_set(int unit, _field_group_t *fg,
                               _field_slice_t *fs, uint8 enable);
extern int _bcm_field_th_stat_action_set(int unit, _field_entry_t *f_ent,
                                         soc_mem_t mem, int tcam_idx,
                                         uint32 *buf);
extern int
_bcm_field_th_forwardingType_set(int unit, _field_stage_id_t stage_id,
                                 bcm_field_entry_t entry,
                                 bcm_field_ForwardingType_t type,
                                 uint32 *data, uint32 *mask);
extern int 
_bcm_field_th_forwardingType_get(int unit, _field_stage_id_t stage_id,
                                 bcm_field_entry_t entry,
                                 uint32 data,  uint32 mask,
                                 bcm_field_ForwardingType_t *type);
extern int
_bcm_field_th_qualify_class(int unit,
                            bcm_field_entry_t entry,
                            bcm_field_qualify_t qual,
                            uint32 *data,
                            uint32 *mask);
extern int 
_bcm_field_th_qualify_svp(int unit, bcm_field_entry_t entry, 
                          bcm_field_qualify_t qual, 
                          uint32 data, uint32 mask,
                          int svp_valid);
extern int
_bcm_field_th_qualify_dvp(int unit, bcm_field_entry_t entry, 
                           bcm_field_qualify_t qual, 
                           uint32 data, uint32 mask,
                           int dvp_valid);

extern int
_bcm_field_th_qualify_trunk(int unit,
                            bcm_field_entry_t entry,
                            bcm_field_qualify_t qual,
                            bcm_trunk_t data, bcm_trunk_t mask);

int
_bcm_field_th_qualify_trunk_get(int unit,
                                bcm_field_entry_t entry,
                                bcm_field_qualify_t qual,
                                bcm_trunk_t *data, bcm_trunk_t *mask);
extern int
_bcm_field_th_qualify_set(int unit, bcm_field_entry_t entry,
                          int qual, uint32 *data, uint32 *mask,
                          uint8 flags);
extern int
_bcm_field_th_qual_value_set(int unit, _bcm_field_qual_offset_t *q_offset, 
                             _field_entry_t *f_ent, 
                             uint32 *p_data, uint32 *p_mask);
extern int
_bcm_field_th_find_empty_slice(int unit, _field_group_t *fg,
                               _field_slice_t **fs_ptr);
extern int
_bcm_field_th_qual_offset_get(int unit, _field_entry_t *f_ent, int qid, 
                             int entry_part, _bcm_field_qual_offset_t **offset);
extern int
_bcm_field_th_entry_qualifier_key_get(int unit, 
                                      bcm_field_entry_t entry, 
                                      int qual_id, 
                                      _bcm_field_qual_data_t q_data,
                                      _bcm_field_qual_data_t q_mask);
extern int 
_bcm_field_th_qualifier_delete(int unit, bcm_field_entry_t entry, 
                               bcm_field_qualify_t qual_id);
extern int _bcm_field_th_qualify_InPorts(int unit,
                                         bcm_field_entry_t entry,
                                         bcm_field_qualify_t qual,
                                         bcm_pbmp_t data, 
                                         bcm_pbmp_t mask);
extern int
_bcm_field_th_qualify_VlanTranslationHit(int               unit,
                                          bcm_field_entry_t entry,
                                          uint8             *data,
                                          uint8             *mask);
extern int
_field_th_group_lt_slice_validate(int unit,
                                  _field_stage_t *stage_fc,
                                  _field_group_t *fg,
                                  int slice_id,
                                  _field_lt_slice_t *lt_fs);
extern int
_bcm_field_th_priority_group_get_next(int unit,
                                      int instance,
                                      _field_stage_id_t stage_id,
                                      int priority,
                                      _field_group_t **curr_group,
                                      _field_group_t **next_group);
extern int
_bcm_field_th_class_stage_add(int unit, _field_control_t *fc,
                              _field_stage_t *stage_fc);
extern int
_bcm_field_th_class_stage_status_init(int unit, _field_stage_t *stage_fc);

extern int
_bcm_field_th_class_stage_status_deinit(int unit, _field_stage_t *stage_fc);

extern int
_bcm_field_th_class_group_add(int unit, _field_group_add_fsm_t *fsm_ptr,
                              bcm_field_aset_t aset);
extern int
_bcm_field_th_data_qualifier_hw_alloc(int unit,
                                      _field_stage_t *stage_fc,
                                      _field_data_qualifier_t *f_dq);
extern int 
_bcm_field_th_field_qset_data_qualifier_add(int unit,
                                            bcm_field_qset_t *qset,
                                            int qual_id);
extern int 
_bcm_field_th_field_qset_data_qualifier_delete(int unit, bcm_field_qset_t *qset,
                                                int qual_id);
extern int 
_bcm_field_th_field_qset_data_qualifier_get(int idx, 
                                       bcm_field_qset_t qset,
                                       bcm_field_qualify_t *qid); 
extern int 
_bcm_field_th_field_qualify_data_elem(int unit, bcm_field_entry_t eid,
                                      _field_data_qualifier_t *f_dq,
                                      uint8 qual_elem_idx,
                                      uint32 data, uint32 mask);
extern int 
_bcm_field_th_field_qualify_data_get(int unit, bcm_field_entry_t eid, int qual_id,
        uint16 length_max,  uint8 *data, uint8 *mask,
        uint16 *length);

extern int
_bcm_field_group_flex_ctr_pool_bmp_update(int unit,
                                         bcm_field_group_t group,
                                         int pool_number);
extern int
_bcm_field_th_em_color_offset_get(int unit,
                                  _field_stat_t *f_st,
                                  uint32 *green_offset,
                                  uint32 *yellow_offset,
                                  uint32 *red_offset);
extern int
_bcm_field_group_flex_ctr_info_get(int unit,
                               bcm_field_group_t group, int *instance,
                               _field_counter_pool_bmp_t *counter_pool_bmp);
extern int
_bcm_field_th_ingress_slice_clear(int unit,
                                  _field_group_t *fg,
                                  _field_slice_t *fs);
extern int
_bcm_field_th_qualify_class_get(int unit,
                                 bcm_field_entry_t entry,
                                 bcm_field_qualify_t qual,
                                 uint32 *data,
                                 uint32 *mask);
extern int
_bcm_field_th_stat_attach(int unit,
                          bcm_field_group_t field_group,
                          uint32 flex_sid, uint32 *stat_id);
extern int
_bcm_field_th_write_slice_map(int unit,
                              _field_stage_t *stage_fc,
                              _field_group_t *fg);
extern void
_bcm_field_th_class_stage_dump(int unit, _field_stage_t *stage_fc);

extern int
_bcm_field_th_class_group_dump(int unit, _field_group_t *class_fg);

extern int
_bcm_field_th_class_group_brief_dump(int unit, _field_group_t *class_fg);

extern int
_bcm_field_th_class_entry_dump(int unit, _field_entry_t *class_ent);

extern int
_bcm_field_th_class_entry_create(int unit, bcm_field_group_t group,
                                 bcm_field_entry_t entry);

extern int
_bcm_field_th_class_entry_destroy(int unit, _field_entry_t *entry);

extern int
_bcm_field_th_class_entry_prio_set(int unit, _field_entry_t *entry, int prio);

extern int
_bcm_field_th_class_entry_prio_get(int unit, _field_entry_t *entry);

extern int
_bcm_field_th_class_qualify_set(int unit, bcm_field_entry_t entry,
                                int qual, uint32 *data, uint32 *mask);
extern int
_bcm_field_th_class_qualify_clear(int unit, bcm_field_entry_t entry);

extern int
_bcm_field_th_class_entry_qualifier_key_get(int unit,
                                            bcm_field_entry_t entry,
                                            int qual_id,
                                            _bcm_field_qual_data_t q_data,
                                            _bcm_field_qual_data_t q_mask);
extern int
_bcm_field_th_class_type_qset_get(int unit, bcm_field_qset_t *qset,
                                  _field_class_type_t *ctype);
extern int
_bcm_field_th_class_action_set(int unit, bcm_field_entry_t entry,
                               bcm_field_class_info_t *class_info);
extern int
_bcm_field_th_class_action_delete(int unit, bcm_field_entry_t entry,
                                  bcm_field_action_t action);
extern int
_bcm_field_th_class_action_get(int unit, bcm_field_entry_t entry,
                               bcm_field_class_info_t *class_info);
extern int
_bcm_field_th_qual_class_size_get(int unit , bcm_field_qualify_t qual,
                                  uint16 *class_size);
extern int
_bcm_field_th_class_entry_install(int unit, bcm_field_entry_t entry);

extern int
_bcm_field_th_class_entry_hwinstall(int unit, _field_entry_t *entry,
                                    _field_class_type_t ctype);
extern int
_bcm_field_th_class_size_get(int unit, _field_class_type_t ctype,
                                       int *memsize);
extern int
_bcm_field_th_class_entry_hwread(int unit, _field_entry_t *entry,
                                 _field_class_type_t ctype,
                                 uint32 *cdata);
extern int
_bcm_field_th_class_entry_remove(int unit, bcm_field_entry_t entry);

extern int
_bcm_field_th_udf_chunks_to_int_qset(int unit, uint32 udf_chunks_bmap,
                                     bcm_field_qset_t *qset);

extern int
_bcm_field_th_qset_udf_offsets_alloc(int unit, _field_stage_id_t stage,
     bcm_field_qset_t qset, int req_offsets, int offset_array[], int *max_chunks,
     int pipe_num);

extern int
_bcm_field_th_qual_udf_offsets_get(int unit, int qid,
                                   int chunk_order[], int *num_chunks,
                                   uint32 udf_hw_bmap);

extern int
_bcm_field_th_qualify_udf_data_elem_get(int unit, int idx,
                                        int max_chunks,
                                        int *qid);
extern int
bcmi_field_th_qualify_udf_get(int unit, bcm_field_entry_t eid,
                              bcm_udf_id_t udf_id, int max_length,
                              uint8 *data, uint8 *mask, int *actual_length);

extern int _field_th_qset_comb_init(int unit, _field_control_t *fc);


extern int
_field_th_emstage_init(int unit, _field_control_t *fc,
                             _field_stage_t *stage_fc);
extern int
_field_th_em_entries_free_get(int unit, _field_group_t *fg,
                                        int *free_count);
extern int
_field_th_emstage_update(int unit, _field_stage_t *stage_fc);
extern int
_field_th_em_group_lookup_get(int unit, int priority,
                                        uint8 *lookup);
extern int
_field_th_em_qualifier_set(int unit, bcm_field_entry_t entry,
                           int qual, uint32 *data, uint32 *mask);
int
_field_th_em_ltid_based_groupid_get(int unit, int ltid,
                                    bcm_field_group_t *group);
extern int
_field_th_em_group_priority_hintbased_qset_update(int unit,
                                                _field_group_t *fg,
                                                bcm_field_hint_t *hint_entry);
extern int
_field_th_emstage_deinit(int unit, _field_stage_t *stage_fc);

extern int
_field_th_exactmatch_slice_validate(int unit, _field_stage_t *stage_fc,
                                      _field_group_t *fg, int slice_id);
extern int
_field_th_keygen_em_profile_entry_pack(int unit, _field_stage_t *stage_fc,
                              _field_group_t *fg, int part, soc_mem_t mem,
                  exact_match_key_gen_program_profile_entry_t *prof_entry);
extern int
_field_th_keygen_em_profile_mask_entry_pack(int unit, _field_stage_t *stage_fc,
                                   _field_group_t *fg, int part, soc_mem_t mem,
                                  exact_match_key_gen_mask_entry_t *prof_entry);
extern int
_field_th_em_entry_install(int unit, _field_entry_t *f_ent);

extern int
_field_th_em_entry_remove(int unit, _field_entry_t *f_ent);

extern int
_field_th_em_entry_action_size_get(int unit, _field_entry_t *f_ent,
                                    int *action_size, uint32 *action_prof_idx);
extern int
_bcm_field_th_flex_counter_status_get(int unit,
                                      _field_group_t *fg,
                                      int *counters_total,
                                      int *counters_free);

extern int
_bcm_field_th_egress_mode_get(int unit, _field_group_t *fg, uint8 *slice_mode);


extern int 
_bcm_field_th_group_update(int unit, bcm_field_group_t group, bcm_field_qset_t *qset);

extern int
_bcm_field_th_entry_phys_dump(int unit, _field_entry_t *f_ent);
extern int _bcm_field_th_entry_tcam_index_get(int unit, bcm_field_entry_t eid,
                                              int *logical_tcam_idx,
                                              int *physical_tcam_idx);
extern int  _bcm_field_th_ifp_slice_mode_get(int unit, int pipe, int slice_num,
                                          int *slice_mode, int *slice_enabled);
#if defined(BCM_TOMAHAWK3_SUPPORT)
extern int 
_bcm_field_th3_qualify_ip_type(int unit, bcm_field_entry_t entry,
                               bcm_field_IpType_t type,
                               bcm_field_qualify_t qual);
extern int 
_bcm_field_th3_qualify_ip_type_encode_get(int unit, uint32 hw_data,
                                                     uint32 hw_mask,
                                                     bcm_field_IpType_t *type);
#endif

extern int
_field_th_stage_action_support_check(int unit,
                                     unsigned stage,
                                     bcm_field_action_t action,
                                     int *result);
extern int 
_bcm_field_th_qualify_PacketRes_get(
                                    int unit, 
                                    bcm_field_entry_t entry, 
                                    uint32 *data, 
                                    uint32 *mask);
extern int
_bcm_field_th_qualify_PacketRes(int               unit,
                                 bcm_field_entry_t entry,
                                 uint32            *data,
                                 uint32            *mask
                                 );

#endif /* !BCM_FIELD_SUPPORT */

#if defined(INCLUDE_L3)
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_th_ipmc_aggid_info_scache_size_get(int unit, uint32 *size);
extern int bcm_th_ipmc_aggid_info_sync(int unit, uint8 **scache_ptr);
extern int bcm_th_ipmc_aggid_info_recover(int unit, uint8 **scache_ptr);
#endif
extern bcm_error_t 
_bcm_niv_gport_get(int unit, int is_trunk, int modid,
                   int port, int dst_vif, bcm_gport_t *niv_gport);
extern bcm_error_t 
_bcm_extender_gport_get(int unit, int is_trunk, int modid,
                        int port, int etag_vid, bcm_gport_t *ext_gport);
extern void
_bcm_th_glp_resolve_embedded_nh(int unit, uint32 gport,
                                bcm_module_t *modid, bcm_port_t *port,
                                int *is_trunk);
extern int bcm_th_ipmc_repl_init(int unit);
extern int bcm_th_ipmc_repl_detach(int unit);
extern int bcm_th_ipmc_repl_set(int unit, int ipmc_id, bcm_port_t port,
                                bcm_vlan_vector_t vlan_vec);
extern int bcm_th_ipmc_repl_get(int unit, int index, bcm_port_t port,
                                bcm_vlan_vector_t vlan_vec);
extern int bcm_th_ipmc_repl_add(int unit, int index, bcm_port_t port,
                                bcm_vlan_t vlan);
extern int bcm_th_ipmc_repl_delete(int unit, int index, bcm_port_t port,
                                bcm_vlan_t vlan);
extern int bcm_th_ipmc_repl_delete_all(int unit, int index,
                                bcm_port_t port);
extern int bcm_th_ipmc_egress_intf_add(int unit, int index, bcm_port_t port,
                                       bcm_l3_intf_t *l3_intf);
extern int bcm_th_ipmc_egress_intf_delete(int unit, int index, bcm_port_t port,
                                       bcm_l3_intf_t *l3_intf);
extern int bcm_th_ipmc_egress_intf_set(int unit, int mc_index,
                                       bcm_port_t port, int if_count,
                                       bcm_if_t *if_array, int is_l3,
                                       int check_port);
extern int bcm_th_ipmc_egress_intf_get(int unit, int mc_index,
                                       bcm_port_t port,
                                       int if_max, bcm_if_t *if_array,
                                       int *if_count);
extern int _bcm_th_ipmc_egress_intf_add(int unit, int index, bcm_port_t port,
                                       int encap_id, int is_l3);
extern int _bcm_th_ipmc_egress_intf_delete(int unit, int index,
                                       bcm_port_t port, int if_max,
                                       int encap_id, int is_l3);
extern int bcm_th_ipmc_trill_mac_update(int unit, uint32 mac_field,
                                        uint8 flag);
extern int bcm_th_ipmc_l3_intf_next_hop_free(int unit, int intf);
extern int _bcm_th_ipmc_l3_intf_next_hop_get(int unit, int intf, int *nh_index);
extern int _bcm_th_ipmc_l3_intf_next_hop_l3_egress_set(int unit, int intf);
extern int _bcm_th_ipmc_l3_intf_next_hop_l3_egress_clear(int unit, int nh_idx);


/* IPMC replication per trunk function */
extern int bcm_th_ipmc_egress_intf_set_for_trunk_first_member(int unit,
                                                              int repl_group,
                                                              bcm_port_t port,
                                                              int if_count,
                                                              bcm_if_t *if_array,
                                                              int is_l3,
                                                              int check_port,
                                                              bcm_trunk_t tgid);
extern int bcm_th_ipmc_egress_intf_set_for_same_pipe_member(int unit,
                                                            int repl_group,
                                                            bcm_port_t port,
                                                            bcm_port_t ori_port,
                                                            bcm_trunk_t tgid);
extern int bcm_th_ipmc_egress_intf_add_in_per_trunk_mode(int unit,
                                                         int repl_group,
                                                         bcm_port_t port,
                                                         int encap_id,
                                                         int is_l3);
extern int bcm_th_ipmc_egress_intf_del_in_per_trunk_mode(int unit,
                                                         int repl_group,
                                                         bcm_port_t port,
                                                         int if_max,
                                                         int encap_id,
                                                         int is_l3);
extern int bcm_th_ipmc_egress_intf_add_for_trunk(int unit, int repl_group,
                                                 bcm_trunk_t tgid,
                                                 int encap_id, int is_l3);
extern int bcm_th_ipmc_egress_intf_del_for_trunk(int unit, int repl_group,
                                                 bcm_trunk_t tgid,
                                                 int if_max, int encap_id,
                                                 int is_l3);
extern int bcm_th_ipmc_egress_intf_add_trunk_member(int unit, int repl_group,
                                                    bcm_port_t port);
extern int bcm_th_ipmc_egress_intf_del_trunk_member(int unit, int repl_group,
                                                    bcm_port_t port);
extern bcm_error_t _th_ipmc_info_get(int unit, int ipmc_index, bcm_ipmc_addr_t *ipmc, 
                                     ipmc_entry_t *entry,
                                     _bcm_esw_ipmc_l3entry_t *use_ipmc_l3entry);
extern bcm_error_t _th_ipmc_glp_set(int unit, bcm_ipmc_addr_t *ipmc,
                                    _bcm_l3_cfg_t *l3cfg);
extern bcm_error_t _th_ipmc_glp_get(int unit, bcm_ipmc_addr_t *ipmc,
                                    _bcm_l3_cfg_t *l3cfg);

extern int _bcm_th_ipmc_src_port_compare(int unit, int ipmc_index,
                                         bcm_ipmc_addr_t *ipmc, int *match);
extern int bcm_th_ipmc_repl_port_attach(int unit, bcm_port_t port);
extern int bcm_th_ipmc_repl_port_detach(int unit, bcm_port_t port);
extern bcm_error_t _bcm_th_macda_oui_profile_init(int unit);
extern void _bcm_th_macda_oui_profile_deinit(int unit);
extern bcm_error_t _bcm_th_vntag_etag_profile_init(int unit);
extern void _bcm_th_vntag_etag_profile_deinit(int unit);
extern bcm_error_t 
_bcm_th_l3_vp_entry_add(int unit, soc_mem_t mem, _bcm_l3_cfg_t *l3cfg,
                        uint32 *buf_p, int *macda_oui_profile_index,
                        int *vntag_etag_profile_index);
extern bcm_error_t
_bcm_th_l3_vp_ent_parse(int unit, soc_mem_t mem,
                        _bcm_l3_cfg_t *l3cfg, void *l3x_entry);
extern bcm_error_t
_bcm_th_l3_vp_entry_del(int unit, _bcm_l3_cfg_t *l3cfg,
                        int macda_oui_profile_index ,int vntag_etag_profile_index);
extern bcm_error_t
_bcm_th_get_extended_profile_index(int unit, soc_mem_t mem_ext,
                                   _bcm_l3_cfg_t *l3cfg,
                                   int *macda_oui_profile_index,
                                   int *vntag_etag_profile_index,
                                   int *ref_count);
extern int _bcm_th_l3_ecmp_across_layers_move(int unit, int threshold,
                                              int direction);
extern void bcm_th_l3_extended_required_scache_size_get(int unit, int *size);
extern bcm_error_t bcm_th_l3_extended_view_sync(int unit, uint8 **scache_ptr,
                                                int offset);
extern bcm_error_t bcm_th_l3_extended_view_reinit(int unit, uint8 **scache_ptr,
                                                  int offset);

extern int bcm_th_aggid_info_detach(int unit);
extern int bcm_th_aggid_info_init(int unit);
extern int bcm_th_set_port_hw_agg_map(int unit, bcm_port_t port, int aggid);
extern int bcm_th_port_aggid_add(int unit, bcm_port_t port, bcm_trunk_t tgid,
                                 int *aggid);
extern int bcm_th_port_aggid_del(int unit, bcm_port_t port);

extern int bcm_th_port_trunkid_get(int unit, bcm_port_t port, int *tgid);
extern int bcm_th_port_last_member_check(int unit, bcm_port_t port, int *val);
extern int bcm_th_port_to_aggid(int unit, bcm_port_t port, int *aggid);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_th_ipmc_repl_l3_intf_scache_size_get(int unit, uint32 *size);
extern int _bcm_th_ipmc_repl_l3_intf_sync(int unit, uint8 **scache_ptr);
extern int _bcm_th_ipmc_repl_l3_intf_scache_recover(int unit, uint8 **scache_ptr);
extern int _bcm_th_ipmc_repl_reload(int unit);
extern int _bcm_th_ipmc_repl_l3_intf_nh_map_scache_size_get(int unit, uint32 *size);
extern int _bcm_th_ipmc_repl_l3_intf_nh_map_sync(int unit, uint8 **scache_ptr);
extern int _bcm_th_ipmc_repl_l3_intf_nh_map_scache_recover(int unit, uint8 **scache_ptr);
extern int _bcm_th_ipmc_repl_l3_intf_trill_nh_map_scache_size_get(int unit, uint32 *size);
extern int _bcm_th_ipmc_repl_l3_intf_trill_nh_map_sync(int unit, uint8 **scache_ptr);
extern int _bcm_th_ipmc_repl_l3_intf_trill_nh_map_scache_recover(int unit, uint8 **scache_ptr);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void _bcm_th_ipmc_repl_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* Resilient Hashing funcitons */
extern int bcm_opt_ecmp_rh_init(int unit);
extern int bcm_opt_ecmp_rh_deinit(int unit);
extern int bcm_opt_l3_egress_ecmp_rh_create(int unit,
                                           bcm_l3_egress_ecmp_t *ecmp,
                                           int intf_count,
                                           bcm_if_t *intf_array,
                                           int op, int count,
                                           bcm_if_t *intf,
                                           bcm_if_t *new_intf_array);
extern int bcm_opt_l3_egress_ecmp_rh_destroy(int unit, bcm_if_t mpintf);
extern int bcm_opt_l3_egress_ecmp_lb_get(int unit, bcm_l3_egress_ecmp_t *ecmp);
extern int bcm_opt_l3_egress_ecmp_rh_shared_copy(int unit, int old_ecmp_group,
                                                int new_ecmp_group);
extern int bcm_opt_ecmp_rh_set_intf_arr(int unit, int intf_count,
                                       bcm_if_t *intf_array,
                                       int ecmp_group_idx,
                                       int group_size,
                                       int max_paths);
extern int bcm_opt_ecmp_rh_multipath_get(int unit, int ecmp_group_idx, int intf_size,
                                        bcm_if_t *intf_array, int *intf_count,
                                        int *max_paths);
extern int bcm_opt_l3_egress_rh_ecmp_find(int unit, int intf_count,
                                         bcm_if_t *intf_array, bcm_if_t *mpintf);
extern int bcm_opt_ecmp_lb_mode_set(int unit,
                                   int ecmp_group_idx,
                                   uint8 lb_mode);
extern int bcm_opt_ecmp_lb_mode_reset(int unit,
                                     bcm_if_t mpintf);
extern void bcm_opt_ecmp_group_rh_set(int unit, int ecmp_group_idx, int enable);
extern int bcm_opt_ecmp_group_is_rh(int unit,
                                   int ecmp_group_idx);
extern int bcm_opt_l3_egress_ecmp_rh_shared_copy(int unit, int old_ecmp_group,
                                                int new_ecmp_group);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_opt_l3_ecmp_rh_member_sync(int unit, uint8 **scache_ptr,
                                          int offset);
extern int bcm_opt_l3_ecmp_rh_member_recover(int unit, uint8 **scache_ptr,
                                            int ecmp_max_paths, int offset);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_opt_ecmp_rh_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#endif /* INCLUDE_L3 */

extern bcm_error_t
_bcm_th_port_speed_supported(int unit, bcm_port_t port, int speed);
extern int bcm_th_phy_lb_check(int unit, bcm_port_t port, int loopback);
extern int bcm_th_phy_lb_set(int unit);

/* Cosq */
extern int _bcm_th_cosq_port_resolve(int unit, bcm_gport_t gport, 
                                     bcm_module_t *modid, bcm_port_t *port,
                                     bcm_trunk_t *trunk_id, int *id, int *qnum);
extern int bcm_th_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *port,
                                 int *numq, uint32 *flags);
extern int bcm_th_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 min_quantum, uint32 max_quantum,
                                          uint32 burst_quantum, uint32 flags);
extern int bcm_th_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 *min_quantum,
                                          uint32 *max_quantum,
                                          uint32 *burst_quantum,
                                          uint32 *flags);

extern int bcm_th_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                                 bcm_cos_queue_t cosq,
                                                 uint32 kbits_burst_min, 
                                                 uint32 kbits_burst_max);
extern int bcm_th_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                                 bcm_cos_queue_t cosq,
                                                 uint32 *kbits_burst_min,
                                                 uint32 *kbits_burst_max);

extern int bcm_th_cosq_gport_sched_get(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq, int *mode,
                                       int *weight);
extern int bcm_th_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq, int mode,
                                       int weight);
extern int bcm_th_cosq_sched_weight_max_get(int unit, int mode,
                                            int *weight_max);

extern int bcm_th_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                                      void *user_data);

extern int bcm_th_cosq_gport_traverse_by_port(int unit, bcm_gport_t port,
                                           bcm_cosq_gport_traverse_cb cb,
                                           void *user_data);
extern int bcm_th_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           uint32 kbits_sec_min,
                                           uint32 kbits_sec_max,
                                           uint32 flags);
extern int bcm_th_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           uint32 *kbits_sec_min,
                                           uint32 *kbits_sec_max,
                                           uint32 *flags);
extern int bcm_th_cosq_port_burst_set(int unit, bcm_port_t port, 
                                      bcm_cos_queue_t cosq, int burst);
extern int bcm_th_cosq_port_burst_get(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq,
                                      int *burst);

extern int bcm_th_port_rate_egress_set(int unit, bcm_port_t port,
                                       uint32 bandwidth, uint32 burst,
                                       uint32 mode);
extern int bcm_th_port_rate_egress_get(int unit, bcm_port_t port,
                                       uint32 *bandwidth, uint32 *burst,
                                       uint32 *mode);
extern int bcm_th_port_drain_cells(int unit, int port);

extern int bcm_th_cosq_port_pps_set(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int pps);
extern int bcm_th_cosq_port_pps_get(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int *pps);

extern int bcm_th_cosq_discard_set(int unit, uint32 flags);
extern int bcm_th_cosq_discard_get(int unit, uint32 *flags);

extern int bcm_th_cosq_discard_port_get(int unit, bcm_port_t port, 
                                        bcm_cos_queue_t cosq, uint32 color,
                                        int *drop_start, 
                                        int *drop_slope, int *average_time);
extern int bcm_th_cosq_discard_port_set(int unit, bcm_port_t port, 
                                        bcm_cos_queue_t cosq,
                                        uint32 color, int drop_start,
                                        int drop_slope,
                                        int average_time);

extern int bcm_th_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                                         bcm_cos_queue_t cosq,
                                         bcm_cosq_buffer_id_t buffer,
                                         bcm_cosq_gport_discard_t *discard);
extern int bcm_th_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                                         bcm_cos_queue_t cosq,
                                         bcm_cosq_buffer_id_t buffer,
                                         bcm_cosq_gport_discard_t *discard);

extern int bcm_th_cosq_wred_resolution_entry_create(int unit, int *index);
extern int bcm_th_cosq_wred_resolution_entry_destroy(int unit, int index);
extern int bcm_th_cosq_wred_resolution_set(int unit, int index,
                                           bcm_cosq_discard_rule_t *rule);
extern int bcm_th_cosq_wred_resolution_get(int unit, int index, int max,
                                           bcm_cosq_discard_rule_t *rule,
                                           int *count);

extern int bcm_th_cosq_port_pfc_op(int unit, bcm_port_t port,
                                   bcm_switch_control_t sctype,
                                   _bcm_cosq_op_t op,
                                   bcm_gport_t *gport,
                                   int gport_count);
extern int bcm_th_cosq_port_pfc_get(int unit, bcm_port_t port,
                                    bcm_switch_control_t sctype,
                                    bcm_gport_t *gport,
                                    int gport_count,
                                    int *actual_gport_count);
extern int
bcm_th_pfc_deadlock_ignore_pfc_xoff_gen(int unit, int priority,
                                        bcm_port_t port, uint32 *rval32);
extern int
bcm_th_cosq_pfc_class_mapping_set(int unit, bcm_gport_t port,
                                  int array_count,
                                  bcm_cosq_pfc_class_mapping_t *mapping_array);

extern int
bcm_th_cosq_pfc_class_mapping_get(int unit, bcm_gport_t port,
                                  int array_max,
                                  bcm_cosq_pfc_class_mapping_t *mapping_array,
                                  int *array_count);
extern int
bcm_th_cosq_safc_class_mapping_set(int unit, bcm_gport_t port,
                                   int array_count,
                                   bcm_cosq_safc_class_mapping_t *mapping_array);

extern int
bcm_th_cosq_safc_class_mapping_get(int unit, bcm_gport_t port,
                                   int array_max,
                                   bcm_cosq_safc_class_mapping_t *mapping_array,
                                   int *array_count);

extern int
bcm_th_port_priority_group_mapping_set(int unit, bcm_gport_t gport,
                                        int prio, int priority_group);
extern int
bcm_th_port_priority_group_mapping_get(int unit, bcm_gport_t gport,
                                        int prio, int *priority_group);
extern int
bcm_th_port_priority_group_config_set(int unit, bcm_gport_t gport,
    int priority_group, bcm_port_priority_group_config_t *prigrp_config);
extern int
bcm_th_port_priority_group_config_get(int unit, bcm_gport_t gport,
    int priority_group, bcm_port_priority_group_config_t *prigrp_config);

extern int bcm_th_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                                      int mode, const int *weights, int delay);
extern int bcm_th_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                                      int *mode, int *weights, int *delay);

extern int bcm_th_cosq_gport_add(int unit, bcm_gport_t port, int numq,
                                 uint32 flags, bcm_gport_t *gport);
extern int bcm_th_cosq_gport_delete(int unit, bcm_gport_t gport);
extern int bcm_th_cosq_gport_attach(int unit, bcm_gport_t input_gport,
                                    bcm_gport_t parent_gport,
                                    bcm_cos_queue_t cosq);
extern int bcm_th_rx_queue_channel_set_test(int unit,
                                            bcm_cos_queue_t queue_id,
                                            bcm_rx_chan_t chan_id);
extern int bcm_th_cosq_gport_detach(int unit, bcm_gport_t input_gport,
                                    bcm_gport_t parent_gport,
                                    bcm_cos_queue_t cosq);
extern int
bcm_th_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                             bcm_gport_t *input_gport, bcm_cos_queue_t *cosq);
extern int
bcm_th_cosq_gport_child_get(int unit, bcm_gport_t in_gport,
                             bcm_cos_queue_t cosq, bcm_gport_t *out_gport);

extern int bcm_th_cosq_mapping_set(int unit, bcm_port_t gport, 
                                   bcm_cos_t priority, bcm_cos_queue_t cosq);
extern int bcm_th_cosq_mapping_get(int unit, bcm_port_t gport,
                                   bcm_cos_t priority, bcm_cos_queue_t *cosq);

extern int bcm_th_cosq_control_set(int unit, bcm_gport_t gport,
                                   bcm_cos_queue_t cosq,
                                   bcm_cosq_buffer_id_t buffer,
                                   bcm_cosq_control_t type, int arg);
extern int bcm_th_cosq_control_get(int unit, bcm_gport_t gport,
                                   bcm_cos_queue_t cosq,
                                   bcm_cosq_buffer_id_t buffer,
                                   bcm_cosq_control_t type, int *arg);

extern int bcm_th_cosq_stat_set(int unit, bcm_gport_t port,
                                bcm_cos_queue_t cosq,
                                bcm_cosq_stat_t stat, uint64 value);
extern int bcm_th_cosq_stat_get(int unit, bcm_port_t port,
                                bcm_cos_queue_t cosq,
                                bcm_cosq_stat_t stat, int sync_mode,
                                uint64 *value);

extern int bcm_th_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                                         bcm_cos_t int_pri, uint32 flags,
                                         bcm_gport_t gport,
                                         bcm_cos_queue_t cosq);
extern int bcm_th_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                                         bcm_cos_t int_pri, uint32 flags,
                                         bcm_gport_t *gport,
                                         bcm_cos_queue_t *cosq);
extern int bcm_th_cosq_priority_mapping_get_all(int unit, bcm_gport_t gport,
                                                int index,
                                                bcm_cosq_priority_mapping_t type,
                                                int pri_max, int *pri_array,
                                                int *pri_count);

extern int bcm_th_cosq_config_set(int unit, int numq);
extern int bcm_th_cosq_config_get(int unit, int *numq);
extern int bcm_th_cosq_buffer_id_multi_get(int unit, bcm_gport_t gport,
                                  bcm_cos_queue_t cosq, bcm_cosq_dir_t direction,
                                  int array_max, bcm_cosq_buffer_id_t *buf_id_array,
                                  int *array_count);
extern int
bcm_th_cosq_field_classifier_get(
    int unit, int classifier_id, bcm_cosq_classifier_t *classifier);

extern int
bcm_th_cosq_field_classifier_id_create(
    int unit, bcm_cosq_classifier_t *classifier, int *classifier_id);

extern int
bcm_th_cosq_field_classifier_id_destroy(
    int unit, int classifier_id);

extern int
bcm_th_cosq_field_classifier_map_set(
    int unit, int classifier_id, int count,
    bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array);

extern int
bcm_th_cosq_field_classifier_map_get(
    int unit, int classifier_id,
    int array_max, bcm_cos_t *priority_array,
    bcm_cos_queue_t *cosq_array, int *array_count);

extern int
bcm_th_cosq_field_classifier_map_clear(
    int unit, int classifier_id);

extern int bcm_th_cosq_init(int unit);
extern int bcm_th_cosq_detach(int unit, int software_state_only);
extern int _bcm_th_pfc_deadlock_init(int unit);
extern int bcm_th_pfc_deadlock_recovery_start(int unit,
                                              bcm_port_t port, int pri);
extern int bcm_th_pfc_deadlock_recovery_exit(int unit,
                                              bcm_port_t port, int pri);

extern int
bcm_th_cosq_bst_profile_set(int unit,
                            bcm_gport_t port,
                            bcm_cos_queue_t cosq,
                            bcm_bst_stat_id_t bid,
                            bcm_cosq_bst_profile_t *profile);

extern int
bcm_th_cosq_bst_profile_get(int unit,
                            bcm_gport_t port,
                            bcm_cos_queue_t cosq,
                            bcm_bst_stat_id_t bid,
                            bcm_cosq_bst_profile_t *profile);

extern int
bcm_th_cosq_bst_multi_profile_set(int unit,
                            bcm_gport_t gport,
                            bcm_cos_queue_t cosq,
                            bcm_bst_stat_id_t bid,
                            int array_size,
                            bcm_cosq_bst_profile_t *profile_array);

extern int
bcm_th_cosq_bst_multi_profile_get(int unit,
                            bcm_gport_t gport,
                            bcm_cos_queue_t cosq,
                            bcm_bst_stat_id_t bid,
                            int array_size,
                            bcm_cosq_bst_profile_t *profile_array,
                            int *count);

extern int
bcm_th_cosq_bst_stat_get(int unit,
                         bcm_gport_t port,
                         bcm_cos_queue_t cosq,
                         bcm_cosq_buffer_id_t buffer,
                         bcm_bst_stat_id_t bid,
                         uint32 options,
                         uint64 *pvalue);

extern int
bcm_th_cosq_bst_stat_multi_get(int unit,
                               bcm_gport_t port,
                               bcm_cos_queue_t cosq,
                               uint32 options,
                               int max_values,
                               bcm_bst_stat_id_t *id_list,
                               uint64 *pvalues);

extern int
bcm_th_cosq_bst_stat_clear(int unit, bcm_gport_t port,
                           bcm_cos_queue_t cosq, bcm_cosq_buffer_id_t buffer,
                           bcm_bst_stat_id_t bid);

extern int bcm_th_cosq_bst_stat_sync(int unit, bcm_bst_stat_id_t bid);

extern int
_bcm_th_bst_hdrm_stat_clear(int unit,
                            bcm_gport_t gport,
                            bcm_cos_queue_t cosq,
                            bcm_cosq_buffer_id_t buffer);

extern int
_bcm_th_cosq_bst_hdrm_stat_get(int unit,
                               bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_buffer_id_t buffer,
                               uint32 options,
                               uint64 *pvalue);

extern int
_bcm_th_port_enqueue_set(int unit, bcm_port_t gport, int enable);

extern int
_bcm_th_port_enqueue_get(int unit, bcm_port_t gport, int *enable);

extern int bcm_th_cosq_cpu_cosq_enable_set(int unit, bcm_cos_queue_t cosq,
                                           int enable);
extern int bcm_th_cosq_cpu_cosq_enable_get(int unit, bcm_cos_queue_t cosq,
                                           int *enable);
extern void bcm_th_cosq_sw_dump(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_th_cosq_sync(int unit);
extern int bcm_th_cosq_reinit(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

typedef enum {
    _BCM_TH_COSQ_INDEX_STYLE_WRED_QUEUE,
    _BCM_TH_COSQ_INDEX_STYLE_WRED_PORT,
    _BCM_TH_COSQ_INDEX_STYLE_WRED_DEVICE,
    _BCM_TH_COSQ_INDEX_STYLE_UCAST_QUEUE,
    _BCM_TH_COSQ_INDEX_STYLE_MCAST_QUEUE,
    _BCM_TH_COSQ_INDEX_STYLE_EGR_POOL,
    _BCM_TH_COSQ_INDEX_STYLE_COS,
    _BCM_TH_COSQ_INDEX_STYLE_COUNT
} _bcm_th_cosq_index_style_t;

extern int _bcm_th_cosq_index_resolve(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, _bcm_th_cosq_index_style_t style,
                                      bcm_port_t *local_port, int *index, int *count);

extern int _bcm_th_cosq_port_cos_resolve(int unit, bcm_port_t port, bcm_cos_t cos,
                                         _bcm_th_cosq_index_style_t style,
                                         bcm_gport_t *gport);
extern int _bcm_th_cosq_inv_mapping_get(int unit, bcm_gport_t gport,
                                        bcm_cos_queue_t cosq,
                                        uint32 flags, bcm_port_t *ing_port,
                                        bcm_cos_t *priority);

extern int bcm_th_cosq_port_attach(int unit, bcm_port_t port);
extern int bcm_th_cosq_port_detach(int unit, bcm_port_t port);

/* OOB FC Tx side global configuration API */
extern int bcm_th_oob_fc_tx_config_set(
    int unit,
    bcm_oob_fc_tx_config_t *config);

/* OOB FC Tx side global configuration get API */
extern int bcm_th_oob_fc_tx_config_get(
    int unit,
    bcm_oob_fc_tx_config_t *config);

extern int
_bcm_th_oob_fc_tx_port_control_set(int unit, bcm_port_t port,
                                   int status, int dir);
extern int
_bcm_th_oob_fc_tx_port_control_get(int unit, bcm_port_t port,
                                   int *status, int dir);

/* OOB FC Tx side global info get API */
extern int bcm_th_oob_fc_tx_info_get(
    int unit,
    bcm_oob_fc_tx_info_t *info);

/*
 * OOB FC Rx Interface side Traffic Class to Priority Mapping Multi Set
 * API
 */
extern int bcm_th_oob_fc_rx_port_tc_mapping_multi_set(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    int array_count,
    uint32 *tc,
    uint32 *pri_bmp);

/*
 * OOB FC Rx Interface side Traffic Class to Priority Mapping Multi Get
 * API
 */
extern int bcm_th_oob_fc_rx_port_tc_mapping_multi_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    int array_max,
    uint32 *tc,
    uint32 *pri_bmp,
    int *array_count);

/* OOB FC Rx Interface side Traffic Class to Priority Mapping set API */
extern int bcm_th_oob_fc_rx_port_tc_mapping_set(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    uint32 tc,
    uint32 pri_bmp);

/* OOB FC Rx Interface side Traffic Class to Priority Mapping get API */
extern int bcm_th_oob_fc_rx_port_tc_mapping_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    uint32 tc,
    uint32 *pri_bmp);

/* OOB FC Rx Interface side Configuration set API */
extern int bcm_th_oob_fc_rx_config_set(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_oob_fc_rx_config_t *config,
    int array_count,
    bcm_gport_t *gport_array);

/* OOB FC Rx Interface side Configuration get API */
extern int bcm_th_oob_fc_rx_config_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_oob_fc_rx_config_t *config,
    int array_max,
    bcm_gport_t *gport_array,
    int *array_count);

/* OOB FC Rx Interface side Channel offset value for a port */
extern int bcm_th_oob_fc_rx_port_offset_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    uint32 *offset);

/* OOB STATS global configuration set API */
extern int bcm_th_oob_stats_config_set(
    int unit,
    bcm_oob_stats_config_t *config);

/* OOB STATS global configuration get API */
extern int bcm_th_oob_stats_config_get(
    int unit,
    bcm_oob_stats_config_t *config);


/* OOB STATS configuration of service pool list multi set API */
extern int bcm_th_oob_stats_pool_mapping_multi_set(
    int unit,
    int array_count,
    int *offset_array,
    uint8 *dir_array,
    bcm_service_pool_id_t *pool_array);

/* OOB STATS configuration of service pool list multi get API */
extern int bcm_th_oob_stats_pool_mapping_multi_get(
    int unit,
    int array_max,
    int *offset_array,
    uint8 *dir_array,
    bcm_service_pool_id_t *pool_array,
    int *array_count);

/* OOB STATS configuration of service pool list set API */
extern int bcm_th_oob_stats_pool_mapping_set(
    int unit,
    int offset,
    uint8 dir,
    bcm_service_pool_id_t pool);

/* OOB STATS configuration of service pool list get API */
extern int bcm_th_oob_stats_pool_mapping_get(
    int unit,
    int offset,
    uint8 *dir,
    bcm_service_pool_id_t *pool);

/* OOB STATS configuration of Queue list set API */
extern int bcm_th_oob_stats_queue_mapping_multi_set(
    int unit,
    int array_count,
    int *offset_array,
    bcm_gport_t *gport_array);

/* OOB STATS configuration of Queue list get API */
extern int bcm_th_oob_stats_queue_mapping_multi_get(
    int unit,
    int array_max,
    int *offset_array,
    bcm_gport_t *gport_array,
    int *array_count);

/* OOB STATS configuration of Queue list set API */
extern int bcm_th_oob_stats_queue_mapping_set(
    int unit,
    int offset,
    bcm_gport_t gport);

/* OOB STATS configuration of Queue list get API */
extern int bcm_th_oob_stats_queue_mapping_get(
    int unit,
    int offset,
    bcm_gport_t *gport);

/* QoS */
extern int bcm_th_qos_init(int unit);
extern int bcm_th_qos_detach(int unit);
extern int bcm_th_qos_map_create(int unit, uint32 flags, int *map_id);
extern int bcm_th_qos_map_destroy(int unit, int map_id);
extern int bcm_th_qos_map_add(int unit,
                              uint32 flags,
                              bcm_qos_map_t *map,
                              int map_id);
extern int bcm_th_qos_map_delete(int unit,
                                 uint32 flags,
                                 bcm_qos_map_t *map,
                                 int map_id);
extern int bcm_th_qos_port_map_set(int unit,
                                   bcm_gport_t port,
                                   int ing_map,
                                   int egr_map);
extern int bcm_th_qos_map_multi_get(int unit,
                                    uint32 flags,
                                    int map_id,
                                    int array_size,
                                    bcm_qos_map_t *array,
                                    int *array_count);
extern int _bcm_th_qos_multi_get(int unit,
                                 int array_size,
                                 int *map_ids_array,
                                 int *flags_array,
                                 int *array_count);
extern int bcm_th_qos_port_map_type_get(int unit, bcm_gport_t port,
                                        uint32 flags, int *map_id);
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_th_qos_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_th_qos_reinit_scache_len_get(int unit, uint32 *scache_len);
extern int _bcm_th_qos_reinit_from_hw_state(int unit, soc_mem_t mem, 
                                            soc_field_t field, uint8 map_type, 
                                            SHR_BITDCL *hw_idx_bmp, int hw_idx_bmp_len);
extern int _bcm_th_qos_unsynchronized_reinit(int unit);
extern int _bcm_th_qos_extended_reinit(int unit, uint8 **scache_ptr);
extern int _bcm_th_qos_reinit_hw_profiles_update(int unit);
extern int _bcm_th_qos_sync(int unit, uint8 **scache_ptr);
#endif /* BCM_WARM_BOOT_SUPPORT */
extern int _bcm_field_th_qualify_LoopbackType(int unit,
                                              bcm_field_LoopbackType_t loopback_type,
                                              uint32                   *tcam_data,
                                              uint32                   *tcam_mask);
extern int _bcm_field_th_qualify_LoopbackType_get(uint8                    tcam_data,
                                                  uint8                    tcam_mask,
                                                  bcm_field_LoopbackType_t *loopback_type);
extern int _bcm_field_th_qual_part_offset_get(int unit, _field_entry_t *f_ent,
                                              int entry_part, int qid,
                                              _bcm_field_qual_offset_t *offset);

extern int _field_th_stage_entry_enable_set(int unit, _field_entry_t *f_ent,
                                   int enable_flag);
extern int _field_th_group_qual_info_get(int unit,
                                         _field_stage_t *stage_fc,
                                         _field_group_t *fg,
                                         bcm_field_qset_t *qset_req,
                                         bcmi_keygen_qual_info_t **qual_info_arr,
                                         uint16 *qual_info_count);
/*
 * 8 Priority Groups supported.
 */
#define TH_PRIOROTY_GROUP_ID_MIN 0
#define TH_PRIOROTY_GROUP_ID_MAX 7
/*
 * Inpur priority range.
 * PFC : 0-7
 * SAFC : 0-15
 */
#define TH_PFC_INPUT_PRIORITY_MIN  0
#define TH_PFC_INPUT_PRIORITY_MAX  7
#define TH_SAFC_INPUT_PRIORITY_MIN TH_PFC_INPUT_PRIORITY_MIN
#define TH_SAFC_INPUT_PRIORITY_MAX 15


#if defined(INCLUDE_FLOWTRACKER)
/* Flowtracker module APIs */
extern int _bcm_th_ft_init(int unit);

extern int _bcm_th_ft_detach(int unit);

extern int
_bcm_th_ft_export_template_create(
               int unit,
               uint32 options,
               bcm_flowtracker_export_template_t *id,
               uint16 set_id,
               int num_export_elements,
               bcm_flowtracker_export_element_info_t *list_of_export_elements);

extern int
_bcm_th_ft_export_template_get(
                 int unit,
                 bcm_flowtracker_export_template_t id,
                 uint16 *set_id,
                 int max_size,
                 bcm_flowtracker_export_element_info_t *list_of_export_elements,
                 int *list_size);

extern int
_bcm_th_ft_export_template_destroy(int unit,
                                   bcm_flowtracker_export_template_t id);

extern int
_bcm_th_ft_group_clear(int unit,
                       bcm_flowtracker_group_t id,
                       uint32 flags);

extern int
_bcm_th_ft_collector_create(int unit,
                            uint32 options,
                            bcm_flowtracker_collector_t *id,
                            bcm_flowtracker_collector_info_t *collector_info);

extern int
_bcm_th_ft_collector_get(int unit,
                         bcm_flowtracker_collector_t id,
                         bcm_flowtracker_collector_info_t *collector_info);

extern int
_bcm_th_ft_collector_get_all(
                     int unit,
                     int max_size,
                     bcm_flowtracker_collector_t *collector_list,
                     int *list_size);

extern int _bcm_th_ft_collector_destroy(int unit,
                                        bcm_flowtracker_collector_t id);

extern int _bcm_th_ft_group_collector_attach(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_collector_t collector_id,
        int export_profile_id,
        bcm_flowtracker_export_template_t template_id);

extern int _bcm_th_ft_group_collector_detach(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_collector_t collector_id,
        int export_profile_id,
        bcm_flowtracker_export_template_t template_id);

extern int _bcm_th_ft_group_collector_attach_get_all(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int max_list_size,
    bcm_flowtracker_collector_t *list_of_collectors,
    int *list_of_export_profile_ids,
    bcm_flowtracker_export_template_t *list_of_templates,
    int *list_size);

extern int
_bcm_th_ft_flow_group_create(int unit,
                             uint32 options,
                             bcm_flowtracker_group_t *id,
                             bcm_flowtracker_group_info_t *flow_group_info);

extern int
_bcm_th_ft_flow_group_get(int unit,
                          bcm_flowtracker_group_t id,
                          bcm_flowtracker_group_info_t *flow_group_info);

extern int
_bcm_th_ft_flow_group_get_all(int unit,
                              int max_size,
                              bcm_flowtracker_group_t *flow_group_list,
                              int *list_size);

extern int _bcm_th_ft_flow_group_flow_limit_set(int unit,
                                                bcm_flowtracker_group_t id,
                                                uint32 flow_limit);

extern int _bcm_th_ft_flow_group_flow_limit_get(int unit,
                                                bcm_flowtracker_group_t id,
                                                uint32 *flow_limit);

extern int _bcm_th_ft_flow_group_stat_modeid_set(int unit,
                                                 bcm_flowtracker_group_t id,
                                                 uint32 stat_modeid);

extern int _bcm_th_ft_flow_group_stat_modeid_get(int unit,
                                                 bcm_flowtracker_group_t id,
                                                 uint32 *stat_modeid);

extern int _bcm_th_ft_flow_group_age_timer_set(int unit,
                                               bcm_flowtracker_group_t id,
                                               uint32 aging_interval_ms);

extern int _bcm_th_ft_flow_group_age_timer_get(int unit,
                                               bcm_flowtracker_group_t id,
                                               uint32 *aging_interval_ms);

extern int
_bcm_th_ft_flow_group_export_trigger_set(
                      int unit,
                      bcm_flowtracker_group_t id,
                      bcm_flowtracker_export_trigger_info_t *export_trigger_info);

extern int
_bcm_th_ft_flow_group_export_trigger_get(
                     int unit,
                     bcm_flowtracker_group_t id,
                     bcm_flowtracker_export_trigger_info_t *export_trigger_info);

extern int _bcm_th_ft_flow_group_flow_count_get(int unit,
                                                bcm_flowtracker_group_t id,
                                                uint32 *flow_count);

extern int _bcm_th_ft_flow_group_destroy(int unit,
                                         bcm_flowtracker_group_t id);

extern int
_bcm_th_ft_flow_group_collector_add(int unit,
                                    bcm_flowtracker_group_t flow_group_id,
                                    bcm_flowtracker_collector_t collector_id,
                                    bcm_flowtracker_export_template_t template_id);

extern int
_bcm_th_ft_flow_group_collector_delete(int unit,
                                       bcm_flowtracker_group_t flow_group_id,
                                       bcm_flowtracker_collector_t collector_id,
                                       bcm_flowtracker_export_template_t template_id);

extern int _bcm_th_ft_flow_group_collector_get_all(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int max_list_size,
    bcm_flowtracker_collector_t *list_of_collectors,
    bcm_flowtracker_export_template_t *list_of_templates,
    int *list_size);

extern int _bcm_th_ft_group_data_get(
                                         int unit,
                                         bcm_flowtracker_group_t flow_group_id,
                                         bcm_flowtracker_flow_key_t *flow_key,
                                         bcm_flowtracker_flow_data_t *flow_data);

extern int
_bcm_th_ft_group_actions_set(
                                int unit,
                                bcm_flowtracker_group_t flow_group_id,
                                uint32 flags,
                                int num_actions,
                                bcm_flowtracker_group_action_info_t *action_list);

extern int
_bcm_th_ft_group_actions_get(
                               int unit,
                               bcm_flowtracker_group_t flow_group_id,
                               uint32 flags,
                               int max_actions,
                               bcm_flowtracker_group_action_info_t *action_list,
                               int *num_actions);

extern int
_bcm_th_ft_template_transmit_config_set(
                             int unit,
                             bcm_flowtracker_export_template_t template_id,
                             bcm_flowtracker_collector_t collector_id,
                             bcm_flowtracker_template_transmit_config_t *config);
extern int
_bcm_th_ft_template_transmit_config_get(
                             int unit,
                             bcm_flowtracker_export_template_t template_id,
                             bcm_flowtracker_collector_t collector_id,
                             bcm_flowtracker_template_transmit_config_t *config);

extern int
_bcm_th_ft_elephant_profile_create(
                                int unit,
                                uint32 options,
                                bcm_flowtracker_elephant_profile_info_t *profile,
                                bcm_flowtracker_elephant_profile_t *profile_id);

extern int
_bcm_th_ft_elephant_profile_destroy(
                                  int unit,
                                  bcm_flowtracker_elephant_profile_t profile_id);

extern int
_bcm_th_ft_elephant_profile_get(
                               int unit,
                               bcm_flowtracker_elephant_profile_t profile_id,
                               bcm_flowtracker_elephant_profile_info_t *profile);

extern int
_bcm_th_ft_elephant_profile_get_all(
                           int unit,
                           int max,
                           bcm_flowtracker_elephant_profile_t *profile_list,
                           int *count);

extern int
_bcm_th_ft_group_elephant_profile_attach(
                                  int unit,
                                  bcm_flowtracker_group_t flow_group_id,
                                  bcm_flowtracker_elephant_profile_t profile_id);

extern int
_bcm_th_ft_group_elephant_profile_attach_get(
                                 int unit,
                                 bcm_flowtracker_group_t flow_group_id,
                                 bcm_flowtracker_elephant_profile_t *profile_id);

extern int
_bcm_th_ft_group_elephant_profile_detach(int unit,
                                         bcm_flowtracker_group_t flow_group_id);

extern int _bcm_th_ft_sync(int unit);

extern int
_bcm_th_ft_flow_group_tracking_params_set(
                      int unit,
                      bcm_flowtracker_group_t id,
                      int num_tracking_params,
                      bcm_flowtracker_tracking_param_info_t *list_of_tracking_params);

extern int
_bcm_th_ft_flow_group_tracking_params_get(
                      int unit,
                      bcm_flowtracker_group_t id,
                      int max_size,
                      int *list_size,
                      bcm_flowtracker_tracking_param_info_t *list_of_tracking_params);

#endif /* INCLUDE_FLOWTRACKER */

/* Congestion Monitoring App Microburst detection APIs */
extern int bcm_th_cosq_burst_monitor_init(int unit);
extern int bcm_th_cosq_burst_monitor_detach(int unit);
extern int bcm_th_cosq_burst_monitor_flow_view_config_set(int unit,
        uint32 options, bcm_cosq_burst_monitor_flow_view_info_t *flow_view);
extern int bcm_th_cosq_burst_monitor_flow_view_config_get(int unit,
        bcm_cosq_burst_monitor_flow_view_info_t *flow_view);
extern int bcm_th_cosq_burst_monitor_flow_view_data_clear(int unit);
extern int bcm_th_cosq_burst_monitor_flow_view_data_get(int unit,
        uint32 usr_mem_size, uint8 *usr_mem_add);
extern int bcm_th_cosq_burst_monitor_get_current_time(int unit, uint64 *time_usecs);
extern int bcm_th_cosq_burst_monitor_view_summary_get(int unit, uint32 mem_size,
        uint8 *mem_addr, int max_num_views, int *num_views);
extern int bcm_th_cosq_burst_monitor_flow_view_stats_get(int unit, uint32 mem_size,
        uint8 *mem_addr, int max_num_flows, int view_id, uint32 flags, int *num_flows);
extern int bcm_th_cosq_burst_monitor_dma_config_set(int unit,
                                                    uint32 host_mem_size,
                                                    uint32 **host_mem_add);
extern int bcm_th_cosq_burst_monitor_dma_config_get(int unit,
                                                    uint32 *host_mem_size,
                                                    uint32 **host_mem_add);
extern int bcm_th_cosq_burst_monitor_set(int unit, int num_gports,
                                         bcm_gport_t *gport_list);
extern int bcm_th_cosq_burst_monitor_get(int unit, int max_gports,
                                         bcm_gport_t *gport_list,
                                         int *num_gports);
extern int bcm_th_cosq_burst_monitor_dma_config_reset(int unit,
                                                      uint8 wb_flag);

extern int
_bcm_field_th_ddrop_profile_alloc(int unit, _field_entry_t *f_ent,
                                   _field_action_t *fa);
extern int
_bcm_field_th_dredirect_profile_alloc(int unit, _field_entry_t *f_ent,
                                   _field_action_t *fa);

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
extern int
bcmi_ipmc_chn_repl_port_attach(int unit);

extern int
bcmi_ipmc_chn_repl_port_detach(int unit);

#endif


#endif /* BCM_TOMAHAWK_SUPPORT  */

#endif /* !_BCM_INT_TOMAHAWK_H_ */
