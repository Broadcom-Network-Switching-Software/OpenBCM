/*! \file index_table_mgmt.h
 *
 * Index-based table management header file.
 * This file contains the header information of index-based table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_IDX_TBL_MGMT_H
#define BCMI_LTSW_IDX_TBL_MGMT_H

#include <sal/sal_mutex.h>
#include <shr/shr_bitop.h>

/*! Only allocate a new slot. */
#define BCMI_LTSW_IDX_TBL_OP_MATCH_DISABLE        0x00000001
/*! Do not update LT table. */
#define BCMI_LTSW_IDX_TBL_OP_WRITE_DISABLE        0x00000002
/*! With ID. */
#define BCMI_LTSW_IDX_TBL_OP_WITH_ID              0x00000004
/*! Replace. */
#define BCMI_LTSW_IDX_TBL_OP_REPLACE              0x00000008
/*! Skip entry 0. */
#define BCMI_LTSW_IDX_TBL_OP_SKIP_INDEX_ZERO      0x00000010

typedef int (*entry_func_hash) (int unit, void *entry, uint16_t *hash);
typedef int (*entry_func_cmp) (int unit, int index, void *entry, int *result);
typedef int (*entry_func_insert) (int unit, uint32_t flags,
                                  int index, void *entry);
typedef int (*entry_func_del) (int unit, int index);

typedef enum bcmi_ltsw_idx_tbl_id_e {
    /*! Egress L3 tunnel initiator table. */
    BCMI_LTSW_IDX_TBL_ID_EGR_L3_TNL = 0,

    /*! Multicast aggregate list member remap table. */
    BCMI_LTSW_IDX_TBL_ID_TM_MC_AGG_LIST_MEMBER_REMAP = 1,

    /*! EGR_MPLS_VPN table. */
    BCMI_LTSW_IDX_TBL_ID_EGR_MPLS_VPN = 2,

    /*! Egress L3 tunnel initiator IPv6 table. */
    BCMI_LTSW_IDX_TBL_ID_EGR_L3_IP6_TNL = 3,

    /*! Egress MPLS tunnel initiator table. */
    BCMI_LTSW_IDX_TBL_ID_TNL_MPLS_ENCAP = 4,

    /*! L2_MC_GROUP table. */
    BCMI_LTSW_IDX_TBL_ID_L2_MC_GROUP = 5,

    /*! INT action profile table. */
    BCMI_LTSW_IDX_TBL_ID_INT_ACTION_PROFILE = 5,

    /*! INT opaque data profile table. */
    BCMI_LTSW_IDX_TBL_ID_INT_OPAQUE_DATA_PROFILE = 6,

    /*! INT metadata profile table. */
    BCMI_LTSW_IDX_TBL_ID_INT_METADATA_PROFILE = 7,

    /*! INT metadata select profile table. */
    BCMI_LTSW_IDX_TBL_ID_INT_METADATA_SELECT_PROFILE = 8,

    /*! INT metadata construct profile table. */
    BCMI_LTSW_IDX_TBL_ID_INT_METADATA_CONSTRUCT_PROFILE = 9,

    /*! Always at last. */
    BCMI_LTSW_IDX_TBL_ID_CNT
} bcmi_ltsw_idx_tbl_id_t;

typedef enum bcmi_ltsw_idx_tbl_egr_l3_tnl_user_id {
    /*! IP tunnel. */
    BCMI_EGR_L3_TNL_USER_ID_IP_TNL = 0,

    /*! MPLS. */
    BCMI_EGR_L3_TNL_USER_ID_MPLS = 1,

    /*! Flex flow. */
    BCMI_EGR_L3_TNL_USER_ID_FLEX_FLOW = 2,

    /*! Always at last. */
    BCMI_EGR_L3_TNL_USER_ID_CNT
} bcmi_ltsw_idx_tbl_egr_l3_tnl_user_id;

typedef enum bcmi_ltsw_idx_tbl_tm_mc_agg_list_member_remap_user_id {
    /*! Multicast. */
    BCMI_TM_MC_AGG_LIST_MEMBER_REMAP_USER_ID_MC = 0,

    /*! Always at last. */
    BCMI_TM_MC_AGG_LIST_MEMBER_REMAP_USER_ID_CNT
} bcmi_ltsw_idx_tbl_tm_mc_agg_list_member_remap_user_id;

typedef enum bcmi_ltsw_idx_tbl_egr_mpls_vpn_user_id {
    /*!MPLS. */
    BCMI_EGR_MPLS_VPN_USER_ID_MPLS = 0,

    /*! INT. */
    BCMI_EGR_MPLS_VPN_USER_ID_INT = 1,

    /*! Always at last. */
    BCMI_EGR_MPLS_VPN_USER_ID_CNT
} bcmi_ltsw_idx_tbl_egr_mpls_vpn_user_id;

