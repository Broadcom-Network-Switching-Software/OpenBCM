/*! \file udf.c
 *
 * UDF Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bsl/bsl.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/udf.h>

#include <shr/shr_bitop.h>
#include <shr/shr_debug.h>

#include <bcm_int/control.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/mbcm/udf.h>
#include <bcm_int/ltsw/udf_int.h>
#include <bcm_int/ltsw/util.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_UDF


/******************************************************************************
 * UDF APIs
 */

/*
 * Function:
 *      bcm_udf_init
 * Purpose:
 *      Initialize UDF module
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      SHR_E_NONE          UDF module initialized successfully.
 *      SHR_E_UNIT          Invalid BCM Unit number.
 *      SHR_E_UNAVAIL       Feature not supported.
 *      SHR_E_XXX           Standard error code.
 * Notes:
 */
int
bcm_ltsw_udf_init(
    int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_udf_common_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_udf_detach
 * Purpose:
 *      Detach UDF module
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      SHR_E_NONE          UDF module detached successfully.
 *      SHR_E_UNIT          Invalid BCM Unit number.
 *      SHR_E_INIT          UDF module not initialized.
 *      SHR_E_UNAVAIL       Feature not supported.
 *      SHR_E_XXX           Standard error code.
 * Notes:
 */
int
bcm_ltsw_udf_detach(
    int unit)
{
    SHR_FUNC_ENTER(unit);

    if (UDF_CONTROL(unit) == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_udf_common_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create UDF object for a given multi chunk information.
 *
 * \param [in]     unit     Unit number
 * \param [in]     hints    Hints to UDF allocator (optional).
 * \param [in]     info     Reference to UDF multi-chunk info structure.
 * \param [in/out] udf_id   UDF object ID.
 *
 *
 * \retval   SHR_E_NONE        UDF object created successfully.
 * \retval   SHR_E_UNIT        Invalid BCM unit number.
 * \retval   SHR_E_INIT        UDF module not initialized.
 * \retval   SHR_E_UNAVAIL     Feature not supported.
 * \retval   SHR_E_xx          Standard error code.
 */
int
bcm_ltsw_udf_multi_chunk_create(
    int unit,
    bcm_udf_alloc_hints_t *hints,
    bcm_udf_multi_chunk_info_t *info,
    bcm_udf_id_t *udf_id)
{
    int id_running_allocated = 0;
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    /* NULL param check */
    if (info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Sanitize input parameters */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_udf_sanitize_inputs(unit, hints, udf_id));

    /* Check if similar entry already existing */
    if ((hints == NULL) || !(hints->flags & BCM_UDF_CREATE_O_WITHID)) {
        /* Allocate new udf id */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_udf_obj_running_id_alloc(unit, udf_id));
        id_running_allocated = 1;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_multi_chunk_create(unit, *udf_id, info));

exit:
    if (SHR_FAILURE(_func_rv)) {
        if (id_running_allocated == 1) {
            /* Decrement the running udf id */
            UDF_CONTROL(unit)->info.udf_id_running -= 1;
        }
    }
    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve UDF multi-chunk information for a given UDF object.
 *
 * \param [in]     unit     Unit number
 * \param [in]     udf_id   UDF object ID.
 * \param [out]    info     Reference to UDF multi chunk info structure.
 *
 *
 * \retval   SHR_E_NONE        UDF info retrieved successfully.
 * \retval   SHR_E_UNIT        Invalid BCM unit number.
 * \retval   SHR_E_INIT        UDF module not initialized.
 * \retval   SHR_E_UNAVAIL     Feature not supported.
 * \retval   SHR_E_xx          Standard error code.
 */
int
bcm_ltsw_udf_multi_chunk_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_multi_chunk_info_t *info)
{
    SHR_FUNC_ENTER(unit);
    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    /* NULL param check */
    if (info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_multi_chunk_info_get(unit, udf_id, info, NULL));
exit:
    BCMINT_UDF_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_udf_destroy
 * Purpose:
 *      Destroys the UDF object
 * Parameters:
 *      unit - (IN) Unit number.
 *      udf_id - (IN) UDF Object ID
 * Returns:
 *      SHR_E_NONE          UDF deleted successfully.
 *      SHR_E_NOT_FOUND     UDF does not exist.
 *      SHR_E_UNIT          Invalid BCM Unit number.
 *      SHR_E_INIT          UDF module not initialized.
 *      SHR_E_UNAVAIL       Feature not supported.
 *      SHR_E_XXX           Standard error code.
 * Notes:
 */
int
bcm_ltsw_udf_destroy(
    int unit,
    bcm_udf_id_t udf_id)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_udf_destroy(unit, udf_id));

exit:
    BCMINT_UDF_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve the list of UDF Objects associated with a given abstract
 *        packet format
 *
 * \param [in]   unit                   Unit number
 * \param [in]   abstract_pkt_format    UDF Abstract packet format.
 * \param [in]   max                    Size of 'udf_id_list' array.
 * \param [in]   udf_id_list            Reference to UDF ID array.
 * \param [out]  actual                 Actual number of supported IDs.
 *
 * \retval   SHR_E_NONE        UDF list retrieved successfully.
 * \retval   SHR_E_UNIT        Invalid BCM unit number.
 * \retval   SHR_E_INIT        UDF module not initialized.
 * \retval   SHR_E_UNAVAIL     Feature not supported.
 * \retval   SHR_E_xx          Standard error code.
 */
int
bcm_ltsw_udf_abstract_pkt_format_object_list_get(
    int unit,
    bcm_udf_abstract_pkt_format_t pkt_fmt,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_abstr_pkt_fmt_object_list_get(unit, pkt_fmt, max,
                                                     udf_id_list, actual));

exit:
    BCMINT_UDF_UNLOCK(unit);
    SHR_FUNC_EXIT();

}

/*
 * Function:
 *      bcm_udf_multi_abstract_pkt_format_info_get
 * Purpose:
 *      Retrieve packet format multi info given the packet format Id
 * Parameters:
 *      unit - (IN) Unit number.
 *      pkt_fmt - (IN) Abstract Packet format
 *      pkt_format - (OUT) UDF packet format info structure
 * Returns:
 *      SHR_E_NONE          Packet format get successful.
 *      SHR_E_NOT_FOUND     Packet format entry does not exist.
 *      SHR_E_UNIT          Invalid BCM Unit number.
 *      SHR_E_INIT          UDF module not initialized.
 *      SHR_E_UNAVAIL       Feature not supported.
 *      SHR_E_XXX           Standard error code.
 * Notes:
 */
int bcm_ltsw_udf_multi_abstract_pkt_format_info_get(
    int unit,
    bcm_udf_abstract_pkt_format_t pkt_fmt,
    bcm_udf_multi_abstract_pkt_format_info_t *pkt_fmt_info)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_multi_pkt_format_info_get(unit,
                                                 pkt_fmt,
                                                 pkt_fmt_info));

