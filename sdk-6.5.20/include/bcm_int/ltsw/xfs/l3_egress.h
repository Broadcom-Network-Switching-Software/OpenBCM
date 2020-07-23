/*! \file l3_egress.h
 *
 * L3 egress header file.
 * This file contains L3 egress definitions internal to the BCM library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_XFS_L3_EGRESS_H
#define BCMI_XFS_L3_EGRESS_H

#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/l3_egress.h>

/*! Max member in source ECMP group. */
#define ECMP_SRC_GROUP_TABLE_MAX_MEMBER 4
#define ECMP_SRC_GROUP_MAX_MEMBER       8

/*! Data structure to save the info of ECMP SRC TABLE */
typedef struct ecmp_src_cfg_s {
    /* Index. */
    uint64_t l3_src_ecmp_group_index;

    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;
#define ES_FLD_COUNT             (1 << 0)
#define ES_FLD_COUNT_GT8         (1 << 1)
#define ES_FLD_L3_OIF            (1 << 2)

    /* Count for available L3 outgoing interfaces. */
    uint64_t count;

    /* If set, L3 outgoing interfaces count is more than 8. */
    uint64_t count_gt8;

    /* Array index where the ECMP member info should start being applied. */
    uint32_t start_idx;

    /* Number of array element to be set/get. */
    uint32_t num_of_set;

    /*
     * Number of array element to be unset. The start index of unset element
     * should be start_idx + num_of_set.
     */
    uint32_t num_of_unset;

    /* L3  outgoing interface. */
    uint64_t l3_oif[ECMP_SRC_GROUP_MAX_MEMBER];
} ecmp_src_cfg_t;

/*!
 * \brief Initialization of L3 nexthop table entry zero.
 *
 * This function is to initialize entry 0 of ING_L3_NEXT_HOP_1/2 table to avoid
 * the destination info deriving from L2 host tables is overwritten by
 * the one coming from L3 nexthop tables.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
extern int
xfs_ltsw_l3_egress_l2_rsv(int unit);

/*!
 * \brief Initialize L3 egress module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_egress_init(int unit);

/*!
 * \brief De-Initialize L3 egress module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_egress_deinit(int unit);

/*!
 * \brief Get the index range of a specified egress object type.
 *
 * \param [in] unit Unit number.
 * \param [in] type Egress object type.
 * \param [out] min Min index of the egress object type.
 * \param [out] max Max index of the egress object type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid egress object type.
 */
extern int
xfs_ltsw_l3_egress_id_range_get(int unit, int type, int *min, int *max);

/*!
 * \brief Create a L3 egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] egr Egress forwarding destination info.
 * \param [in/out] idx Egress object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_egress_create(int unit, bcm_l3_egress_t *egr, int *idx);

/*!
 * \brief Destroy a L3 egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Egress object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_egress_destroy(int unit, int idx);

/*!
 * \brief Get a L3 egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Egress object ID.
 * \param [out] egr Egress object info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_egress_get(int unit, int idx, bcm_l3_egress_t *egr);

/*!
 * \brief Find a L3 egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] egr Egress object info to match.
 * \param [out] idx Egress object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_egress_find(int unit, bcm_l3_egress_t *egr, int *idx);

/*!
 * \brief Traverse L3 egress objects.
 *
 * Goes through egress objects table and runs the user callback
 * function at each valid egress objects entry passing back the
 * information for that object.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_egress_traverse(int unit,
                             bcm_l3_egress_traverse_cb trav_fn,
                             void *user_data);

/*!
 * \brief Get egress object type and nhop/ecmp id from egress object ID.
 *
 * \param [in] unit Unit number.
 * \param [in] intf Egress object ID.
 * \param [out] nh_ecmp_idx Nexthop or ECMP group ID.
 * \param [out] type Egress object type.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_egress_obj_id_resolve(int unit,
                                  bcm_if_t intf,
                                  int *nh_ecmp_idx,
                                  bcmi_ltsw_l3_egr_obj_type_t *type);

/*!
 * \brief Construct an egress object ID from type and nhop/ecmp index.
 *
 * \param [in] unit Unit number.
 * \param [in] nh_ecmp_idx Nexthop or ECMP group ID.
 * \param [in] type Egress object type.
 * \param [out] intf Egress object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_egress_obj_id_construct(int unit,
                                    int nh_ecmp_idx,
                                    bcmi_ltsw_l3_egr_obj_type_t type,
                                    bcm_if_t *intf);

/*!
 * \brief Get the L3 egress object info.
 *
 * \param [in] unit Unit Number.
 * \param [in] intf Egress object ID.
 * \param [out] info L3 egress object info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_egress_obj_info_get(int unit, bcm_if_t intf,
                                bcmi_ltsw_l3_egr_obj_info_t *info);

/*!
 * \brief Get the L3 egress object maximum and used number.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Egress object type.
 * \param [out] max Maximum number of object.
 * \param [out] used Used number of object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_egress_obj_cnt_get(int unit, bcmi_ltsw_l3_egr_obj_type_t type,
                               int *max, int *used);

/*!
 * \brief Set the dvp of given egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf Egress object ID.
 * \param [in] dvp Destination VP.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid egress object ID.
 * \retval SHR_E_NOT_FOUND The given egress object does not exist.
 */
