/*! \file l3_fib.h
 *
 * L3 FIB header file.
 * This file contains L3 FIB definitions internal to the BCM library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_L3_FIB_H
#define BCMI_LTSW_L3_FIB_H

#include <sal/sal_types.h>
#include <bcm/ipmc.h>
#include <bcm/nat.h>
#include <bcm_int/ltsw/flexctr.h>

/*!
 * \name Attributes of FIB entries to be operated.
 * \anchor BCMI_LTSW_L3_FIB_ATTR_xxx
 */

/*! \{ */
/*! Operate IPv4 entries. */
#define BCMI_LTSW_L3_FIB_ATTR_IPV4          (1<<0)
/*! Operate IPv6 entries. */
#define BCMI_LTSW_L3_FIB_ATTR_IPV6          (1<<1)
/*! Operate host entries. */
#define BCMI_LTSW_L3_FIB_ATTR_HOST          (1<<2)
/*! Operate network entries. */
#define BCMI_LTSW_L3_FIB_ATTR_NETWORK       (1<<3)
/*! Operate unicast entries. */
#define BCMI_LTSW_L3_FIB_ATTR_UC            (1<<4)
/*! Operate multicast entries. */
#define BCMI_LTSW_L3_FIB_ATTR_MC            (1<<5)
/*! Operate multicast (S,G) entries. */
#define BCMI_LTSW_L3_FIB_ATTR_SRC_GROUP     (1<<6)
/*! Operate multicast (*,G) entries. */
#define BCMI_LTSW_L3_FIB_ATTR_STAR_GROUP    (1<<7)
/*! Operate NAT host entries. */
#define BCMI_LTSW_L3_FIB_ATTR_NAT           (1<<8)
/*! \} */

/*!
 * \name Operation flag of FIB object.
 * \anchor BCMI_LTSW_L3_FIB_OBJ_F_xxx
 */

/*! \{ */
/*! Count in unit of base-entry (IPv4 Unicast). */
#define BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES (1 << 0)
/*! \} */

/*!
 * \name Internal flags of FIB object with other modules.
 * \anchor BCMI_LTSW_L3_FIB_F_xxx
 */

/*! \{ */
/*! Copy packets to CPU on IPMC RPF failure. */
#define BCMI_LTSW_L3_FIB_F_IPMC_RPF_FAIL_TOCPU           (1 << 0)
/*! Drop packets on IPMC RPF failure. */
#define BCMI_LTSW_L3_FIB_F_IPMC_RPF_FAIL_DROP            (1 << 1)
/*! Do not source port check this entry. */
#define BCMI_LTSW_L3_FIB_F_IPMC_SOURCE_PORT_NOCHECK      (1 << 2)
/*! When set, perform RPF check.. */
#define BCMI_LTSW_L3_FIB_F_IPMC_POST_LOOKUP_RPF_CHECK    (1 << 3)

/*! \} */

