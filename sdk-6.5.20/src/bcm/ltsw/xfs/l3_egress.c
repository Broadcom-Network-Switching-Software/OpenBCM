/*! \file l3_egress.c
 *
 * XFS L3 egress objects management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/l3.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw/sbr.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/l3.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/dlb.h>
#include <bcm_int/ltsw/trunk.h>
#include <bcm_int/ltsw/l3_intf.h>
#include <bcm_int/ltsw/l3_egress.h>
#include <bcm_int/ltsw/l3_egress_int.h>
#include <bcm_int/ltsw/generated/l3_egress_ha.h>
#include <bcm_int/ltsw/xfs/types.h>
#include <bcm_int/ltsw/xfs/l3_egress.h>
#include <bcm_int/ltsw/failover.h>
#include <bcm_int/ltsw/qos.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/ltsw/multicast.h>
#include <bcm_int/ltsw/issu.h>

#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_L3

/* Scale of egress object id. */
#define L3_EGR_OBJ_SCALE 100000

/* Encoding for l2mc_l3mc_l2_oif_sys_dst_valid */
#define DEST_VALID_L2MC     (1 << 0)
#define DEST_VALID_L3MC     (1 << 1)
#define DEST_VALID_L2OIF    (1 << 2)
#define DEST_VALID_SYS      (1 << 3)

/* Data structure to save the info of ING_L3_SRC_NEXT_HOPt entry */
typedef struct snh_cfg_s {
    /* Index. */
    uint64_t l3_src_nhop_index;

    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;
#define SNH_FLD_L3OIF (1 << 0)

    uint64_t l3_oif_value_type;

} snh_cfg_t;

/* Data structure to save the info of ING_L3_NEXT_HOP_1_TABLEt entry */
typedef struct inh1_cfg_s {
    /* Index. */
    uint64_t nhop_index_1;

    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;
#define INH1_FLD_DVP                (1 << 0)
#define INH1_FLD_L3OIF1             (1 << 1)
#define INH1_FLD_MTU_PROF           (1 << 2)
#define INH1_FLD_DEST               (1 << 3)
#define INH1_FLD_DEST_VALID         (1 << 4)
#define INH1_FLD_L3OIF_TYPE         (1 << 5)
#define INH1_FLD_STRENGTH_PRFL_IDX  (1 << 6)
#define INH1_FLD_DATA_TYPE          (1 << 7)

    uint64_t dvp;
    uint64_t l3_oif_1;
    uint64_t mtu_profile_ptr;
    uint64_t dest;
    uint64_t dest_valid;
    uint64_t l3_oif_type;
    uint64_t strength_prfl_idx;

    uint64_t data_type;
#define INH1_VIEW_L2_OIF    0
#define INH1_VIEW_DVP       1

} inh1_cfg_t;

/* Data structure to save the info of ING_L3_NEXT_HOP_2_TABLEt entry */
typedef struct inh2_cfg_s {
    /* Index. */
    uint64_t nhop_index_2;

    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;
#define INH2_FLD_DVP                (1 << 0)
#define INH2_FLD_DEST               (1 << 1)
#define INH2_FLD_DEST_VALID         (1 << 2)
#define INH2_FLD_STRENGTH_PRFL_IDX  (1 << 3)

    uint64_t dvp;
    uint64_t dest;
    uint64_t dest_valid;
    uint64_t strength_prfl_idx;
} inh2_cfg_t;

/* Data structure to save the info of EGR_L3_NEXT_HOP_1t entry */
typedef struct enh1_cfg_s {
    /* Index. */
    uint64_t nhop_index_1;

    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;
#define ENH1_FLD_DMAC                   (1 << 0)
#define ENH1_FLD_EDIT_CTRL_ID           (1 << 1)
#define ENH1_FLD_CLASS_ID               (1 << 2)
#define ENH1_FLD_FLEX_CTR_ACTION        (1 << 3)
#define ENH1_FLD_FLEX_CTR_INDEX         (1 << 4)
#define ENH1_FLD_TOS_REMARK_BASE_PTR    (1 << 5)
#define ENH1_FLD_SWAP_LABEL             (1 << 6)
#define ENH1_FLD_EXP_REMARK_BASE_PTR    (1 << 7)
#define ENH1_FLD_VIEW                   (1 << 8)
#define ENH1_FLD_STRENGTH_PRFL_IDX      (1 << 9)

    uint64_t dmac;

    uint64_t edit_ctrl_id;
#define ENH1_EDIT_CTRL_NO_MODIFY        0x1
#define ENH1_EDIT_CTRL_L2_MODIFY        0x2
#define ENH1_EDIT_CTRL_ROUTE            0x3
#define ENH1_EDIT_CTRL_MPLS_SWAP2SELF   0x8
#define ENH1_EDIT_CTRL_MPLS_SWAP        0x9
#define ENH1_EDIT_CTRL_MPLS_PHP         0xa

    uint64_t class_id;
    uint64_t flex_ctr_action;
    uint64_t flex_ctr_index;
    uint64_t tos_ptr_str;
    uint64_t tos_remark_base_ptr;
    uint64_t swap_label;
    uint64_t exp_remark_base_ptr;

    uint64_t view;
#define ENH1_VT_NHOP    0
#define ENH1_VT_MPLS    1

    uint64_t strength_prfl_idx;
} enh1_cfg_t;

/* Data structure to save the info of EGR_L3_NEXT_HOP_2t entry */
typedef struct enh2_cfg_s {
    /* Index. */
    uint64_t nhop_index_2;

    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;
#define ENH2_FLD_DMAC               (1 << 0)
#define ENH2_FLD_EFP_CTRL_ID        (1 << 1)
#define ENH2_FLD_L3OIF2             (1 << 2)
#define ENH2_FLD_FLEX_CTR_ACTION    (1 << 3)
#define ENH2_FLD_FLEX_CTR_INDEX     (1 << 4)
#define ENH2_FLD_CLASS_ID           (1 << 5)
#define ENH2_FLD_VIEW               (1 << 6)
#define ENH2_FLD_STRENGTH_PRFL_IDX  (1 << 7)
#define ENH2_FLD_IFA_IP_PROTOCOL    (1 << 8)
#define ENH2_FLD_IFA_GNS            (1 << 9)
#define ENH2_FLD_IFA_MAX_LENGTH     (1 << 10)
#define ENH2_FLD_IFA_REQ_VECTOR     (1 << 11)
#define ENH2_FLD_IFA_HOP_LIMIT      (1 << 12)
#define ENH2_FLD_IFA_CURR_LENGTH    (1 << 13)

    uint64_t dmac;
    uint64_t efp_ctrl_id;
    uint64_t l3_oif_2;
    uint64_t flex_ctr_action;
    uint64_t flex_ctr_index;
    uint64_t class_id;

    uint64_t ifa_ip_protocol;
    uint64_t ifa_gns;
    uint64_t ifa_max_length;
    uint64_t ifa_req_vector;
    uint64_t ifa_hop_limit;
    uint64_t ifa_curr_length;

    uint64_t view;
#define ENH2_VT_NHOP    0
#define ENH2_VT_IFA     1

    uint64_t strength_prfl_idx;
} enh2_cfg_t;

/* Data structure to save the info of ECMP_LEVEL0/1 */
typedef struct ecmp_level_cfg_s {
    /* Underlay. */
    bool ul;

    /* Weighted. */
    bool weighted;

    /* Resilient hash */
    bool rh;

    /* Index. */
    uint64_t ecmp_id;

    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;
#define EL_FLD_LB_MODE              (1 << 0)
#define EL_FLD_MAX_PATHS            (1 << 1)
#define EL_FLD_NUM_PATHS            (1 << 2)
#define EL_FLD_WEIGHTED_SIZE        (1 << 3)
#define EL_FLD_NHOP_1               (1 << 4)
#define EL_FLD_DVP                  (1 << 5)
#define EL_FLD_NHOP_2_ECMP_GRP_1    (1 << 6)
#define EL_FLD_ECMP_PROC_SWT        (1 << 7)
#define EL_FLD_STRENGTH_PRFL_IDX    (1 << 8)
#define EL_FLD_RH_MODE              (1 << 9)
#define EL_FLD_RH_NUM_PATHS         (1 << 10)
#define EL_FLD_RH_RANDOM_SEED       (1 << 11)

    uint64_t lb_mode;
    uint64_t max_paths;
    uint64_t weighted_size;
    uint64_t num_paths;
    uint64_t rh_mode;
    uint64_t rh_num_paths;
    uint64_t rh_random_seed;

    /* Array index where the ECMP member info should start being applied. */
    uint32_t start_idx;

    /* Number of array element to be set. */
    uint32_t num_of_set;

    /*
     * Number of array element to be unset. The start index of unset element
     * should be start_idx + num_of_set.
     */
    uint32_t num_of_unset;

    /* Array of ECMP member info to be set. */
    bcmi_ltsw_l3_ecmp_member_info_t *member_info;
} ecmp_level_cfg_t;

typedef struct olnh_entry_info_s {
    /* Flags. BCMI_LTSW_L3_EGR_OBJ_INFO_F_xxx. */
    uint32_t flags;

    /* MTU profile ptr. */
    int mtu_ptr;

    /* Reference count. */
    int ref_cnt;

    /* DVP in single pointer mode. */
    int dvp;

    /* Underlay egress object in single pointer mode. */
    int ul_egr_obj;

} olnh_entry_info_t;

typedef struct l3_egr_flexctr_cfg_s {
    /* Operation of flexctr fields: 0 = set, 1 = get. */
    bool op;
#define L3_EGR_FLXCTR_OP_SET   0
#define L3_EGR_FLXCTR_OP_GET   1

    /* Flags indicate which flexctr field to be operated. */
    uint8_t flags;
#define L3_EGR_FLXCTR_F_ACTION     (1 << 0)
#define L3_EGR_FLXCTR_F_INDEX      (1 << 1)

    /* Flex counter action. */
    uint32_t action;

    /* Distinct flex counter index. */
    uint32_t index;
} l3_egr_flexctr_cfg_t;

/* ECMP_MEMBER_DESTINATION group info. */
typedef struct ecmp_member_dest_grp_s {
    /* Index of first entry of this block in the ECMP member table. */
    uint32_t base_index;

    /* Number of used entries in the group. */
    uint32_t used_ent_cnt;
} ecmp_member_dest_grp_t;

/* ECMP member destination info. */
typedef struct ecmp_member_dest_info_s {

    /* Mutext. */
    sal_mutex_t mutex;

    /* Group number. */
    uint16_t num_grps;

    /* Number of member entries per group. */
    uint16_t num_ents;

    /* Group info. */
    ecmp_member_dest_grp_t *group_info;

    /* ECMP member entry info. */
    bcmint_l3_ecmp_member_ent_t *ent_info;
} ecmp_member_dest_info_t;

#define ECMP_MEMBER_DEST_GRP_START   1

typedef enum l3_ecmp_dynamic_mode_e {
    XFS_ECMP_DM_REGULAR,
    XFS_ECMP_DM_RANDOM,
    XFS_ECMP_DM_RESILIENT,
    XFS_ECMP_DM_DLB,
    XFS_ECMP_DM_DGM,
    XFS_ECMP_DM_CNT
} l3_ecmp_dynamic_mode_t;

typedef struct l3_egr_info_s {
    /* L3 egress manager initialized. */
    uint8_t inited;

    /* Mutex. */
    sal_mutex_t mutex;

    /* Overlay nexthop capacity. */
    int olnh_cap;

    /* Minimum Overlay nexthop index. */
    int olnh_min;

    /* Maximum Overlay nexthop index. */
    int olnh_max;

    /* Bitmap of free overlay nh index . */
    SHR_BITDCL *olnh_fbmp;

    /* Overlay nh flags. */
    uint32_t *olnh_flags;
#define L3_OL_NHOP_F_CASCADED  0x1

    /* Underlay nexthop capacity. */
    int ulnh_cap;

    /* Minimum underlay nexthop index. */
    int ulnh_min;

    /* Maximum underlay nexthop index. */
    int ulnh_max;

    /* Bitmap of free underlay nh index . */
    SHR_BITDCL *ulnh_fbmp;

    /* Bitmap of IFA underlay nh index . */
    SHR_BITDCL *ulnh_ifa_bmp;

    /* Minimum Overlay ECMP index. */
    int olecmp_grp_min;

    /* Maximum Overlay ECMP index. */
    int olecmp_grp_max;

    /* Bitmap of free overlay ECMP group index . */
    SHR_BITDCL *olecmp_grp_fbmp;

    /* Overlay ECMP flags. */
    uint32_t *olecmp_grp_flags;

    /* Overlay ECMP group dynamic mode. */
    uint8_t *olecmp_grp_dyna_mode;

    /* Minimum underlay ECMP index. */
    int ulecmp_grp_min;

    /* Maximum underlay ECMP index. */
    int ulecmp_grp_max;

    /* Bitmap of free underlay ECMP group index . */
    SHR_BITDCL *ulecmp_grp_fbmp;

    /* Underlay ECMP flags. */
    uint32_t *ulecmp_grp_flags;

    /* Underlay ECMP group dynamic mode. */
    uint8_t *ulecmp_grp_dyna_mode;

    /* Maximum ECMP paths. */
    int ecmp_path_max;

    /* Maximum Resilient Hash ECMP paths. */
    int ecmp_rh_path_max;

    /* Maximum weighted ECMP paths. */
    int wgt_ecmp_path_max;

    /* Maximum number of entries for each hash output selection instance. */
    int ecmp_hash_sel_ent_num;

    /* Offset in ECMP group for DLB. */
    int ecmp_dlb_grp_offset;

    /* Minmum ECMP DLB group index. */
    int ecmp_dlb_grp_min;

    /* Array of overlay nexthop entry info. */
    olnh_entry_info_t *olnh_ei;

    /* ECMP member destination info. */
    ecmp_member_dest_info_t ecmp_member_dest_info;

    /* Underlay nhop index of a trunk. */
    int *trunk_ul_nhidx;

} l3_egr_info_t;

static l3_egr_info_t l3_egr_info[BCM_MAX_NUM_UNITS] = {{0}};

#define L3_EGR_INITED(_u) (l3_egr_info[_u].inited)

#define L3_EGR_LOCK(_u)     \
    sal_mutex_take(l3_egr_info[_u].mutex, SAL_MUTEX_FOREVER)

#define L3_EGR_UNLOCK(_u)   \
    sal_mutex_give(l3_egr_info[_u].mutex)

#define L3_OL_NHOP_CAP(_u)   (l3_egr_info[_u].olnh_cap)

#define L3_OL_NHOP_MIN(_u)   (l3_egr_info[_u].olnh_min)

#define L3_OL_NHOP_MAX(_u)   (l3_egr_info[_u].olnh_max)

#define L3_OL_NHOP_FBMP_GET(_u, _id)    \
    SHR_BITGET(l3_egr_info[_u].olnh_fbmp, (_id))

#define L3_OL_NHOP_FBMP_SET(_u, _id)    \
    SHR_BITSET(l3_egr_info[_u].olnh_fbmp, (_id))

#define L3_OL_NHOP_FBMP_CLR(_u, _id)    \
    SHR_BITCLR(l3_egr_info[_u].olnh_fbmp, (_id))

#define L3_OL_NHOP_FLAGS(_u, _id) (l3_egr_info[_u].olnh_flags[_id])

#define L3_UL_NHOP_CAP(_u)   (l3_egr_info[_u].ulnh_cap)

#define L3_UL_NHOP_MIN(_u)   (l3_egr_info[_u].ulnh_min)

#define L3_UL_NHOP_MAX(_u)   (l3_egr_info[_u].ulnh_max)

#define L3_UL_NHOP_FBMP_GET(_u, _id)    \
    SHR_BITGET(l3_egr_info[_u].ulnh_fbmp, (_id))

#define L3_UL_NHOP_FBMP_SET(_u, _id)    \
    SHR_BITSET(l3_egr_info[_u].ulnh_fbmp, (_id))

#define L3_UL_NHOP_FBMP_CLR(_u, _id)    \
    SHR_BITCLR(l3_egr_info[_u].ulnh_fbmp, (_id))

#define L3_UL_NHOP_IFA_BMP_GET(_u, _id)    \
    SHR_BITGET(l3_egr_info[_u].ulnh_ifa_bmp, (_id))

#define L3_UL_NHOP_IFA_BMP_SET(_u, _id)    \
    SHR_BITSET(l3_egr_info[_u].ulnh_ifa_bmp, (_id))

#define L3_UL_NHOP_IFA_BMP_CLR(_u, _id)    \
    SHR_BITCLR(l3_egr_info[_u].ulnh_ifa_bmp, (_id))

#define L3_OL_NHOP_INFO(_u) (l3_egr_info[_u].olnh_ei)

#define L3_OL_NHOP_MTU_PTR(_u, _id) (l3_egr_info[_u].olnh_ei[_id].mtu_ptr)

#define L3_OL_ECMP_GRP_MIN(_u)   (l3_egr_info[_u].olecmp_grp_min)

#define L3_OL_ECMP_GRP_MAX(_u)   (l3_egr_info[_u].olecmp_grp_max)

#define L3_OL_ECMP_GRP_FBMP_GET(_u, _id)    \
    SHR_BITGET(l3_egr_info[_u].olecmp_grp_fbmp, (_id))

#define L3_OL_ECMP_GRP_FBMP_SET(_u, _id)    \
    SHR_BITSET(l3_egr_info[_u].olecmp_grp_fbmp, (_id))

#define L3_OL_ECMP_GRP_FBMP_CLR(_u, _id)    \
    SHR_BITCLR(l3_egr_info[_u].olecmp_grp_fbmp, (_id))

#define L3_OL_ECMP_GRP_FLAGS(_u, _id) (l3_egr_info[_u].olecmp_grp_flags[_id])

#define L3_OL_ECMP_GRP_DYNA_MODE(_u, _id)   \
    (l3_egr_info[_u].olecmp_grp_dyna_mode[_id])

#define L3_UL_ECMP_GRP_MIN(_u)   (l3_egr_info[_u].ulecmp_grp_min)

#define L3_UL_ECMP_GRP_MAX(_u)   (l3_egr_info[_u].ulecmp_grp_max)

#define L3_UL_ECMP_GRP_FBMP_GET(_u, _id)    \
    SHR_BITGET(l3_egr_info[_u].ulecmp_grp_fbmp, (_id))

#define L3_UL_ECMP_GRP_FBMP_SET(_u, _id)    \
    SHR_BITSET(l3_egr_info[_u].ulecmp_grp_fbmp, (_id))

#define L3_UL_ECMP_GRP_FBMP_CLR(_u, _id)    \
    SHR_BITCLR(l3_egr_info[_u].ulecmp_grp_fbmp, (_id))

#define L3_UL_ECMP_GRP_FLAGS(_u, _id) (l3_egr_info[_u].ulecmp_grp_flags[_id])

#define L3_UL_ECMP_GRP_DYNA_MODE(_u, _id)   \
    (l3_egr_info[_u].ulecmp_grp_dyna_mode[_id])

#define L3_ECMP_MAX_PATHS(_u) (l3_egr_info[_u].ecmp_path_max)

#define L3_ECMP_RH_MAX_PATHS(_u) (l3_egr_info[_u].ecmp_rh_path_max)

#define L3_ECMP_WEIGHTED_MAX_PATHS(_u) (l3_egr_info[_u].wgt_ecmp_path_max)

#define L3_ECMP_HASH_SEL_ENT_NUM(_u) (l3_egr_info[_u].ecmp_hash_sel_ent_num)

#define L3_ECMP_DLB_GRP_MIN(_u) (l3_egr_info[_u].ecmp_dlb_grp_min)

#define L3_ECMP_DLB_GRP_OFFSET(_u) (l3_egr_info[_u].ecmp_dlb_grp_offset)

#define L3_ECMP_DLB_GRP_CONFIGURED(_u, _id)                             \
    (((_id) >= L3_ECMP_DLB_GRP_OFFSET(_u)) &&                            \
     bcmi_ltsw_dlb_id_configured(_u, bcmi_dlb_type_ecmp,                \
                                 ((_id) - L3_ECMP_DLB_GRP_OFFSET(_u))))

#define L3_NHOP_IDX_MASK(_u) (l3_egr_info[_u].olnh_max)

/* System destination to underlay nhidx mapping enabled.*/
#define L3_SYSPORT_TO_UL_NHIDX_ENABLED(_u)  \
    (l3_egr_info[_u].trunk_ul_nhidx ? true : false)

#define L3_TRUNK_UL_NHIDX(_u, _tid) (l3_egr_info[_u].trunk_ul_nhidx[_tid])

/*
 * The offset value should be obtained through NPL define table size
 * on (ING_L3_NEXT_HOP_1_TABLE - ING_L3_NEXT_HOP_2_TABLE)
 */
#define L3_UL_OBJ_OFFSET 0x8000

typedef enum l3_ecmp_lb_mode_e {
    BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR = 0,
    BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM = 1,
    BCMINT_LTSW_L3_ECMP_LB_MODE_CNT
} l3_ecmp_lb_mode_t;

static const char *lb_mode_str[] = {
    REGULARs,
    RANDOMs,
    "INVALID"
};

/* Weighted ECMP group size enum definition. */
typedef enum l3_ecmp_wgt_sz_e {
    BCMINT_LTSW_L3_ECMP_WGT_SZ_256 = 0,
    BCMINT_LTSW_L3_ECMP_WGT_SZ_512 = 1,
    BCMINT_LTSW_L3_ECMP_WGT_SZ_1K = 2,
    BCMINT_LTSW_L3_ECMP_WGT_SZ_2K = 3,
    BCMINT_LTSW_L3_ECMP_WGT_SZ_4K = 4,
    BCMINT_LTSW_L3_ECMP_WGT_SZ_8K = 5,
    BCMINT_LTSW_L3_ECMP_WGT_SZ_16K = 6,
    BCMINT_LTSW_L3_ECMP_WGT_SZ_CNT
} l3_ecmp_wgt_sz_t;

static const char *wgt_sz_str[] = {
    WEIGHTED_SIZE_256s,
    WEIGHTED_SIZE_512s,
    WEIGHTED_SIZE_1Ks,
    WEIGHTED_SIZE_2Ks,
    WEIGHTED_SIZE_4Ks,
    WEIGHTED_SIZE_8Ks,
    WEIGHTED_SIZE_16Ks,
    "INVALID"
};

/******************************************************************************
 * Private functions
 */
/*!
 * \brief Get egress object ID from LT index.
 *
 * To avoid collisions in the protection switching table, which is a
 * single PT used to protect all next-hops, the index of overlay and
 * underlay next hop must not be overlapped in source table.
 * The index of underlay nexthop logical table still start with 0.
 * The underlay next hop index in source table should be set with an offset.
 *
 * Eg, the raw ltidx of ING_L3_NEXT_HOP_2_TABLE is in range 0-(32k-1),
 * L3_IPV4_UNICAST_TABLE.NHOP_2_OR_ECMP_GROUP_INDEX_1 will be in
 * range 32k-(64k-1). Hardware will mask out bit 16 of next-hop in this
 * usage model.
 *
 * This function is to add offset to underlay nhop/ecmp ID.
 *
 * \param [in] unit Unit number.
 * \param [in] ltidx Logical table index.
 * \param [in] type Egress object type.
 * \param [out] intf Egress object ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
static int
l3_egress_ltidx_to_obj_id(int unit, int ltidx,
                          bcmi_ltsw_l3_egr_obj_type_t type,
                          bcm_if_t *intf)
{
    int nh_ecmp_idx;

    SHR_FUNC_ENTER(unit);

    if ((type == BCMI_LTSW_L3_EGR_OBJ_T_UL) ||
        (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL)) {
        nh_ecmp_idx = ltidx + L3_UL_OBJ_OFFSET;
    } else {
        nh_ecmp_idx = ltidx;
    }

    SHR_IF_ERR_EXIT
        (xfs_ltsw_l3_egress_obj_id_construct(unit, nh_ecmp_idx, type, intf));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get LT index from egress object ID.
 *
 * \param [in] unit Unit number.
 * \param [in] intf Egress object ID.
 * \param [out] ltidx Logical table index.
 * \param [out] type Egress object type.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
l3_egress_obj_id_to_ltidx(int unit, bcm_if_t intf, int *ltidx,
                          bcmi_ltsw_l3_egr_obj_type_t *type)
{
    int nh_ecmp_idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (xfs_ltsw_l3_egress_obj_id_resolve(unit, intf, &nh_ecmp_idx, type));

    if ((*type == BCMI_LTSW_L3_EGR_OBJ_T_UL) ||
        (*type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL)) {
        *ltidx = nh_ecmp_idx & L3_NHOP_IDX_MASK(unit);
    } else {
        *ltidx = nh_ecmp_idx;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill entry of ING_L3_SRC_NEXT_HOP_TABLEt.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] cfg Data of ING_L3_SRC_NEXT_HOP_TABLEt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_ing_snh_fill(int unit, bcmlt_entry_handle_t eh, snh_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, L3_SRC_NHOP_INDEXs,
                               cfg->l3_src_nhop_index));

    if (cfg->fld_bmp & SNH_FLD_L3OIF) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, L3_OIF_VALUE_TYPEs,
                                   cfg->l3_oif_value_type));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse entry of ING_L3_SRC_NEXT_HOP_TABLEt.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [out] cfg Data of ING_L3_SRC_NEXT_HOP_TABLEt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_ing_snh_parse(int unit, bcmlt_entry_handle_t eh, snh_cfg_t *cfg)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, L3_SRC_NHOP_INDEXs,
                               &cfg->l3_src_nhop_index));

    if (cfg->fld_bmp & SNH_FLD_L3OIF) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, L3_OIF_VALUE_TYPEs,
                                   &cfg->l3_oif_value_type));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill entry of ING_L3_NEXT_HOP_1_TABLEt.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] cfg Data of ING_L3_NEXT_HOP_1.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_ing_nh_1_fill(int unit, bcmlt_entry_handle_t eh, inh1_cfg_t *cfg)
{
    const char *dt_str[] = {
        L2_OIFs,
        DVPs
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, NHOP_INDEX_1s,
                               cfg->nhop_index_1));

    if (cfg->fld_bmp & INH1_FLD_DATA_TYPE) {
        if (cfg->data_type >= COUNTOF(dt_str)) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(eh, VIEW_Ts,
                                          dt_str[cfg->data_type]));
    }

    if (cfg->fld_bmp & INH1_FLD_DVP) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, DVPs, cfg->dvp));
    }

    if (cfg->fld_bmp & INH1_FLD_L3OIF1) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, L3_OIF_1s, cfg->l3_oif_1));
    }

    if (cfg->fld_bmp & INH1_FLD_MTU_PROF) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, MTU_PROFILE_PTRs, cfg->mtu_profile_ptr));
    }

    if (cfg->fld_bmp & INH1_FLD_DEST) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, DESTINATIONs, cfg->dest));
    }

    if (cfg->fld_bmp & INH1_FLD_DEST_VALID) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, L2MC_L3MC_L2_OIF_SYS_DST_VALIDs,
                                   cfg->dest_valid));
    }

    if (cfg->fld_bmp & INH1_FLD_L3OIF_TYPE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, L3_OIF_TYPEs, cfg->l3_oif_type));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, STRENGTH_PROFILE_INDEXs,
                               cfg->strength_prfl_idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill entry of ING_L3_NEXT_HOP_2_TABLEt.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] cfg Data of ING_L3_NEXT_HOP_2.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_ing_nh_2_fill(int unit, bcmlt_entry_handle_t eh, inh2_cfg_t *cfg)
{
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_ING_L3_NEXT_HOP_2;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type = BCMI_LTSW_SBR_PET_NO_DVP;
    int sbr_index;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, NHOP_INDEX_2s,
                               cfg->nhop_index_2));

    if (cfg->fld_bmp & INH2_FLD_DVP) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, DVPs, cfg->dvp));
        if (cfg->dvp) {
            /* The strength profile entry with DVP_STRENGTH. */
            ent_type = BCMI_LTSW_SBR_PET_DEF;
        }
    }

    if (cfg->fld_bmp & INH2_FLD_DEST) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, DESTINATIONs, cfg->dest));
    }

    if (cfg->fld_bmp & INH2_FLD_DEST_VALID) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, L2MC_L3MC_L2_OIF_SYS_DST_VALIDs,
                                   cfg->dest_valid));
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &sbr_index));
    cfg->strength_prfl_idx = sbr_index;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, STRENGTH_PROFILE_INDEXs,
                               cfg->strength_prfl_idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill entry of EGR_L3_NEXT_HOP_1t.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] cfg Data of EGR_L3_NEXT_HOP_1.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_egr_nh_1_fill(int unit, bcmlt_entry_handle_t eh, enh1_cfg_t *cfg)
{
    const char* vt_str[] = {
        VT_NHOP_1s,
        VT_NHOP_1_MPLSs
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, L3_NEXT_HOP_1s, cfg->nhop_index_1));

    if (cfg->fld_bmp & ENH1_FLD_DMAC) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, DST_MAC_ADDRESSs, cfg->dmac));
    }

    if (cfg->fld_bmp & ENH1_FLD_EDIT_CTRL_ID) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, EDIT_CTRL_IDs, cfg->edit_ctrl_id));
    }

    if (cfg->fld_bmp & ENH1_FLD_CLASS_ID) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, CLASS_IDs, cfg->class_id));
    }

    if (cfg->fld_bmp & ENH1_FLD_FLEX_CTR_ACTION) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, FLEX_CTR_ACTIONs, cfg->flex_ctr_action));
    }

    if (cfg->fld_bmp & ENH1_FLD_FLEX_CTR_INDEX) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, FLEX_CTR_INDEXs, cfg->flex_ctr_index));
    }

    if (cfg->fld_bmp & ENH1_FLD_TOS_REMARK_BASE_PTR) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, TOS_REMARK_BASE_PTRs,
                                   cfg->tos_remark_base_ptr));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, TOS_PTR_STRs, cfg->tos_ptr_str));
    }

    if (cfg->fld_bmp & ENH1_FLD_SWAP_LABEL) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, SWAP_LABELs, cfg->swap_label));
    }

    if (cfg->fld_bmp & ENH1_FLD_EXP_REMARK_BASE_PTR) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, EXP_REMARK_BASE_PTRs,
                                   cfg->exp_remark_base_ptr));
    }

    if (cfg->fld_bmp & ENH1_FLD_VIEW) {
        if (cfg->view >= COUNTOF(vt_str)) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(eh, VIEW_Ts, vt_str[cfg->view]));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, STRENGTH_PRFL_IDXs, cfg->strength_prfl_idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill entry of EGR_L3_NEXT_HOP_2t.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] cfg Data of EGR_L3_NEXT_HOP_2.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_egr_nh_2_fill(int unit, bcmlt_entry_handle_t eh, enh2_cfg_t *cfg)
{
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_EGR_L3_NEXT_HOP_2;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type;
    const char *vt_str[] = {
        VT_NHOP_2s,
        VT_IFA_OBJs
    };
    int sbr_index;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, L3_NEXT_HOP_2s, cfg->nhop_index_2));

    if (cfg->fld_bmp & ENH2_FLD_DMAC) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, DST_MAC_ADDRESSs, cfg->dmac));
    }

    if (cfg->fld_bmp & ENH2_FLD_EFP_CTRL_ID) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, EFP_CTRL_IDs, cfg->efp_ctrl_id));
    }

    if (cfg->fld_bmp & ENH2_FLD_L3OIF2) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, L3_OIF_2s, cfg->l3_oif_2));
    }

    if (cfg->fld_bmp & ENH2_FLD_CLASS_ID) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, CLASS_IDs, cfg->class_id));
        ent_type = BCMI_LTSW_SBR_PET_DEF;
    } else {
        ent_type = BCMI_LTSW_SBR_PET_NONE;
    }

    if (cfg->fld_bmp & ENH2_FLD_FLEX_CTR_ACTION) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, FLEX_CTR_ACTIONs, cfg->flex_ctr_action));
    }

    if (cfg->fld_bmp & ENH2_FLD_FLEX_CTR_INDEX) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, FLEX_CTR_INDEXs, cfg->flex_ctr_index));
    }

    if (cfg->fld_bmp & ENH2_FLD_IFA_IP_PROTOCOL) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, IFA_PROTOCOLs, cfg->ifa_ip_protocol));
    }

    if (cfg->fld_bmp & ENH2_FLD_IFA_GNS) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, IFA_GNSs, cfg->ifa_gns));
    }

    if (cfg->fld_bmp & ENH2_FLD_IFA_MAX_LENGTH) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, IFA_MAX_LENGTHs, cfg->ifa_max_length));
    }

    if (cfg->fld_bmp & ENH2_FLD_IFA_REQ_VECTOR) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, IFA_REQUEST_VECTORs,
                                   cfg->ifa_req_vector));
    }

    if (cfg->fld_bmp & ENH2_FLD_IFA_HOP_LIMIT) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, IFA_HOP_LIMITs, cfg->ifa_hop_limit));
    }

    if (cfg->fld_bmp & ENH2_FLD_IFA_CURR_LENGTH) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, IFA_CURRENT_LENGTHs,
                                   cfg->ifa_curr_length));
    }

    if (cfg->fld_bmp & ENH2_FLD_VIEW) {
        if (cfg->view >= COUNTOF(vt_str)) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(eh, VIEW_Ts, vt_str[cfg->view]));
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &sbr_index));
    cfg->strength_prfl_idx = sbr_index;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, STRENGTH_PRFL_IDXs, cfg->strength_prfl_idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse entry of ING_L3_NEXT_HOP_1_TABLEt.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [out] cfg Data of ING_L3_NEXT_HOP_1_TABLEt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_ing_nh_1_parse(int unit, bcmlt_entry_handle_t eh, inh1_cfg_t *cfg)
{
    const char *value;

    SHR_FUNC_ENTER(unit);

    sal_memset(cfg, 0, sizeof(inh1_cfg_t));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, NHOP_INDEX_1s, &cfg->nhop_index_1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(eh, VIEW_Ts, &value));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, MTU_PROFILE_PTRs, &cfg->mtu_profile_ptr));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, L3_OIF_1s, &cfg->l3_oif_1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, L2MC_L3MC_L2_OIF_SYS_DST_VALIDs,
                               &cfg->dest_valid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, L3_OIF_TYPEs, &cfg->l3_oif_type));

    if (!sal_strcmp(L2_OIFs, value)) {
        cfg->data_type = INH1_VIEW_L2_OIF;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, DESTINATIONs, &cfg->dest));

    } else if (!sal_strcmp(DVPs, value)) {
        cfg->data_type = INH1_VIEW_DVP;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, DVPs, &cfg->dvp));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse entry of ING_L3_NEXT_HOP_2_TABLEt.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [out] cfg Data of ING_L3_NEXT_HOP_2_TABLEt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_ing_nh_2_parse(int unit, bcmlt_entry_handle_t eh, inh2_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    sal_memset(cfg, 0, sizeof(inh2_cfg_t));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, NHOP_INDEX_2s,
                               &cfg->nhop_index_2));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, DESTINATIONs, &cfg->dest));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, L2MC_L3MC_L2_OIF_SYS_DST_VALIDs,
                               &cfg->dest_valid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, DVPs, &cfg->dvp));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse entry of EGR_L3_NEXT_HOP_1t.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [out] cfg Data of EGR_L3_NEXT_HOP_1t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_egr_nh_1_parse(int unit, bcmlt_entry_handle_t eh, enh1_cfg_t *cfg)
{
    const char *value;

    SHR_FUNC_ENTER(unit);

    sal_memset(cfg, 0, sizeof(enh1_cfg_t));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, L3_NEXT_HOP_1s, &cfg->nhop_index_1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, DST_MAC_ADDRESSs, &cfg->dmac));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, EDIT_CTRL_IDs, &cfg->edit_ctrl_id));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, CLASS_IDs, &cfg->class_id));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, STRENGTH_PRFL_IDXs,
                               &cfg->strength_prfl_idx));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(eh, VIEW_Ts, &value));

    if (!sal_strcmp(VT_NHOP_1s, value)) {
        cfg->view = ENH1_VT_NHOP;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, FLEX_CTR_ACTIONs,
                                   &cfg->flex_ctr_action));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, FLEX_CTR_INDEXs,
                                   &cfg->flex_ctr_index));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, TOS_PTR_STRs,
                                   &cfg->tos_ptr_str));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, TOS_REMARK_BASE_PTRs,
                                   &cfg->tos_remark_base_ptr));
    } else if (!sal_strcmp(VT_NHOP_1_MPLSs, value)) {
        cfg->view = ENH1_VT_MPLS;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, SWAP_LABELs, &cfg->swap_label));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, EXP_REMARK_BASE_PTRs,
                                   &cfg->exp_remark_base_ptr));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse entry of EGR_L3_NEXT_HOP_2t.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [out] cfg Data of EGR_L3_NEXT_HOP_2t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_egr_nh_2_parse(int unit, bcmlt_entry_handle_t eh, enh2_cfg_t *cfg)
{
    const char *value;

    SHR_FUNC_ENTER(unit);

    sal_memset(cfg, 0, sizeof(enh2_cfg_t));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, L3_NEXT_HOP_2s, &cfg->nhop_index_2));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(eh, VIEW_Ts, &value));

    if (!sal_strcmp(VT_NHOP_2s, value)) {
        cfg->view= ENH2_VT_NHOP;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, FLEX_CTR_ACTIONs, &cfg->flex_ctr_action));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, FLEX_CTR_INDEXs, &cfg->flex_ctr_index));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, DST_MAC_ADDRESSs, &cfg->dmac));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, EFP_CTRL_IDs, &cfg->efp_ctrl_id));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, L3_OIF_2s, &cfg->l3_oif_2));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, CLASS_IDs, &cfg->class_id));
    } else {
        cfg->view = ENH2_VT_IFA;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, IFA_PROTOCOLs, &cfg->ifa_ip_protocol));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, IFA_GNSs, &cfg->ifa_gns));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, IFA_MAX_LENGTHs, &cfg->ifa_max_length));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, IFA_REQUEST_VECTORs,
                                   &cfg->ifa_req_vector));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, IFA_HOP_LIMITs, &cfg->ifa_hop_limit));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, IFA_CURRENT_LENGTHs,
                                   &cfg->ifa_curr_length));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill entry handle of ECMP_LEVEL0/1t.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] cfg Data of ECMP_LEVEL0/1t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_ecmp_level_fill(int unit, bcmlt_entry_handle_t eh, ecmp_level_cfg_t *cfg)
{
    const char *fld_name;
    uint64_t *data = NULL, *val0 = NULL;
    uint32_t i, sz, max_paths, start_idx, num;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_ING_ECMP_MEMBER_1;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type = BCMI_LTSW_SBR_PET_DEF;
    int sbr_index;

    SHR_FUNC_ENTER(unit);

    max_paths = cfg->weighted ? L3_ECMP_WEIGHTED_MAX_PATHS(unit) :
                                (cfg->rh ? L3_ECMP_RH_MAX_PATHS(unit) :
                                           L3_ECMP_MAX_PATHS(unit));

    if ((cfg->start_idx + cfg->num_of_unset + cfg->num_of_set) > max_paths) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, ECMP_IDs, cfg->ecmp_id));

    if (cfg->fld_bmp & EL_FLD_LB_MODE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(eh, LB_MODEs,
                                          lb_mode_str[cfg->lb_mode]));
    }

    if (cfg->fld_bmp & EL_FLD_MAX_PATHS) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, MAX_PATHSs, cfg->max_paths));
    }

    if (cfg->fld_bmp & EL_FLD_WEIGHTED_SIZE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(eh, WEIGHTED_SIZEs,
                                          wgt_sz_str[cfg->weighted_size]));
    }

    if (cfg->fld_bmp & EL_FLD_NUM_PATHS) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, NUM_PATHSs, cfg->num_paths));
    }

    if (cfg->fld_bmp & EL_FLD_RH_MODE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, RH_MODEs, cfg->rh_mode));
    }

    if (cfg->fld_bmp & EL_FLD_RH_NUM_PATHS) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, RH_NUM_PATHSs, cfg->rh_num_paths));
    }

    if (cfg->fld_bmp & EL_FLD_RH_RANDOM_SEED) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, RH_RANDOM_SEEDs, cfg->rh_random_seed));
    }

    if (cfg->num_of_set) {
        sz = sizeof(uint64_t) * cfg->num_of_set;
        SHR_ALLOC(data, sz, "bcmLtswXfsEcmpMemberData");
        SHR_NULL_CHECK(data, SHR_E_MEMORY);

        start_idx = cfg->start_idx;
        num = cfg->num_of_set;

        if (cfg->fld_bmp & EL_FLD_NHOP_1) {
            for (i = 0; i < cfg->num_of_set; i++) {
                data[i] = cfg->member_info[i].nhop_1;
            }
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_add(eh, NHOP_INDEX_1s,
                                             start_idx, data, num));
        }

        if (cfg->fld_bmp & EL_FLD_DVP) {
            for (i = 0; i < cfg->num_of_set; i++) {
                data[i] = cfg->member_info[i].dvp;
            }
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_add(eh, DVPs, start_idx, data, num));
        }

        if (cfg->fld_bmp & EL_FLD_NHOP_2_ECMP_GRP_1) {
            fld_name = cfg->ul ? NHOP_INDEX_2s : NHOP_2_OR_ECMP_GROUP_INDEX_1s;
            for (i = 0; i < cfg->num_of_set; i++) {
                data[i] = cfg->member_info[i].nhop_2_or_ecmp_grp_1;
            }
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_add(eh, fld_name, start_idx, data, num));
        }

        if (cfg->fld_bmp & EL_FLD_ECMP_PROC_SWT) {
            for (i = 0; i < cfg->num_of_set; i++) {
                if (cfg->member_info[i].ul_ecmp) {
                    data[i] = 1 << 2;
                } else {
                    data[i] = 0;
                }
                data[i] |= cfg->member_info[i].prot_swt_prfl_idx;
            }
            fld_name = ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXs;
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_add(eh, fld_name, start_idx, data, num));
        }

        if (cfg->fld_bmp & EL_FLD_STRENGTH_PRFL_IDX) {
            for (i = 0; i < cfg->num_of_set; i++) {
                ent_type = (cfg->member_info[i].nhop_1 > 0) ?
                           BCMI_LTSW_SBR_PET_DEF : BCMI_LTSW_SBR_PET_NONE;
                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type,
                                                         &sbr_index));
                data[i] = sbr_index;
            }
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_add(eh, STRENGTH_PROFILE_INDEXs,
                                             start_idx, data, num));
        }
    }

    if (cfg->num_of_unset) {
        sz = sizeof(uint64_t) * cfg->num_of_unset;
        SHR_ALLOC(val0, sz, "bcmLtswXfsEcmpMemberData");
        SHR_NULL_CHECK(val0, SHR_E_MEMORY);
        sal_memset(val0, 0, sz);

        start_idx = cfg->start_idx + cfg->num_of_set;
        num = cfg->num_of_unset;

        if (cfg->fld_bmp & EL_FLD_NHOP_1) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_add(eh, NHOP_INDEX_1s, start_idx,
                                             val0, num));
        }

        if (cfg->fld_bmp & EL_FLD_DVP) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_add(eh, DVPs, start_idx, val0, num));
        }
        if (cfg->fld_bmp & EL_FLD_NHOP_2_ECMP_GRP_1) {
            fld_name = cfg->ul ? NHOP_INDEX_2s : NHOP_2_OR_ECMP_GROUP_INDEX_1s;
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_add(eh, fld_name, start_idx,
                                             val0, num));
        }
        if (cfg->fld_bmp & EL_FLD_ECMP_PROC_SWT) {
            fld_name = ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXs;
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_add(eh, fld_name, start_idx,
                                             val0, num));
        }

        if (cfg->fld_bmp & EL_FLD_STRENGTH_PRFL_IDX) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_add(eh, STRENGTH_PROFILE_INDEXs,
                                               start_idx, val0, num));
        }
    }

