/*! \file bcmltm_db_info.c
 *
 * Logical Table Manager - Logical Table Database Information.
 *
 * This file contains routines to create the internal database for
 * logical tables and provide access to the information.
 *
 * The database is created from the LRD table definitions and maps
 * and stored locally to provide access to data needed during
 * the LTM metadata construction, such as FA and EE node cookies.
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

#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>

#include "bcmltm_db_md_util.h"
#include "bcmltm_db_dm.h"
#include "bcmltm_db_xfrm.h"
#include "bcmltm_db_cth.h"
#include "bcmltm_db_ltm.h"
#include "bcmltm_db_fv.h"
#include "bcmltm_db_tc.h"
#include "bcmltm_db_info.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*
 * Accessor macros for Logical Tables Database Information
 */
#define DB_INFO(_u)                     db_info[_u]
#define DB_INFO_SID_MAX_COUNT(_u)       DB_INFO(_u)->sid_max_count

#define DB_LT_INFO(_u, _sid)            (&(DB_INFO(_u)->lt[_sid]))

#define DB_ATTR(_u, _sid)               DB_ATTR_INFO(DB_LT_INFO(_u, _sid))
#define DB_MAP_GROUP_COUNT(_u, _sid)    &DB_LT_INFO(_u, _sid)->map_group_count

#define DB_DM(_u, _sid)                 DB_DM_INFO(DB_LT_INFO(_u, _sid))
#define DB_FV(_u, _sid)                 DB_FV_INFO(DB_LT_INFO(_u, _sid))
#define DB_TC(_u, _sid)                 DB_TC_INFO(DB_LT_INFO(_u, _sid))
#define DB_CTH(_u, _sid)                DB_CTH_INFO(DB_LT_INFO(_u, _sid))
#define DB_LTM(_u, _sid)                DB_LTM_INFO(DB_LT_INFO(_u, _sid))

/*
 * Helper macros
 */
/* Check that info has been initialized */
#define DB_INFO_INIT(_u)                        \
    (DB_INFO(_u) != NULL)

/* Check that logical table ID is valid */
#define DB_LT_SID_VALID(_u, _sid)               \
    ((_sid) < DB_INFO_SID_MAX_COUNT(_u))

/*!
 * \brief Database Information for Logical Tables in a unit.
 */
typedef struct db_info_s {
    /*! Total number of logical table IDs. */
    uint32_t sid_max_count;

    /*!
     * Array of logical table information.
     *
     * This implementation is based upon the logical table IDs
     * being in a compact numbering space from [0 .. (sid_max_count - 1)]
     * The logical table ID is the index into the list array.
     */
    bcmltm_db_lt_info_t lt[];

} db_info_t;

/* Logical Tables Information */
static db_info_t *db_info[BCMLTM_MAX_UNITS];


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Check that unit and given table ID are valid.
 *
 * This routine checks that the database information has been
 * initialized and the logical table ID is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_lt_info_valid(int unit, bcmlrd_sid_t sid)
{
    SHR_FUNC_ENTER(unit);

    if (!DB_INFO_INIT(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!DB_LT_SID_VALID(unit, sid)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy LT database information for given table ID.
 *
 *  Destroy LT database information for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
db_lt_info_destroy(int unit, bcmlrd_sid_t sid)
{
    /* Destroy Direct Map table info */
    bcmltm_db_dm_info_destroy(DB_DM(unit, sid));
    DB_DM(unit, sid) = NULL;

    /* Destroy LTM Managed table info */
    bcmltm_db_ltm_info_destroy(DB_LTM(unit, sid));
    DB_LTM(unit, sid) = NULL;

    /* Destroy Custom Table Handler info */
    bcmltm_db_cth_info_destroy(DB_CTH(unit, sid));
    DB_CTH(unit, sid) = NULL;

    /* Destroy Table Commit info */
    bcmltm_db_tc_info_destroy(DB_TC(unit, sid));
    DB_TC(unit, sid) = NULL;

    /* Destroy Field Validation info */
    bcmltm_db_fv_info_destroy(DB_FV(unit, sid));
    DB_FV(unit, sid) = NULL;

    return;
}

/*!
 * \brief Create LT database information for given table ID.
 *
 * Create LT database information for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_lt_info_create(int unit, bcmlrd_sid_t sid)
{
    const bcmltm_db_map_group_count_t *group_count;
    const bcmltm_table_attr_t *attr;

    SHR_FUNC_ENTER(unit);

    /* Get Table Attributes */
    SHR_IF_ERR_EXIT
        (bcmltm_db_table_attr_parse(unit, sid,
                                    DB_ATTR(unit, sid)));
    attr = DB_ATTR(unit, sid);

    /* Get Map Groups count */
    SHR_IF_ERR_EXIT
        (bcmltm_db_map_group_count_get(unit, sid,
                                       DB_MAP_GROUP_COUNT(unit, sid)));

    group_count = DB_MAP_GROUP_COUNT(unit, sid);

    /* Create LT information for Field Validations */
    if ((group_count->fv > 0)) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_fv_info_create(unit, sid,
                                      &DB_FV(unit, sid)));

    }

    /* Create LT information for Table Commit interfaces */
    if ((group_count->tc > 0)) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_tc_info_create(unit, sid,
                                      &DB_TC(unit, sid)));
    }

    /*
     * Create LT information for only ONE of the following table map types
     * in the following order of priority:
     *   - Custom Table Handler group
     *   - LTM group
     *   - PT groups (direct mapped)
     */
    if ((group_count->cth > 0)) {
        /* Create LT information for CTH table */
        SHR_IF_ERR_EXIT
            (bcmltm_db_cth_info_create(unit, sid,
                                       &DB_CTH(unit, sid)));
    } else if ((group_count->ltm > 0)) {
        /* Create LT information for LTM managed table */
        SHR_IF_ERR_EXIT
            (bcmltm_db_ltm_info_create(unit, sid,
                                       &DB_LTM(unit, sid)));
    } else if ((group_count->dm > 0)) {
        /* Create LT information for Direct Mapped table */
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_info_create(unit, sid, attr,
                                      &DB_DM(unit, sid)));

        if ((group_count->fv > 0)) {
            /* Create LTA Validate Copy Field Information (FA node cookie) */
            SHR_IF_ERR_EXIT
                (bcmltm_db_fv_info_copy_list_create(unit, sid,
                           DB_DM(unit, sid)->xfrm->rev_value_xfrm_list,
                           DB_FV(unit, sid)->validate_values,
                         &(DB_FV(unit, sid)->rvalue_to_value)));
        }
    }

 exit:
    if (SHR_FUNC_ERR()) {
        db_lt_info_destroy(unit, sid);
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_info_create(int unit, uint32_t sid_max_count)
{
    db_info_t *info = NULL;
    unsigned int size;
    bcmlrd_sid_t *sid_list = NULL;
    size_t num_sid;
    size_t idx;
    size_t sid;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Check if it has been initialized */
    if (DB_INFO_INIT(unit)) {
        SHR_EXIT();
    }

    /* Allocate */
    size = sizeof(*info) + (sizeof(bcmltm_db_lt_info_t) * sid_max_count);
    SHR_ALLOC(info, size, "bcmltmDbInfo");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, size);
    DB_INFO(unit) = info;

    /* Initialize information */
    info->sid_max_count = sid_max_count;

    if (sid_max_count == 0) {
        /* No LTs to record */
        SHR_EXIT();
    }

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

    /* Create LT NC info for each table */
    for (idx = 0; idx < num_sid; idx++) {
        sid = sid_list[idx];
        /* Create LT info */
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (db_lt_info_create(unit, sid), SHR_E_UNAVAIL);
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_info_destroy(unit);
    }

    SHR_FREE(sid_list);
    SHR_FUNC_EXIT();
}

