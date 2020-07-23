/*! \file bcmfp_cth_common.h
 *
 * Defines, Enums, Structures and APIs common to all
 * FP LTs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_CTH_COMMON_H
#define BCMFP_CTH_COMMON_H

#include <bcmlrd/bcmlrd_types.h>
#include <bcmfp/bcmfp_cth_group.h>
#include <bcmfp/bcmfp_cth_rule.h>
#include <bcmfp/bcmfp_cth_policy.h>
#include <bcmfp/bcmfp_cth_entry.h>
#include <bcmfp/bcmfp_cth_sbr.h>
#include <bcmfp/bcmfp_cth_pdd.h>
#include <bcmfp/bcmfp_cth_psg.h>
#include <bcmfp/bcmfp_cth_pse.h>
#include <bcmfp/bcmfp_cth_meter.h>
#include <bcmfp/bcmfp_cth_range_check.h>
#include <bcmfp/bcmfp_cth_src_class.h>
#include <bcmimm/bcmimm_basic.h>
#include <bcmfp/bcmfp_trie_mgmt.h>

/*
 * There are some LT fields which are more than 64 bits
 * in size or array type LT fields. Such fields are
 * considered as multiple 64 bit chunks with different
 * indexes but same FID. This results in increase of field
 * count while dealing with IMM back end APIs. This macro
 * is set to the maximum of sum of all array sizes across
 * all FP LTs.
 */
#define BCMFP_NUM_FID_EXCESS 64

/* Different BCMFP LT types. */
typedef enum bcmfp_sid_type_s {
    /* FP_XXX_GRP_TEMPLATE LT */
    BCMFP_SID_TYPE_GRP_TEMPLATE = 0,
    /* FP_XXX_RULE_TEMPLATE LT */
    BCMFP_SID_TYPE_RULE_TEMPLATE,
    /* FP_XXX_POLICY_TEMPLATE LT */
    BCMFP_SID_TYPE_POLICY_TEMPLATE,
    /* FP_XXX_PRESEL_ENTRY_TEMPLATE LT */
    BCMFP_SID_TYPE_PSE_TEMPLATE,
    /* FP_XXX_ENTRY LT */
    BCMFP_SID_TYPE_ENTRY,
    /* FP_XXX_SRC_CLASS_MODE LT */
    BCMFP_SID_TYPE_SRC_CLASS_MODE,
    /* METER_FP_XXX_TEMPLATE LT */
    BCMFP_SID_TYPE_METER_TEMPLATE,
    /* FP_XXX_PDD_TEMPLATE LT */
    BCMFP_SID_TYPE_PDD_TEMPLATE,
    /* FP_XXX_SBR_TEMPLATE LT */
    BCMFP_SID_TYPE_SBR_TEMPLATE,
    /* FP_XXX_PRESEL_GRP_TEMPLATE LT */
    BCMFP_SID_TYPE_PSG_TEMPLATE,
    /* CTR_EGR_FP_ENTRY LT */
    BCMFP_SID_TYPE_CTR_ENTRY,
    /* Invalid SID type */
    BCMFP_SID_TYPE_COUNT,
} bcmfp_sid_type_t;

/*! Different LTD buffers used in IMM backend operations. */
typedef struct bcmfp_ltd_buffers_s {
    /*!
     * Array of pointers for max number of key fields
     * amongst all LTs supported in the stage.
     */
    bcmltd_field_t **key_fields;

    /*! Number of field pointers in key_fields. */
    uint16_t num_key_fields;

    /*!
     * Array of pointers for max number of data fields
     * amongst all LTs supported in the stage.
     */
    bcmltd_field_t **data_fields;

    /*! Number of field pointers in data_fields. */
    uint16_t num_data_fields;

    /*!
     * Array of pointers for max number of non default
     * data fields amongst all LTs supported in the
     * stage.
     */
    bcmltd_field_t **non_def_data_fields_1;

    /*! Number of field pointers in non_def_data_fields. */
    uint16_t num_non_def_data_fields_1;

    /*! Flag indicating if non_def_data_fields_1 is in use. */
    bool non_def_data_fields_1_in_use;

    /*!
     * Array of pointers for max number of non default
     * data fields amongst all LTs supported in the
     * stage.
     */
    bcmltd_field_t **non_def_data_fields_2;

    /*! Number of field pointers in non_def_data_fields_2. */
    uint16_t num_non_def_data_fields_2;

} bcmfp_ltd_buffers_t;

