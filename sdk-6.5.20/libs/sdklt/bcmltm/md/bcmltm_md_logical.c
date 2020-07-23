/*! \file bcmltm_md_logical.c
 *
 * Logical Table Manager for Logical Table Metadata.
 *
 * The tables in this category are those defined through
 * various methods to produce a mapping from the API fields to
 * either the physical resources on the device or an internal
 * database.
 *
 * The Logical Table metadata uses the symbol IDs from the
 * LRD component, Logical Resource Database.  These symbol IDs
 * are the identification for all the defined logical tables.
 *
 * The LRD symbol IDs are defined in a compact numbering
 * space [0.. n-1], where 'n' is the total number of LRD symbols.
 *
 * The design of this component is based on the LRD symbol ID attribute
 * define above.   The symbol ID is used as the array index into
 * several of the table metadata array data structure.
 *
 * The LRD symbol ID value is used as the LTM table ID.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>

#include <bsl/bsl.h>

#include <bcmbd/bcmbd.h>

#include <bcmptm/bcmptm.h>

#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/chip/bcmlrd_limits.h>

#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_ha_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>
#include <bcmltm/bcmltm_fa_util_internal.h>
#include <bcmltm/bcmltm_pt.h>
#include <bcmltm/bcmltm_md_logical_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>
#include <bcmltm/bcmltm_db_internal.h>
#include <bcmltm/bcmltm_db_table_internal.h>
#include <bcmltm/bcmltm_dump.h>

#include "bcmltm_md_lt_drv.h"
#include "bcmltm_md_op.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA


/*
 * Accessor macros
 */
#define LOGICAL_LTM_MD(_u)               logical_ltm_md[_u]
#define LOGICAL_SID_MAX_COUNT(_u)        LOGICAL_LTM_MD(_u)->lt_max
#define LOGICAL_HA_SIZES(_u)             ha_state_sizes[_u]


/*
 * Helper macros
 */
/* Check that LTM has been initialized */
#define LOGICAL_INIT(_u)                        \
    (LOGICAL_LTM_MD(_u) != NULL)

/* Check that symbol id is valid */
#define LOGICAL_SID_VALID(_u, _sid)             \
    ((_sid) < LOGICAL_SID_MAX_COUNT(_u))



/*!
 * \brief Per-Key field value boundary information.
 *
 * This structure contains the minimum and maximum values for
 * Index LT Key fields, as initialized.

 */
typedef struct {
    /*! API-facing field identifier. */
    uint32_t api_field_id;

    /*! Currently configured minimum value for this Index LT Key field. */
    uint64_t current_key_min;

    /*! Currently configured maximum value for this Index LT Key field. */
    uint64_t current_key_max;
} bcmltm_key_bound_info_t;

/*!
 * \brief Index LT Key field value boundary information.
 *
 * This structure contains the Index LT Key fields' boundaries,
 * as initialized. These boundaries are used to determine the
 * valid bitmap of Index with Allocation LTs, and to verify the API
 * value of Index LT Key fields for all operations.
 */
typedef struct {
    /*! API-facing field identifier. */
    uint32_t num_keys;

    /*!
     * An array of size num_keys containing the boundary
     * information for each API key field of this Index LT.
     */
    bcmltm_key_bound_info_t key_bounds[];
} bcmltm_index_key_bounds_t;

/*!
 * \brief Valid bitmap information.
 *
 * This structure contains all of the elements necessary to
 * calculate a valid bitmap based on the API key bounds.
 * This structure is passed along the recursive loops over
 * all API key fields.
 */
typedef struct bcmltm_valid_bitmap_info_s {
    /*! The locally created LT entry containing API key fields. */
    bcmltm_entry_t lt_entry;

    /*! Temporary storage for the constructed valid bitmap. */
    SHR_BITDCL *valid_bitmap;

    /*! Working buffer offset to Tracking Index. */
    uint32_t track_offset;

    /*! Current API key bounds. */
    bcmltm_index_key_bounds_t *index_key_bounds;

    /*! Pointer to the metadata for this LT. */
    bcmltm_lt_md_t *lt_md;

    /*! Pointer to the state for this LT. */
    bcmltm_lt_state_t *lt_state;

    /*! Pointer to the working buffer. */
    uint32_t *ltm_buffer;

    /*! Number of function nodes corresponding to
     *  FA root pass 0 of this LT.
     *  These are the function nodes that analyze key fields for validity.
     */
    uint32_t lookup_key_nodes_count;

    /*! Pointer LOOKUP op nodes 0 of this LT. */
    bcmltm_op_node_t **lookup_nodes_array;
} bcmltm_valid_bitmap_info_t;


/* LTM LT Metadata */
static bcmltm_md_t    *logical_ltm_md[BCMLTM_MAX_UNITS];

/* LTM HA state sizes */
static bcmltm_ha_state_sizes_t ha_state_sizes[BCMLTM_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Initialize the key boundary information for a logical table.
 *
 * This routine initializes the key boundary information with
 * the key fields minimum and maximum limits as specified
 * in table map file for the given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] key_bounds Key boundary information to initialize.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_key_bounds_init(int unit, bcmlrd_sid_t sid,
                        bcmltm_index_key_bounds_t *key_bounds)
{
    bcmltm_key_bound_info_t *key_info;
    uint32_t num_fid;
    uint32_t num_fid_idx;
    uint32_t fid;
    uint32_t fid_count;
    uint32_t idx_count;
    unsigned int size;
    bcmltm_fid_idx_t *fid_idx = NULL;
    uint64_t minimum_value;
    uint64_t maximum_value;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    key_bounds->num_keys = 0;

    /* Get count of key field IDs and field elements (fid,idx) */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_key_count_get(unit, sid, &num_fid, &num_fid_idx));
    if (num_fid_idx == 0) {
        SHR_EXIT();
    }

    /* Allocate field id/idx array */
    size = sizeof(*fid_idx) * num_fid;
    SHR_ALLOC(fid_idx, size, "bcmltmKeyFidIdxArr");
    SHR_NULL_CHECK(fid_idx, SHR_E_MEMORY);
    sal_memset(fid_idx, 0, size);

    /* Get list of key field IDs and index count */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_key_list_get(unit, sid,
                                      num_fid, fid_idx, &fid_count));

    /* Populate key bounds information */
    key_info = key_bounds->key_bounds;
    for (i = 0; i < fid_count; i++) {
        fid = fid_idx[i].fid;
        idx_count = fid_idx[i].idx_count;

        /*
         * Sanity check: index count must be 1 for mapped key fields.
         *
         * Key fields for Index LTs can only be scalars (wide fields
         * and field arrays are not allowed).
         *
         * This limitation is due to the following:
         * - PT keys in Index LTs must fit within a 32-bit value.
         *   (this is the size allocated in the PTM WB for keys).
         *   Since logical key fields may be directly mapped to PT keys,
         *   this size restriction reduces the chances of a logical
         *   key being too large to fit in the PTM WB.
         * - The notion of a key field array does not fit the definition
         *   of an array.  An array implies that elements can be absent
         *   during input.  However, key fields are mandatory (with the
         *   exception of IwA LTs).
         */
        if (idx_count != 1) {
            const char *table_name;
            const char *field_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM: Invalid index count for key field "
                                  "%s(ltid=%d) "
                                  "%s(fid=%d) expected=1 actual=%d\n"),
                       table_name, sid, field_name, fid, idx_count));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* Check space */
        if (i >= num_fid_idx) {
            const char *table_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM: Current field count exceeds "
                                  "max count %s(ltid=%d) "
                                  "count=%d max_count=%d\n"),
                       table_name, sid, (i+1), num_fid_idx));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_EXIT
            (bcmltm_db_field_scalar_values_get(unit, sid, fid,
                                               &minimum_value,
                                               &maximum_value,
                                               NULL));

        key_info[i].api_field_id = fid;
        key_info[i].current_key_min = minimum_value;
        key_info[i].current_key_max = maximum_value;
    }

    /* Set number of key elements populated */
    key_bounds->num_keys = i;

 exit:
    SHR_FREE(fid_idx);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy key bounds information.
 *
 * This routine destroys the given key bounds information.
 *
 * \param [in] key_bounds Pointer to info to destroy.
 */
static void
logical_key_bounds_destroy(bcmltm_index_key_bounds_t *key_bounds)
{
    if (key_bounds == NULL) {
        return;
    }

    SHR_FREE(key_bounds);

    return;
}

