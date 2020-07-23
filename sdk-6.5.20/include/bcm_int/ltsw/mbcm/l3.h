/*! \file l3.h
 *
 * BCM L3 module APIs and Structures used only internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_MBCM_L3_H
#define BCMINT_LTSW_MBCM_L3_H

#include <bcm/l3.h>
#include <bcm_int/ltsw/l3.h>
#include <bcm_int/ltsw/l3_int.h>

/*!
 * \brief Initialize L3 module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_init_f)(int unit);

/*!
 * \brief De-initialize L3 module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_deinit_f)(int unit);

/*!
 * \brief Set mtu value.
 *
 * \param [in] unit Unit Number.
 * \param [in] cfg MTU Information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_mtu_set_f)(
    int unit,
    bcmi_ltsw_l3_mtu_cfg_t *cfg);

/*!
 * \brief Get mtu value.
 *
 * \param [in] unit Unit Number.
 * \param [out] cfg MTU Information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_mtu_get_f)(
    int unit,
    bcmi_ltsw_l3_mtu_cfg_t *cfg);

/*!
 * \brief Delete mtu value.
 *
 * \param [in] unit Unit Number.
 * \param [in] mtu_idx MTU entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_mtu_delete_f)(
    int unit,
    bcmi_ltsw_l3_mtu_cfg_t *cfg);

/*!
 * \brief Recover mtu database from LT.
 *
 * \param [in] unit Unit Number.
 * \param [in] mtu_idx MTU entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_mtu_recover_f)(int unit, int mtu_idx);

/*!
 * \brief Get the available L3 information.
 *
 * \param [in] unit Unit Number.
 * \param [in] l3info L3 information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_info_get_f)(int unit, bcm_l3_info_t *l3info);

/*!
 * \brief Enable/disable L3 function for a unit without clearing any L3 tables.
 *
 * \param [in] unit Unit number.
 * \param [in] 1 for enable, 0 for disable.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_enable_set_f)(int unit, int enable);

/*!
 * \brief Set L3 controlling information.
 *
 * \param [in] unit Unit Number.
 * \param [in] l3_type L3 switch control type.
 * \param [in] control Switch control.
 * \param [in] value Type-based controlling value for L3.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_control_set_f)(
    int unit,
    bcmi_ltsw_l3_control_t control,
    uint32_t value);


/*!
 * \brief Get L3 controlling information.
 *
 * \param [in] unit Unit Number.
 * \param [in] l3_type L3 switch control type.
 * \param [in] control Switch control.
 * \param [out] value Type-based controlling value for L3.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_control_get_f)(
    int unit,
    bcmi_ltsw_l3_control_t control,
    uint32_t *value);

/*!
 * \brief Set L3 port controlling information.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] control Control type.
 * \param [in] info Control infomation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_port_control_set_f)(
    int unit,
    int port,
    int control,
    bcmi_ltsw_l3_port_control_t *info);

/*!
 * \brief Get L3 port controlling information.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] control Control type.
 * \param [out] info Control infomation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_port_control_get_f)(
    int unit,
    int port,
    int control,
    bcmi_ltsw_l3_port_control_t *info);

/*!
 * \brief Set ipv6 reserved multicast address and mask.
 *
 * \param [in] unit Unit number.
 * \param [in] num Number of ipv6 address/mask pairs
 * \param [in] ip6_addr  Ipv6 address.
 * \param [in] ip6_mask Ipv6 address mask.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ipv6_reserved_multicast_addr_multi_set_f)(
    int unit,
    int num,
    bcm_ip6_t *ip6_addr,
    bcm_ip6_t *ip6_mask);

/*!
 * \brief Get ipv6 reserved multicast address and mask.
 *
 * \param [in] unit Unit number.
 * \param [in] num Number of ipv6 address/mask pairs
 * \param [out] ip6_addr  Ipv6 address.
 * \param [out] ip6_mask Ipv6 address mask.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ipv6_reserved_multicast_addr_multi_get_f)(
    int unit,
    int num,
    bcm_ip6_t *ip6_addr,
    bcm_ip6_t *ip6_mask);

/*!
 * \brief Create an IP option handling profile.
 *
 * \param [in] unit Unit number.
 * \param [in] flags BCM_L3_WITH_ID: ip4_options_profile_id argument given.
 * \param [in] default_action Default action for options in the profile.
 * \param [out] ip4_options_profile_id L3 IP options profile ID.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ip4_options_profile_create_f)(
    int unit, uint32 flags,
    bcm_l3_ip4_options_action_t default_action,
    int *ip4_options_profile_id);

/*!
 * \brief Delete a IP option handling profile.
 *
 * \param [in] unit Unit number.
 * \param [in] ip4_options_profile_id L3 IP options profile ID.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ip4_options_profile_destroy_f)(
    int unit,
    int ip4_options_profile_id);

/*!
 * \brief Update action for given IP action and Profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] ip4_options_profile_id L3 IP options profile ID.
 * \param [in] ip4_option New Action for IP Options.
 * \param [in] action L3 IP options handing actions.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ip4_options_action_set_f)(
    int unit,
    int ip4_options_profile_id,
    int ip4_option,
    bcm_l3_ip4_options_action_t action);

/*!
 * \brief Retrive action for given IP action and Profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] ip4_options_profile_id L3 IP options profile ID.
 * \param [in] ip4_option New Action for IP Options.
 * \param [out] action L3 IP options handing actions.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ip4_options_action_get_f)(
    int unit,
    int ip4_options_profile_id,
    int ip4_option,
    bcm_l3_ip4_options_action_t *action);

/*!
 * \brief Create an IPv6 prefix map into IPv4 entry.
 *
 * \param [in] unit Unit number.
 * \param [in] ip6_addr New IPv6 translation address.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ip6_prefix_map_add_f)(int unit, bcm_ip6_t ip6_addr);

/*!
 * \brief Get a list of IPv6 96 bit prefixes which are mapped to ipv4 lookup.
 *
 * \param [in] unit Unit number.
 * \param [in] map_size Size of allocated entries in ip6_array.
 * \param [out] ip6_array Array of mapped prefixes.
 * \param [out] ip6_count Number of entries of ip6_array actually filled in.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ip6_prefix_map_get_f)(
    int unit, int map_size,
    bcm_ip6_t *ip6_array, int *ip6_count);

/*!
 * \brief Destroy an IPv6 prefix map entry.
 *
 * \param [in] unit Unit number.
 * \param [in] ip6_addr IPv6 translation address.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ip6_prefix_map_del_f)(int unit, bcm_ip6_t ip6_addr);

/*!
 * \brief Flush all IPv6 prefix maps.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ip6_prefix_map_del_all_f)(int unit);

/*!
 * \brief Translate Profile ID into hardware table index used by API.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_id Profile ID.
 * \param [out] hw_idx Hardware index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ip4_options_profile_id2idx_f)(
    int unit,
    int profile_id,
    int *hw_idx);

/*!
 * \brief Translate hardware table index into Profile ID used by API.
 *
 * \param [in] unit Unit Number.
 * \param [in] hw_idx Hardware index.
 * \param [out] profile_id Profile ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ip4_options_profile_idx2id_f)(
    int unit,
    int hw_idx,
    int *profile_id);

/*!
 * \brief Get L3 switch control logical table info.
 *
 * \param [in] unit Unit Number.
 * \param [out] tbl_info Pointer to table description info.
 *
 * \retval SHR_E_UNAVAIL Logical table is not supported.
 * \retval SHR_E_NONE No errer.
 */
