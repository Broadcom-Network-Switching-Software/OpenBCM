/*! \file field_destination.c
 *
 * Field Destination Implementation above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/field.h>

#include <bcm_int/ltsw/mbcm/field.h>

#include <shr/shr_debug.h>
/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_FP

/*!
 * \brief Defines Software bookkeeping for flow related information.
 */
typedef struct ltsw_field_destination_bookkeeping_s {

    /* Field destination initialized or not. */
    uint8_t initialized;

} ltsw_field_destination_bookkeeping_t;

/*!
 * \Flow book keeping information.
 */
static ltsw_field_destination_bookkeeping_t
       ltsw_field_destination_info[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */
/*!
 * \brief Check if fied destination module is initialized.
 *
 * This function is used to check if field destination module is initialized.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
field_destination_init_check(int unit)
{
    ltsw_field_destination_bookkeeping_t *fd_info;

    SHR_FUNC_ENTER(unit);

    fd_info = &ltsw_field_destination_info[unit];

    if ((fd_info->initialized == FALSE)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */
int
bcmint_ltsw_field_destination_deinit(int unit)
{
    ltsw_field_destination_bookkeeping_t *fd_info;

    SHR_FUNC_ENTER(unit);

    fd_info = &ltsw_field_destination_info[unit];

    if (!fd_info->initialized) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_destination_deinit(unit));
    fd_info->initialized = FALSE;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_field_destination_init(int unit)
{
    ltsw_field_destination_bookkeeping_t *fd_info;

    SHR_FUNC_ENTER(unit);

    fd_info = &ltsw_field_destination_info[unit];

    if (fd_info->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_destination_deinit(unit));
        fd_info->initialized = FALSE;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_destination_init(unit));

    fd_info->initialized = TRUE;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_field_destination_mirror_index_get(
    int unit,
    bcm_field_destination_match_t *match,
    int *mirror_index)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_destination_mirror_index_get(unit, match,
                                                      mirror_index));
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_field_destination_entry_add(
    int unit,
    uint32   options,
    bcm_field_destination_match_t  *match,
    bcm_field_destination_action_t  *action)

{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_init_check(unit));

    SHR_NULL_CHECK(match, SHR_E_PARAM);
    SHR_NULL_CHECK(action, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_destination_entry_add(unit, options, match, action));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_field_destination_entry_get(
    int unit,
    bcm_field_destination_match_t  *match,
    bcm_field_destination_action_t  *action)

{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_init_check(unit));

    SHR_NULL_CHECK(match, SHR_E_PARAM);
    SHR_NULL_CHECK(action, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_destination_entry_get(unit, match, action));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_field_destination_entry_traverse(
    int unit,
    bcm_field_destination_entry_traverse_cb callback,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_init_check(unit));

    SHR_NULL_CHECK(callback, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_destination_entry_traverse(unit, callback, user_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_field_destination_entry_delete(
    int unit,
    bcm_field_destination_match_t  *match)

{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_init_check(unit));

    SHR_NULL_CHECK(match, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_destination_entry_delete(unit, match));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_field_destination_stat_attach(
    int unit,
    bcm_field_destination_match_t *match,
    uint32_t ctr_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_init_check(unit));

    SHR_NULL_CHECK(match, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, ctr_id, &ci));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_attach_counter_id_status_update(unit, ctr_id));

    SHR_IF_ERR_CONT
        (mbcm_ltsw_field_destination_flexctr_attach(unit, match, &ci));

    if (SHR_FUNC_ERR()) {
        (void)
        bcmi_ltsw_flexctr_detach_counter_id_status_update(unit, ctr_id);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_field_destination_stat_detach(
    int unit,
    bcm_field_destination_match_t *match)
{
    bcmi_ltsw_flexctr_counter_info_t ci;
    uint32_t ctr_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_init_check(unit));

    SHR_NULL_CHECK(match, SHR_E_PARAM);

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_destination_flexctr_info_get(unit, match, &ci));

    if (ci.action_index == BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, &ctr_id));

    SHR_IF_ERR_CONT
        (mbcm_ltsw_field_destination_flexctr_detach(unit, match));

    if (!SHR_FUNC_ERR()) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit, ctr_id));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_field_destination_stat_id_get(
    int unit,
    bcm_field_destination_match_t *match,
    uint32_t *ctr_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_init_check(unit));

    SHR_NULL_CHECK(match, SHR_E_PARAM);
    SHR_NULL_CHECK(ctr_id, SHR_E_PARAM);

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_destination_flexctr_info_get(unit, match, &ci));

    if (ci.action_index == BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, ctr_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_field_destination_flexctr_object_set(
    int unit,
    bcm_field_destination_match_t *match,
    uint32_t value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_init_check(unit));

    SHR_NULL_CHECK(match, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_destination_flexctr_object_set(unit, match, value));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_field_destination_flexctr_object_get(
    int unit,
    bcm_field_destination_match_t *match,
    uint32_t *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_init_check(unit));

    SHR_NULL_CHECK(match, SHR_E_PARAM);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_destination_flexctr_object_get(unit, match, value));

exit:
    SHR_FUNC_EXIT();
}