/* L3 FIB objects. */
typedef enum bcmi_ltsw_l3_fib_object_e {
    BCMI_LTSW_L3_FIB_OBJ_HOST_MAX,
    BCMI_LTSW_L3_FIB_OBJ_HOST_USED,
    BCMI_LTSW_L3_FIB_OBJ_HOST_V4_MAX,
    BCMI_LTSW_L3_FIB_OBJ_HOST_V4_USED,
    BCMI_LTSW_L3_FIB_OBJ_HOST_V6_MAX,
    BCMI_LTSW_L3_FIB_OBJ_HOST_V6_USED,
    BCMI_LTSW_L3_FIB_OBJ_HOST_MC_MAX,
    BCMI_LTSW_L3_FIB_OBJ_HOST_MC_USED,
    BCMI_LTSW_L3_FIB_OBJ_HOST_MC_V4_MAX,
    BCMI_LTSW_L3_FIB_OBJ_HOST_MC_V4_USED,
    BCMI_LTSW_L3_FIB_OBJ_HOST_MC_V6_MAX,
    BCMI_LTSW_L3_FIB_OBJ_HOST_MC_V6_USED,
    BCMI_LTSW_L3_FIB_OBJ_HOST_UC_V4_MAX,
    BCMI_LTSW_L3_FIB_OBJ_HOST_UC_V4_USED,
    BCMI_LTSW_L3_FIB_OBJ_HOST_UC_V6_MAX,
    BCMI_LTSW_L3_FIB_OBJ_HOST_UC_V6_USED,
    BCMI_LTSW_L3_FIB_OBJ_HOST_MC_SG_V4_MAX,
    BCMI_LTSW_L3_FIB_OBJ_HOST_MC_SG_V4_USED,
    BCMI_LTSW_L3_FIB_OBJ_HOST_MC_SG_V6_MAX,
    BCMI_LTSW_L3_FIB_OBJ_HOST_MC_SG_V6_USED,
    BCMI_LTSW_L3_FIB_OBJ_HOST_MC_G_V4_MAX,
    BCMI_LTSW_L3_FIB_OBJ_HOST_MC_G_V4_USED,
    BCMI_LTSW_L3_FIB_OBJ_HOST_MC_G_V6_MAX,
    BCMI_LTSW_L3_FIB_OBJ_HOST_MC_G_V6_USED,
    BCMI_LTSW_L3_FIB_OBJ_ROUTE_MAX,
    BCMI_LTSW_L3_FIB_OBJ_ROUTE_USED,
    BCMI_LTSW_L3_FIB_OBJ_ROUTE_V4_MAX,
    BCMI_LTSW_L3_FIB_OBJ_ROUTE_V4_USED,
    BCMI_LTSW_L3_FIB_OBJ_ROUTE_V6_64_MAX,
    BCMI_LTSW_L3_FIB_OBJ_ROUTE_V6_64_USED,
    BCMI_LTSW_L3_FIB_OBJ_ROUTE_V6_128_MAX,
    BCMI_LTSW_L3_FIB_OBJ_ROUTE_V6_128_USED,
    BCMI_LTSW_L3_FIB_OBJ_CNT
} bcmi_ltsw_l3_fib_object_t;

/*!
 * \brief Covers all info needed to config L3 table
 *
 * This data structure covers all info needed to config L3 table
 */
typedef struct bcmi_ltsw_l3_fib_s {

    /*! See <bcm/l3.h> BCM_L3_*. */
    uint32_t flags;

    /*! See <bcm/l3.h> BCM_L3_FLAGS2_*. */
    uint32_t flags2;

    /*! See <bcm/l3.h> BCM_L3_FLAGS3_*. */
    uint32_t flags3;

    /*! See <ltsw/l3_fib.h> BCMI_LTSW_L3_FIB_*. */
    uint32_t int_flags;

    /*! Indicate a network route entry. */
    int network;

    /*! Virtual Router Instance. */
    bcm_vrf_t vrf;

    /*! Virtual Router Instance mask. */
    bcm_vrf_t vrf_mask;

    /*! IPv4 address. */
    bcm_ip_t ip_addr;

    /*! IPv4 address mask. */
    bcm_ip_t ip_addr_mask;

    /*! IPv6 address. */
    bcm_ip6_t ip6_addr;

    /*! IPv6 address mask. */
    bcm_ip6_t ip6_addr_mask;

    /*! Source IPv4 address. */
    bcm_ip_t sip_addr;

    /*! Source IPv4 address mask. */
    bcm_ip_t sip_addr_mask;

    /*! Source IPv6 address. */
    bcm_ip6_t sip6_addr;

    /*! Source IPv6 address mask. */
    bcm_ip6_t sip6_addr_mask;

    /*! Overlay L3 interface number. */
    bcm_if_t ol_intf;

    /*! Underlay L3 interface number. */
    bcm_if_t ul_intf;

    /*! Multicast pacekt MTU. */
    int mtu;

    /*! Multicast group id. */
    bcm_multicast_t mc_group;

    /*! Rendezvous Point Router ID. */
    int rpa_id;

    /*! Key ingress interface. */
    bcm_if_t l3_iif;

    /*! Expected ingress interface. */
    bcm_if_t expected_l3_iif;

    /*! NAT ID. */
    uint32_t nat_id;

    /*! Realm ID for NAT. */
    int realm_id;

    /*! Classification lookup class ID. */
    int class_id;

    /*! Internal Priority value. */
    int int_pri;

    /*! Trunk indicator. */
    bool is_trunk;

    /*! Trunk ID or port ID. */
    int port_tgid;

    /*! Module ID. */
    int mod_id;

    /*! ALPM associated data mode. */
    uint8_t alpm_data_mode;

    /*! Flex counter action index. */
    uint32_t flex_ctr_action_index;

    /*! Flex counter object index. */
    uint32_t flex_ctr_object_index;

    /*! INT action profile ID. */
    uint32_t int_act_profile_id;

    /*! INT OPAQUE DATA profile ID. */
    uint32_t int_opaque_data_profile_id;

    /*! Opaque control ID. */
    int opaque_ctrl_id;
} bcmi_ltsw_l3_fib_t;

