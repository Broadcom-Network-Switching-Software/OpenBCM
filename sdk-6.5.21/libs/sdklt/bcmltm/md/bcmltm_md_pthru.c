/*! \file bcmltm_md_pthru.c
 *
 * Logical Table Manager Physical Table Pass Through (Thru) Metadata.
 *
 * This is the most basic case. Each physical symbol will have a
 * 'logical table' representation, with a 1-1 mapping of all its
 * physical fields to logical fields for a given table.
 *
 * The Physical Table Pass Thru metadata uses the symbol IDs from the
 * DRD component.  These symbol IDs are the identification for all
 * the physical tables (register and memories) defined on a given
 * device.
 *
 * The DRD symbol IDs are defined in a compact numbering
 * space [0.. n-1], where 'n' is the total number of DRD symbols for
 * a given device.
 *
 * The design of this component is based on the DRD symbol ID attribute
 * define above.   The symbol ID is used as the array index into
 * several of the table metadata data structure.
 *
 * The DRD symbol ID value is used as the LTM table ID.
 *
 * Metadata Creation Flow Design:
 *
 * A device often contains a large number of physical memories and
 * registers.  Creating the LTM metadata for all the physical tables
 * for PT Pass Thru requires large memory and significant
 * initialization time.
 *
 * Given that PT Pass Thru is used for debugging purposes and
 * its usage is expected to be low, this large resource consumption
 * is not justified.
 *
 * The following methodology is chosen to optimize time and memory
 * and works with the PT Pass Thru use case:
 * - The metadata for a physical table will be created on demand
 *   and remain in cache until an operation with a new table ID is
 *   requested:
 *   . If table ID is same as last, use existing metadata.
 *   . If table ID is new, destroy previous cached metadata (if one exist)
 *     and create metadata for new table.
 * - At any given time, the LTM contains the cached metadata of at most
 *   one table (last one).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>

#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_db_pthru_internal.h>
#include <bcmltm/bcmltm_md_pthru_internal.h>

#include "bcmltm_md_pthru_op.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*!
 * Table Memory Pool.
 *
 * This structure contains a series of definitions used
 * for getting memory needed for creating the metadata for a table.
 *
 * This offers an alternative to allocating and freeing heap memory
 * for each table operation with a new table ID.  This method helps
 * reduce memory fragmentation for PT Pass Thru.
 */
typedef struct md_pthru_table_mp_s {
    /*! LT metadata memory. */
    bcmltm_lt_md_t lt_md;

    /*! Params metadata memory. */
    bcmltm_lt_params_t lt_params;
} md_pthru_table_mp_t;

/*!
 * Memory Pool Container.
 *
 * This structure contains a series of definitions used
 * for getting memory needed for creating the metadata for tables.
 *
 * This structure contains data for one table.
 * This matches the current PT Pass Thru design (only 1 PT metadata
 * cached at a time).  If there is a need to cache more tables,
 * the member can be expanded to an array.
 *
 * Optimization:
 * If there is certainty that the design will remain unchanged
 * (store no more than one table), the per-table structure can be
 * removed and its content moved here.
 */
typedef struct md_pthru_mp_s {
    /*! Memory pool (1 table). */
    md_pthru_table_mp_t table;
} md_pthru_mp_t;

/*!
 * PT Pass Thru Metadata.
 *
 * This structure contains the PT Pass Thru metadata and
 * is tailored to contain the cached data for one table
 * at any given time.
 *
 * The PT Pass Thru metadata for a given table is created on demand
 * and is destroyed when the next operation contains a new table ID.
 */
typedef struct md_pthru_s {
    /*! Total number of symbols. */
    uint32_t sid_max_count;

    /*! Memory pool container. */
    md_pthru_mp_t mp;

    /*!
     * Active table ID.
     *
     * This is the ID of the cached table in the LTM metadata.
     * The metadata 'ltm_md' keeps cached information for only
     * 1 table (table for last operation) at any given time.
     */
    bcmdrd_sid_t active_sid;

    /*! LTM device metadata. */
    bcmltm_md_t *ltm_md;
} md_pthru_t;

