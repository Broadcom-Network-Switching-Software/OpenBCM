/*! \file spm_imm.c
 *
 * Strenth Profile manager interface to in-memory table.
 *
 * These functions provide the IMM interface that implements a generic
 * CTH for user modification of strength profiles to tables that
 * are reconfigured via tile-mode changes.
 *
 * These LTs come in two forms:
 *
 * 1) User-controlled index
 *    These LTs are implemented as index with allocation.  The application
 *    may supply a key value to use as the PT index.  If the index
 *    is free on all PTs to which a SBR LT is mapped, the INSERT succeeds.
 *    Or the application may choose to let this implementation select
 *    an index that is free for all mapped PTs.
 *
 * 2) Dynamic SBR
 *    These LTs are no-key, single entry LTs.  The SBR-RM manages the
 *    indexes for such PT entries, based upon a (ltid, ptid) key.
 *    The PT index may be different for each mapped PT corresponding
 *    to the selected LT.
 *    Tile mode change configures a default setting for these strengths.
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
#include <bcmimm/bcmimm_basic.h>
#include <bcmptm/bcmlrd_ptrm.h>
#include <bcmptm/bcmptm_spm_internal.h>

#include "sbr_device.h"
#include "sbr_internal.h"
#include "sbr_util.h"
#include "spm_util.h"

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_UFT

static bcmptm_spm_op_info_t spm_op_info_all[BCMDRD_CONFIG_MAX_UNITS];
static bcmptm_spm_lt_record_t *spm_lt_records_all[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief SPM logical table IMM validate callback function.
 *
 * Validate the field values of a strength profile logical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * new/modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Unsupported opcode.
  */
