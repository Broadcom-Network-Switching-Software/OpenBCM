/*! \file l3_fib.h
 *
 * XGS L3 fib module APIs and data structures used only internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_XGS_L3_FIB_H
#define BCMI_LTSW_XGS_L3_FIB_H

#include <bcm/l3.h>
#include <bcm_int/ltsw/l3_fib.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/l3_fib_int.h>

/* Unicast forwarding fields. */
typedef enum bcmint_xgs_l3_fib_fld_uc_e {
    BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0,
    BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1,
    BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK,
    BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1_MASK,
    BCMINT_XGS_L3_FIB_TBL_UC_VRF,
    BCMINT_XGS_L3_FIB_TBL_UC_VRF_MASK,
    BCMINT_XGS_L3_FIB_TBL_UC_ECMP_NHOP,
    BCMINT_XGS_L3_FIB_TBL_UC_ECMP_ID,
    BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID,
    BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI,
    BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI,
    BCMINT_XGS_L3_FIB_TBL_UC_DROP_ON_DIP_MATCH,
    BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID,
    BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT,
    BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID,
    BCMINT_XGS_L3_FIB_TBL_UC_HIT,
    BCMINT_XGS_L3_FIB_TBL_UC_INT_ACT_PROFILE_ID,
    BCMINT_XGS_L3_FIB_TBL_UC_INT_OPAQUE_DATA_PROFILE_ID,
    BCMINT_XGS_L3_FIB_TBL_UC_FLD_CNT
} bcmint_xgs_l3_fib_fld_uc_fld_t;

/* Multicast forwarding fields. */
typedef enum bcmint_xgs_l3_fib_fld_mc_e {
    BCMINT_XGS_L3_FIB_TBL_MC_GROUP_0,
    BCMINT_XGS_L3_FIB_TBL_MC_GROUP_1,
    BCMINT_XGS_L3_FIB_TBL_MC_L3_IIF_ID,
    BCMINT_XGS_L3_FIB_TBL_MC_VRF,
    BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR,
    BCMINT_XGS_L3_FIB_TBL_MC_SIP_1_ADDR,
    BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR_MASK,
    BCMINT_XGS_L3_FIB_TBL_MC_SIP_1_ADDR_MASK,
    BCMINT_XGS_L3_FIB_TBL_MC_GROUP_ID,
    BCMINT_XGS_L3_FIB_TBL_MC_CLASS_ID,
    BCMINT_XGS_L3_FIB_TBL_MC_COPY_TO_CPU,
    BCMINT_XGS_L3_FIB_TBL_MC_DROP,
    BCMINT_XGS_L3_FIB_TBL_MC_DROP_ON_GROUP_MATCH,
    BCMINT_XGS_L3_FIB_TBL_MC_USE_PORT_TRUNK_ID,
    BCMINT_XGS_L3_FIB_TBL_MC_IS_TRUNK,
    BCMINT_XGS_L3_FIB_TBL_MC_TRUNK_ID,
    BCMINT_XGS_L3_FIB_TBL_MC_PORT,
    BCMINT_XGS_L3_FIB_TBL_MC_EXPECTED_L3_IIF_ID,
    BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_OBJECT,
    BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_ACTION_ID,
    BCMINT_XGS_L3_FIB_TBL_MC_DROP_SRC_IPV6_LINK_LOCAL,
    BCMINT_XGS_L3_FIB_TBL_MC_HIT,
    BCMINT_XGS_L3_FIB_TBL_MC_FLD_CNT
} bcmint_xgs_l3_fib_fld_mc_fld_t;

/*!
 * \brief Initialize L3 fib module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_fib_init(int unit);

/*!
 * \brief De-Initialize L3 fib module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_fib_deinit(int unit);

/*!
 * \brief Get the max vrf number.
 *
 * \param [in] unit Unit number.
 * \param [out] max_vrf Max vrf.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_fib_vrf_max_get(int unit, uint32_t *max_vrf);

/*!
 * \brief Add an entry to the L3 route table for IPv4 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_route_ipv4_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv4 unicast from L3 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_route_ipv4_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 route entry for IPv4 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_route_ipv4_uc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib);

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
xgs_ltsw_l3_route_ipv4_uc_traverse(int unit, uint32_t flags,
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
xgs_ltsw_l3_route_ipv4_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv4 multicast from L3 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_route_ipv4_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 route entry for IPv4 multicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_route_ipv4_mc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib);

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
xgs_ltsw_l3_route_ipv4_mc_traverse(int unit, uint32_t attrib,
                                   bcm_ipmc_traverse_cb trav_fn,
                                   bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                                   void *user_data);

/*!
 * \brief Add an entry to the L3 route table for IPv6 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_route_ipv6_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv6 unicast from L3 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_route_ipv6_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 route entry for IPv6 unicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_route_ipv6_uc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib);

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
xgs_ltsw_l3_route_ipv6_uc_traverse(int unit, uint32_t flags,
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
xgs_ltsw_l3_route_ipv6_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an entry of IPv6 multicast from L3 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_route_ipv6_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Find a L3 route entry for IPv6 multiicast.
 *
 * \param [in] unit Unit number.
 * \param [in/out] fib L3 route entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_route_ipv6_mc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib);

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
xgs_ltsw_l3_route_ipv6_mc_traverse(int unit, uint32_t attrib,
                                   bcm_ipmc_traverse_cb trav_fn,
                                   bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                                   void *user_data);

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
xgs_ltsw_l3_route_del_by_intf(int unit, bcmi_ltsw_l3_fib_t *fib, int negate);

/*!
 * \brief Delete all L3 route entries.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib L3 fib operation attributes.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_l3_route_del_all(int unit, uint32_t attrib);

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
xgs_ltsw_l3_route_age(int unit, bcm_l3_route_traverse_cb age_out,
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
xgs_ltsw_l3_route_flexctr_attach(
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
xgs_ltsw_l3_route_flexctr_detach(
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
xgs_ltsw_l3_route_flexctr_info_get(
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
xgs_ltsw_l3_route_flexctr_object_set(
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
xgs_ltsw_l3_route_flexctr_object_get(
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
xgs_ltsw_l3_ipmc_flexctr_attach(
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
xgs_ltsw_l3_ipmc_flexctr_detach(
    int unit,
    bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Get flex counter information of the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib IPMC group info.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_ipmc_flexctr_info_get(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Set flex counter object value for the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 group info.
 * \param [in] value The flex counter object value.
 *
 * The flex counter object value could be used to generate counter index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_ipmc_flexctr_object_set(
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
xgs_ltsw_l3_ipmc_flexctr_object_get(
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
xgs_ltsw_l3_route_hit_clear(
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
xgs_ltsw_l3_ipmc_v4_size_get(
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
xgs_ltsw_l3_ipmc_v4_count_get(
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
xgs_ltsw_l3_ipmc_v6_size_get(
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
xgs_ltsw_l3_ipmc_v6_count_get(
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
xgs_ltsw_l3_ipmc_age(
    int unit,
    uint32_t attrib,
    bcm_ipmc_traverse_cb age_cb,
    bcmi_ltsw_l3_fib_to_ipmc_cb cb,
    void *user_data);

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
xgs_ltsw_l3_route_perf_v4_lt_op(
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
xgs_ltsw_l3_route_perf_v6_lt_op(
    int unit,
    bcmi_ltsw_l3_route_perf_opcode_t op,
    int vrf,
    uint64_t *v6,
    uint64_t *v6_mask,
    int intf,
    int (*alpm_dmode)[2]);

#endif /* BCMI_LTSW_XGS_L3_FIB_H */
