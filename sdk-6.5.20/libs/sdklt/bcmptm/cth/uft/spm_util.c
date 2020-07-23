/*! \file spm_util.c
 *
 * Strength Profile manager utility functions.
 *
 * These functions provide support for the Strength Profile
 * CTHs for user modification of strength profiles to tables that
 * are reconfigured via tile-mode changes.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <sal/sal_assert.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_table_constants.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmlrd_ptrm.h>
#include <bcmptm/bcmptm_spm_internal.h>
#include <bcmptm/bcmptm_uft.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>

#include "sbr_device.h"
#include "sbr_internal.h"
#include "sbr_util.h"
#include "spm_util.h"

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_UFT

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Copy a uint64_t LT field value into a PT entry buffer.
 *
 * Copy the specified LT field value into the provided PT entry
 * buffer at the location and field size specified.
 *
 * \param [in] field_desc bcmlrd_hmf_field_desc_t with minbit, maxbit.
 * \param [in] lt_field API field structure containing field value.
 * \param [out] pt_entry Pointer to the buffer for this PT entry.
 *
 * \retval None.
 */
static void
spm_lt_field_value_into_pt(const bcmlrd_hmf_field_desc_t *field_desc,
                           bcmltd_field_t *lt_field,
                           uint32_t *pt_entry)
{
    uint32_t src_field[2];

    shr_uint64_to_uint32_array(lt_field->data, src_field);

    bcmdrd_field_set(pt_entry,
                     field_desc->minbit, field_desc->maxbit,
                     src_field);
}

/*!
 * \brief Copy a fixed PT field value into a PT entry buffer.
 *
 * Copy the specified fixed PT field value into the provided PT entry
 * buffer at the location and field size specified.
 *
 * \param [in] fixed_field bcmlrd_hmf_fixed_mapping_t with
 *                         minbit, maxbit, and value..
 * \param [out] pt_entry Pointer to the buffer for this PT entry.
 *
 * \retval None.
 */
static void
spm_fixed_field_value_into_pt(const bcmlrd_hmf_fixed_mapping_t *fixed_field,
                              uint32_t *pt_entry)
{

    uint32_t value;

    value = fixed_field->value;
    bcmdrd_field_set(pt_entry,
                     fixed_field->physical.minbit,
                     fixed_field->physical.maxbit,
                     &value);
}

/*!
 * \brief Retrieve the resolved field structure from op working info.
 *
 * Fetch the field structure for the given LT value field ID from the
 * combined field structure list for this op.
 *
 * NB:  All fields in this implementation are expected to be single index.
 *
 * \param [in] spm_op_info Working context for this op.
 * \param [in] field_id Field identifier value.
 *
 * \retval Field structure pointer.
 */
static bcmltd_field_t *
spm_field_get(bcmptm_spm_op_info_t *spm_op_info,
              uint32_t field_id)
{
    uint32_t fix;

    for (fix = 0; fix < spm_op_info->api_field_used; fix++) {
        if (spm_op_info->api_fields[fix].id == field_id) {
            return &(spm_op_info->api_fields[fix]);
        }
    }

    /* This should never be reached. */
    return NULL;
}

/*!
 * \brief Transfer an API field value to a physical entry location.
 *
 * Using the bcmlrd_hmf_field_mapping_t, fetch the field value for the
 * indicated LT value field ID from the combined field value list for
 * this op then copy it into the current PT entry.
 *
 * NB:  All fields in this implementation are expected to be single index.
 *
 * \param [in] field_map
 * \param [in] spm_op_info Working context for this op.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL No matching field value.
 */
static void
spm_field_value_copy(const bcmlrd_hmf_field_mapping_t *field_map,
                     bcmptm_spm_op_info_t *spm_op_info)
{
    bcmltd_field_t *lt_field;
    uint32_t *pt_entry;

    pt_entry = spm_op_info->spm_pt_entries[spm_op_info->spm_pt_used - 1].entry_data;

    lt_field = spm_field_get(spm_op_info, field_map->logical.field_id);
    spm_lt_field_value_into_pt(&(field_map->physical), lt_field, pt_entry);
}

/*!
 * \brief Transfer a fixed field value to a physical entry location.
 *
 * Using the bcmlrd_hmf_fixed_mapping_t, insert the fixed value for the
 * into the current PT entry.
 *
 * NB:  All fields in this implementation are expected to be single index.
 *
 * \param [in] fixed_map
 * \param [in] spm_op_info Working context for this op.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL No matching field value.
 */
static void
spm_fixed_field_copy(const bcmlrd_hmf_fixed_mapping_t *fixed_map,
                     bcmptm_spm_op_info_t *spm_op_info)
{
    uint32_t *pt_entry;

    pt_entry =
        spm_op_info->spm_pt_entries[spm_op_info->spm_pt_used - 1].entry_data;

    spm_fixed_field_value_into_pt(fixed_map, pt_entry);
}

/*!
 * \brief Search a field linked list.
 *
 * Traverse a field structure linked list to locate the field
 * matching the given API field ID and index.  Return the field
 * value structure pointer if found, NULL if not.
 *
 * \param [in] in_fields Linked list of input API fields.
 * \param [in] field_id Field identifier value.
 * \param [in] field_idx The field array index for this field ID to
 *             match in the field value list.
 *
 * \retval Not-NULL Matching field value structure.
 * \retval NULL No matching field value found.
 */
