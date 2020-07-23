/*! \file bcmfp_compression_internal.h
 *
 * API declarations, structures, enumerations and constants
 * corresponding to compression.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_COMPRESSION_INTERNAL_H
#define BCMFP_COMPRESSION_INTERNAL_H

#include <bcmfp/bcmfp_group_internal.h>
#include <bcmfp/bcmfp_qual_internal.h>
#include <bcmfp/generated/bcmfp_ha.h>
#include <bcmfp/bcmfp_cth_filter.h>
#include <bcmfp/bcmfp_trie_mgmt.h>

/*!
 * Metadata that needs to be passed by compression algorithm
 * back to BCMFP component. Different components using the
 * algorithm might require different call back data. Since
 * algorithm is independent of components using it, component
 * itself needs to pass this information to algorith when new
 * key is inserted or deleting existing key. Algorithim in
 * turn will pass it back this information to component when
 * there are changes in cid/cid_mask of existing keys.
 */
typedef struct bcmfp_cid_update_cb_info_s {
    /*! BCMFP stage ID. */
    bcmfp_stage_id_t stage_id;

    /*!
     * Compress type in which compress IDs are updated for
     * existing keys.
     */
    uint8_t compress_type;

    /*!LT Operation arg */
    bcmltd_op_arg_t *op_arg;

    /*! FP ALPM logical table ID */
    bcmltd_sid_t fp_alpm_sid;

    /*! Trie in which CID updates happened. */
    bcmfp_shr_trie_md_t *trie;

    /*! Some compression keys are moved from list to trie. */
    bool keys_moved_from_list_to_trie;

    bcmimm_entry_event_t event;
} bcmfp_cid_update_cb_info_t;

#define BCMFP_COMRESSION_KEY_ENTRY_ARRAY_SIZE 0xF

/*!
 * Compression key specific information like FP
 * entries using it.
 */
typedef struct bcmfp_compress_key_info_s {

    /* Array of entries using the key. */
    bcmfp_entry_id_t *entries;

    /*! Valid elements in entries array. */
    uint32_t num_entries;

    /*! Size of entries array. */
    uint32_t entries_size;

    /*! Compression key */
    bcmfp_key_t key;

    /*! Compression key prefix (MSB 1s in mask) */
    uint32_t prefix;

    /*! Next compression key */
    struct bcmfp_compress_key_info_s *next;

} bcmfp_compress_key_info_t;

/*!
 * List compressed data required to list compress
 * an FP entry.
 */
typedef struct bcmfp_list_compress_data_s {
    /* LT Operation arg */
    bcmltd_op_arg_t *op_arg;

    /*!
     * FP filter on which list compression has
     * to be tried.
     */
    bcmfp_filter_t *filter;

    /*! BCMFP stage to which this filter belongs. */
    bcmfp_stage_id_t stage_id;

    /*!
     * Rule config of the FP entry with which this
     * filter will be list compressed.
     */
    bcmfp_rule_config_t *rule_config;

    /*!
     * Indicates whether the filter matches all
     * the criterias to list compress it with
     * another FP entry.
     */
    bool criteria_matched;

    /*!
     * CID to be used for all the compression types
     * in case filter is qualified for list compression.
     */
    bcmfp_cid_t cid[BCMFP_COMPRESS_TYPES_MAX];

    /*!
     * CID_MASK to be used for all the compression types
     * in case filter is qualified for list compression.
     */
    bcmfp_cid_t cid_mask[BCMFP_COMPRESS_TYPES_MAX];

    /*!
     * Entry ID width which the filter has to be list
     * list compressed.
     */
    bcmfp_entry_id_t entry_id;
} bcmfp_list_compress_data_t;

/*!
 * \brief Check any entry IDs are mapped to (CID, CID_MASK) pair.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] compress_type Tree to which CID and CID_MASK belongs to.
 * \param [in] cid_and_cid_mask AND of CID and CID_MASK.
 * \param [in] not_mapped FALSE -> At least one FP entry is mapped.
 *                        TRUE -> No FP entry is mapped.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Wrong parameters
 * \retval SHR_E_NOT_FOUND (CID and CID_MASK) pair is not found.
 */
extern int
bcmfp_cid_entry_map_check(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint8_t compress_type,
                          uint32_t *cid_and_cid_mask,
                          bool *not_mapped);
/*!
 * \brief Dump CID&CID_MASK to entry id mappings of a given
 *  compression type of a stage.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] compress_type FP ALPM compression type.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Wrong parameters
 */
extern int
bcmfp_cid_entry_map_dump(int unit,
                         bcmfp_stage_id_t stage_id,
                         uint8_t compress_type);
/*!
 * \brief Traverse all entry IDs mapped to (CID, CID_MASK) pair.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] compress_type Tree to which CID and CID_MASK belongs to.
 * \param [in] cid_and_cid_mask AND of CID and CID_MASK.
 * \param [in] cb Callback function called for every entry_id mapped to
 *                the (CID, CID_MASK) pair
 * \param [in] user_data User data to be passed to call back function.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Wrong parameters
 * \retval SHR_E_NOT_FOUND (CID and CID_MASK) pair is not found.
 */
