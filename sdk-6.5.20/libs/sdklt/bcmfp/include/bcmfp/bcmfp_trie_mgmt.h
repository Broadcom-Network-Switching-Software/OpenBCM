/*! \file bcmfp_trie_mgmt.h
 *
 * Defines, Structures, and Functions used to manage AACL compression trie and id.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_TRIE_MGMT_H
#define BCMFP_TRIE_MGMT_H

#define BCMFP_COMP_NULL_CHECK(_ptr)             \
    do {                                        \
        if ((_ptr) == NULL) {                   \
            goto exit;                          \
        }                                       \
    } while (0)

#define BCMFP_COMP_ALLOC(_ptr, _sz, _str)                   \
            do                                              \
            {                                               \
                if ((_ptr) == NULL) {                       \
                    (_ptr) = sal_alloc(_sz, _str);          \
                } else {                                    \
                    goto exit;                              \
                }                                           \
                BCMFP_COMP_NULL_CHECK(_ptr);                \
                sal_memset(_ptr, 0, _sz);                   \
            } while(0)

#define BCMFP_COMP_FREE(_ptr)                   \
    do {                                        \
        if ((_ptr) != NULL) {                   \
            sal_free((void *)(_ptr));           \
            (_ptr) = NULL;                      \
        }                                       \
    } while (0)

/*! Maximum compression ID size. */
#define BCMFP_MAX_CID_SIZE 46

/*! Maximum key length. */
#define BCMFP_MAX_KEY_LENGTH 144

typedef struct bcmfp_cid_s {
    SHR_BITDCLNAME(w, BCMFP_MAX_CID_SIZE);
} bcmfp_cid_t;

typedef struct bcmfp_key_s {
    SHR_BITDCLNAME(w, BCMFP_MAX_KEY_LENGTH);
} bcmfp_key_t;

/*! Each compression key in the trie will have one node of this type. */
typedef struct bcmfp_compress_key_s {
    /*!
     * Actual key data.
     * Prefix number of MS Bits in this array are valid.
     */
    bcmfp_key_t key;

    /*!
     * Key mask value
     */
    bcmfp_key_t key_mask;

    /*!
     * Prefix length(in bits) of the key.
     * Always less than BCMFP_MAX_KEY_LENGTH
     */
    uint8_t prefix;

    /*! Compression Id to be used in FP rules. */
    bcmfp_cid_t cid;

    /*! Compression Id mask to be used in FP rules. */
    bcmfp_cid_t cid_mask;

    /*!
     * Nodes in next level prefixes. Valid next level prefixes are the
     * ones greater than the current prefix.
     */
    struct bcmfp_compress_key_s **child;

    /*!
     * Number of child nodes for which this node is a parent
     * Includes nodes under next level prefixes and their siblings
     */
    uint32_t num_child;

    /*!
     * Child index of this node in its parent (starting 0)
     * Will be used to assign compression Id to this node
     */
    uint32_t child_index;

    /*! Number of times the key is inserted. */
    uint32_t ref_count;

    uint32_t flags;

    /*!
     * List of nodes in same prefix level as this node with same parent node.
     * For list compressed node this points to next node in list
     */
    struct bcmfp_compress_key_s *sibling;

    /*! HA block ID to which this compression key belongs to. */
    uint8_t blk_id;

    /*!
     * Offset in the HA block ID to which this compression
     * key belongs to.
     */
    uint32_t blk_offset;

    /*!
     * Offset of next free compression key in the HA block ID
     * to which this compression key belongs to.
     */
    uint32_t next_blk_offset;
} bcmfp_compress_key_t;

/*!
 * BCMFP_TRIE_NODE_LIST_COMPRESSED flag to be used by FP to indicate
 * that the key follows various FP rules and can be be list compressed.
 *
 * Algo will search the trie and list compressed nodes and try to list
 * compress the incoming key.
 *
 * In case it is not possible to list compress the incoming key due to a match
 * with an existing key, algo will width compress the incoming key as well as
 * the matched key.
 * BCMFP_TRIE_NODE_LIST_COMPRESSED will be removed and FP should check the same
 */
#define BCMFP_TRIE_NODE_LIST_COMPRESSED 0x1

/*!
 * BCMFP_TRIE_NODE_WIDTH_COMPRESSED is an internal flag used to indicate
 * that a key is being removed from list compression nodes and being added in trie
 * during an insert/delete operation
 */
#define BCMFP_TRIE_NODE_WIDTH_COMPRESSED 0x2

