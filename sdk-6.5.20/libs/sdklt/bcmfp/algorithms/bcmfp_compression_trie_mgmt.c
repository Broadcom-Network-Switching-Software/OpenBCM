/*! \file bcmfp_compression_trie_mgmt.c
 *
 * APIs to operate on compression id trie management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <sal/sal_alloc.h>
#include <shr/shr_bitop.h>
#include <bcmfp/bcmfp_trie_mgmt.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

/*!
 * \brief Compare prefix number of bits of in_node_key and curr_node_key
 *
 * \param [in] key1      Key 1
 * \param [in] key2      Key 2
 * \param [in] key1_pfx  Prefix length for Key 1
 * \param [in] key2_pfx  Prefix length for Key 2
 * \param [in] max_pfx   Max prefix length for the trie
 *
 * \retval 1 If keys match on prefix bits
 * \retval 0 If keys don't match on prefix bits
 */

STATIC int
bcmfp_compare_bits(SHR_BITDCL *key1,
                   SHR_BITDCL *key2,
                   uint32_t key1_pfx,
                   uint32_t key2_pfx,
                   uint8_t max_pfx)
{
    if (key2_pfx < key1_pfx) {
        /* Always match with lower prefix length */
        key1_pfx = key2_pfx;
    }

    return (SHR_BITEQ_RANGE(key1, key2,
                            (max_pfx - key1_pfx),
                            key1_pfx));
}

/*
 * Insert current node under parent node
 * Assumes curr_node->prefix matches parent_node->prefix
 */
STATIC void
bcmfp_compress_key_insert_child(bcmfp_shr_trie_md_t *trie,
                                bcmfp_compress_key_t *curr_node,
                                bcmfp_compress_key_t *parent_node)
{
    bcmfp_compress_key_t *temp_node;
    int index;

    index = trie->max_prefix_mask - curr_node->prefix;
    if (parent_node->child[index]) {
        /*
         * If parent has existing child at curr_node->prefix
         * then curr_node will become a sibling to that child
         */
        temp_node = parent_node->child[index];
        while (temp_node->sibling) {
            temp_node = temp_node->sibling;
        }
        temp_node->sibling = curr_node;
    } else {
        parent_node->child[index] = curr_node;
    }

    parent_node->num_child++;
}

/*
 * Delete current node under parent node
 * Assumes curr_node->prefix matches parent_node->prefix
 */
STATIC void
bcmfp_compress_key_delete_child(bcmfp_shr_trie_md_t *trie,
                                bcmfp_compress_key_t *curr_node,
                                bcmfp_compress_key_t *curr_node_prev_sib,
                                bcmfp_compress_key_t *parent_node)
{
    int index;

    index = trie->max_prefix_mask - curr_node->prefix;

    if (parent_node->child[index] == curr_node) {
        /* First child is curr_node */
        parent_node->child[index] = curr_node->sibling;
    } else if ((curr_node->sibling == NULL) && (curr_node_prev_sib != NULL)) {
        /* Last child is curr_node */
        curr_node_prev_sib->sibling = NULL;
    } else if (curr_node_prev_sib != NULL) {
        /* Mid child is deleted */
        curr_node_prev_sib->sibling = curr_node->sibling;
    }
    curr_node->sibling = NULL;
    parent_node->num_child--;
}

STATIC int
bcmfp_compress_key_node_alloc(int unit, bcmfp_basic_info_key_t *node_info,
                              bcmfp_compress_key_t **node,
                              bcmfp_shr_trie_md_t *trie)
{
    int rv = SHR_E_MEMORY;

    if (trie->ckey_acquire) {
        rv = (trie->ckey_acquire(unit, trie->stage_id, trie->trie_type, node));
        if (SHR_FAILURE(rv)) {
            return (rv);
        }
    } else {
        /* Allocate memory for new node */
        BCMFP_COMP_ALLOC(*node, sizeof(bcmfp_compress_key_t),
                         "bcmfpCompressionNode");
    }

    (*node)->prefix = node_info->prefix;
    if (node_info->flags & BCMFP_TRIE_NODE_WIDTH_COMPRESSED) {
        (*node)->ref_count = node_info->ref_count;
    } else {
        (*node)->ref_count++;
    }
    if (node_info->flags) {
        (*node)->flags = node_info->flags;
        sal_memcpy(&(*node)->cid, &node_info->cid, sizeof(bcmfp_cid_t));
        sal_memcpy(&(*node)->cid_mask, &node_info->cid_mask, sizeof(bcmfp_cid_t));
    }

    if (node_info->prefix < trie->max_prefix_mask) {
        BCMFP_COMP_ALLOC((*node)->child, sizeof(bcmfp_compress_key_t *) *
                         (trie->max_prefix_mask - node_info->prefix),
                         "bcmfpCompressionNodeChildren");
    }

    sal_memcpy(&(*node)->key, &node_info->key, sizeof(bcmfp_key_t));

    rv = SHR_E_NONE;
exit:
    return (rv);
}