extern int
xfs_ltsw_l3_egress_dvp_set(int unit, bcm_if_t intf, int dvp);

/*!
 * \brief Attach flexctr action to the given egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_if Interface id of a L3 egress object.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_egress_flexctr_attach(
    int unit,
    bcm_if_t intf_id,
    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach flexctr action of the given egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface id of a L3 egress object.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_egress_flexctr_detach(
    int unit,
    bcm_if_t intf_id);

/*!
 * \brief Get flex counter information of the given egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_if Interface id of a L3 egress object.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_egress_flexctr_info_get(
    int unit,
    bcm_if_t intf_id,
    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Set flex counter object value for the given egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface id of a L3 egress object.
 * \param [in] value Flex counter object value.
 *
 * The flex counter object value could be used to generate counter index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_egress_flexctr_object_set(
    int unit,
    bcm_if_t intf_id,
    uint32_t value);

/*!
 * \brief Get flex counter object value for the given egress object.
 *
 * \param [in] unit Unit Number.
 * \param [in] intf_id L3 interface ID pointing to egress object.
 * \param [out] value Flex counter object value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_egress_flexctr_object_get(
    int unit,
    bcm_if_t intf_id,
    uint32_t *value);

/*!
 * \brief Create a L3 egress object for IFA.
 *
 * \param [in] unit Unit number.
 * \param [in] egr_ifa L3 egress IFA data.
 * \param [in/out] idx Egress object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_egress_ifa_create(
    int unit,
    bcmi_ltsw_l3_egr_ifa_t *egr_ifa,
    int *idx);

/*!
 * \brief Destroy a L3 egress object for IFA.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Egress object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_egress_ifa_destroy(int unit, int idx);

/*!
 * \brief Get a L3 egress object for IFA.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Egress object ID.
 * \param [out] egr_ifa L3 egress IFA data.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_egress_ifa_get(
    int unit,
    int idx,
    bcmi_ltsw_l3_egr_ifa_t *egr_ifa);

/*!
 * \brief Traverse L3 egress objects for IFA.
 *
 * Goes through egress objects table and runs the user callback
 * function at each valid egress objects entry passing back the
 * information for that object.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Callback function.
 * \param [in] cb Callback function to contruct IFA header from L3 egress.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_egress_ifa_traverse(
    int unit,
    bcm_ifa_header_traverse_cb trav_fn,
    bcmi_ltsw_l3_egr_to_ifa_header_cb cb,
    void *user_data);

/*!
 * \brief Create an Egress ECMP forwarding object.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_info ECMP group information.
 * \param [in] ecmp_member_count Number of elements in ecmp_member_array.
 * \param [in] ecmp_member_array Member array of egress forwarding objects.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ecmp_create(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array);

/*!
 * \brief Destroy an Egress ECMP forwarding object.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_group_id L3 ECMP forwarding object ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ecmp_destroy(
    int unit,
    bcm_if_t ecmp_group_id);

/*!
 * \brief Get an Egress ECMP forwarding object information.
 *
 * \param [in] unit Unit number.
 * \param [in/out] ecmp_info ECMP group information.
 * \param [in] ecmp_member_size Size of allocated entries in ecmp_member_array.
 * \param [out] ecmp_member_array Member array of egress forwarding objects.
 * \param [out] ecmp_member_count Number of elements in ecmp_member_array.
 *
 * If the ecmp_member_size is 0, then ecmp_member_array is ignored and
 * ecmp_member_count is filled in with the number of members that would have
 * been filled into ecmp_member_array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ecmp_get(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_size,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    int *ecmp_member_count);

/*!
 * \brief Add a member to an Egress ECMP forwarding object.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_group_id Egress ECMP forwarding object ID.
 * \param [in] ecmp_member Pointer to ECMP objects member structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ecmp_member_add(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_member_t *ecmp_member);

/*!
 * \brief Delete a member from an Egress ECMP forwarding object.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_group_id Egress ECMP forwarding object ID.
 * \param [in] ecmp_member Pointer to ECMP objects member structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ecmp_member_del(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_member_t *ecmp_member);

/*!
 * \brief Delete all member from an Egress ECMP forwarding object.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_group_id Egress ECMP forwarding object ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ecmp_member_del_all(
    int unit,
    bcm_if_t ecmp_group_id);

/*!
 * \brief Find an Egress ECMP forwarding object by given member info.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_member_count Number of elements in ecmp_member_array.
 * \param [in] ecmp_member_array ECMP member array.
 * \param [out] ecmp_info ECMP group info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ecmp_find(
    int unit,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    bcm_l3_egress_ecmp_t *ecmp_info);

/*!
 * \brief Traverse L3 ECMP objects.
 *
 * Goes through Egress ECMP forwarding objects table and runs the
 * user callback function at each Egress ECMP forwarding objects
 * entry passing back the information for that ECMP object.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fun Callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ecmp_trav(
    int unit,
    bcm_l3_ecmp_traverse_cb trav_fn,
    void *user_data);

/*!
 * \brief Get the maximum ECMP paths supported by LT.
 *
 * \param [in] unit Unit number.
 * \param [in] attr ECMP attributes.
 * \param [out] max Maximum number of paths for ECMP.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xfs_ltsw_l3_ecmp_max_paths_get(int unit, uint32_t attr, int *max_paths);

/*!
 * \brief Set the ECMP hash output selection control info.
 *
 * \param [in] unit Unit number.
 * \param [in] underlay Use 1 for underlay ECMP, 0 for overlay ECMP.
 * \param [in] flow_based Use 1 for flow-based hash, 0 for port-based hash.
 *
 * \retval SHR_E_NONE NO errors.
 */
