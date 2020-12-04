/*! \file field_compression_id_mgmt.c
 *
 * APIs to operate on compression id management from keys.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/esw/field_compression/field_compression.h>
#include <bcm_int/esw/field_compression/field_trie_mgmt.h>

/*!
 * \brief Update CID/CID_MASK for parent_node's sub-trie
 *
 * \param [in] trie          Trie on which operation is to be performed
 * \param [in] parent_node
 * \param [in] list          Pointer to list of keys whose cid/cid_mask changed
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_RESOURCE Out of cid bits to assign
 */

STATIC int
_bcm_field_update_cids(int unit, _bcm_field_shr_trie_md_t *trie,
                       _bcm_field_compress_key_t *parent_node,
                       _bcm_field_cid_update_list_t *list)
{
    int prefix, bits_in_cid = 1, start_bit_in_cid = 0, p_cid_mask = 0, index;
    int rv = SHR_E_MEMORY;
    uint32_t child_num = 0;
    _bcm_field_compress_key_t *curr_node;
    _bcm_field_cid_update_list_t *list_node, *list_iter;

    if (parent_node->num_child == 0) {
        /* Nothing to do */
        return (SHR_E_NONE);
    }

    child_num = parent_node->num_child;
    while (child_num >>= 1) {
        /* Bits in CID required for parent_node's children */
        bits_in_cid++;
    }

    /* Get parent_node's CID mask length (in bits) */
    SHR_BITCOUNT_RANGE(parent_node->cid_mask.w, p_cid_mask, 0,
                       trie->max_cid_mask);

    /* Start bit in CID for parent_node's immediate children */
    start_bit_in_cid = trie->max_cid_mask - p_cid_mask - bits_in_cid;

    if (start_bit_in_cid < 0) {
       /* Out of CID bits */
       return (SHR_E_RESOURCE);
    }

    /* Start assigning cid from 1 */
    child_num = 1;
    for (prefix = parent_node->prefix + 1;
         prefix <= trie->max_prefix_mask; prefix++) {
        index = trie->max_prefix_mask - prefix;

        /* Traverse parent_node's sub-trie */
        curr_node = parent_node->child[index];

        while (curr_node) {
            /* Save old cid/cid_mask in update list */
            list_node = NULL;
            _BCM_FIELD_COMP_ALLOC(list_node, sizeof(_bcm_field_cid_update_list_t),
                                  "_bcm_fieldCidUpdateListNode");
            sal_memcpy(&list_node->key, &curr_node->key, sizeof(_bcm_field_key_t));
            list_node->prefix = curr_node->prefix;
            sal_memcpy(&list_node->old_cid, &curr_node->cid,
                       sizeof(_bcm_field_cid_t));
            sal_memcpy(&list_node->old_cid_mask, &curr_node->cid_mask,
                       sizeof(_bcm_field_cid_t));

            SHR_BITCOPY_RANGE(curr_node->cid.w, 0,
                              parent_node->cid.w, 0,
                              trie->max_cid_mask);
            SHR_BITCOPY_RANGE(curr_node->cid.w, start_bit_in_cid,
                              &child_num, 0,
                              bits_in_cid);

            SHR_BITCOPY_RANGE(curr_node->cid_mask.w, 0,
                              parent_node->cid_mask.w, 0,
                              trie->max_cid_mask);
            SHR_BITSET_RANGE(curr_node->cid_mask.w,
                             start_bit_in_cid,
                             bits_in_cid);


            if (!SHR_BITEQ_RANGE(list_node->old_cid.w, curr_node->cid.w,
                                 0, trie->max_cid_mask) ||
                !SHR_BITEQ_RANGE(list_node->old_cid_mask.w, curr_node->cid_mask.w,
                                 0, trie->max_cid_mask) ||
                (curr_node->flags & _BCM_FIELD_TRIE_NODE_WIDTH_COMPRESSED)) {
                /*
                 * Existing cid/cid_mask has changed or node moved from
                 * list compression nodes list to trie.
                 * Save new cid/cid_mask in update list
                 */

                sal_memcpy(&list_node->new_cid, &curr_node->cid,
                           sizeof(_bcm_field_cid_t));
                sal_memcpy(&list_node->new_cid_mask, &curr_node->cid_mask,
                           sizeof(_bcm_field_cid_t));
                if ((curr_node->flags & _BCM_FIELD_TRIE_NODE_WIDTH_COMPRESSED) &&
                    !SHR_BITNULL_RANGE(list_node->old_cid.w, 0, trie->max_cid_mask)) {
                    /* If old cid is 0 then its not actually a node which was
                     * list compressed before
                     */
                    list_node->flags = _BCM_FIELD_TRIE_NODE_WIDTH_COMPRESSED;
                }

                list_iter = list;
                while (list_iter->next) {
                    list_iter = list_iter->next;
                }
                /* Attach the list node at end */
                list_iter->next = list_node;
                curr_node->flags = 0;
            } else {
                /* No change in cid/cid_mask */
                _BCM_FIELD_COMP_FREE(list_node);
                list_node = NULL;
            }

            /* Update child_index based on new cid */
            curr_node->child_index = child_num++;

            /* Call in recursion for the child. */
            rv = (_bcm_field_update_cids(unit, trie, curr_node, list));
            if (SHR_FAILURE(rv)) {
                return (rv);
            }

            curr_node = curr_node->sibling;
        }
    }

    rv = SHR_E_NONE;
exit:
    return (rv);
}