typedef enum bcmi_ltsw_idx_tbl_egr_l3_ip6_tnl_user_id {
    /*! IP tunnel for IPv6. */
    BCMI_EGR_L3_IP6_TNL_USER_ID_IP6_TNL = 0,

    /*! Flex flow. */
    BCMI_EGR_L3_IP6_TNL_USER_ID_FLEX_FLOW = 1,

    /*! Always at last. */
    BCMI_EGR_L3_IP6_TNL_USER_ID_CNT
} bcmi_ltsw_idx_tbl_egr_l3_ip6_tnl_user_id;

typedef enum bcmi_ltsw_idx_tbl_tnl_mpls_encap_user_id {
    /*! MPLS tunnel. */
    BCMI_TNL_MPLS_ENCAP_USER_ID_MPLS = 0,

    /*! Always at last. */
    BCMI_TNL_MPLS_ENCAP_USER_ID_CNT
} bcmi_ltsw_idx_tbl_tnl_mpls_encap_user_id;

typedef enum bcmi_ltsw_idx_tbl_l2_mc_group_user_id {
    /*! VLAN. */
    BCMI_L2_MC_GROUP_USER_ID_VLAN = 0,

    /*! Always at last. */
    BCMI_L2_MC_GROUP_USER_ID_CNT
} bcmi_ltsw_idx_tbl_l2_mc_group_user_id;

typedef enum bcmi_ltsw_idx_tbl_int_action_profile_user_id {
    /*! IFA action profile. */
    BCMI_INT_ACTION_PROFILE_USER_ID_IFA = 0,

    /*! IOAM action profile. */
    BCMI_INT_ACTION_PROFILE_USER_ID_IOAM = 1,

    /*! INT-DP action profile. */
    BCMI_INT_ACTION_PROFILE_USER_ID_INT_DP = 2,

    /*! Always at last. */
    BCMI_INT_ACTION_PROFILE_USER_ID_CNT
} bcmi_ltsw_idx_tbl_int_match_user_id;

typedef enum bcmi_ltsw_idx_tbl_int_opaque_data_profile_user_id {
    /*! INT. */
    BCMI_INT_OPAQUE_DATA_PROFILE_USER_ID_INT = 0,

    /*! Always at last. */
    BCMI_INT_OPAQUE_DATA_PROFILE_USER_ID_CNT
} bcmi_ltsw_idx_tbl_int_opaque_data_profile_user_id;

typedef enum bcmi_ltsw_idx_tbl_int_metadata_select_profile_user_id {
    /*! INT. */
    BCMI_INT_METADATA_SELECT_PROFILE_USER_ID_INT = 0,

    /*! Always at last. */
    BCMI_INT_METADATA_SELECT_PROFILE_USER_ID_CNT
} bcmi_ltsw_idx_tbl_int_metadata_select_profile_user_id;

typedef enum bcmi_ltsw_idx_tbl_int_metadata_construct_profile_user_id {
    /*! INT. */
    BCMI_INT_METADATA_CONSTRUCT_PROFILE_USER_ID_INT = 0,

    /*! Always at last. */
    BCMI_INT_METADATA_CONSTRUCT_PROFILE_USER_ID_CNT
} bcmi_ltsw_idx_tbl_int_metadata_construct_profile_user_id;

typedef enum bcmi_ltsw_idx_tbl_int_metadata_profile_user_id {
    /*! INT. */
    BCMI_INT_METADATA_PROFILE_USER_ID_INT = 0,

    /*! Always at last. */
    BCMI_INT_METADATA_PROFILE_USER_ID_CNT
} bcmi_ltsw_idx_tbl_int_metadata_profile_user_id;

/*
 * Information of corresponding LT tables.
 */
typedef struct bcmi_ltsw_idx_tbl_lt_info_s {
    /*! Max number of user of a given table. */
    uint8_t user_cnt;

    /*! LT name. */
    const char *tbl_name;

    /*! Field of LT to get index range. */
    const char *idx_field;
} bcmi_ltsw_idx_tbl_lt_info_t;

typedef struct bcmi_ltsw_idx_tbl_entry_info_s {
    /*! Reference count. */
    uint32_t ref_cnt;

    /*! Entry digest. */
    uint16_t hash;

    /*! ID of user of this entry. */
    int user_id;
} bcmi_ltsw_idx_tbl_entry_info_t;

typedef struct bcmi_ltsw_idx_tbl_user_info_s {
    /*! User information valid. */
    bool valid;

    /*! Hash function. */
    entry_func_hash func_hash;

    /*! Comparison function. */
    entry_func_cmp func_cmp;

    /*! Insertion function. */
    entry_func_insert func_insert;

    /*! Deletion function. */
    entry_func_del func_del;

    /*! First table index. */
    uint32_t idx_min;

    /*! Last table index. */
    uint32_t idx_max;

    /*! Max used entry index. */
    uint32_t idx_maxused;
} bcmi_ltsw_idx_tbl_user_info_t;

