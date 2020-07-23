/*! \file bcmfp_internal.h
 *
 * APIs to manage all kinds of FP objects(control, stages, groups, entries,
 * qualifiers, actions, extractors)
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_INTERNAL_H
#define BCMFP_INTERNAL_H

#include <bcmfp/bcmfp_util_internal.h>
#include <bcmfp/bcmfp_group_internal.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_em_internal.h>
#include <bcmfp/bcmfp_vlan_internal.h>
#include <bcmfp/bcmfp_em_ft_internal.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmfp/bcmfp_pdd_internal.h>
#include <bcmcth/bcmcth_meter_internal.h>
#include <bcmfp/bcmfp_cth_range_check.h>
#include <bcmfp/bcmfp_cth_ctr.h>
#include <bcmfp/generated/bcmfp_ha.h>
#include <bcmfp/bcmfp_idp_internal.h>
#include <bcmfp/bcmfp_cth_info.h>
#include <bcmptm/bcmptm_uft.h>

/*!
 * \brief Attach device specific driver.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmfp_device_attach(int unit);

/*!
 * \brief Attach device specific driver.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmfp_compress_fid_hw_info_init(int unit);

/*!
 * \brief Attach variant specific driver.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmfp_variant_attach(int unit);

extern int
bcmfp_stages_create(int unit);

extern int
bcmfp_stages_destroy(int unit);

/*!
 * Umberella Structure for FP module. This structure points to
 * s/w state(Stored in HA and non HA memory) of different field
 * processors supported on the device.
 */
typedef struct bcmfp_control_s {
    /*! Ingress Field Processor s/w state */
    bcmfp_stage_t *ifp;

    /*! Vlan Field Processor s/w state */
    bcmfp_stage_t *vfp;

    /*! Egress Field Processor s/w state */
    bcmfp_stage_t *efp;

    /*! Exact Match Field Processor s/w state */
    bcmfp_stage_t *emfp;

    /*! Flow Tracker Exact Match Field Processor s/w state */
    bcmfp_stage_t *emft;

} bcmfp_control_t;

extern int
bcmfp_init(int unit, bool warm);

extern int
bcmfp_cleanup(int unit, bool warm);

extern int
bcmfp_imm_reg(int unit);

/*! Allocate and Initialize the memory required for
 *  per unit global variable of type bcmfp_control_t.
 */
extern int
bcmfp_common_init(int unit);

/*! Initialize FID info of fixed qualifiers for all stages.
 */
extern int
bcmfp_misc_init(int unit);

/*! De allocate the memory created for per unit global
 *  variable of type bcmfp_control_t.
 */
extern int
bcmfp_common_cleanup(int unit);

/*! Fetch the address to the FP module s/w state. */
extern int
bcmfp_control_get(int unit, bcmfp_control_t **fc);

/*! Assign the memory allocated for the given FP stage's s/w state in
 *- the given unit's FP module s/w state(of type bcmfp_control_t).
 */
extern int
bcmfp_stage_set(int unit, bcmfp_stage_id_t stage_id, bcmfp_stage_t *stage);

/*! Get the s/w state of the given stage and unit. */
extern int
bcmfp_stage_get(int unit, bcmfp_stage_id_t stage_id, bcmfp_stage_t **stage);

/*! Initialize the h/w specific attributes of the FP stage. */
extern int
bcmfp_stage_init(int unit, bcmfp_stage_t *stage, bool warm);

/*! Initialize the banks of FP exact match. */
extern int
bcmfp_em_bank_init(int unit, bcmfp_stage_t *stage);

/*! Deallocate the memory allocated for
 *- stage HW entry information. */
extern int
bcmfp_stage_hw_entry_info_cleanup(int unit, bcmfp_stage_t *stage);

/*! Deallocate the memory allocated for
 *- HW format information. */
extern int
bcmfp_stage_hw_field_info_cleanup(int unit, bcmfp_stage_t *stage);

/*! Deallocate the memory allocated for
 *- stage miscellaneous information. */
extern int
bcmfp_stage_misc_info_cleanup(int unit, bcmfp_stage_t *stage);

/*! Deallocate the memory allocated for
 *- PDD HW information. */
extern int
bcmfp_stage_pdd_hw_info_cleanup(int unit, bcmfp_stage_t *stage);

/*! Deallocate the memory allocated for
 *- SBR HW information. */
extern int
bcmfp_stage_sbr_hw_info_cleanup(int unit, bcmfp_stage_t *stage);

/*! Deallocate the memory(is part of HA area) allocated for
 *- stage operational information. */
extern int
bcmfp_stage_oper_info_cleanup(int unit, bcmfp_stage_t *stage);

/*! Assign the FP stage operational mode(Global/PipeLocal) */
extern int
bcmfp_stage_oper_mode_set(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_stage_oper_mode_t oper_mode);

/*! Get the FP stage operational mode(Global/PipeLocal) */
extern int
bcmfp_stage_oper_mode_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_stage_oper_mode_t *oper_mode);

/*! Initilize the extractors(or Multiplexers) available in all hirarchial
 *- levels of all group modes(SINGLE/DBLINTRA/DBLINTER/TRIPLE). In multiwide
 *- group modes(DBLINTER/TRIPLE) extractors for second and third slice will
 *- also be initialized in addition to the extractors of first slice. Memory
 *- allocated to hold this information is stored in non HA memory as this
 *- information will not be changed during the run time. So we build this
 *- information on every the SDK boot(both COLD and WARM boot).
 */
extern int
bcmfp_ext_cfg_db_init(int unit, bcmfp_stage_t *stage);

/*! Deallocate the memory allocated for extractor data base allocated in
 *- bcmfp_ext_cfg_db_init API.
 */
extern int
bcmfp_ext_cfg_db_cleanup(int unit, bcmfp_stage_t *stage);

/*! Initialize all the variables in bcmfp_ext_cfg_t structure to default
 *- values.
 */

/*! Initialize all the variables in bcmfp_ext_codes_t structure to default
 *- values.
 */
extern int
bcmfp_ext_codes_init(int unit, bcmfp_ext_codes_t *ext_codes);

extern int
bcmfp_qual_cfg_db_init(int unit, bcmfp_stage_t *stage);

extern int
bcmfp_qual_cfg_db_cleanup(int unit, bcmfp_stage_t *stage);

extern int
bcmfp_qual_cfg_t_init(int unit,
                      bcmfp_qual_cfg_t *qual_cfg);

extern int
bcmfp_presel_qual_cfg_t_init(int unit,
                      bcmfp_qual_cfg_t *presel_qual_cfg);

extern int
bcmfp_qual_cfg_insert(int unit,
                      bcmfp_stage_t *stage,
                      bcmfp_qualifier_t qid,
                      bcmfp_qual_cfg_t *new_qual_cfg);

extern int
bcmfp_presel_qual_cfg_insert(int unit,
                      bcmfp_stage_t *stage,
                      bcmfp_qualifier_t qid,
                      bcmfp_qual_cfg_t *new_qual_cfg);

extern int
bcmfp_presel_qual_cfg_db_cleanup(int unit,
                          bcmfp_stage_t *stage);

extern int
bcmfp_qual_set(int unit,
               bcmfp_stage_id_t stage_id,
               bcmfp_group_id_t group,
               bcmfp_group_oper_info_t *opinfo,
               bcmfp_qualifier_flags_t flags,
               bcmfp_qualifier_t qual,
               bcmltd_fid_t fid,
               uint16_t fidx,
               uint32_t *bitmap,
               uint32_t *data,
               uint32_t *mask,
               uint32_t **ekw);

extern int
bcmfp_qual_data_size_get(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_qualifier_t qual_id,
                         uint32_t *size);
extern int
bcmfp_ipbm_validate(int unit,
                    bcmfp_stage_id_t stage_id,
                    bcmfp_group_id_t group_id,
                    bcmdrd_pbmp_t device_pbmp);

