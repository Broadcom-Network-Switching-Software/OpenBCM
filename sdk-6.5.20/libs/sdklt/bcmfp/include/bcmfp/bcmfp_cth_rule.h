/*! \file bcmfp_cth_rule.h
 *
 * Defines, Enums, Structures and APIs corresponding to
 * FP rule LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_CTH_RULE_H
#define BCMFP_CTH_RULE_H

#include <shr/shr_bitop.h>
#include <bcmfp/bcmfp_qual_internal.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/bcmfp_tbl_internal.h>
#include <bcmfp/bcmfp_trie_mgmt.h>

/*! Compress key information in rule to be installed in ALPM. */
typedef struct bcmfp_rule_compress_key_s {
    /*!
     * Compress key consisting of all compress
     * fields keys together.
     */
    uint32_t key[BCMFP_MAX_WSIZE];

    /*!
     * Compress mask consisting of all compress
     * fields masks together.
     */
    uint32_t mask[BCMFP_MAX_WSIZE];

    /*! NUmber of consecutive ones in mask from MSB. */
    uint32_t prefix;

    /*! Maximum key size */
    uint32_t key_size;

    /*! Compress ID of the key */
    bcmfp_cid_t cid;

    /*! Compress ID mask of the key */
    bcmfp_cid_t cid_mask;

    /*! Compression FID info to be used for this compress key. */
    bcmfp_tbl_compress_fid_info_t *cfid_info;

    /*! Trie to which this compress key belongs to. */
    bcmfp_shr_trie_md_t *trie;

    /*! Next compress key information in the rule. */
    struct bcmfp_rule_compress_key_s *next;

} bcmfp_rule_compress_key_t;

/*! Configuration provided in FP rule LT entry. */
typedef struct bcmfp_rule_config_s {
    /*! Key */
    uint32_t rule_id;

    /*! Type of PORTS bitmap qualifier if present. */
    bcmfp_qual_pbmp_type_t pbmp_type;

    /*! PBMP qualifier ID */
    bcmfp_qualifier_t pbmp_qual_id;

    /*! Qualifier pbmp data. */
    bcmdrd_pbmp_t pbmp_data;

    /*! Qualifier pbmp Mask. */
    bcmdrd_pbmp_t pbmp_mask;

    /*! Different compress types expected to be applied. */
    bool compress_types[BCMFP_COMPRESS_TYPES_MAX];

    /*! Pointer to linked list of qualifier configurations. */
    bcmfp_qual_data_t *qual_data;
} bcmfp_rule_config_t;

/*!
 * \brief Get the information provided in FP rule LT entry into
 * bcmfp_rule_config_t strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 * \param [out] config            FP rule LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_rule_config_get(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t tbl_id,
                       bcmfp_stage_id_t stage_id,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       bcmfp_rule_config_t **config);

/*!
 * \brief If group is enabled for ACL compression, replace all
 *  the different compression key types key and mask FIDs with
 *  corresponding CID key and mask FIDs in the rule.
 *
 * \param [in] unit               Logical device id.
 * \param [in] cid_qual_data      Linked list all CIDs qual data.
 * \param [out] config            FP group LT entry configuration
 *                                to be compressed.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_rule_config_compress(int unit,
                           bcmfp_qual_data_t *cid_qual_data,
                           bcmfp_rule_config_t *config);
/*!
 * \brief Get the information provided in FP rule LT entry with
 * rule_id as key into bcmfp_rule_config_t strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] rule_id            BCMFP rule ID.
 * \param [out] config            FP rule LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_rule_remote_config_get(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_rule_id_t rule_id,
                             bcmfp_rule_config_t **config);
/*!
 * \brief Insert the entry into rule template LT with a given key.
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
bcmfp_rule_config_insert(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t tbl_id,
                         bcmfp_stage_id_t stage_id,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         bcmltd_fields_t *output_fields);

/*!
 * \brief Update the entry in rule template LT with a given key.
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
bcmfp_rule_config_update(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t tbl_id,
                         bcmfp_stage_id_t stage_id,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         bcmltd_fields_t *output_fields);

/*!
 * \brief  Delete the entry from rule template LT with a given key.
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
bcmfp_rule_config_delete(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t tbl_id,
                         bcmfp_stage_id_t stage_id,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         bcmltd_fields_t *output_fields);
/*!
 * \brief  Free the memory created for FP rule LT entry configuration.
 *
 * \param [in] unit         Logical device id.
 * \param [in] rule_config  FP rule LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmfp_rule_config_free(int unit,
                       bcmfp_rule_config_t *rule_config);
/*!
 * \brief  Free the memory created for FP rule LT entry configuration.
 *
 * \param [in] unit         Logical device id.
 * \param [in] rule_config  FP rule LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 */
extern void
bcmfp_rule_compress_keys_free(int unit,
                              bcmfp_rule_compress_key_t *ckeys);
/*!
 * \brief  Get the compression keys possible in rule.
 *
 * \param [in] unit      Logical device id.
 * \param [in] stage_id  BCMFP stage ID.
 * \param [in] group_id  BCMFP stage ID.
 * \param [in] rconfig   FP rule LT entry configuration
 * \param [out] ckeys    Linked list of possible compression keys
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_rule_compress_keys_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_id_t group_id,
                             bcmfp_rule_config_t *rconfig,
                             bcmfp_rule_compress_key_t **ckeys);
/*!
 * \brief Mark all the qualifiers which can be compressed in
 *  the rule .
 *
 * \param [in] unit      Logical device id.
 * \param [in] stage_id  BCMFP stage ID.
 * \param [in] rconfig   FP rule LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_rule_compress_qualifiers_mark(int unit,
                                    bcmfp_stage_id_t stage_id,
                                    bcmfp_rule_config_t *rconfig);
/*!
 * \brief Check if qualifiers data which are not marked for
 *  compression have same data in two rule configs.
 *
 * \param [in] unit      Logical device id.
 * \param [in] rconfig1  FP rule LT entry configuration
 * \param [in] rconfig2  FP rule LT entry configuration
 * \param [in] matched   rconfig1 and rconfig2 have the
 *                       same data for qualifiers which
 *                       are not marked for compression.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_rule_non_compress_qualifiers_match(int unit,
                                   bcmfp_rule_config_t *rconfig1,
                                   bcmfp_rule_config_t *rconfig2,
                                   bool *matched);
#endif /* BCMFP_CTH_RULE_H */
