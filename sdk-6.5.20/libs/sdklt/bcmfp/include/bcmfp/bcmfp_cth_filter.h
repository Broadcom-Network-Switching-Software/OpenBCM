/*! \file bcmfp_cth_filter.h
 *
 * Defines, Enums, Structures and APIs to create/update/delete
 * a FILTER.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_CTH_FILTER_H
#define BCMFP_CTH_FILTER_H

#include <bcmlrd/bcmlrd_types.h>
#include <bcmfp/bcmfp_cth_group.h>
#include <bcmfp/bcmfp_cth_rule.h>
#include <bcmfp/bcmfp_cth_policy.h>
#include <bcmfp/bcmfp_cth_entry.h>
#include <bcmfp/bcmfp_cth_sbr.h>
#include <bcmfp/bcmfp_cth_pdd.h>
#include <bcmfp/bcmfp_cth_psg.h>
#include <bcmfp/bcmfp_cth_pse.h>
#include <bcmfp/bcmfp_cth_range_check.h>
#include <bcmfp/bcmfp_cth_src_class.h>
#include <bcmfp/bcmfp_cth_common.h>
#include <bcmfp/bcmfp_types_internal.h>

/* Indicated FILTER group configuration get/process */
#define BCMFP_FILTER_GROUP_CONFIG        (0x1 << 0)

/* Indicated FILTER rule configuration get/process */
#define BCMFP_FILTER_RULE_CONFIG         (0x1 << 1)

/* Indicated FILTER policy configuration get/process */
#define BCMFP_FILTER_POLICY_CONFIG       (0x1 << 2)

/* Indicated FILTER entry configuration get/process */
#define BCMFP_FILTER_ENTRY_CONFIG        (0x1 << 3)

/* Indicated FILTER pdd configuration get/process */
#define BCMFP_FILTER_PDD_CONFIG          (0x1 << 4)

/* Indicated FILTER sbr configuration get/process */
#define BCMFP_FILTER_SBR_CONFIG          (0x1 << 5)

/* Indicated FILTER presel group configuration get/process */
#define BCMFP_FILTER_PRESEL_GROUP_CONFIG (0x1 << 6)

/* Indicated FILTER presel entry configuration get/process */
#define BCMFP_FILTER_PRESEL_ENTRY_CONFIG (0x1 << 7)

/* Indicated FILTER default policy configuration get/process */
#define BCMFP_FILTER_DPOLICY_CONFIG      (0x1 << 8)

/* Indicated FILTER default pdd configuration get/process */
#define BCMFP_FILTER_DPDD_CONFIG         (0x1 << 9)

/* Indicated FILTER meter configuration get/process */
#define BCMFP_FILTER_METER_CONFIG        (0x1 << 10)

/* Indicated FILTER counter configuration get/process */
#define BCMFP_FILTER_CTR_CONFIG          (0x1 << 11)