exit:
    SHR_FREE(data);
    SHR_FREE(val0);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse entry handle of ECMP_LEVEL0/1t.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [out] cfg Data of ECMP_LEVEL0/1t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_ecmp_level_parse(int unit, bcmlt_entry_handle_t eh, ecmp_level_cfg_t *cfg)
{
    uint64_t *data = NULL;
    int i, j, sz;
    uint32_t act_size = 0;
    const char *fld_name, *lb_mode, *wgt_sz;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, ECMP_IDs, &cfg->ecmp_id));

    if (cfg->fld_bmp & EL_FLD_LB_MODE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_get(eh, LB_MODEs, &lb_mode));
        for (i = 0; i < BCMINT_LTSW_L3_ECMP_LB_MODE_CNT; i++) {
            if (!sal_strcmp(lb_mode_str[i], lb_mode)) {
                break;
            }
        }
        if (i >= BCMINT_LTSW_L3_ECMP_LB_MODE_CNT) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        cfg->lb_mode = i;
    }

    if (cfg->fld_bmp & EL_FLD_MAX_PATHS) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, MAX_PATHSs, &cfg->max_paths));
    }

    if (cfg->fld_bmp & EL_FLD_NUM_PATHS) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, NUM_PATHSs, &cfg->num_paths));
    }

    if (cfg->fld_bmp & EL_FLD_WEIGHTED_SIZE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_get(eh, WEIGHTED_SIZEs, &wgt_sz));
        for (j = 0; j < BCMINT_LTSW_L3_ECMP_WGT_SZ_CNT; j++) {
            if (!sal_strcmp(wgt_sz_str[j], wgt_sz)) {
                break;
            }
        }
        if (j >= BCMINT_LTSW_L3_ECMP_WGT_SZ_CNT) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        cfg->weighted_size = j;
    }

    if (cfg->fld_bmp & EL_FLD_RH_MODE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, RH_MODEs, &cfg->rh_mode));
    }

    if (cfg->fld_bmp & EL_FLD_RH_NUM_PATHS) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, RH_NUM_PATHSs, &cfg->rh_num_paths));
    }

    if (cfg->fld_bmp & EL_FLD_RH_RANDOM_SEED) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, RH_RANDOM_SEEDs, &cfg->rh_random_seed));
    }

    /* Exit if no member info to be fetched.*/
    if (!cfg->num_of_set) {
        SHR_EXIT();
    }

    sz = sizeof(uint64_t) * cfg->num_of_set;
    SHR_ALLOC(data, sz, "bcmLtswXfsEcmpMemberData");
    SHR_NULL_CHECK(data, SHR_E_MEMORY);

    if (cfg->fld_bmp & EL_FLD_NHOP_1) {
        sal_memset(data, 0, sz);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_get(eh, NHOP_INDEX_1s, cfg->start_idx,
                                         data, cfg->num_of_set, &act_size));
        for (i = 0; i < act_size; i++) {
            cfg->member_info[i].nhop_1 = data[i];
        }
    }

    if (cfg->fld_bmp & EL_FLD_DVP) {
        sal_memset(data, 0, sz);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_get(eh, DVPs, cfg->start_idx, data,
                                         cfg->num_of_set, &act_size));
        for (i = 0; i < act_size; i++) {
            cfg->member_info[i].dvp = data[i];
        }
    }

    if (cfg->fld_bmp & EL_FLD_NHOP_2_ECMP_GRP_1) {
        fld_name = cfg->ul ? NHOP_INDEX_2s : NHOP_2_OR_ECMP_GROUP_INDEX_1s;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_get(eh, fld_name, cfg->start_idx, data,
                                         cfg->num_of_set, &act_size));
        for (i = 0; i < act_size; i++) {
            cfg->member_info[i].nhop_2_or_ecmp_grp_1 = data[i];
        }
    }

    if (cfg->fld_bmp & EL_FLD_ECMP_PROC_SWT) {
        sal_memset(data, 0, sz);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_get(eh,
                                         ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXs,
                                         cfg->start_idx, data,
                                         cfg->num_of_set, &act_size));
        for (i = 0; i < act_size; i++) {
            cfg->member_info[i].ul_ecmp = (data[i] & (1 << 2)) ? true : false;
            cfg->member_info[i].prot_swt_prfl_idx = data[i] & 0x3;
        }
    }

exit:
    SHR_FREE(data);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare two integer values and returns the status.
 *
 * \param [in] a First integer value to compare.
 * \param [in] b Second integer value to compare.
 *
 * \retval -1 a is less than b.
 * \retval 0 a equals b.
 * \retval 1 a is greater than b.
 */
static int
ecmp_cmp_int(const void *a, const void *b)
{
    int first, second;

    first = *(int *)a;
    second = *(int *)b;

    if (first < second) {
        return -1;
    } else if (first > second) {
        return 1;
    }
    return 0;
}

/*!
 * \brief Compare two ECMP member.
 *
 * \param [in] a First compared ltidx of egress object.
 * \param [in] b Second compared ltidx of egress object.
 *
 * \retval -1 a is less than b.
 * \retval 0 a equals b.
 * \retval 1 a is greater than b.
 */
static int
ecmp_cmp_member_info(const void *a, const void *b)
{
    bcmi_ltsw_l3_ecmp_member_info_t *emi_1, *emi_2;
    uint64_t first, second;
    uint64_t nhop1 = 0, nhop2 = 0, dvp = 0, ecmp_grp_1 = 0;

    emi_1 = (bcmi_ltsw_l3_ecmp_member_info_t *)a;
    emi_2 = (bcmi_ltsw_l3_ecmp_member_info_t *)b;

    nhop1 = emi_1->nhop_1;
    dvp = emi_1->dvp;
    if (emi_1->ul_ecmp) {
        ecmp_grp_1 = emi_1->nhop_2_or_ecmp_grp_1;
        nhop2 = 0;
    } else {
        ecmp_grp_1 = 0;
        nhop2 = emi_1->nhop_2_or_ecmp_grp_1;
    }
    first = (dvp << 48) | (ecmp_grp_1 << 32) | (nhop2 << 16) | nhop1;

    nhop1 = emi_2->nhop_1;
    dvp = emi_2->dvp;
    if (emi_2->ul_ecmp) {
        ecmp_grp_1 = emi_2->nhop_2_or_ecmp_grp_1;
        nhop2 = 0;
    } else {
        ecmp_grp_1 = 0;
        nhop2 = emi_2->nhop_2_or_ecmp_grp_1;
    }
    second = (dvp << 48) | (ecmp_grp_1 << 32) | (nhop2 << 16) | nhop1;

    if (first < second) {
        return -1;
    } else if (first > second) {
        return 1;
    }
    return 0;
}

/*!
 * \brief Sort DLB members by nexthop index.
 *
 * \param [in] unit Unit number.
 * \param [in/out] arr The array of elements to be sorted.
 * \param [in] arr_cnt Number of elements in arr.
 * \param [in] size Size of element.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Failure.
 */
static int
ecmp_member_sort(int unit, void *arr, int arr_cnt, int size)
{
    SHR_FUNC_ENTER(unit);

    if (size == sizeof(int)) {
        sal_qsort(arr, arr_cnt, size, ecmp_cmp_int);
    } else if (size == sizeof(bcmi_ltsw_l3_ecmp_member_info_t)) {
        sal_qsort(arr, arr_cnt, size, ecmp_cmp_member_info);
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Operate flex counter related fields of nexthop entry.
 *
 * \param [in] unit Unit number.
 * \param [in] intf L3 interface ID of egress object.
 * \param [in/out] flexctr Pointer to flex counter config info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Failure.
 */
static int
l3_egress_flexctr_op(int unit, bcm_if_t intf_id, l3_egr_flexctr_cfg_t *flexctr)
{
    int dunit, ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    const char *ltname, *fldname;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t opcode;
    uint64_t val;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (l3_egress_obj_id_to_ltidx(unit, intf_id, &ltidx, &type));

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_OL) {
        if (L3_OL_NHOP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        ltname = EGR_L3_NEXT_HOP_1s;
        fldname = L3_NEXT_HOP_1s;
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_UL) {
        if (L3_UL_NHOP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        ltname = EGR_L3_NEXT_HOP_2s;
        fldname = L3_NEXT_HOP_2s;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, fldname, ltidx));

    if (flexctr->op == L3_EGR_FLXCTR_OP_SET) {
        if (flexctr->flags & L3_EGR_FLXCTR_F_ACTION) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(eh, FLEX_CTR_ACTIONs, flexctr->action));
        }

        if (flexctr->flags & L3_EGR_FLXCTR_F_INDEX) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(eh, FLEX_CTR_INDEXs, flexctr->index));
        }

        opcode = BCMLT_OPCODE_UPDATE;
    } else if (flexctr->op == L3_EGR_FLXCTR_OP_GET) {
        opcode = BCMLT_OPCODE_LOOKUP;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, opcode, BCMLT_PRIORITY_NORMAL));

    if (flexctr->op == L3_EGR_FLXCTR_OP_GET) {
        if (flexctr->flags & L3_EGR_FLXCTR_F_ACTION) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_get(eh, FLEX_CTR_ACTIONs, &val));
            flexctr->action = val;
        }

        if (flexctr->flags & L3_EGR_FLXCTR_F_INDEX) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_get(eh, FLEX_CTR_INDEXs, &val));
            flexctr->index = val;
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set source next hop entry.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 * \param [in] egr Next hop entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
snh_ing_set(int unit, int nhidx, bcm_l3_egress_t *egr)
{
    snh_cfg_t snh;
    int dunit, l3_oif = 0;
    int tnl_idx = 0;
    int tnl_flg = 0;
    bcmlt_opcode_t op;
    bcmi_ltsw_l3_egr_intf_type_t intf_type;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&snh, 0, sizeof(snh_cfg_t));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_egr_intf_to_index(unit, egr->intf, &l3_oif, &intf_type));
    if (intf_type != BCMI_LTSW_L3_EGR_INTF_T_OL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_intf_tnl_init_get(unit, l3_oif, NULL, &tnl_idx));
    tnl_flg = (tnl_idx == 0) ? 0 : 1;

    snh.l3_oif_value_type = l3_oif << 1 | tnl_flg;
    snh.fld_bmp |= SNH_FLD_L3OIF;
    snh.l3_src_nhop_index = nhidx;

    op = L3_OL_NHOP_FBMP_GET(unit, nhidx) ? BCMLT_OPCODE_INSERT :
                                            BCMLT_OPCODE_UPDATE;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_SRC_NEXT_HOP_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (lt_ing_snh_fill(unit, eh, &snh));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete source next hop entry.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
snh_ing_delete(int unit, int nhidx)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_SRC_NEXT_HOP_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, L3_SRC_NHOP_INDEXs, nhidx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set ingress overlay next hop entry.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 * \param [in] egr Next hop entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olnh_ing_set(int unit, int nhidx, bcm_l3_egress_t *egr)
{
    inh1_cfg_t inh;
    int dunit, l3_oif, vp = 0;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t op;
    bcmi_ltsw_gport_info_t gport_info;
    bcm_gport_t gport;
    int l2_if, my_modid;
    bcmi_ltsw_l3_egr_intf_type_t type;
    bcmi_ltsw_l3_mtu_cfg_t mtu_cfg;
    bool l3mc = false, cascaded = false;
    int mc_group, sbr_index, ol_egr_obj, ul_egr_obj = 0;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_ING_L3_NEXT_HOP_1;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type =
        BCMI_LTSW_SBR_PET_NO_FORWARDING_TYPE_NO_SYSTEM_OPCODE;
    olnh_entry_info_t *olnh_ei = &l3_egr_info[unit].olnh_ei[nhidx];
    bcmi_ltsw_virtual_vp_info_t vp_info;
    bcmi_ltsw_ecmp_member_dest_info_t dest_info = {0};

    SHR_FUNC_ENTER(unit);

    if (L3_OL_NHOP_FBMP_GET(unit, nhidx)) {
        op = BCMLT_OPCODE_INSERT;
    } else {
        op = BCMLT_OPCODE_UPDATE;
    }

    sal_memset(&inh, 0, sizeof(inh));
    inh.nhop_index_1 = nhidx;

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_egr_intf_to_index(unit, egr->intf, &l3_oif, &type));
    if (type != BCMI_LTSW_L3_EGR_INTF_T_OL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    inh.l3_oif_1 = l3_oif;
    inh.fld_bmp |= INH1_FLD_L3OIF1;

    /* MTU Profile. */
    sal_memset(&mtu_cfg, 0, sizeof(bcmi_ltsw_l3_mtu_cfg_t));
    mtu_cfg.mtu = egr->mtu ? egr->mtu : BCMI_LTSW_L3_MTU_DEFAULT_SIZE;
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_mtu_set(unit, &mtu_cfg));
    inh.mtu_profile_ptr = mtu_cfg.index;
    inh.fld_bmp |= INH1_FLD_MTU_PROF;

    if (egr->flags2 & BCM_L3_FLAGS2_MC_GROUP) {
        if (_BCM_MULTICAST_IS_SET(egr->mc_group)) {
            if (!_BCM_MULTICAST_IS_L2(egr->mc_group)) {
                l3mc = true;
            }
            mc_group = _BCM_MULTICAST_ID_GET(egr->mc_group);
            inh.dest = mc_group;
            inh.dest_valid = l3mc ? DEST_VALID_L3MC : DEST_VALID_L2MC;
            inh.fld_bmp |= INH1_FLD_DEST | INH1_FLD_DEST_VALID;
            inh.data_type = INH1_VIEW_L2_OIF;
            inh.fld_bmp |= INH1_FLD_DATA_TYPE;
        } else {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        if (egr->flags & BCM_L3_DST_DISCARD) {
            /* Point to the reserved black hole l2_oif. */
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_port_to_l2_if(unit, BCM_GPORT_BLACK_HOLE,
                                         &l2_if));
            inh.dest = l2_if;
            inh.dest_valid = DEST_VALID_L2OIF;
            inh.data_type = INH1_VIEW_L2_OIF;
            inh.fld_bmp |= INH1_FLD_DEST | INH1_FLD_DEST_VALID |
                           INH1_FLD_DATA_TYPE;
        } else if (bcmi_ltsw_virtual_vp_is_vfi_type(unit, egr->port)) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_virtual_vfi_type_vp_get(unit, egr->port, &vp));
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_virtual_vp_info_get(unit, vp, &vp_info));

            if (vp_info.flags & BCMI_LTSW_VIRTUAL_VP_CASCADED) {
                if ((op == BCMLT_OPCODE_UPDATE) &&
                    !(olnh_ei->flags & BCMI_LTSW_L3_EGR_OBJ_INFO_F_CASCADED)) {
                    SHR_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Not support to change a non-cascaded "
                                        "egress object to be cascaded.\n")));
                }

                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_virtual_port_egress_obj_get(unit, egr->port,
                                                           &ul_egr_obj));
                cascaded = true;
            }
            inh.dvp = vp;
            inh.fld_bmp |= INH1_FLD_DVP;

            inh.data_type = INH1_VIEW_DVP;
            inh.fld_bmp |= INH1_FLD_DATA_TYPE;
        } else {
            if (egr->flags & BCM_L3_TGID) {
                gport_info.gport_type = _SHR_GPORT_TYPE_TRUNK;
                gport_info.tgid = egr->trunk;
                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_port_gport_construct(unit, &gport_info, &gport));
            } else if (BCM_GPORT_IS_SET(egr->port)) {
                gport = egr->port;
            } else {
                SHR_IF_ERR_EXIT
                    (bcm_ltsw_stk_my_modid_get(unit, &my_modid));
                if (egr->module == my_modid) {
                    gport_info.gport_type = _SHR_GPORT_TYPE_LOCAL;
                } else {
                    gport_info.gport_type = _SHR_GPORT_TYPE_MODPORT;
                    gport_info.modid = egr->module;
                }
                gport_info.port = egr->port;
                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_port_gport_construct(unit,
                                                    &gport_info,
                                                    &gport));
            }
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_port_to_l2_if(unit, gport, &l2_if));

            inh.dest = l2_if;
            inh.dest_valid = DEST_VALID_L2OIF;
            inh.fld_bmp |= INH1_FLD_DEST | INH1_FLD_DEST_VALID;
            inh.data_type = INH1_VIEW_L2_OIF;
            inh.fld_bmp |= INH1_FLD_DATA_TYPE;
        }
    }

    if (egr->flags2 & BCM_L3_FLAGS2_FIELD_ONLY) {
        ent_type = BCMI_LTSW_SBR_PET_NONE;
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &sbr_index));
    inh.strength_prfl_idx = sbr_index;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_NEXT_HOP_1_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (lt_ing_nh_1_fill(unit, eh, &inh));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));

    /* Delete old MTU entry. */
    if (!L3_OL_NHOP_FBMP_GET(unit, nhidx)) {
        mtu_cfg.index = L3_OL_NHOP_MTU_PTR(unit, nhidx);
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_l3_mtu_delete(unit, &mtu_cfg));
    }
    L3_OL_NHOP_MTU_PTR(unit, nhidx) = inh.mtu_profile_ptr;

    if (egr->flags3 & BCM_L3_FLAGS3_EGRESS_CASCADED) {
        cascaded = true;
    }

    /* Update the ECMP Memeber destination. */
    if (olnh_ei->flags & BCMI_LTSW_L3_EGR_OBJ_INFO_F_CASCADED) {
        SHR_IF_ERR_EXIT
            (xfs_ltsw_l3_egress_obj_id_construct(unit, nhidx,
                                                 BCMI_LTSW_L3_EGR_OBJ_T_OL,
                                                 &ol_egr_obj));
        dest_info.type = BCMI_LTSW_ECMP_MEMBER_DEST_T_L3_CASCADED;
        dest_info.ol_egr_obj = ol_egr_obj;
        dest_info.ul_egr_obj = ul_egr_obj;
        dest_info.dvp = vp;
        SHR_IF_ERR_EXIT
            (xfs_ltsw_ecmp_member_dest_update(unit, &dest_info));
    }

    /* Update the egress object info. */
    if (cascaded) {
        olnh_ei->flags |= BCMI_LTSW_L3_EGR_OBJ_INFO_F_CASCADED;
        olnh_ei->dvp = vp;
        olnh_ei->ul_egr_obj = ul_egr_obj;
    }

exit:
    if (SHR_FUNC_ERR() && (inh.fld_bmp & INH1_FLD_MTU_PROF)) {
        mtu_cfg.index = inh.mtu_profile_ptr;
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_l3_mtu_delete(unit, &mtu_cfg));
    }
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set egress overlay next hop entry.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 * \param [in] egr Next hop entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olnh_egr_set(int unit, int nhidx, bcm_l3_egress_t *egr)
{
    enh1_cfg_t enh;
    int dunit, ptr;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t op;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_EGR_L3_NEXT_HOP_1;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type;
    bcmi_ltsw_sbr_tbl_hdl_t hdl = BCMI_LTSW_SBR_TH_EGR_L3_NEXT_HOP_1;
    bcmi_ltsw_sbr_fld_type_t fld_type = BCMI_LTSW_SBR_FT_TOS_PTR;
    int sbr_index, mpls_label = 0;
    bool mpls_view = false;

    SHR_FUNC_ENTER(unit);

    op = L3_OL_NHOP_FBMP_GET(unit, nhidx) ? BCMLT_OPCODE_INSERT :
                                            BCMLT_OPCODE_UPDATE;

    dunit = bcmi_ltsw_dev_dunit(unit);

    sal_memset(&enh, 0, sizeof(enh));
    enh.nhop_index_1 = nhidx;

    if (egr->flags2 & BCM_L3_FLAGS2_MPLS_PHP) {
        mpls_view = true;
        enh.edit_ctrl_id = ENH1_EDIT_CTRL_MPLS_PHP;
    } else if (egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_PRESERVE) {
        mpls_view = true;
        enh.edit_ctrl_id = ENH1_EDIT_CTRL_MPLS_SWAP2SELF;
    } else if (egr->mpls_label != BCM_MPLS_LABEL_INVALID) {
        mpls_view = true;
        enh.edit_ctrl_id = ENH1_EDIT_CTRL_MPLS_SWAP;

        /* Label value 20 bits. */
        if (egr->mpls_label > 0xFFFFF) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit, "Invalid MPLS Label (%d).\n"),
                                 egr->mpls_label));
        }

        mpls_label = egr->mpls_label;
    }

    if (!ltsw_feature(unit, LTSW_FT_MPLS) && mpls_view) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_UNAVAIL,
            (BSL_META_U(unit, "No Support MPLS.\n")));
    }

    if (mpls_view) {
        enh.fld_bmp = ENH1_FLD_VIEW | ENH1_FLD_DMAC | ENH1_FLD_EDIT_CTRL_ID |
                      ENH1_FLD_CLASS_ID | ENH1_FLD_SWAP_LABEL |
                      ENH1_FLD_EXP_REMARK_BASE_PTR;
        enh.view = ENH1_VT_MPLS;

        /* Class of service 3 bits. */
        if (egr->mpls_exp > 0x7) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit, "Invalid MPLS EXP (%d).\n"),
                                 egr->mpls_exp));
        }

        /* TTL and BOS in swap label are not used. */
        BCM_MPLS_HEADER_LABEL_SET(enh.swap_label, mpls_label);
        BCM_MPLS_HEADER_EXP_SET(enh.swap_label, egr->mpls_exp);

        if (egr->mpls_qos_map_id > 0) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_qos_map_id_resolve(unit,
                                              egr->mpls_qos_map_id,
                                              NULL,
                                              &ptr));
            enh.exp_remark_base_ptr = ptr;
        }

    } else {
        enh.fld_bmp = ENH1_FLD_VIEW | ENH1_FLD_DMAC | ENH1_FLD_EDIT_CTRL_ID |
                      ENH1_FLD_CLASS_ID | ENH1_FLD_TOS_REMARK_BASE_PTR;
        enh.view = ENH1_VT_NHOP;

        if (egr->flags2 & BCM_L3_FLAGS2_FIELD_ONLY) {
            /*
             * IFP assigns overlay nexthop to change the L2 fields of
             * bridged packets.
             */
            enh.edit_ctrl_id = ENH1_EDIT_CTRL_L2_MODIFY;
        } else {
            enh.edit_ctrl_id = ENH1_EDIT_CTRL_ROUTE;
        }

        if (egr->qos_map_id > 0) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_qos_map_id_resolve(unit,
                                              egr->qos_map_id,
                                              NULL,
                                              &ptr));
            enh.tos_remark_base_ptr = ptr;

            SHR_IF_ERR_EXIT
                (bcmi_ltsw_sbr_fld_value_get(unit, hdl, fld_type, &sbr_index));
            enh.tos_ptr_str = sbr_index;
        }
    }

    bcmi_ltsw_util_mac_to_uint64(&(enh.dmac), egr->mac_addr);
    enh.class_id = egr->intf_class;

    if (egr->flags & BCM_L3_KEEP_DSTMAC && !egr->intf_class) {
        ent_type = BCMI_LTSW_SBR_PET_NONE;
    } else if (egr->flags & BCM_L3_KEEP_DSTMAC) {
        ent_type = BCMI_LTSW_SBR_PET_NO_MAC_DA;
    } else if (!egr->intf_class) {
        ent_type = BCMI_LTSW_SBR_PET_NO_CLASSID;
    } else {
        ent_type = BCMI_LTSW_SBR_PET_DEF;
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &sbr_index));
    enh.strength_prfl_idx = sbr_index;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, EGR_L3_NEXT_HOP_1s, &eh));

    SHR_IF_ERR_EXIT
        (lt_egr_nh_1_fill(unit, eh, &enh));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get ingress overlay next hop entry.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 * \param [out] egr Next hop entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olnh_ing_get(int unit, int nhidx, bcm_l3_egress_t *egr)
{
    inh1_cfg_t inh;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcm_gport_t gport;
    bcmi_ltsw_l3_mtu_cfg_t mtu_cfg;
    int mc_group;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_NEXT_HOP_1_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, NHOP_INDEX_1s, nhidx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (lt_ing_nh_1_parse(unit, eh, &inh));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_egr_index_to_intf(unit, inh.l3_oif_1,
                                        BCMI_LTSW_L3_EGR_INTF_T_OL,
                                        &egr->intf));

    if (egr->intf == bcmi_ltsw_l3_intf_l2tocpu_idx_get(unit)) {
        egr->flags |= BCM_L3_L2TOCPU;
    }

    if (inh.data_type == INH1_VIEW_DVP) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_virtual_vp_encode_gport(unit, inh.dvp, &egr->port));
    } else {
        if (inh.dest_valid & DEST_VALID_L2OIF) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_l2_if_to_port(unit, inh.dest, &gport));

            if (gport == BCM_GPORT_BLACK_HOLE) {
                egr->flags |= BCM_L3_DST_DISCARD;
                egr->port = gport;
            } else if (BCM_GPORT_IS_TRUNK(gport)) {
                egr->flags |= BCM_L3_TGID;
                egr->trunk = BCM_GPORT_TRUNK_GET(gport);
            } else if (BCM_GPORT_IS_MODPORT(gport)) {
                egr->port = BCM_GPORT_MODPORT_PORT_GET(gport);
                egr->module = BCM_GPORT_MODPORT_MODID_GET(gport);
            } else {
                egr->port = gport;
            }
        }  else if (inh.dest_valid & DEST_VALID_L2MC) {
            mc_group = (int)(inh.dest);
            _BCM_MULTICAST_GROUP_SET(egr->mc_group, _BCM_MULTICAST_TYPE_L2,
                                     mc_group);
            egr->flags2 |= BCM_L3_FLAGS2_MC_GROUP;
        } else if (inh.dest_valid & DEST_VALID_L3MC) {
           SHR_IF_ERR_EXIT
               (bcmi_ltsw_multicast_l3_grp_id_get
                   (unit, (uint32_t)(inh.dest), &(egr->mc_group)));
            egr->flags2 |= BCM_L3_FLAGS2_MC_GROUP;
        } else {
            
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    sal_memset(&mtu_cfg, 0, sizeof(bcmi_ltsw_l3_mtu_cfg_t));
    mtu_cfg.index = inh.mtu_profile_ptr;
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_mtu_get(unit, &mtu_cfg));

    egr->mtu = mtu_cfg.mtu;

    if (L3_OL_NHOP_FLAGS(unit, nhidx) & L3_OL_NHOP_F_CASCADED) {
        egr->flags3 |= BCM_L3_FLAGS3_EGRESS_CASCADED;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}