STATIC void
bcmfp_compress_key_node_free(int unit, bcmfp_compress_key_t *node, bcmfp_shr_trie_md_t *trie)
{
    /* Free node */
    if (node) {
        if (node->child) {
            BCMFP_COMP_FREE(node->child);
        }
        if (trie->ckey_release && node != trie->root) {
            (void)trie->ckey_release(unit, trie->stage_id, trie->trie_type, node);
        } else {
            BCMFP_COMP_FREE(node);
        }
    }
}

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
int
bcmfp_compress_key_recover(int unit,
                           bcmfp_compress_key_t *in_node,
                           bcmfp_shr_trie_md_t *trie)
{
    int rv = SHR_E_PARAM, in_node_linked = 0, prefix, index;
    bcmfp_compress_key_t *parent_node;
    bcmfp_compress_key_t *curr_node, *curr_node_prev_sib;

    BCMFP_COMP_NULL_CHECK(trie);
    BCMFP_COMP_NULL_CHECK(trie->root);
    BCMFP_COMP_NULL_CHECK(in_node);

    if (in_node->prefix > trie->max_prefix_mask) {
        return (SHR_E_PARAM);
    }

    /* Alloc child pointer */
    in_node->child = NULL;
    in_node->sibling = NULL;
    rv = SHR_E_MEMORY;
    if (in_node->prefix < trie->max_prefix_mask) {
        BCMFP_COMP_ALLOC(in_node->child, sizeof(bcmfp_compress_key_t *) *
                         (trie->max_prefix_mask - in_node->prefix),
                         "bcmfpCompressionNodeChildren");
    }

    /* If node is list compressed */
    if (in_node->flags & BCMFP_TRIE_NODE_LIST_COMPRESSED) {
        curr_node = trie->root;
        while (curr_node->sibling) {
            curr_node = curr_node->sibling;
        }
        curr_node->sibling = in_node;
        return (SHR_E_NONE);
    }

    /* Start with root node */
    parent_node = trie->root;
    curr_node_prev_sib = NULL;

    /* Loop through all the prefix levels. Prefix 0 reserved for root */
    for (prefix = 1; prefix <= trie->max_prefix_mask; prefix++) {
        index = trie->max_prefix_mask - prefix;

        curr_node = parent_node->child[index];
        /* curr_node used as an iterator to find a match */

        /* Loop through all nodes of the same prefix until a match is found */
        while (curr_node != NULL) {

            rv = bcmfp_compare_bits(in_node->key.w,
                                    curr_node->key.w,
                                    in_node->prefix,
                                    curr_node->prefix,
                                    trie->max_prefix_mask);
            if (rv) {
                if (in_node->prefix < curr_node->prefix) {
                /* Current node sub-trie need to be moved under incoming node */

                    /* Unlink the current node from the parent node */
                    bcmfp_compress_key_delete_child(trie, curr_node,
                                                    curr_node_prev_sib,
                                                    parent_node);

                    if (!in_node_linked) {
                        /* Link the incoming node to the parent node as a child */
                        bcmfp_compress_key_insert_child(trie, in_node,
                                                        parent_node);
                        in_node_linked = 1;
                    }

                    /* Link the current node to incoming node as a child */
                    bcmfp_compress_key_insert_child(trie, curr_node, in_node);

                    /* Loop through remaining nodes of curr_node->prefix */
                    if (curr_node_prev_sib == NULL) {
                        curr_node = parent_node->child[index];
                    } else {
                        curr_node = curr_node_prev_sib->sibling;
                    }

                } else if (in_node->prefix > curr_node->prefix) {
                /* Incoming node need to be moved under sub-trie of current node */

                    /* Find correct place under sub-trie of current node */
                    parent_node = curr_node;
                    curr_node = NULL;
                } else {
                /* Incoming key has same prefix as current nodes prefix
                 * Duplicate nodes cannot be present in trie
                 */
                    return (SHR_E_INTERNAL);
                }
            } else {
                /* No match with current node */
                curr_node_prev_sib = curr_node;
                curr_node = curr_node->sibling;
            }
        }
        curr_node_prev_sib = NULL;
    }

    if (!in_node_linked) {
        /* Link incoming node to parent node */
        bcmfp_compress_key_insert_child(trie, in_node, parent_node);
    }

    rv = SHR_E_NONE;
exit:
    return (rv);
}

/*!
 * \brief Insert the new compression key info present in in_node to the trie.
 *
 * \param [in] unit         Logical device id
 * \param [in] in_node_info New compress node info to be inserted
 * \param [in] trie         Trie on which operation is to be performed
 * \param [in] user_data     Data passed by caller application which will
 *                           be passed back to it if there are changes
 *                           in cid/cid_mask of existing keys in trie.
 *
 * \retval SHR_E_NONE       Success
 * \retval SHR_E_RESOURCE   Out of cid bits to assign
 */
