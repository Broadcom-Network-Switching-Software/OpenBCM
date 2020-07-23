/*! \file bcmltm_db.c
 *
 * Logical Table Manager - Logical Table Database.
 *
 * This file contains the top level routines to create the logical table
 * database and provide information for logical tables.
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

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_db_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>

#include "bcmltm_db_info.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*
 * Helper macros
 */

/*!
 * \brief Check for null pointer, and if so, exit.
 *
 * This macro is used to check for null pointer, and if so, the
 * calling function exits.
 * This macro is used on functions with SHR_FUNC_ENTER/_EXIT() constructions.
 *
 * Unlike SHR_NULL_CHECK(), this macro does not modify the return
 * error code.  It exits, on null pointer, using the current error
 * code in the function.
 *
 * \param [in] _ptr Pointer to check.
 */
#define BCMLTM_DB_IF_NULL_PTR_EXIT(_ptr)        \
    do {                                        \
        if ((_ptr) == NULL) {                   \
            SHR_EXIT();                         \
        }                                       \
    } while (0)

/*
 * Get logical table database information for given table ID.
 * This causes function to exit on invalid unit or table ID, or NULL info.
 */
#define DB_LT_INFO_GET_IF_NULL_EXIT(_u, _sid, _info)                \
    do {                                                            \
        SHR_IF_ERR_EXIT(bcmltm_db_lt_info_get(_u, _sid, &(_info))); \
        BCMLTM_DB_IF_NULL_PTR_EXIT(_info);                          \
    } while (0);

/*
 * Get Direct Map information for given table ID.
 * This causes function to exit on invalid unit or table ID, or NULL info.
 */
#define DB_DM_INFO_GET_IF_NULL_EXIT(_u, _sid, _info)        \
    do {                                                    \
        const bcmltm_db_lt_info_t *_lt;                     \
                                                            \
        DB_LT_INFO_GET_IF_NULL_EXIT(_u, _sid, _lt);         \
        (_info) = DB_DM_INFO(_lt);                          \
        BCMLTM_DB_IF_NULL_PTR_EXIT(_info);                  \
    } while (0);

/*
 * Get Field Transform information for given table ID.
 * This causes function to exit on invalid unit or table ID, or NULL info.
 */
#define DB_XFRM_INFO_GET_IF_NULL_EXIT(_u, _sid, _info)      \
    do {                                                    \
        const bcmltm_db_dm_info_t *_dm;                     \
                                                            \
        DB_DM_INFO_GET_IF_NULL_EXIT(_u, _sid, _dm);         \
        (_info) = _dm->xfrm;                                \
        BCMLTM_DB_IF_NULL_PTR_EXIT(_info);                  \
    } while (0);

/*
 * Get Field Transform information for given table ID.
 * This causes function to exit on invalid unit or table ID, or NULL info.
 */
#define DB_XFRM_INFO_GET_IF_NULL_EXIT(_u, _sid, _info)      \
    do {                                                    \
        const bcmltm_db_dm_info_t *_dm;                     \
                                                            \
        DB_DM_INFO_GET_IF_NULL_EXIT(_u, _sid, _dm);         \
        (_info) = _dm->xfrm;                                \
        BCMLTM_DB_IF_NULL_PTR_EXIT(_info);                  \
    } while (0);

/*
 * Get Field Validation information for given table ID.
 * This causes function to exit on invalid unit or table ID, or NULL info.
 */
#define DB_FV_INFO_GET_IF_NULL_EXIT(_u, _sid, _info)        \
    do {                                                    \
        const bcmltm_db_lt_info_t *_lt;                     \
                                                            \
        DB_LT_INFO_GET_IF_NULL_EXIT(_u, _sid, _lt);         \
        (_info) = DB_FV_INFO(_lt);                          \
        BCMLTM_DB_IF_NULL_PTR_EXIT(_info);                  \
    } while (0);

/*
 * Get Table Commit information for given table ID.
 * This causes function to exit on invalid unit or table ID, or NULL info.
 */
#define DB_TC_INFO_GET_IF_NULL_EXIT(_u, _sid, _info)        \
    do {                                                    \
        const bcmltm_db_lt_info_t *_lt;                     \
                                                            \
        DB_LT_INFO_GET_IF_NULL_EXIT(_u, _sid, _lt);         \
        (_info) = DB_TC_INFO(_lt);                          \
        BCMLTM_DB_IF_NULL_PTR_EXIT(_info);                  \
    } while (0);

/*
 * Get Custom Table Handler information for given table ID.
 * This causes function to exit on invalid unit or table ID, or NULL info.
 */
#define DB_CTH_INFO_GET_IF_NULL_EXIT(_u, _sid, _info)       \
    do {                                                    \
        const bcmltm_db_lt_info_t *_lt;                     \
                                                            \
        DB_LT_INFO_GET_IF_NULL_EXIT(_u, _sid, _lt);         \
        (_info) = DB_CTH_INFO(_lt);                         \
        BCMLTM_DB_IF_NULL_PTR_EXIT(_info);                  \
    } while (0);

/*
 * Get LTM Managed Table information for given table ID.
 * This causes function to exit on invalid unit or table ID, or NULL info.
 */
#define DB_LTM_INFO_GET_IF_NULL_EXIT(_u, _sid, _info)       \
    do {                                                    \
        const bcmltm_db_lt_info_t *_lt;                     \
                                                            \
        DB_LT_INFO_GET_IF_NULL_EXIT(_u, _sid, _lt);         \
        (_info) = DB_LTM_INFO(_lt);                         \
        BCMLTM_DB_IF_NULL_PTR_EXIT(_info);                  \
    } while (0);


/* Get Field Validation Copy list index */
#define FV_COPY_IDX(_from_idx, _from_num, _to_idx)  \
    (((_to_idx) * (_from_num)) + (_from_idx))

