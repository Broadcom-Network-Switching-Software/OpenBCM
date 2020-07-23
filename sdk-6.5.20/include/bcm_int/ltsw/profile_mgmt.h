/*! \file profile_mgmt.h
 *
 * Profile management header file.
 * This file contains the management for profile table index.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_PROFILE_MGMT_H
#define BCMI_LTSW_PROFILE_MGMT_H

#include <sal/sal_types.h>

/*!
 * \brief Profile Table handle.
 */
typedef enum bcmi_ltsw_profile_hdl_e {
    /*! INVALID_PROFILE. */
    BCMI_LTSW_PROFILE_INVALID = 0,

    /*! PROFILE_VLAN. */
    BCMI_LTSW_PROFILE_VLAN = 1,

    /*! PROFILE_SYSTEM_PORT. */
    BCMI_LTSW_PROFILE_SYSTEM_PORT_PIPE0 = 2,

    /*! PROFILE_PORT_PKT_CTRL. */
    BCMI_LTSW_PROFILE_PORT_PKT_CTRL = 3,

    /*! PROFILE_L3_IIF. */
    BCMI_LTSW_PROFILE_L3_IIF = 5,

    /*! PROFILE_ING_MSHP_CHK. */
    BCMI_LTSW_PROFILE_ING_MSHP_CHK = 6,

    /*! PROFILE_EGR_MSHP_CHK. */
    BCMI_LTSW_PROFILE_EGR_MSHP_CHK = 7,

    /*! PROFILE_EGR_UNTAG. */
    BCMI_LTSW_PROFILE_EGR_UNTAG = 8,

    /*! PROFILE_ING_BLOCK_MASK_VFI_SECT_0. */
    BCMI_LTSW_PROFILE_ING_BLOCK_MASK_VFI_SECT_0 = 9,

    /*! PROFILE_ING_BLOCK_MASK_VFI_SECT_1. */
    BCMI_LTSW_PROFILE_ING_BLOCK_MASK_VFI_SECT_1 = 10,

    /*! PROFILE_ING_BLOCK_MASK_VFI_SECT_2. */
    BCMI_LTSW_PROFILE_ING_BLOCK_MASK_VFI_SECT_2 = 11,

    /*! PROFILE_ING_BLOCK_MASK_VFI_SECT_3. */
    BCMI_LTSW_PROFILE_ING_BLOCK_MASK_VFI_SECT_3 = 12,

    /*! PROFILE_PC_AUTONEG. */
    BCMI_LTSW_PROFILE_PC_AUTONEG = 13,

    /*! PROFILE_WRED_DROP_CURVE. */
    BCMI_LTSW_PROFILE_WRED_DROP_CURVE = 14,

    /*! PROFILE_WRED_TIME. */
    BCMI_LTSW_PROFILE_WRED_TIME = 15,

    /*! PROFILE_ING_COUNTER_OP. */
    BCMI_LTSW_PROFILE_FLEXCTR_ING_PIPE0 = 16,

    /*! PROFILE_EGR_COUNTER_OP. */
    BCMI_LTSW_PROFILE_FLEXCTR_EGR_PIPE0 = 17,

    /*! PROFILE_COS_MAP. */
    BCMI_LTSW_PROFILE_COS_MAP = 18,

    /*! PROFILE_IFP_COS_MAP. */
    BCMI_LTSW_PROFILE_IFP_COS_MAP = 19,

    /*! PROFILE_MIRROR_ENCAP. */
    BCMI_LTSW_PROFILE_MIRROR_ENCAP = 20,

    /*! PROFILE_VLAN_RANGE_CHECK. */
    BCMI_LTSW_PROFILE_VLAN_RANGE_MAP = 21,

    /*! PROFILE_TNL_ENCAP_SEQUENCE. */
    BCMI_LTSW_PROFILE_TNL_ENCAP_SEQUENCE = 22,

    /*! L3_MTU_PROFILE. */
    BCMI_LTSW_PROFILE_L3_MTU = 23,

    /*! PROFILE_BST_THD_PORT_SERVICE_POOL. */
    BCMI_LTSW_PROFILE_BST_THD_PORT_SERVICE_POOL =  26,

    /*! PROFILE_BST_THD_PRI_GRP. */
    BCMI_LTSW_PROFILE_BST_THD_PRI_GRP =  27,

    /*! PROFILE_BST_THD_QUEUE. */
    BCMI_LTSW_PROFILE_BST_THD_QUEUE =  28,

    /*! PROFILE_BST_THD_UC_PORT_SERVICE_POOL. */
    BCMI_LTSW_PROFILE_BST_THD_UC_PORT_SERVICE_POOL =  29,

    /*! PROFILE_BST_THD_MC_PORT_SERVICE_POOL. */
    BCMI_LTSW_PROFILE_BST_THD_MC_PORT_SERVICE_POOL =  30,

    /*! DLB_QUALITY_MAP. */
    BCMI_LTSW_PROFILE_DLB_QUALITY_MAP =  33,

    /*! INGRESS_SAMPLER. */
    BCMI_LTSW_PROFILE_INGRESS_SAMPLER =  34,

    /*! ECN_ING_MPLS_EXP_TO_ECN. */
    BCMI_LTSW_PROFILE_ECN_ING_MPLS_EXP_TO_ECN = 35,

    /*! SYS_PORT_FLOOD_BLOCK. */
    BCMI_LTSW_PROFILE_EGR_MASK = 36,

    /*! PROFILE_EBST. */
    BCMI_LTSW_PROFILE_EBST_PROFILE = 37,

    /*! FLEXDIGEST_LKUP_POLICY_MASK. */
    BCMI_LTSW_PROFILE_FLEXDIGEST_LKUP_POLICY_MASK = 38,

    /*! VLAN_EGR_TAG_ACTION. */
    BCMI_LTSW_PROFILE_VLAN_EGR_TAG_ACTION = 39,

    /*! VLAN_ING_TAG_ACTION. */
    BCMI_LTSW_PROFILE_VLAN_ING_TAG_ACTION = 40,

    /*! VLAN_ING_EGR_MEMBER_PORTS. */
    BCMI_LTSW_PROFILE_VLAN_ING_EGR_MEMBER_PORTS_PROFILE = 41,

    /*! PROFILE_ING_COUNTER_OP. */
    BCMI_LTSW_PROFILE_FLEXCTR_ING_PIPE1 = 42,

    /*! PROFILE_ING_COUNTER_OP. */
    BCMI_LTSW_PROFILE_FLEXCTR_ING_PIPE2 = 43,

    /*! PROFILE_ING_COUNTER_OP. */
    BCMI_LTSW_PROFILE_FLEXCTR_ING_PIPE3 = 44,

    /*! PROFILE_ING_COUNTER_OP. */
    BCMI_LTSW_PROFILE_FLEXCTR_ING_PIPE4 = 45,

    /*! PROFILE_ING_COUNTER_OP. */
    BCMI_LTSW_PROFILE_FLEXCTR_ING_PIPE5 = 46,

    /*! PROFILE_ING_COUNTER_OP. */
    BCMI_LTSW_PROFILE_FLEXCTR_ING_PIPE6 = 47,

    /*! PROFILE_ING_COUNTER_OP. */
    BCMI_LTSW_PROFILE_FLEXCTR_ING_PIPE7 = 48,

    /*! PROFILE_EGR_COUNTER_OP. */
    BCMI_LTSW_PROFILE_FLEXCTR_EGR_PIPE1 = 49,

    /*! PROFILE_EGR_COUNTER_OP. */
    BCMI_LTSW_PROFILE_FLEXCTR_EGR_PIPE2 = 50,

    /*! PROFILE_EGR_COUNTER_OP. */
    BCMI_LTSW_PROFILE_FLEXCTR_EGR_PIPE3 = 51,

    /*! PROFILE_EGR_COUNTER_OP. */
    BCMI_LTSW_PROFILE_FLEXCTR_EGR_PIPE4 = 52,

    /*! PROFILE_EGR_COUNTER_OP. */
    BCMI_LTSW_PROFILE_FLEXCTR_EGR_PIPE5 = 53,

    /*! PROFILE_EGR_COUNTER_OP. */
    BCMI_LTSW_PROFILE_FLEXCTR_EGR_PIPE6 = 54,

    /*! PROFILE_EGR_COUNTER_OP. */
    BCMI_LTSW_PROFILE_FLEXCTR_EGR_PIPE7 = 55,

    /*! PROFILE_ING_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_ING_LKUP_PIPE0 = 56,

    /*! PROFILE_ING_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_ING_LKUP_PIPE1 = 57,

    /*! PROFILE_ING_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_ING_LKUP_PIPE2 = 58,

    /*! PROFILE_ING_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_ING_LKUP_PIPE3 = 59,

    /*! PROFILE_ING_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_ING_LKUP_PIPE4 = 60,

    /*! PROFILE_ING_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_ING_LKUP_PIPE5 = 61,

    /*! PROFILE_ING_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_ING_LKUP_PIPE6 = 62,

    /*! PROFILE_ING_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_ING_LKUP_PIPE7 = 63,

    /*! PROFILE_ING_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_ING_FWD_PIPE0 = 64,

    /*! PROFILE_ING_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_ING_FWD_PIPE1 = 65,

    /*! PROFILE_ING_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_ING_FWD_PIPE2 = 66,

    /*! PROFILE_ING_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_ING_FWD_PIPE3 = 67,

    /*! PROFILE_ING_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_ING_FWD_PIPE4 = 68,

    /*! PROFILE_ING_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_ING_FWD_PIPE5 = 69,

    /*! PROFILE_ING_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_ING_FWD_PIPE6 = 70,

    /*! PROFILE_ING_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_ING_FWD_PIPE7 = 71,

    /*! PROFILE_EGR_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_EGR_PIPE0 = 72,

    /*! PROFILE_EGR_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_EGR_PIPE1 = 73,

    /*! PROFILE_EGR_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_EGR_PIPE2 = 74,

    /*! PROFILE_EGR_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_EGR_PIPE3 = 75,

    /*! PROFILE_EGR_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_EGR_PIPE4 = 76,

    /*! PROFILE_EGR_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_EGR_PIPE5 = 77,

    /*! PROFILE_EGR_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_EGR_PIPE6 = 78,

    /*! PROFILE_EGR_STATE_OP. */
    BCMI_LTSW_PROFILE_FLEXSTATE_EGR_PIPE7 = 79,

    /*! L3_IP_OPTION_CONTROL_PROFILE. */
    BCMI_LTSW_PROFILE_L3_IPV4_OPTION_CONTROL = 80,

    /*! PROFILE_TNL_DECAP_PORT. */
    BCMI_LTSW_PROFILE_TNL_DECAP_PORT = 81,

    /*! TNL_MPLS_DST_MAC. */
    BCMI_LTSW_PROFILE_MPLS_DST_MAC = 82,

    /*! TNL_MPLS_CTR_ING_EFLEX_ACTION. */
    BCMI_LTSW_PROFILE_MPLS_FLEX_ACTION = 83,

    /*! L3_IPV4_MC_CTR_ING_EFLEX_ACTION. */
    BCMI_LTSW_PROFILE_V4MC_FLEX_ACTION = 84,

    /*! L3_SRC_IPV4_UC_CTR_ING_EFLEX_ACTION. */
    BCMI_LTSW_PROFILE_V4SRC_FLEX_ACTION = 85,

    /*! L3_IPV4_UC_ROUTE_CTR_ING_EFLEX_ACTION. */
    BCMI_LTSW_PROFILE_V4UC_FLEX_ACTION = 86,

    /*! L3_IPV6_MC_CTR_ING_EFLEX_ACTION. */
    BCMI_LTSW_PROFILE_V6MC_FLEX_ACTION = 87,

    /*! L3_SRC_IPV6_UC_CTR_ING_EFLEX_ACTION. */
    BCMI_LTSW_PROFILE_V6SRC_FLEX_ACTION = 88,

    /*! L3_IPV6_UC_ROUTE_CTR_ING_EFLEX_ACTION. */
    BCMI_LTSW_PROFILE_V6UC_FLEX_ACTION = 89,

    /*! L3_MC_NHOP_CTR_EGR_EFLEX_ACTION. */
    BCMI_LTSW_PROFILE_L3MC_NHOP_FLEX_ACTION = 90,

    /*! L3_UC_NHOP_CTR_EGR_EFLEX_ACTION. */
    BCMI_LTSW_PROFILE_L3UC_NHOP_FLEX_ACTION = 91,

    /*! TNL_MPLS_NHOP_CTR_EGR_EFLEX_ACTION. */
    BCMI_LTSW_PROFILE_MPLS_NHOP_FLEX_ACTION = 92,

    /*! ECMP_CTR_ING_EFLEX_ACTION. */
    BCMI_LTSW_PROFILE_L3_ECMP_FLEX_ACTION = 93,

    /*! L2_DST_BLOCK. */
    BCMI_LTSW_PROFILE_L2_DST_BLOCK = 94,

    /*! FP_DESTINATION_COS_Q_MAP. */
    BCMI_LTSW_PROFILE_FP_DESTINATION_COS_Q_MAP = 95,

    /*! IFP EGR PORT BLOCK MASK. */
    BCMI_LTSW_PROFILE_FIELD_EGR_MASK = 96,

    /*! COSQ_MOD_DEST. */
    BCMI_LTSW_PROFILE_COSQ_MOD_DEST = 97,

    /*! IFP EGR PORT BLOCK REDIRECT. */
    BCMI_LTSW_PROFILE_FIELD_REDIRECT = 98,

    /*! TRUNK CTR ING EFLEX ACTION. */
    BCMI_LTSW_PROFILE_TRUNK_CTR_ING_EFLEX_ACTION = 99,

    /*! ECMP_OVERLAY_CTR_ING_EFLEX_ACTION. */
    BCMI_LTSW_PROFILE_L3_OL_ECMP_FLEX_ACTION = 100,

    /*! PROFILE_SYSTEM_PORT. */
    BCMI_LTSW_PROFILE_SYSTEM_PORT_PIPE1 = 101,

    /*! PROFILE_SYSTEM_PORT. */
    BCMI_LTSW_PROFILE_SYSTEM_PORT_PIPE2 = 102,

    /*! PROFILE_SYSTEM_PORT. */
    BCMI_LTSW_PROFILE_SYSTEM_PORT_PIPE3 = 103,

    /*! PORT_DVP_MTU_PROFILE. */
    BCMI_LTSW_PROFILE_FLOW_DVP_MTU_PROFILE = 104,

    /*! PORT_SVP_ING_EFLEX_ACTION. */
    BCMI_LTSW_PROFILE_SVP_FLEX_ACT_PROFILE = 105,

    /*! IFP EGR PORT CHECK FOR DELAYED REDIRECT. */
    BCMI_LTSW_PROFILE_FIELD_DELAYED_REDIRECT = 106,

    /*! IFP EGR PORT CHECK FOR DELAYED DROP. */
    BCMI_LTSW_PROFILE_FIELD_DELAYED_DROP = 107,

    /*! MIRROR_EGR_ZERO_PAYLOAD_PROFILE. */
    BCMI_LTSW_PROFILE_MIRROR_ZERO_PAYLOAD = 108,

    /*! DEVICE_TS_PTP_MSG_CONTROL_PROFILE. */
    BCMI_LTSW_PROFILE_DEVICE_TS_PTP_MSG_CONTROL = 109,

    /*! Always the last. */
    BCMI_LTSW_PROFILE_COUNT
} bcmi_ltsw_profile_hdl_t;

