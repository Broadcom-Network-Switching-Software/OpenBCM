/*! \file l3_egress.h
 *
 * BCM L3 egress module APIs and data structures used only internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_MBCM_L3_EGRESS_H
#define BCMI_LTSW_MBCM_L3_EGRESS_H

#include <bcm/l3.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/l3_egress.h>
#include <bcm_int/ltsw/l3_egress_int.h>

typedef int (*l3_egress_init_f)(int unit);

typedef int (*l3_egress_l2_rsv_f)(int unit);

typedef int (*l3_egress_deinit_f)(int unit);

typedef int (*l3_egress_id_range_get_f)(
    int unit,
    int type,
    int *min,
    int *max);

typedef int (*l3_egress_create_f)(
    int unit,
    bcm_l3_egress_t *egr,
    int *idx);

typedef int (*l3_egress_destroy_f)(
    int unit,
    int idx);

typedef int (*l3_egress_get_f)(
    int unit,
    int idx,
    bcm_l3_egress_t *egr);

typedef int (*l3_egress_find_f)(
    int unit,
    bcm_l3_egress_t *egr,
    int *idx);

typedef int (*l3_egress_traverse_f)(
    int unit,
    bcm_l3_egress_traverse_cb trav_fn,
    void *user_data);

typedef int (*l3_egress_obj_id_resolve_f)(
    int unit,
    bcm_if_t intf,
    int *nhidx,
    bcmi_ltsw_l3_egr_obj_type_t *type);

typedef int (*l3_egress_obj_id_construct_f)(
    int unit,
    int nhidx,
    bcmi_ltsw_l3_egr_obj_type_t type,
    bcm_if_t *intf);

typedef int (*l3_egress_obj_info_get_f)(
    int unit,
    bcm_if_t intf,
    bcmi_ltsw_l3_egr_obj_info_t *info);

typedef int (*l3_egress_obj_cnt_get_f)(
    int unit,
    bcmi_ltsw_l3_egr_obj_type_t type,
    int *max,
    int *used);

typedef int (*l3_egress_dvp_set_f)(
    int unit,
    bcm_if_t intf,
    int dvp);

typedef int (*l3_egress_flexctr_attach_f)(
    int unit,
    bcm_if_t intf_id,
    bcmi_ltsw_flexctr_counter_info_t *info);

typedef int (*l3_egress_flexctr_detach_f)(
    int unit,
    bcm_if_t intf_id);

typedef int (*l3_egress_flexctr_info_get_f)(
    int unit,
    bcm_if_t intf_id,
    bcmi_ltsw_flexctr_counter_info_t *info);

typedef int (*l3_egress_flexctr_object_set_f)(
    int unit,
    bcm_if_t intf_id,
    uint32_t value);

typedef int (*l3_egress_flexctr_object_get_f)(
    int unit,
    bcm_if_t intf_id,
    uint32_t *value);

typedef int (*l3_egress_ifa_create_f)(
    int unit,
    bcmi_ltsw_l3_egr_ifa_t *egr_ifa,
    int *idx);

typedef int (*l3_egress_ifa_destroy_f)(
    int unit,
    int idx);

typedef int (*l3_egress_ifa_get_f)(
    int unit,
    int idx,
    bcmi_ltsw_l3_egr_ifa_t *egr_ifa);

typedef int (*l3_egress_ifa_traverse_f)(
    int unit,
    bcm_ifa_header_traverse_cb trav_fn,
    bcmi_ltsw_l3_egr_to_ifa_header_cb cb,
    void *user_data);

typedef int (*l3_ecmp_create_f)(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array);

typedef int (*l3_ecmp_destroy_f)(
    int unit,
    bcm_if_t ecmp_group_id);

typedef int (*l3_ecmp_get_f)(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_size,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    int *ecmp_member_count);

typedef int (*l3_ecmp_member_add_f)(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_member_t *ecmp_member);

typedef int (*l3_ecmp_member_del_f)(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_member_t *ecmp_member);

typedef int (*l3_ecmp_member_del_all_f)(
    int unit,
    bcm_if_t ecmp_group_id);

typedef int (*l3_ecmp_find_f)(
    int unit,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    bcm_l3_egress_ecmp_t *ecmp_info);

typedef int (*l3_ecmp_trav_f)(
    int unit,
    bcm_l3_ecmp_traverse_cb trav_fn,
    void *user_data);

typedef int (*l3_ecmp_max_paths_get_f)(
    int unit,
    uint32_t attr,
    int *max_paths);

typedef int (*l3_ecmp_hash_ctrl_set_f)(
    int unit,
    int underlay,
    int flow_based);

typedef int (*l3_ecmp_hash_ctrl_get_f)(
    int unit,
    int underlay,
    int *flow_based);

typedef int (*l3_ecmp_flow_hash_set_f)(
    int unit,
    bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
    int arg);

typedef int (*l3_ecmp_flow_hash_get_f)(
    int unit,
    bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
    int *arg);

typedef int (*l3_ecmp_port_hash_set_f)(
    int unit,
    int port,
    bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
    int arg);

typedef int (*l3_ecmp_port_hash_get_f)(
    int unit,
    int port,
    bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
    int *arg);

typedef int (*l3_ecmp_member_status_set_f)(
    int unit,
    bcm_if_t intf,
    int status);

typedef int (*l3_ecmp_member_status_get_f)(
    int unit,
    bcm_if_t intf,
    int *status);

typedef int (*l3_ecmp_ethertype_set_f)(
    int unit,
    uint32_t flags,
    int ethertype_count,
    int *ethertype_array);

typedef int (*l3_ecmp_ethertype_get_f)(
    int unit,
    int ethertype_max,
    uint32_t *flags,
    int *ethertype_array,
    int *ethertype_count);

typedef int (*l3_ecmp_dlb_mon_config_set_f)(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg);

typedef int (*l3_ecmp_dlb_mon_config_get_f)(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg);

typedef int (*ecmp_member_dest_alloc_f)(
    int unit,
    bcmi_ltsw_ecmp_member_dest_info_t *dest_info,
    int *dest_index);

typedef int (*ecmp_member_dest_get_f)(
    int unit,
    int dest_index,
    bcmi_ltsw_ecmp_member_dest_info_t *dest_info);

typedef int (*ecmp_member_dest_free_f)(
    int unit,
    int dest_index);

typedef int (*ecmp_member_dest_update_f)(
    int unit,
    bcmi_ltsw_ecmp_member_dest_info_t *dest_info);

typedef int (*l3_egress_size_get_f)(
    int unit,
    int *size);

typedef int (*l3_egress_count_get_f)(
    int unit,
    int *count);

typedef int (*l3_ecmp_size_get_f)(
    int unit,
    int *size);

typedef int (*l3_ecmp_count_get_f)(
    int unit,
    int *count);

typedef int (*l3_ecmp_flexctr_attach_f)(
    int unit,
    bcm_if_t intf_id,
    bcmi_ltsw_flexctr_counter_info_t *info);

typedef int (*l3_ecmp_flexctr_detach_f)(
    int unit,
    bcm_if_t intf_id);

typedef int (*l3_ecmp_flexctr_info_get_f)(
    int unit,
    bcm_if_t intf_id,
    bcmi_ltsw_flexctr_counter_info_t *info);

typedef int (*l3_ecmp_flexctr_object_set_f)(
    int unit,
    bcm_if_t intf_id,
    uint32_t value);

typedef int (*l3_ecmp_flexctr_object_get_f)(
    int unit,
    bcm_if_t intf_id,
    uint32_t *value);

typedef int (*l3_egress_tbl_db_get_f)(
    int unit,
    bcmint_ltsw_l3_egress_tbl_db_t *tbl_db);

typedef int (*l3_ecmp_tbl_db_get_f)(
    int unit,
    bcmint_ltsw_l3_egress_tbl_db_t *tbl_db);

typedef int (*l3_ecmp_flexctr_action_tbl_db_get_f)(
    int unit,
    bcmint_ltsw_l3_egress_tbl_db_t *tbl_db);

typedef int (*l3_overlay_table_init_f)(
    int unit,
    bcmint_ltsw_l3_ol_tbl_info_t *info);

typedef int (*l3_ecmp_dlb_stat_set_f)(
    int unit,
    bcm_if_t ecmp_intf,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t value);

typedef int (*l3_ecmp_dlb_stat_get_f)(
    int unit,
    bcm_if_t ecmp_intf,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t *value);

typedef int (*l3_ecmp_dlb_stat_sync_get_f)(
    int unit,
    bcm_if_t ecmp_intf,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t *value);

typedef int (*l3_egress_add_l2tag_f)(
    int unit,
    int nhidx,
    bcmi_ltsw_l3_egr_l2_tag_info_t *l2_tag_info);

typedef int (*l3_egress_get_l2tag_f)(
    int unit,
    int nhidx,
    bcmi_ltsw_l3_egr_l2_tag_info_t *l2_tag_info);

typedef int (*l3_egress_delete_l2tag_f)(
    int unit,
    int nhidx);

typedef int (*l3_egress_php_action_add_f)(
    int unit,
    bcm_if_t intf);

typedef int (*l3_egress_php_action_del_f)(
    int unit,
    bcm_if_t intf);

typedef int (*l3_secmp_clear_f)(
    int unit);

typedef int (*l3_secmp_set_f)(
    int unit,
    int idx,
    bcm_l3_egress_ecmp_t *ecmp_info,
    bcmi_ltsw_l3_ecmp_member_info_t *member,
    int count);

typedef int (*l3_secmp_del_f)(
    int unit,
    int idx);

typedef int (*l3_secmp_member_add_f)(
    int unit,
    int idx,
    bcmi_ltsw_l3_ecmp_member_info_t *member);

typedef int (*l3_secmp_member_del_f)(
    int unit,
    int idx,
    bcmi_ltsw_l3_ecmp_member_info_t *member,
    int count,
    bcmi_ltsw_l3_ecmp_member_info_t *del_member);

typedef int (*l3_secmp_member_del_all_f)(
    int unit,
    int idx);

typedef int (*l3_egress_trunk_member_del_f)(
    int unit,
    bcm_trunk_t trunk_id,
    int member_cnt,
    bcm_port_t *member_arr);

typedef int (*l3_egress_trunk_member_add_f)(
    int unit,
    bcm_trunk_t trunk_id,
    int member_cnt,
    bcm_port_t *member_arr);

/* L3 Egress function vector driver. */
typedef struct mbcm_ltsw_l3_egress_drv_s {

    l3_egress_l2_rsv_f l3_egress_l2_rsv;

    l3_egress_init_f l3_egress_init;

    l3_egress_deinit_f l3_egress_deinit;

    l3_egress_id_range_get_f l3_egress_id_range_get;

    l3_egress_create_f l3_egress_create;

    l3_egress_destroy_f l3_egress_destroy;

    l3_egress_get_f l3_egress_get;

    l3_egress_find_f l3_egress_find;

    l3_egress_traverse_f l3_egress_traverse;

    l3_egress_obj_id_resolve_f l3_egress_obj_id_resolve;

    l3_egress_obj_id_construct_f l3_egress_obj_id_construct;

    l3_egress_obj_info_get_f l3_egress_obj_info_get;

    l3_egress_obj_cnt_get_f l3_egress_obj_cnt_get;

    l3_egress_dvp_set_f l3_egress_dvp_set;

    l3_egress_flexctr_attach_f l3_egress_flexctr_attach;

    l3_egress_flexctr_detach_f l3_egress_flexctr_detach;

    l3_egress_flexctr_info_get_f l3_egress_flexctr_info_get;

    l3_egress_flexctr_object_set_f l3_egress_flexctr_object_set;

    l3_egress_flexctr_object_get_f l3_egress_flexctr_object_get;

    l3_egress_ifa_create_f l3_egress_ifa_create;

    l3_egress_ifa_destroy_f l3_egress_ifa_destroy;

    l3_egress_ifa_get_f l3_egress_ifa_get;

    l3_egress_ifa_traverse_f l3_egress_ifa_traverse;

    l3_ecmp_create_f l3_ecmp_create;

    l3_ecmp_destroy_f l3_ecmp_destroy;

    l3_ecmp_get_f l3_ecmp_get;

    l3_ecmp_member_add_f l3_ecmp_member_add;

    l3_ecmp_member_del_f l3_ecmp_member_del;

    l3_ecmp_member_del_all_f l3_ecmp_member_del_all;

    l3_ecmp_find_f l3_ecmp_find;

    l3_ecmp_trav_f l3_ecmp_trav;

    l3_ecmp_max_paths_get_f l3_ecmp_max_paths_get;

    l3_ecmp_hash_ctrl_set_f l3_ecmp_hash_ctrl_set;

    l3_ecmp_hash_ctrl_get_f l3_ecmp_hash_ctrl_get;

    l3_ecmp_flow_hash_set_f l3_ecmp_flow_hash_set;

    l3_ecmp_flow_hash_get_f l3_ecmp_flow_hash_get;

    l3_ecmp_port_hash_set_f l3_ecmp_port_hash_set;

    l3_ecmp_port_hash_get_f l3_ecmp_port_hash_get;

    l3_ecmp_member_status_set_f l3_ecmp_member_status_set;

    l3_ecmp_member_status_get_f l3_ecmp_member_status_get;

    l3_ecmp_ethertype_set_f l3_ecmp_ethertype_set;

    l3_ecmp_ethertype_get_f l3_ecmp_ethertype_get;

    l3_ecmp_dlb_mon_config_set_f l3_ecmp_dlb_mon_config_set;

    l3_ecmp_dlb_mon_config_get_f l3_ecmp_dlb_mon_config_get;

    ecmp_member_dest_alloc_f ecmp_member_dest_alloc;

    ecmp_member_dest_get_f ecmp_member_dest_get;

    ecmp_member_dest_free_f ecmp_member_dest_free;

    ecmp_member_dest_update_f ecmp_member_dest_update;

    l3_egress_size_get_f l3_egress_size_get;

    l3_egress_count_get_f l3_egress_count_get;

    l3_ecmp_size_get_f l3_ecmp_size_get;

    l3_ecmp_count_get_f l3_ecmp_count_get;

    l3_ecmp_flexctr_attach_f l3_ecmp_flexctr_attach;

    l3_ecmp_flexctr_detach_f l3_ecmp_flexctr_detach;

    l3_ecmp_flexctr_info_get_f l3_ecmp_flexctr_info_get;

    l3_ecmp_flexctr_object_set_f l3_ecmp_flexctr_object_set;

    l3_ecmp_flexctr_object_get_f l3_ecmp_flexctr_object_get;

    l3_egress_tbl_db_get_f l3_egress_tbl_db_get;

    l3_ecmp_tbl_db_get_f l3_ecmp_tbl_db_get;

    l3_ecmp_flexctr_action_tbl_db_get_f l3_ecmp_flexctr_action_tbl_db_get;

    l3_overlay_table_init_f l3_overlay_table_init;

    l3_ecmp_dlb_stat_set_f l3_ecmp_dlb_stat_set;

    l3_ecmp_dlb_stat_get_f l3_ecmp_dlb_stat_get;

    l3_ecmp_dlb_stat_sync_get_f l3_ecmp_dlb_stat_sync_get;

    l3_egress_add_l2tag_f l3_egress_add_l2tag;

    l3_egress_get_l2tag_f l3_egress_get_l2tag;

    l3_egress_delete_l2tag_f l3_egress_delete_l2tag;

    l3_egress_php_action_add_f l3_egress_php_action_add;

    l3_egress_php_action_del_f l3_egress_php_action_del;

    l3_secmp_clear_f l3_secmp_clear;

    l3_secmp_set_f l3_secmp_set;

    l3_secmp_del_f l3_secmp_del;

    l3_secmp_member_add_f l3_secmp_member_add;

    l3_secmp_member_del_f l3_secmp_member_del;

    l3_secmp_member_del_all_f l3_secmp_member_del_all;

    l3_egress_trunk_member_del_f l3_egress_trunk_member_del;

    l3_egress_trunk_member_add_f l3_egress_trunk_member_add;

} mbcm_ltsw_l3_egress_drv_t;

