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


#ifndef BCMI_LTSW_L3_EGRESS_H
#define BCMI_LTSW_L3_EGRESS_H

#include <shared/l3.h>
#include <bcm/ifa.h>

/*! Egress object start index. */
#define BCMI_LTSW_L3_EGR_OBJ_BASE _SHR_L3_EGRESS_IDX_MIN

/*! Egress object type. */
typedef enum bcmi_ltsw_l3_egr_obj_type_e {

    /*! Egress object for overlay forwarding. */
    BCMI_LTSW_L3_EGR_OBJ_T_OL = 0,

    /*! Egress object for tunnel underlay forwarding. */
    BCMI_LTSW_L3_EGR_OBJ_T_UL = 1,

    /*! Egress object for overlay ECMP group. */
    BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL = 2,

    /*! Egress object for underlay ECMP group. */
    BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL = 3,

    BCMI_LTSW_L3_EGR_OBJ_T_CNT

} bcmi_ltsw_l3_egr_obj_type_t;

/*! ECMP Hash output selection configuration. */
typedef enum bcmi_ltsw_l3_ecmp_hash_cfg_type_e {

    /* Enable hash concatenation for ECMP level 1 macro flow. */
    BCMI_LTSW_ECMP_HASH_CFG_T_OL_CONCAT_EN = 0,

    /* Enable hash concatenation for ECMP level 2 macro flow. */
    BCMI_LTSW_ECMP_HASH_CFG_T_UL_CONCAT_EN = 1,

    /* Set hash seed for ECMP level 1 macro flow. */
    BCMI_LTSW_ECMP_HASH_CFG_T_OL_SEED = 2,

    /* Set hash seed for ECMP level 2 macro flow. */
    BCMI_LTSW_ECMP_HASH_CFG_T_UL_SEED = 3,

    /* Set hash selection for ECMP level 0 (port-based). */
    BCMI_LTSW_ECMP_HASH_CFG_T_OL_OFFSET = 4,

    /* Set hash selection for ECMP level 1 (port-based). */
    BCMI_LTSW_ECMP_HASH_CFG_T_UL_OFFSET = 5,

    /* Set hash selection for DLB ECMP level 0 (port-based). */
    BCMI_LTSW_ECMP_HASH_CFG_T_DLB_OFFSET = 6,

    BCMI_LTSW_ECMP_HASH_CFG_T_CNT

} bcmi_ltsw_l3_ecmp_hash_cfg_type_t;

typedef enum bcmi_ltsw_ecmp_member_dest_user_e {
    /*! Explicit single pointer model for switching into L2 tunnel. */
    BCMI_LTSW_ECMP_MEMBER_DEST_T_L2_CASCADED = 0,

    /*! Explicit single pointer model for routing into L2/L3 tunnel. */
    BCMI_LTSW_ECMP_MEMBER_DEST_T_L3_CASCADED = 1,

    /*! Implicit single pointer model for ALPM routing. */
    BCMI_LTSW_ECMP_MEMBER_DEST_T_ALPM_NO_CASCADED = 2,

    /*! Always at last. */
    BCMI_LTSW_ECMP_MEMBER_DEST_T_CNT
} bcmi_ltsw_ecmp_member_dest_user_t;

/*! ECMP Member destination info. */
typedef struct bcmi_ltsw_ecmp_member_dest_info_s {
    /*! User type. See BCMI_LTSW_ECMP_MEMBER_DEST_T_xxx. */
    bcmi_ltsw_ecmp_member_dest_user_t type;

    /*! Overlay egress object. */
    bcm_if_t ol_egr_obj;

    /*! Underlay egress object. */
    bcm_if_t ul_egr_obj;

    /*! DVP. */
    int dvp;

    /*! Profile Table index for ECMP and Protection Switching SF. */
    int prot_swt_prfl_idx;
} bcmi_ltsw_ecmp_member_dest_info_t;

#define BCMI_LTSW_L3_EGR_OBJ_INFO_F_CASCADED    0x1 /* L3 Casacded */
#define BCMI_LTSW_L3_EGR_OBJ_INFO_F_FAILOVER    0x2 /* Failover protection nh */