/*!
 * \brief Callback function to calculate profile entry hash.
 *
 * This callback function is used to calculate profile entry hash(signature).
 */
typedef int (*bcmi_ltsw_profile_hash_cb)(
    /*! Device Number. */
    int       unit,
    /*!
     * Pointer to the profile table set memory which is defined by user and
     * can be used for hash calculation.
     */
    void      *entries,
    /*! Number of entries in the profile set for master. */
    int       entries_per_set,
    /*! Pointer to hash (signature). */
    uint32_t  *hash
);

/*!
 * \brief Callback function to compare profile set.
 *
 * This callback function is used to compare profile set.
 */
typedef int (*bcmi_ltsw_profile_cmp_cb)(
    /*! Device Number. */
    int      unit,
    /*!
     * Pointer to the profile set memory which is defined by user and
     * can be used for the comparison on profile set.
     */
    void     *entries,
    /*! Number of entries in the profile set for master. */
    int      entries_per_set,
    /*! Entry index of profile table for master. */
    int      index,
    /*! Result of comparison, 0 - equal. */
    int      *cmp_result
);

/*! Profile operation flag to disable profile set match. */
#define BCMI_LTSW_PROFILE_F_MATACH_DISABLE (1 << 0)

/*!
 * \brief Initialize the resource of profile index management.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_profile_init(int unit);

/*!
 * \brief Clear the resource of profile index management.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval None.
 */
