/*! \file bcm56880_a0_fp.h
 *
 * Trident4(56880_A0) specific functions assigned to
 * common function pointers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_FP_H
#define BCM56880_A0_FP_H

#include <bcmcth/bcmcth_meter_util.h>

#define BCM56880_A0_FP_EM_KEY_ONE_BASE_ENTRY    (0)
#define BCM56880_A0_FP_EM_KEY_TWO_BASE_ENTRY    (1)
#define BCM56880_A0_FP_EM_KEY_THREE_BASE_ENTRY  (2)
#define BCM56880_A0_FP_EM_KEY_FOUR_BASE_ENTRY   (3)

#define BCM56880_A0_FP_EM_DATA_ZERO_BASE_ENTRY  (0)
#define BCM56880_A0_FP_EM_DATA_ONE_BASE_ENTRY   (1)
#define BCM56880_A0_FP_EM_DATA_TWO_BASE_ENTRY   (2)
#define BCM56880_A0_FP_EM_DATA_THREE_BASE_ENTRY (3)

#define BCM56880_A0_FP_EM_KEY_TYPE_SBIT         (3)
#define BCM56880_A0_FP_EM_KEY_TYPE_WIDTH        (4)

/*!
 * \brief Initialize following information specific to stage.
 *
 *  1. Set the supported group modes(SINGLE, DBLINTRA etc)
 *  2. Intialize HW entry information for each supported
 *     group mode.
 *  3. Initialize the miscellaneous information like maximum
 *     number of groups, action resolution ids, keygen
 *     profiles, action profiles, qos profiles etc.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_ing_device_consts_init(int unit,
                                      bcmfp_stage_t *stage);

/*!
 * \brief Initialize following information for EFP stage.
 *
 *  1. Set the supported group modes(SINGLE, DBLINTRA etc)
 *  2. Intialize HW entry information for each supported
 *     group mode.
 *  3. Initialize the miscellaneous information like maximum
 *     number of groups, action resolution ids, keygen
 *     profiles, action profiles, qos profiles etc.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_egr_device_consts_init(int unit,
                                      bcmfp_stage_t *stage);

/*!
 * \brief Initialize following information for VFP.
 *
 *  1. Set the supported group modes(SINGLE, DBLINTRA etc)
 *  2. Intialize HW entry information for each supported
 *     group mode.
 *  3. Initialize the miscellaneous information like maximum
 *     number of groups, action resolution ids, keygen
 *     profiles, action profiles, qos profiles etc.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_vlan_device_consts_init(int unit,
                                       bcmfp_stage_t *stage);
/*!
 * \brief Initialize following information specific to stage.
 *
 *  1. Set the supported group modes(SINGLE, DBLINTRA etc)
 *  2. Intialize HW entry information for each supported
 *     group mode.
 *  3. Initialize the miscellaneous information like maximum
 *     number of groups, action resolution ids, keygen
 *     profiles, action profiles, qos profiles etc.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_em_device_consts_init(int unit,
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
bcm56880_a0_fp_ing_alpm_fids_init(int unit,
                                  bcmfp_stage_t *stage);

/*!
 * \brief Build the extractor DB(databse) for main TCAM
 *  in a stage. This DB is used, along with qualifier and
 *  actions DBs, while assigning extractors to groups QSET.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_common_ext_db_init(int unit,
                               bcmfp_stage_t *stage);

/*!
 * \brief Build the extractor DB(databse) for main TCAM
 *  in a stage. This DB is used, along with qualifier and
 *  actions DBs, while assigning extractors to single
 *  wide groups QSET.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_common_ext_db_init_single(int unit,
                                      bcmfp_stage_t *stage);
/*!
 * \brief Build the extractor DB(databse) for main TCAM
 *  in a stage. This DB is used, along with qualifier and
 *  actions DBs, while assigning extractors to dblinter
 *  wide groups QSET.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_common_ext_db_init_dblinter(int unit,
                                        bcmfp_stage_t *stage);
/*!
 * \brief Build the extractor DB(databse) for main TCAM
 *  in EM stage. This DB is used, along with qualifier and
 *  actions DBs, while assigning extractors to groups QSET.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_em_ext_db_init(int unit,
                              bcmfp_stage_t *stage);

/*!
 * \brief Build the extractor DB(databse) for main TCAM
 *  in EM stage. This DB is used, along with qualifier and
 *  actions DBs, while assigning extractors to single
 *  wide groups QSET.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_em_ext_db_init_single(int unit,
                                     bcmfp_stage_t *stage);
/*!
 * \brief Build the extractor DB(databse) for main TCAM
 *  in EM stage. This DB is used, along with qualifier and
 *  actions DBs, while assigning extractors to dblinter
 *  wide groups QSET.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_em_ext_db_init_dblinter(int unit,
                                       bcmfp_stage_t *stage);
/*!
 * \brief Build the extractor DB(databse) for main TCAM
 *  in EM stage. This DB is used, along with qualifier and
 *  actions DBs, while assigning extractors to triple
 *  wide groups QSET.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_em_ext_db_init_quad(int unit,
                                   bcmfp_stage_t *stage);

/*!
 * \brief Build the extractor DB(databse) for main TCAM
 *  in a stage. This DB is used, along with qualifier and
 *  actions DBs, while assigning extractors to triple
 *  wide groups QSET.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_common_ext_db_init_triple(int unit,
                                      bcmfp_stage_t *stage);
/*!
 * \brief Build the extractor DB(databse) for presel TCAM
 *  in a stage. This DB is used, along with qualifier DB,
 *  while assigning extractors to presel group.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_common_presel_ext_db_init(int unit,
                                      bcmfp_stage_t *stage);

/*!
 * \brief Build the action DB(databse) for main TCAM in a stage.
 *  This DB is used while building the edw(EntryDataWords), for
 *  main TCAM HW entries.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_common_action_db_init(int unit,
                                  bcmfp_stage_t *stage);
/*!
 * \brief Build the qualifier DB(databse) for main TCAM in
 *  a stage. This DB is used while assigning extractors to
 *  a group template.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_common_qual_db_init(int unit,
                                bcmfp_stage_t *stage);
/*!
 * \brief Build the qualifier DB(databse) for main TCAM in
 *  Exact match stage. This DB is used while assigning extractors to
 *  a group template.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_em_qual_db_init(int unit,
                               bcmfp_stage_t *stage);
/*!
 * \brief Build the qualifier DB(databse) for presel TCAM in
 *  a stage. This DB is used while building the ekw(EntryKeyWords),
 *  for presel TCAM HW entries or assigning extractors to a
 *  presel group template.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_common_presel_qual_db_init(int unit,
                                       bcmfp_stage_t *stage);
/*!
 * \brief Build the action DB(databse) for presel TCAM in a stage.
 *  This DB is used while building the edw(EntryDataWords), for
 *  presel TCAM HW entries.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if stage is NULL
 */
