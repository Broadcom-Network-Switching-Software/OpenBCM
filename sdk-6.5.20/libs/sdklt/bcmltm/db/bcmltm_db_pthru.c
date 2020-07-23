/*! \file bcmltm_db_pthru.c
 *
 * Logical Table Manager - Pass Thru Table Database.
 *
 * This file contains routines to create and provide
 * metadata information for Pass Thru tables.
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

#include <bcmltd/bcmltd_table.h>

#include <bcmltm/bcmltm_pt.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>
#include <bcmltm/bcmltm_db_pthru_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*
 * Special Fields
 *
 * The DRD does not provide field IDs for the index (memory/register array)
 * or port (port-based register) keys.
 */

/* Special key field min and max bits */
#define DB_PTHRU_FIELD_KEY_INDEX_MINBIT        0
#define DB_PTHRU_FIELD_KEY_INDEX_MAXBIT       31
#define DB_PTHRU_FIELD_KEY_PORT_MINBIT         0
#define DB_PTHRU_FIELD_KEY_PORT_MAXBIT        31
#define DB_PTHRU_FIELD_KEY_INSTANCE_MINBIT     0
#define DB_PTHRU_FIELD_KEY_INSTANCE_MAXBIT    31

/* Maximum number of field indexes for a wide field */
#define DB_PTHRU_MAX_FIELD_IDXS               15

/* Number of field select maps */
#define DB_PTHRU_NUM_FIELD_MAPS                1

/*!
 * \brief Information for a single mapped PT.
 *
 * This structure contains information for a single mapped physical table.
 * The data is used to construct the LTM metadata.  For instance, the
 * information indicates the Working Buffer content to use for the
 * PTM interface.
 */
typedef struct db_pthru_pt_map_s {
    /*! Physical table ID. */
    bcmdrd_sid_t sid;

    /*!
     * Bit flags to denote physical table parameters:
     *     BCMLTM_PT_OP_FLAGS_xxx
     * These are used to indicate the content and format
     * for the PTM parameter list and the entry data sections of the
     * generic working buffer layout.
     */
    uint32_t flags;

    /*! The number of PTM operations to perform for this PT. */
    uint32_t num_ops;

    /*! Working buffer block ID for this PT. */
    bcmltm_wb_block_id_t wb_block_id;

    /*! Working buffer block pointer for this PT. */
    const bcmltm_wb_block_t *wb_block;
} db_pthru_pt_map_t;

/*!
 * \brief PT Mapping Information.
 *
 * This structure contains the information for all the mapped physical
 * tables that correspond to a source table.
 *
 * For a PT Pass Thru table, there is only one mapped physical table
 * (one PT view) per source table.
 */
typedef struct db_pthru_pt_map_info_s {
    /*! Number of different physical tables mapped to a LTM table. */
    uint32_t num_pts;

    /*! Array of mapped PTs. */
    db_pthru_pt_map_t pt_maps[BCMLTM_PTHRU_NUM_PT_VIEWS];
} db_pthru_pt_map_info_t;

/*!
 * Table Memory Pool.
 *
 * This structure contains a series of definitions used for
 * getting memory needed for creating the database information for a table.
 *
 * This offers an alternative to allocating and freeing heap memory
 * for each table operation with a new table ID.  This method helps
 * reduce memory fragmentation for PT Pass Thru.
 */
typedef struct db_pthru_table_mp_s {
    /*! PT mapping information memory. */
    db_pthru_pt_map_info_t pt_map_info;

    /*! Default PT dynamic info memory. */
    bcmbd_pt_dyn_info_t pt_dyn_info[BCMLTM_PTHRU_NUM_PT_VIEWS];

    /*! Default Hash PT dynamic memory. */
    bcmbd_pt_dyn_hash_info_t pt_dyn_hash_info[BCMLTM_PTHRU_NUM_PT_VIEWS];

    /*! PT mem args array memory. */
    bcmltm_pt_mem_args_t mem_args[BCMLTM_PTHRU_NUM_PT_VIEWS];

    /*! PT mem args pointers array memory. */
    bcmltm_pt_mem_args_t *mem_args_ptrs[BCMLTM_PTHRU_NUM_PT_VIEWS];

    /*! PT parameter working buffer offset memory. */
    uint32_t param_offsets[BCMLTM_PTHRU_NUM_PT_VIEWS][BCMLTM_PTHRU_NUM_PT_OPS];

    /*! PT entry working buffer offset memory. */
    uint32_t buffer_offsets[BCMLTM_PTHRU_NUM_PT_VIEWS][BCMLTM_PTHRU_NUM_PT_OPS];

    /*! PT mem operations list array memory. */
    bcmltm_pt_op_list_t op_list[BCMLTM_PTHRU_NUM_PT_VIEWS];

    /*! PT mem operations list pointers array memory. */
    bcmltm_pt_op_list_t *op_list_ptrs[BCMLTM_PTHRU_NUM_PT_VIEWS];

    /*! PT list metadata memory. */
    bcmltm_pt_list_t pt_list;

    /*!
     * Field map list metadata memory.
     * NOTE: Memory for field mapping struct bcmltm_field_select_mapping_t
     * cannot be defined because it includes a flexible array member.
     */
    bcmltm_field_map_list_t field_map_list[BCMLTM_FIELD_TYPE_COUNT]
                                          [DB_PTHRU_NUM_FIELD_MAPS];
} db_pthru_table_mp_t;

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
typedef struct db_pthru_mp_s {
    /*! Memory pool (1 table). */
    db_pthru_table_mp_t table;
} db_pthru_mp_t;

/*!
 * \brief Table Database Information.
 *
 * This structure contains various data for a table
 * used during LTM metadata construction, such as FA and EE node cookies
 * referenced by the LTM opcode trees.
 */
typedef struct db_pthru_table_info_s {
    /*! Physical table ID. */
    bcmdrd_sid_t sid;

    /* Working buffer handle. */
    bcmltm_wb_handle_t wb_handle;

    /* PT mapping information. */
    db_pthru_pt_map_info_t *pt_map_info;

    /*! List of mapped physical tables used as the EE node cookie. */
    bcmltm_pt_list_t *pt_list;

    /*! Key fields mapping used as the FA Keys node cookie. */
    bcmltm_field_select_mapping_t *keys;

    /*! Value fields mapping used as the FA Values node cookie. */
    bcmltm_field_select_mapping_t *values;
} db_pthru_table_info_t;

/*!
 * \brief Database Information.
 *
 * This structure contains various data used during
 * LTM metadata construction, such as FA and EE node cookies
 * referenced by the LTM opcode trees.
 *
 * This structure contains the information for one table.
 * This matches the current PT Pass Thru design (only 1 PT metadata
 * cached at a time).  If there is a need to cache more tables,
 * the member can be expanded to an array.
 *
 * Optimization:
 * If there is certainty that the design will remain unchanged
 * (store no more than one table), the per-table structure can be
 * removed and its content moved here.
 */
typedef struct db_pthru_info_s {
    /*! Database information (1 table). */
    db_pthru_table_info_t table;
} db_pthru_info_t;

/*!
 * \brief PT Pass Thru Database.
 *
 * This structure contains the PT Pass Thru database.
 */
typedef struct db_pthru_s {
    /*! Total number of symbols. */
    uint32_t sid_max_count;

    /*! Memory pool container. */
    db_pthru_mp_t mp;

    /* Database information. */
    db_pthru_info_t info;
} db_pthru_t;

/* PT Pass Thru Database */
static db_pthru_t *db_pthru[BCMLTM_MAX_UNITS];


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Check if database has been initialized.
 *
 * This routine checks if the PT Pass Thru has been initialized
 * for the given unit.
 *
 * \param [in] unit Unit number.
 *
 * \retval TRUE If database has been initialized.
 * \retval FALSE If database has not been initialized or unit is invalid.
 */
static inline bool
db_pthru_is_init(int unit)
{
    if ((unit < 0) || (unit >= BCMLTM_MAX_UNITS)) {
        return FALSE;
    }

    return (db_pthru[unit] != NULL);
}

/*!
 * \brief Check if table ID is valid.
 *
 * This routine checks if given physical table ID is valid.
 * It assumes that the unit is valid and the database has been initialized.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 *
 * \retval TRUE If table ID is valid.
 * \retval FALSE If table ID is invalid.
 */