/* Get Field Transform Copy list index */
#define XFRM_COPY_IDX(_from_idx, _from_num, _to_idx)    \
    (((_to_idx) * (_from_num)) + (_from_idx))

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get Field Transform list for given transform type and table ID.
 *
 * This routine gets the field transform list for given transform type
 * in specified table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Field transform type to get information for.
 * \param [out] xfrm_list_ptr Returning pointer to Field Transform list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_list_get(int unit, bcmlrd_sid_t sid, int xfrm_type,
              const bcmltm_db_xfrm_list_t **xfrm_list_ptr)
{
    const bcmltm_db_xfrm_info_t *xfrm_info;
    const bcmltm_db_xfrm_list_t *xfrm_list = NULL;

    SHR_FUNC_ENTER(unit);

    *xfrm_list_ptr = NULL;

    DB_XFRM_INFO_GET_IF_NULL_EXIT(unit, sid, xfrm_info);

    if (xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM) {
        xfrm_list = xfrm_info->fwd_key_xfrm_list;
    } else if (xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_VALUE_TRANSFORM) {
        xfrm_list = xfrm_info->fwd_value_xfrm_list;
    } else if (xfrm_type == BCMLTM_DB_XFRM_TYPE_REV_KEY_TRANSFORM) {
        xfrm_list = xfrm_info->rev_key_xfrm_list;
    } else if (xfrm_type == BCMLTM_DB_XFRM_TYPE_REV_VALUE_TRANSFORM){
        xfrm_list = xfrm_info->rev_value_xfrm_list;
    }

    *xfrm_list_ptr = xfrm_list;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get field selection information for given selection ID.
 *
 * This routine gets the field selection information for the specified
 * selection ID for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Selection ID to get data for.
 * \param [out] fs_ptr Returning pointer to field selection information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dm_fs_get(int unit,
          bcmlrd_sid_t sid,
          bcmltm_field_selection_id_t selection_id,
          const bcmltm_db_dm_fs_t **fs_ptr)
{
    const bcmltm_db_dm_fs_info_t *fs_info = NULL;

    SHR_FUNC_ENTER(unit);

    *fs_ptr = NULL;

    SHR_IF_ERR_EXIT(bcmltm_db_dm_fs_info_get(unit, sid, &fs_info));
    if (fs_info == NULL) {
        SHR_EXIT();
    }

    *fs_ptr = bcmltm_db_dm_fs_find(fs_info, selection_id);

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get reverse track index transform list.
 *
 * This routine gets the reverse track index transform list
 * for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] list_ptr Returning pointer to field transform list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
track_rev_xfrm_list_get(int unit,
                        bcmlrd_sid_t sid,
                        const bcmltm_db_dm_xfrm_list_t **list_ptr)
{
    const bcmltm_db_dm_track_info_t *track_info = NULL;

    SHR_FUNC_ENTER(unit);

    *list_ptr = NULL;

    SHR_IF_ERR_EXIT(bcmltm_db_dm_track_info_get(unit, sid, &track_info));

    if ((track_info == NULL) ||
        (track_info->track_rev_xfrms == NULL)) {
        SHR_EXIT();
    }

    *list_ptr = track_info->track_rev_xfrms;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get reverse track index transform.
 *
 * This routine gets the reverse track index transform
 * for the specified transform index for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] xfrm_idx Field transform index.
 * \param [out] xfrm_ptr Returning pointer to field transform.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
track_rev_xfrm_get(int unit,
                   bcmlrd_sid_t sid,
                   uint32_t xfrm_idx,
                   const bcmltm_db_dm_xfrm_t **xfrm_ptr)
{
    const bcmltm_db_dm_xfrm_list_t *list = NULL;

    SHR_FUNC_ENTER(unit);

    *xfrm_ptr = NULL;

    SHR_IF_ERR_EXIT(track_rev_xfrm_list_get(unit, sid, &list));
    if ((list == NULL) ||
        (list->xfrms == NULL) ||
        (xfrm_idx >= list->num_xfrms)) {
        SHR_EXIT();
    }

    *xfrm_ptr = list->xfrms[xfrm_idx];

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get field transform list for given selection ID and transform type.
 *
 * This routine gets the field transform list for the specified
 * field selection ID and transform type for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Selection ID to get data for.
 * \param [in] xfrm_type Field transform type to get data for.
 * \param [out] list_ptr Returning pointer to field transform list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dm_xfrm_list_get(int unit,
                 bcmlrd_sid_t sid,
                 bcmltm_field_selection_id_t selection_id,
                 bcmltm_field_xfrm_type_t xfrm_type,
                 const bcmltm_db_dm_xfrm_list_t **list_ptr)
{
    const bcmltm_db_dm_fs_t *fs = NULL;
    const bcmltm_db_dm_xfrm_list_t *list = NULL;

    SHR_FUNC_ENTER(unit);

    *list_ptr = NULL;

    SHR_IF_ERR_EXIT(dm_fs_get(unit, sid, selection_id, &fs));
    if ((fs == NULL) || (fs->xfrms == NULL)) {
        SHR_EXIT();
    }

    if (xfrm_type == BCMLTM_FIELD_XFRM_TYPE_FWD_KEY) {
        list = fs->xfrms->fwd_keys;
    } else if (xfrm_type == BCMLTM_FIELD_XFRM_TYPE_FWD_VALUE) {
        list = fs->xfrms->fwd_values;
    } else if (xfrm_type == BCMLTM_FIELD_XFRM_TYPE_REV_KEY) {
        list = fs->xfrms->rev_keys;
    } else if (xfrm_type == BCMLTM_FIELD_XFRM_TYPE_REV_VALUE){
        list = fs->xfrms->rev_values;
    }

    *list_ptr = list;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get field transform for given selection ID and transform type.
 *
 * This routine gets the field transform for the specified
 * field selection ID, transform type, and index,
 * for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] xfrm_type Field transform type.
 * \param [in] xfrm_idx Field transform index.
 * \param [out] xfrm_ptr Returning pointer to field transform.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dm_xfrm_get(int unit,
            bcmlrd_sid_t sid,
            bcmltm_field_selection_id_t selection_id,
            bcmltm_field_xfrm_type_t xfrm_type,
            uint32_t xfrm_idx,
            const bcmltm_db_dm_xfrm_t **xfrm_ptr)
{
    const bcmltm_db_dm_xfrm_list_t *list = NULL;

    SHR_FUNC_ENTER(unit);

    *xfrm_ptr = NULL;

    SHR_IF_ERR_EXIT
        (dm_xfrm_list_get(unit, sid, selection_id, xfrm_type, &list));
    if ((list == NULL) ||
        (list->xfrms == NULL) ||
        (xfrm_idx >= list->num_xfrms)) {
        SHR_EXIT();
    }

    *xfrm_ptr = list->xfrms[xfrm_idx];

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get field validation list for given selection ID and validate type.
 *
 * This routine gets the field validation list for the specified
 * field selection ID and validation type for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Selection ID to get data for.
 * \param [in] val_type Field validation type to get data for.
 * \param [out] list_ptr Returning pointer to field validation list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_validate_list_get(int unit,
                        bcmlrd_sid_t sid,
                        bcmltm_field_selection_id_t selection_id,
                        bcmltm_field_validate_type_t val_type,
                        const bcmltm_db_validate_list_t **list_ptr)
{
    const bcmltm_db_dm_fs_t *fs = NULL;
    const bcmltm_db_validate_list_t *list = NULL;

    SHR_FUNC_ENTER(unit);

    *list_ptr = NULL;

    SHR_IF_ERR_EXIT(dm_fs_get(unit, sid, selection_id, &fs));
    if ((fs == NULL) || (fs->field_validations == NULL)) {
        SHR_EXIT();
    }

    if (val_type == BCMLTM_FIELD_VALIDATE_TYPE_KEY) {
        list = fs->field_validations->keys;
    } else if (val_type == BCMLTM_FIELD_VALIDATE_TYPE_VALUE) {
        list = fs->field_validations->values;
    }

    *list_ptr = list;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get field validation for given selection ID and validate type.
 *
 * This routine gets the field validation for the specified
 * field selection ID and validation type for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Selection ID to get data for.
 * \param [in] val_type Field validation type to get data for.
 * \param [in] val_idx Field validation index.
 * \param [out] validate_ptr Returning pointer to field validation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_validate_get(int unit,
                   bcmlrd_sid_t sid,
                   bcmltm_field_selection_id_t selection_id,
                   bcmltm_field_validate_type_t val_type,
                   uint32_t val_idx,
                   const bcmltm_db_validate_t **validate_ptr)
{
    const bcmltm_db_validate_list_t *list = NULL;

    SHR_FUNC_ENTER(unit);

    *validate_ptr = NULL;

    SHR_IF_ERR_EXIT
        (field_validate_list_get(unit, sid, selection_id, val_type, &list));
    if ((list == NULL) ||
        (list->validations == NULL) ||
        (val_idx >= list->num_validations)) {
        SHR_EXIT();
    }

    *validate_ptr = &list->validations[val_idx];

 exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_init(int unit, uint32_t sid_max_count)
{
    SHR_FUNC_ENTER(unit);

    /* Create logical table database information */
    SHR_IF_ERR_EXIT(bcmltm_db_info_create(unit, sid_max_count));

exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_info_destroy(unit);
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_db_cleanup(int unit)
{
    /* Destroy logical table database information */
    bcmltm_db_info_destroy(unit);

    return;
}

void
bcmltm_db_info_dump_by_sid(int unit, bcmlrd_sid_t sid,
                           shr_pb_t *pb)
{
    int rv;
    const bcmltm_db_lt_info_t *info;
    SHR_PB_LOCAL_DECL(pb);

    rv = bcmltm_db_lt_info_get(unit, sid, &info);
    if (SHR_SUCCESS(rv)) {
        bcmltm_db_lt_info_dump(unit, sid, info, pb);
    }

    SHR_PB_LOCAL_DONE();
}

void
bcmltm_db_info_dump_by_name(int unit, const char *name,
                            shr_pb_t *pb)
{
    uint32_t ltid_count;
    bcmlrd_sid_t ltid;
    const bcmlrd_table_rep_t *tbl;
    SHR_PB_LOCAL_DECL(pb);

    ltid_count = bcmltm_db_table_count_get();

    for (ltid = 0; ltid < ltid_count; ltid++) {
        tbl = bcmlrd_table_get(ltid);
        if (tbl == NULL) {
            continue;
        }

        if (sal_strcmp(tbl->name, name) == 0) {
            bcmltm_db_info_dump_by_sid(unit, ltid, pb);
        }
    }

    SHR_PB_LOCAL_DONE();
}

void
bcmltm_db_info_dump(int unit,
                    shr_pb_t *pb)
{
    int rv;
    uint32_t sid_max_count;
    bcmlrd_sid_t sid;
    SHR_PB_LOCAL_DECL(pb);

    rv = bcmltm_db_info_sid_max_count_get(unit, &sid_max_count);
    if (SHR_SUCCESS(rv)) {
        for (sid = 0; sid < sid_max_count; sid++) {
            bcmltm_db_info_dump_by_sid(unit, sid, pb);
        }
    }

    SHR_PB_LOCAL_DONE();
}

int
bcmltm_db_table_attr_get(int unit, bcmlrd_sid_t sid,
                         const bcmltm_table_attr_t **attr_ptr)
{
    const bcmltm_db_lt_info_t *lt_info;

    SHR_FUNC_ENTER(unit);

    *attr_ptr = NULL;

    DB_LT_INFO_GET_IF_NULL_EXIT(unit, sid, lt_info);

    *attr_ptr = DB_ATTR_INFO(lt_info);

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_table_type_get(int unit, bcmlrd_sid_t sid,
                         bcmltm_table_type_t *type)
{
    const bcmltm_table_attr_t *attr;

    SHR_FUNC_ENTER(unit);

    *type = 0;

    SHR_IF_ERR_EXIT(bcmltm_db_table_attr_get(unit, sid, &attr));
    BCMLTM_DB_IF_NULL_PTR_EXIT(attr);

    *type = attr->type;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_table_opcode_is_supported(int unit,
                                    bcmlrd_sid_t sid,
                                    bcmlt_opcode_t opcode,
                                    bool *is_supported)
{
    const bcmltm_table_attr_t *attr;
    bcmltm_opcodes_t table_opcodes;

    SHR_FUNC_ENTER(unit);

    *is_supported = FALSE;

    SHR_IF_ERR_EXIT
        (bcmltm_db_table_attr_get(unit, sid, &attr));
    table_opcodes = attr->opcodes;

    if (opcode == BCMLT_OPCODE_NOP) {
        *is_supported = TRUE;
    } else if ((opcode == BCMLT_OPCODE_INSERT) &&
        (table_opcodes & BCMLTM_OPCODE_FLAG_INSERT)) {
        *is_supported = TRUE;
    } else if ((opcode == BCMLT_OPCODE_LOOKUP) &&
        (table_opcodes & BCMLTM_OPCODE_FLAG_LOOKUP)) {
        *is_supported = TRUE;
    } else if ((opcode == BCMLT_OPCODE_DELETE) &&
        (table_opcodes & BCMLTM_OPCODE_FLAG_DELETE)) {
        *is_supported = TRUE;
    } else if ((opcode == BCMLT_OPCODE_UPDATE) &&
        (table_opcodes & BCMLTM_OPCODE_FLAG_UPDATE)) {
        *is_supported = TRUE;
    } else if ((opcode == BCMLT_OPCODE_TRAVERSE) &&
        (table_opcodes & BCMLTM_OPCODE_FLAG_TRAVERSE)) {
        *is_supported = TRUE;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_track_index_max_get(int unit, bcmlrd_sid_t sid,
                                 uint32_t *track_index_max)
{
    const bcmltm_track_index_t *track_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_get(unit, sid, &track_info));

    if (track_info == NULL) {
        /* There is no track index information */
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *track_index_max = track_info->track_index_max;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_track_index_is_mapped(int unit, bcmlrd_sid_t sid,
                                   bool *is_mapped)
{
    const bcmltm_db_dm_info_t *dm_info;

    SHR_FUNC_ENTER(unit);

    *is_mapped = FALSE;

    DB_DM_INFO_GET_IF_NULL_EXIT(unit, sid, dm_info);

    *is_mapped = dm_info->track_index_map;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_track_index_get(int unit, bcmlrd_sid_t sid,
                             const bcmltm_track_index_t **index_ptr)
{
    const bcmltm_db_dm_info_t *dm_info;

    SHR_FUNC_ENTER(unit);

    *index_ptr = NULL;

    DB_DM_INFO_GET_IF_NULL_EXIT(unit, sid, dm_info);

    *index_ptr = dm_info->track_index;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_ee_lt_index_get(int unit, bcmlrd_sid_t sid,
                             const bcmltm_ee_index_info_t **index_ptr)
{
    const bcmltm_db_dm_info_t *dm_info;

    SHR_FUNC_ENTER(unit);

    *index_ptr = NULL;

    DB_DM_INFO_GET_IF_NULL_EXIT(unit, sid, dm_info);

    *index_ptr = dm_info->ee_lt_index;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_track_to_pt_copy_get(int unit, bcmlrd_sid_t sid,
                                  const bcmltm_wb_copy_list_t **list_ptr)
{
    const bcmltm_db_dm_info_t *dm_info;

    SHR_FUNC_ENTER(unit);

    *list_ptr = NULL;

    DB_DM_INFO_GET_IF_NULL_EXIT(unit, sid, dm_info);

    *list_ptr = dm_info->track_to_pt_copy;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_track_to_api_fields_get(int unit,
                                     bcmlrd_sid_t sid,
                        const bcmltm_field_select_mapping_t **fields_ptr)
{
    const bcmltm_db_dm_track_info_t *track_info = NULL;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    SHR_IF_ERR_EXIT(bcmltm_db_dm_track_info_get(unit, sid, &track_info));
    if (track_info == NULL) {
        SHR_EXIT();
    }

    *fields_ptr = track_info->track_to_api;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_track_rev_xfrm_num_get(int unit,
                                    bcmlrd_sid_t sid,
                                    uint32_t *num)
{
    const bcmltm_db_dm_xfrm_list_t *list = NULL;

    SHR_FUNC_ENTER(unit);

    *num = 0;

    SHR_IF_ERR_EXIT(track_rev_xfrm_list_get(unit, sid, &list));
    if (list == NULL) {
        SHR_EXIT();
    }

    *num = list->num_xfrms;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_track_rev_xfrm_params_get(int unit,
                                       bcmlrd_sid_t sid,
                                       uint32_t xfrm_idx,
                          const bcmltm_lta_transform_params_t **params_ptr)
{
    const bcmltm_db_dm_xfrm_t *xfrm = NULL;

    SHR_FUNC_ENTER(unit);

    *params_ptr = NULL;

    SHR_IF_ERR_EXIT(track_rev_xfrm_get(unit, sid, xfrm_idx, &xfrm));
    if (xfrm == NULL) {
        SHR_EXIT();
    }

    *params_ptr = xfrm->params;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_track_rev_xfrm_in_fields_get(int unit,
                                          bcmlrd_sid_t sid,
                                          uint32_t xfrm_idx,
                         const bcmltm_lta_select_field_list_t **fields_ptr)
{
    const bcmltm_db_dm_xfrm_t *xfrm = NULL;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    SHR_IF_ERR_EXIT(track_rev_xfrm_get(unit, sid, xfrm_idx, &xfrm));
    if (xfrm == NULL) {
        SHR_EXIT();
    }

    *fields_ptr = xfrm->in_fields;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_track_rev_xfrm_out_fields_get(int unit,
                                           bcmlrd_sid_t sid,
                                           uint32_t xfrm_idx,
                          const bcmltm_lta_select_field_list_t **fields_ptr)
{
    const bcmltm_db_dm_xfrm_t *xfrm = NULL;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    SHR_IF_ERR_EXIT(track_rev_xfrm_get(unit, sid, xfrm_idx, &xfrm));
    if (xfrm == NULL) {
        SHR_EXIT();
    }

    *fields_ptr = xfrm->out_fields;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_pt_list_get(int unit, bcmlrd_sid_t sid,
                         const bcmltm_pt_list_t **pt_list_ptr)
{
    const bcmltm_db_dm_info_t *dm_info;

    SHR_FUNC_ENTER(unit);

    *pt_list_ptr = NULL;

    DB_DM_INFO_GET_IF_NULL_EXIT(unit, sid, dm_info);

    *pt_list_ptr = dm_info->pt_list;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_pt_keyed_get(int unit, bcmlrd_sid_t sid,
                          bcmptm_op_type_t ptm_op,
                          const bcmltm_pt_keyed_op_info_t **pt_keyed_ptr)
{
    const bcmltm_db_dm_info_t *dm_info;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    *pt_keyed_ptr = NULL;

    DB_DM_INFO_GET_IF_NULL_EXIT(unit, sid, dm_info);

    switch (ptm_op) {
    case BCMPTM_OP_INSERT:
    case BCMPTM_OP_DELETE:
        idx = BCMLTM_DB_DM_PT_KEYED_INS_DEL_IDX;
        break;

    case BCMPTM_OP_LOOKUP:
    case BCMPTM_OP_GET_NEXT:
    case BCMPTM_OP_GET_FIRST:
        idx = BCMLTM_DB_DM_PT_KEYED_LK_TRV_IDX;
        break;

    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
        break;
    }

    *pt_keyed_ptr = dm_info->pt_keyed_list[idx];

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_field_mapping_get(int unit,
                               bcmlrd_sid_t sid,
                               bcmltm_field_type_t field_type,
                               bcmltm_field_dir_t field_dir,
                               const bcmltm_field_select_mapping_t
                               **field_mapping_ptr)
{
    const bcmltm_db_dm_info_t *dm_info;

    SHR_FUNC_ENTER(unit);

    *field_mapping_ptr = NULL;

    DB_DM_INFO_GET_IF_NULL_EXIT(unit, sid, dm_info);

    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        if (field_dir == BCMLTM_FIELD_DIR_IN) {
            *field_mapping_ptr = dm_info->in_key_fields;
        } else {
            *field_mapping_ptr = dm_info->out_key_fields;
        }
    } else {
        /* Both input and output are the same for values */
        *field_mapping_ptr = dm_info->value_fields;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_req_key_field_mapping_get(int unit, bcmlrd_sid_t sid,
                                       const bcmltm_field_select_mapping_t
                                       **field_mapping_ptr)
{
    const bcmltm_db_dm_info_t *dm_info;

    SHR_FUNC_ENTER(unit);

    *field_mapping_ptr = NULL;

    DB_DM_INFO_GET_IF_NULL_EXIT(unit, sid, dm_info);

    *field_mapping_ptr = dm_info->req_key_fields;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_alloc_key_field_mapping_get(int unit, bcmlrd_sid_t sid,
                                         const bcmltm_field_select_mapping_t
                                         **field_mapping_ptr)
{
    const bcmltm_db_dm_info_t *dm_info;

    SHR_FUNC_ENTER(unit);

    *field_mapping_ptr = NULL;

    DB_DM_INFO_GET_IF_NULL_EXIT(unit, sid, dm_info);

    *field_mapping_ptr = dm_info->alloc_key_fields;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_fixed_keys_get(int unit,
                            bcmlrd_sid_t sid,
                            const bcmltm_fixed_field_list_t
                            **fixed_fields_ptr)
{
    const bcmltm_db_dm_info_t *dm_info;

    SHR_FUNC_ENTER(unit);

    *fixed_fields_ptr = NULL;

    DB_DM_INFO_GET_IF_NULL_EXIT(unit, sid, dm_info);

    *fixed_fields_ptr = dm_info->fixed_key_fields;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_fixed_values_get(int unit,
                              bcmlrd_sid_t sid,
                              const bcmltm_fixed_field_list_t
                              **fixed_fields_ptr)
{
    const bcmltm_db_dm_info_t *dm_info;

    SHR_FUNC_ENTER(unit);

    *fixed_fields_ptr = NULL;

    DB_DM_INFO_GET_IF_NULL_EXIT(unit, sid, dm_info);

    *fixed_fields_ptr = dm_info->fixed_value_fields;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_read_only_list_get(int unit,
                              bcmlrd_sid_t sid,
                              const bcmltm_ro_field_list_t
                              **read_only_list_ptr)
{
    const bcmltm_db_dm_info_t *dm_info;

    SHR_FUNC_ENTER(unit);

    *read_only_list_ptr = NULL;

    DB_DM_INFO_GET_IF_NULL_EXIT(unit, sid, dm_info);

    *read_only_list_ptr = dm_info->read_only_fields;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_wide_field_list_get(int unit,
                              bcmlrd_sid_t sid,
                              const bcmltm_wide_field_list_t
                              **wide_field_list_ptr)
{
    const bcmltm_db_dm_info_t *dm_info;

    SHR_FUNC_ENTER(unit);

    *wide_field_list_ptr = NULL;

    DB_DM_INFO_GET_IF_NULL_EXIT(unit, sid, dm_info);

    *wide_field_list_ptr = dm_info->wide_fields;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_xfrm_num_get(int unit, bcmlrd_sid_t sid, int xfrm_type,
                       uint32_t *num)
{
    const bcmltm_db_xfrm_list_t *xfrm_list;

    SHR_FUNC_ENTER(unit);

    *num = 0;

    SHR_IF_ERR_EXIT(xfrm_list_get(unit, sid, xfrm_type, &xfrm_list));

    if (xfrm_list == NULL) {
        SHR_EXIT();
    }

    *num = xfrm_list->num_xfrms;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_xfrm_params_get(int unit, bcmlrd_sid_t sid,
                          int xfrm_type,
                          uint32_t xfrm_idx,
                          const bcmltm_lta_transform_params_t **params_ptr)
{
    const bcmltm_db_xfrm_list_t *xfrm_list;

    SHR_FUNC_ENTER(unit);

    *params_ptr = NULL;

    SHR_IF_ERR_EXIT(xfrm_list_get(unit, sid, xfrm_type, &xfrm_list));

    if ((xfrm_list == NULL) || (xfrm_idx >= xfrm_list->num_xfrms)) {
        SHR_EXIT();
    }

    *params_ptr = xfrm_list->xfrms[xfrm_idx].xfrm_params;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_xfrm_in_fields_get(int unit, bcmlrd_sid_t sid,
                             int xfrm_type,
                             uint32_t xfrm_idx,
                             const bcmltm_lta_field_list_t **fields_ptr)
{
    const bcmltm_db_xfrm_list_t *xfrm_list;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    SHR_IF_ERR_EXIT(xfrm_list_get(unit, sid, xfrm_type, &xfrm_list));

    if ((xfrm_list == NULL) || (xfrm_idx >= xfrm_list->num_xfrms)) {
        SHR_EXIT();
    }

    *fields_ptr = xfrm_list->xfrms[xfrm_idx].fin;

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_xfrm_in_key_fields_get(int unit, bcmlrd_sid_t sid,
                                 int xfrm_type,
                                 uint32_t xfrm_idx,
                                 const bcmltm_lta_field_list_t **fields_ptr)
{
    const bcmltm_db_xfrm_list_t *xfrm_list;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    SHR_IF_ERR_EXIT(xfrm_list_get(unit, sid, xfrm_type, &xfrm_list));

    if ((xfrm_list == NULL) || (xfrm_idx >= xfrm_list->num_xfrms)) {
        SHR_EXIT();
    }

    *fields_ptr = xfrm_list->xfrms[xfrm_idx].fin_key;


exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_xfrm_out_fields_get(int unit, bcmlrd_sid_t sid,
                              int xfrm_type,
                              uint32_t xfrm_idx,
                              const bcmltm_lta_field_list_t **fields_ptr)
{
    const bcmltm_db_xfrm_list_t *xfrm_list;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    SHR_IF_ERR_EXIT(xfrm_list_get(unit, sid, xfrm_type, &xfrm_list));

    if ((xfrm_list == NULL) || (xfrm_idx >= xfrm_list->num_xfrms)) {
        SHR_EXIT();
    }

    *fields_ptr = xfrm_list->xfrms[xfrm_idx].fout;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_xfrm_copy_fields_get(int unit, bcmlrd_sid_t sid,
                               int to_xfrm_type,
                               uint32_t to_xfrm_idx,
                               int from_xfrm_type,
                               uint32_t from_xfrm_idx,
                               const bcmltm_lta_field_list_t **fields_ptr)
{
    const bcmltm_db_xfrm_info_t *xfrm_info;
    const bcmltm_db_xfrm_list_t *from_list;
    const bcmltm_db_xfrm_list_t *to_list;
    const bcmltm_db_xfrm_list_t *copy_list;
    uint32_t num_from = 0;
    uint32_t num_to = 0;
    uint32_t num_copy = 0;
    uint32_t copy_idx;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    DB_XFRM_INFO_GET_IF_NULL_EXIT(unit, sid, xfrm_info);

    /*
     * Cases supported for LTA to LTA copy are:
     *   From: Reverse Key    -  To: Forward Key
     *   From: Reverse Value  -  To: Forward Value
     *   From: Reverse Key    -  To: Reverse Value In Key
     */
    if ((from_xfrm_type == BCMLTM_DB_XFRM_TYPE_REV_KEY_TRANSFORM) &&
        (to_xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM)) {
        from_list = xfrm_info->rev_key_xfrm_list;
        to_list = xfrm_info->fwd_key_xfrm_list;
        copy_list = xfrm_info->rkey_to_fkey_xfrm_list;
    } else if ((from_xfrm_type == BCMLTM_DB_XFRM_TYPE_REV_VALUE_TRANSFORM) &&
        (to_xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_VALUE_TRANSFORM)) {
        from_list = xfrm_info->rev_value_xfrm_list;
        to_list = xfrm_info->fwd_value_xfrm_list;
        copy_list = xfrm_info->rvalue_to_fvalue_xfrm_list;
    } else if ((from_xfrm_type == BCMLTM_DB_XFRM_TYPE_REV_KEY_TRANSFORM) &&
        (to_xfrm_type == BCMLTM_DB_XFRM_TYPE_REV_VALUE_TRANSFORM)) {
        from_list = xfrm_info->rev_key_xfrm_list;
        to_list = xfrm_info->rev_value_xfrm_list;
        copy_list = xfrm_info->rkey_to_rvalue_xfrm_list;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if ((from_list == NULL) || (to_list == NULL) || (copy_list == NULL)) {
        SHR_EXIT();
    }

    num_to = to_list->num_xfrms;
    num_from = from_list->num_xfrms;
    num_copy = copy_list->num_xfrms;
    copy_idx = XFRM_COPY_IDX(from_xfrm_idx, num_from, to_xfrm_idx);

    /* Check transform list indexes */
    if ((from_xfrm_idx >= num_from) || (to_xfrm_idx >= num_to) ||
        (copy_idx >= num_copy)) {
        SHR_EXIT();
    }

    *fields_ptr = copy_list->xfrms[copy_idx].fin;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_fv_num_get(int unit, bcmlrd_sid_t sid,
                     bcmltm_field_type_t field_type,
                     uint32_t *num)
{
    const bcmltm_db_fv_info_t *fv_info;
    const bcmltm_db_fv_list_t *fv_list;

    SHR_FUNC_ENTER(unit);

    *num = 0;

    DB_FV_INFO_GET_IF_NULL_EXIT(unit, sid, fv_info);

    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        fv_list = fv_info->validate_keys;
    } else {
        fv_list = fv_info->validate_values;
    }

    if (fv_list == NULL) {
        SHR_EXIT();
    }

    *num = fv_list->num_fvs;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_fv_validate_params_get(int unit, bcmlrd_sid_t sid,
                                 bcmltm_field_type_t field_type,
                                 uint32_t fld_idx,
                                 const bcmltm_lta_validate_params_t
                                 **params_ptr)
{
    const bcmltm_db_fv_info_t *fv_info;
    const bcmltm_db_fv_list_t *fv_list;

    SHR_FUNC_ENTER(unit);

    *params_ptr = NULL;

    DB_FV_INFO_GET_IF_NULL_EXIT(unit, sid, fv_info);

    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        fv_list = fv_info->validate_keys;
    } else {
        fv_list = fv_info->validate_values;
    }

    if ((fv_list == NULL) || (fld_idx >= fv_list->num_fvs)) {
        SHR_EXIT();
    }

    *params_ptr = fv_list->fvs[fld_idx].validate_params;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_fv_src_fields_get(int unit, bcmlrd_sid_t sid,
                            bcmltm_field_type_t field_type,
                            uint32_t fld_idx,
                            const bcmltm_lta_field_list_t **fields_ptr)
{
    const bcmltm_db_fv_info_t *fv_info;
    const bcmltm_db_fv_list_t *fv_list;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    DB_FV_INFO_GET_IF_NULL_EXIT(unit, sid, fv_info);

    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        fv_list = fv_info->validate_keys;
    } else {
        fv_list = fv_info->validate_values;
    }

    if ((fv_list == NULL) || (fld_idx >= fv_list->num_fvs)) {
        SHR_EXIT();
    }

    *fields_ptr = fv_list->fvs[fld_idx].fsrc;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_fv_copy_fields_get(int unit, bcmlrd_sid_t sid,
                             bcmltm_field_type_t field_type,
                             uint32_t validate_idx,
                             int from_xfrm_type,
                             uint32_t from_xfrm_idx,
                             const bcmltm_lta_field_list_t **fields_ptr)
{
    const bcmltm_db_fv_info_t *fv_info;
    const bcmltm_db_xfrm_info_t *xfrm_info;
    const bcmltm_db_xfrm_list_t *from_list;
    const bcmltm_db_fv_list_t *to_list;
    const bcmltm_db_fv_list_t *copy_list;
    uint32_t num_from = 0;
    uint32_t copy_idx;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    DB_FV_INFO_GET_IF_NULL_EXIT(unit, sid, fv_info);
    DB_XFRM_INFO_GET_IF_NULL_EXIT(unit, sid, xfrm_info);

    /*
     * Cases supported for LTA to LTA copy are:
     *   From: Reverse Key Transform  -  To: Key Validation
     */
    if ((from_xfrm_type == BCMLTM_DB_XFRM_TYPE_REV_VALUE_TRANSFORM) &&
        (field_type == BCMLTM_FIELD_TYPE_VALUE)) {
        from_list = xfrm_info->rev_value_xfrm_list;
        to_list = fv_info->validate_values;
        copy_list = fv_info->rvalue_to_value;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if ((from_list == NULL) || (to_list == NULL) || (copy_list == NULL)) {
        SHR_EXIT();
    }

    num_from = from_list->num_xfrms;
    copy_idx = FV_COPY_IDX(from_xfrm_idx, num_from, validate_idx);

    /* Check list indexes */
    if ((from_xfrm_idx >= num_from) || (validate_idx >= to_list->num_fvs) ||
        (copy_idx >= copy_list->num_fvs)) {
        SHR_EXIT();
    }

    *fields_ptr = copy_list->fvs[copy_idx].fsrc;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_fs_core_info_get(int unit,
                           bcmlrd_sid_t sid,
                           bcmltm_db_fs_core_info_t *info)
{
    const bcmltm_db_dm_fs_info_t *fs_info = NULL;
    const bcmltm_db_dm_fs_t *fs;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    sal_memset(info, 0, sizeof(*info));

    SHR_IF_ERR_EXIT(bcmltm_db_dm_fs_info_get(unit, sid, &fs_info));
    if (fs_info == NULL) {
        SHR_EXIT();
    }

    /* Sanity check */
    if (fs_info->num_selections > BCMLTM_DB_FIELD_SELECT_MAX_COUNT) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    info->num_levels = fs_info->num_levels;
    info->num_selections = fs_info->num_selections;
    info->num_with_selectors = fs_info->num_with_selectors;

    for (i = 0; i < fs_info->num_selections; i++) {
        fs = &fs_info->selections[i];
        info->selections[i].selection_id = fs->selection_id;
        info->selections[i].selector_type = fs->selector_type;
        info->selections[i].level = fs->level;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_fsm_get(int unit,
                     bcmlrd_sid_t sid,
                     bcmltm_field_selection_id_t selection_id,
                     const bcmltm_field_select_maps_t **fsm_ptr)
{
    const bcmltm_db_dm_fs_t *fs = NULL;

    SHR_FUNC_ENTER(unit);

    *fsm_ptr = NULL;

    SHR_IF_ERR_EXIT(dm_fs_get(unit, sid, selection_id, &fs));
    if (fs == NULL) {
        SHR_EXIT();
    }

    *fsm_ptr = fs->fsm;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_apic_key_fields_get(int unit,
                                 bcmlrd_sid_t sid,
                                 bcmltm_field_dir_t field_dir,
                           const bcmltm_field_select_mapping_t **fields_ptr)
{
    const bcmltm_db_dm_fs_t *fs = NULL;
    bcmltm_field_selection_id_t selection_id;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    /*
     * Keys do not have any selector, so they are in
     * the unconditional selection group.
     */
    selection_id = BCMLTM_FIELD_SELECTION_ID_UNCOND;

    SHR_IF_ERR_EXIT(dm_fs_get(unit, sid, selection_id, &fs));
    if ((fs == NULL) || (fs->apic_fields == NULL)) {
        SHR_EXIT();
    }

    /* Both input and output direction shared same data */
    *fields_ptr = fs->apic_fields->keys;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_apic_req_key_fields_get(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_field_dir_t field_dir,
                               const bcmltm_field_select_mapping_t **fields_ptr)
{
    const bcmltm_db_dm_fs_t *fs = NULL;
    bcmltm_field_selection_id_t selection_id;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    /*
     * Keys do not have any selector, so they are in
     * the unconditional selection group.
     */
    selection_id = BCMLTM_FIELD_SELECTION_ID_UNCOND;

    SHR_IF_ERR_EXIT(dm_fs_get(unit, sid, selection_id, &fs));
    if ((fs == NULL) || (fs->apic_fields == NULL)) {
        SHR_EXIT();
    }

    /* Both input and output direction shared same data */
    *fields_ptr = fs->apic_fields->req_keys;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_apic_alloc_key_fields_get(int unit,
                                       bcmlrd_sid_t sid,
                                       bcmltm_field_dir_t field_dir,
                          const bcmltm_field_select_mapping_t **fields_ptr)
{
    const bcmltm_db_dm_fs_t *fs = NULL;
    bcmltm_field_selection_id_t selection_id;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    /*
     * Keys do not have any selector, so they are in
     * the unconditional selection group.
     */
    selection_id = BCMLTM_FIELD_SELECTION_ID_UNCOND;

    SHR_IF_ERR_EXIT(dm_fs_get(unit, sid, selection_id, &fs));
    if ((fs == NULL) || (fs->apic_fields == NULL)) {
        SHR_EXIT();
    }

    /* Both input and output direction shared same data */
    *fields_ptr = fs->apic_fields->alloc_keys;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_apic_value_fields_get(int unit,
                                   bcmlrd_sid_t sid,
                                   bcmltm_field_selection_id_t selection_id,
                                   bcmltm_field_dir_t field_dir,
                             const bcmltm_field_select_mapping_t **fields_ptr)
{
    const bcmltm_db_dm_fs_t *fs = NULL;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    SHR_IF_ERR_EXIT(dm_fs_get(unit, sid, selection_id, &fs));
    if ((fs == NULL) || (fs->apic_fields == NULL)) {
        SHR_EXIT();
    }

    /* Both input and output direction shared same data */
    *fields_ptr = fs->apic_fields->values;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_apic_all_value_fields_get(int unit,
                                       bcmlrd_sid_t sid,
                                       bcmltm_field_dir_t field_dir,
                          const bcmltm_field_select_mapping_t **fields_ptr)
{
    const bcmltm_db_dm_info_t *dm_info = NULL;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    SHR_IF_ERR_EXIT(bcmltm_db_dm_info_get(unit, sid, &dm_info));
    if (dm_info == NULL) {
        SHR_EXIT();
    }

    /* Both input and output direction shared same data */
    *fields_ptr = dm_info->apic_all_value_fields;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_key_fields_get(int unit,
                            bcmlrd_sid_t sid,
                            bcmltm_field_dir_t field_dir,
                            const bcmltm_field_select_mapping_t **fields_ptr)
{
    const bcmltm_db_dm_fs_t *fs = NULL;
    bcmltm_field_selection_id_t selection_id;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    /*
     * Keys do not have any selector, so they are in
     * the unconditional selection group.
     */
    selection_id = BCMLTM_FIELD_SELECTION_ID_UNCOND;

    SHR_IF_ERR_EXIT(dm_fs_get(unit, sid, selection_id, &fs));
    if ((fs == NULL) || (fs->field_maps == NULL)) {
        SHR_EXIT();
    }

    if (field_dir == BCMLTM_FIELD_DIR_IN) {
        *fields_ptr = fs->field_maps->in_keys;
    } else {
        *fields_ptr = fs->field_maps->out_keys;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_req_key_fields_get(int unit,
                                bcmlrd_sid_t sid,
                                bcmltm_field_dir_t field_dir,
                          const bcmltm_field_select_mapping_t **fields_ptr)
{
    const bcmltm_db_dm_fs_t *fs = NULL;
    bcmltm_field_selection_id_t selection_id;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    /*
     * Keys do not have any selector, so they are in
     * the unconditional selection group.
     */
    selection_id = BCMLTM_FIELD_SELECTION_ID_UNCOND;

    SHR_IF_ERR_EXIT(dm_fs_get(unit, sid, selection_id, &fs));
    if ((fs == NULL) || (fs->field_maps == NULL)) {
        SHR_EXIT();
    }

    /* Both input and output direction shared same data */
    *fields_ptr = fs->field_maps->req_keys;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_alloc_key_fields_get(int unit,
                                  bcmlrd_sid_t sid,
                                  bcmltm_field_dir_t field_dir,
                            const bcmltm_field_select_mapping_t **fields_ptr)
{
    const bcmltm_db_dm_fs_t *fs = NULL;
    bcmltm_field_selection_id_t selection_id;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    /*
     * Keys do not have any selector, so they are in
     * the unconditional selection group.
     */
    selection_id = BCMLTM_FIELD_SELECTION_ID_UNCOND;

    SHR_IF_ERR_EXIT(dm_fs_get(unit, sid, selection_id, &fs));
    if ((fs == NULL) || (fs->field_maps == NULL)) {
        SHR_EXIT();
    }

    /* Both input and output direction shared same data */
    *fields_ptr = fs->field_maps->alloc_keys;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_value_fields_get(int unit,
                              bcmlrd_sid_t sid,
                              bcmltm_field_selection_id_t selection_id,
                              bcmltm_field_dir_t field_dir,
                              const bcmltm_field_select_mapping_t **fields_ptr)
{
    const bcmltm_db_dm_fs_t *fs = NULL;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    SHR_IF_ERR_EXIT(dm_fs_get(unit, sid, selection_id, &fs));
    if ((fs == NULL) || (fs->field_maps == NULL)) {
        SHR_EXIT();
    }

    /* Both input and output direction shared same data */
    *fields_ptr = fs->field_maps->values;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_fixed_key_fields_get(int unit,
                                  bcmlrd_sid_t sid,
                            const bcmltm_fixed_field_mapping_t **fields_ptr)
{
    const bcmltm_db_dm_fs_t *fs = NULL;
    bcmltm_field_selection_id_t selection_id;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    /*
     * Keys do not have any selector, so they are in
     * the unconditional selection group.
     */
    selection_id = BCMLTM_FIELD_SELECTION_ID_UNCOND;

    SHR_IF_ERR_EXIT(dm_fs_get(unit, sid, selection_id, &fs));
    if ((fs == NULL) || (fs->fixed_fields == NULL)) {
        SHR_EXIT();
    }

    *fields_ptr = fs->fixed_fields->keys;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_fixed_value_fields_get(int unit,
                                    bcmlrd_sid_t sid,
                                    bcmltm_field_selection_id_t selection_id,
                              const bcmltm_fixed_field_mapping_t **fields_ptr)
{
    const bcmltm_db_dm_fs_t *fs = NULL;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    SHR_IF_ERR_EXIT(dm_fs_get(unit, sid, selection_id, &fs));
    if ((fs == NULL) || (fs->fixed_fields == NULL)) {
        SHR_EXIT();
    }

    *fields_ptr = fs->fixed_fields->values;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_xfrm_num_get(int unit,
                          bcmlrd_sid_t sid,
                          bcmltm_field_selection_id_t selection_id,
                          bcmltm_field_xfrm_type_t xfrm_type,
                          uint32_t *num)
{
    const bcmltm_db_dm_xfrm_list_t *list = NULL;

    SHR_FUNC_ENTER(unit);

    *num = 0;

    SHR_IF_ERR_EXIT
        (dm_xfrm_list_get(unit, sid, selection_id, xfrm_type, &list));
    if (list == NULL) {
        SHR_EXIT();
    }

    *num = list->num_xfrms;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_xfrm_params_get(int unit,
                             bcmlrd_sid_t sid,
                             bcmltm_field_selection_id_t selection_id,
                             bcmltm_field_xfrm_type_t xfrm_type,
                             uint32_t xfrm_idx,
                             const bcmltm_lta_transform_params_t **params_ptr)
{
    const bcmltm_db_dm_xfrm_t *xfrm = NULL;

    SHR_FUNC_ENTER(unit);

    *params_ptr = NULL;

    SHR_IF_ERR_EXIT
        (dm_xfrm_get(unit, sid, selection_id, xfrm_type, xfrm_idx, &xfrm));
    if (xfrm == NULL) {
        SHR_EXIT();
    }

    *params_ptr = xfrm->params;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_xfrm_in_fields_get(int unit,
                                bcmlrd_sid_t sid,
                                bcmltm_field_selection_id_t selection_id,
                                bcmltm_field_xfrm_type_t xfrm_type,
                                uint32_t xfrm_idx,
                   const bcmltm_lta_select_field_list_t **fields_ptr)
{
    const bcmltm_db_dm_xfrm_t *xfrm = NULL;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    SHR_IF_ERR_EXIT
        (dm_xfrm_get(unit, sid, selection_id, xfrm_type, xfrm_idx, &xfrm));
    if (xfrm == NULL) {
        SHR_EXIT();
    }

    *fields_ptr = xfrm->in_fields;

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_xfrm_in_key_fields_get(int unit,
                                    bcmlrd_sid_t sid,
                                    bcmltm_field_selection_id_t selection_id,
                                    bcmltm_field_xfrm_type_t xfrm_type,
                                    uint32_t xfrm_idx,
                       const bcmltm_lta_select_field_list_t **fields_ptr)
{
    const bcmltm_db_dm_xfrm_t *xfrm = NULL;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    SHR_IF_ERR_EXIT
        (dm_xfrm_get(unit, sid, selection_id, xfrm_type, xfrm_idx, &xfrm));
    if (xfrm == NULL) {
        SHR_EXIT();
    }

    *fields_ptr = xfrm->in_key_fields;

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_xfrm_out_fields_get(int unit,
                                 bcmlrd_sid_t sid,
                                 bcmltm_field_selection_id_t selection_id,
                                 bcmltm_field_xfrm_type_t xfrm_type,
                                 uint32_t xfrm_idx,
                    const bcmltm_lta_select_field_list_t **fields_ptr)
{
    const bcmltm_db_dm_xfrm_t *xfrm = NULL;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    SHR_IF_ERR_EXIT
        (dm_xfrm_get(unit, sid, selection_id, xfrm_type, xfrm_idx, &xfrm));
    if (xfrm == NULL) {
        SHR_EXIT();
    }

    *fields_ptr = xfrm->out_fields;

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_validate_num_get(int unit,
                           bcmlrd_sid_t sid,
                           bcmltm_field_selection_id_t selection_id,
                           bcmltm_field_validate_type_t val_type,
                           uint32_t *num)
{
    const bcmltm_db_validate_list_t *list = NULL;

    SHR_FUNC_ENTER(unit);

    *num = 0;

    SHR_IF_ERR_EXIT
        (field_validate_list_get(unit, sid, selection_id, val_type, &list));
    if (list == NULL) {
        SHR_EXIT();
    }

    *num = list->num_validations;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_validate_params_get(int unit,
                              bcmlrd_sid_t sid,
                              bcmltm_field_selection_id_t selection_id,
                              bcmltm_field_validate_type_t val_type,
                              uint32_t val_idx,
                              const bcmltm_lta_validate_params_t **params_ptr)
{
    const bcmltm_db_validate_t *validate = NULL;

    SHR_FUNC_ENTER(unit);

    *params_ptr = NULL;

    SHR_IF_ERR_EXIT
        (field_validate_get(unit, sid, selection_id,
                            val_type, val_idx, &validate));
    if (validate == NULL) {
        SHR_EXIT();
    }

    *params_ptr = validate->params;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_validate_in_fields_get(int unit,
                                 bcmlrd_sid_t sid,
                                 bcmltm_field_selection_id_t selection_id,
                                 bcmltm_field_validate_type_t val_type,
                                 uint32_t val_idx,
                           const bcmltm_lta_select_field_list_t **fields_ptr)
{
    const bcmltm_db_validate_t *validate = NULL;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    SHR_IF_ERR_EXIT
        (field_validate_get(unit, sid, selection_id,
                            val_type, val_idx, &validate));
    if (validate == NULL) {
        SHR_EXIT();
    }

    *fields_ptr = validate->in_fields;

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_table_commit_list_get(int unit, bcmlrd_sid_t sid,
                                const bcmltm_table_commit_list_t **tc_list_ptr)
{
    const bcmltm_db_tc_info_t *tc_info;

    SHR_FUNC_ENTER(unit);

    *tc_list_ptr = NULL;

    DB_TC_INFO_GET_IF_NULL_EXIT(unit, sid, tc_info);

    *tc_list_ptr = tc_info->tc_list;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_table_entry_limit_handler_get(int unit,
                                        bcmlrd_sid_t sid,
               const bcmltd_table_entry_limit_handler_t  **handler_ptr)
{
    *handler_ptr = bcmltm_lta_intf_table_entry_limit_handler_get(unit, sid);

    return SHR_E_NONE;
}

int
bcmltm_db_table_entry_usage_handler_get(int unit,
                                        bcmlrd_sid_t sid,
               const bcmltd_table_entry_usage_handler_t  **handler_ptr)
{
    *handler_ptr = bcmltm_lta_intf_table_entry_usage_handler_get(unit, sid);

    return SHR_E_NONE;
}

int
bcmltm_db_cth_handler_get(int unit, bcmlrd_sid_t sid,
                          const bcmltd_table_handler_t **handler_ptr)
{
    const bcmltm_db_cth_info_t *cth_info;

    SHR_FUNC_ENTER(unit);

    *handler_ptr = NULL;

    DB_CTH_INFO_GET_IF_NULL_EXIT(unit, sid, cth_info);

    *handler_ptr = cth_info->handler;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_cth_handler_intf_type_get(int unit, bcmlrd_sid_t sid,
                                    bcmltm_cth_handler_intf_type_t *intf)
{
    const bcmltm_db_cth_info_t *cth_info;

    SHR_FUNC_ENTER(unit);

    DB_CTH_INFO_GET_IF_NULL_EXIT(unit, sid, cth_info);

    *intf = cth_info->intf_type;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_cth_table_params_get(int unit, bcmlrd_sid_t sid,
                               const bcmltm_lta_table_params_t
                               **table_params_ptr)
{
    const bcmltm_db_cth_info_t *cth_info;

    SHR_FUNC_ENTER(unit);

    *table_params_ptr = NULL;

    DB_CTH_INFO_GET_IF_NULL_EXIT(unit, sid, cth_info);

    *table_params_ptr = cth_info->table_params;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_cth_in_fields_get(int unit, bcmlrd_sid_t sid,
                            const bcmltm_lta_field_list_t **fields_ptr)
{
    const bcmltm_db_cth_info_t *cth_info;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    DB_CTH_INFO_GET_IF_NULL_EXIT(unit, sid, cth_info);

    *fields_ptr = cth_info->fin;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_cth_in_key_fields_get(int unit, bcmlrd_sid_t sid,
                                const bcmltm_lta_field_list_t **fields_ptr)
{
    const bcmltm_db_cth_info_t *cth_info;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    DB_CTH_INFO_GET_IF_NULL_EXIT(unit, sid, cth_info);

    *fields_ptr = cth_info->fin_keys;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_cth_out_fields_get(int unit, bcmlrd_sid_t sid,
                             const bcmltm_lta_field_list_t **fields_ptr)
{
    const bcmltm_db_cth_info_t *cth_info;

    SHR_FUNC_ENTER(unit);

    *fields_ptr = NULL;

    DB_CTH_INFO_GET_IF_NULL_EXIT(unit, sid, cth_info);

    *fields_ptr = cth_info->fout;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_ltm_fa_ltm_get(int unit, bcmlrd_sid_t sid,
                         const bcmltm_fa_ltm_t **ltm_ptr)
{
    const bcmltm_db_ltm_info_t *ltm_info;

    SHR_FUNC_ENTER(unit);

    *ltm_ptr = NULL;

    DB_LTM_INFO_GET_IF_NULL_EXIT(unit, sid, ltm_info);

    *ltm_ptr = ltm_info->fa_ltm;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_ltm_key_get(int unit, bcmlrd_sid_t sid,
                      const bcmltm_field_select_mapping_t **ltm_key_ptr)
{
    const bcmltm_db_ltm_info_t *ltm_info;

    SHR_FUNC_ENTER(unit);

    *ltm_key_ptr = NULL;

    DB_LTM_INFO_GET_IF_NULL_EXIT(unit, sid, ltm_info);

    *ltm_key_ptr = ltm_info->ltm_key;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_ltm_fs_list_get(int unit, bcmlrd_sid_t sid,
                          const bcmltm_field_spec_list_t **fs_list_ptr)
{
    const bcmltm_db_ltm_info_t *ltm_info;

    SHR_FUNC_ENTER(unit);

    *fs_list_ptr = NULL;

    DB_LTM_INFO_GET_IF_NULL_EXIT(unit, sid, ltm_info);

    *fs_list_ptr = ltm_info->fs_list;

 exit:
    SHR_FUNC_EXIT();
}