/*!
 * \brief Set the L3 egress driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] drv L2 driver to set.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_egress_drv_set(int unit, mbcm_ltsw_l3_egress_drv_t *drv);

/*!
 * \brief Initialize the L3 egress entries that reserved for L2.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_egress_l2_rsv(int unit);

/*!
 * \brief Initialize L3 egress module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_egress_init(int unit);

/*!
 * \brief De-Initialize L3 egress module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_egress_deinit(int unit);

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
mbcm_ltsw_l3_egress_id_range_get(int unit, int type, int *min, int *max);

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
mbcm_ltsw_l3_egress_create(int unit, bcm_l3_egress_t *egr, int *idx);

/*!
 * \brief Destroy a L3 egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Egress object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_egress_destroy(int unit, int idx);

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
mbcm_ltsw_l3_egress_get(int unit, int idx, bcm_l3_egress_t *egr);

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
mbcm_ltsw_l3_egress_find(int unit, bcm_l3_egress_t *egr, int *idx);

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
mbcm_ltsw_l3_egress_traverse(int unit,
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
mbcm_ltsw_l3_egress_obj_id_resolve(int unit,
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
mbcm_ltsw_l3_egress_obj_id_construct(int unit,
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
mbcm_ltsw_l3_egress_obj_info_get(int unit, bcm_if_t intf,
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
mbcm_ltsw_l3_egress_obj_cnt_get(int unit, bcmi_ltsw_l3_egr_obj_type_t type,
                                int *max, int *used);

/*!
 * \brief Set the dvp of given egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf Egress object ID.
 * \param [in] dvp Destination VP.
 *
 * \retval SHR_E_PARAM Invalid egress object ID.
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND The given egress object does not exist.
 */