static const bcmltd_field_t *
spm_api_find_field(const bcmltd_field_t *in_fields,
                   uint32_t field_id,
                   uint32_t field_idx)
{
    const bcmltd_field_t *flist = in_fields;

    while (flist != NULL) {
        if ((flist->id == field_id) &&
            (flist->idx == field_idx)) {
            return flist;
        }
        flist = flist->next;
    }

    return NULL;
}

/*!
 * \brief Search a Logical Table Adapter I/O structure.
 *
 * Traverse a LTA I/O structure to locate the field
 * matching the given API field ID and index.  Return the field
 * value structure pointer if found, NULL if not.
 *
 * \param [in] lta_fields LTA fields input/output structure pointer
 * \param [in] field_id Field identifier value.
 * \param [in] field_idx The field array index for this field ID to
 *             match in the field value list.
 *
 * \retval Not-NULL Matching field value structure.
 * \retval NULL No matching field value found.
 */
static bcmltd_field_t *
spm_lta_find_field(const bcmltd_fields_t *lta_fields,
                      uint32_t field_id,
                      uint32_t field_idx)
{
    int fix, max_fields;
    bcmltd_field_t **lta_fields_ptrs;

    if (lta_fields == NULL) {
        return NULL;
    }

    lta_fields_ptrs = lta_fields->field;
    max_fields = lta_fields->count;

    for (fix = 0; fix < max_fields; fix++) {
        if ((lta_fields_ptrs[fix]->id == field_id) &&
            (lta_fields_ptrs[fix]->idx == field_idx)) {
            return lta_fields_ptrs[fix];
        }
    }

    return NULL;
}

/*!
 * \brief Retrieve the low-level L2P map for this PT entry.
 *
 * For a PT entry structure, the core L2P map is several layers of
 * structures deep.  This functions verifies certain requirements
 * of the structure nesting and returns the core L2P map. *
 *
 * \param [in] unit Unit number.
 * \param [in] hw_entry PT entry L2P map structure pointer.
 * \param [in] hmf_instance_p bcmlrd_hmf_instance_t description of PT entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL L2P structures violate SBR assumptions.
 */