/*!
 * Information required to process the existing
 * entry if any of the mapped template ids(group,
 * rule, policy) are inserted or updated or deleted.
 */
typedef struct bcmfp_idp_info_s {
    /*! LT Operational arg */
    bcmltd_op_arg_t *op_arg;

    /*!
     * Type of the BCMFP LT entry that is inserted,
     *  or updated, or deleted and associated to the
     *  remote LT.
     */
    uint32_t sid_type;

    /* Table ID corresponding to the event. */
    bcmltd_sid_t tbl_id;
    /*!
     * Key ID values for the SID type mentioned.
     * Only update for second pass call back. For
     * example pse_id -> group_id -> entry_id.
     */
    uint32_t template_id;

    /*! Key fields and its values in modified LT. */
    const bcmltd_field_t *key;

    /*! Data fields and its values in modified LT. */
    const bcmltd_field_t *data;

    /*!
     * Output parameter can be used by the component
     * to add fields into the entry. Typically this
     * should be used for read-only type fields,
     * otherwise can not be set by the application.
     */
    bcmltd_fields_t *output_fields;

    /*! INSERT or DELETE or UPDATE */
    bcmimm_entry_event_t event_reason;

    /*
     * Oper state of the entry w.r.t the current
     * BCMFP LT.
     */
    bcmfp_entry_state_t entry_state;

    /*! Compress key. */
    uint32_t *ckey;

    /*! Prefix length of compress key mask. */
    uint32_t prefix;

    /*! CID used currenlty in the entry. */
    bcmfp_cid_t *cid_curr;

    /*! CID_MASK used currenlty in the entry. */
    bcmfp_cid_t *cid_mask_curr;

    /*! CID to be used in the entry. */
    bcmfp_cid_t *cid_next;

    /*! CID_MASK to be used in the entry. */
    bcmfp_cid_t *cid_mask_next;

    /*!
     * Compression type in which CID and CID_MASK
     * inserted/deleted/updated.
     */
    uint8_t compress_type;

    /* Compress key is deleted in the current call path. */
    bool compress_delete;

    /*!
     * Parent entry ID of a list of list compressed
     * FP entries.
     */
    bcmfp_entry_id_t list_compress_parent_entry_id;

    /*! List compressed FP entry ID. */
    bcmfp_entry_id_t list_compress_child_entry_id;

    /*!
     * Parent entry ID of list compressed entry ID
     * is found.
     */
    bool list_compress_parent_found;

    /*! Group priority for pse validation. */
    uint16_t group_prio;

    /*! Group ID for pse validation. */
    uint32_t group_id;

    /*! Entry ID for SBR hw index caliculation. */
    uint32_t entry_id;

    /*! Bitmap of FP entry LT ID's. */
    uint32_t *entry_id_bmp;

    /*!
     * 1.Number of entry IDs associated to rule (or)
     *   (or) policy (or) meter template ID.
     * 2.Number of group IDs associated to presel (or)
     *   presel group (or) pdd (or) SBR template ID.
     */
    uint32_t entry_or_group_count;

    void *generic_data;

    uint32_t cid_update_flags;
} bcmfp_idp_info_t;

/*! Create memory for entry key words. */
#define BCMFP_BUFFERS_CREATE_EKW  (0x1 << 0)

/*! Create memory for entry data words. */
#define BCMFP_BUFFERS_CREATE_EDW  (0x1 << 1)

/*! Create memory for entry default data words. */
#define BCMFP_BUFFERS_CREATE_DEDW (0x1 << 2)

/*! Create memory for PDD profiles. */
#define BCMFP_BUFFERS_CREATE_PDD  (0x1 << 3)

/*! Create memory for default PDD profiles. */
#define BCMFP_BUFFERS_CREATE_DPDD (0x1 << 4)

/*! Create memory for SBR profiles. */
#define BCMFP_BUFFERS_CREATE_SBR  (0x1 << 5)

