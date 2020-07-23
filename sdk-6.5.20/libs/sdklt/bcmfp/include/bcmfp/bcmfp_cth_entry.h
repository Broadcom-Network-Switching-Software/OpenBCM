/*! \file bcmfp_cth_entry.h
 *
 * Defines, Enums, Structures and APIs corresponding to
 * FP entry LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_CTH_ENTRY_H
#define BCMFP_CTH_ENTRY_H

#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmimm/bcmimm_int_comp.h>

/*! FP entry to be inserted/updated/deleted is a presel entry. */
#define BCMFP_ENTRY_PRESEL                   (1 << 0)

/*!
 * FP entry to be inserted/updated should use same physical
 * index as the previous entry.
 */
#define BCMFP_ENTRY_SAME_KEY                 (1 << 1)

/*! FP entry to be inserted/updated has to be written per pipe. */
#define BCMFP_ENTRY_WRITE_PER_PIPE_IN_GLOBAL (1 << 2)

/*!
 * FP group corresponding to FP entry to be inserted/deleted/updated
 * has only default presel entry.
 */
#define BCMFP_ENTRY_PRESEL_DEFAULT           (1 << 3)

/*! FP entry to be deleted is the last entry in the group. */
#define BCMFP_ENTRY_LAST_IN_GROUP            (1 << 4)

/*! FP entry to be inserted is the first entry in the group. */
#define BCMFP_ENTRY_FIRST_IN_GROUP           (1 << 5)

/*! FP entry to be inserted/deleted/updated has PDD profiles. */
#define BCMFP_ENTRY_WITH_PDD_PROFILE         (1 << 6)

/*! FP entry to be inserted/deleted/updated has SBR profiles. */
#define BCMFP_ENTRY_WITH_SBR_PROFILE         (1 << 7)

/*! Read the FP entry hardware. */
#define BCMFP_ENTRY_READ_FROM_HW             (1 << 8)

/*!
 * Maximum number of physical indexes needs to be updated
 * corresponding to single LT entry.
 */
#define BCMFP_ENTRY_PARTS_MAX 6

/*!
 * Maximum number of words required for a single physical
 * entry.
 */
#define BCMFP_ENTRY_WORDS_MAX 32

