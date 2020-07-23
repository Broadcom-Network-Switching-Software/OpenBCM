/*! \file l3_fib.h
 *
 * BCM L3 fib module APIs and data structures used only internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_MBCM_L3_FIB_H
#define BCMI_LTSW_MBCM_L3_FIB_H

#include <bcm/l3.h>
#include <bcm_int/ltsw/l3_fib.h>
#include <bcm_int/ltsw/l3_fib_int.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/uft.h>

/*!
 * \brief Initialize L3 fib module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_fib_init_f)(int unit);

/*!
 * \brief De-Initialize L3 fib module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_fib_deinit_f)(int unit);

/*!
 * \brief Get the max vrf number.
 *
 * \param [in] unit Unit number.
 * \param [out] max_vrf Max vrf.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_fib_vrf_max_get_f)(int unit, uint32_t *max_vrf);

/*!
 * \brief Add an entry to the L3 host table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_host_add_f)(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry from L3 host table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_host_delete_f)(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 host entry.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_host_find_f)(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete entries that match or not match a specified L3 interface.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 * \param [in] negate Match L3 interface if negate is 0, otherwise not match.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_host_del_by_intf_f)(int unit, bcmi_ltsw_l3_fib_t *fib,
                                     int negate);

/*!
 * \brief Delete all L3 host entries.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib L3 FIB attributes.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_host_del_all_f)(int unit, uint32_t attrib);

/*!
 * \brief Traverse host table.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib Operation attributes.
 * \param [in] trav_fn User supplied callback function.
 * \param [in] cb Callback function to contruct ipmc info from fib.
 * \param [in] user_data User supplied cookie used in callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_host_traverse_f)(int unit, uint32_t flags,
                                  uint32_t start, uint32_t end,
                                  bcm_l3_host_traverse_cb cb, void *user_data);

/*!
 * \brief Add an entry to the L3 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_route_add_f)(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry from L3 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_route_delete_f)(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 route entry.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_route_find_f)(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete entries that match or not match a specified L3 interface.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 * \param [in] negate Match L3 interface if negate is 0, otherwise not match.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_route_del_by_intf_f)(int unit, bcmi_ltsw_l3_fib_t *fib,
                                      int negate);

/*!
 * \brief Delete all L3 route entries.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib L3 FIB attributes.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_route_del_all_f)(int unit, uint32_t attrib);

/*!
 * \brief Traverse unicast route table.
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
typedef int (*l3_route_traverse_f)(int unit, uint32_t flags,
                                   uint32_t start, uint32_t end,
                                   bcm_l3_route_traverse_cb cb,
                                   void *user_data);

/*!
 * \brief Traverse multicast route table.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib Operation attributes.
 * \param [in] trav_fn User supplied callback function.
 * \param [in] cb Callback function to contruct ipmc info from fib.
 * \param [in] user_data User supplied cookie used in callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_ipmc_traverse_f)(int unit, uint32_t attrib,
                                  bcm_ipmc_traverse_cb trav_fn,
                                  bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                                  void *user_data);

/*!
 * \brief Attach flexctr action to the given VRF.
 *
 * \param [in] unit Unit number.
 * \param [in] vrf Virtual Router Instance.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_vrf_flexctr_attach_f)(
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
typedef int (*l3_vrf_flexctr_detach_f)(
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
typedef int (*l3_vrf_flexctr_info_get_f)(
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
typedef int (*l3_vrf_route_cnt_get_f)(int unit, bcm_vrf_t vrf,
                                      bool ipv6, uint32_t *cnt);

/*!
 * \brief Traverse NAT host table.
 *
 * \param [in] unit Unit number.
 * \param [in] flags Flags to match during traverse.
 * \param [in] trav_fn User supplied callback function.
 * \param [in] cb Callback function to contruct NAT info from fib.
 * \param [in] user_data User supplied cookie used in callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_nat_host_traverse_f)(int unit, uint32_t flags,
                                      bcm_l3_nat_ingress_traverse_cb trav_fn,
                                      bcmi_ltsw_l3_fib_to_nat_cb cb,
                                      void *user_data);

/*!
 * \brief Get the FIB table database.
 *
 * \param [in] unit Unit number.
 * \param [out] tbl_db FIB table databse.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_fib_tbl_db_get_f)(int unit,
                                   bcmint_ltsw_l3_fib_tbl_db_t *tbl_db);

/*
 * \brief Attach flexctr action to the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_mc_flexctr_attach_f)(
        int unit,
        bcmi_ltsw_l3_fib_t *fib,
        bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach flexctr action of the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_mc_flexctr_detach_f)(
        int unit,
        bcmi_ltsw_l3_fib_t *fib);

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
typedef int (*l3_mc_flexctr_info_get_f)(
        int unit,
        bcmi_ltsw_l3_fib_t *fib,
        bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Enable dropping L3 unicast packets if L3_IIF equals to L3_EIF.
 *
 * \param [in] unit Unit number.
 * \param [in] enable 1: Drop packets, 0: Do not drop packets.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_same_intf_drop_set_f)(
        int unit,
        int enable);

/*!
 * \brief Get the drop setting for L3 unicast packets if L3_IIF equals to L3_EIF.
 *
 * \param [in] unit Unit number.
 * \param [out] enable 1: Drop packets, 0: Do not drop packets.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_same_intf_drop_get_f)(
        int unit,
        int *enable);

/*!
 * \brief Age on L3 routing table, clear HIT bit of entry if set.
 *
 * \param [in] unit Unit number.
 * \param [in] age_out User supplied callback function.
 * \param [in] user_data  User provided cookie for callback.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_route_age_f)(int unit, bcm_l3_route_traverse_cb age_out,
                              void *user_data);

/*!
 * \brief Attach flexctr action to the given L3 unicast route.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route info.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_route_flexctr_attach_f)(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach flexctr action of the given L3 unicast route.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route info.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_route_flexctr_detach_f)(
    int unit,
    bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Get flex counter information of the given L3 unicast route.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route info.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_route_flexctr_info_get_f)(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Set flex counter object value for the given L3 unicast route.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route info.
 * \param [in] value The flex counter object value.
 *
 * The flex counter object value could be used to generate counter index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_route_flexctr_object_set_f)(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t value);

/*!
 * \brief Get flex counter object value for the given L3 unicast route.
 *
 * \param [in] unit Unit Number.
 * \param [in] fib L3 route info.
 * \param [out] value The flex counter object value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_route_flexctr_object_get_f)(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t *value);
/*!
 * \brief Set flex counter object value for the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route info.
 * \param [in] value The flex counter object value.
 *
 * The flex counter object value could be used to generate counter index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_mc_flexctr_object_set_f)(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t value);

/*!
 * \brief Get flex counter object value for the given IPMC group.
 *
 * \param [in] unit Unit Number.
 * \param [in] fib L3 route info.
 * \param [out] value The flex counter object value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_mc_flexctr_object_get_f)(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t *value);

/*!
 * \brief Clear hit bits of all L3 route entries if set.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_route_hit_clear_f)(
    int unit);


/*!
 * \brief Set L3_ALPM_CONTROL LT.
 *
 * \param [in] unit Unit number.
 * \param [in] alpm_temp ALPM template.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_alpm_control_set_f)(
    int unit,
    int alpm_temp,
    bcmi_ltsw_uft_mode_t uft_mode);

/*!
 * \brief Clear hit bits of all host entries if set.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_host_hit_clear_f)(
    int unit);

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
typedef int (*l3_fib_aacl_add_f)(
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
typedef int (*l3_fib_aacl_delete_f)(
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
typedef int (*l3_fib_aacl_delete_all_f)(
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
typedef int (*l3_fib_aacl_find_f)(
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
typedef int (*l3_fib_aacl_traverse_f)(
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
typedef int (*l3_fib_aacl_matched_traverse_f)(
    int unit,
    uint32_t flags,
    bcm_l3_aacl_traverse_cb trav_fn,
    void *user_data);

/*!
 * \brief Route projection based on testDB.
 *
 * \param [in] unit Unit number.
 * \param [in] grp ALPM route group.
 * \param [out] prj_cnt Uni-dimensional projected maximum number of route.
 * \param [out] max_usage_lvl The level with maximum resource usage.
 * \param [out] max_usage Maximum resource usage.
 *
 * retval SHR_E_NONE No errors.
 * retval !SHR_E_NONE Failure.
 */