typedef int (*l3_switch_control_tbl_get_f)(
    int unit,
    const bcmint_ltsw_l3_switch_control_tbl_t **tbl_info);

/*!
 * \brief L3 driver structure.
 */
typedef struct mbcm_ltsw_l3_drv_s {

    /*! Init L3 module. */
    l3_init_f l3_init;

    /*! De-init L3 module. */
    l3_deinit_f l3_deinit;

    /*! Set mtu value. */
    l3_mtu_set_f l3_mtu_set;

    /*! Get mtu value. */
    l3_mtu_get_f l3_mtu_get;

    /*! Delete mtu value. */
    l3_mtu_delete_f l3_mtu_delete;

    /*! Recover mtu database from LT. */
    l3_mtu_recover_f l3_mtu_recover;

    /*! Get L3 information. */
    l3_info_get_f l3_info_get;

    /*! Enable L3. */
    l3_enable_set_f l3_enable_set;

    /*! Set L3 control. */
    l3_control_set_f l3_control_set;

    /*! Get L3 control. */
    l3_control_get_f l3_control_get;

    /*! Set L3 port control. */
    l3_port_control_set_f l3_port_control_set;

    /*! Get L3 port control. */
    l3_port_control_get_f l3_port_control_get;

    /*! Set L3 port control. */
    l3_ipv6_reserved_multicast_addr_multi_set_f l3_ipv6_reserved_multicast_addr_multi_set;

    /*! Get L3 port control. */
    l3_ipv6_reserved_multicast_addr_multi_get_f l3_ipv6_reserved_multicast_addr_multi_get;

    /*! Create IPV4 Option profile. */
    l3_ip4_options_profile_create_f l3_ip4_options_profile_create;

    /*! Destroy IPV4 Option profile. */
    l3_ip4_options_profile_destroy_f l3_ip4_options_profile_destroy;

    /*! Set IPV4 Option profile action. */
    l3_ip4_options_action_set_f l3_ip4_options_action_set;

    /*! Get IPV4 Option profile action. */
    l3_ip4_options_action_get_f l3_ip4_options_action_get;

    /*! Add IPV6 Prefix map. */
    l3_ip6_prefix_map_add_f l3_ip6_prefix_map_add;

    /*! Get IPV6 Prefix map. */
    l3_ip6_prefix_map_get_f l3_ip6_prefix_map_get;

    /*! Delete IPV6 Prefix map. */
    l3_ip6_prefix_map_del_f l3_ip6_prefix_map_del;

    /*! Delete all  IPV6 Prefix maps. */
    l3_ip6_prefix_map_del_all_f l3_ip6_prefix_map_del_all;

    /*! L3 IPV4 option idx to ID. */
    l3_ip4_options_profile_id2idx_f l3_ip4_options_profile_id2idx;

    /*! L3 IPV4 option ID to idx. */
    l3_ip4_options_profile_idx2id_f l3_ip4_options_profile_idx2id;

    /*! Get L3 switch control table information. */
    l3_switch_control_tbl_get_f l3_switch_control_tbl_get;

} mbcm_ltsw_l3_drv_t;