/*!
 * \brief Get egress overlay next hop entry.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 * \param [out] egr Next hop entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olnh_egr_get(int unit, int nhidx, bcm_l3_egress_t *egr)
{
    enh1_cfg_t enh;
    int dunit, sbr_index_none, sbr_index_no_dmac;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_EGR_L3_NEXT_HOP_1;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, EGR_L3_NEXT_HOP_1s, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, L3_NEXT_HOP_1s, nhidx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (lt_egr_nh_1_parse(unit, eh, &enh));

    bcmi_ltsw_util_uint64_to_mac(egr->mac_addr, &enh.dmac);
    egr->intf_class = (int)(enh.class_id);

    if (enh.tos_remark_base_ptr) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_qos_map_id_construct(unit, enh.tos_remark_base_ptr,
                                            bcmiQosMapTypeFwdEcnDscpEgress,
                                            &egr->qos_map_id));
    }

    if (enh.exp_remark_base_ptr) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_qos_map_id_construct(unit, enh.exp_remark_base_ptr,
                                            bcmiQosMapTypeMplsEgress,
                                            &egr->mpls_qos_map_id));
    }

    egr->mpls_label = BCM_MPLS_LABEL_INVALID;
    if (enh.view == ENH1_VT_MPLS) {
        BCM_MPLS_HEADER_EXP_GET(enh.swap_label, egr->mpls_exp);
        if (enh.edit_ctrl_id == ENH1_EDIT_CTRL_MPLS_PHP) {
            egr->flags2 |= BCM_L3_FLAGS2_MPLS_PHP;
        } else if (enh.edit_ctrl_id == ENH1_EDIT_CTRL_MPLS_SWAP2SELF) {
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_PRESERVE;
        } else {
            BCM_MPLS_HEADER_LABEL_GET(enh.swap_label, egr->mpls_label);
        }
    } else if (enh.edit_ctrl_id == ENH1_EDIT_CTRL_L2_MODIFY) {
        egr->flags2 |= BCM_L3_FLAGS2_FIELD_ONLY;
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, BCMI_LTSW_SBR_PET_NONE,
                                             &sbr_index_none));
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth,
                                             BCMI_LTSW_SBR_PET_NO_MAC_DA,
                                             &sbr_index_no_dmac));
    if ((enh.strength_prfl_idx == sbr_index_none) ||
         (enh.strength_prfl_idx == sbr_index_no_dmac)) {
        egr->flags |= BCM_L3_KEEP_DSTMAC;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if a trunk is a front panel trunk.
 *
 * \param [in] unit Unit number.
 * \param [in] tid Trunk ID.
 *
 * \retval 0 Not a front panel trunk.
 * \retval 1 A front panel trunk.
 * \retval <0 Failure.
 */
static int
trunk_type_is_frontpanel(int unit, bcm_trunk_t tid)
{
    bcm_trunk_chip_info_t trunk_info = {0};
    int rv;

    if (tid < 0) {
        return SHR_E_PARAM;
    }

    rv = bcm_ltsw_trunk_chip_info_get(unit, &trunk_info);
    if (SHR_SUCCESS(rv)) {
        if (tid <= trunk_info.trunk_id_max) {
            rv = 1;
        } else {
            rv = 0;
        }
    }

    return rv;
}

/*!
 * \brief Get underlay next hop index for system port.
 *
 * \param [in] unit Unit number.
 * \param [in] egr Next hop entry data.
 * \param [out] nhidx Nhop2 index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ing_sys_dst_nhop2_get(int unit, bcm_l3_egress_t *egr, int *nhidx)
{
    bcmi_ltsw_gport_info_t gport_info;
    bcm_gport_t gport;
    bcm_trunk_t tid = BCM_TRUNK_INVALID;
    int my_modid;

    SHR_FUNC_ENTER(unit);

    *nhidx = 0;

    if (!L3_SYSPORT_TO_UL_NHIDX_ENABLED(unit)) {
        SHR_EXIT();
    }

    if ((egr->flags2 & BCM_L3_FLAGS2_MC_GROUP) ||
        (egr->flags & BCM_L3_DST_DISCARD) ||
        (egr->flags & BCM_L3_L2TOCPU) ||
        (bcmi_ltsw_virtual_vp_is_vfi_type(unit, egr->port))) {
        SHR_EXIT();
    }

    if (egr->flags & BCM_L3_TGID) {
        tid = egr->trunk;
    } else if (BCM_GPORT_IS_SET(egr->port)) {
        gport = egr->port;
        if (BCM_GPORT_IS_TRUNK(gport)) {
            tid = BCM_GPORT_TRUNK_GET(gport);
        }
    } else {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &my_modid));
        if (egr->module == my_modid) {
            gport_info.gport_type = _SHR_GPORT_TYPE_LOCAL;
        } else {
            gport_info.gport_type = _SHR_GPORT_TYPE_MODPORT;
            gport_info.modid = egr->module;
        }
        gport_info.port = egr->port;
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_port_gport_construct(unit,
                                            &gport_info,
                                            &gport));
    }

    if (tid != BCM_TRUNK_INVALID) {
        if (trunk_type_is_frontpanel(unit, tid)) {
            *nhidx = L3_TRUNK_UL_NHIDX(unit, tid);
        } else {
            *nhidx = 0;
        }
    } else {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_port_tab_get(unit, gport,
                                    BCMI_PT_ING_SYS_DEST_NHOP_2, nhidx));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set underlay next hop index for system port.
 *
 * \param [in] unit Unit number.
 * \param [in] egr Next hop entry data.
 * \param [in] nhidx Nhop2 index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ing_sys_dst_nhop2_set(int unit, bcm_l3_egress_t *egr, int nhidx)
{
    bcmi_ltsw_gport_info_t gport_info;
    bcm_gport_t gport;
    bcm_trunk_t tid = BCM_TRUNK_INVALID;
    int my_modid, i, cnt, sz;
    bcm_trunk_member_t *members = NULL;

    SHR_FUNC_ENTER(unit);

    if (!L3_SYSPORT_TO_UL_NHIDX_ENABLED(unit)) {
        SHR_EXIT();
    }

    if ((egr->flags2 & BCM_L3_FLAGS2_MC_GROUP) ||
        (egr->flags & BCM_L3_DST_DISCARD) ||
        (egr->flags & BCM_L3_L2TOCPU) ||
        (bcmi_ltsw_virtual_vp_is_vfi_type(unit, egr->port))) {
        SHR_EXIT();
    }

    if (egr->flags & BCM_L3_TGID) {
        tid = egr->trunk;
        gport_info.gport_type = _SHR_GPORT_TYPE_TRUNK;
        gport_info.tgid = egr->trunk;
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_port_gport_construct(unit, &gport_info, &gport));
    } else if (BCM_GPORT_IS_SET(egr->port)) {
        gport = egr->port;
        if (BCM_GPORT_IS_TRUNK(gport)) {
            tid = BCM_GPORT_TRUNK_GET(gport);
        }
    } else {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &my_modid));
        if (egr->module == my_modid) {
            gport_info.gport_type = _SHR_GPORT_TYPE_LOCAL;
        } else {
            gport_info.gport_type = _SHR_GPORT_TYPE_MODPORT;
            gport_info.modid = egr->module;
        }
        gport_info.port = egr->port;
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_port_gport_construct(unit,
                                            &gport_info,
                                            &gport));
    }

    if (tid != BCM_TRUNK_INVALID) {
        if (!trunk_type_is_frontpanel(unit, tid)) {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT
            (bcm_ltsw_trunk_get(unit, tid, NULL, 0, NULL, &cnt));

        if (cnt) {
            sz = cnt * sizeof(bcm_trunk_member_t);
            SHR_ALLOC(members, sz, "bcmXfsl3EgrTrunkports");
            SHR_NULL_CHECK(members, SHR_E_MEMORY);
            sal_memset(members, 0, sz);

            SHR_IF_ERR_EXIT
                (bcm_ltsw_trunk_get(unit, tid, NULL, cnt, members, &cnt));

            for (i = 0; i < cnt; i++) {
                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_port_tab_set(unit, members[i].gport,
                                            BCMI_PT_ING_SYS_DEST_NHOP_2,
                                            nhidx));
            }
        }
    } else {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_port_tab_set(unit, gport,
                                    BCMI_PT_ING_SYS_DEST_NHOP_2, nhidx));
    }

    if (tid != BCM_TRUNK_INVALID) {
        L3_TRUNK_UL_NHIDX(unit, tid) = nhidx;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set ingress underlay next hop entry.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 * \param [in] egr Next hop entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulnh_ing_set(int unit, int nhidx, bcm_l3_egress_t *egr)
{
    inh2_cfg_t inh;
    int dunit, vp;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t op;
    bcmi_ltsw_gport_info_t gport_info;
    bcm_gport_t gport;
    int l2_if, my_modid;
    bool l3mc = false;
    int mc_group;

    SHR_FUNC_ENTER(unit);

    sal_memset(&inh, 0, sizeof(inh));
    inh.fld_bmp |= INH2_FLD_DVP | INH2_FLD_DEST | INH2_FLD_DEST_VALID;
    inh.nhop_index_2 = nhidx;

    if (egr->flags2 & BCM_L3_FLAGS2_MC_GROUP) {
        if (_BCM_MULTICAST_IS_SET(egr->mc_group)) {
            if (!_BCM_MULTICAST_IS_L2(egr->mc_group)) {
                l3mc = true;
            }
            mc_group = _BCM_MULTICAST_ID_GET(egr->mc_group);
            inh.dest = mc_group;
            inh.dest_valid = l3mc ? DEST_VALID_L3MC : DEST_VALID_L2MC;
        } else {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        if (egr->flags & BCM_L3_DST_DISCARD) {
            /* Point to the reserved black hole l2_oif. */
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_port_to_l2_if(unit, BCM_GPORT_BLACK_HOLE,
                                         &l2_if));
            inh.dest = l2_if;
            inh.dest_valid = DEST_VALID_L2OIF;
        } else if (bcmi_ltsw_virtual_vp_is_vfi_type(unit, egr->port)) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_virtual_vfi_type_vp_get(unit, egr->port, &vp));
            inh.dvp = vp;
        } else {
            if (egr->flags & BCM_L3_TGID) {
                gport_info.gport_type = _SHR_GPORT_TYPE_TRUNK;
                gport_info.tgid = egr->trunk;
                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_port_gport_construct(unit, &gport_info, &gport));
            } else if (BCM_GPORT_IS_SET(egr->port)) {
                gport = egr->port;
            } else {
                SHR_IF_ERR_EXIT
                    (bcm_ltsw_stk_my_modid_get(unit, &my_modid));
                if (egr->module == my_modid) {
                    gport_info.gport_type = _SHR_GPORT_TYPE_LOCAL;
                } else {
                    gport_info.gport_type = _SHR_GPORT_TYPE_MODPORT;
                    gport_info.modid = egr->module;
                }
                gport_info.port = egr->port;
                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_port_gport_construct(unit,
                                                    &gport_info,
                                                    &gport));
            }

            SHR_IF_ERR_EXIT
                (bcmi_ltsw_port_to_l2_if(unit, gport, &l2_if));

            inh.dest = l2_if;
            inh.dest_valid = DEST_VALID_L2OIF;
        }
    }

    op = L3_UL_NHOP_FBMP_GET(unit, nhidx) ? BCMLT_OPCODE_INSERT :
                                            BCMLT_OPCODE_UPDATE;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_NEXT_HOP_2_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (lt_ing_nh_2_fill(unit, eh, &inh));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set egress underlay next hop entry.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 * \param [in] egr Next hop entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulnh_egr_set(int unit, int nhidx, bcm_l3_egress_t *egr)
{
    enh2_cfg_t enh;
    int dunit, l3_oif;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t op;
    bcmi_ltsw_l3_egr_intf_type_t type;

    SHR_FUNC_ENTER(unit);

    sal_memset(&enh, 0, sizeof(enh));

    enh.nhop_index_2 = nhidx;
    bcmi_ltsw_util_mac_to_uint64(&(enh.dmac), egr->mac_addr);
    enh.fld_bmp |= ENH2_FLD_DMAC;

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_egr_intf_to_index(unit, egr->intf, &l3_oif, &type));
    if (type != BCMI_LTSW_L3_EGR_INTF_T_UL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    enh.l3_oif_2 = l3_oif;
    enh.fld_bmp |= ENH2_FLD_L3OIF2;

    enh.view = ENH2_VT_NHOP;
    enh.fld_bmp |= ENH2_FLD_VIEW;

    enh.class_id = egr->intf_class;
    enh.fld_bmp |= ENH2_FLD_CLASS_ID;

    enh.efp_ctrl_id = egr->egress_opaque_ctrl_id;
    enh.fld_bmp |= ENH2_FLD_EFP_CTRL_ID;

    op = L3_UL_NHOP_FBMP_GET(unit, nhidx) ? BCMLT_OPCODE_INSERT :
                                            BCMLT_OPCODE_UPDATE;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, EGR_L3_NEXT_HOP_2s, &eh));

    SHR_IF_ERR_EXIT
        (lt_egr_nh_2_fill(unit, eh, &enh));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set egress underlay next hop entry for IFA.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 * \param [in] egr Next hop entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulnh_egr_ifa_set(int unit, int nhidx, bcmi_ltsw_l3_egr_ifa_t *egr_ifa)
{
    enh2_cfg_t enh;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t op;

    SHR_FUNC_ENTER(unit);

    sal_memset(&enh, 0, sizeof(enh));

    enh.nhop_index_2 = nhidx;

    enh.view = ENH2_VT_IFA;
    enh.fld_bmp |= ENH2_FLD_VIEW;

    enh.ifa_ip_protocol = egr_ifa->ip_protocol;
    enh.fld_bmp |= ENH2_FLD_IFA_IP_PROTOCOL;

    enh.ifa_max_length = egr_ifa->max_length;
    enh.fld_bmp |= ENH2_FLD_IFA_MAX_LENGTH;

    enh.ifa_hop_limit = egr_ifa->hop_limit;
    enh.fld_bmp |= ENH2_FLD_IFA_HOP_LIMIT;

    enh.ifa_gns = 0xf;
    enh.fld_bmp |= ENH2_FLD_IFA_GNS;

    enh.ifa_req_vector = 0xff;
    enh.fld_bmp |= ENH2_FLD_IFA_REQ_VECTOR;

    enh.ifa_curr_length = 0x1;
    enh.fld_bmp |= ENH2_FLD_IFA_CURR_LENGTH;

    op = L3_UL_NHOP_FBMP_GET(unit, nhidx) ? BCMLT_OPCODE_INSERT :
                                            BCMLT_OPCODE_UPDATE;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, EGR_L3_NEXT_HOP_2s, &eh));

    SHR_IF_ERR_EXIT
        (lt_egr_nh_2_fill(unit, eh, &enh));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get ingress underlay next hop entry.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 * \param [out] egr Next hop entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulnh_ing_get(int unit, int nhidx, bcm_l3_egress_t *egr)
{
    inh2_cfg_t inh;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcm_gport_t gport;
    int mc_group;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_NEXT_HOP_2_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, NHOP_INDEX_2s, nhidx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (lt_ing_nh_2_parse(unit, eh, &inh));

    if (!inh.dest_valid) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_virtual_vp_encode_gport(unit, inh.dvp, &egr->port));
    } else {
        if (inh.dest_valid & DEST_VALID_L2OIF) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_l2_if_to_port(unit, inh.dest, &gport));

            if (gport == BCM_GPORT_BLACK_HOLE) {
                egr->flags |= BCM_L3_DST_DISCARD;
                egr->port = gport;
            } else if (BCM_GPORT_IS_TRUNK(gport)) {
                egr->flags |= BCM_L3_TGID;
                egr->trunk = BCM_GPORT_TRUNK_GET(gport);
            } else if (BCM_GPORT_IS_MODPORT(gport)) {
                egr->port = BCM_GPORT_MODPORT_PORT_GET(gport);
                egr->module = BCM_GPORT_MODPORT_MODID_GET(gport);
            } else {
                egr->port = gport;
            }
        }  else if (inh.dest_valid & DEST_VALID_L2MC) {
            mc_group = (int)(inh.dest);
            _BCM_MULTICAST_GROUP_SET(egr->mc_group, _BCM_MULTICAST_TYPE_L2,
                                     mc_group);
            egr->flags2 |= BCM_L3_FLAGS2_MC_GROUP;
        } else if (inh.dest_valid & DEST_VALID_L3MC) {
           SHR_IF_ERR_EXIT
               (bcmi_ltsw_multicast_l3_grp_id_get
                   (unit, (uint32_t)(inh.dest), &(egr->mc_group)));
            egr->flags2 |= BCM_L3_FLAGS2_MC_GROUP;
        } else {
            
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }

    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get egress underlay next hop entry.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 * \param [out] egr Next hop entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulnh_egr_get(int unit, int nhidx, bcm_l3_egress_t *egr)
{
    enh2_cfg_t enh;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, EGR_L3_NEXT_HOP_2s, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, L3_NEXT_HOP_2s, nhidx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (lt_egr_nh_2_parse(unit, eh, &enh));

    if (enh.view != ENH2_VT_NHOP) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    bcmi_ltsw_util_uint64_to_mac(egr->mac_addr, &enh.dmac);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_egr_index_to_intf(unit, enh.l3_oif_2,
                                        BCMI_LTSW_L3_EGR_INTF_T_UL,
                                        &egr->intf));

    egr->intf_class = enh.class_id;
    egr->egress_opaque_ctrl_id = enh.efp_ctrl_id;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get egress underlay next hop entry for IFA.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 * \param [out] egr Next hop entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulnh_egr_ifa_get(int unit, int nhidx, bcmi_ltsw_l3_egr_ifa_t *egr_ifa)
{
    enh2_cfg_t enh;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, EGR_L3_NEXT_HOP_2s, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, L3_NEXT_HOP_2s, nhidx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (lt_egr_nh_2_parse(unit, eh, &enh));

    if (enh.view != ENH2_VT_IFA) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    egr_ifa->ip_protocol = enh.ifa_ip_protocol;
    egr_ifa->max_length = enh.ifa_max_length;
    egr_ifa->hop_limit = enh.ifa_hop_limit;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete overlay next hop entry.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olnh_delete(int unit, int nhidx)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit, ol_egr_obj;
    bcmi_ltsw_l3_mtu_cfg_t mtu_cfg;
    olnh_entry_info_t *olnh_ei = &l3_egr_info[unit].olnh_ei[nhidx];
    bcmi_ltsw_ecmp_member_dest_info_t dest_info = {0};

    SHR_FUNC_ENTER(unit);

    if (L3_OL_NHOP_MTU_PTR(unit, nhidx)) {
        mtu_cfg.index = L3_OL_NHOP_MTU_PTR(unit, nhidx);
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_l3_mtu_delete(unit, &mtu_cfg));
        L3_OL_NHOP_MTU_PTR(unit, nhidx) = 0;
    }

    /* Update the ECMP Memeber destination. */
    if (olnh_ei->flags & BCMI_LTSW_L3_EGR_OBJ_INFO_F_CASCADED) {
        SHR_IF_ERR_EXIT
            (xfs_ltsw_l3_egress_obj_id_construct(unit, nhidx,
                                                 BCMI_LTSW_L3_EGR_OBJ_T_OL,
                                                 &ol_egr_obj));
        dest_info.type = BCMI_LTSW_ECMP_MEMBER_DEST_T_L3_CASCADED;
        dest_info.ol_egr_obj = ol_egr_obj;
        dest_info.ul_egr_obj = 0;
        dest_info.dvp = 0;
        SHR_IF_ERR_EXIT
            (xfs_ltsw_ecmp_member_dest_update(unit, &dest_info));

        olnh_ei->flags &= !BCMI_LTSW_L3_EGR_OBJ_INFO_F_CASCADED;
        olnh_ei->dvp = 0;
        olnh_ei->ul_egr_obj = 0;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_NEXT_HOP_1_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, NHOP_INDEX_1s, nhidx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_free(eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, EGR_L3_NEXT_HOP_1s, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, L3_NEXT_HOP_1s, nhidx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

    L3_OL_NHOP_FBMP_SET(unit, nhidx);

    L3_OL_NHOP_FLAGS(unit, nhidx) = 0;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete underlay next hop entry.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulnh_delete(int unit, int nhidx)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;
    bcm_l3_egress_t egr;

    SHR_FUNC_ENTER(unit);

    bcm_l3_egress_t_init(&egr);
    SHR_IF_ERR_EXIT
        (ulnh_ing_get(unit, nhidx, &egr));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_NEXT_HOP_2_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, NHOP_INDEX_2s, nhidx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_free(eh));

    /* Clear the nhop_2 from old system_destination. */
    SHR_IF_ERR_EXIT
        (ing_sys_dst_nhop2_set(unit, &egr, 0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, EGR_L3_NEXT_HOP_2s, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, L3_NEXT_HOP_2s, nhidx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

    L3_UL_NHOP_FBMP_SET(unit, nhidx);

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete underlay next hop entry for IFA.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulnh_ifa_delete(int unit, int nhidx)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, EGR_L3_NEXT_HOP_2s, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, L3_NEXT_HOP_2s, nhidx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

    L3_UL_NHOP_FBMP_SET(unit, nhidx);
    L3_UL_NHOP_IFA_BMP_CLR(unit, nhidx);

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate and fill ECMP member info array.
 *
 * \param [in] unit Unit number.
 * \param [in] count Number of elements to be allocated.
 * \param [in] ecmp_member ECMP members' information.
 * \param [out] member Allocated member info pointer.
 * \param [out] pri_cnt Number of primary paths.
 * \param [out] types Egress object types.
 * \param [out] prot_ul Protection switching on underlay nexthop.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_member_info_array_alloc(int unit, int count,
                             bcm_l3_ecmp_member_t *ecmp_member,
                             bcmi_ltsw_l3_ecmp_member_info_t **member,
                             int *pri_cnt, uint32_t *types, bool *prot_ul)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];
    int sz, ltidx, i, j, m, n, egr_types;
    bcm_if_t intfs[2];
    bool ol, ul, prot_ul_tmp;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bcmi_ltsw_l3_ecmp_member_info_t *emi = NULL;
    SHR_BITDCL *fbmp;

    SHR_FUNC_ENTER(unit);

    if (!count) {
        *member = NULL;
        SHR_EXIT();
    }

    sz = sizeof(bcmi_ltsw_l3_ecmp_member_info_t) * count;
    SHR_ALLOC(emi, sz, "bcmLtswXfsEcmpMemberInfo");
    SHR_NULL_CHECK(emi, SHR_E_MEMORY);
    sal_memset(emi, 0, sz);

    i = 0;
    j = count - 1;
    egr_types = 0;
    prot_ul_tmp = false;
    for (m = 0; m < count; m++) {
        intfs[0] = ecmp_member[m].egress_if;
        intfs[1] = ecmp_member[m].egress_if_2;
        ol = false;
        ul = false;

        if ((intfs[0] <= 0) && (intfs[1] <= 0)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        for (n = 0; n < 2; n++) {
            if (intfs[n] <= 0) {
                continue;
            }
            SHR_IF_ERR_EXIT
                (xfs_ltsw_l3_egress_obj_id_resolve(unit, intfs[n],
                                                   &ltidx, &type));
            if ((type == BCMI_LTSW_L3_EGR_OBJ_T_OL) && !ol) {
                fbmp = ei->olnh_fbmp;
                if (ecmp_member[m].flags & BCM_L3_ECMP_MEMBER_DGM_ALTERNATE) {
                    emi[j].nhop_1 = ltidx;
                }else {
                    emi[i].nhop_1 = ltidx;
                }
                ol = true;
            } else if ((type == BCMI_LTSW_L3_EGR_OBJ_T_UL) && !ul) {
                fbmp = ei->ulnh_fbmp;
                if (ecmp_member[m].flags & BCM_L3_ECMP_MEMBER_DGM_ALTERNATE) {
                    emi[j].nhop_2_or_ecmp_grp_1 = ltidx;
                } else {
                    emi[i].nhop_2_or_ecmp_grp_1 = ltidx;
                }
                ul = true;
            } else if ((type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) && !ul) {
                fbmp = ei->ulecmp_grp_fbmp;
                if (ecmp_member[m].flags & BCM_L3_ECMP_MEMBER_DGM_ALTERNATE) {
                    emi[j].nhop_2_or_ecmp_grp_1 = ltidx;
                    emi[j].ul_ecmp = true;
                } else {
                    emi[i].nhop_2_or_ecmp_grp_1 = ltidx;
                    emi[i].ul_ecmp = true;
                }
                ul = true;
            } else {
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
            }

            if (SHR_BITGET(fbmp, (ltidx & L3_NHOP_IDX_MASK(unit)))) {
                SHR_IF_ERR_MSG_EXIT
                    (SHR_E_PARAM,
                     (BSL_META_U(unit, "Member %d does not exist.\n"),
                      intfs[n]));
            }

            egr_types |= (1 << type);
        }
        if (!ol && !ul) {
            SHR_IF_ERR_MSG_EXIT
                (SHR_E_PARAM,
                 (BSL_META_U(unit, "No egress interface.(%d, %d)\n"),
                  intfs[0], intfs[1]));
        }

        /*
         * DLB member is only in underlay ECMP, which doesn't support
         * PROT_SWT_PROFILE_IDX.
         */
        emi[i].prot_swt_prfl_idx = BCMI_XFS_PROT_SWT_PROFILE_IDX_NHOP1;
        if (ecmp_member[m].flags & BCM_L3_ECMP_MEMBER_FAILOVER_UNDERLAY) {
            if (ul) {
                emi[i].prot_swt_prfl_idx = BCMI_XFS_PROT_SWT_PROFILE_IDX_NHOP2;
            }
            if (!prot_ul_tmp) {
                prot_ul_tmp = true;
            }
        }

        if (ecmp_member[m].flags & BCM_L3_ECMP_MEMBER_DGM_ALTERNATE) {
            j--;
        } else {
            i++;
        }
    }

    *member = emi;

    if (pri_cnt) {
        *pri_cnt = i;
    }

    if (types) {
        *types = egr_types;
    }

    if (prot_ul) {
        *prot_ul = prot_ul_tmp;
    }

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(emi);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set overlay ECMP.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] ecmp_info ECMP group info.
 * \param [in] ecmp_member_count Number of elements in ecmp_member_array.
 * \param [in] ecmp_member_array Member array of egress forwarding objects.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olecmp_set(int unit, int idx, bcm_l3_egress_ecmp_t *ecmp_info,
           int ecmp_member_count,
           bcm_l3_ecmp_member_t *ecmp_member_array)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];
    ecmp_level_cfg_t olecmp;
    bcm_if_t intfs[2] = {0};
    bool ol, ul;
    int dunit, sz, i, j, ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    const char *ltname;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t op;
    SHR_BITDCL *fbmp;
    int rv;
    uint32_t max_paths;

    SHR_FUNC_ENTER(unit);

    sal_memset(&olecmp, 0, sizeof(ecmp_level_cfg_t));
    olecmp.ecmp_id = idx;
    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_WEIGHTED) {
        olecmp.weighted = true;
        olecmp.num_paths = ecmp_member_count;
        max_paths = ecmp_info->max_paths;
    } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        olecmp.rh = true;
        olecmp.rh_mode = true;
        /* No API for random seed. Set to 0 temporarily. */
        olecmp.rh_random_seed = 0;
        olecmp.rh_num_paths = ecmp_member_count;
        max_paths = ecmp_info->dynamic_size;
    } else {
        max_paths = ecmp_info->max_paths;
        olecmp.num_paths = ecmp_member_count;
        olecmp.max_paths = max_paths;

        if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DISABLED) {
            olecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR;
        } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RANDOM) {
            olecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM;
        } else {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (olecmp.weighted || olecmp.rh) {
        ltname = ECMP_LEVEL0_WEIGHTEDs;
        olecmp.fld_bmp = EL_FLD_NUM_PATHS | EL_FLD_NHOP_1 | EL_FLD_DVP |
                         EL_FLD_NHOP_2_ECMP_GRP_1 | EL_FLD_ECMP_PROC_SWT |
                         EL_FLD_WEIGHTED_SIZE | EL_FLD_RH_MODE|
                         EL_FLD_RH_NUM_PATHS | EL_FLD_RH_RANDOM_SEED;

        switch (max_paths) {
        case 256:
            olecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_256;
            break;
        case 512:
            olecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_512;
            break;
        case 1024:
            olecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_1K;
            break;
        case 2048:
            olecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_2K;
            break;
        case 4096:
            olecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_4K;
            break;
        case 8192:
            olecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_8K;
            break;
        case 16384:
            olecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_16K;
            break;
        default:
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        ltname = ECMP_LEVEL0s;
        olecmp.fld_bmp = EL_FLD_NUM_PATHS | EL_FLD_NHOP_1 | EL_FLD_DVP |
                         EL_FLD_NHOP_2_ECMP_GRP_1 | EL_FLD_ECMP_PROC_SWT |
                         EL_FLD_LB_MODE | EL_FLD_MAX_PATHS;
    }

    /* ECMP member info. */
    olecmp.start_idx = 0;
    olecmp.num_of_set = ecmp_member_count;

    sz = sizeof(bcmi_ltsw_l3_ecmp_member_info_t) * ecmp_member_count;
    SHR_ALLOC(olecmp.member_info, sz, "bcmLtswXfsOlecmpMemberInfo");
    SHR_NULL_CHECK(olecmp.member_info, SHR_E_MEMORY);
    sal_memset(olecmp.member_info, 0, sz);

    for (i = 0; i < ecmp_member_count; i++) {
        intfs[0] = ecmp_member_array[i].egress_if;
        intfs[1] = ecmp_member_array[i].egress_if_2;
        ol = false;
        ul = false;

        if ((intfs[0] <= 0) && (intfs[1] <= 0)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        for (j = 0; j < 2; j++) {
            if (intfs[j] <= 0) {
                continue;
            }
            SHR_IF_ERR_EXIT
                (xfs_ltsw_l3_egress_obj_id_resolve(unit, intfs[j],
                                                   &ltidx, &type));
            if ((type == BCMI_LTSW_L3_EGR_OBJ_T_OL) && !ol) {
                olecmp.member_info[i].nhop_1 = ltidx;
                fbmp = ei->olnh_fbmp;
                ol = true;
            } else if ((type == BCMI_LTSW_L3_EGR_OBJ_T_UL) && !ul) {
                olecmp.member_info[i].nhop_2_or_ecmp_grp_1 = ltidx;
                fbmp = ei->ulnh_fbmp;
                ul = true;
            } else if ((type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) && !ul) {
                olecmp.member_info[i].nhop_2_or_ecmp_grp_1 = ltidx;
                olecmp.member_info[i].ul_ecmp = true;
                fbmp = ei->ulecmp_grp_fbmp;
                ul = true;
            } else {
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
            }

            if (SHR_BITGET(fbmp, (ltidx & L3_NHOP_IDX_MASK(unit)))) {
                SHR_IF_ERR_MSG_EXIT
                    (SHR_E_PARAM,
                     (BSL_META_U(unit, "Member %d does not exist.\n"),
                      intfs[j]));
            }
        }

        if (ul && (ecmp_member_array[i].flags &
                   BCM_L3_ECMP_MEMBER_FAILOVER_UNDERLAY)) {
            olecmp.member_info[i].prot_swt_prfl_idx =
                BCMI_XFS_PROT_SWT_PROFILE_IDX_NHOP2;
        } else {
            olecmp.member_info[i].prot_swt_prfl_idx =
                BCMI_XFS_PROT_SWT_PROFILE_IDX_NHOP1;
        }
    }

    if (!(ecmp_info->ecmp_group_flags & BCM_L3_ECMP_PATH_NO_SORTING) &&
        !olecmp.weighted && !olecmp.rh) {
        SHR_IF_ERR_EXIT
            (ecmp_member_sort(unit, olecmp.member_info, ecmp_member_count,
                              sizeof(bcmi_ltsw_l3_ecmp_member_info_t)));
    }

    if (ecmp_info->flags & BCM_L3_REPLACE) {
        op = BCMLT_OPCODE_UPDATE;
        /* Set the unused member elements to default value. */
        olecmp.num_of_unset = max_paths - ecmp_member_count;
    } else {
        op = BCMLT_OPCODE_INSERT;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_fill(unit, eh, &olecmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));

    if (ltsw_feature(unit, LTSW_FT_URPF)) {
        rv = bcmint_ltsw_l3_secmp_set(unit, idx, ecmp_info,
                                      olecmp.member_info, olecmp.num_paths);
        if (SHR_FAILURE(rv) && op == BCMLT_OPCODE_INSERT) {
            (void)bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                                       BCMLT_PRIORITY_NORMAL);
            SHR_IF_ERR_EXIT(rv);
        }
    }

    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_PATH_NO_SORTING) {
        L3_OL_ECMP_GRP_FLAGS(unit, idx) |= BCM_L3_ECMP_PATH_NO_SORTING;
    } else {
        L3_OL_ECMP_GRP_FLAGS(unit, idx) &= ~BCM_L3_ECMP_PATH_NO_SORTING;
    }

    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_WEIGHTED) {
        L3_OL_ECMP_GRP_FLAGS(unit, idx) |= BCM_L3_ECMP_WEIGHTED;
    } else {
        L3_OL_ECMP_GRP_FLAGS(unit, idx) &= ~BCM_L3_ECMP_WEIGHTED;
    }

    if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DISABLED) {
        L3_OL_ECMP_GRP_DYNA_MODE(unit, idx) = XFS_ECMP_DM_REGULAR;
    } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RANDOM) {
        L3_OL_ECMP_GRP_DYNA_MODE(unit, idx) = XFS_ECMP_DM_RANDOM;
    } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        L3_OL_ECMP_GRP_DYNA_MODE(unit, idx) = XFS_ECMP_DM_RESILIENT;
    } else {
        L3_OL_ECMP_GRP_DYNA_MODE(unit, idx) = XFS_ECMP_DM_REGULAR;
    }

