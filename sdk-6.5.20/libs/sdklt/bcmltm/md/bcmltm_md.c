/*! \file bcmltm_md.c
 *
 * Logical Table Manager Metadata.
 *
 * This file contains the initialization for the LTM metadata.
 *
 * The LTM support the following main table categories:
 *
 * 1) Physical Table Pass Through (PT Pass Thru)
 * This is the most basic Logical Table case.
 * Each physical symbol will have a 'logical table' representation,
 * with a 1-1 mapping of all its physical fields to logical fields.
 *
 * 2) Logical Table
 * In this category, the logical tables are defined through various
 * mechanisms to produce a mapping from the API fields to
 * either the physical resources on the device or an internal
 * database.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>

#include <bsl/bsl.h>

#include <bcmdrd_config.h>

#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_md_pthru_internal.h>
#include <bcmltm/bcmltm_md_logical_internal.h>
#include <bcmltm/bcmltm_md.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA


/*
 * Check that table category is valid.
 */
#define LTM_MD_TABLE_CATG_VALID(_c)             \
    ((_c) < BCMLTM_TABLE_CATG_COUNT)


/*
 * Check that unit is valid.  Function exits if unit is invalid.
 * This macro needs to be used in conjunction with the SHR_ debug macros.
 */
#define LTM_MD_UNIT_CHECK(_u)                                    \
    do {                                                         \
        if (!BCMLTM_UNIT_VALID(_u)) {                            \
            LOG_ERROR(BSL_LOG_MODULE,                            \
                      (BSL_META_U((_u),                          \
                                  "Invalid unit %d\n"),          \
                       (_u)));                                   \
            SHR_ERR_EXIT(SHR_E_UNIT);                     \
        }                                                        \
    } while (0)

/*
 * Check that table category is valid.  Function exits if table category
 * is invalid.
 * This macro needs to be used in conjunction with the SHR_ debug macros.
 */
#define LTM_MD_TABLE_CATG_CHECK(_u, _c)                             \
    do {                                                            \
        if (!LTM_MD_TABLE_CATG_VALID(_c)) {                         \
            LOG_ERROR(BSL_LOG_MODULE,                               \
                      (BSL_META_U((_u),                             \
                                  "Invalid table category %d\n"),   \
                       (_c)));                                      \
            SHR_ERR_EXIT(SHR_E_PARAM);                       \
        }                                                           \
    } while (0)


/* Table category string */
static char *table_catg_str[] = {
    "PT Pass Thru",
    "Logical"
};


/*******************************************************************************
 * Private functions
 */


/*!
 * \brief Destroy the given LTM metadata data structure on a given unit.
 *
 * Destroy the LTM metadata data structure and cleanup/free
 * any memory previously allocated during the create process.
 *
 * Assumes unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] catg Table category (BCMLTM_TABLE_CATG_...).
 *
 * \retval SHR_E_NONE No errors
 * \retval !SHR_E_NONE Failure
 */