void
bcmltm_db_info_destroy(int unit)
{
    bcmlrd_sid_t sid;
    bcmlrd_sid_t sid_max;

    if (!DB_INFO_INIT(unit)) {
        return;
    }

    /*
     * This implementation is based upon the symbols IDs
     * being in a compact numbering space from [0 .. (sid_max_count -1)]
     */
    sid_max = DB_INFO_SID_MAX_COUNT(unit) - 1;

    /* Destroy internal information */
    for (sid = 0; sid <= sid_max; sid++) {
        /* LRD symbol ID is the same */
        db_lt_info_destroy(unit, sid);
    }

    SHR_FREE(DB_INFO(unit));
    DB_INFO(unit) = NULL;

    return;
}

int
bcmltm_db_info_sid_max_count_get(int unit, uint32_t *sid_max_count)
{
    SHR_FUNC_ENTER(unit);

    /* Check if database has been initialized */
    if (!DB_INFO_INIT(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    *sid_max_count = DB_INFO_SID_MAX_COUNT(unit);

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_lt_info_get(int unit, bcmlrd_sid_t sid,
                      const bcmltm_db_lt_info_t **info_ptr)
{
    SHR_FUNC_ENTER(unit);

    *info_ptr = NULL;

    SHR_IF_ERR_EXIT(db_lt_info_valid(unit, sid));

    *info_ptr = DB_LT_INFO(unit, sid);

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_info_get(int unit,
                      bcmlrd_sid_t sid,
                      const bcmltm_db_dm_info_t **info_ptr)
{
    const bcmltm_db_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);

    *info_ptr = NULL;

    SHR_IF_ERR_EXIT(bcmltm_db_lt_info_get(unit, sid, &lt_info));
    if (lt_info == NULL) {
        SHR_EXIT();
    }

    *info_ptr = lt_info->dm;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_track_info_get(int unit,
                            bcmlrd_sid_t sid,
                            const bcmltm_db_dm_track_info_t **info_ptr)
{
    const bcmltm_db_dm_info_t *dm_info = NULL;

    SHR_FUNC_ENTER(unit);

    *info_ptr = NULL;

    SHR_IF_ERR_EXIT(bcmltm_db_dm_info_get(unit, sid, &dm_info));
    if (dm_info == NULL) {
        SHR_EXIT();
    }

    *info_ptr = dm_info->track_info;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_dm_fs_info_get(int unit,
                         bcmlrd_sid_t sid,
                         const bcmltm_db_dm_fs_info_t **info_ptr)
{
    const bcmltm_db_dm_info_t *dm_info = NULL;

    SHR_FUNC_ENTER(unit);

    *info_ptr = NULL;

    SHR_IF_ERR_EXIT(bcmltm_db_dm_info_get(unit, sid, &dm_info));
    if (dm_info == NULL) {
        SHR_EXIT();
    }

    *info_ptr = dm_info->fs;

 exit:
    SHR_FUNC_EXIT();
}