/*! Create memory for PSG profiles. */
#define BCMFP_BUFFERS_CREATE_PSG  (0x1 << 6)

/*! Create memory for QOS profiles. */
#define BCMFP_BUFFERS_CREATE_QOS  (0x1 << 7)

/*! Create memory for default QOS profiles. */
#define BCMFP_BUFFERS_CREATE_DQOS (0x1 << 8)

/*! Create memory for default SBR profiles. */
#define BCMFP_BUFFERS_CREATE_DSBR (0x1 << 9)

/*! Create memory for all required buffers. */
#define BCMFP_BUFFERS_CREATE_ALL 0xFFFFFFFF

/*
 * Different buffers required during the while
 * inserting, deleting or updating the FP entries.
 */
typedef struct bcmfp_buffers_s {
    /*! Buffer for entry key words */
    uint32_t **ekw;

    /*! Buffer for entry data words */
    uint32_t **edw;

    /*! Buffer for default policy words */
    uint32_t **dedw;

    /*! Buffer for PDD profiles */
    uint32_t **pdd;

    /*! Buffer for default PDD profiles */
    uint32_t **dpdd;

    /*! Buffer for SBR profiles */
    uint32_t **sbr;

    /*! Buffer for default SBR profiles */
    uint32_t **dsbr;

    /*! Buffer for PSG profile */
    uint32_t **psg;

    /*! Buffer for QOS profile */
    uint32_t **qos;

    /*! Buffer for default QOS profile */
    uint32_t **dqos;

} bcmfp_buffers_t;

/*!
 * BCMFP_BUFFERS_CUSTOMIZE_XXX flags are to control chip
 *  specific customizations to different buffers created
 *  and populated before push them to HW.
 */
/*! Make chip specific customizations to ekw buffers. */
#define BCMFP_BUFFERS_CUSTOMIZE_EKW (0x1 << 0)

/*! Make chip specific customizations to edw buffers. */
#define BCMFP_BUFFERS_CUSTOMIZE_EDW (0x1 << 1)

/*! Diffrent BCMFP LT Key values. */
typedef struct bcmfp_key_ids_s {
    /*! BCMFP group LT ID value */
    bcmfp_group_id_t group_id;

    /*! BCMFP entry LT ID value */
    bcmfp_entry_id_t entry_id;

    /*! BCMFP rule LT ID value */
    bcmfp_rule_id_t rule_id;

    /*! BCMFP policy LT ID value */
    bcmfp_policy_id_t policy_id;

    /*! BCMFP SBR LT ID value */
    bcmfp_sbr_id_t sbr_id;

    /*! BCMFP PDD LT ID value */
    bcmfp_pdd_id_t pdd_id;
} bcmfp_key_ids_t;

/*!
 * \brief Get the LT field value, for the given fid, from the list
 *  of field values.
 *
 * \param [in] unit              Logical device id.
 * \param [in] fid               Logical table field ID
 * \param [in] fid_values        List of field values.
 * \param [out] fid_values       Field value.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_fid_value_get(int unit,
                    uint32_t fid,
                    void *fid_values,
                    uint64_t *fid_value);


/*!
 * \brief Get the max LT field value, for the given fid
 *
 * \param [in] unit              Logical device id.
 * \param [in] ltid              Logical table ID
 * \param [in] fid               Logical table field ID
 * \param [out] max_val          Max value.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */

extern int
bcmfp_fid_max_value_get(int unit,
                            bcmlrd_sid_t ltid,
                            bcmlrd_fid_t fid,
                            uint32_t *max_val);


