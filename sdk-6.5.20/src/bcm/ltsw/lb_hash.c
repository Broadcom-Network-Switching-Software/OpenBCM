/*! \file lb_hash.c
 *
 * LB hash management.
 * This file contains the management for LB hash.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>

#include <bcm_int/ltsw/lb_hash.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/mbcm/lb_hash.h>

#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <bcm_int/ltsw/field_int.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_HASH

/*!
 * \brief Generic database of LB Hash.
 */
typedef struct lb_hash_info_s {

    /*! Initialization signal. */
    bool inited;

    /*! Module lock. */
    sal_mutex_t mutex;

} lb_hash_info_t;

static lb_hash_info_t lb_hash_info[BCM_MAX_NUM_UNITS] = {{0}};

#define LB_HASH_INITED(_u) (lb_hash_info[_u].inited)

#define LB_HASH_LOCK(_u)      \
    sal_mutex_take(lb_hash_info[_u].mutex, SAL_MUTEX_FOREVER)

#define LB_HASH_UNLOCK(_u)    \
    sal_mutex_give(lb_hash_info[_u].mutex)

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public functions
 */
int
bcmint_ltsw_lb_hash_os_tbl_get(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    const bcmint_ltsw_lb_hash_os_tbl_t **tbl_info)
{
    bcmint_ltsw_lb_hash_os_tbl_db_t tbl_db = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_os_tbl_db_get(unit, &tbl_db));

    *tbl_info = &tbl_db.tbls[type];

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_init(int unit)
{
    lb_hash_info_t *lhi = &lb_hash_info[unit];
    SHR_FUNC_ENTER(unit);

    if (lhi->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_lb_hash_deinit(unit));
    }

    lhi->mutex = sal_mutex_create("bcmLtswLbHashInfo");
    SHR_NULL_CHECK(lhi->mutex, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_init(unit));

    lhi->inited = 1;

exit:
    if (SHR_FUNC_ERR()) {
        if (lhi->mutex) {
            sal_mutex_destroy(lhi->mutex);
        }
        (void)bcmi_ltsw_lb_hash_deinit(unit);
    }
    SHR_FUNC_EXIT();

}

