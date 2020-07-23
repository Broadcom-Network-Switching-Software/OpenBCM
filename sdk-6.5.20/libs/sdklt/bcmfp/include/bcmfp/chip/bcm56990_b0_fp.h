/*! \file bcm56990_b0_fp.h
 *
 * Tomahawk-4 B0(56990_B0) specific functions assigned to common function pointers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_B0_FP_H
#define BCM56990_B0_FP_H

extern int
bcmfp_bcm56990_b0_fn_ptrs_init(int unit,
                               bcmfp_stage_id_t stage_id);
/*!
 * \brief Initialize all logical table IDs and other FIDs
 *  for all required logical tables of ingress stage.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcmfp_bcm56990_b0_ing_tbls_init(int unit,
                                bcmfp_stage_t *stage);
/*!
 * \brief Initialize all logical table IDs and other FIDs
 *  for all required logical tables of vlan stage.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcmfp_bcm56990_b0_vlan_tbls_init(int unit,
                                 bcmfp_stage_t *stage);
/*!
 * \brief Initialize all logical table IDs and other FIDs
 *  for all required logical tables of egress stage.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcmfp_bcm56990_b0_egress_tbls_init(int unit,
                                   bcmfp_stage_t *stage);
/*!
 * \brief Capture all key, mask  FIDs and class ID FID of L3 ALPM
 *  compression LTs for all compression types supported.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56990_b0_fp_ing_alpm_fids_init(int unit,
                                  bcmfp_stage_t *stage);
/*!
 * \brief Initialize all logical table IDs and other FIDs
 *  for all required logical tables of Exact Match stage.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcmfp_bcm56990_b0_em_tbls_init(int unit,
                               bcmfp_stage_t *stage);
extern int
bcmfp_bcm56990_b0_ext_codes_init(int unit,
                                 bcmfp_ext_codes_t *ext_codes,
                                 uint8_t part,
                                 bcmdrd_sid_t kgp_sid,
                                 uint32_t *kgp_entry);
extern int
bcmfp_bcm56990_b0_ingress_ext_cfg_db_init(int unit,
                                          bcmfp_stage_t *stage);

extern int
bcmfp_bcm56990_b0_ingress_action_cfg_db_init(int unit,
                                             bcmfp_stage_t *stage);
extern int
bcmfp_bcm56990_b0_ingress_action_cfg_conflict_check(int unit,
                                                    bcmfp_action_t action1,
                                                    bcmfp_action_t action);
extern int
bcmfp_bcm56990_b0_em_action_cfg_conflict_check(int unit,
                                               bcmfp_action_t action1,
                                               bcmfp_action_t action);
extern int
bcmfp_bcm56990_b0_vlan_action_cfg_conflict_check(int unit,
                                                 bcmfp_action_t action1,
                                                 bcmfp_action_t action);
extern int
bcmfp_bcm56990_b0_egress_action_cfg_conflict_check(int unit,
                                                   bcmfp_action_t action1,
                                                   bcmfp_action_t action);
extern int
bcmfp_bcm56990_b0_ingress_qual_cfg_db_init(int unit,
                                           bcmfp_stage_t *stage);
extern int
bcmfp_bcm56990_b0_ingress_presel_qual_cfg_db_init(int unit,
                                                  bcmfp_stage_t *stage);
extern int
bcmfp_bcm56990_b0_ingress_presel_action_cfg_db_init(int unit,
                                                    bcmfp_stage_t *stage);
extern int
bcmfp_bcm56990_b0_ingress_device_consts_init(int unit,
                                             bcmfp_stage_t *stage,
                                             bool warm);
extern int
bcm56990_b0_fp_ing_qual_enum_set(int unit,
                                 bcmfp_qualifier_t qual,
                                 bool is_presel,
                                 int value,
                                 uint32_t *data,
                                 uint32_t *mask);
extern int
bcm56990_b0_fp_ing_qual_enum_get(int unit,
                                 bcmfp_qualifier_t qual,
                                 bool is_presel,
                                 uint32_t data,
                                 uint32_t mask,
                                 int *value);
extern int
bcmfp_bcm56990_b0_em_qual_cfg_db_init(int unit,
                                      bcmfp_stage_t *stage);
extern int
bcmfp_bcm56990_b0_em_src_class_mode_set(int unit,
                                        uint32_t trans_id,
                                        bcmltd_sid_t sid,
                                        int pipe_id,
                                        uint32_t mode);
extern int
bcmfp_bcm56990_b0_ingress_src_class_mode_set(int unit,
                                     uint32_t trans_id,
                                     bcmltd_sid_t sid,
                                     int pipe_id,
                                     uint32_t mode);
extern int
bcm56990_b0_fp_vlan_qual_enum_set(int unit,
                                  bcmfp_qualifier_t qual,
                                  bool is_presel,
                                  int value,
                                  uint32_t *data,
                                  uint32_t *mask);
extern int
bcm56990_b0_fp_vlan_qual_enum_get(int unit,
                                  bcmfp_qualifier_t qual,
                                  bool is_presel,
                                  uint32_t data,
                                  uint32_t mask,
                                  int *value);
extern int
bcm56990_b0_fp_egr_qual_enum_set(int unit,
                                 bcmfp_qualifier_t qual,
                                 bool is_presel,
                                 int value,
                                 uint32_t *data,
                                 uint32_t *mask);
extern int
bcm56990_b0_fp_egr_qual_enum_get(int unit,
                                 bcmfp_qualifier_t qual,
                                 bool is_presel,
                                 uint32_t data,
                                 uint32_t mask,
                                 int *value);
extern int
bcm56990_b0_fp_common_qual_ip_type_set(int unit,
                                       int type,
                                       uint32_t *data,
                                       uint32_t *mask);
extern int
bcm56990_b0_fp_common_qual_ip_type_get(int unit,
                                       uint32_t data,
                                       uint32_t mask,
                                       int *type);
extern int
bcm56990_b0_fp_common_qual_l2format_set(int unit,
                                        int type,
                                        uint32_t *data,
                                        uint32_t *mask);
extern int
bcm56990_b0_fp_common_qual_l2format_get(int unit,
                                        uint32_t data,
                                        uint32_t mask,
                                        int *type);
extern int
bcm56990_b0_fp_common_qual_tpid_set(int unit,
                                    int tpid,
                                    uint32_t *data,
                                    uint32_t *mask);
extern int
bcm56990_b0_fp_common_qual_tpid_get(int unit,
                                    uint32_t data,
                                    uint32_t mask,
                                    int *tpid);
extern int
bcm56990_b0_fp_common_qual_ipfrag_set(int unit,
                                      int frag_info,
                                      uint32_t *data,
                                      uint32_t *mask);
extern int
bcm56990_b0_fp_common_qual_ipfrag_get(int unit,
                                      uint32_t data,
                                      uint32_t mask,
                                      int *frag_info);
extern int
bcm56990_b0_fp_common_qual_color_set(int unit,
                                     int frag_info,
                                     uint32_t *data,
                                     uint32_t *mask);
extern int
bcm56990_b0_fp_common_qual_color_get(int unit,
                                     uint32_t data,
                                     uint32_t mask,
                                     int *frag_info);
extern int
bcm56990_b0_fp_common_qual_fwd_type_set(int unit,
                                        int frag_info,
                                        uint32_t *data,
                                        uint32_t *mask);
extern int
bcm56990_b0_fp_common_qual_fwd_type_get(int unit,
                                        uint32_t data,
                                        uint32_t mask,
                                        int *frag_info);
extern int
bcmfp_bcm56990_b0_ingress_profiles_install(int unit,
                                           uint32_t stage_id,
                                           uint32_t trans_id,
                                           uint32_t flags,
                                           bcmfp_group_id_t group_id,
                                           bcmfp_group_oper_info_t *opinfo,
                                           bcmfp_buffers_t *buffers);
extern int
bcmfp_bcm56990_b0_ingress_profiles_uninstall(int unit,
                                             uint32_t stage_id,
                                             uint32_t trans_id,
                                             uint32_t flags,
                                             bcmfp_group_id_t group_id,
                                             bcmfp_group_oper_info_t *opinfo);
extern int
bcmfp_bcm56990_b0_vlan_action_cfg_db_init(int unit,
                                          bcmfp_stage_t *stage);
extern int
bcmfp_bcm56990_b0_vlan_qual_cfg_db_init(int unit,
                                        bcmfp_stage_t *stage);
extern int
bcmfp_bcm56990_b0_vlan_device_consts_init(int unit,
                                          bcmfp_stage_t *stage);
extern int
bcmfp_bcm56990_b0_egress_qual_cfg_db_init(int unit,
                                          bcmfp_stage_t *stage);
extern int
bcmfp_bcm56990_b0_egress_action_cfg_db_init(int unit,
                                          bcmfp_stage_t *stage);
extern int
bcmfp_bcm56990_b0_egress_device_consts_init(int unit,
                                            bcmfp_stage_t *stage);
extern int
bcmfp_bcm56990_b0_egress_group_selcode_key_get(int unit,
                                               bcmfp_group_t *fg);
extern int
bcmfp_bcm56990_b0_egress_entry_key_match_type_set(int unit,
                               bcmfp_stage_t *stage,
                               bcmfp_group_id_t group_id,
                               bcmfp_group_oper_info_t *opinfo,
                               bcmfp_group_slice_mode_t slice_mode,
                               bcmfp_group_type_t  port_pkt_type,
                               uint32_t **ekw);
extern int
bcmfp_bcm56990_b0_em_ext_cfg_db_init(int unit,
                                     bcmfp_stage_t *stage);
extern int
bcmfp_bcm56990_b0_exactmatch_device_consts_init(int unit,
                                                bcmfp_stage_t *stage);
extern int
bcmfp_bcm56990_b0_em_presel_qual_cfg_db_init(int unit,
                                             bcmfp_stage_t *stage);
extern int
bcmfp_bcm56990_b0_em_action_cfg_db_init(int unit,
                                        bcmfp_stage_t *stage);
extern int
bcmfp_bcm56990_b0_em_presel_action_cfg_db_init(int unit,
                                               bcmfp_stage_t *stage);
extern int
bcmfp_bcm56990_b0_em_profiles_install(int unit,
                                      uint32_t stage_id,
                                      uint32_t trans_id,
                                      uint32_t flags,
                                      bcmfp_group_id_t group_id,
                                      bcmfp_group_oper_info_t *opinfo,
                                      bcmfp_buffers_t *buffers);
extern int
bcmfp_bcm56990_b0_em_profiles_uninstall(int unit,
                                        uint32_t stage_id,
                                        uint32_t trans_id,
                                        uint32_t flags,
                                        bcmfp_group_id_t group_id,
                                        bcmfp_group_oper_info_t *opinfo);
extern int
bcmfp_bcm56990_b0_em_buffers_custom_set(int unit,
                                   uint32_t trans_id,
                                   uint32_t flags,
                                   bcmfp_stage_t *stage,
                                   bcmfp_group_oper_info_t *opinfo,
                                   bcmfp_pdd_config_t *pdd_config,
                                   bcmfp_key_ids_t *key_ids,
                                   bcmfp_buffers_t *buffers);
extern int
bcm56990_b0_fp_ingress_info_tbls_init(int unit,
                                      bcmfp_stage_t *stage);
extern int
bcm56990_b0_fp_egress_info_tbls_init(int unit,
                                      bcmfp_stage_t *stage);
extern int
bcm56990_b0_fp_lookup_info_tbls_init(int unit,
                                      bcmfp_stage_t *stage);
extern int
bcm56990_b0_fp_em_info_tbls_init(int unit,
                                  bcmfp_stage_t *stage);
extern int
bcm56990_b0_fp_em_qual_enum_set(int unit,
                                bcmfp_qualifier_t qual,
                                bool is_presel,
                                int value,
                                uint32_t *data,
                                uint32_t *mask);
extern int
bcm56990_b0_fp_em_qual_enum_get(int unit,
                                bcmfp_qualifier_t qual,
                                bool is_presel,
                                uint32_t data,
                                uint32_t mask,
                                int *value);
extern int
bcmfp_bcm56990_b0_ingress_range_check_grp_set(int unit,
                                              uint32_t trans_id,
                                              bcmltd_sid_t sid,
                                              int pipe_id,
                                              int range_check_grp_id,
                                              bcmfp_range_check_id_bmp_t bmp);
extern int
bcmfp_bcm56990_b0_ingress_meter_actions_set(int unit,
                                            bcmfp_group_id_t group_id,
                                            bcmfp_pdd_id_t pdd_id,
                                            bcmfp_group_oper_info_t *opinfo,
                                            bcmcth_meter_fp_policy_fields_t *meter_entry,
                                            uint32_t **edw);
extern int
bcmfp_bcm56990_b0_egress_meter_actions_set(int unit,
                                           bcmfp_group_id_t group_id,
                                           bcmfp_pdd_id_t pdd_id,
                                           bcmfp_group_oper_info_t *opinfo,
                                           bcmcth_meter_fp_policy_fields_t *meter_entry,
                                           uint32_t **edw);
extern int
bcmfp_bcm56990_b0_vlan_meter_actions_set(int unit,
                                         bcmfp_group_id_t group_id,
                                         bcmfp_pdd_id_t pdd_id,
                                         bcmfp_group_oper_info_t *opinfo,
                                         bcmcth_meter_fp_policy_fields_t *meter_entry,
                                         uint32_t **edw);
extern int
bcmfp_bcm56990_b0_em_meter_actions_set(int unit,
                                       bcmfp_group_id_t group_id,
                                       bcmfp_pdd_id_t pdd_id,
                                       bcmfp_group_oper_info_t *opinfo,
                                       bcmcth_meter_fp_policy_fields_t *meter_entry,
                                       uint32_t **edw);
extern int
bcmfp_bcm56990_b0_ingress_counter_actions_set(int unit,
                                              bcmfp_stage_id_t stage_id,
                                              bcmfp_group_id_t group_id,
                                              bcmfp_pdd_id_t pdd_id,
                                              bcmfp_group_oper_info_t *opinfo,
                                              bcmfp_ctr_params_t *ctr_params,
                                              uint32_t **edw);
extern int
bcmfp_bcm56990_b0_egress_counter_actions_set(int unit,
                                             bcmfp_stage_id_t stage_id,
                                             bcmfp_group_id_t group_id,
                                             bcmfp_pdd_id_t pdd_id,
                                             bcmfp_group_oper_info_t *opinfo,
                                             bcmfp_ctr_params_t *ctr_params,
                                             uint32_t **edw);
extern int
bcmfp_bcm56990_b0_vlan_counter_actions_set(int unit,
                                           bcmfp_stage_id_t stage_id,
                                           bcmfp_group_id_t group_id,
                                           bcmfp_pdd_id_t pdd_id,
                                           bcmfp_group_oper_info_t *opinfo,
                                           bcmfp_ctr_params_t *ctr_params,
                                           uint32_t **edw);
extern int
bcmfp_bcm56990_b0_em_counter_actions_set(int unit,
                                         bcmfp_stage_id_t stage_id,
                                         bcmfp_group_id_t group_id,
                                         bcmfp_pdd_id_t pdd_id,
                                         bcmfp_group_oper_info_t *opinfo,
                                         bcmfp_ctr_params_t *ctr_params,
                                         uint32_t **edw);

extern int
bcmfp_bcm56990_b0_egress_counter_hw_idx_set(int unit,
                                            bcmfp_stage_id_t stage_id,
                                            uint32_t ctr_id,
                                            uint32_t pipe_id,
                                            uint32_t pool_id,
                                            int color,
                                            bcmimm_entry_event_t event_reason);
/*!
 * \brief Build the presel policy to be written to HW.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage identifier.
 * \param [in] group_id BCMFP group identifier.
 * \param [in] opinfo Group operational information.
 * \param [out] edw Presel entry data words buffer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if edw is NULL
 */