extern int
bcmfp_qual_presel_set(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group,
                      bcmfp_group_oper_info_t *opinfo,
                      bcmfp_qualifier_t qual,
                      bcmltd_fid_t fid,
                      uint16_t fidx,
                      uint32_t *bitmap,
                      uint32_t *data,
                      uint32_t *mask,
                      uint32_t **ekw);

extern int
bcmfp_qual_presel_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group,
                      bcmfp_qualifier_t qual_id,
                      uint32_t *data,
                      uint32_t *mask,
                      uint32_t **ekw);

extern int
bcmfp_qualifier_enum_set(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_qualifier_t qual,
                         bool is_presel,
                         int value,
                         uint32_t *data,
                         uint32_t *mask);

extern int
bcmfp_qualifier_enum_get(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_qualifier_t qual,
                         bool is_presel,
                         uint32_t data,
                         uint32_t mask,
                         int *value);

extern int
bcmfp_action_cfg_db_init(int unit, bcmfp_stage_t *stage);

extern int
bcmfp_action_conflict_check(int unit,
                            bcmfp_stage_t *stage,
                            bcmfp_action_t action1,
                            bcmfp_action_t action2);

extern int
bcmfp_action_cfg_db_cleanup(int unit, bcmfp_stage_t *stage);

extern int
bcmfp_action_cfg_t_init(int unit,
                        bcmfp_action_cfg_t *action_cfg);

extern int
bcmfp_presel_action_cfg_t_init(int unit,
                        bcmfp_action_cfg_t *presel_action_cfg);
extern int
bcmfp_action_cfg_get(int unit,
                     bcmfp_stage_id_t stage_id,
                     bcmfp_action_t action,
                     bcmfp_action_cfg_t **action_cfg);
extern int
bcmfp_action_cfg_insert(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_action_t action,
                        bcmfp_action_cfg_t *action_cfg);

extern int
bcmfp_presel_action_cfg_insert(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_action_t action,
                        bcmfp_action_cfg_t *presel_action_cfg);
extern int
bcmfp_presel_action_cfg_db_cleanup(int unit,
                            bcmfp_stage_t *stage);
extern int
bcmfp_action_data_size_get(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_action_t action_id,
                           uint32_t *size);

extern int
bcmfp_action_value_set(int unit,
                       uint32_t *entbuf,
                       bcmfp_action_offset_cfg_t *a_cfg_ptr);
extern int
bcmfp_action_offset_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_action_t action,
                        bcmfp_action_offset_cfg_t *a_cfg_ptr);
/*!
 * \brief Extract the action offsets of a given
 * action and viewtype.
 *
 * \param [in]  unit            Logical device id
 * \param [in]  stage           BCMFP Stage.
 * \param [in]  action          action fid
 * \param [in]  action_cfg_ptr  ptr to the action_cfg of action
 * \param [in]  viewtype        viewtype of the action
 *
 * \retval SHR_E_NONE       Success
 * \retval SHR_E_NOT_FOUND  Action not present in the DB
 */
extern int
bcmfp_action_viewtype_offset_get(int unit,
                                 bcmfp_stage_t *stage,
                                 bcmfp_action_t action,
                                 bcmfp_action_offset_cfg_t *a_cfg_ptr,
                                 bcmfp_action_viewtype_t viewtype);

extern int
bcmfp_action_set(int unit,
                 uint32_t stage_id,
                 uint32_t group_id,
                 uint32_t action,
                 uint32_t param,
                 uint32_t **edw,
                 uint8_t  part_id);
/*!
 * \brief Build the edw of the policy entry
 * for the given action and viewtype.
 *
 * \param [in]  unit          Logical device id
 * \param [in]  stage_id      BCMFP Stage Id.
 * \param [in]  group_id      FP Group Id
 * \param [in]  action        action fid
 * \param [in]  param         value to be configured for the action
 * \param [in]  edw           data word for the policy entry
 * \param [in]  viewtype      viewtype of the policy entry
 * \param [in]  part_id         part to be configured with
 *                              action_data
 *
 * \retval SHR_E_NONE       Success
 * \retval SHR_E_NOT_FOUND  Action not present in the DB
 */
extern int
bcmfp_action_viewtype_set(int unit,
                          uint32_t stage_id,
                          uint32_t group_id,
                          uint32_t action,
                          uint32_t param,
                          uint32_t **edw,
                          bcmfp_action_viewtype_t viewtype,
                          uint8_t part_id);
extern int
bcmfp_pdd_action_set(int unit,
               bcmfp_stage_id_t stage_id,
               bcmfp_pdd_id_t pdd_id,
               bcmfp_group_id_t group_id,
               bcmfp_pdd_oper_type_t pdd_type,
               bcmfp_group_oper_info_t *opinfo,
               bcmfp_action_t action,
               uint32_t data,
               uint32_t **edw);

extern int
bcmfp_em_action_set(int unit,
                    bcmfp_stage_id_t stage_id,
                    bcmfp_group_id_t group_id,
                    bcmfp_pdd_oper_type_t pdd_type,
                    bcmfp_group_oper_info_t *opinfo,
                    bcmfp_pdd_id_t pdd_id,
                    bcmfp_action_t action,
                    uint32_t param,
                    uint32_t **edw,
                    uint32_t **qos);

extern int
bcmfp_action_param_check(int unit,
                         uint32_t group_id,
                         uint32_t stage_id,
                         uint32_t action,
                         uint32_t param);
extern int
bcmfp_presel_action_set(int unit,
                 uint32_t stage_id,
                 uint32_t action,
                 uint32_t param,
                 bcmfp_action_flags_t flags,
                 bcmfp_group_oper_info_t *opinfo,
                 uint32_t **edw);

extern int
bcmfp_fn_ptrs_cleanup(int unit, bcmfp_stage_t *stage);

extern int
bcmfp_group_parts_count(int unit,
                        bool is_presel,
                        uint32_t group_flags,
                        uint8_t *part_count);