/*! Basic information about compression key in the trie. */
typedef struct bcmfp_basic_info_key_s {
    /*!
     * Actual key data.
     * Prefix number of MS bits in this array are valid.
     */
    bcmfp_key_t key;

    /* Key Mask (Trie algo uses prefix instead of mask) */
    bcmfp_key_t key_mask;

    /*!
     * Prefix length(in bits) of the key.
     * Always less than BCMFP_MAX_KEY_LENGTH
     */
    uint8_t prefix;

    /*! Compression Id to be used in FP rules. */
    bcmfp_cid_t cid;

    /*! Compression Id mask to be used in FP rules. */
    bcmfp_cid_t cid_mask;

    /*! Number of times the key is inserted. */
    uint32_t ref_count;

    uint32_t flags;

} bcmfp_basic_info_key_t;


typedef struct bcmfp_cid_update_list_s {
    /*!
     * Actual key data.
     * Prefix number of MS Bits in this array are valid.
     */
    bcmfp_key_t key;

    /*!
     * Prefix length(in bits) of the key.
     * Always less than BCMFP_MAX_KEY_LENGTH
     */
    uint8_t prefix;

    /*! Compression Id being used currently in FP rules. */
    bcmfp_cid_t old_cid;

    /*! Compression Id mask being used currently in FP rules. */
    bcmfp_cid_t old_cid_mask;

    /*! New Compression Id to be used in FP rules. */
    bcmfp_cid_t new_cid;

    /*! New Compression Id mask to be used in FP rules. */
    bcmfp_cid_t new_cid_mask;

    /*!
     * If old_cid/old_cid_mask equal to new_cid/new_cid_mask
     * it indicates that a list compressed node moved to trie
     * BCMFP_TRIE_NODE_WIDTH_COMPRESSED will be set
     */

    uint32_t flags;

    /*! Next entry in the list */
    struct bcmfp_cid_update_list_s *next;
} bcmfp_cid_update_list_t;

/*!
 * Callback function to be invoked when there is a change
 * in cid/cid_mask of existing keys.
 */
typedef int (*bcmfp_fn_cid_update_t)(int unit,
                       bcmfp_cid_update_list_t *list,
                       void *user_data);

/*! Function to allocate memory for compression keys. */
typedef int
(*bcmfp_fn_compress_key_acquire_t)(int unit,
                                   uint8_t stage_id,
                                   uint8_t compress_type,
                                   bcmfp_compress_key_t **ckey);


/*! Function to free the memory allocated for compression keys. */
typedef int
(*bcmfp_fn_compress_key_release_t)(int unit,
                                   uint8_t stage_id,
                                   uint8_t compress_type,
                                   bcmfp_compress_key_t *ckey);

/*! Compression metadata for each trie. */
typedef struct bcmfp_shr_trie_md_s {

    /*! Root node for each trie.
     *  Root's sibling contains list of list compressed nodes
     */
    bcmfp_compress_key_t *root;

    /*!
     * Global maximum prefix mask length(in bits) for a key in this trie.
     * Always less than BCMFP_MAX_KEY_LENGTH
     */
    uint8_t  max_prefix_mask;

    /*!
     * Global maximum cid mask length(in bits) for a cid in this trie.
     * Always less than BCMFP_MAX_CID_SIZE
     */
    uint8_t  max_cid_mask;

    /*!
     * Callback function to be invoked when there is a change
     * in cid/cid_mask of the existing keys in this trie.
     */
    bcmfp_fn_cid_update_t cid_update_cb;

    /*! Trie type. */
    uint8_t  trie_type;

    /*! Stage ID */
    uint8_t stage_id;

    /*!
     * SDKLT module using the trie library will set these
     * functions if they want to allocate the memory for
     * compression key nodes. If this is set to NULL then
     * trie libary will use sal_alloc to allocate memory
     * for compression key node.
     */
    bcmfp_fn_compress_key_acquire_t ckey_acquire;

    /*!
     * SDKLT module using the trie library will set these
     * functions if they want to allocate the memory for
     * compression key nodes. If this is set to NULL then
     * trie libary will use sal_free to free the memory
     * created for compression key node.
     */
    bcmfp_fn_compress_key_release_t ckey_release;
} bcmfp_shr_trie_md_t;

/*!
 * \brief This function is to be called after warmboot so that nodes/keys stored
 *        in HA memory can be linked together appropriately to re-create the trie
 *        This function will NOT reassign cid/cid_mask
 *
 * \param [in] unit         Logical device id
 * \param [in] in_node      Compress node to be inserted in trie
 * \param [in] trie         Trie on which operation is to be performed
 *
 * \retval SHR_E_NONE       Success
 * \retval SHR_E_INTERNAL
 */