/* PT Pass Thru Metadata */
static md_pthru_t *md_pthru[BCMLTM_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Check if metadata has been initialized.
 *
 * This routine checks if the PT Pass Thru metadata has been
 * initialized for the given unit.
 *
 * \param [in] unit Unit number.
 *
 * \retval TRUE If metadata has been initialized.
 * \retval FALSE If metadata has not been initialized or unit is invalid.
 */
static inline bool
md_pthru_is_init(int unit)
{
    if ((unit < 0) || (unit >= BCMLTM_MAX_UNITS)) {
        return FALSE;
    }

    return (md_pthru[unit] != NULL);
}

/*!
 * \brief Check if table ID is valid.
 *
 * This routine checks if given physical table ID is valid.
 * It assumes that the unit is valid and the metadata has been initialized.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 *
 * \retval TRUE If table ID is valid.
 * \retval FALSE If table ID is invalid.
 */
static inline bool
md_pthru_sid_is_valid(int unit,
                      bcmdrd_sid_t sid)
{
    if (sid >= md_pthru[unit]->sid_max_count) {
        return FALSE;
    }

    return bcmdrd_pt_is_valid(unit, sid);
}

/*
 * The following set of functions provides interfaces to
 * obtain memory from the local memory pool.
 *
 * These routines mimic the flow for allocating and freeing memory.
 * It helps isolate the underneath implementation which currently
 * uses memory allocated only once during LTM initialization.
 * It can be replaced by other methodology if needed.
 *
 * Some functions do not need to perform any actions, but are
 * provided to allow future improvements to be incorporated more easily.
 *
 * Similar to how the heap memory allocation routine works,
 * the memory returned from _alloc() is not cleared.
 * The caller is responsible for initializing the memory (if required).
 */

/*!
 * \brief Allocate memory for bcmltm_lt_md_t struct.
 *
 * This routine returns memory for bcmltm_lt_md_t struct.
 *
 * \param [in] unit Unit number.
 *
 * \retval Pointer to requested memory.
 * \retval NULL on error.
 */
static inline bcmltm_lt_md_t *
md_pthru_lt_md_alloc(int unit)
{
    return &md_pthru[unit]->mp.table.lt_md;
}

/*!
 * \brief Free memory.
 *
 * This routine frees the given memory pointer.
 * For the current memory pool implementation, there is no action.
 *
 * \param [in] ptr Pointer to memory to be freed.
 */
static inline void
md_pthru_lt_md_free(bcmltm_lt_md_t *ptr)
{
    /* Nothing to do */
    return;
}

/*!
 * \brief Allocate memory for bcmltm_lt_params_t struct.
 *
 * This routine returns memory for bcmltm_lt_params_t struct.
 *
 * \param [in] unit Unit number.
 *
 * \retval Pointer to requested memory.
 * \retval NULL on error.
 */
static inline bcmltm_lt_params_t *
md_pthru_lt_params_alloc(int unit)
{
    return &md_pthru[unit]->mp.table.lt_params;
}

/*!
 * \brief Free memory.
 *
 * This routine frees the given memory pointer.
 * For the current memory pool implementation, there is no action.
 *
 * \param [in] ptr Pointer to memory to be freed.
 */
static inline void
md_pthru_lt_params_free(bcmltm_lt_params_t *ptr)
{
    /* Nothing to do */
    return;
}

/*!
 * \brief Cleanup table memory pool.
 *
 * This routine frees any necessary resources for the given
 * table memory pool.
 *
 * \param [in] mp Table memory pool.
 */
static inline void
md_pthru_table_mp_cleanup(md_pthru_table_mp_t *mp)
{
    /* Nothing to do */
    return;
}

/*!
 * \brief Initialize table memory pool.
 *
 * This routine initializes the given table memory pool.
 *
 * \param [in] unit Unit number.
 * \param [out] mp Table memory pool.
 *
 * \retval SHR_E_NONE No errors
 */
static inline int
md_pthru_table_mp_init(int unit,
                       md_pthru_table_mp_t *mp)
{
    /* Nothing to do */
    return SHR_E_NONE;
}

/*!
 * \brief Cleanup memory pool container.
 *
 * This routine frees any necessary resources for the given
 * memory pool container.
 *
 * \param [in] mp Memory pool container.
 */
static inline void
md_pthru_mp_cleanup(md_pthru_mp_t *mp)
{
    md_pthru_table_mp_cleanup(&mp->table);
}

/*!
 * \brief Initialize memory pool container.
 *
 * This routine initializes the given memory pool container.
 *
 * \param [in] unit Unit number.
 * \param [out] mp Memory pool container.
 *
 * \retval SHR_E_NONE No errors
 */
static inline int
md_pthru_mp_init(int unit,
                 md_pthru_mp_t *mp)
{
    return md_pthru_table_mp_init(unit, &mp->table);
}

/*!
 * \brief Destroy the given params metadata.
 *
 * This routine destroys the given params metadata.
 *
 * \param [in] lt_params Pointer to params metadata to destroy.
 */
static inline void
md_pthru_lt_params_destroy(bcmltm_lt_params_t *lt_params)
{
    if (lt_params == NULL) {
        return;
    }

    md_pthru_lt_params_free(lt_params);

    return;
}


/*!
 * \brief Create the LT params metadata.
 *
 * This routine creates the LT params metadata for the given table.
 *
 * Assumes:
 *   - unit, sid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] lt_params_ptr Returning pointer to LT params metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_lt_params_create(int unit,
                          bcmdrd_sid_t sid,
                          bcmltm_lt_params_t **lt_params_ptr)
{
    bcmltm_lt_params_t *lt_params = NULL;
    int index_min;
    int index_max;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    lt_params = md_pthru_lt_params_alloc(unit);
    SHR_NULL_CHECK(lt_params, SHR_E_MEMORY);
    sal_memset(lt_params, 0, sizeof(*lt_params));

    index_min = bcmdrd_pt_index_min(unit, sid);
    index_max = bcmdrd_pt_index_max(unit, sid);

    /* Sanity check */
    if ((index_min < 0) || (index_max < 0) || (index_min > index_max)) {
        const char *table_name;

        table_name = bcmltm_pt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM metadata params create: "
                              "%s(ptid=%d) invalid "
                              "index_min=%d index_max=%d\n"),
                   table_name, sid, index_min, index_max));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Fill data */
    lt_params->ltid = BCMLTM_SID_TO_LTID(sid);
    lt_params->lt_flags |= BCMLTM_LT_FLAGS_PASSTHRU;
    lt_params->index_min = index_min;
    lt_params->index_max = index_max;
    lt_params->mem_param_min = 0;
    lt_params->mem_param_max = bcmdrd_pt_num_tbl_inst(unit, sid) - 1;

    /* Not required for PT Pass Thru */
    lt_params->index_key_num = 0;
    lt_params->global_inuse_ltid = BCMDRD_INVALID_ID;
    BCMLTM_HA_PTR_INVALID_SET(lt_params->lt_state_hap);
    BCMLTM_HA_PTR_INVALID_SET(lt_params->lt_rollback_state_hap);

    *lt_params_ptr = lt_params;

 exit:
    if (SHR_FUNC_ERR()) {
        md_pthru_lt_params_destroy(lt_params);
        *lt_params_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the LT metadata.
 *
 * This routine destroys the given LT metadata.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md LT metadata to destroy.
 */
static void
md_pthru_lt_md_destroy(int unit,
                       bcmltm_lt_md_t *lt_md)
{
    if (lt_md == NULL) {
        return;
    }

    /* Destroy params */
    md_pthru_lt_params_destroy(lt_md->params);

    md_pthru_lt_md_free(lt_md);

    return;
}

/*!
 * \brief Create the LT metadata.
 *
 * This routine creates the LT metadata for given table.
 *
 * Assumes:
 *   - unit and sid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] lt_md_ptr Returning pointer to LT metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_lt_md_create(int unit,
                      bcmdrd_sid_t sid,
                      bcmltm_lt_md_t **lt_md_ptr)
{
    bcmltm_lt_md_t *lt_md = NULL;
    bcmltm_lt_params_t *params = NULL;
    bcmltm_lt_op_md_t *op_md = NULL;
    bcmlt_pt_opcode_t opcode;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    lt_md = md_pthru_lt_md_alloc(unit);
    SHR_NULL_CHECK(lt_md, SHR_E_MEMORY);
    sal_memset(lt_md, 0, sizeof(*lt_md));

    /* Create params */
    SHR_IF_ERR_EXIT(md_pthru_lt_params_create(unit, sid, &params));
    lt_md->params = params;

    /* Get opcodes metadata */
    for (opcode = 0; opcode < BCMLT_PT_OPCODE_NUM; opcode++) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_pthru_op_get(unit, sid, opcode, &op_md));
        lt_md->op[opcode] = op_md;
    }

    /* Remaining struct members are not used for PT Pass Thru */

    *lt_md_ptr = lt_md;

 exit:
    if (SHR_FUNC_ERR()) {
        md_pthru_lt_md_destroy(unit, lt_md);
        *lt_md_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy table metadata.
 *
 * This routine destroys the PT Pass Thru metadata for the given table.
 *
 * Assumes:
 *   - unit and sid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
md_pthru_table_destroy(int unit,
                       bcmdrd_sid_t sid)
{
    int rv;
    md_pthru_t *md = md_pthru[unit];
    bcmltm_md_t *ltm_md = md->ltm_md;

    /* Destroy table metadata */
    md_pthru_lt_md_destroy(unit, ltm_md->lt_md[sid]);
    ltm_md->lt_md[sid] = NULL;
    ltm_md->lt_num--;

    /* Cleanup table memory pool */
    md_pthru_table_mp_cleanup(&md->mp.table);

    /* Destroy table opcodes metadata */
    rv = bcmltm_md_pthru_op_table_destroy(unit, sid);
    if (SHR_FAILURE(rv)) {
        const char *table_name;

        table_name = bcmltm_pt_table_sid_to_name(unit, sid);
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failure to destroy PT Pass Thru "
                             "table opcodes metadata %s(ptid=%d)\n"),
                  table_name, sid));

        /* Force destroy of current table opcodes */
        rv = bcmltm_md_pthru_op_table_destroy(unit, BCMDRD_INVALID_ID);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Failure to destroy PT Pass Thru "
                                  "cached table opcodes metadata\n")));
        }
    }

    /* Destroy table database */
    rv = bcmltm_db_pthru_table_destroy(unit, sid);
    if (SHR_FAILURE(rv)) {
        const char *table_name;

        table_name = bcmltm_pt_table_sid_to_name(unit, sid);
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failure to destroy PT Pass Thru "
                             "table database %s(ptid=%d)\n"),
                  table_name, sid));

        /* Force destroy of current table stored in database */
        rv = bcmltm_db_pthru_table_destroy(unit, BCMDRD_INVALID_ID);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Failure to destroy PT Pass Thru "
                                  "cached table database\n")));
        }
    }

    md->active_sid = BCMDRD_INVALID_ID;

    return;
}