/*!
 * \brief Get the LT array field value, for the given fid,
 * from the list of field values.
 *
 * \param [in] unit              Logical device id.
 * \param [in] fid               Logical table field ID
 * \param [in] index             Index in the array
 * \param [in] fid_values        List of field values.
 * \param [out] fid_values       Field value.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_fid_array_value_get(int unit,
                          uint32_t fid,
                          uint8_t index,
                          void *fid_values,
                          uint64_t *fid_value);

/*!
 * \brief Get minimum, maximum and default values from
 *  LRD field data.
 *
 * \param [in] unit              Logical device id.
 * \param [in] ltid      ......  Logical table id.
 * \param [in] fid               Field id
 * \param [out] min_value        Minimum values of the field.
 * \param [out] max_value        Maximum values of the field.
 * \param [out] def_value        Default values of the field.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_fid_min_max_default_get(int unit,
                              bcmlrd_sid_t ltid,
                              bcmlrd_fid_t fid,
                              uint64_t *min_value,
                              uint64_t *max_value,
                              uint64_t *def_value);
/*!
 * \brief Copy uint64_t value to array of uint32_t as per the
 *  field width, index and data provided.
 *
 * \param [in] unit         Logical device id.
 * \param [in] field_width  Width of the LT field.
 * \param [in] field_data   uint64_t field  value to be copied.
 * \param [in] field_idx    Nth uint64_t data of the field.
 * \param [out] field_value pointer to uint32 array.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_tbl_field_value_get(int unit,
                          uint64_t field_data,
                          uint32_t field_width,
                          uint32_t field_idx,
                          uint32_t *field_value);
/*!
 * \brief Check all valid bits in the field value is set or not.
 *
 * \param [in] unit         Logical device id.
 * \param [in] width        Number of valid bits .
 * \param [in] u64_value    LT field value in uint64.
 * \param [out] is_partial  TRUE - All valid bits in LT
 *                                 field value are not set.
 *                          FALSE - All valid bits in LT
 *                                  field value are set.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_fid_value_is_partial(int unit,
                           uint32_t width,
                           uint64_t u64_value,
                           bool *is_partial);
/*!
 * \brief Do IMM lookup in tbl_id for gievn key and key value.
 *
 * \param [in] unit          Logical device id.
 * \param [in] tbl_id        Logical table ID
 * \param [in] key_fid       Logical table key field ID
 * \param [in] key_value     Key value to be looked up
 * \param [in] num_keys      Number of key fids present in tbl_id
 * \param [in] num_fid       NUmber of out fids preset in tbl_id
 * \param [out] in           BCMLTD key fields for IMM lookup.
 * \param [out] out          BCMLTD data fields for IMM lookup.
 * \param [in] buffers       LTD buffers to be used in IMM backend calls.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_imm_entry_lookup(int unit,
                       bcmltd_sid_t tbl_id,
                       bcmltd_fid_t *key_fid,
                       uint32_t *key_value,
                       uint8_t num_keys,
                       uint16_t num_fid,
                       bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       bcmfp_ltd_buffers_t *buffers);
/*!
 * \brief Update and entry in tbl_id for gievn key and key value.
 *
 * \param [in] unit              Logical device id.
 * \param [in] tbl_id            Logical table ID
 * \param [in] key_fid           Logical table key field ID
 * \param [in] key_value         Key value to be looked up
 * \param [in] num_keys          Number of key fids present in tbl_id
 * \param [in] data_fid          Logical table data field ID
 * \param [in] data_value        Data value to be updated
 * \param [in] num_data          NUmber of data fids preset in tbl_id
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_imm_entry_update(int unit,
                       bcmltd_sid_t tbl_id,
                       bcmltd_fid_t *key_fid,
                       uint32_t *key_value,
                       uint8_t num_keys,
                       bcmltd_fid_t *data_fid,
                       uint32_t *data_value,
                       uint8_t num_data);
/*!
 * \brief Free the memory created for elements in bcmltd_fields_t
 *  type variables.
 *
 * \param [in] unit       Logical device id.
 * \param [in] fields     BCMLTD data fields.
 * \param [in] num_fid    Number of function pointers in fields->field.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_ltd_buffers_free(int unit,
                       bcmltd_fields_t *fields,
                       uint16_t num_fid);
/*!
 * \brief Free the array of bcmltd_field_t buffers.
 *
 * \param [in] unit       Logical device id.
 * \param [in] fields     BCMLTD fields array.
 * \param [in] num_fid    Number of elements in fields array.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_ltd_fields_array_free(int unit,
                            bcmltd_field_t **fields,
                            uint16_t num_fid);
/*!
 * \brief Free the linked list of bcmltd_field_t buffers.
 *
 * \param [in] unit       Logical device id.
 * \param [in] field      BCMLTD fields list.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_ltd_fields_list_free(int unit,
                           bcmltd_field_t *fields);
/*!
 * \brief Add a new FID and FID Data to the LTD filed buffers.
 *
 * \param [in] unit     Logical device id.
 * \param [in] fid      Logical table field ID
 * \param [in] data     FID data to be added
 * \param [in] fields   BCMLTD field buffers.
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_ltd_field_add(int unit,
                    bcmltd_fid_t fid,
                    uint32_t data,
                    bcmltd_fields_t *fields);
/*!
 * \brief Build list of new ltd fields from both fields in
 * incoming data and imm data. If a field is present in both
 * the places give preference to incoming data.
 *
 * \param [in] unit           Logical device id.
 * \param [in] tbl_id         Logical table ID
 * \param [in] num_fid        Number of out fids preset in tbl_id
 * \param [in] key            BCMLTD key field buffers.
 * \param [in] data           BCMLTD data field buffers.
 * \param [out] updated_data  BCMLTD updated field buffers.
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_ltd_updated_data_get(int unit,
                           bcmltd_sid_t tbl_id,
                           uint16_t num_fid,
                           bcmltd_field_t *key,
                           bcmltd_field_t *data,
                           bcmltd_fields_t *updated_data);

/*!
 * \brief Build list of ltd fields with non default values
 *  from the list of input fields in a given LT.
 *
 * \param [in] unit               Logical device id.
 * \param [in] tbl_id             Logical table ID
 * \param [in] data               BCMLTD data field buffers.
 * \param [out] non_default_data  BCMLTD field buffers with
 *                                non default values.
 * \param [in] buffers            LTD buffers to be used in IMM
 *                                backend calls.
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_ltd_non_default_data_get(int unit,
                               bcmltd_sid_t tbl_id,
                               bcmltd_field_t *data,
                               bcmltd_field_t **non_default_data,
                               bcmfp_ltd_buffers_t *buffers);

/*!
 * \brief Get the per pipe PBMP from device PBMP for the given
 *  pipe ID.
 *
 * \param [in] unit         Logical device id.
 * \param [in] device_pbmp  Device PBMP(include all pipes PBMP)
 * \param [in] tbl_inst     Device pipe ID.
 * \param [out] pipe_pbmp   Per pipe PBMP.
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_device_ipbm_to_pipe_ipbmp(int unit,
                                bcmdrd_pbmp_t *device_pbmp,
                                int8_t tbl_inst,
                                bcmdrd_pbmp_t *pipe_pbmp);

/*!
 * \brief Get the per pipe PBMP from device PBMP for the given
 *  pipe ID.
 *
 * \param [in] unit         Logical device id.
 * \param [in] pipe_pbmp    Per pipe PBMP.
 * \param [in] tbl_inst     Device pipe ID.
 * \param [out] device_pbmp Device PBMP.
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_pipe_ipbmp_to_device_pbmp(int unit,
                                int8_t tbl_inst,
                                bcmdrd_pbmp_t *pipe_pbmp,
                                bcmdrd_pbmp_t *device_pbmp);

/*!
 * \brief Get the PORTS type qualifer raw data provided in LT
 *  field in PBMP format.
 *
 * \param [in] unit      Logical device id.
 * \param [in] field     INPORTS LT field raw data.
 * \param [out] pbmp     INPORTS PBMP.
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_ports_bitmap_get(int unit,
                       bcmltd_field_t *field,
                       bcmdrd_pbmp_t *pbmp);
/*!
 * \brief Validate the PBMP with PBMP get from the DRD.
 *
 * \param [in] unit      Logical device id.
 * \param [in] pipe_id   Pipe ID.
 * \param [in] stage_id  Stage ID.
 * \param [in] in_pbmp   Port bitmap comprised of one
 *                       or more ports from the pipe.
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 * \retval SHR_E_CONFIG in_pbmp has ports doesnot belong
 *                      to the pipe_id.
 */
