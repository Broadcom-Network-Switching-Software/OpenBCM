/*! \file index_table_mgmt.c
 *
 * Index-based table management.
 * This file contains the management logic for index-based tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>

#include <bcm_int/ltsw/index_table_mgmt.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

typedef struct bcmi_ltsw_idx_tbl_info_s {
    /* Corresponding LT info. */
    bcmi_ltsw_idx_tbl_lt_info_t lt_info;

    /* User information. */
    bcmi_ltsw_idx_tbl_user_info_t *user_info;

    /* Total number of entries. */
    uint32_t size;

    /* First table index. */
    uint32_t idx_min;

    /* Last table index. */
    uint32_t idx_max;

    /* Used entries bitmap. */
    SHR_BITDCL *used_bmp;

    /* Entry information. */
    bcmi_ltsw_idx_tbl_entry_info_t *entry_info;
} bcmi_ltsw_idx_tbl_info_t;

typedef struct ltsw_idx_tbl_mgmt_info_s {
    /* Init status. */
    int initialized;

    bcmi_ltsw_idx_tbl_info_t *tbl_info[BCMI_LTSW_IDX_TBL_ID_CNT];

    /* Lock. */
    sal_mutex_t mutex;
} ltsw_idx_tbl_mgmt_info_t;

static ltsw_idx_tbl_mgmt_info_t ltsw_idx_tbl_mgmt_info[BCM_MAX_NUM_UNITS];
#define LTSW_IDX_TBL_MGMT_INFO(_u_) (&ltsw_idx_tbl_mgmt_info[_u_])
#define LTSW_IDX_TBL_MGMT_INIT_CHECK(_u_) \
    do { \
        if (LTSW_IDX_TBL_MGMT_INFO(_u_)->initialized == 0) { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

#define LTSW_IDX_TBL_MGMT_LOCK(_u_) \
            sal_mutex_take(LTSW_IDX_TBL_MGMT_INFO(_u_)->mutex, \
                           SAL_MUTEX_FOREVER)
#define LTSW_IDX_TBL_MGMT_UNLOCK(_u_) \
            sal_mutex_give(LTSW_IDX_TBL_MGMT_INFO(_u_)->mutex)

#define LTSW_IDX_TBL_INFO(_u_, _tbl_id_) \
            (LTSW_IDX_TBL_MGMT_INFO(_u_)->tbl_info[_tbl_id_])

#define LTSW_IDX_TBL_SIZE(_u_, _tbl_id_) \
            LTSW_IDX_TBL_INFO(_u_, _tbl_id_)->size
#define LTSW_IDX_TBL_IDX_MIN(_u_, _tbl_id_) \
            LTSW_IDX_TBL_INFO(_u_, _tbl_id_)->idx_min
#define LTSW_IDX_TBL_IDX_MAX(_u_, _tbl_id_) \
            LTSW_IDX_TBL_INFO(_u_, _tbl_id_)->idx_max
#define LTSW_IDX_TBL_USED_BMP(_u_, _tbl_id_)  \
            LTSW_IDX_TBL_INFO(_u_, _tbl_id_)->used_bmp

#define LTSW_IDX_TBL_USER_CNT(_u_, _tbl_id_) \
            LTSW_IDX_TBL_INFO(_u_, _tbl_id_)->lt_info.user_cnt
#define LTSW_IDX_TBL_LT_NAME(_u_, _tbl_id_) \
            LTSW_IDX_TBL_INFO(_u_, _tbl_id_)->lt_info.tbl_name
#define LTSW_IDX_TBL_LT_IDX_FIELD(_u_, _tbl_id_) \
            LTSW_IDX_TBL_INFO(_u_, _tbl_id_)->lt_info.idx_field

#define LTSW_IDX_TBL_ENTRY_USED_GET(_u_, _tbl_id_, _entry_id_) \
            SHR_BITGET(LTSW_IDX_TBL_USED_BMP(_u_, _tbl_id_), _entry_id_)
#define LTSW_IDX_TBL_ENTRY_USED_SET(_u_, _tbl_id_, _entry_id_) \
            SHR_BITSET(LTSW_IDX_TBL_USED_BMP(_u_, _tbl_id_), _entry_id_)
#define LTSW_IDX_TBL_ENTRY_USED_CLR(_u_, _tbl_id_, _entry_id_) \
            SHR_BITCLR(LTSW_IDX_TBL_USED_BMP(_u_, _tbl_id_), _entry_id_)

#define LTSW_IDX_TBL_USER_INFO(_u_, _tbl_id_, _user_id_) \
            ((LTSW_IDX_TBL_INFO(_u_, _tbl_id_)->user_info) + _user_id_)
#define LTSW_IDX_TBL_USER_FUNC_CMP(_u_, _tbl_id_, _user_id_) \
            LTSW_IDX_TBL_USER_INFO(_u_, _tbl_id_, _user_id_)->func_cmp
#define LTSW_IDX_TBL_USER_FUNC_HASH(_u_, _tbl_id_, _user_id_) \
            LTSW_IDX_TBL_USER_INFO(_u_, _tbl_id_, _user_id_)->func_hash
#define LTSW_IDX_TBL_USER_FUNC_INSERT(_u_, _tbl_id_, _user_id_) \
            LTSW_IDX_TBL_USER_INFO(_u_, _tbl_id_, _user_id_)->func_insert
#define LTSW_IDX_TBL_USER_FUNC_DEL(_u_, _tbl_id_, _user_id_) \
            LTSW_IDX_TBL_USER_INFO(_u_, _tbl_id_, _user_id_)->func_del
#define LTSW_IDX_TBL_USER_IDX_MIN(_u_, _tbl_id_, _user_id_) \
            LTSW_IDX_TBL_USER_INFO(_u_, _tbl_id_, _user_id_)->idx_min
#define LTSW_IDX_TBL_USER_IDX_MAX(_u_, _tbl_id_, _user_id_) \
            LTSW_IDX_TBL_USER_INFO(_u_, _tbl_id_, _user_id_)->idx_max
#define LTSW_IDX_TBL_USER_IDX_MAXUSED(_u_, _tbl_id_, _user_id_) \
            LTSW_IDX_TBL_USER_INFO(_u_, _tbl_id_, _user_id_)->idx_maxused
#define LTSW_IDX_TBL_USER_USED_GET(_u_, _tbl_id_, _user_id_)  \
            LTSW_IDX_TBL_USER_INFO(_u_, _tbl_id_, _user_id_)->valid
#define LTSW_IDX_TBL_USER_USED_SET(_u_, _tbl_id_, _user_id_)  \
            LTSW_IDX_TBL_USER_INFO(_u_, _tbl_id_, _user_id_)->valid = 1
#define LTSW_IDX_TBL_USER_USED_CLR(_u_, _tbl_id_, _user_id_)  \
            LTSW_IDX_TBL_USER_INFO(_u_, _tbl_id_, _user_id_)->valid = 0

#define LTSW_IDX_TBL_ENTRY_INFO(_u_, _tbl_id_)  \
            LTSW_IDX_TBL_INFO(_u_, _tbl_id_)->entry_info
#define LTSW_IDX_ENTRY_INFO(_u_, _tbl_id_, _entry_id_)  \
            (LTSW_IDX_TBL_ENTRY_INFO(_u_, _tbl_id_) + _entry_id_)
#define LTSW_IDX_ENTRY_REFCNT(_u_, _tbl_id_, _entry_id_)  \
            (LTSW_IDX_ENTRY_INFO(_u_, _tbl_id_, _entry_id_)->ref_cnt)
#define LTSW_IDX_ENTRY_HASH(_u_, _tbl_id_, _entry_id_)  \
            (LTSW_IDX_ENTRY_INFO(_u_, _tbl_id_, _entry_id_)->hash)
#define LTSW_IDX_ENTRY_USER_ID(_u_, _tbl_id_, _entry_id_)  \
            (LTSW_IDX_ENTRY_INFO(_u_, _tbl_id_, _entry_id_)->user_id)

#define LTSW_IDX_TBL_ENTRY_IDX_CHECK(_u_, _tbl_id_, _entry_idx_) \
            do { \
                if ((_entry_idx_ < LTSW_IDX_TBL_IDX_MIN(_u_, _tbl_id_)) \
                    || \
                    (_entry_idx_ > LTSW_IDX_TBL_IDX_MAX(_u_, _tbl_id_))) \
                { \
                    LOG_VERBOSE(BSL_LOG_MODULE, \
                                (BSL_META_U(_u_, "Invalid entry index %d.\n"), \
                                 _entry_idx_)); \
                    SHR_ERR_EXIT(SHR_E_PARAM); \
                } \
            } while(0)

#define LTSW_IDX_TBL_USER_ENTRY_IDX_CHECK(_u_, _tbl_id_, _user_id, _entry_idx_) \
            do { \
                if ((_entry_idx_ < \
                        LTSW_IDX_TBL_USER_IDX_MIN(_u_, _tbl_id_, _user_id)) \
                    || \
                    (_entry_idx_ > \
                         LTSW_IDX_TBL_USER_IDX_MAX(_u_, _tbl_id_, _user_id))) \
                { \
                    LOG_VERBOSE(BSL_LOG_MODULE, \
                                (BSL_META_U(_u_, "Invalid entry index %d.\n"), \
                                 _entry_idx_)); \
                    SHR_ERR_EXIT(SHR_E_PARAM); \
                } \
            } while(0)

#define LTSW_IDX_TBL_ID_CHECK(_u_, _tbl_id_) \
    do { \
        if (_tbl_id_ >= BCMI_LTSW_IDX_TBL_ID_CNT) { \
            SHR_ERR_EXIT(SHR_E_PARAM); \
        } \
    } while (0)

#define LTSW_IDX_TBL_USER_ID_CHECK(_u_, _tbl_id_, _user_id_) \
    do { \
        LTSW_IDX_TBL_ID_CHECK(_u_, _tbl_id_); \
        if (LTSW_IDX_TBL_INFO(unit, tbl_id) == NULL) { \
            SHR_ERR_EXIT(SHR_E_PARAM); \
        } \
        if (_user_id_ >= LTSW_IDX_TBL_USER_CNT(_u_, _tbl_id_)) { \
            SHR_ERR_EXIT(SHR_E_PARAM); \
        } \
    } while (0)

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Make sure corresponding user is registered.
 *
 * \param [in] unit           Unit number.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User Id.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
ltsw_idx_tbl_info_validate(int unit, bcmi_ltsw_idx_tbl_id_t tbl_id,
                           int user_id)
{
    SHR_FUNC_ENTER(unit);

    LTSW_IDX_TBL_USER_ID_CHECK(unit, tbl_id, user_id);

    SHR_NULL_CHECK(LTSW_IDX_TBL_USER_INFO(unit, tbl_id, user_id),
                           SHR_E_INTERNAL);

    if (!(LTSW_IDX_TBL_USER_USED_GET(unit, tbl_id, user_id))) {
        /* User has not registered. */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(LTSW_IDX_TBL_USED_BMP(unit, tbl_id), SHR_E_INTERNAL);
    SHR_NULL_CHECK(LTSW_IDX_TBL_ENTRY_INFO(unit, tbl_id), SHR_E_INTERNAL);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Unregister a user from given table.
 *
 * \param [in] unit           Unit number.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User Id, -1 means all users.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
ltsw_idx_tbl_mgmt_unregister(int unit, bcmi_ltsw_idx_tbl_id_t tbl_id,
                             int user_id)
{
    uint8_t user_idx;
    int user_remain = 0;
    int idx, idx_min, idx_maxused;

    SHR_FUNC_ENTER(unit);

    LTSW_IDX_TBL_ID_CHECK(unit, tbl_id);

    /* This table has not been initialized yet.*/
    if (LTSW_IDX_TBL_INFO(unit, tbl_id) == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (user_id >= LTSW_IDX_TBL_USER_CNT(unit, tbl_id) || (user_id < -1)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(LTSW_IDX_TBL_INFO(unit, tbl_id)->user_info,
                           SHR_E_INTERNAL);

    if ((user_id >= 0) &&
        !(LTSW_IDX_TBL_USER_USED_GET(unit, tbl_id, user_id))) {
        /* User has not been registered. */
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (user_id >= 0) {
        LTSW_IDX_TBL_USER_USED_CLR(unit, tbl_id, user_id);

        for (user_idx = 0; user_idx < LTSW_IDX_TBL_USER_CNT(unit, tbl_id);
             user_idx++) {
            if (LTSW_IDX_TBL_USER_USED_GET(unit, tbl_id, user_idx)) {
                user_remain = 1;
                break;
            }
        }
    }
    if (user_remain == 0) {
        /* Release this table's resources. */
        SHR_FREE(LTSW_IDX_TBL_ENTRY_INFO(unit, tbl_id));
        SHR_FREE(LTSW_IDX_TBL_USED_BMP(unit, tbl_id));
        SHR_FREE(LTSW_IDX_TBL_INFO(unit, tbl_id)->user_info);

        SHR_FREE(LTSW_IDX_TBL_INFO(unit, tbl_id));
        LTSW_IDX_TBL_INFO(unit, tbl_id) = NULL;
    } else {
        /* Only reset the specific user's data. */

        idx_min = LTSW_IDX_TBL_USER_IDX_MIN(unit, tbl_id, user_id);
        idx_maxused = LTSW_IDX_TBL_USER_IDX_MAXUSED(unit, tbl_id, user_id);
        for (idx = idx_min; idx <= idx_maxused; idx++) {
            if (LTSW_IDX_TBL_ENTRY_USED_GET(unit, tbl_id, idx) &&
                (LTSW_IDX_ENTRY_USER_ID(unit, tbl_id, idx) == user_id)) {
                LTSW_IDX_TBL_ENTRY_USED_CLR(unit, tbl_id, idx);
                LTSW_IDX_ENTRY_REFCNT(unit, tbl_id, idx) = 0;
            }
        }

        LTSW_IDX_TBL_USER_USED_CLR(unit, tbl_id, user_id);
        LTSW_IDX_TBL_USER_FUNC_CMP(unit, tbl_id, user_id) = NULL;
        LTSW_IDX_TBL_USER_FUNC_HASH(unit, tbl_id, user_id) = NULL;
        LTSW_IDX_TBL_USER_FUNC_INSERT(unit, tbl_id, user_id) = NULL;
        LTSW_IDX_TBL_USER_FUNC_DEL(unit, tbl_id, user_id) = NULL;
        LTSW_IDX_TBL_USER_IDX_MIN(unit, tbl_id, user_id) = 0;
        LTSW_IDX_TBL_USER_IDX_MAX(unit, tbl_id, user_id) = 0;
        LTSW_IDX_TBL_USER_IDX_MAXUSED(unit, tbl_id, user_id) = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free all allocated resources.
 *
 * \param [in] unit           Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
ltsw_idx_tbl_mgmt_resource_free(int unit)
{
    bcmi_ltsw_idx_tbl_id_t tbl_id;
    int user_id, used_cnt;

    SHR_FUNC_ENTER(unit);

    for (tbl_id = 0; tbl_id < BCMI_LTSW_IDX_TBL_ID_CNT; tbl_id++) {
        if (LTSW_IDX_TBL_INFO(unit, tbl_id) == NULL) {
        /* This table has not been initialized yet.*/
            continue;
        }

        used_cnt = 0;
        for (user_id = 0; user_id < LTSW_IDX_TBL_USER_CNT(unit, tbl_id);
             user_id++) {
            if (!LTSW_IDX_TBL_USER_USED_GET(unit, tbl_id, user_id)) {
                continue;
            }
            used_cnt++;
        }

        for (user_id = 0; user_id < LTSW_IDX_TBL_USER_CNT(unit, tbl_id);
             user_id++) {
            if (!LTSW_IDX_TBL_USER_USED_GET(unit, tbl_id, user_id)) {
                continue;
            }

            SHR_IF_ERR_CONT
                (ltsw_idx_tbl_mgmt_unregister(unit, tbl_id, user_id));
            used_cnt--;
            if (used_cnt == 0) {
                break;
            }
        }
    }

    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Index-based table management module initialization.
 *
 * \param [in] unit           Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int bcmi_ltsw_idx_tbl_mgmt_init(int unit)
{
    int rv;
    SHR_FUNC_ENTER(unit);

    /* Free resoureces previously allocated. */
    rv = ltsw_idx_tbl_mgmt_resource_free(unit);
    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_ERR_EXIT(rv);
    }

    /* Alloc lock. */
    if (LTSW_IDX_TBL_MGMT_INFO(unit)->mutex == NULL) {
        LTSW_IDX_TBL_MGMT_INFO(unit)->mutex =
            sal_mutex_create("Index Table Management Mutex");
        SHR_NULL_CHECK(LTSW_IDX_TBL_MGMT_INFO(unit)->mutex, SHR_E_MEMORY);
    }

    /* Set init flag. */
    LTSW_IDX_TBL_MGMT_INFO(unit)->initialized = 1;

exit:
    if (SHR_FUNC_ERR()) {
        /* Free resoureces. */
        ltsw_idx_tbl_mgmt_resource_free(unit);

        if (LTSW_IDX_TBL_MGMT_INFO(unit)->mutex != NULL) {
            sal_mutex_destroy(LTSW_IDX_TBL_MGMT_INFO(unit)->mutex);
            LTSW_IDX_TBL_MGMT_INFO(unit)->mutex = NULL;
        }

        LTSW_IDX_TBL_MGMT_INFO(unit)->initialized = 0;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Table management module deinit.
 *
 * \param [in] unit           Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int bcmi_ltsw_idx_tbl_mgmt_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (LTSW_IDX_TBL_MGMT_INFO(unit)->initialized == 0) {
        SHR_EXIT();
    }

    LTSW_IDX_TBL_MGMT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_idx_tbl_mgmt_resource_free(unit));

    LTSW_IDX_TBL_MGMT_INFO(unit)->initialized = 0;

    LTSW_IDX_TBL_MGMT_UNLOCK(unit);

    if (LTSW_IDX_TBL_MGMT_INFO(unit)->mutex != NULL) {
        sal_mutex_destroy(LTSW_IDX_TBL_MGMT_INFO(unit)->mutex);
        LTSW_IDX_TBL_MGMT_INFO(unit)->mutex = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register a table user to manage the specific table.
 *
 * This function is used to register a table user to manage the specific table.
 *
 * \param [in] unit           Unit number.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User Id.
 * \param [in] lt_info        LT information.
 * \param [in] user_info      User information.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int bcmi_ltsw_idx_tbl_register(int unit, bcmi_ltsw_idx_tbl_id_t tbl_id,
                               int user_id,
                               bcmi_ltsw_idx_tbl_lt_info_t *lt_info,
                               bcmi_ltsw_idx_tbl_user_info_t *user_info)
{
    bcmi_ltsw_idx_tbl_info_t *new_info = NULL;
    int alloc_sz;
    int lock = 0;
    int user_cnt;
    uint64_t idx_max, idx_min;

    SHR_FUNC_ENTER(unit);

    LTSW_IDX_TBL_MGMT_INIT_CHECK(unit);

    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_info->tbl_name, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_info->idx_field, SHR_E_PARAM);
    SHR_NULL_CHECK(user_info, SHR_E_PARAM);

    LTSW_IDX_TBL_ID_CHECK(unit, tbl_id);

    if (lt_info->user_cnt == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (user_info->idx_min > user_info->idx_max) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }


    LTSW_IDX_TBL_MGMT_LOCK(unit);
    lock = 1;

    if (LTSW_IDX_TBL_INFO(unit, tbl_id) != NULL) {
        if (LTSW_IDX_TBL_USER_CNT(unit, tbl_id) != lt_info->user_cnt) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (user_id >= LTSW_IDX_TBL_USER_CNT(unit, tbl_id)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (LTSW_IDX_TBL_USER_USED_GET(unit, tbl_id, user_id)) {
           SHR_ERR_EXIT(SHR_E_EXISTS);
        }

        if (sal_strcmp(LTSW_IDX_TBL_LT_NAME(unit, tbl_id), lt_info->tbl_name) ||
            sal_strcmp(LTSW_IDX_TBL_LT_IDX_FIELD(unit, tbl_id), lt_info->idx_field)) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    } else {
        user_cnt = lt_info->user_cnt;

        SHR_ALLOC(new_info, sizeof(bcmi_ltsw_idx_tbl_info_t),
                  "ltswIdxTblMgmtTblInfo");
        SHR_NULL_CHECK(new_info, SHR_E_MEMORY);
        sal_memset(new_info, 0, sizeof(bcmi_ltsw_idx_tbl_info_t));

        alloc_sz = sizeof(bcmi_ltsw_idx_tbl_user_info_t) * user_cnt;
        SHR_ALLOC(new_info->user_info, alloc_sz, "ltswIdxTblMgmtUserInfo");
        SHR_NULL_CHECK(new_info->user_info, SHR_E_MEMORY);
        sal_memset(new_info->user_info, 0, alloc_sz);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, lt_info->tbl_name,
                                           lt_info->idx_field,
                                           &idx_min, &idx_max));

        alloc_sz = SHR_BITALLOCSIZE((idx_max + 1));
        SHR_ALLOC(new_info->used_bmp, alloc_sz, "ltswIdxTblMgmtUsedBmp");
        SHR_NULL_CHECK(new_info->used_bmp, SHR_E_MEMORY);
        sal_memset(new_info->used_bmp, 0, alloc_sz);

        alloc_sz = ((int)idx_max + 1) * sizeof(bcmi_ltsw_idx_tbl_entry_info_t);
        SHR_ALLOC(new_info->entry_info, alloc_sz, "ltswIdxTblMgmtEntryInfo");
        SHR_NULL_CHECK(new_info->entry_info, SHR_E_MEMORY);
        sal_memset(new_info->entry_info, 0, alloc_sz);

        new_info->size        = (uint32_t)idx_max + 1;
        new_info->idx_min     = (uint32_t)idx_min;
        new_info->idx_max     = (uint32_t)idx_max;
        new_info->lt_info.user_cnt = lt_info->user_cnt;
        new_info->lt_info.tbl_name = lt_info->tbl_name;
        new_info->lt_info.idx_field = lt_info->idx_field;

        LTSW_IDX_TBL_INFO(unit, tbl_id) = new_info;
    }

    LTSW_IDX_TBL_ENTRY_IDX_CHECK(unit, tbl_id, user_info->idx_min);
    LTSW_IDX_TBL_ENTRY_IDX_CHECK(unit, tbl_id, user_info->idx_max);

    LTSW_IDX_TBL_USER_FUNC_HASH(unit, tbl_id, user_id) = user_info->func_hash;
    LTSW_IDX_TBL_USER_FUNC_CMP(unit, tbl_id, user_id) = user_info->func_cmp;
    LTSW_IDX_TBL_USER_FUNC_INSERT(unit, tbl_id, user_id) = user_info->func_insert;
    LTSW_IDX_TBL_USER_FUNC_DEL(unit, tbl_id, user_id) = user_info->func_del;
    LTSW_IDX_TBL_USER_IDX_MIN(unit, tbl_id, user_id) = user_info->idx_min;
    LTSW_IDX_TBL_USER_IDX_MAX(unit, tbl_id, user_id) = user_info->idx_max;
    LTSW_IDX_TBL_USER_IDX_MAXUSED(unit, tbl_id, user_id) = user_info->idx_min;
    LTSW_IDX_TBL_USER_USED_SET(unit, tbl_id, user_id);

exit:
    if (SHR_FUNC_ERR() && (new_info != NULL)) {
        SHR_FREE(new_info->entry_info);
        SHR_FREE(new_info->used_bmp);
        SHR_FREE(new_info->user_info);
        SHR_FREE(new_info);
        LTSW_IDX_TBL_INFO(unit, tbl_id) = NULL;
    }

    if (lock == 1) {
        LTSW_IDX_TBL_MGMT_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Unregister a user from the specific index table.
 *
 * This function is used to unregister a user from the specific table.
 *
 * \param [in] unit           Unit number.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User ID, -1 means all users.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int bcmi_ltsw_idx_tbl_unregister(int unit, bcmi_ltsw_idx_tbl_id_t tbl_id,
                                 int user_id)
{
    SHR_FUNC_ENTER(unit);

    LTSW_IDX_TBL_MGMT_INIT_CHECK(unit);

    LTSW_IDX_TBL_MGMT_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_idx_tbl_mgmt_unregister(unit, tbl_id, user_id));

    LTSW_IDX_TBL_MGMT_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate an entry from specific table.
 *
 * This function is used to request for a slot from specific table.
 * Note: if any LT operation is potentially involved (i.e. LT read/write) within
 * this function, it is assumed that the LT is properly protected by associated
 * lock before invoking this function.
 *
 * \param [in] unit           Unit number.
 * \param [in] flags          Operation flags.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User Id.
 * \param [in] entry          Content of entry.
 * \param [in/out] index      Entry index.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_idx_tbl_entry_alloc(int unit, uint32_t flags,
                              bcmi_ltsw_idx_tbl_id_t tbl_id, int user_id,
                              void *entry, int *index)
{
    int idx, idx_min, idx_max, idx_found = -1;
    int cmp_result;
    uint16_t hash = 0;
    int lock = 0;

    SHR_FUNC_ENTER(unit);

    LTSW_IDX_TBL_MGMT_INIT_CHECK(unit);

    LTSW_IDX_TBL_USER_ID_CHECK(unit, tbl_id, user_id);
    SHR_NULL_CHECK(index, SHR_E_PARAM);
    if (entry == NULL) {
        if (!(flags & BCMI_LTSW_IDX_TBL_OP_WRITE_DISABLE) ||
            !(flags & BCMI_LTSW_IDX_TBL_OP_MATCH_DISABLE)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    LTSW_IDX_TBL_MGMT_LOCK(unit);
    lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_idx_tbl_info_validate(unit, tbl_id, user_id));

    if ((!(flags & BCMI_LTSW_IDX_TBL_OP_WRITE_DISABLE))
        && (LTSW_IDX_TBL_USER_FUNC_INSERT(unit, tbl_id, user_id) == NULL)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    idx_min = LTSW_IDX_TBL_USER_IDX_MIN(unit, tbl_id, user_id);
    idx_max = LTSW_IDX_TBL_USER_IDX_MAX(unit, tbl_id, user_id);
    if ((flags & BCMI_LTSW_IDX_TBL_OP_SKIP_INDEX_ZERO)
        && (idx_min == 0)) {
        idx_min = 1;
    }

    if (LTSW_IDX_TBL_USER_FUNC_HASH(unit, tbl_id, user_id) && (entry != NULL)) {
        LTSW_IDX_TBL_USER_FUNC_HASH(unit, tbl_id, user_id)(unit, entry, &hash);
    }

    if (flags & BCMI_LTSW_IDX_TBL_OP_WITH_ID) {
        LTSW_IDX_TBL_USER_ENTRY_IDX_CHECK(unit, tbl_id, user_id, *index);
        if (LTSW_IDX_TBL_ENTRY_USED_GET(unit, tbl_id, *index)) {
            if (!(flags & BCMI_LTSW_IDX_TBL_OP_REPLACE)) {
                SHR_ERR_EXIT(SHR_E_EXISTS);
            }
            if (LTSW_IDX_ENTRY_USER_ID(unit, tbl_id, *index) != user_id) {
                /* User should own this entry. */
                SHR_ERR_EXIT(SHR_E_EXISTS);
            }
        }

        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Allocate entry[%d] .\n"), *index));
    } else {
        for (idx = idx_min; idx <= idx_max; idx++) {
            if (!(LTSW_IDX_TBL_ENTRY_USED_GET(unit, tbl_id, idx))) {
                /* Empty. */
                if (idx_found == -1) {
                    idx_found = idx;
                    if (flags & BCMI_LTSW_IDX_TBL_OP_MATCH_DISABLE) {
                        /* Found a free slot. */
                        break;
                    }
                }
            } else {
                /* Entry used. */

                if (flags & BCMI_LTSW_IDX_TBL_OP_MATCH_DISABLE) {
                    continue;
                }

                if (LTSW_IDX_ENTRY_USER_ID(unit, tbl_id, idx) != user_id) {
                    /* Not the same user. */
                    continue;
                }

                if (LTSW_IDX_ENTRY_HASH(unit, tbl_id, idx) != hash) {
                    continue;
                }

                if (LTSW_IDX_TBL_USER_FUNC_CMP(unit, tbl_id, user_id) == NULL) {
                    SHR_ERR_EXIT(SHR_E_NO_HANDLER);
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (LTSW_IDX_TBL_USER_FUNC_CMP(unit, tbl_id, user_id)(unit, idx,
                                                                       entry,
                                                                       &cmp_result));
                if (cmp_result != 0) {
                    /* Not equal. */
                    continue;
                }

                LOG_INFO(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Identical entry[%d] found.\n"),
                        idx));

                *index = idx;
                LTSW_IDX_ENTRY_REFCNT(unit, tbl_id, idx) += 1;
                SHR_EXIT();
            }
        }

        if (idx_found == -1) {
            SHR_ERR_EXIT(SHR_E_FULL);
        }

        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Allocated entry[%d] .\n"),
                idx_found));

        *index = idx_found;
    }

    if (!(flags & BCMI_LTSW_IDX_TBL_OP_WRITE_DISABLE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (LTSW_IDX_TBL_USER_FUNC_INSERT(unit, tbl_id, user_id)(unit,
                                                                  flags,
                                                                  *index,
                                                                  entry));
    }

    if (LTSW_IDX_TBL_USER_IDX_MAXUSED(unit, tbl_id, user_id) < *index) {
        LTSW_IDX_TBL_USER_IDX_MAXUSED(unit, tbl_id, user_id) = *index;
    }

    LTSW_IDX_ENTRY_HASH(unit, tbl_id, *index) = hash;
    LTSW_IDX_ENTRY_USER_ID(unit, tbl_id, *index) = user_id;
    if (!((flags & BCMI_LTSW_IDX_TBL_OP_WITH_ID) &&
        (LTSW_IDX_TBL_ENTRY_USED_GET(unit, tbl_id, *index)))) {
        /* If not replacing case. */
        LTSW_IDX_ENTRY_REFCNT(unit, tbl_id, *index) += 1;
    }
    LTSW_IDX_TBL_ENTRY_USED_SET(unit, tbl_id, *index);

exit:
    if (lock) {
        LTSW_IDX_TBL_MGMT_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Free an entry from specific table.
 *
 * This function is used to free an entry from specific table.
 *
 * \param [in] unit           Unit number.
 * \param [in] flags          Operation flags.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User Id.
 * \param [in] index          Entry index.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_idx_tbl_entry_free(int unit, uint32_t flags,
                             bcmi_ltsw_idx_tbl_id_t tbl_id, int user_id,
                             int index)
{
    uint32_t idx_tmp, idx_min;
    int lock = 0;

    SHR_FUNC_ENTER(unit);

    LTSW_IDX_TBL_MGMT_INIT_CHECK(unit);

    LTSW_IDX_TBL_MGMT_LOCK(unit);
    lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_idx_tbl_info_validate(unit, tbl_id, user_id));

    LTSW_IDX_TBL_USER_ENTRY_IDX_CHECK(unit, tbl_id, user_id, index);

    idx_min = LTSW_IDX_TBL_USER_IDX_MIN(unit, tbl_id, user_id);

    if (!(LTSW_IDX_TBL_ENTRY_USED_GET(unit, tbl_id, index))) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Fail to free empty Entry[%d].\n"),
                     index));
        SHR_IF_ERR_VERBOSE_EXIT
            (SHR_E_NOT_FOUND);
    }

    if (!LTSW_IDX_ENTRY_REFCNT(unit, tbl_id, index)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Entry[%d] Refcnt is 0.\n"),
                     index));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (LTSW_IDX_ENTRY_REFCNT(unit, tbl_id, index) == 1) {
        /* Entry is about to be removed. */
        if (!(flags & BCMI_LTSW_IDX_TBL_OP_WRITE_DISABLE)) {
            SHR_NULL_CHECK
                (LTSW_IDX_TBL_USER_FUNC_DEL(unit, tbl_id, user_id),
                 SHR_E_NO_HANDLER);
        }
    }

    LTSW_IDX_ENTRY_REFCNT(unit, tbl_id, index) -= 1;

    if (LTSW_IDX_ENTRY_REFCNT(unit, tbl_id, index) > 0) {
        /* Entry is still valid. */
        SHR_EXIT();
    }

    /* Note that entry fields are valid only when entry is in used. */
    LTSW_IDX_TBL_ENTRY_USED_CLR(unit, tbl_id, index);
    LTSW_IDX_ENTRY_HASH(unit, tbl_id, index) = 0;
    LTSW_IDX_ENTRY_USER_ID(unit, tbl_id, index) = 0;

    if (index == LTSW_IDX_TBL_USER_IDX_MAXUSED(unit, tbl_id, user_id)) {
        for (idx_tmp = index; idx_tmp > idx_min; idx_tmp--) {
            if (LTSW_IDX_TBL_ENTRY_USED_GET(unit, tbl_id, idx_tmp) &&
                (LTSW_IDX_ENTRY_USER_ID(unit, tbl_id, idx_tmp) == user_id)) {
                break;
            }
        }
        LTSW_IDX_TBL_USER_IDX_MAXUSED(unit, tbl_id, user_id) = idx_tmp;
    }

    if (!(flags & BCMI_LTSW_IDX_TBL_OP_WRITE_DISABLE)) {
        SHR_IF_ERR_EXIT
            (LTSW_IDX_TBL_USER_FUNC_DEL(unit, tbl_id, user_id)(unit, index));
    }

exit:
    if (lock == 1) {
        LTSW_IDX_TBL_MGMT_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_idx_tbl_entry_find(int unit, uint32_t flags,
                             bcmi_ltsw_idx_tbl_id_t tbl_id, int user_id,
                             void *entry, int *index)
{
    int idx, idx_min, idx_maxused;
    int cmp_result;
    uint16_t hash = 0;
    int lock = 0;

    SHR_FUNC_ENTER(unit);

    LTSW_IDX_TBL_MGMT_INIT_CHECK(unit);

    LTSW_IDX_TBL_USER_ID_CHECK(unit, tbl_id, user_id);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    LTSW_IDX_TBL_MGMT_LOCK(unit);
    lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_idx_tbl_info_validate(unit, tbl_id, user_id));

    if (LTSW_IDX_TBL_USER_FUNC_CMP(unit, tbl_id, user_id) == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    idx_min = LTSW_IDX_TBL_USER_IDX_MIN(unit, tbl_id, user_id);
    idx_maxused = LTSW_IDX_TBL_USER_IDX_MAXUSED(unit, tbl_id, user_id);
    if ((flags & BCMI_LTSW_IDX_TBL_OP_SKIP_INDEX_ZERO) && (idx_min == 0)) {
        idx_min = 1;
    }

    if (LTSW_IDX_TBL_USER_FUNC_HASH(unit, tbl_id, user_id)) {
        LTSW_IDX_TBL_USER_FUNC_HASH(unit, tbl_id, user_id)(unit, entry, &hash);
    }

    for (idx = idx_min; idx <= idx_maxused; idx++) {
        if (!(LTSW_IDX_TBL_ENTRY_USED_GET(unit, tbl_id, idx))) {
            /* Entry not used. */
            continue;
        }

        if (LTSW_IDX_ENTRY_USER_ID(unit, tbl_id, idx) != user_id) {
            /* Not the same user. */
            continue;
        }

        if (LTSW_IDX_ENTRY_HASH(unit, tbl_id, idx) != hash) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (LTSW_IDX_TBL_USER_FUNC_CMP(unit, tbl_id, user_id)(unit, idx, entry,
                                                               &cmp_result));
        if (cmp_result != 0) {
            /* Not equal. */
            continue;
        }

        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Entry[%d] found.\n"), idx));
        if (index != NULL) {
            *index = idx;
        }

        SHR_EXIT();
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    if (lock) {
        LTSW_IDX_TBL_MGMT_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_idx_tbl_entry_get_by_idx(int unit, uint32_t flags,
                                   bcmi_ltsw_idx_tbl_id_t tbl_id,
                                   int user_id,
                                   void *entry, int index)
{
    int idx_min, idx_maxused;
    int lock = 0;

    SHR_FUNC_ENTER(unit);

    LTSW_IDX_TBL_MGMT_INIT_CHECK(unit);

    LTSW_IDX_TBL_USER_ID_CHECK(unit, tbl_id, user_id);

    LTSW_IDX_TBL_MGMT_LOCK(unit);
    lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_idx_tbl_info_validate(unit, tbl_id, user_id));

    idx_min = LTSW_IDX_TBL_USER_IDX_MIN(unit, tbl_id, user_id);
    idx_maxused = LTSW_IDX_TBL_USER_IDX_MAXUSED(unit, tbl_id, user_id);
    if ((flags & BCMI_LTSW_IDX_TBL_OP_SKIP_INDEX_ZERO) && (idx_min == 0)) {
        idx_min = 1;
    }

    if ((index < idx_min) || (index > idx_maxused)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (!(LTSW_IDX_TBL_ENTRY_USED_GET(unit, tbl_id, index))) {
        /* Entry not used. */
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (LTSW_IDX_ENTRY_USER_ID(unit, tbl_id, index) != user_id) {
        /* Not the same user. */
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Entry exist, currently we only return the status. */

exit:
    if (lock) {
        LTSW_IDX_TBL_MGMT_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Increase reference count for an specific entry.
 *
 * This function is used to increase reference count for an specific entry.
 * During warmboot, it is the table user's responsibility to call this API
 * to recover software bookkeepings.*
 *
 * \param [in] unit           Unit number.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User Id.
 * \param [in] index          Entry index.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_idx_tbl_entry_ref_increase(int unit, bcmi_ltsw_idx_tbl_id_t tbl_id,
                                     int user_id,
                                     uint32_t index)
{
    int lock = 0;

    SHR_FUNC_ENTER(unit);

    LTSW_IDX_TBL_MGMT_INIT_CHECK(unit);

    LTSW_IDX_TBL_MGMT_LOCK(unit);
    lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_idx_tbl_info_validate(unit, tbl_id, user_id));

    LTSW_IDX_TBL_USER_ENTRY_IDX_CHECK(unit, tbl_id, user_id, index);

    if (LTSW_IDX_TBL_ENTRY_USED_GET(unit, tbl_id, index) == 0) {
        LTSW_IDX_ENTRY_USER_ID(unit, tbl_id, index) = user_id;
        LTSW_IDX_TBL_ENTRY_USED_SET(unit, tbl_id, index);
    }

    LTSW_IDX_ENTRY_REFCNT(unit, tbl_id, index) += 1;

    if (LTSW_IDX_TBL_USER_IDX_MAXUSED(unit, tbl_id, user_id) < index) {
        LTSW_IDX_TBL_USER_IDX_MAXUSED(unit, tbl_id, user_id) = index;
    }

exit:
    if (lock == 1) {
        LTSW_IDX_TBL_MGMT_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get reference count for an specific entry.
 *
 * This function is used to get reference count for an specific entry.
 *
 * \param [in] unit           Unit number.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User Id.
 * \param [in] index          Entry index.
 * \param [out] ref_cnt       Reference count.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_idx_tbl_entry_ref_get(int unit, bcmi_ltsw_idx_tbl_id_t tbl_id,
                                int user_id, uint32_t index,
                                uint32_t *ref_cnt)
{
    int lock = 0;

    SHR_FUNC_ENTER(unit);

    LTSW_IDX_TBL_MGMT_INIT_CHECK(unit);

    SHR_NULL_CHECK(ref_cnt, SHR_E_PARAM);

    LTSW_IDX_TBL_MGMT_LOCK(unit);
    lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_idx_tbl_info_validate(unit, tbl_id, user_id));

    LTSW_IDX_TBL_USER_ENTRY_IDX_CHECK(unit, tbl_id, user_id, index);

    *ref_cnt = LTSW_IDX_ENTRY_REFCNT(unit, tbl_id, index);

exit:
    if (lock == 1) {
        LTSW_IDX_TBL_MGMT_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Update hash data for specific entry.
 *
 * This function is used to update hash data for specific entry.
 * During warmboot, it is the table user's responsibility to call this API
 * to recover software bookkeepings.*
 *
 * \param [in] unit           Unit number.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User Id.
 * \param [in] entry          Entry information.
 * \param [in] index          Entry index.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_idx_tbl_entry_hash_update(int unit, bcmi_ltsw_idx_tbl_id_t tbl_id,
                                    int user_id, void *entry, uint32_t index)
{
    int lock = 0;
    uint16_t hash;

    SHR_FUNC_ENTER(unit);

    LTSW_IDX_TBL_MGMT_INIT_CHECK(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    LTSW_IDX_TBL_MGMT_LOCK(unit);
    lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_idx_tbl_info_validate(unit, tbl_id, user_id));

    LTSW_IDX_TBL_USER_ENTRY_IDX_CHECK(unit, tbl_id, user_id, index);

    if (LTSW_IDX_TBL_USER_FUNC_HASH(unit, tbl_id, user_id) == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    LTSW_IDX_TBL_USER_FUNC_HASH(unit, tbl_id, user_id)(unit, entry, &hash);
    LTSW_IDX_ENTRY_HASH(unit, tbl_id, index) = hash;

exit:
    if (lock == 1) {
        LTSW_IDX_TBL_MGMT_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}