/*!
 * \brief Update CID/CID_MASK based on how trie looks like after
  *       in_node is inserted/deleted in parent_node
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

int
_bcm_field_compress_id_update(int unit, _bcm_field_shr_trie_md_t *trie,
                              _bcm_field_compress_key_t *parent_node,
                              void *user_data)
{
    _bcm_field_cid_update_list_t list, *list_iter;
    _bcm_field_cid_update_list_t *list_tmp_node, *list_node, *list_iter_new;
    _bcm_field_compress_key_t *curr_node;
    int rv;

    list.next = NULL;
    list_iter = &list;

    rv = (_bcm_field_update_cids(unit, trie, parent_node, &list));
    if (SHR_FAILURE(rv)) {
        goto exit;
    }

    /* Update list compressed nodes with new cid/cid_mask if any */
    curr_node = trie->root->sibling;
    list_iter_new = list_iter->next;
    while (curr_node) {
        list_tmp_node = list_iter_new;
        while (list_tmp_node) {
            if (SHR_BITEQ_RANGE(list_tmp_node->old_cid.w, curr_node->cid.w,
                                0, trie->max_cid_mask) &&
                SHR_BITEQ_RANGE(list_tmp_node->old_cid_mask.w,
                                curr_node->cid_mask.w, 0,
                                trie->max_cid_mask)) {
                /*
                 * cid/cid_mask of a list compressed node has changed
                 * Update the cid/cid_mask and add it in update list
                 */
                list_node = NULL;
                rv = SHR_E_MEMORY;
                _BCM_FIELD_COMP_ALLOC(list_node, sizeof(_bcm_field_cid_update_list_t),
                                      "_bcm_fieldCidUpdateListNode");
                sal_memcpy(&list_node->key, &curr_node->key, sizeof(_bcm_field_key_t));
                list_node->prefix = curr_node->prefix;
                list_node->flags = curr_node->flags;
                sal_memcpy(&list_node->old_cid, &curr_node->cid,
                           sizeof(_bcm_field_cid_t));
                sal_memcpy(&list_node->old_cid_mask, &curr_node->cid_mask,
                           sizeof(_bcm_field_cid_t));

                sal_memcpy(&curr_node->cid, &list_tmp_node->new_cid,
                           sizeof(_bcm_field_cid_t));
                sal_memcpy(&curr_node->cid_mask, &list_tmp_node->new_cid_mask,
                           sizeof(_bcm_field_cid_t));

                sal_memcpy(&list_node->new_cid, &curr_node->cid,
                           sizeof(_bcm_field_cid_t));
                sal_memcpy(&list_node->new_cid_mask, &curr_node->cid_mask,
                           sizeof(_bcm_field_cid_t));
                /* Add the new update list node at start of the list */
                list_node->next = list_iter->next;
                list_iter->next = list_node;
                break;
            }
            list_tmp_node = list_tmp_node->next;
        }
        curr_node = curr_node->sibling;
    }

    /*
     * Call callback function
     * Pass list_iter->next pointer to callback function
     */
    if (trie->cid_update_cb != NULL) {
        rv = (trie->cid_update_cb(unit, list_iter->next, user_data));
    } else {
        rv = SHR_E_NONE;
    }

exit:
    /* Free the list. */
    list_iter = list_iter->next;
    while (list_iter) {
        list_tmp_node = list_iter;
        list_iter = list_iter->next;
        _BCM_FIELD_COMP_FREE(list_tmp_node);
        list_tmp_node = NULL;
    }

    return (rv);
}