extern int
bcmfp_cid_entry_map_traverse(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint8_t compress_type,
                             uint32_t *cid_and_cid_mask,
                             bcmfp_entry_id_traverse_cb cb,
                             void *user_data);


/*!
 * \brief Delete (CID and CID_MASK) to entry_id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] compress_type Tree to which CID and CID_MASK belongs to.
 * \param [in] cid_and_cid_mask AND of CID and CID_MASK.
 * \param [in] entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Wrong parameters
 */
extern int
bcmfp_cid_entry_map_add(int unit,
                        bcmfp_stage_id_t stage_id,
                        uint8_t compress_type,
                        uint32_t *cid_and_cid_mask,
                        bcmfp_entry_id_t entry_id);

/*!
 * \brief Delete (CID and CID_MASK) to entry_id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] compress_type Tree to which CID and CID_MASK belongs to.
 * \param [in] cid_and_cid_mask AND of CID and CID_MASK.
 * \param [in] entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Wrong parameters
 * \retval SHR_E_NOT_FOUND (CID and CID_MASK) pair is not found.
 */
extern int
bcmfp_cid_entry_map_delete(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint8_t compress_type,
                           uint32_t *cid_and_cid_mask,
                           bcmfp_entry_id_t entry_id);

/*!
 * \brief Updates CID/CID_MASK of existing ALPM keys and
 *  corresponding FP entries.
 *
 * \param [in] unit Logical device id
 * \param [in] cid_update_info Linked list of cid update info.
 * \param [in] user_data Data passed by component using the
 *                       compression algorithm.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Wrong parameters
 */
extern int
bcmfp_cid_updates(int unit,
                  bcmfp_cid_update_list_t *cid_update_info,
                  void *user_data);

/*!
 * \brief Add compression key to entry_id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] ctype Tree to which compression key belongs to.
 * \param [in] key Compression key.
 * \param [in] prefix Prefix of compression key.
 * \param [in] entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Wrong parameters
 */
extern int
bcmfp_ckey_entry_map_add(int unit,
                         bcmfp_stage_id_t stage_id,
                         uint8_t ctype,
                         uint32_t *key,
                         uint32_t prefix,
                         bcmfp_entry_id_t entry_id);
/*!
 * \brief Delete compression key to entry_id mapping.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] ctype Tree to which compression key belongs to.
 * \param [in] key Compression key.
 * \param [in] prefix Prefix of compression key.
 * \param [in] entry_id Index to FP_XX_ENTRY LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Wrong parameters
 * \retval SHR_E_NOT_FOUND Compression key is not found.
 */
extern int
bcmfp_ckey_entry_map_delete(int unit,
                            bcmfp_stage_id_t stage_id,
                            uint8_t ctype,
                            uint32_t *key,
                            uint32_t prefix,
                            bcmfp_entry_id_t entry_id);
/*!
 * \brief Traverse through all entry_ids mapped to given
 *  compression key.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] ctype Tree to which compression key belongs to.
 * \param [in] key Compression key.
 * \param [in] prefix Prefix of compression key.
 * \param [in] cb Callback function called for every entry_id mapped to
 *                the compression key.
 * \param [in] user_data User data to be passed to call back function.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Wrong parameters
 * \retval SHR_E_NOT_FOUND Compression key is not found.
 */
extern int
bcmfp_ckey_entry_map_traverse(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint8_t ctype,
                              uint32_t *key,
                              uint32_t prefix,
                              bcmfp_entry_id_traverse_cb cb,
                              void *user_data);
/*!
 * \brief Allocate HA block with BCMFP_HA_BLK_TYPE_CKEY if not
 *  available or available but no free compress keys in it and
 *  reserve one compress key node from it.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] ctype Compress type of the trie to which the
 *                   compression key will be inserted.
 * \param [in] ckey Pointer to compression key in HA block.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Wrong parameters
 */
extern int
bcmfp_compress_key_node_acquire(int unit,
                                uint8_t stage_id,
                                uint8_t ctype,
                                bcmfp_compress_key_t **ckey);
/*!
 * \brief Release the compression key to HA block to which it
 * belongs.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] ctype Compress type of the trie to which the
 *                   compression key was inserted.
 * \param [in] ckey Pointer to compression key in HA block.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Wrong parameters
 */
extern int
bcmfp_compress_key_node_release(int unit,
                                uint8_t stage_id,
                                uint8_t ctype,
                                bcmfp_compress_key_t *ckey);

/*!
 * \brief Recreate the compress trie if the HA block is created
 *  for compression keys in cold boot. This function is called
 *  only warm boot.
 *
 * \param [in] unit Logical device id
 * \param [in] ha_mem Pointer to HA block.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Wrong parameters
 */
extern int
bcmfp_compress_trie_recreate(int unit, void *ha_mem);

extern int
bcmfp_compress_oper_info_create(int unit,
                                bcmfp_stage_id_t stage_id);
#endif /* BCMFP_COMPRESSION_INTERNAL_H */