/*! Different entry states possible across all FP stages in all devices. */
typedef enum bcmfp_entry_state_e {

    /*! Entry is successfully installed in hardware. */
    BCMFP_ENTRY_SUCCESS = 0,

    /*! Group ID specified in entry is not created. */
    BCMFP_ENTRY_GROUP_NOT_CREATED = 1,

    /*! Rule ID specified in entry is not created. */
    BCMFP_ENTRY_RULE_NOT_CREATED = 2,

    /*! Policy ID specified in entry is not created. */
    BCMFP_ENTRY_POLICY_NOT_CREATED = 3,

    /*! Qualifiers specified in rule are not present in group */
    BCMFP_ENTRY_RULE_QSET_NOT_IN_GROUP = 4,

    /*!
     * (Qualifier mask in rule & Qualifier bitmap in group) !=
     * (Qualifier mask in rule)
     */
    BCMFP_ENTRY_RULE_QSET_WIDTH_EXCEEDS = 5,

    /*! Meter ID specified in entry is not created. */
    BCMFP_ENTRY_METER_NOT_CREATED = 6,

    /*! Counter ID specified in entry is not created. */
    BCMFP_ENTRY_COUNTER_NOT_CREATED = 7,

    /*! PDD ID specified in group is not created. */
    BCMFP_ENTRY_PDD_NOT_CREATED = 8,

    /*!
     * Two groups with same priority have either same presel entries
     * associated to them or doesnot have presele entries associated
     * to them.
     */
    BCMFP_ENTRY_GROUP_MASKED = 9,

    /*! Presel entry ID specified in group is not created. */
    BCMFP_ENTRY_PRESEL_NOT_CREATED = 10,

    /*! Pipe in which the group is created is different from pipe
     *  in which the meter is created which is associated to the
     *  entry in that group.
     */
    BCMFP_ENTRY_METER_PIPE_ID_MISMATCH = 11,

    /*! SBR ID specified in group/entry is not created. */
    BCMFP_ENTRY_SBR_NOT_CREATED = 12,

    /*! PSG ID specified in group is not created. */
    BCMFP_ENTRY_PSG_NOT_CREATED = 13,

    /*! Default PDD ID specified in EM group is not created. */
    BCMFP_ENTRY_DPDD_NOT_CREATED = 14,

    /*! Default policy ID specified in EM group is not created. */
    BCMFP_ENTRY_DPOLICY_NOT_CREATED = 15,

    /*!
     * Polciy ID is associate to entry but PDD ID is not
     * associated to group.
     */
    BCMFP_ENTRY_POLICY_PRESENT_PDD_MISSING = 16,

    /*!
     * Default polciy ID is associate to EM group but default PDD ID
     * is not associated to EM group.
     */
    BCMFP_ENTRY_DPOLICY_PRESENT_DPDD_MISSING = 17,

    /*!
     * PDD is associated to group but SBR ID is not associated to
     * group/entry.
     */
    BCMFP_ENTRY_PDD_PRESENT_SBR_MISSING = 18,

    /*!
     * Group is not operational due any of the following reasons.
     * 1. QSET does not fit in given group mode.
     * 2. ASET doesnot fit in given group mode.
     */
    BCMFP_ENTRY_GROUP_NOT_OPERATIONAL = 19,

    /*!
     * Conflicting actions are present in polciy ID associated
     * to entry.
     */
    BCMFP_ENTRY_POLCIY_ACTIONS_CONFLICT = 20,

    /*!
     * Conflicting actions are present in default polciy ID
     * associated to EM group.
     */
    BCMFP_ENTRY_DPOLCIY_ACTIONS_CONFLICT = 21,

    /*! Action is specified in policy but not specified in PDD */
    BCMFP_ENTRY_POLICY_ACTION_NOT_IN_PDD = 22,

    /*!
     * Action is specified in default policy but not specified
     * in default PDD.
     */
    BCMFP_ENTRY_DPOLICY_ACTION_NOT_IN_DPDD = 23,

    /*!
     * Qualifier matched in presel entry but not selected in
     * presel group.
     */
    BCMFP_ENTRY_PRESEL_QSET_NOT_IN_PSG = 24,

    /*!
     * (Qualifier mask in presel entry & Qualifier bitmap in presel group)
     *  != (Qualifier mask in presel entry)
     */
    BCMFP_ENTRY_PRESEL_QSET_WIDTH_EXCEEDS = 25,

    /*! Presel entry IDs are associated to group but presel group is not. */
    BCMFP_ENTRY_PSE_PRESENT_PSG_MISSING = 26,

    /*! Groups ASET doesnot fit in given group mode. */
    BCMFP_ENTRY_GROUP_ASET_DOESNT_FIT = 27,

    /*! Groups QSET doesnot fit in given group mode. */
    BCMFP_ENTRY_GROUP_QSET_DOESNT_FIT = 28,

    /*! Groups default ASET exceeds default policy size */
    BCMFP_ENTRY_GROUP_DEFAULT_ASET_DOESNT_FIT = 29,

    /*! Deafult SBR associated to EM group is not created. */
    BCMFP_ENTRY_DSBR_NOT_CREATED = 30,

    /*!
     * Defaulr policy is specified in EM group but not default
     * SBR.
     */
    BCMFP_ENTRY_DPOLICY_PRESENT_DSBR_MISSING = 31,

    /*!
     * PDD ID and default PDD ID are same in the group.
     * This is applicable only for FP stages using EM
     * resources. Because default policy is supported
     * only for EM lookups.
     */
    BCMFP_ENTRY_GROUP_PDD_AND_DPDD_SAME = 32,

    /*!
     * This groups PDD ID is some other groups default
     * PDD ID.
     */
    BCMFP_ENTRY_GROUP_PDD_IS_OTHER_GROUP_DPDD = 33,

    /*!
     * This groups DPDD ID is some other groups PDD ID.
     */
    BCMFP_ENTRY_GROUP_DPDD_IS_OTHER_GROUP_PDD = 34,

    /*!
     * Compression type is enabled in rule but the bit
     * stream comprised of all compression qualifier
     * masks is non LPM mask.
     */
    BCMFP_ENTRY_NON_LPM_COMPRESS_MASK_IN_RULE = 35,

    /*!
     * Compress type is enabled in the rule but not
     * enabled in the group.
     */
    BCMFP_ENTRY_ALPM_COMPRESSION_OFF_IN_GROUP_ON_IN_RULE = 36,

    /*!
     * Some qualifiers corresponding to compression
     * types enabled in the rule are not matched in
     * the rule.
     */
    BCMFP_ENTRY_COMPRESS_KEYS_NOT_MATCHED_IN_RULE = 37,

    /*!
     * Some qualifiers corresponding to compression
     * types enabled in the group are provided with
     * partial bitmaps.
     */
    BCMFP_ENTRY_PARTIAL_COMPRESS_QUAL_BITMAP_IN_GROUP = 38,

    /*! SBR template is associated to both group and entry. */
    BCMFP_ENTRY_SBR_PRESENT_IN_GROUP_AND_ENTRY = 39,

    /*! PDD template is associated to both group and entry. */
    BCMFP_ENTRY_PDD_PRESENT_IN_GROUP_AND_ENTRY = 40,

    /*!
     * Used for cases when the key of the presel group
     * doesn't fit in the HW
     */
    BCMFP_ENTRY_PRESEL_GROUP_QSET_DOESNT_FIT = 41,

    /*!
     * Already a group with same priority exists, which is of
     * diferent group mode, and sharing resources is not feasible with
     * the current configuration.
     */
    BCMFP_ENTRY_GROUP_MULTIMODE_CHECK_FAILED = 42,

    /*! Last in the list of entry states. */
    BCMFP_ENTRY_OP_COUNT

} bcmfp_entry_state_t;

