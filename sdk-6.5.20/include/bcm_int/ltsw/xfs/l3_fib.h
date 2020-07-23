/*! \file l3_fib.h
 *
 * XFS L3 fib module APIs and data structures used only internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_XFS_L3_FIB_H
#define BCMI_LTSW_XFS_L3_FIB_H

#include <bcm/l3.h>
#include <bcm_int/ltsw/l3_fib.h>
#include <bcm_int/ltsw/flexctr.h>

/*!
 * \brief Initialize L3 fib module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_fib_init(int unit);

/*!
 * \brief De-Initialize L3 fib module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_fib_deinit(int unit);

/*!
 * \brief Get the max vrf number.
 *
 * \param [in] unit Unit number.
 * \param [out] max_vrf Max vrf.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_fib_vrf_max_get(int unit, uint32_t *max_vrf);

/*!
 * \brief Add an entry to the L3 host table for IPv4 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv4_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv4 unicast from L3 host table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv4_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete entries of IPv4 unicast based on IP prefix.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv4_uc_del_by_prefix(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 host entry for IPv4 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv4_uc_find(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Traverse IPv4 unicast host table.
 *
 * \param [in] unit Unit number.
 * \param [in] flags Operation flags.
 * \param [in] start Callback called after this many L3 entries.
 * \param [in] end Callback called before this many L3 entries.
 * \param [in] cb User supplied callback function.
 * \param [in] user_data User supplied cookie used in callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv4_uc_traverse(int unit, uint32_t flags,
                                  uint32_t start, uint32_t end,
                                  bcm_l3_host_traverse_cb cb, void *user_data);

/*!
 * \brief Add an entry to the L3 host table for IPv4 multicast.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv4_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv4 multicast from L3 host table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv4_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 host entry for IPv4 multicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv4_mc_find(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Traverse IPv4 multicast host table.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib Operation attributes.
 * \param [in] trav_fn User supplied callback function.
 * \param [in] cb Callback function to contruct ipmc info from fib.
 * \param [in] user_data User supplied cookie used in callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv4_mc_traverse(int unit, uint32_t attrib,
                                  bcm_ipmc_traverse_cb trav_fn,
                                  bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                                  void *user_data);

/*
 * \brief Attach flexctr action to the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv4_mc_flexctr_attach(int unit,
                                        bcmi_ltsw_l3_fib_t *fib,
                                        bcmi_ltsw_flexctr_counter_info_t *ci);

/*!
 * \brief Detach flexctr action of the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv4_mc_flexctr_detach(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Get flex counter information of the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_host_ipv4_mc_flexctr_info_get(int unit,
                                          bcmi_ltsw_l3_fib_t *fib,
                                          bcmi_ltsw_flexctr_counter_info_t *ci);

/*!
 * \brief Add an entry to the L3 host table for IPv6 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv6_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv6 unicast from L3 host table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv6_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete entries of IPv6 unicast based on IP prefix.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv6_uc_del_by_prefix(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 host entry for IPv6 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv6_uc_find(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Traverse IPv6 unicast host table.
 *
 * \param [in] unit Unit number.
 * \param [in] flags Operation flags.
 * \param [in] start Callback called after this many L3 entries.
 * \param [in] end Callback called before this many L3 entries.
 * \param [in] cb User supplied callback function.
 * \param [in] user_data User supplied cookie used in callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv6_uc_traverse(int unit, uint32_t flags,
                                  uint32_t start, uint32_t end,
                                  bcm_l3_host_traverse_cb cb, void *user_data);

/*!
 * \brief Add an entry to the L3 host table for IPv6 multicast.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv6_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv6 multicast from L3 host table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv6_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 host entry for IPv6 multiicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv6_mc_find(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Traverse IPv6 multicast host table.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib Operation attributes.
 * \param [in] trav_fn User supplied callback function.
 * \param [in] cb Callback function to contruct ipmc info from fib.
 * \param [in] user_data User supplied cookie used in callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv6_mc_traverse(int unit, uint32_t attrib,
                                  bcm_ipmc_traverse_cb trav_fn,
                                  bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                                  void *user_data);

/*
 * \brief Attach flexctr action to the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv6_mc_flexctr_attach(int unit,
                                        bcmi_ltsw_l3_fib_t *fib,
                                        bcmi_ltsw_flexctr_counter_info_t *ci);

/*!
 * \brief Detach flexctr action of the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_ipv6_mc_flexctr_detach(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Get flex counter information of the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_host_ipv6_mc_flexctr_info_get(int unit,
                                       bcmi_ltsw_l3_fib_t *fib,
                                       bcmi_ltsw_flexctr_counter_info_t *ci);

/*!
 * \brief Delete entries that match or not match a specified L3 interface.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 * \param [in] negate Match L3 interface if negate is 0, otherwise not match.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_del_by_intf(int unit, bcmi_ltsw_l3_fib_t *fib, int negate);

/*!
 * \brief Delete all L3 host entries.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib L3 fib operation attributes.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_host_del_all(int unit, uint32_t attrib);

/*!
 * \brief Add an entry to the L3 route table for IPv4 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv4_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv4 unicast from L3 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv4_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 route entry for IPv4 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv4_uc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Traverse IPv4 unicast route table.
 *
 * \param [in] unit Unit number.
 * \param [in] flags Operation flags.
 * \param [in] start Callback called after this many L3 entries.
 * \param [in] end Callback called before this many L3 entries.
 * \param [in] cb User supplied callback function.
 * \param [in] user_data User supplied cookie used in callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv4_uc_traverse(int unit, uint32_t flags,
                                   uint32_t start, uint32_t end,
                                   bcm_l3_route_traverse_cb cb,
                                   void *user_data);

/*!
 * \brief Add an entry to the L3 route table for IPv4 multicast.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv4_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv4 multicast from L3 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv4_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 route entry for IPv4 multicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv4_mc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Traverse IPv4 multicast route table.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib Operation attributes.
 * \param [in] trav_fn User supplied callback function.
 * \param [in] cb Callback function to contruct ipmc info from fib.
 * \param [in] user_data User supplied cookie used in callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv4_mc_traverse(int unit, uint32_t attrib,
                                   bcm_ipmc_traverse_cb trav_fn,
                                   bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                                   void *user_data);

/*
 * \brief Attach flexctr action to the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv4_mc_flexctr_attach(int unit,
                                         bcmi_ltsw_l3_fib_t *fib,
                                         bcmi_ltsw_flexctr_counter_info_t *ci);

/*!
 * \brief Detach flexctr action of the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv4_mc_flexctr_detach(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Get flex counter information of the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_route_ipv4_mc_flexctr_info_get(int unit,
                                        bcmi_ltsw_l3_fib_t *fib,
                                        bcmi_ltsw_flexctr_counter_info_t *ci);

/*!
 * \brief Add an entry to the L3 route table for IPv6 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv6_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv6 unicast from L3 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv6_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 route entry for IPv6 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv6_uc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Traverse IPv6 unicast route table.
 *
 * \param [in] unit Unit number.
 * \param [in] flags Operation flags.
 * \param [in] start Callback called after this many L3 entries.
 * \param [in] end Callback called before this many L3 entries.
 * \param [in] cb User supplied callback function.
 * \param [in] user_data User supplied cookie used in callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv6_uc_traverse(int unit, uint32_t flags,
                                   uint32_t start, uint32_t end,
                                   bcm_l3_route_traverse_cb cb,
                                   void *user_data);

/*!
 * \brief Add an entry to the L3 route table for IPv6 multicast.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv6_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv6 multicast from L3 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv6_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 route entry for IPv6 multiicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv6_mc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Traverse IPv6 multicast route table.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib Operation attributes.
 * \param [in] trav_fn User supplied callback function.
 * \param [in] cb Callback function to contruct ipmc info from fib.
 * \param [in] user_data User supplied cookie used in callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv6_mc_traverse(int unit, uint32_t attrib,
                                   bcm_ipmc_traverse_cb trav_fn,
                                   bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                                   void *user_data);
/*
 * \brief Attach flexctr action to the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv6_mc_flexctr_attach(int unit,
                                         bcmi_ltsw_l3_fib_t *fib,
                                         bcmi_ltsw_flexctr_counter_info_t *ci);

/*!
 * \brief Detach flexctr action of the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_ipv6_mc_flexctr_detach(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Get flex counter information of the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_route_ipv6_mc_flexctr_info_get(int unit,
                                        bcmi_ltsw_l3_fib_t *fib,
                                        bcmi_ltsw_flexctr_counter_info_t *ci);

/*!
 * \brief Delete entries that match or not match a specified L3 interface.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 * \param [in] negate Match L3 interface if negate is 0, otherwise not match.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_del_by_intf(int unit, bcmi_ltsw_l3_fib_t *fib, int negate);

/*!
 * \brief Delete all L3 route entries.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib L3 fib operation attributes.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_route_del_all(int unit, uint32_t attrib);

/*!
 * \brief Attach flexctr action to the given VRF.
 *
 * \param [in] unit Unit number.
 * \param [in] vrf Virtual Router Instance.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_vrf_flexctr_attach(
        int unit,
        bcm_vrf_t vrf,
        bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach flexctr action of the given VRF.
 *
 * \param [in] unit Unit number.
 * \param [in] vrf Virtual Router Instance.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_vrf_flexctr_detach(
        int unit,
        bcm_vrf_t vrf);

/*!
 * \brief Get flex counter information of the given VRF.
 *
 * \param [in] unit Unit number.
 * \param [in] vrf Virtual Router Instance.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_vrf_flexctr_info_get(
        int unit,
        bcm_vrf_t vrf,
        bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Get IPv4 or IPv6 route count of the given VRF.
 *
 * \param [in] unit Unit number.
 * \param [in] vrf Virtual Router Instance.
 * \param [in] ipv6 Indicate ipv6 or ipv4.
 * \param [out] cnt  Route count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_vrf_route_cnt_get(int unit, bcm_vrf_t vrf, bool ipv6, uint32_t *cnt);

/*!
 * \brief Traverse L3 NAT host entries.
 *
 * \param [in] unit Unit number.
 * \param [in] flags Flags to match during traverse.
 * \param [in] trav_fn User supplied callback function.
 * \param [in] cb Callback function to construct bcm_l3_nat_ingress_t from fib.
 * \param [in] user_data User supplied cookie used in parameter in
 *                       callback function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_nat_host_traverse(int unit, uint32_t flags,
                              bcm_l3_nat_ingress_traverse_cb trav_fn,
                              bcmi_ltsw_l3_fib_to_nat_cb cb,
                              void *user_data);

/*!
 * \brief Enable dropping L3 unicast packets if L3_IIF equals to L3_EIF.
 *
 * \param [in] unit Unit number.
 * \param [in] enable 1: Drop packets, 0: Do not drop packets.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_same_intf_drop_set(int unit, int enable);

/*!
 * \brief Get the drop setting for L3 unicast packets if L3_IIF equals to L3_EIF.
 *
 * \param [in] unit Unit number.
 * \param [out] enable 1: Drop packets, 0: Do not drop packets
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_same_intf_drop_get(int unit, int *enable);

/*!
 * \brief Clear hit bits for all L3 route entries if set.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_route_hit_clear(
    int unit);

/*!
 * \brief Clear hit bits for all host entries if set.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_host_hit_clear(
    int unit);

/*!
 * \brief Age on IPMC table, clear HIT bit of entry if set.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib Operation attributes.
 * \param [in] age_cb User supplied callback function.
 * \param [in] cb Callback function to construct bcm_ipmc_addr_t from fib info.
 * \param [in] user_data  User provided cookie for callback.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ipmc_age(
    int unit,
    uint32_t attrib,
    bcm_ipmc_traverse_cb age_cb,
    bcmi_ltsw_l3_fib_to_ipmc_cb cb,
    void *user_data);

/*!
 * \brief Adds a L3 AACL to the compression table.
 *
 * \param [in] unit Unit Number.
 * \param [in] options L3 AACL options. See BCM_L3_AACL_OPTIONS_xxx.
 * \param [in] aacl Pointer to bcm_l3_aacl_t struct which contains fields related to compression table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_aacl_add(
    int unit,
    uint32_t options,
    bcm_l3_aacl_t *aacl);

/*!
 * \brief Deletes a L3 AACL from the compression table.
 *
 * \param [in] unit Unit Number.
 * \param [in] aacl Pointer to bcm_l3_aacl_t struct which contains fields related to compression table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_aacl_delete(
    int unit,
    bcm_l3_aacl_t *aacl);

/*!
 * \brief Deletes all AACLs.
 *
 * \param [in] unit Unit Number.
 * \param [in] aacl Pointer to bcm_l3_aacl_t struct which contains fields related to compression table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_aacl_delete_all(
    int unit,
    bcm_l3_aacl_t *aacl);

/*!
 * \brief Finds information from the AACL table.
 *
 * \param [in] unit Unit Number.
 * \param [inout] aacl Pointer to bcm_l3_aacl_t struct which contains fields related to compression table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_aacl_find(
    int unit,
    bcm_l3_aacl_t *aacl);

/*!
 * \brief Traverse through the AACL table and run callback at each entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] trav_fn Traverse call back function.
 * \param [in] user_data User data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_aacl_traverse(
    int unit,
    bcm_l3_aacl_traverse_cb trav_fn,
    void *user_data);

/*!
 * \brief Traverse through the AACL table and run callback at each entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] flags L3 AACL flags. See BCM_L3_AACL_FLAGS_xxx.
 * \param [in] trav_fn Traverse call back function.
 * \param [in] user_data User data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_aacl_matched_traverse(
    int unit,
    uint32_t flags,
    bcm_l3_aacl_traverse_cb trav_fn,
    void *user_data);

/*!
 * \brief Get MC defip ipv6 64 & 128 enable status.
 *
 * \param [in] unit Unit Number.
 * \param [out] ipv6 64 enable status.
 * \param [out] ipv6 128 enable status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
xfs_ltsw_l3_mc_defip_get(int unit, int *v6_64_en, int *v6_128_en);

/*!
 * \brief Get maximum ECMP group could be used in a reduced ALPM entry.
 *
 * \param [in] unit Unit Number.
 *
 * \retval Maximum ECMP group could be used in a reduced ALPM entry.
 * \retval !SHR_E_NONE Failure.
 */