typedef int (*l3_alpm_grp_prj_db_get_f)(
    int unit,
    bcm_l3_route_group_t grp,
    int *prj_cnt,
    int *max_usage_lvl,
    int *max_usage);

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
typedef int (*l3_route_perf_v4_lt_op_f)(
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
typedef int (*l3_route_perf_v6_lt_op_f)(
    int unit,
    bcmi_ltsw_l3_route_perf_opcode_t op,
    int vrf,
    uint64_t *v6,
    uint64_t *v6_mask,
    int intf,
    int (*alpm_dmode)[2]);


/*! L3 Fib function vector driver. */
typedef struct mbcm_ltsw_l3_fib_drv_s {

    /*! FIB manager initialization. */
    l3_fib_init_f l3_fib_init;

    /*! FIB manager de-initialization. */
    l3_fib_deinit_f l3_fib_deinit;

    /*! Get max vrf number. */
    l3_fib_vrf_max_get_f l3_fib_vrf_max_get;

    /*! Add a L3 host entry for IPv4 unicast flow. */
    l3_host_add_f l3_host_ipv4_uc_add;

    /*! Delete a L3 host entry of IPv4 unicast flow. */
    l3_host_delete_f l3_host_ipv4_uc_delete;

    /*! Delete IPv4 unicast host entries based on IP prefix. */
    l3_host_delete_f l3_host_ipv4_uc_del_by_prefix;

    /*! Lookup a L3 host entry of IPv4 unicast flow. */
    l3_host_find_f l3_host_ipv4_uc_find;

    /*! Traverse IPv4 unicast host entries. */
    l3_host_traverse_f l3_host_ipv4_uc_traverse;

    /*! Add a L3 host entry for IPv4 multicast flow. */
    l3_host_add_f l3_host_ipv4_mc_add;

    /*! Delete a L3 host entry of IPv4 multicast flow. */
    l3_host_delete_f l3_host_ipv4_mc_delete;

    /*! Lookup a L3 host entry of IPv4 multicast flow. */
    l3_host_find_f l3_host_ipv4_mc_find;

    /*! Traverse IPv4 multicast host entries. */
    l3_ipmc_traverse_f l3_host_ipv4_mc_traverse;

    /*! Attach flex counter to L3 host IPv4 multicast entry. */
    l3_mc_flexctr_attach_f l3_host_ipv4_mc_flexctr_attach;

    /*! Detach flex counter from L3 host IPv4 multicast entry. */
    l3_mc_flexctr_detach_f l3_host_ipv4_mc_flexctr_detach;

    /*! Get flex counter info associated with L3 host IPv4 multicast entry. */
    l3_mc_flexctr_info_get_f l3_host_ipv4_mc_flexctr_info_get;

    /*! Add a L3 host entry for IPv6 unicast flow. */
    l3_host_add_f l3_host_ipv6_uc_add;

    /*! Delete a L3 host entry of IPv6 unicast flow. */
    l3_host_delete_f l3_host_ipv6_uc_delete;

    /*! Delete IPv6 unicast host entries based on IP prefix. */
    l3_host_delete_f l3_host_ipv6_uc_del_by_prefix;

    /*! Lookup a L3 host entry of IPv6 unicast flow. */
    l3_host_find_f l3_host_ipv6_uc_find;

    /*! Traverse IPv6 unicast host entries. */
    l3_host_traverse_f l3_host_ipv6_uc_traverse;

    /*! Add a L3 host entry for IPv6 multicast flow. */
    l3_host_add_f l3_host_ipv6_mc_add;

    /*! Delete a L3 host entry of IPv6 multicast flow. */
    l3_host_delete_f l3_host_ipv6_mc_delete;

    /*! Lookup a L3 host entry of IPv6 multicast flow. */
    l3_host_find_f l3_host_ipv6_mc_find;

    /*! Traverse IPv6 multicast host entries. */
    l3_ipmc_traverse_f l3_host_ipv6_mc_traverse;

    /*! Attach flex counter to L3 host IPv6 multicast entry. */
    l3_mc_flexctr_attach_f l3_host_ipv6_mc_flexctr_attach;

    /*! Detach flex counter from L3 host IPv6 multicast entry. */
    l3_mc_flexctr_detach_f l3_host_ipv6_mc_flexctr_detach;

    /*! Get flex counter info associated with L3 host IPv6 multicast entry. */
    l3_mc_flexctr_info_get_f l3_host_ipv6_mc_flexctr_info_get;

    /*! Delete L3 host entries that match or not match a specified L3 intf. */
    l3_host_del_by_intf_f l3_host_del_by_intf;

    /*! Delete all L3 host entries. */
    l3_host_del_all_f l3_host_del_all;

    /*! Add a L3 route entry for IPv4 unicast flow. */
    l3_route_add_f l3_route_ipv4_uc_add;

    /*! Delete a L3 route entry of IPv4 unicast flow. */
    l3_route_delete_f l3_route_ipv4_uc_delete;

    /*! Lookup a L3 route entry of IPv4 unicast flow. */
    l3_route_find_f l3_route_ipv4_uc_find;

    /*! Traverse IPv4 unicast route entries. */
    l3_route_traverse_f l3_route_ipv4_uc_traverse;

    /*! Add a L3 route entry for IPv4 multicast flow. */
    l3_route_add_f l3_route_ipv4_mc_add;

    /*! Delete a L3 route entry of IPv4 multicast flow. */
    l3_route_delete_f l3_route_ipv4_mc_delete;

    /*! Lookup a L3 route entry of IPv4 multicast flow. */
    l3_route_find_f l3_route_ipv4_mc_find;

    /*! Traverse IPv4 multicast route entries. */
    l3_ipmc_traverse_f l3_route_ipv4_mc_traverse;

    /*! Attach flex counter to L3 route IPv4 multicast entry. */
    l3_mc_flexctr_attach_f l3_route_ipv4_mc_flexctr_attach;

    /*! Detach flex counter from L3 route IPv4 multicast entry. */
    l3_mc_flexctr_detach_f l3_route_ipv4_mc_flexctr_detach;

    /*! Get flex counter info associated with L3 route IPv4 multicast entry. */
    l3_mc_flexctr_info_get_f l3_route_ipv4_mc_flexctr_info_get;

    /*! Add a L3 route entry for IPv6 unicast flow. */
    l3_route_add_f l3_route_ipv6_uc_add;

    /*! Delete a L3 route entry of IPv6 unicast flow. */
    l3_route_delete_f l3_route_ipv6_uc_delete;

    /*! Lookup a L3 route entry of IPv6 unicast flow. */
    l3_route_find_f l3_route_ipv6_uc_find;

    /*! Traverse IPv6 unicast route entries. */
    l3_route_traverse_f l3_route_ipv6_uc_traverse;

    /*! Add a L3 route entry for IPv6 multicast flow. */
    l3_route_add_f l3_route_ipv6_mc_add;

    /*! Delete a L3 route entry of IPv6 multicast flow. */
    l3_route_delete_f l3_route_ipv6_mc_delete;

    /*! Lookup a L3 route entry of IPv6 multicast flow. */
    l3_route_find_f l3_route_ipv6_mc_find;

    /*! Traverse IPv6 multicast route entries. */
    l3_ipmc_traverse_f l3_route_ipv6_mc_traverse;

    /*! Attach flex counter to L3 route IPv6 multicast entry. */
    l3_mc_flexctr_attach_f l3_route_ipv6_mc_flexctr_attach;

    /*! Detach flex counter from L3 route IPv6 multicast entry. */
    l3_mc_flexctr_detach_f l3_route_ipv6_mc_flexctr_detach;

    /*! Get flex counter info associated with L3 route IPv6 multicast entry. */
    l3_mc_flexctr_info_get_f l3_route_ipv6_mc_flexctr_info_get;

    /*! Delete L3 route entries that match or not match a specified L3 intf. */
    l3_route_del_by_intf_f l3_route_del_by_intf;

    /*! Delete all L3 route entries. */
    l3_route_del_all_f l3_route_del_all;

    /*! Attach flexctr action to the given VRF. */
    l3_vrf_flexctr_attach_f l3_vrf_flexctr_attach;

    /*! Detach flexctr action of the given VRF. */
    l3_vrf_flexctr_detach_f l3_vrf_flexctr_detach;

    /*! Get flex counter information of the given VRF. */
    l3_vrf_flexctr_info_get_f l3_vrf_flexctr_info_get;

    /*! Get IPv4 or IPv6 route count of the given VRF. */
    l3_vrf_route_cnt_get_f l3_vrf_route_cnt_get;

    /*! Traverse IPv4 host table for NAT entries. */
    l3_nat_host_traverse_f l3_nat_host_traverse;

    /*! Get FIB table databse. */
    l3_fib_tbl_db_get_f l3_fib_tbl_db_get;

    /*! Enable dropping L3 unicast packets if L3_IIF equals to L3_EIF. */
    l3_same_intf_drop_set_f l3_same_intf_drop_set;

    /*! Get the drop setting for L3 unicast packets if L3_IIF equals to L3_EIF. */
    l3_same_intf_drop_get_f l3_same_intf_drop_get;

    /*! Age hit bits of all L3 route entries. */
    l3_route_age_f l3_route_age;

    /* Attach L3 route flex counter. */
    l3_route_flexctr_attach_f l3_route_flexctr_attach;

    /* Detach L3 route flex counter. */
    l3_route_flexctr_detach_f l3_route_flexctr_detach;

    /* Get L3 route flex counter info. */
    l3_route_flexctr_info_get_f l3_route_flexctr_info_get;

    /* Set L3 route flex counter object value. */
    l3_route_flexctr_object_set_f l3_route_flexctr_object_set;

    /* Get L3 route flex counter object value. */
    l3_route_flexctr_object_get_f l3_route_flexctr_object_get;

    /* Attach IPMC group flex counter. */
    l3_mc_flexctr_attach_f l3_ipmc_flexctr_attach;

    /* Detach IPMC group flex counter. */
    l3_mc_flexctr_detach_f l3_ipmc_flexctr_detach;

    /* Get IPMC group flex counter info. */
    l3_mc_flexctr_info_get_f l3_ipmc_flexctr_info_get;

    /* Set IPMC group flex counter object value. */
    l3_mc_flexctr_object_set_f l3_ipmc_flexctr_object_set;

    /* Get IPMC group flex counter object value. */
    l3_mc_flexctr_object_get_f l3_ipmc_flexctr_object_get;

    /* Clear hit bits of all L3 route entries. */
    l3_route_hit_clear_f l3_route_hit_clear;

    /* Clear hit bits of all IPMC entries. */
    l3_host_hit_clear_f l3_host_hit_clear;

    /*! Age hit bits of all IPMC entries. */
    l3_ipmc_traverse_f l3_ipmc_age;

    /*! Set L3 ALPM control. */
    l3_alpm_control_set_f l3_alpm_control_set;

    /*! Adds a L3 AACL to the compression table. */
    l3_fib_aacl_add_f l3_fib_aacl_add;

    /*! Deletes a L3 AACL from the compression table. */
    l3_fib_aacl_delete_f l3_fib_aacl_delete;

    /*! Deletes all AACLs. */
    l3_fib_aacl_delete_all_f l3_fib_aacl_delete_all;

    /*! Finds information from the AACL table. */
    l3_fib_aacl_find_f l3_fib_aacl_find;

    /*! Traverse through the AACL table and run callback at each entry. */
    l3_fib_aacl_traverse_f l3_fib_aacl_traverse;

    /*! Traverse through the AACL table and run callback at each entry. */
    l3_fib_aacl_matched_traverse_f l3_fib_aacl_matched_traverse;

    /*! Route projection based on testDB. */
    l3_alpm_grp_prj_db_get_f l3_alpm_grp_prj_db_get;

    /*! Route performance LT operation. */
    l3_route_perf_v4_lt_op_f l3_route_perf_v4_lt_op;

    /*! Route performance LT operation. */
    l3_route_perf_v6_lt_op_f l3_route_perf_v6_lt_op;

} mbcm_ltsw_l3_fib_drv_t;

/*!
 * \brief Set the L3 fib driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] drv L3 driver to set.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_fib_drv_set(int unit, mbcm_ltsw_l3_fib_drv_t *drv);

/*!
 * \brief Initialize L3 fib module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_fib_init(int unit);

/*!
 * \brief De-Initialize L3 fib module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_fib_deinit(int unit);

/*!
 * \brief Get the max vrf number.
 *
 * \param [in] unit Unit number.
 * \param [out] max_vrf Max vrf.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_fib_vrf_max_get(int unit, uint32_t *max_vrf);

/*!
 * \brief Add an entry to the L3 host table for IPv4 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_host_ipv4_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv4 unicast from L3 host table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_host_ipv4_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete entries of IPv4 unicast based on IP prefix.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_host_ipv4_uc_del_by_prefix(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 host entry for IPv4 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_host_ipv4_uc_find(int unit, bcmi_ltsw_l3_fib_t *fib);

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
mbcm_ltsw_l3_host_ipv4_uc_traverse(int unit, uint32_t flags,
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
mbcm_ltsw_l3_host_ipv4_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv4 multicast from L3 host table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_host_ipv4_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 host entry for IPv4 multicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_host_ipv4_mc_find(int unit, bcmi_ltsw_l3_fib_t *fib);

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
mbcm_ltsw_l3_host_ipv4_mc_traverse(int unit, uint32_t attrib,
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
mbcm_ltsw_l3_host_ipv4_mc_flexctr_attach(int unit,
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
mbcm_ltsw_l3_host_ipv4_mc_flexctr_detach(int unit, bcmi_ltsw_l3_fib_t *fib);

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
mbcm_ltsw_l3_host_ipv4_mc_flexctr_info_get(int unit,
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
mbcm_ltsw_l3_host_ipv6_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv6 unicast from L3 host table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_host_ipv6_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete entries of IPv6 unicast based on IP prefix.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_host_ipv6_uc_del_by_prefix(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 host entry for IPv6 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_host_ipv6_uc_find(int unit, bcmi_ltsw_l3_fib_t *fib);

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
mbcm_ltsw_l3_host_ipv6_uc_traverse(int unit, uint32_t flags,
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
mbcm_ltsw_l3_host_ipv6_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv6 multicast from L3 host table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_host_ipv6_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 host entry for IPv6 multiicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_host_ipv6_mc_find(int unit, bcmi_ltsw_l3_fib_t *fib);

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
mbcm_ltsw_l3_host_ipv6_mc_traverse(int unit, uint32_t attrib,
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
mbcm_ltsw_l3_host_ipv6_mc_flexctr_attach(int unit,
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
mbcm_ltsw_l3_host_ipv6_mc_flexctr_detach(int unit, bcmi_ltsw_l3_fib_t *fib);

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
mbcm_ltsw_l3_host_ipv6_mc_flexctr_info_get(int unit,
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
mbcm_ltsw_l3_host_del_by_intf(int unit, bcmi_ltsw_l3_fib_t *fib, int negate);

/*!
 * \brief Delete all L3 host entries.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib L3 FIB attributes.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_host_del_all(int unit, uint32_t attrib);

/*!
 * \brief Add an entry to the L3 route table for IPv4 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_route_ipv4_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv4 unicast from L3 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_route_ipv4_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 route entry for IPv4 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_route_ipv4_uc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib);

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
mbcm_ltsw_l3_route_ipv4_uc_traverse(int unit, uint32_t flags,
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
mbcm_ltsw_l3_route_ipv4_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv4 multicast from L3 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_route_ipv4_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 route entry for IPv4 multicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_route_ipv4_mc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib);

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
mbcm_ltsw_l3_route_ipv4_mc_traverse(int unit, uint32_t attrib,
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
mbcm_ltsw_l3_route_ipv4_mc_flexctr_attach(int unit,
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
mbcm_ltsw_l3_route_ipv4_mc_flexctr_detach(int unit, bcmi_ltsw_l3_fib_t *fib);

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
mbcm_ltsw_l3_route_ipv4_mc_flexctr_info_get(int unit,
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
mbcm_ltsw_l3_route_ipv6_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv6 unicast from L3 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_route_ipv6_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 route entry for IPv6 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_route_ipv6_uc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib);

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
mbcm_ltsw_l3_route_ipv6_uc_traverse(int unit, uint32_t flags,
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
mbcm_ltsw_l3_route_ipv6_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv6 multicast from L3 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_route_ipv6_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 route entry for IPv6 multiicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_route_ipv6_mc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib);

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
mbcm_ltsw_l3_route_ipv6_mc_traverse(int unit, uint32_t attrib,
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
mbcm_ltsw_l3_route_ipv6_mc_flexctr_attach(int unit,
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
mbcm_ltsw_l3_route_ipv6_mc_flexctr_detach(int unit, bcmi_ltsw_l3_fib_t *fib);

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
mbcm_ltsw_l3_route_ipv6_mc_flexctr_info_get(int unit,
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
mbcm_ltsw_l3_route_del_by_intf(int unit, bcmi_ltsw_l3_fib_t *fib, int negate);

/*!
 * \brief Delete all L3 route entries.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib L3 FIB attributes.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_route_del_all(int unit, uint32_t attrib);

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
mbcm_ltsw_l3_vrf_flexctr_attach(
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
mbcm_ltsw_l3_vrf_flexctr_detach(
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
mbcm_ltsw_l3_vrf_flexctr_info_get(
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
mbcm_ltsw_l3_vrf_route_cnt_get(int unit, bcm_vrf_t vrf,
                               bool ipv6, uint32_t *cnt);

/*!
 * \brief Traverse NAT host table.
 *
 * \param [in] unit Unit number.
 * \param [in] flags Flags to match during traverse.
 * \param [in] trav_fn User supplied callback function.
 * \param [in] cb Callback function to contruct NAT info from fib.
 * \param [in] user_data User supplied cookie used in callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_nat_host_traverse(int unit, uint32_t flags,
                               bcm_l3_nat_ingress_traverse_cb trav_fn,
                               bcmi_ltsw_l3_fib_to_nat_cb cb,
                               void *user_data);

/*!
 * \brief Get the FIB table database.
 *
 * \param [in] unit Unit number.
 * \param [out] tbl_db FIB table databse.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_fib_tbl_db_get(int unit,
                            bcmint_ltsw_l3_fib_tbl_db_t *tbl_db);

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
mbcm_ltsw_l3_same_intf_drop_set(int unit, int enable);

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
mbcm_ltsw_l3_same_intf_drop_get(int unit, int *enable);

/*!
 * \brief Set L3_ALPM_CONTROL LT.
 *
 * \param [in] unit Unit number.
 * \param [in] alpm_temp ALPM template.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_alpm_control_set(int unit, int alpm_temp,
                              bcmi_ltsw_uft_mode_t uft_mode);

/*!
 * \brief Age on L3 routing table, clear HIT bit of entry if set.
 *
 * \param [in] unit Unit number.
 * \param [in] age_out User supplied callback function.
 * \param [in] user_data  User provided cookie for callback.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_route_age(int unit, bcm_l3_route_traverse_cb age_out,
                       void *user_data);

/*!
 * \brief Attach flexctr action to the given L3 unicast route.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route info.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_route_flexctr_attach(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach flexctr action of the given L3 unicast route.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_route_flexctr_detach(
    int unit,
    bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Get flex counter information of the given L3 unicast route.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route info.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_route_flexctr_info_get(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Set flex counter object value for the given L3 unicast route.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route info.
 * \param [in] value The flex counter object value.
 *
 * The flex counter object value could be used to generate counter index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_route_flexctr_object_set(
    int unit,
	bcmi_ltsw_l3_fib_t *fib,
    uint32_t value);

/*!
 * \brief Get flex counter object value for the given L3 unicast route.
 *
 * \param [in] unit Unit Number.
 * \param [in] fib L3 route info.
 * \param [out] value The flex counter object value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_route_flexctr_object_get(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t *value);

/*!
 * \brief Attach flexctr action to the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib IPMC group info.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_ipmc_flexctr_attach(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach flexctr action of the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib IPMC group info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_ipmc_flexctr_detach(
    int unit,
    bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Get flex counter information of the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib IPMC info.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ipmc_flexctr_info_get(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Set flex counter object value for the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib IPMC group info.
 * \param [in] value The flex counter object value.
 *
 * The flex counter object value could be used to generate counter index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ipmc_flexctr_object_set(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t value);

/*!
 * \brief Get flex counter object value for the given IPMC group.
 *
 * \param [in] unit Unit Number.
 * \param [in] fib IPMC group info.
 * \param [out] value The flex counter object value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ipmc_flexctr_object_get(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t *value);

/*!
 * \brief Clear hit bits for all L3 route entries if set.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_route_hit_clear(
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
mbcm_ltsw_l3_host_hit_clear(
    int unit);

/*!
 * \brief Public function to get IPMC v4 table size.
 *
 * \param [in] unit Unit Number.
 * \param [out] size IPMC v4 table size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */

extern int
mbcm_ltsw_l3_ipmc_v4_size_get(
    int unit,
    int *size);

/*!
 * \brief Public function to get current IPMC v4 entry count.
 *
 * \param [in] unit Unit Number.
 * \param [out] count IPMC v4 entry count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ipmc_v4_count_get(
    int unit,
    int *count);

/*!
 * \brief Public function to get IPMC v6 table size.
 *
 * \param [in] unit Unit Number.
 * \param [out] size IPMC v6 table size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */

extern int
mbcm_ltsw_l3_ipmc_v6_size_get(
    int unit,
    int *size);

/*!
 * \brief Public function to get current IPMC v6 entry count.
 *
 * \param [in] unit Unit Number.
 * \param [out] count IPMC v6 entry count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ipmc_v6_count_get(
    int unit,
    int *count);

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
mbcm_ltsw_l3_ipmc_age(
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
mbcm_ltsw_l3_fib_aacl_add(
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
mbcm_ltsw_l3_fib_aacl_delete(
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
mbcm_ltsw_l3_fib_aacl_delete_all(
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
mbcm_ltsw_l3_fib_aacl_find(
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
mbcm_ltsw_l3_fib_aacl_traverse(
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
mbcm_ltsw_l3_fib_aacl_matched_traverse(
    int unit,
    uint32_t flags,
    bcm_l3_aacl_traverse_cb trav_fn,
    void *user_data);

/*!
 * \brief Route projection based on testDB.
 *
 * \param [in] unit Unit number.
 * \param [in] grp ALPM route group.
 * \param [out] prj_cnt Uni-dimensional projected maximum number of route.
 * \param [out] max_usage_lvl The level with maximum resource usage.
 * \param [out] max_usage Maximum resource usage.
 *
 * retval SHR_E_NONE No errors.
 * retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_alpm_grp_prj_db_get(
    int unit,
    bcm_l3_route_group_t grp,
    int *prj_cnt,
    int *max_usage_lvl,
    int *max_usage);

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
mbcm_ltsw_l3_route_perf_v4_lt_op(
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
mbcm_ltsw_l3_route_perf_v6_lt_op(
    int unit,
    bcmi_ltsw_l3_route_perf_opcode_t op,
    int vrf,
    uint64_t *v6,
    uint64_t *v6_mask,
    int intf,
    int (*alpm_dmode)[2]);

#endif /* BCMI_LTSW_MBCM_L3_FIB_H */