extern int
bcmfp_compress_key_recover(int unit,
                           bcmfp_compress_key_t *in_node,
                           bcmfp_shr_trie_md_t *trie);

/*!
 * \brief Insert the new compression key info present in in_node to the trie.
 *
 * \param [in] unit         Logical device id
 * \param [in] in_node_info New compress node to be inserted
 * \param [in] trie         Trie on which operation is to be performed
 * \param [in] user_data     Data passed by caller application which will
 *                           be passed back to it if there are changes
 *                           in cid/cid_mask of existing keys in trie.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_RESOURCE Out of cid bits to assign
 */
extern int
bcmfp_compress_key_insert(int unit,
                          bcmfp_basic_info_key_t *in_node_info,
                          bcmfp_shr_trie_md_t *trie,
                          void *user_data);

/*!
 * \brief Lookup compression key in the trie.
 *
 * \param [in] unit         Logical device id
 * \param [in] in_node_info key to be searched
 * \param [in] trie         Trie on which operation is to be performed
 * \retval SHR_E_NONE       If key is present
 * \retval SHR_E_NOT_FOUND If key is absent in the trie
 */
extern int
bcmfp_compress_key_lookup(int unit,
                          bcmfp_basic_info_key_t *in_node_info,
                          bcmfp_shr_trie_md_t *trie);
/*!
 * \brief Traverse compression key in the trie.
 *
 * \param [in]  unit          Logical device id
 * \param [in]  trie          Trie on which operation is to be performed
 * \param [out] out           Trie traversal output
 * \param [out] num_nodes     Number of nodes in trie including list compressed nodes
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_EMPTY  Trie is empty
 * \retval SHR_E_INTERNAL  Trie is not sane
 */
extern int
bcmfp_compress_key_traverse(int unit,
                            bcmfp_shr_trie_md_t *trie,
                            bcmfp_basic_info_key_t *out,
                            uint32_t *num_nodes);

/*!
 * \brief Delete compression key info present in in_node from the trie.
 *
 * \param [in] unit    Logical device id
 * \param [in] in_node Compressed node to be deleted
 * \param [in] trie    Trie on which operation is to be performed
 * \param [in] user_data     Data passed by caller application which will
 *                           be passed back to it if there are changes
 *                           in cid/cid_mask of existing keys in trie.
 *
 * \retval SHR_E_NONE      Success
 * \retval SHR_E_NOT_FOUND If key is absent in the trie
 */
extern int
bcmfp_compress_key_delete(int unit,
                          bcmfp_basic_info_key_t *in_node,
                          bcmfp_shr_trie_md_t *trie,
                          void *user_data);

/*!
 * \brief Create and initialize metadata of trie.
 *
 * \param [in]  unit            Logical device id
 * \param [in]  max_prefix_mask Max prefix length(in bits) for a key in this trie
 * \param [in]  max_cid_mask    Max cid length(in bits) for a key in this trie
 * \param [in]  compress_type   Type of the trie to be created.
 * \param [in]  trie            pointer to created trie
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  If memory allocation failure is detected
 */

extern int
bcmfp_compress_trie_create(int unit,
                           uint8_t max_prefix_mask,
                           uint8_t max_cid_mask,
                           uint8_t compress_type,
                           bcmfp_fn_cid_update_t cid_update_cb,
                           bcmfp_shr_trie_md_t **trie_new);

/*!
 * \brief Free the trie metadata sructure.
 *
 * \param [in]  unit          Logical device id
 * \param [in]  trie          Pointer to created trie
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  If memory allocation failure is detected
 */

extern int
bcmfp_compress_trie_deinit(int unit, bcmfp_shr_trie_md_t *trie);


/*!
 * \brief Update CID/CID_MASK based how trie looks like after
 *        in_node is inserted/deleted in parent_node
 *
 * \param [in] trie          Trie on which operation is to be performed
 * \param [in] parent_node   parent_node of in_node
 * \param [in] user_data     Data passed by caller application which will
 *                           be passed back to it if there are changes
 *                           in cid/cid_mask of existing keys in trie.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_RESOURCE Out of cid bits to assign
 */

extern int
bcmfp_compress_id_update(int unit,
                         bcmfp_shr_trie_md_t *trie,
                         bcmfp_compress_key_t* parent_node,
                         void *user_data);


#endif /* BCMFP_TRIE_MGMT_H */