extern uint32_t
xfs_ltsw_l3_reduced_route_max_ecmp_group_get(int unit);

/*!
 * \brief Performance LT operration.
 *
 * \param [in] unit Unit number.
 * \param [in] op Operation.
 * \param [in] vrf VRF.
 * \param [in] ip IP address.
 * \param [in] ip_mask  IP address mask.
 * \param [in] intf  Egress object.
 * \param [in] alpm_dmode  alpm data mode array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xfs_ltsw_l3_route_perf_v4_lt_op(
    int unit,
    bcmi_ltsw_l3_route_perf_opcode_t op,
    int vrf,
    uint32 ip,
    uint32 ip_mask,
    int intf,
    int (*alpm_dmode)[2]);

/*!
 * \brief Performance LT operration.
 *
 * \param [in] unit Unit number.
 * \param [in] op Operation.
 * \param [in] vrf VRF.
 * \param [in] v6 IPv6 address.
 * \param [in] v6_mask  IPv6 address mask.
 * \param [in] intf  Egress object.
 * \param [in] alpm_dmode  alpm data mode array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xfs_ltsw_l3_route_perf_v6_lt_op(
    int unit,
    bcmi_ltsw_l3_route_perf_opcode_t op,
    int vrf,
    uint64_t *v6,
    uint64_t *v6_mask,
    int intf,
    int (*alpm_dmode)[2]);

#endif /* BCMI_LTSW_XFS_L3_FIB_H */