/*!
 * \brief Get group flags from group mode.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage_id           BCMFP stage info.
 * \param [in] group_mode         Group mode.
 * \param [out] group_flags       Group flags.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_group_mode_to_flags_get(int unit,
                              bcmfp_stage_t *stage,
                              bcmfp_group_mode_t group_mode,
                              uint32_t *group_flags);
extern int
bcmfp_group_update(int unit, uint32_t trans_id,
                   bcmfp_group_t *fg);
extern int
bcmfp_group_delete(int unit, uint32_t trans_id,
                   bcmfp_stage_id_t stage_id,
                   bcmfp_group_id_t group_id);

extern int
bcmfp_group_selcode_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_group_oper_info_t *group_oper_info,
                        bcmptm_rm_tcam_fp_group_selcode_t *selcode);
extern int
bcmfp_group_combined_selcode_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_group_t *fg);
extern int
bcmfp_group_selcodes_install(int unit,
                             bcmfp_group_t *fg);

extern int
bcmfp_group_entry_install(int unit,
                          bcmfp_group_t *fg,
                          uint32_t **ekw,
                          uint32_t **edw);

extern int
bcmfp_group_oper_info_insert(int unit,
                             bcmfp_group_t *fg);

extern int
bcmfp_group_oper_info_delete(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_id_t group_id);

extern int
bcmfp_group_presel_entry_install(int unit,
                                 bcmfp_group_t *fg);

extern int
bcmfp_group_flags_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group_id,
                      uint32_t *group_flags);
extern int
bcmfp_group_tbl_inst_get(int unit,
                      bcmfp_group_id_t group_id,
                      bcmfp_stage_id_t stage_id,
                      int *tbl_inst);

extern int
bcmfp_group_qual_bitmap_get(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_id_t group_id,
                            bcmfp_qualifier_t qual,
                            uint32_t *qual_bitmap);

extern int
bcmfp_group_oper_info_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          bcmfp_group_oper_info_t **group_oper_info);

extern int
bcmfp_group_oper_info_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          bcmfp_group_oper_info_t **group_oper_info);

extern int
bcmfp_profiles_install(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       uint32_t flags,
                       bcmfp_stage_t *stage,
                       bcmfp_group_id_t group_id,
                       bcmfp_group_oper_info_t *opinfo,
                       bcmfp_buffers_t *buffers);

extern int
bcmfp_profiles_uninstall(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         uint32_t flags,
                         bcmfp_stage_t *stage,
                         bcmfp_group_id_t group_id,
                         bcmfp_group_oper_info_t *opinfo);

extern int
bcmfp_group_action_res_id_allocate(int unit,
                                   int tbl_inst,
                                   bcmfp_stage_id_t stage_id,
                                   uint8_t *action_res_id,
                                   uint32_t group_prio);
extern int
bcmfp_group_selcode_key_get(int unit,
                            bcmfp_stage_t *stage,
                            bcmfp_group_t *fg);
extern int
bcmfp_entry_key_match_type_set(int unit,
                               bcmfp_stage_t *stage,
                               bcmfp_group_id_t group_id,
                               bcmfp_group_oper_info_t *opinfo,
                               bcmfp_group_slice_mode_t slice_mode,
                               bcmfp_group_type_t  port_pkt_type,
                               uint32_t **ekw);
extern int
bcmfp_ptm_entry_insert(int unit,
                   uint32_t flags,
                   const bcmltd_op_arg_t *op_arg,
                   int tbl_inst,
                   bcmfp_stage_id_t stage_id,
                   bcmltd_sid_t req_ltid,
                   bcmfp_group_id_t group_id,
                   bcmfp_group_oper_info_t *opinfo,
                   bcmfp_entry_id_t entry_id,
                   uint32_t entry_prio,
                   uint32_t **ekw,
                   uint32_t **edw,
                   bcmfp_entry_profiles_t *ent_prof,
                   bool enable);
extern int
bcmfp_ptm_entry_delete(int unit,
                       uint32_t flags,
                       const bcmltd_op_arg_t *op_arg,
                       bcmfp_stage_id_t stage_id,
                       bcmltd_sid_t req_ltid,
                       bcmfp_group_id_t group_id,
                       bcmfp_group_oper_info_t *opinfo,
                       bcmfp_entry_id_t entry_id,
                       uint32_t **ekw,
                       bcmfp_entry_profiles_t *ent_prof);
extern int
bcmfp_ptm_entry_lookup(int unit,
                       uint32_t flags,
                       const bcmltd_op_arg_t *op_arg,
                       bcmfp_stage_id_t stage_id,
                       bcmltd_sid_t req_ltid,
                       bcmfp_entry_id_t group_id,
                       bcmfp_group_oper_info_t *opinfo,
                       bcmfp_entry_id_t entry_id,
                       bcmfp_buffers_t *req_buffers,
                       bcmfp_buffers_t *rsp_buffers);
extern int
bcmfp_ptm_group_lookup(int unit,
                       uint32_t flags,
                       const bcmltd_op_arg_t *op_arg,
                       bcmfp_stage_id_t stage_id,
                       bcmltd_sid_t req_ltid,
                       bcmfp_entry_id_t group_id,
                       bcmfp_group_oper_info_t *opinfo,
                       bcmfp_buffers_t *req_buffers,
                       bcmfp_buffers_t *rsp_buffers,
                       bcmptm_rm_tcam_entry_attrs_t *attrs);

extern int
bcmfp_ptm_stage_lookup(int unit,
                       uint32_t flags,
                       const bcmltd_op_arg_t *op_arg,
                       bcmfp_stage_id_t stage_id,
                       bcmltd_sid_t req_ltid,
                       bcmfp_buffers_t *req_buffers,
                       bcmfp_buffers_t *rsp_buffers,
                       bcmptm_rm_tcam_entry_attrs_t *attrs);

extern int
bcmfp_ptm_slice_lookup(int unit,
                       uint32_t flags,
                       const bcmltd_op_arg_t *op_arg,
                       bcmfp_stage_id_t stage_id,
                       bcmltd_sid_t req_ltid,
                       bcmfp_entry_id_t slice_id,
                       int pipe_id,
                       bcmfp_buffers_t *req_buffers,
                       bcmfp_buffers_t *rsp_buffers,
                       bcmptm_rm_tcam_entry_attrs_t *attrs);
extern int
bcmfp_filter_buffers_allocate(int unit,
                              uint32_t bflags,
                              bcmltd_sid_t tbl_id,
                              bcmfp_stage_t *stage,
                              bcmfp_buffers_t *buffers);
extern int
bcmfp_filter_buffers_free(int unit, bcmfp_buffers_t *buffers);

extern int
bcmfp_entry_valid_bit_set(int unit,
                          bool is_presel,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          bcmfp_group_oper_info_t *opinfo,
                          uint32_t **entry_words,
                          bool enable);
extern int
bcmfp_entry_key_info_get(int unit,
                         bool is_presel,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_group_id_t group_id,
                         bcmfp_group_oper_info_t *opinfo,
                         uint16_t *start_bit,
                         uint16_t *end_bit);
extern int
bcmfp_entry_mask_info_get(int unit,
                          bool is_presel,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          bcmfp_group_oper_info_t *opinfo,
                          uint16_t *start_bit,
                          uint16_t *end_bit);

extern int
bcmfp_group_mode_max_entries(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_mode_t group_mode,
                             uint32_t *max_entries);
extern int
bcmfp_max_non_conflicting_groups_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_mode_t group_mode,
                             uint32_t *max_groups);
extern int
bcmfp_group_tcam_size_get(int unit,
                          bool is_presel,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          bcmfp_group_oper_info_t *opinfo,
                          uint32_t *size);
extern int
bcmfp_group_policy_size_get(int unit,
                            bool is_presel,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_id_t group_id,
                            bcmfp_group_oper_info_t *opinfo,
                            uint32_t *size);

extern int
bcmfp_profile_add_range(int unit,
                        uint32_t trans_id,
                        int tbl_inst,
                        bcmltd_sid_t req_ltid,
                        uint8_t num_profiles,
                        bcmdrd_sid_t *profile_sids,
                        uint32_t *profile_data[],
                        bcmfp_ref_count_t *ref_counts,
                        int *profile_index,
                        int max_idx,
                        int min_idx);
extern int
bcmfp_profile_add(int unit,
                  uint32_t trans_id,
                  int tbl_inst,
                  bcmltd_sid_t req_ltid,
                  uint8_t num_profiles,
                  bcmdrd_sid_t *profile_sids,
                  uint32_t *profile_data[],
                  bcmfp_ref_count_t *ref_counts,
                  int *profile_index);
extern int
bcmfp_profile_delete(int unit,
                     uint32_t trans_id,
                     int tbl_inst,
                     bcmltd_sid_t req_ltid,
                     uint8_t num_profiles,
                     bcmltd_sid_t *profile_sids,
                     bcmfp_ref_count_t *ref_counts,
                     int profile_index);

extern int
bcmfp_profile_index_alloc(int unit,
                          uint32_t trans_id,
                          int tbl_inst,
                          bcmltd_sid_t req_ltid,
                          uint8_t num_profiles,
                          bcmdrd_sid_t *profile_sids,
                          uint32_t *profile_data[],
                          bcmfp_ref_count_t *ref_counts,
                          int *index,
                          int max_idx,
                          int min_idx);
extern int
bcmfp_udf_qual_id_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      uint8_t udf_chunk_id,
                      bcmfp_qualifier_t *qual_id);

extern int
bcmfp_src_class_mode_set(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmfp_stage_id_t stage_id,
                         bcmltd_sid_t sid,
                         int pipe_id,
                         uint32_t mode);
extern int
bcmfp_entry_qos_profile_id_get(int unit,
                            uint32_t trans_id,
                            bcmfp_stage_t *stage,
                            bcmfp_group_id_t group_id,
                            uint32_t **edw,
                            int *qos_prof_idx);
extern int
bcmfp_buffers_customize(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        uint32_t flags,
                        bcmfp_stage_t *stage,
                        bcmfp_group_oper_info_t *opinfo,
                        bcmfp_pdd_config_t *pdd_config,
                        bcmfp_key_ids_t *key_ids,
                        bcmfp_buffers_t *buffers);

extern int
bcmfp_group_presel_entry_validate(int unit,
                         int tbl_inst,
                         bcmfp_stage_id_t stage_id,
                         uint8_t lookup_id,
                         uint8_t num_presel_ids);

/* Returns a string name corresponding to action enumeration.
 * This is mostly used in LOG messages.
 */
