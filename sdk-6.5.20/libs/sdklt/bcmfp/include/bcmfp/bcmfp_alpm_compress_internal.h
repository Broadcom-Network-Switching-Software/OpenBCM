/*! \file bcmfp_alpm_compress_internal.h
 *
 * Defines enums, structures and APIs to manage
 * ALPM entries to be programmed from BCMFP.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_ALPM_COMPRESS_INTERNAL_H
#define BCMFP_ALPM_COMPRESS_INTERNAL_H

/*! FP compress ALPM LT entry fields data. */
typedef struct bcmfp_alpm_entry_info_s {
    /*! Prefix based key data */
    uint32_t *key;

    /*! Prefix based mask data */
    uint32_t *mask;

    /*! Compress ID. */
    uint32_t *cid;

    /*! Strength based profile index. */
    uint32_t sbr_profile_index;

    /*! Data type of the entry. */
    uint32_t data_type;
} bcmfp_alpm_entry_info_t;

/*!
 * \brief Insert incoming compress key, mask into trie that returns
 *  the CID and CID_MASK. Build the ALPM LT entry words, using
 *  incoming compress key, mask and CID generated, to insert
 *  into HW.
 *
 * \param [in]  unit          Logical device id
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  req_ltid      ALPM LT ID
 * \param [in]  stage_id      BCMFP stage ID
 * \param [in]  key           Key value
 * \param [in]  prefix        Prefix of the key
 * \param [in]  trie          Trie in which the key is to be inserted
 * \param [in]  user_data     Data to be passed back to component using
 *                            compression algorithm if there are changes
 *                            in CID/CID_MASK Of existing ALPM keys.
 * \param [in]  list_compress List compress the compress keys.
 * \param [out] cid           Compression ID assigned
 * \param [out] cid_mask      Compression ID mask assigned
 * \param [out] list_compressed TRUE = List compressed the compress keys.
 *                              FALSE = Not able to list compress even
 *                              though list-compress is set to TRUE.
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters
 */
extern int
bcmfp_alpm_compress_insert_internal(int unit,
                                    const bcmltd_op_arg_t *op_arg,
                                    bcmlrd_sid_t req_ltid,
                                    bcmfp_stage_id_t stage_id,
                                    uint32_t *key,
                                    uint32_t *mask,
                                    uint32_t prefix,
                                    bcmfp_shr_trie_md_t *trie,
                                    void *user_data,
                                    bool list_compress,
                                    bcmfp_cid_t *cid,
                                    bcmfp_cid_t *cid_mask,
                                    bool *list_compressed);
/*!
 * \brief Delete incoming compress key, mask from trie that returns
 *  the CID and CID_MASK. Build the ALPM LT entry words, using
 *  incoming compress key, mask, to delete from HW.
 *
 * \param [in]  unit          Logical device id
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  req_ltid      ALPM LT ID
 * \param [in]  stage_id      BCMFP stage ID
 * \param [in]  key           Key value
 * \param [in]  prefix        Prefix of the key
 * \param [in]  trie          Trie in which the key is to be inserted
 * \param [in]  user_data     Data to be passed back to component using
 *                            compression algorithm if there are changes
 *                            in CID/CID_MASK Of existing ALPM keys.
 * \param [out] cid           Compression ID assigned
 * \param [out] cid_mask      Compression ID mask assigned
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters
 */
extern int
bcmfp_alpm_compress_delete_internal(int unit,
                                    const bcmltd_op_arg_t *op_arg,
                                    bcmlrd_sid_t req_ltid,
                                    bcmfp_stage_id_t stage_id,
                                    uint32_t *key,
                                    uint32_t *mask,
                                    uint32_t prefix,
                                    bcmfp_shr_trie_md_t *trie,
                                    void *user_data,
                                    bcmfp_cid_t *cid,
                                    bcmfp_cid_t *cid_mask);
/*!
 * \brief Update CID of incoming compress key, mask in a giventrie.
 *
 * \param [in]  unit          Logical device id
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  req_ltid      ALPM LT ID
 * \param [in]  stage_id      BCMFP stage ID
 * \param [in]  key           Key value
 * \param [in]  trie          Trie in which the key is updated
 * \param [out] cid           New compression ID assigned
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters
 */
extern int
bcmfp_alpm_compress_update_internal(int unit,
                                    const bcmltd_op_arg_t *op_arg,
                                    bcmlrd_sid_t req_ltid,
                                    bcmfp_stage_id_t stage_id,
                                    uint32_t *key,
                                    uint32_t *mask,
                                    bcmfp_shr_trie_md_t *trie,
                                    bcmfp_cid_t *cid);
/*!
 * \brief Lookup up the incoming compress key, mask in a given trie
 *  and populate cid and cid_mask.
 *
 * \param [in]  unit          Logical device id
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  req_ltid      ALPM LT ID
 * \param [in]  stage_id      BCMFP stage ID
 * \param [in]  key           Key value
 * \param [in]  mask          Mask value
 * \param [in]  prefix        Prefix of the key
 * \param [in]  trie          Trie in which the key is to be inserted
 * \param [in]  trie_only     Do lookup only fro trie(skip PTM)
 * \param [out] cid           Compression ID assigned
 * \param [out] cid_mask      Compression ID mask assigned
 * \param [out] data_type     Compression entry data type
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters
 */
extern int
bcmfp_alpm_compress_lookup_internal(int unit,
                                    const bcmltd_op_arg_t *op_arg,
                                    bcmlrd_sid_t req_ltid,
                                    bcmfp_stage_id_t stage_id,
                                    uint32_t *key,
                                    uint32_t *mask,
                                    uint16_t prefix,
                                    bcmfp_shr_trie_md_t *trie,
                                    bool trie_only,
                                    bcmfp_cid_t *cid,
                                    bcmfp_cid_t *cid_mask,
                                    uint8_t *data_type);
#endif /* BCMFP_ALPM_COMPRESS_INTERNAL_H */