/*! All the configuration required to create an FILTER. */
typedef struct bcmfp_filter_s {
    /*! Entry configuration */
    bcmfp_entry_config_t *entry_config;

    /*! Group configuration */
    bcmfp_group_config_t *group_config;

    /*! Rule configuration */
    bcmfp_rule_config_t *rule_config;

    /*! Policy configuration */
    bcmfp_policy_config_t *policy_config;

    /*! Default policy configuration */
    bcmfp_policy_config_t *dpolicy_config;

    /*! SBR configuration */
    bcmfp_sbr_config_t *sbr_config;

    /*! Default SBR configuration */
    bcmfp_sbr_config_t *dsbr_config;

    /*! PDD configuration */
    bcmfp_pdd_config_t *pdd_config;

    /*! Default PDD configuration */
    bcmfp_pdd_config_t *dpdd_config;

    /*! Presel group configuration */
    bcmfp_psg_config_t *psg_config;

    /*!
     * Presel entry configuration. Group that belongs
     * to a filter can have more than one presel entry
     * associated to it. So this will have pointers to
     * all of those presel entry configurations.
     */
    bcmfp_pse_config_t **pse_config;

    /*!
     * Number of presel entry configurations exists in
     * pse_config variable.
     */
    uint8_t num_pse_configs;

    /*!
     *  FP can have either legacy or flex counters. If FP
     *  supports legacy counters the ctr_entry_config is
     *  relavent. If FP supports flex counters, all the
     *  counter configuration required is part of entry
     *  config and group config structures.
     */
    bcmfp_ctr_entry_config_t *ctr_entry_config;

    /*!
     * Operational info of the group to which this filter
     * belongs to.
     */
    bcmfp_group_oper_info_t *group_oper_info;

    /* Different buffers required for processing the filter. */
    bcmfp_buffers_t buffers;

    /*! Entry State after filter is processed. */
    bcmfp_entry_state_t entry_state;

    /*! INSERT or DELETE or UPDATE */
    bcmimm_entry_event_t event_reason;

    /*! LOOKUP or TRAVERSE or FROM_HW */
    bcmimm_lookup_type_t lkup_type;

    /*!
     * Type of the BCMFP LT entry that is inserted,
     * or updated, or deleted and associated to the
     * remote LT.
     */
    bcmfp_sid_type_t sid_type;


    /*! Compression keys present in filters rule. */
    bcmfp_rule_compress_key_t *compression_keys;

    /*! 1 - Filter is list compressed. */
    bool list_compressed;

    /*! Donot compress the filter while it is inserted or deleted. */
    bool dont_compress;

    /*! Compression keys are inserted in list compression. */
    bool ckeys_inserted;

    /*
     * SBR profile index programmed in presel or main tcam
     * policy.
     */
    uint32_t sbr_profile_index;
    /*
     * Default SBR profile index programmed in presel tcam
     * policy.
     */
    uint32_t dsbr_profile_index;

    /*!
     * if TRUE, dont need to install per entry SBR. This will
     * be set to TRUE when sbr_id of entry is not changed but
     * some other parameter of entry is updated.
     */
    bool sbr_dont_install;
} bcmfp_filter_t;