extern int
xfs_ltsw_l3_ecmp_hash_ctrl_set(int unit, int underlay, int flow_based);

/*!
 * \brief Get the ECMP hash output selection control info.
 *
 * \param [in] unit Unit number.
 * \param [in] underlay Use 1 for underlay ECMP, 0 for overlay ECMP.
 * \param [out] flow_based Use 1 for flow-based hash, 0 for port-based hash.
 *
 * \retval SHR_E_NONE NO errors.
 */
extern int
xfs_ltsw_l3_ecmp_hash_ctrl_get(int unit, int underlay, int *flow_based);

/*!
 * \brief Configure the ECMP flow-based hash output selection.
 *
 * \param [in] unit Unit number.
 * \param [in] type ECMP hash output selection configure type.
 * \param [in] arg Configuration value.
 *
 * \retval SHR_E_NONE NO errors.
 */
extern int
xfs_ltsw_l3_ecmp_flow_hash_set(int unit,
                               bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
                               int arg);

/*!
 * \brief Get the ECMP flow-based hash output selection info.
 *
 * \param [in] unit Unit number.
 * \param [in] type ECMP hash output selection configure type.
 * \param [out] arg Configuration value.
 *
 * \retval SHR_E_NONE NO errors.
 */
extern int
xfs_ltsw_l3_ecmp_flow_hash_get(int unit,
                               bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
                               int *arg);

/*!
 * \brief Configure the ECMP port-based hash output selection.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type ECMP hash output selection configure type.
 * \param [in] arg Configuration value.
 *
 * \retval SHR_E_NONE NO errors.
 */
extern int
xfs_ltsw_l3_ecmp_port_hash_set(int unit,
                               int port,
                               bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
                               int arg);

/*!
 * \brief Get the ECMP port-based hash output selection control.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type ECMP hash output selection configure type.
 * \param [out] arg Configuration value.
 *
 * \retval SHR_E_NONE NO errors.
 */
extern int
xfs_ltsw_l3_ecmp_port_hash_get(int unit,
                               int port,
                               bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
                               int *arg);

