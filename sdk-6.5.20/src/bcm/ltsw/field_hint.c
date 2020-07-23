/*! \file field_hint.c
 *
 * Field Module Hint Implementation above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>

#include <bcm_int/ltsw/field.h>
#include <bcm_int/ltsw/mbcm/field.h>

#include <shr/shr_debug.h>
/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_FP

/* Hintid Bitmap Manupulation Macros */
#define BCMINT_FP_HINTID_BMP_ADD(bmp, hintid)    SHR_BITSET(((bmp).w), (hintid))
#define BCMINT_FP_HINTID_BMP_REMOVE(bmp, hintid) SHR_BITCLR(((bmp).w), (hintid))
#define BCMINT_FP_HINTID_BMP_TEST(bmp, hintid)   SHR_BITGET(((bmp).w), (hintid))


/******************************************************************************
 * Private functions
 */
static int
ltsw_field_hints_id_alloc(int unit, bcm_field_hintid_t *hint_id)
{
    uint32  hint_itr = 0;
    bcmint_field_control_info_t *fp_control = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    for (hint_itr = BCMI_FIELD_HA_HINT_ID_BASE;
         hint_itr < BCMI_FIELD_HA_HINT_ID_MAX; hint_itr++) {
        if (BCMINT_FP_HINTID_BMP_TEST
                ((fp_control->hintid_bmp), hint_itr) == 0) {
            *hint_id = hint_itr;
            SHR_EXIT();
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_field_hints_node_get(int unit,
                          bcmi_field_ha_hints_t *hint_list,
                          bcm_field_hint_t *hint,
                          bcmi_field_ha_hint_entry_t **hint_node)
{
    bcmi_field_ha_hint_entry_t *hint_entry = NULL;
    SHR_FUNC_ENTER(unit);

    /* Input parameter check */
    SHR_NULL_CHECK(hint_list, SHR_E_PARAM);
    SHR_NULL_CHECK(hint, SHR_E_PARAM);

    /* Loop through the hints and search for a match, if found fill
     * the entire hint and also return the hint_node that matched */
    FP_HINT_ENTRY_OPER_FIRST(unit,
                             hint_list->hints.blk_id,
                             hint_list->hints.blk_offset,
                             hint_entry);

    while (hint_entry != NULL) {
        if (hint_entry->hint_type == hint->hint_type) {
            /* Hint Found */
            switch (hint_entry->hint_type) {
            case bcmFieldHintTypeCompression:
                if (hint_entry->qual == hint->qual) {
                    *hint_node = hint_entry;
                    /* Fill flags, max_values, start_bit, end_bit */
                    hint->flags = hint_entry->flags;
                    hint->max_values = hint_entry->max_values;
                    hint->start_bit = hint_entry->start_bit;
                    hint->end_bit = hint_entry->end_bit;
                    SHR_EXIT();
                }
                break;
            case bcmFieldHintTypeExtraction:
                if (hint_entry->qual == hint->qual) {
                    *hint_node = hint_entry;
                    /* Fill flags, max_values, start_bit, end_bit */
                    if (hint->qual == bcmFieldQualifyUdf) {
                        hint->udf_id = hint_entry->udf_id;
                        hint->value = hint_entry->value;
                        hint->start_bit = hint_entry->start_bit;
                        hint->end_bit = hint_entry->end_bit;
                    } else {
                        hint->flags = hint_entry->flags;
                        hint->max_values = hint_entry->max_values;
                        hint->start_bit = hint_entry->start_bit;
                        hint->end_bit = hint_entry->end_bit;
                    }
                    SHR_EXIT();
                }
                break;
            case bcmFieldHintTypeGroupAutoExpansion:
                {
                    *hint_node = hint_entry;
                    /* Fill flags, max_group_size */
                    hint->flags = hint_entry->flags;
                    hint->max_group_size = hint_entry->max_group_size;
                    SHR_EXIT();
                }
                break;
            case bcmFieldHintTypeFlexCtrOpaqueObjectSel:
                if (hint_entry->value == hint->value) {
                    *hint_node = hint_entry;
                    SHR_EXIT();
                }
                break;
            case bcmFieldHintTypePolicerPoolSel:
                {
                    *hint_node = hint_entry;
                    hint->value = hint_entry->value;
                    SHR_EXIT();
                }
                break;
            default:
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                break;
            }
        }
        FP_HINT_ENTRY_OPER_NEXT(unit, hint_entry, hint_entry);
    }
    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_field_hint_entry_insert(int unit,
                             bcm_field_hintid_t hint_id,
                             bcm_field_hint_t *hint)
{
    uint8_t ha_blk_id;
    uint32_t ha_blk_offset;
    void *hash_blk = NULL;
    bcmi_field_ha_blk_type_t blk_type;
    bcmi_field_ha_blk_info_t *hint_blk = NULL;
    bcmi_field_ha_hint_entry_t *oper = NULL;
    bcmi_field_ha_hints_t *hint_list = NULL;

    SHR_FUNC_ENTER(unit);

    ha_blk_id = FP_HINT_LIST_BLK_ID(unit);
    hash_blk = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
    if (hash_blk == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    hint_list = (bcmi_field_ha_hints_t *)
                  (hash_blk + sizeof(bcmi_field_ha_blk_hdr_t));
    hint_blk = &(hint_list[hint_id].hints);
    if ((hint_list == NULL) || (hint_blk == NULL)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    ha_blk_id = hint_blk->blk_id;
    ha_blk_offset = hint_blk->blk_offset;
    FP_HINT_ENTRY_BLK_TYPE(unit, blk_type);
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_ha_blk_element_acquire(unit, blk_type,
                                            (void *)&oper,
                                            &ha_blk_id,
                                            &ha_blk_offset));

    if (hint_blk->blk_id == BCM_FIELD_HA_BLK_ID_INVALID) {
        hint_blk->blk_id = ha_blk_id;
        hint_blk->blk_offset = ha_blk_offset;
        oper->next.blk_id = BCM_FIELD_HA_BLK_ID_INVALID;;
        oper->next.blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
    } else {
        oper->next.blk_id = hint_blk->blk_id;
        oper->next.blk_offset = hint_blk->blk_offset;
        hint_blk->blk_id = ha_blk_id;
        hint_blk->blk_offset = ha_blk_offset;
    }

    /* Update group operation structure */
    oper->hint_type = hint->hint_type;
    oper->qual = hint->qual;
    oper->max_values = hint->max_values;
    oper->start_bit = hint->start_bit;
    oper->end_bit = hint->end_bit;
    oper->flags = hint->flags;
    oper->max_group_size = hint->max_group_size;
    oper->udf_id = hint->udf_id;
    oper->value = hint->value;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_field_hint_entry_delete(int unit,
                             bcm_field_hintid_t hint_id,
                             bcm_field_hint_t *hint)
{
    uint8_t ha_blk_id;
    uint32_t ha_blk_offset;
    void *hash_blk = NULL;
    bcmi_field_ha_blk_type_t blk_type;
    bcmi_field_ha_blk_info_t *hint_blk = NULL;
    bcmi_field_ha_hint_entry_t *hint_oper = NULL;
    bcmi_field_ha_hint_entry_t *hint_oper_temp = NULL;
    bcmi_field_ha_hints_t *hint_list = NULL;

    SHR_FUNC_ENTER(unit);

    ha_blk_id = FP_HINT_LIST_BLK_ID(unit);
    hash_blk = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
    if (hash_blk == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    hint_list = (bcmi_field_ha_hints_t *)
                  (hash_blk + sizeof(bcmi_field_ha_blk_hdr_t));
    hint_blk = &(hint_list[hint_id].hints);
    if ((hint_list == NULL) || (hint_blk == NULL)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    ha_blk_id = hint_blk->blk_id;
    ha_blk_offset = hint_blk->blk_offset;
    FP_HINT_ENTRY_BLK_TYPE(unit, blk_type);

    FP_HINT_ENTRY_OPER_FIRST(unit, ha_blk_id, ha_blk_offset, hint_oper);
    while (hint_oper != NULL) {
        if ((hint_oper->hint_type == hint->hint_type) &&
            (hint_oper->qual == hint->qual)) {
            if (hint_oper_temp == NULL) {
                ha_blk_id = hint_blk->blk_id;
                ha_blk_offset = hint_blk->blk_offset;
                hint_blk->blk_id = hint_oper->next.blk_id;
                hint_blk->blk_offset = hint_oper->next.blk_offset;
            } else {
                ha_blk_id = hint_oper_temp->next.blk_id;
                ha_blk_offset = hint_oper_temp->next.blk_offset;
                hint_oper_temp->next.blk_id = hint_oper->next.blk_id;
                hint_oper_temp->next.blk_offset = hint_oper->next.blk_offset;
            }
            break;
        }
        hint_oper_temp = hint_oper;
        FP_HINT_ENTRY_OPER_NEXT(unit, hint_oper, hint_oper);
    }

    /* Return not found if hash ptr is null */
    if (!hint_oper) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_ha_blk_element_release(unit, ha_blk_id,
                                             ha_blk_offset,
                                             blk_type));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_field_hint_entry_delete_all(int unit,
                                 bcm_field_hintid_t hint_id)
{
    uint8_t blk_id, ha_blk_id;
    uint32_t blk_offset, ha_blk_offset;
    void *hash_blk = NULL;
    bcmi_field_ha_blk_type_t blk_type;
    bcmi_field_ha_blk_info_t *hint_blk = NULL;
    bcmi_field_ha_hint_entry_t *hint_oper = NULL;
    bcmi_field_ha_hints_t *hint_list = NULL;

    SHR_FUNC_ENTER(unit);

    ha_blk_id = FP_HINT_LIST_BLK_ID(unit);
    hash_blk = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
    if (hash_blk == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    hint_list = (bcmi_field_ha_hints_t *)
                  (hash_blk + sizeof(bcmi_field_ha_blk_hdr_t));
    hint_blk = &(hint_list[hint_id].hints);
    if ((hint_list == NULL) || (hint_blk == NULL)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    ha_blk_id = hint_blk->blk_id;
    ha_blk_offset = hint_blk->blk_offset;
    FP_HINT_ENTRY_BLK_TYPE(unit, blk_type);

    FP_HINT_ENTRY_OPER_FIRST(unit, ha_blk_id, ha_blk_offset, hint_oper);
    while (hint_oper != NULL) {
        blk_id = ha_blk_id;
        blk_offset = ha_blk_offset;

        if (hint_oper->next.blk_id != BCM_FIELD_HA_BLK_ID_INVALID) {
            ha_blk_id = hint_oper->next.blk_id;
            ha_blk_offset = hint_oper->next.blk_offset;
            hint_oper = (bcmi_field_ha_hint_entry_t *)
                (FP_HA_BLK_HDR_PTR(unit, ha_blk_id) + ha_blk_offset);
        } else {
            hint_oper = NULL;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_ha_blk_element_release(unit, blk_id,
                                                 blk_offset,
                                                 blk_type));
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */
int
bcmint_field_hint_init(int unit)
{
    uint32_t size = 0;
    uint32_t  hint_itr = 0;
    bcmint_field_control_info_t *fp_control = NULL;
    bcmi_field_ha_hints_t *f_ht = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));
    f_ht = fp_control->hints_hash;
    if (f_ht == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    size = SHR_BITALLOCSIZE(BCMI_FIELD_HA_HINT_ID_MAX);
    BCMINT_FIELD_MEM_ALLOC(fp_control->hintid_bmp.w, size, "HintIds Bitmap");

    for (hint_itr = BCMI_FIELD_HA_HINT_ID_BASE;
         hint_itr < BCMI_FIELD_HA_HINT_ID_MAX; hint_itr++) {
         if (f_ht[hint_itr].hintid != 0) {
             BCMINT_FP_HINTID_BMP_ADD(fp_control->hintid_bmp, hint_itr);
         }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_hint_cleanup(int unit)
{
    bcmint_field_control_info_t *fp_control = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));
    BCMINT_FIELD_MEM_FREE(fp_control->hintid_bmp.w);

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_hints_create(int unit,
                          bcm_field_hintid_t *hint_id)
{
    uint32_t hash_index = 0;
    bcmint_field_control_info_t *fp_control = NULL;
    bcmi_field_ha_hints_t *f_ht = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(hint_id, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));
    SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_hints_id_alloc(unit, hint_id));
    BCMINT_FP_HINTID_BMP_ADD(fp_control->hintid_bmp, *hint_id);

    hash_index = (*hint_id & BCMINT_FP_HASH_INDEX_MASK);
    f_ht = &fp_control->hints_hash[hash_index];
    if (f_ht == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    f_ht->hintid = *hint_id;
    f_ht->hints.blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
    f_ht->hints.blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_hints_destroy(int unit,
                           bcm_field_hintid_t hint_id)
{
    uint32_t hash_index = 0;
    bcmi_field_ha_hints_t *f_ht = NULL;
    bcmint_field_control_info_t *fp_control = NULL;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_control_get(unit, &fp_control));
    rv = ltsw_field_hint_entry_delete_all(unit, hint_id);
    if ( SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    hash_index = (hint_id & BCMINT_FP_HASH_INDEX_MASK);
    f_ht = &fp_control->hints_hash[hash_index];
    if (f_ht == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    sal_memset(f_ht, 0, sizeof(bcmi_field_ha_hints_t));
    f_ht->hints.blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
    f_ht->hints.blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;

    BCMINT_FP_HINTID_BMP_REMOVE(fp_control->hintid_bmp, hint_id);

exit:
    SHR_FUNC_EXIT();
}


int
bcmint_field_hints_add(int unit,
                       bcm_field_hintid_t hint_id,
                       bcm_field_hint_t *hint)
{
    uint32_t hash_index = 0;
    int rv = SHR_E_NONE;
    bcmi_field_ha_hints_t *f_ht = NULL;
    bcm_field_hint_t  tmp_hint;
    bcmint_field_control_info_t *fp_control = NULL;
    bcmi_field_ha_hint_entry_t *hint_node = NULL;
    SHR_FUNC_ENTER(unit);

    /* validate input params */
    SHR_NULL_CHECK(hint, SHR_E_PARAM);
    if (hint_id >= BCMI_FIELD_HA_HINT_ID_MAX) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));
    if (BCMINT_FP_HINTID_BMP_TEST((fp_control->hintid_bmp), hint_id) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    hash_index = (hint_id & BCMINT_FP_HASH_INDEX_MASK);
    f_ht = &fp_control->hints_hash[hash_index];
    if (f_ht == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (f_ht->grp_ref_count != 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
    }

    if (f_ht->hint_count >= BCMI_FIELD_HA_MAX_HINTS_PER_HINTID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
    }

    if (hint->hint_type == bcmFieldHintTypePolicerPoolSel) {
        if (hint->value >= bcmPolicerPoolCount) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    sal_memcpy (&tmp_hint, hint, sizeof (bcm_field_hint_t));
    rv = ltsw_field_hints_node_get(unit, f_ht, &tmp_hint, &hint_node);

    if ((rv == SHR_E_NONE) && (hint_node != NULL)) {
        if ((bcmFieldHintTypeExtraction != hint_node->hint_type) ||
            (hint_node->qual == hint->qual)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_hint_entry_insert(unit, hint_id, hint));

    f_ht->hint_count++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_hints_get(int unit,
                       bcm_field_hintid_t hint_id,
                       bcm_field_hint_t *hint)
{
    uint32_t hash_index = 0;
    bcmi_field_ha_hints_t *f_ht = NULL;
    bcmi_field_ha_hint_entry_t *hint_node = NULL;
    bcmint_field_control_info_t *fp_control = NULL;

    SHR_FUNC_ENTER(unit);

    /* validate input params */
    SHR_NULL_CHECK(hint, SHR_E_PARAM);
    if (hint_id >= BCMI_FIELD_HA_HINT_ID_MAX) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    if (BCMINT_FP_HINTID_BMP_TEST((fp_control->hintid_bmp), hint_id) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    hash_index = (hint_id & BCMINT_FP_HASH_INDEX_MASK);
    f_ht = &fp_control->hints_hash[hash_index];
    if (f_ht == NULL ||
        (f_ht->hints.blk_id == BCM_FIELD_HA_BLK_ID_INVALID) ||
        (f_ht->hints.blk_offset == BCM_FIELD_HA_BLK_OFFSET_INVALID)) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_hints_node_get(unit, f_ht, hint, &hint_node));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_hints_delete(int unit,
                          bcm_field_hintid_t hint_id,
                          bcm_field_hint_t *hint)
{
    uint32_t hash_index = 0;
    bcmi_field_ha_hints_t *f_ht = NULL;
    bcmi_field_ha_hint_entry_t *hint_node = NULL;
    bcmint_field_control_info_t *fp_control = NULL;

    SHR_FUNC_ENTER(unit);

    /* validate input params */
    SHR_NULL_CHECK(hint, SHR_E_PARAM);
    if (hint_id >= BCMI_FIELD_HA_HINT_ID_MAX) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    if (BCMINT_FP_HINTID_BMP_TEST((fp_control->hintid_bmp), hint_id) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    hash_index = (hint_id & BCMINT_FP_HASH_INDEX_MASK);
    f_ht = &fp_control->hints_hash[hash_index];
    if (f_ht == NULL ||
        (f_ht->hints.blk_id == BCM_FIELD_HA_BLK_ID_INVALID) ||
        (f_ht->hints.blk_offset == BCM_FIELD_HA_BLK_OFFSET_INVALID)) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* HintId is attached to a group. Hence deletion not allowed */
    if (f_ht->grp_ref_count != 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
    }
    SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_hints_node_get(unit, f_ht, hint, &hint_node));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_hint_entry_delete(unit, hint_id, hint));

    if (f_ht->hint_count > 0) {
        f_ht->hint_count--;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_hints_delete_all(int unit,
                              bcm_field_hintid_t hint_id)
{
    uint32_t hash_index = 0;
    bcmi_field_ha_hints_t *f_ht = NULL;
    bcmint_field_control_info_t *fp_control = NULL;

    SHR_FUNC_ENTER(unit);

    /* validate input params */
    if (hint_id >= BCMI_FIELD_HA_HINT_ID_MAX) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    if (BCMINT_FP_HINTID_BMP_TEST((fp_control->hintid_bmp), hint_id) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    hash_index = (hint_id & BCMINT_FP_HASH_INDEX_MASK);
    f_ht = &fp_control->hints_hash[hash_index];
    if (f_ht == NULL ||
        (f_ht->hints.blk_id == BCM_FIELD_HA_BLK_ID_INVALID) ||
        (f_ht->hints.blk_offset == BCM_FIELD_HA_BLK_OFFSET_INVALID)) {
        SHR_EXIT();
    }

    /* HintId is attached to a group. Hence deletion not allowed */
    if (f_ht->grp_ref_count != 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_hint_entry_delete_all(unit, hint_id));

    f_ht->hint_count = 0;
    f_ht->hints.blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
    f_ht->hints.blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_hints_destroy_all(int unit)
{
    uint32_t hash_index = 0;
    uint32_t  hint_id = 0;
    bcmi_field_ha_hints_t *f_ht = NULL;
    bcmint_field_control_info_t *fp_control = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    for (hint_id = BCMI_FIELD_HA_HINT_ID_BASE;
         hint_id < BCMI_FIELD_HA_HINT_ID_MAX; hint_id++) {

        if (BCMINT_FP_HINTID_BMP_TEST((fp_control->hintid_bmp), hint_id) == 0) {
            continue;
        }

        hash_index = (hint_id & BCMINT_FP_HASH_INDEX_MASK);
        f_ht = &fp_control->hints_hash[hash_index];
        if (f_ht == NULL ||
            (f_ht->hints.blk_id == BCM_FIELD_HA_BLK_ID_INVALID) ||
            (f_ht->hints.blk_offset == BCM_FIELD_HA_BLK_OFFSET_INVALID)) {
            continue;
        }

        /* HintId is attached to a group. Hence deletion not allowed */
        if (f_ht->grp_ref_count != 0) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_hint_entry_delete_all(unit, hint_id));

        f_ht->hint_count = 0;
        f_ht->hints.blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
        f_ht->hints.blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
    }
    fp_control->hints_hash = NULL;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_hint_group_count_update(int unit,
                                     bcm_field_hintid_t hint_id,
                                     uint8 action)
{
    uint32_t hash_index = 0;
    bcmi_field_ha_hints_t *f_ht = NULL;
    bcmint_field_control_info_t *fp_control = NULL;

    SHR_FUNC_ENTER(unit);

    if (hint_id == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    hash_index = (hint_id & BCMINT_FP_HASH_INDEX_MASK);
    f_ht = &fp_control->hints_hash[hash_index];
    if (!f_ht) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (action) {
        f_ht->grp_ref_count++;
    } else {
        if (f_ht->grp_ref_count > 0) {
            f_ht->grp_ref_count--;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_hints_group_qual_value_update(int unit,
                                           bcm_field_hintid_t hint_id,
                                           uint8_t *qual_hint_extracted,
                                           int qual_idx,
                                           /*width of qual map*/
                                           int width,
                                           /*offset of qual map*/
                                           int map_offset,
                                           uint64_t *max_value)
{
    uint32_t hash_index = 0;
    bcmi_field_ha_hints_t *f_ht = NULL;
    bcmint_field_control_info_t *fp_control = NULL;
    bcmi_field_ha_hint_entry_t *hint_entry = NULL;
    int start_bit = 0;
    int num_bits_to_set = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    hash_index = (hint_id & BCMINT_FP_HASH_INDEX_MASK);
    f_ht = &fp_control->hints_hash[hash_index];
    if (!f_ht || (f_ht->hintid != hint_id)) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    *max_value = 0;
    *qual_hint_extracted = 0;
    FP_HINT_ENTRY_OPER_FIRST(unit,
                             f_ht->hints.blk_id,
                             f_ht->hints.blk_offset,
                             hint_entry);

    while (hint_entry != NULL) {
        if ((bcmFieldHintTypeExtraction == hint_entry->hint_type)
            && (qual_idx == hint_entry->qual)) {
            /* the below code reads the extractor bits, check if the bit range
             * is in the specified qualifier map and sets the
             *  bitmap of the qualifier map accordingly.
             */

            if (hint_entry->start_bit < (map_offset + width)) {
                if (hint_entry->start_bit <= map_offset) {
                    if (hint_entry->end_bit >= map_offset) {
                        if ( hint_entry->end_bit < (map_offset + width)) {
                            /* the end bit is within the qual map */
                            start_bit =  0;
                            num_bits_to_set =
                                hint_entry->end_bit - map_offset + 1;
                        } else {
                            /* the entire map is in the extractor bit range */
                            start_bit = 0;
                            num_bits_to_set = width;
                        }
                    }
                } else {
                    /* the start bit is within the map*/
                    if ( hint_entry->end_bit < (map_offset +width)) {
                        /* end bit is within the qual map*/
                        start_bit = hint_entry->start_bit - map_offset;
                        num_bits_to_set = hint_entry->end_bit -
                                   hint_entry->start_bit + 1;
                    } else {
                        /* end bit is outside the qual map */
                        start_bit = hint_entry->start_bit - map_offset;
                        num_bits_to_set = (map_offset+width) -
                                       hint_entry->start_bit;
                    }
                }
            }
            if (num_bits_to_set) {
                SHR_BITSET_RANGE((uint32 *)max_value,
                                  start_bit, num_bits_to_set);
                *qual_hint_extracted = 1;
            }
            break;
        }
        FP_HINT_ENTRY_OPER_NEXT(unit, hint_entry, hint_entry);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_udf_qset_bitmap_alloc(
    int unit,
    bcmi_field_udf_qset_bitmap_t **udf_qset_bitmap)
{
    bcmi_field_udf_qset_bitmap_t *ptr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(udf_qset_bitmap, SHR_E_PARAM);

    BCMINT_FIELD_MEM_ALLOC((*udf_qset_bitmap),
            sizeof (bcmi_field_udf_qset_bitmap_t),
            "Udf qset bitmap");

    ptr = *udf_qset_bitmap;
    BCMINT_FIELD_MEM_ALLOC(ptr->chunk_b1_bitmap,
            32 * sizeof(uint32_t), "udf chunk b1 bitmap");
    BCMINT_FIELD_MEM_ALLOC(ptr->chunk_b2_bitmap,
            32 * sizeof(uint32_t), "udf chunk b2 bitmap");
    BCMINT_FIELD_MEM_ALLOC(ptr->chunk_b4_bitmap,
            32 * sizeof(uint32_t), "udf chunk b2 bitmap");

exit:
    if (SHR_FUNC_ERR()) {
        bcmint_field_group_udf_qset_bitmap_free(unit,
                udf_qset_bitmap);
    }

    SHR_FUNC_EXIT();
}

int
bcmint_field_group_udf_qset_bitmap_free(
    int unit,
    bcmi_field_udf_qset_bitmap_t **udf_qset_bitmap)
{
    if ((udf_qset_bitmap == NULL) ||
        (*udf_qset_bitmap == NULL)) {
        return SHR_E_NONE;
    }

    BCMINT_FIELD_MEM_FREE((*udf_qset_bitmap)->chunk_b1_bitmap);
    BCMINT_FIELD_MEM_FREE((*udf_qset_bitmap)->chunk_b2_bitmap);
    BCMINT_FIELD_MEM_FREE((*udf_qset_bitmap)->chunk_b4_bitmap);
    BCMINT_FIELD_MEM_FREE(*udf_qset_bitmap);

    *udf_qset_bitmap = NULL;

    return SHR_E_NONE;
}

int
bcmint_field_group_udf_qset_bitmap_update(
    int unit,
    bcm_field_hintid_t hint_id,
    bcm_udf_id_t udf_id,
    bcm_field_qset_t *udf_qset,
    bcmi_field_udf_qset_bitmap_t *udf_qset_bitmap)
{
    uint32_t hash_index = 0;
    bool udf_hint_extracted;
    bcmi_field_ha_hints_t *f_ht = NULL;
    bcmint_field_control_info_t *fp_control = NULL;
    bcmi_field_ha_hint_entry_t *hint_entry = NULL;
    bcm_field_hint_t hint;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    hash_index = (hint_id & BCMINT_FP_HASH_INDEX_MASK);
    f_ht = &fp_control->hints_hash[hash_index];
    if (!f_ht || (f_ht->hintid != hint_id)) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    FP_HINT_ENTRY_OPER_FIRST(unit,
            f_ht->hints.blk_id,
            f_ht->hints.blk_offset,
            hint_entry);

    udf_hint_extracted = FALSE;
    while (hint_entry != NULL) {
        if ((bcmFieldHintTypeExtraction == hint_entry->hint_type)
                && (bcmFieldQualifyUdf == hint_entry->qual)
                && (udf_id == hint_entry->udf_id)) {

            hint.hint_type = bcmFieldHintTypeExtraction;
            hint.qual = bcmFieldQualifyUdf;
            hint.udf_id = hint_entry->udf_id;
            hint.value = hint_entry->value;
            hint.start_bit = hint_entry->start_bit;
            hint.end_bit = hint_entry->end_bit;
            SHR_IF_ERR_EXIT(
                    mbcm_ltsw_field_group_udf_qset_update_with_hints(unit,
                        hint_entry->udf_id, &hint, udf_qset, udf_qset_bitmap));

            udf_hint_extracted = TRUE;
        }
        FP_HINT_ENTRY_OPER_NEXT(unit, hint_entry, hint_entry);
    }

    if (udf_hint_extracted == FALSE) {
        SHR_IF_ERR_EXIT(
                mbcm_ltsw_field_group_udf_qset_update_with_hints(unit,
                    udf_id, NULL, udf_qset, udf_qset_bitmap));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_udf_qset_update_with_hints(
    int unit,
    bcm_field_hintid_t hint_id,
    bcm_field_qset_t *qset,
    bcm_field_qset_t *udf_qset,
    bcmi_field_udf_qset_bitmap_t *udf_qset_bitmap)
{
    int idx, actual;
    uint32_t hash_index = 0;
    bcmi_field_ha_hints_t *f_ht = NULL;
    bcmint_field_control_info_t *fp_control = NULL;
    bcm_udf_id_t udf_obj_arr[100] = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(mbcm_ltsw_field_udf_obj_get(unit,
                *qset, 100, &udf_obj_arr[0], &actual));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    hash_index = (hint_id & BCMINT_FP_HASH_INDEX_MASK);
    f_ht = &fp_control->hints_hash[hash_index];
    if (!f_ht || (f_ht->hintid != hint_id)) {

        /* Hint is not found. Still update udf_qset_bitmap */
        for (idx = 0; idx < actual; idx++) {
            SHR_IF_ERR_EXIT(
                    mbcm_ltsw_field_group_udf_qset_update_with_hints(unit,
                        udf_obj_arr[idx], NULL, udf_qset, udf_qset_bitmap));
        }
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    for (idx = 0; idx < actual; idx++) {

        SHR_IF_ERR_VERBOSE_EXIT(
                bcmint_field_group_udf_qset_bitmap_update(unit,
                    hint_id, udf_obj_arr[idx], udf_qset,
                    udf_qset_bitmap));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_hint_dump(int unit, bcm_field_hintid_t hint_id)
{
    uint32_t hint_count = 1;
    uint32_t hash_index = 0;
    bcmi_field_ha_hints_t *f_ht = NULL;
    bcmi_field_ha_hint_entry_t *hint_entry = NULL;
    bcmint_field_control_info_t *fp_control = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    hash_index = (hint_id & BCMINT_FP_HASH_INDEX_MASK);
    f_ht = &fp_control->hints_hash[hash_index];

    if ((f_ht != NULL) && (f_ht->hintid != 0)) {

        FP_HINT_ENTRY_OPER_FIRST(unit,
                                 f_ht->hints.blk_id,
                                 f_ht->hints.blk_offset,
                                 hint_entry);


        LOG_CLI((BSL_META_U(unit,"Hints information for HintID (%d)\r\n"),hint_id));
        LOG_CLI((BSL_META_U(unit,"==================================\r\n")));
        LOG_CLI((BSL_META_U(unit,
                        "Number of Groups Attached  = %d\r\n"), f_ht->grp_ref_count));
        LOG_CLI((BSL_META_U(unit,
                        "Number of Hints present    = %d\r\n"), f_ht->hint_count));

        while (hint_entry != NULL) {
            LOG_CLI((BSL_META_U(unit, "Hint %d\r\n"), hint_count));
            LOG_CLI((BSL_META_U(unit, "=======\r\n")));
            if (hint_entry->hint_type
                == bcmFieldHintTypeGroupAutoExpansion) {
                LOG_CLI((BSL_META_U(unit,"Hint Type = AutoExpansion\r\n")));
            } else if (hint_entry->hint_type
                       == bcmFieldHintTypeCompression) {
                LOG_CLI((BSL_META_U(unit,"HintType  = Compression\r\n")));
            } else if (hint_entry->hint_type
                       == bcmFieldHintTypeExtraction) {
                LOG_CLI((BSL_META_U(unit,"HintType  = Extraction\r\n")));
            } else if (hint_entry->hint_type
                       == bcmFieldHintTypeExactMatch) {
                LOG_CLI((BSL_META_U(unit,"HintType  = Exact Match\r\n")));
            } else {
                LOG_CLI((BSL_META_U(unit,"HintType      = Unknown\r\n")));
            }
            LOG_CLI((BSL_META_U(unit, "Qual           = %u\r\n"),
                     hint_entry->qual));
            LOG_CLI((BSL_META_U(unit, "Max Values     = %u\r\n"),
                     hint_entry->max_values));
            LOG_CLI((BSL_META_U(unit, "Start Bit      = %u\r\n"),
                     hint_entry->start_bit));
            LOG_CLI((BSL_META_U(unit, "End Bit        = %u\r\n"),
                     hint_entry->end_bit));
            LOG_CLI((BSL_META_U(unit, "Flags          = %u\r\n"),
                     hint_entry->flags));
            LOG_CLI((BSL_META_U(unit, "Max Group Size = %u\r\n"),
                     hint_entry->max_group_size));
            hint_count++;
            FP_HINT_ENTRY_OPER_NEXT(unit, hint_entry, hint_entry);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmint_field_hint_dump_all(int unit)
{
    bcm_field_hintid_t id = 0;
    bcmint_field_control_info_t *fp_control = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    for (id = BCMI_FIELD_HA_HINT_ID_BASE; id < BCMI_FIELD_HA_HINT_ID_MAX; id++) {
        if (BCMINT_FP_HINTID_BMP_TEST(fp_control->hintid_bmp, id)) {
            LOG_CLI((BSL_META_U(unit, "HINTID %d \r\n"), id));
            LOG_CLI((BSL_META_U(unit,"=========\r\n")));
            bcmint_field_hint_dump(unit, id);
        }
    }

exit:
    SHR_FUNC_EXIT();
}