/*!
 * \brief Set the L3 driver of the device.
 *
 * \param [in] unit Unit Number.
 * \param [in] drv L3 driver to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_drv_set(
    int unit,
    mbcm_ltsw_l3_drv_t *drv);

/*!
 * \brief Initialize L3 module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_init(int unit);

/*!
 * \brief De-initialize L3 module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_deinit(int unit);

/*!
 * \brief Set mtu value.
 *
 * \param [in] unit Unit Number.
 * \param [in] cfg MTU Information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_mtu_set(
    int unit,
    bcmi_ltsw_l3_mtu_cfg_t *cfg);

/*!
 * \brief Get mtu value.
 *
 * \param [in] unit Unit Number.
 * \param [out] cfg MTU Information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_mtu_get(
    int unit,
    bcmi_ltsw_l3_mtu_cfg_t *cfg);

/*!
 * \brief Delete mtu value.
 *
 * \param [in] unit Unit Number.
 * \param [in] mtu_idx MTU entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_mtu_delete(
    int unit,
    bcmi_ltsw_l3_mtu_cfg_t *cfg);

/*!
 * \brief Recover mtu database from LT.
 *
 * \param [in] unit Unit Number.
 * \param [in] mtu_idx MTU entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_mtu_recover(int unit, int mtu_idx);

/*!
 * \brief Get the available L3 information.
 *
 * \param [in] unit Unit Number.
 * \param [in] l3info L3 information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_info_get(int unit, bcm_l3_info_t *l3info);

/*!
 * \brief Enable/disable L3 function for a unit without clearing any L3 tables
 *
 * \param [in] unit Unit number.
 * \param [in] 1 for enable, 0 for disable.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_enable_set(int unit, int enable);

/*!
 * \brief Set L3 controlling information.
 *
 * \param [in] unit Unit Number.
 * \param [in] control Switch control.
 * \param [in] value Type-based controlling value for L3.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_control_set(
    int unit,
    bcmi_ltsw_l3_control_t control,
    uint32_t value);

/*!
 * \brief Get L3 controlling information.
 *
 * \param [in] unit Unit Number.
 * \param [in] control Switch control.
 * \param [out] value Type-based controlling value for L3.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_control_get(
    int unit,
    bcmi_ltsw_l3_control_t control,
    uint32_t *value);

/*!
 * \brief Set ipv6 reserved multicast address and mask.
 *
 * \param [in] unit Unit number.
 * \param [in] num Number of ipv6 address/mask pairs
 * \param [in] ip6_addr  Ipv6 address.
 * \param [in] ip6_mask Ipv6 address mask.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ipv6_reserved_multicast_addr_multi_set(
    int unit,
    int num,
    bcm_ip6_t *ip6_addr,
    bcm_ip6_t *ip6_mask);

/*!
 * \brief Get ipv6 reserved multicast address and mask.
 *
 * \param [in] unit Unit number.
 * \param [in] num Number of ipv6 address/mask pairs
 * \param [out] ip6_addr  Ipv6 address.
 * \param [out]] ip6_mask Ipv6 address mask.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ipv6_reserved_multicast_addr_multi_get(
    int unit,
    int num,
    bcm_ip6_t *ip6_addr,
    bcm_ip6_t *ip6_mask);

/*!
 * \brief Set L3 port controlling information.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] control Control type.
 * \param [in] info Control infomation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_port_control_set(
    int unit,
    int port,
    int control,
    bcmi_ltsw_l3_port_control_t *info);

/*!
 * \brief Get L3 port controlling information.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] control Control type.
 * \param [out] info Control infomation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */

