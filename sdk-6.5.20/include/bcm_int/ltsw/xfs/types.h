/*! \file types.h
 *
 * The headfile is to declare internal types definitions for XFS devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef XFS_LTSW_TYPES_H
#define XFS_LTSW_TYPES_H

/*! Destination types. */
typedef enum bcmi_xfs_dest_type_s {
    BCMI_XFS_DEST_TYPE_NO_OP = 0,
    BCMI_XFS_DEST_TYPE_L2_OIF = 1,
    BCMI_XFS_DEST_TYPE_RSVD = 2,
    BCMI_XFS_DEST_TYPE_VP = 3,
    BCMI_XFS_DEST_TYPE_ECMP = 4,
    BCMI_XFS_DEST_TYPE_NHOP = 5,
    BCMI_XFS_DEST_TYPE_L2MC_GROUP = 6,
    BCMI_XFS_DEST_TYPE_L3MC_GROUP = 7,
    BCMI_XFS_DEST_TYPE_RSVD_1 = 8,
    BCMI_XFS_DEST_TYPE_ECMP_MEMBER = 9,
    BCMI_XFS_DEST_TYPE_CNT
} bcmi_xfs_dest_type_t;

/*! Symbol of destination types. */
#define BCMI_XFS_DEST_TYPE_STR {\
    NO_OPs,                     \
    L2_OIFs,                    \
    RESERVEDs,                  \
    VPs,                        \
    ECMPs,                      \
    NHOPs,                      \
    L2MC_GROUPs,                \
    L3MC_GROUPs,                \
    RESERVED_1s,                \
    ECMP_MEMBERs                \
}

/*! CPU managed learning flags */
#define BCMI_XFS_CML_FLAGS_DROP            1
#define BCMI_XFS_CML_FLAGS_COPY_TO_CPU     2
#define BCMI_XFS_CML_FLAGS_LEARN           4

/*! EGR VFI Tag action control flags */
#define BCMI_XFS_TAG_ACT_OTAG_ADD            1
#define BCMI_XFS_TAG_ACT_ITAG_ADD            2
#define BCMI_XFS_TAG_ACT_CLASS_ID_VALID      4

/*!
 * \brief Default value for VLAN TAG Preserve control.
 */
#define BCMI_XFS_VLAN_TAG_PRESERVE_CTRL_DEF 0x3

/*! Field definition of process_ctrl in egr_dvp. */
#define BCMI_XFS_VP_PROCESS_CTRL_ITAG_DEL_SFT 0x0
#define BCMI_XFS_VP_PROCESS_CTRL_ITAG_DEL_MSK 0x1
#define BCMI_XFS_VP_PROCESS_CTRL_EVXLT2_EN_SFT 0x1
#define BCMI_XFS_VP_PROCESS_CTRL_EVXLT2_EN_MSK 0x1
#define BCMI_XFS_VP_PROCESS_CTRL_EVXLT1_DISABLE_SFT 0x2
#define BCMI_XFS_VP_PROCESS_CTRL_EVXLT1_DISABLE_MSK 0x1
#define BCMI_XFS_VP_PROCESS_CTRL_EVXLT1_SELECT_SFT 0x3
#define BCMI_XFS_VP_PROCESS_CTRL_EVXLT1_SELECT_MSK 0x1

/* Protection can be enabled for either nexthop1 or nexthop2. */
#define BCMI_XFS_PROT_SWT_PROFILE_IDX_NHOP1 0
#define BCMI_XFS_PROT_SWT_PROFILE_IDX_NHOP2 1

#endif /* XFS_LTSW_TYPES_H */