extern int
bcmfp_pbmp_validate(int unit,
                    int pipe_id,
                    bcmfp_stage_id_t stage_id,
                    bcmdrd_pbmp_t *in_pbmp);

/*!
 * \brief free the qualifier bit map list.
 *
 * \param [in] unit Logical device id
 * \param [in] qual_bitmap Pointer to the qual bit map list.
 *
 * \retval SHR_E_NONE   Success
 */
extern int
bcmfp_qual_bitmap_free(int unit,
                       bcmfp_qual_bitmap_t *qual_bitmap);

/*!
 * \brief Check if range of bits are all ones.
 *
 * \param [in] buffer Bitstream to be verified.
 * \param [in] offset Start bit in the bit stream.
 * \param [in] width Number of bits in the bit stream.
 *
 * \retval FALSE Not all bits in the range are ones.
 * \retval TRUE  All bits in the range are ones.
 */
extern bool
bcmfp_bits_ones(uint32_t *buffer,
                int offset,
                int width);
/*!
 * \brief Check if range of bits are all zeros.
 *
 * \param [in] buffer Bitstream to be verified.
 * \param [in] offset Start bit in the bit stream.
 * \param [in] width Number of bits in the bit stream.
 *
 * \retval FALSE Not all bits in the range are zeros.
 * \retval TRUE  All bits in the range are zeros.
 */