/*!
 * \brief Index-based table management module initialization.
 *
 * \param [in] unit           Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int bcmi_ltsw_idx_tbl_mgmt_init(int unit);

/*!
 * \brief Table management module deinit.
 *
 * \param [in] unit           Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int bcmi_ltsw_idx_tbl_mgmt_deinit(int unit);

/*!
 * \brief Register a table user to manage the specific table.
 *
 * This function is used to register a table user to manage the specific table.
 *
 * \param [in] unit           Unit number.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User Id.
 * \param [in] lt_info        LT information.
 * \param [in] user_info      User information.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_idx_tbl_register(int unit, bcmi_ltsw_idx_tbl_id_t tbl_id,
                           int user_id,
                           bcmi_ltsw_idx_tbl_lt_info_t *lt_info,
                           bcmi_ltsw_idx_tbl_user_info_t *user_info);

/*!
 * \brief Unregister a user from the specific index table.
 *
 * This function is used to unregister a user from the specific table.
 *
 * \param [in] unit           Unit number.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User ID, -1 means all users.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int bcmi_ltsw_idx_tbl_unregister(int unit, bcmi_ltsw_idx_tbl_id_t tbl_id,
                                        int user_id);

/*!
 * \brief Allocate an entry from specific table.
 *
 * This function is used to request for a slot from specific table.
 *
 * \param [in] unit           Unit number.
 * \param [in] flags          Operation flags.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User Id.
 * \param [in] entry          Content of entry.
 * \param [in/out] index      Entry index.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_idx_tbl_entry_alloc(int unit, uint32_t flags,
                              bcmi_ltsw_idx_tbl_id_t tbl_id, int user_id,
                              void *entry, int *index);

/*!
 * \brief Free an entry from specific table.
 *
 * This function is used to free an entry from specific table.
 *
 * \param [in] unit           Unit number.
 * \param [in] flags          Operation flags.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User Id.
 * \param [in] index          Entry index.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_idx_tbl_entry_free(int unit, uint32_t flags,
                             bcmi_ltsw_idx_tbl_id_t tbl_id, int user_id,
                             int index);

/*!
 * \brief Find an entry from specific table.
 *
 * This function is used to find an entry from specific table. If multiple
 * identical entries exist, only the first entry's index will be returned.
 * Note: if any LT operation is potentially involved (i.e. LT read/write) within
 * this function, it is assumed that the LT is properly protected by associated
 * lock before invoking this function.
 *
 * \param [in] unit           Unit number.
 * \param [in] flags          Operation flags.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User Id.
 * \param [in] entry          Content of entry.
 * \param [out] index         Index of found entry.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_idx_tbl_entry_find(int unit, uint32_t flags,
                             bcmi_ltsw_idx_tbl_id_t tbl_id, int user_id,
                             void *entry, int *index);

/*!
 * \brief Check if an entry exists in specific index.
 *
 * This function is used to check if an entry exists in specific index. If
 * multiple identical entries exist, only the first entry's index will be
 * returned.
 * Note: if any LT operation is potentially involved (i.e. LT read/write) within
 * this function, it is assumed that the LT is properly protected by associated
 * lock before invoking this function.
 *
 * \param [in] unit           Unit number.
 * \param [in] flags          Operation flags.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User Id.
 * \param [in] entry          Content of entry returned (Currently unused).
 * \param [out] index         Index of found entry.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_idx_tbl_entry_get_by_idx(int unit, uint32_t flags,
                                   bcmi_ltsw_idx_tbl_id_t tbl_id,
                                   int user_id,
                                   void *entry, int index);

/*!
 * \brief Increase reference count for an specific entry.
 *
 * This function is used to increase reference count for an specific entry.
 *
 * \param [in] unit           Unit number.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User Id.
 * \param [in] index          Entry index.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_idx_tbl_entry_ref_increase(int unit, bcmi_ltsw_idx_tbl_id_t tbl_id,
                                     int user_id,
                                     uint32_t index);

/*!
 * \brief Get reference count for an specific entry.
 *
 * This function is used to get reference count for an specific entry.
 *
 * \param [in] unit           Unit number.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User Id.
 * \param [in] index          Entry index.
 * \param [out] ref_cnt       Reference count.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_idx_tbl_entry_ref_get(int unit, bcmi_ltsw_idx_tbl_id_t tbl_id,
                                int user_id, uint32_t index,
                                uint32_t *ref_cnt);

/*!
 * \brief Update hash data for specific entry.
 *
 * This function is used to update hash data for specific entry.
 *
 * \param [in] unit           Unit number.
 * \param [in] tbl_id         Id of the specific table.
 * \param [in] user_id        User Id.
 * \param [in] entry          Entry information.
 * \param [in] index          Entry index.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_idx_tbl_entry_hash_update(int unit, bcmi_ltsw_idx_tbl_id_t tbl_id,
                                    int user_id, void *entry,
                                    uint32_t index);

#endif /* BCMI_LTSW_IDX_TBE_MGMT */