extern int
bcm56990_b0_fp_ing_presel_edw_get(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_group_id_t group_id,
                                  bcmfp_group_oper_info_t *opinfo,
                                  uint32_t **edw);
/*!
 * \brief Build the presel policy to be written to HW.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage identifier.
 * \param [in] group_id BCMFP group identifier.
 * \param [in] opinfo Group operational information.
 * \param [out] edw Presel entry data words buffer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if edw is NULL
 */
extern int
bcm56990_b0_fp_em_presel_edw_get(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 bcmfp_group_id_t group_id,
                                 bcmfp_group_oper_info_t *opinfo,
                                 uint32_t **edw);
/*!
 * \brief Build the PDD profile to be written to HW.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage identifier.
 * \param [in] pdd_id BCMFP pdd identifier.
 * \param [in] group_id BCMFP group identifier.
 * \param [in] opinfo Group operational information.
 * \param [in] pdd_type PDD oper type.
 * \param [out] pdd profile data words buffer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if edw is NULL
 */
extern int
bcm56990_b0_fp_em_pdd_profile_entry_get(int unit,
                                        uint32_t trans_id,
                                        bcmfp_stage_id_t stage_id,
                                        bcmfp_pdd_id_t pdd_id,
                                        bcmfp_group_id_t group_id,
                                        bcmfp_group_oper_info_t *opinfo,
                                        bcmfp_pdd_oper_type_t pdd_type,
                                        uint32_t **pdd_profile,
                                        bcmfp_pdd_config_t *pdd_config);
/*!
 * \brief Set the IFP_SPARE_DEBUG register.
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmfp_bcm56990_b0_ifp_spare_debug_set(int unit);
#endif /* BCM56990_b0_FP_H */