typedef enum bcmi_ltsw_l3_route_perf_opcode_s {
    BCMI_LTSW_RP_OPCODE_ADD = 0,
    BCMI_LTSW_RP_OPCODE_UPD = 1,
    BCMI_LTSW_RP_OPCODE_GET = 2,
    BCMI_LTSW_RP_OPCODE_DEL = 3,
    BCMI_LTSW_RP_OPCODE_NUM = 4
} bcmi_ltsw_l3_route_perf_opcode_t;

/*!
 * \brief Definition of callback to construct bcm_ipmc_addr_t from fib info.
 *
 * \param [in] unit Unit number.
 * \param [in] unit fib L3 fib info.
 * \param [in] ipmc IPMC info.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmi_ltsw_l3_fib_to_ipmc_cb)(int unit,
                                           bcmi_ltsw_l3_fib_t *fib,
                                           bcm_ipmc_addr_t *ipmc);

/*!
 * \brief Definition of callback to construct bcm_l3_nat_ingress_t from fib.
 *
 * \param [in] unit Unit number.
 * \param [in] unit fib L3 fib info.
 * \param [in] nat NAT info.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmi_ltsw_l3_fib_to_nat_cb)(int unit,
                                          bcmi_ltsw_l3_fib_t *fib,
                                          bcm_l3_nat_ingress_t *nat);

/*!
 * \brief Initialize L3 fib module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_l3_fib_init(int unit);

/*!
 * \brief De-Initialize L3 fib module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_l3_fib_deinit(int unit);

/*!
 * \brief Get the max VRF ID of a given unit.
 *
 * \param [in] unit Unit number.
 * \param [out] max_vrf Max VRF ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_l3_vrf_max_get(int unit, uint32_t *max_vrf);

/*!
 * \brief Add an L3 multicast entry.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_fib_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an L3 multicast entry.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_fib_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete L3 multicast entries that match the given attributes.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib Operation attributes.
 *
 * The attrib specify the types of L3 multicast entries to be deleted.
 * See the definition of BCMI_LTSW_L3_FIB_ATTR_xxx.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_fib_mc_delete_all(int unit, uint32_t attrib);

/*!
 * \brief Find an L3 multicast entry.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_fib_mc_find(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Traverse L3 multicast entries that match the given attributes.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib Operation attributes.
 * \param [in] trav_fn User supplied callback function.
 * \param [in] cb Callback function to construct bcm_ipmc_addr_t from fib info.
 * \param [in] user_data User supplied cookie used in parameter in
 *                       callback function.
 *
 * The attrib specify the types of L3 multicast entries to be deleted.
 * See the definition of BCMI_LTSW_L3_FIB_ATTR_xxx.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_fib_mc_traverse(int unit, uint32_t attrib,
                             bcm_ipmc_traverse_cb trav_fn,
                             bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                             void *user_data);

/*!
 * \brief Age out the IPMC entry.
 *
 * Age out the IPMC entry by clearing the HIT bit when appropriate,
 * the IPMC entry itself is removed if HIT bit is not set.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib Operation attributes.
 * \param [in] age_cb User supplied callback function.
 * \param [in] cb Callback function to construct bcm_ipmc_addr_t from fib info.
 * \param [in] user_data User supplied cookie used in parameter in
 *                       callback function.
 *
 * The attrib specify the types of L3 multicast entries to be aged.
 * See the definition of BCMI_LTSW_L3_FIB_ATTR_xxx.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_fib_mc_age(int unit, uint32_t attrib,
                        bcm_ipmc_traverse_cb age_cb,
                        bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                        void *user_data);

/*!
 * \brief Attach counter entries to the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 * \param [in] ci Flex counter info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_l3_fib_mc_flexctr_attach(int unit, bcmi_ltsw_l3_fib_t *fib,
                                   bcmi_ltsw_flexctr_counter_info_t *ci);

/*!
 * \brief Detach counter entries to the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_l3_fib_mc_flexctr_detach(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Get counter info associated with given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 * \param [out] ci Flex counter info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_l3_fib_mc_flexctr_info_get(int unit, bcmi_ltsw_l3_fib_t *fib,
                                     bcmi_ltsw_flexctr_counter_info_t *ci);

/*!
 * \brief Add an L3 NAT host entry.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_nat_host_add(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete an L3 NAT host entry.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_nat_host_delete(int unit, bcmi_ltsw_l3_fib_t *fib);

/*!
 * \brief Delete all L3 NAT host entries.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_nat_host_delete_all(int unit);

/*!
 * \brief Find an L3 NAT host entry.
 *
 * \param [in] unit Unit number.
 * \param [in] fib L3 forwarding info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_nat_host_find(int unit, bcmi_ltsw_l3_fib_t *fib);

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
bcmi_ltsw_l3_nat_host_traverse(int unit, uint32_t flags,
                               bcm_l3_nat_ingress_traverse_cb trav_fn,
                               bcmi_ltsw_l3_fib_to_nat_cb cb,
                               void *user_data);

/*!
 * \brief Clear hit bits of all L3 route entries if set.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_route_hit_clear(int unit);

/*!
 * \brief Clear hit bits of all host entries if set.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_host_hit_clear(int unit);

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
bcmi_ltsw_l3_ipmc_v4_size_get(
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
bcmi_ltsw_l3_ipmc_v4_count_get(
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
bcmi_ltsw_l3_ipmc_v6_size_get(
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
bcmi_ltsw_l3_ipmc_v6_count_get(
    int unit,
    int *count);

/*!
 * \brief Public function to get current IPMC entry count.
 *
 * \param [in] unit Unit Number.
 * \param [out] count IPMC entry count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_ipmc_count_get(
    int unit,
    int *count);

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
bcmi_ltsw_l3_ipmc_flexctr_info_get(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    bcmi_ltsw_flexctr_counter_info_t *info);

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
bcmi_ltsw_l3_ipmc_flexctr_attach(
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
bcmi_ltsw_l3_ipmc_flexctr_detach(
    int unit,
    bcmi_ltsw_l3_fib_t *fib);

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
extern int
bcmi_ltsw_l3_ipmc_flexctr_object_set(
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
extern int
bcmi_ltsw_l3_ipmc_flexctr_object_get(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t *value);

/*!
 * \brief Get the statistics of the given FIB object.
 *
 * \param [in] unit Unit number.
 * \param [in] object L3 FIB object.
 * \param [in] flags Operation flag.
 * \param [out] entries Entry number of given object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_fib_object_cnt_get(
    int unit,
    bcmi_ltsw_l3_fib_object_t object,
    uint32_t flags,
    int *entries);

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
bcmi_ltsw_l3_same_intf_drop_set(int unit, int enable);

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
bcmi_ltsw_l3_same_intf_drop_get(int unit, int *enable);

/*!
 * \brief Uni-dimensional ALPM route projection based on current route trend.
 *
 * \param [in] unit Unit number.
 * \param [in] grp Route group.
 * \param [out] cnt Projection route number.
 * \param [out] max_usage_level The level with maximum resource usage.
 * \param [out] max_usage Maximum resource usage.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_route_grp_projection(int unit, bcm_l3_route_group_t grp,
                                  int *prj_cnt, int *max_usage_lvl,
                                  int *max_usage);

/*!
 * \brief ALPM route projection based on testDB.
 *
 * \param [in] unit Unit number.
 * \param [in] grp Route group.
 * \param [in] v4v6_ratio Expected V4_route_cnt/V6_route_cnt ratio.
 * \param [out] uni_prj_cnt Uni-dimensional projected maximum number of route.
 * \param [out] mix_prj_cnt Mixed DB route projection.
 * \param [out] max_usage_level The level with maximum resource usage.
 * \param [out] max_usage Maximum resource usage.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_route_grp_testdb(int unit, bcm_l3_route_group_t grp,
                              int v4v6_ratio, int *uni_prj_cnt,
                              int *mix_prj_cnt, int *max_usage_lvl,
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
extern int
bcmi_l3_route_perf_v4_lt_op(
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
extern int
bcmi_l3_route_perf_v6_lt_op(
    int unit,
    bcmi_ltsw_l3_route_perf_opcode_t op,
    int vrf,
    uint64_t *v6,
    uint64_t *v6_mask,
    int intf,
    int (*alpm_dmode)[2]);

#endif /* BCMI_LTSW_L3_FIB_H */