int
bcmi_ltsw_lb_hash_deinit(int unit)
{
    lb_hash_info_t *lhi = &lb_hash_info[unit];

    SHR_FUNC_ENTER(unit);

    if (lhi->inited) {
        SHR_NULL_CHECK(lhi->mutex, SHR_E_INIT);

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_lb_hash_deinit(unit));

        LB_HASH_LOCK(unit);
        lhi->inited = 0;
        LB_HASH_UNLOCK(unit);

        sal_mutex_destroy(lhi->mutex);
        lhi->mutex = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_os_port_control_set(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcm_port_t port,
    int value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_os_port_control_set(unit, type, port, value));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_lb_hash_os_port_control_get(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcm_port_t port,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_os_port_control_get(unit, type, port, value));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_lb_hash_os_flow_control_set(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcmi_ltsw_lb_hash_flow_field_t field,
    int value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_os_flow_control_set(unit, type, field, value));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_lb_hash_os_flow_control_get(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcmi_ltsw_lb_hash_flow_field_t field,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_os_flow_control_get(unit, type, field, value));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_lb_hash_fields_select_set(
    int unit,
    bcmi_ltsw_lb_hash_fields_select_t type,
    int value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_fields_select_set(unit, type, value));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_lb_hash_fields_select_get(
    int unit,
    bcmi_ltsw_lb_hash_fields_select_t type,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_fields_select_get(unit, type, value));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_bins_set(
    int unit,
    bcmi_ltsw_lb_hash_bin_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_bins_set(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_bins_get(
    int unit,
    bcmi_ltsw_lb_hash_bin_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_bins_get(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_lb_hash_pkt_hdr_sel_set(
    int unit,
    bcmi_ltsw_lb_hash_phs_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_pkt_hdr_sel_set(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_lb_hash_pkt_hdr_sel_get(
    int unit,
    bcmi_ltsw_lb_hash_phs_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_pkt_hdr_sel_get(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_lb_hash_ip6_collapse_set(
    int unit,
    bcmi_ltsw_lb_hash_ip6cs_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_ip6_collapse_set(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_lb_hash_ip6_collapse_get(
    int unit,
    bcmi_ltsw_lb_hash_ip6cs_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_ip6_collapse_get(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_lb_hash_algorithm_set(
    int unit,
    bcmi_ltsw_lb_hash_algorithm_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_algorithm_set(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_lb_hash_algorithm_get(
    int unit,
    bcmi_ltsw_lb_hash_algorithm_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_algorithm_get(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_lb_hash_symmetric_control_set(
    int unit,
    int value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_symmetric_control_set(unit, value));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_lb_hash_symmetric_control_get(
    int unit,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_symmetric_control_get(unit, value));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_lb_hash_control_set(
    int unit,
    bcmi_ltsw_hash_control_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_control_set(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_lb_hash_control_get(
    int unit,
    bcmi_ltsw_hash_control_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_control_get(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_bins_flex_set(
    int unit,
    int enable)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_bins_flex_set(unit, enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_flex_entry_create(
    int unit,
    bcm_field_qset_t qset,
    bcm_hash_entry_t *entry)
{
    int qual_count;
    int qual_array[4];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    LB_HASH_LOCK(unit);

    sal_memset(qual_array, 0,
            sizeof (int) * 4);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qset_id_multi_get(unit, qset,
                                        bcmFieldQualifyUdf,
                                        4,
                                        qual_array, &qual_count));
    if (qual_count == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_flex_entry_alloc(unit, qual_count,
                                            qual_array, entry));

exit:
    LB_HASH_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_flex_entry_destroy(
    int unit,
    bcm_hash_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    LB_HASH_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_flex_entry_remove(unit, entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_flex_entry_free(unit, entry));
exit:

    LB_HASH_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_flex_entry_install(
    int unit,
    bcm_hash_entry_t entry,
    uint32 offset)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    LB_HASH_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_flex_entry_install(unit, entry, 0, offset));

exit:

    LB_HASH_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_flex_entry_reinstall(
    int unit,
    bcm_hash_entry_t entry,
    uint32 offset)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    LB_HASH_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_flex_entry_install(unit, entry, 1, offset));

exit:

    LB_HASH_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_flex_entry_remove(
    int unit,
    bcm_hash_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    LB_HASH_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_flex_entry_remove(unit, entry));

exit:

    LB_HASH_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_flex_entry_set(
    int unit,
    bcm_hash_entry_t entry,
    bcm_switch_hash_entry_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    LB_HASH_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_flex_entry_set(unit, entry, config));

exit:

    LB_HASH_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_flex_entry_get(
    int unit,
    bcm_hash_entry_t entry,
    bcm_switch_hash_entry_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    LB_HASH_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_flex_entry_get(unit, entry, config));

exit:

    LB_HASH_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_flex_entry_traverse(
    int unit,
    int flags,
    bcm_switch_hash_entry_traverse_cb cb_fn,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    LB_HASH_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_flex_entry_traverse(unit, flags,
                                               cb_fn, user_data));

exit:

    LB_HASH_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_flex_qualify_udf(
    int unit,
    bcm_hash_entry_t entry,
    bcm_udf_id_t udf_id,
    int length,
    uint8 *data,
    uint8 *mask)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    LB_HASH_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_flex_qualify_udf(unit, entry, udf_id,
                                            length, data, mask));

exit:
    LB_HASH_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_flex_qualify_udf_get(
    int unit,
    bcm_hash_entry_t entry,
    bcm_udf_id_t udf_id,
    int max_length,
    uint8 *data,
    uint8 *mask,
    int *actual_length)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    LB_HASH_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_flex_qualify_udf_get(unit, entry, udf_id,
                                                max_length, data, mask,
                                                actual_length));

exit:
    LB_HASH_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_versatile_control_set(
    int unit,
    bcmi_ltsw_hash_versatile_control_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    LB_HASH_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_lb_hash_versatile_control_set(unit, control, value));

exit:
    LB_HASH_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_versatile_control_get(
    int unit,
    bcmi_ltsw_hash_versatile_control_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    LB_HASH_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_lb_hash_versatile_control_get(unit, control, value));

exit:
    LB_HASH_UNLOCK(unit);

    SHR_FUNC_EXIT();

}

int
bcmi_ltsw_lb_hash_control_seed_set(
    int unit,
    bcmi_ltsw_hash_control_seed_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    LB_HASH_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_lb_hash_control_seed_set(unit, control, value));

exit:
    LB_HASH_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_lb_hash_control_seed_get(
    int unit,
    bcmi_ltsw_hash_control_seed_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    LB_HASH_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_lb_hash_control_seed_get(unit, control, value));

exit:
    LB_HASH_UNLOCK(unit);

    SHR_FUNC_EXIT();

}

int
bcmi_ltsw_lb_hash_macro_flow_seed_set(
    int unit,
    bcmi_ltsw_macro_flow_t type,
    int value)
{
    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    LB_HASH_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_lb_hash_macro_flow_seed_set(unit, type, value));

exit:
    LB_HASH_UNLOCK(unit);

    SHR_FUNC_EXIT();

}