extern void
bcmi_ltsw_profile_cleanup(int unit);

/*!
 * \brief Register profile configuration information.
 *
 * This function is used to register the configuration information of profile
 * table based on the profile handle. A profile structure can be formed by
 * single profile table, or a combination of multiple profile tables. The first
 * profile in the combination is called master profile, others can be treated as
 * slave profiles if any.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_hdl_array    Pointer to profile handle array.
 * \param [in]  entry_idx_min_array  Pointer to minimum entry index array.
 * \param [in]  entry_idx_max_array  Pointer to maximum entry index array.
 * \param [in]  array_count          Number of entries in array.
 * \param [in]  hash_func            Callback function to calculate profile
 * set hash(signature).
 * \param [in]  cmp_func             Callback function to compare profile set.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_profile_register(int unit, bcmi_ltsw_profile_hdl_t *profile_hdl_array,
                           int *entry_idx_min_array, int *entry_idx_max_array,
                           int array_count, bcmi_ltsw_profile_hash_cb hash_func,
                           bcmi_ltsw_profile_cmp_cb cmp_func);

/*!
 * \brief Check if the master profile is registered.
 *
 * This function is used to check if the master profile is registered.
 *
 * \param [in]  unit               Unit Number.
 * \param [in]  profile_hdl        Profile handle for master.
 *
 * \retval Zero                    Profile has not been registered.
 * \retval None-zero               Profile has been registered.
 */
