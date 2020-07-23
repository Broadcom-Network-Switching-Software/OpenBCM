/*! \file bcmfp_idp.c
 *
 * BCMFP operational information APIs corresponding to
 * LT interdependency. This file contains APIs that are
 * common to all LT  inter dependencies. LT specific
 * inter dependency APIs are part of LT specific
 * bcmfp_oper_xxx.c file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_ha.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/generated/bcmfp_ha.h>
#include <bcmfp/bcmfp_idp_internal.h>
#include <bcmfp/bcmfp_internal.h>
#include <shr/shr_crc.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

STATIC int
map_attrs_get(int unit,
              bcmfp_stage_t *stage,
              int map_type,
              bcmfp_idp_map_attrs_t * map_attrs)
{
    SHR_FUNC_ENTER(unit);

    switch (map_type) {
        case BCMFP_IDP_MAP_TYPE_GROUP_TO_ENTRY:
        case BCMFP_IDP_MAP_TYPE_RULE_TO_ENTRY:
        case BCMFP_IDP_MAP_TYPE_POLICY_TO_ENTRY:
        case BCMFP_IDP_MAP_TYPE_METER_TO_ENTRY:
        case BCMFP_IDP_MAP_TYPE_CTR_TO_ENTRY:
        case BCMFP_IDP_MAP_TYPE_PDD_TO_ENTRY:
        case BCMFP_IDP_MAP_TYPE_SBR_TO_ENTRY:
        case BCMFP_IDP_MAP_TYPE_DPOLICY_TO_GROUP:
        case BCMFP_IDP_MAP_TYPE_PSE_TO_GROUP:
        case BCMFP_IDP_MAP_TYPE_PSG_TO_GROUP:
        case BCMFP_IDP_MAP_TYPE_PDD_TO_GROUP:
        case BCMFP_IDP_MAP_TYPE_DPDD_TO_GROUP:
        case BCMFP_IDP_MAP_TYPE_SBR_TO_GROUP:
        case BCMFP_IDP_MAP_TYPE_DSBR_TO_GROUP:
        case BCMFP_IDP_MAP_TYPE_ENTRY_TO_LC_ENTRY:
            map_attrs->src_id_size = 1;
            if (stage->idp_maps_modulo == 1) {
                map_attrs->hash_lookup = FALSE;
            } else {
                map_attrs->hash_lookup = TRUE;
            }
            break;
        case BCMFP_IDP_MAP_TYPE_CID_TO_ENTRY:
            map_attrs->src_id_size = 1;
            map_attrs->hash_lookup = TRUE;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
map_add_to_hash(int unit,
                bcmfp_stage_t *stage,
                void *map_segment,
                uint32_t map_size,
                int map_type,
                bcmfp_idp_map_attrs_t *map_attrs,
                uint32_t *src_id,
                uint32_t ent_or_grp_id)
{
    bcmfp_ha_blk_info_t *blk_info = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    uint8_t first_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t first_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_generic_data_t *ha_element = NULL;
    bcmfp_generic_data_t *ha_element_bucket = NULL;
    uint32_t hash_value = 0;
    uint8_t *hash_src = NULL;
    int nbits = 0;
    bool src_id_found = FALSE;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(map_segment, SHR_E_PARAM);
    SHR_NULL_CHECK(map_attrs, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(src_id, SHR_E_PARAM);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage->stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                               BCMFP_TRANS_STATE_UC_A));

    if (map_attrs->src_id_size == 1) {
        hash_value = (src_id[0] % map_size);
    } else {
        hash_src = (uint8_t *)src_id;
        nbits = (map_attrs->src_id_size * 32);
        hash_value = shr_crc16b(0, hash_src, nbits);
        hash_value = (hash_value % map_size);
    }
    blk_info = (bcmfp_ha_blk_info_t *)map_segment;
    blk_id = blk_info[hash_value].blk_id;
    blk_offset = blk_info[hash_value].blk_offset;

    /* Find the HA element bucket. */
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
        blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element_bucket));
        blk_id = ha_element_bucket->next_blk_id;
        blk_offset = ha_element_bucket->next_blk_offset;
        src_id_found = TRUE;
        for (idx = 0; idx < map_attrs->src_id_size; idx++) {
            if (ha_element_bucket[idx].value != src_id[idx]) {
                src_id_found = FALSE;
                break;
            }
        }
        if (src_id_found == TRUE) {
            break;
        }
    }

    /*
     * If it is the first time then create HA element with
     * required bucket size.
     */
    if (src_id_found == FALSE) {
        ha_element_bucket = NULL;
        /* Create the bucket. */
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmfp_ha_elements_acquire(unit,
                                      1,
                                      (map_attrs->src_id_size + 1),
                                      &ha_element_bucket,
                                      &first_blk_id,
                                      &first_blk_offset));
        ha_element_bucket[0].value = src_id[0];
        ha_element_bucket[0].next_blk_id =
                           blk_info[hash_value].blk_id;
        ha_element_bucket[0].next_blk_offset =
                           blk_info[hash_value].blk_offset;
        for (idx = 1; idx < map_attrs->src_id_size; idx++) {
            ha_element_bucket[idx].value =  src_id[idx];
            ha_element_bucket[idx].next_blk_id =
                                   BCMFP_HA_BLK_ID_INVALID;
            ha_element_bucket[idx].next_blk_offset =
                                   BCMFP_HA_BLK_OFFSET_INVALID;
        }
        blk_info[hash_value].blk_id = first_blk_id;
        blk_info[hash_value].blk_offset = first_blk_offset;
        first_blk_id = BCMFP_HA_BLK_ID_INVALID;
        first_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
        /* Create first HA element in the bucket. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_elements_acquire(unit,
                                       1,
                                       BCMFP_HA_BLK_BUCKET_SIZE_ONE,
                                       &ha_element,
                                       &first_blk_id,
                                       &first_blk_offset));
        ha_element->value = ent_or_grp_id;
        ha_element->next_blk_id = BCMFP_HA_BLK_ID_INVALID;
        ha_element->next_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;

        ha_element_bucket[idx].value = 0;
        ha_element_bucket[idx].next_blk_id = first_blk_id;
        ha_element_bucket[idx].next_blk_offset = first_blk_offset;
        SHR_EXIT();
    }

    /*
     * Create the HA element to store the entry or group ID and
     * link it to ha bucket.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_elements_acquire(unit,
                                   1,
                                   BCMFP_HA_BLK_BUCKET_SIZE_ONE,
                                   &ha_element,
                                   &first_blk_id,
                                   &first_blk_offset));
    ha_element->value = ent_or_grp_id;
    idx = map_attrs->src_id_size;
    ha_element->next_blk_id = ha_element_bucket[idx].next_blk_id;
    ha_element->next_blk_offset = ha_element_bucket[idx].next_blk_offset;
    ha_element_bucket[idx].next_blk_id = first_blk_id;
    ha_element_bucket[idx].next_blk_offset = first_blk_offset;
exit:
    SHR_FUNC_EXIT();
}

STATIC int
map_add_to_array(int unit,
                 bcmfp_stage_t *stage,
                 void *map_segment,
                 uint32_t src_id,
                 uint32_t ent_or_grp_id)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_ha_blk_info_t *blk_info = NULL;
    bcmfp_generic_data_t *ha_element = NULL;
    uint8_t first_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t first_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;

    SHR_FUNC_ENTER(unit);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage->stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                               BCMFP_TRANS_STATE_UC_A));

    blk_info = (bcmfp_ha_blk_info_t *)map_segment;
    blk_id = blk_info[src_id].blk_id;
    blk_offset = blk_info[src_id].blk_offset;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_elements_acquire(unit,
                                   1,
                                   BCMFP_HA_BLK_BUCKET_SIZE_ONE,
                                   &ha_element,
                                   &first_blk_id,
                                   &first_blk_offset));
    ha_element->next_blk_id = blk_id;
    ha_element->next_blk_offset = blk_offset;
    ha_element->value = ent_or_grp_id;
    blk_info[src_id].blk_id = first_blk_id;
    blk_info[src_id].blk_offset = first_blk_offset;
exit:
    SHR_FUNC_EXIT();
}

STATIC int
map_delete_from_hash(int unit,
                     bcmfp_stage_t *stage,
                     void *map_segment,
                     uint32_t map_size,
                     int map_type,
                     bcmfp_idp_map_attrs_t *map_attrs,
                     uint32_t *src_id,
                     uint32_t ent_or_grp_id)
{
    bcmfp_ha_blk_info_t *blk_info = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bool ha_element_bucket_found = FALSE;
    bool ha_element_not_found = TRUE;
    uint8_t bucket_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t bucket_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_generic_data_t *ha_element = NULL;
    bcmfp_generic_data_t *ha_element_bucket = NULL;
    bcmfp_generic_data_t *prev_ha_element = NULL;
    bcmfp_generic_data_t *prev_ha_element_bucket = NULL;
    uint32_t hash_value = 0;
    uint8_t *hash_src = NULL;
    int nbits = 0;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(map_segment, SHR_E_PARAM);
    SHR_NULL_CHECK(map_attrs, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(src_id, SHR_E_PARAM);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage->stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                               BCMFP_TRANS_STATE_UC_A));

    if (map_attrs->src_id_size == 1) {
        hash_value = (src_id[0] % map_size);
    } else {
        hash_src = (uint8_t *)src_id;
        nbits = (map_attrs->src_id_size * 32);
        hash_value = shr_crc16b(0, hash_src, nbits);
        hash_value = (hash_value % map_size);
    }
    blk_info = (bcmfp_ha_blk_info_t *)map_segment;
    bucket_blk_id = blk_info[hash_value].blk_id;
    bucket_blk_offset = blk_info[hash_value].blk_offset;

    /* Find the HA element bucket. */
    while (bucket_blk_id != BCMFP_HA_BLK_ID_INVALID &&
           bucket_blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  bucket_blk_id,
                                  bucket_blk_offset,
                                  &ha_element_bucket));
        ha_element_bucket_found = TRUE;
        for (idx = 0; idx < map_attrs->src_id_size; idx++) {
            if (ha_element_bucket[idx].value != src_id[idx]) {
               ha_element_bucket_found = FALSE;
               break;
            }
        }
        if (ha_element_bucket_found == TRUE) {
            break;
        }
        prev_ha_element_bucket = ha_element_bucket;
        bucket_blk_id = ha_element_bucket[0].next_blk_id;
        bucket_blk_offset = ha_element_bucket[0].next_blk_offset;
    }

    /* HA element bucket must be found in case of map delete. */
    if (ha_element_bucket_found == FALSE) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Unlink and release the candidate HA element. */
    idx = map_attrs->src_id_size;
    blk_id = ha_element_bucket[idx].next_blk_id;
    blk_offset = ha_element_bucket[idx].next_blk_offset;
    prev_ha_element = &(ha_element_bucket[idx]);
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));
        if (ha_element->value == ent_or_grp_id) {
            ha_element_not_found = FALSE;
            break;
        }

        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
        prev_ha_element = ha_element;
    }

    if (ha_element_not_found == TRUE) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (prev_ha_element == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    } else {
        prev_ha_element->next_blk_id = ha_element->next_blk_id;
        prev_ha_element->next_blk_offset = ha_element->next_blk_offset;
    }
    ha_element->next_blk_id = BCMFP_HA_BLK_ID_INVALID;
    ha_element->next_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_elements_release(unit, blk_id, blk_offset));

    /*
     * If no other entry or group ID is assoiated to the template ID,
     * remove the HA bucket.
     */
    blk_id = ha_element_bucket[idx].next_blk_id;
    blk_offset = ha_element_bucket[idx].next_blk_offset;
    if (blk_id == BCMFP_HA_BLK_ID_INVALID &&
        blk_offset == BCMFP_HA_BLK_OFFSET_INVALID) {
        if (prev_ha_element_bucket != NULL) {
            prev_ha_element_bucket[0].next_blk_id =
                    ha_element_bucket[0].next_blk_id;
            prev_ha_element_bucket[0].next_blk_offset =
                    ha_element_bucket[0].next_blk_offset;
        } else {
            blk_info[hash_value].blk_id =
                    ha_element_bucket[0].next_blk_id;
            blk_info[hash_value].blk_offset =
                    ha_element_bucket[0].next_blk_offset;
        }
        /* Relese the HA element bucket allocated for the template ID. */
        ha_element_bucket[0].next_blk_id =
                             BCMFP_HA_BLK_ID_INVALID;
        ha_element_bucket[0].next_blk_offset =
                             BCMFP_HA_BLK_OFFSET_INVALID;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_elements_release(unit,
                                       bucket_blk_id,
                                       bucket_blk_offset));
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
map_delete_from_array(int unit,
                      bcmfp_stage_t *stage,
                      void *map_segment,
                      uint32_t src_id,
                      uint32_t ent_or_grp_id)
{
    bool element_found = FALSE;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_ha_blk_info_t *blk_info = NULL;
    bcmfp_generic_data_t *ha_element = NULL;
    bcmfp_generic_data_t *prev_ha_element = NULL;

    SHR_FUNC_ENTER(unit);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage->stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                               BCMFP_TRANS_STATE_UC_A));

    blk_info = (bcmfp_ha_blk_info_t *)map_segment;
    blk_id = blk_info[src_id].blk_id;
    blk_offset = blk_info[src_id].blk_offset;
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit, blk_id, blk_offset, &ha_element));
        if (ha_element->value == ent_or_grp_id) {
            element_found = TRUE;
            break;
        }

        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
        prev_ha_element = ha_element;
    }

    if (element_found == FALSE) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (prev_ha_element == NULL) {
        blk_info[src_id].blk_id = ha_element->next_blk_id;
        blk_info[src_id].blk_offset = ha_element->next_blk_offset;
    } else {
        prev_ha_element->next_blk_id = ha_element->next_blk_id;
        prev_ha_element->next_blk_offset = ha_element->next_blk_offset;
    }
    ha_element->next_blk_id = BCMFP_HA_BLK_ID_INVALID;
    ha_element->next_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_elements_release(unit, blk_id, blk_offset));