extern char *bcmfp_action_string(bcmfp_action_t action);

/*!
 * \brief Get the maximum number of entries that can be created in
 *        FP_XXX_ENTRY LT in a stage.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [out] max_entries maximum index id in FP_XXX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM max_entries is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_stage_max_entries_get(int unit,
                            bcmfp_stage_id_t stage_id,
                            uint32_t *max_entries);

/*!
 * \brief Get the maximum value of PDD id that can be used in
 *        FP_XXX_GRP_TEMPLATE LT or FP_XXX_ENTRY LT in a stage.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [out] max_pdd_id maximum pdd id in FP_XXX_ENTRY
 *              or FP_XXX_GRP_TEMPLATE LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM max_pdd_id is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_stage_max_pdd_id_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t *max_pdd_id);

/*!
 * \brief Get the maximum value of meter id that can be used in
 *        FP_XXX_ENTRY LT in a stage.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [out] max_meter_id maximum meter id in FP_XXX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM max_meter_id is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_stage_max_meter_id_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t *max_meter_id);

/*!
 * \brief Get the maximum value of group id that can be used in
 *        FP_XXX_ENTRY LT in a stage.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [out] max_group_id maximum group id in FP_XXX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM max_group_id is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_stage_max_group_id_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t *max_group_id);

/*!
 * \brief Get the maximum value of presel id that can be configured.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [out] max_presel_id maximum presel id.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM max_presel_id is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_stage_max_presel_id_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t *max_presel_id);

/*!
 * \brief Get the maximum value of egress counter ID that can be used in
 *        FP_XXX_ENTRY LT in a stage.
 *
 * \param [in] unit Logical device ID
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [out] max_ctr_id maximum counter ID in FP_XXX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM max_ctr_id is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_stage_max_ctr_id_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t *max_ctr_id);

/*!
 * \brief Get the maximum value of egress counter ID that can be used in
 *        CTR_EGR_FP_ENTRY LT in a stage.
 *
 * \param [in] unit Logical device ID
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_EGRESS).
 * \param [out] max_ctr_pool_id maximum counter pool ID in CTR_EGR_FP_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM max_ctr_pool_id is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_stage_max_ctr_pool_id_get(int unit,
                                bcmfp_stage_id_t stage_id,
                                uint32_t *max_ctr_pool_id);


/*!
 * \brief Get the maximum value of presel id that can be configured.
 *
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [out] max_presel_id maximum presel id.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM max_presel_id is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_stage_max_presel_id_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t *max_presel_id);
/*!
 * \brief Get the maximum value of group id that can be used in
 *        FP_XXX_ENTRY LT in a stage.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [out] max_group_id maximum group id in FP_XXX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM max_group_id is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_stage_max_group_id_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t *max_group_id);

/*!
 * \brief Get the maximum value of PSG id that can be used in
 *        FP_XXX_GRP_TEMPLATE LT or FP_XXX_ENTRY LT in a stage.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [out] max_psg_id maximum psg id in FP_XXX_ENTRY
 *              or FP_XXX_GRP_TEMPLATE LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM max_psg_id is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_stage_max_psg_id_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t *max_psg_id);

/*!
 * \brief Get the maximum value of SBR id that can be used in
 *        FP_XXX_GRP_TEMPLATE LT or FP_XXX_ENTRY LT in a stage.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [out] max_sbr_id maximum sbr id in FP_XXX_ENTRY
 *              or FP_XXX_GRP_TEMPLATE LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM max_sbr_id is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_stage_max_sbr_id_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t *max_sbr_id);
/*!
 * \brief Add entry id to given rule template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] rule_template_id Index to FP_XXX_RULE_TEMPLATE LT.
 * \param [in] entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_rule_entry_map_add(int unit,
                         bcmfp_stage_id_t stage_id,
                         uint32_t rule_template_id,
                         uint32_t entry_id);

/*!
 * \brief Delete entry id from given rule template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] rule_template_id Index to FP_XXX_RULE_TEMPLATE LT.
 * \param [in] entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_rule_entry_map_delete(int unit,
                            bcmfp_stage_id_t stage_id,
                            uint32_t rule_template_id,
                            uint32_t entry_id);

/*!
 * \brief Loop through all entry ids associated to a given
 *  rule template id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] rule_template_id Index to FP_XXX_RULE_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_rule_entry_map_traverse(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint32_t rule_template_id,
                              bcmfp_entry_id_traverse_cb cb,
                              void *user_data);


/*!
 * \brief Check whether rule template id is associated to any
 *  entry id(s).
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] rule_template_id Index to FP_XXX_RULE_TEMPLATE LT.
 * \param [in] not_mapped Flag to indicate map status.
 *             TRUE = rule template id not mapped to entry id(s).
 *             FLASE = rule template id mapped to entry id(s).
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_rule_entry_map_check(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t rule_template_id,
                           bool *not_mapped);

/*!
 * \brief Add entry id to given policy template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] policy_template_id Index to FP_XXX_POLICY_TEMPLATE LT.
 * \param [in] entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_policy_entry_map_add(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t policy_template_id,
                           uint32_t entry_id);

/*!
 * \brief Delete entry id from given policy template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] policy_template_id Index to FP_XXX_POLICY_TEMPLATE LT.
 * \param [in] entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_policy_entry_map_delete(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint32_t policy_template_id,
                              uint32_t entry_id);

/*!
 * \brief Loop through all entry ids associated to a given
 *  policy template id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] policy_template_id Index to FP_XXX_POLICY_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_policy_entry_map_traverse(int unit,
                                bcmfp_stage_id_t stage_id,
                                uint32_t policy_template_id,
                                bcmfp_entry_id_traverse_cb cb,
                                void *user_data);

/*!
 * \brief Check whether policy template id is associated to any
 *  entry id(s).
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] policy_template_id Index to FP_XXX_POLICY_TEMPLATE LT.
 * \param [in] not_mapped Flag to indicate map status.
 *             TRUE = rule template id not mapped to entry id(s).
 *             FLASE = rule template id mapped to entry id(s).
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_policy_entry_map_check(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t policy_template_id,
                             bool *not_mapped);
/*!
 * \brief Add group id to given default policy template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] policy_template_id Index to FP_XXX_POLICY_TEMPLATE LT.
 * \param [in] entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_dpolicy_group_map_add(int unit,
                            bcmfp_stage_id_t stage_id,
                            uint32_t policy_template_id,
                            uint32_t group_id);

/*!
 * \brief Delete group id from given default policy template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] policy_template_id Index to FP_XXX_POLICY_TEMPLATE LT.
 * \param [in] entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_dpolicy_group_map_delete(int unit,
                               bcmfp_stage_id_t stage_id,
                               uint32_t policy_template_id,
                               uint32_t group_id);

/*!
 * \brief Loop through all group ids associated to a given
 *  default policy template id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] policy_template_id Index to FP_XXX_POLICY_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_dpolicy_group_map_traverse(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 uint32_t policy_template_id,
                                 bcmfp_entry_id_traverse_cb cb,
                                 void *user_data);

/*!
 * \brief Check whether default policy template id is associated to any
 *  entry id(s).
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] policy_template_id Index to FP_XXX_POLICY_TEMPLATE LT.
 * \param [in] not_mapped Flag to indicate map status.
 *             TRUE = rule template id not mapped to entry id(s).
 *             FLASE = rule template id mapped to entry id(s).
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_dpolicy_group_map_check(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint32_t policy_template_id,
                              bool *not_mapped);
/*!
 * \brief Add entry id to given meter template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] meter_template_id Index to METER_FP_XXX_TEMPLATE LT.
 * \param [in] entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_meter_entry_map_add(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t meter_template_id,
                          uint32_t entry_id);

/*!
 * \brief Delete entry id from given meter template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] meter_template_id Index to METER_FP_XXX_TEMPLATE LT.
 * \param [in] entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_meter_entry_map_delete(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t meter_template_id,
                             uint32_t entry_id);

/*!
 * \brief Loop through all entry ids associated to a given
 *  meter template id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] meter_template_id Index to METER_FP_XXX_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_meter_entry_map_traverse(int unit,
                               bcmfp_stage_id_t stage_id,
                               uint32_t meter_template_id,
                               bcmfp_entry_id_traverse_cb cb,
                               void *user_data);

/*!
 * \brief Check whether meter template id is associated to any
 *  entry id(s).
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] meter_template_id Index to METER_FP_XXX_TEMPLATE LT.
 * \param [in] not_mapped Flag to indicate map status.
 *             TRUE = rule template id not mapped to entry id(s).
 *             FLASE = rule template id mapped to entry id(s).
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_meter_entry_map_check(int unit,
                            bcmfp_stage_id_t stage_id,
                            uint32_t meter_template_id,
                            bool *not_mapped);

/*!
 * \brief Add group id to given presel template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] pse_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] group_id Index to FP_XXX_GRP_TEMPLATE LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_pse_group_map_add(int unit,
                        bcmfp_stage_id_t stage_id,
                        uint32_t pse_id,
                        uint32_t group_id);

/*!
 * \brief Delete group id from given presel template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] presel_id Index to FP_XXX_PRESEL_TEMPLATE.
 * \param [in] group_id Index to FP_XXX_GRP_TEMPLATE LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_pse_group_map_delete(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t pse_id,
                           uint32_t group_id);

/*!
 * \brief Loop through all groups ids associated to a given
 * presel template id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] pse_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_pse_group_map_traverse(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t pse_id,
                             bcmfp_entry_id_traverse_cb cb,
                             void *user_data);

/*!
 * \brief Loop through all groups ids associated to a given
 * presel template id till condition met is set to TRUE.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] pse_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 * \param [in] cond_met stop the traverse when this status is set to true.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_pse_group_map_cond_traverse(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  uint32_t pse_id,
                                  bcmfp_entry_id_cond_traverse_cb cb,
                                  void *user_data,
                                  bool *cond_met);

/*!
 * \brief Check whether presel template id is associated to any
 *  group id(s).
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] pse_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] not_mapped Flag to indicate map status.
 *             TRUE = presel id not mapped to grouo id(s).
 *             FLASE = presel id mapped to group id(s).
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_pse_group_map_check(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t pse_id,
                          bool *not_mapped);

/*!
 * \brief Get the count of groups with given priority associated with the given
 * presel ID.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] presel_id Presel ID.
 * \param [in] group_priority Group priority.
 * \param [out] group_count Pointer to the group count to be updated.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_presel_same_prio_grp_cnt_get(int unit,
                                   bcmfp_stage_id_t stage_id,
                                   uint32_t presel_id,
                                   uint32_t group_priority,
                                   uint16_t *group_count);

/*!
 * \brief Add entry id to given ctr_egr template ID mapping.
 *
 * \param [in] unit Logical device ID
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] ctr_egr_template_id Index to CTR_EGR_FP_ENTRY LT.
 * \param [in] entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_ctr_entry_map_add(int unit,
                        bcmfp_stage_id_t stage_id,
                        uint32_t ctr_egr_template_id,
                        uint32_t entry_id);

/*!
 * \brief Delete entry id from given ctr_egr template id mapping.
 *
 * \param [in] unit Logical device ID
 * \param [in] stage_id stage ID(BCMFP_STAGE_ID_XXX).
 * \param [in] ctr_egr_template_id Index to CTR_EGR_FP_ENTRY LT.
 * \param [in] entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_ctr_entry_map_delete(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t ctr_egr_template_id,
                           uint32_t entry_id);

/*!
 * \brief Loop through all entry IDs associated to a given
 *  ctr_egr template id.
 *
 * \param [in] unit Logical device ID
 * \param [in] stage_id stage ID(BCMFP_STAGE_ID_XXX).
 * \param [in] ctr_egr_template_id Index to CTR_EGR_FP_ENTRY LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_ctr_entry_map_traverse(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t ctr_egr_template_id,
                             bcmfp_entry_id_traverse_cb cb,
                             void *user_data);

/*!
 * \brief Check whether ctr_egr template ID is associated to any
 *  entry id(s).
 *
 * \param [in] unit Logical device ID
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] ctr_egr_template_id Index to CTR_EGR_FP_ENTRY LT.
 * \param [in] not_mapped Flag to indicate map status.
 *             TRUE = rule template ID not mapped to entry id(s).
 *             FLASE = rule template ID mapped to entry id(s).
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_ctr_entry_map_check(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t ctr_egr_template_id,
                          bool *not_mapped);
/*!
 * \brief Update the operational status of the entry in
 *  FP_XXX_ENTRY LT.
 *
 * \param [in] unit Logical device ID
 * \param [in] stage_id stage ID(BCMFP_STAGE_ID_XXX).
 * \param [in] entry_id Index to FP_XXX_ENTRY LT.
 * \param [in] oper_status Operational status of the entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_entry_oper_state_update(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint32_t entry_id,
                              uint32_t oper_status);

/*!
 * \brief Get the operational status array of FP_XXX_ENTRY LT.
 *
 * \param [in] unit Logical device ID
 * \param [in] stage_id stage ID(BCMFP_STAGE_ID_XXX).
 * \param [out] oper_status_array Pointer to operational
 *              status.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_entry_oper_state_array_get(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 uint32_t **oper_status_array);
/*!
 * \brief Set the operational state of the entries in the current
 *  transaction in IMM.
 *
 * \param [in] unit Logical device ID
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_stage_entry_oper_state_commit(int unit,
                                   bcmfp_stage_id_t stage_id);
/*!
 * \brief Fill the presel entry data words to be written to HW.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] group_id Groups unique id.
 * \param [in] opinfo Group operational information.
 * \param [out] edw Presel entry data words buffers.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_presel_edw_get(int unit,
                     bcmfp_stage_id_t stage_id,
                     bcmfp_group_id_t group_id,
                     bcmfp_group_oper_info_t *opinfo,
                     uint32_t **edw);
/*!
 * \brief Fill the PDD profile entry data words to be written to HW.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] pdd_id PDD's unique id.
 * \param [in] group_id Groups unique id.
 * \param [in] opinfo Group operational information.
 * \param [in] pdd_type PDD oper type.
 * \param [out] PDD profile entry data words buffers.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_pdd_profile_entry_get(int unit,
                   const bcmltd_op_arg_t *op_arg,
                   bcmfp_stage_t *stage,
                   bcmfp_pdd_id_t pdd_id,
                   bcmfp_group_id_t group_id,
                   bcmfp_group_oper_info_t *opinfo,
                   bcmfp_pdd_oper_type_t pdd_type,
                   uint32_t **pdd_profile,
                   bcmfp_pdd_config_t *pdd_config);
/*!
 * \brief Fill the group presel group controls data words
 *  to be written to HW.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] group_id Presel group id.
 * \param [out] presel_group_info Presel group data words buffers.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_presel_group_info_get(int unit,
                     bcmfp_stage_id_t stage_id,
                     bcmfp_group_id_t group_id,
                     uint32_t **presel_group_info);

/*!
 * \brief  Enable or disable control for color dependent FP actions.
 *
 * \param [in] unit      Logical device id
 * \param [in] stage_id  Stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] edw       Entry data word.
 * \param [in] value     Value to enable or disable control.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_INTERNAL Invalid Data
 */

