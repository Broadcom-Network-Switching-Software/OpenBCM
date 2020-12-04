/*! \file udf.c
 *
 * BCM UDF module multiplexing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/ltsw/mbcm/mbcm.h>
#include <bcm_int/ltsw/mbcm/udf.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_UDF

static mbcm_ltsw_udf_drv_t mbcm_ltsw_udf_drv[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public functions
 */

int
mbcm_ltsw_udf_drv_set(int unit, mbcm_ltsw_udf_drv_t *drv)
{
    mbcm_ltsw_udf_drv_t *local = &mbcm_ltsw_udf_drv[unit];

    mbcm_ltsw_drv_init((mbcm_ltsw_drv_t *)local,
                       (mbcm_ltsw_drv_t *)drv,
                       sizeof(*local)/MBCM_LTSW_FNPTR_SZ);

    return SHR_E_NONE;
}

int
mbcm_ltsw_udf_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_multi_chunk_create(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_multi_chunk_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_multi_chunk_create(unit, udf_id, info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_multi_chunk_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_multi_chunk_info_t *info,
    int *flags)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_multi_chunk_info_get(unit, udf_id,
                                                          info, flags));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_destroy(
    int unit,
    bcm_udf_id_t udf_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_destroy(unit, udf_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_is_object_exist(
    int unit,
    bcm_udf_id_t udf_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_is_object_exist(unit, udf_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_multi_pkt_format_info_get(
    int unit,
    bcm_udf_abstract_pkt_format_t pkt_fmt,
    bcm_udf_multi_abstract_pkt_format_info_t *pkt_fmt_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_multi_pkt_fmt_info_get(unit,
                                                            pkt_fmt,
                                                            pkt_fmt_info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_abstr_pkt_fmt_object_list_get(
    int unit,
    bcm_udf_abstract_pkt_format_t pkt_fmt,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_abstr_pkt_fmt_object_list_get(
              unit, pkt_fmt, max, udf_id_list, actual));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_abstr_pkt_fmt_list_get(
    int unit,
    int max,
    bcm_udf_abstract_pkt_format_t *pkt_fmt_list,
    int *actual)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_abstr_pkt_fmt_list_get(
              unit, max, pkt_fmt_list, actual));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_create(
    int unit,
    bcm_udf_alloc_hints_t *hints,
    bcm_udf_t *udf_info,
    bcm_udf_id_t *udf_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_create(
              unit, hints, udf_info, udf_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_t *udf_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_get(
              unit, udf_id, udf_info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_get_all(
    int unit,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_get_all(
              unit, max, udf_id_list, actual));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_pkt_format_create(
    int unit,
    bcm_udf_pkt_format_options_t options,
    bcm_udf_pkt_format_info_t *pkt_format,
    bcm_udf_pkt_format_id_t *pkt_format_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_pkt_format_create(
              unit, options, pkt_format, pkt_format_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_pkt_format_info_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_info_t *pkt_format)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_pkt_format_info_get(
              unit, pkt_format_id, pkt_format));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_pkt_format_destroy(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_pkt_format_destroy(
              unit, pkt_format_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_is_pkt_format_exist(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_is_pkt_format_exist(
            unit, pkt_format_id));

exit:

    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_pkt_format_add(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_pkt_format_id_t pkt_format_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_pkt_format_add(
              unit, udf_id, pkt_format_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_pkt_format_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_pkt_format_get(
              unit, pkt_format_id, max, udf_id_list, actual));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_pkt_format_delete(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_pkt_format_id_t pkt_format_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_pkt_format_delete(
              unit, udf_id, pkt_format_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_pkt_format_get_all(
    int unit,
    bcm_udf_id_t udf_id,
    int max,
    bcm_udf_pkt_format_id_t *pkt_format_id_list,
    int *actual)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_pkt_format_get_all(
              unit, udf_id, max, pkt_format_id_list, actual));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_pkt_format_delete_all(
    int unit,
    bcm_udf_id_t udf_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_pkt_format_delete_all(
              unit, udf_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_oper_mode_get(
    int unit,
    bcm_udf_oper_mode_t *mode)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_oper_mode_get(
              unit, mode));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_hash_config_add(
    int unit,
    uint32 options,
    bcm_udf_hash_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_hash_config_add(unit,
                                                     options,
                                                     config));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_hash_config_delete(
    int unit,
    bcm_udf_hash_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_hash_config_delete(unit,
                                                        config));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_hash_config_delete_all(
    int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_hash_config_delete_all(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_hash_config_get(
    int unit,
    bcm_udf_hash_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_hash_config_get(unit,
                                                     config));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_hash_config_get_all(
    int unit,
    int max,
    bcm_udf_hash_config_t *config_list,
    int *actual)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_hash_config_get_all(unit,
                                                         max,
                                                         config_list,
                                                         actual));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_pkt_format_action_set(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_action_t action,
    int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_pkt_format_action_set(unit,
                                                           pkt_format_id,
                                                           action, arg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_udf_pkt_format_action_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_action_t action,
    int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_udf_drv[unit].udf_pkt_format_action_get(unit,
                                                           pkt_format_id,
                                                           action, arg));

exit:
    SHR_FUNC_EXIT();
}
