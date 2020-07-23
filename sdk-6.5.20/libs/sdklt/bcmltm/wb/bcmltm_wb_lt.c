/*! \file bcmltm_wb_lt.c
 *
 * Logical Table Manager - Logical Table Working Buffer.
 *
 * This file contains the working buffer information
 * for Logical Tables, such as the working buffer handles
 * for the logical tables.
 *
 * This file also offers higher level interfaces to get working
 * buffer information by logical table ID (when possible),
 * rather than by block handle (lower level).
 *
 * The working buffer handle is needed through the metadata construction
 * of a given table in order to properly assign the offsets into
 * the working buffer space used during LTM operation, FA and EE stages.
 *
 * The assumption (throughout the LTM component) is that the
 * logical table IDs (LRD symbol ID) are defined in a compact numbering
 * space [0.. n-1], where 'n' is the total number of LRD symbols.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>

#include <sal/sal_libc.h>

#include <bsl/bsl.h>

#include <bcmlrd/bcmlrd_table.h>

#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>
#include <bcmltm/bcmltm_wb_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_wb_lt_pvt_internal.h>
#include <bcmltm/bcmltm_wb_apic_internal.h>
#include <bcmltm/bcmltm_wb_fs_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*
 * Helper macros
 */
/* Check that info list has been initialized */
#define WB_INFO_INIT(_u)                        \
    (WB_INFO(_u) != NULL)

/* Check that symbol id is valid */
#define WB_INFO_SID_VALID(_u, _sid)             \
    ((_sid) < WB_INFO_SID_MAX_COUNT(_u))

/* Check that unit and symbol id are valid */
#define WB_INFO_VALID(_u, _sid)                         \
    (WB_INFO_INIT(_u) && WB_INFO_SID_VALID(_u, _sid))


/*
 * Accessor macros for Logical Tables info
 */
#define WB_INFO(_u)                      wb_info[_u]
#define WB_INFO_SID_MAX_COUNT(_u)        WB_INFO(_u)->sid_max_count
#define WB_INFO_HANDLE(_u, _sid)         WB_INFO(_u)->handle[_sid]


/*!
 * \brief Working Buffer Information.
 */
typedef struct wb_info_list_s {
    /*! Total number of symbols. */
    uint32_t sid_max_count;

    /*!
     * Array of Working Buffer Handler Pointers.
     *
     * This implementation is based upon the symbols IDs
     * being in a compact numbering space from [0 .. (sid_max_count-1)]
     */
    bcmltm_wb_handle_t *handle[];
} wb_info_list_t;


/* Logical Table Working Buffer Information */
static wb_info_list_t *wb_info[BCMLTM_MAX_UNITS];


/*******************************************************************************
 * Private functions
 */
static int
lt_info_valid(int unit, uint32_t sid)
{
    SHR_FUNC_ENTER(unit);

    if (!WB_INFO_INIT(unit)) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM working buffer list has been "
                              "not initialized: %s(ltid=%d)\n"),
                   table_name, sid));
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!WB_INFO_SID_VALID(unit, sid)) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM working buffer: Invalid sid %s(ltid=%d)"
                              "(max_sid=%d)\n"),
                   table_name, sid, WB_INFO_SID_MAX_COUNT(unit) - 1));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create a working buffer handle for given table ID.
 *
 * This routine allocates and initializes a working buffer handle
 * for the specified table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_wb_lt_handle_create(int unit, uint32_t sid)
{
    bcmltm_wb_handle_t *handle = NULL;

    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (WB_INFO_HANDLE(unit, sid) != NULL) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM working buffer handle is already "
                              "created: %s(ltid=%d)\n"),
                   table_name, sid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate */
    SHR_ALLOC(handle, sizeof(*handle), "bcmltmWbHandle");
    SHR_NULL_CHECK(handle, SHR_E_MEMORY);
    sal_memset(handle, 0, sizeof(*handle));

    /* Initialize handle */
    SHR_IF_ERR_EXIT(bcmltm_wb_handle_init(sid, TRUE, handle));

    WB_INFO_HANDLE(unit, sid) = handle;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_wb_handle_cleanup(handle);
        SHR_FREE(handle);
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy  working buffer handle for given table ID.
 *
 * This routine frees any memory allocated during the
 * construction of the working buffer handle for the specified table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