extern int
bcmi_ltsw_profile_register_check(int unit, bcmi_ltsw_profile_hdl_t profile_hdl);

/*!
 * \brief Unregister profile configuration information.
 *
 * This function is used to unregister the configuration information of profile
 * table based on the profile handle.
 *
 * \param [in]  unit               Unit Number.
 * \param [in]  profile_hdl        Profile handle for master.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_profile_unregister(int unit, bcmi_ltsw_profile_hdl_t profile_hdl);

/*!
 * \brief Allocate profile entry index.
 *
 * This function is used to allocate base index to the entries in profile table
 * based on a profile set(one or more entries). This routine searches for a
 * matching set in the profile table. If a matching set is found, the ref
 * count for the related entries will be incremented and the base entry index
 * of master profile is returned. If a matching set is not found and a free set
 * is found, hash(signature) of the set will be updated, the reference count
 * will be incremented, and the base entry index of the profile set for master
 * will be returned. If no free set is found, an error should be returned.
 * Slave profile index can be derived from master information by profile user
 * itself or call \ref bcmi_ltsw_profile_slave_index_get optionally.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_hdl          Profile handle for master.
 * \param [in]  profile_entries      Pointer to the profile set memory which is
 * defined by user and can be used for hash calculation and profile set
 * comparison.
 * \param [in]  flags                Operation flags(BCMI_LTSW_PROFILE_F_XXX).
 * For example, user can disable profile match, and allocate profile index
 * directly.
 * \param [in]  entries_per_set      Number of entries in the set of master
 * profile table.
 * \param [out]  index               Base index to the entries in master profile
 * table.
 *
 * \retval SHR_E_EXISTS              Return an existing profile index.
 * \retval SHR_E_NONE                Return a free profile index.
 * \retval !(SHR_E_NONE|SHR_E_EXISTS)Failure.
 */