exit:
    BCMINT_UDF_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_abstract_pkt_format_supported_list_get(
    int unit,
    int max,
    bcm_udf_abstract_pkt_format_t *abstr_pkt_fmt_list,
    int *actual)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_abstr_pkt_fmt_list_get(unit, max,
                                              abstr_pkt_fmt_list,
                                              actual));

exit:
    BCMINT_UDF_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_create(
    int unit,
    bcm_udf_alloc_hints_t *hints,
    bcm_udf_t *udf_info,
    bcm_udf_id_t *udf_id)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_create(unit, hints, udf_info, udf_id));

exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_t *udf_info)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_get(unit, udf_id, udf_info));

exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_get_all(
    int unit,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_get_all(unit, max,
                               udf_id_list, actual));

exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_pkt_format_create(
    int unit,
    bcm_udf_pkt_format_options_t options,
    bcm_udf_pkt_format_info_t *pkt_format,
    bcm_udf_pkt_format_id_t *pkt_format_id)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_pkt_format_create(unit, options,
                                         pkt_format, pkt_format_id));

exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_pkt_format_info_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_info_t *pkt_format)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_pkt_format_info_get(unit,
                                           pkt_format_id,
                                           pkt_format));

exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_pkt_format_destroy(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_pkt_format_destroy(unit, pkt_format_id));

exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_pkt_format_add(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_pkt_format_id_t pkt_format_id)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_pkt_format_add(unit, udf_id,
                                      pkt_format_id));

exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_pkt_format_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_pkt_format_get(unit, pkt_format_id,
                                      max, udf_id_list, actual));

exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_pkt_format_delete(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_pkt_format_id_t pkt_format_id)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_pkt_format_delete(unit, udf_id,
                                         pkt_format_id));

exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_pkt_format_get_all(
    int unit,
    bcm_udf_id_t udf_id,
    int max,
    bcm_udf_pkt_format_id_t *pkt_format_id_list,
    int *actual)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_pkt_format_get_all(unit, udf_id, max,
                                          pkt_format_id_list,
                                          actual));

exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_pkt_format_delete_all(
    int unit,
    bcm_udf_id_t udf_id)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_pkt_format_delete_all(unit, udf_id));

exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_oper_mode_get(
    int unit,
    bcm_udf_oper_mode_t *mode)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_oper_mode_get(unit, mode));

exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_hash_config_add(
    int unit,
    uint32 options,
    bcm_udf_hash_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_hash_config_add(unit, options, config));

exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_hash_config_delete(
    int unit,
    bcm_udf_hash_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_hash_config_delete(unit, config));

exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_hash_config_delete_all(
    int unit)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_hash_config_delete_all(unit));

exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_hash_config_get(
    int unit,
    bcm_udf_hash_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_hash_config_get(unit, config));

exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_hash_config_get_all(
    int unit,
    int max,
    bcm_udf_hash_config_t *config_list,
    int *actual)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_hash_config_get_all(unit, max,
                                           config_list, actual));

exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_pkt_format_action_set(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_action_t action,
    int arg)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_pkt_format_action_set(unit, pkt_format_id,
                                             action, arg));
exit:

    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int bcm_ltsw_udf_pkt_format_action_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_action_t action,
    int *arg)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_pkt_format_action_get(unit, pkt_format_id,
                                             action, arg));
exit:
    BCMINT_UDF_UNLOCK(unit);

    SHR_FUNC_EXIT();
}