int
bcmfp_colored_actions_enable(int unit,
                             uint32_t stage_id,
                             uint32_t **edw,
                             uint32_t value);
/*!
 * \brief  To install default policy attached to EM group.
 *
 * \param [in] unit             Logical device id
 * \param [in] trans_id         Transaction ID
 * \param [in] stage            Stage structure.
 * \param [in] fg               group structure.
 * \param [in] policy_config    Default Policy Configuration
 * \param [in] edw              Entry data word.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_INTERNAL Invalid Data
 */
int
bcmfp_group_default_policy_process(int unit,
                                   uint32_t trans_id,
                                   bcmfp_stage_t *stage,
                                   bcmfp_group_t *fg,
                                   uint32_t **edw);
/*!
 * \brief  To install a range check group entry.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transaction ID.
 * \param [in] stage_id           Stage ID.
 * \param [in] sid                Logical table id.
 * \param [in] pipe_id            Instance.
 * \param [in] range_check_grp_id Range Check Group Number.
 * \param [in] bmp                Range group Bitmap.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_INTERNAL Invalid Data
 */
int
bcmfp_range_check_grp_set(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmfp_stage_id_t stage_id,
                         bcmltd_sid_t sid,
                         int pipe_id,
                         int range_check_grp_id,
                         bcmfp_range_check_id_bmp_t bmp);