/*! L3 egress object info. */
typedef struct bcmi_ltsw_l3_egr_obj_info_s {
    /*! Flags. See BCMI_LTSW_L3_EGR_OBJ_INFO_F_xxx. */
    uint32_t flags;

    /*! Underlay egress object. */
    bcm_if_t ul_egr_obj;

    /*! DVP. */
    int dvp;
} bcmi_ltsw_l3_egr_obj_info_t;

/*! L3 egress IFA data. */
typedef struct bcmi_ltsw_l3_egr_ifa_s {
    /*! See <bcm/l3.h> BCM_L3_*. */
    uint32_t flags;

    /*! IP protocol to indicate IFA. */
    uint8_t ip_protocol;

    /*! The max length in the IFA base header. */
    uint8_t max_length;

    /*! The hop limit in the IFA metadata header. */
    uint8_t hop_limit;
} bcmi_ltsw_l3_egr_ifa_t;

typedef enum bcmi_ltsw_vlan_tag_present_action__e {
    /*! No action. */
    BCMI_LTSW_VLAN_TAG_PRESENT_ACTION_NONE = 0,

    /*! Replace VLAN and TPID. */
    BCMI_LTSW_VLAN_TAG_PRESENT_ACTION_REPLACE_VLAN_TPID = 1,

    /*! Replace VLAN only. */
    BCMI_LTSW_VLAN_TAG_PRESENT_ACTION_REPLACE_VLAN = 2,

    /*! Delete the tag. */
    BCMI_LTSW_VLAN_TAG_PRESENT_ACTION_DELETE = 3,

    /*! Replace VLAN, priority and TPID. */
    BCMI_LTSW_VLAN_TAG_PRESENT_ACTION_REPLACE_VLAN_PRI_TPID = 4,

    /*! Replace VLAN, priority. */
    BCMI_LTSW_VLAN_TAG_PRESENT_ACTION_REPLACE_VLAN_PRI = 5,

    /*! Replace priority only. */
    BCMI_LTSW_VLAN_TAG_PRESENT_ACTION_REPLACE_PRI = 6,

    /*! Replace TPID only. */
    BCMI_LTSW_VLAN_TAG_PRESENT_ACTION_REPLACE_TPID = 7,

    /*! Always at last. */
    BCMI_LTSW_VLAN_TAG_PRESENT_ACTION_CNT
} bcmi_ltsw_vlan_tag_present_action_t;

typedef enum bcmi_ltsw_vlan_tag_not_present_action__e {
    /*! No action. */
    BCMI_LTSW_VLAN_TAG_NOT_PRESENT_ACTION_NONE = 0,

    /*! Add VLAN and TPID. */
    BCMI_LTSW_VLAN_TAG_NOT_PRESENT_ACTION_ADD_VLAN_TPID = 1,

    /*! Always at last. */
    BCMI_LTSW_VLAN_TAG_NOT_PRESENT_ACTION_CNT
} bcmi_ltsw_vlan_tag_not_present_action_t;

typedef enum bcmi_ltsw_opaque_tag_present_action__e {
    /*! No action. */
    BCMI_LTSW_OPAQUE_TAG_PRESENT_ACTION_NONE = 0,

    /*! Delete the tag. */
    BCMI_LTSW_OPAQUE_TAG_PRESENT_ACTION_DELETE = 1,

    /*! Replace the tag. */
    BCMI_LTSW_OPAQUE_TAG_PRESENT_ACTION_REPLACE = 2,

    /*! Always at last. */
    BCMI_LTSW_OPAQUE_TAG_PRESENT_ACTION_CNT
} bcmi_ltsw_opaque_tag_present_action_t;


typedef enum bcmi_ltsw_opaque_tag_not_present_action__e {
    /*! No action. */
    BCMI_LTSW_OPAQUE_TAG_NOT_PRESENT_ACTION_NONE = 0,

    /*! Add a tag. */
    BCMI_LTSW_OPAQUE_TAG_NOT_PRESENT_ACTION_ADD = 1,

    /*! Always at last. */
    BCMI_LTSW_OPAQUE_TAG_NOT_PRESENT_ACTION_CNT
} bcmi_ltsw_opaque_tag_not_present_action_t;


