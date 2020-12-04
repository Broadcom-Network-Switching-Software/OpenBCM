/*! \file bcmfp_cth_group.h
 *
 * Defines, Enums, Structures and APIs corresponding to
 * FP group LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_CTH_GROUP_H
#define BCMFP_CTH_GROUP_H

#include <shr/shr_bitop.h>
#include <bcmfp/bcmfp_group_internal.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/bcmfp_cth_common.h>

/* To indicate the extractor is already in use. */
#define BCMFP_GROUP_EXT_IN_USE 0x1000

/* To indicate the extractor is not in use. */
#define BCMFP_GROUP_EXT_NOT_IN_USE -1

/*! Configuration provided in group template LT entry. */
typedef struct bcmfp_group_config_s {
    /*! Group ID. */
    uint32_t group_id;

    /*! Group mode. */
    bcmfp_group_mode_t mode;

    /*! Table instance(pipe id) */
    int pipe_id;

    /*! Group priority */
    uint32_t priority;

    /*! Virtual slice group */
    uint8_t vslice_group;

    /*! Port and Packet type */
    bcmfp_group_type_t type;

    /*! Slice mode */
    bcmfp_group_slice_mode_t slice_mode;

    /*! Number of preselection entries */
    uint8_t num_presels;

    /*! Preselection entry IDs. */
    uint32_t *presel_ids;

    /*!
     * Conflicting qualifiers IDs. Number of elements in this
     * array should be same as num_presels. So the expectation
     * is the number of valid elements in arrays presel_ids
     * and conflict_qset_ids are same.
     */
    uint32_t *conflict_qset_ids;

    /*! Lookup ID. */
    uint8_t lookup_id;

    /*!
     * Flag to indicate that action_res_id has
     * to be generated internally by SDKLT driver.
     */
    bool ltid_auto;

    /*! HW logical table ID used in action resolution. */
    uint8_t action_res_id;

    /*! PDD ID */
    int pdd_id;

    /*! Default PDD ID */
    int default_pdd_id;

    /*! Default meter id */
    uint32_t default_meter_id;

    /*! Default Flex counter action profile ID */
    uint32_t default_flex_ctr_action;

    /*! SBR ID */
    int sbr_id;

    /*! Default PDD ID */
    int default_sbr_id;

    /*! Presel group ID */
    int psg_id;

    /*! Default policy ID */
    uint32_t default_policy_id;

    /*! Indicates compresson is enabled or not. */
    bool compress;

    /*! Different compress types expected to be applied. */
    bool compress_types[BCMFP_COMPRESS_TYPES_MAX];

    /*! List of qualifier bitmaps. */
    bcmfp_qual_bitmap_t *qual_bitmap;

    /*! Groups QSET. */
    bcmfp_qset_t qset;

    /*! PDD parts. */
    uint8_t pdd_parts;

    /*! Disable auto expansion. */
    bool auto_expand;

    /*! Enable/Disable. */
    bool enable;

    /*! Group slice_type. */
    bcmfp_group_slice_type_t group_slice_type;

    /*! Number of counter profile entries */
    uint8_t num_ctr_profiles;

    /*! counter profile IDs. */
    uint32_t *ctr_profile_ids;

} bcmfp_group_config_t;

/*! Different group_template status possible */
typedef enum bcmfp_group_update_state_e {
    /*! Group update successful */
    BCMFP_GROUP_UPDATE_SUCCESS = 0,

    /*! Group update causes qualifier offsets to change */
    BCMFP_GROUP_UPDATE_QUAL_OFFSETS_CHANGE = 1,

    /*! Group update causes action offsets to change */
    BCMFP_GROUP_UPDATE_ACTION_OFFSETS_CHANGE = 2,

    BCMFP_GROUP_UPDATE_COUNT = 3
} bcmfp_group_update_state_t;