static int
spm_imm_validate(int unit,
                 bcmltd_sid_t sid,
                 bcmimm_entry_event_t event,
                 const bcmltd_field_t *key,
                 const bcmltd_field_t *data,
                 void *context)
{
    SHR_FUNC_ENTER(unit);

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
            
            break;
        case BCMIMM_ENTRY_DELETE:
        case BCMIMM_ENTRY_LOOKUP:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief SPM logical table IMM change callback function for staging.
 *
 * Handle SPM logical table IMM change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
spm_imm_stage(int unit,
              bcmltd_sid_t sid,
              const bcmltd_op_arg_t *op_arg,
              bcmimm_entry_event_t event,
              const bcmltd_field_t *key,
              const bcmltd_field_t *data,
              void *context,
              bcmltd_fields_t *output_fields)
{
    bcmltd_field_t *lta_out_field;
    bcmptm_spm_op_info_t *spm_op_info = NULL;
    bcmptm_spm_lt_record_t *lt_record = NULL;
    uint16_t user_index = 0;
    uint32_t key_field_id = 0;
    bcmptm_spm_lt_fields_t *lt_fields;
    bcmltd_field_t key_field;
    bool dynamic_sbr = FALSE;
    bool idx_free = FALSE;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    lt_record = spm_lt_records_all[unit];
    SHR_NULL_CHECK(lt_record, SHR_E_INTERNAL);

    lt_fields = bcmptm_spm_lt_fields_find(lt_record, sid);
    SHR_NULL_CHECK(lt_fields, SHR_E_INTERNAL);

    /* Prepare working space for this op. */
    spm_op_info = &(spm_op_info_all[unit]);
    bcmptm_spm_op_info_reset(unit, lt_record, spm_op_info);
    spm_op_info->lt_fields = lt_fields;

    if (lt_fields->key_num == 0) {
        dynamic_sbr = TRUE;
    } else {
        /* Otherwise multi-entry user mode */
        key_field_id = lt_fields->key_fid;
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            /* Add default fields to list. */
            SHR_IF_ERR_EXIT
                (bcmptm_spm_field_write_list_create(unit,
                                                    spm_op_info, lt_fields,
                                                    data, NULL));

            if (dynamic_sbr) {
                /* Simply write fields as appropriate */
                SHR_IF_ERR_EXIT
                    (bcmptm_spm_pt_index_dynamic_update(unit, spm_op_info,
                                                        op_arg, lt_fields));
                break;
            }

            /* Otherwise, user mode multi-entry with possible IwA */

            if (key != NULL) {
                if (key->next != NULL) {
                    /* Single key should be only field here */
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                if (key->id != key_field_id) {
                    /* Wrong ID */
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                user_index = key->data;
                if (user_index >= lt_fields->lt_index_max) {
                    /* Too big for PTs. */
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                SHR_IF_ERR_EXIT
                    (bcmptm_spm_user_lt_index_test(unit, lt_fields,
                                                   user_index, &idx_free));

                if (!idx_free) {
                    /* User-provided index is not available. */
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }
            } else {
                /* Attempt to allocate index */
                SHR_IF_ERR_EXIT
                    (bcmptm_spm_user_lt_index_search(unit, lt_fields,
                                                     &user_index));

                /* Record index for output to IMM */
                lta_out_field = output_fields->field[0];
                lta_out_field->data = user_index;

                lta_out_field->id = key_field_id;
                lta_out_field->idx = 0;
                output_fields->count = 1;
            }

            /*
             * Write input or default values to all SBR PTs.
             * Update state to IN_USE for this index on all SBR PTs.
             */
            SHR_IF_ERR_EXIT
                (bcmptm_spm_lt_index_user_update(unit, op_arg, spm_op_info,
                                                 lt_fields, user_index,
                                                 event));
            break;
        case BCMIMM_ENTRY_DELETE:
            /* Create default field list. */
            SHR_IF_ERR_EXIT
                (bcmptm_spm_field_write_list_create(unit, spm_op_info,
                                                    lt_fields,
                                                    NULL, NULL));
            if (dynamic_sbr) {
                /* Simply write fields as appropriate */
                SHR_IF_ERR_EXIT
                    (bcmptm_spm_pt_index_dynamic_update(unit, spm_op_info,
                                                        op_arg, lt_fields));
                break;
            }

            /* Otherwise, user mode multi-entry. */
            /* Validate key input */
            if ((key == NULL) ||
                (key->next != NULL) ||
                (key->id != key_field_id)) {
                /* Inconsistent key info */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            } else {
                user_index = key->data;
            }

            /*
             * Write default values.
             * Update state to FREE for this index on all SBR PTs.
             */
            SHR_IF_ERR_EXIT
                (bcmptm_spm_lt_index_user_update(unit, op_arg, spm_op_info,
                                                 lt_fields, user_index,
                                                 event));
            break;
        case BCMIMM_ENTRY_UPDATE:
            /* Prepare LTA output list for IMM LOOKUP */
            bcmptm_spm_lta_output_init(unit, lt_record, spm_op_info);

            /* Perform IMM lookup */
            if (dynamic_sbr) {
                SHR_IF_ERR_EXIT
                    (bcmimm_entry_lookup(unit, sid,
                                         &(spm_op_info->imm_lta_no_keys),
                                         &(spm_op_info->imm_lta_out_fields)));
            } else {
                sal_memcpy(&key_field, key, sizeof(bcmltd_field_t));
                spm_op_info->key_field_ptr = &key_field;
                SHR_IF_ERR_EXIT
                    (bcmimm_entry_lookup(unit, sid,
                                         &(spm_op_info->imm_lta_key_field),
                                         &(spm_op_info->imm_lta_out_fields)));
            }

            /* Merge field lists. */
            SHR_IF_ERR_EXIT
                (bcmptm_spm_field_write_list_create(unit, spm_op_info,
                                                    lt_fields, data,
                                      &(spm_op_info->imm_lta_out_fields)));

            if (dynamic_sbr) {
                /* Simply write fields as appropriate */
                SHR_IF_ERR_EXIT
                    (bcmptm_spm_pt_index_dynamic_update(unit, spm_op_info,
                                                        op_arg, lt_fields));
                break;
            }

            /* Otherwise, user mode multi-entry. */
            /* Validate key input */
            if ((key == NULL) ||
                (key->next != NULL) ||
                (key->id != key_field_id)) {
                /* Inconsistent key info */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            } else {
                user_index = key->data;
            }

            /* Write data to all SBR PTs. */
            SHR_IF_ERR_EXIT
                (bcmptm_spm_lt_index_user_update(unit, op_arg, spm_op_info,
                                                 lt_fields, user_index,
                                                 event));
            break;
        case BCMIMM_ENTRY_LOOKUP:
            /* N/A because this is handled by IMM */
            SHR_EXIT();
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief SPM logical table IMM commit callback function.
 *
 * Commit the changes to SBR-RM due to SPM op.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id The transaction ID.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
spm_imm_commit(int unit,
               bcmltd_sid_t sid,
               uint32_t trans_id,
               void *context)
{
    SHR_FUNC_ENTER(unit);

    /* Forward commit to SBR-RM to lock entry state changes. */
    SHR_IF_ERR_EXIT
        (bcmptm_sbr_tile_mode_commit(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief SPM logical table IMM abort callback function.
 *
 * Abort the changes to SBR-RM due to SPM op.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id The transaction ID.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval None.
 */
static void
spm_imm_abort(int unit,
              bcmltd_sid_t sid,
              uint32_t trans_id,
              void *context)
{
    /* Forward abort to SBR-RM to discard entry state changes. */
    (void) bcmptm_sbr_tile_mode_abort(unit);
}

/*!
 * \brief SPM logical table IMM entry limit callback function.
 *
 * Retrieve the entry limit for this SPM logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id The transaction ID.
 * \param [in] sid This is the logical table ID.
 * \param [out] count The returned value of entry limit.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
spm_imm_entry_limit_get(int unit,
                        uint32_t trans_id,
                        bcmltd_sid_t sid,
                        uint32_t *count)
{
    bcmptm_spm_lt_record_t *lt_record = NULL;
    bcmptm_spm_lt_fields_t *lt_fields;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(spm_lt_records_all[unit], SHR_E_INTERNAL);

    lt_record = spm_lt_records_all[unit];
    SHR_NULL_CHECK(lt_record, SHR_E_INTERNAL);

    lt_fields = bcmptm_spm_lt_fields_find(lt_record, sid);
    SHR_NULL_CHECK(lt_fields, SHR_E_INTERNAL);

    *count = lt_fields->lt_entry_limit;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Strength Profile LT In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to *_STR[EGTH]_PROFILE logical table entry commit stages.
 */
static bcmimm_lt_cb_t spm_imm_callback = {

    /*! Validate function. */
    .validate = spm_imm_validate,

    /*! Extended staging function. */
    .op_stage = spm_imm_stage,

    /*! Commit function. */
    .commit   = spm_imm_commit,

    /*! Abort function. */
    .abort    = spm_imm_abort,

    /*! Entry limit function. */
    .ent_limit_get    = spm_imm_entry_limit_get
};



/*******************************************************************************
 * Public Functions
 */

int
bcmptm_spm_init(int unit, bool warm)
{
    const bcmlrd_map_t *map = NULL;
    int rv;
    uint32_t ix;
    uint32_t lt_max;
    bcmltd_sid_t sid;
    bcmptm_spm_op_info_t *spm_op_info;
    const bcmptm_sbr_var_drv_t *sbr_var_info;
    const bcmptm_sbr_lt_str_pt_list_t *lt_str_pt_list;
    const bcmptm_sbr_lt_str_pt_info_t *lt_str_ptr;
    bcmptm_spm_lt_record_t *spm_lt_record = NULL;
    bcmptm_spm_lt_fields_t *spm_lt_fields;
    uint32_t spm_lt_count;
    uint32_t alloc_size;

    SHR_FUNC_ENTER(unit);

    if (spm_lt_records_all[unit] != NULL) {
        /* Reinitializing without shutdown */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    spm_op_info = &(spm_op_info_all[unit]);

    SHR_IF_ERR_EXIT
        (bcmptm_sbr_var_info_get(unit, &sbr_var_info));
    if (sbr_var_info == NULL) {
        SHR_EXIT();
    }
    lt_str_pt_list = sbr_var_info->sbr_list;
    if (lt_str_pt_list == NULL) {
        SHR_EXIT();
    }

    /*
     * Register these LTs with IMM.
     */

    spm_lt_count = lt_str_pt_list->lt_str_pt_info_count;

    if (spm_lt_count == 0) {
        /* No LTs mapped, nothing to do. */
        SHR_EXIT();
    }

    /* Space for LT records. */
    alloc_size = sizeof(bcmptm_spm_lt_record_t) +
        sizeof(bcmptm_spm_lt_fields_t) * spm_lt_count;
    SHR_ALLOC(spm_lt_record, alloc_size, "bcmptmSpmLtRecord");
    SHR_NULL_CHECK(spm_lt_record, SHR_E_MEMORY);
    sal_memset(spm_lt_record, 0, alloc_size);

    spm_lt_records_all[unit] = spm_lt_record;
    spm_lt_record->lt_num = spm_lt_count;
    spm_lt_record->spm_pt_num = 0;
    spm_lt_record->api_field_num = 0;

    for (ix = 0; ix < spm_lt_count; ix++) {
        lt_str_ptr = &(lt_str_pt_list->lt_str_pt_info[ix]);
        sid = lt_str_ptr->ltid_str_pt;

        rv = bcmlrd_map_get(unit, sid, &map);
        if (SHR_SUCCESS(rv) && map) {
            spm_lt_fields = &(spm_lt_record->lt_fields_info[ix]);
            SHR_IF_ERR_EXIT
                (bcmptm_spm_lt_field_records_init(unit, sid, map,
                                                  lt_str_ptr, spm_lt_fields));
            if (spm_lt_fields->value_num > spm_lt_record->api_field_num) {
                spm_lt_record->api_field_num = spm_lt_fields->value_num;
            }

            if (spm_lt_fields->hmf_instance->memory_count == 0) {
                /* No PTs mapped */
                continue;
            }

            SHR_IF_ERR_EXIT
                (bcmptm_spm_pt_size_init(unit, spm_lt_fields));

            /* Find maximum PT entries for supported LTs */
            if (spm_lt_fields->hmf_instance->memory_count >
                spm_lt_record->spm_pt_num) {
                spm_lt_record->spm_pt_num =
                    spm_lt_fields->hmf_instance->memory_count;
            }

            rv = bcmlrd_map_table_attr_get(unit, sid,
                                  BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT,
                                           &lt_max);

            if (SHR_SUCCESS(rv) &&
                (lt_max < spm_lt_fields->pt_index_max)) {
                /* Constrain LT to declared LT size, if known */
                spm_lt_fields->lt_index_max = lt_max;
            } else {
                /* Otherwise use PT size */
                spm_lt_fields->lt_index_max = spm_lt_fields->pt_index_max;
            }

            /* Now that index_maximum is known, determine entry limit */
            SHR_IF_ERR_EXIT
                (bcmptm_spm_user_lt_entry_count(unit, spm_lt_fields));

            SHR_IF_ERR_EXIT
                (bcmimm_lt_event_reg(unit,
                                     sid,
                                     &spm_imm_callback,
                                     NULL));
        }
    }

    /* Space for tracking and recording modified PT entries */
    alloc_size = sizeof(bcmptm_spm_pt_entry_t) * spm_lt_record->spm_pt_num;
    SHR_ALLOC(spm_op_info->spm_pt_entries, alloc_size, "bcmptmSpmPtEntries");
    SHR_NULL_CHECK(spm_op_info->spm_pt_entries, SHR_E_MEMORY);
    sal_memset(spm_op_info->spm_pt_entries, 0, alloc_size);

    /* Space for recording merged logical field values */
    alloc_size = sizeof(bcmltd_field_t) *  spm_lt_record->api_field_num;
    SHR_ALLOC(spm_op_info->api_fields, alloc_size, "bcmptmSpmApiFields");
    SHR_NULL_CHECK(spm_op_info->api_fields, SHR_E_MEMORY);
    sal_memset(spm_op_info->api_fields, 0, alloc_size);

    /*
     * Space for LTA output structure to retrieve logical field values
     * from IMM on UPDATE.
     */
    alloc_size = (sizeof(bcmltd_field_t *) * spm_lt_record->api_field_num) +
        (sizeof(bcmltd_field_t) * spm_lt_record->api_field_num);
    SHR_ALLOC(spm_op_info->imm_lta_fields_space, alloc_size,
              "bcmptmSpmLtaFields");
    SHR_NULL_CHECK(spm_op_info->imm_lta_fields_space, SHR_E_MEMORY);
    sal_memset(spm_op_info->imm_lta_fields_space, 0, alloc_size);

    /* Set up no-key field LTA */
    spm_op_info->imm_lta_no_keys.count = 0;
    spm_op_info->imm_lta_no_keys.field = NULL;

    /* Set up key field LTA */
    spm_op_info->imm_lta_key_field.count = 1;
    spm_op_info->imm_lta_key_field.field = &(spm_op_info->key_field_ptr);

    /* Last step is to mark info structure valid. */
    spm_lt_record->lt_str_pt_list = lt_str_pt_list;

    /* Now use the valid structure to init resource info LTs */
    SHR_IF_ERR_EXIT
        (bcmptm_spm_resource_info_init(unit, warm, spm_lt_record));

exit:
    if (SHR_FUNC_ERR()) {
        bcmptm_spm_cleanup(unit, warm);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_spm_cleanup(int unit, bool warm)
{
    uint32_t ix;
    bcmptm_spm_op_info_t *spm_op_info;
    uint32_t spm_lt_count;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_spm_resource_info_cleanup(unit, warm));

    spm_op_info = &(spm_op_info_all[unit]);

    SHR_FREE(spm_op_info->imm_lta_fields_space);
    SHR_FREE(spm_op_info->api_fields);
    SHR_FREE(spm_op_info->spm_pt_entries);

    if ((spm_lt_records_all[unit] != NULL) &&
        (spm_lt_records_all[unit]->lt_str_pt_list != NULL)) {
        spm_lt_count =
            spm_lt_records_all[unit]->lt_str_pt_list->lt_str_pt_info_count;
        for (ix = 0; ix < spm_lt_count; ix++) {
            SHR_FREE(spm_lt_records_all[unit]->lt_fields_info[ix].value_fids);
        }
        SHR_FREE(spm_lt_records_all[unit]);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_spm_tile_mode_change_blocked(int unit,
                                    uint32_t tile,
                                    uint32_t current_tile_mode,
                                    uint32_t new_tile_mode,
                                    bool *blocked)
{
    uint32_t ltix, hwe_ix, tmix;
    bcmptm_spm_lt_record_t *lt_record = NULL;
    bcmptm_spm_lt_fields_t *lt_fields;
    bcmltd_table_entry_inuse_t table_data;
    const bcmlrd_hmf_instance_t *hmf_instance;
    const bcmlrd_hmf_memory_metadata_t *metadata;
    const bcmlrd_hmf_tile_mode_t *tmi;

    SHR_FUNC_ENTER(unit);

    lt_record = spm_lt_records_all[unit];

    for (ltix = 0; ltix < lt_record->lt_num; ltix++) {
        lt_fields = &(lt_record->lt_fields_info[ltix]);

        /* Is SPLT empty? */
        SHR_IF_ERR_EXIT
            (bcmimm_basic_table_entry_inuse_get(unit,
                                                BCMPTM_SPM_INDEX_INVALID,
                                                lt_fields->ltid, NULL,
                                                &table_data, NULL));

        /* If not empty, check if any SPPTs are connected to this tile. */
        if (table_data.entry_inuse != 0) {
            /* Get SPPT map for this SPLT */
            hmf_instance = lt_fields->hmf_instance;

            /* For each SPPT in the map */
            for (hwe_ix = 0; hwe_ix < hmf_instance->memory_count; hwe_ix++) {
                metadata = &(hmf_instance->memory[hwe_ix]->metadata);

                /* For each (tile, tile_mode) in the list for this SPPT */
                for (tmix = 0; tmix < metadata->tile_mode_list_count; tmix++) {
                    tmi = &(metadata->tile_mode_list[tmix]);
                    if (tmi->tile == tile) {
                        if ((tmi->tile_mode == current_tile_mode) ||
                            (tmi->tile_mode == new_tile_mode)) {
                            /*
                             * This SPLT either is or will be connected
                             * to the indicated tile,
                             * tile mode change not permitted.
                             */
                            *blocked = TRUE;
                            SHR_EXIT();
                        } /* Else, this SPLT not involved in either tile mode. */
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}