/*!
 * \brief Create the field key bounds information for given table.
 *
 * This routine creates the field key bounds information for
 * the given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [out] key_bounds_ptr Returning key bounds information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_key_bounds_create(int unit,
                          bcmlrd_sid_t sid,
                          bcmltm_index_key_bounds_t **key_bounds_ptr)
{
    bcmltm_index_key_bounds_t *key_bounds = NULL;
    uint32_t num_fid;
    uint32_t num_fid_idx;
    unsigned int alloc_size;

    SHR_FUNC_ENTER(unit);

    *key_bounds_ptr = NULL;

    /* Get count of key field IDs and field elements (fid,idx) */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_key_count_get(unit, sid, &num_fid, &num_fid_idx));

    /* Allocate */
    alloc_size = sizeof(bcmltm_index_key_bounds_t) +
        (sizeof(bcmltm_key_bound_info_t) * num_fid_idx);
    SHR_ALLOC(key_bounds, alloc_size, "bcmltmIndexKeyBounds");
    SHR_NULL_CHECK(key_bounds, SHR_E_MEMORY);
    sal_memset(key_bounds, 0, alloc_size);

    /* Initialize information */
    SHR_IF_ERR_EXIT
        (logical_key_bounds_init(unit, sid, key_bounds));

    *key_bounds_ptr = key_bounds;

 exit:
    if (SHR_FUNC_ERR()) {
        logical_key_bounds_destroy(key_bounds);
        *key_bounds_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a valid bitmap of track indexes.
 *
 * This routine calculates a valid bitmap of track indexes.
 *
 * Using the currently configured index key field bounds, iterate
 * through every valid key field combination to create the valid
 * bitmap of tracking indexes.
 *
 * \param [in] unit Unit number.
 * \param [in] depth Recursion depth.
 * \param [in,out] vbi Pointer to table valid bitmap information.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
valid_track_index_bitmap_recurse(int unit, uint32_t depth,
                                 bcmltm_valid_bitmap_info_t *vbi)
{
    uint64_t key_value;
    uint32_t track_index, trix;
    bcmltm_key_bound_info_t *cur_key_bound;
    bcmltm_field_list_t *cur_api_field;
    bool recurse_end = FALSE;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Check for LT with no keys */
    if (vbi->index_key_bounds->num_keys == 0) {
        /*
         * LT without keys uses an internal track index of zero
         * to keep track of that one logical entry.
         */
        SHR_BITSET(vbi->valid_bitmap, 0);
        SHR_EXIT();
    }

    if ((depth + 1) == vbi->index_key_bounds->num_keys) {
        recurse_end = TRUE;
    }

    cur_key_bound = &(vbi->index_key_bounds->key_bounds[depth]);
    cur_api_field = bcmltm_api_find_field(vbi->lt_entry.in_fields,
                                          cur_key_bound->api_field_id,
                                          0);
    if (cur_api_field == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (key_value = cur_key_bound->current_key_min;
         key_value <= cur_key_bound->current_key_max;
         key_value++) {
        cur_api_field->data = key_value;
        if (recurse_end) {
            /*
             * Use only the LOOKUP FA pass 0 nodes of this LT to convert
             * API key values to Track Index.
             */
            for (trix = 0; trix < vbi->lookup_key_nodes_count; trix++) {
                /* Execute node functions */
                if ((vbi->lookup_nodes_array[trix] != NULL) &&
                    (vbi->lookup_nodes_array[trix]->node_function != NULL)){
                    rv = (vbi->lookup_nodes_array[trix]->node_function)(unit,
                                            vbi->lt_md,
                                            vbi->lt_state,
                                            &(vbi->lt_entry),
                                            vbi->ltm_buffer,
                               vbi->lookup_nodes_array[trix]->node_cookie);
                }
                if (rv == SHR_E_PARAM) {
                    /* Invalid key set, skip */
                    continue;
                }
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }

            /* Mark Tracking Index valid in bitmap. */
            track_index = vbi->ltm_buffer[vbi->track_offset];
            SHR_BITSET(vbi->valid_bitmap, track_index);
        } else {
            /* Continue recursion */
            SHR_IF_ERR_EXIT
                (valid_track_index_bitmap_recurse(unit, depth + 1, vbi));
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the inuse bitmap state data for given table.
 *
 * This routine initializes the inuse bitmap state data for
 * the given logical table as follows:
 * - If table prepopulate is set to true, then set all valid bits in
 *   inuse bitmap (i.e. mark all logical entries as inuse).
 * - Else, inuse bitmap should be set to zero (this function does
 *   not reset to zero since it assumes data is already clear).
 *
 * This should be called only during cold boot.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] lt_state LT state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_state_inuse_bitmap_init(int unit,
                                bcmlrd_sid_t sid,
                                bcmltm_lt_state_t *lt_state)
{
    int rv;
    uint32_t alloc_size;
    SHR_BITDCL *inuse_bitmap = NULL;
    SHR_BITDCL *valid_ti_bitmap = NULL;
    bool prepopulate = FALSE;
    int entry_count;

    SHR_FUNC_ENTER(unit);

    /* Get in-use bitmap */
    rv = bcmltm_state_data_get(unit, lt_state,
                               BCMLTM_STATE_TYPE_INUSE_BITMAP,
                               (void *) &inuse_bitmap);
    if (rv == SHR_E_NOT_FOUND) {
        /* Bitmap is not applicable to table, just return */
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        /* Failure (except for not found, handled earlier) */
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM initialization: "
                              "could not get InUse Bitmap state data"
                              "%s(ltid=%d) rv=%d\n"),
                   table_name, sid, rv));
        SHR_ERR_EXIT(rv);
    }

    /* Get prepopulate attribute */
    SHR_IF_ERR_EXIT
        (bcmltm_db_table_prepopulate_get(unit, sid, &prepopulate));
    if (prepopulate == FALSE) {
        /*
         * Table prepopulate is false, nothing to do.
         * This assumes that the inuse bitmap state data
         * has been initialized to zero.
         */
        SHR_EXIT();
    }

    /*
     * Prepopulate table.
     * Mark all logical entries as inuse.
     */

    /* Create bitmap for valid track indexes */
    SHR_IF_ERR_EXIT
        (bcmltm_state_data_size_get(unit, lt_state,
                                    BCMLTM_STATE_TYPE_INUSE_BITMAP,
                                    &alloc_size));
    if (alloc_size == 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(valid_ti_bitmap, alloc_size,
              "bcmltmValidTrackIndexBitmap");
    SHR_NULL_CHECK(valid_ti_bitmap, SHR_E_MEMORY);
    sal_memset(valid_ti_bitmap, 0, alloc_size);

    /* Get the valid bitmap of track indexes */
    SHR_IF_ERR_EXIT
        (bcmltm_md_logical_valid_track_index_bitmap_get(unit, sid,
                                                        valid_ti_bitmap));

    /* Copy new bitmap over existing valid bitmap in LT state */
    SHR_BITCOPY_RANGE(inuse_bitmap, 0, valid_ti_bitmap,
                      0, lt_state->bitmap_size);

    /* Set entry count */
    SHR_BITCOUNT_RANGE(inuse_bitmap, entry_count, 0, lt_state->bitmap_size);
    lt_state->entry_count = entry_count;

 exit:
    /* Clean up all allocated memory */
    SHR_FREE(valid_ti_bitmap);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the valid bitmap state data for given table.
 *
 * This routine initializes the valid bitmap state data for
 * the given logical table.
 *
 * This should be called only during cold boot.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] lt_state LT state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_state_valid_bitmap_init(int unit,
                                bcmlrd_sid_t sid,
                                bcmltm_lt_state_t *lt_state)
{
    int rv;
    SHR_BITDCL *valid_bitmap = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get valid bitmap */
    rv = bcmltm_state_data_get(unit, lt_state,
                               BCMLTM_STATE_TYPE_VALID_BITMAP,
                               (void *) &valid_bitmap);
    if (rv == SHR_E_NOT_FOUND) {
        /* Bitmap is not applicable to table, just return */
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        /* Failure (except for not found, handled earlier) */
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM initialization: "
                              "could not get Valid Bitmap state data"
                              "%s(ltid=%d) rv=%d\n"),
                   table_name, sid, rv));
        SHR_ERR_EXIT(rv);
    }

    /* The valid bitmap exists for this LT */

    
    SHR_IF_ERR_EXIT
        (bcmltm_state_lt_valid_bitmap_update(unit, sid));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get size of a given state data type for a LT.
 *
 * This routine gets the size in bytes needed for the specified
 * state data type on a given logical table.  If the state data type is
 * not applicable or not needed for logical table, the returned size
 * is zero.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] table_attr Table attributes.
 * \param [in] data_type State data type.
 * \param [out] data_size Size, in bytes, for LT state data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_state_data_size_get(int unit, bcmlrd_sid_t sid,
                            const bcmltm_table_attr_t *table_attr,
                            bcmltm_state_type_t data_type,
                            uint32_t *data_size)
{
    bcmltm_table_map_type_t map_type = table_attr->map_type;
    bcmltm_table_type_t table_type = table_attr->type;
    uint32_t track_index_max;

    SHR_FUNC_ENTER(unit);

    *data_size = 0;

    switch(data_type) {
    case BCMLTM_STATE_TYPE_INUSE_BITMAP:
        /*
         * INUSE_BITMAP: needed Direct PT map - Index LT
         *
         * This bitmap tracks the inuse entries for an index logical table.
         * The bitmap needs to be large enough to set the
         * maximum track index value.
         */
        if (BCMLTM_TABLE_MAP_IS_DM(map_type) &&
            BCMLTM_TABLE_TYPE_IS_INDEX(table_type)) {
            SHR_IF_ERR_EXIT
                (bcmltm_db_dm_track_index_max_get(unit, sid,
                                                  &track_index_max));
            *data_size = SHR_BITALLOCSIZE(track_index_max + 1);
        }
        break;

    case BCMLTM_STATE_TYPE_VALID_BITMAP:
        /*
         * VALID_BITMAP: needed for Direct PT map - Index with Alloc LT
         *
         * This bitmap specifies the track index values that match
         * the legal combinations of the currently configured
         * API key field bounds.  It is used on IwA tables to allocate
         * a new valid tracking index.
         */
        if (BCMLTM_TABLE_MAP_IS_DM(map_type) &&
            BCMLTM_TABLE_TYPE_IS_ALLOC_INDEX(table_type)) {
            SHR_IF_ERR_EXIT
                (bcmltm_db_dm_track_index_max_get(unit, sid,
                                                  &track_index_max));
            *data_size = SHR_BITALLOCSIZE(track_index_max + 1);
        }
        break;

    case BCMLTM_STATE_TYPE_GLOBAL_BITMAP:
        /*
         * GLOBAL_BITMAP: needed for Direct PT map - Master Shared Index LT
         *
         * This global bitmap is maintained by the 'master' table
         * to keep track of the shared entries among overlay tables.
         */
        if (BCMLTM_TABLE_MAP_IS_DM(map_type) &&
            BCMLTM_TABLE_TYPE_IS_INDEX(table_type)) {
            /* Check for Shared 'master' */
            if ((table_attr->flag & BCMLTM_TABLE_FLAG_OVERLAY) &&
                (table_attr->global_ltid == sid)) {
                SHR_IF_ERR_EXIT
                    (bcmltm_db_dm_track_index_max_get(unit, sid,
                                                      &track_index_max));
                *data_size = SHR_BITALLOCSIZE(track_index_max + 1);
            }
        }
        break;

    default:
        break;
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Reset the state data for given logical table.
 *
 * This routine resets the various state data types for
 * the specified logical table to the initial state during cold boot.
 *
 * The assumption is that the state data has been initialized
 * to zero when this function is called.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Table ID.
 * \param [in] lt_state LT state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_state_data_reset(int unit, bcmlrd_sid_t sid,
                         bcmltm_lt_state_t *lt_state)
{
    SHR_FUNC_ENTER(unit);

    /*
     * Initialize the various state data types.
     *
     * The order of initialization of the different state data
     * is important since there are dependencies among some of them.
     */

    /*
     * State Data: BCMLTM_STATE_TYPE_GLOBAL_BITMAP
     *
     * The initial state should be zero during a cold boot.
     * Since the state data is reset to zero when this is added,
     * there is no need to take any actions.
     */

    /*
     * State Data: BCMLTM_STATE_TYPE_INUSE_BITMAP
     */
    SHR_IF_ERR_EXIT
        (logical_state_inuse_bitmap_init(unit, sid, lt_state));

    /*
     * State Data: BCMLTM_STATE_TYPE_VALID_BITMAP
     */
    SHR_IF_ERR_EXIT
        (logical_state_valid_bitmap_init(unit, sid, lt_state));

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Erase the given LT state metadata.
 *
 * Erase the given LT state metadata.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_state_hap HA Pointer to LT state data metadata to destroy.
 */
static void
logical_state_destroy(int unit, bcmltm_ha_ptr_t lt_state_hap)
{
    bcmltm_lt_state_t *state;
    bcmltm_ha_ptr_t cur_state_data_hap, next_state_data_hap, data_hap;
    bcmltm_lt_state_data_t *cur_state_data;
    void *data;

    state = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_hap);
    if (state == NULL) {
        return;
    }

    cur_state_data_hap = state->table_state_data_hap;

    while (!BCMLTM_HA_PTR_IS_INVALID(cur_state_data_hap)) {
        cur_state_data = bcmltm_ha_ptr_to_mem_ptr(unit, cur_state_data_hap);
        if (cur_state_data == NULL) {
            /* Inconsistent state */
            return;
        }

        next_state_data_hap = cur_state_data->next_state_data_hap;
        data_hap = cur_state_data->state_data_hap;

        data = bcmltm_ha_ptr_to_mem_ptr(unit, data_hap);
        if (data != NULL) {
            sal_memset(data, 0, cur_state_data->state_data_size);
        }

        sal_memset(cur_state_data, 0, sizeof(bcmltm_lt_state_data_t));
        cur_state_data_hap = next_state_data_hap;
    }

    sal_memset(state, 0, sizeof(*state));
    return;
}


/*!
 * \brief Add LT additional state data.
 *
 * Allocates and add specified additional state data block
 * to the given table state.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Table ID.
 * \param [in] lt_state_hap HA pointer of LT state to add state data to.
 * \param [in] data_size Size in bytes of state data to add.
 * \param [in] data_type Format of the state data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_state_data_add(int unit,
                       uint32_t ltid,
                       bcmltm_ha_ptr_t lt_state_hap,
                       unsigned int data_size,
                       bcmltm_state_type_t data_type)
{
    bcmltm_lt_state_t *lt_state;
    bcmltm_ha_ptr_t tsd_hap, data_hap;
    bcmltm_lt_state_data_t *tsd = NULL;
    void *data = NULL;

    SHR_FUNC_ENTER(unit);

    lt_state = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_hap);
    SHR_NULL_CHECK(lt_state, SHR_E_INTERNAL);

    /* State data structure */
    tsd_hap = bcmltm_ha_alloc(unit,
                              BCMLTM_HA_BLOCK_TYPE_STATE_DATA,
                              sizeof(bcmltm_lt_state_data_t));
    tsd = bcmltm_ha_ptr_to_mem_ptr(unit, tsd_hap);
    if (tsd == NULL) {
        const char *table_name = NULL;
        uint32_t alloc_size = sizeof(bcmltm_lt_state_data_t);

        table_name = bcmltm_lt_table_sid_to_name(unit, ltid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "HA memory allocation failure "
                              "of TSD header size %d "
                              "for %s(ltid=%d)\n"),
                   alloc_size, table_name, ltid));

        bcmltm_ha_blocks_dump(unit, NULL);
        bcmltm_state_all_dump(unit, NULL);
    }
    SHR_NULL_CHECK(tsd, SHR_E_MEMORY);

    /*
     * Now fill out state_data structure in case of error in null check,
     * so destroy function operates properly.
     */
    tsd->state_data_type = data_type;
    BCMLTM_HA_PTR_INVALID_SET(tsd->state_data_hap);
    BCMLTM_HA_PTR_INVALID_SET(tsd->next_state_data_hap);

    /* Append to list */
    if (BCMLTM_HA_PTR_IS_INVALID(lt_state->table_state_data_hap)) {
        lt_state->table_state_data_hap = tsd_hap;
    } else {
        bcmltm_ha_ptr_t last_tsd_hap = lt_state->table_state_data_hap;
        bcmltm_lt_state_data_t *last_tsd;

        /*
         * We already checked above that last_tsd_hap is valid, so this
         * loop must execute at least once to give a valid last_tsd.
         */
        while (!BCMLTM_HA_PTR_IS_INVALID(last_tsd_hap)) {
            last_tsd = bcmltm_ha_ptr_to_mem_ptr(unit, last_tsd_hap);
            SHR_NULL_CHECK(last_tsd, SHR_E_INTERNAL);

            last_tsd_hap = last_tsd->next_state_data_hap;
        }

        last_tsd->next_state_data_hap = tsd_hap;
    }

    data_hap = bcmltm_ha_alloc(unit,
                               BCMLTM_HA_BLOCK_TYPE_STATE_DATA,
                               data_size);
    data = bcmltm_ha_ptr_to_mem_ptr(unit, data_hap);
    if (data == NULL) {
        const char *table_name = NULL;

        table_name = bcmltm_lt_table_sid_to_name(unit, ltid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "HA memory allocation failure of size %d"
                              " for %s(ltid=%d)\n"),
                              data_size, table_name, ltid));
        bcmltm_ha_blocks_dump(unit, NULL);
        bcmltm_state_all_dump(unit, NULL);
    }
    SHR_NULL_CHECK(data, SHR_E_MEMORY);
    sal_memset(data, 0, data_size);

    /* Update state data */
    tsd->ltid = ltid;
    tsd->state_data_size = data_size;
    tsd->state_data_hap = data_hap;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover prior HA LT state
 *
 * Recover LT state records from HA subsystem.
 * Returns NULL if no previous state found for this LTID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] lt_state_hap_ptr Returning pointer to HA pointer LT state.
 *
 * \retval LT state HA pointer  Success.
 * \retval NULL                 Not found.
 */