bcmltm_wb_lt_handle_destroy(int unit, uint32_t sid)
{
    if (!WB_INFO_VALID(unit, sid)) {
        return;
    }

    if (WB_INFO_HANDLE(unit, sid) == NULL) {
        return;
    }

    /* Cleanup handle */
    bcmltm_wb_handle_cleanup(WB_INFO_HANDLE(unit, sid));

    /* Free handle */
    SHR_FREE(WB_INFO_HANDLE(unit, sid));
    WB_INFO_HANDLE(unit, sid) = NULL;

    return;
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_wb_lt_info_init(int unit, uint32_t sid_max_count)
{
    wb_info_list_t *info = NULL;
    unsigned int size;
    bcmlrd_sid_t *sid_list = NULL;
    size_t num_sid;
    size_t idx;
    size_t sid;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Check if it has been initialized */
    if (WB_INFO_INIT(unit)) {
        SHR_EXIT();
    }

    /* Allocate */
    size = sizeof(*info) + (sizeof(bcmltm_wb_handle_t*) * sid_max_count);
    SHR_ALLOC(info, size, "bcmltmWbInfoList");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, size);
    WB_INFO(unit) = info;

    /* Initialize information */
    info->sid_max_count = sid_max_count;

     /* Allocate sid list array */
    size = sizeof(*sid_list) * sid_max_count;
    if (size == 0) {
        /* No SIDs */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
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

    /* Create WB handle for each table */
    for (idx = 0; idx < num_sid; idx++) {
        sid = sid_list[idx];

        /* Create WB handle */
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmltm_wb_lt_handle_create(unit, sid), SHR_E_UNAVAIL);
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_wb_lt_info_cleanup(unit);
    }

    SHR_FREE(sid_list);
    SHR_FUNC_EXIT();
}


void
bcmltm_wb_lt_info_cleanup(int unit)
{
    uint32_t sid;
    uint32_t sid_max;

    if (!WB_INFO_INIT(unit)) {
        return;
    }

    /*
     * This implementation is based upon the symbols IDs
     * being in a compact numbering space from [0 .. (sid_max_count-1)]
     */
    sid_max = WB_INFO_SID_MAX_COUNT(unit) - 1;

    /* Destroy internal information */
    for (sid = 0; sid <= sid_max; sid++) {
        /* LRD symbol ID is the same */
        bcmltm_wb_lt_handle_destroy(unit, sid);
    }

    SHR_FREE(WB_INFO(unit));
    WB_INFO(unit) = NULL;

    return;
}


bcmltm_wb_handle_t *
bcmltm_wb_lt_handle_get(int unit, uint32_t sid)
{
    /* Sanity check */
    if (!WB_INFO_VALID(unit, sid)) {
        return NULL;
    }

    return WB_INFO_HANDLE(unit, sid);
}


const bcmltm_wb_block_t *
bcmltm_wb_lt_block_get(int unit, uint32_t sid,
                       bcmltm_wb_block_id_t block_id)
{
    /* Sanity check */
    if (!WB_INFO_VALID(unit, sid)) {
        return NULL;
    }

    return bcmltm_wb_block_find(WB_INFO_HANDLE(unit, sid), block_id);
}


const bcmltm_wb_block_t *
bcmltm_wb_lt_pvt_block_get(int unit, uint32_t sid)
{
    int rv = SHR_E_NONE;
    const bcmltm_wb_handle_t *handle = NULL;
    const bcmltm_wb_block_t *block = NULL;
    uint32_t num_blocks = 0;

    /* Sanity check */
    if (!WB_INFO_VALID(unit, sid)) {
        return NULL;
    }

    handle = WB_INFO_HANDLE(unit, sid);
    if  (handle == NULL) {
        return NULL;
    }

    /* Number of LT Private block should be no more than one */
    rv = bcmltm_wb_block_list_get(WB_INFO_HANDLE(unit, sid),
                                  BCMLTM_WB_BLOCK_TYPE_LT_PVT,
                                  1, &block, &num_blocks);
    if (SHR_FAILURE(rv)) {
        return NULL;
    }

    return block;
}

int
bcmltm_wb_lt_pvt_offsets_get_by_sid(int unit, uint32_t sid,
                                    bcmltm_wb_lt_pvt_offsets_t *offsets)
{
    const bcmltm_wb_block_t *lt_pvt_wb_block;

    SHR_FUNC_ENTER(unit);

    lt_pvt_wb_block = bcmltm_wb_lt_pvt_block_get(unit, sid);
    if (lt_pvt_wb_block == NULL) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "WB block for LT Private not found: "
                              "%s(ltid=%d)\n"),
                   table_name, sid));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_pvt_offsets_get(lt_pvt_wb_block, offsets));

 exit:
    SHR_FUNC_EXIT();

}