int
bcmfp_compress_key_insert(int unit,
                          bcmfp_basic_info_key_t *in_node_info,
                          bcmfp_shr_trie_md_t *trie,
                          void *user_data)
{
    int rv = SHR_E_PARAM, in_node_linked = 0, prefix, index;
    bcmfp_compress_key_t *parent_node, *in_node = NULL;
    bcmfp_compress_key_t *curr_node, *curr_node_prev_sib;
    bcmfp_basic_info_key_t node_info;

    BCMFP_COMP_NULL_CHECK(trie);
    BCMFP_COMP_NULL_CHECK(trie->root);
    BCMFP_COMP_NULL_CHECK(in_node_info);

    if (in_node_info->prefix > trie->max_prefix_mask) {
        return (SHR_E_PARAM);
    }

    if (in_node_info->prefix == 0) {
        /* Copy root cid/cid_mask to in_node_info */
        trie->root->ref_count++;
        in_node_info->ref_count = trie->root->ref_count;
        in_node_info->flags = trie->root->flags;
        sal_memcpy(&in_node_info->cid, &trie->root->cid, sizeof(bcmfp_cid_t));
        sal_memcpy(&in_node_info->cid_mask, &trie->root->cid_mask,
                   sizeof(bcmfp_cid_t));
        return (SHR_E_NONE);
    }

    if (in_node_info->flags & BCMFP_TRIE_NODE_LIST_COMPRESSED) {
        if (SHR_BITNULL_RANGE(in_node_info->cid.w, 0, trie->max_cid_mask) ||
            SHR_BITNULL_RANGE(in_node_info->cid_mask.w, 0, trie->max_cid_mask)) {
            /* cid/cid_mask not valid for node to be list compressed */
            return (SHR_E_PARAM);
        }
    }

    if (!(in_node_info->flags & BCMFP_TRIE_NODE_WIDTH_COMPRESSED)) {
        /* First search the list compressed nodes list */
        curr_node_prev_sib = trie->root;
        /* List compressed node list present in root's sibling */
        curr_node = trie->root->sibling;
        while (curr_node) {
            rv = bcmfp_compare_bits(in_node_info->key.w,
                                    curr_node->key.w,
                                    in_node_info->prefix,
                                    curr_node->prefix,
                                    trie->max_prefix_mask);
            if (rv) {
                if (in_node_info->prefix == curr_node->prefix) {
                    if (in_node_info->flags & BCMFP_TRIE_NODE_LIST_COMPRESSED) {
                        /* Same node */
                        curr_node->ref_count++;
                        in_node_info->ref_count = curr_node->ref_count;
                        return (SHR_E_NONE);
                    }
                }

                sal_memset(&node_info, 0, sizeof(bcmfp_basic_info_key_t));
                sal_memcpy(node_info.key.w, &curr_node->key, sizeof(bcmfp_key_t));
                sal_memcpy(node_info.cid.w, &curr_node->cid, sizeof(bcmfp_cid_t));
                sal_memcpy(node_info.cid_mask.w, &curr_node->cid_mask,
                           sizeof(bcmfp_cid_t));
                node_info.ref_count = curr_node->ref_count;
                node_info.prefix = curr_node->prefix;
                node_info.flags = BCMFP_TRIE_NODE_WIDTH_COMPRESSED;

                /* Unlink current node from list compressed nodes list */
                curr_node_prev_sib->sibling = curr_node->sibling;
                /* Free current node */
                bcmfp_compress_key_node_free(unit, curr_node, trie);
                curr_node = curr_node_prev_sib;

                /* Add current node in trie */
                rv = bcmfp_compress_key_insert(unit, &node_info, trie, user_data);

                if (SHR_FAILURE(rv)) {
                    if (rv == SHR_E_RESOURCE) {
                        /* Add the node back in list compression list */
                        node_info.flags = BCMFP_TRIE_NODE_LIST_COMPRESSED;
                        (void)(bcmfp_compress_key_insert(unit, &node_info,
                                                         trie, user_data));
                    }
                    return (rv);
                }

                if (!in_node_linked) {
                    /* Add incoming node in trie */
                    in_node_info->flags = BCMFP_TRIE_NODE_WIDTH_COMPRESSED;
                    in_node_info->ref_count = 1;
                    sal_memset(in_node_info->cid.w, 0, sizeof(bcmfp_cid_t));
                    sal_memset(in_node_info->cid_mask.w, 0, sizeof(bcmfp_cid_t));
                    rv = (bcmfp_compress_key_insert(unit, in_node_info,
                                                    trie, user_data));
                    if (SHR_FAILURE(rv)) {
                        return (rv);
                    }
                    in_node_linked = 1;
                }

            }
            curr_node_prev_sib = curr_node;
            curr_node = curr_node->sibling;
        }
    }
    if (in_node_linked) {
        return (SHR_E_NONE);
    }

    rv = (bcmfp_compress_key_node_alloc(unit, in_node_info, &in_node, trie));
    if (SHR_FAILURE(rv)) {
        return (rv);
    }

    /* Start with root node */
    parent_node = trie->root;
    curr_node_prev_sib = NULL;

    /* Loop through all the prefix levels. Prefix 0 reserved for root */
    for (prefix = 1; prefix <= trie->max_prefix_mask; prefix++) {
        index = trie->max_prefix_mask - prefix;

        curr_node = parent_node->child[index];
        /* curr_node used as an iterator to find a match */

        /* Loop through all nodes of the same prefix until a match is found */
        while (curr_node != NULL) {

            rv = bcmfp_compare_bits(in_node_info->key.w,
                                    curr_node->key.w,
                                    in_node_info->prefix,
                                    curr_node->prefix,
                                    trie->max_prefix_mask);
            if (rv) {
                if (in_node_info->flags & BCMFP_TRIE_NODE_LIST_COMPRESSED) {
                    /* Can't be list compressed */
                    in_node->flags &= ~BCMFP_TRIE_NODE_LIST_COMPRESSED;
                    in_node_info->flags &= ~BCMFP_TRIE_NODE_LIST_COMPRESSED;
                    sal_memset(in_node->cid.w, 0, sizeof(bcmfp_cid_t));
                    sal_memset(in_node_info->cid.w, 0, sizeof(bcmfp_cid_t));
                }

                if (in_node_info->prefix < curr_node->prefix) {
                /* Current node sub-trie need to be moved under incoming node */

                    /* Unlink the current node from the parent node */
                    bcmfp_compress_key_delete_child(trie, curr_node,
                                                    curr_node_prev_sib,
                                                    parent_node);

                    if (!in_node_linked) {
                        /* Link the incoming node to the parent node as a child */
                        bcmfp_compress_key_insert_child(trie, in_node,
                                                        parent_node);
                        in_node_linked = 1;
                    }

                    /* Link the current node to incoming node as a child */
                    bcmfp_compress_key_insert_child(trie, curr_node, in_node);

                    /* Loop through remaining nodes of curr_node->prefix */
                    if (curr_node_prev_sib == NULL) {
                        curr_node = parent_node->child[index];
                    } else {
                        curr_node = curr_node_prev_sib->sibling;
                    }

                } else if (in_node_info->prefix > curr_node->prefix) {
                /* Incoming node need to be moved under sub-trie of current node */

                    /* Find correct place under sub-trie of current node */
                    parent_node = curr_node;
                    curr_node = NULL;
                } else {
                /* Incoming key has same prefix as current nodes prefix */

                    /* Key is in use by multiple rules */
                    curr_node->ref_count++;
                    in_node_info->ref_count = curr_node->ref_count;
                    in_node_info->flags = curr_node->flags;
                    /* Copy existing cid/cid_mask to in_node_info */
                    sal_memcpy(&in_node_info->cid, &curr_node->cid,
                               sizeof(bcmfp_cid_t));
                    sal_memcpy(&in_node_info->cid_mask, &curr_node->cid_mask,
                               sizeof(bcmfp_cid_t));

                    bcmfp_compress_key_node_free(unit, in_node, trie);
                    in_node = NULL;
                    return (SHR_E_NONE);
                }
            } else {
                /* No match with current node */
                curr_node_prev_sib = curr_node;
                curr_node = curr_node->sibling;
            }
        }
        curr_node_prev_sib = NULL;
    }

    if (!in_node_linked) {
        if (in_node_info->flags & BCMFP_TRIE_NODE_LIST_COMPRESSED) {
            /* No match for incoming node in the list compressed nodes or in trie */
            curr_node = trie->root;
            while (curr_node->sibling) {
                curr_node = curr_node->sibling;
            }
            curr_node->sibling = in_node;
            in_node_info->ref_count = in_node->ref_count;
            return (SHR_E_NONE);
        } else {
            /* Link incoming node to parent node */
            bcmfp_compress_key_insert_child(trie, in_node, parent_node);
        }
    }

    /* Update CID/CID Mask */
    rv = bcmfp_compress_id_update(unit, trie, parent_node, user_data);

    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_RESOURCE) {
            /* Delete the newly inserted node if we ran out of cid bits */
            (void)(bcmfp_compress_key_delete(unit, in_node_info, trie, user_data));
        }
        return (rv);
    }

    /* Copy updated cid/cid_mask to in_node_info */
    in_node_info->ref_count = in_node->ref_count;
    sal_memcpy(&in_node_info->cid, &in_node->cid, sizeof(bcmfp_cid_t));
    sal_memcpy(&in_node_info->cid_mask, &in_node->cid_mask,
               sizeof(bcmfp_cid_t));

    rv = SHR_E_NONE;