static int
ltm_md_destroy(int unit, bcmltm_table_catg_t catg)
{
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "LTM metadata destroy: %s(%d)\n"),
                 table_catg_str[catg], catg));

    /* Check table category */
    LTM_MD_TABLE_CATG_CHECK(unit, catg);

    switch(catg) {
    case BCMLTM_TABLE_CATG_PTHRU:
        SHR_IF_ERR_EXIT(bcmltm_md_pthru_destroy(unit));
        break;
    case BCMLTM_TABLE_CATG_LOGICAL:
        SHR_IF_ERR_EXIT(bcmltm_md_logical_destroy(unit));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
        break;
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the LTM metadata data structure on a given unit.
 *
 * Create the LTM metadata  for the specified table category on a given unit.
 * Memory is allocated if necessary.
 *
 * Assumes: unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates if this is cold or warm boot.
 * \param [in] catg Table category (BCMLTM_TABLE_CATG_...).
 *
 * \retval SHR_E_NONE No errors
 * \retval !SHR_E_NONE Failure
 */
static int
ltm_md_create(int unit, bool warm, bcmltm_table_catg_t catg)
{
    SHR_FUNC_ENTER(unit);

    /* Check table category */
    LTM_MD_TABLE_CATG_CHECK(unit, catg);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "LTM metadata create: %s(%d)\n"),
                 table_catg_str[catg], catg));

    switch(catg) {
    case BCMLTM_TABLE_CATG_PTHRU:
        SHR_IF_ERR_EXIT(bcmltm_md_pthru_create(unit));
        break;
    case BCMLTM_TABLE_CATG_LOGICAL:
        SHR_IF_ERR_EXIT(bcmltm_md_logical_create(unit, warm));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
        break;
    }

 exit:
    if (SHR_FUNC_ERR()) {
        ltm_md_destroy(unit, catg);
    }

    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_md_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "LTM Metadata Initialization\n")));

    /* Check unit */
    LTM_MD_UNIT_CHECK(unit);

    /* Create LTM PT Pass Thru metadata */
    SHR_IF_ERR_EXIT
        (ltm_md_create(unit, warm, BCMLTM_TABLE_CATG_PTHRU));

    /* Create LTM Logical metadata */
    SHR_IF_ERR_EXIT
        (ltm_md_create(unit, warm, BCMLTM_TABLE_CATG_LOGICAL));

    /* Initialize handle for PT status IMM operation. */
    SHR_IF_ERR_EXIT
        (bcmltm_table_op_pt_info_init(unit));

    /* Initialize handle for PT status IMM operation. */
    SHR_IF_ERR_EXIT
        (bcmltm_table_op_dop_info_init(unit));

    /* Initialize handle for DEVICE_OP_PT_INFO IMM operations. */
    SHR_IF_ERR_EXIT
        (bcmltm_device_op_pt_info_init(unit));

    /* Initialize handle for DEVICE_OP_DSH_INFO IMM operations. */
    SHR_IF_ERR_EXIT
        (bcmltm_device_op_dsh_info_init(unit));

 exit:
    if (SHR_FUNC_ERR()) {
        (void)bcmltm_md_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}