static int
spm_lt_str_to_instance(int unit,
                       const bcmptm_sbr_lt_str_pt_info_t *lt_str_ptr,
                       const bcmlrd_hmf_instance_t **hmf_instance_p)
{
    const bcmptm_sbr_hw_entry_info_t *hw_entry;
    const bcmlrd_hmf_t *hmf_map;
    const bcmlrd_hmf_view_t *hmf_view;
    const bcmlrd_hmf_instance_t *hmf_instance;

    SHR_FUNC_ENTER(unit);

    hw_entry = lt_str_ptr->hw_entry_info;
    SHR_NULL_CHECK(hw_entry, SHR_E_INTERNAL);

    hmf_map = hw_entry->l2p_map_info.map;
    if ((hmf_map == NULL) || (hmf_map->view_count != 1)) {
        /* SBR PT entry maps must be single view. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    hmf_view = hmf_map->view;
    if ((hmf_view == NULL) || (hmf_view->instance_count != 1)) {
        /* SBR PT entry maps must be single instance. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    hmf_instance = hmf_view->instance[0];
    /* SBR PT entry map must exist. */
    SHR_NULL_CHECK(hmf_instance, SHR_E_INTERNAL);

    *hmf_instance_p = hmf_instance;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine the correct value of an API field.
 *
 * Using provided input API value (if any), retrieve IMM record (if any),
 * and default value (from LRD), determine the field value to
 * write to HW during this operation.
 *
 * NB:  All fields in this implementation are expected to be single index.
 *
 * \param [in] unit Unit number.
 * \param [in] lta_api_fields LTA fields structure holding API input.
 * \param [in] lta_cur_fields LTA fields structure holding IMM record.
 * \param [in] ltid Logical table identifier value.
 * \param [in] field_id Field identifier value.
 * \param [out] field_ptr Field structure to update with resolved value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Invalid field value.
 */
static int
spm_resolve_api_field(int unit,
                      const bcmltd_field_t *api_fields,
                      const bcmltd_fields_t *lta_cur_fields,
                      uint32_t ltid,
                      uint32_t field_id,
                      bcmltd_field_t *field_ptr)
{
    uint32_t num_actual;
    uint64_t def_val;
    const bcmltd_field_t *found_field;
    bool unset = FALSE;

    SHR_FUNC_ENTER(unit);

    /* First look in API list */
    found_field = spm_api_find_field(api_fields, field_id, 0);
    if (found_field != NULL) {
        sal_memcpy(field_ptr, found_field, sizeof(bcmltd_field_t));
        if ((field_ptr->flags & SHR_FMM_FIELD_DEL) != 0) {
            /* Need default value to overwrite */
            unset = TRUE;
        } else {
            /* Done */
            SHR_EXIT();
        }
    }

    /* Next look in current IMM list */
    if (!unset) {
        found_field = spm_lta_find_field(lta_cur_fields, field_id, 0);
        if (found_field != NULL) {
            sal_memcpy(field_ptr, found_field, sizeof(bcmltd_field_t));
            SHR_EXIT();
        }
    }

    /* Use default value */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmlrd_field_default_get(unit, ltid, field_id,
                                 1, &def_val, &num_actual));

    field_ptr->id = field_id;
    field_ptr->idx = 0;
    field_ptr->data = def_val;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Modify a PT entry per bcmlrd_hmf_memory_t description.
 *
 * Using the bcmlrd_hmf_memory_t structure description of API field
 * to PT field maps, plus fixed field maps, update the current PT entry.
 *
 * The PT ID & index for the current PT record must be set before
 * calling this function.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table identifier value.
 * \param [in] op_arg Operation arguments.
 * \param [in] memory_map bcmlrd_hmf_memory_t description of PT entry.
 * \param [in] spm_op_info Working context for this op.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Invalid field value.
 */
static int
spm_pt_entry_modify(int unit,
                    bcmltd_sid_t ltid,
                    const bcmltd_op_arg_t *op_arg,
                    const bcmlrd_hmf_memory_t *memory_map,
                    bcmptm_spm_op_info_t *spm_op_info)
{
    uint32_t fix, pt_index;
    bcmdrd_sid_t pt_id;
    bcmptm_spm_pt_entry_t *pt_entry;
    uint32_t *entry_buf;

    SHR_FUNC_ENTER(unit);

    /* Get current pt_entry */
    pt_entry = &(spm_op_info->spm_pt_entries[spm_op_info->spm_pt_used - 1]);
    entry_buf = pt_entry->entry_data;
    pt_index = pt_entry->index;
    pt_id = pt_entry->pt_id;

    SHR_IF_ERR_EXIT
        (bcmptm_sbr_lt_hw_read(unit, FALSE, ltid, op_arg,
                               pt_id, pt_index,
                               BCMDRD_MAX_PT_WSIZE,
                               entry_buf));

    pt_entry->entry_data_valid = TRUE;

    if (memory_map->value != NULL) {
        for (fix = 0; fix < memory_map->value_count; fix++) {
            spm_field_value_copy(&(memory_map->value[fix]), spm_op_info);
            pt_entry->entry_data_changed = TRUE;
        }
    }

    if (memory_map->fixed != NULL) {
        for (fix = 0; fix < memory_map->fixed_count; fix++) {
            spm_fixed_field_copy(&(memory_map->fixed[fix]), spm_op_info);
            pt_entry->entry_data_changed = TRUE;
        }
    }

    if (pt_entry->entry_data_changed) {
        SHR_IF_ERR_EXIT
            (bcmptm_sbr_lt_hw_write(unit, FALSE, ltid, op_arg,
                                    pt_id, pt_index,
                                    entry_buf));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if a particular SP PT is currently selected for a SP LT
 *        by the active tile modes.
 *
 * Provided the SP PT metadata of a SP LT's mapping, determine if
 * the current tile mode configurations select this SP PT.
 *
 * \param [in] unit Unit number.
 * \param [in] sbr_ptid Strength profile physical table identifier value.
 * \param [in] metadata SP PT properties of this SP LT's map.
 * \param [out] is_mapped Return pointer.
 *                        If TRUE, ptid is currently in use for this LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Invalid field value.
 */
static int
spm_pt_is_mapped(int unit,
                 bcmltd_sid_t sbr_ptid,
                 const bcmlrd_hmf_memory_metadata_t *metadata,
                 bool *is_mapped)
{
    uint32_t tmix;
    const bcmlrd_hmf_tile_mode_t *tmi;
    uint32_t tile_mode, seed;
    bool robust;

    SHR_FUNC_ENTER(unit);

    *is_mapped = TRUE;
    if ((metadata->tile_mode_list_count == 0) ||
        (metadata->tile_mode_list == NULL)) {
        /* No Tile mode info for this SPLT */
        *is_mapped = TRUE;
        SHR_EXIT();
    }

    for (tmix = 0; tmix < metadata->tile_mode_list_count; tmix++) {
        tmi = &(metadata->tile_mode_list[tmix]);

        SHR_IF_ERR_EXIT
            (bcmptm_uft_tile_cfg_get(unit, tmi->tile,
                                     &tile_mode, &robust, &seed));

        if (tmi->tile_mode == tile_mode) {
            *is_mapped = TRUE;
            SHR_EXIT();
        }
    }

    /* No matching SBR PT found */
    *is_mapped = FALSE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if a particular SPLT has any active mappings to a SP LT.
 *
 * From the given SPLT's field structure, determine if any
 * current tile mode configuration selects a SP PT to which
 * the SPLT is mapped.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_fields Pointer to bcmptm_spm_lt_fields_t structure.
 * \param [out] is_mapped Return pointer.
 *                        If TRUE, SPLT is mapped to at least one SP LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Invalid SPLT information.
 */
static int
spm_lt_is_mapped(int unit,
                 bcmptm_spm_lt_fields_t *lt_fields,
                 bool *is_mapped)
{
    const bcmlrd_hmf_instance_t *hmf_instance;
    const bcmlrd_hmf_memory_t *memory_map;
    uint32_t hwe_ix;
    bool pt_mapped;

    SHR_FUNC_ENTER(unit);

    *is_mapped = FALSE;

    hmf_instance = lt_fields->hmf_instance;

    for (hwe_ix = 0; hwe_ix < hmf_instance->memory_count; hwe_ix++) {
        memory_map = hmf_instance->memory[hwe_ix];

        /* Only test SBR PTs for tiles currently mapped for LOOKUP LT */
        pt_mapped = FALSE;
        SHR_IF_ERR_EXIT
            (spm_pt_is_mapped(unit, memory_map->ptsid,
                              &(memory_map->metadata),
                              &pt_mapped));
        if (pt_mapped) {
            *is_mapped = TRUE;
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

void
bcmptm_spm_op_info_reset(int unit,
                         bcmptm_spm_lt_record_t *spm_lt_record,
                         bcmptm_spm_op_info_t *spm_op_info)
{
    uint32_t clear_size;

    spm_op_info->spm_pt_used = 0;
    spm_op_info->api_field_used = 0;

    /* Clear PT entry structures */
    clear_size = sizeof(bcmptm_spm_pt_entry_t) * spm_lt_record->spm_pt_num;
    sal_memset(spm_op_info->spm_pt_entries, 0, clear_size);

    /* Clear API value field space */
    clear_size = (sizeof(shr_fmm_t) * spm_lt_record->api_field_num);
    sal_memset(spm_op_info->api_fields, 0, clear_size);

    /* Clear LTA read space */
    clear_size = sizeof(bcmltd_fields_t);
    sal_memset(&(spm_op_info->imm_lta_out_fields), 0, clear_size);

    /* Clear LTA structure */
    clear_size = (sizeof(bcmltd_field_t *) * spm_lt_record->api_field_num) +
        (sizeof(bcmltd_field_t) * spm_lt_record->api_field_num);
    sal_memset(spm_op_info->imm_lta_fields_space, 0, clear_size);
}

void
bcmptm_spm_lta_output_init(int unit,
                           bcmptm_spm_lt_record_t *spm_lt_record,
                           bcmptm_spm_op_info_t *spm_op_info)
{
    uint32_t fix, field_count, max_fields;
    bcmltd_field_t **lta_fields_ptrs, *cur_lta_field, *lta_field_base;
    bcmltd_fields_t *lta_fields;
    uint8_t *ptr_math;

    lta_fields = &(spm_op_info->imm_lta_out_fields);
    lta_fields_ptrs = spm_op_info->imm_lta_fields_space;
    ptr_math = (uint8_t *)lta_fields_ptrs +
        (sizeof(bcmltd_field_t *) * spm_lt_record->api_field_num);
    lta_field_base = (bcmltd_field_t *)ptr_math;

    lta_fields->count = 0;
    lta_fields->field = lta_fields_ptrs;
    field_count = 0;

    max_fields = spm_lt_record->api_field_num;

    for (fix = 0; fix < max_fields; fix++) {
        cur_lta_field = &(lta_field_base[fix]);
        sal_memset(cur_lta_field, 0,
                   sizeof(bcmltd_field_t));

        lta_fields_ptrs[field_count] = cur_lta_field;
        field_count++;
    }
    lta_fields->count = field_count;
}

int
bcmptm_spm_lt_field_records_init(int unit,
                                 bcmltd_sid_t ltid,
                                 const bcmlrd_map_t *map,
                                 const bcmptm_sbr_lt_str_pt_info_t *lt_str_ptr,
                                 bcmptm_spm_lt_fields_t *lt_fields)
{
    const bcmlrd_table_rep_t *tbl = NULL;
    uint32_t value_fid_count = 0;
    uint32_t fid;
    uint32_t alloc_size;

    SHR_FUNC_ENTER(unit);

    /* Get LRD information */
    tbl = bcmlrd_table_get(ltid);
    SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

    lt_fields->ltid = ltid;
    lt_fields->ltid_lookup = lt_str_ptr->ltid_lookup;

    /*
     * The logical field IDs are always [0..(max_num_fields-1)]
     * by design, where max_num_fields is the total number of
     * fields defined in a table.
     */
    for (fid = 0; fid < tbl->fields; fid++) {
        if (bcmlrd_map_field_is_unmapped(map, fid)) {
            /* Skip unmapped fields */
            continue;
        }
        if (tbl->field[fid].flags & BCMLRD_FIELD_F_KEY) {  /* Key field */
            lt_fields->key_num++;
            lt_fields->key_fid = fid;
        } else {
            lt_fields->value_num++;
        }
    }

    if (lt_fields->key_num > 1) {
        /* Too many keys. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Space for LT value fields IDs. */
    alloc_size = sizeof(uint32_t) * lt_fields->value_num;
    if (alloc_size > 0) {
        SHR_ALLOC(lt_fields->value_fids, alloc_size, "bcmptmSpmLtValueIds");
        SHR_NULL_CHECK(lt_fields->value_fids, SHR_E_MEMORY);
        sal_memset(lt_fields->value_fids, 0, alloc_size);
        /* On error, this is freed in bcmptm_spm_cleanup */

        for (fid = 0; fid < tbl->fields; fid++) {
            if (bcmlrd_map_field_is_unmapped(map, fid)) {
                /* Skip unmapped fields */
                continue;
            }
            if (tbl->field[fid].flags & BCMLRD_FIELD_F_KEY) {  /* Key field */
                continue;
            }

            /* Mapped value field */
            lt_fields->value_fids[value_fid_count++] = fid;
        }
    }

    /* Cache L2P maps for this LT. */
    SHR_IF_ERR_EXIT
        (spm_lt_str_to_instance(unit, lt_str_ptr,
                                &(lt_fields->hmf_instance)));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_spm_field_write_list_create(int unit,
                                   bcmptm_spm_op_info_t *spm_op_info,
                                   bcmptm_spm_lt_fields_t *lt_fields,
                                   const bcmltd_field_t *api_fields,
                                   const bcmltd_fields_t *lta_cur_fields)
{
    uint32_t fix;

    SHR_FUNC_ENTER(unit);

    for (fix = 0; fix < lt_fields->value_num; fix++) {
        SHR_IF_ERR_EXIT
            (spm_resolve_api_field(unit, api_fields,
                                   lta_cur_fields,
                                   lt_fields->ltid,
                                   lt_fields->value_fids[fix],
                                   &(spm_op_info->api_fields[fix])));
    }
    spm_op_info->api_field_used = lt_fields->value_num;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_spm_pt_size_init(int unit,
                        bcmptm_spm_lt_fields_t *lt_fields)
{
    const bcmlrd_hmf_instance_t *hmf_instance;
    const bcmlrd_hmf_memory_t *memory_map;
    uint32_t hwe_ix;
    uint32_t tbl_size, pt_offset;

    SHR_FUNC_ENTER(unit);

    hmf_instance = lt_fields->hmf_instance;

    lt_fields->pt_size_1 = BCMPTM_SPM_INDEX_INVALID;
    lt_fields->pt_size_2 = BCMPTM_SPM_INDEX_INVALID;
    lt_fields->pt_offset = BCMPTM_SPM_INDEX_INVALID;

    for (hwe_ix = 0; hwe_ix < hmf_instance->memory_count; hwe_ix++) {
        memory_map = hmf_instance->memory[hwe_ix];

        if (memory_map->metadata.flags & BCMLRD_HMF_MEMORY_F_OFFSET) {
            pt_offset = memory_map->metadata.offset;

            if (pt_offset != 0) {
                if (lt_fields->pt_offset == BCMPTM_SPM_INDEX_INVALID) {
                    lt_fields->pt_offset = pt_offset;
                } else if (lt_fields->pt_offset != pt_offset) {
                    /* Multiple offsets in device. */
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
        }

        tbl_size = bcmdrd_pt_index_max(unit, memory_map->ptsid)
            - bcmdrd_pt_index_min(unit, memory_map->ptsid) + 1;

        if (lt_fields->pt_size_1 == BCMPTM_SPM_INDEX_INVALID) {
            /* Record first table size */
            lt_fields->pt_size_1 = tbl_size;
        } else if (lt_fields->pt_size_1 != tbl_size) {
            if (lt_fields->pt_size_2 == BCMPTM_SPM_INDEX_INVALID) {
                /* Record first table size */
                lt_fields->pt_size_2 = tbl_size;
            } else if (lt_fields->pt_size_2 != tbl_size) {
                /* Too many table sizes. */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
     }

    /* Sanity checks for PT table sizes */
    if (lt_fields->pt_size_1 == BCMPTM_SPM_INDEX_INVALID) {
        /* No PT table sizes. */
        /* LTs mapped, but no PTs? */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    } else if (lt_fields->pt_size_2 != BCMPTM_SPM_INDEX_INVALID) {
        /*
         * Two PT sizes detected.
         * LT index size should be based on smaller PT size.
         */
        if (lt_fields->pt_size_1 ==
            (2 * lt_fields->pt_size_2)) {
            lt_fields->pt_index_max = lt_fields->pt_size_2;
        } else if (lt_fields->pt_size_2 ==
                   (2 * lt_fields->pt_size_1)) {
            lt_fields->pt_index_max = lt_fields->pt_size_1;
        } else {
            /* Don't know how to allocate LT index */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        if (lt_fields->pt_offset != BCMPTM_SPM_INDEX_INVALID) {
            if (lt_fields->pt_index_max != lt_fields->pt_offset) {
                /* Don't understand offset calculations. */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
    } else {
        /* Only one size */
        if (lt_fields->pt_offset == BCMPTM_SPM_INDEX_INVALID) {
            /* No offset, one entry per table */
            lt_fields->pt_index_max = lt_fields->pt_size_1;
        } else {
            if (lt_fields->pt_size_1 != (2 * lt_fields->pt_offset)) {
                /* Don't know how to allocate LT index */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            } else {
                lt_fields->pt_index_max = lt_fields->pt_offset;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_spm_pt_index_dynamic_update(int unit,
                                   bcmptm_spm_op_info_t *spm_op_info,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmptm_spm_lt_fields_t *lt_fields)
{
    int rv;
    bcmptm_sbr_lt_lookup_t lookup_type;
    const bcmlrd_hmf_instance_t *hmf_instance;
    const bcmlrd_hmf_memory_t *memory_map;
    uint32_t hwe_ix;
    uint16_t pt_index;
    bcmptm_spm_pt_entry_t *pt_entry;
    bool is_mapped = FALSE;

    SHR_FUNC_ENTER(unit);

    hmf_instance = lt_fields->hmf_instance;

    for (hwe_ix = 0; hwe_ix < hmf_instance->memory_count; hwe_ix++) {
        memory_map = hmf_instance->memory[hwe_ix];
        if (memory_map->metadata.flags & BCMLRD_HMF_MEMORY_F_LOOKUP0) {
            lookup_type = BCMPTM_SBR_LOOKUP0;
        } else if (memory_map->metadata.flags & BCMLRD_HMF_MEMORY_F_LOOKUP1) {
            lookup_type = BCMPTM_SBR_LOOKUP1;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* Only update SBR PTs for tiles currently mapped for LOOKUP LT */
        SHR_IF_ERR_EXIT
            (spm_pt_is_mapped(unit, memory_map->ptsid,
                              &(memory_map->metadata),
                              &is_mapped));
        if (!is_mapped) {
            continue;
        }

        rv = bcmptm_sbr_entry_index_get(unit, memory_map->ptsid,
                                        lt_fields->ltid_lookup,
                                        lookup_type,
                                        &pt_index);

        if (rv == SHR_E_NOT_FOUND) {
            /* Lookup table not mapped in this tile configuration.  Skip. */
            continue;
        } else {
            /* There should be no other error types from this function. */
            SHR_IF_ERR_EXIT(rv);
        }

        /* Update next PT entry */
        pt_entry = &(spm_op_info->spm_pt_entries[spm_op_info->spm_pt_used]);
        spm_op_info->spm_pt_used++;
        pt_entry->pt_id = memory_map->ptsid;
        pt_entry->index = pt_index;

        SHR_IF_ERR_EXIT
            (spm_pt_entry_modify(unit, lt_fields->ltid,
                                 op_arg, memory_map, spm_op_info));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_spm_user_lt_index_test(int unit,
                              bcmptm_spm_lt_fields_t *lt_fields,
                              uint16_t test_lt_index,
                              bool *idx_free)
{
    const bcmlrd_hmf_instance_t *hmf_instance;
    const bcmlrd_hmf_memory_t *memory_map;
    uint32_t hwe_ix;
    uint16_t pt_index;
    bcmptm_sbr_entry_state_t *entry_state = NULL;
    bcmptm_sbr_pt_state_t *spt_state = NULL;
    bool is_mapped, lt_mapped;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (spm_lt_is_mapped(unit, lt_fields, &lt_mapped));

    if (!lt_mapped) {
        /*
         * No SP PTs are mapped by this SPLT.
`        * It is likely that the LOOKUP table is inactive.
         */
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    hmf_instance = lt_fields->hmf_instance;

    for (hwe_ix = 0; hwe_ix < hmf_instance->memory_count; hwe_ix++) {
        memory_map = hmf_instance->memory[hwe_ix];

        /* Only test SBR PTs for tiles currently mapped for LOOKUP LT */
        SHR_IF_ERR_EXIT
            (spm_pt_is_mapped(unit, memory_map->ptsid,
                              &(memory_map->metadata),
                              &is_mapped));
        if (!is_mapped) {
            continue;
        }

        pt_index = test_lt_index;

        if (memory_map->metadata.flags & BCMLRD_HMF_MEMORY_F_OFFSET) {
            pt_index += memory_map->metadata.offset;
        }

        SHR_IF_ERR_EXIT
            (bcmptm_sbr_entry_state_get(unit, memory_map->ptsid,
                                        pt_index, &entry_state, &spt_state));

        if (entry_state->usage_mode != BCMPTM_SBR_ENTRY_FREE) {
            /* This index is not available for use. */
            *idx_free = FALSE;
            SHR_EXIT();
        }
    }

    *idx_free = TRUE;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_spm_user_lt_index_state(int unit,
                               bcmptm_spm_lt_record_t *lt_record,
                               bcmptm_spm_lt_fields_t *lt_fields,
                               uint16_t test_lt_index,
                               bcmptm_spm_entry_state_t *idx_state)
{
    const bcmlrd_hmf_instance_t *hmf_instance;
    const bcmlrd_hmf_memory_t *memory_map;
    uint32_t hwe_ix;
    uint16_t pt_index;
    bcmptm_sbr_entry_state_t *entry_state = NULL;
    bcmptm_sbr_pt_state_t *spt_state = NULL;
    bcmptm_spm_lt_fields_t *test_lt_fields;
    bool inuse = FALSE, allocated_other = FALSE;
    bool is_mapped, lt_mapped;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (spm_lt_is_mapped(unit, lt_fields, &lt_mapped));

    if (!lt_mapped) {
        /*
         * No SP PTs are mapped by this SPLT.
`        * It is likely that the LOOKUP table is inactive.
         */
        *idx_state = BCMPTM_SPM_ENTRY_UNMAPPED;
        SHR_EXIT();
    }

    hmf_instance = lt_fields->hmf_instance;

    for (hwe_ix = 0; hwe_ix < hmf_instance->memory_count; hwe_ix++) {
        memory_map = hmf_instance->memory[hwe_ix];

        /* Only test SBR PTs for tiles currently mapped for LOOKUP LT */
        SHR_IF_ERR_EXIT
            (spm_pt_is_mapped(unit, memory_map->ptsid,
                              &(memory_map->metadata),
                              &is_mapped));
        if (!is_mapped) {
            continue;
        }

        pt_index = test_lt_index;

        if (memory_map->metadata.flags & BCMLRD_HMF_MEMORY_F_OFFSET) {
            pt_index += memory_map->metadata.offset;
        }

        SHR_IF_ERR_EXIT
            (bcmptm_sbr_entry_state_get(unit, memory_map->ptsid,
                                        pt_index, &entry_state, &spt_state));

        if (entry_state->usage_mode == BCMPTM_SBR_ENTRY_IN_USE) {
            /* Entry allocated, but by which LT? */
            if (entry_state->ltid == lt_fields->ltid) {
                /*
                 * Index is used by this SPLT, validated upon INSERT.
                 * No need to continue checking.
                 */
                *idx_state = BCMPTM_SPM_ENTRY_ALLOCATED_SELF;
                SHR_EXIT();
            } else {
                /* Allocated, but by SPM or SBR-RM? */
                test_lt_fields =
                    bcmptm_spm_lt_fields_find(lt_record, entry_state->ltid);
                if (test_lt_fields == NULL) {
                    /* Not SPLT. */
                    inuse = TRUE;
                } else if (test_lt_fields->key_num == 0) {
                    /* Single-entry SPLT, allocated by SBR-RM per tile mode. */
                    inuse = TRUE;
                } else {
                    /* Multi-entry SPLT, allocated by SPM. */
                   allocated_other = TRUE;
                }
            }
        } else if (entry_state->usage_mode != BCMPTM_SBR_ENTRY_FREE) {
            /* This index is reserved by compiler. */
            *idx_state = BCMPTM_SPM_ENTRY_RESERVED;
            SHR_EXIT();
        }
    }

    if (inuse) {
        *idx_state = BCMPTM_SPM_ENTRY_INUSE;
    } else if (allocated_other) {
        *idx_state = BCMPTM_SPM_ENTRY_ALLOCATED_OTHER;
    } else {
        *idx_state = BCMPTM_SPM_ENTRY_FREE;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_spm_user_lt_entry_count(int unit,
                               bcmptm_spm_lt_fields_t *lt_fields)
{
    const bcmlrd_hmf_instance_t *hmf_instance;
    const bcmlrd_hmf_memory_t *memory_map;
    uint32_t hwe_ix;
    uint16_t pt_index;
    bcmptm_sbr_entry_state_t *entry_state = NULL;
    bcmptm_sbr_pt_state_t *spt_state = NULL;
    uint16_t lt_index;
    uint32_t index_count = 0;
    bool idx_free;

    SHR_FUNC_ENTER(unit);

    hmf_instance = lt_fields->hmf_instance;

    for (lt_index = 0; lt_index < lt_fields->lt_index_max; lt_index++) {
        idx_free = TRUE;
        for (hwe_ix = 0; hwe_ix < hmf_instance->memory_count; hwe_ix++) {
            memory_map = hmf_instance->memory[hwe_ix];

            pt_index = lt_index;

            if (memory_map->metadata.flags & BCMLRD_HMF_MEMORY_F_OFFSET) {
                pt_index += memory_map->metadata.offset;
            }

            SHR_IF_ERR_EXIT
                (bcmptm_sbr_entry_state_get(unit, memory_map->ptsid,
                                            pt_index, &entry_state,
                                            &spt_state));

            if ((entry_state->usage_mode != BCMPTM_SBR_ENTRY_FREE) &&
                (entry_state->usage_mode != BCMPTM_SBR_ENTRY_IN_USE)) {
                /* This index is not available for use. */
                idx_free = FALSE;
                break;
            }
        }

        if (idx_free) {
            index_count++;
        }
    }

    lt_fields->lt_entry_limit = index_count;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_spm_user_lt_index_search(int unit,
                                bcmptm_spm_lt_fields_t *lt_fields,
                                uint16_t *lt_index)
{
    uint16_t lt_size;
    uint16_t lt_ix = 0;
    bool idx_free = FALSE;

    SHR_FUNC_ENTER(unit);

    lt_size = lt_fields->lt_index_max;

    for (lt_ix = 0; lt_ix < lt_size; lt_ix++) {
        /* Check each PT SBR-RM state to see if this index
         * value is free for this LT.
         */
        SHR_IF_ERR_EXIT
            (bcmptm_spm_user_lt_index_test(unit,
                                           lt_fields, lt_ix, &idx_free));
        if (idx_free) {
            break;
        }
    }

    if (idx_free) {
        if (lt_ix < lt_size) {
            /* Found valid index */
            *lt_index = lt_ix;
        } else {
            /* Inconsistent state */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_spm_lt_index_user_update(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmptm_spm_op_info_t *spm_op_info,
                                bcmptm_spm_lt_fields_t *lt_fields,
                                uint16_t lt_index,
                                bcmimm_entry_event_t event)
{
    bcmptm_sbr_lt_lookup_t lookup_type;
    const bcmlrd_hmf_instance_t *hmf_instance;
    const bcmlrd_hmf_memory_t *memory_map;
    uint32_t hwe_ix;
    uint32_t pt_write_index;
    bcmptm_spm_pt_entry_t *pt_entry;
    bcmptm_sbr_entry_state_t *entry_state = NULL;
    bcmptm_sbr_pt_state_t *spt_state = NULL;
    bool is_mapped;

    SHR_FUNC_ENTER(unit);

    hmf_instance = lt_fields->hmf_instance;

    for (hwe_ix = 0; hwe_ix < hmf_instance->memory_count; hwe_ix++) {
        memory_map = hmf_instance->memory[hwe_ix];

        /* Only update SBR PTs for tiles currently mapped for LOOKUP LT */
        SHR_IF_ERR_EXIT
            (spm_pt_is_mapped(unit, memory_map->ptsid,
                              &(memory_map->metadata),
                              &is_mapped));
        if (!is_mapped) {
            continue;
        }

        if (memory_map->metadata.flags & BCMLRD_HMF_MEMORY_F_LOOKUP0) {
            lookup_type = BCMPTM_SBR_LOOKUP0;
        } else if (memory_map->metadata.flags & BCMLRD_HMF_MEMORY_F_LOOKUP1) {
            lookup_type = BCMPTM_SBR_LOOKUP1;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        pt_write_index = lt_index;

        if (memory_map->metadata.flags & BCMLRD_HMF_MEMORY_F_OFFSET) {
            pt_write_index += memory_map->metadata.offset;
        }

        SHR_IF_ERR_EXIT
            (bcmptm_sbr_entry_state_get(unit, memory_map->ptsid,
                                        pt_write_index, &entry_state,
                                        &spt_state));

        switch (event) {
        case BCMIMM_ENTRY_INSERT:
            if (entry_state->usage_mode == BCMPTM_SBR_ENTRY_IN_USE) {
                /*
                 * The main and ext SBR profile tables share the entry_state.
                 * So, skip entry_state and spt_state updates for ext_drd_sid.
                 */
                break;
            }
            /* Index state was already checked. */
            entry_state->usage_mode = BCMPTM_SBR_ENTRY_IN_USE;
            entry_state->ltid = lt_fields->ltid;
            entry_state->lookup_type = lookup_type;
            entry_state->ref_count++;
            spt_state->free_entries_count--;
            break;
        case BCMIMM_ENTRY_DELETE:
            if (entry_state->usage_mode == BCMPTM_SBR_ENTRY_FREE) {
                /*
                 * The main and ext SBR profile tables share the entry_state.
                 * So, skip entry_state and spt_state updates for ext_drd_sid.
                 */
                break;
            }
            /* IMM validated this index. */
            if (entry_state->ltid != lt_fields->ltid) {
                /* Data records are inconsistent. */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            if (entry_state->lookup_type != lookup_type) {
                /* Data records are inconsistent. */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            entry_state->usage_mode = BCMPTM_SBR_ENTRY_FREE;
            entry_state->ltid = BCMLTD_INVALID_LT;
            entry_state->lookup_type = BCMPTM_SBR_LOOKUP_INVALID;
            entry_state->ref_count--;
            spt_state->free_entries_count++;
            break;
        case BCMIMM_ENTRY_UPDATE:
            /* IMM validated this index. */
            if (entry_state->ltid != lt_fields->ltid) {
                /* Data records are inconsistent. */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            if (entry_state->lookup_type != lookup_type) {
                /* Data records are inconsistent. */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
        }

        /* Update next PT entry */
        pt_entry = &(spm_op_info->spm_pt_entries[spm_op_info->spm_pt_used]);
        spm_op_info->spm_pt_used++;
        pt_entry->pt_id = memory_map->ptsid;
        pt_entry->index = pt_write_index;

        SHR_IF_ERR_EXIT
            (spm_pt_entry_modify(unit, lt_fields->ltid, op_arg,
                                 memory_map, spm_op_info));
    }

exit:
    SHR_FUNC_EXIT();
}

bcmptm_spm_lt_fields_t *
bcmptm_spm_lt_fields_find(bcmptm_spm_lt_record_t *lt_record,
                          uint32_t ltid)
{
    uint32_t ltix;

    for (ltix = 0; ltix < lt_record->lt_num; ltix++) {
        if (lt_record->lt_fields_info[ltix].ltid == ltid) {
            return &(lt_record->lt_fields_info[ltix]);
        }
    }

    return NULL;
}

bcmptm_spm_lt_resource_info_t *
bcmptm_spm_lt_resource_find(bcmptm_spm_resource_root_t *resource_root,
                            uint32_t ltid)
{
    uint32_t ltix;

    for (ltix = 0; ltix < resource_root->resource_lt_count; ltix++) {
        if (resource_root->resource_lt[ltix].ltid == ltid) {
            return &(resource_root->resource_lt[ltix]);
        }
    }

    return NULL;
}