exit:
    return (rv);
}

/*!
 * \brief Lookup compression key in the trie.
 *
 * \param [in] unit         Logical device id
 * \param [in] in_node_info key to be searched
 * \param [in] trie         Trie on which operation is to be performed
 * \retval SHR_E_NONE       If key is present
 * \retval SHR_E_NOT_FOUND If key is absent in the trie
 */
int
bcmfp_compress_key_lookup(int unit,
                          bcmfp_basic_info_key_t *in_node_info,
                          bcmfp_shr_trie_md_t *trie)
{
    bcmfp_compress_key_t *parent_node, *curr_node;
    int prefix, rv = SHR_E_PARAM, found = 0, index;

    BCMFP_COMP_NULL_CHECK(trie);
    BCMFP_COMP_NULL_CHECK(trie->root);
    BCMFP_COMP_NULL_CHECK(in_node_info);

    if (in_node_info->prefix > trie->max_prefix_mask) {
        return (SHR_E_PARAM);
    }

    /* Start with root node */
    parent_node = trie->root;

    /* Fetching the root node. */
    if (in_node_info->prefix == 0) {
        /* Copy cid/cid_mask to in_node_info */
        sal_memcpy(&in_node_info->cid, &parent_node->cid, sizeof(bcmfp_cid_t));
        sal_memcpy(&in_node_info->cid_mask, &parent_node->cid_mask,
                   sizeof(bcmfp_cid_t));
        return (SHR_E_NONE);
    }

    /* First search the list compressed nodes list */
    curr_node = trie->root->sibling;
    while (curr_node) {
        rv = bcmfp_compare_bits(in_node_info->key.w,
                                curr_node->key.w,
                                in_node_info->prefix,
                                curr_node->prefix,
                                trie->max_prefix_mask);
        if (rv) {
            if (in_node_info->prefix == curr_node->prefix) {
                found = 1;
                in_node_info->flags = BCMFP_TRIE_NODE_LIST_COMPRESSED;
                break;
            }
        }
        curr_node = curr_node->sibling;
    }

    /* Loop through all the prefix levels */
    for (prefix = 1; prefix <= trie->max_prefix_mask && found == 0; prefix++) {
        index = trie->max_prefix_mask - prefix;

        curr_node = parent_node->child[index];

        /* Loop through all nodes of the same prefix until a match is found */
        while (curr_node != NULL) {
            rv = bcmfp_compare_bits(in_node_info->key.w,
                                    curr_node->key.w,
                                    in_node_info->prefix,
                                    curr_node->prefix,
                                    trie->max_prefix_mask);
            if (rv) {
                if (in_node_info->prefix < curr_node->prefix) {
                    /* Node not found in trie */
                    return (SHR_E_NOT_FOUND);

                } else if (in_node_info->prefix > curr_node->prefix) {
                    /* Find correct place under sub-trie of current node */
                    parent_node = curr_node;
                    curr_node = NULL;
                } else {
                    /* Incoming key has same prefix as current nodes prefix */
                    found = 1;
                    in_node_info->flags = 0;
                    break;
                }
            } else {
                /* No match with curr_node */
                curr_node = curr_node->sibling;
            }
        }
    }

    if (found) {
        /* Copy cid/cid_mask to in_node_info */
        sal_memcpy(&in_node_info->cid, &curr_node->cid, sizeof(bcmfp_cid_t));
        sal_memcpy(&in_node_info->cid_mask, &curr_node->cid_mask,
                   sizeof(bcmfp_cid_t));
        in_node_info->ref_count = curr_node->ref_count;
        rv = SHR_E_NONE;
    } else {
        /* Node not found in trie */
        rv = SHR_E_NOT_FOUND;
    }

exit:
    return (rv);
}

