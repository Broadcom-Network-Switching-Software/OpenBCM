/*! \file l3.c
 *
 * L3 management.
 * This file contains the management for L3.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <bcm/l3.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/l2.h>
#include <bcm_int/ltsw/l3.h>
#include <bcm_int/ltsw/l3_egress.h>
#include <bcm_int/ltsw/l3_fib.h>
#include <bcm_int/ltsw/l3_intf.h>
#include <bcm_int/ltsw/l3_aacl.h>
#include <bcm_int/ltsw/tunnel.h>
#include <bcm_int/ltsw/nat.h>
#include <bcm_int/ltsw/mbcm/l3.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/feature.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

/* L3 module initialized. */
static bool l3_inited[BCM_MAX_NUM_UNITS] = { false };

/* L3 mbcm driver initialized. */
static bool l3_drv_inited[BCM_MAX_NUM_UNITS] = { false };

/******************************************************************************
 * Private functions
 */

static int
l3_drv_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (l3_drv_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_l3_deinit(unit));
        l3_drv_inited[unit] = false;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
l3_drv_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (l3_drv_inited[unit]) {
        l3_drv_deinit(unit);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_init(unit));

    l3_drv_inited[unit] = true;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_l3_switch_control_tbl_get(
    int unit,
    const bcmint_ltsw_l3_switch_control_tbl_t **tbl_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_switch_control_tbl_get(unit, tbl_info));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Internal HSDK functions
 */

