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

#include <bcm_int/ltsw/lb_hash_int.h>
#include <bcm_int/ltsw/mbcm/lb_hash.h>
#include <bcm_int/ltsw/mbcm/mbcm.h>

#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_HASH

/*!
 * \brief The Lb_hash mbcm drivers.
 */
static mbcm_ltsw_lb_hash_drv_t mbcm_ltsw_lb_hash_drv[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public functions
 */
int
mbcm_ltsw_lb_hash_drv_set(
    int unit,
    mbcm_ltsw_lb_hash_drv_t *drv)
{
    mbcm_ltsw_lb_hash_drv_t *local = &mbcm_ltsw_lb_hash_drv[unit];

    SHR_FUNC_ENTER(unit);


    mbcm_ltsw_drv_init((mbcm_ltsw_drv_t *)local,
                       (mbcm_ltsw_drv_t *)drv,
                       sizeof(*local)/MBCM_LTSW_FNPTR_SZ);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_deinit(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_os_port_control_set(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcm_port_t port,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_os_port_control_set(unit,
                                                                 type,
                                                                 port,
                                                                 value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_os_port_control_get(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcm_port_t port,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_os_port_control_get(unit,
                                                                 type,
                                                                 port,
                                                                 value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_os_flow_control_set(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcmi_ltsw_lb_hash_flow_field_t field,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_os_flow_control_set(unit,
                                                                 type,
                                                                 field,
                                                                 value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_os_flow_control_get(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcmi_ltsw_lb_hash_flow_field_t field,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_os_flow_control_get(unit,
                                                                 type,
                                                                 field,
                                                                 value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_fields_select_set(
    int unit,
    bcmi_ltsw_lb_hash_fields_select_t type,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_fields_select_set(unit,
                                                               type,
                                                               value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_fields_select_get(
    int unit,
    bcmi_ltsw_lb_hash_fields_select_t type,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_fields_select_get(unit,
                                                               type,
                                                               value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_bins_set(
    int unit,
    bcmi_ltsw_lb_hash_bin_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_bins_set(unit,
                                                      control,
                                                      value));

exit:
    SHR_FUNC_EXIT();
}


int
mbcm_ltsw_lb_hash_bins_get(
    int unit,
    bcmi_ltsw_lb_hash_bin_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_bins_get(unit,
                                                      control,
                                                      value));

exit:
    SHR_FUNC_EXIT();
}


int
mbcm_ltsw_lb_hash_pkt_hdr_sel_set(
    int unit,
    bcmi_ltsw_lb_hash_phs_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_pkt_hdr_sel_set(unit,
                                                             control,
                                                             value));

exit:
    SHR_FUNC_EXIT();
}


int
mbcm_ltsw_lb_hash_pkt_hdr_sel_get(
    int unit,
    bcmi_ltsw_lb_hash_phs_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_pkt_hdr_sel_get(unit,
                                                             control,
                                                             value));

exit:
    SHR_FUNC_EXIT();
}



int
mbcm_ltsw_lb_hash_ip6_collapse_set(
    int unit,
    bcmi_ltsw_lb_hash_ip6cs_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_ip6_collapse_set(unit,
                                                              control,
                                                              value));

exit:
    SHR_FUNC_EXIT();
}


int
mbcm_ltsw_lb_hash_ip6_collapse_get(
    int unit,
    bcmi_ltsw_lb_hash_ip6cs_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_ip6_collapse_get(unit,
                                                              control,
                                                              value));

exit:
    SHR_FUNC_EXIT();
}


int
mbcm_ltsw_lb_hash_algorithm_set(
    int unit,
    bcmi_ltsw_lb_hash_algorithm_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_algorithm_set(unit,
                                                           control,
                                                           value));

exit:
    SHR_FUNC_EXIT();
}


int
mbcm_ltsw_lb_hash_algorithm_get(
    int unit,
    bcmi_ltsw_lb_hash_algorithm_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_algorithm_get(unit,
                                                           control,
                                                           value));

exit:
    SHR_FUNC_EXIT();
}


int
mbcm_ltsw_lb_hash_symmetric_control_set(
    int unit,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_symmetric_control_set(unit,
                                                                   value));

exit:
    SHR_FUNC_EXIT();
}


int
mbcm_ltsw_lb_hash_symmetric_control_get(
    int unit,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_symmetric_control_get(unit,
                                                                   value));

exit:
    SHR_FUNC_EXIT();
}


int
mbcm_ltsw_lb_hash_control_set(
    int unit,
    bcmi_ltsw_hash_control_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_control_set(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}


int
mbcm_ltsw_lb_hash_control_get(
    int unit,
    bcmi_ltsw_hash_control_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_control_get(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_bins_flex_set(
    int unit,
    int enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_bins_flex_set(unit, enable));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_flex_entry_alloc(
    int unit,
    int qual_count,
    int *qual_list,
    bcm_hash_entry_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_flex_entry_alloc(unit, qual_count,
                                                              qual_list, entry));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_flex_entry_free(
    int unit,
    bcm_hash_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_flex_entry_free(unit, entry));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_flex_entry_install(
    int unit,
    bcm_hash_entry_t entry,
    int is_reinstall,
    uint32 offset)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_flex_entry_install(unit, entry,
                                                                is_reinstall, offset));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_flex_entry_remove(
    int unit,
    bcm_hash_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_flex_entry_remove(unit, entry));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_flex_entry_set(
    int unit,
    bcm_hash_entry_t entry,
    bcm_switch_hash_entry_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_flex_entry_set(unit, entry, config));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_flex_entry_get(
    int unit,
    bcm_hash_entry_t entry,
    bcm_switch_hash_entry_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_flex_entry_get(unit, entry, config));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_flex_entry_traverse(
    int unit,
    int flags,
    bcm_switch_hash_entry_traverse_cb cb_fn,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

#if 0
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_flex_entry_traverse(unit, flags,
                                                                 cb_fn, user_data));

exit:
#endif

    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_flex_qualify_udf(
    int unit,
    bcm_hash_entry_t entry,
    bcm_udf_id_t udf_id,
    int length,
    uint8 *data,
    uint8 *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_flex_qualify_udf(unit, entry,
                                                              udf_id, length,
                                                              data, mask));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_flex_qualify_udf_get(
    int unit,
    bcm_hash_entry_t entry,
    bcm_udf_id_t udf_id,
    int max_length,
    uint8 *data,
    uint8 *mask,
    int *actual_length)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_flex_qualify_udf_get(unit, entry,
                                                                  udf_id, max_length,
                                                                  data, mask,
                                                                  actual_length));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_os_tbl_db_get(
    int unit,
    bcmint_ltsw_lb_hash_os_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_os_tbl_db_get(unit, tbl_db));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_versatile_control_set(
    int unit,
    bcmi_ltsw_hash_versatile_control_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_versatile_control_set(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_versatile_control_get(
    int unit,
    bcmi_ltsw_hash_versatile_control_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_versatile_control_get(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_control_seed_set(
    int unit,
    bcmi_ltsw_hash_control_seed_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_control_seed_set(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_control_seed_get(
    int unit,
    bcmi_ltsw_hash_control_seed_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_control_seed_get(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_lb_hash_macro_flow_seed_set(
    int unit,
    bcmi_ltsw_macro_flow_t type,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv[unit].lb_hash_macro_flow_seed_set(unit, type, value));

exit:
    SHR_FUNC_EXIT();
}