/*!
 * \brief Traverse the trie below.
 *
 * \param [in]  unit          Logical device id
 * \param [in]  index         Index for the output nodes
 * \param [in]  in_node       Incoming node whose trie is to be traversed
 * \param [out] out           Trie traversal output
 *
 * \retval SHR_E_NONE      Success
 * \retval SHR_E_INTERNAL  Trie is not sane
 */
STATIC int
bcmfp_compress_key_traverse_trie_children(int unit,
                                          uint32_t *oindex,
                                          bcmfp_compress_key_t *in_node,
                                          bcmfp_shr_trie_md_t *trie,
                                          bcmfp_basic_info_key_t *out)
{
    int prefix = 0, rv = 0, index, p_cid_mask, cid_mask;
    uint32_t num_child;
    bcmfp_compress_key_t *curr_node;
    bcmfp_compress_key_t *list_cmp_node;

    num_child = 0;
    for (prefix = in_node->prefix + 1;
         prefix <= trie->max_prefix_mask; prefix++) {
        index = trie->max_prefix_mask - prefix;

        curr_node = in_node->child[index];

        /* Get parent_node's CID mask length (in bits) */
        SHR_BITCOUNT_RANGE(in_node->cid_mask.w, p_cid_mask, 0,
                           trie->max_cid_mask);
        while (curr_node) {

            if (prefix != curr_node->prefix) {
                /* Child node is not correctly linked */
                return (SHR_E_INTERNAL);
            }

            if (prefix <= in_node->prefix) {
                /* Child prefix should always be greater than parent prefix */
                return (SHR_E_INTERNAL);
            }

            if (curr_node->sibling &&
                curr_node->sibling->prefix != curr_node->prefix) {
                /* All siblings should have same prefix */
                return (SHR_E_INTERNAL);
            }

            rv = bcmfp_compare_bits(in_node->key.w, curr_node->key.w,
                                    curr_node->prefix, in_node->prefix,
                                    trie->max_prefix_mask);
            if (rv == 0) {
                /* Child should always have matching prefix */
                return (SHR_E_INTERNAL);
            }

            /* Get child node's CID mask length (in bits) */
            SHR_BITCOUNT_RANGE(curr_node->cid_mask.w, cid_mask, 0,
                               trie->max_cid_mask);

            if (cid_mask <= p_cid_mask) {
                /* Child cid_mask should always be greater than parent cid_mask */
                return (SHR_E_INTERNAL);
            }

            rv = bcmfp_compare_bits(in_node->cid_mask.w, curr_node->cid_mask.w,
                                    (uint32_t)cid_mask, (uint32_t)p_cid_mask,
                                    trie->max_cid_mask);

            if (rv == 0) {
                /* Child should always have matching cid_mask */
                return (SHR_E_INTERNAL);
            }

            /* Call in recursion till child present. */
            rv = (bcmfp_compress_key_traverse_trie_children(unit, oindex,
                                                            curr_node, trie, out));
            if (SHR_FAILURE(rv)) {
                return rv;
            }

            /* Iterate all the list compressed nodes */
            list_cmp_node = trie->root->sibling;
            while (list_cmp_node) {
                /* list compressed node should not have a match with a node in trie */
                rv = bcmfp_compare_bits(list_cmp_node->key.w, curr_node->key.w,
                                    curr_node->prefix, list_cmp_node->prefix,
                                    trie->max_prefix_mask);
                if (rv != 0) {
                    return (SHR_E_INTERNAL);
                }
                list_cmp_node = list_cmp_node->sibling;
            }

            if (out != NULL) {
                sal_memcpy(&out[*oindex].key, &curr_node->key,
                           sizeof(bcmfp_key_t));
                sal_memcpy(&out[*oindex].cid, &curr_node->cid,
                           sizeof(bcmfp_cid_t));
                sal_memcpy(&out[*oindex].cid_mask, &curr_node->cid_mask,
                           sizeof(bcmfp_cid_t));
                out[*oindex].prefix = curr_node->prefix;
            }

            /* Increment index value. */
            (*oindex)++;
            num_child++;
            curr_node = curr_node->sibling;
        }
    }

    if (in_node->num_child != num_child) {
        /* Inconsistency in number of children nodes */
        return (SHR_E_INTERNAL);
    }

    return SHR_E_NONE;
}