/*!
 * \brief  To update EDW with meter actions data.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage              Stage Structure.
 * \param [in] group_id           group ID.
 * \param [in] pdd_id             Pdd entry Instance.
 * \param [in] opinfo             Group operational information.
 * \param [in] meter_entry        Meter entry Structure.
 * \param [in/out] edw            Update meter actions in edw.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_INTERNAL Invalid Data
 */
extern int
bcmfp_meter_actions_set(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_group_id_t group_id,
                        bcmfp_pdd_id_t pdd_id,
                        bcmfp_group_oper_info_t *opinfo,
                        bcmcth_meter_fp_policy_fields_t *meter_entry,
                        uint32_t **edw);
/*!
 * \brief  To update EDW with ctr actions data.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage              Stage Structure.
 * \param [in] group_id           group ID.
 * \param [in] pdd_id             Pdd entry Instance.
 * \param [in] opinfo             Group operational information.
 * \param [in] ctr_params         counter config Structure.
 * \param [in/out] edw            Update ctr actions in edw.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_INTERNAL Invalid Data
 */
extern int
bcmfp_counter_actions_set(int unit,
                          bcmfp_stage_t *stage,
                          bcmfp_group_id_t group_id,
                          bcmfp_pdd_id_t pdd_id,
                          bcmfp_group_oper_info_t *opinfo,
                          bcmfp_ctr_params_t *ctr_params,
                          uint32_t **edw);
/*!
 * \brief  To allocate EFP counter index based on the counter pool_id and color.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage_id           Stage ID.
 * \param [in] ctr_id             Counter ID.
 * \param [in] pipe_id            Pipe ID.
 * \param [in] pool_id            Pool ID.
 * \param [in] color              Color.
 * \param [in] event_reason       Event.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_INTERNAL Invalid Data
 */
extern int
bcmfp_counter_hw_idx_set(int unit,
                         bcmfp_stage_t *stage,
                         uint32_t ctr_id,
                         uint32_t pipe_id,
                         uint32_t pool_id,
                         int color,
                         bcmimm_entry_event_t event_reason);
extern int
bcmfp_bank_sids_get(int unit,
                    bcmfp_stage_t *stage,
                    uint32_t bank_sid,
                    uint32_t *ptinfo);

/*!
 * \brief  Initialize the LRD information maintained
 *  locally by BCMFP module. This is done for faster
 *  accessing of LRD info.
 *
 * \param [in] unit       Logical device id.
 * \param [in] stage_id   BCMFP stage pointer.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters
 */
extern int
bcmfp_lrd_info_init(int unit,
                    bcmfp_stage_t *stage);

extern int
bcmfp_em_entry_build_ekw(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t req_ltid,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_group_id_t group_id,
                         bcmfp_group_oper_info_t *opinfo,
                         bcmfp_entry_id_t entry_id,
                         bcmfp_group_config_t *group_config,
                         bcmfp_rule_config_t *rule_config,
                         uint32_t **ekw);
/*!
 * \brief Get the array of compress types expected to be
 *  applied on the group.
 *
 * \param [in] unit             Logical device id.
 * \param [in] stage_id         BCMFP stage ID.
 * \param [in] group_id         BCMFP group ID.
 * \param [in] compress_types   FALSE/TRUE flag for different
 *                              compress types.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters
 */
extern int
bcmfp_group_compress_types_get(int unit,
                               bcmfp_stage_id_t stage_id,
                               bcmfp_group_id_t group_id,
                               bool **compress_types);
/*!
 * \brief Create the compression tries.
 *
 * \param [in] unit       Logical device id.
 * \param [in] stage      BCMFP stage structure.
 * \param [in] cb_func    Callback function to be
 *                        called when there is a
 *                        change in cid/cid_mask
 *                        of existing ALPM keys.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters
 */
extern int
bcmfp_compress_init(int unit,
                    bcmfp_stage_t *stage,
                    bcmfp_fn_cid_update_t cb_func);
/*!
 * \brief Deallocate the compression tries.
 *
 * \param [in] unit       Logical device id.
 * \param [in] stage      BCMFP stage structure.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters
 */
extern int
bcmfp_compress_cleanup(int unit,
                       bcmfp_stage_t *stage);
/*!
 * \brief Status of whether group has to be compressed
 *  or not.
 *
 * \param [in] unit             Logical device id.
 * \param [in] stage_id         BCMFP stage ID.
 * \param [in] group_id         BCMFP group ID.
 * \param [in] compress_enabled Group has to be compressed
 *                              or not?
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters
 */
extern int
bcmfp_group_compress_enabled(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_id_t group_id,
                             bool *compress_enabled);
/*!
 * \brief Checks whether a given qualifier fid in given
 *  sid_type is supported or not.
 *
 * \param [in] unit         Logical device id.
 * \param [in] stage_id     BCMFP stage ID.
 * \param [in] sid_type     BCMFP LT type.
 * \param [in] table_name   BCMFP LT name.
 * \param [in] fid          Qualifier FID
 * \param [in] field_name   BCMFP LT field name.
 * \param [out] supported   Flag to indicate the qualifier
 *                          is supported or not.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters
 */
extern int
bcmfp_qualifier_is_supported(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_sid_type_t sid_type,
                             const char *table_name,
                             bcmltd_fid_t fid,
                             const char *field_name,
                             bool *supported);
/*!
 * \brief Checks whether a given action fid in given
 *  sid_type is supported or not.
 *
 * \param [in] unit         Logical device id.
 * \param [in] stage_id     BCMFP stage ID.
 * \param [in] sid_type     BCMFP LT type.
 * \param [in] table_name   BCMFP LT name.
 * \param [in] fid          Action FID
 * \param [in] field_name   BCMFP LT field name.
 * \param [out] supported   Flag to indicate the action
 *                          is supported or not.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters
 */
extern int
bcmfp_action_is_supported(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_sid_type_t sid_type,
                          const char *table_name,
                          bcmltd_fid_t fid,
                          const char *field_name,
                          bool *supported);
/*!
 * \brief Get the field ID from field name in an LT.
 *
 * \param [in] unit         Logical device id.
 * \param [in] table_name   FP LT name.
 * \param [in] field_name   FP LT field name.
 * \param [out] fid         FP LT field ID.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters
 */
extern int
bcmfp_field_name_to_id_get(int unit,
                           const char *table_name,
                           const char *field_name,
                           bcmltd_fid_t *fid);
/*!
 * \brief Get the field name from field ID in an LT.
 *
 * \param [in] unit         Logical device id.
 * \param [in] table_name   FP LT name.
 * \param [in] fid          FP LT field ID.
 * \param [out] field_name  FP LT field name.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters
 */
extern int
bcmfp_field_id_to_name_get(int unit,
                           const char *table_name,
                           bcmltd_fid_t fid,
                           char *field_name);