extern int
bcmi_ltsw_profile_index_allocate(int unit, bcmi_ltsw_profile_hdl_t profile_hdl,
                                 void *profile_entries, int flags,
                                 int entries_per_set, int *index);

/*!
 * \brief Free profile entry index.
 *
 * This function is used to decrease the reference count of profile set at the
 * specified base profile entry index and try to free the profile index. If the
 * reference count is decreased to ZERO, the index will be freed. If the ref
 * count is greater than ZERO, which means the profile set is still used by
 * other users, the profile index won't be freed and SHR_E_BUSY will be
 * returned accordingly.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_hdl          Profile handle for master.
 * \param [in]  index                Base index to the entries in master profile
 * table.
 *
 * \retval SHR_E_BUSY                Profile index is still used by others.
 * \retval SHR_E_NONE                Free profile index successfully.
 * \retval !(SHR_E_NONE|SHR_E_BUSY)  Failure.
 */
extern int
bcmi_ltsw_profile_index_free(int unit,
                             bcmi_ltsw_profile_hdl_t profile_hdl, int index);

/*!
 * \brief Increase the reference count of profile set.
 *
 * This function is used to increase the reference count of profile set at the
 * specified base profile entry index.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_hdl          Profile handle for master.
 * \param [in]  entries_per_set      Number of entries in the set of master
 * profile table.
 * \param [in]  index                Base index to the entries in master profile
 * table.
 * \param [in]  depth                Increased depth.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_profile_ref_count_increase(int unit,
                                     bcmi_ltsw_profile_hdl_t profile_hdl,
                                     int entries_per_set,
                                     int index, uint32_t depth);

/*!
 * \brief Decrease the reference count of profile set.
 *
 * This function is used to decrease the reference count of profile set at the
 * specified base profile entry index.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_hdl          Profile handle for master.
 * \param [in]  entries_per_set      Number of entries in the set of master
 * profile table.
 * \param [in]  index                Base index to the entries in master profile
 * table.
 * \param [in]  depth                Increased depth.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_profile_ref_count_decrease(int unit,
                                     bcmi_ltsw_profile_hdl_t profile_hdl,
                                     int entries_per_set,
                                     int index, uint32_t depth);

/*!
 * \brief Get the reference count of profile set.
 *
 * This function is used to get the reference count of profile set at the
 * specified base profile entry index.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_hdl          Profile handle for master.
 * \param [in]  index                Base index to the entries in master profile
 * table.
 * \param [OUT] ref_count            Reference count of profile set.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_profile_ref_count_get(int unit, bcmi_ltsw_profile_hdl_t profile_hdl,
                                int index, uint32_t *ref_count);

/*!
 * \brief Update hash(signature) of profile set.
 *
 * This function is used to update hash (signature) of profile set at the
 * specified base profile entry index.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_hdl          Profile handle for master.
 * \param [in]  profile_entries      Pointer to the profile set memory which is
 * defined by user and can be used for hash calculation and profile set
 * comparison.
 * \param [in]  entries_per_set      Number of entries in the set of master
 * profile table.
 * \param [in]  index                Base index to the entries in master profile
 * table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_profile_hash_update(int unit, bcmi_ltsw_profile_hdl_t profile_hdl,
                              void *profile_entries,
                              int entries_per_set, int index);

/*!
 * \brief Get slave profile index based on master profile information.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  master_profile_hdl   Profile handle for master.
 * \param [in]  slave_profile_hdl    Profile handle for slave.
 * \param [in]  master_index         Base index to the entries in master profile
 * table.
 * \param [out] slave_index          Base index to the entries in slave profile
 * table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
bcmi_ltsw_profile_slave_index_get(int unit,
                                  bcmi_ltsw_profile_hdl_t master_profile_hdl,
                                  bcmi_ltsw_profile_hdl_t slave_profile_hdl,
                                  int master_index, int *slave_index);

/*!
 * \brief Dump profile information for debug usage.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval None.
 */
extern void
bcmi_ltsw_profile_sw_dump(int unit);

#endif /* BCMI_LTSW_PROFILE_MGMT_H */