/*!
 * \brief Traverse the trie from root
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
int
bcmfp_compress_key_traverse(int unit,
                            bcmfp_shr_trie_md_t *trie,
                            bcmfp_basic_info_key_t *out,
                            uint32_t *num_nodes)
{
    uint32_t oindex = 0;
    bcmfp_compress_key_t *list_cmp_node;
    int rv = SHR_E_PARAM;

    BCMFP_COMP_NULL_CHECK(trie);
    BCMFP_COMP_NULL_CHECK(trie->root);

    /* Fill list compressed nodes first */
    list_cmp_node = trie->root->sibling;
    while (list_cmp_node) {
        if (out != NULL) {
            sal_memcpy(&out[oindex].key, &list_cmp_node->key,
                       sizeof(bcmfp_key_t));
            sal_memcpy(&out[oindex].cid, &list_cmp_node->cid,
                       sizeof(bcmfp_cid_t));
            sal_memcpy(&out[oindex].cid_mask, &list_cmp_node->cid_mask,
                       sizeof(bcmfp_cid_t));
            out[oindex].prefix = list_cmp_node->prefix;
        }
        /* Increment index value. */
        oindex++;
        list_cmp_node = list_cmp_node->sibling;
    }

    rv = (bcmfp_compress_key_traverse_trie_children(unit, &oindex, trie->root,
                                                    trie, out));
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    if (oindex == 0) {
        return (SHR_E_EMPTY);
    }
    if (num_nodes != NULL) {
        *num_nodes = oindex;
    }

exit:
    return (rv);
}

/*!
 * \brief Delete compression key info present in in_node_info from the trie.
 *
 * \param [in] unit         Logical device id
 * \param [in] in_node_info Compressed node info to be deleted
 * \param [in] trie         Trie on which operation is to be performed
 * \param [in] user_data     Data passed by caller application which will
 *                           be passed back to it if there are changes
 *                           in cid/cid_mask of existing keys in trie.
 *
 * \retval SHR_E_NONE       Success
 * \retval SHR_E_NOT_FOUND If key is absent in the trie
 */