/*! L3 egress l2 tag info. */
typedef struct bcmi_ltsw_l3_egr_l2_tag_info_s {
    /*! Ethernet segment (ES) identifier. */
    uint32 es_id;

    /*! Outer VLAN priority. */
    uint8 pri;

     /*! Outer VLAN CFI. */
    uint8 cfi;

    /*! TPID ID. */
    uint16 tpid_id;

    /*! Outer LAN. */
    bcm_vlan_t vlan;

    /*! Enable remarking CFI. */
    uint8 remark_cfi;

    /*! Enable PHB map. */
    uint8 is_phb_map_id;

    /*! PHB map id. */
    uint16 phb_egr_l2_int_pri_to_otag_id;

    /*! L2 tag action if the tag is present. */
    bcmi_ltsw_vlan_tag_present_action_t vlan_tag_present_action;

    /*! L2 tag action if the tag is not present. */
    bcmi_ltsw_vlan_tag_not_present_action_t vlan_tag_not_present_action;

    /*! L2 opaque tag action if the tag is present. */
    bcmi_ltsw_opaque_tag_present_action_t opaque_tag_present_action;

    /*! L2 opaque tag action if the tag is not present. */
    bcmi_ltsw_opaque_tag_not_present_action_t opaque_tag_not_present_action;
} bcmi_ltsw_l3_egr_l2_tag_info_t;

/*! ECMP member information. */
typedef struct bcmi_ltsw_l3_ecmp_member_info_s {
    /*! Underlay ECMP. */
    bool ul_ecmp;

    /*! NEXT_HOP_1_INDEX. */
    uint16_t nhop_1;

    /*! NEXT_HOP_2_INDEX or ECMP_GROUP_1_ID. */
    uint16_t nhop_2_or_ecmp_grp_1;

    /*! DVP. */
    uint16_t dvp;

    /*! Encoding for ecmp_and_prot_swt_sfc_profile_index. */
    uint16_t prot_swt_prfl_idx;

    /*! PORT ID. */
    int port_id;
} bcmi_ltsw_l3_ecmp_member_info_t;