static bcmltm_lt_state_t *
retrieve_ha_lt_state(int unit,
                     bcmlrd_sid_t sid,
                     bcmltm_ha_ptr_t *lt_state_hap_ptr)
{
    bcmltm_lt_state_t *state_ent;
    bcmltm_ha_ptr_t hap_ptr;
    bcmltm_ha_ptr_t prev_hap_ptr = BCMLTM_HA_PTR_INVALID;

    hap_ptr = bcmltm_ha_blk_get(unit, BCMLTM_HA_BLOCK_TYPE_STATE);

    do {
        state_ent = bcmltm_ha_ptr_to_mem_ptr(unit, hap_ptr);
        prev_hap_ptr = hap_ptr;
        hap_ptr = bcmltm_ha_ptr_increment(unit,
                                          hap_ptr,
                                          sizeof(bcmltm_lt_state_t));
    } while(state_ent && state_ent->ltid != sid &&
            hap_ptr != BCMLTM_HA_PTR_INVALID);

    if (state_ent && state_ent->ltid == sid) {
        *lt_state_hap_ptr = prev_hap_ptr;
        return state_ent;
    } else {
        return NULL;
    }
}

/*!
 * \brief Create the LT state.
 *
 * Create the LT state for the given table.
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit, sid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] table_attr Table attribute.
 * \param [in] warm Indicates if this is cold or warm boot.
 * \param [out] lt_state_hap_ptr Returning pointer to HA pointer LT state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_state_create(int unit, bcmlrd_sid_t sid,
                     const bcmltm_table_attr_t *table_attr,
                     bool warm,
                     bcmltm_ha_ptr_t *lt_state_hap_ptr)
{
    bcmltm_ha_ptr_t lt_state_hap;
    bcmltm_lt_state_t *lt_state = NULL;
    bcmltm_state_type_t data_type;
    uint32_t data_size;
    bcmltm_table_type_t table_type = table_attr->type;
    bcmltm_table_map_type_t map_type = table_attr->map_type;
    uint32_t track_index_max;
    bcmltm_table_info_t table_info;

    SHR_FUNC_ENTER(unit);

    BCMLTM_HA_PTR_INVALID_SET(lt_state_hap);

    /* Allocate */
    if (warm) {
        /*
         * LT state HA record either existed in previous version,
         * or was created by ISSU update for new LTs.
         */
        lt_state = retrieve_ha_lt_state(unit, sid, &lt_state_hap);
    } else {
        lt_state_hap = bcmltm_ha_alloc(unit, BCMLTM_HA_BLOCK_TYPE_STATE,
                                       sizeof(*lt_state));
        lt_state = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_hap);
    }
    if (lt_state == NULL) {
        const char *table_name = NULL;
        uint32_t alloc_size = sizeof(*lt_state);

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "HA memory allocation failure of "
                              "LT state header size %d "
                              "for %s(ltid=%d)\n"),
                              alloc_size, table_name, sid));
        bcmltm_ha_blocks_dump(unit, NULL);
        bcmltm_state_all_dump(unit, NULL);
    }
    SHR_NULL_CHECK(lt_state, SHR_E_MEMORY);
    if (lt_state->initialized) {
        /* HA recovery */
        if (lt_state->ltid != sid) {
            const char *table_name1 = NULL;
            const char *table_name2 = NULL;

            table_name1 = bcmltm_lt_table_sid_to_name(unit, lt_state->ltid);
            table_name2 = bcmltm_lt_table_sid_to_name(unit, sid);
            /* Mismatched LT state */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM state HA recovery: "
                                  "mismatched LTIDs - state %s(ltid=%d) "
                                  "expected %s(ltid=%d)\n"),
                       table_name1, lt_state->ltid,
                       table_name2, sid));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        } else {
            /* Nothing to do beyond connect LT to recovered state below. */
            SHR_EXIT();
        }
    } /* Else LT state HA record either created here, or by ISSU update */

    if (!warm) {
        sal_memset(lt_state, 0, sizeof(*lt_state));
        lt_state->ltid = sid;
    }

    /* Fill data */
    lt_state->entry_count = 0;
    
    /*
     * If table has an LTA table_entry_limit handler, max_entry_count
     * is set in bcmltm_md_logical_state_data_reset.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_table_info_get(unit, sid,
                                  &table_info));
    lt_state->max_entry_count = table_info.entry_maximum;

    BCMLTM_HA_PTR_INVALID_SET(lt_state->table_state_data_hap);

    /* Add all applicable state data blocks (i.e. size if > 0) */
    for (data_type = 0; data_type < BCMLTM_STATE_TYPE_COUNT; data_type++) {
        SHR_IF_ERR_EXIT
            (logical_state_data_size_get(unit, sid, table_attr,
                                         data_type, &data_size));
        if (data_size > 0) {
            SHR_IF_ERR_EXIT
                (logical_state_data_add(unit, sid, lt_state_hap,
                                        data_size, data_type));
        }
    }

    if (BCMLTM_TABLE_MAP_IS_DM(map_type) &&
        BCMLTM_TABLE_TYPE_IS_INDEX(table_type)) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_track_index_max_get(unit, sid,
                                              &track_index_max));
        lt_state->bitmap_size = track_index_max + 1;
    }

    /* Now LT state is prepared */
    lt_state->initialized = TRUE;

 exit:
    if (SHR_FUNC_ERR()) {
        logical_state_destroy(unit, lt_state_hap);
        *lt_state_hap_ptr = BCMLTM_HA_PTR_INVALID;
    } else {
        *lt_state_hap_ptr = lt_state_hap;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine an LTs state size requirements.
 *
 * Evaluate the LT state metadata size required for the given table.
 * This routine returns the required memory for the LT state and
 * the state data.
 * This function is used to determine the HA memory requirements
 * for the LTM.
 *
 * Assumes:
 *   - unit, sid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] state_size_ptr Returning pointer to LT state size.
 * \param [out] state_data_size_ptr Returning pointer to LT state data size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_state_size(int unit, bcmlrd_sid_t sid,
                   uint32_t *state_size_ptr,
                   uint32_t *state_data_size_ptr)
{
    const bcmltm_table_attr_t *table_attr;
    uint32_t state_size = 0, state_data_size = 0;
    bcmltm_state_type_t data_type;
    uint32_t data_size;

    SHR_FUNC_ENTER(unit);

    *state_size_ptr = 0;
    *state_data_size_ptr = 0;

    SHR_IF_ERR_EXIT
        (bcmltm_db_table_attr_get(unit, sid, &table_attr));

    /* Size of bcmltm_lt_state_t structure. */
    state_size +=
        BCMLTM_WORDSIZEBYTES(sizeof(bcmltm_lt_state_t));

    /* Add all applicable state data blocks (i.e. size if > 0) */
    for (data_type = 0; data_type < BCMLTM_STATE_TYPE_COUNT; data_type++) {
        SHR_IF_ERR_EXIT
            (logical_state_data_size_get(unit, sid, table_attr,
                                         data_type, &data_size));
        if (data_size > 0) {
            /* Size of bcmltm_lt_state_data_t structure. */
            state_data_size +=
                BCMLTM_WORDSIZEBYTES(sizeof(bcmltm_lt_state_data_t));

            /* Size of state data */
            state_data_size += BCMLTM_WORDSIZEBYTES(data_size);
        }
    }

    *state_size_ptr = state_size;
    *state_data_size_ptr = state_data_size;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given params metadata.
 *
 * Destroy the given params metadata.
 *
 * \param [in] unit Unit number.
 * \param [in] params Pointer to params metadata to destroy.+
 */
static void
logical_params_destroy(int unit,
                       bcmltm_lt_params_t *params)
{
    if (params == NULL) {
        return;
    }

    SHR_FREE(params);

    return;
}


/*!
 * \brief Create the LT params metadata.
 *
 * Create the LT params metadata for the given table.
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit, sid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] warm Indicates if this is cold or warm boot.
 * \param [out] params_ptr Returning pointer to LT params metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_params_create(int unit, bcmlrd_sid_t sid, bool warm,
                      bcmltm_lt_params_t **params_ptr)
{
    bcmltm_lt_params_t *params = NULL;
    const bcmltm_table_attr_t *table_attr;
    bcmltm_table_type_t table_type;
    uint32_t index_min;
    uint32_t index_max;
    uint32_t mem_param_min;
    uint32_t mem_param_max;
    uint32_t num_fid;
    uint32_t num_fid_idx;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(params, sizeof(*params), "bcmltmLtParams");
    SHR_NULL_CHECK(params, SHR_E_MEMORY);
    sal_memset(params, 0, sizeof(*params));

    /* Retrieve table attributes to determine interactive/modeled mode */
    SHR_IF_ERR_EXIT
        (bcmltm_db_table_attr_get(unit, sid, &table_attr));

    table_type = table_attr->type;

    SHR_IF_ERR_EXIT
        (bcmltm_db_field_key_count_get(unit, sid,
                                       &num_fid, &num_fid_idx));

    /*
     * Get lt index minimum and maximum value.
     * If table does not have such key, min and max are set to zeros.
     *
     * The lt index represents a group of entries in a table.
     * In addition, a table may have a memory parameter which identifies
     * a given group.  This memory parameter is accounted for
     * when creating the bitmap used for tracking entries in
     * an Index LT (lt_state).
     */
    index_min = table_attr->index_min;
    index_max = table_attr->index_max;

    /* Sanity check */
    if (index_min > index_max) {
        const char *table_name = NULL;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM metadata params create: "
                              "%s(ltid=%d) invalid "
                              "index_min=%d index_max=%d\n"),
                   table_name, sid, index_min, index_max));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    mem_param_min = table_attr->mem_param_min;
    mem_param_max = table_attr->mem_param_max;

    /* Sanity check */
    if (mem_param_min > mem_param_max) {
        const char *table_name = NULL;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM metadata params create: "
                              "%s(ltid=%d) invalid "
                              "mem_param_min=%d mem_param_max=%d\n"),
                   table_name, sid, mem_param_min, mem_param_max));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Fill data */
    params->ltid = BCMLTM_SID_TO_LTID(sid);
    if (table_attr->mode != BCMLTM_TABLE_MODE_INTERACTIVE) {
        params->lt_flags |= BCMLTM_LT_FLAGS_MODELED;
    }
    if (table_attr->flag & BCMLTM_TABLE_FLAG_OVERLAY) {
        params->lt_flags |= BCMLTM_LT_FLAGS_OVERLAY;
    }
    if (num_fid_idx == 0) {
        params->lt_flags |= BCMLTM_LT_FLAGS_NO_KEY;
    }
    
    if (BCMLTM_TABLE_TYPE_IS_INDEX(table_type)) {
        params->index_key_num = num_fid_idx;
    }
    params->index_min = index_min;
    params->index_max = index_max;
    params->mem_param_min = mem_param_min;
    params->mem_param_max = mem_param_max;
    /* End obsolete fields */
    params->global_inuse_ltid = table_attr->global_ltid;
    SHR_IF_ERR_EXIT(logical_state_create(unit, sid, table_attr, warm,
                                         &params->lt_state_hap));

    *params_ptr = params;

 exit:
    if (SHR_FUNC_ERR()) {
        logical_params_destroy(unit, params);
        *params_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine the HA state size requirements.
 *
 * Evaluate all LT state metadata sizes required to support the
 * configured set of Logical Tables.  Record the total and maximum
 * state sizes for use in calculating the HA block sizes.
 *
 * Assumes:
 *   - unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid_max_count Maximum number of tables IDs.
 * \param [out] ha_state_size_ptr Returning pointer to HA state size record.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_md_ha_sizes_get(int unit,
                       uint32_t sid_max_count,
                       bcmltm_ha_state_sizes_t *ha_state_size_ptr)
{
    uint32_t sid;
    bcmlrd_sid_t *sid_list = NULL;
    size_t num_sid;
    size_t idx;
    unsigned int size;
    uint32_t state_size, state_data_size;
    int rv;
    SHR_FUNC_ENTER(unit);

    sal_memset(ha_state_size_ptr, 0, sizeof(*ha_state_size_ptr));

    /* Allocate sid list array */
    sid_max_count = bcmltm_db_table_count_get();
    if (sid_max_count == 0) {
        /* No LTs to record */
        SHR_EXIT();
    }
    size = sizeof(*sid_list) * sid_max_count;
    SHR_ALLOC(sid_list, size, "bcmltmSidArr");
    SHR_NULL_CHECK(sid_list, SHR_E_MEMORY);
    sal_memset(sid_list, 0, size);

    /* Get SID list */
    rv = bcmlrd_table_list_get(unit, sid_max_count, sid_list, &num_sid);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_UNAVAIL) {
            /* No tables are defined */
            num_sid = 0;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Could not get table ID list rv=%d\n"),
                       rv));
            SHR_ERR_EXIT(rv);
        }
    }

    for (idx = 0; idx < num_sid; idx++) {
        sid = sid_list[idx];

        state_size = 0;
        state_data_size = 0;

        SHR_IF_ERR_EXIT
            (logical_state_size(unit, sid, &state_size, &state_data_size));

        if (state_size > ha_state_size_ptr->maximum_state_size) {
            ha_state_size_ptr->maximum_state_size = state_size;
        }

        if (state_data_size > ha_state_size_ptr->maximum_state_data_size) {
            ha_state_size_ptr->maximum_state_data_size = state_data_size;
        }

        ha_state_size_ptr->total_state_size += state_size;
        ha_state_size_ptr->total_state_data_size += state_data_size;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "LT state size %d, LT state data size %d\n"),
                 ha_state_size_ptr->total_state_size,
                 ha_state_size_ptr->total_state_data_size));

 exit:
    SHR_FREE(sid_list);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the LTM HA state in preparation for metadata creation.
 *
 * During metadata creation, the LT state is connected to the
 * metadata structures.  This connection may take the following forms:
 *
 * Cold Boot   - The state record for each LT is created within
 *               the HA space and initialized.
 * Warm Boot   - The state record of each LT is recovered from
 *               the HA space.
 * HA recovery - The state record of each LT is recovered from
 *               the HA space, and sanity checks performed.
 * ISSU/ISSD   - The state record of each LT is recovered from
 *               the HA space.  The supported LT set may change,
 *               so the HA space may require reconfiguration to
 *               match the correct ordering of the new LT set.
 *               The LT state format may have changed, so the
 *               prior state information must be transcribed
 *               into the new format if so.
 *               Each LT may require individual further adjustment
 *               to its state information.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates if this is cold or warm boot.
 * \param [in] sid_max_count Maximum number of tables IDs.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_md_ha_init(int unit,
                  bool warm,
                  uint32_t sid_max_count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmltm_md_ha_sizes_get(unit, sid_max_count,
                                &(LOGICAL_HA_SIZES(unit))));

    SHR_IF_ERR_EXIT
        (bcmltm_ha_init(unit, warm, &(LOGICAL_HA_SIZES(unit))));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create operation metadata for the given opcode.
 *
 * Create the operation metadata for given table ID to use for the
 * specified opcode.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] lt_drv LT driver (assumes not null).
 * \param [in] opcode Opcode.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_op_md_create(int unit, bcmlrd_sid_t sid,
                     const bcmltm_md_lt_drv_t *lt_drv,
                     bcmlt_opcode_t opcode,
                     bcmltm_lt_op_md_t **op_md_ptr)
{
    bool supported;

    SHR_FUNC_ENTER(unit);

    if (lt_drv == NULL) {
        SHR_EXIT();
    }

    *op_md_ptr = NULL;

    /* Skip unsupported opcodes */
    SHR_IF_ERR_EXIT
        (bcmltm_db_table_opcode_is_supported(unit, sid, opcode, &supported));
    if (!supported) {
        SHR_EXIT();
    }

    switch (opcode) {
    case BCMLT_OPCODE_NOP:
        SHR_EXIT();
        break;
    case BCMLT_OPCODE_INSERT:   /* Insert an entry */
        if (lt_drv->op_insert_create != NULL) {
            SHR_IF_ERR_EXIT
                (lt_drv->op_insert_create(unit, sid, op_md_ptr));
        }
        SHR_EXIT();
        break;
    case BCMLT_OPCODE_LOOKUP:   /* Retrieve the contents of an entry */
        if (lt_drv->op_lookup_create != NULL) {
            SHR_IF_ERR_EXIT
                (lt_drv->op_lookup_create(unit, sid, op_md_ptr));
        }
        SHR_EXIT();
        break;
    case BCMLT_OPCODE_DELETE:  /* Delete an entry */
        if (lt_drv->op_delete_create != NULL) {
            SHR_IF_ERR_EXIT
                (lt_drv->op_delete_create(unit, sid, op_md_ptr));
        }
        SHR_EXIT();
        break;
    case BCMLT_OPCODE_UPDATE:  /* Modify the specified fields of an entry */
        if (lt_drv->op_update_create != NULL) {
            SHR_IF_ERR_EXIT
                (lt_drv->op_update_create(unit, sid, op_md_ptr));
        }
        SHR_EXIT();
        break;
    case BCMLT_OPCODE_TRAVERSE:  /* Retrieve next entry */
        if (lt_drv->op_traverse_create != NULL) {
            SHR_IF_ERR_EXIT
                (lt_drv->op_traverse_create(unit, sid, op_md_ptr));
        }
        SHR_EXIT();
        break;
    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
        break;
    }

 exit:
    if (SHR_FUNC_ERR()) {
        lt_drv->op_destroy(*op_md_ptr);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if field select driver is applicable.
 *
 * This routine checks if the field select driver is
 * applicable to a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] field_select TRUE, field select driver is applicable.
 *                           FALSE, field select driver is not applicable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_lt_drv_is_field_select(int unit,
                               bcmlrd_sid_t sid,
                               bool *is_field_select)
{
    bcmltm_db_fs_core_info_t fs_info;

    SHR_FUNC_ENTER(unit);

    *is_field_select = FALSE;

    SHR_IF_ERR_EXIT
        (bcmltm_db_fs_core_info_get(unit, sid, &fs_info));

    /*
     * Use field select driver version if there is some
     * field selection information.
     *
     * Consideration:
     * The field select driver also works for tables without
     * any field selector.  For this option to work,
     * the database field selection information needs to be adjusted
     * so that it creates select information for tables
     * with and without selectors.
     *
     * Currently, the field selection information is only created
     * for tables with selectors.
     */
    if (fs_info.num_selections == 0) {
        /* No field selectors */
        SHR_EXIT();
    }

    *is_field_select = TRUE;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get LT driver based on table type.
 *
 * Get the Logical Table driver based on the table type.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Table ID.
 * \param [in] drv Returning pointer to LT driver.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_lt_drv_get(int unit,
                   bcmlrd_sid_t sid,
                   const bcmltm_md_lt_drv_t **drv)
{
    const bcmltm_table_attr_t *table_attr;
    bcmltm_table_map_type_t map_type;
    bcmltm_table_type_t table_type;
    bool field_select = FALSE;

    SHR_FUNC_ENTER(unit);

    *drv = NULL;

    /* Get table type */
    SHR_IF_ERR_EXIT
        (bcmltm_db_table_attr_get(unit, sid, &table_attr));

    map_type = table_attr->map_type;
    table_type = table_attr->type;

    /* Check for LT map type */
    if (BCMLTM_TABLE_MAP_IS_LTM(map_type)) {
        /*
         * LT with LTM map group.
         * E.g. TABLE_CONTROL, TABLE_INFO, TABLE_STATS.
         * Key types (i.e index, hash, etc.) are not relevant for this type.
         */
        *drv = bcmltm_md_lt_ltm_drv_get();
    } else if (BCMLTM_TABLE_MAP_IS_CTH(map_type)) {
        /*
         * LT with CTH map.
         * Key types (i.e index, hash, etc.) are not relevant for this type.
         */
        *drv = bcmltm_md_lta_cth_drv_get();
    } else if (BCMLTM_TABLE_MAP_IS_DM(map_type)) {
        /*
         * LT with direct PT maps (Direct Mapped LT).
         * Check for specific key types.
         */

        /* Check if field select driver is applicable */
        SHR_IF_ERR_EXIT
            (logical_lt_drv_is_field_select(unit, sid, &field_select));

        if (BCMLTM_TABLE_TYPE_IS_INDEX(table_type)) {
            if (field_select == TRUE) {
                *drv = bcmltm_md_lt_index_fs_drv_get();
            } else {
                *drv = bcmltm_md_lt_index_drv_get();
            }
        } else if (BCMLTM_TABLE_TYPE_IS_KEYED(table_type)) {
            if (field_select == TRUE) {
                *drv = bcmltm_md_lt_keyed_fs_drv_get();
            } else {
                *drv = bcmltm_md_lt_keyed_drv_get();
            }
        }
    }

    if (*drv == NULL) {
        const char *table_name = NULL;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s(ltid=%d): "
                              "Table type=%d map=%d not supported\n"),
                   table_name, sid, table_type, map_type));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the LT metadata.
 *
 * Destroy the LT metadata for given table.
 *
 * Assumes:
 *   - unit and sid are valid
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] lt_md LT metadata to destroy.
 */