int
bcmfp_compress_key_delete(int unit,
                          bcmfp_basic_info_key_t *in_node_info,
                          bcmfp_shr_trie_md_t *trie,
                          void *user_data)
{
    bcmfp_compress_key_t *parent_node, *curr_node, *curr_node_prev_sib;
    bcmfp_compress_key_t *lc_node = NULL;
    bcmfp_basic_info_key_t node_info;
    int prefix, rv = SHR_E_PARAM, in_node_linked = 1, index;

    BCMFP_COMP_NULL_CHECK(trie);
    BCMFP_COMP_NULL_CHECK(trie->root);
    BCMFP_COMP_NULL_CHECK(in_node_info);

    if (in_node_info->prefix > trie->max_prefix_mask) {
        return (SHR_E_PARAM);
    }

    /* Root not allowed to be deleted. */
    if (sal_memcmp(&in_node_info->key, &trie->root->key,
                   sizeof(bcmfp_key_t)) == 0) {
        return (SHR_E_PARAM);
    }

    if (in_node_info->prefix == 0) {
        if (trie->root->ref_count > 1) {
            trie->root->ref_count--;
        }
        in_node_info->ref_count = trie->root->ref_count;
        return (SHR_E_NONE);
    }

    if (!(in_node_info->flags & BCMFP_TRIE_NODE_WIDTH_COMPRESSED)) {
        /* First search the list compressed nodes list */
        curr_node_prev_sib = trie->root;
        curr_node = trie->root->sibling;
        while (curr_node) {
            rv = bcmfp_compare_bits(in_node_info->key.w,
                                    curr_node->key.w,
                                    in_node_info->prefix,
                                    curr_node->prefix,
                                    trie->max_prefix_mask);
            if (rv) {
                if (in_node_info->prefix == curr_node->prefix) {
                    in_node_linked = 0;
                    curr_node->ref_count--;
                    in_node_info->ref_count--;
                    if (curr_node->ref_count > 0) {
                        /* Key is in use by multiple rules */
                        return (SHR_E_NONE);
                    }

                    /* Remove current node from list compressed nodes list */
                    curr_node_prev_sib->sibling = curr_node->sibling;
                    /* Free current node */
                    bcmfp_compress_key_node_free(unit, curr_node, trie);
                    curr_node = NULL;
                    return (SHR_E_NONE);
                }
            }
            if (!(in_node_info->flags & BCMFP_TRIE_NODE_LIST_COMPRESSED)) {
                /* Node to be deleted is not list compressed */
                if (SHR_BITEQ_RANGE(in_node_info->cid.w, curr_node->cid.w,
                                    0, trie->max_cid_mask) &&
                    in_node_info->ref_count == 1) {
                    /*
                     * If there is a list compressed node using
                     * the cid of the node to be deleted,
                     * then it needs to be moved to the trie
                     * only if ref_count of the node to be deleted is 1
                     */

                    sal_memcpy(node_info.key.w, &curr_node->key, sizeof(bcmfp_key_t));
                    sal_memcpy(node_info.cid.w, &curr_node->cid, sizeof(bcmfp_cid_t));
                    sal_memcpy(node_info.cid_mask.w, &curr_node->cid_mask, sizeof(bcmfp_cid_t));
                    node_info.ref_count = curr_node->ref_count;
                    node_info.prefix = curr_node->prefix;
                    node_info.flags = BCMFP_TRIE_NODE_WIDTH_COMPRESSED;

                    /* Remove current node from list compressed nodes list */
                    curr_node_prev_sib->sibling = curr_node->sibling;
                    lc_node = curr_node;
                    break;
                }
            }

            curr_node_prev_sib = curr_node;
            curr_node = curr_node->sibling;
        }
    }

    /* Start with root node */
    parent_node = trie->root;
    curr_node_prev_sib = NULL;

    /* Loop through all the prefix levels */
    for (prefix = 1; prefix <= trie->max_prefix_mask; prefix++) {
        index = trie->max_prefix_mask - prefix;

        curr_node = parent_node->child[index];

        /* Loop through all nodes of the same prefix until a match is found */
        while (curr_node != NULL) {
            rv = bcmfp_compare_bits(in_node_info->key.w,
                                    curr_node->key.w,
                                    in_node_info->prefix,
                                    curr_node->prefix,
                                    trie->max_prefix_mask);
            if (rv) {

                if (in_node_info->prefix < curr_node->prefix) {
                    /* Node not found in trie */
                    return (SHR_E_NOT_FOUND);
                } else if (in_node_info->prefix > curr_node->prefix) {
                /* Incoming node is present under sub-trie of current node */

                    /* Find correct place under sub-trie of current node */
                    parent_node = curr_node;
                    curr_node = NULL;
                } else {
                /* Incoming key has same prefix as current nodes prefix */
                    curr_node->ref_count--;
                    in_node_info->ref_count--;
                    if (curr_node->ref_count > 0) {
                        /* Key is in use by multiple rules */
                        return (SHR_E_NONE);
                    }

                    /* Unlink the current node from the parent node */
                    bcmfp_compress_key_delete_child(trie, curr_node,
                                                    curr_node_prev_sib,
                                                    parent_node);
                    in_node_linked = 0;

                    /* Link current node sub-trie to parent node */
                    parent_node->num_child += curr_node->num_child;
                    for (prefix = curr_node->prefix + 1;
                         prefix <= trie->max_prefix_mask; prefix++) {
                        index = trie->max_prefix_mask - prefix;
                        if (curr_node->child[index] == NULL) {
                            continue;
                        }
                        bcmfp_compress_key_insert_child(trie,
                                                        curr_node->child[index],
                                                        parent_node);
                        /* Already assigned */
                        parent_node->num_child--;
                    }

                    /* Free current node */
                    bcmfp_compress_key_node_free(unit, curr_node, trie);
                    curr_node = NULL;
                }
            } else {
                /* No match with curr_node */
                curr_node_prev_sib = curr_node;
                curr_node = curr_node->sibling;
            }
        }
        curr_node_prev_sib = NULL;
    }

    if (in_node_linked) {
        /* Node not found in trie */
        return (SHR_E_NOT_FOUND);
    }

    if (lc_node) {
        /* Add list compressed node in trie */
        rv = bcmfp_compress_key_insert(unit, &node_info, trie, user_data);
        if (SHR_FAILURE(rv)) {
            if (rv == SHR_E_RESOURCE) {
                /* Add the node back in list compression list */
                node_info.flags = BCMFP_TRIE_NODE_LIST_COMPRESSED;
                (void)(bcmfp_compress_key_insert(unit, &node_info, trie, user_data));

                /* Add the deleted node back to trie */
                in_node_info->flags = BCMFP_TRIE_NODE_WIDTH_COMPRESSED;
                in_node_info->ref_count = 1;
                sal_memset(in_node_info->cid.w, 0, sizeof(bcmfp_cid_t));
                sal_memset(in_node_info->cid_mask.w, 0, sizeof(bcmfp_cid_t));
                (void)(bcmfp_compress_key_insert(unit, in_node_info, trie, user_data));
            }
            return (rv);
        }

        /* Free list compressed node */
        bcmfp_compress_key_node_free(unit, lc_node, trie);
        lc_node = NULL;
    } else {
        /* Update CID/CID Mask */
        rv = (bcmfp_compress_id_update(unit, trie, parent_node, user_data));
        if (SHR_FAILURE(rv)) {
            return (rv);
        }
    }

    rv = SHR_E_NONE;
exit:
    return (rv);
}

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
int
bcmfp_compress_trie_create(int unit,
                           uint8_t max_prefix_mask,
                           uint8_t max_cid_mask,
                           uint8_t trie_type,
                           bcmfp_fn_cid_update_t cid_update_cb,
                           bcmfp_shr_trie_md_t **trie_new)
{
    bcmfp_shr_trie_md_t *trie = NULL;
    bcmfp_basic_info_key_t root_info;
    int rv = SHR_E_MEMORY;

    BCMFP_COMP_ALLOC(trie, sizeof(bcmfp_shr_trie_md_t),
                     "bcmfpTrieManagementDataStructure");
    *trie_new = trie;
    trie->max_prefix_mask = max_prefix_mask;
    trie->max_cid_mask = max_cid_mask;
    trie->cid_update_cb = cid_update_cb;
    trie->trie_type = trie_type;

    sal_memset(&root_info, 0, sizeof(bcmfp_basic_info_key_t));

    rv = (bcmfp_compress_key_node_alloc(unit, &root_info, &trie->root, trie));
    if (SHR_FAILURE(rv)) {
        return (rv);
    }

    SHR_BITSET(trie->root->cid.w, trie->max_cid_mask - 1);
    SHR_BITSET(trie->root->cid_mask.w, trie->max_cid_mask - 1);

exit:
    return (rv);
}