/*!
 * \brief Create the FILTER in HW.
 *
 * \param [in] unit        Logical device id.
 * \param [in] trans_id    Transaction ID.
 * \param [in] tbl_id      Logical Table ID.
 * \param [in] stage_id    BCMFP stage ID.
 * \param [in] filter      Access control list.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_filter_create(int unit,
                 const bcmltd_op_arg_t *op_arg,
                 bcmltd_sid_t tbl_id,
                 bcmfp_stage_id_t stage_id,
                 bcmfp_filter_t *filter);
/*!
 * \brief Update the FILTER in HW from previous
 *  to current configuration.
 *
 * \param [in] unit           Logical device id.
 * \param [in] trans_id       Transaction ID.
 * \param [in] tbl_id         Logical Table ID.
 * \param [in] stage_id    BCMFP stage ID.
 * \param [in] filter_prev    Previous filter.
 * \param [in] filter_curr    Current filter.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_filter_update(int unit,
                    const bcmltd_op_arg_t *op_arg,
                    bcmltd_sid_t tbl_id,
                    bcmfp_stage_id_t stage_id,
                    bcmfp_filter_t *filter_prev,
                    bcmfp_filter_t *filter_curr);

/*!
 * \brief Update the FILTERs SBR profile in HW.
 *
 * \param [in] unit           Logical device id.
 * \param [in] trans_id       Transaction ID.
 * \param [in] tbl_id         Logical Table ID.
 * \param [in] stage_id       BCMFP stage ID.
 * \param [in] filter         Filter with current sbr.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_filter_update_sbr(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_filter_t *filter);
/*!
 * \brief Update the CID and CID_MASK of a FILTER
 *  in HW to new values.
 *
 * \param [in] unit           Logical device id.
 * \param [in] trans_id       Transactio ID.
 * \param [in] compress_id    Type of CID/CID_MASK.
 * \param [in] stage          BCMFP stage pointer.
 * \param [in] cid            New compression ID.
 * \param [in] cid_mask       New compression ID mask.
 * \param [in] filter         Filter to install in HW.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_filter_cid_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmfp_stage_id_t stage_id,
                        bcmltd_sid_t tbl_id,
                        uint8_t compress_type,
                        bcmfp_cid_t *cid,
                        bcmfp_cid_t *cid_mask,
                        bcmfp_filter_t *filter);
/*!
 * \brief Copy the FILTER to a different entry ID.
 *
 * \param [in] unit           Logical device id.
 * \param [in] trans_id       Transactio ID.
 * \param [in] stage_id       BCMFP stage ID.
 * \param [in] child_eid      Entry ID to which filter
 *                            has to be copied.
 * \param [in] filter         Filter to install in HW.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_filter_copy(int unit,
                  const bcmltd_op_arg_t *op_arg,
                  bcmfp_stage_id_t stage_id,
                  bcmltd_sid_t tbl_id,
                  bcmfp_entry_id_t child_eid,
                  bcmfp_filter_t *filter);

/*!
 * \brief Copy the FILTER to a different entry ID and update
 *  the cid and cid_mask in it for the given compression
 *  type.
 *
 * \param [in] unit           Logical device id.
 * \param [in] trans_id       Transactio ID.
 * \param [in] stage_id       BCMFP stage ID.
 * \param [in] child_eid      Entry ID to which filter
 *                            has to be copied.
 * \param [in] filter         Filter to install in HW.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_filter_copy_cid_update(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmfp_stage_id_t stage_id,
                             bcmltd_sid_t tbl_id,
                             uint8_t compress_type,
                             bcmfp_cid_t *cid,
                             bcmfp_cid_t *cid_mask,
                             bcmfp_entry_id_t parent_eid,
                             bcmfp_filter_t *filter);
/*!
 * \brief Delete the FILTER in HW.
 *
 * \param [in] unit        Logical device id.
 * \param [in] trans_id    Transaction ID.
 * \param [in] tbl_id      Logical Table ID.
 * \param [in] stage_id    BCMFP stage ID.
 * \param [in] filter      Access control list.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_filter_delete(int unit,
                 const bcmltd_op_arg_t *op_arg,
                 bcmltd_sid_t tbl_id,
                 bcmfp_stage_id_t stage_id,
                 bcmfp_filter_t *filter);

/*!
 * \brief Lookup the PSE in HW.
 *
 * \param [in] unit        Logical device id.
 * \param [in] trans_id    Transaction ID.
 * \param [in] tbl_id      Logical Table ID.
 * \param [in] stage_id    BCMFP stage ID.
 * \param [in] filter      Access control list.
 * \param [in] data        BCMLTD buffers for data fields.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_filter_pse_lookup(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_filter_t *filter,
                        bcmltd_fields_t *data);

/*!
 * \brief Lookup the FILTER in HW.
 *
 * \param [in] unit        Logical device id.
 * \param [in] trans_id    Transaction ID.
 * \param [in] tbl_id      Logical Table ID.
 * \param [in] stage_id    BCMFP stage ID.
 * \param [in] filter      Access control list.
 * \param [in] data        BCMLTD buffers for data fields.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_filter_lookup(int unit,
                    const bcmltd_op_arg_t *op_arg,
                    bcmltd_sid_t tbl_id,
                    bcmfp_stage_id_t stage_id,
                    bcmfp_filter_t *filter,
                    bcmltd_fields_t *data);
/*!
 * \brief Free the different configurations created in filter.
 *
 * \param [in] unit        Logical device id.
 * \param [in] filter      BCMFP filter structure.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_filter_configs_free(int unit,
                          bcmfp_filter_t *filter);
/*!
 * \brief Compress the filter if compression is enabled.
 *
 * \param [in] unit         Logical device id.
 * \param [in] trans_id     Transaction ID.
 * \param [in] stage_id     Logical Table ID.
 * \param [in] opcode       BCMFP opcode.
 * \param [in] prev_filter  Previous filter.
 * \param [in] curr_filter  Current filter.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_filter_compress(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_opcode_t opcode,
                      bcmfp_filter_t *prev_filter,
                      bcmfp_filter_t *curr_filter);

/*!
 * \brief  Update the presel config in the hardware.
 *
 * \param [in] unit        Logical device id.
 * \param [in] trans_id    Transaction ID.
 * \param [in] stage       BCMFP stage pointer.
 * \param [in] filter      filter.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_filter_presel_update(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_filter_t *filter);

/*!
 * \brief Program SBR config in the FILTER in HW.
 *
 * \param [in] unit        Logical device id.
 * \param [in] trans_id    Transaction ID.
 * \param [in] tbl_id      Logical Table ID.
 * \param [in] is_default  Default SBR or not.
 * \param [in] stage_id    BCMFP stage id.
 * \param [in] filter      Access control list.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_filter_sbr_config_update_process(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t tbl_id,
                             bool is_default,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_filter_t *filter);

#endif /* BCMFP_CTH_FILTER_H */