extern int
bcm56880_a0_fp_common_presel_action_db_init(int unit,
                                         bcmfp_stage_t *stage);
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
bcm56880_a0_fp_ing_presel_edw_get(int unit,
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
bcm56880_a0_fp_vlan_presel_edw_get(int unit,
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
bcm56880_a0_fp_egr_presel_edw_get(int unit,
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
bcm56880_a0_fp_em_presel_edw_get(int unit,
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
bcm56880_a0_fp_common_pdd_profile_entry_get(int unit,
                               uint32_t trans_id,
                               bcmfp_stage_id_t stage_id,
                               bcmfp_pdd_id_t pdd_id,
                               bcmfp_group_id_t group_id,
                               bcmfp_group_oper_info_t *opinfo,
                               bcmfp_pdd_oper_type_t pdd_type,
                               uint32_t **pdd_profile,
                               bcmfp_pdd_config_t *pdd_config);
/*!
 * \brief Build the presel group key controls to be
 *  written to HW.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage identifier.
 * \param [in] group_id BCMFP group identifier.
 * \param [out] group presel group key control buffer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if edw is NULL
 */
extern int
bcm56880_a0_fp_common_presel_group_info_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        uint32_t **presel_group_info);

extern int
bcm56880_a0_fp_common_meter_actions_set(int unit,
                                        bcmfp_group_id_t group_id,
                                        bcmfp_pdd_id_t pdd_id,
                                        bcmfp_group_oper_info_t *opinfo,
                                        bcmcth_meter_fp_policy_fields_t *meter_entry,
                                        uint32_t **edw);

extern int
bcm56880_a0_fp_common_counter_actions_set(int unit,
                                          bcmfp_stage_id_t stage_id,
                                          bcmfp_group_id_t group_id,
                                          bcmfp_pdd_id_t pdd_id,
                                          bcmfp_group_oper_info_t *opinfo,
                                          bcmfp_ctr_params_t *ctr_params,
                                          uint32_t **edw);

/*!
 * \brief Custom buffers handling.
 *
 * \param [in] unit Logical device id
 * \param [in] trans_id transaction ID
 * \param [in] flags flags if any
 * \param [in] stage BCMFP stage ptr.
 * \param [in] opinfo Group operational info.
 * \param [in] Key_ids BCMFP key fields
 * \param [out] buffers filter buffers
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if edw is NULL
 */
extern int
bcm56880_a0_fp_common_buffers_custom_set(int unit,
                               uint32_t trans_id,
                               uint32_t flags,
                               bcmfp_stage_t *stage,
                               bcmfp_group_oper_info_t *opinfo,
                               bcmfp_pdd_config_t *pdd_config,
                               bcmfp_key_ids_t *key_ids,
                               bcmfp_buffers_t *buffers);
/*!
 * \brief Initialize the compression related information for
 *  FP ingress stage.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage structure.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid Parameters
 */
extern int
bcm56880_a0_fp_ing_compression_init(int unit,
                                    bcmfp_stage_t *stage);

extern int
bcm56880_a0_fp_info_tbls_init(int unit,
                                      bcmfp_stage_t *stage);
/*!
 * \brief Custom buffers handling.
 *
 * \param [in] unit Logical device id
 * \param [in] trans_id transaction ID
 * \param [in] flags flags if any
 * \param [in] stage BCMFP stage ptr.
 * \param [in] opinfo Group operational info.
 * \param [in] Key_ids BCMFP key fields
 * \param [out] buffers filter buffers
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if edw is NULL
 */
extern int
bcm56880_a0_fp_em_buffers_custom_set(int unit,
                                     uint32_t trans_id,
                                     uint32_t flags,
                                     bcmfp_stage_t *stage,
                                     bcmfp_group_oper_info_t *opinfo,
                                     bcmfp_pdd_config_t *pdd_config,
                                     bcmfp_key_ids_t *key_ids,
                                     bcmfp_buffers_t *buffers);
extern int
bcm56880_a0_fp_em_profiles_install(int unit,
                                   uint32_t stage_id,
                                   uint32_t trans_id,
                                   uint32_t flags,
                                   bcmfp_group_id_t group_id,
                                   bcmfp_group_oper_info_t *opinfo,
                                   bcmfp_buffers_t *buffers);
extern int
bcm56880_a0_fp_em_profiles_uninstall(int unit,
                                     uint32_t stage_id,
                                     uint32_t trans_id,
                                     uint32_t flags,
                                     bcmfp_group_id_t group_id,
                                     bcmfp_group_oper_info_t *opinfo);
extern int
bcm56880_a0_fp_em_profiles_copy(int unit,
                                uint32_t trans_id,
                                bcmfp_stage_t *stage);

extern int
bcm56880_a0_fp_em_bank_lookup_id_get(int unit,
                                     bcmdrd_sid_t bank_sid,
                                     uint8_t *lookup_id);
extern int
bcm56880_a0_fp_ing_profiles_install(int unit,
                                   uint32_t stage_id,
                                   uint32_t trans_id,
                                   uint32_t flags,
                                   bcmfp_group_id_t group_id,
                                   bcmfp_group_oper_info_t *opinfo,
                                   bcmfp_buffers_t *buffers);
extern int
bcm56880_a0_fp_ing_profiles_uninstall(int unit,
                                     uint32_t stage_id,
                                     uint32_t trans_id,
                                     uint32_t flags,
                                     bcmfp_group_id_t group_id,
                                     bcmfp_group_oper_info_t *opinfo);
extern int
bcm56880_a0_fp_vlan_profiles_install(int unit,
                                   uint32_t stage_id,
                                   uint32_t trans_id,
                                   uint32_t flags,
                                   bcmfp_group_id_t group_id,
                                   bcmfp_group_oper_info_t *opinfo,
                                   bcmfp_buffers_t *buffers);
extern int
bcm56880_a0_fp_vlan_profiles_uninstall(int unit,
                                     uint32_t stage_id,
                                     uint32_t trans_id,
                                     uint32_t flags,
                                     bcmfp_group_id_t group_id,
                                     bcmfp_group_oper_info_t *opinfo);
extern int
bcm56880_a0_fp_egr_profiles_install(int unit,
                                   uint32_t stage_id,
                                   uint32_t trans_id,
                                   uint32_t flags,
                                   bcmfp_group_id_t group_id,
                                   bcmfp_group_oper_info_t *opinfo,
                                   bcmfp_buffers_t *buffers);
extern int
bcm56880_a0_fp_egr_profiles_uninstall(int unit,
                                     uint32_t stage_id,
                                     uint32_t trans_id,
                                     uint32_t flags,
                                     bcmfp_group_id_t group_id,
                                     bcmfp_group_oper_info_t *opinfo);

extern int
bcm56880_a0_fp_vlan_bank_info_set(int unit,
                                  bcmfp_stage_t *stage);

extern int
bcm56880_a0_fp_em_bank_info_set(int unit,
                                bcmfp_stage_t *stage);

extern int
bcm56880_a0_fp_em_bank_sids_get(int unit,
                               bcmdrd_sid_t bank_sid,
                               uint32_t *ptid);
extern int
bcm56880_a0_fp_em_sid_exists(int unit,
                             bcmdrd_sid_t *sid_arr,
                             uint32_t count,
                             bcmdrd_sid_t ptid);
extern int
bcm56880_a0_fp_em_lts_tcam_sids_get(int unit,
                                    bcmdrd_sid_t lts_tcam_sid,
                                    uint32_t *ptid);
extern int
bcm56880_a0_fp_fixed_qual_init(int unit,
                               bcmfp_stage_t *stage,
                               uint32_t fid);

#endif /* BCM56880_A0_FP_H */