/*!
 * \brief Get the information provided in FP group LT entry into
 * bcmfp_group_config_t strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 * \param [out] config            FP group LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_group_config_get(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t tbl_id,
                       bcmfp_stage_id_t stage_id,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       bcmfp_group_config_t **config);

/*!
 * \brief Get the information provided in FP group LT entry with
 * group_id as key into bcmfp_group_config_t strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] group_id           BCMFP group ID.
 * \param [out] config            FP group LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_group_remote_config_get(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_group_id_t group_id,
                              bcmfp_group_config_t **config);

/*!
 * \brief If group is enabled for ACL compression, replace all
 *  different compression key types bitmap FIDs with corresponding
 *  CID bitmaps.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] group_id           BCMFP group ID.
 * \param [out] config            FP group LT entry configuration
 *                                to be compressed.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 * \retval SHR_E_CONFIG   All compression key bitmap FIDs are not
 *                        present in the groups QSET.
 *                                  (OR)
 *                        Exact match BITAMP is not specified for
 *                        all compression key bitmap FIDs.
 */
extern int
bcmfp_group_config_compress(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_id_t group_id,
                            bcmfp_group_config_t *config);

/*!
 * \brief Insert the entry into group template LT with a given key.
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
bcmfp_group_config_insert(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields);

/*!
 * \brief Update the entry in group template LT with a given key.
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
bcmfp_group_config_update(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields);

/*!
 * \brief  Delete the entry from group template LT with a given key.
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
bcmfp_group_config_delete(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields);
/*!
 * \brief  Free the memory created for FP group LT entry configuration.
 *
 * \param [in] unit          Logical device id.
 * \param [in] group_config  FP group LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmfp_group_config_free(int unit,
                        bcmfp_group_config_t *group_config);
/*!
 * \brief  Process LT idp(Inter dependency) signal from other LTs to
 * group LT.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] group_id           BCMFP group ID.
 * \param [in] template_id        PDD/SBR/PRESEL_GROUP/PRESEL_ENTRY ID.
 * \param [in] user_data          Data to be processed.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_group_idp_process(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        uint32_t template_id,
                        void *user_data);
/*!
 * \brief Staging the updated to qual_config in grp_template_action LT.
 *
 * \param [in] unit                  Logical device id.
 * \param [in] op_arg                operational args
 * \param [in] tbl_id                Logical Table ID.
 * \param [in] stage_id              BCMFP stage ID.
 * \param [in] updated_group_config  group config with updated qualifier bitmaps.
 * \param [out] status               group update staging status
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_group_config_update_staging(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmltd_sid_t tbl_id,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_group_config_t *updated_group_config,
                                  bcmfp_group_update_state_t *status);

/*!
 * \brief Process the updated to qual_config in grp_template_action LT.
 *
 * \param [in] unit                  Logical device id.
 * \param [in] op_arg                operational args
 * \param [in] tbl_id                Logical Table ID.
 * \param [in] stage_id              BCMFP stage ID.
 * \param [in] updated_group_config  group config with updated qualifier bitmaps.
 * \param [in] qual_update_staging   Perform staging alone without updating
 *                                   the hardware entries
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_group_config_update_process(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmltd_sid_t tbl_id,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_group_config_t *group_config,
                                  bcmfp_group_oper_info_t *opinfo,
                                  bool qual_update_staging);

/*!
 * \brief  Process LT idp(Inter dependency) signal from conflict qualifier
 * set LT to group LT.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] group_id           BCMFP group ID.
 * \param [in] user_data          Data to be processed.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_group_idp_conflict_qset_process(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_id_t group_id,
                            void *user_data);

/*!
 * \brief Process LT idp(Inter dependency) signal from any other
 * LT to group LT. It deletes the current groups and recreates
 * them corresponding to IDP callback.
 *
 * \param [in] unit               Logical device id.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] group_id           BCMFP group ID.
 * \param [in] user_data          Data to be processed.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_group_idp_common_process(int unit,
                               bcmfp_stage_id_t stage_id,
                               bcmfp_group_id_t group_id,
                               void *user_data);
#endif /* BCMFP_CTH_GROUP_H */