/*! Configuration provided in FP entry LT entry. */
typedef struct bcmfp_entry_config_s {
    /*! Entry id */
    uint32_t entry_id;

    /*! Group id associated to FP entry */
    uint32_t group_id;

    /*! Rule id associated to FP entry */
    uint32_t rule_id;

    /*! Policy id associated to FP entry */
    uint32_t policy_id;

    /*! SBR id associated to FP entry */
    uint32_t sbr_id;

    /*! PDD id associated to FP entry */
    uint32_t pdd_id;

    /*! meter id associated to FP entry */
    uint32_t meter_id;

    /*! Counter command associated to FP entry */
    uint32_t ctr_cmd;

    /*! Egress counter id associated to FP entry */
    uint32_t ctr_entry_id;

    /*! Entry priority */
    uint32_t priority;

    /*! Flex counter base index */
    uint32_t flex_ctr_base_idx;

    /*! Flex counter offset mode */
    uint32_t flex_ctr_offset_mode;

    /*! Flex counter pool id */
    uint32_t flex_ctr_pool_id;

    /*! Flex counter action profile ID */
    uint32_t flex_ctr_action;

    /*! Entry operational state. */
    bcmfp_entry_state_t entry_state;

    /*! Entry Enable/Disable */
    bool enable;

} bcmfp_entry_config_t;

/*!
 * Different profiles passed on from BCMFP to RM-TCAM and the
 * corresponding indexes allocated by RM-TCAM passed back to
 * BCMFP.
 */
typedef struct bcmfp_entry_profiles_s {

    /*! PDD profile data. */
    uint32_t **pdd;

    /*! HW index of PDD profile. */
    uint32_t pdd_index;

    /*! SBR profile data. */
    uint32_t **sbr;

    /*! HW index of SBR profile. */
    uint32_t sbr_index;

    /*! PSG(presel group) profile data. */
    uint32_t **psg;

    /*! HW index of PSG(presel group) profile. */
    uint32_t psg_index;
} bcmfp_entry_profiles_t;