static void
logical_lt_destroy(int unit, bcmlrd_sid_t sid,
                   bcmltm_lt_md_t *lt_md)
{
    bcmlt_opcode_t opcode;
    const bcmltm_md_lt_drv_t *lt_drv = NULL;

    if (lt_md == NULL) {
        return;
    }

    /* Get table driver */
    (void) logical_lt_drv_get(unit, sid, &lt_drv);

    /* Destroy ops */
    if ((lt_drv != NULL) && (lt_drv->op_destroy != NULL)) {
        for (opcode = 0; opcode < BCMLT_OPCODE_NUM; opcode++) {
            lt_drv->op_destroy(lt_md->op[opcode]);
        }
    }

    /* Destroy params */
    logical_params_destroy(unit, lt_md->params);

    SHR_FREE(lt_md);

    return;
}


/*!
 * \brief Create the LT metadata.
 *
 * Create the LT metadata for given table.
 *
 * Assumes:
 *   - unit and sid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] warm Indicates if this is cold or warm boot.
 * \param [out] lt_md_ptr Returning pointer to LT metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_lt_create(int unit,
                  bcmlrd_sid_t sid,
                  bool warm,
                  bcmltm_lt_md_t **lt_md_ptr)
{
    bcmltm_lt_md_t *lt_md = NULL;
    bcmltm_lt_params_t *params = NULL;
    bcmltm_lt_op_md_t *op_md = NULL;
    bcmlt_opcode_t opcode;
    const bcmltm_md_lt_drv_t *lt_drv = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(lt_md, sizeof(*lt_md), "bcmltmLtMd");
    SHR_NULL_CHECK(lt_md, SHR_E_MEMORY);
    sal_memset(lt_md, 0, sizeof(*lt_md));

    /* Get table driver */
    SHR_IF_ERR_EXIT
        (logical_lt_drv_get(unit, sid, &lt_drv));

    /* Create params */
    SHR_IF_ERR_EXIT(logical_params_create(unit, sid, warm, &params));
    lt_md->params = params;

    /* Create ops */
    for (opcode = 0; opcode < BCMLT_OPCODE_NUM; opcode++) {
        SHR_IF_ERR_EXIT
            (logical_op_md_create(unit, sid, lt_drv, opcode, &op_md));
        if (op_md != NULL) {
            SHR_IF_ERR_EXIT
                (bcmltm_md_op_trees_to_array(unit, op_md));
        }
        lt_md->op[opcode] = op_md;
    }

    /* Set Table Commit handler list */
    SHR_IF_ERR_EXIT(bcmltm_db_table_commit_list_get(unit, sid,
                                                    &lt_md->tc_list));

    /* Set LTA TABLE handlers */
    SHR_IF_ERR_EXIT
        (bcmltm_db_table_entry_limit_handler_get(unit, sid,
                                                 &lt_md->table_entry_limit));
    SHR_IF_ERR_EXIT
        (bcmltm_db_table_entry_usage_handler_get(unit, sid,
                                                 &lt_md->table_entry_usage));

    /* Set CTH handler */
    SHR_IF_ERR_EXIT(bcmltm_db_cth_handler_get(unit, sid, &lt_md->cth));

    *lt_md_ptr = lt_md;

 exit:
    if (SHR_FUNC_ERR()) {
        logical_lt_destroy(unit, sid, lt_md);
        *lt_md_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the LTM metadata.
 *
 * Destroy the LTM metadata for given unit.
 *
 * This implementation is based upon the symbols IDs
 * being in a compact numbering space from [0 .. (sid_max_count-1)]
 *
 * Assumes:
 *   - unit is valid
 *
 * \param [in] unit Unit number.
 * \param [in] ltm_md LTM metadata to destroy.
 */
static void
logical_ltm_destroy(int unit, bcmltm_md_t *ltm_md)
{
    bcmlrd_sid_t sid;
    bcmlrd_sid_t sid_max;

    if (ltm_md == NULL) {
        return;
    }

    sid_max = ltm_md->lt_max - 1;

    /* Destroy LT metadata for each table */
    for (sid = 0; sid <= sid_max; sid++) {
        logical_lt_destroy(unit, sid, ltm_md->lt_md[sid]);
        ltm_md->lt_md[sid] = NULL;
    }

    /* Free LTM metadata */
    SHR_FREE(ltm_md);

    return;
}


/*!
 * \brief Create the LTM metadata for given unit.
 *
 * Create the LTM metadata for given unit.
 *
 * This implementation is based upon the symbols IDs
 * being in a compact numbering space from [0 .. (sid_max_count-1)]
 *
 * Assumes:
 *   - unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid_max_count Maximum number of tables IDs.
 * \param [in] warm Indicates if this is cold or warm boot.
 * \param [out] ltm_md_ptr Returning pointer to LT metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_ltm_create(int unit,
                   uint32_t sid_max_count,
                   bool warm,
                   bcmltm_md_t **ltm_md_ptr)
{
    unsigned int size;
    bcmlrd_sid_t sid;
    bcmlrd_sid_t sid_max;
    bcmltm_md_t *ltm_md = NULL;
    bcmltm_lt_md_t *lt_md = NULL;
    bcmlrd_sid_t *sid_list = NULL;
    size_t num_sid;
    size_t idx;
    size_t opx;
    uint32_t *wb_max_size_p;
    int rv;

    SHR_FUNC_ENTER(unit);

    if (sid_max_count == 0) {
        /* Nothing to do */
        SHR_EXIT();
    }

    /* SID max value */
    sid_max = sid_max_count - 1;

    /* Allocate sid list array */
    size = sizeof(*sid_list) * sid_max_count;
    SHR_ALLOC(sid_list, size, "bcmltmSidArr");
    SHR_NULL_CHECK(sid_list, SHR_E_MEMORY);
    sal_memset(sid_list, 0, size);

    /* Get SID list */
    rv = bcmlrd_table_list_get(unit, sid_max_count, sid_list, &num_sid);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_UNAVAIL) {
            /* No tables are defined */
            num_sid = 0;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Could not get table ID list rv=%d\n"),
                       rv));
            SHR_ERR_EXIT(rv);
        }
    }

    /* Verify that SID value fits design criteria [0 .. (max_count-1)] */
    for (idx = 0; idx < num_sid; idx++) {
        if (sid_list[idx] > sid_max) {
            const char *table_name = NULL;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid_list[idx]);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Invalid SID %s(ltid=%d), "
                                  "SID should not be larger than %d\n"),
                       table_name, sid_list[idx], sid_max));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* Allocate LTM metadata structure */
    size = sizeof(bcmltm_md_t) +
        (sizeof(bcmltm_lt_md_t *) * sid_max_count);
    SHR_ALLOC(ltm_md, size, "bcmltmMd");
    SHR_NULL_CHECK(ltm_md, SHR_E_MEMORY);
    sal_memset(ltm_md, 0, size);

    /* Initialize LTM metadata */
    ltm_md->lt_num = 0;
    ltm_md->lt_max = sid_max_count;
    ltm_md->wb_max_modeled = 0;
    ltm_md->wb_max_interactive = 0;

    /* Create LT metadata for each table */
    for (idx = 0; idx < num_sid; idx++) {
        sid = sid_list[idx];
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (logical_lt_create(unit, sid, warm, &lt_md), SHR_E_UNAVAIL);
        /* Some Logical tables may not be applicable to a device */
        if (lt_md != NULL) {
            ltm_md->lt_md[sid] = lt_md;
            ltm_md->lt_num++;

            /* Check for maximum Working Buffer size */
            if (lt_md->params->lt_flags & BCMLTM_LT_FLAGS_MODELED) {
                wb_max_size_p = &(ltm_md->wb_max_modeled);
            } else {
                wb_max_size_p = &(ltm_md->wb_max_interactive);
            }
            for (opx = BCMLT_OPCODE_NOP; opx < BCMLT_OPCODE_NUM; opx++) {
                if (lt_md->op[opx] != NULL) {
                    if (lt_md->op[opx]->working_buffer_size >
                        *wb_max_size_p) {
                        *wb_max_size_p =
                            lt_md->op[opx]->working_buffer_size;
                    }
                }
            }
        }
    }

    *ltm_md_ptr = ltm_md;

 exit:
    if (SHR_FUNC_ERR()) {
        logical_ltm_destroy(unit, ltm_md);
        *ltm_md_ptr = NULL;
    }

    SHR_FREE(sid_list);
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */
int
bcmltm_md_logical_create(int unit, bool warm)
{
    uint32_t sid_max_count;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "LTM Logical Table metadata create\n")));

    /* Check if metadata has been initialized */
    if (LOGICAL_INIT(unit)) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "LTM Logical Table metadata is already "
                             "created\n")));
        SHR_EXIT();
    }

    /* Get total table symbol IDs count */
    sid_max_count = bcmltm_db_table_count_get();
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Total Logical tables is %d\n"),
                 (int)sid_max_count));

    /* Initialize Working Buffer information */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_info_init(unit, sid_max_count));

    /* Initialize and create LT database */
    SHR_IF_ERR_EXIT
        (bcmltm_db_init(unit, sid_max_count));

    /* Initialize HA information */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ha_init(unit, warm, sid_max_count));

    /* Create LTM metadata */
    rv = logical_ltm_create(unit, sid_max_count, warm, &LOGICAL_LTM_MD(unit));
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_UNAVAIL) {
            SHR_EXIT();
        } else {
            SHR_ERR_EXIT(rv);
        }
    }

 exit:
    if (SHR_FUNC_ERR() || (rv == SHR_E_UNAVAIL)) {
        bcmltm_md_logical_destroy(unit);
    }

    SHR_FUNC_EXIT();
}


