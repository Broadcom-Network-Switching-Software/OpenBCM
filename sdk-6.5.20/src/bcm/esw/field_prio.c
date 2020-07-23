/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field_prio.c
 * Purpose:     FP entry priority handling (for entries in External TCAM)
 */

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/cmic.h>
#include <soc/field.h>

#ifdef BCM_FIELD_SUPPORT
#include <bcm/error.h>
#include <bcm/field.h>
#include <bcm_int/esw/field.h>
#if defined(BCM_TRIUMPH_SUPPORT)
#include <bcm/l2.h>
#include <bcm/l3.h>
#include <bcm/vlan.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */


/*
 * Function:
 *     _bcm_field_prio_mgmt_init
 * Purpose:
 *     Allocate memory for priority management 
 * Parameters:
 *     unit     - (IN) BCM unit
 *     stage_fc - (IN) Stage
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_MEMORY   - No memory
 * Notes:
 *     Used only for External TCAM
 */
int
_bcm_field_prio_mgmt_init(int unit, _field_stage_t *stage_fc)
{
    int i;
    _field_prio_mgmt_t *prio_temp;

    if (stage_fc->stage_id != _BCM_FIELD_STAGE_EXTERNAL) {
        return (BCM_E_NONE);
    }

    for (i = 0; i < stage_fc->tcam_slices; i++) {

        prio_temp = sal_alloc(sizeof (struct _field_prio_mgmt_s), 
                              "Entry prio mgmt");
        if (prio_temp == NULL) {
            _bcm_field_prio_mgmt_deinit(unit, stage_fc);
            return (BCM_E_MEMORY);
        }

        prio_temp->prio = BCM_FIELD_ENTRY_PRIO_LOWEST;
        prio_temp->start_index = 0;
        prio_temp->end_index =
            stage_fc->slices[_FP_DEF_INST][i].entry_count - 1;
        prio_temp->num_free_entries =
            stage_fc->slices[_FP_DEF_INST][i].entry_count;
        prio_temp->prev = NULL;
        prio_temp->next = NULL;

        stage_fc->slices[_FP_DEF_INST][i].prio_mgmt = prio_temp;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_prio_mgmt_deinit
 * Purpose:
 *     Free memory used for priority management 
 * Parameters:
 *     unit     - (IN) BCM unit
 *     stage_fc - (IN) Stage
 * Returns:
 *     BCM_E_NONE     - Success
 * Notes:
 *     Used only for External TCAM
 */
int
_bcm_field_prio_mgmt_deinit(int unit, _field_stage_t *stage_fc)
{
    int i;
    _field_prio_mgmt_t *prio1, *prio2;
    
    if (stage_fc->stage_id != _BCM_FIELD_STAGE_EXTERNAL) {
        return (BCM_E_NONE);
    }

    for (i = 0; i < stage_fc->tcam_slices; i++) {
        prio1 = stage_fc->slices[_FP_DEF_INST][i].prio_mgmt;
        while (prio1 != NULL) {
            prio2 = prio1->next;
            sal_free(prio1);
            prio1 = prio2;
        }
        stage_fc->slices[_FP_DEF_INST][i].prio_mgmt = NULL;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_prio_mgmt_move
 * Purpose:
 *     Move f_ent from index1 to index2
 * Parameters:
 *     unit   - (IN) BCM unit
 *     f_ent  - (IN) Entry to be moved
 *     index1 - (IN) source index
 *     index2 - (IN) destination index
 * Returns:
 *     none
 * Notes:
 *     Used only for External TCAM
 *     It is the calling function's responsibility to
 *         update the prio_mgmt structure.
 */
void
_field_prio_mgmt_move(int unit, _field_entry_t *f_ent, int index1, int index2)
{
#if defined (BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        _bcm_field_tr3_external_entry_move(unit, f_ent, index1, index2);
        return;
    } 
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined (BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIUMPH(unit) || SOC_IS_TRIUMPH2(unit))  {
        _bcm_field_tr_external_entry_move(unit, f_ent, index1, index2);
        return;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
}

/*
 * Function:
 *     _field_prio_mgmt_shift_prev
 * Purpose:
 *     Create an empty slot in target_node
 * Parameters:
 *     unit           - (IN) BCM unit
 *     stage_fc       - (IN) Stage
 *     slice_numb     - (IN) Slice under consideration
 *     prev_free_node - (IN) prev node which has a free slot
 *     target_node    - (IN) node where an empty slot needs to be created
 * Returns:
 *     none
 */
void
_field_prio_mgmt_shift_prev(int unit, _field_stage_t *stage_fc, int slice_numb,
                            _field_prio_mgmt_t *prev_free_node, 
                            _field_prio_mgmt_t *target_node)
{
    _field_prio_mgmt_t *node1, *node2;

    node1 = prev_free_node;
    node2 = target_node;

    node1->end_index--;
    node1->num_free_entries--;

    node1->next->start_index--;
    node1->next->num_free_entries++;

    node1 = node1->next;

    while (node1 != node2) {
        _field_prio_mgmt_move(unit, 
            stage_fc->slices[_FP_DEF_INST][slice_numb]
            .entries[node1->end_index], 
            node1->end_index, node1->start_index);

        node1->end_index--;
        node1->num_free_entries--;

        node1->next->start_index--;
        node1->next->num_free_entries++;

        node1 = node1->next;
    }
}

/*
 * Function:
 *     _field_prio_mgmt_shift_next 
 * Purpose:
 *     Create an empty slot in target_node
 * Parameters:
 *     unit           - (IN) BCM unit
 *     stage_fc       - (IN) Stage
 *     slice_numb     - (IN) Slice under consideration
 *     target_node    - (IN) node where an empty slot needs to be created
 *     next_free_node - (IN) next node which has a free slot
 * Returns:
 *     none
 */
void
_field_prio_mgmt_shift_next(int unit, _field_stage_t *stage_fc, int slice_numb,
                            _field_prio_mgmt_t *target_node, 
                            _field_prio_mgmt_t *next_free_node)
{
    _field_prio_mgmt_t *node1, *node2;

    node1 = target_node;
    node2 = next_free_node;

    while (node2 != node1) {
        if ((node2->end_index - node2->start_index + 1) >
            (node2->num_free_entries)) {
            _field_prio_mgmt_move(unit, 
                stage_fc->slices[_FP_DEF_INST][slice_numb]
                .entries[node2->start_index], 
                node2->start_index, 
                node2->end_index + 1 - node2->num_free_entries);
        }

        node2->start_index++;
        node2->num_free_entries--;

        node2->prev->end_index++;
        node2->prev->num_free_entries++;

        node2 = node2->prev;
    }

    /* 
     * Now it is guaranteed that there is a free slot at 
     *     target_node->end_index
     * Also, target_node == node1 == node2 now.
     *     See comment after the calling location
     */
    _field_prio_mgmt_move(unit, 
        stage_fc->slices[_FP_DEF_INST][slice_numb].entries[node2->start_index],
        node2->start_index, node2->end_index);
}

/*
 * Function:
 *     _bcm_field_entry_target_location
 * Purpose:
 *     Find the target location for an entry, given its location
 * Parameters:
 *     unit     - (IN) BCM unit
 *     stage_fc - (IN) Stage
 *     f_ent    - (IN) Entry whose target location needs to be found
 *     new_prio - (IN) new priority of the entry
 *     new_location - (OUT) target location
 * Returns:
 *     BCM_E_NONE     - Success
 * Notes:
 *     - Used only for External TCAM
 *     - This may involve moving entries in the slice
 *     - There is some more scope for optimization, in the main ELSE clause
 *         e.g. instead of getting 1 entry, try for more.
 */
int
_bcm_field_entry_target_location(int unit, _field_stage_t *stage_fc, 
                                 _field_entry_t *f_ent, int new_prio, 
                                 uint32 *new_location)
{
    _field_prio_mgmt_t *list, *target_node, *node;
    _field_prio_mgmt_t *prev_free_node, *next_free_node;
    int prev_free_dist, next_free_dist;
    int slice_numb;
    
    prev_free_node = next_free_node = NULL;
    prev_free_dist = next_free_dist = 0;
    
    slice_numb = f_ent->fs->slice_number;
    list = stage_fc->slices[_FP_DEF_INST][slice_numb].prio_mgmt;

    while (list != NULL) {
        if (_field_entry_prio_cmp(list->prio, new_prio) <= 0) {
            break;
        }
        if (list->num_free_entries > 0) {
            prev_free_node = list;
            prev_free_dist = 0;
        } else {
            prev_free_dist++; /* Valid only if prev_free_node != NULL */
        }
        list = list->next;
    }

    assert(list != NULL);
    
    target_node = list;

    /*
     * COVERITY
     *
     * assert passes only when list is not NULL
     */
    /* coverity[var_deref_op : FALSE] */
    list = list->next;
    /* Find next free node */
    while (list != NULL) {
        next_free_dist++; /* Valid only if next_free_node != NULL */
        if (list->num_free_entries > 0) {
            next_free_node = list;
            break;
        }
        list = list->next;
    }

    /* See if target_node has free entries */
    if (target_node->num_free_entries > 0) {
        /* 
         * If priority is same, use an empty slot 
         * OPTIMIZATION: as packed, use the next free index
         */
        if (target_node->prio == new_prio) {
            *new_location = target_node->end_index + 1 - 
                            target_node->num_free_entries;
        }
        /* 
         * Otherwise, divide this node into 2 regions
         */
        else {
            /* 
             * See if there are any entries. 
             *   If so, Move entry at target_node->start_index to an empty slot
             * OPTIMIZATION: as packed, use the next free index
             */
            if ((target_node->end_index - target_node->start_index + 1) >
                (target_node->num_free_entries)) {
                _field_prio_mgmt_move(unit, 
                    stage_fc->slices[_FP_DEF_INST][slice_numb].
                             entries[target_node->start_index],
                    target_node->start_index, 
                    target_node->end_index + 1 - target_node->num_free_entries);
            }

            /* Now form a new node with only 1 entry */
            node = sal_alloc(sizeof(struct _field_prio_mgmt_s), "prio node");
            if (node == NULL) {
                return (BCM_E_MEMORY);
            }
            node->prio = new_prio;

            node->start_index = node->end_index = target_node->start_index;
            *new_location = node->start_index;
            target_node->start_index++;

            node->num_free_entries = 1;
            target_node->num_free_entries--;

            node->prev = target_node->prev;
            if (node->prev == NULL) {
                stage_fc->slices[_FP_DEF_INST][slice_numb].prio_mgmt = node;
            } else {
                node->prev->next = node;
            }
            node->next = target_node;
            target_node->prev = node;
        }
    } else {
        if (prev_free_node != NULL) {
            if (next_free_node != NULL) {
                if (prev_free_dist <= next_free_dist) {
                    _field_prio_mgmt_shift_prev(unit, stage_fc, slice_numb, 
                                                prev_free_node, target_node);
                } else {
                    _field_prio_mgmt_shift_next(unit, stage_fc, slice_numb, 
                                                target_node, next_free_node);
                }
            } else {
                _field_prio_mgmt_shift_prev(unit, stage_fc, slice_numb, 
                                            prev_free_node, target_node);
            }
        } else {
            if (next_free_node != NULL) {
                _field_prio_mgmt_shift_next(unit, stage_fc, slice_numb, 
                                            target_node, next_free_node);
            } else {
                return (BCM_E_RESOURCE);
            }
        }
        /*
         * Now an empty slot is created at the
         *     target_node->start_index
         * NOTE: for new_prio == target_node->prio, 1 shift could
         *       be avoided if the empty slot was created at the end,
         *       when shift_next was being used.
         *     BUT for uniformity with new_prio != target_node->prio, 
         *       and with shift_prev, we don't do this.    
         */
        *new_location = target_node->start_index;

        if (target_node->prio != new_prio) {
            /* Form a new node */
            node = sal_alloc(sizeof(struct _field_prio_mgmt_s), "prio node");
            if (node == NULL) {
                return (BCM_E_MEMORY);
            }
            node->prio = new_prio;
            
            node->start_index = node->end_index = target_node->start_index;
            target_node->start_index++;
            
            node->num_free_entries = 1;
            target_node->num_free_entries = 0;
            
            node->prev = target_node->prev;
            if (node->prev == NULL) {
                stage_fc->slices[_FP_DEF_INST][slice_numb].prio_mgmt = node;
            } else {
                node->prev->next = node;
            }
            node->next = target_node;
            target_node->prev = node;
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_entry_prio_mgmt_update
 * Purpose:
 *     Updates the prio_mgmt structure
 * Parameters:
 *     unit  - (IN) unit
 *     f_ent - (IN) entry which was installed/removed
 *     flag  - (IN) +1 = Install, -1 = Remove
 *     old_location - (IN) Valid only if flag == -1
 *                         Location from which entry is/was removed.
 *                         is: entry_remove; was: prio_set
 * Returns: 
 *     none
 * Notes:
 *     Can optimize to collapse a node if all its entries are free
 */
int
_bcm_field_entry_prio_mgmt_update(int unit, _field_entry_t *f_ent, 
                                  int flag, uint32 old_location)
{
    _field_stage_t *stage_fc;
    _field_prio_mgmt_t *list;
    int prio;

    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, f_ent->fs->stage_id, &stage_fc));

    list = f_ent->fs->prio_mgmt;
    prio = f_ent->prio;

    while (list != NULL) {
        if (list->prio == prio) {
            break;
        }
        list = list->next;
    }
    if (list == NULL) {
        return (BCM_E_INTERNAL);
    }

    if (flag == 1) {
        list->num_free_entries--;
    } else {
        /* 
         * Pack the enties in this priority class 
         *     Move the bottom most entry to old_location
         *         unless old_location was bottom most (may be last)
         */
        if (old_location != (list->end_index - list->num_free_entries)) {
            _field_prio_mgmt_move(unit,
                stage_fc->slices[_FP_DEF_INST][f_ent->fs->slice_number].
                entries[list->end_index - list->num_free_entries],
                list->end_index - list->num_free_entries,
                old_location);
        }

        list->num_free_entries++;
    }
    return (BCM_E_NONE);
}


/* Rebuild the linked list of entry indices spanned by priorities
 */

int
_bcm_field_prio_mgmt_slice_reinit(int            unit,
                                  _field_stage_t *stage_fc,
                                  _field_slice_t *fs
                                  )
{
    unsigned           eidx;
    _field_entry_t     *f_ent;
    _field_prio_mgmt_t *lastnode, *newnode, *temp_node;

    /* Free the memory allocated during init */
    while (fs->prio_mgmt) {
        temp_node = fs->prio_mgmt->next;
        sal_free(fs->prio_mgmt);
        fs->prio_mgmt = temp_node;
    }
    fs->prio_mgmt = NULL;

    /* Scan all entries in slice */

    for (lastnode = 0, eidx = 0; eidx < fs->entry_count; ++eidx) {
        f_ent = fs->entries[eidx];
        if ((lastnode == 0) || (lastnode->prio != 0) || (f_ent && f_ent->prio != lastnode->prio)) {
            /* Create entry-index-span node */
            
            newnode = (struct _field_prio_mgmt_s *)
                sal_alloc(sizeof(*newnode), "Entry prio mgmt");
            
            if (newnode == 0) {
                return (BCM_E_MEMORY);
            }
            
            /* Priority of entries in this span */
            newnode->prio = f_ent ? f_ent->prio : 0;
            /* Span starts here */
            newnode->start_index = eidx;
            /* No free entries yet */
            newnode->num_free_entries = 0;
            
            /* Hook in new span node */
            
            *(lastnode ? &lastnode->next : &fs->prio_mgmt) = newnode;
            newnode->prev = lastnode;
            newnode->next = 0;
            
            lastnode = newnode;
        }

        if (f_ent == 0) {
            /* Entry not in use */
            
            ++lastnode->num_free_entries;
        }   

        lastnode->end_index = eidx;
    }

    return (BCM_E_NONE);
}

#endif /* BCM_FIELD_SUPPORT */