/*!
 * \brief Add group id to given pdd template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] pdd_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] group_id Index to FP_XXX_GRP_TEMPLATE LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_pdd_group_map_add(int unit,
                        bcmfp_stage_id_t stage_id,
                        uint32_t pdd_id,
                        uint32_t group_id);

/*!
 * \brief Delete group id from given pdd template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] presel_id Index to FP_XXX_PRESEL_TEMPLATE.
 * \param [in] group_id Index to FP_XXX_GRP_TEMPLATE LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_pdd_group_map_delete(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t pdd_id,
                           uint32_t group_id);

/*!
 * \brief Loop through all groups ids associated to a given
 * pdd template id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] pdd_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_pdd_group_map_traverse(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t pdd_id,
                             bcmfp_entry_id_traverse_cb cb,
                             void *user_data);
/*!
 * \brief Check whether pdd template id is associated to any
 *  group id(s).
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] pdd_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] not_mapped Flag to indicate map status.
 *             TRUE = presel id not mapped to grouo id(s).
 *             FLASE = presel id mapped to group id(s).
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_pdd_group_map_check(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t pdd_id,
                          bool *not_mapped);

/*!
 * \brief Add entry id to given pdd template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] pdd_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] entry_id Index to FP_XXX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_pdd_entry_map_add(int unit,
                        bcmfp_stage_id_t stage_id,
                        uint32_t pdd_id,
                        uint32_t entry_id);

/*!
 * \brief Delete entry id from given pdd template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] presel_id Index to FP_XXX_PRESEL_TEMPLATE.
 * \param [in] entry_id Index to FP_XXX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_pdd_entry_map_delete(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t pdd_id,
                           uint32_t entry_id);

/*!
 * \brief Loop through all entry ids associated to a given
 * pdd template id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] pdd_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_pdd_entry_map_traverse(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t pdd_id,
                             bcmfp_entry_id_traverse_cb cb,
                             void *user_data);
/*!
 * \brief Check whether pdd template id is associated to any
 *  entry id(s).
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] pdd_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] not_mapped Flag to indicate map status.
 *             TRUE = presel id not mapped to grouo id(s).
 *             FLASE = presel id mapped to group id(s).
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_pdd_entry_map_check(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t pdd_id,
                          bool *not_mapped);
/*!
 * \brief Add group id to given default pdd template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] pdd_id Index to FP_XXX_PDD_TEMPLATE LT.
 * \param [in] group_id Index to FP_XXX_GRP_TEMPLATE LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_dpdd_group_map_add(int unit,
                         bcmfp_stage_id_t stage_id,
                         uint32_t pdd_id,
                         uint32_t group_id);

/*!
 * \brief Delete group id from given default pdd template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] presel_id Index to FP_XXX_PDD_TEMPLATE.
 * \param [in] group_id Index to FP_XXX_GRP_TEMPLATE LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_dpdd_group_map_delete(int unit,
                            bcmfp_stage_id_t stage_id,
                            uint32_t pdd_id,
                            uint32_t group_id);

/*!
 * \brief Loop through all groups ids associated to a given
 * default pdd template id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] pdd_id Index to FP_XXX_PDD_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_dpdd_group_map_traverse(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint32_t pdd_id,
                              bcmfp_entry_id_traverse_cb cb,
                              void *user_data);
/*!
 * \brief Check whether default pdd template id is associated to any
 *  group id(s).
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] pdd_id Index to FP_XXX_PDD_TEMPLATE LT.
 * \param [in] not_mapped Flag to indicate map status.
 *             TRUE = presel id not mapped to grouo id(s).
 *             FLASE = presel id mapped to group id(s).
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_dpdd_group_map_check(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t pdd_id,
                           bool *not_mapped);
/*!
 * \brief Add group id to given sbr template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] sbr_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] group_id Index to FP_XXX_GRP_TEMPLATE LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_sbr_group_map_add(int unit,
                        bcmfp_stage_id_t stage_id,
                        uint32_t sbr_id,
                        uint32_t group_id);

/*!
 * \brief Delete group id from given sbr template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] presel_id Index to FP_XXX_PRESEL_TEMPLATE.
 * \param [in] group_id Index to FP_XXX_GRP_TEMPLATE LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_sbr_group_map_delete(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t sbr_id,
                           uint32_t group_id);

/*!
 * \brief Loop through all groups ids associated to a given
 * sbr template id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] sbr_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 * \param [in] cond_met traverse till cond_met is true.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_sbr_group_map_cond_traverse(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  uint32_t sbr_id,
                                  bcmfp_entry_id_cond_traverse_cb cb,
                                  void *user_data,
                                  bool *cond_met);
/*!
 * \brief Loop through all groups ids associated to a given
 * sbr template id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] sbr_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_sbr_group_map_traverse(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t sbr_id,
                             bcmfp_entry_id_traverse_cb cb,
                             void *user_data);
/*!
 * \brief Check whether sbr template id is associated to any
 *  group id(s).
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] sbr_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] not_mapped Flag to indicate map status.
 *             TRUE = presel id not mapped to grouo id(s).
 *             FLASE = presel id mapped to group id(s).
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_sbr_group_map_check(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t sbr_id,
                          bool *not_mapped);

/*!
 * \brief Loop through all groups ids associated to a given
 * sbr template id and get the first one.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] sbr_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_sbr_group_map_traverse_first(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t sbr_id,
                             bcmfp_entry_id_traverse_cb cb,
                             void *user_data);
/*!
 * \brief Add entry id to given sbr template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] sbr_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] entry_id Index to FP_XXX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_sbr_entry_map_add(int unit,
                        bcmfp_stage_id_t stage_id,
                        uint32_t sbr_id,
                        uint32_t entry_id);

/*!
 * \brief Delete entry id from given sbr template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] presel_id Index to FP_XXX_PRESEL_TEMPLATE.
 * \param [in] entry_id Index to FP_XXX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_sbr_entry_map_delete(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t sbr_id,
                           uint32_t entry_id);

/*!
 * \brief Loop through all entry ids associated to a given
 * sbr template id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] sbr_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 * \param [in] cond_met tarverse till cond_met is true.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_sbr_entry_map_cond_traverse(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  uint32_t sbr_id,
                                  bcmfp_entry_id_cond_traverse_cb cb,
                                  void *user_data,
                                  bool *cond_met);
/*!
 * \brief Loop through all entry ids associated to a given
 * sbr template id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] sbr_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_sbr_entry_map_traverse(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t sbr_id,
                             bcmfp_entry_id_traverse_cb cb,
                             void *user_data);
/*!
 * \brief Check whether sbr template id is associated to any
 *  entry id(s).
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] sbr_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] not_mapped Flag to indicate map status.
 *             TRUE = presel id not mapped to grouo id(s).
 *             FLASE = presel id mapped to group id(s).
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_sbr_entry_map_check(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t sbr_id,
                          bool *not_mapped);

/*!
 * \brief Loop through all entry ids associated to a given
 * sbr template id and get the first one.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] sbr_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_sbr_entry_map_traverse_first(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t sbr_id,
                             bcmfp_entry_id_traverse_cb cb,
                             void *user_data);
/*!
 * \brief Add group id to given default sbr template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] sbr_id Index to FP_XXX_SBR_TEMPLATE LT.
 * \param [in] group_id Index to FP_XXX_GRP_TEMPLATE LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_dsbr_group_map_add(int unit,
                         bcmfp_stage_id_t stage_id,
                         uint32_t sbr_id,
                         uint32_t group_id);

/*!
 * \brief Delete group id from given default sbr template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] presel_id Index to FP_XXX_SBR_TEMPLATE.
 * \param [in] group_id Index to FP_XXX_GRP_TEMPLATE LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_dsbr_group_map_delete(int unit,
                            bcmfp_stage_id_t stage_id,
                            uint32_t sbr_id,
                            uint32_t group_id);

/*!
 * \brief Loop through all groups ids associated to a given
 * default sbr template id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] sbr_id Index to FP_XXX_SBR_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_dsbr_group_map_traverse(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint32_t sbr_id,
                              bcmfp_entry_id_traverse_cb cb,
                              void *user_data);
/*!
 * \brief Check whether default sbr template id is associated to any
 *  group id(s).
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] sbr_id Index to FP_XXX_SBR_TEMPLATE LT.
 * \param [in] not_mapped Flag to indicate map status.
 *             TRUE = presel id not mapped to grouo id(s).
 *             FLASE = presel id mapped to group id(s).
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_dsbr_group_map_check(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t sbr_id,
                           bool *not_mapped);

/*!
 * \brief Loop through all groups ids associated to a given
 * default sbr template id and get the first one.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] sbr_id Index to FP_XXX_SBR_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_dsbr_group_map_traverse_first(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint32_t sbr_id,
                              bcmfp_entry_id_traverse_cb cb,
                              void *user_data);
/*!
 * \brief Add group id to given psg template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] psg_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] group_id Index to FP_XXX_GRP_TEMPLATE LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_psg_group_map_add(int unit,
                        bcmfp_stage_id_t stage_id,
                        uint32_t psg_id,
                        uint32_t group_id);

/*!
 * \brief Delete group id from given psg template id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] presel_id Index to FP_XXX_PRESEL_TEMPLATE.
 * \param [in] group_id Index to FP_XXX_GRP_TEMPLATE LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_psg_group_map_delete(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t psg_id,
                           uint32_t group_id);

/*!
 * \brief Loop through all groups ids associated to a given
 * psg template id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] psg_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_psg_group_map_traverse(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t psg_id,
                             bcmfp_entry_id_traverse_cb cb,
                             void *user_data);
/*!
 * \brief Check whether psg template id is associated to any
 *  group id(s).
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] psg_id Index to FP_XXX_PRESEL_TEMPLATE LT.
 * \param [in] not_mapped Flag to indicate map status.
 *             TRUE = presel id not mapped to grouo id(s).
 *             FLASE = presel id mapped to group id(s).
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_psg_group_map_check(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t psg_id,
                          bool *not_mapped);

/*!
 * \brief Update the qualifier FIDs qulifier ID in both
 *  group and rule template LTs of all FP stages.
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmfp_fid_maps_adjust(int unit);

/*!
 * \brief Update the manually added qualifier and action fids
 * in group and rule template LTs of all FP stages.
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmfp_misc_init(int unit);

extern int
bcmfp_entry_list_compress_status_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_entry_id_t entry_id,
                             bool *list_compressed);
/*!
 * \brief Add parent entry id to given list compressed entry id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] child_entry_id Index to FP_XXX_ENTRY LT.
 * \param [in] parent_entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_entry_list_compress_parent_entry_map_add(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 uint32_t child_entry_id,
                                 uint32_t parent_entry_id);

/*!
 * \brief Delete list compressed entry id to given entry id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] child_entry_id Index to FP_XXX_ENTRY LT.
 * \param [in] parent_entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_entry_list_compress_parent_entry_map_delete(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 uint32_t child_entry_id,
                                 uint32_t parent_entry_id);

/*!
 * \brief Loop through all the parent compressed entry ids associated
 *  to a given list compressed entry id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] entry_id Index to FP_XXX_ENTRY LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_entry_list_compress_parent_entry_map_traverse(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 uint32_t entry_id,
                                 bcmfp_entry_id_traverse_cb cb,
                                 void *user_data);


/*!
 * \brief Check whether parent entry id is associated to list compressed
 *  entry id(s).
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] entry_id Index to FP_XXX_ENTRY LT.
 * \param [in] not_mapped Flag to indicate map status.
 *             TRUE = entry id not mapped to list compressed entry id(s).
 *             FALSE = entry id mapped to list compressed entry id(s).
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_entry_list_compress_parent_entry_map_check(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 uint32_t entry_id,
                                 bool *not_mapped);

/*!
 * \brief Add list compressed entry id to given entry id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] entry_id Index to FP_XXX_ENTRY LT.
 * \param [in] lc_entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_entry_list_compress_child_entry_map_add(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t entry_id,
                             uint32_t lc_entry_id);

/*!
 * \brief Delete list compressed entry id to given entry id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] entry_id Index to FP_XXX_ENTRY LT.
 * \param [in] lc_entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_entry_list_compress_child_entry_map_delete(int unit,
                                bcmfp_stage_id_t stage_id,
                                uint32_t entry_id,
                                uint32_t lc_entry_id);

/*!
 * \brief Loop through all list compressed entry ids associated
 *  to a given entry id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] entry_id Index to FP_XXX_ENTRY LT.
 * \param [in] cb User callback function
 * \param [in] user_data Data to be passed to traverse callback fucntion.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_entry_list_compress_child_entry_map_traverse(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  uint32_t entry_id,
                                  bcmfp_entry_id_traverse_cb cb,
                                  void *user_data);


/*!
 * \brief Check whether entry id is associated to any list compressed
 *  entry id(s).
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] entry_id Index to FP_XXX_ENTRY LT.
 * \param [in] not_mapped Flag to indicate map status.
 *             TRUE = entry id not mapped to list compressed entry id(s).
 *             FALSE = entry id mapped to list compressed entry id(s).
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_entry_list_compress_child_entry_map_check(int unit,
                               bcmfp_stage_id_t stage_id,
                               uint32_t entry_id,
                               bool *not_mapped);

extern int
bcmfp_entry_list_compress_parent_entry_get(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 bcmfp_entry_id_t child_entry_id,
                                 bcmfp_entry_id_t *parent_entry_id);
extern int
bcmfp_entry_list_compress_child_entry_map_move(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 bcmfp_entry_id_t parent_entry_id,
                                 bcmfp_entry_id_t *child_entry_id);

/*!
 * \brief Get valid pipe bitmap for the device for specific
 *  IP/EP block.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [out] Pipe_map Bitmap of valid pipes.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
extern int
bcmfp_dev_valid_pipes_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t *pipe_map);
/*!
 * \brief Create and initialize the LTD buffers to be used
 * in backend IMM calls.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmfp_ltd_buffers_init(int unit,
                       bcmfp_stage_t *stage);
/*!
 * \brief initialize the hit profile tbls
 *
 * \param[in] unit Logical device id
 * \param[in] trans_id Transaction ID
 * \param[in] stage BCMFP stage pointer
 * \param[in] warm boot flag
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmfp_hit_context_init(int unit,
                       uint32_t trans_id,
                       bcmfp_stage_t *stage,
                       bool warm);
/*!
 * \brief Deallocate the hit profile conetext info.
 *
 * \param [in] unit       Logical device id.
 * \param [in] stage      BCMFP stage structure.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters
 */