/*!
 * \brief Create table metadata.
 *
 * This routine creates the PT Pass Thru metadata for the given table.
 *
 * Assumes:
 *   - unit and sid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_table_create(int unit,
                      bcmdrd_sid_t sid)
{
    md_pthru_t *md = md_pthru[unit];
    bcmltm_md_t *ltm_md = md->ltm_md;
    bcmltm_lt_md_t *lt_md = NULL;

    SHR_FUNC_ENTER(unit);

    md->active_sid = sid;

    /* Create table database */
    SHR_IF_ERR_EXIT
        (bcmltm_db_pthru_table_create(unit, sid));

    /* Create table opcodes metadata */
    SHR_IF_ERR_EXIT
        (bcmltm_md_pthru_op_table_create(unit, sid));

    /* Initialize table memory pool */
    SHR_IF_ERR_EXIT
        (md_pthru_table_mp_init(unit, &md->mp.table));

    /* Create table metadata */
    SHR_IF_ERR_EXIT
        (md_pthru_lt_md_create(unit, sid, &lt_md));
    ltm_md->lt_md[sid] = lt_md;
    ltm_md->lt_num++;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Reset table metadata.
 *
 * This routine resets the cached table metadata with a new table:
 * - If given table ID matches currently cached table ID, do nothing.
 * - If given table ID differs from currently cached table ID,
 *   destroy current table data and create metadata for new table.
 *
 * Assumes:
 *   - unit and sid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_table_reset(int unit,
                     bcmdrd_sid_t sid)
{
    md_pthru_t *md = md_pthru[unit];

    SHR_FUNC_ENTER(unit);

    /* If new table is current active table, just return */
    if (sid == md->active_sid) {
        SHR_EXIT();
    }

    /* Cleanup data for current active table (if valid) */
    if (md_pthru_sid_is_valid(unit, md->active_sid)) {
        md_pthru_table_destroy(unit, md->active_sid);
    }

    /* Create data for new active table */
    SHR_IF_ERR_EXIT
        (md_pthru_table_create(unit, sid));

 exit:
    if (SHR_FUNC_ERR()) {
        md_pthru_table_destroy(unit, sid);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the LTM metadata.
 *
 * This routine destroys the LTM metadata for given unit.
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
md_pthru_ltm_destroy(int unit,
                     bcmltm_md_t *ltm_md)
{
    bcmdrd_sid_t sid;
    bcmdrd_sid_t sid_max;

    if (ltm_md == NULL) {
        return;
    }

    sid_max = ltm_md->lt_max - 1;

    /* Destroy LT metadata for each table */
    for (sid = 0; sid <= sid_max; sid++) {
        md_pthru_lt_md_destroy(unit, ltm_md->lt_md[sid]);
        ltm_md->lt_md[sid] = NULL;
    }

    /* Free LTM metadata */
    SHR_FREE(ltm_md);

    return;
}

/*!
 * \brief Create the LTM metadata for given unit.
 *
 * This routine creates the LTM metadata for given unit.
 *
 * In order to optimize time and memory, the per-table metadata is not
 * created yet.  The metadata for a given table is created
 * on demand during a request for a new table operation.
 *
 * This implementation is based upon the symbols IDs
 * being in a compact numbering space from [0 .. (sid_max_count-1)]
 *
 * Assumes:
 *   - unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid_max_count Maximum number of tables IDs.
 * \param [out] ltm_md_ptr Returning pointer to LTM metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_ltm_create(int unit,
                    uint32_t sid_max_count,
                    bcmltm_md_t **ltm_md_ptr)
{
    unsigned int alloc_size;
    bcmdrd_sid_t sid_max;
    bcmltm_md_t *ltm_md = NULL;
    bcmdrd_sid_t *sid_list = NULL;
    size_t num_sid;
    size_t idx;
    uint32_t wb_wsize = 0;

    SHR_FUNC_ENTER(unit);

    /* SID max value */
    sid_max = sid_max_count - 1;

    /* Allocate sid list array */
    alloc_size = sizeof(*sid_list) * sid_max_count;
    if (alloc_size == 0) {
        /* No SIDs */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(sid_list, alloc_size, "bcmltmMdPthruSidArr");
    SHR_NULL_CHECK(sid_list, SHR_E_MEMORY);
    sal_memset(sid_list, 0, alloc_size);

    /* Get SID list */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_sid_list_get(unit, sid_max_count, sid_list, &num_sid));

    /* Verify that SID value fits design criteria [0 .. (max_count-1)] */
    for (idx = 0; idx < num_sid; idx++) {
        if (sid_list[idx] > sid_max) {
            const char *table_name;

            table_name = bcmltm_pt_table_sid_to_name(unit, sid_list[idx]);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Invalid SID %s(ptid=%d), "
                                  "SID should not be larger than %d\n"),
                       table_name, sid_list[idx], sid_max));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* Get max working buffer size for a PT Pass Thru table */
    wb_wsize = bcmltm_wb_ptm_pthru_wsize_max_get(unit,
                                                 BCMLTM_PTHRU_NUM_PT_OPS);

    /* Allocate LTM metadata structure */
    alloc_size = sizeof(bcmltm_md_t) +
        (sizeof(bcmltm_lt_md_t *) * sid_max_count);
    SHR_ALLOC(ltm_md, alloc_size, "bcmltmMdPthruMd");
    SHR_NULL_CHECK(ltm_md, SHR_E_MEMORY);
    sal_memset(ltm_md, 0, alloc_size);

    ltm_md->lt_num = 0;
    ltm_md->lt_max = sid_max_count;
    ltm_md->wb_max_modeled = 0;
    ltm_md->wb_max_interactive = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Table metadata is not created yet */
    for (idx = 0; idx < sid_max_count; idx++) {
        ltm_md->lt_md[idx] = NULL;
    }

    *ltm_md_ptr = ltm_md;

 exit:
    if (SHR_FUNC_ERR()) {
        md_pthru_ltm_destroy(unit, ltm_md);
        *ltm_md_ptr = NULL;
    }

    SHR_FREE(sid_list);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_md_pthru_create(int unit)
{
    size_t sid_max_count;
    md_pthru_t *md = NULL;
    bcmltm_md_t *ltm_md = NULL;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "LTM PT Pass Thru metadata create\n")));

    /* Check if metadata has been initialized */
    if (md_pthru_is_init(unit)) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "LTM PT Pass Thru metadata is already "
                             "created\n")));
        SHR_EXIT();
    }

    /* Get total table symbol IDs count */
    SHR_IF_ERR_EXIT(bcmdrd_pt_sid_list_get(unit, 0, NULL, &sid_max_count));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Total PT Pass Thru tables is %d\n"),
                 (int)sid_max_count));

    /* Allocate */
    SHR_ALLOC(md, sizeof(*md ), "bcmltmMdPthru");
    SHR_NULL_CHECK(md, SHR_E_MEMORY);
    sal_memset(md, 0, sizeof(*md));
    md_pthru[unit] = md;

    /* Initialize database */
    SHR_IF_ERR_EXIT
        (bcmltm_db_pthru_init(unit, sid_max_count));

    /* Initialize opcode metadata */
    SHR_IF_ERR_EXIT
        (bcmltm_md_pthru_op_init(unit, sid_max_count));

    /* Initialize memory pool */
    SHR_IF_ERR_EXIT
        (md_pthru_mp_init(unit, &md->mp));

    /* Initialize internal information */
    md->sid_max_count = sid_max_count;
    md->active_sid = BCMDRD_INVALID_ID;

    /* Create LTM metadata */
    SHR_IF_ERR_EXIT
        (md_pthru_ltm_create(unit, sid_max_count, &ltm_md));
    md->ltm_md = ltm_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_pthru_destroy(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmltm_md_pthru_destroy(int unit)
{
    md_pthru_t *md = NULL;

    SHR_FUNC_ENTER(unit);

    /* Check if it has been initialized */
    if (!md_pthru_is_init(unit)) {
        SHR_EXIT();
    }

    md = md_pthru[unit];

    /* Destroy LTM metadata */
    md_pthru_ltm_destroy(unit, md->ltm_md);
    md->ltm_md = NULL;

    /* Cleanup memory pool */
    md_pthru_mp_cleanup(&md->mp);

    /* Cleanup opcode metadata */
    bcmltm_md_pthru_op_cleanup(unit);

    /* Cleanup database */
    bcmltm_db_pthru_cleanup(unit);

    SHR_FREE(md);
    md_pthru[unit] = NULL;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_pthru_lt_retrieve(int unit,
                            uint32_t ltid,
                            bcmltm_lt_md_t **ltm_md_ptr)
{
    bcmdrd_sid_t sid;
    md_pthru_t *md;

    SHR_FUNC_ENTER(unit);

    *ltm_md_ptr = NULL;

    /* Check if metadata has been initialized */
    if (!md_pthru_is_init(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    sid = BCMLTM_LTID_TO_SID(ltid);

    if (!md_pthru_sid_is_valid(unit, sid)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    md = md_pthru[unit];

    /* Reset table cached information */
    SHR_IF_ERR_EXIT
        (md_pthru_table_reset(unit, sid));

    *ltm_md_ptr = md->ltm_md->lt_md[sid];

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_pthru_wb_max_get(int unit,
                           uint32_t *wb_max)
{
    md_pthru_t *md;

    SHR_FUNC_ENTER(unit);

    /* Check if metadata has been initialized */
    if (!md_pthru_is_init(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    md = md_pthru[unit];
    *wb_max = md->ltm_md->wb_max_interactive;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_pthru_key_field_list_retrieve(int unit,
                                        bcmdrd_sid_t sid,
                                        const bcmltm_field_map_list_t **list)
{
    const bcmltm_field_select_mapping_t *fields = NULL;

    SHR_FUNC_ENTER(unit);

    /* Check if metadata has been initialized */
    if (!md_pthru_is_init(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!md_pthru_sid_is_valid(unit, sid)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Reset table cached information */
    SHR_IF_ERR_EXIT
        (md_pthru_table_reset(unit, sid));

    SHR_IF_ERR_EXIT
        (bcmltm_db_pthru_key_fields_get(unit, sid, &fields));

    if ((fields == NULL) || (fields->num_maps == 0)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *list = fields->field_map_list[0];

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_pthru_value_field_list_retrieve(int unit,
                                          bcmdrd_sid_t sid,
                                          const bcmltm_field_map_list_t **list)
{
    const bcmltm_field_select_mapping_t *fields = NULL;

    SHR_FUNC_ENTER(unit);

    /* Check if metadata has been initialized */
    if (!md_pthru_is_init(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!md_pthru_sid_is_valid(unit, sid)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Reset table cached information */
    SHR_IF_ERR_EXIT
        (md_pthru_table_reset(unit, sid));

    SHR_IF_ERR_EXIT
        (bcmltm_db_pthru_value_fields_get(unit, sid, &fields));

    if ((fields == NULL) || (fields->num_maps == 0)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *list = fields->field_map_list[0];

 exit:
    SHR_FUNC_EXIT();
}