/*!
 * \brief Get the information provided in FP entry LT entry into
 * bcmfp_entry_config_t strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 * \param [out] config            FP entry LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_entry_config_get(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t tbl_id,
                       bcmfp_stage_id_t stage_id,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       bcmfp_entry_config_t **config);

/*!
 * \brief Get the information provided in FP entry LT entry with
 * entry_id as key into bcmfp_entry_config_t strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] entry_id           BCMFP entry ID.
 * \param [out] config            FP entry LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_entry_remote_config_get(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_entry_id_t entry_id,
                              bcmfp_entry_config_t **config);
/*!
 * \brief Insert the entry into entry template LT with a given key.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 * \param [out] output_fields     If LT has any read only fields, BCMFP
 *                                BCMFP driver can add those fields to
 *                                LT entry in IMM so that application can
 *                                see those values upon LT entry LOOKUP.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_entry_config_insert(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields);

/*!
 * \brief Update the entry in entry template LT with a given key.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 * \param [out] output_fields     If LT has any read only fields, BCMFP
 *                                BCMFP driver can add those fields to
 *                                LT entry in IMM so that application can
 *                                see those values upon LT entry LOOKUP.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_entry_config_update(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields);

/*!
 * \brief  Delete the entry from entry template LT with a given key.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 * \param [out] output_fields     If LT has any read only fields, BCMFP
 *                                BCMFP driver can add those fields to
 *                                LT entry in IMM so that application can
 *                                see those values upon LT entry LOOKUP.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_entry_config_delete(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields);

/*!
 * \brief  HW lookup the entry from entry template LT with a given key.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] lkup_type          IMM lookup type.
 * \param [in] key                Key consisting of all key field values.
 * \param [out] data              Data consisting of user specified data
 *                                field values.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_entry_config_lookup(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmltd_sid_t tbl_id,
                      bcmfp_stage_id_t stage_id,
                      bcmimm_lookup_type_t lkup_type,
                      const bcmltd_fields_t *key,
                      bcmltd_fields_t *data);
/*!
 * \brief  Free the memory created for FP entry LT entry configuration.
 *
 * \param [in] unit          Logical device id.
 * \param [in] entry_config  FP entry LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmfp_entry_config_free(int unit,
                        bcmfp_entry_config_t *entry_config);

/*!
 * \brief  Process LT idp(Inter dependency) signal from
 * CID/CID_MASK change to FP entry LT.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] entry_id           BCMFP entry ID.
 * \param [in] cid                Compress ID. This parameter
 *                                is unused. CID/CID_MASK can be
 *                                more than 32 bits and should
 *                                be part of user_data.
 * \param [in] user_data          Data to be processed.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_entry_idp_cid_process(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_entry_id_t entry_id,
                            uint32_t cid,
                            void *user_data);
/*!
 * \brief  Process LT idp(Inter dependency) signal from
 * counter entry LT to entry LT.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] entry_id           BCMFP entry ID.
 * \param [in] ctr_entry_id       Counter entry LT ID.
 * \param [in] user_data          Data to be processed.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_entry_idp_ctr_entry_process(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_entry_id_t entry_id,
                          bcmfp_ctr_entry_id_t ctr_entry_id,
                          void *user_data);
/*!
 * \brief  Process LT idp(Inter dependency) signal from meter LT to
 * entry LT.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] entry_id           BCMFP entry ID.
 * \param [in] meter_id           Meter LT ID.
 * \param [in] user_data          Data to be processed.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_entry_idp_meter_process(int unit,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_entry_id_t entry_id,
                              bcmfp_meter_id_t meter_id,
                              void *user_data);
/*!
 * \brief  Process LT idp(Inter dependency) signal from policy LT to
 * entry LT.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] entry_id           BCMFP entry ID.
 * \param [in] policy_id          Policy LT ID.
 * \param [in] user_data          Data to be processed.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_entry_idp_policy_process(int unit,
                               bcmfp_stage_id_t stage_id,
                               bcmfp_entry_id_t entry_id,
                               bcmfp_policy_id_t policy_id,
                               void *user_data);
/*!
 * \brief  Process LT idp(Inter dependency) signal from rule LT to
 * entry LT.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] entry_id           BCMFP entry ID.
 * \param [in] rule_id            Rule LT ID.
 * \param [in] user_data          Data to be processed.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_entry_idp_rule_process(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_entry_id_t entry_id,
                             bcmfp_rule_id_t rule_id,
                             void *user_data);
/*!
 * \brief  Process LT idp(Inter dependency) signal from group LT to
 * entry LT.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] entry_id           BCMFP entry ID.
 * \param [in] group_id           Group LT ID.
 * \param [in] user_data          Data to be processed.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_entry_idp_group_process(int unit,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_entry_id_t entry_id,
                              bcmfp_group_id_t group_id,
                              void *user_data);
/*!
 * \brief  Process LT idp(Inter dependency) signal from presel entry LT to
 * entry LT.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] entry_id           BCMFP entry ID.
 * \param [in] group_id           Group LT ID.
 * \param [in] user_data          Data to be processed.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_entry_idp_pse_process(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_entry_id_t entry_id,
                            bcmfp_group_id_t group_id,
                            void *user_data);

/*!
 * \brief  Process LT idp(Inter dependency) signal from SBR LT to
 * entry LT.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] entry_id           BCMFP entry ID.
 * \param [in] sbr_id             SBR LT ID.
 * \param [in] user_data          Data to be processed.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_entry_idp_sbr_process(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_entry_id_t entry_id,
                            bcmfp_policy_id_t sbr_id,
                            void *user_data);

/*!
 * \brief  Process LT idp(Inter dependency) signal from entry ID to
 * list compression.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] entry_id           BCMFP entry ID.
 * \param [in] lc_entry_id        BCMFP list compressed entry ID.
 * \param [in] user_data          Data to be processed.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_entry_idp_list_compress_process(int unit,
                                      bcmfp_stage_id_t stage_id,
                                      bcmfp_entry_id_t entry_id,
                                      bcmfp_entry_id_t lc_entry_id,
                                      void *user_data);
/*!
 * \brief  Process LT idp(Inter dependency) signal from entry ID to
 * compression key.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] entry_id           BCMFP entry ID.
 * \param [in] ckey               Compression key.
 * \param [in] user_data          Data to be processed.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_entry_idp_ckey_process(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_entry_id_t entry_id,
                             uint32_t ckey,
                             void *user_data);
/*!
 * \brief Copy entry configuration from one structure to the
 *  other structure.
 *
 * \param [in] unit          Logical device id.
 * \param [in] src_econfig   Source entry configuration.
 * \param [out] dst_econfig  Destination entry configuration.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_entry_config_copy(int unit,
                        bcmfp_entry_config_t *src_econfig,
                        bcmfp_entry_config_t *dst_econfig);


#endif /* BCMFP_CTH_ENTRY_H */