extern int
bcmfp_hit_context_cleanup(int unit,
                       bcmfp_stage_t *stage);
/*!
 * \brief Free the LTD buffers to be used in backend IMM calls.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmfp_ltd_buffers_cleanup(int unit,
                          bcmfp_stage_t *stage);
extern int
bcmfp_ltd_buffers_clear(int unit,
                        bcmfp_stage_id_t stage_id);

extern int
bcmfp_profiles_copy(int unit,
                    uint32_t trans_id,
                    bcmfp_stage_t *stage);

/*!
 * \brief Free the profiles hardware information.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmfp_stage_profiles_hw_info_cleanup(int unit,
                                     bcmfp_stage_t *stage);

/*!
 * \brief Set bank specific information for this FP stage
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmfp_bank_info_set(int unit,
                    bcmfp_stage_t *stage);

/*!
 * \brief Get the granularity of a section in first
 *  level in multi hierarcical multiplexer database.
 *
 * \param [in] unit Logical device id
 * \param [in] tile_id Physical tile ID.
 * \param [in] section_id Section ID from first level.
 * \param [out] gran Granularity of the section ID.
 *              Granularity of a section represent the
 *              granulrity of muxers going out of this
 *              section.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmfp_tile_l0_section_gran_get(int unit,
                               uint32_t tile_id,
                               uint8_t section_id,
                               uint8_t *gran);
#endif /* BCMFP_INTERNAL_H */