extern bool
bcmfp_bits_zeros(uint32_t *buffer,
                 int offset,
                 int width);

/*!
 * \brief Insert an entry in tbl_id for gievn key and key value.
 *
 * \param [in] unit              Logical device id.
 * \param [in] tbl_id            Logical table ID
 * \param [in] key_fid           Logical table key field ID
 * \param [in] key_value         Key value to be looked up
 * \param [in] num_keys          Number of key fids preset in tbl_id
 * \param [in] num_fid           Number of out fids preset in tbl_id
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_imm_entry_insert(int unit,
                       bcmltd_sid_t tbl_id,
                       bcmltd_fid_t *key_fid,
                       uint32_t *key_value,
                       uint8_t num_keys);

/*!
 * \brief Delete an entry in tbl_id for gievn key and key value.
 *
 * \param [in] unit              Logical device id.
 * \param [in] tbl_id            Logical table ID
 * \param [in] key_fid           Logical table key field ID
 * \param [in] key_value         Key value to be looked up
 * \param [in] num_keys          Number of key fids preset in tbl_id
 * \param [in] num_fid           Number of out fids preset in tbl_id
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_imm_entry_delete(int unit,
                       bcmltd_sid_t tbl_id,
                       bcmltd_fid_t *key_fid,
                       uint32_t *key_value,
                       uint8_t num_keys);

/*!
 * \brief Update an array field in entry in tbl_id for given key and key value.
 *
 * \param [in] unit              Logical device id.
 * \param [in] tbl_id            Logical table ID
 * \param [in] key_fid           Logical table key field ID
 * \param [in] key_value         Key value to be looked up
 * \param [in] num_keys          Number of key fids present in tbl_id
 * \param [in] data_fid          Logical table data field ID
 * \param [in] data_value        Data value to be updated
 * \param [in] num_data          NUmber of data fid arrays indexes present in fid
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_imm_entry_update_array(int unit,
                       bcmltd_sid_t tbl_id,
                       bcmltd_fid_t *key_fid,
                       uint32_t *key_value,
                       uint8_t num_keys,
                       bcmltd_fid_t data_fid,
                       uint32_t *data_value,
                       uint8_t num_data);

/*!
 * \brief Get 32 bit value from field mentioned in LTID buffer.
*
 * \param [in] unit              Logical device id.
 * \param [in] buffer            Logical table ID buffer.
 * \param [in] id_fid            Logical table key field ID
 * \param [out] id                Field valu in buffer.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */

extern int
bcmfp_fid_value_u32_get(int unit,
             bcmltd_field_t *buffer,
             uint32_t id_fid,
             uint32_t *id);

/*!
 * \brief Get the prefix length of a mask.
 *
 * \param [in] unit               Logical device id.
 * \param [in] mask               Array of words for which
 *                                prefix length has to be
 *                                caliculated..
 * \param [in] max_prefix_length  Maximum prefix length.
 * \param [out] prefix_length     Prefix length of the mask.
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_prefix_length_get(int unit,
                        uint32_t *mask,
                        uint16_t max_prefix_length,
                        uint16_t *prefix_length);
#endif /* BCMFP_CTH_COMMON_H */