extern int
mbcm_ltsw_l3_port_control_get(
    int unit,
    int port,
    int control,
    bcmi_ltsw_l3_port_control_t *info);

/*!
 * \brief Create an IP option handling profile.
 *
 * \param [in] unit Unit number.
 * \param [in] flags BCM_L3_WITH_ID: ip4_options_profile_id argument given.
 * \param [in] default_action Default action for options in the profile.
 * \param [out] ip4_options_profile_id L3 IP options profile ID.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ip4_options_profile_create(
    int unit, uint32 flags,
    bcm_l3_ip4_options_action_t default_action,
    int *ip4_options_profile_id);

/*!
 * \brief Delete a IP option handling profile.
 *
 * \param [in] unit Unit number.
 * \param [in] ip4_options_profile_id L3 IP options profile ID.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ip4_options_profile_destroy(
    int unit,
    int ip4_options_profile_id);

/*!
 * \brief Update action for given IP action and Profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] ip4_options_profile_id L3 IP options profile ID.
 * \param [in] ip4_option New Action for IP Options.
 * \param [in] action L3 IP options handing actions.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ip4_options_action_set(
    int unit,
    int ip4_options_profile_id,
    int ip4_option,
    bcm_l3_ip4_options_action_t action);

/*!
 * \brief Retrive action for given IP action and Profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] ip4_options_profile_id L3 IP options profile ID.
 * \param [in] ip4_option New Action for IP Options.
 * \param [out] action L3 IP options handing actions.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ip4_options_action_get(
    int unit,
    int ip4_options_profile_id,
    int ip4_option,
    bcm_l3_ip4_options_action_t *action);

/*!
 * \brief Create an IPv6 prefix map into IPv4 entry.
 *
 * \param [in] unit Unit number.
 * \param [in] ip6_addr New IPv6 translation address.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ip6_prefix_map_add(int unit, bcm_ip6_t ip6_addr);

/*!
 * \brief Get a list of IPv6 96 bit prefixes which are mapped to ipv4 lookup.
 *
 * \param [in] unit Unit number.
 * \param [in] map_size Size of allocated entries in ip6_array.
 * \param [out] ip6_array Array of mapped prefixes.
 * \param [out] ip6_count Number of entries of ip6_array actually filled in.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ip6_prefix_map_get(
    int unit, int map_size,
    bcm_ip6_t *ip6_array, int *ip6_count);

/*!
 * \brief Destroy an IPv6 prefix map entry.
 *
 * \param [in] unit Unit number.
 * \param [in] ip6_addr IPv6 translation address.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ip6_prefix_map_del(int unit, bcm_ip6_t ip6_addr);

/*!
 * \brief Flush all IPv6 prefix maps.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ip6_prefix_map_del_all(int unit);

/*!
 * \brief Translate Profile ID into hardware table index used by API.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_id Profile ID.
 * \param [out] hw_idx Hardware index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ip4_options_profile_id2idx(
    int unit,
    int profile_id,
    int *hw_idx);

/*!
 * \brief Translate hardware table index into Profile ID used by API.
 *
 * \param [in] unit Unit Number.
 * \param [in] hw_idx Hardware index.
 * \param [out] profile_id Profile ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ip4_options_profile_idx2id(
    int unit,
    int hw_idx,
    int *profile_id);

/*!
 * \brief Get L3 switch control logical table info.
 *
 * \param [in] unit Unit Number.
 * \param [out] tbl_info Pointer to table description info.
 *
 * \retval SHR_E_UNAVAIL Logical table is not supported.
 * \retval SHR_E_NONE No errer.
 */
extern int
mbcm_ltsw_l3_switch_control_tbl_get(
    int unit,
    const bcmint_ltsw_l3_switch_control_tbl_t **tbl_info);

#endif /* BCMINT_LTSW_MBCM_L3_H */