/*!
 * \param [in] unit Unit number.
 * \param [in] egr_ifa L3 egress IFA data.
 * \param [in] header IFA header.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmi_ltsw_l3_egr_to_ifa_header_cb)(int unit,
                                                 bcmi_ltsw_l3_egr_ifa_t *egr_ifa,
                                                 bcm_ifa_header_t *header);

/*!
 * \brief Initialize L3 egress entries that reserved for L2.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_l3_egress_l2_rsv(int unit);

/*!
 * \brief Initialize L3 egress module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_l3_egress_init(int unit);

/*!
 * \brief De-Initialize L3 egress module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_l3_egress_deinit(int unit);

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
bcmi_ltsw_l3_egress_obj_id_resolve(int unit,
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
bcmi_ltsw_l3_egress_obj_id_construct(int unit,
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
bcmi_ltsw_l3_egress_obj_info_get(int unit, bcm_if_t intf,
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
bcmi_ltsw_l3_egress_obj_cnt_get(int unit, bcmi_ltsw_l3_egr_obj_type_t type,
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
bcmi_ltsw_l3_egress_dvp_set(int unit, bcm_if_t intf, int dvp);

/*!
 * \brief Set the maximum ECMP paths allowed for a route.
 *
 * \param [in] unit Unit number.
 * \param [in] max Maximum number of paths for ECMP.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_l3_ecmp_max_paths_set(int unit, int max_paths);

/*!
 * \brief Get the maximum ECMP paths allowed for a route.
 *
 * \param [in] unit Unit number.
 * \param [out] max Maximum number of paths for ECMP.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_l3_ecmp_max_paths_get(int unit, int *max_paths);

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
bcmi_ltsw_l3_ecmp_hash_ctrl_set(int unit, int underlay, int flow_based);

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
bcmi_ltsw_l3_ecmp_hash_ctrl_get(int unit, int underlay, int *flow_based);

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
bcmi_ltsw_l3_ecmp_flow_hash_set(int unit,
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
bcmi_ltsw_l3_ecmp_flow_hash_get(int unit,
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
bcmi_ltsw_l3_ecmp_port_hash_set(int unit,
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
bcmi_ltsw_l3_ecmp_port_hash_get(int unit,
                                int port,
                                bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
                                int *arg);

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
bcmi_ltsw_ecmp_member_dest_alloc(int unit,
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
bcmi_ltsw_ecmp_member_dest_get(int unit,
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
bcmi_ltsw_ecmp_member_dest_free(int unit, int dest_index);

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
bcmi_ltsw_ecmp_member_dest_update(int unit,
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
bcmi_ltsw_l3_egress_size_get(
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
bcmi_ltsw_l3_egress_count_get(
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
bcmi_ltsw_l3_ecmp_size_get(
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
bcmi_ltsw_l3_ecmp_count_get(
    int unit,
    int *count);

/*!
 * \brief Public function to create L3 egress objects for IFA.
 *
 * \param [in] unit Unit Number.
 * \param [in] egr_ifa L3 egress IFA data.
 * \param [in/out] intf L3 egress obj ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_egress_ifa_create(
    int unit,
    bcmi_ltsw_l3_egr_ifa_t *egr_ifa,
    bcm_if_t *intf);

/*!
 * \brief Public function to destroy L3 egress objects for IFA.
 *
 * \param [in] unit Unit Number.
 * \param [in] intf L3 egress obj ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_egress_ifa_destroy(
    int unit,
    bcm_if_t intf);

/*!
 * \brief Public function to get L3 egress objects for IFA.
 *
 * \param [in] unit Unit Number.
 * \param [in] intf L3 egress obj ID.
 * \param [out] egr_ifa L3 egress IFA data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_egress_ifa_get(
    int unit,
    bcm_if_t intf,
    bcmi_ltsw_l3_egr_ifa_t *egr_ifa);

/*!
 * \brief Public function to traverse L3 egress objects for IFA..
 *
 * \param [in] unit Unit Number.
 * \param [in] trav_fn Callback function.
 * \param [in] cb Callback function to contruct IFA header from L3 egress.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_egress_ifa_traverse(
    int unit,
    bcm_ifa_header_traverse_cb trav_fn,
    bcmi_ltsw_l3_egr_to_ifa_header_cb cb,
    void *user_data);

/*!
 * \brief Public function to add L2 tag for  L3 egress.
 *
 * \param [in] unit Unit Number.
 * \param [in] nhidx Nexthop index.
 * \param [in] l2_tag_info L2 tag information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_egress_add_l2tag(
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
bcmi_ltsw_l3_egress_delete_l2tag(
    int unit,
    int nhidx);

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
bcmi_ltsw_l3_egress_get_l2tag(
    int unit,
    int nhidx,
    bcmi_ltsw_l3_egr_l2_tag_info_t *l2_tag_info);

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
bcmi_ltsw_l3_egress_id_range_get(int unit, int type, int *min, int *max);

/*!
 * \brief Public function to add php action to L3 egress.
 *
 * \param [in] unit Unit Number.
 * \param [in] nhidx Nexthop index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_egress_php_action_add(
    int unit,
    bcm_if_t intf);

/*!
 * \brief Public function to delete php action from L3 egress.
 *
 * \param [in] unit Unit Number.
 * \param [in] nhidx Nexthop index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_egress_php_action_delete(
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
bcmint_ltsw_l3_secmp_clear(int unit);

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
bcmint_ltsw_l3_secmp_set(
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
bcmint_ltsw_l3_secmp_del(
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
bcmint_ltsw_l3_secmp_member_add(
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
bcmint_ltsw_l3_secmp_member_del(
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
bcmint_ltsw_l3_secmp_member_del_all(
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
bcmi_ltsw_l3_egress_trunk_member_delete(int unit, bcm_trunk_t trunk_id,
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
bcmi_ltsw_l3_egress_trunk_member_add(int unit, bcm_trunk_t trunk_id,
                                     int member_cnt, bcm_port_t *member_arr);
#endif /* BCMI_LTSW_L3_EGRESS_H */