int
bcmi_ltsw_l3_mtu_set(int unit, bcmi_ltsw_l3_mtu_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_drv_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_mtu_set(unit, cfg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_mtu_get(int unit, bcmi_ltsw_l3_mtu_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_drv_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_mtu_get(unit, cfg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_mtu_delete(int unit, bcmi_ltsw_l3_mtu_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_drv_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_mtu_delete(unit, cfg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_mtu_recover(int unit, int mtu_idx)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_drv_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_mtu_recover(unit, mtu_idx));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_control_set(
    int unit,
    bcmi_ltsw_l3_control_t control,
    uint32_t value)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_control_set(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_control_get(
    int unit,
    bcmi_ltsw_l3_control_t control,
    uint32_t *value)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_control_get(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_port_control_set(
    int unit,
    int port,
    int control,
    bcmi_ltsw_l3_port_control_t *info)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_port_control_set(unit, port, control, info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_port_control_get(
    int unit,
    int port,
    int control,
    bcmi_ltsw_l3_port_control_t *info)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_port_control_get(unit, port, control, info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ip4_options_profile_id2idx(
    int unit,
    int profile_id,
    int *hw_idx)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(hw_idx, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ip4_options_profile_id2idx(unit, profile_id, hw_idx));

exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_l3_ip4_options_profile_idx2id(
    int unit,
    int hw_idx,
    int *profile_id)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(profile_id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ip4_options_profile_idx2id(unit, hw_idx, profile_id));

exit:
    SHR_FUNC_EXIT();

}


/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Initialize the L3 subsystem.
 *
 * This function has to be called before any other L3 functions.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_L3)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (!bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ENABLE, 1)) {
        SHR_ERR_EXIT(SHR_E_DISABLED);
    }

    if (l3_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_l3_cleanup(unit));
    }

    /* Initialize L3 mbcm driver. */
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_drv_init(unit));

    /* Egress/ingress L3 interface table init. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_intf_init(unit));

    /* Initialize L3 Egress Object manager. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_init(unit));

    /* IP tunnel init. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_init(unit));

    /* L3 fib init. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_fib_init(unit));

    if (ltsw_feature(unit, LTSW_FT_NAT)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_nat_init(unit));
    }
    if (ltsw_feature(unit, LTSW_FT_L3_AACL)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_aacl_init(unit));
    }

    l3_inited[unit] = true;

exit:
    if (SHR_FUNC_ERR()) {
       (void)bcm_ltsw_l3_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize L3 subsystem.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Detach successfully.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    l3_inited[unit] = false;

    if (ltsw_feature(unit, LTSW_FT_L3_AACL)) {
        SHR_IF_ERR_CONT
            (bcmi_ltsw_l3_aacl_deinit(unit));
    }
    if (ltsw_feature(unit, LTSW_FT_NAT)) {
        SHR_IF_ERR_CONT
            (bcmi_ltsw_nat_deinit(unit));
    }

    SHR_IF_ERR_CONT
        (bcmi_ltsw_l3_fib_deinit(unit));

    SHR_IF_ERR_CONT
        (bcmi_ltsw_tunnel_deinit(unit));

    SHR_IF_ERR_CONT
        (bcmi_ltsw_l3_egress_deinit(unit));

    SHR_IF_ERR_CONT
        (bcmi_ltsw_l3_intf_deinit(unit));

    SHR_IF_ERR_CONT
        (l3_drv_deinit(unit));

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the L3 information
 *
 * \param [in] unit Unit number.
 * \param [out] l3info Available L3 information.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_info(int unit, bcm_l3_info_t *l3info)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_drv_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(l3info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_info_get(unit, l3info));

    if (!ltsw_feature(unit, LTSW_FT_NO_HOST)) {
        /* Host */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                             BCMI_LTSW_L3_FIB_OBJ_HOST_MAX,
                                             BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                             &l3info->l3info_max_host));
        l3info->l3info_max_l3 = l3info->l3info_max_host;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                             BCMI_LTSW_L3_FIB_OBJ_HOST_USED,
                                             BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                             &l3info->l3info_used_host));
        l3info->l3info_used_l3 = l3info->l3info_used_host;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                             BCMI_LTSW_L3_FIB_OBJ_HOST_V4_USED,
                                             BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                             &l3info->l3info_used_host_ip4));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                             BCMI_LTSW_L3_FIB_OBJ_HOST_V6_USED,
                                             BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                             &l3info->l3info_used_host_ip6));
    }

    /* Route */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_MAX,
                                         BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                         &l3info->l3info_max_route));
    l3info->l3info_max_defip = l3info->l3info_max_route;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_USED,
                                         BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                         &l3info->l3info_used_route));
    l3info->l3info_used_defip = l3info->l3info_used_route;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/disable L3 function for a unit without clearing any L3 tables
 *
 * \param [in] unit Unit number.
 * \param [in] 1 for enable, 0 for disable.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_enable_set(int unit, int enable)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_drv_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_enable_set(unit, enable));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Add VRID for the given VLAN/VSI.
 *
 * \param [in] unit Unit number.
 * \param [in] vlan VLAN/VSI.
 * \param [in] vrid Virtual router ID to be added.
 *
 * Adding a VRID using this API means the physical node has become the
 * master for the virtual router.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_vrrp_add(int unit, bcm_vlan_t vlan, uint32_t vrid)
{
    bcm_l2_addr_t l2addr;
    bcm_mac_t mac = {0x00, 0x00, 0x5e, 0x00, 0x01, 0x00};

    SHR_FUNC_ENTER(unit);

    if (!BCM_VLAN_VALID(vlan) || (vrid > 0xff) || (vrid == 0)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    mac[5] = vrid;

    /* Set l2 address info. */
    bcm_l2_addr_t_init(&l2addr, mac, vlan);

    /* Set l2 entry flags. */
    l2addr.flags = BCM_L2_L3LOOKUP | BCM_L2_STATIC | BCM_L2_REPLACE_DYNAMIC;

    /* Set l2 entry port to CPU port. */
    l2addr.port = bcmi_ltsw_port_cpu(unit);

    /* Set l2 entry module id to local module. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stk_my_modid_get(unit, &l2addr.modid));

    /* Overwrite existing entry if any. */
    (void)bcm_ltsw_l2_addr_delete(unit, mac, vlan);

    /* Add entry to l2 table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_l2_addr_add(unit, &l2addr));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete VRID for a particular VLAN/VSI.
 *
 * \param [in] unit Unit number.
 * \param [in] vlan VLAN/VSI.
 * \param [in] vrid Virtual router ID to be added.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_vrrp_delete(int unit, bcm_vlan_t vlan, uint32_t vrid)
{
    bcm_mac_t mac = {0x00, 0x00, 0x5e, 0x00, 0x01, 0x00};

    SHR_FUNC_ENTER(unit);

    if (!BCM_VLAN_VALID(vlan) || (vrid > 0xff)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    mac[5] = vrid;

    /* Delete existing entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_l2_addr_delete(unit, mac, vlan));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all VRIDs for a particular VLAN/VSI.
 *
 * \param [in] unit Unit number.
 * \param [in] vlan VLAN/VSI.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_vrrp_delete_all(int unit, bcm_vlan_t vlan)
{
    bcm_mac_t mac = {0x00, 0x00, 0x5e, 0x00, 0x01, 0x00};
    int vrid;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (!BCM_VLAN_VALID(vlan)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* RFC3768 Valid VRID range 1-255. */
    for (vrid = 1; vrid <= 255; vrid ++) {
        mac[5] = vrid;

        rv = bcm_ltsw_l2_addr_delete(unit, mac, vlan);
        if (rv != SHR_E_NOT_FOUND) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get all VRIDs for a particular VLAN/VSI.
 *
 * \param [in] unit Unit number.
 * \param [in] vlan VLAN/VSI.
 * \param [in] alloc_size Size of vrid_array.
 * \param [out] vrid_array Pointer to the array of VRIDs
 * \prarm [out] count Number of VRIDs copied.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_vrrp_get(int unit, bcm_vlan_t vlan, int alloc_size,
                     int *vrid_array, int *count)
{
    bcm_l2_addr_t l2addr;
    bcm_mac_t mac = {0x00, 0x00, 0x5e, 0x00, 0x01, 0x00};
    int vrid, cnt, rv;

    SHR_FUNC_ENTER(unit);

    if (!BCM_VLAN_VALID(vlan) || !vrid_array || !count) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    cnt = 0;
    /* RFC3768 Valid VRID range 1-255. */
    for (vrid = 1; vrid <= 255; vrid ++) {
        if (alloc_size > cnt) {
            mac[5] = vrid;

            /* Set l2 address info. */
            bcm_l2_addr_t_init(&l2addr, mac, vlan);

            rv = bcm_ltsw_l2_addr_get(unit, mac, vlan, &l2addr);
            if (SHR_SUCCESS(rv)) {
                vrid_array[cnt] = vrid;
                cnt++;
            }
        }
    }
    *count = cnt;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Apply IPv6 mask on address.
 *
 * \param [in]     mask       IPv6 mask.
 * \param [in/out] addr       Address to apply the mask.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_l3_mask6_apply(bcm_ip6_t mask, bcm_ip6_t addr)
{
    /* Mask bytes iterator. */
    uint8_t *mask_iter;
    /* Mask bytes iterator. */
    uint8_t *addr_iter;
    /* IPv6 address iterator. */
    int idx;

    if ((mask == NULL) || (addr == NULL)) {
        return SHR_E_PARAM;
    }

    mask_iter = (uint8_t *)mask;
    addr_iter = (uint8_t *)addr;

    for (idx = 0; idx < 16; idx++) {
        (*addr_iter) &= (*mask_iter);
        addr_iter++;
        mask_iter++;
    }

    return SHR_E_NONE;
}
int
bcm_ltsw_l3_ip4_options_profile_create(int unit, uint32_t flags,
                                       bcm_l3_ip4_options_action_t default_action,
                                       int *ip4_options_profile_id)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (ip4_options_profile_id == NULL) {
        return BCM_E_PARAM;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ip4_options_profile_create(unit, flags, default_action, ip4_options_profile_id));
exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_l3_ip4_options_profile_destroy(int unit,
                                        int ip4_options_profile_id)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ip4_options_profile_destroy(unit, ip4_options_profile_id));
exit:
    SHR_FUNC_EXIT();
}



int
bcm_ltsw_l3_ip4_options_action_set(int unit,
                                   int ip4_options_profile_id,
                                   int ip4_option,
                                   bcm_l3_ip4_options_action_t action)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ip4_options_action_set(unit, ip4_options_profile_id, ip4_option, action));
exit:
    SHR_FUNC_EXIT();
}



int
bcm_ltsw_l3_ip4_options_action_get(int unit,
                                   int ip4_options_profile_id,
                                   int ip4_option,
                                   bcm_l3_ip4_options_action_t *action)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (action == NULL) {
        return BCM_E_PARAM;
    }

    SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_l3_ip4_options_action_get(unit, ip4_options_profile_id, ip4_option, action));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l3_ip6_prefix_map_get(int unit, int map_size,
                               bcm_ip6_t *ip6_array, int *ip6_count)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if ((ip6_array == NULL) || (ip6_count == NULL)) {
        return BCM_E_PARAM;
    }

    SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_l3_ip6_prefix_map_get(unit, map_size, ip6_array, ip6_count));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l3_ip6_prefix_map_add(int unit, bcm_ip6_t ip6_addr)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_l3_ip6_prefix_map_add(unit, ip6_addr));
exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_l3_ip6_prefix_map_delete(int unit, bcm_ip6_t ip6_addr)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_l3_ip6_prefix_map_del(unit, ip6_addr));
exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_l3_ip6_prefix_map_delete_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!l3_inited[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_l3_ip6_prefix_map_del_all(unit));
exit:
    SHR_FUNC_EXIT();
}