exit:
    SHR_FREE(olecmp.member_info);
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete overlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index of overlay ECMP group.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olecmp_del(int unit, int idx)
{
    int dunit;
    const char *ltname;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if ((L3_OL_ECMP_GRP_FLAGS(unit, idx) & BCM_L3_ECMP_WEIGHTED) ||
        (L3_OL_ECMP_GRP_DYNA_MODE(unit, idx) == XFS_ECMP_DM_RESILIENT)) {
        ltname = ECMP_LEVEL0_WEIGHTEDs;
    } else {
        ltname = ECMP_LEVEL0s;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, ECMP_IDs, idx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

    if (ltsw_feature(unit, LTSW_FT_URPF)) {
        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_secmp_del(unit, idx));
    }

    L3_OL_ECMP_GRP_FBMP_SET(unit, idx);
    L3_OL_ECMP_GRP_FLAGS(unit, idx) = 0;
    L3_OL_ECMP_GRP_DYNA_MODE(unit, idx) = 0;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get overlay ECMP.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] ecmp_member_size Size of allocated entries in ecmp_member_array.
 * \param [out] ecmp_info ECMP group info.
 * \param [out] ecmp_member_count Number of members of ecmp_member_array.
 * \param [out] ecmp_member_array Member array of egress forwarding objects.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olecmp_get(int unit, int idx, bcm_l3_egress_ecmp_t *ecmp_info,
           int ecmp_member_size, bcm_l3_ecmp_member_t *ecmp_member_array,
           int *ecmp_member_count)
{
    ecmp_level_cfg_t olecmp;
    bcm_if_t intfs[2];
    int dunit, sz, i, j, ltidx, num_paths;
    bcmi_ltsw_l3_egr_obj_type_t type;
    const char *ltname;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    ecmp_info->ecmp_group_flags = L3_OL_ECMP_GRP_FLAGS(unit, idx) |
                                  BCM_L3_ECMP_OVERLAY;

    sal_memset(&olecmp, 0, sizeof(ecmp_level_cfg_t));
    olecmp.ecmp_id = idx;

    if (L3_OL_ECMP_GRP_FLAGS(unit, idx) & BCM_L3_ECMP_WEIGHTED) {
        ltname = ECMP_LEVEL0_WEIGHTEDs;
        olecmp.weighted = true;
    } else if(L3_OL_ECMP_GRP_DYNA_MODE(unit, idx) == XFS_ECMP_DM_RESILIENT)  {
        ltname = ECMP_LEVEL0_WEIGHTEDs;
        olecmp.rh = true;
    } else {
        ltname = ECMP_LEVEL0s;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_fill(unit, eh, &olecmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    if (olecmp.weighted) {
        olecmp.fld_bmp = EL_FLD_NUM_PATHS | EL_FLD_WEIGHTED_SIZE;
    } else if (olecmp.rh) {
        olecmp.fld_bmp = EL_FLD_RH_NUM_PATHS | EL_FLD_WEIGHTED_SIZE;
    } else {
        olecmp.fld_bmp = EL_FLD_NUM_PATHS | EL_FLD_LB_MODE | EL_FLD_MAX_PATHS;
    }

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_parse(unit, eh, &olecmp));

    if (olecmp.weighted) {
        ecmp_info->max_paths = 1 << (olecmp.weighted_size + 8);
        num_paths = olecmp.num_paths;
    } else if (olecmp.rh) {
        ecmp_info->dynamic_size = 1 << (olecmp.weighted_size + 8);
        ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
        num_paths = olecmp.rh_num_paths;
    } else {
        ecmp_info->max_paths = olecmp.max_paths;
        num_paths = olecmp.num_paths;
        if (olecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR) {
            ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DISABLED;
        } else if (olecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM) {
            ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
        } else {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    if (ecmp_member_size > 0) {
        *ecmp_member_count = ecmp_member_size < num_paths ?
                             ecmp_member_size : num_paths;

        olecmp.start_idx = 0;
        olecmp.num_of_set = *ecmp_member_count;
        sz = sizeof(bcmi_ltsw_l3_ecmp_member_info_t) * olecmp.num_of_set;
        SHR_ALLOC(olecmp.member_info, sz, "bcmLtswXfsOlecmpMemberInfo");
        SHR_NULL_CHECK(olecmp.member_info, SHR_E_MEMORY);
        sal_memset(olecmp.member_info, 0, sz);

        olecmp.fld_bmp = EL_FLD_NHOP_1 | EL_FLD_DVP |
                         EL_FLD_NHOP_2_ECMP_GRP_1 | EL_FLD_ECMP_PROC_SWT;

        SHR_IF_ERR_EXIT
            (lt_ecmp_level_parse(unit, eh, &olecmp));

        for (i = 0; i < *ecmp_member_count; i++) {
            j = 0;
            if (olecmp.member_info[i].nhop_1) {
                ltidx = olecmp.member_info[i].nhop_1;
                type = BCMI_LTSW_L3_EGR_OBJ_T_OL;

                SHR_IF_ERR_EXIT
                    (xfs_ltsw_l3_egress_obj_id_construct(unit, ltidx, type,
                                                         &intfs[j]));
                j++;
            }
            if (olecmp.member_info[i].nhop_2_or_ecmp_grp_1) {
                type = olecmp.member_info[i].ul_ecmp ?
                        BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL :
                        BCMI_LTSW_L3_EGR_OBJ_T_UL;
                ltidx = olecmp.member_info[i].nhop_2_or_ecmp_grp_1;
                SHR_IF_ERR_EXIT
                    (xfs_ltsw_l3_egress_obj_id_construct(unit, ltidx, type,
                                                         &intfs[j]));
                j++;
            }
            if (j == 0) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            ecmp_member_array[i].egress_if = intfs[0];
            ecmp_member_array[i].egress_if_2 = (j == 2) ? intfs[1] : 0;
            if (olecmp.member_info[i].prot_swt_prfl_idx ==
                    BCMI_XFS_PROT_SWT_PROFILE_IDX_NHOP2) {
                ecmp_member_array[i].flags |=
                    BCM_L3_ECMP_MEMBER_FAILOVER_UNDERLAY;
            }
        }
    } else {
        *ecmp_member_count = num_paths;
    }

exit:
    SHR_FREE(olecmp.member_info);
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a member to an overlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] ecmp_member Pointer to ECMP member structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olecmp_member_add(int unit, int idx, bcm_l3_ecmp_member_t *ecmp_member)
{
    ecmp_level_cfg_t olecmp;
    int dunit, rv;
    uint32_t sz, num_paths, max_paths;
    bcmi_ltsw_l3_ecmp_member_info_t *member_info = NULL, *arr = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t eh_new = BCMLT_INVALID_HDL;
    const char *ltname;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ecmp_member_info_array_alloc(unit, 1, ecmp_member, &member_info,
                                      NULL, NULL, NULL));

    sal_memset(&olecmp, 0, sizeof(ecmp_level_cfg_t));
    olecmp.ecmp_id = idx;

    if (L3_OL_ECMP_GRP_FLAGS(unit, idx) & BCM_L3_ECMP_WEIGHTED) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    } else if (L3_OL_ECMP_GRP_DYNA_MODE(unit, idx) == XFS_ECMP_DM_RESILIENT) {
        ltname = ECMP_LEVEL0_WEIGHTEDs;
        olecmp.rh = true;
    } else {
        ltname = ECMP_LEVEL0s;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_fill(unit, eh, &olecmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    olecmp.fld_bmp = olecmp.rh ? (EL_FLD_RH_NUM_PATHS | EL_FLD_WEIGHTED_SIZE) :
                                 (EL_FLD_NUM_PATHS | EL_FLD_MAX_PATHS);

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_parse(unit, eh, &olecmp));

    if (olecmp.rh) {
        num_paths = olecmp.rh_num_paths;
        max_paths = 1 << (olecmp.weighted_size + 8);
    } else {
        num_paths = olecmp.num_paths;
        max_paths = olecmp.max_paths;
    }

    /* Check if there is room in ECMP group. */
    if (num_paths == max_paths) {
        SHR_IF_ERR_EXIT(SHR_E_FULL);
    }

    if ((L3_OL_ECMP_GRP_FLAGS(unit, idx) & BCM_L3_ECMP_PATH_NO_SORTING) ||
        olecmp.rh) {
        /* Add the new member to the tail. */
        olecmp.start_idx = num_paths;
        num_paths++;
        olecmp.num_of_set = 1;
        olecmp.member_info = member_info;
    } else {
        /* Sort the members. */
        num_paths = olecmp.num_paths + 1;
        sz = sizeof(bcmi_ltsw_l3_ecmp_member_info_t) * num_paths;
        SHR_ALLOC(arr, sz, "bcmLtswXfsOlecmpMemberInfo");
        SHR_NULL_CHECK(arr, SHR_E_MEMORY);
        sal_memset(arr, 0, sz);

        olecmp.num_of_set = olecmp.num_paths;
        olecmp.member_info = arr;
        olecmp.fld_bmp = EL_FLD_NHOP_1 | EL_FLD_DVP |
                         EL_FLD_NHOP_2_ECMP_GRP_1 | EL_FLD_ECMP_PROC_SWT;
        SHR_IF_ERR_EXIT
            (lt_ecmp_level_parse(unit, eh, &olecmp));

        sal_memcpy(&arr[olecmp.num_paths], member_info,
                   sizeof(bcmi_ltsw_l3_ecmp_member_info_t));
        SHR_IF_ERR_EXIT
            (ecmp_member_sort(unit, arr, num_paths,
                              sizeof(bcmi_ltsw_l3_ecmp_member_info_t)));
        olecmp.start_idx = 0;
        olecmp.num_of_set = num_paths;
    }

    if (olecmp.rh) {
        olecmp.fld_bmp = EL_FLD_RH_NUM_PATHS | EL_FLD_NHOP_1 | EL_FLD_DVP |
                         EL_FLD_NHOP_2_ECMP_GRP_1 | EL_FLD_ECMP_PROC_SWT;
        olecmp.rh_num_paths = num_paths;
    } else {
        olecmp.fld_bmp = EL_FLD_NUM_PATHS | EL_FLD_NHOP_1 | EL_FLD_DVP |
                         EL_FLD_NHOP_2_ECMP_GRP_1 | EL_FLD_ECMP_PROC_SWT;
        olecmp.num_paths = num_paths;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh_new));

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_fill(unit, eh_new, &olecmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh_new, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

    if (ltsw_feature(unit, LTSW_FT_URPF)) {
        rv = bcmint_ltsw_l3_secmp_member_add(unit, idx, member_info);
        if (SHR_FAILURE(rv)) {
            /* Roll back the newly added member in ECMP_LEVEL0. */
            (void)bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                                       BCMLT_PRIORITY_NORMAL);
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FREE(member_info);
    SHR_FREE(arr);
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    if (eh_new != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh_new);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a member from an overlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] ecmp_member Pointer to ECMP member structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olecmp_member_del(int unit, int idx, bcm_l3_ecmp_member_t *ecmp_member)
{
    ecmp_level_cfg_t olecmp;
    int dunit, i, sz;
    uint32_t num_paths, max_paths;
    bcmi_ltsw_l3_ecmp_member_info_t *member_info = NULL;
    bcmi_ltsw_l3_ecmp_member_info_t *arr = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *ltname;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ecmp_member_info_array_alloc(unit, 1, ecmp_member, &member_info,
                                      NULL, NULL, NULL));

    sal_memset(&olecmp, 0, sizeof(ecmp_level_cfg_t));
    olecmp.ecmp_id = idx;

    if (L3_OL_ECMP_GRP_FLAGS(unit, idx) & BCM_L3_ECMP_WEIGHTED) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    } else if (L3_OL_ECMP_GRP_DYNA_MODE(unit, idx) == XFS_ECMP_DM_RESILIENT) {
        ltname = ECMP_LEVEL0_WEIGHTEDs;
        olecmp.rh = true;
        max_paths = L3_ECMP_RH_MAX_PATHS(unit);
    } else {
        ltname = ECMP_LEVEL0s;
        max_paths = L3_ECMP_MAX_PATHS(unit);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_fill(unit, eh, &olecmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    sz = sizeof(bcmi_ltsw_l3_ecmp_member_info_t) * max_paths;
    SHR_ALLOC(arr, sz, "bcmLtswXfsOlecmpMemberInfo");
    SHR_NULL_CHECK(arr, SHR_E_MEMORY);
    sal_memset(arr, 0, sz);
    olecmp.member_info = arr;

    olecmp.start_idx = 0;
    olecmp.num_of_set = max_paths;
    if (olecmp.rh) {
        olecmp.fld_bmp = EL_FLD_RH_NUM_PATHS | EL_FLD_NHOP_1 | EL_FLD_DVP |
                         EL_FLD_NHOP_2_ECMP_GRP_1 | EL_FLD_ECMP_PROC_SWT;

    } else {
        olecmp.fld_bmp = EL_FLD_NUM_PATHS | EL_FLD_NHOP_1 | EL_FLD_DVP |
                         EL_FLD_NHOP_2_ECMP_GRP_1 | EL_FLD_ECMP_PROC_SWT;
    }

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_parse(unit, eh, &olecmp));

    num_paths = olecmp.rh ? olecmp.rh_num_paths : olecmp.num_paths;

    for (i = 0; i < num_paths; i++) {
        if (!sal_memcmp(member_info, &arr[i], sizeof(bcmi_ltsw_l3_ecmp_member_info_t))) {
            break;
        }
    }

    /* Check if interface that to be deleted was found. */
    if (i == num_paths) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Shift remaining elements of member array. */
    olecmp.start_idx = i;
    olecmp.member_info = &arr[i+1];
    olecmp.num_of_set = num_paths - i - 1;
    olecmp.num_of_unset = 1;
    if (olecmp.rh) {
        olecmp.rh_num_paths --;
    } else {
        olecmp.num_paths--;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_clear(eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_fill(unit, eh, &olecmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

    if (ltsw_feature(unit, LTSW_FT_URPF)) {
        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_secmp_member_del(unit, idx, arr,
                                             num_paths, member_info));
    }

exit:
    if (member_info) {
        SHR_FREE(member_info);
    }
    if (arr) {
        SHR_FREE(arr);
    }
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all members from an overlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] ecmp_member Pointer to ECMP member structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olecmp_member_del_all(int unit, int idx)
{
    ecmp_level_cfg_t olecmp;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *ltname;
    uint32_t num_paths;

    SHR_FUNC_ENTER(unit);

    sal_memset(&olecmp, 0, sizeof(ecmp_level_cfg_t));
    olecmp.ecmp_id = idx;

    if (L3_OL_ECMP_GRP_FLAGS(unit, idx) & BCM_L3_ECMP_WEIGHTED) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    } else if (L3_OL_ECMP_GRP_DYNA_MODE(unit, idx) == XFS_ECMP_DM_RESILIENT) {
        ltname = ECMP_LEVEL0_WEIGHTEDs;
        olecmp.rh = true;
    } else {
        ltname = ECMP_LEVEL0s;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_fill(unit, eh, &olecmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    olecmp.fld_bmp = olecmp.rh ? EL_FLD_RH_NUM_PATHS : EL_FLD_NUM_PATHS;

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_parse(unit, eh, &olecmp));

    num_paths = olecmp.rh ? olecmp.rh_num_paths : olecmp.num_paths;
    if (!num_paths) {
        SHR_EXIT();
    }

    olecmp.start_idx = 0;
    olecmp.num_of_set = 0;
    olecmp.num_of_unset = num_paths;
    olecmp.num_paths = 0;
    olecmp.rh_num_paths = 0;
    olecmp.fld_bmp |= EL_FLD_NHOP_1 | EL_FLD_DVP | EL_FLD_NHOP_2_ECMP_GRP_1 |
                      EL_FLD_ECMP_PROC_SWT;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_clear(eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_fill(unit, eh, &olecmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

    if (ltsw_feature(unit, LTSW_FT_URPF)) {
        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_secmp_member_del_all(unit, idx));
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find an overlay Egress ECMP forwarding object by given member info.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_member_count Number of elements in ecmp_member_array.
 * \param [in] ecmp_member ECMP member array.
 * \param [out] ecmp_info ECMP group info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olecmp_find(int unit, int ecmp_member_count, bcmi_ltsw_l3_ecmp_member_info_t *ecmp_member,
            bcm_l3_egress_ecmp_t *ecmp_info)
{
    int dunit, ltidx, sz;
    bool wgt_i, rh_i;
    bcmi_ltsw_l3_ecmp_member_info_t *emi = NULL;
    const char *ltname;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    ecmp_level_cfg_t cfg;
    uint32_t num_paths;

    SHR_FUNC_ENTER(unit);

    sz = L3_ECMP_WEIGHTED_MAX_PATHS(unit)* sizeof(bcmi_ltsw_l3_ecmp_member_info_t);
    SHR_ALLOC(emi, sz, "bcmLtswXfsEcmpMemberInfo");
    SHR_NULL_CHECK(emi, SHR_E_MEMORY);

    dunit = bcmi_ltsw_dev_dunit(unit);

    for (ltidx = L3_OL_ECMP_GRP_MIN(unit); ltidx <= L3_OL_ECMP_GRP_MAX(unit);
         ltidx ++) {

        /* Skip unused entries. */
        if (L3_OL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            continue;
        }

        wgt_i = (L3_OL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_WEIGHTED) ?
                true : false;

        rh_i = (L3_OL_ECMP_GRP_DYNA_MODE(unit, ltidx) == XFS_ECMP_DM_RESILIENT) ?
               true : false;

        ltname = (wgt_i || rh_i) ? ECMP_LEVEL0_WEIGHTEDs : ECMP_LEVEL0s;

        sal_memset(&cfg, 0, sizeof(ecmp_level_cfg_t));
        cfg.weighted = wgt_i ? true : false;
        cfg.rh = rh_i ? true : false;

        if (eh != BCMLT_INVALID_HDL) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_free(eh));
            eh = BCMLT_INVALID_HDL;
        }

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, ltname, &eh));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, ECMP_IDs, ltidx));

        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));

        cfg.fld_bmp = rh_i ? EL_FLD_RH_NUM_PATHS : EL_FLD_NUM_PATHS;

        SHR_IF_ERR_EXIT
            (lt_ecmp_level_parse(unit, eh, &cfg));

        num_paths = rh_i ? cfg.rh_num_paths : cfg.num_paths;
        if (num_paths != ecmp_member_count) {
            continue;
        }

        cfg.fld_bmp = EL_FLD_NHOP_1 | EL_FLD_DVP |
                      EL_FLD_NHOP_2_ECMP_GRP_1 | EL_FLD_ECMP_PROC_SWT;
        cfg.member_info = emi;
        sal_memset(emi, 0, sz);
        cfg.num_of_set = num_paths;

        SHR_IF_ERR_EXIT
            (lt_ecmp_level_parse(unit, eh, &cfg));

        if ((L3_OL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_PATH_NO_SORTING) ||
            wgt_i || rh_i) {
            SHR_IF_ERR_EXIT
                (ecmp_member_sort(unit, emi, ecmp_member_count,
                                  sizeof(bcmi_ltsw_l3_ecmp_member_info_t)));
        }

        if (sal_memcmp(ecmp_member, emi,
                       sizeof(bcmi_ltsw_l3_ecmp_member_info_t) * num_paths)) {
            continue;
        }

        cfg.fld_bmp = (wgt_i || rh_i) ? EL_FLD_WEIGHTED_SIZE :
                                        (EL_FLD_LB_MODE | EL_FLD_MAX_PATHS);
        SHR_IF_ERR_EXIT
            (lt_ecmp_level_parse(unit, eh, &cfg));

        if (wgt_i) {
            ecmp_info->max_paths = 1 << (cfg.weighted_size + 8);
        } else if (rh_i) {
            ecmp_info->dynamic_size = 1 << (cfg.weighted_size + 8);
            ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
        } else {
            ecmp_info->max_paths = cfg.max_paths;
            if (cfg.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR) {
                ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DISABLED;
            } else if (cfg.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM) {
                ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
            } else {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
        ecmp_info->ecmp_group_flags = L3_OL_ECMP_GRP_FLAGS(unit, ltidx) |
                                      BCM_L3_ECMP_OVERLAY;
        SHR_IF_ERR_EXIT
            (l3_egress_ltidx_to_obj_id(unit, ltidx,
                                       BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL,
                                       &ecmp_info->ecmp_intf));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FREE(emi);
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set underlay ECMP table.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] ecmp_info ECMP group info.
 * \param [in] count Number of elements in member.
 * \param [in] member Member array of ECMP group.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulecmp_config_set(int unit, int idx, bcm_l3_egress_ecmp_t *ecmp_info,
                  int count, bcmi_ltsw_l3_ecmp_member_info_t* member)
{
    ecmp_level_cfg_t ulecmp;
    int dunit;
    const char *ltname;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t op;
    uint32_t max_paths;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ulecmp, 0, sizeof(ecmp_level_cfg_t));
    ulecmp.ecmp_id = idx;
    ulecmp.ul = true;
    ulecmp.member_info = member;

    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_WEIGHTED) {
        ulecmp.weighted = true;
        ulecmp.num_paths = count;
        max_paths = ecmp_info->max_paths;
    } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        ulecmp.rh = true;
        ulecmp.rh_mode = true;
        ulecmp.rh_random_seed = 0;
        ulecmp.rh_num_paths = count;
        max_paths = ecmp_info->dynamic_size;
    } else {
        ulecmp.num_paths = count;
        ulecmp.max_paths = ecmp_info->max_paths;
        max_paths = ecmp_info->max_paths;

        if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DISABLED) {
            ulecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR;
        } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RANDOM) {
            ulecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM;
        } else if ((ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_NORMAL)
            || (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED)
            || (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL)
            || (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM)) {
            ulecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR;
        } else {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (ulecmp.weighted || ulecmp.rh) {
        ltname = ECMP_LEVEL1_WEIGHTEDs;
        ulecmp.fld_bmp = EL_FLD_NUM_PATHS | EL_FLD_NHOP_1 |
                         EL_FLD_NHOP_2_ECMP_GRP_1 | EL_FLD_STRENGTH_PRFL_IDX |
                         EL_FLD_WEIGHTED_SIZE | EL_FLD_RH_MODE|
                         EL_FLD_RH_NUM_PATHS | EL_FLD_RH_RANDOM_SEED;

        switch (max_paths) {
        case 256:
            ulecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_256;
            break;
        case 512:
            ulecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_512;
            break;
        case 1024:
            ulecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_1K;
            break;
        case 2048:
            ulecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_2K;
            break;
        case 4096:
            ulecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_4K;
            break;
        case 8192:
            ulecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_8K;
            break;
        case 16384:
            ulecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_16K;
            break;
        default:
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        ltname = ECMP_LEVEL1s;
        ulecmp.fld_bmp = EL_FLD_NUM_PATHS | EL_FLD_NHOP_1 |
                         EL_FLD_NHOP_2_ECMP_GRP_1 | EL_FLD_STRENGTH_PRFL_IDX |
                         EL_FLD_LB_MODE | EL_FLD_MAX_PATHS;

    }

    /* ECMP member info. */
    ulecmp.start_idx = 0;
    ulecmp.num_of_set = count;

    if (ecmp_info->flags & BCM_L3_REPLACE) {
        op = BCMLT_OPCODE_UPDATE;
        /* Set the unused member elements to default value. */
        ulecmp.num_of_unset = max_paths - count;
    } else {
        op = BCMLT_OPCODE_INSERT;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_fill(unit, eh, &ulecmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete underlay ECMP.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulecmp_config_del(int unit, int idx)
{
    int dunit;
    const char *ltname;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if ((L3_UL_ECMP_GRP_FLAGS(unit, idx) & BCM_L3_ECMP_WEIGHTED) ||
        (L3_UL_ECMP_GRP_DYNA_MODE(unit, idx) == XFS_ECMP_DM_RESILIENT)) {
        ltname = ECMP_LEVEL1_WEIGHTEDs;
    } else {
        ltname = ECMP_LEVEL1s;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, ECMP_IDs, idx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a member to an underlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] member Pointer toECMP member info.
 * \param [in] alt Indicate an alternate path to be added.
 * \param [in] pri_cnt Number of primary paths.
 * \param [out] dlb_member Member info for DLB group.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulecmp_config_member_add(int unit, int idx, bcmi_ltsw_l3_ecmp_member_info_t *member,
                         bool alt, int pri_cnt, bcmi_ltsw_l3_ecmp_member_info_t *dlb_member)
{
    ecmp_level_cfg_t ulecmp;
    int dunit, alt_cnt, i;
    uint32_t sz, num_paths, max_paths;
    bcmi_ltsw_l3_ecmp_member_info_t *arr = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *ltname;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ulecmp, 0, sizeof(ecmp_level_cfg_t));
    ulecmp.ecmp_id = idx;
    ulecmp.ul = true;

    if (L3_UL_ECMP_GRP_FLAGS(unit, idx) & BCM_L3_ECMP_WEIGHTED) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    } else if (L3_UL_ECMP_GRP_DYNA_MODE(unit, idx) == XFS_ECMP_DM_RESILIENT) {
        ltname = ECMP_LEVEL1_WEIGHTEDs;
        ulecmp.rh = true;
    } else {
        ltname = ECMP_LEVEL1s;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_fill(unit, eh, &ulecmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    ulecmp.fld_bmp = ulecmp.rh ? (EL_FLD_RH_NUM_PATHS | EL_FLD_WEIGHTED_SIZE) :
                                 (EL_FLD_NUM_PATHS | EL_FLD_MAX_PATHS);

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_parse(unit, eh, &ulecmp));

    if (ulecmp.rh) {
        num_paths = ulecmp.rh_num_paths;
        max_paths = 1 << (ulecmp.weighted_size + 8);
    } else {
        num_paths = ulecmp.num_paths;
        max_paths = ulecmp.max_paths;
    }

    if (ltsw_feature(unit, LTSW_FT_DLB) &&
        L3_ECMP_DLB_GRP_CONFIGURED(unit, idx)) {
        if (num_paths < pri_cnt) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
    } else {
        pri_cnt = num_paths;
    }

    /* Check if there is room in ECMP group. */
    if (num_paths == max_paths) {
        SHR_IF_ERR_EXIT(SHR_E_FULL);
    }

    alt_cnt = num_paths - pri_cnt;

    sz = sizeof(bcmi_ltsw_l3_ecmp_member_info_t) * (num_paths + 1);
    SHR_ALLOC(arr, sz, "bcmLtswXfsUlecmpMemberInfo");
    SHR_NULL_CHECK(arr, SHR_E_MEMORY);
    sal_memset(arr, 0, sz);

    ulecmp.num_of_set = num_paths;
    ulecmp.member_info = arr;
    ulecmp.fld_bmp = EL_FLD_NHOP_1 | EL_FLD_NHOP_2_ECMP_GRP_1 |
                     EL_FLD_STRENGTH_PRFL_IDX;
    SHR_IF_ERR_EXIT
        (lt_ecmp_level_parse(unit, eh, &ulecmp));

    if (alt || !alt_cnt) {
        /* Add the new member to the tail*/
        sal_memcpy(&arr[num_paths], member,
                   sizeof(bcmi_ltsw_l3_ecmp_member_info_t));
    } else {
        /* Shift the alternate paths. */
        for (i = num_paths; i > pri_cnt; i--) {
            sal_memcpy(&arr[i], &arr[i - 1], sizeof(bcmi_ltsw_l3_ecmp_member_info_t));
        }
        /* Add the new member before alternate paths. */
        sal_memcpy(&arr[pri_cnt], member, sizeof(bcmi_ltsw_l3_ecmp_member_info_t));
    }
    /* Sort the members. */
    if (!(L3_UL_ECMP_GRP_FLAGS(unit, idx) & BCM_L3_ECMP_PATH_NO_SORTING) &&
        !ulecmp.rh) {
        if (alt) {
            SHR_IF_ERR_EXIT
                (ecmp_member_sort(unit, &arr[pri_cnt], alt_cnt + 1,
                                  sizeof(bcmi_ltsw_l3_ecmp_member_info_t)));
        } else {
            SHR_IF_ERR_EXIT
                (ecmp_member_sort(unit, arr, pri_cnt + 1,
                                  sizeof(bcmi_ltsw_l3_ecmp_member_info_t)));
        }
    }
    ulecmp.num_paths = num_paths + 1;
    ulecmp.start_idx = 0;
    ulecmp.num_of_set = num_paths + 1;

    if (ulecmp.rh) {
        ulecmp.fld_bmp = EL_FLD_RH_NUM_PATHS | EL_FLD_NHOP_1 |
                         EL_FLD_NHOP_2_ECMP_GRP_1 | EL_FLD_STRENGTH_PRFL_IDX;
    } else {
        ulecmp.fld_bmp = EL_FLD_NUM_PATHS | EL_FLD_NHOP_1 |
                         EL_FLD_NHOP_2_ECMP_GRP_1 | EL_FLD_STRENGTH_PRFL_IDX;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_clear(eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_fill(unit, eh, &ulecmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));
    if (dlb_member) {
        sal_memcpy(dlb_member, arr, (num_paths + 1) * sizeof(bcmi_ltsw_l3_ecmp_member_info_t));
    }

exit:
    SHR_FREE(arr);
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a member from an underlay ECMP table.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] member Pointer to ECMP member structure.
 * \param [in] alt Indicate a alternate path to be removed.
 * \param [in] pri_cnt Number of primary paths.
 * \param [out] member_idx Index of member to be removed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulecmp_config_member_del(int unit, int idx, bcmi_ltsw_l3_ecmp_member_info_t *member,
                         bool alt, int pri_cnt, int *member_idx)
{
    ecmp_level_cfg_t ulecmp;
    int dunit, i, sz, min, max;
    uint32_t num_paths, max_paths;
    bcmi_ltsw_l3_ecmp_member_info_t *arr = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *ltname;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ulecmp, 0, sizeof(ecmp_level_cfg_t));
    ulecmp.ecmp_id = idx;
    ulecmp.ul = true;

    if (L3_UL_ECMP_GRP_FLAGS(unit, idx) & BCM_L3_ECMP_WEIGHTED) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    } else if (L3_UL_ECMP_GRP_DYNA_MODE(unit, idx) == XFS_ECMP_DM_RESILIENT) {
        ltname = ECMP_LEVEL1_WEIGHTEDs;
        ulecmp.rh = true;
        max_paths = L3_ECMP_RH_MAX_PATHS(unit);
    } else {
        ltname = ECMP_LEVEL1s;
        max_paths = L3_ECMP_MAX_PATHS(unit);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_fill(unit, eh, &ulecmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    sz = sizeof(bcmi_ltsw_l3_ecmp_member_info_t) * max_paths;;
    SHR_ALLOC(arr, sz, "bcmLtswXfsUlecmpMemberInfo");
    SHR_NULL_CHECK(arr, SHR_E_MEMORY);
    sal_memset(arr, 0, sz);
    ulecmp.member_info = arr;

    ulecmp.start_idx = 0;
    ulecmp.num_of_set = max_paths;

    if (ulecmp.rh) {
        ulecmp.fld_bmp = EL_FLD_RH_NUM_PATHS | EL_FLD_NHOP_1 |
                         EL_FLD_NHOP_2_ECMP_GRP_1;
    } else {
        ulecmp.fld_bmp = EL_FLD_NUM_PATHS | EL_FLD_NHOP_1 |
                         EL_FLD_NHOP_2_ECMP_GRP_1;
    }

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_parse(unit, eh, &ulecmp));

    num_paths = ulecmp.rh ? ulecmp.rh_num_paths : ulecmp.num_paths;

    if (ltsw_feature(unit, LTSW_FT_DLB) &&
        L3_ECMP_DLB_GRP_CONFIGURED(unit, idx)) {
        if (num_paths < pri_cnt) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }

        min = alt ? pri_cnt : 0;
        max = alt ? num_paths : pri_cnt;
    } else {
        min = 0;
        max = num_paths;
    }
    for (i = min; i < max; i++) {
        if (!sal_memcmp(member, &arr[i], sizeof(bcmi_ltsw_l3_ecmp_member_info_t))) {
            break;
        }
    }

    /* Check if interface that to be deleted was found. */
    if (i >= max) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Shift remaining elements of member array. */
    ulecmp.start_idx = i;
    ulecmp.member_info = &arr[i + 1];
    ulecmp.num_of_set = num_paths - i - 1;
    ulecmp.num_of_unset = 1;
    ulecmp.fld_bmp |= EL_FLD_STRENGTH_PRFL_IDX;
    if (ulecmp.rh) {
        ulecmp.rh_num_paths--;
    } else {
        ulecmp.num_paths--;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_clear(eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_fill(unit, eh, &ulecmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

    if (member_idx) {
        *member_idx = i;
    }

exit:
    SHR_FREE(arr);
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all member from a underlay ECMP table.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulecmp_config_member_del_all(int unit, int idx)
{
    ecmp_level_cfg_t ulecmp;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *ltname;
    uint32_t num_paths;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ulecmp, 0, sizeof(ecmp_level_cfg_t));
    ulecmp.ecmp_id = idx;
    ulecmp.ul = true;

    if (L3_UL_ECMP_GRP_FLAGS(unit, idx) & BCM_L3_ECMP_WEIGHTED) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    } else if (L3_UL_ECMP_GRP_DYNA_MODE(unit, idx) == XFS_ECMP_DM_RESILIENT) {
        ltname = ECMP_LEVEL1_WEIGHTEDs;
        ulecmp.rh = true;
    } else {
        ltname = ECMP_LEVEL1s;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_fill(unit, eh, &ulecmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    ulecmp.fld_bmp = ulecmp.rh ? EL_FLD_RH_NUM_PATHS :  EL_FLD_NUM_PATHS;

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_parse(unit, eh, &ulecmp));

    num_paths = ulecmp.rh ? ulecmp.rh_num_paths : ulecmp.num_paths;
    if (!num_paths) {
        SHR_EXIT();
    }

    ulecmp.start_idx = 0;
    ulecmp.num_of_set = 0;
    ulecmp.num_of_unset = num_paths;
    ulecmp.num_paths = 0;
    ulecmp.rh_num_paths = 0;
    ulecmp.fld_bmp |= EL_FLD_NHOP_1 | EL_FLD_NHOP_2_ECMP_GRP_1 |
                      EL_FLD_STRENGTH_PRFL_IDX;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_clear(eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_fill(unit, eh, &ulecmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the DLB member port.
 *
 * \param [in] unit Unit number.
 * \param [in] count Number of elements in nhop_array.
 * \param [in/out] member ECMP member info array.
 * \param [in] ul DLB based on underlay egress object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dlb_member_port_get(int unit, int count, bcmi_ltsw_l3_ecmp_member_info_t *member, bool ul)
{
    int dunit, i, ltidx;
    uint64_t value;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_transaction_hdl_t trh = BCMLT_INVALID_HDL;
    bcmlt_entry_info_t ei;
    const char *lt = ul ? ING_L3_NEXT_HOP_2_TABLEs : ING_L3_NEXT_HOP_1_TABLEs;
    const char *key = ul ? NHOP_INDEX_2s : NHOP_INDEX_1s;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trh));

    dunit = bcmi_ltsw_dev_dunit(unit);

    for (i = 0; i < count; i++) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, lt, &eh));

        ltidx = (ul ? member[i].nhop_2_or_ecmp_grp_1 : member[i].nhop_1) &
                L3_NHOP_IDX_MASK(unit);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, key, ltidx));

        SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_add(trh, BCMLT_OPCODE_LOOKUP, eh));

        eh = BCMLT_INVALID_HDL;
    }

    SHR_IF_ERR_EXIT
        (bcmi_lt_transaction_commit(unit, trh, BCMLT_PRIORITY_NORMAL));

    for (i = 0; i < count; i++) {
        SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_num_get(trh, i, &ei));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(ei.entry_hdl,
                                   L2MC_L3MC_L2_OIF_SYS_DST_VALIDs,
                                   &value));
        if (value != DEST_VALID_L2OIF) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(ei.entry_hdl, DESTINATIONs, &value));
        member[i].port_id = value;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(ei.entry_hdl, key, &value));
        if (ul) {
            member[i].nhop_2_or_ecmp_grp_1 = value + L3_UL_OBJ_OFFSET;
        } else {
            member[i].nhop_1 = value;
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    if (trh != BCMLT_INVALID_HDL) {
        (void)bcmlt_transaction_free(trh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get DLB member assignment mode from ECMP dynamic mode.
 *
 * \param [in] unit Unit Number.
 * \param [in] dmode ECMP dynamic mode.
 * \param [out] amode DLB member assignment mode.
 *
 * \retval SHE_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_dmode_to_dlb_amode(int unit, uint32_t dmode, int *amode)
{
    SHR_FUNC_ENTER(unit);

    switch (dmode) {
        case BCM_L3_ECMP_DYNAMIC_MODE_NORMAL:
            *amode = BCMI_LTSW_DLB_ASSIGNMENT_MODE_TIME_ELIGIBILITY;
            break;
        case BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED:
            *amode = BCMI_LTSW_DLB_ASSIGNMENT_MODE_FIXED;
            break;
        case BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL:
            *amode = BCMI_LTSW_DLB_ASSIGNMENT_MODE_PACKET_SPRAY;
            break;
        default:
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get ECMP dynamic mode from DLB member assignment mode.
 *
 * \param [in] unit Unit Number.
 * \param [in] amode DLB member assignment mode.
 * \param [out] dmode ECMP dynamic mode.
 *
 * \retval SHE_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dlb_amode_to_ecmp_dmode(int unit, int amode, uint32_t *dmode)
{
    SHR_FUNC_ENTER(unit);

    switch (amode) {
        case BCMI_LTSW_DLB_ASSIGNMENT_MODE_TIME_ELIGIBILITY:
            *dmode = BCM_L3_ECMP_DYNAMIC_MODE_NORMAL;
            break;
        case BCMI_LTSW_DLB_ASSIGNMENT_MODE_FIXED:
            *dmode = BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED;
            break;
        case BCMI_LTSW_DLB_ASSIGNMENT_MODE_PACKET_SPRAY:
            *dmode = BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL;
            break;
        default:
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set an ECMP DLB group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx ECMP index.
 * \param [in] ecmp_info ECMP group info.
 * \param [in] pri_cnt Number of primary path in member array.
 * \param [in] alt_cnt Number of alternate path in member array.
 * \param [in] member Array of ecmp member info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_dlb_set(int unit, int idx, bcm_l3_egress_ecmp_t *ecmp_info,
             int pri_cnt, int alt_cnt, bcmi_ltsw_l3_ecmp_member_info_t *member)
{
    int dlb_id, i, sz;
    bcmi_ltsw_dlb_config_t cfg;
    bcmi_ltsw_dlb_member_t *dlb_member = NULL;
    bcmi_ltsw_dlb_member_t *alt_dlb_member = NULL;
    bool dlb_id_created = false;

    SHR_FUNC_ENTER(unit);

    sal_memset(&cfg, 0, sizeof(bcmi_ltsw_dlb_config_t));

    cfg.inactivity_time = ecmp_info->dynamic_age;
    cfg.flowset_size = ecmp_info->dynamic_size;

    if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) {
        cfg.flags = BCMI_LTSW_DLB_CF_DGM_ENABLED;
        cfg.alternate_config.alternate_path_cost = ecmp_info->dgm.cost;
        cfg.alternate_config.alternate_path_bias = ecmp_info->dgm.bias;
        cfg.alternate_config.primary_path_threshold = ecmp_info->dgm.threshold;
    } else {
        SHR_IF_ERR_EXIT
            (ecmp_dmode_to_dlb_amode(unit, ecmp_info->dynamic_mode,
                                     &cfg.assignment_mode));
    }

    if (pri_cnt) {

        sz = sizeof(bcmi_ltsw_dlb_member_t) * pri_cnt;
        SHR_ALLOC(dlb_member, sz, "bcmXfsL3EcmpDlbMember");
        SHR_NULL_CHECK(dlb_member, SHR_E_MEMORY);
        sal_memset(dlb_member, 0, sz);

        cfg.num_paths = pri_cnt;
        cfg.member = dlb_member;
        for (i = 0; i < pri_cnt; i++) {
            dlb_member[i].port_id = member[i].port_id;
        }
    }

    if (alt_cnt) {
        sz = sizeof(bcmi_ltsw_dlb_member_t) * alt_cnt;
        SHR_ALLOC(alt_dlb_member, sz, "bcmXfsL3EcmpDlbMember");
        SHR_NULL_CHECK(alt_dlb_member, SHR_E_MEMORY);
        sal_memset(alt_dlb_member, 0, sz);

        cfg.alternate_num_paths = alt_cnt;
        cfg.alternate_member = alt_dlb_member;
        for (i = 0; i < alt_cnt; i++) {
            alt_dlb_member[i].port_id = member[i + pri_cnt].port_id;
        }
    }

    dlb_id = idx - L3_ECMP_DLB_GRP_OFFSET(unit);

    if (!((ecmp_info->flags & BCM_L3_REPLACE) &&
          L3_ECMP_DLB_GRP_CONFIGURED(unit, idx))) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_dlb_id_create(unit, BCMI_LTSW_DLB_O_WITH_ID,
                                     bcmi_dlb_type_ecmp, &dlb_id));
            dlb_id_created = true;
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dlb_config_set(unit, bcmi_dlb_type_ecmp, dlb_id,
                                  &cfg));

exit:
    if (SHR_FUNC_ERR()) {
        if (dlb_id_created) {
            (void)bcmi_ltsw_dlb_id_destroy(unit, bcmi_dlb_type_ecmp, dlb_id);
        }
    }
    SHR_FREE(dlb_member);
    SHR_FREE(alt_dlb_member);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an ECMP DLB group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx ECMP index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_dlb_del(int unit, int idx)
{
    int dlb_id = idx - L3_ECMP_DLB_GRP_OFFSET(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dlb_config_set(unit, bcmi_dlb_type_ecmp, dlb_id, NULL));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dlb_id_destroy(unit, bcmi_dlb_type_ecmp, dlb_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a member from an ECMP DLB group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx ECMP index.
 * \param [in] member_idx Index of member to be removed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_dlb_member_del(int unit, int idx, int member_idx)
{
    int sz, dlb_id, i, num;
    bcmi_ltsw_dlb_capability_t cap;
    bcmi_ltsw_dlb_config_t cfg;
    bcmi_ltsw_dlb_member_t *ptr, *dlb_member = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&cfg, 0, sizeof(bcmi_ltsw_dlb_config_t));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dlb_capability_get(unit, bcmi_dlb_type_ecmp, &cap));

    sz = (cap.max_members_per_group + cap.max_alt_members_per_group) *
         sizeof(bcmi_ltsw_dlb_member_t);
    SHR_ALLOC(dlb_member, sz, "bcmLtswXfsEcmpDlbMember");
    SHR_NULL_CHECK(dlb_member, SHR_E_MEMORY);
    sal_memset(dlb_member, 0, sz);

    cfg.member = dlb_member;
    cfg.max_num_paths = cap.max_members_per_group;
    cfg.alternate_member = dlb_member + cap.max_members_per_group;
    cfg.max_alternate_num_paths = cap.max_alt_members_per_group;

    dlb_id = idx - L3_ECMP_DLB_GRP_OFFSET(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dlb_config_get(unit, bcmi_dlb_type_ecmp, dlb_id, &cfg));

    if (member_idx >= cfg.num_paths ) {
        if (!cfg.alternate_num_paths) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        i = member_idx - cfg.num_paths;
        num = cfg.alternate_num_paths;
        ptr = cfg.alternate_member;
        cfg.alternate_num_paths--;
    } else {
        if (!cfg.num_paths) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        i = member_idx;
        num = cfg.num_paths;
        ptr = cfg.member;
        cfg.num_paths--;
    }

    for (; i < (num - 1); i++) {
        ptr[i].port_id = ptr[i + 1].port_id;
    }
    ptr[num - 1].nhop_id = 0;
    ptr[num - 1].port_id = 0;

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dlb_config_set(unit, bcmi_dlb_type_ecmp, dlb_id,
                                  &cfg));

exit:
    SHR_FREE(dlb_member);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all members from an ECMP DLB group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx ECMP index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_dlb_member_del_all(int unit, int idx)
{
    int dlb_id;
    bcmi_ltsw_dlb_config_t cfg;

    SHR_FUNC_ENTER(unit);

    sal_memset(&cfg, 0, sizeof(bcmi_ltsw_dlb_config_t));

    dlb_id = idx - L3_ECMP_DLB_GRP_OFFSET(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dlb_config_get(unit, bcmi_dlb_type_ecmp, dlb_id, &cfg));

    if (!cfg.alternate_num_paths && !cfg.num_paths) {
        SHR_EXIT();
    }

    cfg.alternate_num_paths = 0;
    cfg.num_paths = 0;
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dlb_config_set(unit, bcmi_dlb_type_ecmp, dlb_id,
                                  &cfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the local port of a DLB member
 *
 * \param [in] unit Unit number.
 * \param [in] egr Egress object of DLB member.
 * \param [out] port Port number (Not gport).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_dlb_port_get(int unit, bcm_l3_egress_t *egr, int *port)
{
    int my_modid;

    SHR_FUNC_ENTER(unit);

    if (egr->flags & BCM_L3_TGID) {
        /* ECMP DLB members shouldn't be LAG */
        SHR_ERR_EXIT(SHR_E_PARAM);
    } else if (BCM_GPORT_IS_SET(egr->port)) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_port_gport_validate(unit, egr->port, port));
    } else {
        /* ECMP DLB members should be local */
        SHR_IF_ERR_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &my_modid));
        if (egr->module != my_modid) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        *port = egr->port;
    }

    if (*port == bcmi_ltsw_port_cpu(unit)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set an underlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] ecmp_info ECMP group info.
 * \param [in] ecmp_member_count Number of elements in ecmp_member_array.
 * \param [in] ecmp_member_array Member array of egress forwarding objects.
 * \param [in] dlb_en Indicate a DLB ECMP group.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulecmp_set(int unit, int idx, bcm_l3_egress_ecmp_t *ecmp_info,
           int ecmp_member_count,
           bcm_l3_ecmp_member_t *ecmp_member_array, bool dlb_en)
{
    int pri_cnt = 0, alt_cnt;
    bcmi_ltsw_l3_ecmp_member_info_t *member = NULL;
    uint32_t egr_types = 0, dlb_ft;
    bool wgt, dgm, dlb_pre, rh, ecmp_added = false;
    bcmi_ltsw_dlb_capability_t cap;
    bool dlb_ul_obj = false, prot_ul = false;

    SHR_FUNC_ENTER(unit);

    wgt = (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_WEIGHTED) ? true : false;
    dgm = (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) ?
          true : false;
    rh = (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) ?
         true : false;
    dlb_pre = ((ecmp_info->flags & BCM_L3_REPLACE) &&
               L3_ECMP_DLB_GRP_CONFIGURED(unit, idx)) ? true : false;

    SHR_IF_ERR_EXIT
        (ecmp_member_info_array_alloc(unit, ecmp_member_count,
                                      ecmp_member_array, &member,
                                      &pri_cnt, &egr_types, &prot_ul));

    /*
     * Validate the egress objects:
     *  1. No underlay ECMP object as a member.
     *  2. No alternate path if without DGM enabled.
     */
    if ((egr_types & (1 << BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL)) ||
        ((pri_cnt != ecmp_member_count) && !dgm)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (prot_ul) {
        SHR_ERR_MSG_EXIT(SHR_E_PARAM,
                         (BSL_META_U(unit,
                              "BCM_L3_FLAGS2_FAILOVER_UNDERLAY only "
                              "applies on Overlay ECMP member.\n")));
    }

    alt_cnt = ecmp_member_count - pri_cnt;

    if (dlb_en) {
        /* Validate the number of members. */
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_dlb_capability_get(unit, bcmi_dlb_type_ecmp, &cap));

        SHR_IF_ERR_EXIT
            (bcmi_ltsw_dlb_feature_get(unit, &dlb_ft));

        if (dgm && !(dlb_ft & BCMI_LTSW_DLB_F_ECMP_DGM)) {
            SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
        }

        if ((pri_cnt > cap.max_members_per_group) ||
            (alt_cnt > cap.max_alt_members_per_group)) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        if (egr_types & (1 << BCMI_LTSW_L3_EGR_OBJ_T_UL)) {
            /* Load balance on underlay egress objects of the DLB ECMP group. */
            dlb_ul_obj = true;
        }

        /* Get ports for each DLB ECMP member. */
        if (pri_cnt) {
            SHR_IF_ERR_EXIT
                (dlb_member_port_get(unit, pri_cnt, member, dlb_ul_obj));
        }
        if (alt_cnt) {
            SHR_IF_ERR_EXIT
                (dlb_member_port_get(unit, alt_cnt, &member[pri_cnt],
                                     dlb_ul_obj));
        }
    }

    if (!(ecmp_info->ecmp_group_flags & BCM_L3_ECMP_PATH_NO_SORTING) &&
        !wgt && !rh) {
        /* Sort primary paths. */
        if (pri_cnt) {
            SHR_IF_ERR_EXIT
                (ecmp_member_sort(unit, member, pri_cnt,
                                  sizeof(bcmi_ltsw_l3_ecmp_member_info_t)));
        }
        /* Sort alternate paths. */
        if (alt_cnt) {
            SHR_IF_ERR_EXIT
                (ecmp_member_sort(unit, &member[pri_cnt], alt_cnt,
                                  sizeof(bcmi_ltsw_l3_ecmp_member_info_t)));
        }
    }

    SHR_IF_ERR_EXIT
        (ulecmp_config_set(unit, idx, ecmp_info, ecmp_member_count, member));

    if (!(ecmp_info->flags & BCM_L3_REPLACE)) {
        ecmp_added = true;
    }

    if (dlb_en) {
        SHR_IF_ERR_EXIT
            (ecmp_dlb_set(unit, idx, ecmp_info, pri_cnt, alt_cnt, member));
    } else if (dlb_pre) {
        SHR_IF_ERR_EXIT
            (ecmp_dlb_del(unit, idx));
    }

    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_PATH_NO_SORTING) {
        L3_UL_ECMP_GRP_FLAGS(unit, idx) |= BCM_L3_ECMP_PATH_NO_SORTING;
    } else {
        L3_UL_ECMP_GRP_FLAGS(unit, idx) &= ~BCM_L3_ECMP_PATH_NO_SORTING;
    }

    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_WEIGHTED) {
        L3_UL_ECMP_GRP_FLAGS(unit, idx) |= BCM_L3_ECMP_WEIGHTED;
    } else {
        L3_UL_ECMP_GRP_FLAGS(unit, idx) &= ~BCM_L3_ECMP_WEIGHTED;
    }

    if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DISABLED) {
        L3_UL_ECMP_GRP_DYNA_MODE(unit, idx) = XFS_ECMP_DM_REGULAR;
    } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RANDOM) {
        L3_UL_ECMP_GRP_DYNA_MODE(unit, idx) = XFS_ECMP_DM_RANDOM;
    } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        L3_UL_ECMP_GRP_DYNA_MODE(unit, idx) = XFS_ECMP_DM_RESILIENT;
    } else if (dlb_en) {
        L3_UL_ECMP_GRP_DYNA_MODE(unit, idx) = dgm ? XFS_ECMP_DM_DGM :
                                                    XFS_ECMP_DM_DLB;
    } else {
        L3_UL_ECMP_GRP_DYNA_MODE(unit, idx) = XFS_ECMP_DM_REGULAR;
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (ecmp_added) {
            (void)ulecmp_config_del(unit, idx);
        }
    }
    SHR_FREE(member);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an underlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx ECMP index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulecmp_del(int unit, int idx)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_DLB) &&
        L3_ECMP_DLB_GRP_CONFIGURED(unit, idx)) {
        SHR_IF_ERR_EXIT
            (ecmp_dlb_del(unit, idx));
    }

    SHR_IF_ERR_EXIT
        (ulecmp_config_del(unit, idx));

    L3_UL_ECMP_GRP_FBMP_SET(unit, idx);
    L3_UL_ECMP_GRP_FLAGS(unit, idx) = 0;
    L3_UL_ECMP_GRP_DYNA_MODE(unit, idx) = 0;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get underlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] ecmp_member_size Size of allocated entries in ecmp_member_array.
 * \param [out] ecmp_info ECMP group info.
 * \param [out] ecmp_member_count Number of members of ecmp_member_array.
 * \param [out] ecmp_member_array Member array of egress forwarding objects.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulecmp_get(int unit, int idx, bcm_l3_egress_ecmp_t *ecmp_info,
           int ecmp_member_size, bcm_l3_ecmp_member_t *ecmp_member_array,
           int *ecmp_member_count)
{
    ecmp_level_cfg_t ulecmp;
    int dunit, sz, i, j, ltidx;
    bcm_if_t intfs[2];
    const char *ltname;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmi_ltsw_dlb_config_t dlb;
    bool dlb_en = false;
    uint32_t num_paths;

    SHR_FUNC_ENTER(unit);

    ecmp_info->ecmp_group_flags = L3_UL_ECMP_GRP_FLAGS(unit, idx) |
                                  BCM_L3_ECMP_UNDERLAY;

    /* Fetch group info from DLB_ECMP. */
    if (ltsw_feature(unit, LTSW_FT_DLB) &&
        L3_ECMP_DLB_GRP_CONFIGURED(unit, idx)) {
        dlb_en = true;

        sal_memset(&dlb, 0, sizeof(bcmi_ltsw_dlb_config_t));
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_dlb_config_get(unit, bcmi_dlb_type_ecmp,
                                      idx - L3_ECMP_DLB_GRP_OFFSET(unit),
                                      &dlb));
    }

    /* Fetch group and member info from ECMP_LEVEL1 */
    sal_memset(&ulecmp, 0, sizeof(ecmp_level_cfg_t));
    ulecmp.ecmp_id = idx;
    ulecmp.ul = true;

    if (L3_UL_ECMP_GRP_FLAGS(unit, idx) & BCM_L3_ECMP_WEIGHTED) {
        ltname = ECMP_LEVEL1_WEIGHTEDs;
        ulecmp.weighted = true;
    } else if(L3_UL_ECMP_GRP_DYNA_MODE(unit, idx) == XFS_ECMP_DM_RESILIENT)  {
        ltname = ECMP_LEVEL1_WEIGHTEDs;
        ulecmp.rh = true;
    } else {
        ltname = ECMP_LEVEL1s;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_fill(unit, eh, &ulecmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    if (ulecmp.weighted) {
        ulecmp.fld_bmp = EL_FLD_NUM_PATHS | EL_FLD_WEIGHTED_SIZE;
    } else if (ulecmp.rh) {
        ulecmp.fld_bmp = EL_FLD_RH_NUM_PATHS | EL_FLD_WEIGHTED_SIZE;
    } else {
        ulecmp.fld_bmp = EL_FLD_NUM_PATHS | EL_FLD_LB_MODE | EL_FLD_MAX_PATHS;
    }

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_parse(unit, eh, &ulecmp));

    num_paths = ulecmp.rh ? ulecmp.rh_num_paths : ulecmp.num_paths;

    if (dlb_en) {
        if (num_paths != (dlb.num_paths + dlb.alternate_num_paths)) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }

        ecmp_info->max_paths = ulecmp.max_paths;
        ecmp_info->dynamic_age = dlb.inactivity_time;
        ecmp_info->dynamic_size = dlb.flowset_size;

        if (dlb.flags & BCMI_LTSW_DLB_CF_DGM_ENABLED) {
            ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DGM;
            ecmp_info->dgm.cost = dlb.alternate_config.alternate_path_cost;
            ecmp_info->dgm.bias = dlb.alternate_config.alternate_path_bias;
            ecmp_info->dgm.threshold =
                               dlb.alternate_config.primary_path_threshold;
        } else {
            SHR_IF_ERR_EXIT
                (dlb_amode_to_ecmp_dmode(unit, dlb.assignment_mode,
                                         &ecmp_info->dynamic_mode));
        }
    } else if (ulecmp.weighted) {
        ecmp_info->max_paths = 1 << (ulecmp.weighted_size + 8);
    } else if (ulecmp.rh) {
        ecmp_info->dynamic_size = 1 << (ulecmp.weighted_size + 8);
        ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    } else {
        ecmp_info->max_paths = ulecmp.max_paths;
        if (ulecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR) {
            ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DISABLED;
        } else if (ulecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM) {
            ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
        } else {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    if (ecmp_member_size > 0) {
        *ecmp_member_count = ecmp_member_size < num_paths ?
                             ecmp_member_size : num_paths ;

        ulecmp.start_idx = 0;
        ulecmp.num_of_set = *ecmp_member_count;
        sz = sizeof(bcmi_ltsw_l3_ecmp_member_info_t) * ulecmp.num_of_set;
        SHR_ALLOC(ulecmp.member_info, sz, "bcmLtswXfsUlecmpMemberInfo");
        SHR_NULL_CHECK(ulecmp.member_info, SHR_E_MEMORY);
        sal_memset(ulecmp.member_info, 0, sz);

        ulecmp.fld_bmp = EL_FLD_NHOP_1 | EL_FLD_NHOP_2_ECMP_GRP_1;

        SHR_IF_ERR_EXIT
            (lt_ecmp_level_parse(unit, eh, &ulecmp));

        for (i = 0; i < *ecmp_member_count; i++) {
            j = 0;
            if (ulecmp.member_info[i].nhop_1) {
                ltidx = ulecmp.member_info[i].nhop_1;
                type = BCMI_LTSW_L3_EGR_OBJ_T_OL;
                SHR_IF_ERR_EXIT
                    (xfs_ltsw_l3_egress_obj_id_construct(unit, ltidx, type,
                                                         &intfs[j]));
                j++;
            }
            if (ulecmp.member_info[i].nhop_2_or_ecmp_grp_1) {
                type = BCMI_LTSW_L3_EGR_OBJ_T_UL;
                ltidx = ulecmp.member_info[i].nhop_2_or_ecmp_grp_1;
                SHR_IF_ERR_EXIT
                    (xfs_ltsw_l3_egress_obj_id_construct(unit, ltidx, type,
                                                         &intfs[j]));
                j++;
            }
            if (j == 0) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            ecmp_member_array[i].egress_if = intfs[0];
            ecmp_member_array[i].egress_if_2 = (j == 2) ? intfs[1] : 0;
            if (dlb_en && (i >= dlb.num_paths)) {
                ecmp_member_array[i].flags = BCM_L3_ECMP_MEMBER_DGM_ALTERNATE;
            }
        }
    } else {
        *ecmp_member_count = num_paths;
    }

exit:
    SHR_FREE(ulecmp.member_info);
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a member to an underlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] ecmp_member Pointer to ECMP member structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulecmp_member_add(int unit, int idx, bcm_l3_ecmp_member_t *ecmp_member)
{
    bool dlb, dgm, alt;
    int dlb_id, pri_cnt, rv, sz, i;
    uint32_t egr_types;
    bcmi_ltsw_l3_ecmp_member_info_t *member = NULL;
    bcmi_ltsw_l3_ecmp_member_info_t *dlb_emi = NULL;
    bcmi_ltsw_dlb_member_t *dlb_member = NULL;
    bcmi_ltsw_dlb_config_t cfg;
    bool dlb_ul_obj = false, prot_ul = false;

    SHR_FUNC_ENTER(unit);

    dlb = (ltsw_feature(unit, LTSW_FT_DLB) &&
           L3_ECMP_DLB_GRP_CONFIGURED(unit, idx)) ? true : false;
    dlb_id = idx - L3_ECMP_DLB_GRP_OFFSET(unit);

    sal_memset(&cfg, 0, sizeof(bcmi_ltsw_dlb_config_t));
    if (dlb) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_dlb_config_get(unit, bcmi_dlb_type_ecmp, dlb_id, &cfg));

        sz = (cfg.num_paths + cfg.alternate_num_paths + 1) *
              sizeof(bcmi_ltsw_l3_ecmp_member_info_t);
        SHR_ALLOC(dlb_emi, sz, "bcmLtswXfsDlbEcmpMemberInfo");
        SHR_NULL_CHECK(dlb_emi, SHR_E_MEMORY);
        sal_memset(dlb_emi, 0, sz);

        sz = (cfg.num_paths + cfg.alternate_num_paths + 1) *
              sizeof(bcmi_ltsw_dlb_member_t);
        SHR_ALLOC(dlb_member, sz, "bcmLtswXfsDlbMemberInfo");
        SHR_NULL_CHECK(dlb_member, SHR_E_MEMORY);
        sal_memset(dlb_member, 0, sz);
    }
    dgm = (cfg.flags & BCMI_LTSW_DLB_CF_DGM_ENABLED) ? true : false;

    SHR_IF_ERR_EXIT
        (ecmp_member_info_array_alloc(unit, 1, ecmp_member, &member,
                                      &pri_cnt, &egr_types, &prot_ul));

    /*
     * Validate the egress objects:
     *  1. No underlay ECMP object as a member.
     *  2. No alternate path if without DGM enabled.
     */
    if ((egr_types & (1 << BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL)) ||
        (!pri_cnt && !dgm)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (prot_ul) {
        SHR_ERR_MSG_EXIT(SHR_E_PARAM,
                         (BSL_META_U(unit,
                              "BCM_L3_FLAGS2_FAILOVER_UNDERLAY only "
                              "applies on Overlay ECMP member.\n")));
    }

    alt = pri_cnt ? false : true;

    SHR_IF_ERR_EXIT
        (ulecmp_config_member_add(unit, idx, member, alt, cfg.num_paths,
                                  dlb_emi));

    if (dlb) {
        if (alt) {
            cfg.alternate_num_paths++;
        } else {
            cfg.num_paths++;
        }
        if (egr_types & (1 << BCMI_LTSW_L3_EGR_OBJ_T_UL)) {
            /* Load balance on underlay egress objects of the DLB ECMP group. */
            dlb_ul_obj = true;
        }

        rv = dlb_member_port_get(unit,
                                 (cfg.alternate_num_paths + cfg.num_paths),
                                 dlb_emi, dlb_ul_obj);
        if (SHR_FAILURE(rv)) {
            /* Roll back the newly added member in ECMP_LEVEL1. */
            (void)ulecmp_config_member_del(unit, idx, member, alt, cfg.num_paths,
                                           NULL);
            SHR_IF_ERR_EXIT(rv);
        }

        for (i = 0; i < cfg.num_paths + cfg.alternate_num_paths; i++) {
            dlb_member[i].port_id = dlb_emi[i].port_id;
        }

        cfg.member = dlb_member;
        cfg.alternate_member = dlb_member + cfg.num_paths;

        rv = bcmi_ltsw_dlb_config_set(unit, bcmi_dlb_type_ecmp, dlb_id, &cfg);
        if (SHR_FAILURE(rv)) {
            /* Roll back the newly added member in ECMP_LEVEL1. */
            (void)ulecmp_config_member_del(unit, idx, member, alt, cfg.num_paths,
                                           NULL);
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FREE(member);
    SHR_FREE(dlb_emi);
    SHR_FREE(dlb_member);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a member from an underlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] ecmp_member Pointer to ECMP member structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulecmp_member_del(int unit, int idx, bcm_l3_ecmp_member_t *ecmp_member)
{
    bool dlb, dgm, alt, prot_ul;
    int dlb_id, pri_cnt, member_idx;
    uint32_t egr_types;
    bcmi_ltsw_l3_ecmp_member_info_t *member = NULL;
    bcmi_ltsw_dlb_config_t cfg;

    SHR_FUNC_ENTER(unit);

    dlb = (ltsw_feature(unit, LTSW_FT_DLB) &&
           L3_ECMP_DLB_GRP_CONFIGURED(unit, idx)) ? true : false;

    sal_memset(&cfg, 0, sizeof(bcmi_ltsw_dlb_config_t));
    if (dlb) {
        dlb_id = idx - L3_ECMP_DLB_GRP_OFFSET(unit);
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_dlb_config_get(unit, bcmi_dlb_type_ecmp, dlb_id, &cfg));
    }
    dgm = (cfg.flags & BCMI_LTSW_DLB_CF_DGM_ENABLED) ? true : false;

    SHR_IF_ERR_EXIT
        (ecmp_member_info_array_alloc(unit, 1, ecmp_member, &member,
                                      &pri_cnt, &egr_types, &prot_ul));

    /*
     * Validate the egress objects:
     *  1. No underlay ECMP object as a member.
     *  2. No alternate path if without DGM enabled.
     */
    if ((egr_types & (1 << BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL)) ||
        (!pri_cnt && !dgm)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (prot_ul) {
        SHR_ERR_MSG_EXIT(SHR_E_PARAM,
                         (BSL_META_U(unit,
                              "BCM_L3_FLAGS2_FAILOVER_UNDERLAY only "
                              "applies on Overlay ECMP member.\n")));
    }

    alt = pri_cnt ? false : true;

    SHR_IF_ERR_EXIT
        (ulecmp_config_member_del(unit, idx, member, alt, cfg.num_paths,
                                  &member_idx));

    if (dlb) {
        SHR_IF_ERR_EXIT
            (ecmp_dlb_member_del(unit, idx, member_idx));
    }

exit:
    SHR_FREE(member);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all member from a underlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulecmp_member_del_all(int unit, int idx)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_DLB) &&
        L3_ECMP_DLB_GRP_CONFIGURED(unit, idx)) {
        SHR_IF_ERR_EXIT
            (ecmp_dlb_member_del_all(unit, idx));
    }

    SHR_IF_ERR_EXIT
        (ulecmp_config_member_del_all(unit, idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find an underlay Egress ECMP forwarding object by given member info.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_member_count Number of elements in ecmp_member_array.
 * \param [in] ecmp_member ECMP member array.
 * \param [in] pri_cnt Number of primary path in member array.
 * \param [out] ecmp_info ECMP group info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulecmp_find(int unit, int ecmp_member_count, bcmi_ltsw_l3_ecmp_member_info_t *ecmp_member,
            int pri_cnt, bcm_l3_egress_ecmp_t *ecmp_info)
{
    int dunit, ltidx, sz, alt_cnt;
    bool wgt_i, rh_i, dlb_en;
    bcmi_ltsw_l3_ecmp_member_info_t *emi = NULL;
    const char *ltname;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    ecmp_level_cfg_t ulecmp;
    bcmi_ltsw_dlb_config_t dlb;
    uint32_t num_paths;

    SHR_FUNC_ENTER(unit);

    alt_cnt = ecmp_member_count - pri_cnt;

    sz = L3_ECMP_WEIGHTED_MAX_PATHS(unit) * sizeof(bcmi_ltsw_l3_ecmp_member_info_t);
    SHR_ALLOC(emi, sz, "bcmLtswXfsEcmpMemberInfo");
    SHR_NULL_CHECK(emi, SHR_E_MEMORY);

    dunit = bcmi_ltsw_dev_dunit(unit);

    for (ltidx = L3_UL_ECMP_GRP_MIN(unit); ltidx <= L3_UL_ECMP_GRP_MAX(unit);
         ltidx++) {

        /* Skip unused entries. */
        if (L3_UL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            continue;
        }

        if (L3_ECMP_DLB_GRP_CONFIGURED(unit, ltidx)) {
            wgt_i = false;
            dlb_en = true;
            rh_i = false;

            /* Fetch group info from DLB_ECMP. */
            sal_memset(&dlb, 0, sizeof(bcmi_ltsw_dlb_config_t));
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_dlb_config_get(unit, bcmi_dlb_type_ecmp,
                                          ltidx - L3_ECMP_DLB_GRP_OFFSET(unit),
                                          &dlb));
            if ((dlb.alternate_num_paths != alt_cnt) ||
                (dlb.num_paths != pri_cnt)) {
                continue;
            }
        } else {
            wgt_i = (L3_UL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_WEIGHTED) ?
                    true : false;
            rh_i = (L3_OL_ECMP_GRP_DYNA_MODE(unit, ltidx) == XFS_ECMP_DM_RESILIENT) ?
                   true : false;
            dlb_en = false;
        }

        /* Fetch group and member info from ECMP_LEVEL1 */
        ltname = (wgt_i || rh_i) ? ECMP_LEVEL1_WEIGHTEDs : ECMP_LEVEL1s;

        sal_memset(&ulecmp, 0, sizeof(ecmp_level_cfg_t));
        ulecmp.ul = true;
        ulecmp.weighted = wgt_i ? true : false;
        ulecmp.rh = rh_i ? true: false;

        if (eh != BCMLT_INVALID_HDL) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_free(eh));
            eh = BCMLT_INVALID_HDL;
        }

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, ltname, &eh));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, ECMP_IDs, ltidx));

        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));

        ulecmp.fld_bmp = rh_i ? EL_FLD_RH_NUM_PATHS : EL_FLD_NUM_PATHS;

        SHR_IF_ERR_EXIT
            (lt_ecmp_level_parse(unit, eh, &ulecmp));

        num_paths = rh_i ? ulecmp.rh_num_paths : ulecmp.num_paths;
        if (num_paths != ecmp_member_count) {
            continue;
        }

        ulecmp.fld_bmp = EL_FLD_NHOP_1 | EL_FLD_NHOP_2_ECMP_GRP_1;
        ulecmp.member_info = emi;
        sal_memset(emi, 0, sz);
        ulecmp.num_of_set = num_paths;

        SHR_IF_ERR_EXIT
            (lt_ecmp_level_parse(unit, eh, &ulecmp));

        if ((L3_UL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_PATH_NO_SORTING) ||
            wgt_i || rh_i) {
            if (pri_cnt) {
                SHR_IF_ERR_EXIT
                    (ecmp_member_sort(unit, emi, pri_cnt,
                                      sizeof(bcmi_ltsw_l3_ecmp_member_info_t)));
            }
            if (alt_cnt) {
                SHR_IF_ERR_EXIT
                    (ecmp_member_sort(unit, emi + pri_cnt, alt_cnt,
                                      sizeof(bcmi_ltsw_l3_ecmp_member_info_t)));
            }
        }

        if (sal_memcmp(ecmp_member, emi,
                       (sizeof(bcmi_ltsw_l3_ecmp_member_info_t) * num_paths))) {
            continue;
        }

        ulecmp.fld_bmp = (wgt_i || rh_i) ? EL_FLD_WEIGHTED_SIZE :
                                           (EL_FLD_LB_MODE | EL_FLD_MAX_PATHS);
        SHR_IF_ERR_EXIT
            (lt_ecmp_level_parse(unit, eh, &ulecmp));

        if (dlb_en) {
            ecmp_info->max_paths = ulecmp.max_paths;
            ecmp_info->dynamic_age = dlb.inactivity_time;
            ecmp_info->dynamic_size = dlb.flowset_size;

            if (dlb.flags & BCMI_LTSW_DLB_CF_DGM_ENABLED) {
                ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DGM;
                ecmp_info->dgm.cost = dlb.alternate_config.alternate_path_cost;
                ecmp_info->dgm.bias = dlb.alternate_config.alternate_path_bias;
                ecmp_info->dgm.threshold =
                                   dlb.alternate_config.primary_path_threshold;
            } else {
                SHR_IF_ERR_EXIT
                    (dlb_amode_to_ecmp_dmode(unit, dlb.assignment_mode,
                                             &ecmp_info->dynamic_mode));
            }
        } else if (wgt_i) {
            ecmp_info->max_paths = 1 << (ulecmp.weighted_size + 8);
        } else if (rh_i) {
            ecmp_info->dynamic_size = 1 << (ulecmp.weighted_size + 8);
            ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
        } else {
            ecmp_info->max_paths = ulecmp.max_paths;
            if (ulecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR) {
                ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DISABLED;
            } else if (ulecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM) {
                ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
            } else {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
        ecmp_info->ecmp_group_flags = L3_UL_ECMP_GRP_FLAGS(unit, ltidx) |
                                      BCM_L3_ECMP_UNDERLAY;
        SHR_IF_ERR_EXIT
            (l3_egress_ltidx_to_obj_id(unit, ltidx,
                                       BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL,
                                       &ecmp_info->ecmp_intf));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FREE(emi);
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize DLB ECMP.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_ecmp_dlb_init(int unit)
{
    bcmi_ltsw_dlb_capability_t cap;
    l3_egr_info_t *ei = &l3_egr_info[unit];

    SHR_FUNC_ENTER(unit);

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_dlb_type_init(unit, bcmi_dlb_type_ecmp));
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dlb_capability_get(unit, bcmi_dlb_type_ecmp, &cap));

    ei->ecmp_dlb_grp_offset = ei->ulecmp_grp_max - cap.id_max;
    ei->ecmp_dlb_grp_min = ei->ecmp_dlb_grp_offset + cap.id_min;

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_dlb_control_set(unit,
                                       bcmi_dlb_type_ecmp,
                                       bcmiDlbControlTypeBaseEcmpId,
                                       ei->ecmp_dlb_grp_offset));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize DLB ECMP.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_ecmp_dlb_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_dlb_type_deinit(unit, bcmi_dlb_type_ecmp));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover ECMP member destination database from LTs.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_member_dest_recover(int unit)
{
    ecmp_member_dest_info_t *di = &l3_egr_info[unit].ecmp_member_dest_info;
    bcmint_l3_ecmp_member_ent_t *ent = di->ent_info;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit, i, total_ent;
    int rv = SHR_E_NONE;
    uint64_t grp, base_idx;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ECMP_MEMBER0s, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, ECMP_MEMBER0_IDs, &grp));

        /*Skip the reserved group. */
        if (grp < ECMP_MEMBER_DEST_GRP_START) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, BASE_INDEX_OPERs, &base_idx));

        di->group_info[grp - ECMP_MEMBER_DEST_GRP_START].base_index = base_idx;
    }

    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_EXIT(rv);
    }

    total_ent = di->num_grps * di->num_ents;

    for (i = 0; i < total_ent; i++) {
        if (ent[i].ref_cnt) {
            di->group_info[i / di->num_ents].used_ent_cnt++;
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Init default ecmp member dest table entry.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_member_dest_table_init(int unit)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL, eh1 = BCMLT_INVALID_HDL;
    int dunit;
    uint64_t data_zero = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Reserve one ECMP_MEMBER0 entry and init to zero. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ECMP_MEMBER0s, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, ECMP_MEMBER0_IDs, data_zero));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(eh, NHOP_INDEX_1s, 0, &data_zero, 1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(eh, NHOP_2_OR_ECMP_GROUP_INDEX_1s, 0,
                                     &data_zero, 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(eh, DVPs, 0, &data_zero, 1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(eh, ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXs,
                                     0, &data_zero, 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, NUM_ENTRIESs, 1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, BASE_INDEX_AUTOs, data_zero));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, BASE_INDEXs, data_zero));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

    /* Reserve one ECMP_MEMBER1 entry and init to zero. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ECMP_MEMBER1s, &eh1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh1, ECMP_MEMBER1_IDs, data_zero));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(eh1, NHOP_INDEX_1s, 0, &data_zero, 1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(eh1, NHOP_INDEX_2s, 0, &data_zero, 1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(eh1, STRENGTH_PROFILE_INDEXs,
                                     0, &data_zero, 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh1, NUM_ENTRIESs, 1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh1, BASE_INDEX_AUTOs, data_zero));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh1, BASE_INDEXs, data_zero));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh1, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    if (eh1 != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh1);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize ECMP member destination database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_member_dest_init(int unit)
{
    ecmp_member_dest_info_t *di = &l3_egr_info[unit].ecmp_member_dest_info;
    uint64_t max;
    uint32_t sz, alloc_sz;
    int rv;

    SHR_FUNC_ENTER(unit);

    di->mutex = sal_mutex_create("bcmXfsL3EcmpMemberDestInfo");
    SHR_NULL_CHECK(di->mutex, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, ECMP_MEMBER0s,
                                       ECMP_MEMBER0_IDs, NULL, &max));
    di->num_grps = max + 1 - ECMP_MEMBER_DEST_GRP_START;

    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, ECMP_MEMBER0s,
                                       NUM_ENTRIESs, NULL, &max));
    di->num_ents = max;

    /* ECMP member destination groups. */
    sz = sizeof(ecmp_member_dest_grp_t) * di->num_grps;
    di->group_info = sal_alloc(sz, "bcmXfsEcmpMemberDestGrpInfo");
    SHR_NULL_CHECK(di->group_info, SHR_E_MEMORY);
    sal_memset(di->group_info, 0, sz);

    /* ECMP member destination entries. */
    sz = sizeof(bcmint_l3_ecmp_member_ent_t) * di->num_grps * di->num_ents;
    alloc_sz = sz;
    di->ent_info = bcmi_ltsw_ha_mem_alloc(unit,
                                        BCMI_HA_COMP_ID_L3_EGR,
                                        BCMINT_LTSW_L3_EGR_HA_ECMP_MEMBER_DEST,
                                        "bcmL3EcmpMembrDest",
                                        &alloc_sz);
    SHR_NULL_CHECK(di->ent_info, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);
    rv = bcmi_ltsw_issu_struct_info_report(unit,
                                        BCMI_HA_COMP_ID_L3_EGR,
                                        BCMINT_LTSW_L3_EGR_HA_ECMP_MEMBER_DEST,
                                        0, sz, 1,
                                        BCMINT_L3_ECMP_MEMBER_ENT_T_ID);
    if (rv != SHR_E_EXISTS) {
        SHR_IF_ERR_EXIT(rv);
    }

    if (!bcmi_warmboot_get(unit)) {
        sal_memset(di->ent_info, 0, alloc_sz);
        SHR_IF_ERR_EXIT
            (ecmp_member_dest_table_init(unit));
    } else {
        SHR_IF_ERR_EXIT
            (ecmp_member_dest_recover(unit));
    }

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(di->group_info);
        if (di->mutex) {
            sal_mutex_destroy(di->mutex);
        }
        if (!bcmi_warmboot_get(unit)) {
            (void)bcmi_ltsw_ha_mem_free(unit, di->ent_info);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize ECMP member destination database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static void
ecmp_member_dest_deinit(int unit)
{
    ecmp_member_dest_info_t *di = &l3_egr_info[unit].ecmp_member_dest_info;

    SHR_FREE(di->group_info);

    sal_mutex_destroy(di->mutex);
}

/*!
 * \brief Get ecmp member info from destination info.
 *
 * \param [in] unit Unit number.
 * \param [in] dest_info ECMP member destination info.
 * \param [out] member_info ECMP member info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_member_dest_info_resolve(int unit,
                              bcmi_ltsw_ecmp_member_dest_info_t *dest_info,
                              bcmi_ltsw_l3_ecmp_member_info_t *member_info)
{
    bcmi_ltsw_l3_egr_obj_type_t type;
    int nh_ecmp_idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(member_info, 0, sizeof(bcmi_ltsw_l3_ecmp_member_info_t));

    if (dest_info->ul_egr_obj) {
        SHR_IF_ERR_EXIT
            (xfs_ltsw_l3_egress_obj_id_resolve(unit, dest_info->ul_egr_obj,
                                               &nh_ecmp_idx, &type));
        if (type == BCMI_LTSW_L3_EGR_OBJ_T_UL) {
            member_info->nhop_2_or_ecmp_grp_1 = nh_ecmp_idx;
        } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
            member_info->nhop_2_or_ecmp_grp_1 = nh_ecmp_idx;
            member_info->ul_ecmp = true;
        } else {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if ((dest_info->type == BCMI_LTSW_ECMP_MEMBER_DEST_T_ALPM_NO_CASCADED) ||
        (dest_info->type == BCMI_LTSW_ECMP_MEMBER_DEST_T_L3_CASCADED)) {
        SHR_IF_ERR_EXIT
            (xfs_ltsw_l3_egress_obj_id_resolve(unit, dest_info->ol_egr_obj,
                                               &nh_ecmp_idx, &type));
        if (type != BCMI_LTSW_L3_EGR_OBJ_T_OL) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        member_info->nhop_1 = nh_ecmp_idx;
    }

    if (dest_info->type == BCMI_LTSW_ECMP_MEMBER_DEST_T_L2_CASCADED) {
        member_info->dvp = dest_info->dvp;
    }

    member_info->prot_swt_prfl_idx = dest_info->prot_swt_prfl_idx;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a path to ECMP_MEMBER_DESTINATION table.
 *
 * \param [in] unit Unit number.
 * \param [in] ent_idx Logical entry index.
 * \param [in] dest_info ECMP member destination info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_member_dest_path_add(int unit, int ent_idx,
                          bcmi_ltsw_ecmp_member_dest_info_t *dest_info)
{
    ecmp_member_dest_info_t *di = &l3_egr_info[unit].ecmp_member_dest_info;
    ecmp_member_dest_grp_t *grp = di->group_info;
    bcmint_l3_ecmp_member_ent_t *ent = di->ent_info;
    bcmi_ltsw_l3_ecmp_member_info_t member_info;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t op;
    int dunit, grp_idx, arr_idx;
    uint64_t val;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ecmp_member_dest_info_resolve(unit, dest_info, &member_info));

    grp_idx = ent_idx / di->num_ents;
    arr_idx = ent_idx % di->num_ents;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ECMP_MEMBER0s, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, ECMP_MEMBER0_IDs,
                               grp_idx + ECMP_MEMBER_DEST_GRP_START));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, NUM_ENTRIESs, di->num_ents));

    val = member_info.nhop_1;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(eh, NHOP_INDEX_1s, arr_idx, &val, 1));

    val = member_info.nhop_2_or_ecmp_grp_1;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(eh, NHOP_2_OR_ECMP_GROUP_INDEX_1s, arr_idx,
                                     &val, 1));

    val = member_info.dvp;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(eh, DVPs, arr_idx, &val, 1));

    val = (member_info.ul_ecmp << 2) | member_info.prot_swt_prfl_idx;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(eh, ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXs,
                                     arr_idx, &val, 1));

    if (!grp[grp_idx].used_ent_cnt) {
        op = BCMLT_OPCODE_INSERT;
    } else {
        op = BCMLT_OPCODE_UPDATE;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, BASE_INDEX_AUTOs, 1));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));

    if (op == BCMLT_OPCODE_INSERT) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_clear(eh));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, ECMP_MEMBER0_IDs,
                                   grp_idx + ECMP_MEMBER_DEST_GRP_START));

        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, BASE_INDEX_OPERs, &val));

        grp[grp_idx].base_index = val;
    }

    grp[grp_idx].used_ent_cnt++;

    ent[ent_idx].type = dest_info->type;
    ent[ent_idx].ol_egr_obj = dest_info->ol_egr_obj;
    ent[ent_idx].ul_egr_obj = dest_info->ul_egr_obj;
    ent[ent_idx].dvp = dest_info->dvp;
    ent[ent_idx].prot_swt_prfl_idx = dest_info->prot_swt_prfl_idx;
    ent[ent_idx].ref_cnt = 1;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a path from ECMP_MEMBER_DESTINATION table.
 *
 * \param [in] unit Unit number.
 * \param [in] ent_idx Logical entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_member_dest_path_del(int unit, int ent_idx)
{
    ecmp_member_dest_info_t *di = &l3_egr_info[unit].ecmp_member_dest_info;
    ecmp_member_dest_grp_t *grp = di->group_info;
    bcmint_l3_ecmp_member_ent_t *ent = di->ent_info;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t op;
    int dunit, grp_idx, arr_idx;
    uint64_t val = 0;

    SHR_FUNC_ENTER(unit);

    grp_idx = ent_idx / di->num_ents;
    arr_idx = ent_idx % di->num_ents;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ECMP_MEMBER0s, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, ECMP_MEMBER0_IDs,
                               grp_idx + ECMP_MEMBER_DEST_GRP_START));

    if (grp[grp_idx].used_ent_cnt == 1) {
        op = BCMLT_OPCODE_DELETE;
    } else {
        op = BCMLT_OPCODE_UPDATE;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, NUM_ENTRIESs, di->num_ents));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(eh, NHOP_INDEX_1s, arr_idx, &val, 1));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(eh, NHOP_2_OR_ECMP_GROUP_INDEX_1s,
                                         arr_idx, &val, 1));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(eh, DVPs, arr_idx, &val, 1));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(eh,
                                         ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXs,
                                         arr_idx, &val, 1));
    }

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));

    sal_memset(&ent[ent_idx], 0, sizeof(bcmint_l3_ecmp_member_ent_t));

    grp[grp_idx].used_ent_cnt--;

    if (op == BCMLT_OPCODE_DELETE) {
        grp[grp_idx].base_index = 0;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update a path in ECMP_MEMBER_DESTINATION table.
 *
 * \param [in] unit Unit number.
 * \param [in] ent_idx Logical entry index.
 * \param [in] dest_info ECMP member destination info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_member_dest_path_update(int unit, int ent_idx,
                             bcmi_ltsw_ecmp_member_dest_info_t *dest_info)
{
    ecmp_member_dest_info_t *di = &l3_egr_info[unit].ecmp_member_dest_info;
    bcmint_l3_ecmp_member_ent_t *ent = di->ent_info;
    bcmi_ltsw_l3_ecmp_member_info_t member_info;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit, grp_idx, arr_idx;
    uint64_t val = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ecmp_member_dest_info_resolve(unit, dest_info, &member_info));

    grp_idx = ent_idx / di->num_ents;
    arr_idx = ent_idx % di->num_ents;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ECMP_MEMBER0s, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, ECMP_MEMBER0_IDs,
                               grp_idx + ECMP_MEMBER_DEST_GRP_START));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, NUM_ENTRIESs, di->num_ents));

    val = member_info.nhop_2_or_ecmp_grp_1;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(eh, NHOP_2_OR_ECMP_GROUP_INDEX_1s,
                                     arr_idx, &val, 1));

    val = (member_info.ul_ecmp << 2) | member_info.prot_swt_prfl_idx;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(eh,
                                     ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXs,
                                     arr_idx, &val, 1));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

    ent[ent_idx].ul_egr_obj = dest_info->ul_egr_obj;
    ent[ent_idx].prot_swt_prfl_idx = dest_info->prot_swt_prfl_idx;
    if (dest_info->type == BCMI_LTSW_ECMP_MEMBER_DEST_T_L3_CASCADED) {
        ent[ent_idx].dvp = dest_info->dvp;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a destination index in ECMP_MEMBER_DESTINATION table.
 *
 * \param [in] unit Unit number.
 * \param [in] dest_info ECMP member destination info.
 * \param [out] dest_index Destination index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_member_dest_index_alloc(int unit,
                             bcmi_ltsw_ecmp_member_dest_info_t *dest_info,
                             int *dest_index)
{
    ecmp_member_dest_info_t *di = &l3_egr_info[unit].ecmp_member_dest_info;
    bcmint_l3_ecmp_member_ent_t *ent = di->ent_info;
    int i, total_ent, grp_idx, ent_idx, free_idx = -1;

    SHR_FUNC_ENTER(unit);

    total_ent = di->num_grps * di->num_ents;

    for (i = 0; i < total_ent; i++) {
        if (ent[i].ref_cnt) {
            if ((ent[i].type != dest_info->type) ||
                (ent[i].prot_swt_prfl_idx != dest_info->prot_swt_prfl_idx)) {
                continue;
            }
            if (ent[i].type == BCMI_LTSW_ECMP_MEMBER_DEST_T_ALPM_NO_CASCADED) {
                if ((dest_info->ol_egr_obj == ent[i].ol_egr_obj) &&
                    (dest_info->ul_egr_obj == ent[i].ul_egr_obj)) {
                    break;
                }
            } else if (ent[i].type == BCMI_LTSW_ECMP_MEMBER_DEST_T_L2_CASCADED) {
                if (dest_info->dvp == ent[i].dvp) {
                    break;
                }
            } else if (ent[i].type == BCMI_LTSW_ECMP_MEMBER_DEST_T_L3_CASCADED) {
                if (dest_info->ol_egr_obj == ent[i].ol_egr_obj) {
                    break;
                }
            } else {
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
            }
        } else {
            if (free_idx == -1) {
                free_idx = i;
            }
        }
    }
    if (i == total_ent && free_idx == -1) {
        SHR_IF_ERR_EXIT(SHR_E_FULL);
    }
    if (i < total_ent) {
        ent[i].ref_cnt++;
        ent_idx = i;
    } else {
        SHR_IF_ERR_EXIT
            (ecmp_member_dest_path_add(unit, free_idx, dest_info));
        ent_idx = free_idx;
    }

    grp_idx = ent_idx / di->num_ents;
    *dest_index = di->group_info[grp_idx].base_index + (ent_idx % di->num_ents);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free a destination index in ECMP_MEMBER_DESTINATION table.
 *
 * \param [in] unit Unit number.
 * \param [in] dest_index Destination index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_member_dest_index_free(int unit, int dest_index)
{
    ecmp_member_dest_info_t *di = &l3_egr_info[unit].ecmp_member_dest_info;
    ecmp_member_dest_grp_t *grp = di->group_info;
    bcmint_l3_ecmp_member_ent_t *ent = di->ent_info;
    int grp_idx, ent_idx;

    SHR_FUNC_ENTER(unit);

    for (grp_idx = 0; grp_idx < di->num_grps; grp_idx++) {
        if (!grp[grp_idx].used_ent_cnt) {
            continue;
        }
        if ((dest_index < grp[grp_idx].base_index) ||
            (dest_index >= grp[grp_idx].base_index + di->num_ents)) {
            continue;
        }
        ent_idx = dest_index - grp[grp_idx].base_index + grp_idx * di->num_ents;
        if (ent[ent_idx].ref_cnt == 0) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        ent[ent_idx].ref_cnt--;

        if (!ent[ent_idx].ref_cnt) {
            SHR_IF_ERR_EXIT
                (ecmp_member_dest_path_del(unit, ent_idx));
        }
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get destination info for given destination index.
 *
 * \param [in] unit Unit number.
 * \param [in] dest_index Destination index.
 * \param [out] dest_info ECMP member destination info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_member_dest_get(int unit, int dest_index,
                     bcmi_ltsw_ecmp_member_dest_info_t *dest_info)
{
    ecmp_member_dest_info_t *di = &l3_egr_info[unit].ecmp_member_dest_info;
    ecmp_member_dest_grp_t *grp = di->group_info;
    bcmint_l3_ecmp_member_ent_t *ent = di->ent_info;
    int grp_idx, ent_idx;

    SHR_FUNC_ENTER(unit);

    for (grp_idx = 0; grp_idx < di->num_grps; grp_idx++) {
        if (!grp[grp_idx].used_ent_cnt) {
            continue;
        }
        if ((dest_index < grp[grp_idx].base_index) ||
            (dest_index >= grp[grp_idx].base_index + di->num_ents)) {
            continue;
        }
        ent_idx = dest_index - grp[grp_idx].base_index + grp_idx * di->num_ents;
        if (ent[ent_idx].ref_cnt == 0) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        dest_info->type = ent[ent_idx].type;
        dest_info->ol_egr_obj = ent[ent_idx].ol_egr_obj;
        dest_info->ul_egr_obj = ent[ent_idx].ul_egr_obj;
        dest_info->dvp = ent[ent_idx].dvp;
        dest_info->prot_swt_prfl_idx = ent[ent_idx].prot_swt_prfl_idx;

        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update destination info for cascaded egress objects case.
 *
 * \param [in] unit Unit number.
 * \param [in] dest_info ECMP member destination info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_member_dest_update(int unit,
                        bcmi_ltsw_ecmp_member_dest_info_t *dest_info)
{
    ecmp_member_dest_info_t *di = &l3_egr_info[unit].ecmp_member_dest_info;
    bcmint_l3_ecmp_member_ent_t *ent = di->ent_info;
    int i, total_ent;

    SHR_FUNC_ENTER(unit);

    total_ent = di->num_grps * di->num_ents;

    for (i = 0; i < total_ent; i++) {
        if ((!ent[i].ref_cnt) ||
            (ent[i].type == BCMI_LTSW_ECMP_MEMBER_DEST_T_ALPM_NO_CASCADED )) {
            continue;
        }
        if (dest_info->type == BCMI_LTSW_ECMP_MEMBER_DEST_T_L3_CASCADED) {
            if ((ent[i].type == BCMI_LTSW_ECMP_MEMBER_DEST_T_L2_CASCADED) ||
                (ent[i].ol_egr_obj != dest_info->ol_egr_obj)) {
                continue;
            }
        }

        if ((dest_info->type == BCMI_LTSW_ECMP_MEMBER_DEST_T_L2_CASCADED) &&
            (ent[i].dvp != dest_info->dvp)) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (ecmp_member_dest_path_update(unit, i, dest_info));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the tables in L3 egress module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_egress_tables_clear(int unit)
{
    int i, num;
    const char *ltname[] = {
        ECMP_LEVEL0s,
        ECMP_LEVEL1s,
        ECMP_LEVEL0_WEIGHTEDs,
        ECMP_LEVEL1_WEIGHTEDs,
        ECMP_CONTROLs,
        ECMP_MEMBER0s,
        ECMP_MEMBER1s,
        ING_L3_SRC_NEXT_HOP_TABLEs
    };

    SHR_FUNC_ENTER(unit);

    if (bcmi_warmboot_get(unit)) {
        SHR_EXIT();
    }

    for (i = L3_OL_NHOP_MIN(unit);
         i <= L3_OL_NHOP_MAX(unit) && L3_OL_NHOP_CAP(unit); i++) {
        if (L3_OL_NHOP_FBMP_GET(unit, i)) {
            continue;
        }
        (void)olnh_delete(unit, i);
    }

    for (i = L3_UL_NHOP_MIN(unit);
         i <= L3_UL_NHOP_MAX(unit) && L3_UL_NHOP_CAP(unit); i++) {
        if (L3_UL_NHOP_FBMP_GET(unit, i)) {
            continue;
        }

        if (!L3_UL_NHOP_IFA_BMP_GET(unit, i)) {
            (void)ulnh_delete(unit, i);
        } else {
            (void)ulnh_ifa_delete(unit, i);
        }
    }

    num = COUNTOF(ltname);

    for (i = 0; i < num; i++) {
        SHR_IF_ERR_CONT
            (bcmi_lt_clear(unit, ltname[i]));
    }
    SHR_IF_ERR_CONT
        (bcmint_ltsw_l3_secmp_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover L3 egress database from LTs.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_egress_recover(int unit)
{
    inh1_cfg_t cfg1;
    inh2_cfg_t cfg2;
    enh2_cfg_t e_cfg2;
    l3_egr_info_t *ei = &l3_egr_info[unit];
    olnh_entry_info_t *olnh_ei = L3_OL_NHOP_INFO(unit);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit, ul_egr_obj;
    int rv = SHR_E_NONE;
    bcmi_ltsw_ecmp_member_dest_info_t di;
    uint64_t ecmp_id;
    bcmi_ltsw_virtual_vp_info_t vp_info;
    bcm_port_t port;
    bcm_trunk_chip_info_t trunk_info = {0};
    bcm_trunk_t tid;

    SHR_FUNC_ENTER(unit);

    if (ei->trunk_ul_nhidx) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_trunk_chip_info_get(unit, &trunk_info));
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_NEXT_HOP_1_TABLEs, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_EXIT
            (lt_ing_nh_1_parse(unit, eh, &cfg1));

        /* Don't recover from entry that not eligible for LT operation. */
        if ((cfg1.nhop_index_1 < L3_OL_NHOP_MIN(unit)) ||
            (cfg1.nhop_index_1 > L3_OL_NHOP_MAX(unit))) {
            continue;
        }

        L3_OL_NHOP_FBMP_CLR(unit, cfg1.nhop_index_1);

        olnh_ei[cfg1.nhop_index_1].mtu_ptr = cfg1.mtu_profile_ptr;
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_l3_mtu_recover(unit, cfg1.mtu_profile_ptr));

        if (cfg1.data_type == INH1_VIEW_DVP) {
            sal_memset(&di, 0, sizeof(bcmi_ltsw_ecmp_member_dest_info_t));
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_virtual_vp_info_get(unit, cfg1.dvp, &vp_info));

            if (vp_info.flags & BCMI_LTSW_VIRTUAL_VP_CASCADED) {
                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_virtual_vp_encode_gport(unit, cfg1.dvp, &port));
                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_virtual_port_egress_obj_get(unit, port,
                                                           &ul_egr_obj));
                olnh_ei[cfg1.nhop_index_1].flags |=
                                    BCMI_LTSW_L3_EGR_OBJ_INFO_F_CASCADED;
                olnh_ei[cfg1.nhop_index_1].dvp = cfg1.dvp;
                olnh_ei[cfg1.nhop_index_1].ul_egr_obj = ul_egr_obj;
            }
        }

        if (L3_OL_NHOP_FLAGS(unit, cfg1.nhop_index_1) & L3_OL_NHOP_F_CASCADED) {
            olnh_ei[cfg1.nhop_index_1].flags |=
                                    BCMI_LTSW_L3_EGR_OBJ_INFO_F_CASCADED;
        }
    }

    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_free(eh));

    eh = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_NEXT_HOP_2_TABLEs, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

        SHR_IF_ERR_EXIT
            (lt_ing_nh_2_parse(unit, eh, &cfg2));

        /* Don't recover from entry that not eligible for LT operation. */
        if ((cfg2.nhop_index_2 < L3_UL_NHOP_MIN(unit)) ||
            (cfg2.nhop_index_2 > L3_UL_NHOP_MAX(unit))) {
            continue;
        }

        L3_UL_NHOP_FBMP_CLR(unit, cfg2.nhop_index_2);

        /* Recover the underlay nexthop index of a trunk. */
        if ((cfg2.dest_valid & DEST_VALID_L2OIF) && ei->trunk_ul_nhidx) {
            if (SHR_SUCCESS
                    (bcmi_ltsw_trunk_l2_if_to_tid(unit, cfg2.dest, &tid))) {
                if (tid <= trunk_info.trunk_id_max) {
                    ei->trunk_ul_nhidx[tid] = cfg2.nhop_index_2;
                }
            }
        }
    }

    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_free(eh));

    eh = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, EGR_L3_NEXT_HOP_2s, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

        SHR_IF_ERR_EXIT
            (lt_egr_nh_2_parse(unit, eh, &e_cfg2));

        /* Don't recover from entry that not eligible for LT operation. */
        if ((e_cfg2.nhop_index_2 < L3_UL_NHOP_MIN(unit)) ||
            (e_cfg2.nhop_index_2 > L3_UL_NHOP_MAX(unit))) {
            continue;
        }

        if (e_cfg2.view != ENH2_VT_IFA) {
            continue;
        }

        L3_UL_NHOP_FBMP_CLR(unit, e_cfg2.nhop_index_2);
        L3_UL_NHOP_IFA_BMP_SET(unit, e_cfg2.nhop_index_2);
    }

    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_free(eh));

    eh = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ECMP_LEVEL0s, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, ECMP_IDs, &ecmp_id));

        L3_OL_ECMP_GRP_FBMP_CLR(unit, ecmp_id);
    }

    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_free(eh));

    eh = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ECMP_LEVEL0_WEIGHTEDs, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, ECMP_IDs, &ecmp_id));

        L3_OL_ECMP_GRP_FBMP_CLR(unit, ecmp_id);
    }

    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_free(eh));

    eh = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ECMP_LEVEL1s, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, ECMP_IDs, &ecmp_id));

        L3_UL_ECMP_GRP_FBMP_CLR(unit, ecmp_id);

    }

    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_free(eh));

    eh = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ECMP_LEVEL1_WEIGHTEDs, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, ECMP_IDs, &ecmp_id));

        L3_UL_ECMP_GRP_FBMP_CLR(unit, ecmp_id);

    }

    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_EXIT(rv);
    } else {
        SHR_EXIT();
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Internal HSDK functions
 */

int
xfs_ltsw_l3_egress_l2_rsv(int unit)
{
    int prf1_idx, prf2_idx, prf3_idx, prf4_idx;
    uint32_t cap;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field1[] =
    {
        /*0*/ {NHOP_INDEX_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {STRENGTH_PROFILE_INDEXs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_lt_field_t field2[] =
    {
        /*0*/ {L3_NEXT_HOP_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {STRENGTH_PRFL_IDXs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_lt_field_t field3[] =
    {
        /*0*/ {NHOP_INDEX_2s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {STRENGTH_PROFILE_INDEXs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_lt_field_t field4[] =
    {
        /*0*/ {L3_NEXT_HOP_2s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {STRENGTH_PRFL_IDXs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    /* Underlay Nexthop capacity. */
    SHR_IF_ERR_EXIT
        (bcmi_lt_capacity_get(unit, ING_L3_NEXT_HOP_2_TABLEs, &cap));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit,
            BCMI_LTSW_SBR_PTH_ING_L3_NEXT_HOP_1,
            BCMI_LTSW_SBR_PET_NONE,
            &prf1_idx));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit,
            BCMI_LTSW_SBR_PTH_EGR_L3_NEXT_HOP_1,
            BCMI_LTSW_SBR_PET_NONE,
            &prf2_idx));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit,
            BCMI_LTSW_SBR_PTH_ING_L3_NEXT_HOP_2,
            BCMI_LTSW_SBR_PET_NONE,
            &prf3_idx));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit,
            BCMI_LTSW_SBR_PTH_EGR_L3_NEXT_HOP_2,
            BCMI_LTSW_SBR_PET_NONE,
            &prf4_idx));

    lt_entry.fields = field1;
    lt_entry.nfields = sizeof(field1)/sizeof(field1[0]);
    lt_entry.attr = 0;
    field1[0].u.val = 0;
    field1[1].u.val = prf1_idx;
    /* Insert entry 0 of ING_L3_NEXT_HOP_1_TABLE. */
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_set(unit, ING_L3_NEXT_HOP_1_TABLEs, &lt_entry, NULL));

    lt_entry.fields = field2;
    lt_entry.nfields = sizeof(field2)/sizeof(field2[0]);
    lt_entry.attr = 0;
    field2[0].u.val = 0;
    field2[1].u.val = prf2_idx;
    /* Insert entry 0 of EGR_L3_NEXT_HOP_1. */
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_set(unit, EGR_L3_NEXT_HOP_1s, &lt_entry, NULL));

    if (cap) {
        lt_entry.fields = field3;
        lt_entry.nfields = sizeof(field3)/sizeof(field3[0]);
        lt_entry.attr = 0;
        field3[0].u.val = 0;
        field3[1].u.val = prf3_idx;
        /* Insert entry 0 of ING_L3_NEXT_HOP_2_TABLE. */
        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_set(unit, ING_L3_NEXT_HOP_2_TABLEs, &lt_entry, NULL));

        lt_entry.fields = field4;
        lt_entry.nfields = sizeof(field4)/sizeof(field4[0]);
        lt_entry.attr = 0;
        field4[0].u.val = 0;
        field4[1].u.val = prf4_idx;
        /* Insert entry 0 of EGR_L3_NEXT_HOP_2. */
        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_set(unit, EGR_L3_NEXT_HOP_2s, &lt_entry, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_init(int unit)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];
    uint32_t sz, nbits, alloc_sz, cap;
    uint64_t min, max;
    int max_vplag_grps;
    bcm_trunk_chip_info_t trunk_info = {0};

    SHR_FUNC_ENTER(unit);

    if (ei->inited) {
        SHR_IF_ERR_EXIT
            (xfs_ltsw_l3_egress_deinit(unit));
    }

    sal_memset(ei, 0, sizeof(l3_egr_info_t));

    ei->mutex = sal_mutex_create("bcmXfsl3EgrInfo");
    SHR_NULL_CHECK(ei->mutex, SHR_E_MEMORY);

    /* Overlay Nexthop */
    SHR_IF_ERR_EXIT
        (bcmi_lt_capacity_get(unit, ING_L3_NEXT_HOP_1_TABLEs, &cap));
    ei->olnh_cap = cap;

    /* Overlay Nexthop */
    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, ING_L3_NEXT_HOP_1_TABLEs,
                                       NHOP_INDEX_1s, &min, &max));
    /* Reserve entry 0 for all NPL-defined index LT in DNA. */
    ei->olnh_min = min ? min : 1;
    ei->olnh_max = max;
    sz = SHR_BITALLOCSIZE(max + 1);
    ei->olnh_fbmp = sal_alloc(sz, "bcmXfsl3EgrOlnhFbmp");
    SHR_NULL_CHECK(ei->olnh_fbmp, SHR_E_MEMORY);
    SHR_BITCLR_RANGE(ei->olnh_fbmp, 0, ei->olnh_min);
    nbits = ei->olnh_max - ei->olnh_min + 1;
    SHR_BITSET_RANGE(ei->olnh_fbmp, ei->olnh_min, nbits);

    sz = (ei->olnh_max + 1) * sizeof(olnh_entry_info_t);
    ei->olnh_ei = sal_alloc(sz, "bcmXfsl3EgrOlnhInfo");
    SHR_NULL_CHECK(ei->olnh_ei, SHR_E_MEMORY);
    sal_memset(ei->olnh_ei, 0, sz);

    sz = (ei->olnh_max + 1) * sizeof(uint32_t);
    alloc_sz = sz;
    ei->olnh_flags = bcmi_ltsw_ha_mem_alloc(unit,
                                            BCMI_HA_COMP_ID_L3_EGR,
                                            BCMINT_LTSW_L3_EGR_HA_OL_NHOP_FLAGS,
                                            "bcmL3OlnhFlags",
                                            &alloc_sz);
    SHR_NULL_CHECK(ei->olnh_flags, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);

    /* Underlay Nexthop */
    SHR_IF_ERR_EXIT
        (bcmi_lt_capacity_get(unit, ING_L3_NEXT_HOP_2_TABLEs, &cap));
    ei->ulnh_cap = cap;

    if (cap) {
        SHR_IF_ERR_EXIT
            (bcmi_lt_field_value_range_get(unit, ING_L3_NEXT_HOP_2_TABLEs,
                                           NHOP_INDEX_2s, &min, &max));
        /* Reserve entry 0 for all NPL-defined index LT in DNA. */
        ei->ulnh_min = min ? min : 1;
        ei->ulnh_max = max;
        sz = SHR_BITALLOCSIZE(max + 1);
        ei->ulnh_fbmp = sal_alloc(sz, "bcmXfsl3EgrUlnhFbmp");
        SHR_NULL_CHECK(ei->ulnh_fbmp, SHR_E_MEMORY);
        SHR_BITCLR_RANGE(ei->ulnh_fbmp, 0, ei->ulnh_min);
        nbits = ei->ulnh_max - ei->ulnh_min + 1;
        SHR_BITSET_RANGE(ei->ulnh_fbmp, ei->ulnh_min, nbits);

        ei->ulnh_ifa_bmp = sal_alloc(sz, "bcmXfsl3EgrUlnhIfabmp");
        SHR_NULL_CHECK(ei->ulnh_ifa_bmp, SHR_E_MEMORY);
        nbits = ei->ulnh_max + 1;
        SHR_BITCLR_RANGE(ei->ulnh_ifa_bmp, 0, nbits);

        if (ltsw_feature(unit, LTSW_FT_DEST_SYSPORT_TO_UL_NH)) {
            SHR_IF_ERR_EXIT
                (bcm_ltsw_trunk_chip_info_get(unit, &trunk_info));
            sz = (trunk_info.trunk_id_max + 1) * sizeof(int);
            ei->trunk_ul_nhidx = sal_alloc(sz, "bcmXfsl3EgrTrunkUlnhidx");
            SHR_NULL_CHECK(ei->trunk_ul_nhidx, SHR_E_MEMORY);
            sal_memset(ei->trunk_ul_nhidx, 0, sz);
        }
    }

    /* Overlay ECMP */
    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, ECMP_LEVEL0s, ECMP_IDs,
                                       &min, &max));

    /*
     * Reserve entry 0 for all NPL-defined index LT in DNA.
     * VPLAG shares same resource with ECMP_LEVEL0.
     * Reserve index 1 ~ max_vplag_groups for VLPAG.
     */
    max_vplag_grps = bcmi_ltsw_property_get(unit, BCMI_CPN_MAX_VP_LAGS, 0);
    if (max_vplag_grps >= max) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "No resource for ECMP Level0.\n")));
        max_vplag_grps = max;
    }

    ei->olecmp_grp_min = min ? (min + max_vplag_grps) : (1 + max_vplag_grps);
    ei->olecmp_grp_max = max;
    sz = SHR_BITALLOCSIZE(max + 1);
    ei->olecmp_grp_fbmp = sal_alloc(sz, "bcmXfsEcmp0Fbmp");
    SHR_NULL_CHECK(ei->olecmp_grp_fbmp, SHR_E_MEMORY);
    SHR_BITCLR_RANGE(ei->olecmp_grp_fbmp, 0, ei->olecmp_grp_min);
    nbits = ei->olecmp_grp_max - ei->olecmp_grp_min + 1;
    SHR_BITSET_RANGE(ei->olecmp_grp_fbmp, ei->olecmp_grp_min, nbits);

    sz = (max + 1) * sizeof(uint32_t);
    alloc_sz = sz;
    ei->olecmp_grp_flags = bcmi_ltsw_ha_mem_alloc(unit,
                                          BCMI_HA_COMP_ID_L3_EGR,
                                          BCMINT_LTSW_L3_EGR_HA_OL_ECMP_FLAGS,
                                          "bcmL3OLEcmpFlags",
                                          &alloc_sz);
    SHR_NULL_CHECK(ei->olecmp_grp_flags, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);

    sz = (max + 1) * sizeof(uint8_t);
    alloc_sz = sz;
    ei->olecmp_grp_dyna_mode = bcmi_ltsw_ha_mem_alloc(unit,
                                          BCMI_HA_COMP_ID_L3_EGR,
                                          BCMINT_LTSW_L3_EGR_HA_OL_ECMP_LB_MODE,
                                          "bcmL3OLEcmpDynaMode",
                                          &alloc_sz);
    SHR_NULL_CHECK(ei->olecmp_grp_dyna_mode, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);

    /* Underlay ECMP */
    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, ECMP_LEVEL1s, ECMP_IDs,
                                       &min, &max));
    /* Reserve entry 0 for all NPL-defined index LT in DNA. */
    ei->ulecmp_grp_min = min ? min : 1;
    ei->ulecmp_grp_max = max;
    sz = SHR_BITALLOCSIZE(max + 1);
    ei->ulecmp_grp_fbmp = sal_alloc(sz, "bcmXfsEcmp1Fbmp");
    SHR_NULL_CHECK(ei->ulecmp_grp_fbmp, SHR_E_MEMORY);
    SHR_BITCLR_RANGE(ei->ulecmp_grp_fbmp, 0, ei->ulecmp_grp_min);
    nbits = ei->ulecmp_grp_max - ei->ulecmp_grp_min + 1;
    SHR_BITSET_RANGE(ei->ulecmp_grp_fbmp, ei->ulecmp_grp_min, nbits);

    sz = (max + 1) * sizeof(uint32_t);
    alloc_sz = sz;
    ei->ulecmp_grp_flags = bcmi_ltsw_ha_mem_alloc(unit,
                                          BCMI_HA_COMP_ID_L3_EGR,
                                          BCMINT_LTSW_L3_EGR_HA_UL_ECMP_FLAGS,
                                          "bcmL3ULEcmpFlags",
                                          &alloc_sz);
    SHR_NULL_CHECK(ei->ulecmp_grp_flags, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);

    sz = (max + 1) * sizeof(uint8_t);
    alloc_sz = sz;
    ei->ulecmp_grp_dyna_mode = bcmi_ltsw_ha_mem_alloc(unit,
                                          BCMI_HA_COMP_ID_L3_EGR,
                                          BCMINT_LTSW_L3_EGR_HA_UL_ECMP_LB_MODE,
                                          "bcmL3ULEcmpDynaMode",
                                          &alloc_sz);
    SHR_NULL_CHECK(ei->ulecmp_grp_dyna_mode, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);

    /* Max paths */
    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, ECMP_LEVEL0s, MAX_PATHSs,
                                       &min, &max));
    ei->ecmp_path_max = max;

    /* Max RH paths */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, ECMP_LEVEL0_WEIGHTEDs,
                                       RH_NUM_PATHSs, &min, &max));
    ei->ecmp_rh_path_max = max;

    /* Max paths of weighted ECMP. */
    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, ECMP_LEVEL0_WEIGHTEDs, NUM_PATHSs,
                                       &min, &max));
    ei->wgt_ecmp_path_max = max;

    /* Maximum number of entries for each hash output selection instance. */
    SHR_IF_ERR_EXIT
        (bcmi_lt_capacity_get(unit, LB_HASH_ECMP_LEVEL0_OUTPUT_SELECTIONs,
                              &sz));
    ei->ecmp_hash_sel_ent_num = sz/2;

    SHR_IF_ERR_EXIT
        (ecmp_member_dest_init(unit));

    if (bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_EXIT
            (l3_egress_recover(unit));
    } else {
        sz = (ei->olecmp_grp_max + 1) * sizeof(uint32_t);
        sal_memset(ei->olecmp_grp_flags, 0, sz);

        sz = (ei->olecmp_grp_max + 1) * sizeof(uint8_t);
        sal_memset(ei->olecmp_grp_dyna_mode, 0, sz);

        sz = (ei->ulecmp_grp_max + 1) * sizeof(uint32_t);
        sal_memset(ei->ulecmp_grp_flags, 0, sz);

        sz = (ei->ulecmp_grp_max + 1) * sizeof(uint8_t);
        sal_memset(ei->ulecmp_grp_dyna_mode, 0, sz);

        sz = (ei->olnh_max + 1) * sizeof(uint32_t);
        sal_memset(ei->olnh_flags, 0, sz);
    }

    if (ltsw_feature(unit, LTSW_FT_DLB)) {
        SHR_IF_ERR_EXIT
            (l3_ecmp_dlb_init(unit));
    }

    ei->inited = 1;

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(ei->olnh_fbmp);
        SHR_FREE(ei->olnh_ei);
        SHR_FREE(ei->ulnh_fbmp);
        SHR_FREE(ei->ulnh_ifa_bmp);
        SHR_FREE(ei->olecmp_grp_fbmp);
        SHR_FREE(ei->ulecmp_grp_fbmp);
        SHR_FREE(ei->trunk_ul_nhidx);
        if (ei->mutex) {
            sal_mutex_destroy(ei->mutex);
        }
        if (!bcmi_warmboot_get(unit)) {
            (void)bcmi_ltsw_ha_mem_free(unit, ei->olecmp_grp_flags);
            (void)bcmi_ltsw_ha_mem_free(unit, ei->ulecmp_grp_flags);
            (void)bcmi_ltsw_ha_mem_free(unit, ei->olnh_flags);
        }
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_deinit(int unit)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];

    SHR_FUNC_ENTER(unit);

    if (!ei->inited || !ei->mutex) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_mutex_take(ei->mutex, SAL_MUTEX_FOREVER);

    ei->inited = 0;

    ecmp_member_dest_deinit(unit);

    SHR_IF_ERR_CONT
        (l3_ecmp_dlb_deinit(unit));

    SHR_IF_ERR_CONT
        (l3_egress_tables_clear(unit));

    SHR_FREE(ei->olnh_fbmp);
    SHR_FREE(ei->olnh_ei);
    SHR_FREE(ei->ulnh_fbmp);
    SHR_FREE(ei->ulnh_ifa_bmp);
    SHR_FREE(ei->olecmp_grp_fbmp);
    SHR_FREE(ei->ulecmp_grp_fbmp);
    SHR_FREE(ei->trunk_ul_nhidx);

    sal_mutex_give(ei->mutex);

    sal_mutex_destroy(ei->mutex);

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_id_range_get(int unit, int type, int *min, int *max)
{
    int min_intf = BCM_IF_INVALID;
    int max_intf = BCM_IF_INVALID;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    switch (type) {
    case BCMI_LTSW_L3_EGR_OBJ_T_OL:
        min_intf = BCMI_LTSW_L3_EGR_OBJ_BASE + L3_OL_NHOP_MIN(unit);
        max_intf = BCMI_LTSW_L3_EGR_OBJ_BASE + L3_OL_NHOP_MAX(unit);
        break;
    case BCMI_LTSW_L3_EGR_OBJ_T_UL:
        if (L3_UL_NHOP_CAP(unit)) {
            min_intf = BCMI_LTSW_L3_EGR_OBJ_BASE + L3_UL_OBJ_OFFSET +
                       L3_UL_NHOP_MIN(unit);
            max_intf = BCMI_LTSW_L3_EGR_OBJ_BASE + L3_UL_OBJ_OFFSET +
                       L3_UL_NHOP_MAX(unit);
        }
        break;
    case BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL:
        min_intf = BCMI_LTSW_L3_EGR_OBJ_BASE + L3_EGR_OBJ_SCALE +
                   L3_OL_ECMP_GRP_MIN(unit);
        max_intf = BCMI_LTSW_L3_EGR_OBJ_BASE + L3_EGR_OBJ_SCALE +
                   L3_OL_ECMP_GRP_MAX(unit);
        break;
    case BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL:
        min_intf = BCMI_LTSW_L3_EGR_OBJ_BASE + L3_EGR_OBJ_SCALE +
                   L3_UL_OBJ_OFFSET + L3_UL_ECMP_GRP_MIN(unit);
        max_intf = BCMI_LTSW_L3_EGR_OBJ_BASE + L3_EGR_OBJ_SCALE +
                   L3_UL_OBJ_OFFSET + L3_UL_ECMP_GRP_MAX(unit);
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (min) {
        *min = min_intf;
    }

    if (max) {
        *max = max_intf;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_obj_id_resolve(int unit, bcm_if_t intf, int *nh_ecmp_idx,
                                  bcmi_ltsw_l3_egr_obj_type_t *type)
{
    bcmi_ltsw_l3_egr_obj_type_t i;
    int min, max;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (intf == BCM_IF_INVALID) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (i = 0; i < BCMI_LTSW_L3_EGR_OBJ_T_CNT; i++) {
        SHR_IF_ERR_EXIT
            (xfs_ltsw_l3_egress_id_range_get(unit, i, &min, &max));
        if ((intf >= min) && (intf <= max)) {
            break;
        }
    }
    if ( i >= BCMI_LTSW_L3_EGR_OBJ_T_CNT) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (type) {
        *type = i;
    }

    if (nh_ecmp_idx) {
        *nh_ecmp_idx = intf - BCMI_LTSW_L3_EGR_OBJ_BASE -
                       (i / 2 * L3_EGR_OBJ_SCALE);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_obj_id_construct(int unit, int nh_ecmp_idx,
                                    bcmi_ltsw_l3_egr_obj_type_t type,
                                    bcm_if_t *intf)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (((type == BCMI_LTSW_L3_EGR_OBJ_T_OL) &&
         (nh_ecmp_idx >= L3_OL_NHOP_MIN(unit)) &&
         (nh_ecmp_idx <= L3_OL_NHOP_MAX(unit))) ||
        ((type == BCMI_LTSW_L3_EGR_OBJ_T_UL) &&
         (nh_ecmp_idx >= (L3_UL_NHOP_MIN(unit)) + L3_UL_OBJ_OFFSET) &&
         (nh_ecmp_idx <= (L3_UL_NHOP_MAX(unit)) + L3_UL_OBJ_OFFSET)) ||
        ((type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) &&
         (nh_ecmp_idx >= L3_OL_ECMP_GRP_MIN(unit)) &&
         (nh_ecmp_idx <= L3_OL_ECMP_GRP_MAX(unit))) ||
        ((type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) &&
         (nh_ecmp_idx >= (L3_UL_ECMP_GRP_MIN(unit)) + L3_UL_OBJ_OFFSET) &&
         (nh_ecmp_idx <= (L3_UL_ECMP_GRP_MAX(unit)) + L3_UL_OBJ_OFFSET))) {
        *intf = nh_ecmp_idx + BCMI_LTSW_L3_EGR_OBJ_BASE +
                (type / 2 * L3_EGR_OBJ_SCALE);
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_obj_info_get(int unit, bcm_if_t intf,
                                bcmi_ltsw_l3_egr_obj_info_t *info)
{
    int ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    olnh_entry_info_t *olnh_ei = l3_egr_info[unit].olnh_ei;
    uint16_t prot_en, prot_offset;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(info, 0, sizeof(bcmi_ltsw_l3_egr_obj_info_t));

    SHR_IF_ERR_EXIT
        (l3_egress_obj_id_to_ltidx(unit, intf, &ltidx, &type));

    /* Cascaded info. */
    if (type == BCMI_LTSW_L3_EGR_OBJ_T_OL) {
        info->flags |= olnh_ei[ltidx].flags;
        info->dvp = olnh_ei[ltidx].dvp;
        info->ul_egr_obj = olnh_ei[ltidx].ul_egr_obj;
    }

    /* Failover info. */
    prot_en = bcmi_ltsw_property_get(unit,
                                     BCMI_FAILOVER_FIXED_NH_OFFSET_ENABLE,
                                     0);
    if (prot_en && ((type == BCMI_LTSW_L3_EGR_OBJ_T_OL) ||
                    (type == BCMI_LTSW_L3_EGR_OBJ_T_UL))) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_failover_prot_offset_get(unit, &prot_offset));
        if (ltidx >= prot_offset) {
            info->flags |= BCMI_LTSW_L3_EGR_OBJ_INFO_F_FAILOVER;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_obj_cnt_get(int unit, bcmi_ltsw_l3_egr_obj_type_t type,
                                int *max, int *used)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];
    int max_cnt = 0, free_cnt = 0;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    switch (type) {
        case BCMI_LTSW_L3_EGR_OBJ_T_OL:
            max_cnt = ei->olnh_cap;
            SHR_BITCOUNT_RANGE(ei->olnh_fbmp, free_cnt, ei->olnh_min, max_cnt);
            break;
        case BCMI_LTSW_L3_EGR_OBJ_T_UL:
            max_cnt = ei->ulnh_cap;
            if (max_cnt) {
                SHR_BITCOUNT_RANGE(ei->ulnh_fbmp, free_cnt, ei->ulnh_min,
                                   max_cnt);
            }
            break;
        case BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL:
            max_cnt = ei->olecmp_grp_max - ei->olecmp_grp_min + 1;
            SHR_BITCOUNT_RANGE(ei->olecmp_grp_fbmp, free_cnt,
                               ei->olecmp_grp_min, max_cnt);
            break;
        case BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL:
            max_cnt = ei->ulecmp_grp_max - ei->ulecmp_grp_min + 1;
            SHR_BITCOUNT_RANGE(ei->ulecmp_grp_fbmp, free_cnt,
                               ei->ulecmp_grp_min, max_cnt);
            break;
        default:
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (max) {
        *max = max_cnt;
    }

    if (used) {
        *used = max_cnt - free_cnt;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_create(int unit, bcm_l3_egress_t *egr, int *idx)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];
    bcm_l3_egress_t pre_egr;
    int nhidx, min, max, pre_nhidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    uint16_t prot_offset;
    SHR_BITDCL *fbmp;
    bool locked = false;
    bool ul = false;
    bool nh_add = false;
    bool old_cascaded = false, new_cascaded = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!egr || !idx) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((egr->flags2 & BCM_L3_FLAGS2_UNDERLAY) && !L3_UL_NHOP_CAP(unit)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if ((egr->flags2 & BCM_L3_FLAGS2_NO_ECMP_OVERLAP) &&
        (egr->flags & BCM_L3_WITH_ID)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_failover_prot_offset_get(unit, &prot_offset));

    ul = (egr->flags2 & BCM_L3_FLAGS2_UNDERLAY) ? true :  false;
    fbmp = ul ? ei->ulnh_fbmp : ei->olnh_fbmp;
    max = prot_offset ? (prot_offset - 1) : (ul ? ei->ulnh_max : ei->olnh_max);
    new_cascaded = (egr->flags3 & BCM_L3_FLAGS3_EGRESS_CASCADED) ? true : false;

    if (egr->flags2 & BCM_L3_FLAGS2_NO_ECMP_OVERLAP) {
        /* NO_ECMP_OVERLAP only supported on overlay nexthop. */
        if (ul) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        min = ei->olecmp_grp_max + 1;
    } else {
        min = ul ? ei->ulnh_min : ei->olnh_min;
    }

    L3_EGR_LOCK(unit);
    locked = true;

    if (egr->flags & BCM_L3_WITH_ID) {
        SHR_IF_ERR_EXIT
            (l3_egress_obj_id_to_ltidx(unit, *idx, &nhidx, &type));

        if ((type == BCMI_LTSW_L3_EGR_OBJ_T_UL) ^ ul) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        if (!SHR_BITGET(fbmp, nhidx) && !(egr->flags & BCM_L3_REPLACE)) {
            SHR_IF_ERR_EXIT(SHR_E_EXISTS);
        }

        if (SHR_BITGET(fbmp, nhidx)) {
            egr->flags &= ~ BCM_L3_REPLACE;
        } else if (!ul) {
            if (L3_OL_NHOP_FLAGS(unit, nhidx) & L3_OL_NHOP_F_CASCADED) {
                old_cascaded = true;
            }
            if (old_cascaded ^ new_cascaded) {
                SHR_ERR_MSG_EXIT(SHR_E_PARAM,
                        (BSL_META_U(unit,
                                    "Not support to change cascaded mode "
                                    "of an existing egress object.\n")));
            }
        }

        /* When Failover with fixed egress nh offset is enable,
         * egr->failover_id is used internally to distinguish between user
         * created primary intf (should be zero) and prot_intf created from
         * bcm_failover_egress_set (BCMI_LTSW_FAILOVER_FIXED_NH_OFFSET_ON)
         */
        if (prot_offset > 0) {
            if (!(egr->failover_id & BCMI_LTSW_FAILOVER_FIXED_NH_OFFSET_ON) &&
                (nhidx >= prot_offset)) {
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
            }
            egr->failover_id &= ~BCMI_LTSW_FAILOVER_FIXED_NH_OFFSET_ON;
        }
    } else {
        egr->flags &= ~ BCM_L3_REPLACE;

        /* Get the first free index. */
        for (nhidx = min; nhidx <= max; nhidx++) {
            if (SHR_BITGET(fbmp, nhidx)) {
                break;
            }
        }

        if (nhidx > max) {
            SHR_IF_ERR_EXIT(SHR_E_FULL);
        }
    }

    if (ul) {
        if (egr->flags & BCM_L3_REPLACE) {
            if (L3_UL_NHOP_IFA_BMP_GET(unit, nhidx)) {
                SHR_IF_ERR_EXIT(SHR_E_CONFIG);
            }

            if (L3_SYSPORT_TO_UL_NHIDX_ENABLED(unit)) {
                bcm_l3_egress_t_init(&pre_egr);
                SHR_IF_ERR_EXIT
                    (ulnh_ing_get(unit, nhidx, &pre_egr));
            }
        }

        if (L3_SYSPORT_TO_UL_NHIDX_ENABLED(unit)) {
            /* Validate the 1:1 mapping between nhop2 and system port. */
            SHR_IF_ERR_EXIT
                (ing_sys_dst_nhop2_get(unit, egr, &pre_nhidx));
            if (pre_nhidx && (pre_nhidx != nhidx)) {
                SHR_ERR_MSG_EXIT(SHR_E_RESOURCE,
                        (BSL_META_U(unit,
                                    "VXLAN egress port can be configured with "
                                    "one underlay nexthop.\n")));
            }
        }

        nh_add = true;

        SHR_IF_ERR_EXIT
            (ulnh_ing_set(unit, nhidx, egr));

        if (L3_SYSPORT_TO_UL_NHIDX_ENABLED(unit)) {
            if (egr->flags & BCM_L3_REPLACE) {
                /* Clear the nhop_2 from old system_destination. */
                SHR_IF_ERR_EXIT
                    (ing_sys_dst_nhop2_set(unit, &pre_egr, 0));
            }

            /* Set the nhop_2 for the new system_destination. */
            SHR_IF_ERR_EXIT
                (ing_sys_dst_nhop2_set(unit, egr, nhidx));
        }

        SHR_IF_ERR_EXIT
            (ulnh_egr_set(unit, nhidx, egr));

    } else {

        nh_add = true;

        SHR_IF_ERR_EXIT
            (olnh_ing_set(unit, nhidx, egr));

        SHR_IF_ERR_EXIT
            (olnh_egr_set(unit, nhidx, egr));

        if (ltsw_feature(unit, LTSW_FT_URPF)) {
            SHR_IF_ERR_EXIT
                (snh_ing_set(unit, nhidx, egr));
        }

        if (new_cascaded) {
            L3_OL_NHOP_FLAGS(unit, nhidx) |= L3_OL_NHOP_F_CASCADED;
        }
    }

    if (!(egr->flags & BCM_L3_WITH_ID)) {
        type = ul ? BCMI_LTSW_L3_EGR_OBJ_T_UL : BCMI_LTSW_L3_EGR_OBJ_T_OL;
        SHR_IF_ERR_EXIT
            (l3_egress_ltidx_to_obj_id(unit, nhidx, type, idx));
    }

    SHR_BITCLR(fbmp, nhidx);

exit:
    if (SHR_FUNC_ERR()) {
        if (!(egr->flags & BCM_L3_REPLACE) && nh_add) {
            if (ul) {
                (void)ulnh_delete(unit, nhidx);
            } else {
                if (ltsw_feature(unit, LTSW_FT_URPF)) {
                    (void)snh_ing_delete(unit, nhidx);
                }
                (void)olnh_delete(unit, nhidx);
            }
        }
    }
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_destroy(int unit, int idx)
{
    int nhidx, min;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;
    uint16_t prot_offset;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (l3_egress_obj_id_to_ltidx(unit, idx, &nhidx, &type));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_failover_prot_offset_get(unit, &prot_offset));

    /* Validate the nhidx for switch protection enabled case. */
    if (prot_offset > 0) {
        min = (type == BCMI_LTSW_L3_EGR_OBJ_T_UL) ? L3_UL_NHOP_MIN(unit) :
                                                    L3_OL_NHOP_MIN(unit);

        if((nhidx >= prot_offset) && (nhidx < (prot_offset + min))) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }

    L3_EGR_LOCK(unit);
    locked = true;

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_UL) {
        if (L3_UL_NHOP_FBMP_GET(unit, nhidx) ||
            L3_UL_NHOP_IFA_BMP_GET(unit, nhidx)) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        SHR_IF_ERR_EXIT
            (ulnh_delete(unit, nhidx));
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_OL) {
        if (L3_OL_NHOP_FBMP_GET(unit, nhidx)) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        if (ltsw_feature(unit, LTSW_FT_URPF)) {
            SHR_IF_ERR_EXIT
                (snh_ing_delete(unit, nhidx));
        }

        SHR_IF_ERR_EXIT
            (olnh_delete(unit, nhidx));

    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_get(int unit, int idx, bcm_l3_egress_t *egr)
{
    int nhidx, min;
    bcmi_ltsw_l3_egr_obj_type_t type;
    uint16_t prot_offset;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (l3_egress_obj_id_to_ltidx(unit, idx, &nhidx, &type));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_failover_prot_offset_get(unit, &prot_offset));

    /* Validate the nhidx for switch protection enabled case. */
    if (prot_offset > 0) {
        min = (type == BCMI_LTSW_L3_EGR_OBJ_T_UL) ? L3_UL_NHOP_MIN(unit) :
                                                    L3_OL_NHOP_MIN(unit);

        if((nhidx >= prot_offset) && (nhidx < (prot_offset + min))) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_UL) {
        if (L3_UL_NHOP_IFA_BMP_GET(unit, nhidx)) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        SHR_IF_ERR_EXIT
            (ulnh_ing_get(unit, nhidx, egr));

        SHR_IF_ERR_EXIT
            (ulnh_egr_get(unit, nhidx, egr));

        egr->flags2 |= BCM_L3_FLAGS2_UNDERLAY;
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_OL) {
        SHR_IF_ERR_EXIT
            (olnh_ing_get(unit, nhidx, egr));

        SHR_IF_ERR_EXIT
            (olnh_egr_get(unit, nhidx, egr));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_find(int unit, bcm_l3_egress_t *egr, int *idx)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];
    int nhidx, underlay, min, max, type;
    SHR_BITDCL *fbmp;
    bcm_l3_egress_t int_egr;

    SHR_FUNC_ENTER(unit);

    if ((egr->flags2 & BCM_L3_FLAGS2_UNDERLAY) && !L3_UL_NHOP_CAP(unit)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    underlay = (egr->flags2 & BCM_L3_FLAGS2_UNDERLAY) ? true : false;
    fbmp = underlay ? ei->ulnh_fbmp : ei->olnh_fbmp;
    min = underlay ? ei->ulnh_min : ei->olnh_min;
    max = underlay ? ei->ulnh_max : ei->olnh_max;
    type = underlay ? BCMI_LTSW_L3_EGR_OBJ_T_UL : BCMI_LTSW_L3_EGR_OBJ_T_OL;

    for (nhidx = min; nhidx <= max; nhidx ++) {
        if (SHR_BITGET(fbmp, nhidx)) {
            continue;
        }

        sal_memset(&int_egr, 0, sizeof(bcm_l3_egress_t));
        if (underlay) {
            if (L3_UL_NHOP_IFA_BMP_GET(unit, nhidx)) {
                continue;
            }

            SHR_IF_ERR_EXIT
                (ulnh_ing_get(unit, nhidx, &int_egr));

            SHR_IF_ERR_EXIT
                (ulnh_egr_get(unit, nhidx, &int_egr));
        } else {
            SHR_IF_ERR_EXIT
                (olnh_ing_get(unit, nhidx, &int_egr));

            SHR_IF_ERR_EXIT
                (olnh_egr_get(unit, nhidx, &int_egr));
        }

        
        if (egr->intf == int_egr.intf &&
            egr->port == int_egr.port &&
            !sal_memcmp(egr->mac_addr, int_egr.mac_addr, sizeof(bcm_mac_t))) {
            break;
        }
    }

    if (nhidx > max ) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (l3_egress_ltidx_to_obj_id(unit, nhidx, type, idx));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_traverse(int unit,
                            bcm_l3_egress_traverse_cb trav_fn,
                            void *user_data)
{
    bcm_l3_egress_t egr;
    bcm_if_t intf;
    int nhidx, rv;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(trav_fn, SHR_E_PARAM);

    L3_EGR_LOCK(unit);
    locked = true;

    for (nhidx = L3_OL_NHOP_MIN(unit); nhidx <= L3_OL_NHOP_MAX(unit);
         nhidx ++) {
        /* Skip unused entries. */
        if (L3_OL_NHOP_FBMP_GET(unit, nhidx)) {
            continue;
        }

        bcm_l3_egress_t_init(&egr);

        SHR_IF_ERR_EXIT
            (olnh_ing_get(unit, nhidx, &egr));

        SHR_IF_ERR_EXIT
            (olnh_egr_get(unit, nhidx, &egr));

        SHR_IF_ERR_EXIT
            (l3_egress_ltidx_to_obj_id(unit, nhidx,
                                       BCMI_LTSW_L3_EGR_OBJ_T_OL,
                                       &intf));

        rv = trav_fn(unit, intf, &egr, user_data);
        if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
            SHR_IF_ERR_EXIT(rv);
        }
    }

    for (nhidx = L3_UL_NHOP_MIN(unit);
         nhidx <= L3_UL_NHOP_MAX(unit) && L3_UL_NHOP_CAP(unit); nhidx ++) {
        /* Skip unused entries. */
        if (L3_UL_NHOP_FBMP_GET(unit, nhidx) ||
            L3_UL_NHOP_IFA_BMP_GET(unit, nhidx)) {
            continue;
        }

        bcm_l3_egress_t_init(&egr);

        SHR_IF_ERR_EXIT
            (ulnh_ing_get(unit, nhidx, &egr));

        SHR_IF_ERR_EXIT
            (ulnh_egr_get(unit, nhidx, &egr));

        egr.flags2 |= BCM_L3_FLAGS2_UNDERLAY;

        SHR_IF_ERR_EXIT
            (l3_egress_ltidx_to_obj_id(unit, nhidx,
                                       BCMI_LTSW_L3_EGR_OBJ_T_UL,
                                       &intf));

        rv = trav_fn(unit, intf, &egr, user_data);
        if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_dvp_set(int unit, bcm_if_t intf, int dvp)
{
    int dunit, nhidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *ltname;
    const char *keyname;
    const char *view;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type;
    int sbr_index;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (l3_egress_obj_id_to_ltidx(unit, intf, &nhidx, &type));

    ltname = (type == BCMI_LTSW_L3_EGR_OBJ_T_OL) ?
             ING_L3_NEXT_HOP_1_TABLEs : ING_L3_NEXT_HOP_2_TABLEs;

    keyname = (type == BCMI_LTSW_L3_EGR_OBJ_T_OL) ?
              NHOP_INDEX_1s : NHOP_INDEX_2s;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, keyname, nhidx));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, DVPs, dvp));

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_OL) {
        view = dvp ? DVPs : L2_OIFs;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(eh, VIEW_Ts, view));
    } else {
        /* Update strength profile ptr for DVP. */
        pth = BCMI_LTSW_SBR_PTH_ING_L3_NEXT_HOP_2;
        ent_type = dvp ? BCMI_LTSW_SBR_PET_DEF : BCMI_LTSW_SBR_PET_NO_DVP;

        SHR_IF_ERR_EXIT
            (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type,
                                                 &sbr_index));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, STRENGTH_PROFILE_INDEXs, sbr_index));
    }

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_flexctr_attach(int unit, bcm_if_t intf_id,
                                  bcmi_ltsw_flexctr_counter_info_t *info)
{
    l3_egr_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_egr_flexctr_cfg_t));
    flexctr.op = L3_EGR_FLXCTR_OP_SET;
    flexctr.flags = L3_EGR_FLXCTR_F_ACTION;
    flexctr.action = info->action_index;

    SHR_IF_ERR_EXIT
        (l3_egress_flexctr_op(unit, intf_id, &flexctr));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_flexctr_detach(int unit, bcm_if_t intf_id)
{
    l3_egr_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_egr_flexctr_cfg_t));
    flexctr.op = L3_EGR_FLXCTR_OP_SET;
    flexctr.flags = L3_EGR_FLXCTR_F_ACTION;
    flexctr.action = 0;

    SHR_IF_ERR_EXIT
        (l3_egress_flexctr_op(unit, intf_id, &flexctr));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_flexctr_info_get(int unit, bcm_if_t intf_id,
                                    bcmi_ltsw_flexctr_counter_info_t *info)
{
    int dunit, ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    const char *ltname, *fldname;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t val;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (l3_egress_obj_id_to_ltidx(unit, intf_id, &ltidx, &type));

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_OL) {
        if (L3_OL_NHOP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        ltname = EGR_L3_NEXT_HOP_1s;
        fldname = L3_NEXT_HOP_1s;
        info->source = bcmFlexctrSourceL3EgressOverlay;
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_UL) {
        if (L3_UL_NHOP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        ltname = EGR_L3_NEXT_HOP_2s;
        fldname = L3_NEXT_HOP_2s;
        info->source = bcmFlexctrSourceL3EgressUnderlay;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (info->direction != BCMI_LTSW_FLEXCTR_DIR_EGRESS) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, fldname, ltidx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, FLEX_CTR_ACTIONs, &val));

    info->stage = BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR;
    info->action_index = val;
    info->table_name = ltname;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_flexctr_object_set(
    int unit,
    bcm_if_t intf_id,
    uint32_t value)
{
    l3_egr_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_egr_flexctr_cfg_t));
    flexctr.op = L3_EGR_FLXCTR_OP_SET;
    flexctr.flags = L3_EGR_FLXCTR_F_INDEX;
    flexctr.index = value;

    SHR_IF_ERR_EXIT
        (l3_egress_flexctr_op(unit, intf_id, &flexctr));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_flexctr_object_get(
    int unit,
    bcm_if_t intf_id,
    uint32_t *value)
{
    l3_egr_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_egr_flexctr_cfg_t));
    flexctr.op = L3_EGR_FLXCTR_OP_GET;
    flexctr.flags = L3_EGR_FLXCTR_F_INDEX;

    SHR_IF_ERR_EXIT
        (l3_egress_flexctr_op(unit, intf_id, &flexctr));

    *value = flexctr.index;

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_ifa_create(
    int unit,
    bcmi_ltsw_l3_egr_ifa_t *egr_ifa,
    int *idx)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];
    int nhidx, max;
    bcmi_ltsw_l3_egr_obj_type_t type;
    uint16_t prot_offset;
    SHR_BITDCL *fbmp;
    bool locked = false;
    bool nh_add = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!egr_ifa || !idx) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_failover_prot_offset_get(unit, &prot_offset));

    fbmp = ei->ulnh_fbmp;
    max = prot_offset ? (prot_offset - 1) : ei->ulnh_max;

    L3_EGR_LOCK(unit);
    locked = true;

    if (egr_ifa->flags & BCM_L3_WITH_ID) {
        SHR_IF_ERR_EXIT
            (l3_egress_obj_id_to_ltidx(unit, *idx, &nhidx, &type));

        if (type != BCMI_LTSW_L3_EGR_OBJ_T_UL) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        if (!SHR_BITGET(fbmp, nhidx) && !(egr_ifa->flags & BCM_L3_REPLACE)) {
            SHR_IF_ERR_EXIT(SHR_E_EXISTS);
        }

        if (SHR_BITGET(fbmp, nhidx)) {
            egr_ifa->flags &= ~ BCM_L3_REPLACE;
        }
    } else {
        egr_ifa->flags &= ~ BCM_L3_REPLACE;

        /* Get the first free index. */
        SHR_BIT_ITER(fbmp, (max + 1), nhidx) {
            break;
        }

        if (nhidx > max) {
            SHR_IF_ERR_EXIT(SHR_E_FULL);
        }
    }

    if (egr_ifa->flags & BCM_L3_REPLACE) {
        if (!L3_UL_NHOP_IFA_BMP_GET(unit, nhidx)) {
            SHR_IF_ERR_EXIT(SHR_E_CONFIG);
        }
    }

    nh_add = true;

    SHR_IF_ERR_EXIT
        (ulnh_egr_ifa_set(unit, nhidx, egr_ifa));

    if (!(egr_ifa->flags & BCM_L3_WITH_ID)) {
        type = BCMI_LTSW_L3_EGR_OBJ_T_UL;
        SHR_IF_ERR_EXIT
            (l3_egress_ltidx_to_obj_id(unit, nhidx, type, idx));
    }

    SHR_BITCLR(fbmp, nhidx);
    L3_UL_NHOP_IFA_BMP_SET(unit, nhidx);