int
bcmltm_md_logical_destroy(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Destroy LTM metadata */
    logical_ltm_destroy(unit, LOGICAL_LTM_MD(unit));
    LOGICAL_LTM_MD(unit) = NULL;

    /* Release HA information */
    bcmltm_ha_cleanup(unit);

    /* Cleanup LT database */
    bcmltm_db_cleanup(unit);

    /* Cleanup Working Buffer information */
    bcmltm_wb_lt_info_cleanup(unit);

    SHR_FUNC_EXIT();
}


int
bcmltm_md_logical_lt_retrieve(int unit,
                              uint32_t ltid,
                              bcmltm_lt_md_t **ltm_md_ptr)
{
    bcmlrd_sid_t sid;

    SHR_FUNC_ENTER(unit);

    /* Check if metadata has been initialized */
    if (!LOGICAL_INIT(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    sid = BCMLTM_LTID_TO_SID(ltid);

    /* Check that unit has metadata */
    if (LOGICAL_LTM_MD(unit) == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (!LOGICAL_SID_VALID(unit, sid)) {
        const char *table_name = NULL;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Invalid table id %s(sid=%d ltid=%d)\n"),
                     table_name, sid, ltid));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * Check that table is valid on the given unit.
     *
     * Logical table IDs is the super of all valid tables in all devices.
     * As such, there may be tables that are not defined for a given
     * device.  In this case, the pointer of the table metadata is NULL.
     */
    if (LOGICAL_LTM_MD(unit)->lt_md[sid] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    *ltm_md_ptr = LOGICAL_LTM_MD(unit)->lt_md[sid];

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_logical_retrieve(int unit,
                           bcmltm_md_t **ltm_md_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Check if metadata has been initialized */
    if (!LOGICAL_INIT(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Check that unit has metadata */
    if (LOGICAL_LTM_MD(unit) == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *ltm_md_ptr = LOGICAL_LTM_MD(unit);

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_logical_wb_max_get(int unit,
                             bcmltm_table_mode_t mode,
                             uint32_t *wb_max)
{
    SHR_FUNC_ENTER(unit);

    /* Check if metadata has been initialized */
    if (!LOGICAL_INIT(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Check that unit has metadata */
    if (LOGICAL_LTM_MD(unit) == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    switch(mode) {
    case BCMLTM_TABLE_MODE_MODELED:
        *wb_max = LOGICAL_LTM_MD(unit)->wb_max_modeled;
        break;

    case BCMLTM_TABLE_MODE_INTERACTIVE:
        *wb_max = LOGICAL_LTM_MD(unit)->wb_max_interactive;
        break;

    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_logical_state_data_reset(int unit, uint32_t ltid)
{
    int rv;
    bcmltm_lt_md_t *lt_md;
    bcmltm_ha_ptr_t lt_state_hap;
    bcmltm_lt_state_t *lt_state;
    bcmltm_table_info_t table_info;
    bcmltd_table_entry_limit_arg_t table_arg;
    bcmltd_table_entry_limit_t table_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmltm_md_logical_lt_retrieve(unit, ltid, &lt_md));

    /* Skip tables with no state data information */
    if (lt_md->params == NULL) {
        SHR_EXIT();
    }

    lt_state_hap = lt_md->params->lt_state_hap;
    if (lt_state_hap == BCMLTM_HA_PTR_INVALID) {
        SHR_EXIT();
    }

    lt_state = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_hap);
    if (lt_state == NULL) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (logical_state_data_reset(unit, ltid, lt_state));

    if ((lt_md->table_entry_limit != NULL) &&
        (lt_md->table_entry_limit->entry_limit_get != NULL)) {
        sal_memset(&table_arg, 0, sizeof(table_arg));
        SHR_IF_ERR_EXIT
            (bcmltm_db_table_info_get(unit, ltid, &table_info));
        table_arg.entry_maximum = table_info.entry_maximum;

        rv = lt_md->table_entry_limit->entry_limit_get(unit,
                                     BCMPTM_DIRECT_TRANS_ID,
                                     ltid,
                                     &table_arg,
                                     &table_data,
                                     lt_md->table_entry_limit->arg);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
        if (rv != SHR_E_UNAVAIL) {
            lt_state->max_entry_count = table_data.entry_limit;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_logical_valid_track_index_bitmap_get(int unit,
                                               uint32_t sid,
                                               SHR_BITDCL *valid_bitmap)
{
    bcmltm_field_list_t *api_keys = NULL;
    bcmltm_field_list_t *cur_field;
    bcmltm_lt_md_t *lt_md = NULL;
    bcmltm_lt_state_t *lt_state = NULL;
    uint32_t num_keys, fix;
    uint32_t alloc_size;
    bcmltm_index_key_bounds_t *index_key_bounds = NULL;
    bcmltm_key_bound_info_t *cur_key_bound;
    uint32_t wb_max_size;
    uint32_t *recurse_wb = NULL;
    const bcmltm_track_index_t *ti_nc_ptr = NULL;
    bcmltm_valid_bitmap_info_t vbi;

    SHR_FUNC_ENTER(unit);

    /* Get LT metadata */
    SHR_IF_ERR_EXIT
        (bcmltm_md_logical_lt_retrieve(unit, sid, &lt_md));
    SHR_NULL_CHECK(lt_md, SHR_E_INTERNAL);
    vbi.lt_md = lt_md;

    /* Get LT state */
    SHR_IF_ERR_EXIT
        (bcmltm_state_lt_get(unit, sid, &lt_state));
    SHR_NULL_CHECK(lt_state, SHR_E_INTERNAL);
    vbi.lt_state = lt_state;

    /* Get this LT's WB Track Index offset */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_get(unit, sid, &ti_nc_ptr));
    SHR_NULL_CHECK(ti_nc_ptr, SHR_E_INTERNAL);
    vbi.track_offset = ti_nc_ptr->track_index_offset;

    /* Get LT API key bounds */
    SHR_IF_ERR_EXIT
        (logical_key_bounds_create(unit, sid,
                                   &index_key_bounds));
    vbi.index_key_bounds = index_key_bounds;

    /* Set up returning bitmap location */
    vbi.valid_bitmap = valid_bitmap;

    /* Create scratch Working Buffer */
    SHR_IF_ERR_EXIT
        (bcmltm_md_logical_wb_max_get(unit, BCMLTM_TABLE_MODE_MODELED,
                                      &wb_max_size));
    if (wb_max_size == 0) {
        /* No LTs to record */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(recurse_wb, wb_max_size,
              "bcmltmValidTrackIndexBitmapWb");
    SHR_NULL_CHECK(recurse_wb, SHR_E_MEMORY);
    sal_memset(recurse_wb, 0, wb_max_size);
    vbi.ltm_buffer = recurse_wb;

    /* Create API key list */
    num_keys = index_key_bounds->num_keys;
    if (num_keys > 0) {
        alloc_size = num_keys * sizeof(bcmltm_field_list_t);
        SHR_ALLOC(api_keys, alloc_size, "bcmltmKeyFieldList");
        SHR_NULL_CHECK(api_keys, SHR_E_MEMORY);
        sal_memset(api_keys, 0, alloc_size);

        /* Set up API key list - connect array as linked list */
        for (fix = 0; fix < num_keys; fix++) {
            cur_field = &(api_keys[fix]);
            cur_key_bound = &(index_key_bounds->key_bounds[fix]);
            cur_field->id = cur_key_bound->api_field_id;
            cur_field->idx = 0;
            if ((fix + 1) < num_keys) {
                cur_field->next = &(api_keys[fix + 1]);
            }
        }
    }

    /* Set up LT entry to inform FA nodes of proper operation. */
    sal_memset(&(vbi.lt_entry), 0, sizeof(vbi.lt_entry));
    vbi.lt_entry.unit = unit;
    vbi.lt_entry.table_id = sid;
    vbi.lt_entry.opcode.lt_opcode = BCMLT_OPCODE_LOOKUP;
    vbi.lt_entry.in_fields = api_keys;

    /* Note the FA pass 0 info for LOOKUP op for this LT. */
    vbi.lookup_nodes_array = lt_md->op[BCMLT_OPCODE_LOOKUP]->nodes_array;
    vbi.lookup_key_nodes_count =
        lt_md->op[BCMLT_OPCODE_LOOKUP]->num_nodes.first_tree_node_count;

    /* Kick off recursion over key fields */
    SHR_IF_ERR_EXIT
        (valid_track_index_bitmap_recurse(unit, 0, &(vbi)));

 exit:
    /* Clean up all allocated memory */
    SHR_FREE(api_keys);
    SHR_FREE(recurse_wb);
    logical_key_bounds_destroy(index_key_bounds);

    SHR_FUNC_EXIT();
}