exit:
    SHR_FUNC_EXIT();
}

STATIC int
map_check_in_array(int unit,
                   bcmfp_stage_t *stage,
                   void *map_segment,
                   uint32_t src_id,
                   bool *not_mapped)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(not_mapped, SHR_E_PARAM);
    SHR_NULL_CHECK(map_segment, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

STATIC int
map_check_in_hash(int unit,
                  bcmfp_stage_t *stage,
                  void *map_segment,
                  uint32_t map_size,
                  int map_type,
                  bcmfp_idp_map_attrs_t *map_attrs,
                  uint32_t *src_id,
                  bool *not_mapped)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_ha_blk_info_t *blk_info = NULL;
    uint32_t hash_value = 0;
    bcmfp_generic_data_t *ha_element = NULL;
    uint8_t *hash_src = NULL;
    int nbits = 0;
    bool src_id_found = FALSE;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(not_mapped, SHR_E_PARAM);
    SHR_NULL_CHECK(map_segment, SHR_E_PARAM);
    SHR_NULL_CHECK(map_attrs, SHR_E_PARAM);
    SHR_NULL_CHECK(src_id, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (map_attrs->src_id_size == 1) {
        hash_value = (src_id[0] % map_size);
    } else {
        hash_src = (uint8_t *)src_id;
        nbits = (map_attrs->src_id_size * 32);
        hash_value = shr_crc16b(0, hash_src, nbits);
        hash_value = (hash_value % map_size);
    }
    *not_mapped = FALSE;
    blk_info = (bcmfp_ha_blk_info_t *)map_segment;
    blk_id = blk_info[hash_value].blk_id;
    blk_offset = blk_info[hash_value].blk_offset;

    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));
        src_id_found = TRUE;
        for (idx = 0; idx < map_attrs->src_id_size; idx++) {
             if (ha_element[idx].value != src_id[idx]) {
                 src_id_found = FALSE;
                 break;
             }
        }
        if (src_id_found == TRUE) {
            blk_id = ha_element[idx].next_blk_id;
            blk_offset = ha_element[idx].next_blk_offset;
            break;
        }
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
    }

    if (blk_id == BCMFP_HA_BLK_ID_INVALID &&
        blk_offset == BCMFP_HA_BLK_OFFSET_INVALID) {
        *not_mapped = TRUE;
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
map_src_id_get_first_in_array(int unit,
                        bcmfp_stage_t *stage,
                        void *map_segment,
                        uint32_t map_size,
                        bcmfp_idp_map_type_t map_type,
                        bcmfp_idp_map_attrs_t *map_attrs,
                        uint32_t hash_value,
                        uint32_t *src_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(src_id, SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

STATIC int
map_src_id_get_first_in_hash(int unit,
                        bcmfp_stage_t *stage,
                        void *map_segment,
                        uint32_t map_size,
                        bcmfp_idp_map_type_t map_type,
                        bcmfp_idp_map_attrs_t *map_attrs,
                        uint32_t hash_value,
                        uint32_t *src_id)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_ha_blk_info_t *blk_info = NULL;
    bcmfp_generic_data_t *ha_element = NULL;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(src_id, SHR_E_PARAM);
    SHR_NULL_CHECK(map_attrs, SHR_E_PARAM);

    blk_info = (bcmfp_ha_blk_info_t *)map_segment;
    blk_id = blk_info[hash_value].blk_id;
    blk_offset = blk_info[hash_value].blk_offset;

    if (blk_id != BCMFP_HA_BLK_ID_INVALID &&
        blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));
        for (idx = 0; idx < map_attrs->src_id_size; idx++) {
             src_id[idx] = ha_element[idx].value;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
map_src_id_get_next_in_array(int unit,
                       bcmfp_stage_t *stage,
                       void *map_segment,
                       uint32_t map_size,
                       bcmfp_idp_map_type_t map_type,
                       bcmfp_idp_map_attrs_t *map_attrs,
                       uint32_t hash_value,
                       uint32_t *src_id,
                       uint32_t *next_src_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(src_id, SHR_E_PARAM);
    SHR_NULL_CHECK(next_src_id, SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

STATIC int
map_src_id_get_next_in_hash(int unit,
                       bcmfp_stage_t *stage,
                       void *map_segment,
                       uint32_t map_size,
                       bcmfp_idp_map_type_t map_type,
                       bcmfp_idp_map_attrs_t *map_attrs,
                       uint32_t hash_value,
                       uint32_t *src_id,
                       uint32_t *next_src_id)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_ha_blk_info_t *blk_info = NULL;
    bcmfp_generic_data_t *ha_element = NULL;
    uint8_t idx = 0;
    bool src_id_found = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(src_id, SHR_E_PARAM);
    SHR_NULL_CHECK(next_src_id, SHR_E_PARAM);

    blk_info = (bcmfp_ha_blk_info_t *)map_segment;
    blk_id = blk_info[hash_value].blk_id;
    blk_offset = blk_info[hash_value].blk_offset;

    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));
        src_id_found = TRUE;
        for (idx = 0; idx < map_attrs->src_id_size; idx++) {
             if (ha_element[idx].value != src_id[idx]) {
                 src_id_found = FALSE;
                 break;
             }
        }
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
    }

    if (src_id_found != TRUE) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (blk_id == BCMFP_HA_BLK_ID_INVALID &&
        blk_offset == BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_element_get(unit,
                              blk_id,
                              blk_offset,
                              &ha_element));

    for (idx = 0; idx < map_attrs->src_id_size; idx++) {
         next_src_id[idx] = ha_element[idx].value;
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
map_traverse_in_array(int unit,
                      bcmfp_stage_t *stage,
                      void *map_segment,
                      uint32_t src_id,
                      bcmfp_entry_id_traverse_cb cb,
                      void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(map_segment, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

STATIC int
map_cond_traverse_in_array(int unit,
                           bcmfp_stage_t *stage,
                           void *map_segment,
                           uint32_t src_id,
                           bcmfp_entry_id_cond_traverse_cb cb,
                           void *user_data,
                           bool *cond_met)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(map_segment, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}


STATIC int
map_traverse_in_hash(int unit,
                     bcmfp_stage_t *stage,
                     void *map_segment,
                     uint32_t map_size,
                     int map_type,
                     bcmfp_idp_map_attrs_t *map_attrs,
                     uint32_t *src_id,
                     bcmfp_entry_id_traverse_cb cb,
                     void *user_data)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    uint32_t main_tbl_id = 0;
    bcmfp_ha_blk_info_t *blk_info = NULL;
    bcmfp_generic_data_t *ha_element = NULL;
    uint32_t hash_value = 0;
    uint8_t *hash_src = NULL;
    int nbits = 0;
    bool src_id_found = FALSE;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);
    SHR_NULL_CHECK(src_id, SHR_E_PARAM);
    SHR_NULL_CHECK(map_segment, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (map_attrs->src_id_size == 1) {
        hash_value = (src_id[0] % map_size);
    } else {
        hash_src = (uint8_t *)src_id;
        nbits = (map_attrs->src_id_size * 32);
        hash_value = shr_crc16b(0, hash_src, nbits);
        hash_value = (hash_value % map_size);
    }
    blk_info = (bcmfp_ha_blk_info_t *)map_segment;
    blk_id = blk_info[hash_value].blk_id;
    blk_offset = blk_info[hash_value].blk_offset;

    /* Step-1: Get the head of LL for the template ID. */
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));
        src_id_found = TRUE;
        for (idx = 0; idx < map_attrs->src_id_size; idx++) {
             if (ha_element[idx].value != src_id[idx]) {
                 src_id_found = FALSE;
                 break;
             }
        }
        if (src_id_found == TRUE) {
            blk_id = ha_element[idx].next_blk_id;
            blk_offset = ha_element[idx].next_blk_offset;
            break;
        }
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
    }

    /* Step-2: Loop through LL of the template ID. */
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
        main_tbl_id = ha_element->value;
        SHR_IF_ERR_VERBOSE_EXIT(cb(unit,
                                   stage->stage_id,
                                   main_tbl_id,
                                   src_id[0],
                                   user_data));
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
map_cond_traverse_in_hash(int unit,
                           bcmfp_stage_t *stage,
                           void *map_segment,
                           uint32_t map_size,
                           int map_type,
                           bcmfp_idp_map_attrs_t *map_attrs,
                           uint32_t *src_id,
                           bcmfp_entry_id_cond_traverse_cb cb,
                           void *user_data,
                           bool *cond_met)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    uint32_t main_tbl_id = 0;
    bcmfp_ha_blk_info_t *blk_info = NULL;
    bcmfp_generic_data_t *ha_element = NULL;
    uint32_t hash_value = 0;
    uint8_t *hash_src = NULL;
    int nbits = 0;
    bool src_id_found = FALSE;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);
    SHR_NULL_CHECK(src_id, SHR_E_PARAM);
    SHR_NULL_CHECK(map_segment, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (map_attrs->src_id_size == 1) {
        hash_value = (src_id[0] % map_size);
    } else {
        hash_src = (uint8_t *)src_id;
        nbits = (map_attrs->src_id_size * 32);
        hash_value = shr_crc16b(0, hash_src, nbits);
        hash_value = (hash_value % map_size);
    }
    blk_info = (bcmfp_ha_blk_info_t *)map_segment;
    blk_id = blk_info[hash_value].blk_id;
    blk_offset = blk_info[hash_value].blk_offset;

    /* Step-1: Get the head of LL for the template ID. */
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));
        src_id_found = TRUE;
        for (idx = 0; idx < map_attrs->src_id_size; idx++) {
             if (ha_element[idx].value != src_id[idx]) {
                 src_id_found = FALSE;
                 break;
             }
        }
        if (src_id_found == TRUE) {
            blk_id = ha_element[idx].next_blk_id;
            blk_offset = ha_element[idx].next_blk_offset;
            break;
        }
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
    }

    /* Step-2: Loop through LL of the template ID. */
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
        main_tbl_id = ha_element->value;
        SHR_IF_ERR_VERBOSE_EXIT(cb(unit,
                                stage->stage_id,
                                main_tbl_id,
                                src_id[0],
                                user_data,
                                cond_met));
            if (*cond_met == TRUE) {
                SHR_EXIT();
            }
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
map_traverse_first_in_array(int unit,
                      bcmfp_stage_t *stage,
                      void *map_segment,
                      uint32_t src_id,
                      bcmfp_entry_id_traverse_cb cb,
                      void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(map_segment, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

STATIC int
map_traverse_first_in_hash(int unit,
                      bcmfp_stage_t *stage,
                      void *map_segment,
                      uint32_t map_size,
                      int map_type,
                      bcmfp_idp_map_attrs_t *map_attrs,
                      uint32_t *src_id,
                      bcmfp_entry_id_traverse_cb cb,
                      void *user_data)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    uint32_t main_tbl_id = 0;
    bcmfp_ha_blk_info_t *blk_info = NULL;
    bcmfp_generic_data_t *ha_element = NULL;
    uint32_t hash_value = 0;
    uint8_t *hash_src = NULL;
    int nbits = 0;
    bool src_id_found = FALSE;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);
    SHR_NULL_CHECK(src_id, SHR_E_PARAM);
    SHR_NULL_CHECK(map_segment, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (map_attrs->src_id_size == 1) {
        hash_value = (src_id[0] % map_size);
    } else {
        hash_src = (uint8_t *)src_id;
        nbits = (map_attrs->src_id_size * 32);
        hash_value = shr_crc16b(0, hash_src, nbits);
        hash_value = (hash_value % map_size);
    }
    blk_info = (bcmfp_ha_blk_info_t *)map_segment;
    blk_id = blk_info[hash_value].blk_id;
    blk_offset = blk_info[hash_value].blk_offset;

    /* Step-1: Get the head of LL for the template ID. */
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));
        src_id_found = TRUE;
        for (idx = 0; idx < map_attrs->src_id_size; idx++) {
             if (ha_element[idx].value != src_id[idx]) {
                 src_id_found = FALSE;
                 break;
             }
        }
        if (src_id_found == TRUE) {
            blk_id = ha_element[idx].next_blk_id;
            blk_offset = ha_element[idx].next_blk_offset;
            break;
        }
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
    }

    /* Step-2: Validate head LL of the template ID. */
    if (blk_id != BCMFP_HA_BLK_ID_INVALID &&
        blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));
        main_tbl_id = ha_element->value;
        SHR_IF_ERR_VERBOSE_EXIT(cb(unit,
                                   stage->stage_id,
                                   main_tbl_id,
                                   src_id[0],
                                   user_data));
        SHR_EXIT();
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_idp_map_check(int unit,
                    bcmfp_stage_id_t stage_id,
                    void *map_segment,
                    uint32_t map_size,
                    bcmfp_idp_map_type_t map_type,
                    uint32_t *src_id,
                    bool *not_mapped)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_idp_map_attrs_t map_attrs;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(src_id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    sal_memset(&map_attrs, 0, sizeof(bcmfp_idp_map_attrs_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (map_attrs_get(unit, stage, map_type, &map_attrs));

    if (map_attrs.hash_lookup == FALSE) {
       SHR_IF_ERR_VERBOSE_EXIT
           (map_check_in_array(unit,
                               stage,
                               map_segment,
                               src_id[0],
                               not_mapped));
    } else {
       SHR_IF_ERR_VERBOSE_EXIT
           (map_check_in_hash(unit,
                              stage,
                              map_segment,
                              map_size,
                              map_type,
                              &map_attrs,
                              src_id,
                              not_mapped));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_idp_map_src_id_get_first(int unit,
                         bcmfp_stage_id_t stage_id,
                         void *map_segment,
                         uint32_t map_size,
                         bcmfp_idp_map_type_t map_type,
                         uint32_t hash_value,
                         uint32_t *src_id)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_idp_map_attrs_t map_attrs;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(src_id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    sal_memset(&map_attrs, 0, sizeof(bcmfp_idp_map_attrs_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (map_attrs_get(unit, stage, map_type, &map_attrs));

    if (map_attrs.hash_lookup == FALSE) {
       SHR_IF_ERR_VERBOSE_EXIT
           (map_src_id_get_first_in_array(unit,
                                    stage,
                                    map_segment,
                                    map_size,
                                    map_type,
                                    &map_attrs,
                                    hash_value,
                                    src_id));
    } else {
       SHR_IF_ERR_VERBOSE_EXIT
           (map_src_id_get_first_in_hash(unit,
                                    stage,
                                    map_segment,
                                    map_size,
                                    map_type,
                                    &map_attrs,
                                    hash_value,
                                    src_id));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_idp_map_src_id_get_next(int unit,
                         bcmfp_stage_id_t stage_id,
                         void *map_segment,
                         uint32_t map_size,
                         bcmfp_idp_map_type_t map_type,
                         uint32_t hash_value,
                         uint32_t *src_id,
                         uint32_t *next_src_id)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_idp_map_attrs_t map_attrs;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(src_id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    sal_memset(&map_attrs, 0, sizeof(bcmfp_idp_map_attrs_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (map_attrs_get(unit, stage, map_type, &map_attrs));

    if (map_attrs.hash_lookup == FALSE) {
       SHR_IF_ERR_VERBOSE_EXIT
           (map_src_id_get_next_in_array(unit,
                                   stage,
                                   map_segment,
                                   map_size,
                                   map_type,
                                   &map_attrs,
                                   hash_value,
                                   src_id,
                                   next_src_id));
    } else {
       SHR_IF_ERR_VERBOSE_EXIT
           (map_src_id_get_next_in_hash(unit,
                                   stage,
                                   map_segment,
                                   map_size,
                                   map_type,
                                   &map_attrs,
                                   hash_value,
                                   src_id,
                                   next_src_id));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_idp_map_traverse(int unit,
                       bcmfp_stage_id_t stage_id,
                       void *map_segment,
                       uint32_t map_size,
                       bcmfp_idp_map_type_t map_type,
                       uint32_t *src_id,
                       bcmfp_entry_id_traverse_cb cb,
                       void *user_data)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_idp_map_attrs_t map_attrs;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(src_id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    sal_memset(&map_attrs, 0, sizeof(bcmfp_idp_map_attrs_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (map_attrs_get(unit, stage, map_type, &map_attrs));

    if (map_attrs.hash_lookup == FALSE) {
       SHR_IF_ERR_VERBOSE_EXIT
           (map_traverse_in_array(unit,
                                  stage,
                                  map_segment,
                                  src_id[0],
                                  cb,
                                  user_data));
    } else {
       SHR_IF_ERR_VERBOSE_EXIT
           (map_traverse_in_hash(unit,
                                 stage,
                                 map_segment,
                                 map_size,
                                 map_type,
                                 &map_attrs,
                                 src_id,
                                 cb,
                                 user_data));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_idp_map_cond_traverse(int unit,
                            bcmfp_stage_id_t stage_id,
                            void *map_segment,
                            uint32_t map_size,
                            bcmfp_idp_map_type_t map_type,
                            uint32_t *src_id,
                            bcmfp_entry_id_cond_traverse_cb cb,
                            void *user_data,
                            bool *cond_met)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_idp_map_attrs_t map_attrs;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(src_id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    sal_memset(&map_attrs, 0, sizeof(bcmfp_idp_map_attrs_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (map_attrs_get(unit, stage, map_type, &map_attrs));

    if (map_attrs.hash_lookup == FALSE) {
       SHR_IF_ERR_VERBOSE_EXIT
           (map_cond_traverse_in_array(unit,
                                       stage,
                                       map_segment,
                                       src_id[0],
                                       cb,
                                       user_data,
                                       cond_met));
    } else {
       SHR_IF_ERR_VERBOSE_EXIT
           (map_cond_traverse_in_hash(unit,
                                      stage,
                                      map_segment,
                                      map_size,
                                      map_type,
                                      &map_attrs,
                                      src_id,
                                      cb,
                                      user_data,
                                      cond_met));
    }
exit:
    SHR_FUNC_EXIT();
}


int
bcmfp_idp_map_add(int unit,
                  bcmfp_stage_id_t stage_id,
                  void *map_segment,
                  uint32_t map_size,
                  bcmfp_idp_map_type_t map_type,
                  uint32_t *src_id,
                  uint32_t ent_or_grp_id)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_idp_map_attrs_t map_attrs;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(src_id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    sal_memset(&map_attrs, 0, sizeof(bcmfp_idp_map_attrs_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (map_attrs_get(unit, stage, map_type, &map_attrs));

    if (map_attrs.hash_lookup == FALSE) {
       SHR_IF_ERR_VERBOSE_EXIT
           (map_add_to_array(unit,
                             stage,
                             map_segment,
                             src_id[0],
                             ent_or_grp_id));
    } else {
       SHR_IF_ERR_VERBOSE_EXIT
           (map_add_to_hash(unit,
                            stage,
                            map_segment,
                            map_size,
                            map_type,
                            &map_attrs,
                            src_id,
                            ent_or_grp_id));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_idp_map_delete(int unit,
                     bcmfp_stage_id_t stage_id,
                     void *map_segment,
                     uint32_t map_size,
                     bcmfp_idp_map_type_t map_type,
                     uint32_t *src_id,
                     uint32_t ent_or_grp_id)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_idp_map_attrs_t map_attrs;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    sal_memset(&map_attrs, 0, sizeof(bcmfp_idp_map_attrs_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (map_attrs_get(unit, stage, map_type, &map_attrs));

    if (map_attrs.hash_lookup == FALSE) {
       SHR_IF_ERR_VERBOSE_EXIT
           (map_delete_from_array(unit,
                                  stage,
                                  map_segment,
                                  src_id[0],
                                  ent_or_grp_id));
    } else {
       SHR_IF_ERR_VERBOSE_EXIT
           (map_delete_from_hash(unit,
                                 stage,
                                 map_segment,
                                 map_size,
                                 map_type,
                                 &map_attrs,
                                 src_id,
                                 ent_or_grp_id));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_idp_map_traverse_first(int unit,
                       bcmfp_stage_id_t stage_id,
                       void *map_segment,
                       uint32_t map_size,
                       bcmfp_idp_map_type_t map_type,
                       uint32_t *src_id,
                       bcmfp_entry_id_traverse_cb cb,
                       void *user_data)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_idp_map_attrs_t map_attrs;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(src_id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    sal_memset(&map_attrs, 0, sizeof(bcmfp_idp_map_attrs_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (map_attrs_get(unit, stage, map_type, &map_attrs));

    if (map_attrs.hash_lookup == FALSE) {
       SHR_IF_ERR_VERBOSE_EXIT
           (map_traverse_first_in_array(unit,
                                        stage,
                                        map_segment,
                                        src_id[0],
                                        cb,
                                        user_data));
    } else {
       SHR_IF_ERR_VERBOSE_EXIT
           (map_traverse_first_in_hash(unit,
                                       stage,
                                       map_segment,
                                       map_size,
                                       map_type,
                                       &map_attrs,
                                       src_id,
                                       cb,
                                       user_data));
    }
exit:
    SHR_FUNC_EXIT();
}