static inline bool
db_pthru_sid_is_valid(int unit,
                      bcmdrd_sid_t sid)
{
    if (sid >= db_pthru[unit]->sid_max_count) {
        return FALSE;
    }

    return bcmdrd_pt_is_valid(unit, sid);
}

/*!
 * \brief Check if table ID and key field ID are valid.
 *
 * This routine checks if the given physical table ID and
 * key field ID are valid.
 *
 * It assumes that the unit is valid and the database has been initialized.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] fid Physical key field ID.
 *
 * \retval TRUE If table ID and key field ID are valid.
 * \retval FALSE If table ID or key field ID is invalid.
 */
static bool
db_pthru_key_field_is_valid(int unit,
                            bcmdrd_sid_t sid,
                            bcmdrd_fid_t fid)
{
    bool valid =  FALSE;
    uint32_t flags;

    if (!db_pthru_sid_is_valid(unit, sid)) {
        return FALSE;
    }

    flags = bcmltm_wb_ptm_op_flags_pthru_get(unit, sid);

    /* Check for PT keys */
    if ((fid == BCMLTM_PT_FIELD_KEY_INDEX) &&
        (flags & BCMLTM_PT_OP_FLAGS_INDEX)) {
        valid = TRUE;
    } else if ((fid == BCMLTM_PT_FIELD_KEY_PORT) &&
               (flags & BCMLTM_PT_OP_FLAGS_PORT)) {
        valid = TRUE;
    } else if ((fid == BCMLTM_PT_FIELD_KEY_INSTANCE) &&
               (flags & BCMLTM_PT_OP_FLAGS_TABLE_INST)) {
        valid = TRUE;
    }

    return valid;
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
 * \brief Allocate memory for db_pthru_pt_map_info_t struct.
 *
 * This routine returns memory for db_pthru_pt_map_info_t struct
 * that can accommodate the given number of PT views.
 *
 * \param [in] unit Unit number.
 * \param [in] num_pt_views Number of PT views.
 *
 * \retval Pointer to requested memory.
 * \retval NULL on error.
 */
static inline db_pthru_pt_map_info_t *
db_pthru_pt_map_info_alloc(int unit,
                           uint32_t num_pt_views)
{
    if (num_pt_views > BCMLTM_PTHRU_NUM_PT_VIEWS) {
        return NULL;
    }

    return &db_pthru[unit]->mp.table.pt_map_info;
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
db_pthru_pt_map_info_free(db_pthru_pt_map_info_t *ptr)
{
    /* Nothing to do */
    return;
}

/*!
 * \brief Allocate memory for bcmbd_pt_dyn_info_t struct.
 *
 * This routine returns memory for bcmbd_pt_dyn_info_t struct
 * for the given PT view.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_view PT view.
 *
 * \retval Pointer to requested memory.
 * \retval NULL on error.
 */
static inline bcmbd_pt_dyn_info_t *
db_pthru_pt_dyn_info_alloc(int unit,
                           uint32_t pt_view)
{
    if (pt_view >= BCMLTM_PTHRU_NUM_PT_VIEWS) {
        return NULL;
    }

    return &db_pthru[unit]->mp.table.pt_dyn_info[pt_view];
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
db_pthru_pt_dyn_info_free(bcmbd_pt_dyn_info_t *ptr)
{
    /* Nothing to do */
    return;
}

/*!
 * \brief Allocate memory for bcmbd_pt_dyn_hash_info_t struct.
 *
 * This routine returns memory for bcmbd_pt_dyn_hash_info_t struct
 * for the given PT view.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_view PT view.
 *
 * \retval Pointer to requested memory.
 * \retval NULL on error.
 */
static inline bcmbd_pt_dyn_hash_info_t *
db_pthru_pt_dyn_hash_info_alloc(int unit,
                                uint32_t pt_view)
{
    if (pt_view >= BCMLTM_PTHRU_NUM_PT_VIEWS) {
        return NULL;
    }

    return &db_pthru[unit]->mp.table.pt_dyn_hash_info[pt_view];
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
db_pthru_pt_dyn_hash_info_free(bcmbd_pt_dyn_hash_info_t *ptr)
{
    /* Nothing to do */
    return;
}

/*!
 * \brief Allocate memory for bcmltm_pt_mem_args_t struct.
 *
 * This routine returns memory for bcmltm_pt_mem_args_t struct
 * for the given PT view.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_view PT view.
 *
 * \retval Pointer to requested memory.
 * \retval NULL on error.
 */
static inline bcmltm_pt_mem_args_t *
db_pthru_pt_mem_args_alloc(int unit,
                           uint32_t pt_view)
{
    if (pt_view >= BCMLTM_PTHRU_NUM_PT_VIEWS) {
        return NULL;
    }

    return &db_pthru[unit]->mp.table.mem_args[pt_view];
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
db_pthru_pt_mem_args_free(bcmltm_pt_mem_args_t *ptr)
{
    /* Nothing to do */
    return;
}

/*!
 * \brief Allocate memory for array of pointers to bcmltm_pt_mem_args_t.
 *
 * This routine returns memory for array of pointers to bcmltm_pt_mem_args_t.
 *
 * \param [in] unit Unit number.
 * \param [in] num_pt_views Number of PT views.
 *
 * \retval Pointer to requested memory.
 * \retval NULL on error.
 */
static inline bcmltm_pt_mem_args_t **
db_pthru_pt_mem_args_ptrs_alloc(int unit,
                                uint32_t num_pt_views)
{
    if (num_pt_views > BCMLTM_PTHRU_NUM_PT_VIEWS) {
        return NULL;
    }

    return &db_pthru[unit]->mp.table.mem_args_ptrs[0];
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
db_pthru_pt_mem_args_ptrs_free(bcmltm_pt_mem_args_t **ptr)
{
    /* Nothing to do */
    return;
}

/*!
 * \brief Allocate memory for array of param_offsets.
 *
 * This routine returns memory for array of param offsets
 * for given PT view.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_view PT view.
 * \param [in] num_ops Number of PT ops.
 *
 * \retval Pointer to requested memory.
 * \retval NULL on error.
 */
static inline uint32_t *
db_pthru_pt_param_offsets_alloc(int unit,
                                uint32_t pt_view,
                                uint32_t num_ops)
{
    if (pt_view >= BCMLTM_PTHRU_NUM_PT_VIEWS) {
        return NULL;
    }
    if (num_ops > BCMLTM_PTHRU_NUM_PT_OPS) {
        return NULL;
    }

    return &db_pthru[unit]->mp.table.param_offsets[pt_view][0];
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
db_pthru_pt_param_offsets_free(uint32_t *ptr)
{
    /* Nothing to do */
    return;
}

/*!
 * \brief Allocate memory for buffer_offsets.
 *
 * This routine returns memory for array of buffer offsets
 * for given PT view.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_view PT view.
 * \param [in] num_ops Number of PT ops.
 *
 * \retval Pointer to requested memory.
 * \retval NULL on error.
 */
static inline uint32_t *
db_pthru_pt_buffer_offsets_alloc(int unit,
                                 uint32_t pt_view,
                                 uint32_t num_ops)
{
    if (pt_view >= BCMLTM_PTHRU_NUM_PT_VIEWS) {
        return NULL;
    }
    if (num_ops > BCMLTM_PTHRU_NUM_PT_OPS) {
        return NULL;
    }

    return &db_pthru[unit]->mp.table.buffer_offsets[pt_view][0];
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
db_pthru_pt_buffer_offsets_free(uint32_t *ptr)
{
    /* Nothing to do */
    return;
}

/*!
 * \brief Allocate memory for bcmltm_pt_op_list_t struct.
 *
 * This routine returns memory for bcmltm_pt_op_list_t struct
 * for the given PT view.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_view PT view.
 *
 * \retval Pointer to requested memory.
 * \retval NULL on error.
 */
static inline bcmltm_pt_op_list_t *
db_pthru_pt_op_list_alloc(int unit,
                          uint32_t pt_view)
{
    if (pt_view >= BCMLTM_PTHRU_NUM_PT_VIEWS) {
        return NULL;
    }

    return &db_pthru[unit]->mp.table.op_list[pt_view];
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
db_pthru_pt_op_list_free(bcmltm_pt_op_list_t *ptr)
{
    /* Nothing to do */
    return;
}

/*!
 * \brief Allocate memory for array of pointers to bcmltm_pt_op_list_t.
 *
 * This routine returns memory for array of pointers to bcmltm_pt_op_list_t.
 *
 * \param [in] unit Unit number.
 * \param [in] num_pt_views Number of PT views.
 *
 * \retval Pointer to requested memory.
 * \retval NULL on error.
 */
static inline bcmltm_pt_op_list_t **
db_pthru_pt_op_list_ptrs_alloc(int unit,
                               uint32_t num_pt_views)
{
    if (num_pt_views > BCMLTM_PTHRU_NUM_PT_VIEWS) {
        return NULL;
    }

    return &db_pthru[unit]->mp.table.op_list_ptrs[0];
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
db_pthru_pt_op_list_ptrs_free(bcmltm_pt_op_list_t **ptr)
{
    /* Nothing to do */
    return;
}

/*!
 * \brief Allocate memory for bcmltm_pt_list_t struct.
 *
 * This routine returns memory for bcmltm_pt_list_t struct.
 *
 * \param [in] unit Unit number.
 *
 * \retval Pointer to requested memory.
 * \retval NULL on error.
 */
static inline bcmltm_pt_list_t *
db_pthru_pt_list_alloc(int unit)
{
    return &db_pthru[unit]->mp.table.pt_list;
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
db_pthru_pt_list_free(bcmltm_pt_list_t *ptr)
{
    /* Nothing to do */
    return;
}

/*!
 * \brief Allocate memory for bcmltm_field_map_list_t struct.
 *
 * This routine returns memory for bcmltm_field_map_list_t struct
 * for the given field type and field selection map index.
 *
 * \param [in] unit Unit number.
 * \param [in] field_type Field type.
 * \param [in] field_map_idx Field selection mapping index.
 *
 * \retval Pointer to requested memory.
 * \retval NULL on error.
 */
static inline bcmltm_field_map_list_t *
db_pthru_field_map_list_alloc(int unit,
                              bcmltm_field_type_t field_type,
                              uint32_t field_map_idx)
{
    if (field_type >= BCMLTM_FIELD_TYPE_COUNT) {
        return NULL;
    }
    if (field_map_idx >= DB_PTHRU_NUM_FIELD_MAPS) {
        return NULL;
    }

    return &db_pthru[unit]->mp.table.field_map_list[field_type][field_map_idx];
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
db_pthru_field_map_list_free(bcmltm_field_map_list_t *ptr)
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
db_pthru_table_mp_cleanup(db_pthru_table_mp_t *mp)
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
db_pthru_table_mp_init(int unit,
                       db_pthru_table_mp_t *mp)
{
    /* Nothing to do */
    return SHR_E_NONE;
}

/*!
 * \brief Cleanup memory pool container
 *
 * This routine frees any necessary resources for the given
 * memory pool container.
 *
 * \param [in] mp Memory pool container.
 */
static inline void
db_pthru_mp_cleanup(db_pthru_mp_t *mp)
{
    db_pthru_table_mp_cleanup(&mp->table);
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
db_pthru_mp_init(int unit,
                 db_pthru_mp_t *mp)
{
    return db_pthru_table_mp_init(unit, &mp->table);
}

/*!
 * \brief Destroy the given PT dynamic info metadata.
 *
 * This routine destroys the given PT dynamic info metadata.
 *
 * \param [in] pt_dyn_info Pointer to PT dynamic info to destroy.
 */
static inline void
db_pthru_pt_dyn_info_destroy(bcmbd_pt_dyn_info_t *pt_dyn_info)
{
    if (pt_dyn_info == NULL) {
        return;
    }

    db_pthru_pt_dyn_info_free(pt_dyn_info);

    return;
}

/*!
 * \brief Create the PT dynamic info metadata.
 *
 * This routine creates the PT dynamic info metadata for the given PT view.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_view PT view.
 * \param [out] pt_dyn_info_ptr Returning pointer to PT dynamic info metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_pt_dyn_info_create(int unit,
                            uint32_t pt_view,
                            bcmbd_pt_dyn_info_t **pt_dyn_info_ptr)
{
    bcmbd_pt_dyn_info_t *pt_dyn_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    pt_dyn_info = db_pthru_pt_dyn_info_alloc(unit, pt_view);
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_MEMORY);
    sal_memset(pt_dyn_info, 0, sizeof(*pt_dyn_info));

    /* Fill data */
    pt_dyn_info->index = 0;
    pt_dyn_info->tbl_inst = -1;

    *pt_dyn_info_ptr = pt_dyn_info;

 exit:
    if (SHR_FUNC_ERR()) {
        db_pthru_pt_dyn_info_destroy(pt_dyn_info);
        *pt_dyn_info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given Hash PT dynamic info metadata.
 *
 * This routine destroys the given Hash PT dynamic info metadata.
 *
 * \param [in] pt_dyn_hash_info Pointer to Hash PT dynamic info to destroy.
 */
static inline void
db_pthru_pt_dyn_hash_info_destroy(bcmbd_pt_dyn_hash_info_t *pt_dyn_hash_info)
{
    if (pt_dyn_hash_info == NULL) {
        return;
    }

    db_pthru_pt_dyn_hash_info_free(pt_dyn_hash_info);

    return;
}

/*!
 * \brief Create the Hash PT dynamic info metadata.
 *
 * This routine creates the Hash PT dynamic info metadata
 * for the given PT view.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_view PT view.
 * \param [out] pt_dyn_hash_info_ptr Returning pointer to Hash PT dynamic info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_pt_dyn_hash_info_create(int unit,
                        uint32_t pt_view,
                        bcmbd_pt_dyn_hash_info_t **pt_dyn_hash_info_ptr)
{
    bcmbd_pt_dyn_hash_info_t *pt_dyn_hash_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    pt_dyn_hash_info = db_pthru_pt_dyn_hash_info_alloc(unit, pt_view);
    SHR_NULL_CHECK(pt_dyn_hash_info, SHR_E_MEMORY);
    sal_memset(pt_dyn_hash_info, 0, sizeof(*pt_dyn_hash_info));

    /* Fill data */
    pt_dyn_hash_info->tbl_inst = -1;
    pt_dyn_hash_info->bank = 0;

    *pt_dyn_hash_info_ptr = pt_dyn_hash_info;

 exit:
    if (SHR_FUNC_ERR()) {
        db_pthru_pt_dyn_hash_info_destroy(pt_dyn_hash_info);
        *pt_dyn_hash_info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given PT mem args metadata.
 *
 * This routine destroys the given PT mem args metadata.
 *
 * \param [in] mem_args Pointer to PT mem args metadata to destroy.
 */
static inline void
db_pthru_pt_mem_args_destroy(bcmltm_pt_mem_args_t *mem_args)
{
    if (mem_args == NULL) {
        return;
    }

    db_pthru_pt_dyn_hash_info_destroy(mem_args->pt_dyn_hash_info);
    db_pthru_pt_dyn_info_destroy(mem_args->pt_dyn_info);

    db_pthru_pt_mem_args_free(mem_args);

    return;
}

/*!
 * \brief Create the PT mem args metadata.
 *
 * This routine creates the PT mem args metadata for the given table
 * and PT view.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] pt_view PT view.
 * \param [out] mem_args_ptr Returning pointer to PT mem args metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_pt_mem_args_create(int unit,
                            bcmdrd_sid_t sid,
                            uint32_t pt_view,
                            bcmltm_pt_mem_args_t **mem_args_ptr)
{
    bcmltm_pt_mem_args_t *mem_args = NULL;
    uint64_t flags = 0;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    mem_args = db_pthru_pt_mem_args_alloc(unit, pt_view);
    SHR_NULL_CHECK(mem_args, SHR_E_MEMORY);
    sal_memset(mem_args, 0, sizeof(*mem_args));

    /* Indicates PTM access is for PT PassThru (rather than LT interactive) */
    flags |= BCMPTM_REQ_FLAGS_PASSTHRU;

    /* Fill data */
    mem_args->flags = flags;
    mem_args->pt = sid;
    mem_args->pt_static_info = NULL;

    SHR_IF_ERR_EXIT
        (db_pthru_pt_dyn_info_create(unit, pt_view,
                                     &mem_args->pt_dyn_info));

    if (bcmdrd_pt_attr_is_hashed(unit, sid)) {
        SHR_IF_ERR_EXIT
            (db_pthru_pt_dyn_hash_info_create(unit, pt_view,
                                              &mem_args->pt_dyn_hash_info));
    } else {
        mem_args->pt_dyn_hash_info = NULL;
    }

    *mem_args_ptr = mem_args;

 exit:
    if (SHR_FUNC_ERR()) {
        db_pthru_pt_mem_args_destroy(mem_args);
        *mem_args_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given list of pointers to PT mem args metadata.
 *
 * This routine destroys the given list of pointers to PT mem args metadata.
 *
 * \param [in] num_pt_views Number of PT view entries in list.
 * \param [in] mem_args_ptrs List of pointers to PT mem args list.
 */
static void
db_pthru_pt_mem_args_ptrs_destroy(uint32_t num_pt_views,
                                  bcmltm_pt_mem_args_t **mem_args_ptrs)
{
    uint32_t pt_view;

    if (mem_args_ptrs == NULL) {
        return;
    }

    for (pt_view = 0; pt_view < num_pt_views; pt_view++) {
        if (mem_args_ptrs[pt_view] == NULL) {
            continue;
        }
        db_pthru_pt_mem_args_destroy(mem_args_ptrs[pt_view]);
    }

    db_pthru_pt_mem_args_ptrs_free(mem_args_ptrs);

    return;
}

/*!
 * \brief Create the list of pointers to PT mem args metadata.
 *
 * This routine creates the list of pointers to PT mem args metadata
 * for the given list of physical table IDs.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_map_info PT mapping information.
 * \param [out] mem_args_ptrs_ptr Returning pointer to PT mem args list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_pt_mem_args_ptrs_create(int unit,
                                 const db_pthru_pt_map_info_t *pt_map_info,
                                 bcmltm_pt_mem_args_t ***mem_args_ptrs_ptr)
{
    unsigned int alloc_size;
    uint32_t num_pt_views = pt_map_info->num_pts;
    uint32_t pt_view;
    bcmltm_pt_mem_args_t **mem_args_ptrs = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    alloc_size = sizeof(bcmltm_pt_mem_args_t *) * num_pt_views;
    if (alloc_size == 0) {
        /* No PT views */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    mem_args_ptrs = db_pthru_pt_mem_args_ptrs_alloc(unit, num_pt_views);
    SHR_NULL_CHECK(mem_args_ptrs, SHR_E_MEMORY);
    sal_memset(mem_args_ptrs, 0, alloc_size);

    for (pt_view = 0; pt_view < num_pt_views; pt_view++) {
        SHR_IF_ERR_EXIT
            (db_pthru_pt_mem_args_create(unit,
                                         pt_map_info->pt_maps[pt_view].sid,
                                         pt_view,
                                         &mem_args_ptrs[pt_view]));
    }

    *mem_args_ptrs_ptr = mem_args_ptrs;

 exit:
    if (SHR_FUNC_ERR()) {
        db_pthru_pt_mem_args_ptrs_destroy(num_pt_views, mem_args_ptrs);
        *mem_args_ptrs_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given PT param offsets metadata.
 *
 * This routine destroys the given PT param offsets metadata.
 *
 * \param [in] param_offsets Pointer to PT param offsets metadata to destroy.
 */
static inline void
db_pthru_pt_param_offsets_destroy(uint32_t *param_offsets)
{
    if (param_offsets == NULL) {
        return;
    }

    db_pthru_pt_param_offsets_free(param_offsets);

    return;
}

/*!
 * \brief Create the PT param offsets metadata.
 *
 * This routine creates the PT parameter indexes metadata for
 * the given table map and PT view.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_map Table PT map information.
 * \param [in] pt_view PT view.
 * \param [out] param_offsets_ptr Returning pointer to PT param offsets.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_pt_param_offsets_create(int unit,
                                 const db_pthru_pt_map_t *pt_map,
                                 uint32_t pt_view,
                                 uint32_t **param_offsets_ptr)
{
    uint32_t *param_offsets = NULL;
    unsigned int alloc_size;
    uint32_t num_ops = pt_map->num_ops;
    const bcmltm_wb_block_t *wb_block = pt_map->wb_block;
    uint32_t op_idx;

    SHR_FUNC_ENTER(unit);

    /* Allocate for all PT ops needed for this PT view */
    alloc_size = sizeof(uint32_t) * num_ops;
    if (alloc_size == 0) {
        /* No ops */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    param_offsets = db_pthru_pt_param_offsets_alloc(unit, pt_view, num_ops);
    SHR_NULL_CHECK(param_offsets, SHR_E_MEMORY);
    sal_memset(param_offsets, 0, alloc_size);

    for (op_idx = 0; op_idx < num_ops; op_idx++) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_ptm_param_base_offset_get(wb_block,
                                                 op_idx,
                                                 &param_offsets[op_idx]));
    }

    *param_offsets_ptr = param_offsets;

exit:
    if (SHR_FUNC_ERR()) {
        db_pthru_pt_param_offsets_destroy(param_offsets);
        *param_offsets_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given PT buffer offsets metadata.
 *
 * This routine destroys the given PT buffer offsets metadata.
 *
 * \param [in] buffer_offsets Pointer to PT buffer offsets to destroy
 */
static inline void
db_pthru_pt_buffer_offsets_destroy(uint32_t *buffer_offsets)
{
    if (buffer_offsets == NULL) {
        return;
    }

    db_pthru_pt_buffer_offsets_free(buffer_offsets);

    return;
}

/*!
 * \brief Create the PT buffer offsets metadata.
 *
 * This routine creates the PT buffer offsets metadata for the given PT.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_map Table PT map information.
 * \param [in] pt_view PT view.
 * \param [out] buffer_offsets_ptr Returning pointer to PT buffer offsets md.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_pt_buffer_offsets_create(int unit,
                                  const db_pthru_pt_map_t *pt_map,
                                  uint32_t pt_view,
                                  uint32_t **buffer_offsets_ptr)
{
    uint32_t *buffer_offsets = NULL;
    unsigned int alloc_size;
    uint32_t num_ops = pt_map->num_ops;
    const bcmltm_wb_block_t *wb_block = pt_map->wb_block;
    uint32_t op_idx;

    SHR_FUNC_ENTER(unit);

    /* Allocate for all PT (ops) needed for this PT view */
    alloc_size = sizeof(uint32_t) * num_ops;
    if (alloc_size == 0) {
        /* No ops */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    buffer_offsets = db_pthru_pt_buffer_offsets_alloc(unit, pt_view, num_ops);
    SHR_NULL_CHECK(buffer_offsets, SHR_E_MEMORY);
    sal_memset(buffer_offsets, 0, alloc_size);

    for (op_idx = 0; op_idx < num_ops; op_idx++) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_ptm_entry_base_offset_get(wb_block,
                                                 op_idx,
                                                 &buffer_offsets[op_idx]));
    }

    *buffer_offsets_ptr = buffer_offsets;

exit:
    if (SHR_FUNC_ERR()) {
        db_pthru_pt_buffer_offsets_destroy(buffer_offsets);
        *buffer_offsets_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given PT op list metadata.
 *
 * This routine destroys the given PT op list metadata.
 *
 * \param [in] op_list Pointer to PT op list metadata to destroy.
 */
static inline void
db_pthru_pt_op_list_destroy(bcmltm_pt_op_list_t *op_list)
{
    if (op_list == NULL) {
        return;
    }

    db_pthru_pt_buffer_offsets_destroy(op_list->buffer_offsets);
    db_pthru_pt_param_offsets_destroy(op_list->param_offsets);

    db_pthru_pt_op_list_free(op_list);

    return;
}

/*!
 * \brief Create the PT op list metadata.
 *
 * This routine creates the PT op list metadata for the given table map
 * and PT view.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_map Table PT map information.
 * \param [in] pt_view PT view.
 * \param [out] op_list_ptr Returning pointer to PT op list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_pt_op_list_create(int unit,
                           const db_pthru_pt_map_t *pt_map,
                           uint32_t pt_view,
                           bcmltm_pt_op_list_t **op_list_ptr)
{
    bcmltm_pt_op_list_t *op_list = NULL;
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);

    sid = pt_map->sid;

    /* Allocate */
    op_list = db_pthru_pt_op_list_alloc(unit, pt_view);
    SHR_NULL_CHECK(op_list, SHR_E_MEMORY);
    sal_memset(op_list, 0, sizeof(*op_list));

    op_list->flags = pt_map->flags;
    op_list->num_pt_ops = pt_map->num_ops;
    op_list->word_size = bcmdrd_pt_entry_wsize(unit, sid);

    SHR_IF_ERR_EXIT
        (db_pthru_pt_param_offsets_create(unit, pt_map, pt_view,
                                          &op_list->param_offsets));
    SHR_IF_ERR_EXIT
        (db_pthru_pt_buffer_offsets_create(unit, pt_map, pt_view,
                                           &op_list->buffer_offsets));

    *op_list_ptr = op_list;

 exit:
    if (SHR_FUNC_ERR()) {
        db_pthru_pt_op_list_destroy(op_list);
        *op_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given list of pointers to PT op list metadata.
 *
 * This routine destroys the given list of pointers to PT op list metadata.
 *
 * \param [in] num_pt_views Number of entries in list.
 * \param [in] op_list_ptrs List of pointers to PT op list.
 */
static void
db_pthru_pt_op_list_ptrs_destroy(uint32_t num_pt_views,
                                 bcmltm_pt_op_list_t **op_list_ptrs)
{
    uint32_t pt_view;

    if (op_list_ptrs == NULL) {
        return;
    }

    for (pt_view = 0; pt_view < num_pt_views; pt_view++) {
        if (op_list_ptrs[pt_view] == NULL) {
            continue;
        }
        db_pthru_pt_op_list_destroy(op_list_ptrs[pt_view]);
    }

    db_pthru_pt_op_list_ptrs_free(op_list_ptrs);

    return;
}

/*!
 * \brief Create the list of pointers to PT op list metadata.
 *
 * This routine creates the list of pointers to PT op list metadata
 * for the given list of physical table IDs.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_map_info PT mapping information.
 * \param [in] wb_handle Working buffer handle.
 * \param [out] op_list_ptrs_ptr Returning pointer to list of PT op list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_pt_op_list_ptrs_create(int unit,
                                const db_pthru_pt_map_info_t *pt_map_info,
                                const bcmltm_wb_handle_t *wb_handle,
                                bcmltm_pt_op_list_t ***op_list_ptrs_ptr)
{
    unsigned int alloc_size;
    uint32_t pt_view;
    uint32_t num_pt_views = pt_map_info->num_pts;
    bcmltm_pt_op_list_t **op_list_ptrs = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    alloc_size = sizeof(bcmltm_pt_op_list_t *) * num_pt_views;
    if (alloc_size == 0) {
        /* No PT views */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    op_list_ptrs = db_pthru_pt_op_list_ptrs_alloc(unit, num_pt_views);
    SHR_NULL_CHECK(op_list_ptrs, SHR_E_MEMORY);
    sal_memset(op_list_ptrs, 0, alloc_size);

    for (pt_view = 0; pt_view < num_pt_views; pt_view++) {
        SHR_IF_ERR_EXIT
            (db_pthru_pt_op_list_create(unit,
                                        &pt_map_info->pt_maps[pt_view],
                                        pt_view,
                                        &op_list_ptrs[pt_view]));
    }

    *op_list_ptrs_ptr = op_list_ptrs;

 exit:
    if (SHR_FUNC_ERR()) {
        db_pthru_pt_op_list_ptrs_destroy(num_pt_views, op_list_ptrs);
        *op_list_ptrs_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given PT list metadata.
 *
 * This routine destroys the given PT list metadata.
 *
 * \param [in] pt_list List of PT metadata.
 */
static inline void
db_pthru_pt_list_destroy(bcmltm_pt_list_t *pt_list)
{
    if (pt_list == NULL) {
        return;
    }

    db_pthru_pt_op_list_ptrs_destroy(pt_list->num_pt_view,
                                     pt_list->memop_lists);
    db_pthru_pt_mem_args_ptrs_destroy(pt_list->num_pt_view,
                                      pt_list->mem_args);

    db_pthru_pt_list_free(pt_list);

    return;
}

/*!
 * \brief Create the PT list metadata.
 *
 * This routine creates the PT list metadata for the given
 * list of mapped physical table IDs.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_map_info PT mapping information.
 * \param [in] wb_handle Working buffer handle.
 * \param [out] pt_list_ptr Returning pointer PT list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_pt_list_create(int unit,
                        const db_pthru_pt_map_info_t *pt_map_info,
                        const bcmltm_wb_handle_t *wb_handle,
                        bcmltm_pt_list_t **pt_list_ptr)
{
    bcmltm_pt_list_t *pt_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    pt_list = db_pthru_pt_list_alloc(unit);
    SHR_NULL_CHECK(pt_list, SHR_E_MEMORY);
    sal_memset(pt_list, 0, sizeof(*pt_list));

    /* Fill data */
    pt_list->num_pt_view = pt_map_info->num_pts;

    SHR_IF_ERR_EXIT
        (db_pthru_pt_mem_args_ptrs_create(unit, pt_map_info,
                                          &pt_list->mem_args));
    SHR_IF_ERR_EXIT
        (db_pthru_pt_op_list_ptrs_create(unit, pt_map_info, wb_handle,
                                         &pt_list->memop_lists));

    *pt_list_ptr = pt_list;

 exit:
    if (SHR_FUNC_ERR()) {
        db_pthru_pt_list_destroy(pt_list);
        *pt_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the minimum and maximum bit positions for given field.
 *
 * This routine returns the minimum and maximum bit positions for given field.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] fid Field ID.
 * \param [out] minbit Returning minimum bit position.
 * \param [out] maxbit Returning maximum bit position.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_field_min_max_bit_get(int unit,
                               bcmdrd_sid_t sid,
                               bcmdrd_fid_t fid,
                               uint16_t *minbit,
                               uint16_t *maxbit)
{
    *minbit = 0;
    *maxbit = 0;

    if (fid == BCMLTM_PT_FIELD_KEY_INDEX) {
        *minbit = DB_PTHRU_FIELD_KEY_INDEX_MINBIT;
        *maxbit = DB_PTHRU_FIELD_KEY_INDEX_MAXBIT;
    } else if (fid == BCMLTM_PT_FIELD_KEY_PORT) {
        *minbit = DB_PTHRU_FIELD_KEY_PORT_MINBIT;
        *maxbit = DB_PTHRU_FIELD_KEY_PORT_MAXBIT;
    } else if (fid == BCMLTM_PT_FIELD_KEY_INSTANCE) {
        *minbit = DB_PTHRU_FIELD_KEY_INSTANCE_MINBIT;
        *maxbit = DB_PTHRU_FIELD_KEY_INSTANCE_MAXBIT;
    } else {
        int min, max;

        min = bcmdrd_pt_field_minbit(unit, sid, fid);
        max = bcmdrd_pt_field_maxbit(unit, sid, fid);

        if ((min < 0) || (max < 0)) {
            const char *table_name;
            const char *field_name;

            table_name = bcmltm_pt_table_sid_to_name(unit, sid);
            field_name = bcmltm_pt_field_fid_to_name(unit, sid, fid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Invalid field bit positions: "
                                  "%s(ptid=%d) %s(fid=%d) "
                                  "minbit=%d maxbit=%d\n"),
                       table_name, sid, field_name, fid,
                       *minbit, *maxbit));
            return SHR_E_INTERNAL;
        }

        *minbit = min;
        *maxbit = max;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Get the total number of field elements for given field.
 *
 * This routine returns the number of field index elements required
 * to fit the width of the given field.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] fid Field ID.
 *
 * \retval Number of field indexes (> 0), if successful.
 *         Otherwise, 0 on failure.
 */
static uint32_t
db_pthru_field_idx_count_get(int unit,
                             bcmdrd_sid_t sid,
                             bcmdrd_fid_t fid)
{
    int rv;
    uint32_t width;
    uint16_t minbit;
    uint16_t maxbit;
    uint32_t idx_count;

    rv = db_pthru_field_min_max_bit_get(unit, sid, fid, &minbit, &maxbit);
    if (SHR_FAILURE(rv)) {
        return 0;
    }

    width = maxbit - minbit + 1;
    idx_count = bcmltd_field_idx_count_get(width);

    return idx_count;
}

/*!
 * \brief Get the list of key fields IDs for given table ID.
 *
 * This routine gets the list of key fields IDs for given table ID.
 *
 * The function will always return the total number of field IDs
 * in num_fid, irrespective of the value of list_max, i.e. if
 * num_sid is greater than list_max, then the returned fid_list was
 * truncated.
 *
 * If list_max is zero, then the number of field IDs is returned
 * in num_fid, but the fid_list is not updated (can be
 * specified as NULL). The returned num_fid can then be used to
 * allocate a sufficiently large sid_list array.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] list_max Size of allocated entries in list.
 * \param [out] fid_list Returning list of field IDs.
 * \param [out] num_fid Total number of field IDs returned.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
db_pthru_pt_fid_list_key_get(int unit,
                             bcmdrd_sid_t sid,
                             size_t list_max,
                             bcmdrd_fid_t *fid_list,
                             size_t *num_fid)
{
    size_t idx = 0;
    uint32_t flags;

    flags = bcmltm_wb_ptm_op_flags_pthru_get(unit, sid);

    /* Check for index parameter */
    if (flags & BCMLTM_PT_OP_FLAGS_INDEX) {
        if (idx < list_max) {
            fid_list[idx] = BCMLTM_PT_FIELD_KEY_INDEX;
        }
        idx++;
    }

    /* Check for port parameter */
    if (flags & BCMLTM_PT_OP_FLAGS_PORT) {
        if (idx < list_max) {
            fid_list[idx] = BCMLTM_PT_FIELD_KEY_PORT;
        }
        idx++;
    }

    /* Check table instance parameter */
    if (flags & BCMLTM_PT_OP_FLAGS_TABLE_INST) {
        if (idx < list_max) {
            fid_list[idx] = BCMLTM_PT_FIELD_KEY_INSTANCE;
        }
        idx++;
    }

    *num_fid = idx;

    return SHR_E_NONE;
}

/*!
 * \brief Get the field map metadata for given field ID.
 *
 * This routine gets the field map(s) information for the given field ID.
 * If the field is wide, i.e. width is larger than what a field
 * element can fit (64 bits), then the necessary number of field
 * maps will be filled out.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] wb_block Working buffer block.
 * \param [in] fid Field ID.
 * \param [in] field_type Indicates key or value field.
 * \param [in] field_map_size Array count of field map.
 * \param [out] field_map Start of field map array to fill.
 * \param [out] num_field_maps Number of field maps returned.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
db_pthru_field_map_get(int unit,
                       bcmdrd_sid_t sid,
                       const bcmltm_wb_block_t *wb_block,
                       bcmdrd_fid_t fid,
                       bcmltm_field_type_t field_type,
                       uint32_t field_map_size,
                       bcmltm_field_map_t *field_map,
                       uint32_t *num_field_maps)
{
    uint32_t wbc_offset;
    uint16_t minbit;
    uint16_t maxbit;
    uint32_t idx;
    uint32_t idx_count;
    uint16_t idx_minbit[DB_PTHRU_MAX_FIELD_IDXS];
    uint16_t idx_maxbit[DB_PTHRU_MAX_FIELD_IDXS];
    uint32_t field_flags = 0x0;

    SHR_FUNC_ENTER(unit);

    *num_field_maps = 0;

    /* Get working buffer offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_field_pthru_offset_get(wb_block,
                                              fid,
                                              &wbc_offset));

    /*
     * Get corresponding minimum and maximum bit positions for
     * each field element.
     */
    SHR_IF_ERR_EXIT
        (db_pthru_field_min_max_bit_get(unit, sid, fid, &minbit, &maxbit));

    SHR_IF_ERR_EXIT
        (bcmltm_db_field_to_fidx_min_max(unit, fid,
                                         minbit, maxbit,
                                         DB_PTHRU_MAX_FIELD_IDXS,
                                         idx_minbit, idx_maxbit,
                                         &idx_count));
    /* Sanity check */
    if (idx_count > field_map_size) {
        const char *table_name;
        const char *field_name;

        table_name = bcmltm_pt_table_sid_to_name(unit, sid);
        field_name = bcmltm_pt_field_fid_to_name(unit, sid, fid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM PT Pass Thru field map array size "
                              "not large enough: "
                              "%s(ptid=%d) %s(fid=%d) size=%d need=%d\n"),
                   table_name, sid, field_name, fid,
                   field_map_size, idx_count));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        field_flags |= BCMLTM_FIELD_KEY;
    }

    for (idx = 0; idx < idx_count; idx++) {
        field_map[idx].field_id = fid;
        field_map[idx].field_idx = idx;
        field_map[idx].apic_offset = BCMLTM_WB_OFFSET_INVALID;
        field_map[idx].wbc.buffer_offset = wbc_offset;
        field_map[idx].wbc.minbit = idx_minbit[idx];
        field_map[idx].wbc.maxbit = idx_maxbit[idx];
        field_map[idx].pt_changed_offset = BCMLTM_WB_OFFSET_INVALID;
        field_map[idx].flags = field_flags;
    }

    *num_field_maps = idx_count;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the list of field maps metadata.
 *
 * This routine destroys the list of field maps metadata.
 *
 * \param [in] field_maps Field maps metadata to destroy.
 */
static inline void
db_pthru_field_maps_destroy(bcmltm_field_map_t *field_maps)
{
    SHR_FREE(field_maps);
    return;
}

/*!
 * \brief Create the field maps list metadata.
 *
 * This routine creates the field maps list metadata.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] pt_map_info PT mapping information.
 * \param [in] wb_handle Working buffer handle.
 * \param [in] field_type Indicates key or value field.
 * \param [out] field_maps_ptr Returning pointer to list of field maps.
 * \param [out] num_fields Number of field maps returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_field_maps_create(int unit,
                           bcmdrd_sid_t sid,
                           const db_pthru_pt_map_info_t *pt_map_info,
                           bcmltm_field_type_t field_type,
                           bcmltm_field_map_t **field_maps_ptr,
                           size_t *num_fields)
{
    bcmltm_field_map_t *field_maps = NULL;
    bcmdrd_fid_t *fid_list = NULL;
    unsigned int alloc_size;
    size_t fid_max_count;
    size_t num_fid;
    size_t total_field_maps = 0;
    size_t i;
    uint32_t pt_view = 0; /* PT Pass Thru only has 1 view */
    const bcmltm_wb_block_t *wb_block = NULL;
    uint32_t num_field_maps = 0;
    uint32_t field_maps_count = 0;

    SHR_FUNC_ENTER(unit);

    *num_fields = 0;
    *field_maps_ptr = NULL;

    /* Get total number of fields in symbol */
    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        /* Keys */
        SHR_IF_ERR_EXIT
            (db_pthru_pt_fid_list_key_get(unit, sid, 0, NULL, &fid_max_count));
    } else {
        /* Values */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_fid_list_get(unit, sid, 0, NULL, &fid_max_count));
    }

    if (fid_max_count == 0) {
        SHR_EXIT();
    }

    /* Allocate fid list array */
    alloc_size = sizeof(*fid_list) * fid_max_count;
    if (alloc_size == 0) {
        /* No FIDs */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fid_list, alloc_size, "bcmltmPthruFidArr");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, alloc_size);

    /* Get list of Field IDs */
    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        /* Keys */
        SHR_IF_ERR_EXIT
            (db_pthru_pt_fid_list_key_get(unit, sid, fid_max_count,
                                          fid_list, &num_fid));
    } else {
        /* Values */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_fid_list_get(unit, sid, fid_max_count,
                                    fid_list, &num_fid));
    }

    if (fid_max_count != num_fid) {
        const char *table_name;

        table_name = bcmltm_pt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM Number of fields mismatch: "
                              "%s(ptid=%d) expected=%d actual=%d\n"),
                   table_name, sid, (int) fid_max_count, (int) num_fid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get total field maps required (unique fid,idx) */
    for (i = 0; i < fid_max_count; i++) {
        total_field_maps += db_pthru_field_idx_count_get(unit, sid,
                                                         fid_list[i]);
    }

    /* Allocate field_map */
    alloc_size = sizeof(*field_maps) * total_field_maps;
    if (alloc_size == 0) {
        /* No field maps */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(field_maps, alloc_size, "bcmltmPthruFieldMaps");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, alloc_size);

    /* PT Pass Thru only has 1 pt view */
    wb_block = pt_map_info->pt_maps[pt_view].wb_block;

    /* Populate field map list metadata */
    for (i = 0; i < fid_max_count; i++) {
        SHR_IF_ERR_EXIT
            (db_pthru_field_map_get(unit, sid,
                                    wb_block, fid_list[i], field_type,
                                    total_field_maps - num_field_maps,
                                    &field_maps[num_field_maps],
                                    &field_maps_count));
        num_field_maps += field_maps_count;
    }

    *num_fields = num_field_maps;
    *field_maps_ptr = field_maps;

 exit:
    if (SHR_FUNC_ERR()) {
        db_pthru_field_maps_destroy(field_maps);
        *num_fields = 0;
        *field_maps_ptr = NULL;
    }

    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the field map list metadata.
 *
 * This routine destroys the field map list metadata.
 *
 * \param [in] field_list Field list metadata to destroy.
 */
static inline void
db_pthru_field_map_list_destroy(bcmltm_field_map_list_t *field_map_list)
{
    if (field_map_list == NULL) {
        return;
    }

    db_pthru_field_maps_destroy(field_map_list->field_maps);

    db_pthru_field_map_list_free(field_map_list);

    return;
}

/*!
 * \brief Create the field map list metadata.
 *
 * This routine creates the field map list metadata for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] pt_map_info PT mapping information.
 * \param [in] field_type Indicates key or value field.
 * \param [in] field_map_idx Field mapping index.
 * \param [out] field_list_ptr Returning pointer to field list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_field_map_list_create(int unit,
                               bcmdrd_sid_t sid,
                               const db_pthru_pt_map_info_t *pt_map_info,
                               bcmltm_field_type_t field_type,
                               uint32_t field_map_idx,
                               bcmltm_field_map_list_t **field_list_ptr)
{
    bcmltm_field_map_list_t *field_list = NULL;
    size_t num_fields;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    field_list = db_pthru_field_map_list_alloc(unit, field_type, field_map_idx);
    SHR_NULL_CHECK(field_list, SHR_E_MEMORY);
    sal_memset(field_list, 0, sizeof(*field_list));

    /* Get field map */
    SHR_IF_ERR_EXIT
        (db_pthru_field_maps_create(unit, sid,
                                    pt_map_info, field_type,
                                    &field_list->field_maps,
                                    &num_fields));
    field_list->num_fields = num_fields;

    /* Not applicable */
    field_list->index_absent_offset = BCMLTM_WB_OFFSET_INVALID;

    *field_list_ptr = field_list;

 exit:
    if (SHR_FUNC_ERR()) {
        db_pthru_field_map_list_destroy(field_list);
        *field_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the field mapping metadata.
 *
 * This routine destroys the field mapping metadata.
 *
 * \param [in] field_mapping Field mapping metadata to destroy.
 */
static inline void
db_pthru_field_mapping_destroy(bcmltm_field_select_mapping_t *field_mapping)
{
    if (field_mapping == NULL) {
        return;
    }

    db_pthru_field_map_list_destroy(field_mapping->field_map_list[0]);

    SHR_FREE(field_mapping);

    return;
}

/*!
 * \brief Create the field mapping metadata.
 *
 * This routine creates the field mapping metadata for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] pt_map_info PT mapping information.
 * \param [in] field_type Indicates key or value field.
 * \param [out] field_mapping_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_field_mapping_create(int unit,
                              bcmdrd_sid_t sid,
                              const db_pthru_pt_map_info_t *pt_map_info,
                              bcmltm_field_type_t field_type,
                              bcmltm_field_select_mapping_t **field_mapping_ptr)
{
    bcmltm_field_select_mapping_t *field_mapping = NULL;
    uint32_t alloc_size;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    alloc_size = sizeof(*field_mapping) +
        (sizeof(bcmltm_field_map_list_t *) * DB_PTHRU_NUM_FIELD_MAPS);
    SHR_ALLOC(field_mapping, alloc_size,
              "bcmltmPthruFieldSelectMapping");
    SHR_NULL_CHECK(field_mapping, SHR_E_MEMORY);
    sal_memset(field_mapping, 0, alloc_size);
    field_mapping->map_index_offset = BCMLTM_WB_OFFSET_INVALID;
    field_mapping->num_maps = DB_PTHRU_NUM_FIELD_MAPS;

    /* Fill data */
    SHR_IF_ERR_EXIT
        (db_pthru_field_map_list_create(unit, sid,
                                        pt_map_info, field_type, 0,
                                        &(field_mapping->field_map_list[0])));

    *field_mapping_ptr = field_mapping;

 exit:
    if (SHR_FUNC_ERR()) {
        db_pthru_field_mapping_destroy(field_mapping);
        *field_mapping_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given PT mapping information.
 *
 * This routine destroys the given PT mapping information.
 *
 * \param [in] info Pointer to PT mapping information to destroy.
 */
static inline void
db_pthru_pt_map_info_destroy(db_pthru_pt_map_info_t *info)
{
    if (info == NULL) {
        return;
    }

    db_pthru_pt_map_info_free(info);

    return;
}

/*!
 * \brief Create PT mapping information for given table.
 *
 * This routine creates the information for all mapped PTs for
 * the given table.  For PT Pass Thru, the LT to PT mapping is 1-1.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] wb_handle Working buffer handle.
 * \param [out] info_ptr Returning pointer to PT mapping information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_pt_map_info_create(int unit,
                            bcmdrd_sid_t sid,
                            bcmltm_wb_handle_t *wb_handle,
                            db_pthru_pt_map_info_t **info_ptr)
{
    db_pthru_pt_map_info_t *info = NULL;
    const uint32_t num_pts = BCMLTM_PTHRU_NUM_PT_VIEWS;
    const uint32_t num_ops = BCMLTM_PTHRU_NUM_PT_OPS;
    uint32_t flags;
    bcmltm_wb_block_id_t wb_block_id;
    bcmltm_wb_block_t *wb_block = NULL;

    SHR_FUNC_ENTER(unit);

    *info_ptr = NULL;

    flags = bcmltm_wb_ptm_op_flags_pthru_get(unit, sid);
    SHR_IF_ERR_EXIT
        (bcmltm_wb_block_ptm_pthru_add(unit, sid, flags, num_ops,
                                       wb_handle, &wb_block_id, &wb_block));

    info = db_pthru_pt_map_info_alloc(unit, num_pts);
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    info->num_pts = num_pts;
    info->pt_maps[0].sid = sid;  /* PT ID is same as LT ID */
    info->pt_maps[0].flags = flags;
    info->pt_maps[0].num_ops = num_ops;
    info->pt_maps[0].wb_block_id = wb_block_id;
    info->pt_maps[0].wb_block = wb_block;

    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        db_pthru_pt_map_info_destroy(info);
        *info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create table database information.
 *
 * This routine creates the database information for the
 * given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] info Table database information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_table_info_create(int unit,
                           bcmdrd_sid_t sid,
                           db_pthru_table_info_t *info)
{
   SHR_FUNC_ENTER(unit);

    info->sid = sid;

    /* Initialize Working Buffer handle */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_handle_init(sid, FALSE, &info->wb_handle));

    /* Create PT mapping information */
    SHR_IF_ERR_EXIT
        (db_pthru_pt_map_info_create(unit, sid, &info->wb_handle,
                                     &info->pt_map_info));

    /* Create PT list (EE node cookie) */
    SHR_IF_ERR_EXIT
        (db_pthru_pt_list_create(unit, info->pt_map_info, &info->wb_handle,
                                 &info->pt_list));

    /* Create field mapping for keys and values (FA node cookies) */
    SHR_IF_ERR_EXIT
        (db_pthru_field_mapping_create(unit, sid,
                                       info->pt_map_info,
                                       BCMLTM_FIELD_TYPE_KEY,
                                       &info->keys));
    SHR_IF_ERR_EXIT
        (db_pthru_field_mapping_create(unit, sid,
                                       info->pt_map_info,
                                       BCMLTM_FIELD_TYPE_VALUE,
                                       &info->values));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy table database information.
 *
 * This routine destroys the given table database information.
 *
 * \param [in] info Table database information to destroy.
 */
static void
db_pthru_table_info_destroy(db_pthru_table_info_t *info)
{
    if (info == NULL) {
        return;
    }

    db_pthru_field_mapping_destroy(info->values);
    info->values = NULL;

    db_pthru_field_mapping_destroy(info->keys);
    info->keys = NULL;

    db_pthru_pt_list_destroy(info->pt_list);
    info->pt_list= NULL;

    db_pthru_pt_map_info_destroy(info->pt_map_info);
    info->pt_map_info = NULL;

    bcmltm_wb_handle_cleanup(&info->wb_handle);

    info->sid = BCMDRD_INVALID_ID;

    return;
}

/*!
 * \brief Initialize table database information.
 *
 * This routine initializes the given table database information.
 *
 * \param [in] unit Unit number.
 * \param [out] info Table database information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_table_info_init(int unit,
                         db_pthru_table_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    sal_memset(info, 0, sizeof(*info));

    info->sid = BCMDRD_INVALID_ID;

    SHR_IF_ERR_EXIT
        (bcmltm_wb_handle_init(BCMDRD_INVALID_ID, FALSE, &info->wb_handle));

    info->pt_list = NULL;
    info->keys = NULL;
    info->values = NULL;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get table database information.
 *
 * This routine gets the table database information for the given
 * table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] info_ptr Returning pointer to table database information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_table_info_get(int unit,
                        bcmdrd_sid_t sid,
                        const db_pthru_table_info_t **info_ptr)
{
    const db_pthru_table_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    *info_ptr = NULL;

    if (!db_pthru_is_init(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    info = &db_pthru[unit]->info.table;

    /* Check if ID matches current database */
    if (info->sid != sid) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *info_ptr = info;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup database information.
 *
 * This routine destroys the given database information.
 *
 * \param [in] info Database information to destroy.
 */
static inline void
db_pthru_info_cleanup(db_pthru_info_t *info)
{
    db_pthru_table_info_destroy(&info->table);
}

/*!
 * \brief Initialize database information.
 *
 * This routine initializes the given database information.
 *
 * \param [in] unit Unit number.
 * \param [out] info Database information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static inline int
db_pthru_info_init(int unit,
                   db_pthru_info_t *info)
{
    return db_pthru_table_info_init(unit, &info->table);
}

/*!
 * \brief Get field information for register/memory key field.
 *
 * This routine gets the field information for the given table ID
 * and key field ID.
 *
 * This function gets the information on the fly and does not use
 * the cached table data.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] fid Key field ID.
 * \param [out] finfo Field information to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_pthru_key_field_info_get(int unit,
                            bcmdrd_sid_t sid,
                            bcmdrd_fid_t fid,
                            bcmdrd_sym_field_info_t *finfo)
{
    uint16_t minbit;
    uint16_t maxbit;

    SHR_FUNC_ENTER(unit);

    if (!db_pthru_key_field_is_valid(unit, sid, fid)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (db_pthru_field_min_max_bit_get(unit, sid, fid, &minbit, &maxbit));

    finfo->fid = fid;
    finfo->view = 0;
    finfo->minbit = minbit;
    finfo->maxbit = maxbit;
    finfo->flags |= BCMDRD_SYMBOL_FIELD_FLAG_KEY;

    if (fid == BCMLTM_PT_FIELD_KEY_INDEX) {
        finfo->name = BCMLTM_PT_FIELD_KEY_INDEX_NAME;
    } else if (fid == BCMLTM_PT_FIELD_KEY_PORT) {
        finfo->name = BCMLTM_PT_FIELD_KEY_PORT_NAME;
    } else if (fid == BCMLTM_PT_FIELD_KEY_INSTANCE) {
        finfo->name = BCMLTM_PT_FIELD_KEY_INSTANCE_NAME;
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

 exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_pthru_init(int unit,
                     uint32_t sid_max_count)
{
    db_pthru_t *db = NULL;

    SHR_FUNC_ENTER(unit);

    /* Check if database has been initialized */
    if (db_pthru_is_init(unit)) {
        SHR_EXIT();
    }

    /* Allocate */
    SHR_ALLOC(db, sizeof(*db), "bcmltmDbPthru");
    SHR_NULL_CHECK(db, SHR_E_MEMORY);
    sal_memset(db, 0, sizeof(*db));
    db_pthru[unit] = db;

    /* Initialize information */
    db->sid_max_count = sid_max_count;

    SHR_IF_ERR_EXIT
        (db_pthru_mp_init(unit, &db->mp));

    SHR_IF_ERR_EXIT
        (db_pthru_info_init(unit, &db->info));

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_pthru_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_db_pthru_cleanup(int unit)
{
    db_pthru_t *db = NULL;

    /* Check if database has been initialized */
    if (!db_pthru_is_init(unit)) {
        return;
    }

    db = db_pthru[unit];

    /* Cleanup internal information */
    db_pthru_info_cleanup(&db->info);
    db_pthru_mp_cleanup(&db->mp);

    SHR_FREE(db);
    db_pthru[unit] = NULL;

    return;
}

int
bcmltm_db_pthru_table_create(int unit,
                             bcmdrd_sid_t sid)
{
    db_pthru_table_mp_t *mp = NULL;
    db_pthru_table_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    if (!db_pthru_is_init(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!db_pthru_sid_is_valid(unit, sid)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    mp = &db_pthru[unit]->mp.table;
    info = &db_pthru[unit]->info.table;

    /* Check if existing cached table matches new ID */
    if (info->sid == sid) {
        SHR_EXIT();
    }

    /* Check if there is an existing cached table database */
    if (info->sid != BCMDRD_INVALID_ID) {
        /* No more internal space for another table */
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    /* Initialize memory pool */
    SHR_IF_ERR_EXIT
        (db_pthru_table_mp_init(unit, mp));

    /* Create database information */
    SHR_IF_ERR_EXIT
        (db_pthru_table_info_create(unit, sid, info));

 exit:
    if (SHR_FUNC_ERR()) {
        db_pthru_table_info_destroy(info);
    }

    SHR_FUNC_EXIT();
}

int
bcmltm_db_pthru_table_destroy(int unit,
                              bcmdrd_sid_t sid)
{
    db_pthru_table_mp_t *mp = NULL;
    db_pthru_table_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    if (!db_pthru_is_init(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    mp = &db_pthru[unit]->mp.table;
    info = &db_pthru[unit]->info.table;

    /*
     * Check if ID matches current database.
     * BCMDRD_INVALID_ID indicates to destroy current cached data (any ID).
     */
    if ((sid != BCMDRD_INVALID_ID) && (info->sid != sid)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    db_pthru_table_info_destroy(info);
    db_pthru_table_mp_cleanup(mp);

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_pthru_wb_handle_get(int unit,
                              uint32_t sid,
                              const bcmltm_wb_handle_t **wb_handle_ptr)
{
    const db_pthru_table_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    *wb_handle_ptr = NULL;

    SHR_IF_ERR_EXIT(db_pthru_table_info_get(unit, sid, &info));

    if (info != NULL) {
        *wb_handle_ptr = &info->wb_handle;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_pthru_pt_list_get(int unit,
                            bcmdrd_sid_t sid,
                            const bcmltm_pt_list_t **pt_list_ptr)
{
    const db_pthru_table_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    *pt_list_ptr = NULL;

    SHR_IF_ERR_EXIT(db_pthru_table_info_get(unit, sid, &info));

    if (info != NULL) {
        *pt_list_ptr = info->pt_list;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_pthru_key_fields_get(int unit,
                               bcmdrd_sid_t sid,
                         const bcmltm_field_select_mapping_t **fields_ptr)
{
    const db_pthru_table_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    SHR_IF_ERR_EXIT(db_pthru_table_info_get(unit, sid, &info));

    if (info != NULL) {
        *fields_ptr = info->keys;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_pthru_value_fields_get(int unit,
                                 bcmdrd_sid_t sid,
                           const bcmltm_field_select_mapping_t **fields_ptr)
{
    const db_pthru_table_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    SHR_IF_ERR_EXIT(db_pthru_table_info_get(unit, sid, &info));

    if (info != NULL) {
        *fields_ptr = info->values;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_pthru_field_info_get(int unit,
                               bcmdrd_sid_t sid,
                               bcmdrd_fid_t fid,
                               bcmdrd_sym_field_info_t *finfo)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Check if database has been initialized */
    if (!db_pthru_is_init(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!db_pthru_sid_is_valid(unit, sid)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(finfo, 0, sizeof(*finfo));

    /* Check if field is a key */
    if ((fid == BCMLTM_PT_FIELD_KEY_INDEX) ||
        (fid == BCMLTM_PT_FIELD_KEY_PORT) ||
        (fid == BCMLTM_PT_FIELD_KEY_INSTANCE)) {
        SHR_IF_ERR_EXIT
            (db_pthru_key_field_info_get(unit, sid, fid, finfo));
    } else {
        /* Value field, get info from DRD */
        rv = bcmdrd_pt_field_info_get(unit, sid, fid, finfo);
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}
