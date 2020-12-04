/*! \file l3.c
 *
 * BCM L3 module multiplexing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>

#include <bcm_int/ltsw/mbcm/l3.h>
#include <bcm_int/ltsw/mbcm/mbcm.h>

#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

static mbcm_ltsw_l3_drv_t mbcm_ltsw_l3_drv[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Public functions
 */

int
mbcm_ltsw_l3_drv_set(int unit, mbcm_ltsw_l3_drv_t *drv)
{
    mbcm_ltsw_l3_drv_t *local = &mbcm_ltsw_l3_drv[unit];

    SHR_FUNC_ENTER(unit);

    mbcm_ltsw_drv_init((mbcm_ltsw_drv_t *)local,
                       (mbcm_ltsw_drv_t *)drv,
                       sizeof(*local)/MBCM_LTSW_FNPTR_SZ);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_init(unit));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_deinit(unit));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_mtu_set(int unit, bcmi_ltsw_l3_mtu_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_mtu_set(unit, cfg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_mtu_get(int unit, bcmi_ltsw_l3_mtu_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_mtu_get(unit, cfg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_mtu_delete(int unit, bcmi_ltsw_l3_mtu_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_mtu_delete(unit, cfg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_mtu_recover(int unit, int mtu_idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_mtu_recover(unit, mtu_idx));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_info_get(int unit, bcm_l3_info_t *l3info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_info_get(unit, l3info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_enable_set(int unit, int enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_enable_set(unit, enable));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_control_set(int unit,
                         bcmi_ltsw_l3_control_t control, uint32_t value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_control_set(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_control_get(int unit,
                         bcmi_ltsw_l3_control_t control, uint32_t *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_control_get(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_port_control_set(int unit,
                              int port,
                              int control,
                              bcmi_ltsw_l3_port_control_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_port_control_set(unit, port, control, info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_port_control_get(int unit,
                              int port,
                              int control,
                              bcmi_ltsw_l3_port_control_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_port_control_get(unit, port, control, info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ipv6_reserved_multicast_addr_multi_set(
    int unit,
    int num,
    bcm_ip6_t *ip6_addr,
    bcm_ip6_t *ip6_mask)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_ipv6_reserved_multicast_addr_multi_set(unit, num,
                                                                          ip6_addr,
                                                                          ip6_mask));

exit:
    SHR_FUNC_EXIT();

}

int
mbcm_ltsw_l3_ipv6_reserved_multicast_addr_multi_get(
    int unit,
    int num,
    bcm_ip6_t *ip6_addr,
    bcm_ip6_t *ip6_mask)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_ipv6_reserved_multicast_addr_multi_get(unit, num,
                                                                          ip6_addr,
                                                                          ip6_mask));


exit:
    SHR_FUNC_EXIT();

}

int
mbcm_ltsw_l3_ip4_options_profile_create(int unit, uint32 flags,
                                        bcm_l3_ip4_options_action_t default_action,
                                        int *ip4_options_profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_ip4_options_profile_create(unit, flags,
                                                              default_action,
                                                              ip4_options_profile_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ip4_options_profile_destroy(int unit,
                                         int ip4_options_profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_ip4_options_profile_destroy(unit,
                                                               ip4_options_profile_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ip4_options_action_set(int unit, int ip4_options_profile_id,
                                    int ip4_option,
                                    bcm_l3_ip4_options_action_t action)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_ip4_options_action_set(unit, ip4_options_profile_id,
                                                          ip4_option, action));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ip4_options_action_get(int unit, int ip4_options_profile_id,
                                    int ip4_option,
                                    bcm_l3_ip4_options_action_t *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_ip4_options_action_get(unit,
                                                          ip4_options_profile_id,
                                                          ip4_option,
                                                          action));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ip6_prefix_map_add(int unit, bcm_ip6_t ip6_addr)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_ip6_prefix_map_add(unit, ip6_addr));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ip6_prefix_map_get(
    int unit, int map_size,
    bcm_ip6_t *ip6_array, int *ip6_count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_ip6_prefix_map_get(unit, map_size,
                                                      ip6_array,
                                                      ip6_count));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ip6_prefix_map_del(int unit, bcm_ip6_t ip6_addr)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_ip6_prefix_map_del(unit, ip6_addr));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ip6_prefix_map_del_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_ip6_prefix_map_del_all(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ip4_options_profile_id2idx(
    int unit,
    int profile_id,
    int *hw_idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_ip4_options_profile_id2idx(unit, profile_id, hw_idx));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_ip4_options_profile_idx2id(
    int unit,
    int hw_idx,
    int *profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_ip4_options_profile_idx2id(unit, hw_idx, profile_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_l3_switch_control_tbl_get(
    int unit,
    const bcmint_ltsw_l3_switch_control_tbl_t **tbl_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv[unit].l3_switch_control_tbl_get(unit, tbl_info));
exit:
    SHR_FUNC_EXIT();
}