/*!
 * \brief Set ECMP dynamic load balancing member status.
 *
 * \param [in] unit Unit number.
 * \param [in] intf L3 interface ID pointing to an Egress Object.
 * \param [in] status ECMP member status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ecmp_member_status_set(int unit, bcm_if_t intf, int status);

/*!
 * \brief Get ECMP dynamic load balancing member status.
 *
 * \param [in] unit Unit number.
 * \param [in] intf L3 interface ID pointing to an Egress Object.
 * \param [out] status ECMP member status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_ecmp_member_status_get(int unit, bcm_if_t intf, int *status);

/*!
 * \brief Set the ECMP dlb monitor configurations.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid ECMP Identifier.
 * \param [in] dlb_mon_cfg ECMP DLB monitor configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xfs_ltsw_l3_ecmp_dlb_mon_config_set(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg);

/*!
 * \brief Get the ECMP dlb monitor configurations.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid ECMP Identifier.
 * \param [out] dlb_mon_cfg ECMP DLB monitor configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xfs_ltsw_l3_ecmp_dlb_mon_config_get(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg);

/*!
 * \brief Set the DLB statistics based on the ECMP interface.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_intf ECMP interface id.
 * \param [in] type L3 ECMP DLB statistics type..
 * \param [in] value 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xfs_ltsw_l3_ecmp_dlb_stat_set(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t value);

/*!
 * \brief Get the DLB statistics based on the ECMP interface.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_intf ECMP interface id.
 * \param [in] type L3 ECMP DLB statistics type..
 * \param [out] value 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xfs_ltsw_l3_ecmp_dlb_stat_get(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t *value);

/*!
 * \brief Get the DLB statistics based on the ECMP interface in sync mode.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_intf ECMP interface id.
 * \param [in] type L3 ECMP DLB statistics type..
 * \param [out] value 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xfs_ltsw_l3_ecmp_dlb_stat_sync_get(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t *value);

/*!
 * \brief Allocate an entry from ECMP Level 0 member table.
 *
 * \param [in] unit Unit Number.
 * \param [in] dest_info Destination info.
 * \param [out] dest_index Entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_ecmp_member_dest_alloc(int unit,
                                bcmi_ltsw_ecmp_member_dest_info_t *dest_info,
                                int *dest_index);

/*!
 * \brief Get ECMP MEMBER destination info.
 *
 * \param [in] unit Unit Number.
 * \param [in] dest_index Entry index.
 * \param [out] dest_info Destination info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_ecmp_member_dest_get(int unit,
                              int dest_index,
                              bcmi_ltsw_ecmp_member_dest_info_t *dest_info);

/*!
 * \brief Free an entry to ECMP Level 0 member table.
 *
 * \param [in] unit Unit Number.
 * \param [in] dest_index Destination index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_ecmp_member_dest_free(int unit, int dest_index);

/*!
 * \brief Update an entry in ECMP Level 0 member table.
 *
 * \param [in] unit Unit Number.
 * \param [in] dest_info Destination info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_ecmp_member_dest_update(int unit,
                                 bcmi_ltsw_ecmp_member_dest_info_t *dest_info);

/*!
 * \brief Get L3 OIF in source next hop entry.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 * \param [out] l3_oif L3 OIF.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_snh_ing_l3_oif_get(int unit, int nhidx, uint32_t *l3_oif);

/*!
 * \brief Clear trunk port membership from EGR_SYSPORT_TO_NHI
 *
 * \param [in] unit Unit number.
 * \param [in] tid Trunk ID.
 * \param [out] member_cnt Count of trunk members to be deleted.
 * \param [out] member_arr Trunk member gports to be deleted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid egress object type.
 */
extern int
xfs_ltsw_l3_egress_trunk_member_delete(int unit, bcm_trunk_t trunk_id,
                                       int member_cnt, bcm_port_t *member_arr);

/*!
 * \brief Active trunk port membership in EGR_SYSPORT_TO_NHI
 *
 * \param [in] unit Unit number.
 * \param [in] tid Trunk ID.
 * \param [out] member_cnt Count of trunk members to be added.
 * \param [out] member_arr Trunk member gports to be added.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid egress object type.
 */
extern int
xfs_ltsw_l3_egress_trunk_member_add(int unit, bcm_trunk_t trunk_id,
                                    int member_cnt, bcm_port_t *member_arr);
#endif /* BCMI_XFS_L3_EGRESS_H */