exit:
    if (SHR_FUNC_ERR()) {
        if (!(egr_ifa->flags & BCM_L3_REPLACE) && nh_add) {
            (void)ulnh_ifa_delete(unit, nhidx);
        }
    }
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_ifa_destroy(int unit, int idx)
{
    int nhidx, min;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;
    uint16_t prot_offset;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (l3_egress_obj_id_to_ltidx(unit, idx, &nhidx, &type));

    if (type != BCMI_LTSW_L3_EGR_OBJ_T_UL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_failover_prot_offset_get(unit, &prot_offset));

    /* Validate the nhidx for switch protection enabled case. */
    if (prot_offset > 0) {
        min = L3_UL_NHOP_MIN(unit);
        if((nhidx >= prot_offset) && (nhidx < (prot_offset + min))) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }

    L3_EGR_LOCK(unit);
    locked = true;

    if (L3_UL_NHOP_FBMP_GET(unit, nhidx) ||
        !L3_UL_NHOP_IFA_BMP_GET(unit, nhidx)) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (ulnh_ifa_delete(unit, nhidx));

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_ifa_get(int unit, int idx, bcmi_ltsw_l3_egr_ifa_t *egr_ifa)
{
    int nhidx, min;
    bcmi_ltsw_l3_egr_obj_type_t type;
    uint16_t prot_offset;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (l3_egress_obj_id_to_ltidx(unit, idx, &nhidx, &type));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_failover_prot_offset_get(unit, &prot_offset));

    /* Validate the nhidx for switch protection enabled case. */
    if (prot_offset > 0) {
        min = L3_UL_NHOP_MIN(unit);
        if((nhidx >= prot_offset) && (nhidx < (prot_offset + min))) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (!L3_UL_NHOP_IFA_BMP_GET(unit, nhidx)) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (ulnh_egr_ifa_get(unit, nhidx, egr_ifa));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_ifa_traverse(
    int unit,
    bcm_ifa_header_traverse_cb trav_fn,
    bcmi_ltsw_l3_egr_to_ifa_header_cb cb,
    void *user_data)
{
    bcmi_ltsw_l3_egr_ifa_t egr_ifa;
    bcm_ifa_header_t ifa_header;
    bcm_if_t intf;
    int nhidx, rv;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(trav_fn, SHR_E_PARAM);

    L3_EGR_LOCK(unit);
    locked = true;

    for (nhidx = L3_UL_NHOP_MIN(unit); nhidx <= L3_UL_NHOP_MAX(unit);
         nhidx ++) {
        /* Skip unused entries. */
        if (L3_UL_NHOP_FBMP_GET(unit, nhidx) ||
            !L3_UL_NHOP_IFA_BMP_GET(unit, nhidx)) {
            continue;
        }

        sal_memset(&egr_ifa, 0, sizeof(bcmi_ltsw_l3_egr_ifa_t));

        SHR_IF_ERR_EXIT
            (ulnh_egr_ifa_get(unit, nhidx, &egr_ifa));

        SHR_IF_ERR_EXIT
            (l3_egress_ltidx_to_obj_id(unit, nhidx,
                                       BCMI_LTSW_L3_EGR_OBJ_T_UL,
                                       &intf));

        SHR_IF_ERR_EXIT
            (cb(unit, &egr_ifa, &ifa_header));
        rv = trav_fn(unit, (int *)&intf, &ifa_header, user_data);
        if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ecmp_create(int unit, bcm_l3_egress_ecmp_t *ecmp_info,
                        int ecmp_member_count,
                        bcm_l3_ecmp_member_t *ecmp_member_array)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];
    int idx, min, max;
    bcmi_ltsw_l3_egr_obj_type_t type;
    SHR_BITDCL *fbmp;
    uint32_t *flags;
    uint8_t *dyna_mode;
    bool locked = false, ul = false, dlb_en = false;
    bool pre_wgt = false, wgt = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!ecmp_info || !ecmp_member_array) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Only one ECMP Level can be specified for a group */
    if ((ecmp_info->ecmp_group_flags & BCM_L3_ECMP_OVERLAY) &&
        (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_UNDERLAY)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_NORMAL) ||
        (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED) ||
        (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL) ||
        (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM)) {
        if (!ltsw_feature(unit, LTSW_FT_DLB)) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
        dlb_en = true;
    }

    if ((ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) &&
        (ecmp_member_count > L3_ECMP_RH_MAX_PATHS(unit))) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    ul = (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_UNDERLAY) ? true : false;

    if (dlb_en && !ul) {
        /* A DLB group can only be programmed at ECMP Level 2 */
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    fbmp = ul ? ei->ulecmp_grp_fbmp : ei->olecmp_grp_fbmp;
    flags = ul ? ei->ulecmp_grp_flags : ei->olecmp_grp_flags;
    dyna_mode = ul ? ei->ulecmp_grp_dyna_mode : ei->olecmp_grp_dyna_mode;
    min = ul ? ei->ulecmp_grp_min : ei->olecmp_grp_min;
    max = ul ? ei->ulecmp_grp_max : ei->olecmp_grp_max;

    L3_EGR_LOCK(unit);
    locked = true;

    if (ecmp_info->flags & BCM_L3_WITH_ID) {
        SHR_IF_ERR_EXIT
            (l3_egress_obj_id_to_ltidx(unit, ecmp_info->ecmp_intf, &idx,
                                       &type));

        if ((type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) ^ ul) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        if (!SHR_BITGET(fbmp, idx) && !(ecmp_info->flags & BCM_L3_REPLACE)) {
            SHR_IF_ERR_EXIT(SHR_E_EXISTS);
        }

        if (dlb_en && (idx < L3_ECMP_DLB_GRP_MIN(unit))) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        if (ecmp_info->flags & BCM_L3_REPLACE) {
            if (SHR_BITGET(fbmp, idx)) {
                SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
            }

            if ((flags[idx] & BCM_L3_ECMP_WEIGHTED) ||
                (dyna_mode[idx] == XFS_ECMP_DM_RESILIENT)) {
                pre_wgt = true;
            }

            if ((ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT)
                ||
                (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_WEIGHTED)) {
                wgt = true;
            }

            /*
             * ECMP_LEVELx & ECMP_LEVELx_WEIGHTED share same PT, thus need
             * to free the index from one LT before inserting it to another LT.
             */
            if (pre_wgt ^ wgt) {
                if (ul) {
                    SHR_IF_ERR_EXIT
                        (ulecmp_del(unit, idx));
                } else {
                    SHR_IF_ERR_EXIT
                        (olecmp_del(unit, idx));
                }
                ecmp_info->flags &= ~BCM_L3_REPLACE;
            }
        }
    } else {
        if (dlb_en) {
            min = L3_ECMP_DLB_GRP_MIN(unit);
        }

        /* Get the first free index. */
        for (idx = min; idx <= max; idx++) {
            if (SHR_BITGET(fbmp, idx)) {
                break;
            }
        }

        if (idx > max) {
            SHR_IF_ERR_EXIT(SHR_E_FULL);
        }
    }

    if (ul) {
        SHR_IF_ERR_EXIT
            (ulecmp_set(unit, idx, ecmp_info, ecmp_member_count,
                        ecmp_member_array, dlb_en));
    } else {
        SHR_IF_ERR_EXIT
            (olecmp_set(unit, idx, ecmp_info, ecmp_member_count,
                        ecmp_member_array));
    }

    if (!(ecmp_info->flags & BCM_L3_WITH_ID)) {
        type = ul ? BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL :
                    BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL;
        SHR_IF_ERR_EXIT
            (l3_egress_ltidx_to_obj_id(unit, idx, type, &ecmp_info->ecmp_intf));
    }

    SHR_BITCLR(fbmp, idx);

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ecmp_destroy(int unit, bcm_if_t ecmp_group_id)
{
    int ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (l3_egress_obj_id_to_ltidx(unit, ecmp_group_id, &ltidx, &type));

    L3_EGR_LOCK(unit);
    locked = true;

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
        SHR_IF_ERR_EXIT
            (ulecmp_del(unit, ltidx));
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) {
        SHR_IF_ERR_EXIT
            (olecmp_del(unit, ltidx));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ecmp_get(int unit, bcm_l3_egress_ecmp_t *ecmp_info,
                     int ecmp_member_size,
                     bcm_l3_ecmp_member_t *ecmp_member_array,
                     int *ecmp_member_count)
{
    int ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!ecmp_info || !ecmp_member_count) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (l3_egress_obj_id_to_ltidx(unit, ecmp_info->ecmp_intf, &ltidx, &type));

    L3_EGR_LOCK(unit);
    locked = true;

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
        SHR_IF_ERR_EXIT
            (ulecmp_get(unit, ltidx, ecmp_info, ecmp_member_size,
                        ecmp_member_array, ecmp_member_count));
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) {
        SHR_IF_ERR_EXIT
            (olecmp_get(unit, ltidx, ecmp_info, ecmp_member_size,
                        ecmp_member_array, ecmp_member_count));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ecmp_member_add(int unit, bcm_if_t ecmp_group_id,
                            bcm_l3_ecmp_member_t *ecmp_member)
{
    int ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(ecmp_member, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (l3_egress_obj_id_to_ltidx(unit, ecmp_group_id, &ltidx, &type));

    L3_EGR_LOCK(unit);
    locked = true;

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
        if (L3_UL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        if (L3_UL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_WEIGHTED) {
            /* Can't add member to weighted ECMP group. */
            SHR_IF_ERR_EXIT(SHR_E_FULL);
        }

        SHR_IF_ERR_EXIT
            (ulecmp_member_add(unit, ltidx, ecmp_member));
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) {
        if (L3_OL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        if (L3_OL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_WEIGHTED) {
            /* Can't add member to weighted ECMP group. */
            SHR_IF_ERR_EXIT(SHR_E_FULL);
        }

        SHR_IF_ERR_EXIT
            (olecmp_member_add(unit, ltidx, ecmp_member));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ecmp_member_del(int unit, bcm_if_t ecmp_group_id,
                            bcm_l3_ecmp_member_t *ecmp_member)
{
    int ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(ecmp_member, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (l3_egress_obj_id_to_ltidx(unit, ecmp_group_id, &ltidx, &type));

    L3_EGR_LOCK(unit);
    locked = true;

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
        if (L3_UL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        if (L3_UL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_WEIGHTED) {
            /* Can't delete member from weighted ECMP group. */
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_EXIT
            (ulecmp_member_del(unit, ltidx, ecmp_member));
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) {
        if (L3_OL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        if (L3_OL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_WEIGHTED) {
            /* Can't delete member from weighted ECMP group. */
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_EXIT
            (olecmp_member_del(unit, ltidx, ecmp_member));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ecmp_member_del_all(int unit, bcm_if_t ecmp_group_id)
{
    int ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (l3_egress_obj_id_to_ltidx(unit, ecmp_group_id, &ltidx, &type));

    L3_EGR_LOCK(unit);
    locked = true;

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
        if (L3_UL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        if (L3_UL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_WEIGHTED) {
            /* Can't delete member from weighted ECMP group. */
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_EXIT
            (ulecmp_member_del_all(unit, ltidx));
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) {
        if (L3_OL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        if (L3_OL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_WEIGHTED) {
            /* Can't delete member from weighted ECMP group. */
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_EXIT
            (olecmp_member_del_all(unit, ltidx));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ecmp_find(int unit, int ecmp_member_count,
                      bcm_l3_ecmp_member_t *ecmp_member_array,
                      bcm_l3_egress_ecmp_t *ecmp_info)
{
    bcmi_ltsw_l3_ecmp_member_info_t *emi_in = NULL;
    int pri_cnt, alt_cnt, rv;
    bool locked = false, ul_skip = false;
    bool dgm_en = false;
    uint32_t dlb_ft, egr_types;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!ecmp_member_count || !ecmp_member_array || !ecmp_info) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (ecmp_member_info_array_alloc(unit, ecmp_member_count,
                                      ecmp_member_array, &emi_in,
                                      &pri_cnt, &egr_types, NULL));

    if (pri_cnt < ecmp_member_count) {
        if (!ltsw_feature(unit, LTSW_FT_DLB)) {
            SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
        }
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_dlb_feature_get(unit, &dlb_ft));
        if (!(dlb_ft & BCMI_LTSW_DLB_F_ECMP_DGM)) {
            SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
        }
        dgm_en = true;
    }

    if (egr_types & (1 << BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL)) {
        ul_skip = true;
    }

    if (dgm_en && ul_skip) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    alt_cnt = ecmp_member_count - pri_cnt;

    /* Sort primary paths. */
    if (pri_cnt) {
        SHR_IF_ERR_EXIT
            (ecmp_member_sort(unit, emi_in, pri_cnt,
                              sizeof(bcmi_ltsw_l3_ecmp_member_info_t)));
    }
    /* Sort alternate paths. */
    if (alt_cnt) {
        SHR_IF_ERR_EXIT
            (ecmp_member_sort(unit, &emi_in[pri_cnt], alt_cnt,
                              sizeof(bcmi_ltsw_l3_ecmp_member_info_t)));
    }

    L3_EGR_LOCK(unit);
    locked = true;

    if (!dgm_en) {
        rv = olecmp_find(unit, ecmp_member_count, emi_in, ecmp_info);
        if (rv != SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(rv);
        }
    }

    if (ul_skip) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (ulecmp_find(unit, ecmp_member_count, emi_in, pri_cnt, ecmp_info));

exit:
    SHR_FREE(emi_in);
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ecmp_trav(int unit, bcm_l3_ecmp_traverse_cb trav_fn,
                      void *user_data)
{
    bcm_l3_egress_ecmp_t ecmp_info;
    bcm_l3_ecmp_member_t *ecmp_member_array = NULL;
    int ltidx, max_paths, sz, ecmp_member_count, rv;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(trav_fn, SHR_E_PARAM);

    max_paths = L3_ECMP_WEIGHTED_MAX_PATHS(unit);
    sz = sizeof(bcm_l3_ecmp_member_t) * max_paths;
    SHR_ALLOC(ecmp_member_array, sz, "bcmLtswXfsEcmpMember");
    SHR_NULL_CHECK(ecmp_member_array, SHR_E_MEMORY);

    L3_EGR_LOCK(unit);
    locked = true;

    for (ltidx = L3_OL_ECMP_GRP_MIN(unit); ltidx <= L3_OL_ECMP_GRP_MAX(unit);
         ltidx++) {
        /* Skip unused entries. */
        if (L3_OL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            continue;
        }

        bcm_l3_egress_ecmp_t_init(&ecmp_info);
        sal_memset(ecmp_member_array, 0, sz);

        SHR_IF_ERR_EXIT
            (l3_egress_ltidx_to_obj_id(unit, ltidx,
                                       BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL,
                                       &ecmp_info.ecmp_intf));

        SHR_IF_ERR_EXIT
            (olecmp_get(unit, ltidx, &ecmp_info, max_paths, ecmp_member_array,
                        &ecmp_member_count));

        rv = trav_fn(unit, &ecmp_info, ecmp_member_count, ecmp_member_array,
                     user_data);
        if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
            SHR_IF_ERR_EXIT(rv);
        }
    }

    for (ltidx = L3_UL_ECMP_GRP_MIN(unit); ltidx <= L3_UL_ECMP_GRP_MAX(unit);
         ltidx++) {
        /* Skip unused entries. */
        if (L3_UL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            continue;
        }

        bcm_l3_egress_ecmp_t_init(&ecmp_info);
        sal_memset(ecmp_member_array, 0, sz);

        SHR_IF_ERR_EXIT
            (l3_egress_ltidx_to_obj_id(unit, ltidx,
                                       BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL,
                                       &ecmp_info.ecmp_intf));

        SHR_IF_ERR_EXIT
            (ulecmp_get(unit, ltidx, &ecmp_info, max_paths,
                        ecmp_member_array, &ecmp_member_count));

        rv = trav_fn(unit, &ecmp_info, ecmp_member_count, ecmp_member_array,
                     user_data);
        if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FREE(ecmp_member_array);
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ecmp_max_paths_get(int unit, uint32_t attr, int *max_paths)
{
    bcmi_ltsw_dlb_capability_t cap;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(max_paths, SHR_E_PARAM);

    if (attr & BCMI_LTSW_ECMP_ATTR_DLB) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_dlb_capability_get(unit, bcmi_dlb_type_ecmp, &cap));
        *max_paths = cap.max_members_per_group;
        if (attr & BCMI_LTSW_ECMP_ATTR_DGM) {
            *max_paths += cap.max_alt_members_per_group;
        }
    } else if (attr & BCMI_LTSW_ECMP_ATTR_WGT) {
        *max_paths = L3_ECMP_WEIGHTED_MAX_PATHS(unit);
    } else {
        *max_paths = L3_ECMP_MAX_PATHS(unit);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ecmp_member_status_set(int unit, bcm_if_t intf, int status)
{
    int nhidx, local_port;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bcm_l3_egress_t egr;


    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (l3_egress_obj_id_to_ltidx(unit, intf, &nhidx, &type));

    if (type != BCMI_LTSW_L3_EGR_OBJ_T_OL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    bcm_l3_egress_t_init(&egr);

    SHR_IF_ERR_EXIT
        (olnh_ing_get(unit, nhidx, &egr));

    SHR_IF_ERR_EXIT
        (ecmp_dlb_port_get(unit, &egr, &local_port));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dlb_member_status_set(unit, local_port, bcmi_dlb_type_ecmp,
                                         status));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ecmp_member_status_get(int unit, bcm_if_t intf, int *status)
{
    int nhidx, local_port;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bcm_l3_egress_t egr;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (l3_egress_obj_id_to_ltidx(unit, intf, &nhidx, &type));

    if (type != BCMI_LTSW_L3_EGR_OBJ_T_OL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    bcm_l3_egress_t_init(&egr);

    SHR_IF_ERR_EXIT
        (olnh_ing_get(unit, nhidx, &egr));

    SHR_IF_ERR_EXIT
        (ecmp_dlb_port_get(unit, &egr, &local_port));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dlb_member_status_get(unit, local_port, bcmi_dlb_type_ecmp,
                                         status));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ecmp_dlb_mon_config_set(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg)
{
    bcmi_ltsw_dlb_type_t dlb_type;
    bcmi_ltsw_dlb_monitor_t dlb_monitor;
    int dlb_id;
    int ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;

    SHR_FUNC_ENTER(unit);

    dlb_type = bcmi_dlb_type_ecmp;

    SHR_IF_ERR_EXIT
        (l3_egress_obj_id_to_ltidx(unit, ecmp_group_id, &ltidx, &type));

    dlb_id = ltidx - L3_ECMP_DLB_GRP_OFFSET(unit);

    dlb_monitor.enable = dlb_mon_cfg->enable;
    dlb_monitor.sample_rate = dlb_mon_cfg->sample_rate;
    switch (dlb_mon_cfg->action) {
        case BCM_L3_ECMP_DLB_MON_TRACE:
            dlb_monitor.action = BCMI_LTSW_DLB_MON_TRACE;
            break;
        case BCM_L3_ECMP_DLB_MON_ACTION_NONE:
            dlb_monitor.action = BCMI_LTSW_DLB_MON_ACTION_NONE;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dlb_monitor_set(unit, dlb_type, dlb_id, &dlb_monitor));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ecmp_dlb_mon_config_get(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg)
{
    bcmi_ltsw_dlb_type_t dlb_type;
    bcmi_ltsw_dlb_monitor_t dlb_monitor;
    int dlb_id;
    int ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;

    SHR_FUNC_ENTER(unit);

    dlb_type = bcmi_dlb_type_ecmp;

    SHR_IF_ERR_EXIT
        (l3_egress_obj_id_to_ltidx(unit, ecmp_group_id, &ltidx, &type));

    dlb_id = ltidx - L3_ECMP_DLB_GRP_OFFSET(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dlb_monitor_get(unit, dlb_type, dlb_id, &dlb_monitor));

    dlb_mon_cfg->enable = dlb_monitor.enable;
    dlb_mon_cfg->sample_rate = dlb_monitor.sample_rate;
    switch (dlb_monitor.action) {
        case BCMI_LTSW_DLB_MON_TRACE:
            dlb_mon_cfg->action = BCM_L3_ECMP_DLB_MON_TRACE;
            break;
        case BCMI_LTSW_DLB_MON_ACTION_NONE:
            dlb_mon_cfg->action = BCM_L3_ECMP_DLB_MON_ACTION_NONE;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ecmp_dlb_stat_set(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t value)
{
    bcmi_ltsw_dlb_type_t dlb_type;
    bcmi_ltsw_dlb_stat_t dlb_stat;
    int dlb_id;
    int ltidx;
    int rv;
    bcmi_ltsw_l3_egr_obj_type_t egr_obj_type;

    SHR_FUNC_ENTER(unit);

    dlb_type = bcmi_dlb_type_ecmp;

    rv = l3_egress_obj_id_to_ltidx(unit, ecmp_group_id, &ltidx, &egr_obj_type);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_PARAM) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        } else {
            SHR_ERR_EXIT(rv);
        }
    }

    dlb_id = ltidx - L3_ECMP_DLB_GRP_OFFSET(unit);


    dlb_stat.fail_count = (uint64_t)0xffffffff;
    dlb_stat.port_reassignment_count = (uint64_t)0xffffffff;
    dlb_stat.member_reassignment_count = (uint64_t)0xffffffff;

    switch (type) {
        case bcmL3ECMPDlbStatFailPackets:
            dlb_stat.fail_count = value;
            break;
        case bcmL3ECMPDlbStatPortReassignmentCount:
            dlb_stat.port_reassignment_count = value;
            break;
        case bcmL3ECMPDlbStatMemberReassignmentCount:
            dlb_stat.member_reassignment_count = value;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dlb_stat_set(unit, dlb_type, dlb_id, &dlb_stat));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ecmp_dlb_stat_get(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t *value)
{
    bcmi_ltsw_dlb_type_t dlb_type;
    bcmi_ltsw_dlb_stat_t dlb_stat;
    int dlb_id;
    int ltidx;
    int rv;
    bcmi_ltsw_l3_egr_obj_type_t egr_obj_type;

    SHR_FUNC_ENTER(unit);

    dlb_type = bcmi_dlb_type_ecmp;

    sal_memset(&dlb_stat, 0, sizeof(dlb_stat));

    rv = l3_egress_obj_id_to_ltidx(unit, ecmp_group_id, &ltidx, &egr_obj_type);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_PARAM) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        } else {
            SHR_ERR_EXIT(rv);
        }
    }

    dlb_id = ltidx - L3_ECMP_DLB_GRP_OFFSET(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dlb_stat_get(unit, dlb_type, dlb_id, &dlb_stat));

    switch (type) {
        case bcmL3ECMPDlbStatFailPackets:
            *value = dlb_stat.fail_count;
            break;
        case bcmL3ECMPDlbStatPortReassignmentCount:
            *value = dlb_stat.port_reassignment_count;
            break;
        case bcmL3ECMPDlbStatMemberReassignmentCount:
            *value = dlb_stat.member_reassignment_count;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ecmp_dlb_stat_sync_get(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t *value)
{
    bcmi_ltsw_dlb_type_t dlb_type;
    bcmi_ltsw_dlb_stat_t dlb_stat;
    int dlb_id;
    int ltidx;
    int rv;
    bcmi_ltsw_l3_egr_obj_type_t egr_obj_type;

    SHR_FUNC_ENTER(unit);

    dlb_type = bcmi_dlb_type_ecmp;

    sal_memset(&dlb_stat, 0, sizeof(dlb_stat));

    rv = l3_egress_obj_id_to_ltidx(unit, ecmp_group_id, &ltidx, &egr_obj_type);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_PARAM) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        } else {
            SHR_ERR_EXIT(rv);
        }
    }

    dlb_id = ltidx - L3_ECMP_DLB_GRP_OFFSET(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dlb_stat_sync_get(unit, dlb_type, dlb_id, &dlb_stat));

    switch (type) {
        case bcmL3ECMPDlbStatFailPackets:
            *value = dlb_stat.fail_count;
            break;
        case bcmL3ECMPDlbStatPortReassignmentCount:
            *value = dlb_stat.port_reassignment_count;
            break;
        case bcmL3ECMPDlbStatMemberReassignmentCount:
            *value = dlb_stat.member_reassignment_count;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_ecmp_member_dest_alloc(int unit,
                                bcmi_ltsw_ecmp_member_dest_info_t *dest_info,
                                int *dest_index)
{
    ecmp_member_dest_info_t *di = &l3_egr_info[unit].ecmp_member_dest_info;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_mutex_take(di->mutex, SAL_MUTEX_FOREVER);
    locked = true;

    SHR_IF_ERR_EXIT
        (ecmp_member_dest_index_alloc(unit, dest_info, dest_index));

exit:
    if (locked) {
        sal_mutex_give(di->mutex);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_ecmp_member_dest_get(int unit,
                              int dest_index,
                              bcmi_ltsw_ecmp_member_dest_info_t *dest_info)
{
    ecmp_member_dest_info_t *di = &l3_egr_info[unit].ecmp_member_dest_info;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_mutex_take(di->mutex, SAL_MUTEX_FOREVER);
    locked = true;

    SHR_IF_ERR_EXIT
        (ecmp_member_dest_get(unit, dest_index, dest_info));

exit:
    if (locked) {
        sal_mutex_give(di->mutex);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_ecmp_member_dest_free(int unit, int dest_index)
{
    ecmp_member_dest_info_t *di = &l3_egr_info[unit].ecmp_member_dest_info;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_mutex_take(di->mutex, SAL_MUTEX_FOREVER);
    locked = true;

    SHR_IF_ERR_EXIT
        (ecmp_member_dest_index_free(unit, dest_index));

exit:
    if (locked) {
        sal_mutex_give(di->mutex);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_ecmp_member_dest_update(int unit,
                                 bcmi_ltsw_ecmp_member_dest_info_t *dest_info)
{
    ecmp_member_dest_info_t *di = &l3_egr_info[unit].ecmp_member_dest_info;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_mutex_take(di->mutex, SAL_MUTEX_FOREVER);
    locked = true;

    SHR_IF_ERR_EXIT
        (ecmp_member_dest_update(unit, dest_info));

exit:
    if (locked) {
        sal_mutex_give(di->mutex);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_snh_ing_l3_oif_get(int unit, int nhidx, uint32_t *l3_oif)
{
    snh_cfg_t snh;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_SRC_NEXT_HOP_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, L3_SRC_NHOP_INDEXs, nhidx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    sal_memset(&snh, 0, sizeof(snh_cfg_t));
    snh.fld_bmp = SNH_FLD_L3OIF;

    SHR_IF_ERR_EXIT
        (lt_ing_snh_parse(unit, eh, &snh));

    *l3_oif = (uint32_t)snh.l3_oif_value_type;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_trunk_member_delete(int unit, bcm_trunk_t tid,
                                       int member_cnt, bcm_port_t *member_arr)
{
    int i;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_SYSPORT_TO_UL_NHIDX_ENABLED(unit) ||
        !trunk_type_is_frontpanel(unit, tid)) {
        SHR_EXIT();
    }

    L3_EGR_LOCK(unit);
    locked = true;

    for (i = 0; i < member_cnt; i++) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_port_tab_set(unit, member_arr[i],
                                    BCMI_PT_ING_SYS_DEST_NHOP_2, 0));
    }

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egress_trunk_member_add(int unit, bcm_trunk_t tid,
                                    int member_cnt, bcm_port_t *member_arr)
{
    int i, nhidx;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_SYSPORT_TO_UL_NHIDX_ENABLED(unit) ||
        !trunk_type_is_frontpanel(unit, tid)) {
        SHR_EXIT();
    }

    L3_EGR_LOCK(unit);
    locked = true;

    nhidx = L3_TRUNK_UL_NHIDX(unit, tid);

    for (i = 0; i < member_cnt; i++) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_port_tab_set(unit, member_arr[i],
                                    BCMI_PT_ING_SYS_DEST_NHOP_2, nhidx));
    }

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}