int
bcmltm_wb_lt_track_index_offset_get_by_sid(int unit, uint32_t sid,
                                           uint32_t *offset)
{
    bcmltm_wb_lt_pvt_offsets_t lt_pvt_wb_offsets;

    SHR_FUNC_ENTER(unit);

    *offset = BCMLTM_WB_OFFSET_INVALID;

    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_pvt_offsets_get_by_sid(unit, sid,
                                             &lt_pvt_wb_offsets));
    *offset = lt_pvt_wb_offsets.track_index;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_wb_lt_index_absent_offset_get_by_sid(int unit, uint32_t sid,
                                            uint32_t *offset)
{
    bcmltm_wb_lt_pvt_offsets_t lt_pvt_wb_offsets;

    SHR_FUNC_ENTER(unit);

    *offset = BCMLTM_WB_OFFSET_INVALID;

    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_pvt_offsets_get_by_sid(unit, sid,
                                             &lt_pvt_wb_offsets));
    *offset = lt_pvt_wb_offsets.index_absent;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_wb_lt_pt_suppress_offset_get_by_sid(int unit, uint32_t sid,
                                           uint32_t *offset)
{
    bcmltm_wb_lt_pvt_offsets_t lt_pvt_wb_offsets;

    SHR_FUNC_ENTER(unit);

    *offset = BCMLTM_WB_OFFSET_INVALID;

    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_pvt_offsets_get_by_sid(unit, sid,
                                             &lt_pvt_wb_offsets));
    *offset = lt_pvt_wb_offsets.pt_suppress;

 exit:
    SHR_FUNC_EXIT();
}

const bcmltm_wb_block_t *
bcmltm_wb_lt_apic_block_get(int unit, uint32_t sid)
{
    int rv = SHR_E_NONE;
    const bcmltm_wb_handle_t *handle = NULL;
    const bcmltm_wb_block_t *block = NULL;
    uint32_t num_blocks = 0;

    /* Sanity check */
    if (!WB_INFO_VALID(unit, sid)) {
        return NULL;
    }

    handle = WB_INFO_HANDLE(unit, sid);
    if  (handle == NULL) {
        return NULL;
    }

    /* Number of API Cache blocks should be no more than one */
    rv = bcmltm_wb_block_list_get(WB_INFO_HANDLE(unit, sid),
                                  BCMLTM_WB_BLOCK_TYPE_APIC,
                                  1, &block, &num_blocks);
    if (SHR_FAILURE(rv)) {
        return NULL;
    }

    return block;
}

int
bcmltm_wb_lt_apic_field_offset_get_by_sid(int unit, uint32_t sid,
                                          uint32_t field_id,
                                          uint32_t field_idx,
                                          uint32_t *offset)
{
    const bcmltm_wb_block_t *apic_wb_block;

    SHR_FUNC_ENTER(unit);

    *offset = BCMLTM_WB_OFFSET_INVALID;

    apic_wb_block = bcmltm_wb_lt_apic_block_get(unit, sid);
    if (apic_wb_block == NULL) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "WB block for API Cache not found: "
                              "%s(ltid=%d)\n"),
                   table_name, sid));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmltm_wb_apic_field_offset_get(apic_wb_block,
                                         field_id,
                                         field_idx,
                                         offset));
 exit:
    SHR_FUNC_EXIT();
}

const bcmltm_wb_block_t *
bcmltm_wb_lt_fs_block_get(int unit, uint32_t sid)
{
    int rv = SHR_E_NONE;
    const bcmltm_wb_handle_t *handle = NULL;
    const bcmltm_wb_block_t *block = NULL;
    uint32_t num_blocks = 0;

    /* Sanity check */
    if (!WB_INFO_VALID(unit, sid)) {
        return NULL;
    }

    handle = WB_INFO_HANDLE(unit, sid);
    if  (handle == NULL) {
        return NULL;
    }

    /* Number of Field Selection blocks should be no more than one */
    rv = bcmltm_wb_block_list_get(WB_INFO_HANDLE(unit, sid),
                                  BCMLTM_WB_BLOCK_TYPE_FS,
                                  1, &block, &num_blocks);
    if (SHR_FAILURE(rv)) {
        return NULL;
    }

    return block;
}

int
bcmltm_wb_lt_fs_map_index_offset_get_by_sid(int unit, uint32_t sid,
                                            bcmltm_field_selection_id_t sel_id,
                                            uint32_t *offset)
{
    const bcmltm_wb_block_t *fs_wb_block;

    SHR_FUNC_ENTER(unit);

    *offset = BCMLTM_WB_OFFSET_INVALID;

    fs_wb_block = bcmltm_wb_lt_fs_block_get(unit, sid);
    if (fs_wb_block == NULL) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "WB block for Field Selection not found: "
                              "%s(ltid=%d)\n"),
                   table_name, sid));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmltm_wb_fs_map_index_offset_get(fs_wb_block,
                                           sel_id,
                                           offset));
 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_wb_lt_wsize_get(int unit, uint32_t sid,
                       uint32_t *size)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    *size = bcmltm_wb_wsize_get(WB_INFO_HANDLE(unit, sid));

 exit:
    SHR_FUNC_EXIT();
}

