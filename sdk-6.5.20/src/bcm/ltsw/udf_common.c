/*! \file udf_common.c
 *
 * UDF Module Internal common routines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bsl/bsl.h>
#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm_int/control.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/mbcm/udf.h>

#include <shr/shr_bitop.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_UDF

#include <bcm_int/ltsw/udf_int.h>

bcmint_udf_control_t *ltsw_udf_control[BCM_MAX_NUM_UNITS] = {0};

/*!
 * \brief Create protection mutex for UDF module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_udf_lock_create(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (UDF_CONTROL(unit)->lock == NULL) {
        UDF_CONTROL(unit)->lock = sal_mutex_create("LTSW UDF Mutex");
        SHR_NULL_CHECK(UDF_CONTROL(unit)->lock, SHR_E_MEMORY);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy protection mutex for UDF module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 */
static int
ltsw_udf_lock_destroy(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (UDF_CONTROL(unit)->lock != NULL) {
        sal_mutex_destroy(UDF_CONTROL(unit)->lock);
        UDF_CONTROL(unit)->lock = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief To allocate running id for UDF IDs.
 *
 * \param [in]  unit Unit Number.
 * \param [out] Reference to UDF object ID.
 *
 * \retval SHR_E_NONE Success.
 */
int
bcmint_udf_obj_running_id_alloc(int unit, bcm_udf_id_t *udf_id)
{
    int rv;
    int id;
    int min_id, max_id;

    SHR_FUNC_ENTER(unit);

    /* Validate Range check */
    min_id = UDF_CONTROL(unit)->info.min_obj_id;
    max_id = UDF_CONTROL(unit)->info.max_obj_id;

    id = BCMINT_UDF_OBJ_ID_RUNNING(unit);

    if (id > max_id) {
        for (id = min_id; id <= max_id; id++) {
            rv = mbcm_ltsw_udf_is_object_exist(unit, *udf_id);
            if (rv == SHR_E_NOT_FOUND) {
                break;
            }

            if (BCM_FAILURE(rv)) {
               SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }

        /* All the udf ids are used up */
        if (id > max_id) {
           SHR_IF_ERR_VERBOSE_EXIT(BCM_E_FULL);
        }
    }

    *udf_id = id;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief To allocate running id for UDF Pkt Format IDs.
 *
 * \param [in]  unit Unit Number.
 * \param [out] Reference to UDF pkt format object ID.
 *
 * \retval SHR_E_NONE Success.
 */
int
bcmint_udf_pkt_format_obj_running_id_alloc(
    int unit,
    bcm_udf_pkt_format_id_t *pkt_format_id)
{
    int rv = SHR_E_NONE;
    int id;
    int min_id, max_id;

    SHR_FUNC_ENTER(unit);

    /* Validate Range check */
    min_id = UDF_CONTROL(unit)->info.min_obj_id;
    max_id = UDF_CONTROL(unit)->info.max_obj_id;

    id = BCMINT_UDF_PKT_FORMAT_ID_RUNNING(unit);

    if (id > max_id) {
        for (id = min_id; id <= max_id; id++) {
            rv = mbcm_ltsw_udf_is_pkt_format_exist(unit, id);
            if (rv == SHR_E_NOT_FOUND) {
                break;
            }

            if (BCM_FAILURE(rv)) {
               SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }

        /* All the udf ids are used up */
        if (id > max_id) {
           SHR_IF_ERR_VERBOSE_EXIT(BCM_E_FULL);
        }
    }

    *pkt_format_id = id;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to detach UDF common resources.
 */
int bcmint_udf_common_detach(int unit)
{
    bcmint_udf_control_t *udf_ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    udf_ctrl = UDF_CONTROL(unit);
    if (udf_ctrl == NULL) {
        return BCM_E_NONE;
    }

    /* Detach the driver */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_detach(unit));

    /* Destroy the mutex */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lock_destroy(unit));

    /* Free the UDF control */
    BCMINT_UDF_MEMFREE(udf_ctrl);
    UDF_CONTROL(unit) = NULL;
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to initialize UDF common resources and
 * device specific routines.
 */
int bcmint_udf_common_init(int unit)
{
    int rv = SHR_E_NONE;
    bcmint_udf_control_t *udf_ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    udf_ctrl = UDF_CONTROL(unit);

    /* Detach first if it's been previously initialized. */
    if (udf_ctrl != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_udf_common_detach(unit));
        udf_ctrl = NULL;
    }

    /* Allocate a bcm_udf_control */
    BCMINT_UDF_MEMALLOC(udf_ctrl, sizeof (bcmint_udf_control_t),
                     "UDF common control");
    if (NULL == udf_ctrl) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_MEMORY);
    }
    UDF_CONTROL(unit) = udf_ctrl;

    /* create the mutex lock */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lock_create(unit));

    /* Initialize the driver */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_init(unit));

exit:
    if (SHR_FAILURE(rv) && (UDF_CONTROL(unit) != NULL)) {
        BCMINT_UDF_MEMFREE(UDF_CONTROL(unit));
        UDF_CONTROL(unit) = NULL;
    }

    SHR_FUNC_EXIT();
}

int
bcmint_udf_sanitize_inputs(
    int unit,
    bcm_udf_alloc_hints_t *hints,
    bcm_udf_id_t *udf_id)
{
    int rv;
    SHR_FUNC_ENTER(unit);

    /* Validate hints */
    if (hints != NULL) {
        /* Replace flag is not supported. */
        if (hints->flags & BCM_UDF_CREATE_O_REPLACE) {
            SHR_IF_ERR_VERBOSE_EXIT(BCM_E_PARAM);
        }

        /* Validate UDF ID */
        if (hints->flags & BCM_UDF_CREATE_O_WITHID) {
            BCMINT_UDF_ID_VALIDATE(unit, *udf_id);

            /* Validate whether UDF object is already exist */
            rv = mbcm_ltsw_udf_is_object_exist(unit, *udf_id);
            if (SHR_SUCCESS(rv)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
            }
        }

        /* check if chip supports Range Check */
        if (hints->flags & BCM_UDF_CREATE_O_RANGE_CHECK) {
            /* Check if device supports Range Check. */
            if (!(UDF_CONTROL(unit)->info.flags &
                  BCMINT_UDF_RANGE_CHECK_SUPPORTED)) {
                SHR_IF_ERR_VERBOSE_EXIT(BCM_E_PARAM);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_udf_multi_chunk_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_multi_chunk_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_multi_chunk_info_get(unit, udf_id, info, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_udf_destroy(
    int unit,
    bcm_udf_id_t udf_id)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_destroy(unit, udf_id));
exit:
    SHR_FUNC_EXIT();
}