/*!
* \brief Delete all the compression nodes in in_node's sub-trie.
*
* \param [in]  Node  Node to be freed.
*
* \retval SHR_E_NONE   Success
* \retval SHR_E_PARAM  Invalid parameters
*/

STATIC void
bcmfp_compress_key_nodes_delete(int unit,
                                bcmfp_shr_trie_md_t *trie,
                                bcmfp_compress_key_t *in_node)
{
    int prefix;
    bcmfp_compress_key_t *curr_node = NULL;
    bcmfp_compress_key_t *curr_node_sib = NULL;

    for (prefix = in_node->prefix + 1;
         prefix <= trie->max_prefix_mask; prefix++) {
        curr_node = in_node->child[trie->max_prefix_mask - prefix];
        while (curr_node) {
            curr_node_sib = curr_node->sibling;
            bcmfp_compress_key_nodes_delete(unit, trie, curr_node);
            curr_node = curr_node_sib;
        }
    }
    bcmfp_compress_key_node_free(unit, in_node, trie);
    in_node = NULL;
}

/*!
 * \brief Free the trie metadata sructure.
 *
 * \param [in]  unit   Logical device id
 * \param [in]  trie   Pointer to created trie
 *
 * \retval SHR_E_NONE  Success
 * \retval SHR_E_PARAM Invalid parameters
 */

int
bcmfp_compress_trie_deinit(int unit, bcmfp_shr_trie_md_t *trie)
{
    bcmfp_compress_key_t *curr_node = NULL;
    bcmfp_compress_key_t *curr_node_tmp = NULL;
    if (trie) {
        if (trie->root) {
            curr_node = trie->root->sibling;
            while (curr_node) {
                /* Delete all list compressed nodes */
                curr_node_tmp = curr_node;
                curr_node = curr_node->sibling;
                bcmfp_compress_key_node_free(unit, curr_node_tmp, trie);
                curr_node_tmp = NULL;
            }
            /* Delete all nodes in the trie. */
            bcmfp_compress_key_nodes_delete(unit, trie, trie->root);
        }
        BCMFP_COMP_FREE(trie);
        trie = NULL;
    }
    return SHR_E_NONE;
}