int
bcmltm_md_cleanup(int unit)
{
    int catg;
    int rv;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "LTM Metadata Cleanup\n")));

    /* Check unit */
    LTM_MD_UNIT_CHECK(unit);

    for (catg = 0; catg < BCMLTM_TABLE_CATG_COUNT; catg++) {
        rv = ltm_md_destroy(unit, catg);
        if (rv == SHR_E_UNAVAIL) {
            rv = SHR_E_NONE;
        }
        SHR_IF_ERR_CONT(rv);
    }

    /* Cleanup handle for PT status IMM operation. */
    SHR_IF_ERR_EXIT
        (bcmltm_table_op_pt_info_cleanup(unit));

    /* Cleanup handle for PT status IMM operation. */
    SHR_IF_ERR_EXIT
        (bcmltm_table_op_dop_info_cleanup(unit));

    /* Cleanup handle for DEVICE_OP_PT_INFO IMM operations. */
    SHR_IF_ERR_EXIT
        (bcmltm_device_op_pt_info_cleanup(unit));

    /* Cleanup handle for DEVICE_OP_DSH_INFO IMM operations. */
    SHR_IF_ERR_EXIT
        (bcmltm_device_op_dsh_info_cleanup(unit));

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_lt_retrieve(int unit, bcmltm_table_catg_t catg,
                      uint32_t ltid,
                      bcmltm_lt_md_t **ltm_md_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Check unit */
    LTM_MD_UNIT_CHECK(unit);

    /* Check table category */
    LTM_MD_TABLE_CATG_CHECK(unit, catg);

    switch(catg) {
    case BCMLTM_TABLE_CATG_PTHRU:
        SHR_IF_ERR_EXIT(bcmltm_md_pthru_lt_retrieve(unit, ltid, ltm_md_ptr));
        break;
    case BCMLTM_TABLE_CATG_LOGICAL:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmltm_md_logical_lt_retrieve(unit, ltid, ltm_md_ptr));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
        break;
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_pt_field_count_get(int unit, bcmdrd_sid_t sid, size_t *num_fid)
{
    const bcmltm_field_map_list_t *key_list;
    const bcmltm_field_map_list_t *value_list;
    size_t fid_count = 0;
    size_t map_idx;

    SHR_FUNC_ENTER(unit);

    /* Check unit */
    LTM_MD_UNIT_CHECK(unit);

    /* Check NULL */
    SHR_NULL_CHECK(num_fid, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmltm_md_pthru_key_field_list_retrieve(unit, sid,
                                                 &key_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_pthru_value_field_list_retrieve(unit, sid,
                                                   &value_list));

    /* Keys */
    for (map_idx = 0; map_idx < key_list->num_fields; map_idx++) {
        /* Count distinct field IDs (1st index) */
        if (key_list->field_maps[map_idx].field_idx > 0) {
            continue;
        }
        fid_count++;
    }

    /* Values */
    for (map_idx = 0; map_idx < value_list->num_fields; map_idx++) {
        /* Count distinct field IDs (1st index) */
        if (value_list->field_maps[map_idx].field_idx > 0) {
            continue;
        }
        fid_count++;
    }

    *num_fid = fid_count;

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_pt_field_list_get(int unit, bcmdrd_sid_t sid, size_t list_max,
                            bcmdrd_fid_t *fid_list, size_t *num_fid)
{
    const bcmltm_field_map_list_t *field_list;
    size_t fid_idx = 0;
    size_t map_idx;

    SHR_FUNC_ENTER(unit);

    /* Check unit */
    LTM_MD_UNIT_CHECK(unit);

    if (list_max == 0) {
        SHR_EXIT();
    }

    /* Check NULL */
    SHR_NULL_CHECK(fid_list, SHR_E_PARAM);
    SHR_NULL_CHECK(num_fid, SHR_E_PARAM);

    /* Keys */
    SHR_IF_ERR_EXIT
        (bcmltm_md_pthru_key_field_list_retrieve(unit, sid,
                                                 &field_list));
    for (map_idx = 0;
         (fid_idx < list_max) && (map_idx < field_list->num_fields);
         map_idx++) {
        /* Get only maps with distinct field IDs (1st index) */
        if (field_list->field_maps[map_idx].field_idx > 0) {
            continue;
        }
        fid_list[fid_idx] = field_list->field_maps[map_idx].field_id;
        fid_idx++;
    }

    /* Values */
    SHR_IF_ERR_EXIT
        (bcmltm_md_pthru_value_field_list_retrieve(unit, sid,
                                                   &field_list));
    for (map_idx = 0;
         (fid_idx < list_max) && (map_idx < field_list->num_fields);
         map_idx++) {
        /* Get only maps with distinct field IDs (1st index) */
        if (field_list->field_maps[map_idx].field_idx > 0) {
            continue;
        }
        fid_list[fid_idx] = field_list->field_maps[map_idx].field_id;
        fid_idx++;
    }

    *num_fid = fid_idx;

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_pt_field_info_get(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                            bcmdrd_sym_field_info_t *finfo)
{
    SHR_FUNC_ENTER(unit);

    /* Check unit */
    LTM_MD_UNIT_CHECK(unit);

    /* Check NULL */
    SHR_NULL_CHECK(finfo, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmltm_md_pthru_field_info_get(unit, sid, fid, finfo));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_xfrm_field_get(int unit, const bcmlrd_map_entry_t *entry, bool src,
                         uint32_t *num_fields,
                         const bcmltd_field_list_t **flist)
{
    const bcmltd_xfrm_handler_t *xfrm = NULL;

    SHR_FUNC_ENTER(unit);

    /* Check unit */
    LTM_MD_UNIT_CHECK(unit);

    /* Check NULL */
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Transform Handler */
    xfrm = bcmltm_lta_intf_xfrm_handler_get(unit, entry->u.xfrm.desc->handler_id);
    SHR_NULL_CHECK(xfrm, SHR_E_PARAM);

    /* Forward transform */
    if ((entry->entry_type == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) ||
        (entry->entry_type == BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER)) {
        if (src) {
            *flist = xfrm->arg->field_list;
            *num_fields = xfrm->arg->fields;
        } else {
            *flist = xfrm->arg->rfield_list;
            *num_fields = xfrm->arg->rfields;
        }
    } else { /* Reverse transform */
        if (src) {
            *flist = xfrm->arg->rfield_list;
            *num_fields = xfrm->arg->rfields;
        } else {
            *flist = xfrm->arg->field_list;
            *num_fields = xfrm->arg->fields;
        }
    }

exit:
    SHR_FUNC_EXIT();
}