extern int
mbcm_ltsw_l3_egress_dvp_set(int unit, bcm_if_t intf, int dvp);

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
mbcm_ltsw_l3_egress_flexctr_attach(
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
mbcm_ltsw_l3_egress_flexctr_detach(
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
mbcm_ltsw_l3_egress_flexctr_info_get(
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
mbcm_ltsw_l3_egress_flexctr_object_set(
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
mbcm_ltsw_l3_egress_flexctr_object_get(
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
mbcm_ltsw_l3_egress_ifa_create(
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
mbcm_ltsw_l3_egress_ifa_destroy(int unit, int idx);

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
mbcm_ltsw_l3_egress_ifa_get(
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
mbcm_ltsw_l3_egress_ifa_traverse(
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
mbcm_ltsw_l3_ecmp_create(
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
mbcm_ltsw_l3_ecmp_destroy(
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
mbcm_ltsw_l3_ecmp_get(
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
mbcm_ltsw_l3_ecmp_member_add(
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
mbcm_ltsw_l3_ecmp_member_del(
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
mbcm_ltsw_l3_ecmp_member_del_all(
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
mbcm_ltsw_l3_ecmp_find(
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
mbcm_ltsw_l3_ecmp_trav(
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
mbcm_ltsw_l3_ecmp_max_paths_get(int unit, uint32_t attr, int *max_paths);

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
mbcm_ltsw_l3_ecmp_hash_ctrl_set(int unit, int underlay, int flow_based);

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
mbcm_ltsw_l3_ecmp_hash_ctrl_get(int unit, int underlay, int *flow_based);

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
mbcm_ltsw_l3_ecmp_flow_hash_set(int unit,
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
mbcm_ltsw_l3_ecmp_flow_hash_get(int unit,
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
mbcm_ltsw_l3_ecmp_port_hash_set(int unit,
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
mbcm_ltsw_l3_ecmp_port_hash_get(int unit,
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
mbcm_ltsw_l3_ecmp_member_status_set(int unit, bcm_if_t intf, int status);

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
mbcm_ltsw_l3_ecmp_member_status_get(int unit, bcm_if_t intf, int *status);

/*!
 * \brief Set DLB Eligibility Ethertype.
 *
 * \param [in] unit Unit Number.
 * \param [in] flags Flags.
 * \param [in] ethertype_count Ethertype count.
 * \param [in] ethertype_array Ethertype array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ecmp_ethertype_set(
    int unit,
    uint32_t flags,
    int ethertype_count,
    int *ethertype_array);

/*!
 * \brief Get DLB Eligibility Ethertype.
 *
 * \param [in] unit Unit Number.
 * \param [in] ethertype_max Max ethertype count.
 * \param [out] flags Flags.
 * \param [out] ethertype_array Ethertype array.
 * \param [out] ethertype_count Actual ethertype count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ecmp_ethertype_get(
    int unit,
    int ethertype_max,
    uint32_t *flags,
    int *ethertype_array,
     int *ethertype_count);

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
extern int
mbcm_ltsw_l3_ecmp_dlb_mon_config_set(int unit,
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
extern int
mbcm_ltsw_l3_ecmp_dlb_mon_config_get(int unit,
                                     bcm_if_t ecmp_group_id,
                                     bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg);

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
mbcm_ltsw_ecmp_member_dest_alloc(int unit,
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
mbcm_ltsw_ecmp_member_dest_get(int unit,
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
mbcm_ltsw_ecmp_member_dest_free(int unit, int dest_index);

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
mbcm_ltsw_ecmp_member_dest_update(int unit,
                                  bcmi_ltsw_ecmp_member_dest_info_t *dest_info);

/*!
 * \brief Public function to get L3 egress next hop table size.
 *
 * \param [in] unit Unit Number.
 * \param [out] size L3 egress next hop table size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */

extern int
mbcm_ltsw_l3_egress_size_get(
    int unit,
    int *size);

/*!
 * \brief Public function to get current L3 egress next hop entry count.
 *
 * \param [in] unit Unit Number.
 * \param [out] count L3 egress next hop entry count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egress_count_get(
    int unit,
    int *count);

/*!
 * \brief Public function to get L3 ECMP table size.
 *
 * \param [in] unit Unit Number.
 * \param [out] size L3 ECMP table size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */

extern int
mbcm_ltsw_l3_ecmp_size_get(
    int unit,
    int *size);

/*!
 * \brief Public function to get current L3 ECMP entry count.
 *
 * \param [in] unit Unit Number.
 * \param [out] count L3 ECMP entry count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ecmp_count_get(
    int unit,
    int *count);

/*!
 * \brief Attach flexctr action to the given L3 ECMP object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_if Interface id of a L3 ECMP object.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_ecmp_flexctr_attach(
    int unit,
    bcm_if_t intf_id,
    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach flexctr action of the given L3 ECMP object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface id of a L3 ECMP object.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_ecmp_flexctr_detach(
    int unit,
    bcm_if_t intf_id);

/*!
 * \brief Get flex counter information of the given L3 ECMP object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_if Interface id of aL3 ECMP object.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ecmp_flexctr_info_get(
    int unit,
    bcm_if_t intf_id,
    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Set flex counter object value for the given L3 ECMP object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface id of a L3 ECMP object.
 * \param [in] value Flex counter object value.
 *
 * The flex counter object value could be used to generate counter index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ecmp_flexctr_object_set(
    int unit,
    bcm_if_t intf_id,
    uint32_t value);

/*!
 * \brief Get flex counter object value for the given L3 ECMP object.
 *
 * \param [in] unit Unit Number.
 * \param [in] intf_id L3 interface ID pointing to L3 ECMP object.
 * \param [out] value Flex counter object value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ecmp_flexctr_object_get(
    int unit,
    bcm_if_t intf_id,
    uint32_t *value);

/*!
 * \brief Get the L3 egress table database.
 *
 * \param [in] unit Unit number.
 * \param [out] tbl_db L3 egress table databse.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_egress_tbl_db_get(
    int unit,
    bcmint_ltsw_l3_egress_tbl_db_t *tbl_db);

/*!
 * \brief Get the L3 ECMP table database.
 *
 * \param [in] unit Unit number.
 * \param [out] tbl_db L3 ECMP table databse.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_ecmp_tbl_db_get(
    int unit,
    bcmint_ltsw_l3_egress_tbl_db_t *tbl_db);

/*!
 * \brief Get the L3 ECMP flex ctr action table database.
 *
 * \param [in] unit Unit number.
 * \param [out] tbl_db L3 ECMP flex ctr action table databse.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_ecmp_flexctr_action_tbl_db_get(
    int unit,
    bcmint_ltsw_l3_egress_tbl_db_t *tbl_db);

/*!
 * \brief Init L3 overlay entries.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_overlay_table_init(
    int unit,
    bcmint_ltsw_l3_ol_tbl_info_t *info);

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
mbcm_ltsw_l3_ecmp_dlb_stat_set(
    int unit,
    bcm_if_t ecmp_intf,
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
mbcm_ltsw_l3_ecmp_dlb_stat_get(
    int unit,
    bcm_if_t ecmp_intf,
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
mbcm_ltsw_l3_ecmp_dlb_stat_sync_get(
    int unit,
    bcm_if_t ecmp_intf,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t *value);

/*!
 * \brief Public function to add L2 tag for L3 egress.
 *
 * \param [in] unit Unit Number.
 * \param [in] nhidx Nexthop index.
 * \param [in] l2_tag_info L2 tag information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egress_add_l2tag(
    int unit,
    int nhidx,
    bcmi_ltsw_l3_egr_l2_tag_info_t *l2_tag_info);

/*!
 * \brief Public function to get L2 tag from L3 egress.
 *
 * \param [in] unit Unit Number.
 * \param [in] nhidx Nexthop index.
 * \param [out] l2_tag_info L2 tag information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egress_get_l2tag(
    int unit,
    int nhidx,
    bcmi_ltsw_l3_egr_l2_tag_info_t *l2_tag_info);

/*!
 * \brief Public function to delete L2 tag from L3 egress.
 *
 * \param [in] unit Unit Number.
 * \param [in] nhidx Nexthop index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egress_delete_l2tag(
    int unit,
    int nhidx);

/*!
 * \brief Public function to add PHP action to L3 egress.
 *
 * \param [in] unit Unit Number.
 * \param [in] nhidx Nexthop index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egress_php_action_add(
    int unit,
    bcm_if_t intf);

/*!
 * \brief Public function to delete PHP action from L3 egress.
 *
 * \param [in] unit Unit Number.
 * \param [in] nhidx Nexthop index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egress_php_action_delete(
    int unit,
    bcm_if_t intf);

/*!
 * \brief clear all source ECMP table.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_secmp_clear(int unit);

/*!
 * \brief Set source ECMP.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] ecmp_info ECMP group info.
 * \param [in] member Array of ECMP member info.
 * \param [in] count Count of ECMP member.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_secmp_set(
    int unit,
    int idx,
    bcm_l3_egress_ecmp_t *ecmp_info,
    bcmi_ltsw_l3_ecmp_member_info_t *member,
    int count);
/*!
 * \brief Delete source ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index of source ECMP group.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_secmp_del(
    int unit,
    int idx);

/*!
 * \brief Add a member to a source ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] member Pointer to ECMP member info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_secmp_member_add(
    int unit,
    int idx,
    bcmi_ltsw_l3_ecmp_member_info_t *member);

/*!
 * \brief Delete a member from a source ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] member Array of ECMP member info.
 * \param [in] count Count of ECMP member.
 * \param [in] del_member Information to ECMP member to be deleted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_secmp_member_del(
    int unit,
    int idx,
    bcmi_ltsw_l3_ecmp_member_info_t *member,
    int count,
    bcmi_ltsw_l3_ecmp_member_info_t *del_member);

/*!
 * \brief Delete all members from a source ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_secmp_member_del_all(
    int unit,
    int idx);

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
mbcm_ltsw_l3_egress_trunk_member_delete(int unit, bcm_trunk_t trunk_id,
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
mbcm_ltsw_l3_egress_trunk_member_add(int unit, bcm_trunk_t trunk_id,
                                     int member_cnt, bcm_port_t *member_arr);
#endif /* BCMI_LTSW_MBCM_L3_EGRESS_H */

